dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)

AC_INIT([libmodplug], [0.8.9.0])
AC_CONFIG_SRCDIR([Makefile.am])

AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE([enable])

dnl Checks for programs.
dnl  I am disabling static libraries here because otherwise libtool insists on
dnl  compiling everything twice -- once with and once without -fPIC.  Pisses me
dnl  off.  Just do everything with -fPIC, damnit!  Compiling everything twice
dnl  probably wastes more cycles than not using -fPIC saves.
AC_DISABLE_STATIC
AC_DISABLE_STATIC([])
AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AC_C_BIGENDIAN

#AC_LIBTOOL_WIN32_DLL
#AC_PROG_LIBTOOL
LT_INIT([win32-dll])

AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h stdint.h malloc.h])
AC_CHECK_FUNCS(sinf)

CXXFLAGS="$CXXFLAGS -fno-exceptions -Wall -ffast-math -fno-common -D_REENTRANT"

AC_CANONICAL_HOST
case "$host" in
	*mingw* | *cygwin*)
		LT_LDFLAGS="-no-undefined"
		;;
	*)
		LT_LDFLAGS=""
		;;
esac
AC_SUBST(LT_LDFLAGS)

# require 10.5+ for osx/x86_64 builds
case "$host" in
	x86_64-*-darwin*)
		CXXFLAGS="$CXXFLAGS -mmacosx-version-min=10.5"
		LDFLAGS="$LDFLAGS -mmacosx-version-min=10.5" ;;
esac

# symbol visibility
ac_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -fvisibility=hidden -Werror"
AC_CACHE_CHECK([if compiler supports visibility attributes],[libmodplug_cv_gcc_visibility],
	AC_TRY_COMPILE([void foo(void);
__attribute__((visibility("default"))) void foo(void) {}],
		[],
		[libmodplug_cv_gcc_visibility=yes],
		[libmodplug_cv_gcc_visibility=no])
	)
# we want symbol -fvisibility for elf targets, however it works
# with darwin/macho too.  other than that, windows, dos and os2
# do not need it:  for any such targets, the -Werror switch is
# supposed to fail the above check. (I'm adding the manual test
# below nonetheless, just in case.)
case $host_os in
   mingw*|cygwin*|emx*|*djgpp)
	libmodplug_cv_gcc_visibility=no
	;;
esac
CXXFLAGS="$ac_save_CXXFLAGS"
if test $libmodplug_cv_gcc_visibility = yes ;then
   CXXFLAGS="$CXXFLAGS -DSYM_VISIBILITY -fvisibility=hidden"
fi

case ${target_os} in
	*sun* | *solaris*)
		CXXFLAGS="$CXXFLAGS -fpermissive"
		;;
esac

# portable types. requires autoconf 2.60
# `configure' will check if these are defined in system headers.
# if not, it will auto-detect and define them in `config.h'
AC_TYPE_INT8_T
AC_TYPE_UINT8_T
AC_TYPE_INT16_T
AC_TYPE_UINT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT64_T

MODPLUG_LIBRARY_VERSION=1:0:0

AC_SUBST(MODPLUG_LIBRARY_VERSION)

AC_CONFIG_FILES([Makefile
	  src/Makefile
	  libmodplug.pc])
AC_OUTPUT
