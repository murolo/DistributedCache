# Makefile.in generated by automake 1.9.6 from Makefile.am.
# src/dataserver/Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# 2003, 2004, 2005  Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.




srcdir = .
top_srcdir = ../..

pkgdatadir = $(datadir)/tair_rdb
pkglibdir = $(libdir)/tair_rdb
pkgincludedir = $(includedir)/tair_rdb
top_builddir = ../..
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_triplet = x86_64-unknown-linux-gnu
host_triplet = x86_64-unknown-linux-gnu
sbin_PROGRAMS = tair_server$(EXEEXT)
subdir = src/dataserver
DIST_COMMON = $(include_HEADERS) $(srcdir)/Makefile.am \
	$(srcdir)/Makefile.in
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/ax_boost.m4 \
	$(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
mkinstalldirs = $(install_sh) -d
CONFIG_CLEAN_FILES =
am__installdirs = "$(DESTDIR)$(sbindir)" "$(DESTDIR)$(includedir)"
sbinPROGRAMS_INSTALL = $(INSTALL_PROGRAM)
PROGRAMS = $(sbin_PROGRAMS)
am_tair_server_OBJECTS = dup_sync_manager.$(OBJEXT) \
	duplicate_manager.$(OBJEXT) request_processor.$(OBJEXT) \
	tair_manager.$(OBJEXT) update_log.$(OBJEXT) \
	heartbeat_thread.$(OBJEXT) migrate_manager.$(OBJEXT) \
	table_manager.$(OBJEXT) tair_server.$(OBJEXT) \
	stat_helper.$(OBJEXT)
tair_server_OBJECTS = $(am_tair_server_OBJECTS)
tair_server_LDADD = $(LDADD)
#am__DEPENDENCIES_1 =  \
#	$(top_builddir)/src/storage/kdb/libkdb.a \
#	$(top_builddir)/src/storage/kdb/kyotocabinet/libkyotocabinet.a
tair_server_DEPENDENCIES = $(TBLIB_ROOT)/lib/libtbnet.a \
	$(TBLIB_ROOT)/lib/libtbsys.a \
	$(top_builddir)/src/storage/mdb/libmdb.a \
	$(top_builddir)/src/storage/fdb/libfdb.a \
	$(top_builddir)/src/storage/rdb/librdb.a \
	$(top_builddir)/src/storage/rdb/libredisdb.a \
	$(top_builddir)/src/storage/rdb/redis/libredis.a \
	$(top_builddir)/src/storage/rdb/libredisprofiler.a \
	$(top_builddir)/src/storage/mock/libmockdb.a \
	$(am__DEPENDENCIES_1) \
	$(top_builddir)/src/plugin/libtair_plugins.a \
	$(top_builddir)/src/packets/libtair_packets.a \
	$(top_builddir)/src/storage/libdump.a \
	$(TBLIB_ROOT)/lib/libtbsys.a
DEFAULT_INCLUDES = -I. -I$(srcdir)
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__depfiles_maybe = depfiles
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --tag=CXX --mode=compile $(CXX) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(CXXLD) $(AM_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --tag=CC --mode=compile $(CC) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(LIBTOOL) --tag=CC --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
SOURCES = $(tair_server_SOURCES)
DIST_SOURCES = $(tair_server_SOURCES)
am__vpath_adj_setup = srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`;
am__vpath_adj = case $$p in \
    $(srcdir)/*) f=`echo "$$p" | sed "s|^$$srcdirstrip/||"`;; \
    *) f=$$p;; \
  esac;
am__strip_dir = `echo $$p | sed -e 's|^.*/||'`;
includeHEADERS_INSTALL = $(INSTALL_HEADER)
HEADERS = $(include_HEADERS)
ETAGS = etags
CTAGS = ctags
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = ${SHELL} /home/yinchunxiang/WorkGit/agile/src/third_party/tair/tair_rdb_release_20121009/missing --run aclocal-1.9
AMDEP_FALSE = #
AMDEP_TRUE = 
AMTAR = ${SHELL} /home/yinchunxiang/WorkGit/agile/src/third_party/tair/tair_rdb_release_20121009/missing --run tar
AR = ar
AUTOCONF = ${SHELL} /home/yinchunxiang/WorkGit/agile/src/third_party/tair/tair_rdb_release_20121009/missing --run autoconf
AUTOHEADER = ${SHELL} /home/yinchunxiang/WorkGit/agile/src/third_party/tair/tair_rdb_release_20121009/missing --run autoheader
AUTOMAKE = ${SHELL} /home/yinchunxiang/WorkGit/agile/src/third_party/tair/tair_rdb_release_20121009/missing --run automake-1.9
AWK = gawk
BOOST_CPPFLAGS = -pthread -I/usr/include
BOOST_DATE_TIME_LIB = 
BOOST_FILESYSTEM_LIB = 
BOOST_IOSTREAMS_LIB = 
BOOST_LDFLAGS = -L/usr/lib
BOOST_PROGRAM_OPTIONS_LIB = 
BOOST_REGEX_LIB = 
BOOST_SERIALIZATION_LIB = 
BOOST_SIGNALS_LIB = 
BOOST_THREAD_LIB = 
BOOST_UNIT_TEST_FRAMEWORK_LIB = 
BOOST_WSERIALIZATION_LIB = 
CC = gcc
CCDEPMODE = depmode=gcc3
CFLAGS = -g -O2
CPP = gcc -E
CPPFLAGS = 
CXX = g++
CXXCPP = g++ -E
CXXDEPMODE = depmode=gcc3
CXXFLAGS = -g -Wall -DBOOST_SPIRIT_THREADSAFE -D__USE_LARGEFILE64 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE  -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DTAIR_DEBUG -DHAVE_LIBREADLINE
CYGPATH_W = echo
DEFS = -DPACKAGE_NAME=\"tair_rdb\" -DPACKAGE_TARNAME=\"tair_rdb\" -DPACKAGE_VERSION=\"2.3.6.1\" -DPACKAGE_STRING=\"tair_rdb\ 2.3.6.1\" -DPACKAGE_BUGREPORT=\"ruohai@taobao.com\" -DPACKAGE=\"tair_rdb\" -DVERSION=\"2.3.6.1\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DHAVE_BOOST= -DHAVE_BOOST_FILESYSTEM= -DHAVE_BOOST_PROGRAM_OPTIONS= -DHAVE_BOOST_THREAD= -DHAVE_BOOST_IOSTREAMS= -DHAVE_BOOST_SERIALIZATION= -DHAVE_BOOST_SIGNALS= -DHAVE_BOOST_DATE_TIME= -DHAVE_BOOST_REGEX= -DHAVE_BOOST_UNIT_TEST_FRAMEWORK=
DEPDIR = .deps
ECHO = echo
ECHO_C = 
ECHO_N = -n
ECHO_T = 
EGREP = /bin/grep -E
EXEEXT = 
F77 = gfortran
FFLAGS = -g -O2
GCOV_LIB = 
GREP = /bin/grep
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LDFLAGS = 
LIBOBJS = 
LIBS = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
LTLIBOBJS = 
MAKEINFO = ${SHELL} /home/yinchunxiang/WorkGit/agile/src/third_party/tair/tair_rdb_release_20121009/missing --run makeinfo
OBJEXT = o
PACKAGE = tair_rdb
PACKAGE_BUGREPORT = ruohai@taobao.com
PACKAGE_NAME = tair_rdb
PACKAGE_STRING = tair_rdb 2.3.6.1
PACKAGE_TARNAME = tair_rdb
PACKAGE_VERSION = 2.3.6.1
PATH_SEPARATOR = :
RANLIB = ranlib
SED = /bin/sed
SET_MAKE = 
SHELL = /bin/sh
STRIP = strip
TEST_OPT_FALSE = 
TEST_OPT_TRUE = #
VERSION = 2.3.6.1
WITH_KDB_FALSE = 
WITH_KDB_TRUE = #
WITH_READLINE_FALSE = #
WITH_READLINE_TRUE = 
ac_ct_CC = gcc
ac_ct_CXX = g++
ac_ct_F77 = gfortran
am__fastdepCC_FALSE = #
am__fastdepCC_TRUE = 
am__fastdepCXX_FALSE = #
am__fastdepCXX_TRUE = 
am__include = include
am__leading_dot = .
am__quote = 
am__tar = ${AMTAR} chof - "$$tardir"
am__untar = ${AMTAR} xf -
bindir = ${exec_prefix}/bin
build = x86_64-unknown-linux-gnu
build_alias = 
build_cpu = x86_64
build_os = linux-gnu
build_vendor = unknown
datadir = ${datarootdir}
datarootdir = ${prefix}/share
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
dvidir = ${docdir}
exec_prefix = ${prefix}
host = x86_64-unknown-linux-gnu
host_alias = 
host_cpu = x86_64
host_os = linux-gnu
host_vendor = unknown
htmldir = ${docdir}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
install_sh = /home/yinchunxiang/WorkGit/agile/src/third_party/tair/tair_rdb_release_20121009/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localedir = ${datarootdir}/locale
localstatedir = ${prefix}/var
mandir = ${datarootdir}/man
mkdir_p = mkdir -p --
oldincludedir = /usr/include
pdfdir = ${docdir}
prefix = /home/yinchunxiang/tair_rdb_bin
program_transform_name = s,x,x,
psdir = ${docdir}
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
sysconfdir = ${prefix}/etc
target_alias = 
#KDB_CPPFLAGS = -DWITH_KDB -I${top_srcdir}/src/storage/kdb -I${top_srcdir}/src/storage/kdb/kyotocabinet
#KDB_LDFLAGS = $(top_builddir)/src/storage/kdb/libkdb.a $(top_builddir)/src/storage/kdb/kyotocabinet/libkyotocabinet.a
AM_CPPFLAGS = -I$(TBLIB_ROOT)/include/tbsys \
	     -I$(TBLIB_ROOT)/include/tbnet \
	     -I${top_srcdir}/src \
	     -I$(TBLIB_ROOT)/src/configserver \
	     -I${top_srcdir}/src/storage \
	     -I${top_srcdir}/src/dataserver \
	     -I${top_srcdir}/src/common \
	     -I${top_srcdir}/src/packets \
	     -I${top_srcdir}/src/storage/fdb \
	     -I${top_srcdir}/src/storage/mdb \
	     -I${top_srcdir}/src/storage/rdb \
	     -I${top_srcdir}/src/storage/mock \
	     $(KDB_CPPFLAGS) \
	     -I${top_srcdir}/src/plugin \
	     -pthread -I/usr/include

AM_LDFLAGS = -rdynamic $(BOOST_LDFLAGS)  ${GCOV_LIB} -ltcmalloc -Wall -Werror
LDADD = $(TBLIB_ROOT)/lib/libtbnet.a \
      $(TBLIB_ROOT)/lib/libtbsys.a \
      $(top_builddir)/src/storage/mdb/libmdb.a \
      $(top_builddir)/src/storage/fdb/libfdb.a \
      $(top_builddir)/src/storage/rdb/librdb.a \
      $(top_builddir)/src/storage/rdb/libredisdb.a \
      $(top_builddir)/src/storage/rdb/redis/libredis.a \
      $(top_builddir)/src/storage/rdb/libredisprofiler.a \
      $(top_builddir)/src/storage/mock/libmockdb.a \
      $(KDB_LDFLAGS) \
      $(top_builddir)/src/plugin/libtair_plugins.a \
      $(top_builddir)/src/packets/libtair_packets.a \
      $(top_builddir)/src/storage/libdump.a \
      $(TBLIB_ROOT)/lib/libtbsys.a \
      -lz -lrt -lpthread \
      -ldl \
      

include_HEADERS = tair_manager.hpp table_manager.hpp duplicate_manager.hpp duplicate_base.hpp \
 		update_log.hpp

tair_server_SOURCES = dup_sync_manager.cpp duplicate_manager.cpp request_processor.cpp tair_manager.cpp update_log.cpp \
		    heartbeat_thread.cpp migrate_manager.cpp table_manager.cpp tair_server.cpp \
		    $(top_builddir)/src/common/stat_helper.cpp	\
		    base_migrate.hpp	\
		    base_tair_manager.hpp	\
		    duplicate_base.hpp	\
		    duplicate_manager.hpp	\
			dup_sync_manager.hpp \
		    heartbeat_thread.hpp	\
		    migrate_manager.hpp	\
		    request_processor.hpp	\
		    table_manager.hpp	\
		    tair_manager.hpp	\
		    tair_server.hpp	\
		    update_log.hpp

all: all-am

.SUFFIXES:
.SUFFIXES: .cpp .lo .o .obj
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh \
		&& exit 0; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --gnu  src/dataserver/Makefile'; \
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  src/dataserver/Makefile
.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe);; \
	esac;

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
install-sbinPROGRAMS: $(sbin_PROGRAMS)
	@$(NORMAL_INSTALL)
	test -z "$(sbindir)" || $(mkdir_p) "$(DESTDIR)$(sbindir)"
	@list='$(sbin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	     || test -f $$p1 \
	  ; then \
	    f=`echo "$$p1" | sed 's,^.*/,,;$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(sbinPROGRAMS_INSTALL) '$$p' '$(DESTDIR)$(sbindir)/$$f'"; \
	   $(INSTALL_PROGRAM_ENV) $(LIBTOOL) --mode=install $(sbinPROGRAMS_INSTALL) "$$p" "$(DESTDIR)$(sbindir)/$$f" || exit 1; \
	  else :; fi; \
	done

uninstall-sbinPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(sbin_PROGRAMS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f '$(DESTDIR)$(sbindir)/$$f'"; \
	  rm -f "$(DESTDIR)$(sbindir)/$$f"; \
	done

clean-sbinPROGRAMS:
	@list='$(sbin_PROGRAMS)'; for p in $$list; do \
	  f=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  echo " rm -f $$p $$f"; \
	  rm -f $$p $$f ; \
	done
tair_server$(EXEEXT): $(tair_server_OBJECTS) $(tair_server_DEPENDENCIES) 
	@rm -f tair_server$(EXEEXT)
	$(CXXLINK) $(tair_server_LDFLAGS) $(tair_server_OBJECTS) $(tair_server_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/dup_sync_manager.Po
include ./$(DEPDIR)/duplicate_manager.Po
include ./$(DEPDIR)/heartbeat_thread.Po
include ./$(DEPDIR)/migrate_manager.Po
include ./$(DEPDIR)/request_processor.Po
include ./$(DEPDIR)/stat_helper.Po
include ./$(DEPDIR)/table_manager.Po
include ./$(DEPDIR)/tair_manager.Po
include ./$(DEPDIR)/tair_server.Po
include ./$(DEPDIR)/update_log.Po

.cpp.o:
	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ $<

.cpp.obj:
	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ `$(CYGPATH_W) '$<'`; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ `$(CYGPATH_W) '$<'`

.cpp.lo:
	if $(LTCXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Plo"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE) -c -o $@ $<

stat_helper.o: $(top_builddir)/src/common/stat_helper.cpp
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -MT stat_helper.o -MD -MP -MF "$(DEPDIR)/stat_helper.Tpo" -c -o stat_helper.o `test -f '$(top_builddir)/src/common/stat_helper.cpp' || echo '$(srcdir)/'`$(top_builddir)/src/common/stat_helper.cpp; \
	then mv -f "$(DEPDIR)/stat_helper.Tpo" "$(DEPDIR)/stat_helper.Po"; else rm -f "$(DEPDIR)/stat_helper.Tpo"; exit 1; fi
#	source='$(top_builddir)/src/common/stat_helper.cpp' object='stat_helper.o' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -c -o stat_helper.o `test -f '$(top_builddir)/src/common/stat_helper.cpp' || echo '$(srcdir)/'`$(top_builddir)/src/common/stat_helper.cpp

stat_helper.obj: $(top_builddir)/src/common/stat_helper.cpp
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -MT stat_helper.obj -MD -MP -MF "$(DEPDIR)/stat_helper.Tpo" -c -o stat_helper.obj `if test -f '$(top_builddir)/src/common/stat_helper.cpp'; then $(CYGPATH_W) '$(top_builddir)/src/common/stat_helper.cpp'; else $(CYGPATH_W) '$(srcdir)/$(top_builddir)/src/common/stat_helper.cpp'; fi`; \
	then mv -f "$(DEPDIR)/stat_helper.Tpo" "$(DEPDIR)/stat_helper.Po"; else rm -f "$(DEPDIR)/stat_helper.Tpo"; exit 1; fi
#	source='$(top_builddir)/src/common/stat_helper.cpp' object='stat_helper.obj' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -c -o stat_helper.obj `if test -f '$(top_builddir)/src/common/stat_helper.cpp'; then $(CYGPATH_W) '$(top_builddir)/src/common/stat_helper.cpp'; else $(CYGPATH_W) '$(srcdir)/$(top_builddir)/src/common/stat_helper.cpp'; fi`

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

distclean-libtool:
	-rm -f libtool
uninstall-info-am:
install-includeHEADERS: $(include_HEADERS)
	@$(NORMAL_INSTALL)
	test -z "$(includedir)" || $(mkdir_p) "$(DESTDIR)$(includedir)"
	@list='$(include_HEADERS)'; for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f=$(am__strip_dir) \
	  echo " $(includeHEADERS_INSTALL) '$$d$$p' '$(DESTDIR)$(includedir)/$$f'"; \
	  $(includeHEADERS_INSTALL) "$$d$$p" "$(DESTDIR)$(includedir)/$$f"; \
	done

uninstall-includeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(include_HEADERS)'; for p in $$list; do \
	  f=$(am__strip_dir) \
	  echo " rm -f '$(DESTDIR)$(includedir)/$$f'"; \
	  rm -f "$(DESTDIR)$(includedir)/$$f"; \
	done

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique
tags: TAGS

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	if test -z "$(ETAGS_ARGS)$$tags$$unique"; then :; else \
	  test -n "$$unique" || unique=$$empty_fix; \
	  $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	    $$tags $$unique; \
	fi
ctags: CTAGS
CTAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(CTAGS_ARGS)$$tags$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags

distdir: $(DISTFILES)
	@srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's|.|.|g'`; \
	list='$(DISTFILES)'; for file in $$list; do \
	  case $$file in \
	    $(srcdir)/*) file=`echo "$$file" | sed "s|^$$srcdirstrip/||"`;; \
	    $(top_srcdir)/*) file=`echo "$$file" | sed "s|^$$topsrcdirstrip/|$(top_builddir)/|"`;; \
	  esac; \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkdir_p) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(PROGRAMS) $(HEADERS)
installdirs:
	for dir in "$(DESTDIR)$(sbindir)" "$(DESTDIR)$(includedir)"; do \
	  test -z "$$dir" || $(mkdir_p) "$$dir"; \
	done
install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-generic clean-libtool clean-sbinPROGRAMS \
	mostlyclean-am

distclean: distclean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-libtool distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

info: info-am

info-am:

install-data-am: install-includeHEADERS

install-exec-am: install-sbinPROGRAMS

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-includeHEADERS uninstall-info-am \
	uninstall-sbinPROGRAMS

.PHONY: CTAGS GTAGS all all-am check check-am clean clean-generic \
	clean-libtool clean-sbinPROGRAMS ctags distclean \
	distclean-compile distclean-generic distclean-libtool \
	distclean-tags distdir dvi dvi-am html html-am info info-am \
	install install-am install-data install-data-am install-exec \
	install-exec-am install-includeHEADERS install-info \
	install-info-am install-man install-sbinPROGRAMS install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool pdf pdf-am ps ps-am \
	tags uninstall uninstall-am uninstall-includeHEADERS \
	uninstall-info-am uninstall-sbinPROGRAMS

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
