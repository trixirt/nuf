/*
 * Copyright (c) 2017 Tom Rix
 * All rights reserved.
 *
 * You may distribute under the terms of :
 *
 * the BSD 2-Clause license
 *
 * Any patches released for this software are to be released under these
 * same license terms.
 *
 * BSD 2-Clause license:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "forth.h"
#include "dictionary.h"
#include "parse/parsers.h"
#include "visitors/forth_emit.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"

Forth::Forth(std::shared_ptr<CPU> cpu, std::string filename)
    : Lang(cpu, filename), _stack_size(256), _return_stack_size(64) {
  scanner =
      new forth_scanner(filename, forth_dictionary, strlen(forth_dictionary));
  parser = FORTHParseAlloc(malloc);

  _t["c"] = llvm::Type::getIntNTy(ctx, 8);
  _t["c*"] = llvm::Type::getIntNPtrTy(ctx, 8);
  _t["i"] = llvm::Type::getIntNTy(ctx, pointer_size());
  _t["i*"] = llvm::Type::getIntNPtrTy(ctx, pointer_size());
  _t["i2"] = llvm::Type::getIntNTy(ctx, 2 * pointer_size());
  _t["i2*"] = llvm::Type::getIntNPtrTy(ctx, 2 * pointer_size());
  _t["v"] = llvm::Type::getVoidTy(ctx);

  _ft_void = llvm::FunctionType::get(_t["v"], false);

  /* Some useful constants */
  _zero = constant(0);
  _one = constant(1);
  _minus_one = constant(-1);
  _ffff = llvm::Constant::getAllOnesValue(_t["i"]);
  _space = llvm::ConstantInt::get(_t["c"], ' ');
  _cr = llvm::ConstantInt::get(_t["c"], '\n');

  setEntryName("_nuf");
}
Forth::~Forth() {
  if (scanner) {
    delete scanner;
    scanner = NULL;
  }
  if (parser) {
    FORTHParseFree(parser, free);
    parser = NULL;
  }
}

int Forth::parse(int tokenID, std::shared_ptr<N> token) {
  FORTHParse(parser, tokenID, token, this);
  return 0;
}

std::shared_ptr<N> Forth::scan(int *i) { return scanner->scan(i); }

void Forth::in(llvm::Instruction *i) {
  llvm::BasicBlock *bb = getBasicBlock();
  in(bb, i);
}

void Forth::in(llvm::BasicBlock *b, llvm::Instruction *i) {
  llvm::TerminatorInst *ti = b->getTerminator();
  if (ti) {
    auto itr = b->getInstList().end();
    itr--;
    b->getInstList().insert(itr, i);
  } else {
    b->getInstList().push_back(i);
  }
}

void Forth::te(llvm::BasicBlock *b, llvm::Instruction *i) {
  b->getInstList().push_back(i);
}

void Forth::te(llvm::Instruction *i) {
  llvm::BasicBlock *bb = getBasicBlock();
  te(bb, i);
}

void Forth::ph(llvm::BasicBlock *b, llvm::Instruction *i) {
  b->getInstList().push_front(i);
}

void Forth::ph(llvm::Instruction *i) {
  llvm::BasicBlock *bb = getBasicBlock();
  ph(bb, i);
}

llvm::Constant *Forth::constant(int a) {
  return llvm::ConstantInt::get(_t["i"], a);
}

llvm::LoadInst *Forth::string(char *a) {
  llvm::GlobalVariable *gv;
  std::string s = a;
  llvm::Value *i[2] = {_zero, _zero};
  if (strings.count(s)) {
    gv = strings[s];
  } else {
    llvm::Constant *c = llvm::ConstantDataArray::getString(ctx, a);
    gv = new llvm::GlobalVariable(
        *_mod, c->getType(), true,
        llvm::GlobalValue::LinkageTypes::PrivateLinkage, c, "_str", nullptr);
    strings[s] = gv;
  }
  llvm::GetElementPtrInst *gep =
      llvm::GetElementPtrInst::Create(nullptr, gv, i);
  llvm::AllocaInst *al = new llvm::AllocaInst(_t["c*"], 0);
  llvm::StoreInst *st = new llvm::StoreInst(gep, al);
  llvm::LoadInst *ld = new llvm::LoadInst(al);

  in(gep);
  in(al);
  in(st);
  in(ld);

  return (ld);
}

