dnl Process this file with autoconf to produce a configure script. -*-m4-*-

dnl The package_version file will be automatically synced to the git revision
dnl by the update_version script when configured in the repository, but will
dnl remain constant in tarball releases unless it is manually edited.
m4_define([CURRENT_VERSION],
          m4_esyscmd([ if test -e package_version || ./update_version; then
                           . ./package_version
                           printf "$PACKAGE_VERSION"
                       else
                           printf "unknown"
                       fi ]))

AC_INIT([opus],[CURRENT_VERSION],[opus@xiph.org])

AC_CONFIG_SRCDIR(src/opus_encoder.c)

dnl enable silent rules on automake 1.11 and later
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])


# For libtool.
dnl Please update these for releases.
OPUS_LT_CURRENT=4
OPUS_LT_REVISION=0
OPUS_LT_AGE=4

AC_SUBST(OPUS_LT_CURRENT)
AC_SUBST(OPUS_LT_REVISION)
AC_SUBST(OPUS_LT_AGE)

AM_INIT_AUTOMAKE([no-define])
AM_MAINTAINER_MODE([enable])

AC_CANONICAL_HOST
AC_MINGW32
AM_PROG_LIBTOOL
AM_PROG_CC_C_O

AC_PROG_CC_C99
AC_C_CONST
AC_C_INLINE

