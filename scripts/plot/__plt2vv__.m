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
## @deftypefn {Function File} {} __plt2vv__ (@var{h}, @var{x}, @var{y}, @var{options})
## @end deftypefn

## Author: jwe

function __plt2vv__ (h, x, y, options)

  if (nargin < 3 || nargin > 4)
    print_usage ();
  endif

  if (nargin < 4 || isempty (options))
    options = __default_plot_options__ ();
  endif

  if (numel (options) > 1)
    options = options(1);
  endif

  [x_nr, x_nc] = size (x);
  [y_nr, y_nc] = size (y);

  if (x_nr == 1)
    x = x';
    tmp = x_nr;
    x_nr = x_nc;
    x_nc = tmp;
  endif

  if (y_nr == 1)
    y = y';
    tmp = y_nr;
    y_nr = y_nc;
    y_nc = tmp;
  endif

  if (x_nr == y_nr)
    key = options.key;
    if (! isempty (key))
      set (h, "key", "on");
    endif
    color = options.color;
    if (isempty (color))
      color = __next_line_color__ ();
    endif
    line (x, y, "keylabel", key, "color", color,
	  "linestyle", options.linestyle,
	  "marker", options.marker);
  else
    error ("__plt2vv__: vector lengths must match");
  endif

endfunction
