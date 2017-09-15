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
#ifndef NUF_AST_STATEMENT_H
#define NUF_AST_STATEMENT_H

#include "code.h"
#include "source_location.h"
#include "type.h"
#include <vector>

class Statement : public N {
public:
  virtual void accept(Visitor *a);
};

class SimpleStatement : public Statement {
public:
  SimpleStatement(std::shared_ptr<N> a) { stmt = a; }
  virtual void accept(Visitor *a);
  std::shared_ptr<N> stmt;
};

class CallStatement : public Statement {
public:
  CallStatement(std::shared_ptr<N> a) { stmt = a; }
  virtual void accept(Visitor *a);
  std::shared_ptr<N> stmt;
};

class StatementList : public N {

public:
  StatementList(std::shared_ptr<Statement> a) { stmts.push_back(a); }
  void append(std::shared_ptr<Statement> a);
  std::vector<std::shared_ptr<Statement>> stmts;
  virtual void accept(Visitor *a);
};

class CompoundStatement : public Statement {
public:
  CompoundStatement(std::shared_ptr<StatementList> a) { stmt_list = a; }
  virtual void accept(Visitor *a);
  std::shared_ptr<StatementList> stmt_list;
};

class IfStatement : public Statement {
public:
  IfStatement(std::shared_ptr<StatementList> a) { stmt_list = a; }
  virtual void accept(Visitor *a);
  std::shared_ptr<StatementList> stmt_list;
};

class IfElseStatement : public IfStatement {
public:
  IfElseStatement(std::shared_ptr<StatementList> a,
                  std::shared_ptr<StatementList> b)
      : IfStatement(a) {
    else_list = b;
  }
  virtual void accept(Visitor *a);
  std::shared_ptr<StatementList> else_list;
};

class LoopStatement : public Statement {
public:
  LoopStatement(std::shared_ptr<StatementList> a);
  virtual void accept(Visitor *a);
  std::shared_ptr<StatementList> stmt_list;
};

class IncLoopStatement : public LoopStatement {
public:
  IncLoopStatement(std::shared_ptr<StatementList> a);
  IncLoopStatement(std::shared_ptr<StatementList> a, std::shared_ptr<N> b);
  virtual void accept(Visitor *a);
  std::shared_ptr<Number> inc;
};

class UntilLoopStatement : public LoopStatement {
public:
  UntilLoopStatement(std::shared_ptr<StatementList> a);
  virtual void accept(Visitor *a);
};

class CheckLoopStatement : public LoopStatement {
public:
  CheckLoopStatement(std::shared_ptr<StatementList> a,
                     std::shared_ptr<StatementList> b)
      : LoopStatement(a) {
    check_list = b;
  }
  virtual void accept(Visitor *a);
  std::shared_ptr<StatementList> check_list;
};

#endif
