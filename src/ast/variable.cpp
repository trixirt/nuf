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
#include "ast/variable.h"
#include "ast/code.h"

Variable::Variable(std::shared_ptr<N> a, std::string type_name)
    : _type_name(type_name) {
  Literal *lit = dynamic_cast<Literal *>(a.get());
  if (lit) {
    SourceLocation sl = SourceLocation(lit->lineno(), lit->position());
    _name = lit->text();
    _decl_location = sl;
  }
}
void Variable::accept(Visitor *a) { a->visitor(this); };
std::string Variable::name() { return _name; };
std::string Variable::type_name() { return _type_name; };

VariableDefinition::VariableDefinition(std::shared_ptr<Symbol> a, size_t size)
    : SymbolDefinition(a), _size(size) {}
void VariableDefinition::accept(Visitor *a) { a->visitor(this); };
size_t VariableDefinition::size() { return _size; };

Constant::Constant(std::shared_ptr<N> symbol_name, std::string type_name,
                   std::shared_ptr<N> initializer)
    : Variable(symbol_name, type_name) {
  Literal *lit = dynamic_cast<Literal *>(initializer.get());
  if (lit) {
    _initializer = lit->getNumber();
  }
}
void Constant::accept(Visitor *a) { a->visitor(this); };
int Constant::initializer() { return _initializer; }

ConstantDefinition::ConstantDefinition(std::shared_ptr<Symbol> a,
                                       std::shared_ptr<Number> b)
    : SymbolDefinition(a), initializer(b) {}
void ConstantDefinition::accept(Visitor *a) { a->visitor(this); };
