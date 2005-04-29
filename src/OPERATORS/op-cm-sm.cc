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
#include "ov-cx-mat.h"
#include "ops.h"
#include "xdiv.h"

#include "sparse-xpow.h"
#include "sparse-xdiv.h"
#include "smx-sm-cm.h"
#include "smx-cm-sm.h"
#include "ov-re-sparse.h"

// complex matrix by sparse matrix ops.

DEFBINOP_OP (add, complex_matrix, sparse_matrix, +)
DEFBINOP_OP (sub, complex_matrix, sparse_matrix, -)

DEFBINOP (mul, complex_matrix, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex_matrix&, 
		   const octave_sparse_matrix&);
  
  Matrix tmp (v2.matrix_value ());

  return octave_value (v1.complex_matrix_value() * tmp);
}

DEFBINOP (div, complex_matrix, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex_matrix&, octave_sparse_matrix&);
  
  SparseType typ = v2.sparse_type ();

  ComplexMatrix ret = xdiv (v1.complex_matrix_value (), 
			    v2.sparse_matrix_value (), typ);

  v2.sparse_type (typ);
  return ret;
}

DEFBINOPX (pow, complex_matrix, sparse_matrix)
{
  error ("can't do A ^ B for A and B both matrices");
  return octave_value ();
}

DEFBINOP (ldiv, complex_matrix, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex_matrix&, 
		   const octave_sparse_matrix&);
  
  return xleftdiv (v1.complex_matrix_value (), v2.matrix_value ());
}

DEFBINOP_FN (lt, complex_matrix, sparse_matrix, mx_el_lt)
DEFBINOP_FN (le, complex_matrix, sparse_matrix, mx_el_le)
DEFBINOP_FN (eq, complex_matrix, sparse_matrix, mx_el_eq)
DEFBINOP_FN (ge, complex_matrix, sparse_matrix, mx_el_ge)
DEFBINOP_FN (gt, complex_matrix, sparse_matrix, mx_el_gt)
DEFBINOP_FN (ne, complex_matrix, sparse_matrix, mx_el_ne)

DEFBINOP_FN (el_mul, complex_matrix, sparse_matrix, product)
DEFBINOP_FN (el_div, complex_matrix, sparse_matrix, quotient)

DEFBINOP (el_pow, complex_matrix, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex_matrix&, 
		   const octave_sparse_matrix&);
  
  return octave_value 
    (elem_xpow ( SparseComplexMatrix (v1.complex_matrix_value ()),
		 v2.sparse_matrix_value ()));
}

DEFBINOP (el_ldiv, complex_matrix, sparse_matrix)
{
  CAST_BINOP_ARGS (const octave_complex_matrix&, 
		   const octave_sparse_matrix&);
  return octave_value
    (quotient (v2.sparse_matrix_value (), v1.complex_matrix_value ()));
}

DEFBINOP_FN (el_and, complex_matrix, sparse_matrix, mx_el_and)
DEFBINOP_FN (el_or,  complex_matrix, sparse_matrix, mx_el_or)

DEFCATOP (cm_sm, complex_matrix, sparse_matrix)
{
  CAST_BINOP_ARGS (octave_complex_matrix&, const octave_sparse_matrix&);
  SparseComplexMatrix tmp (v1.complex_matrix_value ());
  return octave_value (tmp. concat (v2.sparse_matrix_value (), ra_idx));
}

void
install_cm_sm_ops (void)
{
  INSTALL_BINOP (op_add, octave_complex_matrix, octave_sparse_matrix, add);
  INSTALL_BINOP (op_sub, octave_complex_matrix, octave_sparse_matrix, sub);
  INSTALL_BINOP (op_mul, octave_complex_matrix, octave_sparse_matrix, mul);
  INSTALL_BINOP (op_div, octave_complex_matrix, octave_sparse_matrix, div);
  INSTALL_BINOP (op_pow, octave_complex_matrix, octave_sparse_matrix, pow);
  INSTALL_BINOP (op_ldiv, octave_complex_matrix, octave_sparse_matrix, ldiv);
  INSTALL_BINOP (op_lt, octave_complex_matrix, octave_sparse_matrix, lt);
  INSTALL_BINOP (op_le, octave_complex_matrix, octave_sparse_matrix, le);
  INSTALL_BINOP (op_eq, octave_complex_matrix, octave_sparse_matrix, eq);
  INSTALL_BINOP (op_ge, octave_complex_matrix, octave_sparse_matrix, ge);
  INSTALL_BINOP (op_gt, octave_complex_matrix, octave_sparse_matrix, gt);
  INSTALL_BINOP (op_ne, octave_complex_matrix, octave_sparse_matrix, ne);
  INSTALL_BINOP (op_el_mul, octave_complex_matrix, octave_sparse_matrix, 
		 el_mul);
  INSTALL_BINOP (op_el_div, octave_complex_matrix, octave_sparse_matrix, 
		 el_div);
  INSTALL_BINOP (op_el_pow, octave_complex_matrix, octave_sparse_matrix, 
		 el_pow);
  INSTALL_BINOP (op_el_ldiv, octave_complex_matrix, octave_sparse_matrix, 
		 el_ldiv);
  INSTALL_BINOP (op_el_and, octave_complex_matrix, octave_sparse_matrix, 
		 el_and);
  INSTALL_BINOP (op_el_or, octave_complex_matrix, octave_sparse_matrix, 
		 el_or);

  INSTALL_CATOP (octave_complex_matrix, octave_sparse_matrix, cm_sm);

}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
