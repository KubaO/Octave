## Copyright (C) 1995, 1996, 1997  Kurt Hornik
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
## @deftypefn {Function File} {} binomial_pdf (@var{x}, @var{n}, @var{p})
## For each element of @var{x}, compute the probability density function
## (PDF) at @var{x} of the binomial distribution with parameters @var{n}
## and @var{p}.
## @end deftypefn

## Author: KH <Kurt.Hornik@ci.tuwien.ac.at>
## Description: PDF of the binomial distribution

function pdf = binomial_pdf (x, n, p)

  if (nargin != 3)
    usage ("binomial_pdf (x, n, p)");
  endif

  if (!isscalar (n) || !isscalar (p))
    [retval, x, n, p] = common_size (x, n, p);
    if (retval > 0)
      error ("binomial_pdf: x, n and p must be of common size or scalar");
    endif
  endif

  sz = size (x);
  pdf = zeros (sz);

  k = find (isnan (x) | !(n >= 0) | (n != round (n)) | !(p >= 0) | !(p <= 1));
  if (any (k))
    pdf(k) = NaN;
  endif

  k = find ((x >= 0) & (x <= n) & (x == round (x))
	    & (n == round (n)) & (p >= 0) & (p <= 1));
  if (any (k))
    if (isscalar (n) && isscalar (p))
      pdf(k) = (bincoeff (n, x(k)) .* (p .^ x(k))
		.* ((1 - p) .^ (n - x(k))));
    else
      pdf(k) = (bincoeff (n(k), x(k)) .* (p(k) .^ x(k))
		.* ((1 - p(k)) .^ (n(k) - x(k))));
    endif
  endif

endfunction
