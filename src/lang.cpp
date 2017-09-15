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
#include "lang.h"

Lang::Lang(std::shared_ptr<CPU> c, std::string filename) : _cpu(c) {
  _mod = new llvm::Module("M", ctx);
}

Lang::~Lang() {
  user_functions.clear();
  delete _mod;
  tokens.clear();
}

bool Lang::lex() {
  bool ret = false;
  std::shared_ptr<N> token;
  int tokenID;

  while (1) {
    tokenID = 0;
    token = scan(&tokenID);
    if (token) {
      tokens.push_back(token);
    }

    /* EOF */
    if (tokenID == 0) {
      parse(0, token);
      break;
    }

    if (!is_comment(token.get())) {
      parse(tokenID, token);
    }
    token = NULL;
  }
  ret = true;
  return ret;
}

bool Lang::addUserFunction(std::string n, llvm::Function *f) {
  bool ret = false;
  user_functions[n] = f;
  ret = true;

  return ret;
}
