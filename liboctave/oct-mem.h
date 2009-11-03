/*

Copyright (C) 2009 VZLU Prague

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#if !defined (octave_memory_h)
#define octave_memory_h 1

#include <cstddef>
#include <cstring>
#include <algorithm>

#include "oct-cmplx.h"
#include "oct-inttypes.h"

// NOTE: These functions are used to optimize stuff where performance is a
// priority. They assume that the std::complex and octave_int can be
// manipulated as plain memory, an assumption that is always true in practice
// but not theoretically guaranteed by the C++ standard. In the future, C++ may
// provide a better way to accomplish these tasks.

// Unaliased copy. This boils down to memcpy, even for octave_int and complex types.

template <class T>
inline void octave_ucopy (octave_idx_type n, const T *src, T *dest)
{ std::copy (src, src + n, dest); }

#define DEFINE_POD_UCOPY(T) \
inline void octave_ucopy (octave_idx_type n, const T *src, T *dest) \
{ std::memcpy (dest, src, n * sizeof (T)); }

DEFINE_POD_UCOPY (double)
DEFINE_POD_UCOPY (float)
DEFINE_POD_UCOPY (char)
DEFINE_POD_UCOPY (short)
DEFINE_POD_UCOPY (int)
DEFINE_POD_UCOPY (long)
DEFINE_POD_UCOPY (unsigned char)
DEFINE_POD_UCOPY (unsigned short)
DEFINE_POD_UCOPY (unsigned int)
DEFINE_POD_UCOPY (unsigned long)

DEFINE_POD_UCOPY (Complex)
DEFINE_POD_UCOPY (FloatComplex)

template <class T>
DEFINE_POD_UCOPY (octave_int<T>)

// Fill by value, with a check for zero. This boils down to memset if value is
// a POD zero.
template <class T>
inline void octave_fill (octave_idx_type n, const T& value, T *dest)
{ std::fill_n (dest, n, value); }

template <class T>
inline bool octave_fill_iszero (const T& value)
{ return value == T(); }

template <class T>
inline bool octave_fill_iszero (const std::complex<T>& value)
{ return value.real () == T() && value.imag () == T(); }

template <class T>
inline bool octave_fill_iszero (const octave_int<T>& value)
{ return value.value () == T(); }

#define DEFINE_POD_FILL(T) \
inline void octave_fill (octave_idx_type n, const T& value, T *dest) \
{ \
  if (octave_fill_iszero (value)) \
    std::memset (dest, 0, n * sizeof (T)); \
  else \
    std::fill_n (dest, n, value); \
}

DEFINE_POD_FILL (double)
DEFINE_POD_FILL (float)
DEFINE_POD_FILL (char)
DEFINE_POD_FILL (short)
DEFINE_POD_FILL (int)
DEFINE_POD_FILL (long)
DEFINE_POD_FILL (unsigned char)
DEFINE_POD_FILL (unsigned short)
DEFINE_POD_FILL (unsigned int)
DEFINE_POD_FILL (unsigned long)

DEFINE_POD_FILL (Complex)
DEFINE_POD_FILL (FloatComplex)

template <class T>
DEFINE_POD_FILL (octave_int<T>)

// Uninitialized allocation. Will not initialize memory for complex and octave_int.
// Memory allocated by octave_new should be freed by octave_delete.
template <class T>
inline T *octave_new (octave_idx_type n)
{ return new T[n]; }
template <class T>
inline void octave_delete (T *ptr)
{ delete [] ptr; }

#define DEFINE_POD_NEW_DELETE(T) \
template <> \
inline T *octave_new<T > (octave_idx_type n) \
{ return reinterpret_cast<T *> (new char[n * sizeof (T)]); } \
template <> \
inline void octave_delete<T > (T *ptr) \
{ delete [] reinterpret_cast<char *> (ptr); }

DEFINE_POD_NEW_DELETE (Complex)
DEFINE_POD_NEW_DELETE (FloatComplex)

DEFINE_POD_NEW_DELETE (octave_int8)
DEFINE_POD_NEW_DELETE (octave_int16)
DEFINE_POD_NEW_DELETE (octave_int32)
DEFINE_POD_NEW_DELETE (octave_int64)
DEFINE_POD_NEW_DELETE (octave_uint8)
DEFINE_POD_NEW_DELETE (octave_uint16)
DEFINE_POD_NEW_DELETE (octave_uint32)
DEFINE_POD_NEW_DELETE (octave_uint64)

#endif /* octave_memory_h */
