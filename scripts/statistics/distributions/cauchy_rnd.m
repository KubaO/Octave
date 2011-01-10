## Copyright (C) 1995, 1996, 1997, 1998, 2000, 2002, 2004, 2005, 2006,
##               2007 Kurt Hornik
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or (at
## your option) any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn  {Function File} {} cauchy_rnd (@var{location}, @var{scale}, @var{r}, @var{c})
## @deftypefnx {Function File} {} cauchy_rnd (@var{location}, @var{scale}, @var{sz})
## Return an @var{r} by @var{c} or a @code{size (@var{sz})} matrix of 
## random samples from the Cauchy distribution with parameters @var{location} 
## and @var{scale} which must both be scalar or of size @var{r} by @var{c}.
##
## If @var{r} and @var{c} are omitted, the size of the result matrix is
## the common size of @var{location} and @var{scale}.
## @end deftypefn

## Author: KH <Kurt.Hornik@wu-wien.ac.at>
## Description: Random deviates from the Cauchy distribution

function rnd = cauchy_rnd (location, scale, r, c)

  if (nargin > 1)
    if (!isscalar (location) || !isscalar (scale)) 
      [retval, location, scale] = common_size (location, scale);
      if (retval > 0)
        error ("cauchy_rnd: LOCATION and SCALE must be of common size or scalar");
      endif
    endif
  endif

  if (nargin == 4)
    if (! (isscalar (r) && (r > 0) && (r == round (r))))
      error ("cauchy_rnd: R must be a positive integer");
    endif
    if (! (isscalar (c) && (c > 0) && (c == round (c))))
      error ("cauchy_rnd: C must be a positive integer");
    endif
    sz = [r, c];

    if (any (size (location) != 1)
        && (length (size (location)) != length (sz)
            || any (size (location) != sz)))
      error ("cauchy_rnd: LOCATION and SCALE must be scalar or of size [R, C]");
    endif
  elseif (nargin == 3)
    if (isscalar (r) && (r > 0))
      sz = [r, r];
    elseif (isvector(r) && all (r > 0))
      sz = r(:)';
    else
      error ("cauchy_rnd: R must be a positive integer or vector");
    endif

    if (any (size (location) != 1)
        && (length (size (location)) != length (sz)
        || any (size (location) != sz)))
      error ("cauchy_rnd: LOCATION and SCALE must be scalar or of size SZ");
    endif
  elseif (nargin == 2)
    sz = size(location);
  else
    print_usage ();
  endif

  if (isscalar (location) && isscalar (scale)) 
    if (find (!(location > -Inf) | !(location < Inf)
                | !(scale > 0) | !(scale < Inf)))
      rnd = NaN (sz);
    else
      rnd = location - cot (pi * rand (sz)) .* scale;
    endif
  else
    rnd = NaN (sz);
    k = find ((location > -Inf) & (location < Inf) 
              & (scale > 0) & (scale < Inf));
    if (any (k))
      rnd(k) = location(k)(:) - cot (pi * rand (size (k))) .* scale(k)(:);
    endif
  endif

endfunction
