//                                  -*- C++ -*-
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

#if !defined (octave_QR_h)
#define octave_QR_h 1

#if defined (__GNUG__)
#pragma interface
#endif

class ostream;

#include "dMatrix.h"

extern "C++" {

class QR
{
public:

  enum type
    {
      std,
      raw,
      economy,
    };

  QR (void) {}

  QR (const Matrix& A, type qr_type = QR::std);

  QR (const QR& a);

  QR& operator = (const QR& a);

  Matrix Q (void) const;
  Matrix R (void) const;

  friend ostream&  operator << (ostream& os, const QR& a);

protected:

  Matrix q;
  Matrix r;
};

inline QR::QR (const QR& a)
{
  q = a.q;
  r = a.r;
}

inline QR& QR::operator = (const QR& a)
{
  q = a.q;
  r = a.r;
  return *this;
}

inline Matrix QR::Q (void) const
{
  return q;
}

inline Matrix QR::R (void) const
{
  return r;
}

} // extern "C++"

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