void Forth::inc() { inc(_idx); }
void Forth::inc(llvm::GlobalVariable *v) {
  llvm::LoadInst *ld1 = new llvm::LoadInst(v);
  llvm::BinaryOperator *add = llvm::BinaryOperator::CreateAdd(
      /* Instruction::Add */
      /* Value * V1       */ ld1,
      /* Value * V2       */ _one);
  llvm::StoreInst *st = new llvm::StoreInst(add, v);

  in(ld1);
  in(add);
  in(st);
}

void Forth::dec() { dec(_idx); }
void Forth::dec(llvm::GlobalVariable *v) {
  llvm::LoadInst *ld1 = new llvm::LoadInst(v);
  llvm::BinaryOperator *sub = llvm::BinaryOperator::CreateSub(ld1, _one);
  llvm::StoreInst *st = new llvm::StoreInst(sub, v);
  in(ld1);
  in(sub);
  in(st);
}

llvm::LoadInst *Forth::get() { return get(_st, _idx, _t["i*"]); }

llvm::LoadInst *Forth::get(llvm::GlobalVariable *v, llvm::GlobalVariable *idx,
                           llvm::Type *t) {
  llvm::LoadInst *ld1 = new llvm::LoadInst(idx);
  llvm::Value *i[2] = {_zero, ld1};
  /* From include/IR/Instructions.h
     when PointeeType is nullptr (0), the type of Ptr is used */
  llvm::GetElementPtrInst *gep = llvm::GetElementPtrInst::Create(
      /* Type *PointeeType */ nullptr,
      /* Value *Ptr */ v,
      /* ArrayRef<Value *> IdxLixt */ i);
  llvm::BitCastInst *bc = new llvm::BitCastInst(gep, t);
  llvm::LoadInst *ld2 = new llvm::LoadInst(bc);

  in(ld1);
  in(gep);
  in(bc);
  in(ld2);
  return ld2;
}

llvm::LoadInst *Forth::pop() { return pop(_st, _idx, _t["i*"]); }
llvm::LoadInst *Forth::pop2() { return pop(_st, _idx, _t["i2*"]); }

llvm::CastInst *Forth::popC() {
  llvm::Value *v = pop();
  llvm::CastInst *cst =
      llvm::CastInst::Create(llvm::Instruction::Trunc, v, _t["c"]);
  in(cst);
  return cst;
}

llvm::LoadInst *Forth::popR() { return pop(_rst, _ridx, _t["i*"]); }

llvm::LoadInst *Forth::pop(llvm::GlobalVariable *v, llvm::GlobalVariable *idx,
                           llvm::Type *t) {
  dec(idx);
  llvm::LoadInst *ret = get(v, idx, t);
  if (t == _t["i2*"])
    dec(idx);
  return ret;
}

