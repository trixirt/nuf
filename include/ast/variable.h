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
#ifndef NUF_AST_VARIABLE_H
#define NUF_AST_VARIABLE_H

#include "source_location.h"
#include "symbol.h"
#include "type.h"
#include <vector>

class Variable : public N {
public:
  Variable(std::shared_ptr<N> a, std::string type_name);
  virtual void accept(Visitor *a);
  std::string name();
  std::string type_name();

protected:
  std::string _name;
  std::string _type_name;
  SourceLocation _decl_location;
  std::vector<SourceLocation> _use_locations;
};

class VariableDefinition : public SymbolDefinition {
public:
  VariableDefinition(std::shared_ptr<Symbol> a, size_t size);
  virtual void accept(Visitor *a);
  size_t size();

protected:
  size_t _size;
};

class Constant : public Variable {
public:
  Constant(std::shared_ptr<N> symbol_name, std::string type_name,
           std::shared_ptr<N> initializer);
  virtual void accept(Visitor *a);
  int initializer();

protected:
  // xxx this needs work
  int _initializer;
};

class ConstantDefinition : public SymbolDefinition {
public:
  ConstantDefinition(std::shared_ptr<Symbol> a, std::shared_ptr<Number> b);
  virtual void accept(Visitor *a);

protected:
  std::shared_ptr<Number> initializer;
};

#endif
