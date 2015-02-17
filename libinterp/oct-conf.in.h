// %NO_EDIT_WARNING%
/*

Copyright (C) 1996-2015 John W. Eaton

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

#if !defined (octave_conf_h)
#define octave_conf_h 1

#ifndef OCTAVE_CONF_ALL_CFLAGS
#define OCTAVE_CONF_ALL_CFLAGS %OCTAVE_CONF_ALL_CFLAGS%
#endif

#ifndef OCTAVE_CONF_ALL_CXXFLAGS
#define OCTAVE_CONF_ALL_CXXFLAGS %OCTAVE_CONF_ALL_CXXFLAGS%
#endif

#ifndef OCTAVE_CONF_ALL_FFLAGS
#define OCTAVE_CONF_ALL_FFLAGS %OCTAVE_CONF_ALL_FFLAGS%
#endif

#ifndef OCTAVE_CONF_ALL_LDFLAGS
#define OCTAVE_CONF_ALL_LDFLAGS %OCTAVE_CONF_ALL_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_AMD_CPPFLAGS
#define OCTAVE_CONF_AMD_CPPFLAGS %OCTAVE_CONF_AMD_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_AMD_LDFLAGS
#define OCTAVE_CONF_AMD_LDFLAGS %OCTAVE_CONF_AMD_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_AMD_LIBS
#define OCTAVE_CONF_AMD_LIBS %OCTAVE_CONF_AMD_LIBS%
#endif

#ifndef OCTAVE_CONF_ARFLAGS
#define OCTAVE_CONF_ARFLAGS %OCTAVE_CONF_ARFLAGS%
#endif

#ifndef OCTAVE_CONF_AR
#define OCTAVE_CONF_AR %OCTAVE_CONF_AR%
#endif

#ifndef OCTAVE_CONF_ARPACK_CPPFLAGS
#define OCTAVE_CONF_ARPACK_CPPFLAGS %OCTAVE_CONF_ARPACK_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_ARPACK_LDFLAGS
#define OCTAVE_CONF_ARPACK_LDFLAGS %OCTAVE_CONF_ARPACK_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_ARPACK_LIBS
#define OCTAVE_CONF_ARPACK_LIBS %OCTAVE_CONF_ARPACK_LIBS%
#endif

#ifndef OCTAVE_CONF_BLAS_LIBS
#define OCTAVE_CONF_BLAS_LIBS %OCTAVE_CONF_BLAS_LIBS%
#endif

#ifndef OCTAVE_CONF_CAMD_CPPFLAGS
#define OCTAVE_CONF_CAMD_CPPFLAGS %OCTAVE_CONF_CAMD_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_CAMD_LDFLAGS
#define OCTAVE_CONF_CAMD_LDFLAGS %OCTAVE_CONF_CAMD_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_CAMD_LIBS
#define OCTAVE_CONF_CAMD_LIBS %OCTAVE_CONF_CAMD_LIBS%
#endif

#ifndef OCTAVE_CONF_CARBON_LIBS
#define OCTAVE_CONF_CARBON_LIBS %OCTAVE_CONF_CARBON_LIBS%
#endif

#ifndef OCTAVE_CONF_CC
#define OCTAVE_CONF_CC %OCTAVE_CONF_CC%
#endif

// FIXME: OCTAVE_CONF_CCC_VERSION is deprecated.  Remove in version 3.12
#ifndef OCTAVE_CONF_CC_VERSION
#define OCTAVE_CONF_CC_VERSION %OCTAVE_CONF_CC_VERSION%
#endif

#ifndef OCTAVE_CONF_CCOLAMD_CPPFLAGS
#define OCTAVE_CONF_CCOLAMD_CPPFLAGS %OCTAVE_CONF_CCOLAMD_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_CCOLAMD_LDFLAGS
#define OCTAVE_CONF_CCOLAMD_LDFLAGS %OCTAVE_CONF_CCOLAMD_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_CCOLAMD_LIBS
#define OCTAVE_CONF_CCOLAMD_LIBS %OCTAVE_CONF_CCOLAMD_LIBS%
#endif

#ifndef OCTAVE_CONF_CFLAGS
#define OCTAVE_CONF_CFLAGS %OCTAVE_CONF_CFLAGS%
#endif

#ifndef OCTAVE_CONF_CHOLMOD_CPPFLAGS
#define OCTAVE_CONF_CHOLMOD_CPPFLAGS %OCTAVE_CONF_CHOLMOD_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_CHOLMOD_LDFLAGS
#define OCTAVE_CONF_CHOLMOD_LDFLAGS %OCTAVE_CONF_CHOLMOD_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_CHOLMOD_LIBS
#define OCTAVE_CONF_CHOLMOD_LIBS %OCTAVE_CONF_CHOLMOD_LIBS%
#endif

#ifndef OCTAVE_CONF_COLAMD_CPPFLAGS
#define OCTAVE_CONF_COLAMD_CPPFLAGS %OCTAVE_CONF_COLAMD_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_COLAMD_LDFLAGS
#define OCTAVE_CONF_COLAMD_LDFLAGS %OCTAVE_CONF_COLAMD_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_COLAMD_LIBS
#define OCTAVE_CONF_COLAMD_LIBS %OCTAVE_CONF_COLAMD_LIBS%
#endif

#ifndef OCTAVE_CONF_CPICFLAG
#define OCTAVE_CONF_CPICFLAG %OCTAVE_CONF_CPICFLAG%
#endif

#ifndef OCTAVE_CONF_CPPFLAGS
#define OCTAVE_CONF_CPPFLAGS %OCTAVE_CONF_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_CURL_CPPFLAGS
#define OCTAVE_CONF_CURL_CPPFLAGS %OCTAVE_CONF_CURL_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_CURL_LDFLAGS
#define OCTAVE_CONF_CURL_LDFLAGS %OCTAVE_CONF_CURL_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_CURL_LIBS
#define OCTAVE_CONF_CURL_LIBS %OCTAVE_CONF_CURL_LIBS%
#endif

#ifndef OCTAVE_CONF_CXSPARSE_CPPFLAGS
#define OCTAVE_CONF_CXSPARSE_CPPFLAGS %OCTAVE_CONF_CXSPARSE_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_CXSPARSE_LDFLAGS
#define OCTAVE_CONF_CXSPARSE_LDFLAGS %OCTAVE_CONF_CXSPARSE_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_CXSPARSE_LIBS
#define OCTAVE_CONF_CXSPARSE_LIBS %OCTAVE_CONF_CXSPARSE_LIBS%
#endif

#ifndef OCTAVE_CONF_CXXCPP
#define OCTAVE_CONF_CXXCPP %OCTAVE_CONF_CXXCPP%
#endif

#ifndef OCTAVE_CONF_CXXFLAGS
#define OCTAVE_CONF_CXXFLAGS %OCTAVE_CONF_CXXFLAGS%
#endif

#ifndef OCTAVE_CONF_CXXPICFLAG
#define OCTAVE_CONF_CXXPICFLAG %OCTAVE_CONF_CXXPICFLAG%
#endif

#ifndef OCTAVE_CONF_CXX
#define OCTAVE_CONF_CXX %OCTAVE_CONF_CXX%
#endif

// FIXME: OCTAVE_CONF_CXX_VERSION is deprecated.  Remove in version 3.12
#ifndef OCTAVE_CONF_CXX_VERSION
#define OCTAVE_CONF_CXX_VERSION %OCTAVE_CONF_CXX_VERSION%
#endif

#ifndef OCTAVE_CONF_DEFAULT_PAGER
#define OCTAVE_CONF_DEFAULT_PAGER %OCTAVE_CONF_DEFAULT_PAGER%
#endif

#ifndef OCTAVE_CONF_DEFS
#define OCTAVE_CONF_DEFS %OCTAVE_CONF_DEFS%
#endif

#ifndef OCTAVE_CONF_DL_LD
#define OCTAVE_CONF_DL_LD %OCTAVE_CONF_DL_LD%
#endif

#ifndef OCTAVE_CONF_DL_LDFLAGS
#define OCTAVE_CONF_DL_LDFLAGS %OCTAVE_CONF_DL_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_DL_LIBS
#define OCTAVE_CONF_DL_LIBS %OCTAVE_CONF_DL_LIBS%
#endif

#ifndef OCTAVE_CONF_ENABLE_DYNAMIC_LINKING
#define OCTAVE_CONF_ENABLE_DYNAMIC_LINKING %OCTAVE_CONF_ENABLE_DYNAMIC_LINKING%
#endif

#ifndef OCTAVE_CONF_EXEEXT
#define OCTAVE_CONF_EXEEXT %OCTAVE_CONF_EXEEXT%
#endif

#ifndef OCTAVE_CONF_GCC_VERSION
#define OCTAVE_CONF_GCC_VERSION %OCTAVE_CONF_GCC_VERSION%
#endif

#ifndef OCTAVE_CONF_GXX_VERSION
#define OCTAVE_CONF_GXX_VERSION %OCTAVE_CONF_GXX_VERSION%
#endif

#ifndef OCTAVE_CONF_F77
#define OCTAVE_CONF_F77 %OCTAVE_CONF_F77%
#endif

#ifndef OCTAVE_CONF_F77_FLOAT_STORE_FLAG
#define OCTAVE_CONF_F77_FLOAT_STORE_FLAG %OCTAVE_CONF_F77_FLOAT_STORE_FLAG%
#endif

#ifndef OCTAVE_CONF_F77_INTEGER_8_FLAG
#define OCTAVE_CONF_F77_INTEGER_8_FLAG %OCTAVE_CONF_F77_INTEGER_8_FLAG%
#endif

#ifndef OCTAVE_CONF_FC
#define OCTAVE_CONF_FC %OCTAVE_CONF_FC%
#endif

#ifndef OCTAVE_CONF_FFLAGS
#define OCTAVE_CONF_FFLAGS %OCTAVE_CONF_FFLAGS%
#endif

#ifndef OCTAVE_CONF_FFTW3_CPPFLAGSS
#define OCTAVE_CONF_FFTW3_CPPFLAGS %OCTAVE_CONF_FFTW3_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_FFTW3_LDFLAGSS
#define OCTAVE_CONF_FFTW3_LDFLAGS %OCTAVE_CONF_FFTW3_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_FFTW3_LIBS
#define OCTAVE_CONF_FFTW3_LIBS %OCTAVE_CONF_FFTW3_LIBS%
#endif

#ifndef OCTAVE_CONF_FFTW3F_CPPFLAGSS
#define OCTAVE_CONF_FFTW3F_CPPFLAGS %OCTAVE_CONF_FFTW3F_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_FFTW3F_LDFLAGSS
#define OCTAVE_CONF_FFTW3F_LDFLAGS %OCTAVE_CONF_FFTW3F_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_FFTW3F_LIBS
#define OCTAVE_CONF_FFTW3F_LIBS %OCTAVE_CONF_FFTW3F_LIBS%
#endif

#ifndef OCTAVE_CONF_FLIBS
#define OCTAVE_CONF_FLIBS %OCTAVE_CONF_FLIBS%
#endif

#ifndef OCTAVE_CONF_FLTK_CFLAGS
#define OCTAVE_CONF_FLTK_CFLAGS %OCTAVE_CONF_FLTK_CFLAGS%
#endif

#ifndef OCTAVE_CONF_FLTK_LIBS
#define OCTAVE_CONF_FLTK_LIBS %OCTAVE_CONF_FLTK_LIBS%
#endif

#ifndef OCTAVE_CONF_FONTCONFIG_CFLAGS
#define OCTAVE_CONF_FONTCONFIG_CFLAGS %OCTAVE_CONF_FONTCONFIG_CFLAGS%
#endif

#ifndef OCTAVE_CONF_FPICFLAG
#define OCTAVE_CONF_FPICFLAG %OCTAVE_CONF_FPICFLAG%
#endif

#ifndef OCTAVE_CONF_FT2_CFLAGS
#define OCTAVE_CONF_FT2_CFLAGS %OCTAVE_CONF_FT2_CFLAGS%
#endif

#ifndef OCTAVE_CONF_FT2_LIBS
#define OCTAVE_CONF_FT2_LIBS %OCTAVE_CONF_FT2_LIBS%
#endif

#ifndef OCTAVE_CONF_GLPK_CPPFLAGS
#define OCTAVE_CONF_GLPK_CPPFLAGS %OCTAVE_CONF_GLPK_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_GLPK_LDFLAGS
#define OCTAVE_CONF_GLPK_LDFLAGS %OCTAVE_CONF_GLPK_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_GLPK_LIBS
#define OCTAVE_CONF_GLPK_LIBS %OCTAVE_CONF_GLPK_LIBS%
#endif

#ifndef OCTAVE_CONF_GNUPLOT
#define OCTAVE_CONF_GNUPLOT %OCTAVE_CONF_GNUPLOT%
#endif

#ifndef OCTAVE_CONF_HDF5_CPPFLAGS
#define OCTAVE_CONF_HDF5_CPPFLAGS %OCTAVE_CONF_HDF5_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_HDF5_LDFLAGS
#define OCTAVE_CONF_HDF5_LDFLAGS %OCTAVE_CONF_HDF5_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_HDF5_LIBS
#define OCTAVE_CONF_HDF5_LIBS %OCTAVE_CONF_HDF5_LIBS%
#endif

#ifndef OCTAVE_CONF_INCFLAGS
#define OCTAVE_CONF_INCFLAGS %OCTAVE_CONF_INCFLAGS%
#endif

#ifndef OCTAVE_CONF_INCLUDEDIR
#define OCTAVE_CONF_INCLUDEDIR %OCTAVE_CONF_INCLUDEDIR%
#endif

#ifndef OCTAVE_CONF_LAPACK_LIBS
#define OCTAVE_CONF_LAPACK_LIBS %OCTAVE_CONF_LAPACK_LIBS%
#endif

#ifndef OCTAVE_CONF_LDFLAGS
#define OCTAVE_CONF_LDFLAGS %OCTAVE_CONF_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_LD_CXX
#define OCTAVE_CONF_LD_CXX %OCTAVE_CONF_LD_CXX%
#endif

#ifndef OCTAVE_CONF_LD_STATIC_FLAG
#define OCTAVE_CONF_LD_STATIC_FLAG %OCTAVE_CONF_LD_STATIC_FLAG%
#endif

#ifndef OCTAVE_CONF_LEXLIB
#define OCTAVE_CONF_LEXLIB %OCTAVE_CONF_LEXLIB%
#endif

#ifndef OCTAVE_CONF_LEX
#define OCTAVE_CONF_LEX %OCTAVE_CONF_LEX%
#endif

#ifndef OCTAVE_CONF_LFLAGS
#define OCTAVE_CONF_LFLAGS %OCTAVE_CONF_LFLAGS%
#endif

#ifndef OCTAVE_CONF_LIBEXT
#define OCTAVE_CONF_LIBEXT %OCTAVE_CONF_LIBEXT%
#endif

#ifndef OCTAVE_CONF_LIBFLAGS
#define OCTAVE_CONF_LIBFLAGS %OCTAVE_CONF_LIBFLAGS%
#endif

#ifndef OCTAVE_CONF_LIBOCTAVE
#define OCTAVE_CONF_LIBOCTAVE %OCTAVE_CONF_LIBOCTAVE%
#endif

#ifndef OCTAVE_CONF_LIBOCTINTERP
#define OCTAVE_CONF_LIBOCTINTERP %OCTAVE_CONF_LIBOCTINTERP%
#endif

#ifndef OCTAVE_CONF_LIBPLPLOT
#define OCTAVE_CONF_LIBPLPLOT %OCTAVE_CONF_LIBPLPLOT%
#endif

#ifndef OCTAVE_CONF_LIBS
#define OCTAVE_CONF_LIBS %OCTAVE_CONF_LIBS%
#endif

#ifndef OCTAVE_CONF_LN_S
#define OCTAVE_CONF_LN_S %OCTAVE_CONF_LN_S%
#endif

#ifndef OCTAVE_CONF_MAGICK_CPPFLAGS
#define OCTAVE_CONF_MAGICK_CPPFLAGS %OCTAVE_CONF_MAGICK_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_MAGICK_LDFLAGS
#define OCTAVE_CONF_MAGICK_LDFLAGS %OCTAVE_CONF_MAGICK_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_MAGICK_LIBS
#define OCTAVE_CONF_MAGICK_LIBS %OCTAVE_CONF_MAGICK_LIBS%
#endif

#ifndef OCTAVE_CONF_LLVM_CPPFLAGS
#define OCTAVE_CONF_LLVM_CPPFLAGS %OCTAVE_CONF_LLVM_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_LLVM_LDFLAGS
#define OCTAVE_CONF_LLVM_LDFLAGS %OCTAVE_CONF_LLVM_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_LLVM_LIBS
#define OCTAVE_CONF_LLVM_LIBS %OCTAVE_CONF_LLVM_LIBS%
#endif

#ifndef OCTAVE_CONF_MKOCTFILE_DL_LDFLAGS
#define OCTAVE_CONF_MKOCTFILE_DL_LDFLAGS %OCTAVE_CONF_MKOCTFILE_DL_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_OCTAVE_LINK_DEPS
#define OCTAVE_CONF_OCTAVE_LINK_DEPS %OCTAVE_CONF_OCTAVE_LINK_DEPS%
#endif

#ifndef OCTAVE_CONF_OCTAVE_LINK_OPTS
#define OCTAVE_CONF_OCTAVE_LINK_OPTS %OCTAVE_CONF_OCTAVE_LINK_OPTS%
#endif

#ifndef OCTAVE_CONF_OCTINCLUDEDIR
#define OCTAVE_CONF_OCTINCLUDEDIR %OCTAVE_CONF_OCTINCLUDEDIR%
#endif

#ifndef OCTAVE_CONF_OCTLIBDIR
#define OCTAVE_CONF_OCTLIBDIR %OCTAVE_CONF_OCTLIBDIR%
#endif

#ifndef OCTAVE_CONF_OCT_LINK_DEPS
#define OCTAVE_CONF_OCT_LINK_DEPS %OCTAVE_CONF_OCT_LINK_DEPS%
#endif

#ifndef OCTAVE_CONF_OCT_LINK_OPTS
#define OCTAVE_CONF_OCT_LINK_OPTS %OCTAVE_CONF_OCT_LINK_OPTS%
#endif

#ifndef OCTAVE_CONF_OPENGL_LIBS
#define OCTAVE_CONF_OPENGL_LIBS %OCTAVE_CONF_OPENGL_LIBS%
#endif

#ifndef OCTAVE_CONF_PCRE_CPPFLAGS
#define OCTAVE_CONF_PCRE_CPPFLAGS %OCTAVE_CONF_PCRE_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_PCRE_LIBS
#define OCTAVE_CONF_PCRE_LIBS %OCTAVE_CONF_PCRE_LIBS%
#endif

#ifndef OCTAVE_CONF_PREFIX
#define OCTAVE_CONF_PREFIX %OCTAVE_CONF_PREFIX%
#endif

#ifndef OCTAVE_CONF_PTHREAD_CFLAGS
#define OCTAVE_CONF_PTHREAD_CFLAGS %OCTAVE_CONF_PTHREAD_CFLAGS%
#endif

#ifndef OCTAVE_CONF_PTHREAD_LIBS
#define OCTAVE_CONF_PTHREAD_LIBS %OCTAVE_CONF_PTHREAD_LIBS%
#endif

#ifndef OCTAVE_CONF_QHULL_CPPFLAGS
#define OCTAVE_CONF_QHULL_CPPFLAGS %OCTAVE_CONF_QHULL_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_QHULL_LDFLAGS
#define OCTAVE_CONF_QHULL_LDFLAGS %OCTAVE_CONF_QHULL_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_QHULL_LIBS
#define OCTAVE_CONF_QHULL_LIBS %OCTAVE_CONF_QHULL_LIBS%
#endif

#ifndef OCTAVE_CONF_QRUPDATE_CPPFLAGS
#define OCTAVE_CONF_QRUPDATE_CPPFLAGS %OCTAVE_CONF_QRUPDATE_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_QRUPDATE_LDFLAGS
#define OCTAVE_CONF_QRUPDATE_LDFLAGS %OCTAVE_CONF_QRUPDATE_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_QRUPDATE_LIBS
#define OCTAVE_CONF_QRUPDATE_LIBS %OCTAVE_CONF_QRUPDATE_LIBS%
#endif

#ifndef OCTAVE_CONF_QT_CPPFLAGS
#define OCTAVE_CONF_QT_CPPFLAGS %OCTAVE_CONF_QT_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_QT_LDFLAGS
#define OCTAVE_CONF_QT_LDFLAGS %OCTAVE_CONF_QT_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_QT_LIBS
#define OCTAVE_CONF_QT_LIBS %OCTAVE_CONF_QT_LIBS%
#endif

#ifndef OCTAVE_CONF_RANLIB
#define OCTAVE_CONF_RANLIB %OCTAVE_CONF_RANLIB%
#endif

#ifndef OCTAVE_CONF_RDYNAMIC_FLAG
#define OCTAVE_CONF_RDYNAMIC_FLAG %OCTAVE_CONF_RDYNAMIC_FLAG%
#endif

#ifndef OCTAVE_CONF_READLINE_LIBS
#define OCTAVE_CONF_READLINE_LIBS %OCTAVE_CONF_READLINE_LIBS%
#endif

#ifndef OCTAVE_CONF_SED
#define OCTAVE_CONF_SED %OCTAVE_CONF_SED%
#endif

#ifndef OCTAVE_CONF_SHARED_LIBS
#define OCTAVE_CONF_SHARED_LIBS %OCTAVE_CONF_SHARED_LIBS%
#endif

#ifndef OCTAVE_CONF_SHLEXT
#define OCTAVE_CONF_SHLEXT %OCTAVE_CONF_SHLEXT%
#endif

#ifndef OCTAVE_CONF_SHLEXT_VER
#define OCTAVE_CONF_SHLEXT_VER %OCTAVE_CONF_SHLEXT_VER%
#endif

#ifndef OCTAVE_CONF_SH_LD
#define OCTAVE_CONF_SH_LD %OCTAVE_CONF_SH_LD%
#endif

#ifndef OCTAVE_CONF_SH_LDFLAGS
#define OCTAVE_CONF_SH_LDFLAGS %OCTAVE_CONF_SH_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_SONAME_FLAGS
#define OCTAVE_CONF_SONAME_FLAGS %OCTAVE_CONF_SONAME_FLAGS%
#endif

#ifndef OCTAVE_CONF_STATIC_LIBS
#define OCTAVE_CONF_STATIC_LIBS %OCTAVE_CONF_STATIC_LIBS%
#endif

#ifndef OCTAVE_CONF_TERM_LIBS
#define OCTAVE_CONF_TERM_LIBS %OCTAVE_CONF_TERM_LIBS%
#endif

#ifndef OCTAVE_CONF_UMFPACK_CPPFLAGS
#define OCTAVE_CONF_UMFPACK_CPPFLAGS %OCTAVE_CONF_UMFPACK_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_UMFPACK_LDFLAGS
#define OCTAVE_CONF_UMFPACK_LDFLAGS %OCTAVE_CONF_UMFPACK_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_UMFPACK_LIBS
#define OCTAVE_CONF_UMFPACK_LIBS %OCTAVE_CONF_UMFPACK_LIBS%
#endif

#ifndef OCTAVE_CONF_USE_64_BIT_IDX_T
#define OCTAVE_CONF_USE_64_BIT_IDX_T %OCTAVE_CONF_USE_64_BIT_IDX_T%
#endif

#ifndef OCTAVE_CONF_WARN_CFLAGS
#define OCTAVE_CONF_WARN_CFLAGS %OCTAVE_CONF_WARN_CFLAGS%
#endif

#ifndef OCTAVE_CONF_WARN_CXXFLAGS
#define OCTAVE_CONF_WARN_CXXFLAGS %OCTAVE_CONF_WARN_CXXFLAGS%
#endif

#ifndef OCTAVE_CONF_X11_INCFLAGS
#define OCTAVE_CONF_X11_INCFLAGS %OCTAVE_CONF_X11_INCFLAGS%
#endif

#ifndef OCTAVE_CONF_X11_LIBS
#define OCTAVE_CONF_X11_LIBS %OCTAVE_CONF_X11_LIBS%
#endif

#ifndef OCTAVE_CONF_XTRA_CFLAGS
#define OCTAVE_CONF_XTRA_CFLAGS %OCTAVE_CONF_XTRA_CFLAGS%
#endif

#ifndef OCTAVE_CONF_XTRA_CXXFLAGS
#define OCTAVE_CONF_XTRA_CXXFLAGS %OCTAVE_CONF_XTRA_CXXFLAGS%
#endif

#ifndef OCTAVE_CONF_YACC
#define OCTAVE_CONF_YACC %OCTAVE_CONF_YACC%
#endif

#ifndef OCTAVE_CONF_YFLAGS
#define OCTAVE_CONF_YFLAGS %OCTAVE_CONF_YFLAGS%
#endif

#ifndef OCTAVE_CONF_Z_CPPFLAGS
#define OCTAVE_CONF_Z_CPPFLAGS %OCTAVE_CONF_Z_CPPFLAGS%
#endif

#ifndef OCTAVE_CONF_Z_LDFLAGS
#define OCTAVE_CONF_Z_LDFLAGS %OCTAVE_CONF_Z_LDFLAGS%
#endif

#ifndef OCTAVE_CONF_Z_LIBS
#define OCTAVE_CONF_Z_LIBS %OCTAVE_CONF_Z_LIBS%
#endif

#ifndef OCTAVE_CONF_config_opts
#define OCTAVE_CONF_config_opts %OCTAVE_CONF_config_opts%
#endif

#endif
