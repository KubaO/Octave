/*

Copyright (C) 1993-2017 John W. Eaton
Copyright (C) 2009 VZLU Prague, a.s.

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

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <sstream>

#include "fcn-info.h"
#include "interpreter-private.h"
#include "interpreter.h"
#include "ov-fcn.h"
#include "ov-usr-fcn.h"
#include "symrec.h"
#include "symscope.h"
#include "symtab.h"
#include "utils.h"

namespace octave
{
  octave_value
  symbol_scope::find (const std::string& name, const octave_value_list& args,
                      bool skip_variables, bool local_funcs)
  {
    // Variable.

    symbol_table& symtab
      = __get_symbol_table__ ("symbol_scope::find");

    if (! skip_variables)
      {
        table_iterator p = m_symbols.find (name);

        if (p != m_symbols.end ())
          {
            symbol_record sr = p->second;

            if (sr.is_global ())
              return symtab.global_varval (name);
            else
              {
                octave_value val = sr.varval ();

                if (val.is_defined ())
                  return val;
              }
          }
      }

    if (local_funcs)
      {
        // Subfunction.  I think it only makes sense to check for
        // subfunctions if we are currently executing a function defined
        // from a .m file.

        octave_value fcn = find_subfunction (name);

        if (fcn.is_defined ())
          return fcn;
      }

    return symtab.fcn_table_find (name, args, local_funcs);
  }

  symbol_record&
  symbol_scope::insert (const std::string& name, bool force_add)
  {
    table_iterator p = m_symbols.find (name);

    if (p == m_symbols.end ())
      {
        symbol_record ret (this, name);

        if (m_is_nested && m_parent && m_parent->look_nonlocal (name, ret))
          return m_symbols[name] = ret;
        else
          {
            if (m_is_static && ! force_add)
              ret.mark_added_static ();

            return m_symbols[name] = ret;
          }
      }
    else
      return p->second;
  }

  void
  symbol_scope::clear_global (const std::string& name)
  {
    table_iterator p = m_symbols.find (name);

    if (p != m_symbols.end ())
      {
        symbol_record& sr = p->second;

        if (sr.is_global ())
          sr.unmark_global ();
      }

    symbol_table& symtab
      = __get_symbol_table__ ("symbol_scope::clear_global");

    symtab.erase_global (name);
  }

  void
  symbol_scope::clear_global_pattern (const std::string& pat)
  {
    glob_match pattern (pat);

    for (auto& nm_sr : m_symbols)
      {
        symbol_record& sr = nm_sr.second;

        if (sr.is_global () && pattern.match (sr.name ()))
          sr.unmark_global ();
      }

    symbol_table& symtab
      = __get_symbol_table__ ("symbol_scope::clear_global_pattern");

    symtab.erase_global_pattern (pattern);
  }

  std::list<workspace_element>
  symbol_scope::workspace_info (void) const
  {
    std::list<workspace_element> retval;

    for (const auto& nm_sr : m_symbols)
      {
        std::string nm = nm_sr.first;
        symbol_record sr = nm_sr.second;

        if (! sr.is_hidden ())
          {
            octave_value val = sr.varval ();

            if (val.is_defined ())
              {
                // FIXME: fix size for objects, see kluge in variables.cc
                //dim_vector dv = val.dims ();
                octave_value tmp = val;
                Matrix sz = tmp.size ();
                dim_vector dv = dim_vector::alloc (sz.numel ());
                for (octave_idx_type i = 0; i < dv.ndims (); i++)
                  dv(i) = sz(i);

                char storage = ' ';
                if (sr.is_global ())
                  storage = 'g';
                else if (sr.is_persistent ())
                  storage = 'p';
                else if (sr.is_automatic ())
                  storage = 'a';
                else if (sr.is_formal ())
                  storage = 'f';
                else if (sr.is_hidden ())
                  storage = 'h';
                else if (sr.is_inherited ())
                  storage = 'i';

                std::ostringstream buf;
                val.short_disp (buf);
                std::string short_disp_str = buf.str ();

                workspace_element elt (storage, nm, val.class_name (),
                                       short_disp_str, dv.str (),
                                       val.iscomplex ());

                retval.push_back (elt);
              }
          }
      }

    return retval;
  }

  octave_value
  symbol_scope::dump (void) const
  {
    std::map<std::string, octave_value> m
      = {{ "name", m_name },
         { "symbols", dump_symbols_map () },
         { "persistent_variables", m_persistent_symbols },
         { "subfunctions", dump_function_map (m_subfunctions) }};

    return octave_value (m);
  }

  octave_value
  symbol_scope::dump_symbols_map (void) const
  {
    std::map<std::string, octave_value> info_map;

    for (const auto& nm_sr : m_symbols)
      {
        std::string nm = nm_sr.first;
        const symbol_record& sr = nm_sr.second;
        info_map[nm] = sr.dump ();
      }

    return octave_value (info_map);
  }

  void
  symbol_scope::install_subfunction (const std::string& name,
                                     const octave_value& fval, bool is_nested)
  {
    m_subfunctions[name] = fval;

    // This can be simpler once the scope object is stored in the function
    // object...
    octave_user_function *fcn = fval.user_function_value ();

    symbol_scope *fcn_scope = fcn->scope ();

    fcn_scope->set_parent (this);

    if (is_nested)
      {
        m_children.push_back (fcn_scope);

        fcn->mark_as_nested_function ();

        fcn_scope->m_is_nested = true;
      }

  }

  octave_value
  symbol_scope::find_subfunction (const std::string& name) const
  {
    subfunctions_const_iterator p = m_subfunctions.find (name);

    if (p != m_subfunctions.end ())
      return p->second;

    if (m_parent)
      return m_parent->find_subfunction (name);

    return octave_value ();
  }

  void
  symbol_scope::mark_subfunctions_in_scope_as_private (const std::string& class_name)
  {
    for (auto& nm_sf : m_subfunctions)
      {
        octave_function *fcn = nm_sf.second.function_value ();

        if (fcn)
          fcn->mark_as_private_function (class_name);
      }
  }

  void
  symbol_scope::set_parent (symbol_scope *p)
  {
    m_parent = p;

    if (m_parent)
      {
        // If m_parent is the top-level scope, there will be no parent
        // function.

        octave_function *current_fcn = function ();

        if (current_fcn && current_fcn->is_anonymous_function ())
          {
            octave_function *parent_fcn = m_parent->function ();

            if (parent_fcn)
              m_parent_fcn = octave_value (parent_fcn, true);
          }
      }
  }

  void
  symbol_scope::update_nest (void)
  {
    if (m_parent)
      {
        // fix bad symbol_records
        for (auto& nm_sr : m_symbols)
          {
            symbol_record& ours = nm_sr.second;

            if (! ours.is_formal ()
                && m_is_nested && m_parent->look_nonlocal (nm_sr.first, ours))
              {
                if (ours.is_global () || ours.is_persistent ())
                  error ("global and persistent may only be used in the topmost level in which a nested variable is used");
              }
            else
              ours.set_curr_fcn (m_fcn);
          }

        // The scopes of nested functions are static.
        m_is_static = true;
      }
    else if (m_children.size ())
      {
        // Parents of nested functions have static scopes.
        m_is_static = true;

        for (auto& nm_sr : m_symbols)
          nm_sr.second.set_curr_fcn (m_fcn);
      }

    for (auto& symtab_p : m_children)
      symtab_p->update_nest ();
  }

  bool
  symbol_scope::look_nonlocal (const std::string& name, symbol_record& result)
  {
    table_iterator p = m_symbols.find (name);
    if (p == m_symbols.end ())
      {
        if (m_is_nested && m_parent)
          return m_parent->look_nonlocal (name, result);
      }
    else if (! p->second.is_automatic ())
      {
        result.bind_fwd_rep (p->second);
        return true;
      }

    return false;
  }

  void
  symbol_scope::bind_script_symbols (symbol_scope *curr_scope)
  {
    for (auto& nm_sr : m_symbols)
      nm_sr.second.bind_fwd_rep (curr_scope->find_symbol (nm_sr.first));
  }

  void
  symbol_scope::unbind_script_symbols (void)
  {
    for (auto& nm_sr : m_symbols)
      nm_sr.second.unbind_fwd_rep ();
  }
}