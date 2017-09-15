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
#ifndef NUF_LANG_H
#define NUF_LANG_H

#include "ast.h"
#include "cpu.h"
#include "scan/uscanner.h"
#include <memory>
#include <stack>
#include <string>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

class Lang {
public:
  Lang() = delete;
  Lang(std::shared_ptr<CPU> c, std::string f);
  virtual ~Lang();
  bool is_comment(N *a) {
    if (dynamic_cast<Comment *>(a) != nullptr) {
      return true;
    }
    return false;
  }
  bool is_op(N *a) {
    if (dynamic_cast<Op *>(a) != nullptr) {
      return true;
    }
    return false;
  }
  size_t pointer_size() { return _cpu->pointer_size(); }
  virtual std::shared_ptr<N> scan(int *) = 0;
  virtual int parse(int tokenID, std::shared_ptr<N> token) = 0;
  virtual bool emit(std::string filename) = 0;
  bool lex();
  llvm::BasicBlock *getBasicBlock() { return bbs.top(); }

  std::shared_ptr<Program> _program;
  bool addUserFunction(std::string n, llvm::Function *f);

protected:
  llvm::LLVMContext ctx;
  llvm::Module *_mod;
  std::vector<std::shared_ptr<N>> tokens; /* token list */
  uscanner *scanner;
  void *parser;
  std::stack<llvm::BasicBlock *> bbs;
  std::deque<llvm::Function *> fs;
  std::deque<llvm::Value *> _lvs;         /* loop variables */
  std::map<std::string, llvm::Type *> _t; /* types */
  std::map<std::string, llvm::GlobalVariable *> global_variables;
  std::map<std::string, llvm::Function *> runtime_functions;
  std::map<std::string, llvm::Function *> user_functions;
  std::map<std::string, llvm::GlobalVariable *> strings;
  std::shared_ptr<CPU> _cpu;

private:
};

#endif
