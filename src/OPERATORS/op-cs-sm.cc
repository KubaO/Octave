/*

Copyright (C) 2004 David Bateman
Copyright (C) 1998-2004 Andy Adler

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gripes.h"
#include "oct-obj.h"
#include "ov.h"
#include "ov-typeinfo.h"
#include "ov-complex.h"
#include "ops.h"
#include "xpow.h"

#include "sparse-xpow.h"
#include "sparse-xdiv.h"
#include "ov-re-sparse.h"
#include "ov-cx-sparse.h"
#include "smx-cs-sm.h"
#include "smx-sm-cs.h"

// complex by sparse matrix ops.

DEFBINOP_OP (add, complex, sparse_matrix, +)
DEFBINOP_OP (sub, complex, sparse_matrix, -)
DEFBINOP_OP (mul, complex, sparse_matrix, *)

DEFBINOP (div, complex, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_sparse_matrix&);

  ComplexMatrix m1 = ComplexMatrix (1, 1, v1.complex_value ());
  SparseMatrix m2 = v2.sparse_matrix_value ();

  return xdiv (m1, m2);
}

DEFBINOP (pow, complex, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_sparse_matrix&);
  return xpow (v1.complex_value (), v2.matrix_value ());
}

DEFBINOP (ldiv, complex, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_sparse_matrix&);

  Complex d = v1.complex_value ();
  octave_value retval;

  if (d == 0.0)
    {
      gripe_divide_by_zero ();

      retval = octave_value (v2.matrix_value () / d);
    }
  else
    retval = octave_value (v2.sparse_matrix_value () / d);

  return retval;
}

DEFBINOP_FN (lt, complex, sparse_matrix, mx_el_lt)
DEFBINOP_FN (le, complex, sparse_matrix, mx_el_le)
DEFBINOP_FN (eq, complex, sparse_matrix, mx_el_eq)
DEFBINOP_FN (ge, complex, sparse_matrix, mx_el_ge)
DEFBINOP_FN (gt, complex, sparse_matrix, mx_el_gt)
DEFBINOP_FN (ne, complex, sparse_matrix, mx_el_ne)

DEFBINOP_OP (el_mul, complex, sparse_matrix, *)
DEFBINOP_FN (el_div, complex, sparse_matrix, x_el_div)
DEFBINOP_FN (el_pow, complex, sparse_matrix, elem_xpow)

DEFBINOP (el_ldiv, complex, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_sparse_matrix&);

  Complex d = v1.complex_value ();
  octave_value retval;

  if (d == 0.0)
    {
      gripe_divide_by_zero ();

      retval = octave_value (v2.matrix_value () / d);
    }
  else
    retval = octave_value (v2.sparse_matrix_value () / d);

  return retval;
}

DEFBINOP_FN (el_and, complex, sparse_matrix, mx_el_and)
DEFBINOP_FN (el_or,  complex, sparse_matrix, mx_el_or)

DEFCATOP (cs_sm, sparse_matrix, complex)
{
  CAST_BINOP_ARGS (octave_complex&, const octave_sparse_matrix&);
  SparseComplexMatrix tmp (1, 1, v1.complex_value ());
  return octave_value (tmp. concat (v2.sparse_matrix_value (), ra_idx));
}

DEFCONV (sparse_matrix_conv, complex, sparse_matrix)
{
  CAST_CONV_ARG (const octave_complex&);

  return new octave_sparse_matrix 
    (SparseMatrix (v.matrix_value ()));
}

void
install_cs_sm_ops (void)
{
  INSTALL_BINOP (op_add, octave_complex, octave_sparse_matrix, add);
  INSTALL_BINOP (op_sub, octave_complex, octave_sparse_matrix, sub);
  INSTALL_BINOP (op_mul, octave_complex, octave_sparse_matrix, mul);
  INSTALL_BINOP (op_div, octave_complex, octave_sparse_matrix, div);
  INSTALL_BINOP (op_pow, octave_complex, octave_sparse_matrix, pow);
  INSTALL_BINOP (op_ldiv, octave_complex, octave_sparse_matrix, ldiv);
  INSTALL_BINOP (op_lt, octave_complex, octave_sparse_matrix, lt);
  INSTALL_BINOP (op_le, octave_complex, octave_sparse_matrix, le);
  INSTALL_BINOP (op_eq, octave_complex, octave_sparse_matrix, eq);
  INSTALL_BINOP (op_ge, octave_complex, octave_sparse_matrix, ge);
  INSTALL_BINOP (op_gt, octave_complex, octave_sparse_matrix, gt);
  INSTALL_BINOP (op_ne, octave_complex, octave_sparse_matrix, ne);
  INSTALL_BINOP (op_el_mul, octave_complex, octave_sparse_matrix, el_mul);
  INSTALL_BINOP (op_el_div, octave_complex, octave_sparse_matrix, el_div);
  INSTALL_BINOP (op_el_pow, octave_complex, octave_sparse_matrix, el_pow);
  INSTALL_BINOP (op_el_ldiv, octave_complex, octave_sparse_matrix, el_ldiv);
  INSTALL_BINOP (op_el_and, octave_complex, octave_sparse_matrix, el_and);
  INSTALL_BINOP (op_el_or, octave_complex, octave_sparse_matrix, el_or);

  INSTALL_CATOP (octave_complex, octave_sparse_matrix, cs_sm);

  INSTALL_ASSIGNCONV (octave_complex, octave_sparse_matrix, octave_sparse_matrix);

  INSTALL_WIDENOP (octave_complex, octave_sparse_matrix, sparse_matrix_conv);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
