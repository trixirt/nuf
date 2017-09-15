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
#ifndef NUF_AST_SOURCE_LOCATION_H
#define NUF_AST_SOURCE_LOCATION_H

#include <stdint.h>

/*
 * SL
 * Source Location
 */
class SourceLocation {
public:
  SourceLocation() {
    clear();
    _intrinsic = true;
  }
  SourceLocation(uint64_t l, uint64_t p) {
    clear();
    if (l > 0)
      _lineno = l;
    if (p > 0)
      _position = p;
  }
  void clear() {
    _lineno = 1;
    _position = 1;
    _intrinsic = false;
  }
  SourceLocation &operator=(SourceLocation &rhs) {
    _lineno = rhs._lineno;
    _position = rhs._position;
    _intrinsic = rhs._intrinsic;
    return *this;
  }
  SourceLocation &operator+(uint64_t a) {
    if (!intrinsic())
      _position += a;
    return *this;
  }
  SourceLocation &operator*(uint64_t a) {
    if (!intrinsic())
      _lineno += a;
    return *this;
  }
  SourceLocation &operator-(uint64_t a) {
    if (!intrinsic()) {
      if (_position > a)
        _position -= a;
      else
        _position = 1;
    }
    return *this;
  }
  SourceLocation &operator/(uint64_t a) {
    if (!intrinsic()) {
      if (_lineno > a)
        _lineno -= a;
      else
        _lineno = 1;
    }
    return *this;
  }
  uint64_t lineno() { return _lineno; }
  uint64_t position() { return _position; }
  bool intrinsic() { return _intrinsic; }

private:
  uint64_t _lineno;
  uint64_t _position;
  bool _intrinsic;
};

#endif
