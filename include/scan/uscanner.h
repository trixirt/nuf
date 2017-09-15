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
#ifndef NUF_USCANNER_H
#define NUF_USCANNER_H
#include "ast.h"
#include "scanner.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

class uscanner : public scanner {
public:
  uscanner(std::string filename, const char *pre = NULL, size_t n = 0);
  virtual ~uscanner();
  virtual std::shared_ptr<N> scan(int *) = 0;

protected:
  std::shared_ptr<N> code();
  std::shared_ptr<N> comment();
  std::shared_ptr<N> delimiter();
  std::shared_ptr<N> empty();
  std::shared_ptr<N> error();
  std::shared_ptr<N> literal();
  std::shared_ptr<N> number();
  std::shared_ptr<N> string();
  std::shared_ptr<N> symbol();
};

class forth_scanner : public uscanner {
public:
  forth_scanner(std::string filename, const char *pre = NULL, size_t n = 0);
  ~forth_scanner();
  std::shared_ptr<N> scan(int *) override;
};

#endif /* NUF_USCANNER_H */
