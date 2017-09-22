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
#include "visitors/forth_emit.h"
#include "ast.h"
#include "forth.h"
#include <stdio.h>

ForthEmitVisitor::ForthEmitVisitor(Forth *f) { forth = f; }
void ForthEmitVisitor::visitor(Comment *a) {}
void ForthEmitVisitor::visitor(Code *a) {}
void ForthEmitVisitor::visitor(Delimiter *a) {}
void ForthEmitVisitor::visitor(Op *a) {}
void ForthEmitVisitor::visitor(PushOp *a) {
  // TODO
  // May not be needed, literal visistor does this work
  llvm::Constant *c =
      llvm::ConstantInt::get(forth->getStackElementType(), a->getNumber());
  forth->push(c);
}

void ForthEmitVisitor::visitor(BaseOp *a) {
  llvm::Value *v = forth->fetch(forth->_base);
  forth->push(v);
}

void ForthEmitVisitor::visitor(IncLoopStatement *a) {
  enum bv { INITIALIZE = 0, COMPARE, LOOP_TOP, LOOP_BOTTOM, END };

  std::vector<llvm::BasicBlock *> bbs = forth->blocks(bv::END + 1);
  llvm::Constant *increment =
      llvm::ConstantInt::get(forth->getStackElementType(), a->inc->getNumber());

  forth->leavePush(bbs[END]);
  forth->moveTerminator(bbs[END]);
  forth->op_br(bbs[INITIALIZE]);

  /* initialize */
  bbs[INITIALIZE]->setName("inc-loop-init");
  forth->blockReplace(bbs[INITIALIZE]);
  llvm::Value *initial = forth->pop();
  llvm::Value *final = forth->pop();
  forth->op_br(bbs[COMPARE]);

  /* compare */
  bbs[COMPARE]->setName("inc-loop-cmp");
  forth->blockReplace(bbs[COMPARE]);
  llvm::PHINode *loop_variable = forth->phi(2, "loop_variable");
  llvm::Value *cmp = forth->op_equals(loop_variable, final);
  forth->op_condbr(cmp, bbs[END], bbs[LOOP_TOP]);
  /* save the loop variable for i,j access */
  forth->loopPush(loop_variable);

  /* loop top */
  bbs[LOOP_TOP]->setName("inc-loop-top");
  forth->blockReplace(bbs[LOOP_TOP]);
  forth->op_br(bbs[LOOP_BOTTOM]);

  a->stmt_list->accept(this);

  /* loop bootom */
  bbs[LOOP_BOTTOM]->setName("inc-loop-bottom");
  forth->blockReplace(bbs[LOOP_BOTTOM]);
  forth->op_br(bbs[COMPARE]);
  llvm::Value *loop_variable_tmp = forth->op_plus(loop_variable, increment);

  /* end */
  bbs[END]->setName("inc-loop-end");
  forth->blockReplace(bbs[END]);
  forth->leavePop();
  /* remove loop variable */
  forth->loopPop();
  /* Finish phi setup */
  loop_variable->addIncoming(initial, bbs[INITIALIZE]);
  loop_variable->addIncoming(loop_variable_tmp, bbs[LOOP_BOTTOM]);
}

void ForthEmitVisitor::visitor(IOp *a) {
  if (forth->_lvs.size() <= 0) {
    throw Forth::SYNTAX_I;
  } else {
    llvm::Value *v = forth->_lvs[0];
    forth->push(v);
  }
}
void ForthEmitVisitor::visitor(JOp *a) {
  if (forth->_lvs.size() <= 1) {
    throw Forth::SYNTAX_J;
  } else {
    llvm::Value *v = forth->_lvs[1];
    forth->push(v);
  }
}

void ForthEmitVisitor::visitor(IfStatement *a) {

  enum bv { TRUE = 0, END };
  std::vector<llvm::BasicBlock *> bbs = forth->blocks(bv::END + 1);
  llvm::Value *v = forth->pop();
  llvm::Value *cmp = forth->op_equals(v, forth->_ffff);

  forth->moveTerminator(bbs[END]);
  forth->op_condbr(cmp, bbs[TRUE], bbs[END]);

  bbs[TRUE]->setName("if");
  forth->blockReplace(bbs[TRUE]);
  forth->op_br(bbs[END]);

  a->stmt_list->accept(this);

  bbs[END]->setName("end-if");
  forth->blockReplace(bbs[END]);
}

