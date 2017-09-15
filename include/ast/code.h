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
#ifndef NUF_AST_CODE_H
#define NUF_AST_CODE_H

#include <string>

#include "ast/n.h"
#include "ast/source_location.h"

#define AST_INTERNAL_LINENO -1
#define AST_INTERNAL_POSITION -1

/*
 * C
 * Code
 */
class Code : public N {
public:
  Code(std::string text, uint64_t lineno, uint64_t position)
      : sl(lineno, position), d(text) {}
  Code(std::string text) : sl(), d(text) {}
  const char *text();
  std::string string();
  void clear();
  uint64_t lineno();
  uint64_t position();
  virtual void accept(Visitor *a);

private:
  SourceLocation sl;
  std::string d;
};

class String : public Code {
public:
  String(std::string text, uint64_t lineno, uint64_t position)
      : Code(text, lineno, position) {}
  String(std::string text) : Code(text) {}
  virtual void accept(Visitor *a);
};

class Literal : public Code {
public:
  Literal(std::string text, uint64_t lineno, uint64_t position)
      : Code(text, lineno, position) {}
  Literal(std::string text) : Code(text) {}
  virtual void accept(Visitor *a);
  int getNumber();
  std::string getString();
};

class False : public Code {
public:
  False(std::string text, uint64_t lineno, uint64_t position)
      : Code(text, lineno, position) {}
  False() : Code("False") {}
  virtual void accept(Visitor *a);
};

class True : public Code {
public:
  True(std::string text, uint64_t lineno, uint64_t position)
      : Code(text, lineno, position) {}
  True() : Code("True") {}
  virtual void accept(Visitor *a);
};

class Number : public Literal {
public:
  Number(std::string text, uint64_t lineno, uint64_t position)
      : Literal(text, lineno, position) {}
  Number(std::string text) : Literal(text) {}
  virtual void accept(Visitor *a);
};

#endif
