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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cassert>
#include <cstring>

#include <iomanip>
#include <fstream>
#include <string>

#include <Array.h>
#include <Array2.h>
#include <Array3.h>

#include <Array.cc>

#include "byte-swap.h"
#include "lo-ieee.h"
#include "lo-mappers.h"
#include "lo-sstream.h"
#include "lo-utils.h"
#include "str-vec.h"
#include "quit.h"

#include "error.h"
#include "input.h"
#include "oct-stdstrm.h"
#include "oct-stream.h"
#include "oct-obj.h"
#include "utils.h"

// Possible values for conv_err:
//
//   1 : not a real scalar
//   2 : value is NaN
//   3 : value is not an integer

static int
convert_to_valid_int (const octave_value& tc, int& conv_err)
{
  int retval = 0;

  conv_err = 0;

  double dval = tc.double_value ();

  if (! error_state)
    {
      if (! lo_ieee_is_NaN_or_NA (dval))
	{
	  int ival = NINT (dval);

	  if (ival == dval)
	    retval = ival;
	  else
	    conv_err = 3;
	}
      else
	conv_err = 2;
    }
  else
    conv_err = 1;

  return retval;
}

static int
get_size (double d, const std::string& who)
{
  int retval = -1;

  if (! lo_ieee_is_NaN_or_NA (d))
    {
      if (! xisinf (d))
	{
	  if (d >= 0.0)
	    retval = NINT (d);
	  else
	    ::error ("%s: negative value invalid as size specification",
		     who.c_str ());
	}
      else
	retval = -1;
    }
  else
    ::error ("%s: NaN is invalid as size specification", who.c_str ());

  return retval;
}

static void
get_size (const Array<double>& size, int& nr, int& nc, bool& one_elt_size_spec,
	  const std::string& who)
{
  nr = -1;
  nc = -1;

  one_elt_size_spec = false;

  double dnr = -1.0;
  double dnc = -1.0;

  int sz_len = size.length ();

  if (sz_len == 1)
    {
      one_elt_size_spec = true;

      dnr = size (0);

      dnc = (dnr == 0.0) ? 0.0 : 1.0;
    }
  else if (sz_len == 2)
    {
      dnr = size (0);

      if (! xisinf (dnr))
	dnc = size (1);
      else
	::error ("%s: invalid size specification", who.c_str ());
    }
  else
    ::error ("%s: invalid size specification", who.c_str ());

  if (! error_state)
    {
      nr = get_size (dnr, who);

      if (! error_state && dnc >= 0.0)
	nc = get_size (dnc, who);
    }
}

scanf_format_list::scanf_format_list (const std::string& s)
  : nconv (0), curr_idx (0), list (16), buf (0)
{
  int num_elts = 0;

  int n = s.length ();

  int i = 0;

  int width = 0;
  bool discard = false;
  char modifier = '\0';
  char type = '\0';

  bool have_more = true;

  while (i < n)
    {
      have_more = true;

      if (! buf)
	buf = new OSSTREAM ();

      if (s[i] == '%')
	{
	  // Process percent-escape conversion type.

	  process_conversion (s, i, n, width, discard, type, modifier,
			      num_elts);
	  have_more = (buf != 0);
	}
      else if (isspace (s[i]))
	{
	  type = scanf_format_elt::whitespace_conversion;

	  width = 0;
	  discard = false;
	  modifier = '\0';
	  *buf << " ";

	  while (++i < n && isspace (s[i]))
	    /* skip whitespace */;

	  add_elt_to_list (width, discard, type, modifier, num_elts);

	  have_more = false;
	}
      else
	{
	  type = scanf_format_elt::literal_conversion;

	  width = 0;
	  discard = false;
	  modifier = '\0';

	  while (i < n && ! isspace (s[i]) && s[i] != '%')
	    *buf << s[i++];

	  add_elt_to_list (width, discard, type, modifier, num_elts);

	  have_more = false;
	}

      if (nconv < 0)
	{
	  have_more = false;
	  break;
	}
    }

  if (have_more)
    add_elt_to_list (width, discard, type, modifier, num_elts);

  list.resize (num_elts);

  delete buf;
}

scanf_format_list::~scanf_format_list (void)
{
  int n = list.length ();

  for (int i = 0; i < n; i++)
    {
      scanf_format_elt *elt = list(i);
      delete elt;
    }	
}

void
scanf_format_list::add_elt_to_list (int width, bool discard, char type,
				    char modifier, int& num_elts,
				    const std::string& char_class)
{
  if (buf)
    {
      *buf << OSSTREAM_ENDS;

      std::string text = OSSTREAM_STR (*buf);

      OSSTREAM_FREEZE (*buf);

      if (! text.empty ())
	{
	  scanf_format_elt *elt
	    = new scanf_format_elt (text.c_str (), width, discard, type,
				    modifier, char_class);

	  if (num_elts == list.length ())
	    list.resize (2 * num_elts);

	  list(num_elts++) = elt;
	}

      delete buf;
      buf = 0;
    }
}

static std::string
expand_char_class (const std::string& s)
{
  std::string retval;

  size_t len = s.length ();

  size_t i = 0;

  while (i < len)
    {
      unsigned char c = s[i++];

      if (c == '-' && i > 1 && i < len
	  && (unsigned char) s[i-2] <= (unsigned char) s[i])
	{
	  // Add all characters from the range except the first (we
	  // already added it below).

	  for (c = s[i-2]+1; c < s[i]; c++)
	    retval += c;
	}
      else
	{
	  // Add the character to the class.  Only add '-' if it is
	  // the last character in the class.

	  if (c != '-' || i == len)
	    retval += c;
	}
    }

  return retval;
}

void
scanf_format_list::process_conversion (const std::string& s, int& i, int n,
				       int& width, bool& discard, char& type,
				       char& modifier, int& num_elts)
{
  width = 0;
  discard = false;
  modifier = '\0';
  type = '\0';

  *buf << s[i++];

  bool have_width = false;

  while (i < n)
    {
      switch (s[i])
	{
	case '*':
	  if (discard)
	    nconv = -1;
	  else
	    {
	      discard = true;
	      *buf << s[i++];
	    }
	  break;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	  if (have_width)
	    nconv = -1;
	  else
	    {
	      char c = s[i++];
	      width = width * 10 + c - '0';
	      have_width = true;
	      *buf << c;
	      while (i < n && isdigit (s[i]))
		{
		  c = s[i++];
		  width = width * 10 + c - '0';
		  *buf << c;
		}
	    }
	  break;

	case 'h': case 'l': case 'L':
	  if (modifier != '\0')
	    nconv = -1;
	  else
	    modifier = s[i++];
	  break;

	case 'd': case 'i': case 'o': case 'u': case 'x':
	  if (modifier == 'L')
	    {
	      nconv = -1;
	      break;
	    }
	  goto fini;

	case 'e': case 'f': case 'g':
	  if (modifier == 'h')
	    {
	      nconv = -1;
	      break;
	    }

	  // No float or long double conversions, thanks.
	  *buf << 'l';

	  goto fini;

	case 'c': case 's': case 'p': case '%': case '[':
	  if (modifier != '\0')
	    {
	      nconv = -1;
	      break;
	    }
	  goto fini;

	fini:
	  {
	    if (finish_conversion (s, i, n, width, discard, type,
				   modifier, num_elts) == 0)
	      return;
	  }
	  break;

	default:
	  nconv = -1;
	  break;
	}

      if (nconv < 0)
	break;
    }

  nconv = -1;
}

int
scanf_format_list::finish_conversion (const std::string& s, int& i, int n,
				      int& width, bool discard, char& type,
				      char modifier, int& num_elts)
{
  int retval = 0;

  std::string char_class;

  int beg_idx = -1;
  int end_idx = -1;

  if (s[i] == '%')
    {
      type = '%';
      *buf << s[i++];
    }
  else
    {
      type = s[i];

      if (s[i] == '[')
	{
	  *buf << s[i++];

	  if (i < n)
	    {
	      beg_idx = i;

	      if (s[i] == '^')
		{
		  type = '^';
		  *buf << s[i++];

		  if (i < n)
		    {
		      beg_idx = i;

		      if (s[i] == ']')
			*buf << s[i++];
		    }
		}
	      else if (s[i] == ']')
		*buf << s[i++];
	    }

	  while (i < n && s[i] != ']')
	    *buf << s[i++];

	  if (i < n && s[i] == ']')
	    {
	      end_idx = i-1;
	      *buf << s[i++];
	    }

	  if (s[i-1] != ']')
	    retval = nconv = -1;
	}
      else
	*buf << s[i++];
    }

  nconv++;

  if (nconv > 0)
    {
      if (beg_idx >= 0 && end_idx >= 0)
	char_class = expand_char_class (s.substr (beg_idx,
						  end_idx - beg_idx + 1));

      add_elt_to_list (width, discard, type, modifier, num_elts, char_class);
    }

  return retval;
}

void
scanf_format_list::printme (void) const
{
  int n = list.length ();

  for (int i = 0; i < n; i++)
    {
      scanf_format_elt *elt = list(i);

      std::cerr
	<< "width:      " << elt->width << "\n"
	<< "discard:    " << elt->discard << "\n"
	<< "type:       ";

      if (elt->type == scanf_format_elt::literal_conversion)
	std::cerr << "literal text\n";
      else if (elt->type == scanf_format_elt::whitespace_conversion)
	std::cerr << "whitespace\n";
      else
	std::cerr << elt->type << "\n";

      std::cerr
	<< "modifier:   " << elt->modifier << "\n"
	<< "char_class: `" << undo_string_escapes (elt->char_class) << "'\n"
	<< "text:       `" << undo_string_escapes (elt->text) << "'\n\n";
    }
}

bool
scanf_format_list::all_character_conversions (void)
{
  int n = list.length ();

  if (n > 0)
    {
      for (int i = 0; i < n; i++)
	{
	  scanf_format_elt *elt = list(i);

	  switch (elt->type)
	    {
	    case 'c': case 's': case '%': case '[': case '^':
	    case scanf_format_elt::literal_conversion:
	    case scanf_format_elt::whitespace_conversion:
	      break;

	    default:
	      return false;
	      break;
	    }
	}

      return true;
    }
  else
    return false;
}

bool
scanf_format_list::all_numeric_conversions (void)
{
  int n = list.length ();

  if (n > 0)
    {
      for (int i = 0; i < n; i++)
	{
	  scanf_format_elt *elt = list(i);

	  switch (elt->type)
	    {
	    case 'd': case 'i': case 'o': case 'u': case 'x':
	    case 'e': case 'f': case 'g':
	      break;

	    default:
	      return false;
	      break;
	    }
	}

      return true;
    }
  else
    return false;
}

// Ugh again.

