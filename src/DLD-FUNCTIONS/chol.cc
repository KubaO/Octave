/*

Copyright (C) 1996, 1997, 1999, 2000, 2002, 2005, 2006, 2007
              John W. Eaton

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

// The cholupdate function was written by Jaroslav Hajek
// <highegg@gmail.com>, Copyright (C) 2008  VZLU Prague, a.s., Czech
// Republic.

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CmplxCHOL.h"
#include "dbleCHOL.h"
#include "SparseCmplxCHOL.h"
#include "SparsedbleCHOL.h"
#include "oct-spparms.h"
#include "sparse-util.h"

#include "ov-re-sparse.h"
#include "ov-cx-sparse.h"
#include "defun-dld.h"
#include "error.h"
#include "gripes.h"
#include "oct-obj.h"
#include "utils.h"

DEFUN_DLD (chol, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{r}} = chol (@var{a})\n\
@deftypefnx {Loadable Function} {[@var{r}, @var{p}]} = chol (@var{a})\n\
@deftypefnx {Loadable Function} {[@var{r}, @var{p}, @var{q}]} = chol (@var{s})\n\
@deftypefnx {Loadable Function} {[@var{r}, @var{p}, @var{q}]} = chol (@var{s}, 'vector')\n\
@deftypefnx {Loadable Function} {[@var{l}, @dots{}]} = chol (@dots{}, 'lower')\n\
@cindex Cholesky factorization\n\
Compute the Cholesky factor, @var{r}, of the symmetric positive definite\n\
matrix @var{a}, where\n\
@iftex\n\
@tex\n\
$ R^T R = A $.\n\
@end tex\n\
@end iftex\n\
@ifinfo\n\
\n\
@example\n\
@var{r}' * @var{r} = @var{a}.\n\
@end example\n\
@end ifinfo\n\
\n\
Called with one output argument @code{chol} fails if @var{a} or @var{s} is\n\
not positive definite. With two or more output arguments @var{p} flags\n\
whether the matrix was positive definite and @code{chol} does not fail. A\n\
zero value indicated that the matrix was positive definite and the @var{r}\n\
gives the factorization, annd @var{p} will have a positive value otherwise.\n\
\n\
If called with 3 outputs then a sparsity preserving row/column permutation\n\
is applied to @var{a} prior to the factorization. That is @var{r}\n\
is the factorization of @code{@var{a}(@var{q},@var{q})} such that\n\
@iftex\n\
@tex\n\
$ R^T R = Q^T A Q$.\n\
@end tex\n\
@end iftex\n\
@ifinfo\n\
\n\
@example\n\
@var{r}' * @var{r} = @var{q}' * @var{a} * @var{q}.\n\
@end example\n\
@end ifinfo\n\
\n\
The sparsity preserving permutation is generally returned as a matrix.\n\
However, given the flag 'vector', @var{q} will be returned as a vector\n\
such that\n\
@iftex\n\
@tex\n\
$ R^T R = A (Q, Q)$.\n\
@end tex\n\
@end iftex\n\
@ifinfo\n\
\n\
@example\n\
@var{r}' * @var{r} = a (@var{q}, @var{q}).\n\
@end example\n\
@end ifinfo\n\
\n\
Called with either a sparse or full matrix and uing the 'lower' flag,\n\
@code{chol} returns the lower triangular factorization such that\n\
@iftex\n\
@tex\n\
$ L L^T = A $.\n\
@end tex\n\
@end iftex\n\
@ifinfo\n\
\n\
@example\n\
@var{l} * @var{l}' = @var{a}.\n\
@end example\n\
@end ifinfo\n\
\n\
In general the lower trinagular factorization is significantly faster for\n\
sparse matrices.\n\
@seealso{cholinv, chol2inv}\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();
  bool LLt = false;
  bool vecout = false;

  if (nargin < 1 || nargin > 3 || nargout > 3 
      || (! args(0).is_sparse_type () && nargout > 2))
    {
      print_usage ();
      return retval;
    }

  int n = 1;
  while (n < nargin && ! error_state)
    {
      std::string tmp = args(n++).string_value ();

      if (! error_state )
	{
	  if (tmp.compare ("vector") == 0)
	    vecout = true;
	  else if (tmp.compare ("lower") == 0)
	    LLt = true;
	  else if (tmp.compare ("upper") == 0)
	    LLt = false;
	  else
	    error ("chol: unexpected second or third input");
	}
      else
	error ("chol: expecting trailing string arguments");
    }

  if (! error_state)
    {
      octave_value arg = args(0);
    
      octave_idx_type nr = arg.rows ();
      octave_idx_type nc = arg.columns ();
      bool natural = (nargout != 3);

      int arg_is_empty = empty_arg ("chol", nr, nc);

      if (arg_is_empty < 0)
	return retval;
      if (arg_is_empty > 0)
	return octave_value (Matrix ());

      if (arg.is_sparse_type ())
	{
	  if (arg.is_real_type ())
	    {
	      SparseMatrix m = arg.sparse_matrix_value ();

	      if (! error_state)
		{
		  octave_idx_type info;
		  SparseCHOL fact (m, info, natural);
		  if (nargout == 3)
		    if (vecout)
		      retval(2) = fact.perm ();
		    else
		      retval(2) = fact.Q();

		  if (nargout > 1 || info == 0)
		    {
		      retval(1) = fact.P();
		      if (LLt)
			retval(0) = fact.L();
		      else
			retval(0) = fact.R();
		    }
		  else
		    error ("chol: matrix not positive definite");
		}
	    }
	  else if (arg.is_complex_type ())
	    {
	      SparseComplexMatrix m = arg.sparse_complex_matrix_value ();

	      if (! error_state)
		{
		  octave_idx_type info;
		  SparseComplexCHOL fact (m, info, natural);

		  if (nargout == 3)
		    if (vecout)
		      retval(2) = fact.perm ();
		    else
		      retval(2) = fact.Q();
	  
		  if (nargout > 1 || info == 0)
		    {
		      retval(1) = fact.P();
		      if (LLt)
			retval(0) = fact.L();
		      else
			retval(0) = fact.R();
		    }
		  else
		    error ("chol: matrix not positive definite");
		}
	    }
	  else
	    gripe_wrong_type_arg ("chol", arg);
	}
      else
	{
	  if (arg.is_real_type ())
	    {
	      Matrix m = arg.matrix_value ();

	      if (! error_state)
		{
		  octave_idx_type info;
		  CHOL fact (m, info);
		  if (nargout == 2 || info == 0)
		    {
		      retval(1) = static_cast<double> (info);
		      if (LLt)
			retval(0) = fact.chol_matrix ().transpose ();
		      else
			retval(0) = fact.chol_matrix ();
		    }
		  else
		    error ("chol: matrix not positive definite");
		}
	    }
	  else if (arg.is_complex_type ())
	    {
	      ComplexMatrix m = arg.complex_matrix_value ();

	      if (! error_state)
		{
		  octave_idx_type info;
		  ComplexCHOL fact (m, info);
		  if (nargout == 2 || info == 0)
		    {
		      retval(1) = static_cast<double> (info);
		      if (LLt)
			retval(0) = fact.chol_matrix ().hermitian ();
		      else
			retval(0) = fact.chol_matrix ();
		    }
		  else
		    error ("chol: matrix not positive definite");
		}
	    }
	  else
	    gripe_wrong_type_arg ("chol", arg);
	}
    }

  return retval;
}

DEFUN_DLD (cholinv, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} cholinv (@var{a})\n\
Use the Cholesky factorization to compute the inverse of the\n\
symmetric positive definite matrix @var{a}.\n\
@seealso{chol, chol2inv}\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
      octave_value arg = args(0);
    
      octave_idx_type nr = arg.rows ();
      octave_idx_type nc = arg.columns ();

      if (nr == 0 || nc == 0)
	retval = Matrix ();
      else
	{
	  if (arg.is_sparse_type ())
	    {
	      if (arg.is_real_type ())
		{
		  SparseMatrix m = arg.sparse_matrix_value ();

		  if (! error_state)
		    {
		      octave_idx_type info;
		      SparseCHOL chol (m, info);
		      if (info == 0)
			retval = chol.inverse ();
		      else
			error ("cholinv: matrix not positive definite");
		    }
		}
	      else if (arg.is_complex_type ())
		{
		  SparseComplexMatrix m = arg.sparse_complex_matrix_value ();

		  if (! error_state)
		    {
		      octave_idx_type info;
		      SparseComplexCHOL chol (m, info);
		      if (info == 0)
			retval = chol.inverse ();
		      else
			error ("cholinv: matrix not positive definite");
		    }
		}
	      else
		gripe_wrong_type_arg ("cholinv", arg);
	    }
	  else
	    {
	      if (arg.is_real_type ())
		{
		  Matrix m = arg.matrix_value ();

		  if (! error_state)
		    {
		      octave_idx_type info;
		      CHOL chol (m, info);
		      if (info == 0)
			retval = chol.inverse ();
		      else
			error ("cholinv: matrix not positive definite");
		    }
		}
	      else if (arg.is_complex_type ())
		{
		  ComplexMatrix m = arg.complex_matrix_value ();

		  if (! error_state)
		    {
		      octave_idx_type info;
		      ComplexCHOL chol (m, info);
		      if (info == 0)
			retval = chol.inverse ();
		      else
			error ("cholinv: matrix not positive definite");
		    }
		}
	      else
		gripe_wrong_type_arg ("chol", arg);
	    }
	}
    }
  else
    print_usage ();

  return retval;
}

DEFUN_DLD (chol2inv, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} chol2inv (@var{u})\n\
Invert a symmetric, positive definite square matrix from its Cholesky\n\
decomposition, @var{u}.  Note that @var{u} should be an upper-triangular\n\
matrix with positive diagonal elements.  @code{chol2inv (@var{u})}\n\
provides @code{inv (@var{u}'*@var{u})} but it is much faster than\n\
using @code{inv}.\n\
@seealso{chol, cholinv}\n\
@end deftypefn")
{
  octave_value retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
      octave_value arg = args(0);
    
      octave_idx_type nr = arg.rows ();
      octave_idx_type nc = arg.columns ();

      if (nr == 0 || nc == 0)
	retval = Matrix ();
      else
	{
	  if (arg.is_sparse_type ())
	    {
	      if (arg.is_real_type ())
		{
		  SparseMatrix r = arg.sparse_matrix_value ();

		  if (! error_state)
		    retval = chol2inv (r);
		}
	      else if (arg.is_complex_type ())
		{
		  SparseComplexMatrix r = arg.sparse_complex_matrix_value ();

		  if (! error_state)
		    retval = chol2inv (r);
		}
	      else
		gripe_wrong_type_arg ("chol2inv", arg);
	    }
	  else
	    {
	      if (arg.is_real_type ())
		{
		  Matrix r = arg.matrix_value ();

		  if (! error_state)
		    retval = chol2inv (r);
		}
	      else if (arg.is_complex_type ())
		{
		  ComplexMatrix r = arg.complex_matrix_value ();

		  if (! error_state)
		    retval = chol2inv (r);
		}
	      else
		gripe_wrong_type_arg ("chol2inv", arg);
	    }
	}
    }
  else
    print_usage ();

  return retval;
}

DEFUN_DLD (cholupdate, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {[@var{R1}, @var{info}]} = cholupdate (@var{R}, @var{u}, @var{op})\n\
Update or downdate a Cholesky factorization.  Given an upper triangular\n\
matrix @var{R} and a column vector @var{u}, attempt to determine another\n\
upper triangular matrix @var{R1} such that\n\
@itemize @bullet\n\
@item\n\
@var{R1}'*@var{R1} = @var{R}'*@var{R} + @var{u}*@var{u}'\n\
if @var{op} is \"+\"\n\
@item\n\
@var{R1}'*@var{R1} = @var{R}'*@var{R} - @var{u}*@var{u}'\n\
if @var{op} is \"-\"\n\
@end itemize\n\
\n\
If @var{op} is \"-\", @var{info} is set to\n\
@itemize\n\
@item 0 if the downdate was successful,\n\
@item 1 if @var{R}'*@var{R} - @var{u}*@var{u}' is not positive definite,\n\
@item 2 if @var{R} is singular.\n\
@end itemize\n\
\n\
If @var{info} is not present, an error message is printed in cases 1 and 2.\n\
@seealso{chol, qrupdate}\n\
@end deftypefn")
{
  octave_idx_type nargin = args.length ();

  octave_value_list retval;

  if (nargin > 3 || nargin < 2)
    {
      print_usage ();
      return retval;
    }

  octave_value argr = args(0);
  octave_value argu = args(1);

  if (argr.is_matrix_type () && argu.is_matrix_type ()
      && (nargin < 3 || args(2).is_string ()))
    {
      octave_idx_type n = argr.rows ();

      std::string op = (nargin < 3) ? "+" : args(2).string_value ();

      bool down = op == "-";

      if (down || op == "+")
        if (argr.columns () == n && argu.rows () == n && argu.columns () == 1)
          {
            if (argr.is_real_matrix () && argu.is_real_matrix ())
              {
                // real case
                Matrix R = argr.matrix_value ();
                Matrix u = argu.matrix_value ();

                CHOL fact;
                fact.set (R);
                int err = 0;

                if (down)
                  err = fact.downdate (u);
                else
                  fact.update (u);

                if (nargout > 1)
                  retval(1) = err;
                else if (err)
                  error ("cholupdate: downdate violates positiveness");

                retval(0) = fact.chol_matrix ();
              }
            else
              {
                // complex case
                ComplexMatrix R = argr.complex_matrix_value ();
                ComplexMatrix u = argu.complex_matrix_value ();

                ComplexCHOL fact;
                fact.set (R);
                int err = 0;

                if (down)
                  err = fact.downdate (u);
                else
                  fact.update (u);

                if (nargout > 1)
                  retval(1) = err;
                else if (err)
                  error ("cholupdate: downdate violates positiveness");

                retval(0) = fact.chol_matrix ();
              }
          }
        else
          error ("cholupdate: dimension mismatch");
      else
        error ("cholupdate: op must be \"+\" or \"-\"");
    }
  else
    print_usage ();

  return retval;
}

/*
%!test
%! A = [  0.436997  -0.131721   0.124120  -0.061673 ;
%!       -0.131721   0.738529   0.019851  -0.140295 ;
%!        0.124120   0.019851   0.354879  -0.059472 ;
%!       -0.061673  -0.140295  -0.059472   0.600939 ];
%! 
%! u = [  0.98950 ;
%!        0.39844 ;
%!        0.63484 ;
%!        0.13351 ];
%! 
%! R = chol(A);
%! 
%! R1 = cholupdate(R,u);
%! 
%! assert(norm(triu(R1)-R1,Inf) == 0)
%! assert(norm(R1'*R1 - R'*R - u*u',Inf) < 1e1*eps)
%! 
%! R1 = cholupdate(R1,u,"-");
%! 
%! assert(norm(triu(R1)-R1,Inf) == 0)
%! assert(norm(R1 - R,Inf) < 1e1*eps)
%! 
%!test
%! A = [  0.5585528 + 0.0000000i  -0.1662088 - 0.0315341i   0.0107873 + 0.0236411i  -0.0276775 - 0.0186073i ;
%!       -0.1662088 + 0.0315341i   0.6760061 + 0.0000000i   0.0011452 - 0.0475528i   0.0145967 + 0.0247641i ;
%!        0.0107873 - 0.0236411i   0.0011452 + 0.0475528i   0.6263149 - 0.0000000i  -0.1585837 - 0.0719763i ;
%!       -0.0276775 + 0.0186073i   0.0145967 - 0.0247641i  -0.1585837 + 0.0719763i   0.6034234 - 0.0000000i ];
%! 
%! u = [ 0.54267 + 0.91519i ;
%!       0.99647 + 0.43141i ;
%!       0.83760 + 0.68977i ;
%!       0.39160 + 0.90378i ];
%! 
%! R = chol(A);
%! 
%! R1 = cholupdate(R,u);
%! 
%! assert(norm(triu(R1)-R1,Inf) == 0)
%! assert(norm(R1'*R1 - R'*R - u*u',Inf) < 1e1*eps)
%! 
%! R1 = cholupdate(R1,u,"-");
%! 
%! assert(norm(triu(R1)-R1,Inf) == 0)
%! assert(norm(R1 - R,Inf) < 1e1*eps)
*/

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/

