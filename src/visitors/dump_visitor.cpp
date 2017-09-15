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
#include "visitors/dump_visitor.h"
#include "ast.h"
#include "forth.h"
#include <stdio.h>

DumpVisitor::DumpVisitor() {}
void DumpVisitor::visitor(Comment *a) {}
void DumpVisitor::visitor(Code *a) {}
void DumpVisitor::visitor(Number *a) {}
void DumpVisitor::visitor(Delimiter *a) {}
void DumpVisitor::visitor(Op *a) {}
void DumpVisitor::visitor(PushOp *a) {}

void DumpVisitor::visitor(IfStatement *a) {}

void DumpVisitor::visitor(BaseOp *a) {}

void DumpVisitor::visitor(Program *a) {}

void DumpVisitor::visitor(SimpleStatement *a) {}

void DumpVisitor::visitor(StatementList *a) {}

void DumpVisitor::visitor(Literal *a) {}

void DumpVisitor::visitor(HexOp *a) {}
void DumpVisitor::visitor(DecimalOp *a) {}

void DumpVisitor::visitor(DepthOp *a) {}

void DumpVisitor::visitor(DecOp *a) {}
void DumpVisitor::visitor(DivideOp *a) {}
void DumpVisitor::visitor(DivideModuloOp *a) {}

void DumpVisitor::visitor(DotOp *a) {}
void DumpVisitor::visitor(UnsignedDotOp *a) {}

void DumpVisitor::visitor(SpaceOp *a) {}

void DumpVisitor::visitor(SpacesOp *a) {}

void DumpVisitor::visitor(DropOp *a) {}

void DumpVisitor::visitor(DupOp *a) {}
void DumpVisitor::visitor(EqualsZeroOp *a) {}
void DumpVisitor::visitor(ExclusiveOrOp *a) {}
void DumpVisitor::visitor(IncOp *a) {}
void DumpVisitor::visitor(InvertOp *a) {}
void DumpVisitor::visitor(LeftShiftOp *a) {}
void DumpVisitor::visitor(MinusOp *a) {}
void DumpVisitor::visitor(ModuloOp *a) {}
void DumpVisitor::visitor(MultiplyOp *a) {}

void DumpVisitor::visitor(LessThanZeroOp *a) {}

void DumpVisitor::visitor(LessThanSignedOp *a) {}

void DumpVisitor::visitor(LessThanUnsignedOp *a) {}

void DumpVisitor::visitor(GreaterThanSignedOp *a) {}

void DumpVisitor::visitor(GreaterThanUnsignedOp *a) {}

void DumpVisitor::visitor(MultiplyDivideOp *a) {}
void DumpVisitor::visitor(MultiplyDivideModuloOp *a) {}
void DumpVisitor::visitor(NegateOp *a) {}
void DumpVisitor::visitor(OrOp *a) {}
void DumpVisitor::visitor(OverOp *a) {}
void DumpVisitor::visitor(PlusOp *a) {}
void DumpVisitor::visitor(RightShiftOp *a) {}
void DumpVisitor::visitor(RotateOp *a) {}
void DumpVisitor::visitor(SwapOp *a) {}

void DumpVisitor::visitor(StoreReturnOp *a) {}
void DumpVisitor::visitor(ToReturnOp *a) {}
void DumpVisitor::visitor(FromReturnOp *a) {}
void DumpVisitor::visitor(StoreOp *a) {}
void DumpVisitor::visitor(FetchOp *a) {}
void DumpVisitor::visitor(Constant *a) {}
void DumpVisitor::visitor(Variable *a) {}
void DumpVisitor::visitor(N *a) {}
