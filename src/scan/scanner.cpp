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
#include "scan/scanner.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define PRINTABLE(c)                                                           \
  (((c) >= (__typeof__(c))0x20 && (c) < (__typeof__(c))127) ? (c) : '.')

scanner::scanner(std::string filename, const char *pre_buffer,
                 size_t pre_buffer_size) {
  m_file_size = 0;
  m_mmap = (void *)-1;
  m_p = m_plinestart = m_pmarker = NULL;
  m_pend = NULL;
  m_eof = 0xff;
  m_false_buffer = NULL;
  m_buffer = nullptr;
  m_pre_buffer_size = 0;
  m_in_pre = false;

  if (!filename.empty()) {
    struct stat buf;
    if (0 == stat(filename.c_str(), &buf)) {
      m_file_size = buf.st_size;
      if (m_file_size > 0) {
        FILE *fp;
        m_filename = filename;
        fp = fopen(filename.c_str(), "rt");
        if (fp) {
          m_buffer = (char *)malloc(m_file_size + pre_buffer_size);
          if (m_buffer) {
            if (pre_buffer_size) {
              m_pre_buffer_size = pre_buffer_size;
              memcpy(m_buffer, pre_buffer, pre_buffer_size);
              m_in_pre = true;
            }
            fread(&m_buffer[m_pre_buffer_size], 1, m_file_size, fp);
            m_pend = m_buffer + m_pre_buffer_size + m_file_size;
            m_pstart = m_p = m_plinestart = m_pmarker = m_buffer;
          }
          fclose(fp);
        }
      }
    }
  }

  m_lineno = 1; /* base 1 */
}

scanner::~scanner() {

  if (m_buffer != nullptr) {
    free(m_buffer);
    m_buffer = nullptr;
  }
  m_pre_buffer_size = 0;

  if (m_false_buffer != NULL) {
    free(m_false_buffer);
    m_false_buffer = NULL;
  }
}

/*
 * n : number of tokens needed
 *
 * returns
 * true : success
 * false : fail
 *
 * lexer/parser needs to handle the eof token
 */
bool scanner::fill(off_t n) {
  bool ret = true;

  // fprintf(stdout, "%s %u %p %p %p\n", __func__, n, m_buffer, m_p, m_pend);

  if (n > 0) {
#if 0
      /* check if we are in the prebuffer */
      if (m_in_pre) {
	  if (m_buffer - m_p >= m_pre_buffer_size) {
	      m_lineno = 1; /* base 1 */
	      m_in_pre = false;
	  }
      }
#endif

    /* Ran out of real input, now we fake it.. */
    if (m_p + n >= m_pend) {
      /* how much is left for the real file */
      unsigned int l = m_pend - m_pstart;
      /* how large we have to make the fake buffer */
      unsigned int s = n + l;
      char *old_false_buffer = m_false_buffer;
      m_false_buffer = (char *)malloc(s);
      if (m_false_buffer) {
        /* where we are in the current stream */
        size_t cur = m_p - m_pstart;
        /* everyone starts off as an eof token */
        memset(m_false_buffer, m_eof, s);
        /* copy the remaining part of the real buffer to start of fake */
        memcpy(m_false_buffer, m_pstart, l);
        /* adjust the start location */
        m_pstart = &m_false_buffer[0];
        /* adjust the current location */
        m_p = m_pstart + cur;
        /* adjust the end location */
        m_pend = m_pstart + n;
      } else {
        ret = false;
      }
      if (old_false_buffer)
        free(old_false_buffer);
    }
  } else {
    ret = false;
  }
  return ret;
}

bool scanner::atoi(long *out, unsigned char *start, unsigned char *end,
                   int base) {
  bool ret = false;
  size_t n = end - start;
  char *str = (char *)malloc(n + 1);
  if (str) {
    char *endptr;
    long o;
    memcpy(str, start, n);
    str[n] = '\0';
    o = strtol(str, &endptr, base);
    if ((*str != '\0') && (*endptr == '\0')) {
      *out = o;
      ret = true;
    }
    free(str);
  }
  return ret;
}

bool scanner::atou(unsigned long *out, unsigned char *start, unsigned char *end,
                   int base) {
  bool ret = false;
  size_t n = end - start;
  char *str = (char *)malloc(n + 1);
  if (str) {
    char *endptr;
    unsigned long o;
    memcpy(str, start, n);
    str[n] = '\0';
    o = strtoul(str, &endptr, base);
    if ((*str != '\0') && (*endptr == '\0')) {
      *out = o;
      ret = true;
    }
    free(str);
  }
  return ret;
}

void scanner::inc_lineno() {
  m_lineno++;
  m_plinestart = m_p;
}

char *scanner::loc_string() {
  static char loc[64];
  unsigned int char_loc = position();
  sprintf(loc, "%lu:%u", m_lineno, char_loc);
  return loc;
}

bool scanner::str(char **out, char *start, char *end) {
  bool ret = false;
  size_t n = end - start;
  char *s = static_cast<char *>(malloc(n + 1));
  if (s) {
    memcpy(s, start, n);
    s[n] = '\0';
    *out = s;
    ret = true;
  }
  return ret;
}

void scanner::err() {
  fprintf(stderr, "Scanning error at %s : unhandled character: '%c - %d'\n",
          loc_string(), PRINTABLE(*m_p), *m_p);
}

char *scanner::strchr(int c) {
  char *r = ::strchr(m_p, c);
  return r;
}

char *scanner::line() {
  char *s = m_plinestart;
  char *e = strchr('\n');
  if (e == NULL) {
    e = m_pend;
  }
  char *r = NULL;
  str(&r, s, e);
  return r;
}
