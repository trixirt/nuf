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
#include "cmdline.h"
#include "forth.h"
#include "global.h"
#include <algorithm>

int main(int argc, char *argv[]) {
  llvm::LLVMContext ctx;

  int ret = 1;
  std::shared_ptr<CPU> cpu;
  class Lang *lang = NULL;

  if (cmdline_init(argc, argv)) {
    int input_type = cmdline_input_type();
    int c = cmdline_cpu();

    switch (c) {
    default:
    case CMDLINE_CPU_X86_64:
      cpu = std::shared_ptr<CPU>(new x86_64());
    }

    switch (input_type) {
    case CMDLINE_INPUT_FORTH:
      lang = new Forth(cpu, gInputFilename);
      break;

    default:
      fprintf(stderr, "Unknown file type %s\n", gInputFilename.c_str());
      break;
    }
  }

  if (lang) {
    if (lang->lex()) {
      lang->emit(gOutputFilename);
      ret = 0;
    } else {
      fprintf(stderr, "Failed lex\n");
    }
  }

  if (lang)
    delete lang;

  cmdline_cleanup();
  return ret;
}
