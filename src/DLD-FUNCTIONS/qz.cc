/*

Copyright (C) 1998, 1999, 2000, 2002, 2003, 2004, 2005, 2006, 2007
              A. S. Hodel

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

// Generalized eigenvalue balancing via LAPACK

// Author: A. S. Hodel <scotte@eng.auburn.edu>

#undef DEBUG
#undef DEBUG_SORT
#undef DEBUG_EIG

#include "config.h"

#include <cfloat>

#include <iostream>
#include <iomanip>

#include "CmplxQRP.h"
#include "dbleQR.h"
#include "f77-fcn.h"
#include "lo-math.h"
#include "quit.h"

#include "defun-dld.h"
#include "error.h"
#include "gripes.h"
#include "oct-obj.h"
#include "oct-map.h"
#include "ov.h"
#include "pager.h"
#if defined (DEBUG) || defined (DEBUG_SORT)
#include "pr-output.h"
#endif
#include "symtab.h"
#include "utils.h"
#include "variables.h"

typedef octave_idx_type (*sort_function) (const octave_idx_type& LSIZE, const double& ALPHA,
			      const double& BETA, const double& S,
			      const double& P);

extern "C"
{
  F77_RET_T
  F77_FUNC (dggbal, DGGBAL) (F77_CONST_CHAR_ARG_DECL,
			     const octave_idx_type& N, double* A, const octave_idx_type& LDA,
			     double* B, const octave_idx_type& LDB, octave_idx_type& ILO,
			     octave_idx_type& IHI, double* LSCALE, double* RSCALE,
			     double* WORK, octave_idx_type& INFO
			     F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (dggbak, DGGBAK) (F77_CONST_CHAR_ARG_DECL,
			     F77_CONST_CHAR_ARG_DECL,
			     const octave_idx_type& N, const octave_idx_type& ILO,
			     const octave_idx_type& IHI, const double* LSCALE,
			     const double* RSCALE, octave_idx_type& M, double* V,
			     const octave_idx_type& LDV, octave_idx_type& INFO
			     F77_CHAR_ARG_LEN_DECL
			     F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (dgghrd, DGGHRD) (F77_CONST_CHAR_ARG_DECL,
			     F77_CONST_CHAR_ARG_DECL,
			     const octave_idx_type& N, const octave_idx_type& ILO,
			     const octave_idx_type& IHI, double* A,
			     const octave_idx_type& LDA, double* B,
			     const octave_idx_type& LDB, double* Q,
			     const octave_idx_type& LDQ, double* Z,
			     const octave_idx_type& LDZ, octave_idx_type& INFO
			     F77_CHAR_ARG_LEN_DECL
			     F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (dhgeqz, DHGEQZ) (F77_CONST_CHAR_ARG_DECL,
			     F77_CONST_CHAR_ARG_DECL,
			     F77_CONST_CHAR_ARG_DECL,
			     const octave_idx_type& N, const octave_idx_type& ILO, const octave_idx_type& IHI,
			     double* A, const octave_idx_type& LDA, double* B,
			     const octave_idx_type& LDB, double* ALPHAR,
			     double* ALPHAI, double* BETA, double* Q,
			     const octave_idx_type& LDQ, double* Z,
			     const octave_idx_type& LDZ, double* WORK,
			     const octave_idx_type& LWORK, octave_idx_type& INFO
			     F77_CHAR_ARG_LEN_DECL
			     F77_CHAR_ARG_LEN_DECL
			     F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (dlag2, DLAG2) (const double* A, const octave_idx_type& LDA, const double* B,
			   const octave_idx_type& LDB, const double& SAFMIN,
			   double& SCALE1, double& SCALE2,
			   double& WR1, double& WR2, double& WI);

  // Van Dooren's code (netlib.org: toms/590) for reordering
  // GEP.  Only processes Z, not Q.
  F77_RET_T
  F77_FUNC (dsubsp, DSUBSP) (const octave_idx_type& NMAX, const octave_idx_type& N, double* A,
			     double* B, double* Z, sort_function,
			     const double& EPS, octave_idx_type& NDIM, octave_idx_type& FAIL,
			     octave_idx_type* IND);

  // documentation for DTGEVC incorrectly states that VR, VL are
  // complex*16; they are declared in DTGEVC as double precision
  // (probably a cut and paste problem fro ZTGEVC)
  F77_RET_T
  F77_FUNC (dtgevc, DTGEVC) (F77_CONST_CHAR_ARG_DECL,
			     F77_CONST_CHAR_ARG_DECL,
			     octave_idx_type* SELECT, const octave_idx_type& N, double* A,
			     const octave_idx_type& LDA, double* B,
			     const octave_idx_type& LDB, double* VL,
			     const octave_idx_type& LDVL, double* VR,
			     const octave_idx_type& LDVR, const octave_idx_type& MM,
			     octave_idx_type& M, double* WORK, octave_idx_type& INFO
			     F77_CHAR_ARG_LEN_DECL
			     F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (xdlamch, XDLAMCH) (F77_CONST_CHAR_ARG_DECL,
			       double& retval
			       F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (xdlange, XDLANGE) (F77_CONST_CHAR_ARG_DECL,
			       const octave_idx_type&, const octave_idx_type&, const double*,
			       const octave_idx_type&, double*, double&
			       F77_CHAR_ARG_LEN_DECL);
}

// fcrhp, fin, fout, folhp:
// routines for ordering of generalized eigenvalues
// return 1 if  test is passed, 0 otherwise
//    fin: |lambda| < 1
//    fout: |lambda| >= 1
//    fcrhp: real(lambda) >= 0
//    folhp: real(lambda) < 0

static octave_idx_type
fcrhp (const octave_idx_type& lsize, const double& alpha,
       const double& beta, const double& s, const double&)
{
  if (lsize == 1)
    return (alpha*beta >= 0 ? 1 : -1);
  else
    return (s >= 0 ? 1 : -1);
}

static octave_idx_type
fin (const octave_idx_type& lsize, const double& alpha,
     const double& beta, const double&, const double& p)
{
  octave_idx_type retval;

  if (lsize == 1)
    retval = (fabs (alpha) < fabs (beta) ? 1 : -1);
  else
    retval = (fabs (p) < 1 ? 1 : -1);

#ifdef DEBUG
  std::cout << "qz: fin: retval=" << retval << std::endl;
#endif

  return retval;
}

static octave_idx_type
folhp (const octave_idx_type& lsize, const double& alpha,
       const double& beta, const double& s, const double&)
{
  if (lsize == 1)
    return (alpha*beta < 0 ? 1 : -1);
  else
    return (s < 0 ? 1 : -1);
}

static octave_idx_type
fout (const octave_idx_type& lsize, const double& alpha,
      const double& beta, const double&, const double& p)
{
  if (lsize == 1)
    return (fabs (alpha) >= fabs (beta) ? 1 : -1);
  else
    return (fabs (p) >= 1 ? 1 : -1);
}

DEFUN_DLD (qz, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {@var{lambda} =} qz (@var{a}, @var{b})\n\
Generalized eigenvalue problem @math{A x = s B x},\n\
@var{QZ} decomposition. There are three ways to call this function:\n\
@enumerate\n\
@item @code{lambda = qz(A,B)}\n\
\n\
Computes the generalized eigenvalues\n\
@iftex\n\
@tex\n\
$\\lambda$\n\
@end tex\n\
@end iftex\n\
@ifinfo\n\
@var{lambda}\n\
@end ifinfo\n\
of @math{(A - s B)}.\n\
@item @code{[AA, BB, Q, Z, V, W, lambda] = qz (A, B)}\n\
\n\
Computes qz decomposition, generalized eigenvectors, and \n\
generalized eigenvalues of @math{(A - sB)}\n\
@iftex\n\
@tex\n\
$$ AV = BV{ \\rm diag }(\\lambda) $$\n\
$$ W^T A = { \\rm diag }(\\lambda)W^T B $$\n\
$$ AA = Q^T AZ, BB = Q^T BZ $$\n\
@end tex\n\
@end iftex\n\
@ifinfo\n\
@example\n\
@group\n\
\n\
    A*V = B*V*diag(lambda)\n\
    W'*A = diag(lambda)*W'*B\n\
    AA = Q'*A*Z, BB = Q'*B*Z\n\
\n\
@end group\n\
@end example\n\
@end ifinfo\n\
with @var{Q} and @var{Z} orthogonal (unitary)= @var{I}\n\
\n\
@item @code{[AA,BB,Z@{, lambda@}] = qz(A,B,opt)}\n\
\n\
As in form [2], but allows ordering of generalized eigenpairs\n\
for (e.g.) solution of discrete time algebraic Riccati equations.\n\
Form 3 is not available for complex matrices, and does not compute\n\
the generalized eigenvectors @var{V}, @var{W}, nor the orthogonal matrix @var{Q}.\n\
@table @var\n\
@item opt\n\
for ordering eigenvalues of the GEP pencil.  The leading  block\n\
of the revised pencil contains all eigenvalues that satisfy:\n\
@table @code\n\
@item \"N\"\n\
= unordered (default) \n\
\n\
@item \"S\"\n\
= small: leading block has all |lambda| <=1 \n\
\n\
@item \"B\"\n\
= big: leading block has all |lambda| >= 1 \n\
\n\
@item \"-\"\n\
= negative real part: leading block has all eigenvalues\n\
in the open left half-plane\n\
\n\
@item \"+\"\n\
= nonnegative real part: leading block has all eigenvalues\n\
in the closed right half-plane\n\
@end  table\n\
@end table\n\
@end enumerate\n\
\n\
Note: qz performs permutation balancing, but not scaling (see balance).\n\
Order of output arguments was selected for compatibility with MATLAB\n\
\n\
@seealso{balance, dare, eig, schur}\n\
@end deftypefn")
{
  octave_value_list retval;
  int nargin = args.length ();

#ifdef DEBUG
  std::cout << "qz: nargin = " << nargin << ", nargout = " << nargout << std::endl;
#endif

  if (nargin < 2 || nargin > 3 || nargout > 7)
    {
      print_usage ();
      return retval;
    }
  else if (nargin == 3 && (nargout < 3 || nargout > 4))
    {
      error ("qz: invalid number of output arguments for form [3] call");
      return retval;
    }

#ifdef DEBUG
  std::cout << "qz: determine ordering option" << std::endl;
#endif

  // Determine ordering option
  std::string ord_job;
  static double safmin;

  if (nargin == 2)
    ord_job = "N";
  else if (!args(2).is_string ())
    {
      error ("qz: argument 3 must be a string");
      return retval;
    }
  else
    {
      ord_job = args(2).string_value ();

      if (ord_job[0] != 'N'
	  && ord_job[0] != 'S'
	  && ord_job[0] != 'B'
	  && ord_job[0] != '+'
	  && ord_job[0] != '-')
	{
	  error ("qz: invalid order option");
	  return retval;
	}

      // overflow constant required by dlag2
      F77_FUNC (xdlamch, XDLAMCH) (F77_CONST_CHAR_ARG2 ("S", 1),
				   safmin
				   F77_CHAR_ARG_LEN (1));

#ifdef DEBUG_EIG
      std::cout << "qz: initial value of safmin=" << setiosflags (std::ios::scientific)
	   << safmin << std::endl;
#endif

      // some machines (e.g., DEC alpha) get safmin = 0;
      // for these, use eps instead to avoid problems in dlag2
      if (safmin == 0)
	{
#ifdef DEBUG_EIG
	  std::cout << "qz: DANGER WILL ROBINSON: safmin is 0!" << std::endl;
#endif

	  F77_FUNC (xdlamch, XDLAMCH) (F77_CONST_CHAR_ARG2 ("E", 1),
				       safmin
				       F77_CHAR_ARG_LEN (1));

#ifdef DEBUG_EIG
	  std::cout << "qz: safmin set to " << setiosflags (std::ios::scientific)
	       << safmin << std::endl;
#endif
	}
    }

#ifdef DEBUG
  std::cout << "qz: check argument 1" << std::endl;
#endif

  // Argument 1: check if it's o.k. dimensioned
  octave_idx_type nn = args(0).rows ();

#ifdef DEBUG
  std::cout << "argument 1 dimensions: (" << nn << "," << args(0).columns () << ")"
       << std::endl;
#endif

  int arg_is_empty = empty_arg ("qz", nn, args(0).columns ());

  if (arg_is_empty < 0)
    {
      gripe_empty_arg ("qz: parameter 1", 0);
      return retval;
    }
  else if (arg_is_empty > 0)
    {
      gripe_empty_arg ("qz: parameter 1; continuing", 0);
      return octave_value_list (2, Matrix ());
    }
  else if (args(0).columns () != nn)
    {
      gripe_square_matrix_required ("qz");
      return retval;
    }

  // Argument 1: dimensions look good; get the value
  Matrix aa;
  ComplexMatrix caa;

  if (args(0).is_complex_type ())
    caa = args(0).complex_matrix_value ();
  else
    aa = args(0).matrix_value ();

  if (error_state)
    return retval;

#ifdef DEBUG
  std::cout << "qz: check argument 2" << std::endl;
#endif

  // Extract argument 2 (bb, or cbb if complex)
  if ((nn != args(1).columns ()) || (nn != args(1).rows ()))
    {
      gripe_nonconformant ();
      return retval;
    }

  Matrix bb;
  ComplexMatrix cbb;

  if (args(1).is_complex_type ())
    cbb = args(1).complex_matrix_value ();
  else
    bb = args(1).matrix_value ();

  if (error_state)
    return retval;

  // Both matrices loaded, now let's check what kind of arithmetic:
  //declared static to avoid compiler warnings about long jumps, vforks.

  static int complex_case
    = (args(0).is_complex_type () || args(1).is_complex_type ());

  if (nargin == 3 && complex_case)
    {
      error ("qz: cannot re-order complex qz decomposition.");
      return retval;
    }

  // first, declare variables used in both the real and complex case
  Matrix QQ(nn,nn), ZZ(nn,nn), VR(nn,nn), VL(nn,nn);
  RowVector alphar(nn), alphai(nn), betar(nn);

  ComplexMatrix CQ(nn,nn), CZ(nn,nn), CVR(nn,nn), CVL(nn,nn);
  octave_idx_type ilo, ihi, info;
  char compq = (nargout >= 3 ? 'V' : 'N');
  char compz = (nargout >= 4 ? 'V' : 'N');

  // initialize Q, Z to identity if we need either of them
  if (compq == 'V' || compz == 'V')
    for (octave_idx_type ii = 0; ii < nn; ii++)
      for (octave_idx_type jj = 0; jj < nn; jj++)
	{
	  OCTAVE_QUIT;
	  QQ(ii,jj) = ZZ(ii,jj) = (ii == jj ? 1.0 : 0.0);
	}

  // always perform permutation balancing
  const char bal_job = 'P';
  RowVector lscale(nn), rscale(nn), work(6*nn);

  if (complex_case)
    {
      error ("Complex case not implemented yet");
      return retval;
    }
  else
    {
#ifdef DEBUG
      if (compq == 'V')
	std::cout << "qz: performing balancing; QQ=" << std::endl << QQ << std::endl;
#endif

      F77_XFCN (dggbal, DGGBAL,
		(F77_CONST_CHAR_ARG2 (&bal_job, 1),
		 nn, aa.fortran_vec (), nn, bb.fortran_vec (),
		 nn, ilo, ihi, lscale.fortran_vec (),
		 rscale.fortran_vec (), work.fortran_vec (), info
		 F77_CHAR_ARG_LEN (1)));

      if (f77_exception_encountered)
	{
	  error ("unrecoverable error in qz (bal)");
	  return retval;
	}
    }

  // Since we just want the balancing matrices, we can use dggbal
  // for both the real and complex cases;
  // left first

  if (compq == 'V')
    {
      F77_XFCN (dggbak, DGGBAK,
		(F77_CONST_CHAR_ARG2 (&bal_job, 1),
		 F77_CONST_CHAR_ARG2 ("L", 1),
		 nn, ilo, ihi, lscale.data (), rscale.data (),
		 nn, QQ.fortran_vec (), nn, info
		 F77_CHAR_ARG_LEN (1)
		 F77_CHAR_ARG_LEN (1)));

#ifdef DEBUG
      if (compq == 'V')
	std::cout << "qz: balancing done; QQ=" << std::endl << QQ << std::endl;
#endif

    if (f77_exception_encountered)
      {
	error ("unrecoverable error in qz (bal-L)");
	return retval;
      }
  }

  // then right
  if (compz == 'V')
    {
      F77_XFCN (dggbak, DGGBAK,
		(F77_CONST_CHAR_ARG2 (&bal_job, 1),
		 F77_CONST_CHAR_ARG2 ("R", 1),
		 nn, ilo, ihi, lscale.data (), rscale.data (),
		 nn, ZZ.fortran_vec (), nn, info
		 F77_CHAR_ARG_LEN (1)
		 F77_CHAR_ARG_LEN (1)));

#ifdef DEBUG
      if (compz == 'V')
	std::cout << "qz: balancing done; ZZ=" << std::endl << ZZ << std::endl;
#endif

      if (f77_exception_encountered)
	{
	  error ("unrecoverable error in qz (bal-R)");
	  return retval;
	}
    }

  static char qz_job;
  qz_job = (nargout < 2 ? 'E' : 'S');	

  if (complex_case)
    {
      // complex case
      if (args(0).is_real_type ())
	caa = ComplexMatrix (aa);

      if (args(1).is_real_type ())
	cbb = ComplexMatrix (bb);

      if (compq == 'V')
	CQ = ComplexMatrix (QQ);

      if (compz == 'V')
	CZ = ComplexMatrix (ZZ);

      error ("complex case not done yet");
      return retval;
    }
  else  	// real matrices case
    {
#ifdef DEBUG
      std::cout << "qz: peforming qr decomposition of bb" << std::endl;
#endif

      // compute the QR factorization of bb
      QR bqr (bb);

#ifdef DEBUG
      std::cout << "qz: qr (bb) done; now peforming qz decomposition" << std::endl;
#endif

      bb = bqr.R ();

#ifdef DEBUG
      std::cout << "qz: extracted bb" << std::endl;
#endif

      aa = (bqr.Q ()).transpose ()*aa;

#ifdef DEBUG
      std::cout << "qz: updated aa " << std::endl;
      std::cout << "bqr.Q () = " << std::endl << bqr.Q () << std::endl;

      if (compq == 'V')
	std::cout << "QQ =" << QQ << std::endl;
#endif

      if (compq == 'V')
	QQ = QQ*bqr.Q ();

#ifdef DEBUG
      std::cout << "qz: precursors done..." << std::endl;
#endif

#ifdef DEBUG
      std::cout << "qz: compq = " << compq << ", compz = " << compz << std::endl;
#endif

      // reduce  to generalized hessenberg form
      F77_XFCN (dgghrd, DGGHRD,
		(F77_CONST_CHAR_ARG2 (&compq, 1),
		 F77_CONST_CHAR_ARG2 (&compz, 1),
		 nn, ilo, ihi, aa.fortran_vec (),
		 nn, bb.fortran_vec (), nn, QQ.fortran_vec (), nn,
		 ZZ.fortran_vec (), nn, info
		 F77_CHAR_ARG_LEN (1)
		 F77_CHAR_ARG_LEN (1)));

      if (f77_exception_encountered)
	{
	  error ("unrecoverable error in qz (dgghrd)");
	  return retval;
	}

      // check if just computing generalized eigenvalues or if we're
      // actually computing the decomposition

      // reduce to generalized Schur form
      F77_XFCN (dhgeqz, DHGEQZ,
		(F77_CONST_CHAR_ARG2 (&qz_job, 1),
		 F77_CONST_CHAR_ARG2 (&compq, 1),
		 F77_CONST_CHAR_ARG2 (&compz, 1),
		 nn, ilo, ihi, aa.fortran_vec (), nn, bb.fortran_vec (),
		 nn, alphar.fortran_vec (), alphai.fortran_vec (),
		 betar.fortran_vec (), QQ.fortran_vec (), nn,
		 ZZ.fortran_vec (), nn, work.fortran_vec (), nn, info
		 F77_CHAR_ARG_LEN (1)
		 F77_CHAR_ARG_LEN (1)
		 F77_CHAR_ARG_LEN (1)));

      if (f77_exception_encountered)
	{
	  error ("unrecoverable error in qz (dhgeqz)");
	  return retval;
	}
    }

  // order the QZ decomposition?
  if (ord_job[0] != 'N')
    {
      if (complex_case)
	{
	  // probably not needed, but better be safe
	  error ("qz: cannot re-order complex qz decomposition.");
	  return retval;
	}
      else
	{
#ifdef DEBUG_SORT
	  std::cout << "qz: ordering eigenvalues: ord_job = " << ord_job[0] << std::endl;
#endif

	  // declared static to avoid vfork/long jump compiler complaints
	  static sort_function sort_test;
	  sort_test = NULL;

	  switch (ord_job[0])
	    {
	    case 'S':
	      sort_test = &fin;
	      break;

	    case 'B':
	      sort_test = &fout;
	      break;

	    case '+':
	      sort_test = &fcrhp;
	      break;

	    case '-':
	      sort_test = &folhp;
	      break;

	    default:
	      // invalid order option (should never happen, since we
	      // checked the options at the top).
	      panic_impossible ();
	      break;
	    }

	  octave_idx_type ndim, fail;
	  double inf_norm;

	  F77_XFCN (xdlange, XDLANGE,
		    (F77_CONST_CHAR_ARG2 ("I", 1),
		     nn, nn, aa.data (), nn, work.fortran_vec (), inf_norm
		     F77_CHAR_ARG_LEN (1)));

	  double eps = DBL_EPSILON*inf_norm*nn;

#ifdef DEBUG_SORT
	  std::cout << "qz: calling dsubsp: aa=" << std::endl;
	  octave_print_internal (std::cout, aa, 0);
	  std::cout << std::endl << "bb="  << std::endl;
	  octave_print_internal (std::cout, bb, 0);
	  if (compz == 'V')
	    {
	      std::cout << std::endl << "ZZ="  << std::endl;
	      octave_print_internal (std::cout, ZZ, 0);
	    }
	  std::cout << std::endl;
	  std::cout << "alphar = " << std::endl;
	  octave_print_internal (std::cout, (Matrix) alphar, 0);
	  std::cout << std::endl << "alphai = " << std::endl;
	  octave_print_internal (std::cout, (Matrix) alphai, 0);
	  std::cout << std::endl << "beta = " << std::endl;
	  octave_print_internal (std::cout, (Matrix) betar, 0);
	  std::cout << std::endl;
#endif

	  Array<octave_idx_type> ind (nn);

	  F77_XFCN (dsubsp, DSUBSP,
		    (nn, nn, aa.fortran_vec (), bb.fortran_vec (),
		     ZZ.fortran_vec (), sort_test, eps, ndim, fail,
		     ind.fortran_vec ()));

#ifdef DEBUG
	  std::cout << "qz: back from dsubsp: aa=" << std::endl;
	  octave_print_internal (std::cout, aa, 0);
	  std::cout << std::endl << "bb="  << std::endl;
	  octave_print_internal (std::cout, bb, 0);
	  if (compz == 'V')
	    {
	      std::cout << std::endl << "ZZ="  << std::endl;
	      octave_print_internal (std::cout, ZZ, 0);
	    }
	  std::cout << std::endl;
#endif

	  // manually update alphar, alphai, betar
	  static int jj;

	  jj=0;
	  while (jj < nn)
	    {
#ifdef DEBUG_EIG
	      std::cout << "computing gen eig #" << jj << std::endl;
#endif

	      static int zcnt;	// number of zeros in this block

	      if (jj == (nn-1))
		zcnt = 1;
	      else if (aa(jj+1,jj) == 0)
		zcnt = 1;
	      else zcnt = 2;

	      if (zcnt == 1)  // real zero
		{
#ifdef DEBUG_EIG
		  std::cout << "  single gen eig:" << std::endl;
		  std::cout << "  alphar(" << jj << ") = " << aa(jj,jj) << std::endl;
		  std::cout << "  betar( " << jj << ") = " << bb(jj,jj) << std::endl;
		  std::cout << "  alphai(" << jj << ") = 0" << std::endl;
#endif

		  alphar(jj) = aa(jj,jj);
		  alphai(jj) = 0;
		  betar(jj) = bb(jj,jj);
		}
	      else
		{
		  // complex conjugate pair
#ifdef DEBUG_EIG
		  std::cout << "qz: calling dlag2:" << std::endl;
		  std::cout << "safmin="
		       << setiosflags (std::ios::scientific) << safmin << std::endl;

		  for (int idr = jj; idr <= jj+1; idr++)
		    {
		      for (int idc = jj; idc <= jj+1; idc++)
			{
			  std::cout << "aa(" << idr << "," << idc << ")="
			       << aa(idr,idc) << std::endl;
			  std::cout << "bb(" << idr << "," << idc << ")="
			       << bb(idr,idc) << std::endl;
			}
		    }
#endif

		  // FIXME -- probably should be using
		  // fortran_vec instead of &aa(jj,jj) here.

		  double scale1, scale2, wr1, wr2, wi;
		  const double *aa_ptr = aa.data () + jj*nn+jj;
		  const double *bb_ptr = bb.data () + jj*nn+jj;
		  F77_XFCN (dlag2, DLAG2,
			    (aa_ptr, nn, bb_ptr, nn, safmin,
			     scale1, scale2, wr1, wr2, wi));

#ifdef DEBUG_EIG
		  std::cout << "dlag2 returns: scale1=" << scale1
		       << "\tscale2=" << scale2 << std::endl
		       << "\twr1=" << wr1 << "\twr2=" << wr2
		       << "\twi=" << wi << std::endl;
#endif

		  // just to be safe, check if it's a real pair
		  if (wi == 0)
		    {
		      alphar(jj) = wr1;
		      alphai(jj) = 0;
		      betar(jj) = scale1;
		      alphar(jj+1) = wr2;
		      alphai(jj+1) = 0;
		      betar(jj+1) = scale2;
		    }
		  else
		    {
		      alphar(jj) = alphar(jj+1)=wr1;
		      alphai(jj) = -(alphai(jj+1) = wi);
		      betar(jj)  = betar(jj+1) = scale1;
		    }
		}

	      // advance past this block
	      jj += zcnt;
	    }

#ifdef DEBUG_SORT
	  std::cout << "qz: back from dsubsp: aa=" << std::endl;
	  octave_print_internal (std::cout, aa, 0);
	  std::cout << std::endl << "bb="  << std::endl;
	  octave_print_internal (std::cout, bb, 0);

	  if (compz == 'V')
	    {
	      std::cout << std::endl << "ZZ="  << std::endl;
	      octave_print_internal (std::cout, ZZ, 0);
	    }
	  std::cout << std::endl << "qz: ndim=" << ndim << std::endl
	       << "fail=" << fail << std::endl;
	  std::cout << "alphar = " << std::endl;
	  octave_print_internal (std::cout, (Matrix) alphar, 0);
	  std::cout << std::endl << "alphai = " << std::endl;
	  octave_print_internal (std::cout, (Matrix) alphai, 0);
	  std::cout << std::endl << "beta = " << std::endl;
	  octave_print_internal (std::cout, (Matrix) betar, 0);
	  std::cout << std::endl;
#endif
	}
    }

  // compute  generalized eigenvalues?
  ComplexColumnVector gev;

  if (nargout < 2 || nargout == 7 || (nargin == 3 && nargout == 4))
    {
      if (complex_case)
	{
	  error ("complex case not yet implemented");
	  return retval;
	}
      else
	{
#ifdef DEBUG
	  std::cout << "qz: computing generalized eigenvalues" << std::endl;
#endif

	  // return finite generalized eigenvalues
	  int cnt = 0;

	  for (int ii = 0; ii < nn; ii++)
	    if (betar(ii) != 0)
	      cnt++;

	  ComplexColumnVector tmp(cnt);

	  cnt = 0;
	  for (int ii = 0; ii < nn; ii++)
	    if (betar(ii) != 0)
	      tmp(cnt++) = Complex(alphar(ii), alphai(ii))/betar(ii);
	  gev = tmp;
	}
    }

  // right, left eigenvector matrices
  if (nargout >= 5)
    {
      char side = (nargout == 5 ? 'R' : 'B');	// which side to compute?
      char howmny = 'B';  // compute all of them and backtransform
      octave_idx_type *select = NULL; // dummy pointer; select is not used.
      octave_idx_type m;

      if (complex_case)
	{
	  error ("complex type not yet implemented");
	  return retval;
	}
      else
	{
#ifdef DEBUG
	  std::cout << "qz: computing  generalized eigenvectors" << std::endl;
#endif

	  VL = QQ;
	  VR = ZZ;

	  F77_XFCN (dtgevc, DTGEVC,
		    (F77_CONST_CHAR_ARG2 (&side, 1),
		     F77_CONST_CHAR_ARG2 (&howmny, 1),
		     select, nn, aa.fortran_vec (), nn, bb.fortran_vec (),
		     nn, VL.fortran_vec (), nn, VR.fortran_vec (), nn, nn,
		     m, work.fortran_vec (), info
		     F77_CHAR_ARG_LEN (1)
		     F77_CHAR_ARG_LEN (1)));

	  if (f77_exception_encountered)
	    {
	      error ("unrecoverable error in qz (dtgevc)");
	      return retval;
	    }

	  // now construct the complex form of VV, WW
	  int jj = 0;

	  while (jj < nn)
	    {
	      OCTAVE_QUIT;

	      // see if real or complex eigenvalue
	      int cinc = 2;	// column increment; assume complex eigenvalue

	      if (jj == (nn-1))
		cinc = 1;	// single column
	      else if (aa(jj+1,jj) == 0)
		cinc = 1;

	      // now copy the eigenvector (s) to CVR, CVL
	      if (cinc == 1)
		{
		  for (int ii = 0; ii < nn; ii++)
		    CVR(ii,jj) = VR(ii,jj);

		  if (side == 'B')
		    for (int ii = 0; ii < nn; ii++)
		      CVL(ii,jj) = VL(ii,jj);
		}
	      else
		{
		  // double column; complex vector

		  for (int ii = 0; ii < nn; ii++)
		    {
		      CVR(ii,jj) = Complex (VR(ii,jj), VR(ii,jj+1));
		      CVR(ii,jj+1) = Complex (VR(ii,jj), -VR(ii,jj+1));
		    }

		  if (side == 'B')
		    for (int ii = 0; ii < nn; ii++)
		      {
			CVL(ii,jj) = Complex (VL(ii,jj), VL(ii,jj+1));
			CVL(ii,jj+1) = Complex (VL(ii,jj), -VL(ii,jj+1));
		      }
		}

	      // advance to next eigenvectors (if any)
	      jj += cinc;
	    }
	}
    }

  switch (nargout)
    {
    case 7:
      retval(6) = gev;

    case 6:	// return eigenvectors
      retval(5) = CVL;

    case 5:	// return eigenvectors
      retval(4) = CVR;

    case 4:
      if (nargin == 3)
	{
#ifdef DEBUG
	  std::cout << "qz: sort: retval(3) = gev = " << std::endl;
	  octave_print_internal (std::cout, gev);
	  std::cout << std::endl;
#endif
	  retval(3) = gev;
	}
      else
	retval(3) = ZZ;

    case 3:
      if (nargin == 3)
	retval(2) = ZZ;
      else
	retval(2) = QQ;

    case 2:
#ifdef DEBUG
      std::cout << "qz: retval (1) = bb = " << std::endl;
      octave_print_internal (std::cout, bb, 0);
      std::cout << std::endl << "qz: retval(0) = aa = " <<std::endl;
      octave_print_internal (std::cout, aa, 0);
      std::cout << std::endl;
#endif
      retval(1) = bb;
      retval(0) = aa;
      break;

    case 1:
    case 0:
#ifdef DEBUG
      std::cout << "qz: retval(0) = gev = " << gev << std::endl;
#endif
      retval(0) = gev;
      break;

    default:
      error ("qz: too many return arguments.");
      break;
  }

#ifdef DEBUG
  std::cout << "qz: exiting (at long last)" << std::endl;
#endif

  return retval;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
