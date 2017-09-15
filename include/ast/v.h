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
#ifndef NUF_VISITORS_VISITOR_H
#define NUF_VISITORS_VISITOR_H

class N;
class Code;
class Number;
class Literal;
class Op;
class BaseOp;
class Comment;
class Constant;
class DecOp;
class DecimalOp;
class Delimiter;
class DepthOp;
class DivideOp;
class DivideModuloOp;
class DotOp;
class DropOp;
class DupOp;
class EqualsZeroOp;
class ExclusiveOrOp;
class FetchOp;
class FromReturnOp;
class HexOp;
class IncOp;
class InvertOp;
class LeftShiftOp;
class LessThanZeroOp;
class LessThanSignedOp;
class LessThanUnsignedOp;
class GreaterThanSignedOp;
class GreaterThanUnsignedOp;
class MinusOp;
class MultiplyOp;
class MultiplyDivideOp;
class MultiplyDivideModuloOp;
class ModuloOp;
class NegateOp;
class OrOp;
class OverOp;
class PlusOp;
class Program;
class PushOp;
class RightShiftOp;
class RotateOp;
class SimpleStatement;
class SpaceOp;
class SpacesOp;
class StatementList;
class StoreOp;
class StoreReturnOp;
class SwapOp;
class ToReturnOp;
class UnsignedDotOp;
class Variable;

class Visitor {
public:
  Visitor(){};
  virtual ~Visitor(){};
  virtual void visitor(Code *a) = 0;
  virtual void visitor(Comment *a) = 0;
  virtual void visitor(N *a) = 0;
  virtual void visitor(Op *a) = 0;
  virtual void visitor(BaseOp *a) = 0;
  virtual void visitor(Constant *a) = 0;
  virtual void visitor(ExclusiveOrOp *a) = 0;
  virtual void visitor(DecOp *a) = 0;
  virtual void visitor(DecimalOp *a) = 0;
  virtual void visitor(Delimiter *a) = 0;
  virtual void visitor(DepthOp *a) = 0;
  virtual void visitor(DivideOp *a) = 0;
  virtual void visitor(DivideModuloOp *a) = 0;
  virtual void visitor(DotOp *a) = 0;
  virtual void visitor(DropOp *a) = 0;
  virtual void visitor(DupOp *a) = 0;
  virtual void visitor(EqualsZeroOp *a) = 0;
  virtual void visitor(FetchOp *a) = 0;
  virtual void visitor(FromReturnOp *a) = 0;
  virtual void visitor(HexOp *a) = 0;
  virtual void visitor(IncOp *a) = 0;
  virtual void visitor(InvertOp *a) = 0;
  virtual void visitor(LessThanZeroOp *a) = 0;
  virtual void visitor(LessThanSignedOp *a) = 0;
  virtual void visitor(LessThanUnsignedOp *a) = 0;
  virtual void visitor(GreaterThanSignedOp *a) = 0;
  virtual void visitor(GreaterThanUnsignedOp *a) = 0;
  virtual void visitor(LeftShiftOp *a) = 0;
  virtual void visitor(Literal *a) = 0;
  virtual void visitor(MinusOp *a) = 0;
  virtual void visitor(ModuloOp *a) = 0;
  virtual void visitor(MultiplyOp *a) = 0;
  virtual void visitor(MultiplyDivideOp *a) = 0;
  virtual void visitor(MultiplyDivideModuloOp *a) = 0;
  virtual void visitor(NegateOp *a) = 0;
  virtual void visitor(Number *a) = 0;
  virtual void visitor(OrOp *a) = 0;
  virtual void visitor(OverOp *a) = 0;
  virtual void visitor(PlusOp *a) = 0;
  virtual void visitor(Program *a) = 0;
  virtual void visitor(PushOp *a) = 0;
  virtual void visitor(RightShiftOp *a) = 0;
  virtual void visitor(RotateOp *a) = 0;
  virtual void visitor(SimpleStatement *a) = 0;
  virtual void visitor(SpaceOp *a) = 0;
  virtual void visitor(SpacesOp *a) = 0;
  virtual void visitor(StatementList *a) = 0;
  virtual void visitor(StoreOp *a) = 0;
  virtual void visitor(StoreReturnOp *a) = 0;
  virtual void visitor(SwapOp *a) = 0;
  virtual void visitor(ToReturnOp *a) = 0;
  virtual void visitor(UnsignedDotOp *a) = 0;
  virtual void visitor(Variable *a) = 0;
};

#endif
