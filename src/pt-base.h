// tree-base.h                                           -*- C++ -*-
/*

Copyright (C) 1992, 1993, 1994 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#if !defined (octave_tree_base_h)
#define octave_tree_base_h 1

class tree_constant;

/*
 * Base class for the parse tree.
 */
class
tree
{
public:
  tree (int l = -1, int c = -1)
    {
      line_num = l;
      column_num = c;
    }

  virtual ~tree (void) { }

  virtual int line (void) const { return line_num; }

  virtual int column (void) const { return column_num; }

private:
  int line_num;
  int column_num;
};

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
