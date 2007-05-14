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
## Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
## 02110-1301, USA.

## -*- texinfo -*-
## @deftypefn {Function File} {} normal_rnd (@var{m}, @var{v}, @var{r}, @var{c})
## @deftypefnx {Function File} {} normal_rnd (@var{m}, @var{v}, @var{sz})
## Return an @var{r} by @var{c}  or @code{size (@var{sz})} matrix of
## random samples from the normal distribution with parameters @var{m} 
## and @var{v}.  Both @var{m} and @var{v} must be scalar or of size 
## @var{r} by @var{c}.
##
## If @var{r} and @var{c} are omitted, the size of the result matrix is
## the common size of @var{m} and @var{v}.
## @end deftypefn

## Author: KH <Kurt.Hornik@wu-wien.ac.at>
## Description: Random deviates from the normal distribution

function rnd = normal_rnd (varargin)

 if (nargin > 1)
   varargin{2} = sqrt (varargin{2});
 endif

 rnd =  normrnd (varargin{:});

endfunction
