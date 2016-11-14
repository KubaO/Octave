/*

Copyright (C) 2010-2016 VZLU Prague

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Octave is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#if ! defined (octave_lo_macros_h)
#define octave_lo_macros_h 1

#include "octave-config.h"

// Core macros.  Iteration is supported up to count 10.

#define OCT_ITERATE_PARAM_MACRO0(MACRO, PARAM)
#define OCT_ITERATE_PARAM_MACRO1(MACRO, PARAM)  \
  MACRO(0, PARAM)
#define OCT_ITERATE_PARAM_MACRO2(MACRO, PARAM)  \
  MACRO(0, PARAM) MACRO(1, PARAM)
#define OCT_ITERATE_PARAM_MACRO3(MACRO, PARAM)          \
  MACRO(0, PARAM) MACRO(1, PARAM) MACRO(2, PARAM)
#define OCT_ITERATE_PARAM_MACRO4(MACRO, PARAM)                          \
  MACRO(0, PARAM) MACRO(1, PARAM) MACRO(2, PARAM) MACRO(3, PARAM)
#define OCT_ITERATE_PARAM_MACRO5(MACRO, PARAM)                          \
  MACRO(0, PARAM) MACRO(1, PARAM) MACRO(2, PARAM) MACRO(3, PARAM) MACRO(4, PARAM)
#define OCT_ITERATE_PARAM_MACRO6(MACRO, PARAM)                  \
  OCT_ITERATE_PARAM_MACRO5(MACRO, PARAM) MACRO(5, PARAM)
#define OCT_ITERATE_PARAM_MACRO7(MACRO, PARAM)                          \
  OCT_ITERATE_PARAM_MACRO5(MACRO, PARAM) MACRO(5, PARAM) MACRO(6, PARAM)
#define OCT_ITERATE_PARAM_MACRO8(MACRO, PARAM)                          \
  OCT_ITERATE_PARAM_MACRO5(MACRO, PARAM) MACRO(5, PARAM) MACRO(6, PARAM) MACRO(7, PARAM)
#define OCT_ITERATE_PARAM_MACRO9(MACRO, PARAM)                  \
  OCT_ITERATE_PARAM_MACRO8(MACRO, PARAM) MACRO(8, PARAM)
#define OCT_ITERATE_PARAM_MACRO10(MACRO, PARAM)                         \
  OCT_ITERATE_PARAM_MACRO8(MACRO, PARAM) MACRO(8, PARAM) MACRO(9, PARAM)

// expands to MACRO(0, PARAM) MACRO(1, PARAM) ... MACRO(NUM-1, PARAM)
#define OCT_ITERATE_PARAM_MACRO(MACRO, PARAM, NUM)      \
  OCT_ITERATE_PARAM_MACRO##NUM(MACRO,PARAM)

#define OCT_IF_PARAM0(MACRO, PARAM)
#define OCT_IF_PARAM1(MACRO, PARAM) MACRO(PARAM)
#define OCT_IF_PARAM2(MACRO, PARAM) MACRO(PARAM)
#define OCT_IF_PARAM3(MACRO, PARAM) MACRO(PARAM)
#define OCT_IF_PARAM4(MACRO, PARAM) MACRO(PARAM)
#define OCT_IF_PARAM5(MACRO, PARAM) MACRO(PARAM)
#define OCT_IF_PARAM6(MACRO, PARAM) MACRO(PARAM)
#define OCT_IF_PARAM7(MACRO, PARAM) MACRO(PARAM)
#define OCT_IF_PARAM8(MACRO, PARAM) MACRO(PARAM)
#define OCT_IF_PARAM9(MACRO, PARAM) MACRO(PARAM)

// expands to MACRO(PARAM) if NUM > 0
#define OCT_IF_PARAM(NUM, MACRO, PARAM) OCT_IF_PARAM##NUM(MACRO, PARAM)

// concatenation and reverse concat
#define OCT_CONCAT2(ARG1, ARG2) ARG1 ## ARG2
#define OCT_CONCAT2_REVERSE(ARG1, ARG2) ARG2 ## ARG1

#define OCT_IF_HELPER(X) X

// expands to X if NUM > 0
#define OCT_IF(NUM, X) OCT_IF_PARAM(NUM, OCT_IF_HELPER, X)

#define OCT_ITERATE_MACRO_HELPER(NUM, MACRO) MACRO(NUM)

// expands to MACRO(0) ... MACRO(NUM-1)
#define OCT_ITERATE_MACRO(MACRO, NUM) OCT_ITERATE_PARAM_MACRO(OCT_ITERATE_MACRO_HELPER, MACRO, NUM)

#define OCT_MAKE_LIST_HELPER1(ARG) ARG,

// expands to MACRO(0), MACRO(1), ..., MACRO(NUM-1)
#define OCT_MAKE_LIST_HELPER(NUM, MACRO) OCT_IF_PARAM(NUM,OCT_MAKE_LIST_HELPER1,) MACRO(NUM)

#define OCT_MAKE_LIST(MACRO, NUM) OCT_ITERATE_PARAM_MACRO(OCT_MAKE_LIST_HELPER, MACRO, NUM)

#define OCT_MAKE_DECL_LIST_HELPER(NUM, PREFIX)                          \
  OCT_IF_PARAM(NUM,OCT_MAKE_LIST_HELPER1,) OCT_CONCAT2(PREFIX, NUM)

// expands to TYPE PREFIX0, TYPE PREFIX1, ..., TYPE PREFIX ## (NUM-1)
#define OCT_MAKE_DECL_LIST(TYPE, PREFIX, NUM)                           \
  OCT_ITERATE_PARAM_MACRO(OCT_MAKE_DECL_LIST_HELPER, TYPE PREFIX, NUM)

// expands to PREFIX0, PREFIX1, ..., PREFIX ## (NUM-1)
#define OCT_MAKE_ARG_LIST(PREFIX, NUM)                                  \
  OCT_ITERATE_PARAM_MACRO(OCT_MAKE_DECL_LIST_HELPER, PREFIX, NUM)

#endif

