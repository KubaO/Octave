/*

Copyright (C) 2004 John W. Eaton

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

#if defined (__GNUG__) && defined (USE_PRAGMA_INTERFACE_IMPLEMENTATION)
#pragma implementation
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gripes.h"
#include "oct-obj.h"
#include "ov.h"
#include "ov-int8.h"
#include "ov-int16.h"
#include "ov-int32.h"
#include "ov-int64.h"
#include "ov-uint8.h"
#include "ov-uint16.h"
#include "ov-uint32.h"
#include "ov-uint64.h"
#include "ov-bool.h"
#include "ov-bool-mat.h"
#include "ov-scalar.h"
#include "ov-re-mat.h"
#include "ov-typeinfo.h"
#include "ops.h"

// conversion ops

DEFDBLCONVFN (int8_matrix_to_double_matrix, int8_matrix, int8_array)
DEFDBLCONVFN (int16_matrix_to_double_matrix, int16_matrix, int16_array)
DEFDBLCONVFN (int32_matrix_to_double_matrix, int32_matrix, int32_array)
DEFDBLCONVFN (int64_matrix_to_double_matrix, int64_matrix, int64_array)

DEFDBLCONVFN (uint8_matrix_to_double_matrix, uint8_matrix, uint8_array)
DEFDBLCONVFN (uint16_matrix_to_double_matrix, uint16_matrix, uint16_array)
DEFDBLCONVFN (uint32_matrix_to_double_matrix, uint32_matrix, uint32_array)
DEFDBLCONVFN (uint64_matrix_to_double_matrix, uint64_matrix, uint64_array)

DEFDBLCONVFN (int8_scalar_to_double_matrix, int8_scalar, int8_array)
DEFDBLCONVFN (int16_scalar_to_double_matrix, int16_scalar, int16_array)
DEFDBLCONVFN (int32_scalar_to_double_matrix, int32_scalar, int32_array)
DEFDBLCONVFN (int64_scalar_to_double_matrix, int64_scalar, int64_array)

DEFDBLCONVFN (uint8_scalar_to_double_matrix, uint8_scalar, uint8_array)
DEFDBLCONVFN (uint16_scalar_to_double_matrix, uint16_scalar, uint16_array)
DEFDBLCONVFN (uint32_scalar_to_double_matrix, uint32_scalar, uint32_array)
DEFDBLCONVFN (uint64_scalar_to_double_matrix, uint64_scalar, uint64_array)

DEFDBLCONVFN (bool_matrix_to_double_matrix, bool_matrix, bool_array)
DEFDBLCONVFN (bool_scalar_to_double_matrix, bool, bool_array)

DEFDBLCONVFN (double_scalar_to_double_matrix, scalar, array)

void
install_double_conv_ops (void)
{
  INSTALL_CONVOP (octave_int8_matrix, octave_matrix, int8_matrix_to_double_matrix);
  INSTALL_CONVOP (octave_int16_matrix, octave_matrix, int16_matrix_to_double_matrix);
  INSTALL_CONVOP (octave_int32_matrix, octave_matrix, int32_matrix_to_double_matrix);
  INSTALL_CONVOP (octave_int64_matrix, octave_matrix, int64_matrix_to_double_matrix);

  INSTALL_CONVOP (octave_uint8_matrix, octave_matrix, uint8_matrix_to_double_matrix);
  INSTALL_CONVOP (octave_uint16_matrix, octave_matrix, uint16_matrix_to_double_matrix);
  INSTALL_CONVOP (octave_uint32_matrix, octave_matrix, uint32_matrix_to_double_matrix);
  INSTALL_CONVOP (octave_uint64_matrix, octave_matrix, uint64_matrix_to_double_matrix);

  INSTALL_CONVOP (octave_int8_scalar, octave_matrix, int8_scalar_to_double_matrix);
  INSTALL_CONVOP (octave_int16_scalar, octave_matrix, int16_scalar_to_double_matrix);
  INSTALL_CONVOP (octave_int32_scalar, octave_matrix, int32_scalar_to_double_matrix);
  INSTALL_CONVOP (octave_int64_scalar, octave_matrix, int64_scalar_to_double_matrix);

  INSTALL_CONVOP (octave_uint8_scalar, octave_matrix, uint8_scalar_to_double_matrix);
  INSTALL_CONVOP (octave_uint16_scalar, octave_matrix, uint16_scalar_to_double_matrix);
  INSTALL_CONVOP (octave_uint32_scalar, octave_matrix, uint32_scalar_to_double_matrix);
  INSTALL_CONVOP (octave_uint64_scalar, octave_matrix, uint64_scalar_to_double_matrix);

  INSTALL_CONVOP (octave_bool_matrix, octave_matrix, bool_matrix_to_double_matrix);
  INSTALL_CONVOP (octave_bool, octave_matrix, bool_scalar_to_double_matrix);

  INSTALL_CONVOP (octave_scalar, octave_matrix, double_scalar_to_double_matrix);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
p*/
