// f-hess.cc                                           -*- C++ -*-
/*

Copyright (C) 1993, 1994 John W. Eaton

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
Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dbleHESS.h"
#include "CmplxHESS.h"

#include "tree-const.h"
#include "user-prefs.h"
#include "error.h"
#include "gripes.h"
#include "utils.h"
#include "help.h"
#include "defun-dld.h"

DEFUN_DLD ("hess", Fhess, Shess, 2, 2,
  "[P, H] = hess (A) or H = hess (A): Hessenberg decomposition")
{
  Octave_object retval;

  if (args.length () != 2 || nargout > 2)
    {
      print_usage ("hess");
      return retval;
    }

  tree_constant arg = args(1);

  int nr = arg.rows ();
  int nc = arg.columns ();

  if (empty_arg ("hess", nr, nc) < 0)
    return retval;

  if (nr != nc)
    {
      gripe_square_matrix_required ("hess");
      return retval;
    }

  if (arg.is_real_type ())
    {
      Matrix tmp = arg.matrix_value ();

      if (! error_state)
	{
	  HESS result (tmp);

	  if (nargout == 0 || nargout == 1)
	    {
	      retval.resize (1);
	      retval(0) = result.hess_matrix ();
	    }
	  else
	    {
	      retval.resize (2);
	      retval(0) = result.unitary_hess_matrix ();
	      retval(1) = result.hess_matrix ();
	    }
	}
    }
  else if (arg.is_complex_type ())
    {
      ComplexMatrix ctmp = arg.complex_matrix_value ();

      if (! error_state)
	{
	  ComplexHESS result (ctmp);

	  if (nargout == 0 || nargout == 1)
	    {
	      retval.resize (1);
	      retval(0) = result.hess_matrix ();
	    }
	  else
	    {
	      retval.resize (2);
	      retval(0) = result.unitary_hess_matrix ();
	      retval(1) = result.hess_matrix ();
	    }
	}
    }
  else
    {
      gripe_wrong_type_arg ("hess", arg);
    }

  return retval;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
