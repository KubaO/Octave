/*

Copyright (C) 1996, 1997 John W. Eaton

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

#if defined (__GNUG__)
#pragma implementation
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gripes.h"
#include "ov.h"
#include "ov-re-mat.h"
#include "ov-typeinfo.h"
#include "ops.h"
#include "xdiv.h"
#include "xpow.h"

// matrix by matrix ops.

DEFBINOP_OP (add, matrix, matrix, +)
DEFBINOP_OP (sub, matrix, matrix, -)
DEFBINOP_OP (mul, matrix, matrix, *)

DEFBINOP (div, matrix, matrix)
{
  CAST_BINOP_ARGS (const octave_matrix&, const octave_matrix&);

  return xdiv (v1.matrix_value (), v2.matrix_value ());
}

DEFBINOPX (pow, matrix, matrix)
{
  error ("can't do A ^ B for A and B both matrices");
  return octave_value ();
}

DEFBINOP_FN (ldiv, matrix, matrix, xleftdiv)

DEFBINOP_FN (lt, matrix, matrix, mx_el_lt)
DEFBINOP_FN (le, matrix, matrix, mx_el_le)
DEFBINOP_FN (eq, matrix, matrix, mx_el_eq)
DEFBINOP_FN (ge, matrix, matrix, mx_el_ge)
DEFBINOP_FN (gt, matrix, matrix, mx_el_gt)
DEFBINOP_FN (ne, matrix, matrix, mx_el_ne)

DEFBINOP_FN (el_mul, matrix, matrix, product)
DEFBINOP_FN (el_div, matrix, matrix, quotient)
DEFBINOP_FN (el_pow, matrix, matrix, elem_xpow)

DEFBINOP (el_ldiv, matrix, matrix)
{
  CAST_BINOP_ARGS (const octave_matrix&, const octave_matrix&);

  return octave_value (quotient (v2.matrix_value (), v1.matrix_value ()));
}

DEFBINOP_FN (el_and, matrix, matrix, mx_el_and)
DEFBINOP_FN (el_or, matrix, matrix, mx_el_or)

DEFASSIGNOP_FN (assign, matrix, matrix, assign)

void
install_m_m_ops (void)
{
  INSTALL_BINOP (add, octave_matrix, octave_matrix, add);
  INSTALL_BINOP (sub, octave_matrix, octave_matrix, sub);
  INSTALL_BINOP (mul, octave_matrix, octave_matrix, mul);
  INSTALL_BINOP (div, octave_matrix, octave_matrix, div);
  INSTALL_BINOP (pow, octave_matrix, octave_matrix, pow);
  INSTALL_BINOP (ldiv, octave_matrix, octave_matrix, ldiv);
  INSTALL_BINOP (lt, octave_matrix, octave_matrix, lt);
  INSTALL_BINOP (le, octave_matrix, octave_matrix, le);
  INSTALL_BINOP (eq, octave_matrix, octave_matrix, eq);
  INSTALL_BINOP (ge, octave_matrix, octave_matrix, ge);
  INSTALL_BINOP (gt, octave_matrix, octave_matrix, gt);
  INSTALL_BINOP (ne, octave_matrix, octave_matrix, ne);
  INSTALL_BINOP (el_mul, octave_matrix, octave_matrix, el_mul);
  INSTALL_BINOP (el_div, octave_matrix, octave_matrix, el_div);
  INSTALL_BINOP (el_pow, octave_matrix, octave_matrix, el_pow);
  INSTALL_BINOP (el_ldiv, octave_matrix, octave_matrix, el_ldiv);
  INSTALL_BINOP (el_and, octave_matrix, octave_matrix, el_and);
  INSTALL_BINOP (el_or, octave_matrix, octave_matrix, el_or);

  INSTALL_ASSIGNOP (asn_eq, octave_matrix, octave_matrix, assign);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