printf_format_list::printf_format_list (const std::string& s)
  : nconv (0), curr_idx (0), list (16), buf (0)
{
  int num_elts = 0;

  int n = s.length ();

  int i = 0;

  int args = 0;
  std::string flags;
  int fw = 0;
  int prec = 0;
  char modifier = '\0';
  char type = '\0';

  bool have_more = true;
  bool empty_buf = true;

  if (n == 0)
    {
      printf_format_elt *elt
	= new printf_format_elt ("", args, fw, prec, flags, type, modifier);

      list(num_elts++) = elt;

      list.resize (num_elts);
    }
  else
    {
      while (i < n)
	{
	  have_more = true;

	  if (! buf)
	    {
	      buf = new OSSTREAM ();
	      empty_buf = true;
	    }

	  switch (s[i])
	    {
	    case '%':
	      {
		if (empty_buf)
		  {
		    process_conversion (s, i, n, args, flags, fw, prec,
					type, modifier, num_elts);

		    have_more = (buf != 0);
		  }
		else
		  add_elt_to_list (args, flags, fw, prec, type, modifier,
				   num_elts);
	      }
	      break;

	    default:
	      {
		args = 0;
		flags = "";
		fw = 0;
		prec = 0;
		modifier = '\0';
		type = '\0';
		*buf << s[i++];
		empty_buf = false;
	      }
	      break;
	    }

	  if (nconv < 0)
	    {
	      have_more = false;
	      break;
	    }
	}

      if (have_more)
	add_elt_to_list (args, flags, fw, prec, type, modifier, num_elts);

      list.resize (num_elts);

      delete buf;
    }
}

printf_format_list::~printf_format_list (void)
{
  int n = list.length ();

  for (int i = 0; i < n; i++)
    {
      printf_format_elt *elt = list(i);
      delete elt;
    }	
}

void
printf_format_list::add_elt_to_list (int args, const std::string& flags,
				     int fw, int prec, char type,
				     char modifier, int& num_elts)
{
  if (buf)
    {
      *buf << OSSTREAM_ENDS;

      std::string text = OSSTREAM_STR (*buf);

      OSSTREAM_FREEZE (*buf);

      if (! text.empty ())
	{
	  printf_format_elt *elt
	    = new printf_format_elt (text.c_str (), args, fw, prec, flags,
				     type, modifier);

	  if (num_elts == list.length ())
	    list.resize (2 * num_elts);

	  list(num_elts++) = elt;
	}

      delete buf;
      buf = 0;
    }
}

void
printf_format_list::process_conversion
  (const std::string& s, int& i, int n, int& args, std::string& flags,
   int& fw, int& prec, char& modifier, char& type, int& num_elts)
{
  args = 0;
  flags = "";
  fw = 0;
  prec = 0;
  modifier = '\0';
  type = '\0';

  *buf << s[i++];

  bool nxt = false;

  while (i < n)
    {
      switch (s[i])
	{
	case '-': case '+': case ' ': case '0': case '#':
	  flags += s[i];
	  *buf << s[i++];
	  break;

	default:
	  nxt = true;
	  break;
	}

      if (nxt)
	break;
    }

  if (i < n)
    {
      if (s[i] == '*')
	{
	  fw = -1;
	  args++;
	  *buf << s[i++];
	}
      else
	{
	  if (isdigit (s[i]))
	    {
	      int nn = 0;
	      std::string tmp = s.substr (i);
	      sscanf (tmp.c_str (), "%d%n", &fw, &nn);
	    }

	  while (i < n && isdigit (s[i]))
	    *buf << s[i++];
	}
    }

  if (i < n && s[i] == '.')
    {
      *buf << s[i++];

      if (i < n)
	{
	  if (s[i] == '*')
	    {
	      prec = -1;
	      args++;
	      *buf << s[i++];
	    }
	  else
	    {
	      if (isdigit (s[i]))
		{
		  int nn = 0;
		  std::string tmp = s.substr (i);
		  sscanf (tmp.c_str (), "%d%n", &prec, &nn);
		}

	      while (i < n && isdigit (s[i]))
		*buf << s[i++];
	    }
	}
    }

  if (i < n)
    {
      switch (s[i])
	{
	case 'h': case 'l': case 'L':
	  modifier = s[i];
	  *buf << s[i++];
	  break;

	default:
	  break;
	}
    }

  if (i < n)
    finish_conversion (s, i, args, flags, fw, prec, modifier, type, num_elts);
  else
    nconv = -1;
}

void
printf_format_list::finish_conversion
  (const std::string& s, int& i, int args, const std::string& flags,
   int fw, int prec, char modifier, char& type, int& num_elts)

{
  switch (s[i])
    {
    case 'd': case 'i': case 'o': case 'x': case 'X':
    case 'u': case 'c':
      if (modifier == 'L')
	{
	  nconv = -1;
	  break;
	}
      goto fini;

    case 'f': case 'e': case 'E': case 'g': case 'G':
      if (modifier == 'h' || modifier == 'l')
	{
	  nconv = -1;
	  break;
	}
      goto fini;

    case 's': case 'p': case '%':
      if (modifier != '\0')
	{
	  nconv = -1;
	  break;
	}
      goto fini;

    fini:

      type = s[i];

      *buf << s[i++];

      if (type != '%' || args != 0)
	nconv++;

      if (type != '%')
	args++;

      add_elt_to_list (args, flags, fw, prec, type, modifier, num_elts);

      break;

    default:
      nconv = -1;
      break;
    }
}

void
printf_format_list::printme (void) const
{
  int n = list.length ();

  for (int i = 0; i < n; i++)
    {
      printf_format_elt *elt = list(i);

      std::cerr
	<< "args:     " << elt->args << "\n"
	<< "flags:    `" << elt->flags << "'\n"
	<< "width:    " << elt->fw << "\n"
	<< "prec:     " << elt->prec << "\n"
	<< "type:     `" << elt->type << "'\n"
	<< "modifier: `" << elt->modifier << "'\n"
	<< "text:     `" << undo_string_escapes (elt->text) << "'\n\n";
    }
}

int
octave_base_stream::file_number (void)
{
  // Kluge alert!

  if (name () == "stdin")
    return 0;

  if (name () == "stdout")
    return 1;

  if (name () == "stderr")
    return 2;

  int retval = -1;

  std::istream *is = input_stream ();
  std::ostream *os = output_stream ();

  // There is no standard way to get the underlying file descriptor from 
  // std::filebuf (nor in the GNU libstdc++-v3 implementation). We cache
  // the descriptor in c_file_ptr_buf, and then extract it here.

  c_file_ptr_buf *ibuf = is ?
    dynamic_cast<c_file_ptr_buf *> (is->rdbuf ()) : 0;
  c_file_ptr_buf *obuf = os ?
    dynamic_cast<c_file_ptr_buf *> (os->rdbuf ()) : 0;

  int i_fid = ibuf ? ibuf->file_number () : -1;
  int o_fid = obuf ? obuf->file_number () : -1;

  if (i_fid >= 0)
    {
      if (o_fid >= 0)
	retval = (i_fid == o_fid) ? i_fid : -1;
      else
	retval = i_fid;
    }
  else if (o_fid >= 0)
    retval = o_fid;

  return retval;
}

void
octave_base_stream::error (const std::string& msg)
{
  fail = true;
  errmsg = msg;
}

void
octave_base_stream::error (const std::string& who, const std::string& msg)
{
  fail = true;
  errmsg = who + msg;
}

void
octave_base_stream::clear (void)
{
  fail = false;
  errmsg = "";
}

void
octave_base_stream::clearerr (void)
{
  std::istream *is = input_stream ();
  std::ostream *os = output_stream ();

  if (is)
    is->clear ();

  if (os)
    os->clear ();
}

// Functions that are defined for all input streams (input streams
// are those that define is).

std::string
octave_base_stream::do_gets (int max_len, bool& err,
			     bool strip_newline, const std::string& who)
{
  std::string retval;

  err = false;

  std::istream *isp = input_stream ();

  if (isp)
    {
      std::istream& is = *isp;

      OSSTREAM buf;

      int c = 0;
      int char_count = 0;
      int newline_stripped = 0;

      while (is && (c = is.get ()) != EOF)
	{
	  char_count++;

	  if (c == '\n')
	    {
	      if (! strip_newline)
		buf << (char) c;
	      else
		newline_stripped = 1;

	      break;
	    }
	  else
	    buf << (char) c;

	  if (max_len > 0 && char_count == max_len)
	    break;
	}

      if (is.good () || (is.eof () && char_count > 0))
	{
	  buf << OSSTREAM_ENDS;
	  retval = OSSTREAM_STR (buf);
	  OSSTREAM_FREEZE (buf);
	}
      else
	{
	  err = true;

	  if (is.eof () && char_count == 0)
	    error (who, "at end of file");
	  else
	    error (who, "read error");
	}
    }
  else
    {
      err = true;
      invalid_operation (who, "reading");
    }

  return retval;
}

std::string
octave_base_stream::getl (int max_len, bool& err, const std::string& who)
{
  return do_gets (max_len, err, true, who);
}

std::string
octave_base_stream::gets (int max_len, bool& err, const std::string& who)
{
  return do_gets (max_len, err, false, who);
}

#if defined (__GNUG__) && ! defined (CXX_ISO_COMPLIANT_LIBRARY)

#define OCTAVE_SCAN(is, fmt, arg) is.scan ((fmt).text, arg)

#else

#define OCTAVE_SCAN(is, fmt, arg) octave_scan (is, fmt, arg)

// XXX FIXME XXX -- this needs to be fixed to handle formats which
// specify a maximum width.

template <class T>
std::istream&
octave_scan (std::istream& is, const scanf_format_elt& fmt, T* valptr)
{
  T& ref = *valptr;

  switch (fmt.type)
    {
    case 'o':
      is >> std::oct >> ref >> std::dec;
      break;

    case 'x':
      is >> std::hex >> ref >> std::dec;
      break;

    case 'i':
      {
	int c1 = is.get ();

	if (! is.eof ())
	  {
	    if (c1 == '0')
	      {
		int c2 = is.get ();

		if (c2 == 'x' || c2 == 'X')
		  is >> std::hex >> ref >> std::dec;
		else
		  {
		    is.putback (c2);

		    if (c2 == '0' || c2 == '1' || c2 == '2'
			|| c2 == '3' || c2 == '4' || c2 == '5'
			|| c2 == '6' || c2 == '7')
		      is >> std::oct >> ref >> std::dec;
		    else
		      ref = 0;
		  }
	      }
	    else
	      {
		is.putback (c1);

		is >> ref;
	      }
	  }
      }
      break;

    default:
      is >> ref;
      break;
    }

  return is;
}

// Note that this specialization is only used for reading characters, not 
// character strings. See BEGIN_S_CONVERSION for details.

