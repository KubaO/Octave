/*

Copyright (C) 1996, 1997, 1998, 2000, 2001, 2002, 2003, 2004, 2005,
              2006, 2007 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "quit.h"

#include "error.h"
#include "gripes.h"
#include "oct-map.h"
#include "oct-lvalue.h"
#include "ov.h"
#include "pt-arg-list.h"
#include "pt-bp.h"
#include "pt-cmd.h"
#include "pt-exp.h"
#include "pt-jump.h"
#include "pt-loop.h"
#include "pt-stmt.h"
#include "pt-walk.h"
#include "unwind-prot.h"

// TRUE means we are evaluating some kind of looping construct.
bool evaluating_looping_command = false;

// Decide if it's time to quit a for or while loop.
static inline bool
quit_loop_now (void)
{
  OCTAVE_QUIT;

  // Maybe handle `continue N' someday...

  if (tree_continue_command::continuing)
    tree_continue_command::continuing--;

  bool quit = (error_state
	       || tree_return_command::returning
	       || tree_break_command::breaking
	       || tree_continue_command::continuing);

  if (tree_break_command::breaking)
    tree_break_command::breaking--;

  return quit;
}

// While.

tree_while_command::~tree_while_command (void)
{
  delete expr;
  delete list;
  delete lead_comm;
  delete trail_comm;
}

tree_command *
tree_while_command::dup (symbol_table::scope_id scope,
			 symbol_table::context_id context)
{
  return new tree_while_command (expr ? expr->dup (scope, context) : 0,
				 list ? list->dup (scope, context) : 0,
				 lead_comm ? lead_comm->dup () : 0,
				 trail_comm ? trail_comm->dup (): 0,
				 line (), column ());
}

void
tree_while_command::accept (tree_walker& tw)
{
  tw.visit_while_command (*this);
}

// Do-Until

tree_command *
tree_do_until_command::dup (symbol_table::scope_id scope,
			    symbol_table::context_id context)
{
  return new tree_do_until_command (expr ? expr->dup (scope, context) : 0,
				    list ? list->dup (scope, context) : 0,
				    lead_comm ? lead_comm->dup () : 0,
				    trail_comm ? trail_comm->dup (): 0,
				    line (), column ());
}

void
tree_do_until_command::accept (tree_walker& tw)
{
  tw.visit_do_until_command (*this);
}

// For.

tree_simple_for_command::~tree_simple_for_command (void)
{
  delete expr;
  delete list;
  delete lead_comm;
  delete trail_comm;
}

tree_command *
tree_simple_for_command::dup (symbol_table::scope_id scope,
			      symbol_table::context_id context)
{
  return new tree_simple_for_command (lhs ? lhs->dup (scope, context) : 0,
				      expr ? expr->dup (scope, context) : 0,
				      list ? list->dup (scope, context) : 0,
				      lead_comm ? lead_comm->dup () : 0,
				      trail_comm ? trail_comm->dup () : 0,
				      line (), column ());
}

void
tree_simple_for_command::accept (tree_walker& tw)
{
  tw.visit_simple_for_command (*this);
}

tree_complex_for_command::~tree_complex_for_command (void)
{
  delete expr;
  delete list;
  delete lead_comm;
  delete trail_comm;
}

tree_command *
tree_complex_for_command::dup (symbol_table::scope_id scope,
			       symbol_table::context_id context)
{
  return new tree_complex_for_command (lhs ? lhs->dup (scope, context) : 0,
				       expr ? expr->dup (scope, context) : 0,
				       list ? list->dup (scope, context) : 0,
				       lead_comm ? lead_comm->dup () : 0,
				       trail_comm ? trail_comm->dup () : 0,
				       line (), column ());
}

void
tree_complex_for_command::accept (tree_walker& tw)
{
  tw.visit_complex_for_command (*this);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
