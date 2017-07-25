/*

Copyright (C) 1996-2017 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Octave is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#if ! defined (octave_pt_select_h)
#define octave_pt_select_h 1

#include "octave-config.h"

#include "base-list.h"
#include "comment-list.h"
#include "pt-cmd.h"
#include "pt-walk.h"
#include "symtab.h"

namespace octave
{
  class tree_expression;
  class tree_statement_list;

  // If.

  class tree_if_clause : public tree
  {
  public:

    tree_if_clause (int l = -1, int c = -1)
      : tree (l, c), expr (nullptr), list (nullptr), lead_comm (nullptr) { }

    tree_if_clause (tree_statement_list *sl, comment_list *lc = nullptr,
                    int l = -1, int c = -1)
      : tree (l, c), expr (nullptr), list (sl), lead_comm (lc) { }

    tree_if_clause (tree_expression *e, tree_statement_list *sl,
                    comment_list *lc = nullptr,
                    int l = -1, int c = -1)
      : tree (l, c), expr (e), list (sl), lead_comm (lc) { }

    // No copying!

    tree_if_clause (const tree_if_clause&) = delete;

    tree_if_clause& operator = (const tree_if_clause&) = delete;

    ~tree_if_clause (void);

    bool is_else_clause (void) { return ! expr; }

    tree_expression * condition (void) { return expr; }

    tree_statement_list * commands (void) { return list; }

    comment_list * leading_comment (void) { return lead_comm; }

    void accept (tree_walker& tw)
    {
      tw.visit_if_clause (*this);
    }

  private:

    // The condition to test.
    tree_expression *expr;

    // The list of statements to evaluate if expr is true.
    tree_statement_list *list;

    // Comment preceding ELSE or ELSEIF token.
    comment_list *lead_comm;
  };

  class tree_if_command_list : public base_list<tree_if_clause *>
  {
  public:

    tree_if_command_list (void) { }

    tree_if_command_list (tree_if_clause *t) { append (t); }

    // No copying!

    tree_if_command_list (const tree_if_command_list&) = delete;

    tree_if_command_list& operator = (const tree_if_command_list&) = delete;

    ~tree_if_command_list (void)
    {
      while (! empty ())
        {
          iterator p = begin ();
          delete *p;
          erase (p);
        }
    }

    void accept (tree_walker& tw)
    {
      tw.visit_if_command_list (*this);
    }
  };

  class tree_if_command : public tree_command
  {
  public:

    tree_if_command (int l = -1, int c = -1)
      : tree_command (l, c), list (nullptr),
        lead_comm (nullptr), trail_comm (nullptr)
    { }

    tree_if_command (tree_if_command_list *lst, comment_list *lc,
                     comment_list *tc, int l = -1, int c = -1)
      : tree_command (l, c), list (lst), lead_comm (lc), trail_comm (tc) { }

    // No copying!

    tree_if_command (const tree_if_command&) = delete;

    tree_if_command& operator = (const tree_if_command&) = delete;

    ~tree_if_command (void);

    tree_if_command_list * cmd_list (void) { return list; }

    comment_list * leading_comment (void) { return lead_comm; }

    comment_list * trailing_comment (void) { return trail_comm; }

    void accept (tree_walker& tw)
    {
      tw.visit_if_command (*this);
    }

  private:

    // List of if commands (if, elseif, elseif, ... else, endif)
    tree_if_command_list *list;

    // Comment preceding IF token.
    comment_list *lead_comm;

    // Comment preceding ENDIF token.
    comment_list *trail_comm;
  };

  // Switch.

  class tree_switch_case : public tree
  {
  public:

    tree_switch_case (int l = -1, int c = -1)
      : tree (l, c), label (nullptr), list (nullptr), lead_comm (nullptr) { }

    tree_switch_case (tree_statement_list *sl, comment_list *lc = nullptr,
                      int l = -1, int c = -1)
      : tree (l, c), label (nullptr), list (sl), lead_comm (lc) { }

    tree_switch_case (tree_expression *e, tree_statement_list *sl,
                      comment_list *lc = nullptr,
                      int l = -1, int c = -1)
      : tree (l, c), label (e), list (sl), lead_comm (lc) { }

    // No copying!

    tree_switch_case (const tree_switch_case&) = delete;

    tree_switch_case& operator = (const tree_switch_case&) = delete;

    ~tree_switch_case (void);

    bool is_default_case (void) { return ! label; }

    tree_expression * case_label (void) { return label; }

    tree_statement_list * commands (void) { return list; }

    comment_list * leading_comment (void) { return lead_comm; }

    void accept (tree_walker& tw)
    {
      tw.visit_switch_case (*this);
    }

  private:

    // The case label.
    tree_expression *label;

    // The list of statements to evaluate if the label matches.
    tree_statement_list *list;

    // Comment preceding CASE or OTHERWISE token.
    comment_list *lead_comm;
  };

  class tree_switch_case_list : public base_list<tree_switch_case *>
  {
  public:

    tree_switch_case_list (void) { }

    tree_switch_case_list (tree_switch_case *t) { append (t); }

    // No copying!

    tree_switch_case_list (const tree_switch_case_list&) = delete;

    tree_switch_case_list& operator = (const tree_switch_case_list&) = delete;

    ~tree_switch_case_list (void)
    {
      while (! empty ())
        {
          iterator p = begin ();
          delete *p;
          erase (p);
        }
    }

    void accept (tree_walker& tw)
    {
      tw.visit_switch_case_list (*this);
    }
  };

  class tree_switch_command : public tree_command
  {
  public:

    tree_switch_command (int l = -1, int c = -1)
      : tree_command (l, c), expr (nullptr), list (nullptr),
        lead_comm (nullptr), trail_comm (nullptr) { }

    tree_switch_command (tree_expression *e, tree_switch_case_list *lst,
                         comment_list *lc, comment_list *tc,
                         int l = -1, int c = -1)
      : tree_command (l, c), expr (e), list (lst), lead_comm (lc),
        trail_comm (tc) { }

    // No copying!

    tree_switch_command (const tree_switch_command&) = delete;

    tree_switch_command& operator = (const tree_switch_command&) = delete;

    ~tree_switch_command (void);

    tree_expression * switch_value (void) { return expr; }

    tree_switch_case_list * case_list (void) { return list; }

    comment_list * leading_comment (void) { return lead_comm; }

    comment_list * trailing_comment (void) { return trail_comm; }

    void accept (tree_walker& tw)
    {
      tw.visit_switch_command (*this);
    }

  private:

    // Value on which to switch.
    tree_expression *expr;

    // List of cases (case 1, case 2, ..., default)
    tree_switch_case_list *list;

    // Comment preceding SWITCH token.
    comment_list *lead_comm;

    // Comment preceding ENDSWITCH token.
    comment_list *trail_comm;
  };
}

#if defined (OCTAVE_USE_DEPRECATED_FUNCTIONS)

OCTAVE_DEPRECATED (4.4, "use 'octave::tree_if_clause' instead")
typedef octave::tree_if_clause tree_if_clause;

// tree_if_command_list is derived from a template.

OCTAVE_DEPRECATED (4.4, "use 'octave::tree_if_command' instead")
typedef octave::tree_if_command tree_if_command;

OCTAVE_DEPRECATED (4.4, "use 'octave::tree_switch_case' instead")
typedef octave::tree_switch_case tree_switch_case;

// tree_switch_case_list is derived from a template.

OCTAVE_DEPRECATED (4.4, "use 'octave::tree_switch_command' instead")
typedef octave::tree_switch_command tree_switch_command;

#endif

#endif
