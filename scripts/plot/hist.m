## Copyright (C) 1996, 1997 John W. Eaton
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
## Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
## 02110-1301, USA.

## -*- texinfo -*-
## @deftypefn {Function File} {} hist (@var{y}, @var{x}, @var{norm})
## Produce histogram counts or plots.
##
## With one vector input argument, plot a histogram of the values with
## 10 bins.  The range of the histogram bins is determined by the range
## of the data.
##
## Given a second scalar argument, use that as the number of bins.
##
## Given a second vector argument, use that as the centers of the bins,
## with the width of the bins determined from the adjacent values in
## the vector.
##
## If third argument is provided, the histogram is normalised such that
## the sum of the bars is equal to @var{norm}.
##
## Extreme values are lumped in the first and last bins.
##
## With two output arguments, produce the values @var{nn} and @var{xx} such
## that @code{bar (@var{xx}, @var{nn})} will plot the histogram.
## @end deftypefn
##
## @seealso{bar}

## Author: jwe

function [nn, xx] = hist (y, x, norm)

  if (nargin < 1 || nargin > 3)
    usage ("[nn, xx] = hist (y, x, norm)");
  endif

  arg_is_vector = isvector (y);

  if (rows (y) == 1)
    y = y(:);
  endif

  if (isreal (y))
    max_val = max (y);
    min_val = min (y);
  else
    error ("hist: first argument must be a vector");
  endif

  if (nargin == 1)
    n = 10;
    x = [0.5:n]'/n;
    x = x * (max_val - min_val) + ones(size(x)) * min_val;
  else
    ## nargin is either 2 or 3
    if (isscalar (x))
      n = x;
      if (n <= 0)
        error ("hist: number of bins must be positive");
      endif
      x = [0.5:n]'/n;
      x = x * (max_val - min_val) + ones(size(x)) * min_val;
    elseif (isreal (x))
      if (isvector (x))
	x = x(:);
      endif
      tmp = sort (x);
      if (any (tmp != x))
        warning ("hist: bin values not sorted on input");
        x = tmp;
      endif
    else
      error ("hist: second argument must be a scalar or a vector");
    endif
  endif

  cutoff = (x(1:end-1,:) + x(2:end,:)) / 2;
  n = rows (x);
  if (n < 30 && columns (x) == 1)
    ## The following algorithm works fastest for n less than about 30.
    chist = zeros (n+1, columns (y));
    for i = 1:n-1
      chist(i+1,:) = sum (y <= cutoff(i));
    endfor
    chist(n+1,:) = rows (y);
  else
    ## The following algorithm works fastest for n greater than about 30.
    ## Put cutoff elements between boundaries, integrate over all
    ## elements, keep totals at boundaries.
    [s, idx] = sort ([y; cutoff]);
    len = rows (y);
    chist = cumsum (idx <= len);
    t1 = zeros (1, columns (y));
    t2 = reshape (chist(idx > len), size (cutoff));
    t3 = chist(end,:);
    chist = [t1; t2; t3];
  endif

  freq = diff (chist);

  if (nargin == 3)
    ## Normalise the histogram.
    freq = freq / rows (y) * norm;
  endif

  if (nargout > 0)
    if (arg_is_vector)
      nn = freq';
      xx = x';
    else
      nn = freq;
      xx = x;
    endif
  else
    bar (x, freq);
  endif

endfunction

%!test
%!  [nn,xx]=hist([1:4],3);
%!  assert(xx, [1.5,2.5,3.5]);
%!  assert(nn, [2,1,1]);
%!test
%!  [nn,xx]=hist([1:4]',3);
%!  assert(xx, [1.5,2.5,3.5]);
%!  assert(nn, [2,1,1]);
%!test
%!  [nn,xx]=hist([[1:4]',[1:4]'],3);
%!  assert(xx, [[1.5,2.5,3.5]',[1.5,2.5,3.5]']);
%!  assert(nn, [[2,1,1]',[2,1,1]']);
%!assert(hist(1,1),1);
%!test
%!  for n = [10, 30, 100, 1000]
%!    assert( sum(hist([1:n], n)), n );
%!    assert( sum(hist([1:n], [2:n-1])), n);
%!    assert( sum(hist([1:n], [1:n])), n );
%!    assert( sum(hist([1:n], 29)), n);
%!    assert( sum(hist([1:n], 30)), n);
%!  endfor