template<>
std::istream&
octave_scan<> (std::istream& is, const scanf_format_elt& /* fmt */,
	       char* valptr)
{
  return is >> valptr;
}

template std::istream&
octave_scan (std::istream&, const scanf_format_elt&, int*);

template std::istream&
octave_scan (std::istream&, const scanf_format_elt&, long int*);

template std::istream&
octave_scan (std::istream&, const scanf_format_elt&, short int*);

template std::istream&
octave_scan (std::istream&, const scanf_format_elt&, unsigned int*);

template std::istream&
octave_scan (std::istream&, const scanf_format_elt&, unsigned long int*);

template std::istream&
octave_scan (std::istream&, const scanf_format_elt&, unsigned short int*);

#if 0
template std::istream&
octave_scan (std::istream&, const scanf_format_elt&, float*);
#endif

template <>
std::istream&
octave_scan (std::istream& is, const scanf_format_elt& fmt, double* valptr)
{
  double& ref = *valptr;

  switch (fmt.type)
    {
    case 'e':
    case 'f':
    case 'g':
      {
	int c1;

	while (is && (c1 = is.get ()) != EOF && isspace (c1))
	  /* skip whitespace */;

	if (c1 != EOF)
	  {
	    if (c1 == 'N')
	      {
		int c2 = is.get ();

		if (c2 != EOF)
		  {
		    if (c2 == 'A')
		      {
			int c3 = is.get ();

			if (c3 != EOF)
			  {
			    is.putback (c3);

			    if (isspace (c3) || ispunct (c3))
			      ref = octave_NA;
			    else
			      {
				is.putback (c2);
				is.putback (c1);

				is >> ref;
			      }
			  }
			else
			  {
			    is.clear ();

			    ref = octave_NA;
			  }
		      }
		    else if (c2 == 'a')
		      {
			int c3 = is.get ();

			if (c3 != EOF)
			  {
			    if (c3 == 'N')
			      {
				int c4 = is.get ();

				if (c4 != EOF)
				  {
				    is.putback (c4);

				    if (isspace (c4) || ispunct (c4))
				      ref = octave_NaN;
				    else
				      {
					is.putback (c3);
					is.putback (c2);
					is.putback (c1);

					is >> ref;
				      }
				  }
				else
				  {
				    is.clear ();

				    ref = octave_NaN;
				  }
			      }
			    else
			      {
				is.putback (c3);
				is.putback (c2);
				is.putback (c1);

				is >> ref;
			      }
			  }
		      }
		    else
		      {
			is.putback (c2);
			is.putback (c1);

			is >> ref;
		      }
		  }
	      }
	    else if (c1 == 'I')
	      {
		int c2 = is.get ();

		if (c2 != EOF)
		  {
		    if (c2 == 'n')
		      {
			int c3 = is.get ();

			if (c3 != EOF)

			  if (c3 == 'f')
			    {
			      int c4 = is.get ();

			      if (c4 != EOF)
				{
				  is.putback (c4);

				  if (isspace (c4) || ispunct (c4))
				    ref = octave_Inf;
				  else
				    {
				      is.putback (c3);
				      is.putback (c2);
				      is.putback (c1);

				      is >> ref;
				    }
				}
			      else
				{
				  is.clear ();

				  ref = octave_Inf;
				}
			    }
			  else
			    {
			      is.putback (c3);
			      is.putback (c2);
			      is.putback (c1);

			      is >> ref;
			    }
		      }
		    else
		      {
			is.putback (c2);
			is.putback (c1);

			is >> ref;
		      }
		  }
	      }
	    else
	      {
		is.putback (c1);

		is >> ref;
	      }
	  }
      }
      break;

    default:
      panic_impossible ();
      break;
    }

  return is;
}

template std::istream&
octave_scan (std::istream&, const scanf_format_elt&, double*);

#endif

template <class T>
void
do_scanf_conv (std::istream& is, const scanf_format_elt& fmt,
	       T valptr, Matrix& mval, double *data, int& idx,
	       int& conversion_count, int nr, int max_size,
	       bool discard) 
{
  OCTAVE_SCAN (is, fmt, valptr);

  if (is)
    {
      if (idx == max_size && ! discard)
	{
	  max_size *= 2;

	  if (nr > 0)
	    mval.resize (nr, max_size / nr, 0.0);
	  else
	    mval.resize (max_size, 1, 0.0);

	  data = mval.fortran_vec ();
	}

      if (! discard)
	{
	  conversion_count++;
	  data[idx++] = *(valptr);
	}
    }
}

template void
do_scanf_conv (std::istream&, const scanf_format_elt&, int*,
	       Matrix&, double*, int&, int&, int, int, bool);

template void
do_scanf_conv (std::istream&, const scanf_format_elt&, long int*,
	       Matrix&, double*, int&, int&, int, int, bool);

template void
do_scanf_conv (std::istream&, const scanf_format_elt&, short int*,
	       Matrix&, double*, int&, int&, int, int, bool);

template void
do_scanf_conv (std::istream&, const scanf_format_elt&, unsigned int*,
	       Matrix&, double*, int&, int&, int, int, bool);

template void
do_scanf_conv (std::istream&, const scanf_format_elt&, unsigned long int*,
	       Matrix&, double*, int&, int&, int, int, bool);

template void
do_scanf_conv (std::istream&, const scanf_format_elt&, unsigned short int*,
	       Matrix&, double*, int&, int&, int, int, bool);

#if 0
template void
do_scanf_conv (std::istream&, const scanf_format_elt&, float*,
	       Matrix&, double*, int&, int&, int, int, bool);
#endif

template void
do_scanf_conv (std::istream&, const scanf_format_elt&, double*,
	       Matrix&, double*, int&, int&, int, int, bool);

#define DO_WHITESPACE_CONVERSION() \
  do \
    { \
      int c = EOF; \
 \
      while (is && (c = is.get ()) != EOF && isspace (c)) \
	/* skip whitespace */; \
 \
      if (c != EOF) \
	is.putback (c); \
    } \
  while (0)

#define DO_LITERAL_CONVERSION() \
  do \
    { \
      int c = EOF; \
 \
      int n = strlen (fmt); \
      int i = 0; \
 \
      while (i < n && is && (c = is.get ()) != EOF) \
	{ \
	  if (c == fmt[i]) \
	    { \
	      i++; \
	      continue; \
	    } \
	  else \
	    { \
	      is.putback (c); \
	      break; \
	    } \
	} \
 \
      if (i != n) \
	is.setstate (std::ios::failbit); \
    } \
  while (0)

#define DO_PCT_CONVERSION() \
  do \
    { \
      int c = is.get (); \
 \
      if (c != EOF) \
	{ \
	  if (c != '%') \
	    { \
	      is.putback (c); \
	      is.setstate (std::ios::failbit); \
	    } \
	} \
      else \
	is.setstate (std::ios::failbit); \
    } \
  while (0)

#define BEGIN_C_CONVERSION() \
  is.unsetf (std::ios::skipws); \
 \
  int width = elt->width ? elt->width : 1; \
 \
  char *tbuf = new char[width + 1]; \
 \
  int c = EOF; \
  int n = 0; \
 \
  while (is && n < width && (c = is.get ()) != EOF) \
    tbuf[n++] = (char) c; \
 \
  tbuf[n] = '\0'; \
 \
  std::string tmp = tbuf; \
 \
  delete [] tbuf

// For a `%s' format, skip initial whitespace and then read until the
// next whitespace character.
#define BEGIN_S_CONVERSION() \
  int width = elt->width; \
 \
  std::string tmp; \
 \
  do \
    { \
      if (width) \
	{ \
	  char *tbuf = new char [width+1]; \
 \
	  OCTAVE_SCAN (is, *elt, tbuf); \
 \
	  tbuf[width] = '\0'; \
          tmp = tbuf; \
          delete [] tbuf; \
	} \
      else \
	{ \
	  is >> std::ws >> tmp; \
	} \
    } \
  while (0)

// This format must match a nonempty sequence of characters.
#define BEGIN_CHAR_CLASS_CONVERSION() \
  int width = elt->width; \
 \
  std::string tmp; \
 \
  do \
    { \
      if (width) \
	{ \
	  char *tbuf = new char[width+1]; \
 \
	  OCTAVE_SCAN (is, *elt, tbuf); \
 \
	  tbuf[width] = '\0'; \
          tmp = tbuf; \
          delete [] tbuf; \
	} \
      else \
	{ \
	  OSSTREAM buf; \
 \
	  std::string char_class = elt->char_class; \
 \
	  int c = EOF; \
 \
	  if (elt->type == '[') \
	    { \
	      while (is && (c = is.get ()) != EOF \
		     && char_class.find (c) != NPOS) \
		buf << (char) c; \
	    } \
	  else \
	    { \
	      while (is && (c = is.get ()) != EOF \
		     && char_class.find (c) == NPOS) \
		buf << (char) c; \
	    } \
 \
	  if (c != EOF) \
	    is.putback (c); \
 \
	  buf << OSSTREAM_ENDS; \
	  tmp = OSSTREAM_STR (buf); \
	  OSSTREAM_FREEZE (buf); \
 \
	  if (tmp.empty ()) \
	    is.setstate (std::ios::failbit); \
	} \
    } \
  while (0)

#define FINISH_CHARACTER_CONVERSION() \
  do \
    { \
      width = tmp.length (); \
 \
      if (is) \
	{ \
	  int i = 0; \
 \
	  if (! discard) \
	    { \
	      conversion_count++; \
 \
	      while (i < width && tmp[i] != '\0') \
		{ \
		  if (data_index == max_size) \
		    { \
		      max_size *= 2; \
 \
		      if (all_char_conv) \
			{ \
			  if (one_elt_size_spec) \
			    mval.resize (1, max_size, 0.0); \
			  else if (nr > 0) \
			    mval.resize (nr, max_size / nr, 0.0); \
			  else \
			    panic_impossible (); \
			} \
		      else if (nr > 0) \
			{ \
			  if (nc <= 0) \
			    mval.resize (nr, max_size / nr, 0.0); \
			  else \
			    panic_impossible (); \
			} \
		      else \
			mval.resize (max_size, 1, 0.0); \
 \
		      data = mval.fortran_vec (); \
		    } \
 \
		  data[data_index++] = tmp[i++]; \
		} \
	    } \
	} \
    } \
  while (0)

