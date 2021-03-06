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
#ifndef NUF_VISITORS_DUMP_H
#define NUF_VISITORS_DUMP_H

#include "visitor.h"

class DumpVisitor : public Visitor {
public:
  DumpVisitor();
  virtual void visitor(Code *a);
  virtual void visitor(Comment *a);
  virtual void visitor(N *a);
  virtual void visitor(Op *a);
  virtual void visitor(BaseOp *a);
  virtual void visitor(Constant *a);
  virtual void visitor(ExclusiveOrOp *a);
  virtual void visitor(DecOp *a);
  virtual void visitor(DecimalOp *a);
  virtual void visitor(Delimiter *a);
  virtual void visitor(DepthOp *a);
  virtual void visitor(DivideOp *a);
  virtual void visitor(DivideModuloOp *a);
  virtual void visitor(DotOp *a);
  virtual void visitor(DropOp *a);
  virtual void visitor(DupOp *a);
  virtual void visitor(EqualsZeroOp *a);
  virtual void visitor(FetchOp *a);
  virtual void visitor(FromReturnOp *a);
  virtual void visitor(HexOp *a);
  virtual void visitor(IfStatement *a);
  virtual void visitor(IncOp *a);
  virtual void visitor(InvertOp *a);
  virtual void visitor(LessThanZeroOp *a);
  virtual void visitor(LessThanSignedOp *a);
  virtual void visitor(LessThanUnsignedOp *a);
  virtual void visitor(GreaterThanSignedOp *a);
  virtual void visitor(GreaterThanUnsignedOp *a);
  virtual void visitor(LeftShiftOp *a);
  virtual void visitor(Literal *a);
  virtual void visitor(MinusOp *a);
  virtual void visitor(ModuloOp *a);
  virtual void visitor(MultiplyOp *a);
  virtual void visitor(MultiplyDivideOp *a);
  virtual void visitor(MultiplyDivideModuloOp *a);
  virtual void visitor(NegateOp *a);
  virtual void visitor(Number *a);
  virtual void visitor(OrOp *a);
  virtual void visitor(OverOp *a);
  virtual void visitor(PlusOp *a);
  virtual void visitor(Program *a);
  virtual void visitor(PushOp *a);
  virtual void visitor(RightShiftOp *a);
  virtual void visitor(RotateOp *a);
  virtual void visitor(SimpleStatement *a);
  virtual void visitor(SpaceOp *a);
  virtual void visitor(SpacesOp *a);
  virtual void visitor(StatementList *a);
  virtual void visitor(StoreOp *a);
  virtual void visitor(StoreReturnOp *a);
  virtual void visitor(SwapOp *a);
  virtual void visitor(ToReturnOp *a);
  virtual void visitor(UnsignedDotOp *a);
  virtual void visitor(Variable *a);
};

#endif
