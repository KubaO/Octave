include $(top_srcdir)/liboctave/cruft/link-deps.mk

LIBOCTAVE_LINK_DEPS = \
	$(GNULIB_LINK_DEPS) \
  $(SPARSE_XLIBS) \
  $(ARPACK_LIBS) \
  $(QRUPDATE_LIBS) \
  $(FFTW_XLIBS) \
  $(LAPACK_LIBS) \
  $(BLAS_LIBS) \
  $(READLINE_LIBS) \
  $(TERM_LIBS) \
  $(LIBGLOB) \
  $(REGEX_LIBS) \
  $(DL_LIBS) \
  $(PTHREAD_LIBS) \
  $(LIBS)

LIBOCTAVE_LINK_OPTS = \
  $(SPARSE_XLDFLAGS) \
  $(ARPACK_LDFLAGS) \
  $(QRUPDATE_LDFLAGS) \
  $(FFTW_XLDFLAGS)

if AMCOND_LINK_ALL_DEPS
  LIBOCTAVE_LINK_DEPS += $(LIBCRUFT_LINK_DEPS)
  LIBOCTAVE_LINK_OPTS += $(LIBCRUFT_LINK_OPTS)
endif