octave_value
octave_base_stream::do_scanf (scanf_format_list& fmt_list,
			      int nr, int nc, bool one_elt_size_spec,
			      int& conversion_count, const std::string& who)
{
  conversion_count = 0;

  int nconv = fmt_list.num_conversions ();

  int data_index = 0;

  octave_value retval = Matrix ();

  if (nr == 0 || nc == 0)
    {
      if (one_elt_size_spec)
	nc = 0;

      return Matrix (nr, nc, 0.0);
    }

  std::istream *isp = input_stream ();

  bool all_char_conv = fmt_list.all_character_conversions ();

  Matrix mval;
  double *data = 0;
  int max_size = 0;
  int max_conv = 0;

  int final_nr = 0;
  int final_nc = 0;

  if (all_char_conv)
    {
      // Any of these could be resized later (if we have %s
      // conversions, we may read more than one element for each
      // conversion).

      if (one_elt_size_spec)
	{
	  max_size = 512;
	  mval.resize (1, max_size, 0.0);

	  if (nr > 0)
	    max_conv = nr;
	}
      else if (nr > 0)
	{
	  if (nc > 0)
	    {
	      mval.resize (nr, nc, 0.0);
	      max_size = max_conv = nr * nc;
	    }
	  else
	    {
	      mval.resize (nr, 32, 0.0);
	      max_size = nr * 32;
	    }
	}
      else
	panic_impossible ();
    }
  else if (nr > 0)
    {
      if (nc > 0)
	{
	  // Will not resize later.
	  mval.resize (nr, nc, 0.0);
	  max_size = nr * nc;
	  max_conv = max_size;
	}
      else
	{
	  // Maybe resize later.
	  mval.resize (nr, 32, 0.0);
	  max_size = nr * 32;
	}
    }
  else
    {
      // Maybe resize later.
      mval.resize (32, 1, 0.0);
      max_size = 32;
    }

  data = mval.fortran_vec ();

  if (isp)
    {
      std::istream& is = *isp;

      const scanf_format_elt *elt = fmt_list.first ();

      std::ios::fmtflags flags = is.flags ();

      for (;;)
	{
	  OCTAVE_QUIT;

	  if (elt)
	    {
	      if (max_conv > 0 && conversion_count == max_conv)
		{
		  if (all_char_conv && one_elt_size_spec)
		    {
		      final_nr = 1;
		      final_nc = data_index;
		    }
		  else
		    {
		      final_nr = nr;
		      final_nc = (data_index - 1) / nr + 1;
		    }

		  break;
		}
	      else if (data_index == max_size)
		{
		  max_size *= 2;

		  if (all_char_conv)
		    {
		      if (one_elt_size_spec)
			mval.resize (1, max_size, 0.0);
		      else if (nr > 0)
			mval.resize (nr, max_size / nr, 0.0);
		      else
			panic_impossible ();
		    }
		  else if (nr > 0)
		    {
		      if (nc <= 0)
			mval.resize (nr, max_size / nr, 0.0);
		      else
			panic_impossible ();
		    }
		  else
		    mval.resize (max_size, 1, 0.0);

		  data = mval.fortran_vec ();
		}

	      const char *fmt = elt->text;

	      bool discard = elt->discard;

	      switch (elt->type)
		{
		case scanf_format_elt::whitespace_conversion:
		  DO_WHITESPACE_CONVERSION ();
		  break;

		case scanf_format_elt::literal_conversion:
		  DO_LITERAL_CONVERSION ();
		  break;

		case '%':
		  DO_PCT_CONVERSION ();
		  break;

		case 'd': case 'i':
		  {
		    switch (elt->modifier)
		      {
		      case 'h':
			{
			  short int tmp;
			  do_scanf_conv (is, *elt, &tmp, mval, data,
					 data_index, conversion_count,
					 nr, max_size, discard);
			}
			break;

		      case 'l':
			{
			  long int tmp;
			  do_scanf_conv (is, *elt, &tmp, mval, data,
					 data_index, conversion_count,
					 nr, max_size, discard);
			}
			break;

		      default:
			{
			  int tmp;
			  do_scanf_conv (is, *elt, &tmp, mval, data,
					 data_index, conversion_count,
					 nr, max_size, discard);
			}
			break;
		      }
		  }
		  break;

		case 'o': case 'u': case 'x':
		  {
		    switch (elt->modifier)
		      {
		      case 'h':
			{
			  unsigned short int tmp;
			  do_scanf_conv (is, *elt, &tmp, mval, data,
					 data_index, conversion_count,
					 nr, max_size, discard);
			}
			break;

		      case 'l':
			{
			  unsigned long int tmp;
			  do_scanf_conv (is, *elt, &tmp, mval, data,
					 data_index, conversion_count,
					 nr, max_size, discard);
			}
			break;

		      default:
			{
			  unsigned int tmp;
			  do_scanf_conv (is, *elt, &tmp, mval, data,
					 data_index, conversion_count,
					 nr, max_size, discard);
			}
			break;
		      }
		  }
		  break;

		case 'e': case 'f': case 'g':
		  {
		    double tmp;

		    do_scanf_conv (is, *elt, &tmp, mval, data,
				   data_index, conversion_count,
				   nr, max_size, discard);
		  }
		  break;

		case 'c':
		  {
		    BEGIN_C_CONVERSION ();

		    FINISH_CHARACTER_CONVERSION ();

		    is.setf (flags);
		  }
		  break;

		case 's':
		  {
		    BEGIN_S_CONVERSION ();

		    FINISH_CHARACTER_CONVERSION ();
		  }
		  break;

		case '[': case '^':
		  {
		    BEGIN_CHAR_CLASS_CONVERSION ();

		    FINISH_CHARACTER_CONVERSION ();
		  }
		  break;

		case 'p':
		  error ("%s: unsupported format specifier", who.c_str ());
		  break;

		default:
		  error ("%s: internal format error", who.c_str ());
		  break;
		}

	      if (! ok ())
		{
		  break;
		}
	      else if (! is)
		{
		  if (all_char_conv)
		    {
		      if (one_elt_size_spec)
			{
			  final_nr = 1;
			  final_nc = data_index;
			}
		      else if (data_index > nr)
			{
			  final_nr = nr;
			  final_nc = (data_index - 1) / nr + 1;
			}
		      else
			{
			  final_nr = data_index;
			  final_nc = 1;
			}
		    }
		  else if (nr > 0)
		    {
		      if (data_index > nr)
			{
			  final_nr = nr;
			  final_nc = (data_index - 1) / nr + 1;
			}
		      else
			{
			  final_nr = data_index;
			  final_nc = 1;
			}
		    }
		  else
		    {
		      final_nr = data_index;
		      final_nc = 1;
		    }

		  // If it looks like we have a matching failure, then
		  // reset the failbit in the stream state.

		  if (is.rdstate () & std::ios::failbit)
		    is.clear (is.rdstate () & (~std::ios::failbit));

		  // XXX FIXME XXX -- is this the right thing to do?

		  if (interactive && name () == "stdin")
		    {
		      is.clear ();

		      // Skip to end of line.

		      bool err;
		      do_gets (-1, err, false, who);
		    }

		  break;
		}
	    }
	  else
	    {
	      error ("%s: internal format error", who.c_str ());
	      break;
	    }

	  elt = fmt_list.next (nconv > 0);
	}
    }

  if (ok ())
    {
      mval.resize (final_nr, final_nc, 0.0);

      retval = mval;

      if (all_char_conv)
	retval = retval.convert_to_str ();
    }

  return retval;
}

octave_value
octave_base_stream::scanf (const std::string& fmt, const Array<double>& size,
			   int& conversion_count, const std::string& who)
{
  octave_value retval = Matrix ();

  conversion_count = 0;

  std::istream *isp = input_stream ();

  if (isp)
    {
      scanf_format_list fmt_list (fmt);

      if (fmt_list.num_conversions () == -1)
	::error ("%s: invalid format specified", who.c_str ());
      else
	{
	int nr = -1;
	int nc = -1;

	bool one_elt_size_spec;

	get_size (size, nr, nc, one_elt_size_spec, who);

	if (! error_state)
	  retval = do_scanf (fmt_list, nr, nc, one_elt_size_spec,
			     conversion_count, who);
	}
    }
  else
    invalid_operation (who, "reading");

  return retval;
}

bool
octave_base_stream::do_oscanf (const scanf_format_elt *elt,
			       octave_value& retval, const std::string& who)
{
  bool quit = false;

  std::istream *isp = input_stream ();

  if (isp)
    {
      std::istream& is = *isp;

      std::ios::fmtflags flags = is.flags ();

      if (elt)
	{
	  const char *fmt = elt->text;

	  bool discard = elt->discard;

	  switch (elt->type)
	    {
	    case scanf_format_elt::whitespace_conversion:
	      DO_WHITESPACE_CONVERSION ();
	      break;

	    case scanf_format_elt::literal_conversion:
	      DO_LITERAL_CONVERSION ();
	      break;

	    case '%':
	      {
		DO_PCT_CONVERSION ();

		if (! is)
		  quit = true;

	      }
	      break;

	    case 'd': case 'i':
	      {
		int tmp;

		if (OCTAVE_SCAN (is, *elt, &tmp))
		  {
		    if (! discard)
		      retval = tmp;
		  }
		else
		  quit = true;
	      }
	      break;

	    case 'o': case 'u': case 'x':
	      {
		long int tmp;

		if (OCTAVE_SCAN (is, *elt, &tmp))
		  {
		    if (! discard)
		      retval = tmp;
		  }
		else
		  quit = true;
	      }
	      break;

	    case 'e': case 'f': case 'g':
	      {
		double tmp;

		if (OCTAVE_SCAN (is, *elt, &tmp))
		  {
		    if (! discard)
		      retval = tmp;
		  }
		else
		  quit = true;
	      }
	      break;

	    case 'c':
	      {
		BEGIN_C_CONVERSION ();

		if (! discard)
		  retval = tmp;

		if (! is)
		  quit = true;

		is.setf (flags);
	      }
	      break;

	    case 's':
	      {
		BEGIN_S_CONVERSION ();

		if (! discard)
		  retval = tmp;

		if (! is)
		  quit = true;
	      }
	      break;

	    case '[': case '^':
	      {
		BEGIN_CHAR_CLASS_CONVERSION ();

		if (! discard)
		  retval = tmp;

		if (! is)
		  quit = true;
	      }
	      break;

	    case 'p':
	      error ("%s: unsupported format specifier", who.c_str ());
	      break;

	    default:
	      error ("%s: internal format error", who.c_str ());
	      break;
	    }
	}

      if (ok () && is.fail ())
	{
	  error ("%s: read error", who.c_str ());

	  // XXX FIXME XXX -- is this the right thing to do?

	  if (interactive && name () == "stdin")
	    {
	      // Skip to end of line.

	      bool err;
	      do_gets (-1, err, false, who);
	    }
	}
    }

  return quit;
}

