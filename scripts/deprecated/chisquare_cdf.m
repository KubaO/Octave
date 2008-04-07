## Copyright (C) 1995, 1996, 1997, 2005, 2007, 2008 Kurt Hornik
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
## @deftypefn {Function File} {} chisquare_cdf (@var{x}, @var{n})
## For each element of @var{x}, compute the cumulative distribution
## function (CDF) at @var{x} of the chisquare distribution with @var{n}
## degrees of freedom.
## @end deftypefn

## Author: TT <Teresa.Twaroch@ci.tuwien.ac.at>
## Description: CDF of the chi-square distribution

## Deprecated in version 3.0

function cdf = chisquare_cdf (varargin)

  persistent warned = false;
  if (! warned)
    warned = true;
    warning ("Octave:deprecated-function",
        ["chisquare_cdf is obsolete and will be removed from a future\n",
	       "version of Octave, please use chi2cdf instead"]);
  endif

 cdf =  chi2cdf (varargin{:});

endfunction
