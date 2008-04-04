## Copyright (C) 1995, 1996, 1997, 2005, 2006, 2007 Kurt Hornik
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
## @deftypefn {Function File} {} chisquare_pdf (@var{x}, @var{n})
## For each element of @var{x}, compute the probability density function
## (PDF) at @var{x} of the chisquare distribution with @var{n} degrees
## of freedom. 
## @end deftypefn

## Author: TT <Teresa.Twaroch@ci.tuwien.ac.at>
## Description: PDF of the chi-sqaure distribution

## Deprecated in version 3.0

function pdf = chisquare_pdf (varargin)

 pdf =  chi2pdf (varargin{:});

endfunction
