//                                  -*- C++ -*-
/*

Copyright (C) 1992, 1993, 1994, 1995 John W. Eaton

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

#if !defined (octave_ComplexSVD_h)
#define octave_ComplexSVD_h 1

#if defined (__GNUG__)
#pragma interface
#endif

class ostream;

#include "dDiagMatrix.h"
#include "CMatrix.h"
#include "dbleSVD.h"

class ComplexSVD
{
friend class ComplexMatrix;

public:

  ComplexSVD (void) { }


  ComplexSVD (const ComplexMatrix& a, SVD::type svd_type = SVD::std)
    {
      init (a, svd_type);
    }

  ComplexSVD (const ComplexMatrix& a, int& info,
	      SVD::type svd_type = SVD::std)
    {
      info = init (a, svd_type);
    }

  ComplexSVD (const ComplexSVD& a)
    {
      sigma = a.sigma;
      left_sm = a.left_sm;
      right_sm = a.right_sm;
    }

  ComplexSVD& operator = (const ComplexSVD& a)
    {
      sigma = a.sigma;
      left_sm = a.left_sm;
      right_sm = a.right_sm;

      return *this;
    }

  DiagMatrix singular_values (void) const { return sigma; }

  ComplexMatrix left_singular_matrix (void) const { return left_sm; }

  ComplexMatrix right_singular_matrix (void) const { return right_sm; }

  friend ostream&  operator << (ostream& os, const ComplexSVD& a);

private:

  int init (const ComplexMatrix& a, SVD::type svd_type = SVD::std);

  DiagMatrix sigma;
  ComplexMatrix left_sm;
  ComplexMatrix right_sm;
};

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