octave_value_list
octave_base_stream::oscanf (const std::string& fmt, const std::string& who)
{
  octave_value_list retval;

  std::istream *isp = input_stream ();

  if (isp)
    {
      std::istream& is = *isp;

      scanf_format_list fmt_list (fmt);

      int nconv = fmt_list.num_conversions ();

      if (nconv == -1)
	::error ("%s: invalid format specified", who.c_str ());
      else
	{
	  is.clear ();

	  int len = fmt_list.length ();

	  retval.resize (nconv+1, Matrix ());

	  const scanf_format_elt *elt = fmt_list.first ();

	  int num_values = 0;

	  bool quit = false;

	  for (int i = 0; i < len; i++)
	    {
	      octave_value tmp;

	      quit = do_oscanf (elt, tmp, who);

	      if (quit)
		break;
	      else
		{
		  if (tmp.is_defined ())
		    retval (num_values++) = tmp;

		  if (! ok ())
		    break;

		  elt = fmt_list.next (nconv > 0);
		}
	    }

	  retval(nconv) = num_values;

	  if (! quit)
	    {
	      // Pick up any trailing stuff.
	      if (ok () && len > nconv)
		{
		  octave_value tmp;

		  elt = fmt_list.next ();

		  do_oscanf (elt, tmp, who);
		}
	    }
	}
    }
  else
    invalid_operation (who, "reading");

  return retval;
}

// Functions that are defined for all output streams (output streams
// are those that define os).

int
octave_base_stream::flush (void)
{
  int retval = -1;

  std::ostream *os = output_stream ();

  if (os)
    {
      os->flush ();

      if (os->good ())
	retval = 0;
    }
  else
    invalid_operation ("fflush", "writing");

  return retval;
}

class
printf_value_cache
{
public:

  enum state { ok, conversion_error };

  printf_value_cache (const octave_value_list& args)
    : values (args), val_idx (0), elt_idx (0),
      n_vals (values.length ()), n_elts (0), data (0),
      curr_state (ok) { }

  ~printf_value_cache (void) { }

  // Get the current value as a double and advance the internal pointer.
  double double_value (void);

  // Get the current value as an int and advance the internal pointer.
  int int_value (void);

  // Get the current value as a string and advance the internal pointer.
  std::string string_value (void);

  operator bool () const { return (curr_state == ok); }

  bool exhausted (void) { return (val_idx >= n_vals); }

private:

  const octave_value_list values;
  int val_idx;
  int elt_idx;
  int n_vals;
  int n_elts;
  const double *data;
  NDArray curr_val;
  state curr_state;

  // Must create value cache with values!

  printf_value_cache (void);

  // No copying!

  printf_value_cache (const printf_value_cache&);

  printf_value_cache& operator = (const printf_value_cache&);
};

double
printf_value_cache::double_value (void)
{
  double retval = 0.0;

  if (exhausted ())
    curr_state = conversion_error;

  while (! exhausted ())
    {
      if (! data)
	{
	  octave_value tmp_val = values (val_idx);

	  curr_val = tmp_val.array_value ();

	  if (! error_state)
	    {
	      elt_idx = 0;
	      n_elts = curr_val.length ();
	      data = curr_val.data ();
	    }
	  else
	    {
	      curr_state = conversion_error;
	      break;
	    }
	}

      if (elt_idx < n_elts)
	{
	  retval = data[elt_idx++];

	  if (elt_idx >= n_elts)
	    {
	      elt_idx = 0;
	      val_idx++;
	      data = 0;
	    }

	  break;
	}
      else
	{
	  val_idx++;
	  data = 0;

	  if (n_elts == 0 && exhausted ())
	    curr_state = conversion_error;

	  continue;
	}
    }

  return retval;
}

int
printf_value_cache::int_value (void)
{
  int retval = 0;

  double dval = double_value ();

  if (! error_state)
    {
      if (D_NINT (dval) == dval)
	retval = NINT (dval);
      else
	curr_state = conversion_error;
    }

  return retval;
}

std::string
printf_value_cache::string_value (void)
{
  std::string retval;

  if (exhausted ())
    curr_state = conversion_error;
  else
    {
      octave_value tval = values (val_idx++);

      if (tval.rows () == 1)
	retval = tval.string_value ();
      else
	{
	  // In the name of Matlab compatibility.

	  charMatrix chm = tval.char_matrix_value ();

	  int nr = chm.rows ();
	  int nc = chm.columns ();

	  int k = 0;

	  retval.resize (nr * nc, '\0');

	  for (int j = 0; j < nc; j++)
	    for (int i = 0; i < nr; i++)
	      retval[k++] = chm(i,j);
	}

      if (error_state)
	curr_state = conversion_error;
    }

  return retval;
}

// Ugh again and again.

template <class T>
int
do_printf_conv (std::ostream& os, const char *fmt, int nsa, int sa_1,
		int sa_2, T arg, const std::string& who)
{
  int retval = 0;

  switch (nsa)
    {
    case 2:
      retval = octave_format (os, fmt, sa_1, sa_2, arg);
      break;

    case 1:
      retval = octave_format (os, fmt, sa_1, arg);
      break;

    case 0:
      retval = octave_format (os, fmt, arg);
      break;

    default:
      ::error ("%s: internal error handling format", who.c_str ());
      break;
    }

  return retval;
}

template int
do_printf_conv (std::ostream&, const char*, int, int, int, int,
		const std::string&);

template int
do_printf_conv (std::ostream&, const char*, int, int, int, long,
		const std::string&);

template int
do_printf_conv (std::ostream&, const char*, int, int, int, unsigned int,
		const std::string&);

template int
do_printf_conv (std::ostream&, const char*, int, int, int, unsigned long,
		const std::string&);

template int
do_printf_conv (std::ostream&, const char*, int, int, int, double,
		const std::string&);

template int
do_printf_conv (std::ostream&, const char*, int, int, int, const char*,
		const std::string&);

int
octave_base_stream::do_printf (printf_format_list& fmt_list,
			       const octave_value_list& args,
			       const std::string& who)
{
  int retval = 0;

  int nconv = fmt_list.num_conversions ();

  std::ostream *osp = output_stream ();

  if (osp)
    {
      std::ostream& os = *osp;

      const printf_format_elt *elt = fmt_list.first ();

      printf_value_cache val_cache (args);

      for (;;)
	{
	  OCTAVE_QUIT;

	  if (elt)
	    {
	      // NSA is the number of `star' args to convert.

	      int nsa = (elt->fw < 0) + (elt->prec < 0);

	      int sa_1 = 0;
	      int sa_2 = 0; 

	      if (nsa > 0)
		{
		  sa_1 = val_cache.int_value ();

		  if (! val_cache)
		    break;
		  else
		    {
		      if (nsa > 1)
			{
			  sa_2 = val_cache.int_value ();

			  if (! val_cache)
			    break;
			}
		    }
		}

	      const char *fmt = elt->text;

	      if (elt->type == '%')
		{
		  os << "%";
		  retval++;
		}
	      else if (elt->args == 0 && elt->text)
		{
		  os << elt->text;
		  retval += strlen (elt->text);
		}	      
	      else if (elt->type == 's')
		{
		  std::string val = val_cache.string_value ();

		  if (val_cache)
		    retval += do_printf_conv (os, fmt, nsa, sa_1,
					      sa_2, val.c_str (), who);
		  else
		    break;
		}
	      else
		{
		  double val = val_cache.double_value ();

		  if (val_cache)
		    {
		      if (lo_ieee_is_NaN_or_NA (val) || xisinf (val)
			  || ((val > INT_MAX || val < INT_MIN)
			      && (elt->type == 'd'
				  || elt->type == 'i'
				  || elt->type == 'c'
				  || elt->type == 'o'
				  || elt->type == 'x'
				  || elt->type == 'X'
				  || elt->type == 'u')))
			{
			  std::string tfmt = fmt;

			  if (lo_ieee_is_NaN_or_NA (val) || xisinf (val))
			    {
			      tfmt.replace (tfmt.rfind (elt->type), 1, 1, 's');

			      const char *tval = xisinf (val)
				? (val < 0 ? "-Inf" : "Inf")
				: (lo_ieee_is_NA (val) ? "NA" : "NaN");

			      retval += do_printf_conv (os, tfmt.c_str (),
							nsa, sa_1, sa_2,
							tval, who);
			    }
			  else
			    {
			      tfmt.replace (tfmt.rfind (elt->type), 1, ".f");

			      retval += do_printf_conv (os, tfmt.c_str (),
							nsa, sa_1, sa_2,
							val, who);
			    }
			}
		      else
			{
			  switch (elt->type)
			    {
			    case 'd': case 'i': case 'c':
			      {
				if (elt->modifier == 'l')
				  retval += do_printf_conv
				    (os, fmt, nsa, sa_1, sa_2,
				     static_cast<long int> (val), who);
				else
				  retval += do_printf_conv
				    (os, fmt, nsa, sa_1, sa_2,
				     static_cast<int> (val), who);
			      }
			      break;

			    case 'o': case 'x': case 'X': case 'u':
			      {
				if (elt->modifier == 'l')
				  retval += do_printf_conv
				    (os, fmt, nsa, sa_1, sa_2,
				     static_cast<unsigned long int> (val),
				     who);
				else
				  retval += do_printf_conv
				    (os, fmt, nsa, sa_1, sa_2,
				     static_cast<unsigned int> (val), who);
			      }
			      break;

			    case 'f': case 'e': case 'E':
			    case 'g': case 'G':
			      retval
				+= do_printf_conv (os, fmt, nsa, sa_1, sa_2,
						   val, who);
			      break;

			    default:
			      error ("%s: invalid format specifier",
				     who.c_str ());
			      return -1;
			      break;
			    }
			}
		    }
		  else
		    break;
		}

	      if (! os)
		{
		  error ("%s: write error", who.c_str ());
		  break;
		}
	    }
	  else
	    {
	      ::error ("%s: internal error handling format", who.c_str ());
	      retval = -1;
	      break;
	    }

	  elt = fmt_list.next (nconv > 0 && ! val_cache.exhausted ());

	  if (! elt || (val_cache.exhausted () && elt->args > 0))
	    break;
	}	      
    }
  else
    invalid_operation (who, "writing");

  return retval;
}

int
octave_base_stream::printf (const std::string& fmt,
			    const octave_value_list& args,
			    const std::string& who)
{
  int retval = 0;

  printf_format_list fmt_list (fmt);

  if (fmt_list.num_conversions () == -1)
    ::error ("%s: invalid format specified", who.c_str ());
  else
    retval = do_printf (fmt_list, args, who);

  return retval;
}

int
octave_base_stream::puts (const std::string& s, const std::string& who)
{
  int retval = -1;

  std::ostream *osp = output_stream ();

  if (osp)
    {
      std::ostream& os = *osp;

      os << s;

      if (os)
	{
	  // XXX FIXME XXX -- why does this seem to be necessary?
	  // Without it, output from a loop like
	  //
	  //   for i = 1:100, fputs (stdout, "foo\n"); endfor
	  //
	  // doesn't seem to go to the pager immediately.

	  os.flush ();

	  if (os)
	    retval = 0;
	  else
	    error ("%s: write error", who.c_str ());
	}
      else
	error ("%s: write error", who.c_str ());
    }
  else
    invalid_operation (who, "writing");

  return retval;
}

