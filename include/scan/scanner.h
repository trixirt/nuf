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
#ifndef NUF_SCANNER_H
#define NUF_SCANNER_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

class scanner {
public:
  scanner(std::string filename, const char *pre_buffer = NULL, size_t n = 0);
  ~scanner();
  bool atoi(long *out, unsigned char *start, unsigned char *end, int base = 10);
  bool atou(unsigned long *out, unsigned char *start, unsigned char *end,
            int base = 10);
  void inc_lineno();
  void err();
  bool fill(off_t n);
  char *strchr(int c);
  char *loc_string(); /* user must not free return */
  bool str(char **out, char *start, char *end);
  uint64_t lineno() { return m_lineno; }
  uint64_t position() { return 1 + (m_p - m_plinestart); }
  char *line(); /* user must 'free' return */

protected:
  void *m_mmap;
  std::string m_filename;
  char *m_p;
  char *m_pend;
  char *m_pmarker;
  char *m_pstart;
  unsigned char m_eof;
  char *m_false_buffer;
  char *m_buffer;
  size_t m_pre_buffer_size;
  bool m_in_pre;

private:
  uint64_t m_lineno;
  char *m_plinestart;
  off_t m_file_size;
  bool m_comment;
};

#endif /* NUF_SCANNER_H */
