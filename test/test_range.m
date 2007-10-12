## Copyright (C) 2007 John W. Eaton
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

## Test values of range

%!assert(full(1:9), [ 1 2 3 4 5 6 7 8 9 ])
%!assert(full(1:0.4:3), [ 1.0 1.4 1.8 2.2 2.6 3.0 ])
%!assert(full(9:1), zeros(1,0))
%!assert(full(9:-1:1), [ 9 8 7 6 5 4 3 2 1 ])
%!assert(full(1:-1:9), zeros(1,0))


## Test mixing integer range with other types

%!shared expect, r, z
%! expect = [ 1 2 3 4 5 6 7 8 9
%!            0 0 0 0 0 0 0 0 0 ];
%! z = zeros(1,9);
%! r = 1:9;

%!assert([ r ; z                  ], expect)
%!assert([ r ; logical(z)         ], expect)
%!assert([ r ; sparse(z)          ], expect)
%!assert([ r ; sparse(logical(z)) ], expect)

%!assert([ r ; int8(z)            ], int8(expect))
%!assert([ r ; int16(z)           ], int16(expect))
%!assert([ r ; int32(z)           ], int32(expect))
%!assert([ r ; int64(z)           ], int64(expect))
%!assert([ r ; uint8(z)           ], uint8(expect))
%!assert([ r ; uint16(z)          ], uint16(expect))
%!assert([ r ; uint32(z)          ], uint32(expect))
%!assert([ r ; uint64(z)          ], uint64(expect))


## Test mixing non integer range with other types

%!shared expect, r, z
%! expect = [ 1.0 1.4 1.8 2.2 2.6 3.0
%!            0   0   0   0   0   0   ];
%! z = zeros(1,6);
%! r = 1:0.4:3;

%!assert([ r ; z                  ], expect)
%!assert([ r ; logical(z)         ], expect)
%!assert([ r ; sparse(z)          ], expect)
%!assert([ r ; sparse(logical(z)) ], expect)

%!assert([ r ; int8(z)            ], int8(expect))
%!assert([ r ; int16(z)           ], int16(expect))
%!assert([ r ; int32(z)           ], int32(expect))
%!assert([ r ; int64(z)           ], int64(expect))
%!assert([ r ; uint8(z)           ], uint8(expect))
%!assert([ r ; uint16(z)          ], uint16(expect))
%!assert([ r ; uint32(z)          ], uint32(expect))
%!assert([ r ; uint64(z)          ], uint64(expect))

