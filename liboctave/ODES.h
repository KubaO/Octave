/*

Copyright (C) 2002 John W. Eaton

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

#if !defined (octave_ODES_h)
#define octave_ODES_h 1

#if defined (__GNUG__) && ! defined (NO_PRAGMA_INTERFACE_IMPLEMENTATION)
#pragma interface
#endif

#include "ODESFunc.h"
#include "base-de.h"

class
ODES : public base_diff_eqn, public ODESFunc
{
public:

  ODES (void)
    : base_diff_eqn (), ODESFunc (), theta () { }

  ODES (const ColumnVector& x, double t, ODESFunc& f)
    : base_diff_eqn (x, t), ODESFunc (f), xdot (x.length (), 0.0), theta () { }

  ODES (const ColumnVector& x, const ColumnVector& xtheta, double t,
	ODESFunc& f)
    : base_diff_eqn (x, t), ODESFunc (f), xdot (x.length (), 0.0),
      theta (xtheta) { }

  ODES (const ODES& a)
    : base_diff_eqn (a), ODESFunc (a), theta (a.theta) { }

  ODES& operator = (const ODES& a)
    {
      if (this != &a)
	{
	  base_diff_eqn::operator = (a);
	  ODESFunc::operator = (a);

	  xdot = a.xdot;
	  theta = a.theta;
	}
      return *this;
    }

  ~ODES (void) { }

  ColumnVector parameter_vector (void) { return theta; }

  void initialize (const ColumnVector& x, double t);

  void initialize (const ColumnVector& x, double t,
		   const ColumnVector& theta);

protected:

  // State vector time derivatives.
  ColumnVector xdot;

  // Parameter vector.
  ColumnVector theta;
};

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
