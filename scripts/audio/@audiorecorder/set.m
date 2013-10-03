## Copyright (C) 2013 Vytautas Jančauskas
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
## @deftypefn {Function File} {} set (@var{recorder}, @var{name}, @var{value})
## Set the value of property specified by @var{name} to a given @var{value}.
## @end deftypefn
## @deftypefn {Function File} {} set (@var{recorder}, @var{names}, @var{values})
## Given a cell array of property @var{names} and a cell array of @var{values},
## set each property to a corresponding value.
## @end deftypefn
## @deftypefn {Function File} {} set (@var{recorder}, @var{properties})
## Given a structure where fields are property names, set the value of those
## properties for an audiorecorder object to corresponding values.
## @end deftypefn
## @deftypefn {Function File} {@var{properties} =} set (@var{recorder})
## Returns a structure of settable properties.
## @end deftypefn

function settable = set (varargin)
  if nargin < 1 || nargin > 3
    print_usage ();
  endif
  recorder = struct (varargin{1}).recorder;
  if nargin == 1
    settable.SampleRate = {};
    settable.Tag = {};
    settable.UserData = {};
  elseif nargin == 2
    for [value, property] = varargin{2}
      setproperty (recorder, property, value);
    endfor
  elseif nargin == 3
    if iscell (varargin{2})
      index = 1;
      for property = varargin{2}
        setproperty (recorder, char (property), varargin{3}{index});
        index = index + 1;
      endfor
    else
      setproperty (recorder, varargin{2}, varargin{3});
    endif
  else
    error ("audiorecorder: wrong number of arguments to the set method");
  endif
endfunction

function setproperty (recorder, property, value)
  switch (property)
    case "SampleRate"
      __recorder_set_fs__ (recorder, value);
    case "Tag"
      __recorder_set_tag__ (recorder, value);
    case "UserData"
      __recorder_set_userdata__ (recorder, value);
    otherwise
      error ("audiorecorder: no such property or the property specified is read-only");
  endswitch
endfunction
