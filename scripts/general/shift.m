## Copyright (C) 1995, 1996  Kurt Hornik
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, write to the Free
## Software Foundation, 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.

## -*- texinfo -*-
## @deftypefn {Function File} {} shift (@var{x}, @var{b})
## @deftypefnx {Function File} {} shift (@var{x}, @var{b}, @var{dim})
## If @var{x} is a vector, perform a circular shift of length @var{b} of
## the elements of @var{x}.
##
## If @var{x} is a matrix, do the same for each column of @var{x}.
## If the optional @var{dim} argument is given, operate along this 
## dimension
## @end deftypefn

## Author: AW <Andreas.Weingessel@ci.tuwien.ac.at>
## Created: 14 September 1994
## Adapted-By: jwe

function y = shift (x, b, dim)

  if (nargin != 2 && nargin != 3)
    usage ("shift (X, b, dim)");
  endif

  if (! (isscalar (b) && b == round (b)))
    error ("shift: b must be an integer");
  endif

  nd = ndims (x);
  sz = size (x);

  if (nargin == 3)
    if (! (isscalar (dim) && dim == round (dim)) && dim > 0 && 
	dim < (nd + 1))
      error ("shift: dim must be an integer and valid dimension");
    endif
  else
    %% Find the first non-singleton dimension
    dim  = 1;
    while (dim < nd + 1 && sz (dim) == 1)
      dim = dim + 1;
    endwhile
    if (dim > nd)
      dim = 1;
    endif
  endif

  if (numel (x) < 1)
    error ("shift: x must not be empty");
  endif

  d = sz (dim);

  save_warn_empty_list_elements = warn_empty_list_elements;
  unwind_protect
    warn_empty_list_elements = 0;

    idx = cell ();
    for i = 1:nd
      idx {i} = 1:sz(i);
    endfor
    if (b >= 0)
      b = rem (b, d);
      idx {dim} = [d-b+1:d, 1:d-b];
    elseif (b < 0)
      b = rem (abs (b), d);
      idx {dim} = [b+1:d, 1:b];
    endif
    y = x (idx {:});

  unwind_protect_cleanup
    warn_empty_list_elements = save_warn_empty_list_elements;
  end_unwind_protect

endfunction
