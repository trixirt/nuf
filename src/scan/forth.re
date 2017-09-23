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
#include "scan/uscanner.h"
#include "parse/forth_p.h"

#define YYCTYPE   unsigned char
#define YYCURSOR  m_p
#define YYLIMIT   m_pend
#define YYMARKER  m_pmarker
#define YYFILL(n) if (!fill(n)) return 0

std::shared_ptr<N> forth_scanner::scan(int *tokenid) {

    m_pstart = m_p;
    fprintf(stderr, "m_p %p\n", m_p);
standard:

/*!re2c

eof = [\377];
any = [\000-\376];
any_c = [\000-\011,\013-\376];
ws  = [ \r\t\f];
nl  = [\n];
alpha = [a-zA-Z];
alphanum = [a-zA-Z0-9];
alphanumspec = [>!/*?_@a-zA-Z0-9];
/* f12 3.4.1.3 Text interpretor input number conversion */
binary_digit = [0-1];
binary_number = "%"[-]?(binary_digit)(binary_digit)*;
decimal_digit = [0-9];
decimal_number = [-]?(decimal_digit)(decimal_digit)*;
hexidecimal_digit = ([0-9] | [a-f] | [A-F]);
hexidecimal_number = "$"[-]?(hexidecimal_digit)(hexidecimal_digit)*;
base_digit = ([0-9,a-z,A-Z]);
base_number = [-]?(base_digit)(base_digit)*;
printable_char = [\040-\376];
cnum = "'" printable_char "'";
// base_number matches too much
// So move 'number' def from this :
// number = (base_number) | (decimal_number) | (hexidecimal_number) | (binary_number);
// To this :
// number = (decimal_number) | (hexidecimal_number) | (binary_number);
number = (decimal_number);
symbol = (alphanumspec)(alphanumspec)*;
literal = (symbol) | (number);

positive_decimal = [+]?[1-9][0-9]*;
negative_decimal = [-]positive_decimal;
decimal = [0] | positive_decimal | negative_decimal;
word = alpha alphanum*;
*/

/*
   keyword taken care of by compiler's dictionary
   ABS
 */

/*!re2c
    '('            { goto comment;                          }
    '\\'           { goto comment_line;                     }
    '."'           { goto string;                            }
    eof            { *tokenid = 0;                           return empty();   }
    nl             { *tokenid = FORTH_LF;      inc_lineno(); return comment();   }
    '0<'           { *tokenid = FORTH_LTZERO;                return empty();   }
    '0>'           { *tokenid = FORTH_GTZERO;                return empty();   }
    '0<>'          { *tokenid = FORTH_NEQZERO;               return empty();   }	
    '0='           { *tokenid = FORTH_EQZERO;                return empty();   }
    '1+'           { *tokenid = FORTH_INC;                   return empty();   }
    '1-'           { *tokenid = FORTH_DEC;                   return empty();   }
    '2DUP'         { *tokenid = FORTH_DUP_TWO;               return empty();   }
    '2OVER'        { *tokenid = FORTH_OVER_TWO;              return empty();   }
    '2SWAP'        { *tokenid = FORTH_SWAP_TWO;              return empty();   }
    '2VARIABLE'    { *tokenid = FORTH_VARIABLE_TWO;          return empty();   }
    '<'            { *tokenid = FORTH_LTSIGNED;              return empty();   }
    '>'            { *tokenid = FORTH_GTSIGNED;              return empty();   }	
    '<>'           { *tokenid = FORTH_NEQ;                   return empty();   }
    '+'            { *tokenid = FORTH_PLUS;                  return empty();   }
    '+LOOP'        { *tokenid = FORTH_LOOP_PLUS;             return empty();   }
    '*'            { *tokenid = FORTH_MUL;                   return empty();   }
    '*/'           { *tokenid = FORTH_MULDIV;                return empty();   }
    '*/MOD'        { *tokenid = FORTH_MULDIVMOD;             return empty();   }
    '-'            { *tokenid = FORTH_MINUS;                 return empty();   }
    '/MOD'         { *tokenid = FORTH_DIVMOD;                return empty();   }	
    '/'            { *tokenid = FORTH_SL;                    return empty();   }
    '!'            { *tokenid = FORTH_EXL;                   return empty();   }
    '@'            { *tokenid = FORTH_AT;                    return empty();   }
    '.'            { *tokenid = FORTH_DOT;                   return empty();   }
    ':'            { *tokenid = FORTH_COL;                   return empty();   }
    ';'            { *tokenid = FORTH_SCO;                   return empty();   }
    '"'            { *tokenid = FORTH_DQU;                   return empty();   }
    '>R'           { *tokenid = FORTH_TOR;                   return empty();   }
    'AGAIN'        { *tokenid = FORTH_AGAIN;                 return empty();   }
    'ALIGN'        { *tokenid = FORTH_ALIGN;                 return empty();   }
    'BASE'         { *tokenid = FORTH_BASE;                  return empty();   }
    'BEGIN'        { *tokenid = FORTH_BEGIN;                 return empty();   }
    'BL'           { *tokenid = FORTH_BL;                    return empty();   }
    'C!'           { *tokenid = FORTH_STORE_C;               return empty();   }
    'C@'           { *tokenid = FORTH_FETCH_C;               return empty();   }
    'CHAR+'        { *tokenid = FORTH_CELL_PLUS;             return empty();   }
    'CHARS'        { *tokenid = FORTH_CELLS;                 return empty();   }
    'CELL+'        { *tokenid = FORTH_CELL_PLUS;             return empty();   }
    'CELLS'        { *tokenid = FORTH_CELLS;                 return empty();   }
    'CONSTANT'     { *tokenid = FORTH_CONSTANT;              return empty();   }
    'CR'           { *tokenid = FORTH_CR;                    return empty();   }
    'DECIMAL'      { *tokenid = FORTH_DECIMAL;               return empty();   }
    'DEPTH'        { *tokenid = FORTH_DEPTH;                 return empty();   }
    'DO'           { *tokenid = FORTH_DO;                    return empty();   }    
    'DROP'         { *tokenid = FORTH_DROP;                  return empty();   }
    'DUP'          { *tokenid = FORTH_DUP;                   return empty();   }
    'ELSE'         { *tokenid = FORTH_ELSE;                  return empty();   }
    'EMIT'         { *tokenid = FORTH_EMIT;                  return empty();   }
    'EXIT'         { printf("mmmm\n"); *tokenid = FORTH_EXIT;                  return empty();   }
    'FALSE'        { *tokenid = FORTH_FALSE;                 return empty();   }    
    'HEX'          { *tokenid = FORTH_HEX;                   return empty();   }
    'HERE'         { *tokenid = FORTH_HERE;                  return empty();   }
    'I'            { *tokenid = FORTH_I;                     return empty();   }
    'IF'           { *tokenid = FORTH_IF;                    return empty();   }
    'INVERT'       { *tokenid = FORTH_INVERT;                return empty();   }
    'J'            { *tokenid = FORTH_J;                     return empty();   }
    'KEY'          { *tokenid = FORTH_KEY;                   return empty();   }
    'LEAVE'        { *tokenid = FORTH_LEAVE;                 return empty();   }
    'LOOP'         { *tokenid = FORTH_LOOP;                  return empty();   }
    'LSHIFT'       { *tokenid = FORTH_LSHIFT;                return empty();   }
    'M*'           { *tokenid = FORTH_MUL;                   return empty();   }
    'MAX'          { *tokenid = FORTH_MAX;                   return empty();   }
    'MIN'          { *tokenid = FORTH_MIN;                   return empty();   }
    'MOD'          { *tokenid = FORTH_MOD;                   return empty();   }
    'NEGATE'       { *tokenid = FORTH_NEGATE;                return empty();   }
    'NIP'          { *tokenid = FORTH_NIP;                   return empty();   }
    'OR'           { *tokenid = FORTH_OR;                    return empty();   }
    'OVER'         { *tokenid = FORTH_OVER;                  return empty();   }
    'QUIT'         { *tokenid = FORTH_QUIT;                  return empty();   }
    'ROT'          { *tokenid = FORTH_ROT;                   return empty();   }
    'REPEAT'       { *tokenid = FORTH_REPEAT;                return empty();   }
    'RSHIFT'       { *tokenid = FORTH_RSHIFT;                return empty();   }
    'R>'           { *tokenid = FORTH_FROMR;                 return empty();   }
    'R@'           { *tokenid = FORTH_STORER;                return empty();   }
    'S>D'          { *tokenid = FORTH_STOD;                  return empty();   }
    'SM/REM'       { *tokenid = FORTH_DIVMOD;                return empty();   }
    'SPACE'        { *tokenid = FORTH_SPACE;                 return empty();   }
    'SPACES'       { *tokenid = FORTH_SPACES;                return empty();   }
    'SWAP'         { *tokenid = FORTH_SWAP;                  return empty();   }
    'THEN'         { *tokenid = FORTH_THEN;                  return empty();   }
    'TRUE'         { *tokenid = FORTH_TRUE;                  return empty();   }
    'TUCK'         { *tokenid = FORTH_TUCK;                  return empty();   }
    'WHILE'        { *tokenid = FORTH_WHILE;                 return empty();   }        
    'U.'           { *tokenid = FORTH_UDOT;                  return empty();   }
    'U<'           { *tokenid = FORTH_LTUNSIGNED;            return empty();   }
    'U>'           { *tokenid = FORTH_GTUNSIGNED;            return empty();   }
    'UM*'          { *tokenid = FORTH_MUL;                   return empty();   }
    'UM/REM'       { *tokenid = FORTH_DIVMOD;                return empty();   }
    'UNTIL'        { *tokenid = FORTH_UNTIL;                 return empty();   }	
    'VARIABLE'     { *tokenid = FORTH_VARIABLE;              return empty();   }
    'XOR'          { *tokenid = FORTH_XOR;                   return empty();   }
    number         { *tokenid = FORTH_NUMBER;                return number();  }
    symbol         { *tokenid = FORTH_SYMBOL;                return symbol();  }
    any            { *tokenid = FORTH_LEX_ERROR;             return comment(); }	
*/

string:
/*!re2c
	'"'        { *tokenid = FORTH_STRING; return string(); }
	eof        { *tokenid = 0;                               return string(); }
        any        { goto string;                                                 }
*/

comment:
/*!re2c
	')'        { *tokenid = FORTH_COMMENT; goto standard;                             }
	nl         { inc_lineno(); goto comment;                               }
	eof        { *tokenid = 0;                           return comment(); }
        any        { goto comment;                                             }
*/

comment_line:
/*!re2c
	nl         { *tokenid = FORTH_COMMENT; inc_lineno(); return comment();       }
	eof        { *tokenid = 0;                           return comment(); }
        any        { goto comment_line;                                        }
*/

return 0;
}


