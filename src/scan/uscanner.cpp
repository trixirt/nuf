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
#include "scan/uscanner.h"
#include "ast.h"
#include <assert.h>

uscanner::uscanner(std::string filename, const char *pre, size_t n)
    : scanner(filename, pre, n) {}
uscanner::~uscanner() {}

std::shared_ptr<N> uscanner::code() {
  char *s = NULL;
  std::shared_ptr<Code> n;
  if (str(&s, m_pstart, m_p)) {
    n = std::shared_ptr<Code>(new Code(s, lineno(), position()));
    free(s);
  }
  return n;
}

std::shared_ptr<N> uscanner::number() {
  char *s = NULL;
  std::shared_ptr<Number> n;
  if (str(&s, m_pstart, m_p)) {
    n = std::shared_ptr<Number>(new Number(s, lineno(), position()));
    free(s);
  }
  return n;
}

std::shared_ptr<N> uscanner::literal() {
  char *s = NULL;
  std::shared_ptr<Literal> n;
  if (str(&s, m_pstart, m_p)) {
    n = std::shared_ptr<Literal>(new Literal(s, lineno(), position()));
    free(s);
  }
  return n;
}

std::shared_ptr<N> uscanner::symbol() {
  char *s = NULL;
  std::shared_ptr<Symbol> n;
  if (str(&s, m_pstart, m_p)) {
    n = std::shared_ptr<Symbol>(new Symbol(s, lineno(), position()));
    free(s);
  }
  return n;
}

std::shared_ptr<N> uscanner::error() {
  char *s = NULL;
  std::shared_ptr<Error> n;
  if (str(&s, m_pstart, m_p)) {
    n = std::shared_ptr<Error>(new Error(s, lineno(), position()));
    free(s);
  }
  return n;
}

std::shared_ptr<N> uscanner::empty() {
  std::shared_ptr<N> n;
  return n;
}

std::shared_ptr<N> uscanner::comment() {
  char *s = NULL;
  std::shared_ptr<Comment> n;
  if (str(&s, m_pstart, m_p)) {
    n = std::shared_ptr<Comment>(new Comment(s, lineno(), position()));
    free(s);
  }
  return n;
}

std::shared_ptr<N> uscanner::delimiter() {
  char *s = NULL;
  std::shared_ptr<Delimiter> n;
  if (str(&s, m_pstart, m_p)) {
    n = std::shared_ptr<Delimiter>(new Delimiter(s, lineno(), position()));
    free(s);
  }
  return n;
}

std::shared_ptr<N> uscanner::string() {
  char *s = NULL;
  std::shared_ptr<String> n;
  if (str(&s, m_pstart, m_p)) {
    n = std::shared_ptr<String>(new String(s, lineno(), position()));
    free(s);
  }
  return n;
}

forth_scanner::forth_scanner(std::string filename, const char *pre, size_t n)
    : uscanner(filename, pre, n) {}
forth_scanner::~forth_scanner() {}