void ForthEmitVisitor::visitor(IfElseStatement *a) {
  enum bv { TRUE = 0, ELSE, END };
  std::vector<llvm::BasicBlock *> bbs = forth->blocks(bv::END + 1);
  llvm::Value *v = forth->pop();
  llvm::Value *cmp = forth->op_equals(v, forth->_ffff);

  forth->moveTerminator(bbs[END]);
  forth->op_condbr(cmp, bbs[TRUE], bbs[ELSE]);

  bbs[TRUE]->setName("if-else");
  forth->blockReplace(bbs[TRUE]);
  forth->op_br(bbs[END]);

  a->stmt_list->accept(this);

  bbs[ELSE]->setName("else");
  forth->blockReplace(bbs[ELSE]);
  forth->op_br(bbs[END]);

  a->else_list->accept(this);

  bbs[END]->setName("end-if-else");
  forth->blockReplace(bbs[END]);
}

void ForthEmitVisitor::visitor(BlOp *a) { forth->push(forth->_space); }

void ForthEmitVisitor::visitor(Symbol *a) { forth->symbol(a); }

void ForthEmitVisitor::visitor(FunctionCall *a) {
  forth->callLocalFunction(a->string());
}

void ForthEmitVisitor::visitor(FunctionDefinition *a) {
  forth->startLocalFunction(a->string());
  a->stmts->accept(this);
  forth->endFunction();
}
void ForthEmitVisitor::visitor(FunctionDefinitionList *a) {
  for (auto e : a->fns) {
    e->accept(this);
  }
}

void ForthEmitVisitor::visitor(Program *a) {
  if (a->fns) {
    a->fns->accept(this);
  }
  forth->startProgramFunction();
  if (a->stmts) {
    a->stmts->accept(this);
  }
  forth->endFunction();
}

void ForthEmitVisitor::visitor(UntilLoopStatement *a) {
  enum bv { LOOP = 0, COMPARE, END };
  std::vector<llvm::BasicBlock *> bbs = forth->blocks(bv::END + 1);

  forth->blockReplace(bbs[LOOP]);
  a->stmt_list->accept(this);
  forth->blockReplace(bbs[COMPARE]);
  llvm::Value *flag = forth->pop();
  llvm::Value *cmp = forth->op_equals(flag, forth->_ffff);
  forth->op_condbr(cmp, bbs[END], bbs[LOOP]);
  forth->blockReplace(bbs[END]);
}

void ForthEmitVisitor::visitor(CheckLoopStatement *a) {
  enum bv { LOOP = 0, COMPARE, CHECK_LOOP, END };
  std::vector<llvm::BasicBlock *> bbs = forth->blocks(bv::END + 1);

  forth->blockReplace(bbs[LOOP]);
  a->stmt_list->accept(this);
  forth->blockReplace(bbs[COMPARE]);
  llvm::Value *flag = forth->pop();
  llvm::Value *cmp = forth->op_equals(flag, forth->_ffff);
  forth->op_condbr(cmp, bbs[CHECK_LOOP], bbs[END]);
  forth->blockReplace(bbs[CHECK_LOOP]);
  a->check_list->accept(this);
  forth->op_br(bbs[LOOP]);
  forth->blockReplace(bbs[END]);
}

void ForthEmitVisitor::visitor(AlignOp *a) { /* NOOP */
}

void ForthEmitVisitor::visitor(LoopStatement *a) {
  enum bv { LOOP, END };
  std::vector<llvm::BasicBlock *> bbs = forth->blocks(bv::END + 1);
  forth->blockReplace(bbs[LOOP]);
  a->stmt_list->accept(this);
  forth->op_br(bbs[LOOP]);
  forth->blockReplace(bbs[END]);
}

void ForthEmitVisitor::visitor(LeaveOp *a) { forth->op_br(forth->leaveGet()); }

void ForthEmitVisitor::visitor(Literal *a) {
  llvm::Constant *c =
      llvm::ConstantInt::get(forth->getStackElementType(), a->getNumber());
  forth->push(c);
}

void ForthEmitVisitor::visitor(Number *a) {
  llvm::Constant *c =
      llvm::ConstantInt::get(forth->getStackElementType(), a->getNumber());
  forth->push(c);
}

void ForthEmitVisitor::visitor(QuitOp *a) {
  llvm::CallInst *call = nullptr;
  std::string f = "_nuf_runtime_end";
  call = forth->op_call(f);
  if (call == nullptr) {
    forth->x_error_info = f;
    throw(Forth::RUNTIME_FUNCTION_ERROR);
  }
}

void ForthEmitVisitor::visitor(SimpleStatement *a) { a->stmt->accept(this); }

