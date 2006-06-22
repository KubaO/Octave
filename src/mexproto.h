/*

Copyright (C) 2006 Paul Kienzle

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
Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.

*/

/*

This code was originally distributed as part of Octave Forge under
the follwoing terms:

Author: Paul Kienzle
I grant this code to the public domain.
2001-03-22

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

*/

/* mex.h is for use in C-programs only; do NOT include it in mex.cc */

#if ! defined (MEXPROTO_H)
#define MEXPROTO_H

#if defined (__cplusplus)
extern "C" {
#endif

/* Floating point representation.  */
extern int mxIsNaN (double v);
extern int mxIsFinite (double v);
extern int mxIsInf (double v);
extern double mxGetEps (void);
extern double mxGetInf (void);
extern double mxGetNaN (void);
  
/* Interface to the interpreter */
extern const char *mexFunctionName (void);

extern int mexCallMATLAB (int nargout, mxArray *argout[], int nargin,
			  mxArray *argin[], const char *fname);

extern void mexSetTrapFlag (int flag);
extern int mexEvalString (const char *s);
extern void mexErrMsgTxt (const char *s);
extern void mexWarnMsgTxt (const char *s);
extern void mexPrintf (const char *fmt, ...);
  
extern mxArray *mexGetArray (const char *name, const char *space);
extern mxArray *mexGetArrayPtr (const char *name, const char *space);

extern int mexPutVariable (const char *space, const char *name, mxArray *ptr);

/* Memory.  */
extern void *mxMalloc (int n);
extern void *mxCalloc (int n, int size);
extern void mxFree (void *ptr);
extern void mexMakeArrayPersistent (mxArray *ptr);
extern void mexMakeMemoryPersistent (void *ptr);
  
/* Interpreter values.  */
extern mxArray *mxCreateDoubleMatrix (int nr, int nc, int iscomplex);
extern mxArray *mxCreateDoubleScalar (double val);
extern mxArray *mxCreateLogicalScalar (int val);

extern void mxDestroyArray (mxArray *v);

extern mxArray *mxDuplicateArray (const mxArray *v);

extern int mxIsChar (const mxArray *ptr);

extern int mxIsSparse (const mxArray *ptr);
extern int mxIsStruct (const mxArray *ptr);
extern int mxIsFull (const mxArray *ptr);
extern int mxIsDouble (const mxArray *ptr);
extern int mxIsNumeric (const mxArray *ptr);
extern int mxIsComplex (const mxArray *ptr);
extern int mxIsEmpty (const mxArray *ptr);
extern int mxIsLogicalScalar (const mxArray *ptr);
extern int mxGetM (const mxArray *ptr);
extern int mxGetN (const mxArray *ptr);
extern int mxGetNumberOfDimensions (const mxArray *ptr);
extern int mxGetNumberOfElements (const mxArray *ptr);
extern double *mxGetPr (const mxArray *ptr);

/* Structure support.  */
extern int mxIsStruct (const mxArray *ptr);
extern mxArray *mxGetField (const mxArray *ptr, int index, const char *key);
extern void mxSetField (mxArray *ptr, int index, const char *key, mxArray *val);
extern int mxGetNumberOfFields (const mxArray *ptr);
extern const char *mxGetFieldNameByNumber (const mxArray *ptr, int key_num);
extern int mxGetFieldNumber (const mxArray *ptr, const char *key);
extern mxArray *mxGetFieldByNumber (const mxArray *ptr, int index, int key_num);
extern void mxSetFieldByNumber (mxArray *ptr, int index, int key_num,
				mxArray *val);
extern mxArray *mxCreateStructMatrix (int rows, int cols, int num_keys,
				      const char **keys);
#if 0
extern mxArray *mxCreateStructArray (int num_dims, const int  *dims, 
				     int numkeys, const char **keys);
#endif

extern double *mxGetPi (const mxArray *ptr);
extern void mxSetM (mxArray *ptr, int M);
extern void mxSetN (mxArray *ptr, int N);
extern void mxSetPr (mxArray *ptr, double *pr);
extern void mxSetPi (mxArray *ptr, double *pi);
  
extern int mxGetString (const mxArray *ptr, char *buf, int buflen);
extern char *mxArrayToString (const mxArray *ptr);
extern mxArray *mxCreateString (const char *str);
  
extern double mxGetScalar (const mxArray *ptr);
  
#if defined (__cplusplus)
}
#endif

#endif

/*
;;; Local Variables: ***
;;; mode: C ***
;;; End: ***
*/
