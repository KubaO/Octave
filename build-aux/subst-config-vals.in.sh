#! /bin/sh
#
# Copyright (C) 2016-2017 John W. Eaton
#
# This file is part of Octave.
#
# Octave is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Octave is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Octave; see the file COPYING.  If not, see
# <http://www.gnu.org/licenses/>.

: ${SED=@SED@}

NO_OCT_FILE_STRIP="@NO_OCT_FILE_STRIP@"
bindir="@bindir@"
AMD_CPPFLAGS="@AMD_CPPFLAGS@"
AMD_LDFLAGS="@AMD_LDFLAGS@"
AMD_LIBS="@AMD_LIBS@"
AR="@AR@"
ARFLAGS="@ARFLAGS@"
ARPACK_CPPFLAGS="@ARPACK_CPPFLAGS@"
ARPACK_LDFLAGS="@ARPACK_LDFLAGS@"
ARPACK_LIBS="@ARPACK_LIBS@"
BLAS_LIBS="@BLAS_LIBS@"
CAMD_CPPFLAGS="@CAMD_CPPFLAGS@"
CAMD_LDFLAGS="@CAMD_LDFLAGS@"
CAMD_LIBS="@CAMD_LIBS@"
CARBON_LIBS="@CARBON_LIBS@"
CC="@CC@"
CCOLAMD_CPPFLAGS="@CCOLAMD_CPPFLAGS@"
CCOLAMD_LDFLAGS="@CCOLAMD_LDFLAGS@"
CCOLAMD_LIBS="@CCOLAMD_LIBS@"
CFLAGS="@CFLAGS@"
CHOLMOD_CPPFLAGS="@CHOLMOD_CPPFLAGS@"
CHOLMOD_LDFLAGS="@CHOLMOD_LDFLAGS@"
CHOLMOD_LIBS="@CHOLMOD_LIBS@"
COLAMD_CPPFLAGS="@COLAMD_CPPFLAGS@"
COLAMD_LDFLAGS="@COLAMD_LDFLAGS@"
COLAMD_LIBS="@COLAMD_LIBS@"
CPICFLAG="@CPICFLAG@"
CPPFLAGS="@CPPFLAGS@"
CURL_CPPFLAGS="@CURL_CPPFLAGS@"
CURL_LDFLAGS="@CURL_LDFLAGS@"
CURL_LIBS="@CURL_LIBS@"
CXSPARSE_CPPFLAGS="@CXSPARSE_CPPFLAGS@"
CXSPARSE_LDFLAGS="@CXSPARSE_LDFLAGS@"
CXSPARSE_LIBS="@CXSPARSE_LIBS@"
CXX="@CXX@"
CXXCPP="@CXXCPP@"
CXXFLAGS="@CXXFLAGS@"
CXXPICFLAG="@CXXPICFLAG@"
DEFAULT_PAGER="@DEFAULT_PAGER@"
DEFS="@DEFS@"
DEPEND_FLAGS="@DEPEND_FLAGS@"
DEPEND_EXTRA_SED_PATTERN="@DEPEND_EXTRA_SED_PATTERN@"
DL_LD="@DL_LD@"
DL_LDFLAGS="@DL_LDFLAGS@"
DL_LIBS="@DL_LIBS@"
EXEEXT="@EXEEXT@"
GCC_VERSION="@GCC_VERSION@"
GXX_VERSION="@GXX_VERSION@"
F77="@F77@"
F77_FLOAT_STORE_FLAG="@F77_FLOAT_STORE_FLAG@"
F77_INTEGER_8_FLAG="@F77_INTEGER_8_FLAG@"
FFLAGS="@FFLAGS@"
FFTW3_CPPFLAGS="@FFTW3_CPPFLAGS@"
FFTW3_LDFLAGS="@FFTW3_LDFLAGS@"
FFTW3_LIBS="@FFTW3_LIBS@"
FFTW3F_CPPFLAGS="@FFTW3F_CPPFLAGS@"
FFTW3F_LDFLAGS="@FFTW3F_LDFLAGS@"
FFTW3F_LIBS="@FFTW3F_LIBS@"
FLIBS="@FLIBS@"
FLTK_CPPFLAGS="@FLTK_CPPFLAGS@"
FLTK_LDFLAGS="@FLTK_LDFLAGS@"
FLTK_LIBS="@FLTK_LIBS@"
FONTCONFIG_CPPFLAGS="@FONTCONFIG_CPPFLAGS@"
FONTCONFIG_LIBS="@FONTCONFIG_LIBS@"
FPICFLAG="@FPICFLAG@"
FT2_CPPFLAGS="@FT2_CPPFLAGS@"
FT2_LIBS="@FT2_LIBS@"
GLPK_CPPFLAGS="@GLPK_CPPFLAGS@"
GLPK_LDFLAGS="@GLPK_LDFLAGS@"
GLPK_LIBS="@GLPK_LIBS@"
GNUPLOT="@GNUPLOT@"
HDF5_CPPFLAGS="@HDF5_CPPFLAGS@"
HDF5_LDFLAGS="@HDF5_LDFLAGS@"
HDF5_LIBS="@HDF5_LIBS@"
includedir="@includedir@"
KLU_CPPFLAGS="@KLU_CPPFLAGS@"
KLU_LDFLAGS="@KLU_LDFLAGS@"
KLU_LIBS="@KLU_LIBS@"
LAPACK_LIBS="@LAPACK_LIBS@"
LD_CXX="@LD_CXX@"
LDFLAGS="@LDFLAGS@"
LD_STATIC_FLAG="@LD_STATIC_FLAG@"
LEX="@LEX@"
LEXLIB="@LEXLIB@"
LFLAGS="@LFLAGS@"
libdir="@libdir@"
LIBOCTAVE="@LIBOCTAVE@"
LIBOCTINTERP="@LIBOCTINTERP@"
LIBS="@LIBS@"
LLVM_CPPFLAGS="@LLVM_CPPFLAGS@"
LLVM_LDFLAGS="@LLVM_LDFLAGS@"
LLVM_LIBS="@LLVM_LIBS@"
LN_S="@LN_S@"
MAGICK_CPPFLAGS="@MAGICK_CPPFLAGS@"
MAGICK_LDFLAGS="@MAGICK_LDFLAGS@"
MAGICK_LIBS="@MAGICK_LIBS@"
MKOCTFILE_AR="@MKOCTFILE_AR@"
MKOCTFILE_CC="@MKOCTFILE_CC@"
MKOCTFILE_CXX="@MKOCTFILE_CXX@"
MKOCTFILE_DL_LD="@MKOCTFILE_DL_LD@"
MKOCTFILE_DL_LDFLAGS="@MKOCTFILE_DL_LDFLAGS@"
MKOCTFILE_F77="@MKOCTFILE_F77@"
MKOCTFILE_LD_CXX="@MKOCTFILE_LD_CXX@"
MKOCTFILE_RANLIB="@MKOCTFILE_RANLIB@"
OCTAVE_LINK_DEPS="@OCTAVE_LINK_DEPS@"
OCTAVE_LINK_OPTS="@OCTAVE_LINK_OPTS@"
octincludedir="@octincludedir@"
octlibdir="@octlibdir@"
OCT_LINK_DEPS="@OCT_LINK_DEPS@"
OCT_LINK_OPTS="@OCT_LINK_OPTS@"
OPENGL_LIBS="@OPENGL_LIBS@"
OSMESA_CPPFLAGS="@OSMESA_CPPFLAGS@"
OSMESA_LDFLAGS="@OSMESA_LDFLAGS@"
OSMESA_LIBS="@OSMESA_LIBS@"
PCRE_CPPFLAGS="@PCRE_CPPFLAGS@"
PCRE_LDFLAGS="@PCRE_LDFLAGS@"
PCRE_LIBS="@PCRE_LIBS@"
prefix="@prefix@"
PTHREAD_CFLAGS="@PTHREAD_CFLAGS@"
PTHREAD_LIBS="@PTHREAD_LIBS@"
QHULL_CPPFLAGS="@QHULL_CPPFLAGS@"
QHULL_LDFLAGS="@QHULL_LDFLAGS@"
QHULL_LIBS="@QHULL_LIBS@"
QRUPDATE_CPPFLAGS="@QRUPDATE_CPPFLAGS@"
QRUPDATE_LDFLAGS="@QRUPDATE_LDFLAGS@"
QRUPDATE_LIBS="@QRUPDATE_LIBS@"
QT_CPPFLAGS="@QT_CPPFLAGS@"
QT_LDFLAGS="@QT_LDFLAGS@"
QT_LIBS="@QT_LIBS@"
RANLIB="@RANLIB@"
RDYNAMIC_FLAG="@RDYNAMIC_FLAG@"
READLINE_LIBS="@READLINE_LIBS@"
SED="@SED@"
SHARED_LIBS="@SHARED_LIBS@"
SH_LD="@SH_LD@"
SH_LDFLAGS="@SH_LDFLAGS@"
STATIC_LIBS="@STATIC_LIBS@"
SUITESPARSE_CONFIG_LIBS="@SUITESPARSE_CONFIG_LIBS@"
SUNDIALS_IDA_CPPFLAGS="@SUNDIALS_IDA_CPPFLAGS@"
SUNDIALS_IDA_LDFLAGS="@SUNDIALS_IDA_LDFLAGS@"
SUNDIALS_IDA_LIBS="@SUNDIALS_IDA_LIBS@"
SUNDIALS_NVECSERIAL_CPPFLAGS="@SUNDIALS_NVECSERIAL_CPPFLAGS@"
SUNDIALS_NVECSERIAL_LDFLAGS="@SUNDIALS_NVECSERIAL_LDFLAGS@"
SUNDIALS_NVECSERIAL_LIBS="@SUNDIALS_NVECSERIAL_LIBS@"
TERM_LIBS="@TERM_LIBS@"
UMFPACK_CPPFLAGS="@UMFPACK_CPPFLAGS@"
UMFPACK_LDFLAGS="@UMFPACK_LDFLAGS@"
UMFPACK_LIBS="@UMFPACK_LIBS@"
version="@PACKAGE_VERSION@"
WARN_CFLAGS="@WARN_CFLAGS@"
WARN_CXXFLAGS="@WARN_CXXFLAGS@"
X11_INCFLAGS="@X11_INCFLAGS@"
X11_LIBS="@X11_LIBS@"
XTRA_CFLAGS="@XTRA_CFLAGS@"
XTRA_CXXFLAGS="@XTRA_CXXFLAGS@"
YACC="@YACC@"
YFLAGS="@YFLAGS@"
Z_CPPFLAGS="@Z_CPPFLAGS@"
Z_LDFLAGS="@Z_LDFLAGS@"
Z_LIBS="@Z_LIBS@"
config_opts="@config_opts@"