#Use a hacked up version of autoconf's AC_C_RESTRICT because it's not
#strong enough a test to detect old buggy versions of GCC (e.g. 2.95.3)
AC_CACHE_CHECK([for C/C++ restrict keyword], ac_cv_c_restrict,
  [ac_cv_c_restrict=no
   # The order here caters to the fact that C++ does not require restrict.
   for ac_kw in __restrict __restrict__ _Restrict restrict; do
     AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
      [[typedef int * int_ptr;
	int foo (int_ptr $ac_kw ip, int * $ac_kw baz[]) {
	return ip[0];
       }]],
      [[int s[1];
	int * $ac_kw t = s;
	t[0] = 0;
	return foo(t, (void *)0)]])],
      [ac_cv_c_restrict=$ac_kw])
     test "$ac_cv_c_restrict" != no && break
   done
  ])
 AH_VERBATIM([restrict],
[/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#undef restrict
/* Work around a bug in Sun C++: it does not support _Restrict or
   __restrict__, even though the corresponding Sun C compiler ends up with
   "#define restrict _Restrict" or "#define restrict __restrict__" in the
   previous line.  Perhaps some future version of Sun C++ will work with
   restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif])
 case $ac_cv_c_restrict in
   restrict) ;;
   no) AC_DEFINE([restrict], []) ;;
   *)  AC_DEFINE_UNQUOTED([restrict], [$ac_cv_c_restrict]) ;;
 esac

AC_DEFINE([OPUS_BUILD], [], [This is a build of OPUS])

AC_MSG_CHECKING(for C99 variable-size arrays)
AC_TRY_COMPILE( [], [static int x; char a[++x]; a[sizeof a - 1] = 0; int N; return a[0];],
[has_var_arrays=yes;AC_DEFINE([VAR_ARRAYS], [], [Use C99 variable-size arrays])
],
has_var_arrays=no
)
AC_MSG_RESULT($has_var_arrays)

AC_CHECK_HEADERS([alloca.h getopt.h])
AC_MSG_CHECKING(for alloca)
AC_TRY_COMPILE( [#include <alloca.h>], [
int foo=10;
int *array = alloca(foo);
],
[
has_alloca=yes;
if test x$has_var_arrays = "xno" ; then
AC_DEFINE([USE_ALLOCA], [], [Make use of alloca])
fi
],
has_alloca=no
)
AC_MSG_RESULT($has_alloca)

AC_CHECK_FUNC(exp,[fp_libm_not_needed=yes;LIBM=],[fp_libm_not_needed=dunno])
if test x"$fp_libm_not_needed" = xdunno; then
  AC_CHECK_LIB([m], [exp], [LIBS="-lm $LIBS"; LIBM="-lm"],[LIBM=])
fi
AC_SUBST([LIBM])

has_float_approx=no
#case "$host_cpu" in
#i[[3456]]86 | x86_64 | powerpc64 | powerpc32 | ia64)
#  has_float_approx=yes
#  ;;
#esac    

ac_enable_fixed="no";
ac_enable_float="yes";
AC_ARG_ENABLE(fixed-point, [  --enable-fixed-point       compile without floating point (for machines without a fast enough FPU)],
[if test "$enableval" = yes; then
  ac_enable_fixed="yes";
  ac_enable_float="no";
  AC_DEFINE([FIXED_POINT], [1], [Compile as fixed-point (for machines without a fast enough FPU)])
fi])

ac_enable_fixed_debug="no"
AC_ARG_ENABLE(fixed-point-debug, [  --enable-fixed-point-debug debug fixed-point implementation],
[if test "$enableval" = yes; then
  ac_enable_fixed_debug="yes"
  AC_DEFINE([FIXED_DEBUG], , [Debug fixed-point implementation])
fi])

ac_enable_custom_modes="no"
AC_ARG_ENABLE(custom-modes, [  --enable-custom-modes      enable non-Opus modes, e.g. 44.1 kHz & 2^n frames],
[if test "$enableval" = yes; then
  ac_enable_custom_modes="yes"
  AC_DEFINE([CUSTOM_MODES], , [Custom modes])
fi])

float_approx=$has_float_approx
AC_ARG_ENABLE(float-approx, [  --enable-float-approx      enable fast approximations for floating point],
    [ if test "$enableval" = yes; then
    AC_WARN([Floating point approximations are not supported on all platforms.])
    float_approx=yes
    else
    float_approx=no
    fi], [ float_approx=$has_float_approx ])

if test "x${float_approx}" = "xyes"; then
    AC_DEFINE([FLOAT_APPROX], , [Float approximations])
fi

ac_enable_assertions="no"
AC_ARG_ENABLE(assertions, [  --enable-assertions        enable additional software error checking],
[if test "$enableval" = yes; then
  ac_enable_assertions="yes"
  AC_DEFINE([ENABLE_ASSERTIONS], , [Assertions])
fi])

ac_enable_fuzzing="no"
AC_ARG_ENABLE(fuzzing, [  --enable-fuzzing           causes the encoder to make random decisions],
[if test "$enableval" = yes; then
  ac_enable_fuzzing="yes"
  AC_DEFINE([FUZZING], , [Fuzzing])
fi])

ac_enable_doc="yes"
AC_ARG_ENABLE([doc],
  AS_HELP_STRING([--disable-doc], [Do not build API documentation]),
                 [ac_enable_doc=$enableval])
AC_CHECK_PROG(HAVE_DOXYGEN, [doxygen], [yes], [no])
if test "$HAVE_DOXYGEN" != "yes" -o "$ac_enable_doc" != "yes"; then
  HAVE_DOXYGEN="false"
  ac_enable_doc="no"
fi
AM_CONDITIONAL(HAVE_DOXYGEN, [test $HAVE_DOXYGEN = yes])

ac_enable_encoder="no";
AC_ARG_ENABLE(encoder, [  --enable-encoder           compile with encoder support],
[if test "$enableval" = yes; then
  ac_enable_encoder="yes";
  AC_DEFINE([OPUS_ENABLE_ENCODER], [1], [Compile with encoder support])
fi])

saved_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -fvisibility=hidden"
AC_MSG_CHECKING([if ${CC} supports -fvisibility=hidden])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([char foo;])],
      [ AC_MSG_RESULT([yes])
        SYMBOL_VISIBILITY="-fvisibility=hidden" ],
        AC_MSG_RESULT([no]))
CFLAGS="$saved_CFLAGS $SYMBOL_VISIBILITY"
AC_SUBST(SYMBOL_VISIBILITY)

CFLAGS="$CFLAGS -W"

saved_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall -Wextra -Wcast-align -Wnested-externs -Wshadow -Wstrict-prototypes"
AC_MSG_CHECKING([if ${CC} supports -Wall -Wextra -Wcast-align -Wnested-externs -Wshadow -Wstrict-prototypes])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([char foo;])],
      [ AC_MSG_RESULT([yes])
        EXTRA_WARNS="-Wall -Wextra -Wcast-align -Wnested-externs -Wshadow -Wstrict-prototypes" ],
        AC_MSG_RESULT([no]))
CFLAGS="$saved_CFLAGS $EXTRA_WARNS"

AC_CHECK_FUNCS([lrintf])
AC_CHECK_FUNCS([lrint])
AC_CHECK_FUNCS([__malloc_hook])

AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

if test x$has_char16 = "xyes" ; then
        case 1 in
                $ac_cv_sizeof_short) SIZE16="short";;
                $ac_cv_sizeof_int) SIZE16="int";;
        esac
else
        case 2 in
                $ac_cv_sizeof_short) SIZE16="short";;
                $ac_cv_sizeof_int) SIZE16="int";;
        esac
fi

if test x$has_char16 = "xyes" ; then
        case 2 in
                $ac_cv_sizeof_int) SIZE32="int";;
                $ac_cv_sizeof_long) SIZE32="long";;
                $ac_cv_sizeof_short) SIZE32="short";;
        esac
else
        case 4 in
                $ac_cv_sizeof_int) SIZE32="int";;
                $ac_cv_sizeof_long) SIZE32="long";;
                $ac_cv_sizeof_short) SIZE32="short";;
        esac
fi

AC_SUBST(SIZE16)
AC_SUBST(SIZE32)

AM_CONDITIONAL([FIXED_POINT], [test x$ac_enable_fixed = xyes])
AM_CONDITIONAL([CUSTOM_MODES], [test x$ac_enable_custom_modes = xyes])
AM_CONDITIONAL([USE_ENCODER], [test x$ac_enable_encoder = xyes])

dnl subsitutions for the pkg-config files
if test x$ac_enable_float = xyes; then
  PC_BUILD="floating-point"
  PC_LIBM=$LIBM
else
  PC_BUILD="fixed-point"
  PC_LIBM=
fi
dnl opus_custom requires libm as well
if test x$ac_enable_custom_modes = xyes; then
  PC_BUILD="${PC_BUILD}, custom modes"
  PC_LIBM=$LIBM
fi
AC_SUBST([PC_BUILD])
AC_SUBST([PC_LIBM])


AC_CONFIG_FILES([Makefile opus.pc opus-uninstalled.pc
                 doc/Makefile doc/Doxyfile])
AC_CONFIG_HEADERS([config.h])

AC_OUTPUT

AC_MSG_RESULT([
------------------------------------------------------------------------
  $PACKAGE_NAME $PACKAGE_VERSION:  Automatic configuration OK.

    Compiler support:

      C99 var arrays: ................ ${has_var_arrays}
      C99 lrintf: .................... ${ac_cv_func_lrintf}
      Alloca: ........................ ${has_alloca}
    
    General configuration:
    
      Floating point support: ........ ${ac_enable_float}
      Fast float approximations: ..... ${float_approx}
      Fixed point debugging: ......... ${ac_enable_fixed_debug}
      Encoder support: ............... ${ac_enable_encoder}
      Custom modes: .................. ${ac_enable_custom_modes}
      Assertion checking: ............ ${ac_enable_assertions}
      Fuzzing: ....................... ${ac_enable_fuzzing}

      API documentation: ............. ${ac_enable_doc}
------------------------------------------------------------------------
])

echo "Type \"make; make install\" to compile and install";
echo "Type \"make check\" to run the test suite";