llvm::BinaryOperator *Forth::op_plus(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateAdd(
      /* Instruction::Add */
      /* Value * V1       */ a,
      /* Value * V2       */ b);
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_divide(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateSDiv(a, b);
  // TODO
  // Some type checking.. to use udiv if required
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_leftshift(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateShl(a, b);
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_minus(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateSub(a, b);
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_not(llvm::Value *a) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateNot(a);
  in(op);
  return op;
}

llvm::CastInst *Forth::op_cast(llvm::Value *a) {
  llvm::CastInst *op =
      llvm::CastInst::Create(llvm::Instruction::SExt, a, _t["i2"]);
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_rightshift(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateLShr(a, b);
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_modulo(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateSRem(a, b);
  // TODO
  // Some type checking.. to use urem if required
  in(op);
  return op;
}

llvm::CallInst *Forth::op_call(std::string function_name) {
  // tbd sync with user call
  llvm::CallInst *op = nullptr;
  if (runtime_functions.count(function_name)) {
    llvm::Function *f = runtime_functions[function_name];
    op = llvm::CallInst::Create(f->getFunctionType(), f,
                                llvm::ArrayRef<llvm::Value *>({}));
    in(op);
  }
  return op;
}

llvm::CallInst *Forth::op_call(std::string function_name, llvm::Value *a) {
  // tbd sync with user call
  llvm::CallInst *op = nullptr;
  if (runtime_functions.count(function_name)) {
    llvm::Function *f = runtime_functions[function_name];
    op = llvm::CallInst::Create(f->getFunctionType(), f,
                                llvm::ArrayRef<llvm::Value *>({a}));
    in(op);
  }
  return op;
}

llvm::CallInst *Forth::op_call(std::string function_name, llvm::Value *a,
                               llvm::Value *b) {
  llvm::CallInst *op = nullptr;
  if (runtime_functions.count(function_name)) {
    llvm::Function *f = runtime_functions[function_name];
    op = llvm::CallInst::Create(f->getFunctionType(), f,
                                llvm::ArrayRef<llvm::Value *>({a, b}));
    in(op);
  }
  return op;
}

llvm::ICmpInst *Forth::op_equals(llvm::Value *a, llvm::Value *b) {
  llvm::ICmpInst *op = new llvm::ICmpInst(llvm::ICmpInst::ICMP_EQ, a, b);
  in(op);
  return op;
}

llvm::ICmpInst *Forth::op_less_than_signed(llvm::Value *a, llvm::Value *b) {
  llvm::ICmpInst *op = new llvm::ICmpInst(llvm::ICmpInst::ICMP_SLT, a, b);
  in(op);
  return op;
}

llvm::ICmpInst *Forth::op_less_than_unsigned(llvm::Value *a, llvm::Value *b) {
  llvm::ICmpInst *op = new llvm::ICmpInst(llvm::ICmpInst::ICMP_ULT, a, b);
  in(op);
  return op;
}

llvm::ICmpInst *Forth::op_greater_than_signed(llvm::Value *a, llvm::Value *b) {
  llvm::ICmpInst *op = new llvm::ICmpInst(llvm::ICmpInst::ICMP_SGT, a, b);
  in(op);
  return op;
}

llvm::ICmpInst *Forth::op_greater_than_unsigned(llvm::Value *a,
                                                llvm::Value *b) {
  llvm::ICmpInst *op = new llvm::ICmpInst(llvm::ICmpInst::ICMP_UGT, a, b);
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_multiply(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateMul(a, b);
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_or(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateOr(a, b);
  in(op);
  return op;
}

llvm::BinaryOperator *Forth::op_exclusive_or(llvm::Value *a, llvm::Value *b) {
  llvm::BinaryOperator *op = llvm::BinaryOperator::CreateXor(a, b);
  in(op);
  return op;
}

llvm::BranchInst *Forth::op_condbr(llvm::Value *a, llvm::BasicBlock *t,
                                   llvm::BasicBlock *f) {
  llvm::BranchInst *op = llvm::BranchInst::Create(t, f, a);
  te(op);
  return op;
}

llvm::BranchInst *Forth::op_br(llvm::BasicBlock *b) {
  llvm::BranchInst *op = llvm::BranchInst::Create(b);
  te(op);
  return op;
}

llvm::PHINode *Forth::phi(unsigned int n, const llvm::Twine &Name) {
  llvm::PHINode *op = llvm::PHINode::Create(_t["i"], n, Name);
  ph(op);
  return op;
}

llvm::SelectInst *Forth::op_select(llvm::Value *c, llvm::Value *t,
                                   llvm::Value *f) {
  llvm::SelectInst *op = llvm::SelectInst::Create(c, t, f);
  in(op);
  return op;
}

void Forth::here() {
  llvm::LoadInst *ld = new llvm::LoadInst(_idx);
  llvm::Value *i[2] = {_zero, ld};
  llvm::GetElementPtrInst *gep =
      llvm::GetElementPtrInst::Create(nullptr, _st, i);

  gep->setName("here");
  //  llvm::CastInst *cst =
  //      llvm::CastInst::Create(llvm::Instruction::ZExt, gep, _t["i"]);
  //  cst->setName("here");
  in(ld);
  in(gep);
  //  in(cst);
  push(gep);
}

void Forth::set(llvm::Value *a) { Forth::set(a, _st, _idx); }
void Forth::set(llvm::Value *a, llvm::GlobalVariable *v,
                llvm::GlobalVariable *idx) {
  llvm::LoadInst *ld = new llvm::LoadInst(idx);
  llvm::Value *i[2] = {_zero, ld};
  /* From include/IR/Instructions.h
     when PointeeType is nullptr (0), tyep type of Ptr is used */
  llvm::GetElementPtrInst *gep = llvm::GetElementPtrInst::Create(
      /* Type *PointeeType */ nullptr,
      /* Value *Ptr */ v,
      /* ArrayRef<Value *> IdxLixt */ i);

  llvm::BitCastInst *bc =
      new llvm::BitCastInst(gep, a->getType()->getPointerTo());
  llvm::StoreInst *st = new llvm::StoreInst(a, bc);

  in(ld);
  in(gep);
  in(bc);
  in(st);
}

void Forth::store(llvm::GlobalVariable *a, llvm::Value *b) {
  llvm::StoreInst *st = new llvm::StoreInst(b, a);
  in(st);
}

void Forth::storeC(llvm::GlobalVariable *a, llvm::Value *b) {
  llvm::CastInst *cst =
      llvm::CastInst::Create(llvm::Instruction::ZExt, b, _t["i"]);
  in(cst);
  store(a, cst);
}

llvm::LoadInst *Forth::fetch(llvm::GlobalVariable *a) {
  llvm::LoadInst *ret = new llvm::LoadInst(a);
  in(ret);
  return ret;
}

llvm::CastInst *Forth::fetchC(llvm::GlobalVariable *a) {
  llvm::LoadInst *ld = new llvm::LoadInst(a);
  llvm::CastInst *cst =
      llvm::CastInst::Create(llvm::Instruction::Trunc, ld, _t["c"]);
  in(ld);
  in(cst);
  return cst;
}

void Forth::push(llvm::Value *a) { push(a, _st, _idx); }

void Forth::pushC(llvm::Value *a) {
  llvm::CastInst *cst =
      llvm::CastInst::Create(llvm::Instruction::ZExt, a, _t["i"]);
  in(cst);
  push(cst);
}
void Forth::pushR(llvm::Value *a) { push(a, _rst, _ridx); }

void Forth::push(llvm::Value *a, llvm::GlobalVariable *v,
                 llvm::GlobalVariable *idx) {
  set(a, v, idx);
  inc(idx);
  if (a->getType() == _t["i2"])
    inc(idx);
}

bool Forth::emit(std::string output) {
  ForthEmitVisitor *ev = new ForthEmitVisitor(this);
  /* The main stack */
  llvm::ArrayType *at = llvm::ArrayType::get(_t["i"], _stack_size);
  /* From Constants.cpp :
     Empty arrays are canonicalized to ConstantAggregateZero. */
  llvm::ArrayRef<llvm::Constant *> aref;
  llvm::Constant *ai = llvm::ConstantArray::get(
      /* ArrayType            */ at,
      /* ArrayRef<Constant *> */ aref);
  _st = new llvm::GlobalVariable(
      /* Module       */ *_mod,
      /* Type         */ at,
      /* isConstant   */ false,
      /* Linkage      */ llvm::GlobalValue::LinkageTypes::PrivateLinkage,
      /* Initializer  */ ai,
      /* Name         */ "_st",
      /* InsertBefore */ nullptr);

  _idx = new llvm::GlobalVariable(
      /* Module       */ *_mod,
      /* Type         */ _t["i"],
      /* isConstant   */ false,
      /* Linkange     */ llvm::GlobalValue::LinkageTypes::PrivateLinkage,
      /* Initializer  */ llvm::Constant::getNullValue(_t["i"]),
      /* Name         */ "_idx",
      /* InsertBefore */ _st);

  global_variables["_st"] = _st;
  global_variables["_idx"] = _idx;

  /* The return stack */
  at = llvm::ArrayType::get(_t["i"], _return_stack_size);
  ai = llvm::ConstantArray::get(at, aref);
  _rst = new llvm::GlobalVariable(
      *_mod, at, false, llvm::GlobalValue::LinkageTypes::PrivateLinkage, ai,
      "_rst", nullptr);
  _ridx = new llvm::GlobalVariable(
      *_mod, _t["i"], false, llvm::GlobalValue::LinkageTypes::PrivateLinkage,
      llvm::Constant::getNullValue(_t["i"]), "_ridx", nullptr);
  global_variables["_rst"] = _rst;
  global_variables["_ridx"] = _ridx;

  /* Some globals */
  _base = new llvm::GlobalVariable(
      *_mod, _t["i"], false, llvm::GlobalValue::LinkageTypes::PrivateLinkage,
      llvm::ConstantInt::get(_t["i"], 10), "_base", nullptr);

  /* runtime */
  std::string a = "_nuf_runtime_print_n";
  runtime_functions[a] = llvm::Function::Create(
      llvm::FunctionType::get(_t["v"], llvm::ArrayRef<llvm::Type *>({_t["i"]}),
                              false),
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, a, _mod);
  a = "_nuf_runtime_print_u";
  runtime_functions[a] = llvm::Function::Create(
      llvm::FunctionType::get(_t["v"], llvm::ArrayRef<llvm::Type *>({_t["i"]}),
                              false),
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, a, _mod);
  a = "_nuf_runtime_print_c_n";
  runtime_functions[a] = llvm::Function::Create(
      llvm::FunctionType::get(
          _t["v"], llvm::ArrayRef<llvm::Type *>({_t["c"], _t["i"]}), false),
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, a, _mod);
  a = "_nuf_runtime_get_c";
  runtime_functions[a] = llvm::Function::Create(
      llvm::FunctionType::get(_t["i"], llvm::ArrayRef<llvm::Type *>({}), false),
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, a, _mod);
  a = "_nuf_runtime_put_c";
  runtime_functions[a] = llvm::Function::Create(
      llvm::FunctionType::get(_t["v"], llvm::ArrayRef<llvm::Type *>({_t["i"]}),
                              false),
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, a, _mod);
  a = "_nuf_runtime_print_s";
  runtime_functions[a] = llvm::Function::Create(
      llvm::FunctionType::get(_t["v"], llvm::ArrayRef<llvm::Type *>({_t["c*"]}),
                              false),
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, a, _mod);

  try {
    _program->accept(ev);
  } catch (enum ForthEmitVisitor::X e) {
    switch (e) {
    case ForthEmitVisitor::X::RUNTIME_FUNCTION_ERROR:
      std::cout << "Exception with specifying runtime function " << _error_info
                << std::endl;
      break;
      //      default:
      //	  std::cout << "Unhandled exception " << e << std::endl;
    }
    return false;
  }

  if (!output.empty()) {
    std::error_code EC;
    llvm::raw_fd_ostream OS(output, EC, llvm::sys::fs::F_None);
    if (!EC)
      _mod->print(OS, NULL /* AssemblyAnnotationWriter */);
  }
  //
  // TESTING CODE
  // Dump the module
  // _mod->dump();

  return true;
};

llvm::Function *Forth::startFunction(std::string n,
                                     llvm::GlobalValue::LinkageTypes lt) {
  /*
    define void @FOO() {
    Entry:
    br label %Exit

    Exit:                                             ; preds = %Entry
    ret void
    }
  */
  enum bv { ENTRY = 0, EXIT };
  llvm::Function *f = llvm::Function::Create(_ft_void, lt, n, _mod);
  fs.push_front(f);
  std::vector<llvm::BasicBlock *> bbs = blocks(bv::EXIT + 1);
  bbs[ENTRY]->setName("Entry");
  bbs[EXIT]->setName("Exit");
  leavePush(bbs[EXIT]);
  bbs[EXIT]->getInstList().push_back(llvm::ReturnInst::Create(ctx));
  blockPush(bbs[ENTRY]);
  op_br(bbs[EXIT]);
  return f;
}

void Forth::startProgramFunction() {
  _nuf = startFunction(getEntryName(),
                       llvm::GlobalValue::LinkageTypes::ExternalLinkage);
}

void Forth::endFunction() {
  blockPop();
  leavePop();
  fs.pop_front();
}

void Forth::callLocalFunction(std::string n) {
  // tbd sync with builtin call
  if (user_functions.find(n) != user_functions.end()) {
    llvm::Function *f = user_functions[n];
    llvm::CallInst *op =
        llvm::CallInst::Create(_ft_void, f, llvm::ArrayRef<llvm::Value *>({}));
    in(op);
  } else {
    // tbd handle error
  }
}

void Forth::startLocalFunction(std::string n) {
  llvm::Function *f =
      startFunction(n, llvm::GlobalValue::LinkageTypes::PrivateLinkage);
  addUserFunction(n, f);
}

llvm::BasicBlock *Forth::blockReplace(llvm::BasicBlock *bb) {
  blockPop();
  bbs.push(bb);
  return (bb);
}

llvm::BasicBlock *Forth::blockPush(llvm::BasicBlock *bb) {
  bbs.push(bb);
  return (bb);
}

llvm::BasicBlock *Forth::blockPush(const llvm::Twine &Name) {
  llvm::Function *f = fs.front();
  llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "", f);
  bb->setName(Name);
  return blockPush(bb);
}
void Forth::blockPop() { bbs.pop(); }

std::vector<llvm::BasicBlock *> Forth::blocks(unsigned int n,
                                              llvm::BasicBlock *InsertBefore) {
  llvm::Function *f = fs.front();
  std::vector<llvm::BasicBlock *> r;
  while (n--) {
    r.push_back(llvm::BasicBlock::Create(ctx, "", f, InsertBefore));
  }
  return r;
}

void Forth::syntax_error() {
  char *line = scanner->line();
  char *loc = scanner->loc_string();
  if ((line != NULL) && (loc != NULL))
    fprintf(stderr, "syntax error at %s\n%s\n", loc, line);
  else
    fprintf(stderr, "syntax error at %lu\n", scanner->lineno());

  if (scanner->position() < 400) {
    for (uint64_t l = 0; l < scanner->position(); l++)
      fprintf(stderr, " ");
    fprintf(stderr, "^\n");
    for (uint64_t l = 0; l < scanner->position(); l++)
      fprintf(stderr, " ");
    fprintf(stderr, "|\n");
  }
  if (line != NULL)
    free(line);
  exit(-1);
}

void Forth::leavePush(llvm::BasicBlock *bb) { leave_bbs.push(bb); }
void Forth::leavePop() { leave_bbs.pop(); }
llvm::BasicBlock *Forth::leaveGet() { return leave_bbs.top(); }

std::string Forth::getEntryName() { return _entry_name; }
void Forth::setEntryName(std::string a) { _entry_name = a; }

llvm::BasicBlock *Forth::splitBasicBlock(const llvm::Twine &Name) {
  llvm::BasicBlock *ret = nullptr;
  llvm::BasicBlock *bb = getBasicBlock();

  auto itr = bb->getInstList().end();
  if (itr == bb->getInstList().begin()) {
    // TBD
  } else {
    itr--;
  }
  ret = bb->splitBasicBlock(itr, Name);
  return ret;
}

void Forth::loopPush(llvm::PHINode *a) { _lvs.push_front(a); }
void Forth::loopPop() { _lvs.pop_front(); }

void Forth::moveTerminator(llvm::BasicBlock *a) {
  llvm::BasicBlock *bb = getBasicBlock();
  if (bb) {
    llvm::TerminatorInst *ti = bb->getTerminator();
    if (ti) {
      ti->removeFromParent();
      te(a, ti);
    } else {
      printf("Ooops\n");
    }
  }
}
