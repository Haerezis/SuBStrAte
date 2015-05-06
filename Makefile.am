#
#   /*------------------------------------------------------------------+
#    |                           SuBStrAte                              |
#    |------------------------------------------------------------------|
#    |                          makefile.am                             |
#    |------------------------------------------------------------------|
#    |                   First version: 06/05/2015                      |
#    +------------------------------------------------------------------+
#
# Makefile.am (or makefile if generated) of SuBStrAte.
# Makefile.am is not a makefile, you must run the 'autogen.sh' THEN the
# "./configure" shellscript to generate the Makefile thanks to this file.
#
#############################################################################

if BUNDLED_OSL
    MAYBE_OSL = osl
    OSL_LA  = $(top_builddir)/osl/libosl.la
endif
if BUNDLED_CANDL
    MAYBE_CANDL = candl
    CANDL_LA = $(top_builddir)/candl/libcandl.la
endif

AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS          = $(MAYBE_OSL) $(MAYBE_CANDL) doc . 
DIST_SUBDIRS     = $(MAYBE_OSL) $(MAYBE_CANDL) doc .
ACLOCAL_AMFLAGS  = -I m4

#############################################################################

bin_PROGRAMS    = substrate
lib_LTLIBRARIES = libsubstrate.la

#############################################################################

pkginclude_HEADERS = \
	include/substrate/analyze.h \
	include/substrate/parallelism.h \
	include/substrate/reuse_struct.h \
	include/substrate/utils.h \
	include/substrate/optimization.h \
	include/substrate/parallelism_struct.h \
	include/substrate/statement_profile.h \
	include/substrate/vectorization.h \
	include/substrate/options.h \
	include/substrate/reuse.h \
	include/substrate/substrate.h \
	include/substrate/vectorization_struct.h

DEFAULT_CPPFLAGS = -I.
AM_CPPFLAGS = -I. -I$(abs_builddir)/include -I$(abs_srcdir)/include
AM_CFLAGS = $(CFLAGS_WARN)

#############################################################################

libsubstrate_la_LIBADD   = @OSL_LIBS@ @CANDL_LIBS@ $(OSL_LA) $(CANDL_LA)
libsubstrate_la_CPPFLAGS = ${AM_CPPFLAGS} @OSL_CPPFLAGS@ @CANDL_CPPFLAGS@
libsubstrate_la_SOURCES  = \
	src/analyze.c \
	src/optimization.c \
	src/options.c \
	src/parallelism.c \
	src/parallelism_struct.c \
	src/reuse.c \
	src/reuse_struct.c \
	src/statement_profile.c \
	src/substrate.c \
	src/utils.c \
	src/vectorization.c \
	src/vectorization_struct.c
#############################################################################

LDADD             = @OSL_LIBS@ @CANDL_LIBS@ libsubstrate.la
substrate_CPPFLAGS     = ${AM_CPPFLAGS} @OSL_CPPFLAGS@ @CANDL_CPPFLAGS@ -g
substrate_DEPENDENCIES = libsubstrate.la
substrate_SOURCES      = src/substrate.c

#############################################################################

MAINTAINERCLEANFILES = \
	Makefile.in \
	aclocal.m4 \
	configure \
	$(AUX_DIST)

#############################################################################

#EXTRA_DIST = substrate-config.cmake

install-data-local:
	@test -z "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)" || $(mkdir_p) "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)"
#	$(INSTALL_DATA) "$(top_srcdir)/substrate-config.cmake" "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/"

uninstall-local:
#	rm -f "$(DESTDIR)$(libdir)/$(PACKAGE_NAME)/substrate-config.cmake"

dist-hook:
	(cd $(distdir) && mkdir -p $(ac_aux_dir))
	for file in $(AUX_DIST); do \
	  cp $$file $(distdir)/$$file; \
	done
#############################################################################

