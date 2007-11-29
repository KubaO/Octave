## Copyright (C) 1996, 1997, 2006, 2007 John W. Eaton
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

## Undocumented internal function.

## Author: jwe

function [h, varargin, narg] = __plt_get_axis_arg__ (caller, varargin)

  if (islogical (caller))
    nogca = caller;
    caller = varargin{1};
    varargin(1) = [];
  else
    nogca = false;
  endif

  if (nargin > 1 && length (varargin) > 0 && ishandle (varargin{1}))
    tmp = varargin{1};
    obj = get (tmp);
    if (strcmp (obj.type, "axes") || strcmp (obj.type, "hggroup"))
      h = ancestor (tmp, "axes");
      varargin(1) = [];
      if (isempty (varargin))
	varargin = {};
      endif
    else
      error ("%s: expecting first argument to be axes handle", caller);
    endif
  else
    f = get (0, "currentfigure");
    if (! isempty (f))
      h = get (f, 'currentaxes');
    elseif (nogca)
      h = NaN;
    else
      h = gca ();
    endif
    if (nargin < 2)
      varargin = {};
    endif
  endif

  narg = length (varargin);

endfunction
