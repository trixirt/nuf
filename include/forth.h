/*
 * Copyright (c) 2016-2017 Tom Rix
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
#ifndef NUF_FORTH_H
#define NUF_FORTH_H

#include "lang.h" // for Lang
#include "visitors/forth_emit.h"
#include <stddef.h> // for size_t
#include <string>   // for string
namespace llvm {
class BinaryOperator;
class Constant;
class Function;
class GlobalVariable;
class LoadInst;
class Type;
class Value;
} // namespace llvm

class Forth : public Lang {
public:
  Forth(std::shared_ptr<CPU> cpu, std::string filename);
  ~Forth();

  std::shared_ptr<N> scan(int *tokenid) override;
  int parse(int tokenID, std::shared_ptr<N> token) override;
  bool emit(std::string filename) override;
  llvm::LoadInst *fetch(llvm::Value *a);
  llvm::CastInst *fetchC(llvm::Value *a);
  void store(llvm::Value *a, llvm::Value *b);
  void storeC(llvm::Value *a, llvm::Value *b);
  llvm::CastInst *op_cast(llvm::Value *a);
  llvm::CallInst *op_call(std::string function_name);
  llvm::CallInst *op_call(std::string function_name, llvm::Value *a);
  llvm::CallInst *op_call(std::string function_name, llvm::Value *a,
                          llvm::Value *b);
  llvm::ICmpInst *op_equals(llvm::Value *a, llvm::Value *b);
  llvm::ICmpInst *op_greater_than_signed(llvm::Value *a, llvm::Value *b);
  llvm::ICmpInst *op_greater_than_unsigned(llvm::Value *a, llvm::Value *b);
  llvm::ICmpInst *op_less_than_signed(llvm::Value *a, llvm::Value *b);
  llvm::ICmpInst *op_less_than_unsigned(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_exclusive_or(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_divide(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_leftshift(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_minus(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_modulo(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_multiply(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_not(llvm::Value *a);
  llvm::BinaryOperator *op_or(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_plus(llvm::Value *a, llvm::Value *b);
  llvm::BinaryOperator *op_rightshift(llvm::Value *a, llvm::Value *b);
  llvm::BranchInst *op_br(llvm::BasicBlock *b);
  llvm::BranchInst *op_condbr(llvm::Value *a, llvm::BasicBlock *t,
                              llvm::BasicBlock *f);
  llvm::SelectInst *op_select(llvm::Value *c, llvm::Value *t, llvm::Value *f);
  llvm::Type *getStackElementType() { return _t["i"]; }

  void syntaxError(const char *a = nullptr, Code *b = nullptr);

  void leavePush(llvm::BasicBlock *bb);
  void leavePop();
  llvm::BasicBlock *leaveGet();

  std::string getEntryName();
  void setEntryName(std::string a);

private:
  llvm::Function *startFunction(std::string n,
                                llvm::GlobalValue::LinkageTypes lt);
  void startLocalFunction(std::string n);
  void startProgramFunction();
  void endFunction();
  void callLocalFunction(std::string n);

  llvm::BasicBlock *blockPush(const llvm::Twine &Name = "");
  llvm::BasicBlock *blockPush(llvm::BasicBlock *bb);
  llvm::BasicBlock *blockReplace(llvm::BasicBlock *bb);
  void blockPop();
  std::vector<llvm::BasicBlock *>
  blocks(unsigned int n, llvm::BasicBlock *InsertBefore = nullptr);
  std::stack<llvm::BasicBlock *> leave_bbs;
  llvm::BasicBlock *splitBasicBlock(const llvm::Twine &Name = "");
  void loopPush(llvm::PHINode *a);
  void loopPop();
  void moveTerminator(llvm::BasicBlock *a);

  void inc();
  void inc(llvm::GlobalVariable *v);
  void dec();
  void dec(llvm::GlobalVariable *v);
  llvm::LoadInst *get();
  llvm::LoadInst *get2();
  llvm::LoadInst *get(llvm::GlobalVariable *v, llvm::GlobalVariable *i,
                      llvm::Type *t);
  llvm::LoadInst *pop();
  llvm::LoadInst *pop2();
  llvm::LoadInst *popR();
  llvm::CastInst *popC();
  llvm::LoadInst *pop(llvm::GlobalVariable *v, llvm::GlobalVariable *i,
                      llvm::Type *t);
  void symbol(Symbol *a);
  void here();
  void push(llvm::Value *a);
  void pushC(llvm::Value *a);
  void pushR(llvm::Value *a);
  void push(llvm::Value *a, llvm::GlobalVariable *v, llvm::GlobalVariable *i);
  void set(llvm::Value *a);
  void set(llvm::Value *a, llvm::GlobalVariable *v, llvm::GlobalVariable *i);
  llvm::PHINode *phi(unsigned int n, const llvm::Twine &Name = "");

  void in(llvm::BasicBlock *b, llvm::Instruction *i);
  void in(llvm::Instruction *i);
  void te(llvm::BasicBlock *b, llvm::Instruction *i);
  void te(llvm::Instruction *i);
  void ph(llvm::BasicBlock *b, llvm::Instruction *i);
  void ph(llvm::Instruction *i);

  llvm::Constant *constant(int a);
  llvm::LoadInst *string(char *a);

  size_t _stack_size;
  size_t _return_stack_size;
  llvm::GlobalVariable *_st;    /* stack */
  llvm::GlobalVariable *_idx;   /* stack index */
  llvm::GlobalVariable *_rst;   /* return stack */
  llvm::GlobalVariable *_ridx;  /* return stack index */
  llvm::GlobalVariable *_base;  /* base for number conversions */
  llvm::Constant *_zero;        /* 0 */
  llvm::Constant *_one;         /* 1 */
  llvm::Constant *_minus_one;   /* -1 */
  llvm::Constant *_ffff;        /* -1 */
  llvm::Constant *_space;       /* ' ' */
  llvm::Constant *_cr;          /* '\n' */
  llvm::Constant *_cell;        /* size of a cell */
  llvm::Function *_nuf;         /* main function */
  llvm::FunctionType *_ft_void; /* every functions type */

  std::string _entry_name;
  /* Friends */
  friend class ForthEmitVisitor;
  /* For execptions */
  std::string x_error_info;
  Code *x_code;
  enum X {
    SYNTAX_I,
    SYNTAX_J,
    SYNTAX_STRING,
    MEMORY,
    RUNTIME_FUNCTION_ERROR,
    UNDEFINED_SYMBOL
  };
};

#endif