int
octave_base_stream::rewind (void)
{
  return seek (0, std::ios::beg);
}

// Return current error message for this stream.

std::string
octave_base_stream::error (bool clear_err, int& err_num)
{
  err_num = fail ? -1 : 0;

  std::string tmp = errmsg;

  if (clear_err)
    clear ();

  return tmp;
}

void
octave_base_stream::invalid_operation (const std::string& who, const char *rw)
{
  // Note that this is not ::error () !

  error (who + ": stream not open for " + rw);
}

octave_stream::octave_stream (octave_base_stream *bs)
  : rep (bs)
{
  if (rep)
    rep->count = 1;
}

octave_stream::~octave_stream (void)
{
  if (rep && --rep->count == 0)
    delete rep;
}

octave_stream::octave_stream (const octave_stream& s)
  : rep (s.rep)
{
  if (rep)
    rep->count++;
}

octave_stream&
octave_stream::operator = (const octave_stream& s)
{
  if (rep != s.rep)
    {
      if (rep && --rep->count == 0)
	delete rep;

      rep = s.rep;

      if (rep)
	rep->count++;
    }

  return *this;
}

int
octave_stream::flush (void)
{
  int retval = -1;

  if (stream_ok ("fflush"))
    retval = rep->flush ();

  return retval;
}

std::string
octave_stream::getl (int max_len, bool& err, const std::string& who)
{
  std::string retval;

  if (stream_ok (who))
    retval = rep->getl (max_len, err, who);

  return retval;
}

std::string
octave_stream::getl (const octave_value& tc_max_len, bool& err,
		     const std::string& who)
{
  std::string retval;

  err = false;

  int conv_err = 0;

  int max_len = convert_to_valid_int (tc_max_len, conv_err);

  if (conv_err || max_len < 0)
    {
      err = true;
      ::error ("%s: invalid maximum length specified", who.c_str ());
    }
  else
    retval = getl (max_len, err, who);

  return retval;
}

std::string
octave_stream::gets (int max_len, bool& err, const std::string& who)
{
  std::string retval;

  if (stream_ok (who))
    retval = rep->gets (max_len, err, who);

  return retval;
}

std::string
octave_stream::gets (const octave_value& tc_max_len, bool& err,
		     const std::string& who)
{
  std::string retval;

  err = false;

  int conv_err = 0;

  int max_len = convert_to_valid_int (tc_max_len, conv_err);

  if (conv_err || max_len < 0)
    {
      err = true;
      ::error ("%s: invalid maximum length specified", who.c_str ());
    }
  else
    retval = gets (max_len, err, who);

  return retval;
}

int
octave_stream::seek (long offset, int origin)
{
  int status = -1;

  if (stream_ok ("fseek"))
    {
      clearerr ();

      long orig_pos = rep->tell ();

      status = rep->seek (offset, origin);

      if (status == 0)
	{
	  long save_pos = rep->tell ();

	  rep->seek (0, SEEK_END);

	  long pos_eof = rep->tell ();

	  // I don't think save_pos can be less than zero, but we'll
	  // check anyway...

	  if (save_pos > pos_eof || save_pos < 0)
	    {
	      // Seek outside bounds of file.  Failure should leave
	      // position unchanged.

	      rep->seek (orig_pos, SEEK_SET);

	      status = -1;
	    }
	  else
	    {
	      // Is it possible for this to fail?  We are just
	      // returning to a position after the first successful
	      // seek.

	      rep->seek (save_pos, SEEK_SET);
	    }
	}
    }

  return status;
}

int
octave_stream::seek (const octave_value& tc_offset,
		     const octave_value& tc_origin)
{
  int retval = -1;

  long xoffset = tc_offset.long_value (true);

  if (! error_state)
    {
      int conv_err = 0;

      int origin = SEEK_SET;

      if (tc_origin.is_string ())
	{
	  std::string xorigin = tc_origin.string_value ();

	  if (xorigin == "bof")
	    origin = SEEK_SET;
	  else if (xorigin == "cof")
	    origin = SEEK_CUR;
	  else if (xorigin == "eof")
	    origin = SEEK_END;
	  else
	    conv_err = -1;
	}
      else
	{
	  int xorigin = convert_to_valid_int (tc_origin, conv_err);

	  if (! conv_err)
	    {
	      if (xorigin == -1)
		origin = SEEK_SET;
	      else if (xorigin == 0)
		origin = SEEK_CUR;
	      else if (xorigin == 1)
		origin = SEEK_END;
	      else
		conv_err = -1;
	    }
	}

      if (! conv_err)
	{
	  retval = seek (xoffset, origin);

	  if (retval != 0)
	    error ("fseek: failed to seek to requested position");
	}
      else
	error ("fseek: invalid value for origin");
    }
  else
    error ("fseek: invalid value for offset");

  return retval;
}

long
octave_stream::tell (void)
{
  long retval = -1;

  if (stream_ok ("tell"))
    retval = rep->tell ();

  return retval;
}

int
octave_stream::rewind (void)
{
  int retval = -1;

  if (stream_ok ("frewind"))
    retval = rep->rewind ();

  return retval;
}

bool
octave_stream::is_open (void) const
{
  bool retval = false;

  if (stream_ok ("is_open"))
    retval = rep->is_open ();

  return retval;
}

void
octave_stream::close (void)
{
  if (stream_ok ("close"))
    rep->close ();
}

template <class RET_T, class READ_T>
octave_value
do_read (octave_stream& strm, int nr, int nc, int block_size,
	 int skip, bool do_float_fmt_conv,
	 oct_mach_info::float_format from_flt_fmt, int& count)
{
  octave_value retval;

  RET_T nda;

  count = 0;

  typename octave_array_type_traits<RET_T>::element_type elt_zero
    = typename octave_array_type_traits<RET_T>::element_type ();

  typename octave_array_type_traits<RET_T>::element_type *dat = 0;

  int max_size = 0;

  int final_nr = 0;
  int final_nc = 1;

  if (nr > 0)
    {
      if (nc > 0)
	{
	  nda.resize (dim_vector (nr, nc), elt_zero);
	  dat = nda.fortran_vec ();
	  max_size = nr * nc;
	}
      else
	{
	  nda.resize (dim_vector (nr, 32), elt_zero);
	  dat = nda.fortran_vec ();
	  max_size = nr * 32;
	}
    }
  else
    {
      nda.resize (dim_vector (32, 1), elt_zero);
      dat = nda.fortran_vec ();
      max_size = 32;
    }

  // XXX FIXME XXX -- byte order for Cray?

  bool swap = false;

  if (oct_mach_info::words_big_endian ())
    swap = (from_flt_fmt == oct_mach_info::flt_fmt_ieee_little_endian
	    || from_flt_fmt == oct_mach_info::flt_fmt_vax_g
	    || from_flt_fmt == oct_mach_info::flt_fmt_vax_g);
  else
    swap = (from_flt_fmt == oct_mach_info::flt_fmt_ieee_big_endian);

  union
  {
    char buf[sizeof (typename octave_type_traits<READ_T>::val_type)];
    typename octave_type_traits<READ_T>::val_type val;
  } u;

  std::istream *isp = strm.input_stream ();

  if (isp)
    {
      std::istream& is = *isp;

      int elts_read = 0;

      for (;;)
	{
	  // XXX FIXME XXX -- maybe there should be a special case for
	  // skip == 0.

	  if (is)
	    {
	      if (nr > 0 && nc > 0 && count == max_size)
		{
		  final_nr = nr;
		  final_nc = nc;

		  break;
		}

	      is.read (u.buf, sizeof (typename octave_type_traits<READ_T>::val_type));

	      // We only swap bytes for integer types.  For float
	      // types, the format conversion will also handle byte
	      // swapping.

	      if (swap)
		swap_bytes<sizeof (typename octave_type_traits<READ_T>::val_type)> (u.buf);
	      else if (do_float_fmt_conv)
		do_float_format_conversion
		  (u.buf,
		   sizeof (typename octave_type_traits<READ_T>::val_type),
		   1, from_flt_fmt, oct_mach_info::float_format ());

	      typename octave_array_type_traits<RET_T>::element_type tmp
		= static_cast <typename octave_array_type_traits<RET_T>::element_type> (u.val);

	      if (is)
		{
		  if (count == max_size)
		    {
		      max_size *= 2;

		      if (nr > 0)
			nda.resize (dim_vector (nr, max_size / nr),
				    elt_zero);
		      else
			nda.resize (dim_vector (max_size, 1), elt_zero);

		      dat = nda.fortran_vec ();
		    }

		  dat[count++] = tmp;

		  elts_read++;
		}

	      if (skip != 0 && elts_read == block_size)
		{
		  strm.seek (skip, SEEK_CUR);
		  elts_read = 0;
		}

	      if (is.eof ())
		{
		  if (nr > 0)
		    {
		      if (count > nr)
			{
			  final_nr = nr;
			  final_nc = (count - 1) / nr + 1;
			}
		      else
			{
			  final_nr = count;
			  final_nc = 1;
			}
		    }
		  else
		    {
		      final_nr = count;
		      final_nc = 1;
		    }

		  break;
		}
	    }
	  else if (is.eof ())
	    break;
	}
    }

  nda.resize (dim_vector (final_nr, final_nc), elt_zero);

  retval = nda;

  return retval;
}

#define DO_READ_VAL_TEMPLATE(RET_T, READ_T) \
  template octave_value \
  do_read<RET_T, READ_T> (octave_stream&, int, int, int, int, bool, \
			  oct_mach_info::float_format, int&)

// XXX FIXME XXX -- should we only have float if it is a different
// size from double?

#define INSTANTIATE_DO_READ(VAL_T) \
  DO_READ_VAL_TEMPLATE (VAL_T, octave_int8); \
  DO_READ_VAL_TEMPLATE (VAL_T, octave_uint8); \
  DO_READ_VAL_TEMPLATE (VAL_T, octave_int16); \
  DO_READ_VAL_TEMPLATE (VAL_T, octave_uint16); \
  DO_READ_VAL_TEMPLATE (VAL_T, octave_int32); \
  DO_READ_VAL_TEMPLATE (VAL_T, octave_uint32); \
  DO_READ_VAL_TEMPLATE (VAL_T, octave_int64); \
  DO_READ_VAL_TEMPLATE (VAL_T, octave_uint64); \
  DO_READ_VAL_TEMPLATE (VAL_T, float); \
  DO_READ_VAL_TEMPLATE (VAL_T, double); \
  DO_READ_VAL_TEMPLATE (VAL_T, char); \
  DO_READ_VAL_TEMPLATE (VAL_T, signed char); \
  DO_READ_VAL_TEMPLATE (VAL_T, unsigned char)

