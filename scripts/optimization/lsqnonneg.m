## Copyright (C) 2008 Bill Denney
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
## @deftypefn {Function File} {@var{x} =} lsqnonneg (@var{c}, @var{d})
## @deftypefnx {Function File} {@var{x} =} lsqnonneg (@var{c}, @var{d}, @var{x0})
## @deftypefnx {Function File} {[@var{x}, @var{resnorm}] =} lsqnonneg (@dots{})
## @deftypefnx {Function File} {[@var{x}, @var{resnorm}, @var{residual}] =} lsqnonneg (@dots{})
## @deftypefnx {Function File} {[@var{x}, @var{resnorm}, @var{residual}, @var{exitflag}] =} lsqnonneg (@dots{})
## @deftypefnx {Function File} {[@var{x}, @var{resnorm}, @var{residual}, @var{exitflag}, @var{output}] =} lsqnonneg (@dots{})
## @deftypefnx {Function File} {[@var{x}, @var{resnorm}, @var{residual}, @var{exitflag}, @var{output}, @var{lambda}] =} lsqnonneg (@dots{})
## Minimize @code{norm (@var{c}*@var{x}-d)} subject to @code{@var{x} >=
## 0}. @var{c} and @var{d} must be real.  @var{x0} is an optional
## initial guess for @var{x}.
##
## Outputs:
## @itemize @bullet
## @item resnorm
##
## The squared 2-norm of the residual: norm(@var{c}*@var{x}-@var{d})^2
## @item residual
##
## The residual: @var{d}-@var{c}*@var{x}
## @item exitflag
##
## An indicator of convergence.  0 indicates that the iteration count
## was exceeded, and therefore convergence was not reached; >0 indicates
## that the algorithm converged.  (The algorithm is stable and will
## converge given enough iterations.)
## @item output
##
## A structure with two fields:
## @itemize @bullet
## @item "algorithm": The algorithm used ("nnls")
## @item "iterations": The number of iterations taken.
## @end itemize
## @item lambda
##
## Not implemented.
## @end itemize
## @seealso{optimset}
## @end deftypefn

## This is implemented from Lawson and Hanson's 1973 algorithm on page
## 161 of Solving Least Squares Problems.

function [x, resnorm, residual, exitflag, output, lambda] = lsqnonneg (c, d, x = [], options = [])

  ## Lawson-Hanson Step 1 (LH1): initialize the variables.
  if (isempty (x))
    ## Initial guess is 0s.
    x = zeros (columns (c), 1);
  endif

  if (isempty (options))
    ## FIXME: what are the correct defaults?
    options = optimset ("maxiter", 1e5, "tolx", 1e-8);
  endif

  ## Initialize the values.
  p = [];
  z = 1:numel (x);
  ## LH2: compute the gradient.
  w = c'*(d - c*x);

  iter = 0;
  ## LH3: test for completion.
  while (! isempty (z) && any (w(z) > 0) && iter < options.MaxIter)
    ## LH4: find the maximum gradient.
    idx = find (w == max (w));
    if (numel (idx) > 1)
      warning ("lsqnonneg:nonunique",
               "A non-unique solution may be returned due to equal gradients.");
      idx = idx(1);
    endif
    ## LH5: move the index from Z to P.
    z(z == idx) = [];
    p(end+1) = idx;

    newx = false;
    while (! newx && iter < options.MaxIter)
      iter++;

      ## LH6: compute the positive matrix and find the min norm solution
      ## of the positive problem.
      cpos = c;
      cpos(:,z) = 0;
      ## Find min norm solution to the positive matrix.
      [cpos_q, cpos_r] = qr (cpos, 0);
      xtmp = (cpos_r\cpos_q')*d;
      xtmp(z) = 0;
      if (all (xtmp >= 0))
        ## LH7: tmp solution found, iterate.
        newx = true;
        x = xtmp;
      else
        ## LH8, LH9: find the scaling factor and adjust X.
        mask = (xtmp < 0);
        alpha = min (x(mask)./(x(mask) - xtmp(mask)));
        ## LH10: adjust X.
        x = x + alpha*(xtmp - x);
        ## LH11: move from P to Z all X == 0.
        idx = find (x == 0);
        p(ismember (p, idx)) = [];
        z = [z idx];
      endif
    endwhile
    w = c'*(d - c*x);
  endwhile
  ## LH12: complete.

  ## Generate the additional output arguments.
  if (nargout > 1)
    resnorm = norm (C*x-d) ^ 2;
  endif
  if (nargout > 2)
    residual = d-C*x;
  endif
  exitflag = iter;
  if (nargout > 3 && iter >= options.MaxIter)
    exitflag = 0;
  endif
  if (nargout > 4)
    output = struct ("algorithm", "nnls", "iterations", iter);
  endif
  if (nargout > 5)
    ## FIXME
    error ("lsqnonneg: lambda is not yet implemented");
  endif

endfunction

## Tests
%!test
%! C = [1 0;0 1;2 1];
%! d = [1;3;-2];
%! assert (lsqnonneg (C, d), [0;0.5], 100*eps)

%!test
%! C = [0.0372 0.2869;0.6861 0.7071;0.6233 0.6245;0.6344 0.6170];
%! d = [0.8587;0.1781;0.0747;0.8405];
%! xnew = [0;0.6929];
%! assert (lsqnonneg (C, d), xnew, 0.0001)
