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
#include "global.h"
#include "llvm/Support/CommandLine.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static char *s_program = NULL;

mode_t g_file_mode = 0;

std::string gInputFilename;
std::string gOutputFilename;

void cmdline_usage() { fprintf(stderr, "Usage : %s <filename>\n", s_program); }

llvm::cl::opt<std::string, true>
    OutputFilename("o", llvm::cl::desc("Specify output filename"),
                   llvm::cl::value_desc("filename"),
                   llvm::cl::location(gOutputFilename));
llvm::cl::opt<std::string, true>
    InputFilename(llvm::cl::Positional, llvm::cl::desc("<input file>"),
                  llvm::cl::Required, llvm::cl::location(gInputFilename));

void cmdline_cleanup() {
  g_file_mode = 0;
  gInputFilename.clear();
  gOutputFilename.clear();
}

bool cmdline_init(int argc, char *argv[]) {
  bool ret = false;
  struct stat buf;

  s_program = argv[0];
  llvm::cl::ParseCommandLineOptions(argc, argv);
  const char *t = gInputFilename.c_str();
  if (0 == stat(t, &buf)) {
    g_file_mode = buf.st_mode;
    if (gOutputFilename.empty()) {
      gOutputFilename = gInputFilename;
      gOutputFilename.erase(gOutputFilename.find_last_of("."));
      gOutputFilename += ".ll";
    }
    ret = true;
  } else {
    perror("Error with input file : ");
  }
  return ret;
}

int cmdline_input_type() {
  int ret = CMDLINE_INPUT_UNKNOWN;
  const char *ext = strrchr(gInputFilename.c_str(), '.');
  if (ext) {
    if (ext[1] != '\0') {
      ext++;
      if (0 == strncmp(ext, "4th", 3))
        return CMDLINE_INPUT_FORTH;
      else if (0 == strncmp(ext, "s", 1))
        return CMDLINE_INPUT_AS;
      else if (0 == strncmp(ext, "c", 1))
        return CMDLINE_INPUT_C;
      else if (0 == strncmp(ext, "f", 1))
        return CMDLINE_INPUT_FORTRAN;
    }
  }
  return ret;
}

int cmdline_cpu() {
  int ret = CMDLINE_CPU_X86_64;
  return ret;
}