INSTANTIATE_DO_READ (int8NDArray);
INSTANTIATE_DO_READ (uint8NDArray);
INSTANTIATE_DO_READ (int16NDArray);
INSTANTIATE_DO_READ (uint16NDArray);
INSTANTIATE_DO_READ (int32NDArray);
INSTANTIATE_DO_READ (uint32NDArray);
INSTANTIATE_DO_READ (int64NDArray);
INSTANTIATE_DO_READ (uint64NDArray);
// INSTANTIATE_DO_READ (floatNDArray);
INSTANTIATE_DO_READ (NDArray);
INSTANTIATE_DO_READ (charNDArray);

typedef octave_value (*read_fptr) (octave_stream&, int, int, int, int, bool,
				   oct_mach_info::float_format ffmt, int&);

INSTANTIATE_ARRAY (read_fptr);
template class Array2<read_fptr>;

#define FILL_TABLE_ROW(R, VAL_T) \
  read_fptr_table(R,oct_data_conv::dt_int8) = do_read<VAL_T, octave_int8>; \
  read_fptr_table(R,oct_data_conv::dt_uint8) = do_read<VAL_T, octave_uint8>; \
  read_fptr_table(R,oct_data_conv::dt_int16) = do_read<VAL_T, octave_int16>; \
  read_fptr_table(R,oct_data_conv::dt_uint16) = do_read<VAL_T, octave_uint16>; \
  read_fptr_table(R,oct_data_conv::dt_int32) = do_read<VAL_T, octave_int32>; \
  read_fptr_table(R,oct_data_conv::dt_uint32) = do_read<VAL_T, octave_uint32>; \
  read_fptr_table(R,oct_data_conv::dt_int64) = do_read<VAL_T, octave_int64>; \
  read_fptr_table(R,oct_data_conv::dt_uint64) = do_read<VAL_T, octave_uint64>; \
  read_fptr_table(R,oct_data_conv::dt_single) = do_read<VAL_T, float>; \
  read_fptr_table(R,oct_data_conv::dt_double) = do_read<VAL_T, double>; \
  read_fptr_table(R,oct_data_conv::dt_char) = do_read<VAL_T, char>; \
  read_fptr_table(R,oct_data_conv::dt_schar) = do_read<VAL_T, signed char>; \
  read_fptr_table(R,oct_data_conv::dt_uchar) = do_read<VAL_T, unsigned char>; \
  read_fptr_table(R,oct_data_conv::dt_logical) = do_read<VAL_T, unsigned char>

octave_value
octave_stream::read (const Array<double>& size, int block_size,
		     oct_data_conv::data_type input_type,
		     oct_data_conv::data_type output_type,
		     int skip, oct_mach_info::float_format ffmt,
		     int& char_count)
{
  static bool initialized = false;

  // Table function pointers for return types x read types.

  static Array2<read_fptr> read_fptr_table (oct_data_conv::dt_unknown, 14, 0);

  if (! initialized)
    {
      FILL_TABLE_ROW (oct_data_conv::dt_int8, int8NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_uint8, uint8NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_int16, int16NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_uint16, uint16NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_int32, int32NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_uint32, uint32NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_int64, int64NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_uint64, uint64NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_double, NDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_char, charNDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_schar, charNDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_uchar, charNDArray);
      FILL_TABLE_ROW (oct_data_conv::dt_logical, boolNDArray);

      initialized = true;
    }

  octave_value retval;

  if (stream_ok ("fread"))
    {
      // XXX FIXME XXX -- we may eventually want to make this extensible.

      // XXX FIXME XXX -- we need a better way to ensure that this
      // numbering stays consistent with the order of the elements in the
      // data_type enum in the oct_data_conv class.

      char_count = 0;

      int nr = -1;
      int nc = -1;

      bool ignore;

      get_size (size, nr, nc, ignore, "fread");

      if (! error_state)
	{
	  if (nr == 0 || nc == 0)
	    retval = Matrix (nr, nc);
	  else
	    {
	      if (ffmt == oct_mach_info::flt_fmt_unknown)
		ffmt = float_format ();

	      read_fptr fcn = read_fptr_table (output_type, input_type);

	      bool do_float_fmt_conv = ((input_type == oct_data_conv::dt_double
					 || input_type == oct_data_conv::dt_single)
					&& ffmt != float_format ());

	      if (fcn)
		{
		  retval = (*fcn) (*this, nr, nc, block_size, skip,
				   do_float_fmt_conv, ffmt, char_count);

		  // XXX FIXME XXX -- kluge!

		  if (! error_state
		      && (output_type == oct_data_conv::dt_char
			  || output_type == oct_data_conv::dt_schar
			  || output_type == oct_data_conv::dt_uchar))
		    retval = octave_value (retval.char_matrix_value (), true);
		}
	      else
		error ("fread: unable to read and convert requested types");
	    }
	}
      else
	invalid_operation ("fread", "reading");
    }

  return retval;
}

int
octave_stream::write (const octave_value& data, int block_size,
		      oct_data_conv::data_type output_type, int skip,
		      oct_mach_info::float_format flt_fmt)
{
  int retval = -1;

  if (stream_ok ("fwrite"))
    {
      if (! error_state)
	{
	  if (flt_fmt == oct_mach_info::flt_fmt_unknown)
	    flt_fmt = float_format ();

	  int status = data.write (*this, block_size, output_type,
				   skip, flt_fmt);

	  if (status < 0)
	    error ("fwrite: write error");
	  else
	    retval = status;
	}
      else
	invalid_operation ("fwrite", "writing");
    }

  return retval;
}

template <class T>
void
write_int (std::ostream& os, bool swap, const T& val)
{
  typename octave_type_traits<T>::val_type tmp = val.value ();

  if (swap)
    swap_bytes<sizeof (typename octave_type_traits<T>::val_type)> (&tmp);

  os.write (reinterpret_cast<const char *> (&tmp),
	    sizeof (typename octave_type_traits<T>::val_type));
}

template void write_int (std::ostream&, bool, const octave_int8&);
template void write_int (std::ostream&, bool, const octave_uint8&);
template void write_int (std::ostream&, bool, const octave_int16&);
template void write_int (std::ostream&, bool, const octave_uint16&);
template void write_int (std::ostream&, bool, const octave_int32&);
template void write_int (std::ostream&, bool, const octave_uint32&);
template void write_int (std::ostream&, bool, const octave_int64&);
template void write_int (std::ostream&, bool, const octave_uint64&);

template <class T>
static inline bool
do_write (std::ostream& os, const T& val, oct_data_conv::data_type output_type,
	  oct_mach_info::float_format flt_fmt, bool swap,
	  bool do_float_conversion)
{
  bool retval = true;

  // For compatibility, Octave converts to the output type, then
  // writes.  This means that truncation happens on the conversion.
  // For example, the following program prints 0:
  //
  //   x = int8 (-1)
  //   f = fopen ("foo.dat", "w");
  //   fwrite (f, x, "unsigned char");
  //   fclose (f);
  //   f = fopen ("foo.dat", "r");
  //   y = fread (f, 1, "unsigned char");
  //   printf ("%d\n", y);

  switch (output_type)
    {
    case oct_data_conv::dt_char:
    case oct_data_conv::dt_schar:
    case oct_data_conv::dt_int8:
      write_int (os, swap, octave_int8 (val));
      break;

    case oct_data_conv::dt_uchar:
    case oct_data_conv::dt_uint8:
      write_int (os, swap, octave_uint8 (val));
      break;

    case oct_data_conv::dt_int16:
      write_int (os, swap, octave_int16 (val));
      break;

    case oct_data_conv::dt_uint16:
      write_int (os, swap, octave_uint16 (val));
      break;

    case oct_data_conv::dt_int32:
      write_int (os, swap, octave_int32 (val));
      break;

    case oct_data_conv::dt_uint32:
      write_int (os, swap, octave_uint32 (val));
      break;

    case oct_data_conv::dt_int64:
      write_int (os, swap, octave_int64 (val));
      break;

    case oct_data_conv::dt_uint64:
      write_int (os, swap, octave_uint64 (val));
      break;

    case oct_data_conv::dt_single:
      {
	float f = static_cast<float> (val);

	if (do_float_conversion)
	  do_float_format_conversion (&f, 1, flt_fmt);

	os.write (reinterpret_cast<const char *> (&f), sizeof (float));
      }
      break;

    case oct_data_conv::dt_double:
      {
	double d = static_cast<double> (val);
	if (do_float_conversion)
	  do_double_format_conversion (&d, 1, flt_fmt);

	os.write (reinterpret_cast<const char *> (&d), sizeof (double));
      }
      break;

    default:
      retval = false;
      (*current_liboctave_error_handler)
	("write: invalid type specification");
      break;
    }

  return retval;
}

template <class T>
int
octave_stream::write (const Array<T>& data, int block_size,
		      oct_data_conv::data_type output_type,
		      int skip, oct_mach_info::float_format flt_fmt)
{
  int retval = -1;

  bool status = true;

  int count = 0;

  const T *d = data.data ();

  int n = data.length ();

  oct_mach_info::float_format native_flt_fmt
    = oct_mach_info::float_format ();

  bool do_float_conversion = (flt_fmt != native_flt_fmt);

  // XXX FIXME XXX -- byte order for Cray?

  bool swap = false;

  if (oct_mach_info::words_big_endian ())
    swap = (flt_fmt == oct_mach_info::flt_fmt_ieee_little_endian
	    || flt_fmt == oct_mach_info::flt_fmt_vax_g
	    || flt_fmt == oct_mach_info::flt_fmt_vax_g);
  else
    swap = (flt_fmt == oct_mach_info::flt_fmt_ieee_big_endian);

  for (int i = 0; i < n; i++)
    {
      std::ostream *osp = output_stream ();

      if (osp)
	{
	  std::ostream& os = *osp;

	  if (skip != 0 && (i % block_size) == 0)
	    seek (skip, SEEK_CUR);

	  if (os)
	    {
	      status = do_write (os, d[i], output_type, flt_fmt, swap,
				 do_float_conversion);

	      if (os && status)
		count++;
	      else
		break;
	    }
	  else
	    {
	      status = false;
	      break;
	    }
	}
      else
	{
	  status = false;
	  break;
	}
    }

  if (status)
    retval = count;

  return retval;
}

