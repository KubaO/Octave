//                                  -*- C++ -*-
/*

Copyright (C) 1996 John W. Eaton

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

#if !defined (octave_Matrix_int_h)
#define octave_Matrix_int_h 1

#if defined (__GNUG__)
#pragma interface
#endif

// For FILE...
#include <cstdio>

#include "MArray.h"

#include "mx-defs.h"

class Matrix : public MArray2<double>
{
friend class ComplexMatrix;
friend class AEPBAL;
friend class CHOL;
friend class GEPBAL;
friend class HESS;
friend class LU;
friend class QR;
friend class QRP;
friend class SCHUR;
friend class SVD;

public:

  Matrix (void) : MArray2<double> () { }
  Matrix (int r, int c) : MArray2<double> (r, c) { }
  Matrix (int r, int c, double val) : MArray2<double> (r, c, val) { }
  Matrix (const MArray2<double>& a) : MArray2<double> (a) { }
  Matrix (const Matrix& a) : MArray2<double> (a) { }
  Matrix (const MDiagArray<double>& a) : MArray2<double> (a) { }
  Matrix (const DiagMatrix& a);

  Matrix (const charMatrix& a);

  Matrix& operator = (const Matrix& a)
    {
      MArray2<double>::operator = (a);
      return *this;
    }

  int operator == (const Matrix& a) const;
  int operator != (const Matrix& a) const;

  // destructive insert/delete/reorder operations

  Matrix& insert (const Matrix& a, int r, int c);
  Matrix& insert (const RowVector& a, int r, int c);
  Matrix& insert (const ColumnVector& a, int r, int c);
  Matrix& insert (const DiagMatrix& a, int r, int c);

  Matrix& fill (double val);
  Matrix& fill (double val, int r1, int c1, int r2, int c2);

  Matrix append (const Matrix& a) const;
  Matrix append (const RowVector& a) const;
  Matrix append (const ColumnVector& a) const;
  Matrix append (const DiagMatrix& a) const;

  Matrix stack (const Matrix& a) const;
  Matrix stack (const RowVector& a) const;
  Matrix stack (const ColumnVector& a) const;
  Matrix stack (const DiagMatrix& a) const;

  Matrix transpose (void) const;

  friend Matrix real (const ComplexMatrix& a);
  friend Matrix imag (const ComplexMatrix& a);

  // resize is the destructive equivalent for this one

  Matrix extract (int r1, int c1, int r2, int c2) const;

  // extract row or column i.

  RowVector row (int i) const;
  RowVector row (char *s) const;

  ColumnVector column (int i) const;
  ColumnVector column (char *s) const;

  Matrix inverse (void) const;
  Matrix inverse (int& info) const;
  Matrix inverse (int& info, double& rcond, int force = 0) const;

  Matrix pseudo_inverse (double tol = 0.0);

  ComplexMatrix fourier (void) const;
  ComplexMatrix ifourier (void) const;

  ComplexMatrix fourier2d (void) const;
  ComplexMatrix ifourier2d (void) const;

  DET determinant (void) const;
  DET determinant (int& info) const;
  DET determinant (int& info, double& rcond) const;

  Matrix solve (const Matrix& b) const;
  Matrix solve (const Matrix& b, int& info) const;
  Matrix solve (const Matrix& b, int& info, double& rcond) const;

  ComplexMatrix solve (const ComplexMatrix& b) const;
  ComplexMatrix solve (const ComplexMatrix& b, int& info) const;
  ComplexMatrix solve (const ComplexMatrix& b, int& info, double& rcond) const;

  ColumnVector solve (const ColumnVector& b) const;
  ColumnVector solve (const ColumnVector& b, int& info) const;
  ColumnVector solve (const ColumnVector& b, int& info, double& rcond) const;

  ComplexColumnVector solve (const ComplexColumnVector& b) const;
  ComplexColumnVector solve (const ComplexColumnVector& b, int& info) const;
  ComplexColumnVector solve (const ComplexColumnVector& b, int& info,
			     double& rcond) const;

  Matrix lssolve (const Matrix& b) const;
  Matrix lssolve (const Matrix& b, int& info) const;
  Matrix lssolve (const Matrix& b, int& info, int& rank) const;

  ComplexMatrix lssolve (const ComplexMatrix& b) const;
  ComplexMatrix lssolve (const ComplexMatrix& b, int& info) const;
  ComplexMatrix lssolve (const ComplexMatrix& b, int& info,
			 int& rank) const;

  ColumnVector lssolve (const ColumnVector& b) const;
  ColumnVector lssolve (const ColumnVector& b, int& info) const;
  ColumnVector lssolve (const ColumnVector& b, int& info, int& rank) const;

  ComplexColumnVector lssolve (const ComplexColumnVector& b) const;
  ComplexColumnVector lssolve (const ComplexColumnVector& b, int& info) const;
  ComplexColumnVector lssolve (const ComplexColumnVector& b, int& info,
			       int& rank) const;

  Matrix expm (void) const;

  Matrix& operator += (const Matrix& a);
  Matrix& operator -= (const Matrix& a);

  Matrix& operator += (const DiagMatrix& a);
  Matrix& operator -= (const DiagMatrix& a);

  // unary operations

  Matrix operator ! (void) const;

  // column vector by row vector -> matrix operations

  friend Matrix operator * (const ColumnVector& a, const RowVector& a);

  // diagonal matrix by scalar -> matrix operations

  friend Matrix operator + (const DiagMatrix& a, double s);
  friend Matrix operator - (const DiagMatrix& a, double s);

  // scalar by diagonal matrix -> matrix operations

  friend Matrix operator + (double s, const DiagMatrix& a);
  friend Matrix operator - (double s, const DiagMatrix& a);

  // matrix by diagonal matrix -> matrix operations

  friend Matrix operator + (const Matrix& a, const DiagMatrix& b);
  friend Matrix operator - (const Matrix& a, const DiagMatrix& b);
  friend Matrix operator * (const Matrix& a, const DiagMatrix& b);

  // diagonal matrix by matrix -> matrix operations

  friend Matrix operator + (const DiagMatrix& a, const Matrix& b);
  friend Matrix operator - (const DiagMatrix& a, const Matrix& b);
  friend Matrix operator * (const DiagMatrix& a, const Matrix& b);

  // matrix by matrix -> matrix operations

  friend Matrix operator * (const Matrix& a, const Matrix& b);

  // other operations

  friend Matrix map (d_d_Mapper f, const Matrix& a);
  friend Matrix map (d_c_Mapper f, const ComplexMatrix& a);
  void map (d_d_Mapper f);

  Matrix all (void) const;
  Matrix any (void) const;

  Matrix cumprod (void) const;
  Matrix cumsum (void) const;
  Matrix prod (void) const;
  Matrix sum (void) const;
  Matrix sumsq (void) const;

  ColumnVector diag (void) const;
  ColumnVector diag (int k) const;

  ColumnVector row_min (void) const;
  ColumnVector row_min_loc (void) const;

  ColumnVector row_max (void) const;
  ColumnVector row_max_loc (void) const;

  RowVector column_min (void) const;
  RowVector column_min_loc (void) const;

  RowVector column_max (void) const;
  RowVector column_max_loc (void) const;

  // i/o

  friend ostream& operator << (ostream& os, const Matrix& a);
  friend istream& operator >> (istream& is, Matrix& a);

  int read (FILE *fptr, const char *type);
  int write (FILE *fptr, const char *type);

private:

  Matrix (double *d, int r, int c) : MArray2<double> (d, r, c) { }
};

Matrix Givens (double, double);

Matrix Sylvester (const Matrix&, const Matrix&, const Matrix&);

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; page-delimiter: "^/\\*" ***
;;; End: ***
*/