void ForthEmitVisitor::visitor(StatementList *a) {
  /*
    define void @_nuf() {
    Entry:
    br label %statement-list

    statement-list:                                   ; preds = %Entry
    br label %Exit

    Exit:                                             ; preds = %statement-list
    ret void
    }
  */

  llvm::BasicBlock *nbb = forth->splitBasicBlock("statement-list");
  forth->blockPush(nbb);
  for (auto e : a->stmts) {
    e->accept(this);
  }
  forth->blockPop();
}

void ForthEmitVisitor::visitor(False *a) { forth->push(forth->_zero); }

void ForthEmitVisitor::visitor(True *a) { forth->push(forth->_ffff); }

void ForthEmitVisitor::visitor(HereOp *a) { forth->here(); }

void ForthEmitVisitor::visitor(HexOp *a) {
  llvm::Constant *c = llvm::ConstantInt::get(forth->_t["i"], 16);
  forth->store(forth->_base, c);
}

void ForthEmitVisitor::visitor(DecimalOp *a) {
  llvm::Constant *c = llvm::ConstantInt::get(forth->_t["i"], 10);
  forth->store(forth->_base, c);
}

void ForthEmitVisitor::visitor(DepthOp *a) {
  llvm::Value *v = forth->fetch(forth->_idx);
  forth->push(v);
}

void ForthEmitVisitor::visitor(SToDOp *a) {
  llvm::Value *v = forth->pop();
  llvm::Value *r = forth->op_cast(v);
  forth->push(r);
}

void ForthEmitVisitor::visitor(CellPlusOp *a) {
  llvm::Value *v1 = forth->_cell;
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_plus(v0, v1);
  forth->push(r);
}

void ForthEmitVisitor::visitor(CellsOp *a) {
  llvm::Value *v1 = forth->_cell;
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_multiply(v0, v1);
  forth->push(r);
}

