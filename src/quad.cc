// f-quad.cc                                           -*- C++ -*-
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

#include <strstream.h>

#include "Quad.h"

#include "tree-const.h"
#include "variables.h"
#include "mappers.h"
#include "gripes.h"
#include "error.h"
#include "utils.h"
#include "pager.h"
#include "defun-dld.h"

// Global pointer for user defined function required by quadrature functions.
static tree_fvc *quad_fcn;

static Quad_options quad_opts;

double
quad_user_function (double x)
{
  double retval = 0.0;

//  tree_constant name = quad_fcn->name ();
  Octave_object args (2);
//  args(0) = name;
  args(1) = x;

  if (quad_fcn)
    {
      Octave_object tmp = quad_fcn->eval (0, 1, args);

      if (error_state)
	{
	  quad_integration_error = 1;  // XXX FIXME XXX
	  gripe_user_supplied_eval ("quad");
	  return retval;
	}

      if (tmp.length () && tmp(0).is_defined ())
	retval = tmp(0).to_scalar ();
      else
	{
	  quad_integration_error = 1;  // XXX FIXME XXX
	  gripe_user_supplied_eval ("quad");
	}
    }

  return retval;
}

DEFUN_DLD ("quad", Fquad, Squad, 6, 3,
  "[V, IER, NFUN] = quad (F, A, B [, TOL] [, SING])\n\
\n\
Where the first argument is the name of the  function to call to\n\
compute the value of the integrand.  It must have the form\n\
\n\
  y = f (x)
\n\
where y and x are scalars.\n\
\n\
The second and third arguments are limits of integration.  Either or\n\
both may be infinite.  The optional argument TOL specifies the desired\n\
accuracy of the result.  The optional argument SING is a vector of\n\
at which the integrand is singular.")
{
  Octave_object retval;

  int nargin = args.length ();

  if (nargin < 4 || nargin > 6 || nargout > 4)
    {
      print_usage ("quad");
      return retval;
    }

  quad_fcn = is_valid_function (args(1), "fsolve", 1);
  if (! quad_fcn || takes_correct_nargs (quad_fcn, 2, "fsolve", 1) != 1)
    return retval;

  double a = args(2).to_scalar ();
  double b = args(3).to_scalar ();

  int indefinite = 0;
  IndefQuad::IntegralType indef_type = IndefQuad::doubly_infinite;
  double bound = 0.0;
  if ((int) xisinf (a) && (int) xisinf (b))
    {
      indefinite = 1;
      indef_type = IndefQuad::doubly_infinite;
    }
  else if ((int) xisinf (a))
    {
      indefinite = 1;
      bound = b;
      indef_type = IndefQuad::neg_inf_to_bound;
    }
  else if ((int) xisinf (b))
    {
      indefinite = 1;
      bound = a;
      indef_type = IndefQuad::bound_to_inf;
    }

  int ier = 0;
  int nfun = 0;
  double abserr = 0.0;
  double val = 0.0;
  double abstol = 1e-6;
  double reltol = 1e-6;
  Vector tol (2);
  Vector sing;
  int have_sing = 0;
  switch (nargin)
    {
    case 6:
      if (indefinite)
	{
	  error("quad: singularities not allowed on infinite intervals");
	  return retval;
	}
      have_sing = 1;
      sing = args(5).to_vector ();
    case 5:
      tol = args(4).to_vector ();
      switch (tol.capacity ())
	{
	case 2:
	  reltol = tol.elem (1);
	case 1:
	  abstol = tol.elem (0);
	  break;
	default:
	  error ("quad: expecting tol to contain no more than two values");
	  return retval;
	}
    case 4:
      if (indefinite)
	{
	  IndefQuad iq (quad_user_function, bound, indef_type, abstol, reltol);
	  iq.copy (quad_opts);
	  val = iq.integrate (ier, nfun, abserr);
	}
      else
	{
	  if (have_sing)
	    {
	      DefQuad dq (quad_user_function, a, b, sing, abstol, reltol);
	      dq.copy (quad_opts);
	      val = dq.integrate (ier, nfun, abserr);
	    }
	  else
	    {
	      DefQuad dq (quad_user_function, a, b, abstol, reltol);
	      dq.copy (quad_opts);
	      val = dq.integrate (ier, nfun, abserr);
	    }
	}
      break;
    default:
      panic_impossible ();
      break;
    }

  retval.resize (4);

  retval(0) = val;
  retval(1) = ier;
  retval(2) = nfun;
  retval(3) = abserr;

  return retval;
}

typedef void (Quad_options::*d_set_opt_mf) (double);
typedef double (Quad_options::*d_get_opt_mf) (void);

#define MAX_TOKENS 2

struct QUAD_OPTIONS
{
  char *keyword;
  char *kw_tok[MAX_TOKENS + 1];
  int min_len[MAX_TOKENS + 1];
  int min_toks_to_match;
  d_set_opt_mf d_set_fcn;
  d_get_opt_mf d_get_fcn;
};

static QUAD_OPTIONS quad_option_table [] =
{
  { "absolute tolerance",
    { "absolute", "tolerance", 0, },
    { 1, 0, 0, }, 1,
    Quad_options::set_absolute_tolerance,
    Quad_options::absolute_tolerance, },

  { "relative tolerance",
    { "relative", "tolerance", 0, },
    { 1, 0, 0, }, 1,
    Quad_options::set_relative_tolerance,
    Quad_options::relative_tolerance, },

  { 0,
    { 0, 0, 0, },
    { 0, 0, 0, }, 0,
    0, 0, },
};

static void
print_quad_option_list (void)
{
  ostrstream output_buf;

  print_usage ("quad_options", 1);

  output_buf << "\n"
	     << "Options for quad include:\n\n"
	     << "  keyword                                  value\n"
	     << "  -------                                  -----\n\n";

  QUAD_OPTIONS *list = quad_option_table;

  char *keyword;
  while ((keyword = list->keyword) != 0)
    {
      output_buf.form ("  %-40s ", keyword);

      double val = (quad_opts.*list->d_get_fcn) ();
      if (val < 0.0)
	output_buf << "computed automatically";
      else
	output_buf << val;

      output_buf << "\n";
      list++;
    }

  output_buf << "\n" << ends;
  maybe_page_output (output_buf);
}

static void
do_quad_option (char *keyword, double val)
{
  QUAD_OPTIONS *list = quad_option_table;

  while (list->keyword != 0)
    {
      if (keyword_almost_match (list->kw_tok, list->min_len, keyword,
				list->min_toks_to_match, MAX_TOKENS))
	{
	  (quad_opts.*list->d_set_fcn) (val);

	  return;
	}
      list++;
    }

  warning ("quad_options: no match for `%s'", keyword);
}

DEFUN_DLD ("quad_options", Fquad_options, Squad_options, -1, 1,
  "quad_options (KEYWORD, VALUE)\n\
\n\
Set or show options for quad.  Keywords may be abbreviated\n\
to the shortest match.")
{
  Octave_object retval;

  int nargin = args.length ();

  if (nargin == 1)
    print_quad_option_list ();
  else if (nargin == 3)
    {
      if (args(1).is_string_type ())
	{
	  char *keyword = args(1).string_value ();
	  double val = args(2).double_value ();
	  do_quad_option (keyword, val);
	}
      else
	print_usage ("quad_options");
    }
  else
    print_usage ("quad_options");

  return retval;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
