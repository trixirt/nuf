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
#ifndef NUF_AST_OP_H
#define NUF_AST_OP_H

#include "ast/code.h"
#include "ast/n.h"
#include <assert.h>
#include <string>
#include <vector>

class Op : public N {
public:
  Op(std::string a) { op = a; }
  virtual void accept(Visitor *a) { a->visitor(this); };
  int getNumber(size_t idx = 0) {
    int ret = 0;
    Number *n = dynamic_cast<Number *>(args[idx].get());
    assert(n != nullptr);
    if (n) {
      ret = atoi(n->text());
    }
    return ret;
  }

protected:
  std::vector<std::shared_ptr<N>> args;
  std::string op;
};

class IntrinsicOp : public Op {
public:
  IntrinsicOp(std::string op) : Op(op) {}
  virtual void accept(Visitor *a) { a->visitor(this); };
};

#define IO(O)                                                                  \
  class O##Op : public IntrinsicOp {                                           \
  public:                                                                      \
    O##Op() : IntrinsicOp(#O) {}                                               \
    virtual void accept(Visitor *a) { a->visitor(this); };                     \
  };

IO(Align)
IO(Base)
IO(Bl)
IO(Cr)
IO(Decimal)
IO(Depth)
IO(Here)
IO(Hex)
IO(I)
IO(J)
IO(Key)
IO(Leave)
IO(Quit)
IO(Space)

class UnaryOp : public Op {
public:
  UnaryOp(std::string op, std::shared_ptr<N> a) : Op(op) { args.push_back(a); }
  UnaryOp(std::string op) : Op(op) {}
  virtual void accept(Visitor *a) { a->visitor(this); };
};

#define UO(O)                                                                  \
  class O##Op : public UnaryOp {                                               \
  public:                                                                      \
    O##Op(std::shared_ptr<N> a) : UnaryOp(#O, a) {}                            \
    O##Op() : UnaryOp(#O) {}                                                   \
    virtual void accept(Visitor *a) { a->visitor(this); };                     \
  };

UO(CellPlus)
UO(Cells)
UO(Dec)
UO(Dot)
UO(Drop)
UO(Dup)
UO(Dup2)
UO(Emit)
UO(EqualsZero)
UO(FromReturn)
UO(GreaterThanSigned)
UO(GreaterThanUnsigned)
UO(GreaterThanZero)
UO(Inc)
UO(Invert)
UO(LessThanSigned)
UO(LessThanUnsigned)
UO(LessThanZero)
UO(Negate)
UO(NotEqualsZero)
UO(Push)
UO(Spaces)
UO(SToD)
UO(StoreReturn)
UO(ToReturn)
UO(UnsignedDot)

class BinaryOp : public UnaryOp {
public:
  BinaryOp(std::string op, std::shared_ptr<N> a, std::shared_ptr<N> b)
      : UnaryOp(op, a) {
    args.push_back(b);
  }
  BinaryOp(std::string op, std::shared_ptr<N> b) : UnaryOp(op) {
    args.push_back(b);
  }
  BinaryOp(std::string op) : UnaryOp(op) {}
  virtual void accept(Visitor *a) { a->visitor(this); };
};

#define BO(O)                                                                  \
  class O##Op : public BinaryOp {                                              \
  public:                                                                      \
    O##Op(std::shared_ptr<N> a, std::shared_ptr<N> b) : BinaryOp(#O, a, b) {}  \
    O##Op(std::shared_ptr<N> a) : BinaryOp(#O, a) {}                           \
    O##Op() : BinaryOp(#O) {}                                                  \
    virtual void accept(Visitor *a) { a->visitor(this); };                     \
  };

BO(ExclusiveOr)
BO(Divide)
BO(DivideModulo)
BO(LeftShift)
BO(Max)
BO(Min)
BO(Nip)
BO(Minus)
BO(Modulo)
BO(Multiply)
BO(NotEquals)
BO(Or)
BO(Over)
BO(Over2)
BO(Plus)
BO(RightShift)
BO(Swap)
BO(Swap2)
BO(Tuck)

class StoreBaseOp : public BinaryOp {
public:
  StoreBaseOp(std::shared_ptr<N> a, std::shared_ptr<N> b);
  StoreBaseOp(std::shared_ptr<N> b);
  StoreBaseOp();
  virtual void accept(Visitor *a);
  std::string name();
};

#define SO(O)                                                                  \
  class O##Op : public StoreBaseOp {                                           \
  public:                                                                      \
    O##Op(std::shared_ptr<N> a, std::shared_ptr<N> b) : StoreBaseOp(a, b) {}   \
    O##Op(std::shared_ptr<N> a) : StoreBaseOp(a) {}                            \
    O##Op() : StoreBaseOp() {}                                                 \
    virtual void accept(Visitor *a) { a->visitor(this); };                     \
  };

SO(Store)
SO(StoreC)

// TODO
// Refactor fetch to a base class

class FetchOp : public BinaryOp {
public:
  FetchOp(std::shared_ptr<N> a, std::shared_ptr<N> b);
  FetchOp(std::shared_ptr<N> b);
  FetchOp();
  virtual void accept(Visitor *a);
  std::string name();
};

class FetchCOp : public BinaryOp {
public:
  FetchCOp(std::shared_ptr<N> a, std::shared_ptr<N> b);
  FetchCOp(std::shared_ptr<N> b);
  FetchCOp();
  virtual void accept(Visitor *a);
  std::string name();
};

class TertiaryOp : public BinaryOp {
public:
  TertiaryOp(std::string op, std::shared_ptr<N> a, std::shared_ptr<N> b,
             std::shared_ptr<N> c)
      : BinaryOp(op, a, b) {
    args.push_back(c);
  }
  TertiaryOp(std::string op, std::shared_ptr<N> b, std::shared_ptr<N> c)
      : BinaryOp(op, b) {
    args.push_back(c);
  }
  TertiaryOp(std::string op, std::shared_ptr<N> c) : BinaryOp(op) {
    args.push_back(c);
  }
  TertiaryOp(std::string op) : BinaryOp(op) {}
  virtual void accept(Visitor *a) { a->visitor(this); };
};

#define TO(O)                                                                  \
  class O##Op : public TertiaryOp {                                            \
  public:                                                                      \
    O##Op(std::shared_ptr<N> a, std::shared_ptr<N> b, std::shared_ptr<N> c)    \
        : TertiaryOp(#O, a, b, c) {}                                           \
    O##Op(std::shared_ptr<N> a, std::shared_ptr<N> b)                          \
        : TertiaryOp(#O, a, b) {}                                              \
    O##Op(std::shared_ptr<N> a) : TertiaryOp(#O, a) {}                         \
    O##Op() : TertiaryOp(#O) {}                                                \
    virtual void accept(Visitor *a) { a->visitor(this); };                     \
  };

TO(MultiplyDivide)
TO(MultiplyDivideModulo)
TO(Rotate)

#endif