template int
octave_stream::write (const Array<char>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<bool>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<double>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<octave_int8>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<octave_uint8>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<octave_int16>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<octave_uint16>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<octave_int32>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<octave_uint32>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<octave_int64>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

template int
octave_stream::write (const Array<octave_uint64>&, int,
		      oct_data_conv::data_type,
		      int, oct_mach_info::float_format);

octave_value
octave_stream::scanf (const std::string& fmt, const Array<double>& size,
		      int& count, const std::string& who)
{
  octave_value retval;

  if (stream_ok (who))
    retval = rep->scanf (fmt, size, count, who);

  return retval;
}

octave_value_list
octave_stream::oscanf (const std::string& fmt, const std::string& who)
{
  octave_value_list retval;

  if (stream_ok (who))
    retval = rep->oscanf (fmt, who);

  return retval;
}

int
octave_stream::printf (const std::string& fmt, const octave_value_list& args,
		       const std::string& who)
{
  int retval = -1;

  if (stream_ok (who))
    retval = rep->printf (fmt, args, who);

  return retval;
}

int
octave_stream::puts (const std::string& s, const std::string& who)
{
  int retval = -1;

  if (stream_ok (who))
    retval = rep->puts (s, who);

  return retval;
}

// XXX FIXME XXX -- maybe this should work for string arrays too.

int
octave_stream::puts (const octave_value& tc_s, const std::string& who)
{
  int retval = -1;

  if (tc_s.is_string ())
    {
      std::string s = tc_s.string_value ();      
      retval = rep->puts (s, who);
    }
  else
    {
      // Note that this is not ::error () !

      error (who + ": argument must be a string");
    }

  return retval;
}

bool
octave_stream::eof (void) const
{
  int retval = -1;

  if (stream_ok ("feof"))
    retval = rep->eof ();

  return retval;
}

std::string
octave_stream::error (bool clear, int& err_num)
{
  std::string retval;

  if (stream_ok ("ferror", false))
    retval = rep->error (clear, err_num);

  return retval;
}

std::string
octave_stream::name (void) const
{
  std::string retval;

  if (stream_ok ("name"))
    retval = rep->name ();

  return retval;
}

int
octave_stream::mode (void) const
{
  int retval = 0;

  if (stream_ok ("mode"))
    retval = rep->mode ();

  return retval;
}

oct_mach_info::float_format
octave_stream::float_format (void) const
{
  oct_mach_info::float_format retval = oct_mach_info::flt_fmt_unknown;

  if (stream_ok ("float_format"))
    retval = rep->float_format ();

  return retval;
}

std::string
octave_stream::mode_as_string (int mode)
{
  std::string retval = "???";
  std::ios::openmode in_mode = static_cast<std::ios::openmode> (mode);

  if (in_mode == std::ios::in)
    retval = "r";
  else if (in_mode == std::ios::out 
           || in_mode == (std::ios::out | std::ios::trunc))
    retval = "w";
  else if (in_mode == (std::ios::out | std::ios::app))
    retval = "a";
  else if (in_mode == (std::ios::in | std::ios::out))
    retval = "r+";
  else if (in_mode == (std::ios::in | std::ios::out | std::ios::trunc))
    retval = "w+";
  else if (in_mode == (std::ios::in | std::ios::out | std::ios::ate))
    retval = "a+";
  else if (in_mode == (std::ios::in | std::ios::binary))
    retval = "rb";
  else if (in_mode == (std::ios::out | std::ios::binary)
	   || in_mode == (std::ios::out | std::ios::trunc | std::ios::binary))
    retval = "wb";
  else if (in_mode == (std::ios::out | std::ios::app | std::ios::binary))
    retval = "ab";
  else if (in_mode == (std::ios::in | std::ios::out | std::ios::binary))
    retval = "r+b";
  else if (in_mode == (std::ios::in | std::ios::out | std::ios::trunc 
		       | std::ios::binary))
    retval = "w+b";
  else if (in_mode == (std::ios::in | std::ios::out | std::ios::ate
		       | std::ios::binary))
    retval = "a+b";

  return retval;
}

void
octave_stream::invalid_stream_error (const std::string& who) const
{
  ::error ("%s: attempt to use invalid I/O stream", who.c_str ());
}

octave_stream_list *octave_stream_list::instance = 0;

bool
octave_stream_list::instance_ok (void)
{
  bool retval = true;

  if (! instance)
    instance = new octave_stream_list ();

  if (! instance)
    {
      ::error ("unable to create stream list object!");

      retval = false;
    }

  return retval;
}

octave_value
octave_stream_list::insert (const octave_stream& os)
{
  return (instance_ok ()) ? instance->do_insert (os) : octave_value (-1.0);
}

octave_stream
octave_stream_list::lookup (int fid, const std::string& who)
{
  return (instance_ok ()) ? instance->do_lookup (fid, who) : octave_stream ();
}

octave_stream
octave_stream_list::lookup (const octave_value& fid, const std::string& who)
{
  return (instance_ok ()) ? instance->do_lookup (fid, who) : octave_stream ();
}

int
octave_stream_list::remove (int fid, const std::string& who)
{
  return (instance_ok ()) ? instance->do_remove (fid, who) : -1;
}

int
octave_stream_list::remove (const octave_value& fid, const std::string& who)
{
  return (instance_ok ()) ? instance->do_remove (fid, who) : -1;
}

void
octave_stream_list::clear (void)
{
  if (instance)
    instance->do_clear ();
}

string_vector
octave_stream_list::get_info (int fid)
{
  return (instance_ok ()) ? instance->do_get_info (fid) : string_vector ();
}

string_vector
octave_stream_list::get_info (const octave_value& fid)
{
  return (instance_ok ()) ? instance->do_get_info (fid) : string_vector ();
}

std::string
octave_stream_list::list_open_files (void)
{
  return (instance_ok ()) ? instance->do_list_open_files () : std::string ();
}

octave_value
octave_stream_list::open_file_numbers (void)
{
  return (instance_ok ())
    ? instance->do_open_file_numbers () : octave_value ();
}

int
octave_stream_list::get_file_number (const octave_value& fid)
{
  return (instance_ok ()) ? instance->do_get_file_number (fid) : -1;
}

octave_value
octave_stream_list::do_insert (const octave_stream& os)
{
  octave_value retval;

  int stream_number = -1;

  // Insert item in first open slot, increasing size of list if
  // necessary.

  for (int i = 0; i < curr_len; i++)
    {
      octave_stream tmp = list(i);

      if (! tmp)
	{
	  list(i) = os;
	  stream_number = i;
	  break;
	}
    }

  if (stream_number < 0)
    {
      int total_len = list.length ();

      if (curr_len == total_len)
	list.resize (total_len * 2);

      list(curr_len) = os;
      stream_number = curr_len;
      curr_len++;
    }

  return octave_value (os, stream_number);
}

static void
gripe_invalid_file_id (int fid, const std::string& who)
{
  if (who.empty ())
    ::error ("invalid stream number = %d", fid);
  else
    ::error ("%s: invalid stream number = %d", who.c_str (), fid);
}

octave_stream
octave_stream_list::do_lookup (int fid, const std::string& who) const
{
  octave_stream retval;

  if (fid >= 0 && fid < curr_len)
    retval = list(fid);
  else
    gripe_invalid_file_id (fid, who);

  return retval;
}

octave_stream
octave_stream_list::do_lookup (const octave_value& fid,
			       const std::string& who) const
{
  octave_stream retval;

  int i = get_file_number (fid);

  if (! error_state)
    retval = do_lookup (i, who);

  return retval;
}

int
octave_stream_list::do_remove (int fid, const std::string& who)
{
  int retval = -1;

  // Can't remove stdin (std::cin), stdout (std::cout), or stderr
  // (std::cerr).

  if (fid > 2 && fid < curr_len)
    {
      octave_stream os = list(fid);

      if (os.is_valid ())
	{
	  os.close ();
	  list(fid) = octave_stream ();
	  retval = 0;
	}
      else
	gripe_invalid_file_id (fid, who);
    }
  else
    gripe_invalid_file_id (fid, who);

  return retval;
}

int
octave_stream_list::do_remove (const octave_value& fid, const std::string& who)
{
  int retval = -1;

  int i = get_file_number (fid);

  if (! error_state)
    retval = do_remove (i, who);

  return retval;
}

void
octave_stream_list::do_clear (void)
{
  // Do flush stdout and stderr.

  list(0) . flush ();
  list(1) . flush ();

  // But don't delete them or stdin.

  for (int i = 3; i < curr_len; i++)
    list(i) = octave_stream ();
}

string_vector
octave_stream_list::do_get_info (int fid) const
{
  string_vector retval;

  octave_stream os = do_lookup (fid);

  if (os.is_valid ())
    {
      retval.resize (3);

      retval(0) = os.name ();
      retval(1) = octave_stream::mode_as_string (os.mode ());
      retval(2) = oct_mach_info::float_format_as_string (os.float_format ());
    }
  else
    ::error ("invalid file id = %d", fid);

  return retval;
}

string_vector
octave_stream_list::do_get_info (const octave_value& fid) const
{
  string_vector retval;

  int conv_err = 0;

  int int_fid = convert_to_valid_int (fid, conv_err);

  if (! conv_err)
    retval = do_get_info (int_fid);
  else
    ::error ("file id must be a file object or integer value");

  return retval;
}

std::string
octave_stream_list::do_list_open_files (void) const
{
  std::string retval;

  OSSTREAM buf;

  buf << "\n"
      << "  number  mode  arch       name\n"
      << "  ------  ----  ----       ----\n";

  for (int i = 0; i < curr_len; i++)
    {
      octave_stream os = list(i);

      buf << "  "
	  << std::setiosflags (std::ios::right)
	  << std::setw (4) << i << "     "
	  << std::setiosflags (std::ios::left)
	  << std::setw (3)
	  << octave_stream::mode_as_string (os.mode ())
	  << "  "
	  << std::setw (9)
	  << oct_mach_info::float_format_as_string (os.float_format ())
	  << "  "
	  << os.name () << "\n";
    }

  buf << "\n" << OSSTREAM_ENDS;

  retval = OSSTREAM_STR (buf);

  OSSTREAM_FREEZE (buf);

  return retval;
}

octave_value
octave_stream_list::do_open_file_numbers (void) const
{
  Matrix retval (1, curr_len, 0.0);

  int num_open = 0;

  // Skip stdin, stdout, and stderr.

  for (int i = 3; i < curr_len; i++)
    {
      if (list(i))
	retval (0, num_open++) = i;
    }

  retval.resize ((num_open > 0), num_open);

  return retval;
}

int
octave_stream_list::do_get_file_number (const octave_value& fid) const
{
  int retval = -1;

  if (fid.is_string ())
    {
      std::string nm = fid.string_value ();

      // stdin (std::cin), stdout (std::cout), and stderr (std::cerr)
      // are unnamed.

      for (int i = 3; i < curr_len; i++)
	{
	  octave_stream os = list(i);

	  if (os && os.name () == nm)
	    {
	      retval = i;
	      break;
	    }
	}
    }
  else
    {
      int conv_err = 0;

      int int_fid = convert_to_valid_int (fid, conv_err);

      if (conv_err)
	::error ("file id must be a file object, std::string, or integer value");
      else
	retval = int_fid;
    }

  return retval;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
