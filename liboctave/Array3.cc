// Template array classes                              -*- C++ -*-
/*

Copyright (C) 1996 John W. Eaton

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

#if defined (__GNUG__)
#pragma implementation
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cassert>

#include <iostream.h>

#include "Array3.h"

#if defined (HEAVYWEIGHT_INDEXING)
#include "idx-vector.h"
#include "Array3-idx.h"
#endif

#include "lo-error.h"

// Three dimensional array class.

template <class T>
T&
Array3<T>::checkelem (int i, int j, int k)
{
  if (i < 0 || j < 0 || k < 0 || i >= d1 || j >= d2 || k >= d3)
    {
      (*current_liboctave_error_handler) ("range error");
      static T foo;
      return foo;
    }
  return Array2<T>::elem (i, d1*k+j);
}

template <class T>
T
Array3<T>::elem (int i, int j, int k) const
{
  return Array2<T>::elem (i, d2*k+j);
}

template <class T>
T
Array3<T>::checkelem (int i, int j, int k) const
{
  if (i < 0 || j < 0 || k < 0 || i >= d1 || j >= d2 || k >= d3)
    {
      (*current_liboctave_error_handler) ("range error");
      T foo;
      static T *bar = &foo;
      return foo;
    }
  return Array2<T>::elem (i, d1*k+j);
}

template <class T>
T
Array3<T>::operator () (int i, int j, int k) const
{
  if (i < 0 || j < 0 || k < 0 || i >= d1 || j >= d2 || k >= d3)
    {
      (*current_liboctave_error_handler) ("range error");
      T foo;
      static T *bar = &foo;
      return foo;
    }
  return Array2<T>::elem (i, d2*k+j);
}

template <class T>
void
Array3<T>::resize (int n, int m, int k)
{
  assert (0); // XXX FIXME XXX
}

template <class T>
void
Array3<T>::resize (int n, int m, int k, const T& val)
{
  assert (0); // XXX FIXME XXX
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
