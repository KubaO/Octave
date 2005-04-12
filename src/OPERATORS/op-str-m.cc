/*

Copyright (C) 2003 John W. Eaton

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gripes.h"
#include "oct-obj.h"
#include "ov.h"
#include "ov-re-mat.h"
#include "ov-str-mat.h"
#include "ov-typeinfo.h"
#include "ops.h"

DEFASSIGNOP (assign, char_matrix_str, octave_matrix)
{
  CAST_BINOP_ARGS (octave_char_matrix_str&, const octave_matrix&);

  octave_value tmp
    = v2.convert_to_str_internal (false, false,
				  a1.is_sq_string () ? '\'' : '"');

  if (! error_state)
    v1.assign (idx, tmp.char_matrix_value ());

  return octave_value ();
}

DEFCATOP (str_m, char_matrix_str, matrix)
{
  CAST_BINOP_ARGS (octave_char_matrix_str&, const octave_matrix&);

  if (Vwarn_num_to_str)
    gripe_implicit_conversion (v2.type_name (), v1.type_name ());

  return octave_value (v1.char_array_value (). concat (v2.array_value (), 
						       ra_idx),
		       true, a1.is_sq_string () ? '\'' : '"');
}

DEFCATOP (m_str, matrix, char_matrix_str)
{
  CAST_BINOP_ARGS (octave_matrix&, const octave_char_matrix_str&);

  if (Vwarn_num_to_str)
    gripe_implicit_conversion (v1.type_name (), v2.type_name ());

  return octave_value (v1.array_value (). concat (v2.char_array_value (), 
						  ra_idx),
		       true, a2.is_sq_string () ? '\'' : '"');
}

void
install_str_m_ops (void)
{
  INSTALL_ASSIGNOP (op_asn_eq, octave_char_matrix_str, octave_matrix, assign);
  INSTALL_ASSIGNOP (op_asn_eq, octave_char_matrix_sq_str, octave_matrix, assign);

  INSTALL_CATOP (octave_char_matrix_str, octave_matrix, str_m);
  INSTALL_CATOP (octave_char_matrix_sq_str, octave_matrix, str_m);

  INSTALL_CATOP (octave_matrix, octave_char_matrix_str, m_str);
  INSTALL_CATOP (octave_matrix, octave_char_matrix_sq_str, m_str);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
