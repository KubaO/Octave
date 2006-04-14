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
#include "ops.h"

#include "sparse-xdiv.h"
#include "sparse-xpow.h"
#include "ov-re-sparse.h"
#include "ov-cx-sparse.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gripes.h"
#include "oct-obj.h"
#include "ov.h"
#include "ov-cx-mat.h"
#include "ov-typeinfo.h"
#include "ops.h"
#include "xdiv.h"
#include "xpow.h"

// unary sparse complex matrix ops.

DEFUNOP_OP (not, sparse_complex_matrix, !)
DEFUNOP_OP (uplus, sparse_complex_matrix, /* no-op */)
DEFUNOP_OP (uminus, sparse_complex_matrix, -)

DEFUNOP (transpose, sparse_complex_matrix)
{
  CAST_UNOP_ARG (const octave_sparse_complex_matrix&);
  return octave_value 
    (v.sparse_complex_matrix_value().transpose (),
     v.sparse_type ().transpose ());
}

DEFUNOP (hermitian, sparse_complex_matrix)
{
  CAST_UNOP_ARG (const octave_sparse_complex_matrix&);
  return octave_value 
    (v.sparse_complex_matrix_value().hermitian (),
     v.sparse_type ().transpose ());
}

#if 0
DEFUNOP (incr, sparse_complex_matrix)
{
  CAST_UNOP_ARG (const octave_sparse_complex_matrix&);

  return octave_value (v.complex_matrix_value () .increment ());
}

DEFUNOP (decr, sparse_complex_matrix)
{
  CAST_UNOP_ARG (const octave_sparse_complex_matrix&);

  return octave_value (v.complex_matrix_value () .decrement ());
}
#endif

// complex matrix by complex matrix ops.

DEFBINOP_OP (add, sparse_complex_matrix, sparse_complex_matrix, +)
DEFBINOP_OP (sub, sparse_complex_matrix, sparse_complex_matrix, -)

DEFBINOP_OP (mul, sparse_complex_matrix, sparse_complex_matrix, *)

DEFBINOP (div, sparse_complex_matrix, sparse_complex_matrix)
{
  CAST_BINOP_ARGS (const octave_sparse_complex_matrix&, 
		   const octave_sparse_complex_matrix&);
  SparseType typ = v2.sparse_type ();
  SparseComplexMatrix ret = xdiv (v1.sparse_complex_matrix_value (), 
				  v2.sparse_complex_matrix_value (), typ);
  
  v2.sparse_type (typ);
  return ret;
}

DEFBINOPX (pow, sparse_complex_matrix, sparse_complex_matrix)
{
  error ("can't do A ^ B for A and B both matrices");
  return octave_value ();
}

DEFBINOP (ldiv, sparse_complex_matrix, sparse_complex_matrix)
{
  CAST_BINOP_ARGS (const octave_sparse_complex_matrix&, 
		   const octave_sparse_complex_matrix&);
  SparseType typ = v1.sparse_type ();

  SparseComplexMatrix ret = xleftdiv (v1.sparse_complex_matrix_value (), 
				      v2.sparse_complex_matrix_value (), typ);

  v1.sparse_type (typ);
  return ret;
}

DEFBINOP_FN (lt, sparse_complex_matrix, sparse_complex_matrix, mx_el_lt)
DEFBINOP_FN (le, sparse_complex_matrix, sparse_complex_matrix, mx_el_le)
DEFBINOP_FN (eq, sparse_complex_matrix, sparse_complex_matrix, mx_el_eq)
DEFBINOP_FN (ge, sparse_complex_matrix, sparse_complex_matrix, mx_el_ge)
DEFBINOP_FN (gt, sparse_complex_matrix, sparse_complex_matrix, mx_el_gt)
DEFBINOP_FN (ne, sparse_complex_matrix, sparse_complex_matrix, mx_el_ne)

DEFBINOP_FN (el_mul, sparse_complex_matrix, sparse_complex_matrix, product)
DEFBINOP_FN (el_div, sparse_complex_matrix, sparse_complex_matrix, quotient)
DEFBINOP_FN (el_pow, sparse_complex_matrix, sparse_complex_matrix, elem_xpow)

DEFBINOP (el_ldiv, sparse_complex_matrix, sparse_complex_matrix)
{
  CAST_BINOP_ARGS (const octave_sparse_complex_matrix&, 
		   const octave_sparse_complex_matrix&);

  return octave_value (quotient (v2.sparse_complex_matrix_value (), 
				 v1.sparse_complex_matrix_value ()));
}

DEFBINOP_FN (el_and, sparse_complex_matrix, sparse_complex_matrix, mx_el_and)
DEFBINOP_FN (el_or,  sparse_complex_matrix, sparse_complex_matrix, mx_el_or)

DEFCATOP_FN (scm_scm, sparse_complex_matrix, sparse_complex_matrix, concat)

DEFASSIGNOP_FN (assign, sparse_complex_matrix, sparse_complex_matrix, assign)

void
install_scm_scm_ops (void)
{
  INSTALL_UNOP (op_not, octave_sparse_complex_matrix, not);
  INSTALL_UNOP (op_uplus, octave_sparse_complex_matrix, uplus);
  INSTALL_UNOP (op_uminus, octave_sparse_complex_matrix, uminus);
  INSTALL_UNOP (op_transpose, octave_sparse_complex_matrix, transpose);
  INSTALL_UNOP (op_hermitian, octave_sparse_complex_matrix, hermitian);

#if 0
  INSTALL_NCUNOP (op_incr, octave_sparse_complex_matrix, incr);
  INSTALL_NCUNOP (op_decr, octave_sparse_complex_matrix, decr);
#endif

  INSTALL_BINOP (op_add, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, add);
  INSTALL_BINOP (op_sub, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, sub);
  INSTALL_BINOP (op_mul, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, mul);
  INSTALL_BINOP (op_div, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, div);
  INSTALL_BINOP (op_pow, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, pow);
  INSTALL_BINOP (op_ldiv, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, ldiv);
  INSTALL_BINOP (op_lt, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, lt);
  INSTALL_BINOP (op_le, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, le);
  INSTALL_BINOP (op_eq, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, eq);
  INSTALL_BINOP (op_ge, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, ge);
  INSTALL_BINOP (op_gt, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, gt);
  INSTALL_BINOP (op_ne, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, ne);
  INSTALL_BINOP (op_el_mul, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, el_mul);
  INSTALL_BINOP (op_el_div, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, el_div);
  INSTALL_BINOP (op_el_pow, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, el_pow);
  INSTALL_BINOP (op_el_ldiv, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, el_ldiv);
  INSTALL_BINOP (op_el_and, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, el_and);
  INSTALL_BINOP (op_el_or, octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, el_or);

  INSTALL_CATOP (octave_sparse_complex_matrix, 
		 octave_sparse_complex_matrix, scm_scm);

  INSTALL_ASSIGNOP (op_asn_eq, octave_sparse_complex_matrix, 
		    octave_sparse_complex_matrix, assign);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
