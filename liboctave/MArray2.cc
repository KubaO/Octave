/*

Copyright (C) 1996, 1997 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#if defined (USE_PRAGMA_INTERFACE_IMPLEMENTATION)
#pragma implementation
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MArray2.h"
#include "lo-error.h"

#include "MArray-defs.h"

// Two dimensional array with math ops.

// Element by element MArray2 by scalar ops.

template <class T>
MArray2<T>&
operator += (MArray2<T>& a, const T& s)
{
  DO_VS_OP2 (+=)
  return a;
}

template <class T>
MArray2<T>&
operator -= (MArray2<T>& a, const T& s)
{
  DO_VS_OP2 (-=)
  return a;
}

// Element by element MArray2 by MArray2 ops.

template <class T>
MArray2<T>&
operator += (MArray2<T>& a, const MArray2<T>& b)
{
  int r = a.rows ();
  int c = a.cols ();
  int br = b.rows ();
  int bc = b.cols ();
  if (r != br || c != bc)
    gripe_nonconformant ("operator +=", r, c, br, bc);
  else
    {
      if (r > 0 && c > 0)
	{
	  int l = a.length ();
	  DO_VV_OP2 (+=);
	}
    }
  return a;
}

template <class T>
MArray2<T>&
operator -= (MArray2<T>& a, const MArray2<T>& b)
{
  int r = a.rows ();
  int c = a.cols ();
  int br = b.rows ();
  int bc = b.cols ();
  if (r != br || c != bc)
    gripe_nonconformant ("operator -=", r, c, br, bc);
  else
    {
      if (r > 0 && c > 0)
	{
	  int l = a.length ();
	  DO_VV_OP2 (-=);
	}
    }
  return a;
}

// Element by element MArray2 by scalar ops.

#define MARRAY_A2S_OP(OP) \
  template <class T> \
  MArray2<T> \
  operator OP (const MArray2<T>& a, const T& s) \
  { \
    MArray2<T> result (a.rows (), a.cols ()); \
    T *r = result.fortran_vec (); \
    int l = a.length (); \
    const T *v = a.data (); \
    DO_VS_OP (r, l, v, OP, s); \
    return result; \
  }

MARRAY_A2S_OP (+)
MARRAY_A2S_OP (-)
MARRAY_A2S_OP (*)
MARRAY_A2S_OP (/)

// Element by element scalar by MArray2 ops.

#define MARRAY_SA2_OP(OP) \
  template <class T> \
  MArray2<T> \
  operator OP (const T& s, const MArray2<T>& a) \
  { \
    MArray2<T> result (a.rows (), a.cols ()); \
    T *r = result.fortran_vec (); \
    int l = a.length (); \
    const T *v = a.data (); \
    DO_SV_OP (r, l, s, OP, v); \
    return result; \
  }

MARRAY_SA2_OP (+)
MARRAY_SA2_OP (-)
MARRAY_SA2_OP (*)
MARRAY_SA2_OP (/)

// Element by element MArray2 by MArray2 ops.

#define MARRAY_A2A2_OP(FCN, OP) \
  template <class T> \
  MArray2<T> \
  FCN (const MArray2<T>& a, const MArray2<T>& b) \
  { \
    int a_nr = a.rows (); \
    int a_nc = a.cols (); \
    int b_nr = b.rows (); \
    int b_nc = b.cols (); \
    if (a_nr != b_nr || a_nc != b_nc) \
      { \
        gripe_nonconformant (#FCN, a_nr, a_nc, b_nr, b_nc); \
	return MArray2<T> (); \
      } \
    if (a_nr == 0 || a_nc == 0) \
      return MArray2<T> (a_nr, a_nc); \
    int l = a.length (); \
    MArray2<T> result (a_nr, a_nc); \
    T *r = result.fortran_vec (); \
    const T *x = a.data (); \
    const T *y = b.data (); \
    DO_VV_OP (r, l, x, OP, y); \
    return result; \
  }

MARRAY_A2A2_OP (operator +, +)
MARRAY_A2A2_OP (operator -, -)
MARRAY_A2A2_OP (product,    *)
MARRAY_A2A2_OP (quotient,   /)

// Unary MArray2 ops.

template <class T>
MArray2<T>
operator + (const MArray2<T>& a)
{
  return a;
}

template <class T>
MArray2<T>
operator - (const MArray2<T>& a)
{
  int l = a.length ();
  MArray2<T> result (a.rows (), a.cols ());
  T *r = result.fortran_vec ();
  const T *x = a.data ();
  NEG_V (r, l, x);
  return result;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
