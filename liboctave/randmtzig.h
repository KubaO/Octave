/* 
   A C-program for MT19937, with initialization improved 2002/2/10.
   Coded by Takuji Nishimura and Makoto Matsumoto.
   This is a faster version by taking Shawn Cokus's optimization,
   Matthe Bellew's simplification, Isaku Wada's real version.
   David Bateman added normal and exponential distributions following
   Marsaglia and Tang's Ziggurat algorithm.

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   Copyright (C) 2004, David Bateman
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER 
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef _RANDMTZIG_H
#define _RANDMTZIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "oct-types.h"

#define MT_N 624

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#else
#if SIZEOF_INT == 4
typedef unsigned int uint32_t;
#elif SIZEOF_LONG == 4
typedef unsigned long uint32_t;
#else
#error "No 4 byte integer type found!"
#endif

#if SIZEOF_LONG == 8
typedef unsigned long uint64_t;
#else
#if SIZEOF_LONG_LONG == 8
typedef unsigned long long uint64_t;
#endif
#endif
#endif

/* === Mersenne Twister === */
extern void oct_init_by_int (uint32_t s);
extern void oct_init_by_array (uint32_t init_key[], int key_length);
extern void oct_init_by_entropy (void);
extern void oct_set_state (uint32_t save[]);
extern void oct_get_state (uint32_t save[]);

/* === Array generators === */
extern double oct_randu (void);
extern double oct_randn (void);
extern double oct_rande (void);

/* === Array generators === */
extern void oct_fill_randu (octave_idx_type n, double *p);
extern void oct_fill_randn (octave_idx_type n, double *p);
extern void oct_fill_rande (octave_idx_type n, double *p);

#ifdef  __cplusplus
}
#endif
#endif

/*
;;; Local Variables: ***
;;; mode: C ***
;;; End: ***
*/

