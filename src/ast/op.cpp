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
#include "ast/op.h"

StoreBaseOp::StoreBaseOp(std::shared_ptr<N> a, std::shared_ptr<N> b)
    : BinaryOp("Store", a, b) {}
StoreBaseOp::StoreBaseOp(std::shared_ptr<N> a) : BinaryOp("Store", a) {}
StoreBaseOp::StoreBaseOp() : BinaryOp("Store") {}
void StoreBaseOp::accept(Visitor *a) { a->visitor(this); };
std::string StoreBaseOp::name() {
  std::string r;
  Literal *lit = nullptr;
  if (args.size() == 1)
    lit = dynamic_cast<Literal *>(args[0].get());
  else if (args.size() == 2)
    lit = dynamic_cast<Literal *>(args[1].get());
  if (lit) {
    r = lit->text();
  }
  return r;
}

// TODO
// Refactor fetch to a base class

FetchOp::FetchOp(std::shared_ptr<N> a, std::shared_ptr<N> b)
    : BinaryOp("Fetch", a, b) {}
FetchOp::FetchOp(std::shared_ptr<N> b) : BinaryOp("Fetch", b) {}
FetchOp::FetchOp() : BinaryOp("Fetch") {}
void FetchOp::accept(Visitor *a) { a->visitor(this); };

std::string FetchOp::name() {
  std::string r;
  Literal *lit = nullptr;
  if (args.size() == 1)
    lit = dynamic_cast<Literal *>(args[0].get());
  else if (args.size() == 2)
    lit = dynamic_cast<Literal *>(args[1].get());
  if (lit) {
    r = lit->text();
  }
  return r;
}

FetchCOp::FetchCOp(std::shared_ptr<N> a, std::shared_ptr<N> b)
    : BinaryOp("FetchC", a, b) {}
FetchCOp::FetchCOp(std::shared_ptr<N> b) : BinaryOp("FetchC", b) {}
void FetchCOp::accept(Visitor *a) { a->visitor(this); };

std::string FetchCOp::name() {
  std::string r;
  Literal *lit = nullptr;
  if (args.size() == 1)
    lit = dynamic_cast<Literal *>(args[0].get());
  else if (args.size() == 2)
    lit = dynamic_cast<Literal *>(args[1].get());
  if (lit) {
    r = lit->text();
  }
  return r;
}