$SED \
  -e "s|%NO_EDIT_WARNING%|DO NOT EDIT!  Generated automatically by subst-config-vals.|" \
  -e "s|%NO_OCT_FILE_STRIP%|${NO_OCT_FILE_STRIP}|" \
  -e "s|%OCTAVE_BINDIR%|\"${bindir}\"|" \
  -e "s|%OCTAVE_CONF_AMD_CPPFLAGS%|\"${AMD_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_AMD_LDFLAGS%|\"${AMD_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_AMD_LIBS%|\"${AMD_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_AR%|\"${AR}\"|" \
  -e "s|%OCTAVE_CONF_ARFLAGS%|\"${ARFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_ARPACK_CPPFLAGS%|\"${ARPACK_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_ARPACK_LDFLAGS%|\"${ARPACK_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_ARPACK_LIBS%|\"${ARPACK_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_BLAS_LIBS%|\"${BLAS_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_CAMD_CPPFLAGS%|\"${CAMD_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CAMD_LDFLAGS%|\"${CAMD_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CAMD_LIBS%|\"${CAMD_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_CARBON_LIBS%|\"${CARBON_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_CC%|\"${CC}\"|" \
  -e "s|%OCTAVE_CONF_CCOLAMD_CPPFLAGS%|\"${CCOLAMD_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CCOLAMD_LDFLAGS%|\"${CCOLAMD_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CCOLAMD_LIBS%|\"${CCOLAMD_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_CFLAGS%|\"${CFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CHOLMOD_CPPFLAGS%|\"${CHOLMOD_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CHOLMOD_LDFLAGS%|\"${CHOLMOD_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CHOLMOD_LIBS%|\"${CHOLMOD_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_COLAMD_CPPFLAGS%|\"${COLAMD_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_COLAMD_LDFLAGS%|\"${COLAMD_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_COLAMD_LIBS%|\"${COLAMD_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_CPICFLAG%|\"${CPICFLAG}\"|" \
  -e "s|%OCTAVE_CONF_CPPFLAGS%|\"${CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CURL_CPPFLAGS%|\"${CURL_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CURL_LDFLAGS%|\"${CURL_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CURL_LIBS%|\"${CURL_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_CXSPARSE_CPPFLAGS%|\"${CXSPARSE_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CXSPARSE_LDFLAGS%|\"${CXSPARSE_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CXSPARSE_LIBS%|\"${CXSPARSE_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_CXX%|\"${CXX}\"|" \
  -e "s|%OCTAVE_CONF_CXXCPP%|\"${CXXCPP}\"|" \
  -e "s|%OCTAVE_CONF_CXXFLAGS%|\"${CXXFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_CXXPICFLAG%|\"${CXXPICFLAG}\"|" \
  -e "s|%OCTAVE_CONF_DEFAULT_PAGER%|\"${DEFAULT_PAGER}\"|" \
  -e "s|%OCTAVE_CONF_DEFS%|\"${DEFS}\"|" \
  -e "s|%OCTAVE_CONF_DEPEND_FLAGS%|\"${DEPEND_FLAGS}\"|" \
  -e "s|%OCTAVE_CONF_DEPEND_EXTRA_SED_PATTERN%|\"${DEPEND_EXTRA_SED_PATTERN}\"|" \
  -e "s|%OCTAVE_CONF_DL_LD%|\"${DL_LD}\"|" \
  -e "s|%OCTAVE_CONF_DL_LDFLAGS%|\"${DL_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_DL_LIBS%|\"${DL_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_EXEEXT%|\"${EXEEXT}\"|" \
  -e "s|%OCTAVE_CONF_GCC_VERSION%|\"${GCC_VERSION}\"|" \
  -e "s|%OCTAVE_CONF_GXX_VERSION%|\"${GXX_VERSION}\"|" \
  -e "s|%OCTAVE_CONF_F77%|\"${F77}\"|" \
  -e "s|%OCTAVE_CONF_F77_FLOAT_STORE_FLAG%|\"${F77_FLOAT_STORE_FLAG}\"|" \
  -e "s|%OCTAVE_CONF_F77_INTEGER_8_FLAG%|\"${F77_INTEGER_8_FLAG}\"|" \
  -e "s|%OCTAVE_CONF_FFLAGS%|\"${FFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FFTW3_CPPFLAGS%|\"${FFTW3_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FFTW3_LDFLAGS%|\"${FFTW3_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FFTW3_LIBS%|\"${FFTW3_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_FFTW3F_CPPFLAGS%|\"${FFTW3F_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FFTW3F_LDFLAGS%|\"${FFTW3F_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FFTW3F_LIBS%|\"${FFTW3F_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_FLIBS%|\"${FLIBS}\"|" \
  -e "s|%OCTAVE_CONF_FLTK_CPPFLAGS%|\"${FLTK_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FLTK_LDFLAGS%|\"${FLTK_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FLTK_LIBS%|\"${FLTK_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_FONTCONFIG_CPPFLAGS%|\"${FONTCONFIG_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FONTCONFIG_LIBS%|\"${FONTCONFIG_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_FPICFLAG%|\"${FPICFLAG}\"|" \
  -e "s|%OCTAVE_CONF_FT2_CPPFLAGS%|\"${FT2_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_FT2_LIBS%|\"${FT2_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_GLPK_CPPFLAGS%|\"${GLPK_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_GLPK_LDFLAGS%|\"${GLPK_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_GLPK_LIBS%|\"${GLPK_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_GNUPLOT%|\"${GNUPLOT}\"|" \
  -e "s|%OCTAVE_CONF_HDF5_CPPFLAGS%|\"${HDF5_CPPFLAGS}\"|" | \
  $SED -e "s|%OCTAVE_CONF_HDF5_LDFLAGS%|\"${HDF5_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_HDF5_LIBS%|\"${HDF5_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_INCLUDEDIR%|\"${includedir}\"|" \
  -e "s|%OCTAVE_CONF_KLU_CPPFLAGS%|\"${KLU_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_KLU_LDFLAGS%|\"${KLU_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_KLU_LIBS%|\"${KLU_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_LAPACK_LIBS%|\"${LAPACK_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_LD_CXX%|\"${LD_CXX}\"|" \
  -e "s|%OCTAVE_CONF_LDFLAGS%|\"${LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_LD_STATIC_FLAG%|\"${LD_STATIC_FLAG}\"|" \
  -e "s|%OCTAVE_CONF_LEX%|\"${LEX}\"|" \
  -e "s|%OCTAVE_CONF_LEXLIB%|\"${LEXLIB}\"|" \
  -e "s|%OCTAVE_CONF_LFLAGS%|\"${LFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_LIBDIR%|\"${libdir}\"|" \
  -e "s|%OCTAVE_CONF_LIBOCTAVE%|\"${LIBOCTAVE}\"|" \
  -e "s|%OCTAVE_CONF_LIBOCTINTERP%|\"${LIBOCTINTERP}\"|" \
  -e "s|%OCTAVE_CONF_LIBS%|\"${LIBS}\"|" \
  -e "s|%OCTAVE_CONF_LLVM_CPPFLAGS%|\"${LLVM_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_LLVM_LDFLAGS%|\"${LLVM_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_LLVM_LIBS%|\"${LLVM_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_LN_S%|\"${LN_S}\"|" \
  -e "s|%OCTAVE_CONF_MAGICK_CPPFLAGS%|\"${MAGICK_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_MAGICK_LDFLAGS%|\"${MAGICK_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_MAGICK_LIBS%|\"${MAGICK_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_MKOCTFILE_AR%|\"${MKOCTFILE_AR}\"|" \
  -e "s|%OCTAVE_CONF_MKOCTFILE_CC%|\"${MKOCTFILE_CC}\"|" \
  -e "s|%OCTAVE_CONF_MKOCTFILE_CXX%|\"${MKOCTFILE_CXX}\"|" \
  -e "s|%OCTAVE_CONF_MKOCTFILE_DL_LD%|\"${MKOCTFILE_DL_LD}\"|" \
  -e "s|%OCTAVE_CONF_MKOCTFILE_DL_LDFLAGS%|\"${MKOCTFILE_DL_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_MKOCTFILE_F77%|\"${MKOCTFILE_F77}\"|" \
  -e "s|%OCTAVE_CONF_MKOCTFILE_LD_CXX%|\"${MKOCTFILE_LD_CXX}\"|" \
  -e "s|%OCTAVE_CONF_MKOCTFILE_RANLIB%|\"${MKOCTFILE_RANLIB}\"|" \
  -e "s|%OCTAVE_CONF_OCTAVE_LINK_DEPS%|\"${OCTAVE_LINK_DEPS}\"|" \
  -e "s|%OCTAVE_CONF_OCTAVE_LINK_OPTS%|\"${OCTAVE_LINK_OPTS}\"|" \
  -e "s|%OCTAVE_CONF_OCTINCLUDEDIR%|\"${octincludedir}\"|" \
  -e "s|%OCTAVE_CONF_OCTLIBDIR%|\"${octlibdir}\"|" \
  -e "s|%OCTAVE_CONF_OCT_LINK_DEPS%|\"${OCT_LINK_DEPS}\"|" \
  -e "s|%OCTAVE_CONF_OCT_LINK_OPTS%|\"${OCT_LINK_OPTS}\"|" \
  -e "s|%OCTAVE_CONF_OPENGL_LIBS%|\"${OPENGL_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_OSMESA_CPPFLAGS%|\"${OSMESA_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_OSMESA_LDFLAGS%|\"${OSMESA_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_OSMESA_LIBS%|\"${OSMESA_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_PCRE_CPPFLAGS%|\"${PCRE_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_PCRE_LDFLAGS%|\"${PCRE_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_PCRE_LIBS%|\"${PCRE_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_PREFIX%|\"${prefix}\"|" \
  -e "s|%OCTAVE_CONF_PTHREAD_CFLAGS%|\"${PTHREAD_CFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_PTHREAD_LIBS%|\"${PTHREAD_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_QHULL_CPPFLAGS%|\"${QHULL_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_QHULL_LDFLAGS%|\"${QHULL_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_QHULL_LIBS%|\"${QHULL_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_QRUPDATE_CPPFLAGS%|\"${QRUPDATE_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_QRUPDATE_LDFLAGS%|\"${QRUPDATE_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_QRUPDATE_LIBS%|\"${QRUPDATE_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_QT_CPPFLAGS%|\"${QT_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_QT_LDFLAGS%|\"${QT_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_QT_LIBS%|\"${QT_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_RANLIB%|\"${RANLIB}\"|" \
  -e "s|%OCTAVE_CONF_RDYNAMIC_FLAG%|\"${RDYNAMIC_FLAG}\"|" \
  -e "s|%OCTAVE_CONF_READLINE_LIBS%|\"${READLINE_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_SED%|\"${SED}\"|" \
  -e "s|%OCTAVE_CONF_SHARED_LIBS%|\"${SHARED_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_SH_LD%|\"${SH_LD}\"|" \
  -e "s|%OCTAVE_CONF_SH_LDFLAGS%|\"${SH_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_STATIC_LIBS%|\"${STATIC_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_SUITESPARSE_CONFIG_LIBS%|\"${SUITESPARSE_CONFIG_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_SUNDIALS_IDA_CPPFLAGS%|\"${SUNDIALS_IDA_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_SUNDIALS_IDA_LDFLAGS%|\"${SUNDIALS_IDA_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_SUNDIALS_IDA_LIBS%|\"${SUNDIALS_IDA_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_SUNDIALS_NVECSERIAL_CPPFLAGS%|\"${SUNDIALS_NVECSERIAL_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_SUNDIALS_NVECSERIAL_LDFLAGS%|\"${SUNDIALS_NVECSERIAL_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_SUNDIALS_NVECSERIAL_LIBS%|\"${SUNDIALS_NVECSERIAL_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_TERM_LIBS%|\"${TERM_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_UMFPACK_CPPFLAGS%|\"${UMFPACK_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_UMFPACK_LDFLAGS%|\"${UMFPACK_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_UMFPACK_LIBS%|\"${UMFPACK_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_VERSION%|\"${version}\"|" \
  -e "s|%OCTAVE_CONF_WARN_CFLAGS%|\"${WARN_CFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_WARN_CXXFLAGS%|\"${WARN_CXXFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_X11_INCFLAGS%|\"${X11_INCFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_X11_LIBS%|\"${X11_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_XTRA_CFLAGS%|\"${XTRA_CFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_XTRA_CXXFLAGS%|\"${XTRA_CXXFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_YACC%|\"${YACC}\"|" \
  -e "s|%OCTAVE_CONF_YFLAGS%|\"${YFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_Z_CPPFLAGS%|\"${Z_CPPFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_Z_LDFLAGS%|\"${Z_LDFLAGS}\"|" \
  -e "s|%OCTAVE_CONF_Z_LIBS%|\"${Z_LIBS}\"|" \
  -e "s|%OCTAVE_CONF_config_opts%|\"${config_opts}\"|"
