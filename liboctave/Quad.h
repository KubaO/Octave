/*

Copyright (C) 1993, 1994, 1995, 1996, 1997, 2000, 2002, 2004, 2005,
              2006, 2007 John W. Eaton

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

#if !defined (octave_Quad_h)
#define octave_Quad_h 1

#include <cfloat>
#include <cmath>

#include "dColVector.h"

#if !defined (octave_Quad_typedefs)
#define octave_Quad_typedefs 1

typedef double (*integrand_fcn) (double x);

#endif

// FIXME -- would be nice to not have to have this global
// variable.
// Nonzero means an error occurred in the calculation of the integrand
// function, and the user wants us to quit.
extern OCTAVE_API int quad_integration_error;

#include "Quad-opts.h"

class
OCTAVE_API
Quad : public Quad_options
{
 public:

  Quad (integrand_fcn fcn)
    : Quad_options (), f (fcn) { }

  virtual ~Quad (void) { }

  virtual double integrate (void)
    {
      octave_idx_type ier, neval;
      double abserr;
      return do_integrate (ier, neval, abserr);
    }

  virtual double integrate (octave_idx_type& ier)
    {
      octave_idx_type neval;
      double abserr;
      return do_integrate (ier, neval, abserr);
    }

  virtual double integrate (octave_idx_type& ier, octave_idx_type& neval)
    {
      double abserr;
      return do_integrate (ier, neval, abserr);
    }

  virtual double integrate (octave_idx_type& ier, octave_idx_type& neval, double& abserr)
    {
      return do_integrate (ier, neval, abserr);
    }

  virtual double do_integrate (octave_idx_type& ier, octave_idx_type& neval, double& abserr) = 0;

 protected:

  integrand_fcn f;
};

class
OCTAVE_API
DefQuad : public Quad
{
 public:

  DefQuad (integrand_fcn fcn)
    : Quad (fcn), lower_limit (0.0), upper_limit (1.0), singularities () { }

  DefQuad (integrand_fcn fcn, double ll, double ul)
    : Quad (fcn), lower_limit (ll), upper_limit (ul), singularities () { }

  DefQuad (integrand_fcn fcn, double ll, double ul,
	   const ColumnVector& sing)
    : Quad (fcn), lower_limit (ll), upper_limit (ul),
      singularities (sing) { }

  DefQuad (integrand_fcn fcn, const ColumnVector& sing)
    : Quad (fcn), lower_limit (0.0), upper_limit (1.0),
      singularities (sing) { }

  ~DefQuad (void) { }

  double do_integrate (octave_idx_type& ier, octave_idx_type& neval, double& abserr);

 private:

  double lower_limit;
  double upper_limit;

  ColumnVector singularities;
};

class
OCTAVE_API
IndefQuad : public Quad
{
 public:

  enum IntegralType { bound_to_inf, neg_inf_to_bound, doubly_infinite };

  IndefQuad (integrand_fcn fcn)
    : Quad (fcn), bound (0.0), type (bound_to_inf) { }

  IndefQuad (integrand_fcn fcn, double b, IntegralType t)
    : Quad (fcn), bound (b), type (t) { }

  ~IndefQuad (void) { }

  double do_integrate (octave_idx_type& ier, octave_idx_type& neval, double& abserr);

 private:

  double bound;
  IntegralType type;
  int integration_error;
};

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
