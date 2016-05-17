/*

Copyright (C) 1994-2015 John W. Eaton

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

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <string>
#include <vector>

#include "Array-util.h"
#include "CMatrix.h"
#include "dMatrix.h"
#include "f77-fcn.h"
#include "fCMatrix.h"
#include "fMatrix.h"
#include "gepbalance.h"
#include "oct-locbuf.h"

extern "C"
{
  F77_RET_T
  F77_FUNC (dggbal, DGGBAL) (F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type& N,
                             double* A, const octave_idx_type& LDA,
                             double* B, const octave_idx_type& LDB,
                             octave_idx_type& ILO, octave_idx_type& IHI,
                             double* LSCALE, double* RSCALE,
                             double* WORK, octave_idx_type& INFO
                             F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (dggbak, DGGBAK) (F77_CONST_CHAR_ARG_DECL,
                             F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type& N,
                             const octave_idx_type& ILO,
                             const octave_idx_type& IHI,
                             const double* LSCALE, const double* RSCALE,
                             octave_idx_type& M, double* V,
                             const octave_idx_type& LDV, octave_idx_type& INFO
                             F77_CHAR_ARG_LEN_DECL
                             F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (sggbal, SGGBAL) (F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type& N, float* A,
                             const octave_idx_type& LDA, float* B,
                             const octave_idx_type& LDB,
                             octave_idx_type& ILO, octave_idx_type& IHI,
                             float* LSCALE, float* RSCALE,
                             float* WORK, octave_idx_type& INFO
                             F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (sggbak, SGGBAK) (F77_CONST_CHAR_ARG_DECL,
                             F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type& N,
                             const octave_idx_type& ILO,
                             const octave_idx_type& IHI,
                             const float* LSCALE, const float* RSCALE,
                             octave_idx_type& M, float* V,
                             const octave_idx_type& LDV, octave_idx_type& INFO
                             F77_CHAR_ARG_LEN_DECL
                             F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (zggbal, ZGGBAL) (F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type& N, Complex* A,
                             const octave_idx_type& LDA, Complex* B,
                             const octave_idx_type& LDB,
                             octave_idx_type& ILO, octave_idx_type& IHI,
                             double* LSCALE, double* RSCALE,
                             double* WORK, octave_idx_type& INFO
                             F77_CHAR_ARG_LEN_DECL);

  F77_RET_T
  F77_FUNC (cggbal, CGGBAL) (F77_CONST_CHAR_ARG_DECL,
                             const octave_idx_type& N,
                             FloatComplex* A, const octave_idx_type& LDA,
                             FloatComplex* B, const octave_idx_type& LDB,
                             octave_idx_type& ILO, octave_idx_type& IHI,
                             float* LSCALE, float* RSCALE,
                             float* WORK, octave_idx_type& INFO
                             F77_CHAR_ARG_LEN_DECL);
}

template <>
octave_idx_type
gepbalance<Matrix>::init (const Matrix& a, const Matrix& b,
                          const std::string& balance_job)
{
  octave_idx_type n = a.cols ();

  if (a.rows () != n)
    (*current_liboctave_error_handler) ("GEPBALANCE requires square matrix");

  if (a.dims () != b.dims ())
    err_nonconformant ("GEPBALANCE", n, n, b.rows(), b.cols());

  octave_idx_type info;
  octave_idx_type ilo;
  octave_idx_type ihi;

  OCTAVE_LOCAL_BUFFER (double, plscale, n);
  OCTAVE_LOCAL_BUFFER (double, prscale, n);
  OCTAVE_LOCAL_BUFFER (double, pwork, 6 * n);

  balanced_mat = a;
  double *p_balanced_mat = balanced_mat.fortran_vec ();
  balanced_mat2 = b;
  double *p_balanced_mat2 = balanced_mat2.fortran_vec ();

  char job = balance_job[0];

  F77_XFCN (dggbal, DGGBAL, (F77_CONST_CHAR_ARG2 (&job, 1),
                             n, p_balanced_mat, n, p_balanced_mat2,
                             n, ilo, ihi, plscale, prscale, pwork, info
                             F77_CHAR_ARG_LEN  (1)));

  balancing_mat = Matrix (n, n, 0.0);
  balancing_mat2 = Matrix (n, n, 0.0);
  for (octave_idx_type i = 0; i < n; i++)
    {
      octave_quit ();
      balancing_mat.elem (i ,i) = 1.0;
      balancing_mat2.elem (i ,i) = 1.0;
    }

  double *p_balancing_mat = balancing_mat.fortran_vec ();
  double *p_balancing_mat2 = balancing_mat2.fortran_vec ();

  // first left
  F77_XFCN (dggbak, DGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("L", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  // then right
  F77_XFCN (dggbak, DGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("R", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat2, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  return info;
}

template <>
octave_idx_type
gepbalance<FloatMatrix>::init (const FloatMatrix& a, const FloatMatrix& b,
                               const std::string& balance_job)
{
  octave_idx_type n = a.cols ();

  if (a.rows () != n)
    (*current_liboctave_error_handler)
      ("FloatGEPBALANCE requires square matrix");

  if (a.dims () != b.dims ())
    err_nonconformant ("FloatGEPBALANCE", n, n, b.rows(), b.cols());

  octave_idx_type info;
  octave_idx_type ilo;
  octave_idx_type ihi;

  OCTAVE_LOCAL_BUFFER (float, plscale, n);
  OCTAVE_LOCAL_BUFFER (float, prscale, n);
  OCTAVE_LOCAL_BUFFER (float, pwork, 6 * n);

  balanced_mat = a;
  float *p_balanced_mat = balanced_mat.fortran_vec ();
  balanced_mat2 = b;
  float *p_balanced_mat2 = balanced_mat2.fortran_vec ();

  char job = balance_job[0];

  F77_XFCN (sggbal, SGGBAL, (F77_CONST_CHAR_ARG2 (&job, 1),
                             n, p_balanced_mat, n, p_balanced_mat2,
                             n, ilo, ihi, plscale, prscale, pwork, info
                             F77_CHAR_ARG_LEN  (1)));

  balancing_mat = FloatMatrix (n, n, 0.0);
  balancing_mat2 = FloatMatrix (n, n, 0.0);
  for (octave_idx_type i = 0; i < n; i++)
    {
      octave_quit ();
      balancing_mat.elem (i ,i) = 1.0;
      balancing_mat2.elem (i ,i) = 1.0;
    }

  float *p_balancing_mat = balancing_mat.fortran_vec ();
  float *p_balancing_mat2 = balancing_mat2.fortran_vec ();

  // first left
  F77_XFCN (sggbak, SGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("L", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  // then right
  F77_XFCN (sggbak, SGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("R", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat2, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  return info;
}

template <>
octave_idx_type
gepbalance<ComplexMatrix>::init (const ComplexMatrix& a,
                                 const ComplexMatrix& b,
                                 const std::string& balance_job)
{
  octave_idx_type n = a.cols ();

  if (a.rows () != n)
    (*current_liboctave_error_handler)
      ("ComplexGEPBALANCE requires square matrix");

  if (a.dims () != b.dims ())
    err_nonconformant ("ComplexGEPBALANCE", n, n, b.rows(), b.cols());

  octave_idx_type info;
  octave_idx_type ilo;
  octave_idx_type ihi;

  OCTAVE_LOCAL_BUFFER (double, plscale, n);
  OCTAVE_LOCAL_BUFFER (double, prscale,  n);
  OCTAVE_LOCAL_BUFFER (double, pwork, 6 * n);

  balanced_mat = a;
  Complex *p_balanced_mat = balanced_mat.fortran_vec ();
  balanced_mat2 = b;
  Complex *p_balanced_mat2 = balanced_mat2.fortran_vec ();

  char job = balance_job[0];

  F77_XFCN (zggbal, ZGGBAL, (F77_CONST_CHAR_ARG2 (&job, 1),
                             n, p_balanced_mat, n, p_balanced_mat2,
                             n, ilo, ihi, plscale, prscale, pwork, info
                             F77_CHAR_ARG_LEN (1)));

  balancing_mat = Matrix (n, n, 0.0);
  balancing_mat2 = Matrix (n, n, 0.0);
  for (octave_idx_type i = 0; i < n; i++)
    {
      octave_quit ();
      balancing_mat.elem (i ,i) = 1.0;
      balancing_mat2.elem (i ,i) = 1.0;
    }

  double *p_balancing_mat = balancing_mat.fortran_vec ();
  double *p_balancing_mat2 = balancing_mat2.fortran_vec ();

  // first left
  F77_XFCN (dggbak, DGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("L", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  // then right
  F77_XFCN (dggbak, DGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("R", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat2, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  return info;
}

template <>
octave_idx_type
gepbalance<FloatComplexMatrix>::init (const FloatComplexMatrix& a,
                                      const FloatComplexMatrix& b,
                                      const std::string& balance_job)
{
  octave_idx_type n = a.cols ();

  if (a.rows () != n)
    {
      (*current_liboctave_error_handler)
        ("FloatComplexGEPBALANCE requires square matrix");
      return -1;
    }

  if (a.dims () != b.dims ())
    err_nonconformant ("FloatComplexGEPBALANCE", n, n, b.rows(), b.cols());

  octave_idx_type info;
  octave_idx_type ilo;
  octave_idx_type ihi;

  OCTAVE_LOCAL_BUFFER (float, plscale, n);
  OCTAVE_LOCAL_BUFFER (float, prscale, n);
  OCTAVE_LOCAL_BUFFER (float, pwork, 6 * n);

  balanced_mat = a;
  FloatComplex *p_balanced_mat = balanced_mat.fortran_vec ();
  balanced_mat2 = b;
  FloatComplex *p_balanced_mat2 = balanced_mat2.fortran_vec ();

  char job = balance_job[0];

  F77_XFCN (cggbal, CGGBAL, (F77_CONST_CHAR_ARG2 (&job, 1),
                             n, p_balanced_mat, n, p_balanced_mat2,
                             n, ilo, ihi, plscale,prscale, pwork, info
                             F77_CHAR_ARG_LEN (1)));

  balancing_mat = FloatMatrix (n, n, 0.0);
  balancing_mat2 = FloatMatrix (n, n, 0.0);
  for (octave_idx_type i = 0; i < n; i++)
    {
      octave_quit ();
      balancing_mat.elem (i ,i) = 1.0;
      balancing_mat2.elem (i ,i) = 1.0;
    }

  float *p_balancing_mat = balancing_mat.fortran_vec ();
  float *p_balancing_mat2 = balancing_mat2.fortran_vec ();

  // first left
  F77_XFCN (sggbak, SGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("L", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  // then right
  F77_XFCN (sggbak, SGGBAK, (F77_CONST_CHAR_ARG2 (&job, 1),
                             F77_CONST_CHAR_ARG2 ("R", 1),
                             n, ilo, ihi, plscale, prscale,
                             n, p_balancing_mat2, n, info
                             F77_CHAR_ARG_LEN (1)
                             F77_CHAR_ARG_LEN (1)));

  return info;
}

// Instantiations we need.

template class gepbalance<Matrix>;

template class gepbalance<FloatMatrix>;

template class gepbalance<ComplexMatrix>;

template class gepbalance<FloatComplexMatrix>;
