## Copyright (C) 1996, 1998, 1999, 2005, 2006, 2007 Kurt Hornik
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
## @deftypefn {Function File} {} str2num (@var{s})
## Convert the string @var{s} to a number.
## @end deftypefn

## Author: jwe

function m = str2num (s)

  if (nargin == 1 && ischar (s))
    [nr, nc] = size (s);
    sep = ";";
    sep = sep (ones (nr, 1), 1);
    s = sprintf ("m = [%s];", reshape ([s, sep]', 1, nr * (nc + 1)));
    eval (s, "m = [];");
    if (ischar (m))
      m = [];
    endif
  else
    print_usage ();
  endif

endfunction

%!assert(str2num ("-1.3e2") == -130 && str2num ("[1, 2; 3, 4]") == [1, 2; 3, 4]);

%!error str2num ();

%!error str2num ("string", 1);