void ForthEmitVisitor::visitor(DecOp *a) {
  llvm::Value *v1 = forth->_one;
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_minus(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(DivideOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_divide(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(DivideModuloOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r0 = forth->op_modulo(v0, v1);
  llvm::Value *r1 = forth->op_divide(v0, v1);
  forth->push(r0);
  forth->push(r1);
}

void ForthEmitVisitor::visitor(KeyOp *a) {
  llvm::Value *r;
  std::string f = "_nuf_runtime_get_c";
  r = forth->op_call(f);
  if (r == nullptr) {
    forth->x_error_info = f;
    throw(Forth::RUNTIME_FUNCTION_ERROR);
  }
  forth->push(r);
}

void ForthEmitVisitor::visitor(EmitOp *a) {
  llvm::Value *v = forth->pop();
  std::string f = "_nuf_runtime_put_c";
  llvm::CallInst *o = nullptr;

  o = forth->op_call(f, v);
  if (o == nullptr) {
    forth->x_error_info = f;
    throw(Forth::RUNTIME_FUNCTION_ERROR);
  }
}

void ForthEmitVisitor::visitor(DotOp *a) {
  llvm::Value *v = forth->pop();
  llvm::CallInst *o = nullptr;
  std::string f = "_nuf_runtime_print_n";
  o = forth->op_call(f, v);
  if (o == nullptr) {
    forth->x_error_info = f;
    throw(Forth::RUNTIME_FUNCTION_ERROR);
  }
}
void ForthEmitVisitor::visitor(UnsignedDotOp *a) {
  llvm::Value *v = forth->pop();
  llvm::CallInst *o = nullptr;
  std::string f = "_nuf_runtime_print_u";
  o = forth->op_call(f, v);
  if (o == nullptr) {
    forth->x_error_info = f;
    throw(Forth::RUNTIME_FUNCTION_ERROR);
  }
}

void ForthEmitVisitor::visitor(CrOp *a) {
  llvm::CallInst *o = nullptr;
  std::string f = "_nuf_runtime_print_c_n";
  o = forth->op_call(f, forth->_cr, forth->_one);
  if (o == nullptr) {
    forth->x_error_info = f;
    throw(Forth::RUNTIME_FUNCTION_ERROR);
  }
}

void ForthEmitVisitor::visitor(SpaceOp *a) {
  llvm::CallInst *o = nullptr;
  std::string f = "_nuf_runtime_print_c_n";
  o = forth->op_call(f, forth->_space, forth->_one);
  if (o == nullptr) {
    forth->x_error_info = f;
    throw(Forth::RUNTIME_FUNCTION_ERROR);
  }
}

void ForthEmitVisitor::visitor(SpacesOp *a) {
  llvm::CallInst *o = nullptr;
  llvm::Value *v = forth->pop();
  std::string f = "_nuf_runtime_print_c_n";
  o = forth->op_call(f, forth->_space, v);
  if (o == nullptr) {
    forth->x_error_info = f;
    throw(Forth::RUNTIME_FUNCTION_ERROR);
  }
}

void ForthEmitVisitor::visitor(DropOp *a) { forth->pop(); }

void ForthEmitVisitor::visitor(DupOp *a) {
  llvm::Value *v = forth->pop();
  forth->push(v);
  forth->push(v);
}

void ForthEmitVisitor::visitor(Dup2Op *a) {
  llvm::Value *va = forth->pop();
  llvm::Value *vb = forth->pop();

  forth->push(vb);
  forth->push(va);
  forth->push(vb);
  forth->push(va);
}

void ForthEmitVisitor::visitor(EqualsZeroOp *a) {
  llvm::Value *v = forth->pop();
  llvm::Value *cmp = forth->op_equals(v, forth->_zero);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(NotEqualsOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_not_equals(v0, v1);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(NotEqualsZeroOp *a) {
  llvm::Value *v = forth->pop();
  llvm::Value *cmp = forth->op_not_equals(v, forth->_zero);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(ExclusiveOrOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_exclusive_or(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(IncOp *a) {
  llvm::Value *v1 = forth->_one;
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_plus(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(InvertOp *a) {
  llvm::Value *v = forth->pop();
  llvm::Value *r = forth->op_not(v);
  forth->push(r);
}
void ForthEmitVisitor::visitor(LeftShiftOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_leftshift(v0, v1);
  forth->push(r);
}

void ForthEmitVisitor::visitor(LessThanZeroOp *a) {
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_less_than_signed(v0, forth->_zero);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(GreaterThanZeroOp *a) {
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_greater_than_signed(v0, forth->_zero);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(LessThanSignedOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_less_than_signed(v0, v1);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(LessThanUnsignedOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_less_than_unsigned(v0, v1);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(GreaterThanSignedOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_greater_than_signed(v0, v1);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(GreaterThanUnsignedOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_greater_than_unsigned(v0, v1);
  llvm::Value *r = forth->op_select(cmp, forth->_ffff, forth->_zero);
  forth->push(r);
}

void ForthEmitVisitor::visitor(MaxOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_greater_than_signed(v0, v1);
  llvm::Value *r = forth->op_select(cmp, v0, v1);
  forth->push(r);
}

void ForthEmitVisitor::visitor(MinOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *cmp = forth->op_less_than_signed(v0, v1);
  llvm::Value *r = forth->op_select(cmp, v0, v1);
  forth->push(r);
}

void ForthEmitVisitor::visitor(MinusOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_minus(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(ModuloOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_modulo(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(MultiplyOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_multiply(v0, v1);
  forth->push(r);
}

void ForthEmitVisitor::visitor(MultiplyDivideOp *a) {
  llvm::Value *v2 = forth->pop();
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *m = forth->op_multiply(v0, v1);
  llvm::Value *r = forth->op_divide(m, v2);
  forth->push(r);
}
void ForthEmitVisitor::visitor(MultiplyDivideModuloOp *a) {
  llvm::Value *v2 = forth->pop();
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *m = forth->op_multiply(v0, v1);
  llvm::Value *r0 = forth->op_modulo(m, v2);
  llvm::Value *r1 = forth->op_divide(m, v2);
  forth->push(r0);
  forth->push(r1);
}
void ForthEmitVisitor::visitor(NegateOp *a) {
  llvm::Value *v1 = forth->_minus_one;
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_multiply(v0, v1);
  r->setName("negate");
  forth->push(r);
}
void ForthEmitVisitor::visitor(OrOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_or(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(OverOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  forth->push(v0);
  forth->push(v1);
  forth->push(v0);
}

void ForthEmitVisitor::visitor(Over2Op *a) {
  llvm::Value *v1a = forth->pop();
  llvm::Value *v1b = forth->pop();
  llvm::Value *v0a = forth->pop();
  llvm::Value *v0b = forth->pop();
  forth->push(v0b);
  forth->push(v0a);
  forth->push(v1b);
  forth->push(v1a);
  forth->push(v0b);
  forth->push(v0a);
}

void ForthEmitVisitor::visitor(PlusOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_plus(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(RightShiftOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->op_rightshift(v0, v1);
  forth->push(r);
}
void ForthEmitVisitor::visitor(RotateOp *a) {
  llvm::Value *v2 = forth->pop();
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  forth->push(v2);
  forth->push(v0);
  forth->push(v1);
}
void ForthEmitVisitor::visitor(SwapOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  forth->push(v1);
  forth->push(v0);
}

void ForthEmitVisitor::visitor(Swap2Op *a) {
  llvm::Value *v1a = forth->pop();
  llvm::Value *v1b = forth->pop();
  llvm::Value *v0a = forth->pop();
  llvm::Value *v0b = forth->pop();
  forth->push(v1b);
  forth->push(v1a);
  forth->push(v0b);
  forth->push(v0a);
}

void ForthEmitVisitor::visitor(NipOp *a) {
  llvm::Value *v1 = forth->pop();
  forth->pop();
  forth->push(v1);
}

void ForthEmitVisitor::visitor(TuckOp *a) {
  llvm::Value *v1 = forth->pop();
  llvm::Value *v0 = forth->pop();
  forth->push(v1);
  forth->push(v0);
  forth->push(v1);
}

void ForthEmitVisitor::visitor(FetchOp *a) {
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->fetch(v0);
  forth->push(r);
}

void ForthEmitVisitor::visitor(FetchCOp *a) {
  llvm::Value *v0 = forth->pop();
  llvm::Value *r = forth->fetchC(v0);
  forth->push(r);
}

void ForthEmitVisitor::visitor(FromReturnOp *a) {
  llvm::Value *v0 = forth->popR();
  forth->push(v0);
}

void ForthEmitVisitor::visitor(ToReturnOp *a) {
  llvm::Value *v0 = forth->pop();
  forth->pushR(v0);
}

void ForthEmitVisitor::visitor(StoreOp *a) {
  llvm::Value *v0 = forth->pop();
  llvm::Value *v1 = forth->pop();
  forth->store(v0, v1);
}

void ForthEmitVisitor::visitor(StoreCOp *a) {
  llvm::Value *v0 = forth->pop();
  llvm::Value *v1 = forth->pop();
  forth->storeC(v0, v1);
}

void ForthEmitVisitor::visitor(StoreReturnOp *a) {
  llvm::Value *v0 = forth->popR();
  forth->pushR(v0);
  forth->push(v0);
}

void ForthEmitVisitor::visitor(String *a) {
  char *b = strdup(a->string().c_str());
  if (b) {
    size_t s = strlen(b);
    if (s >= 3) {
      if (s != 3) {
        llvm::CallInst *o = nullptr;
        std::string f = "_nuf_runtime_print_s";
        char *sb = b;
        /* remove trailing '"' */
        sb[s - 1] = 0;
        /* remove leading '."' */
        sb += 2;
        o = forth->op_call(f, forth->string(sb));
        if (o == nullptr) {
          forth->x_error_info = f;
          throw(Forth::RUNTIME_FUNCTION_ERROR);
        }
      }
      free(b);
    } else {
      free(b);
      throw Forth::SYNTAX_STRING;
    }
  } else {
    throw Forth::MEMORY;
  }
}
void ForthEmitVisitor::visitor(Constant *a) {
  auto i = forth->global_variables.find(a->name());
  if (i == forth->global_variables.end()) {
    // TODO
    // HANDLE NON-INTRINSIC
    llvm::Type *t = forth->_t[a->type_name()];
    llvm::GlobalVariable *v;
    v = new llvm::GlobalVariable(
        *(forth->_mod), t, true,
        llvm::GlobalValue::LinkageTypes::PrivateLinkage,
        llvm::ConstantInt::get(t, a->initializer()), a->name(), nullptr);
    forth->global_variables[a->name()] = v;
  } else {
    // TODO
    // HANDLE ERROR
  }
}
void ForthEmitVisitor::visitor(VariableDefinition *a) {
  auto i = forth->global_variables.find(a->getString());
  if (i == forth->global_variables.end()) {
    // TODO
    // HANDLE NON-INTRINSIC
    size_t els = 0;
    if (a->size())
      els = (a->size() + forth->pointer_size() - 1) / forth->pointer_size();
    llvm::ArrayType *at = llvm::ArrayType::get(forth->_t["i"], els);
    llvm::ArrayRef<llvm::Constant *> aref;
    llvm::Constant *ai = llvm::ConstantArray::get(at, aref);
    llvm::GlobalVariable *v;
    v = new llvm::GlobalVariable(
        *(forth->_mod), at, false,
        llvm::GlobalValue::LinkageTypes::PrivateLinkage, ai, a->getString(),
        nullptr);
    forth->global_variables[a->getString()] = v;
  } else {
    // TODO
    // HANDLE ERROR
  }
}
void ForthEmitVisitor::visitor(N *a) {}
