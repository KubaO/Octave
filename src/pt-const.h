/*

Copyright (C) 1996, 1997 John W. Eaton

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
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#if !defined (octave_tree_const_h)
#define octave_tree_const_h 1

#if defined (__GNUG__)
#pragma interface
#endif

#include <string>

class ostream;

#include "oct-alloc.h"

#include "pt-mvr-base.h"

class octave_value_list;

class tree_walker;

#include "ov.h"

class
tree_constant : public tree_multi_val_ret
{
public:

  tree_constant (int l = -1, int c = -1)
    : tree_multi_val_ret (l, c), val (), orig_text () { }

  tree_constant (const octave_value& v, int l = -1, int c = -1)
    : tree_multi_val_ret (l, c), val (v), orig_text () { }

  tree_constant (const tree_constant& a)
    : tree_multi_val_ret (-1, -1), val (a.val), orig_text () { }

  ~tree_constant (void) { }

  tree_constant& operator = (const tree_constant& a)
    {
      if (this != &a)
	{
	  tree_multi_val_ret::operator = (a);
	  val = a.val;
	}
      return *this;
    }

  void *operator new (size_t size)
    { return allocator.alloc (size); }

  void operator delete (void *p, size_t size)
    { allocator.free (p, size); }

  // Indexed assignment.

  octave_value index (const octave_value_list& idx) const
    { return val.index (idx); }

  octave_value& reference (void)
    {
      val.make_unique ();
      return val;
    }

  octave_value value (void) const
    { return val; }

  octave_value assign (octave_value::assign_op op,
		       const octave_value_list& idx,
		       const octave_value& rhs)
    {
      val.assign (op, idx, rhs);
      return val;
    }

  // Type.  It would be nice to eliminate the need for this.

  bool is_constant (void) const { return true; }

  void maybe_mutate (void)
    { val.maybe_mutate (); }

  void print (ostream& os, bool pr_as_read_syntax = false,
	      bool pr_orig_txt = true);

  void print_raw (ostream& os, bool pr_as_read_syntax = false,
		  bool pr_orig_txt = true);

  octave_value eval (bool print = false);

  octave_value_list
  eval (bool print, int nargout, const octave_value_list& args);

  // Store the original text corresponding to this constant for later
  // pretty printing.

  void stash_original_text (const string& s)
    { orig_text = s; }

  string original_text (void) const
    { return orig_text; }

  void accept (tree_walker& tw);

private:

  // For custom memory management.
  static octave_allocator allocator;

  // The actual value that this constant refers to.
  octave_value val;

  // The original text form of this constant.
  string orig_text;
};

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
