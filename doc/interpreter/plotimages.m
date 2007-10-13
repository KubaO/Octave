## Copyright (C) 2007 John W. Eaton and David Bateman
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

function plotimages (nm, typ)
  bury_output ();
  if (strcmp (nm, "plot"))
    x = -10:0.1:10;
    plot (x, sin (x));
    print (strcat (nm, ".", typ), strcat ("-d", typ))    
  elseif (strcmp (nm, "hist"))
    hist (randn (10000, 1), 30);
    print (strcat (nm, ".", typ), strcat ("-d", typ))    
  elseif (strcmp (nm, "errorbar"))
    x = 0:0.1:10;
    y = sin (x);
    yp =  0.1 .* randn (size (x));
    ym = -0.1 .* randn (size (x));
    errorbar (x, sin (x), ym, yp);
    print (strcat (nm, ".", typ), strcat ("-d", typ))    
  elseif (strcmp (nm, "polar"))
    polar (0:0.1:10*pi, 0:0.1:10*pi);
    print (strcat (nm, ".", typ), strcat ("-d", typ))    
  elseif (strcmp (nm, "mesh"))
    tx = ty = linspace (-8, 8, 41)';
    [xx, yy] = meshgrid (tx, ty);
    r = sqrt (xx .^ 2 + yy .^ 2) + eps;
    tz = sin (r) ./ r;
    mesh (tx, ty, tz);
    print (strcat (nm, ".", typ), strcat ("-d", typ))    
  elseif (strcmp (nm, "plot3"))
    t = 0:0.1:10*pi;
    r = linspace (0, 1, numel (t));
    z = linspace (0, 1, numel (t));
    plot3 (r.*sin(t), r.*cos(t), z);
    print (strcat (nm, ".", typ), strcat ("-d", typ))    
  else
    error ("unrecognized plot requested");
  endif
  bury_output ();
endfunction

## Use this function before plotting commands and after every call to
## print since print() resets output to stdout (unfortunately, gnpulot
## can't pop output as it can the terminal type).
function bury_output ()
  f = figure (1);
  set (f, "visible", "off");
endfunction
