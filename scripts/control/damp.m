# Copyright (C) 1993, 1994, 1995 John W. Eaton
# 
# This file is part of Octave.
# 
# Octave is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2, or (at your option) any
# later version.
# 
# Octave is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Octave; see the file COPYING.  If not, write to the Free
# Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

function damp(p, tsam)
# Usage: damp(p[, tsam])
#      Displays eigenvalues, natural frequencies and damping ratios
#      of the eigenvalues of a matrix p or the A-matrix of a
#      system p, respectively.
#      If p is a system, tsam must not be specified.
#      If p is a matrix and tsam is specified, eigenvalues
#      of p are assumed to be in z-domain.
#
# See also: eig

# Written by Kai P. Mueller September 29, 1997.
# Update
# $Revision: 1.1.1.1 $
# $Log: damp.m,v $
# Revision 1.1.1.1  1998/05/19 20:24:06  jwe
#
# Revision 1.3  1997/12/01 16:51:50  scotte
# updated by Mueller 27 Nov 97
#
# Revision 1.1  1997/11/11  17:32:13  mueller
# Initial revision
#

  # assume a continuous system
  DIGITAL = 0;
  if(nargin < 1 || nargin > 2)
    usage("damp(p,[ tsamp])")
  endif
  if(is_struct(p))
    if (nargin != 1)
      error("damp: when p is a system, tsamp parameter is not allowed.");
    endif
    [aa, b, c, d, t_samp] = sys2ss(p);
    DIGITAL = is_digital(p);
  else
    aa = p;
    if (nargin == 2)
        DIGITAL = 1;
        t_samp = tsam;
    endif
  endif
  if (!is_square(aa))
    error("damp: Matrix p is not square.")
  endif
  if (DIGITAL && t_samp <= 0.0)
    error("damp: Sampling time tsam must not be <= 0.")
  endif

  # all checks done.
  e = eig(aa);
  [n, m] = size(aa);
  if (DIGITAL)
    printf("  (discrete system with sampling time %f)\n", t_samp);
  endif
  printf("............... Eigenvalue ...........     Damping     Frequency\n");
  printf("--------[re]---------[im]--------[abs]----------------------[Hz]\n");
  for i = 1:n
    pole = e(i);
    cpole = pole;
    if (DIGITAL)
      cpole = log(pole) / t_samp;
    endif
    d0 = -cos(atan2(imag(cpole), real(cpole)));
    f0 = 0.5 / pi * abs(cpole);
    if (abs(imag(cpole)) < eps)
      printf("%12f         ---  %12f  %10f  %12f\n",
             real(pole), abs(pole), d0, f0);
    else
      printf("%12f %12f %12f  %10f  %12f\n",
             real(pole), imag(pole), abs(pole), d0, f0);
    endif
  endfor

endfunction
