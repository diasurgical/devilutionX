dnl configure.ac: source for the configure script

dnl copyright by the mpg123 project - free software under the terms of the LGPL 2.1
dnl see COPYING and AUTHORS files in distribution or http://mpg123.org
dnl initially written by Nicholas J. Humfrey

dnl Require autoconf version >= 2.57
AC_PREREQ(2.57)

dnl ############# Initialisation
AC_INIT([mpg123], [1.25.6], [maintainer@mpg123.org])
dnl Increment API_VERSION when the API gets changes (new functions).

dnl libmpg123
API_VERSION=44
LIB_PATCHLEVEL=5

dnl libout123
OUTAPI_VERSION=2
OUTLIB_PATCHLEVEL=1

dnl Since we want to be backwards compatible, both sides get set to API_VERSION.
LIBMPG123_VERSION=$API_VERSION:$LIB_PATCHLEVEL:$API_VERSION
LIBOUT123_VERSION=$OUTAPI_VERSION:$OUTLIB_PATCHLEVEL:$OUTAPI_VERSION
AC_SUBST(LIBMPG123_VERSION)
AC_SUBST(API_VERSION)
AC_SUBST(LIBOUT123_VERSION)
AC_SUBST(OUTAPI_VERSION)


AC_CONFIG_SRCDIR(src/mpg123.c)
AC_CONFIG_AUX_DIR(build)
AC_CONFIG_SRCDIR(doc)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

dnl Version 1.7 of automake is recommended
dnl Not sure what minimal version does not choke on sub directories.
dnl Testing with 1.14.
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADERS([src/config.h])


# You get strange symptoms like jack module build failing because the AC_C_CONST failed to detect the working const support.
# In that case, the test failed because -Werror, not because no const there...
# After looking again, there are possibly more tests being obscured by false failures.
AC_MSG_CHECKING([for -Werror in CFLAGS (It breaks tests)])
if echo "$CFLAGS" | grep Werror; then
	AC_MSG_RESULT([yes])
	AC_MSG_WARN([You have -Werror in CFLAGS. That may break some tests and make this configure bogus.
If you want paranoid compilation, use --enable-nagging option, which adds -Werror for gcc.
Also note that you shall not run make distcheck after configuring with --enable-nagging.
distcheck uses the generated CFLAGS...
Anyhow, continuing at your own risk.])
else
	AC_MSG_RESULT([no])
fi

buffer=enabled # try to build with buffer by default

dnl ############# Compiler and tools Checks

LT_LDFLAGS=-export-dynamic
EXEC_LT_LDFLAGS=
be_static=no
all_static=no
lib_static=no
AC_MSG_CHECKING([if you are up to something totally static with LDFLAGS/CFLAGS])
for f in $LDFLAGS $CFLAGS
do
	case "$f" in
		-all-static)
			be_static=yes
			all_static=yes
			lib_static=yes
		;;
		-static)
			be_static=yes
			lib_static=yes
		;;
		-static-libgcc)
			lib_static=yes
		;;
	esac
done
if test "x$be_static" = xyes; then
	AC_MSG_RESULT([yes])
	LT_LDFLAGS=-all-static
	EXEC_LT_LDFLAGS="$LT_LDFLAGS"
else
	AC_MSG_RESULT([no])
fi
if test "x$all_static" = xyes; then
	AC_MSG_WARN( Use -static in LDFLAGS for all-static linking! Your compiler may blow up on that -all-static. )
fi

AM_PROG_AS
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_INSTALL
AC_CHECK_LIBM
AC_SUBST(LIBM)
dnl "Checking for egrep is broken after removal of libltdl stuff... checks use $EGREP, so searching it here."
AC_PROG_EGREP
AC_C_CONST
AC_INLINE
AC_C_BIGENDIAN

if test "x$lib_static" = xyes; then
        CC="$CC -static-libgcc"
        CCLD="$CC"
	echo "Adding -static-libgcc"
fi

dnl ############# Use Libtool for dynamic module loading

modules=auto
OUTPUT_OBJ="module.\$(OBJEXT)"
AC_ARG_ENABLE(modules,
[  --enable-modules=[no/yes] dynamically loadable output modules],
[
	if test "x$enableval" = xyes
	then
		modules=enabled
	else
		modules=disabled
	fi
],
[
	if test "x$be_static" = "xyes"; then
		modules=disabled
	else
		modules=auto
	fi
])

dnl We only want shared libraries by default
AC_DISABLE_STATIC
AC_ENABLE_SHARED

if test x"$enable_shared" = xno; then
	modules=disabled
	LT_LDFLAGS=
else
	AC_DEFINE(DYNAMIC_BUILD, 1, [ Define if building with dynamcally linked libmpg123])
fi

dnl We need the windows header also for checking the module mechanism.
AC_CHECK_HEADERS([windows.h])

if test x"$modules" = xdisabled
then
  echo "Modules disabled, not checking for dynamic loading."
else
  have_dl=no
  # The dlopen() API is either in libc or in libdl.
  if test x$ac_cv_header_windows_h = xyes; then
    AC_MSG_CHECKING([if LoadLibrary should be used])
    AC_LINK_IFELSE([AC_LANG_SOURCE([

#ifdef __CYGWIN__
#error Cygwin should use dlopen
#endif

#include <windows.h>

int main() {
  LoadLibraryW(0);
  GetProcAddress(0, 0);
  FreeLibrary(0);
}
    ])], [
    have_dl=yes
    AC_MSG_RESULT([Using LoadLibrary])
    ],
  [AC_MSG_RESULT([no])])
  else
    AC_SEARCH_LIBS(dlopen, dl)
    AC_CHECK_HEADER(dlfcn.h)
    AC_CHECK_FUNCS(dlopen dlsym dlclose, [ have_dl=yes ])
  fi
  if test x"$modules" = xenabled -a x"$have_dl" = xno; then
    AC_MSG_ERROR([Modules enabled but no runtime loader found! This will not work...])
  fi
  if test x"$modules" = xauto; then
    if test x"$have_dl" = xyes; then
      modules=enabled
      echo "We found a runtime loader: Modules enabled."
    else
      echo "We did not find a runtime loader: Modules disabled."
      modules=disabled
    fi
  fi
fi

dnl Configure libtool

AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

if test x"$modules" = xdisabled
then
  echo "Modules disabled."
else
  # Enable module support in source code
  AC_DEFINE( USE_MODULES, 1, [Define if modules are enabled] )
  # Export the module file suffix as LT_MODULE_EXT
  LT_SYS_MODULE_EXT
fi
AM_CONDITIONAL( [HAVE_MODULES], [test "x$modules" = xenabled] )

AC_SUBST(LT_LDFLAGS)
AC_SUBST(EXEC_LT_LDFLAGS)

dnl ############## Configurable Options

AC_ARG_ENABLE(debug,
              [  --enable-debug=[no/yes] turn on debugging],
              [
                if test "x$enableval" = xyes
                then
                  debugging="enabled"
                else
                  debugging="disabled"
                fi
              ],
              [ debugging="disabled" ]
)

AC_ARG_ENABLE(nagging,
              [  --enable-nagging=[no/yes] turn on GCC's pedantic nagging with error on warnings, does not include --enable-debug anymore ],
              [
                if test "x$enableval" = xyes
                then
                  nagging="enabled"
               else
                  nagging="disabled"
                fi
              ],
              [ nagging="disabled" ]
)

if test x"$debugging" = xenabled; then
	AC_DEFINE(DEBUG, 1, [ Define if debugging is enabled. ])
fi

AC_ARG_ENABLE(gapless,
              [  --enable-gapless=[no/yes] turn on gapless (enabled per default)],
              [
                if test "x$enableval" = xyes
                then
                  gapless="enabled"
                  AC_DEFINE(GAPLESS, 1, [ Define if gapless is enabled. ])
                else
                  gapless="disabled"
                fi
              ],
              [
                gapless="enabled"
                AC_DEFINE(GAPLESS, 1, [ Define if gapless is enabled. ])
              ]
)

AC_ARG_ENABLE(fifo,
              [  --enable-fifo=[no/yes] FIFO support for control interface (auto-enabled on linux) ],
              [
                if test "x$enableval" = xyes
                then
                  fifo="enabled"
                else
                  fifo="disabled"
                fi
              ],
              [
                fifo="auto"
              ]
)

AC_ARG_ENABLE(ipv6,
              [  --enable-ipv6=[no/yes] IPv6 support (actually any protocol your libc does with getaddrinfo) ],
              [
                if test "x$enableval" = xyes
                then
                  ipv6="enabled"
                else
                  ipv6="disabled"
                fi
              ],
              [
                ipv6="auto"
              ]
)

AC_ARG_ENABLE(network,
              [  --enable-network=[no/yes] network support (http streams / webradio) ],
              [
                if test "x$enableval" = xyes
                then
                  network="enabled"
                else
                  network="disabled"
                fi
              ],
              [
                network="auto"
              ]
)

dnl Optional objects list, depends on decoder choice and core feature selection.
dnl Not just for specific decoders anymore...
s_fpu=
DECODER_OBJ=
DECODER_LOBJ=

dnl Core features that can be disabled to reduce binary size.

id3v2=enabled
AC_ARG_ENABLE(id3v2,
              [  --disable-id3v2=[no/yes] no ID3v2 parsing ],
              [
                if test "x$enableval" = xno; then
                  id3v2="disabled"
                fi
              ], [])

# id3v2 depends on strings... so check that in between.
string=enabled
AC_ARG_ENABLE(string,
              [  --disable-string=[no/yes] no string API (this will disable ID3v2; main mpg123 won't build anymore) ],
              [
                if test "x$enableval" = xno; then
                  string="disabled"
                fi
              ], [])

if test "x$string" = "xdisabled"; then
  AC_DEFINE(NO_STRING, 1, [ Define to disable string functions. ])
  id3v2=disabled
 	AC_MSG_WARN([ID3v2 support disabled because of string API being disabled.])
else
  DECODER_OBJ="$DECODER_OBJ stringbuf.\$(OBJEXT)"
  DECODER_LOBJ="$DECODER_LOBJ stringbuf.lo"
fi

if test "x$id3v2" = "xdisabled"; then
  AC_DEFINE(NO_ID3V2, 1, [ Define to disable ID3v2 parsing. ])
fi

icy=enabled
AC_ARG_ENABLE(icy,
              [  --disable-icy=[no/yes] no ICY metainfo parsing/conversion (main mpg123 won't build!) ],
              [
                if test "x$enableval" = xno; then
                  icy="disabled"
                fi
              ], [])

if test "x$icy" = "xdisabled"; then
  AC_DEFINE(NO_ICY, 1, [ Define to disable ICY handling. ])
else
  DECODER_OBJ="$DECODER_OBJ icy.\$(OBJEXT) icy2utf8.\$(OBJEXT)"
  DECODER_LOBJ="$DECODER_LOBJ icy.lo icy2utf8.lo"
fi

ntom=enabled
AC_ARG_ENABLE(ntom,
              [  --disable-ntom=[no/yes] no flexible resampling ],
              [
                if test "x$enableval" = xno; then
                  ntom="disabled"
                fi
              ], [])

if test "x$ntom" = "xdisabled"; then
  AC_DEFINE(NO_NTOM, 1, [ Define to disable ntom resampling. ])
else
  DECODER_OBJ="$DECODER_OBJ ntom.\$(OBJEXT)"
  DECODER_LOBJ="$DECODER_LOBJ ntom.lo"
fi

downsample=enabled
AC_ARG_ENABLE(downsample,
              [  --disable-downsample=[no/yes] no downsampled decoding ],
              [
                if test "x$enableval" = xno; then
                  downsample="disabled"
                fi
              ], [])

if test "x$downsample" = "xdisabled"; then
  AC_DEFINE(NO_DOWNSAMPLE, 1, [ Define to disable downsampled decoding. ])
fi

feeder=enabled
AC_ARG_ENABLE(feeder,
              [  --disable-feeder=[no/yes] no feeder decoding, no buffered readers ],
              [
                if test "x$enableval" = xno; then
                  feeder="disabled"
                fi
              ], [])

if test "x$feeder" = "xdisabled"; then
  AC_DEFINE(NO_FEEDER, 1, [ Define to disable feeder and buffered readers. ])
fi

messages=enabled
AC_ARG_ENABLE(messages,
              [  --disable-messages=[no/yes] no error/warning messages on the console ],
              [
                if test "x$enableval" = xno; then
                  messages="disabled"
                fi
              ], [])

if test "x$messages" = "xdisabled"; then
  AC_DEFINE(NO_WARNING, 1, [ Define to disable warning messages. ])
  AC_DEFINE(NO_ERRORMSG,   1, [ Define to disable error messages. ])
  AC_DEFINE(NO_ERETURN, 1, [ Define to disable error messages in combination with a return value (the return is left intact). ])
fi

newhuff=enabled
AC_ARG_ENABLE(new-huffman,
[  --enable-new-huffman=[yes/no] use new huffman decoding scheme by Taihei (faster on modern CPUs at least, so on by default) ],
[
  if test "x$enableval" = xno; then
    newhuff=disabled
  fi
]
, [])

if test "x$newhuff" = "xenabled"; then
  AC_DEFINE(USE_NEW_HUFFTABLE, 1, [ Define for new Huffman decoding scheme. ])
fi

integers=fast
AC_ARG_ENABLE(int-quality,
[  --enable-int-quality=[yes/no] use rounding instead of fast truncation for integer output, where possible ],
[
  if test "x$enableval" = xyes; then
    integers=quality
    AC_DEFINE(ACCURATE_ROUNDING,   1, [ Define to use proper rounding. ])
  fi
], [])

int16=enabled
AC_ARG_ENABLE(16bit,
              [  --disable-16bit=[no/yes] no 16 bit integer output ],
              [
                if test "x$enableval" = xno; then
                  int16="disabled"
                fi
              ], [])

int8=enabled
AC_ARG_ENABLE(8bit,
              [  --disable-8bit=[no/yes] no 8 bit integer output ],
              [
                if test "x$enableval" = xno; then
                  int8="disabled"
                fi
              ], [])

int32=enabled
AC_ARG_ENABLE(32bit,
              [  --disable-32bit=[no/yes] no 32 bit integer output (also 24 bit) ],
              [
                if test "x$enableval" = xno; then
                  int32="disabled"
                fi
              ], [])

real=enabled
AC_ARG_ENABLE(real,
              [  --disable-real=[no/yes] no real (floating point) output ],
              [
                if test "x$enableval" = xno; then
                  real="disabled"
                fi
              ], [])

equalizer=enabled
AC_ARG_ENABLE(equalizer,
              [  --disable-equalizer=[no/yes] no equalizer support ],
              [
                if test "x$enableval" = xno; then
                  equalizer="disabled"
                fi
              ], [])

			  
AC_ARG_WITH([cpu], [
  --with-cpu=generic[[_fpu]]      Use generic processor code with floating point arithmetic
  --with-cpu=generic_float      Plain alias to generic_fpu now... float output is a normal runtime option!
  --with-cpu=generic_nofpu      Use generic processor code with fixed point arithmetic (p.ex. ARM)
  --with-cpu=generic_dither     Use generic processor code with floating point arithmetic and dithering for 1to1 16bit decoding.
  --with-cpu=i386[[_fpu]]         Use code optimized for i386 processors with floating point arithmetic
  --with-cpu=i386_nofpu         Use code optimized for i386 processors with fixed point arithmetic
  --with-cpu=i486         Use code optimized for i486 processors (only usable alone!)
  --with-cpu=i586         Use code optimized for i586 processors
  --with-cpu=i586_dither  Use code optimized for i586 processors with dithering (noise shaping), adds 256K to binary size
  --with-cpu=3dnow         Use code optimized for 3DNow processors
  --with-cpu=3dnow_vintage Use code optimized for older 3DNow processors (K6 family)
  --with-cpu=3dnowext      Use code optimized for 3DNowExt processors (K6-3+, Athlon)
  --with-cpu=3dnowext_alone     Really only 3DNowExt decoder, without 3DNow fallback for flexible rate
  --with-cpu=3dnow_vintage       Use code optimized for older extended 3DNow processors (like K6-III+)
  --with-cpu=mmx          Use code optimized for MMX processors
  --with-cpu=mmx_alone          Really only MMX decoder, without i586 fallback for flexible rate
  --with-cpu=sse          Use code optimized for SSE processors
  --with-cpu=sse_vintage  Use code optimized for older SSE processors (plain C DCT36)
  --with-cpu=sse_alone          Really only SSE decoder, without i586 fallback for flexible rate
  --with-cpu=avx          Use code optimized for x86-64 with AVX processors
  --with-cpu=x86          Pack all x86 opts into one binary (excluding i486, including dither)
  --with-cpu=x86-64       Use code optimized for x86-64 processors (AMD64 and Intel64, including AVX and dithered generic)
  --with-cpu=altivec      Use code optimized for Altivec processors (PowerPC G4 and G5)
  --with-cpu=ppc_nofpu    Use code optimized for PowerPC processors with fixed point arithmetic
  --with-cpu=neon         Use code optimized for ARM NEON SIMD engine (Cortex-A series)
  --with-cpu=arm_fpu      Pack neon and generic[[_dither]] decoders, for ARM processors with FPU and/or NEON
  --with-cpu=arm_nofpu    Use code optimized for ARM processors with fixed point arithmetic
  --with-cpu=neon64       Use code optimized for AArch64 NEON SIMD engine
  --with-cpu=aarch64      Pack neon64 and generic[[_dither]] decoders, for 64bit ARM processors
])

use_yasm=auto
AC_ARG_ENABLE(yasm,
              [  --enable-yasm=[no/yes] enforce yasm instad of default assembler for some optimizations (AVX, currently) ],
              [
                if test "x$enableval" = xyes; then
                  use_yasm="enabled"
                else
                  use_yasm="disabled"
                fi
              ], [])

AC_ARG_ENABLE(ieeefloat,
  [  --enable-ieeefloat=[yes/no] use special hackery relying on IEEE 754 floating point storage format (to accurately round to 16 bit integer at bit more efficiently in generic decoder, enabled by default, disable in case you have a very special computer) ],
  [
    if test "x$enableval" = xyes; then
      ieee=enabled
    else
      ieee=disabled
    fi
  ], [ ieee=enabled ])

if test "x$ieee" = xenabled; then
  echo "We assume IEEE754 floating point format."
  AC_DEFINE(IEEE_FLOAT,   1, [ Define to indicate that float storage follows IEEE754. ])
fi

sys_cppflags=
newoldwritesample=disabled
case $host in
  aarch64-*linux*|arm64-*linux*|aarch64-*bsd*|arm64-*bsd*|aarch64-apple-darwin*|arm64-apple-darwin*)
    cpu_type="aarch64"
  ;;
  arm*-*-linux*-*eabihf|armv7hl*-*-linux*)
    cpu_type="arm_fpu"
  ;;
  arm*-*-linux*)
    # check that... perhaps we are better off on arm with kernel math emulation
    cpu_type="arm_nofpu"
  ;;
  armv7*-apple-darwin*)
    cpu_type="arm_fpu"
  ;;
  i386-*-linux*|i386-*-kfreebsd*-gnu)
    cpu_type="i386_fpu"
    newoldwritesample=enabled
  ;;
  i486-*-linux*|i486-*-kfreebsd*-gnu)
    cpu_type="i486"
    newoldwritesample=enabled
  ;;
  i586-*-linux*|i586-*-kfreebsd*-gnu)
    cpu_type="x86"
    newoldwritesample=enabled
  ;;
  i686-*-linux*|i686-*-kfreebsd*-gnu)
    cpu_type="x86"
    newoldwritesample=enabled
  ;;
  x86_64-*-linux*|x86_64-*-kfreebsd*-gnu)
    cpu_type="x86-64"
  ;;
  *-*-linux*|*-*-kfreebsd*-gnu)
    cpu_type="generic_fpu"
  ;;
  i?86-apple-darwin10*)
    AC_MSG_CHECKING([if CPU type supports x86-64])
    case `sysctl -n hw.optional.x86_64` in
      1)
        AC_MSG_RESULT([yes])
        cpu_type="x86-64"
      ;;
      *)
        AC_MSG_RESULT([no])
        cpu_type="x86"
        newoldwritesample=enabled
      ;;
    esac
  ;;
  i?86-apple-darwin*)
    cpu_type="x86"
    newoldwritesample=enabled
  ;;
  x86_64-apple-darwin*)
    cpu_type="x86-64"
  ;;
  *-apple-darwin*)
    AC_MSG_CHECKING([if CPU type supports AltiVec])
    case `machine` in
      ppc7400 | ppc7450 | ppc970)
        AC_MSG_RESULT([yes])
        cpu_type="altivec"
      ;;
      *)
        AC_MSG_RESULT([no])
        cpu_type="generic_fpu"
      ;;
    esac
  ;;
  i?86-*-dragonfly* | i?86-*-freebsd* | i?86-*-midnightbsd* | i?86-*-mirbsd* | i?86-*-netbsd* | i?86-*-openbsd* | i?86-*-haiku*)
    cpu_type="x86"
    newoldwritesample=enabled
  ;;
  x86_64-*-dragonfly* | x86_64-*-freebsd* | x86_64-*-midnightbsd* | x86_64-*-mirbsd* | x86_64-*-netbsd* | x86_64-*-openbsd* | x86_64-*-haiku*)
    cpu_type="x86-64"
  ;;
  *-*-dragonfly* | *-*-freebsd* | *-*-midnightbsd* | *-*-mirbsd* | *-*-netbsd* | *-*-openbsd*)
    cpu_type="generic_fpu"
  ;;
  i386-*-solaris*)
    cpu_type=x86
    newoldwritesample=enabled
  ;;
  x86_64-*-solaris*)
    cpu_type=x86-64
  ;;
  *-*-solaris*)
    cpu_type="generic_fpu"
  ;;
  # os2-emx = OS/2 with some Unix fun; so p.ex. buffer works.
  # Till we sorted out the assembler troubles, generic CPU is default.
  i386-pc-os2-emx)
    cpu_type=generic_fpu
    newoldwritesample=enabled
  ;;
  x86_64-pc-os2-emx)
    # We are optimistic hat the future knows OS/2 on x86-64;-)
    cpu_type=generic_fpu
  ;;
  *-pc-os2-emx)
    cpu_type="generic_fpu"
  ;;
  *-dec-osf*)
    cpu_type="generic_fpu"
  ;;
  x86_64-*-cygwin*)
    cpu_type="x86-64"
  ;;
  i686-*-cygwin*)
    cpu_type="x86"
    newoldwritesample=enabled
  ;;
  i586-*-cygwin*)
    cpu_type="x86"
    newoldwritesample=enabled
  ;;
  i486-*-cygwin*)
    cpu_type="i486"
    newoldwritesample=enabled
  ;;
  i386-*-cygwin*)
    cpu_type="i386"
    newoldwritesample=enabled
  ;;
  *-cygwin*)
    cpu_type="generic_fpu"
  ;;
  i@<:@3-7@:>@86-*-mingw32*)
    LIBS="$LIBS"
    buffer=disabled
    cpu_type="x86"
    newoldwritesample=enabled
  ;;
  x86_64-*-mingw32*)
    LIBS="$LIBS"
    buffer=disabled
    cpu_type="x86-64"
  ;;
  i386-*-nto-qnx*)
    cpu_type="x86"
    newoldwritesample=enabled
  ;;
  *-ibm-aix*)
    AC_MSG_WARN([AIX system detected. You might want to --disable-largefile when trouble about conflicting types for lseek64 and friends occurs.])
    # Altivec instead? It is developed for MacOS ...
    cpu_type=generic_fpu
    # no struct winsize without _ALL_SOURCE
    sys_cppflags=-D_ALL_SOURCE
  ;;
	i386-*)
		AC_MSG_WARN([Unknown host operating system])
		cpu_type="i386"
		buffer=disabled
		sys_cppflags=-DGENERIC
    newoldwritesample=enabled
	;;
	i486-*)
		AC_MSG_WARN([Unknown host operating system])
		cpu_type="i486"
		buffer=disabled
		sys_cppflags=-DGENERIC
    newoldwritesample=enabled
	;;
	i586-*)
		AC_MSG_WARN([Unknown host operating system])
		cpu_type="x86"
		buffer=disabled
		sys_cppflags=-DGENERIC
    newoldwritesample=enabled
	;;
	i686-*)
		AC_MSG_WARN([Unknown host operating system])
		cpu_type="x86"
		buffer=disabled
		sys_cppflags=-DGENERIC
    newoldwritesample=enabled
	;;
	x86_64-*)
		AC_MSG_WARN([Unknown host operating system])
		cpu_type="x86-64"
		buffer=disabled
		sys_cppflags=-DGENERIC
	;;
  *)
  	AC_MSG_WARN([Unknown host operating system])
    cpu_type="generic_fpu"
    buffer=disabled
    sys_cppflags=-DGENERIC
  ;;
esac

AC_ARG_ENABLE(buffer,
  [  --enable-buffer=[yes/no] disable audio buffer code (default uses system whitelist... proper checks later) ],
  [
    if test "x$enableval" = xyes
    then
      echo "Note: Enabling buffer per request... perhaps it will not build anyway."
      buffer="enabled"
    else
      echo "Note: Disabling buffer per request."
      buffer="disabled"
    fi
  ]
)

AC_ARG_ENABLE(newoldwritesample,
[  --enable-newoldwritesample=[no/yes] enable new/old WRITE_SAMPLE macro for non-accurate 16 bit output, faster on certain CPUs (default on on x86-32)],
[
	if test "x$enableval" = xyes
	then
		newoldwritesample=enabled
	else
		newoldwritesample=disabled
	fi
])

dnl Did user choose other CPU type ?
if test "x$with_cpu" != "x"; then
	cpu_type=$with_cpu
fi

# Flag for 32 bit synth output or post-processing.
case "$cpu_type" in
*_nofpu)
  synth32=false
  AC_DEFINE(NO_SYNTH32, 1, [ Define for post-processed 32 bit formats. ])
;;
*)
  synth32=true
;;
esac

if test "x$int16" = "xdisabled"; then
  AC_DEFINE(NO_16BIT, 1, [ Define to disable 16 bit integer output. ])
else
  DECODER_OBJ="$DECODER_OBJ synth.\$(OBJEXT)"
  DECODER_LOBJ="$DECODER_LOBJ synth.lo"
fi

# 8bit works only through 16bit
if test "x$int16" = "xdisabled"; then
  int8=disabled
fi
if test "x$int8" = "xdisabled"; then
  AC_DEFINE(NO_8BIT, 1, [ Define to disable 8 bit integer output. ])
else
  DECODER_OBJ="$DECODER_OBJ synth_8bit.\$(OBJEXT)"
  DECODER_LOBJ="$DECODER_LOBJ synth_8bit.lo"
fi

if test "x$int32" = "xdisabled"; then
  AC_DEFINE(NO_32BIT, 1, [ Define to disable 32 bit and 24 bit integer output. ])
else
  if $synth32; then
    s_fpu="$s_fpu synth_s32"
  fi
fi

if test "x$real" = "xdisabled"; then
  AC_DEFINE(NO_REAL, 1, [ Define to disable real output. ])
else
  if $synth32; then
    s_fpu="$s_fpu synth_real"
  fi
fi

if test "x$equalizer" = "xdisabled"; then
  AC_DEFINE(NO_EQUALIZER, 1, [ Define to disable equalizer. ])
fi

layer1=enabled
AC_ARG_ENABLE(layer1,
              [  --disable-layer1=[no/yes] no layer I decoding ],
              [
                if test "x$enableval" = xno; then
                  layer1="disabled"
                fi
              ], [])

if test "x$layer1" = "xdisabled"; then
  AC_DEFINE(NO_LAYER1, 1, [ Define to disable layer I. ])
else
  # layer1 needs code in layer2
  DECODER_OBJ="$DECODER_OBJ layer1.\$(OBJEXT) layer2.\$(OBJEXT)"
  DECODER_LOBJ="$DECODER_LOBJ layer1.lo layer2.lo"
fi

layer2=enabled
AC_ARG_ENABLE(layer2,
              [  --disable-layer2=[no/yes] no layer II decoding ],
              [
                if test "x$enableval" = xno; then
                  layer2="disabled"
                fi
              ], [])

if test "x$layer2" = "xdisabled"; then
  AC_DEFINE(NO_LAYER2, 1, [ Define to disable layer II. ])
else
  # layer1 may have added the objects already
  if test "x$layer1" = "xdisabled"; then
    DECODER_OBJ="$DECODER_OBJ layer2.\$(OBJEXT)"
    DECODER_LOBJ="$DECODER_LOBJ layer2.lo"
  fi
fi

layer3=enabled
AC_ARG_ENABLE(layer3,
              [  --disable-layer3=[no/yes] no layer III decoding ],
              [
                if test "x$enableval" = xno; then
                  layer3="disabled"
                fi
              ], [])

if test "x$layer3" = "xdisabled"; then
  AC_DEFINE(NO_LAYER3, 1, [ Define to disable layer III. ])
else
  DECODER_OBJ="$DECODER_OBJ layer3.\$(OBJEXT)"
  DECODER_LOBJ="$DECODER_LOBJ layer3.lo"
fi

AC_ARG_WITH([audio], [
  --with-audio=<list of modules>  Select a list (or only one) of audio output modules (comma or space separated list).
])
AC_ARG_WITH([default-audio], [
  --with-default-audio=aix            Use AIX as default audio output sub-system
  --with-default-audio=alib           Use Alib as default audio output sub-system (for HPUX)
  --with-default-audio=alsa           Use ALSA as default audio output sub-system (libasound)
  --with-default-audio=tinyalsa       Use ALSA as default audio output sub-system (tinyalsa)
  --with-default-audio=arts           Use aRts as default audio output sub-system (KDE sound server)
  --with-default-audio=dummy          Use dummy as default audio (when no sound card is available)
  --with-default-audio=esd            Use ESoundD as default audio output sub-system
  --with-default-audio=hp             Use HP as default audio output sub-system
  --with-default-audio=jack           Use JACK as default low-latency audio server
  --with-default-audio=coreaudio      Use Mac OS X as default audio output sub-system (CoreAudio)
  --with-default-audio=mint           Use MinT as default audio output sub-system (Atari)
  --with-default-audio=nas            Use NAS as default audio output (Network Audio System)
  --with-default-audio=os2            Use OS2 as default audio output sub-system
  --with-default-audio=oss            Use OSS as default audio output sub-system (/dev/dsp)
  --with-default-audio=portaudio      Use PortAudio as default audio output sub-system
  --with-default-audio=pulse          Use Pulse audio server as default audio output sub-system
  --with-default-audio=qsa            Use QSA as default audio output sub-system
  --with-default-audio=sdl            Use SDL as default audio output sub-system (Simple DirectMedia Layer)
  --with-default-audio=sgi            Use SGI as default audio output sub-system (IRIX)
  --with-default-audio=sndio          Use OpenBSD's sndio as default audio output sub-system
  --with-default-audio=sun            Use Sun as default audio output sub-system (/dev/audio)
  --with-default-audio=win32          Use Win32 audio as default audio output sub-system
  --with-default-audio=win32_wasapi   Use Win32 wasapi audio as default audio output sub-system
])

AC_ARG_WITH([optimization], [
  --with-optimization=0   No Optimization
  --with-optimization=1   Limited Optimization (-O) (for gcc)
  --with-optimization=2   Default Optimization (-O2 ...) (for gcc)
  --with-optimization=3   More Optimize than default (-O3 ...) (for gcc)
  --with-optimization=4   Optimize yet more (-O4 ...) (for gcc)
])

AC_ARG_WITH([seektable], [
  --with-seektable=<size> choose size of seek index table (0 disables it), default 1000
])


dnl ############## Modules

# Dummy audio output module is always supported
output_modules="dummy"

dnl ############## Assembler, compiler properties

# based on posting from John Dalgliesh <johnd@defyne.org> on ffmpeg (LGPL) mailing list
# extended to use balign if present
AC_MSG_CHECKING([if .balign is present])
echo '.balign 4' > conftest.s
if $CCAS -c -o conftest.o conftest.s 1>/dev/null 2>&1; then

  AC_MSG_RESULT([yes])
  AC_DEFINE(ASMALIGN_BALIGN, 1, [ Define if .balign is present. ])

else

AC_MSG_RESULT([no])

# find if .align arg is power-of-two or not
asmalign_exp="unknown"
if test x"$asmalign_exp" = xunknown; then
	AC_MSG_CHECKING([if .align takes 2-exponent])
	asmalign_exp="no"
	echo '.align 3' > conftest.s
	if $CCAS -c -o conftest.o conftest.s 1>/dev/null 2>&1; then
		asmalign_exp="yes"
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
	fi
	rm -f conftest.o conftest.s
fi
if test x"$asmalign_exp" = xyes; then
	AC_DEFINE(ASMALIGN_EXP, 1, [ Define if .align takes 3 for alignment of 2^3=8 bytes instead of 8. ])
else
	AC_DEFINE(ASMALIGN_BYTE, 1, [ Define if .align just takes byte count. ])
fi

fi

ccalign="unknown"
if test x"$ccalign" = xunknown; then
	AC_MSG_CHECKING([__attribute__((aligned(16)))])
	ccalign="no"
	echo '__attribute__((aligned(16))) float var;' > conftest.c
	if $CC -c -o conftest.o conftest.c >/dev/null 2>&1; then
		ccalign="yes"
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
	fi
	rm -f conftest.o conftest.c
fi

dnl We apply alignment hints only to cpus that need it.
dnl See further below for the definition of CCALIGN

avx_support="unknown"
if test x"$avx_support" = xunknown; then
	AC_MSG_CHECKING([if assembler supports AVX instructions])
	avx_support="no"
	echo '.text' > conftest.s
	echo 'vaddps %ymm0,%ymm0,%ymm0' >> conftest.s
	if $CCAS -c -o conftest.o conftest.s 1>/dev/null 2>&1; then
		avx_support="yes"
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
	fi
	rm -f conftest.o conftest.s
fi

check_yasm=no
if test x"$avx_support" = xno || test x"$use_yasm" = xenabled; then
  check_yasm=yes
fi
if test x"$use_yasm" = xdisabled; then
  check_yasm=no
fi

if test x"$check_yasm" = xyes; then
	AC_CHECK_PROGS(YASM,[yasm],no)
	if test x"$YASM" != xno; then
		AC_MSG_CHECKING([if $YASM supports GAS syntax and AVX instructions])
		echo '.text' > conftest.s
		echo 'vaddps %ymm0,%ymm0,%ymm0' >> conftest.s
		if $YASM -pgas -rcpp -o conftest.o conftest.s 1>/dev/null 2>&1; then
			avx_support="yes"
			YASMFLAGS="-pgas -rgas -mamd64"
			AC_MSG_RESULT([yes])
		else
			AC_MSG_RESULT([no])
		fi
		rm -f conftest.o conftest.s
	else
		if test x"$use_yasm" = xenabled; then
			AC_MSG_ERROR([Yasm enforced but not found!])
		fi
	fi
else
	YASM="no"
fi

if test "x$cpu_type" = "xavx"; then
	if test "x$avx_support" != "xyes"; then
		AC_MSG_ERROR([Assembler doesn't understand AVX instructions.])
	fi
fi

AC_SUBST(YASM)
AC_SUBST(YASMFLAGS)

dnl ############## Really basic headers, needed for other checks.

AC_HEADER_STDC
dnl Is it too paranoid to specifically check for stdint.h and limits.h?
AC_CHECK_HEADERS([stdio.h stdlib.h string.h unistd.h sched.h sys/ioctl.h sys/types.h stdint.h limits.h inttypes.h sys/time.h sys/wait.h sys/resource.h sys/signal.h signal.h sys/select.h dirent.h sys/stat.h])

dnl ############## Types

dnl Large file support stuff needs cleanup. There are superfluous variables.

dnl Detect large file support, enable switches if needed.
AC_SYS_LARGEFILE
dnl If we do have a switch for large files, rename off_t-aware API calls.
dnl Using the file_offset_bits variable here is fine for linux (possibly Solaris),
dnl Others... we'll have to see.
dnl Note: I started writing this with with multiline replacements.
dnl Does not work. Automake insists on putting these into Makefiles where they break things.
dnl It is also assumed that a system that does not set file offset bits is not
dnl sensitive to largefile changes, i.e. FreeBSD always using 64 bit off_t.
if test "x$ac_cv_sys_file_offset_bits" = x || echo "$ac_cv_sys_file_offset_bits" | $GREP  '@<:@^0-9@:>@' > /dev/null; then
	dnl if it has non-numeric chars or is empty... ignore...
	LFS_LOBJ=
	largefile_sensitive=no
else
	# Add dual-mode wrapper code.
	LFS_LOBJ=lfs_wrap.lo
	largefile_sensitive=yes
fi

# Using the lower level macros instead of AC_TYPE_* for compatibility with not freshest autoconf.
AC_CHECK_TYPE(size_t, unsigned long)
AC_CHECK_TYPE(uintptr_t, unsigned long)
AC_CHECK_TYPE(ssize_t, long)
AC_CHECK_TYPE(off_t,  long int)
AC_CHECK_TYPE(int32_t, int)
AC_CHECK_TYPE(int64_t, long long)
AC_CHECK_TYPE(uint32_t, unsigned int)
AC_CHECK_TYPE(int16_t, short)
AC_CHECK_TYPE(uint16_t, unsigned short)
AC_CHECK_SIZEOF(size_t,4)
AC_CHECK_SIZEOF(ssize_t,4)
AC_CHECK_SIZEOF(off_t,4)
AC_CHECK_SIZEOF(int32_t)
AC_CHECK_SIZEOF(long,4)

dnl The native type used for aliases is what off_t maps to without any largefile-
dnl enabling switches. So, it's long int if the system is largefile-senstive,
dnl but it is actual plain off_t if the system does not have such switches.
if test "x$largefile_sensitive" = xyes; then
  lfs_alias_type=long
  lfs_alias_size=$ac_cv_sizeof_long
else
  lfs_alias_type=off_t
  lfs_alias_size=$ac_cv_sizeof_off_t
fi

if test "x$lfs_alias_size" = "x"; then 
  AC_MSG_ERROR([Cannot determine sizeof(lfs_alias_t)?])
else
  LFS_ALIAS_BITS=`expr "$lfs_alias_size" "*" "8"`
  AC_DEFINE_UNQUOTED([lfs_alias_t], $lfs_alias_type,
    [Define to the native offset type (long or actually off_t).])
  AC_DEFINE_UNQUOTED([LFS_ALIAS_BITS], $LFS_ALIAS_BITS,
    [Define this to the size of native offset type in bits, used for LFS alias functions.])
fi

lfs_alias=enabled
AC_ARG_ENABLE(lfs-alias,
              [  --disable-lfs-alias disable alias wrappers for largefile bitness (mpg123_seek_32 or mpg123_seek_64 in addition to mpg123_seek, or the other way around; It is a mess, do not play with this!) ],
              [
                if test "x$enableval" = xno; then
                  lfs_alias="disabled"
                fi
              ],
              [
                case $host in
                *-cygwin*)
                  lfs_alias="disabled"
                  AC_MSG_NOTICE([lfs-alias disabled for Cygwin, use --enable-lfs-alias explicitly to enable, only if you know what you are doing])
                ;;
                esac

              ])

AC_MSG_CHECKING([if we want to enable alias wrappers for largefile])
if test "x$lfs_alias" = "xenabled"; then
  AC_MSG_RESULT([yes])
  LFS_LOBJ="$LFS_LOBJ lfs_alias.lo"
else
  AC_MSG_RESULT([no])
fi

# Again, prepend path for non-recursive make.
LFS_LOBJ=`for i in $LFS_LOBJ; do printf ' src/libmpg123/%s' $i; done`
AC_SUBST(LFS_LOBJ)


dnl ############## Function Checks

AC_FUNC_MMAP

# Check if system supports termios
AC_SYS_POSIX_TERMIOS
if test "x$ac_cv_sys_posix_termios" = "xyes"; then
  AC_DEFINE_UNQUOTED([HAVE_TERMIOS], 1, 
  [Define this if you have the POSIX termios library])
fi

AC_CHECK_FUNCS( random )

# Check for sched_setscheduler
AC_CHECK_FUNCS( sched_setscheduler setuid getuid)

# Check for setpriority
AC_CHECK_FUNCS( setpriority )

AC_CHECK_FUNCS( strerror )

AC_CHECK_FUNCS( setlocale nl_langinfo )

AC_CHECK_FUNCS( atoll )

AC_CHECK_FUNCS( mkfifo, [ have_mkfifo=yes ], [ have_mkfifo=no ] )

dnl ############## Header and Library Checks

# locale headers
AC_CHECK_HEADERS([locale.h langinfo.h])
# Headers for network (http) stuff
network_type=Unknown
AC_CHECK_HEADERS([netdb.h sys/param.h sys/socket.h netinet/in.h arpa/inet.h])
if test "x$ac_cv_header_netdb_h" = "xyes"      &&
   test "x$ac_cv_header_sys_param_h" = "xyes"  &&
   test "x$ac_cv_header_sys_socket_h" = "xyes" &&
   test "x$ac_cv_header_netinet_in_h" = "xyes" &&
   test "x$ac_cv_header_arpa_inet_h"  = "xyes"; then
  have_network=yes
  network_type=Posix
else
  have_network=no
fi

dnl trying to get that socket lib settled in one line
AC_SEARCH_LIBS(gethostbyname, nsl socket network) 
dnl OK, two lines... Solaris needs -lnsl -lsocket
AC_SEARCH_LIBS(socket, socket) 

AC_CHECK_FUNCS( getaddrinfo, [ have_ipv6=yes ], [ have_ipv6=no ] )

APR_CHECK_GETADDRINFO_ADDRCONFIG()

# Substitutions for the installable mpg123.h header
if test "x$ac_cv_header_stdio_h" = "xyes"; then
	INCLUDE_STDIO_H="#include <stdio.h>"
else
	INCLUDE_STDIO_H="/* #include <stdio.h> is not available on this system */"
fi
AC_SUBST(INCLUDE_STDIO_H)

if test "x$ac_cv_header_stdlib_h" = "xyes"; then
	INCLUDE_STDLIB_H="#include <stdlib.h>"
else
	INCLUDE_STDLIB_H="/* #include <stdlib.h> is not available on this system */"
fi
AC_SUBST(INCLUDE_STDLIB_H)

if test "x$ac_cv_header_sys_types_h" = "xyes"; then
	INCLUDE_SYS_TYPE_H="#include <sys/types.h>"
else
	INCLUDE_SYS_TYPE_H="/* #include <sys/types.h> is not available on this system */"
fi
AC_SUBST(INCLUDE_SYS_TYPE_H)


# Checks for maths libraries.
AC_CHECK_LIB([m], [sqrt])
AC_CHECK_LIB([mx], [powf])

# attempt to make the signal stuff work... also with GENERIC - later
#if test x"$ac_cv_header_sys_signal_h" = xyes; then
#	AC_CHECK_FUNCS( sigemptyset sigaddset sigprocmask sigaction )
#	if test x"$ac_cv_func_sigemptyset" = xyes &&
#	   test x"$ac_cv_func_sigaddset" = xyes &&
#	   test x"$ac_cv_func_sigprocmask" = xyes &&
#	   test x"$ac_cv_func_sigaction" = xyes; then
#	AC_DEFINE( 
#fi

dnl ############## Choose compiler flags and CPU

# do not assume gcc here, so no flags by default
ADD_CFLAGS=""
ADD_CPPFLAGS="$sys_cppflags"
ADD_LDFLAGS=""
LIBS="$LIBS"

# Consider moving that stuff.
AC_CHECK_HEADER([os2.h], [ADD_CPPFLAGS="$ADD_CPPFLAGS -DOS2"])
# On OS/2, we need to link to os2term to make terminal control actually work.
AC_CHECK_LIB([os2term], [tcsetattr], [ADD_LDFLAGS="$ADD_LDFLAGS -los2term"])

# If debugging is enabled, just enable debugging symbols.
# All other stuff enters nagging territory.
if test x"$debugging" = xenabled; then
	ADD_CFLAGS="-g"
fi
# gcc specific...
if test x"$GCC" = xyes; then
	if test x"$nagging" = xenabled; then
		ADD_CFLAGS="$ADD_CFLAGS -Wall -Werror -std=c89 -pedantic -DPLAIN_C89"
	fi
fi

dnl Only try the attribute_align_arg mumbo-jumbo on x86, x86-64 warns/errors out on that attribute.
dnl ...dunno even what about other architectures.
case $host in
  i?86-*)
    AC_DEFINE(ABI_ALIGN_FUN, 1, [ Define if your architecture wants/needs/can use attribute_align_arg and alignment checks. It is for 32bit x86... ])
 ;;
esac

s_altivec="synth_altivec dct64_altivec"
s_i386="dct64_i386"
s_i486="$s_i386 synth_i486 dct64_i486"
s_i586="$s_i386 synth_i586"
s_i586d="$s_i386 synth_i586_dither"
s_3dnow="$s_i386 synth_3dnow dct64_3dnow equalizer_3dnow"
s_3dnowext="$s_i386 dct64_3dnowext tabinit_mmx synth_3dnowext"
s_3dnow_vintage=$s_3dnow
s_3dnowext_vintage=$s_3dnowext
if test "x$layer3" = "xenabled"; then
  s_3dnow_vintage="$s_3dnow_vintage dct36_3dnow"
  s_3dnowext_vintage="$s_3dnowext_vintage dct36_3dnowext"
fi
s_mmx="$s_i386 dct64_mmx tabinit_mmx synth_mmx"
s_sse_vintage="$s_i386 tabinit_mmx dct64_sse_float synth_sse_float synth_stereo_sse_float synth_sse_s32 synth_stereo_sse_s32 "
s_sse="$s_sse_vintage dct36_sse"
s_x86_64="dct36_x86_64 dct64_x86_64_float synth_x86_64_float synth_x86_64_s32 synth_stereo_x86_64_float synth_stereo_x86_64_s32"
s_x86_64_mono_synths="synth_x86_64_float synth_x86_64_s32"
s_x86_64_avx="dct36_avx dct64_avx_float synth_stereo_avx_float synth_stereo_avx_s32"
s_x86multi="getcpuflags"
s_x86_64_multi="getcpuflags_x86_64"
s_dither="dither"
s_neon="dct36_neon dct64_neon_float synth_neon_float synth_neon_s32 synth_stereo_neon_float synth_stereo_neon_s32"
s_neon64="dct36_neon64 dct64_neon64_float synth_neon64_float synth_neon64_s32 synth_stereo_neon64_float synth_stereo_neon64_s32"
s_arm_multi="getcpuflags_arm check_neon"

# choose optimized 16bit decoder for SSE, quality or fast
# note: supporting deactivation of output formats for these decoders would need more logic here
if test "x$integers" = "xquality"; then
  s_sse="$s_sse synth_sse_accurate synth_stereo_sse_accurate"
  s_sse_vintage="$s_sse_vintage synth_sse_accurate synth_stereo_sse_accurate"
  s_x86_64="$s_x86_64 synth_x86_64_accurate synth_stereo_x86_64_accurate"
  s_x86_64_mono_synths="$s_x86_64_mono_synths synth_x86_64_accurate"
  s_x86_64_avx="$s_x86_64_avx synth_stereo_avx_accurate"
  s_arm="synth_arm_accurate"
  s_neon="$s_neon synth_neon_accurate synth_stereo_neon_accurate"
  s_neon64="$s_neon64 synth_neon64_accurate synth_stereo_neon64_accurate"
else
  s_sse="$s_sse dct64_sse synth_sse" # no stereo
  s_sse_vintage="$s_sse_vintage dct64_sse synth_sse" # no stereo
  s_x86_64="$s_x86_64 synth_x86_64 dct64_x86_64 synth_stereo_x86_64"
  s_x86_64_mono_synths="$s_x86_64_mono_synths synth_x86_64"
  s_x86_64_avx="$s_x86_64_avx dct64_avx synth_stereo_avx"
  s_arm="synth_arm"
  s_neon="$s_neon dct64_neon synth_neon synth_stereo_neon"
  s_neon64="$s_neon64 dct64_neon64 synth_neon64 synth_stereo_neon64"
fi

dnl CPU specific compiler flags and sources
case $cpu_type in
  generic)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_GENERIC -DREAL_IS_FLOAT"
    more_sources="$s_fpu"
    ccalign=no
  ;;
  generic_dither)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_GENERIC_DITHER -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_dither"
    ccalign=no
  ;;
dnl Not disabling buffer for float mode... check that this is OK now!
  generic_fpu | generic_float)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_GENERIC -DREAL_IS_FLOAT"
    more_sources="$s_fpu"
    ccalign=no
  ;;
  generic_nofpu)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_GENERIC -DREAL_IS_FIXED"
    more_sources=
    ccalign=no
  ;;
  ppc_nofpu)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_GENERIC -DOPT_PPC -DREAL_IS_FIXED"
    more_sources=
    ccalign=no
  ;;
  arm_nofpu)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_ARM -DREAL_IS_FIXED"
    more_sources="$s_arm"
    ccalign=no
  ;;
  altivec)
    ADD_CFLAGS="$ADD_CFLAGS -maltivec"
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DREAL_IS_FLOAT -DOPT_ALTIVEC"
    more_sources="$s_altivec $s_fpu"
    AC_MSG_CHECKING([if $CC accepts -faltivec])
    touch conftest.c
    if $CC -faltivec -c -o conftest.o conftest.c >/dev/null 2>&1; then
        ADD_CFLAGS="$ADD_CFLAGS -faltivec"
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
    fi
    rm -f conftest.o conftest.c
  ;;
  neon)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_NEON -DREAL_IS_FLOAT"
    more_sources="$s_neon $s_fpu"
  ;;
  arm_fpu)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MULTI -DOPT_GENERIC -DOPT_GENERIC_DITHER -DOPT_NEON -DREAL_IS_FLOAT"
    more_sources="$s_neon $s_fpu $s_dither $s_arm_multi"
  ;;
  neon64)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_NEON64 -DREAL_IS_FLOAT"
    more_sources="$s_neon64 $s_fpu"
  ;;
  aarch64)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MULTI -DOPT_GENERIC -DOPT_GENERIC_DITHER -DOPT_NEON64 -DREAL_IS_FLOAT"
    more_sources="$s_neon64 $s_fpu $s_dither $s_arm_multi"
  ;;
  i386) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_I386 -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_i386"
    ccalign=no
  ;;
  i386_fpu) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_I386 -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_i386"
    ccalign=no
  ;;
  i386_nofpu) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_I386 -DREAL_IS_FIXED"
    more_sources="$s_i386"
    ccalign=no
  ;;
  i486) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_I486 -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_i486"
    ccalign=no
  ;;
  i586) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_I586 -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_i586"
  ;;
  i586_dither) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_I586_DITHER -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_i586d $s_dither"
  ;;
  3dnow)
    # legacy 3dnow had the 3dnow paired with i586...
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MULTI -DOPT_I586 -DOPT_3DNOW -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_i586 $s_3dnow $s_x86multi "
  ;;
  3dnow_vintage)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MULTI -DOPT_I586 -DOPT_3DNOW_VINTAGE -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_i586 $s_3dnow_vintage $s_x86multi "
  ;;
  3dnow_alone)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_3DNOW -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_3dnow"
  ;;
  3dnowext_alone) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_3DNOWEXT -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_3dnowext"
  ;;
  3dnowext_vintage) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MULTI -DOPT_3DNOW -DOPT_3DNOWEXT_VINTAGE -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_3dnowext_vintage $s_3dnow $s_x86multi"
  ;;
  3dnowext) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MULTI -DOPT_3DNOW -DOPT_3DNOWEXT -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_3dnowext $s_3dnow $s_x86multi"
  ;;
  mmx_alone|mmx) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MMX -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_mmx"
  ;;
  sse_alone|sse) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_SSE -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_sse"
  ;;
  sse_vintage) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_SSE_VINTAGE -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_sse_vintage"
  ;;
  avx) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_AVX -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_x86_64_avx $s_x86_64_mono_synths"
	if test "x$YASM" != "xno"; then
		use_yasm_for_avx="yes"
	fi
  ;;
  x86|x86_dither)
    # plain C dct36 always there, for vintage/non-vintage
    # Selection of non-/vintage sources implies that
    # $s_3dnow_vintage contains $s_3dnow,
    # $s_sse contains $s_sse_vintage.
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MULTI -DOPT_GENERIC -DOPT_GENERIC_DITHER -DOPT_I386 -DOPT_I586 -DOPT_I586_DITHER -DOPT_MMX -DOPT_3DNOW -DOPT_3DNOW_VINTAGE -DOPT_3DNOWEXT -DOPT_3DNOWEXT_VINTAGE -DOPT_SSE -DOPT_SSE_VINTAGE -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_i386 $s_i586 $s_i586d $s_mmx $s_3dnow_vintage $s_3dnowext_vintage $s_sse $s_x86multi $s_dither"
  ;;
  x86-64_alone) 
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_X86_64 -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_x86_64"
  ;;
  x86-64|x86-64_all|x86-64_dither)
    ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_MULTI -DOPT_X86_64 -DOPT_GENERIC -DOPT_GENERIC_DITHER -DREAL_IS_FLOAT"
    more_sources="$s_fpu $s_x86_64 $s_dither $s_x86_64_multi"
	if test "x$avx_support" = "xyes"; then
		ADD_CPPFLAGS="$ADD_CPPFLAGS -DOPT_AVX"
		more_sources="$more_sources $s_x86_64_avx"
		if test "x$YASM" != "xno"; then
			use_yasm_for_avx="yes"
		fi
	fi
  ;;
  *)
  	AC_MSG_ERROR([Unknown CPU type '$cpu_type'])
  ;;
esac

# Use yasm instead of the default assembler for AVX sources
if test "x$use_yasm_for_avx" = "xyes"; then
	case $host_os in
		*cygwin*|*mingw*)
			YASM_FORMAT="-f win64"
		;;
		*darwin*)
			YASM_FORMAT="-f macho"
		;;
		*)
			YASM_FORMAT="-f elf"
		;;
	esac
	AC_DEFINE(USE_YASM_FOR_AVX, 1, [Define to use yasm for assemble AVX sources.])
fi
AM_CONDITIONAL( [USE_YASM_FOR_AVX], [test "x$use_yasm_for_avx" = xyes] )
AC_SUBST(YASM_FORMAT)

# Check if we want feature report function.
AC_MSG_CHECKING([if we want feature report function])
feature_report=enabled
AC_ARG_ENABLE(feature_report,
	[  --disable-feature_report Disable feature report function ],
	[
		if test "x$enableval" = xno; then
		AC_MSG_RESULT([no])
		feature_report=disabled
		else
		AC_MSG_RESULT([yes])
		fi
	],
	[AC_MSG_RESULT([yes])])

if test "x$feature_report" = xenabled; then
	more_sources="$more_sources feature"
fi

# Return 0 if first arg appears in list of following arguments.
# Return 1 otherwise.
word_in_list() {
  word=$1
  shift
  # Avoid naming conflict with the outside!
  # Is local not POSIX sh?
  for given_word in "$@"
  do
    if test "x$word" = "x$given_word"; then
      return 0
    fi
  done
  return 1
}

for i in $more_sources
do
  # Make sure every file is only once in the list.
  word_in_list "$i.\$(OBJEXT)" $DECODER_OBJ || DECODER_OBJ="$DECODER_OBJ $i.\$(OBJEXT)"
  word_in_list "$i.lo" $DECODER_LOBJ || DECODER_LOBJ="$DECODER_LOBJ $i.lo"
done

# Another preprocessing step: Append prefix for non-recursive make.
# Just because $(addprefix ...) is a GNU extension.

DECODER_OBJ=`for i in $DECODER_OBJ; do printf ' src/libmpg123/%s' $i; done`
DECODER_LOBJ=`for i in $DECODER_LOBJ; do printf ' src/libmpg123/%s' $i; done`

AC_SUBST(DECODER_OBJ)
AC_SUBST(DECODER_LOBJ)

dnl Finally insert the code switch for alignment, above cpu selection having possibly changed the setting.
if test x"$ccalign" = xyes; then
	AC_DEFINE(CCALIGN, 1, [ Define if __attribute__((aligned(16))) shall be used ])
fi


dnl ############## Output module choice

# The full list of supported modules to check, first come, first serve.
check_modules="alsa tinyalsa oss coreaudio sndio sun win32 win32_wasapi os2 esd jack portaudio pulse sdl nas arts openal dummy"
# Only check qsa before all else on QNX.
# It would mask ALSA otherwise.
case $host in
  *-nto-qnx*)
    check_modules="qsa $check_modules"
  ;;
esac

# The final list.
output_modules=

check_forced=no
check_failed=no
if test "x$with_audio" != "x"; then
	check_modules="`echo $with_audio|tr , ' '` dummy"
	echo "Limiting outputs to build according to your preference: $check_modules"
	check_forced=yes
fi

PKG_PROG_PKG_CONFIG([])

# Now go through the modules to check and do the chores.
for m in $check_modules
do
	case "$m" in
		dummy)
			AC_MSG_CHECKING([if you are too dumbing dumb for the dummy])
			AC_MSG_RESULT([no])
			output_modules="$output_modules dummy"
			HAVE_DUMMY=yes
		;;
		oss)
			AC_CHECK_HEADERS([sys/soundcard.h linux/soundcard.h machine/soundcard.h])
			if test "x${ac_cv_header_sys_soundcard_h}" = "xyes" \
				 -o "x${ac_cv_header_linux_soundcard_h}" = "xyes" \
				 -o "x${ac_cv_header_machine_soundcard_h}" = "xyes";
			then
				output_modules="$output_modules oss"
				HAVE_OSS="yes"
			else
				check_failed=yes
			fi
		;;
		tinyalsa)
			TINYALSA_LIBS="-ltinyalsa"
			# Check for ALSA
			AC_CHECK_LIB( [tinyalsa], [pcm_open],
				[ AC_CHECK_HEADER( [tinyalsa/asoundlib.h],
					[ output_modules="$output_modules tinyalsa" HAVE_TINYALSA="yes"],
					[ AC_MSG_WARN([Found TINYALSA library but NOT header files on your system]) ] )
				]
			)
			if test "x$HAVE_TINYALSA" != xyes; then
				check_failed=yes
			fi
		;;
		# QSA and ALSA are not distinguishable in these tests, need to block
		# each other and play with test order depending on platform.
		alsa)
			if test "x$HAVE_QSA" = xyes; then
				HAVE_ALSA=no
			else

			ALSA_LIBS="-lasound"
			# Check for ALSA
			AC_CHECK_LIB( [asound], [snd_pcm_open],
				[ AC_CHECK_HEADER( [alsa/asoundlib.h],
					[ output_modules="$output_modules alsa" HAVE_ALSA="yes"],
					[ AC_MSG_WARN([Found ALSA library but NOT header files on your system]) ] )
				]
			)
			if test "x$HAVE_ALSA" != xyes; then
				check_failed=yes
			fi

			fi # HAVE_QSA
		;;
		qsa)
			if test "x$HAVE_ALSA" = xyes; then
				HAVE_QSA=no
			else

			QSA_LIBS="-lasound"
			# Check for QSA
			AC_CHECK_LIB( [asound], [snd_pcm_open],
				[ AC_CHECK_HEADER( [sys/asoundlib.h],
					[ output_modules="$output_modules qsa" HAVE_QSA="yes"],
					[ AC_MSG_WARN([Found QSA library but NOT header files on your system]) ] )
				]
			)
			if test "x$HAVE_QSA" != xyes; then
				check_failed=yes
			fi

			fi # HAVE_ALSA
		;;
		jack)
			# JACK module uses semaphores, hence -lpthread.
			AC_CHECK_LIB( [pthread], [sem_post],
				[ PKG_CHECK_MODULES(JACK, jack,
					[
						output_modules="$output_modules jack"
						HAVE_JACK="yes"
						JACK_LIBS="$JACK_LIBS -lpthread"
					], [ HAVE_JACK="no" check_failed=yes ] )
				],
				[ HAVE_JACK=no check_failed=yes ]
			)
		;;
		pulse)
			PKG_CHECK_MODULES(PULSE, libpulse-simple, output_modules="$output_modules pulse" HAVE_PULSE="yes", HAVE_PULSE="no" check_failed=yes)
		;;
		esd)
			PKG_CHECK_MODULES(ESD, esound, output_modules="$output_modules esd" HAVE_ESD="yes", HAVE_ESD="no" check_failed=yes)
		;;
		portaudio)
			# Remember: This looks only insane because you chose an insane tab width!
			PORTAUDIO_LIBS="-lportaudio"
			PORTAUDIO_CFLAGS=
			case $host in
				*-*-mingw32)
					# We tested portaudio with MME
					if test "x$HAVE_PORTAUDIO" != "xyes"; then
						AC_CHECK_LIB( [portaudio], [Pa_Initialize],
							[ AC_CHECK_HEADER( [portaudio.h],
								[ output_modules="$output_modules portaudio" HAVE_PORTAUDIO_WINMM="yes" HAVE_PORTAUDIO="yes" ],
								[ AC_MSG_WARN([Found PortAudio library but NOT header files on your system]) ] )
								PORTAUDIO_LIBS="$PORTAUDIO_LIBS -lwinmm"
							],
							[ HAVE_PORTAUDIO="no"],
							[ -lwinmm ]
						)
					fi
				;;
				*)
					AC_CHECK_LIB( [portaudio], [Pa_Initialize],
						[ AC_CHECK_HEADER( [portaudio.h],
							[ output_modules="$output_modules portaudio" HAVE_PORTAUDIO="yes" ],
							[ AC_MSG_WARN([Found PortAudio library but NOT header files on your system]) ] )
						]
					)
				;;
			esac
			if test "x$HAVE_PORTAUDIO" != xyes; then
				check_failed=yes
			else
				# See if we have v19 or v18
				AC_CHECK_LIB( [portaudio], [Pa_GetVersion], [:], [AC_DEFINE( [PORTAUDIO18], 1, [Define if portaudio v18 API is wanted.]) ], [$PORTAUDIO_LIBS] )
			fi
		;;
		sdl)
			PKG_CHECK_MODULES(SDL, sdl, output_modules="$output_modules sdl" HAVE_SDL="yes", HAVE_SDL="no" check_failed=yes)
		;;
		nas)
			NAS_LIBS=-laudio
			AC_CHECK_LIB( [audio], [AuOpenServer],
				[ AC_CHECK_HEADER( [audio/audiolib.h],
					[ output_modules="$output_modules nas" HAVE_NAS="yes"],
					[ AC_MSG_WARN([Found NAS library but NOT header files on your system]) ] )
				]
			)
			if test "x$HAVE_NAS" != xyes; then
				check_failed=yes
			fi
		;;
		win32)
			# Check for windows ... and win32 audio
			# Does not work... instead just check for header
			# AC_CHECK_LIB( [winmm], [waveOutOpen] )
			WIN32_LIBS=-lwinmm
			if test x$ac_cv_header_windows_h = xyes; then
				output_modules="$output_modules win32"
				HAVE_WIN32=yes
			else
				HAVE_WIN32=no
				check_failed=yes
			fi
		;;
		win32_wasapi)
			# Check for windows ... and win32 wasapi audio
			# Does not work... instead just check for header
			# AC_CHECK_LIB( [avrt], [] )
            AC_MSG_CHECKING([if we have wasapi headers])
            OLD_LIBS=$LIBS
            LIBS="$LIBS -lole32 -lavrt"
            AC_LINK_IFELSE([AC_LANG_SOURCE([
#define COBJMACROS 1
#define _WIN32_WINNT 0x601
#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <avrt.h>
int main(){
  /* UUID Checks */
  GUID *IDs[] = {
    &CLSID_MMDeviceEnumerator,
    &IID_IMMDeviceEnumerator,
    &IID_IAudioClient,
    &IID_IAudioRenderClient
  };
  return 0;
}
    ])], [HAVE_WIN32_WASAPI=yes], [HAVE_WIN32_WASAPI=no])
			LIBS=$OLD_LIBS
			WIN32_WASAPI_LIBS="-lole32 -lavrt"
			AC_MSG_RESULT([$HAVE_WIN32_WASAPI])
			if test "x$HAVE_WIN32_WASAPI" = xyes; then
				output_modules="$output_modules win32_wasapi"
			else
				check_failed=yes
			fi
		;;
		sndio)
			SNDIO_LIBS=-lsndio
			AC_CHECK_LIB([sndio], [sio_open],
				[AC_CHECK_HEADERS([sndio.h],
					[output_modules="$output_modules sndio" HAVE_SNDIO="yes"])
				]
			)
			if test "x$HAVE_SNDIO" != xyes; then
				check_failed=yes
			fi
		;;
		sun)
			AC_CHECK_HEADERS([sun/audioio.h sys/audioio.h asm/audioio.h sys/audio.h])
			if test "x${ac_cv_header_sun_audioio_h}" = "xyes" \
				 -o "x${ac_cv_header_sys_audioio_h}" = "xyes" \
				 -o "x${ac_cv_header_asm_audioio_h}" = "xyes";
			then
				output_modules="$output_modules sun"
				HAVE_SUN="yes"
			else
				check_failed=yes
			fi
		;;
		coreaudio)
			COREAUDIO_LIBS="-framework AudioToolbox"
			AC_CHECK_HEADERS([AudioUnit/AudioUnit.h AudioToolbox/AudioToolbox.h])
			if test "x${ac_cv_header_AudioUnit_AudioUnit_h}" = "xyes" \
				 -a "x${ac_cv_header_AudioToolbox_AudioToolbox_h}" = "xyes";
			then
				OLD_LIBS=$LIBS
				LIBS="$LIBS $COREAUDIO_LIBS"
    			AC_MSG_CHECKING([if AudioUnit functions are available within AudioToolbox])
				AC_TRY_LINK(
					[#include <AudioUnit/AudioUnit.h>],
					[AudioUnitInitialize(NULL);],
					[AC_MSG_RESULT([yes])],
					[AC_MSG_RESULT([no])
					 COREAUDIO_LIBS="$COREAUDIO_LIBS -framework AudioUnit"]
				)
				LIBS=$OLD_LIBS
				AC_CHECK_HEADERS([CoreServices/CoreServices.h],
					[COREAUDIO_LIBS="$COREAUDIO_LIBS -framework CoreServices"]
				)
				if test x"$modules" = xdisabled; then
					AC_MSG_WARN([Disabling buffer because of directly linked CoreAudio! Use the module if you need the buffer.])
					buffer=disabled
				fi
				output_modules="$output_modules coreaudio"
				HAVE_COREAUDIO="yes"
			else
				check_failed=yes
			fi
		;;
		arts)
			AC_MSG_CHECKING([for artsc])
			if artsc-config > /dev/null 2>&1; then
				AC_MSG_RESULT([yes])
				output_modules="$output_modules arts"
				HAVE_ARTS=yes
				ARTS_LIBS=`artsc-config --libs`
				ARTS_CFLAGS=`artsc-config --cflags`
			else
				AC_MSG_RESULT([no])
				check_failed=yes
			fi
		;;
		openal)
			AC_CHECK_HEADERS([OpenAL/al.h OpenAL/alc.h AL/al.h AL/alc.h al.h alc.h])
			if test "x${ac_cv_header_OpenAL_al_h}" = "xyes" \
				 -a "x${ac_cv_header_OpenAL_alc_h}" = "xyes";
			then #Mac OS X
				output_modules="$output_modules openal"
				OPENAL_LIBS="-framework OpenAL"
				OPENAL_CFLAGS="-DOPENAL_SUBDIR_OPENAL"
				HAVE_OPENAL="yes"
			elif test "x${ac_cv_header_AL_al_h}" = "xyes" \
				 -a "x${ac_cv_header_AL_alc_h}" = "xyes";
			then #Linux
				output_modules="$output_modules openal"
				OPENAL_LIBS="-lopenal"
				OPENAL_CFLAGS="-DOPENAL_SUBDIR_AL"
				HAVE_OPENAL="yes"
			elif test "x${ac_cv_header_al_h}" = "xyes" \
				 -a "x${ac_cv_header_alc_h}" = "xyes";
			then #Windows?
				output_modules="$output_modules openal"
				OPENAL_LIBS="-lopenal"
				OPENAL_CFLAGS=""
				HAVE_OPENAL="yes"
			else
				check_failed=yes
			fi
		;;
		os2)
			OS2_LIBS="-los2me -lmmpm2 -lsocket"
			AC_CHECK_HEADERS([os2.h])
			# os2me.h depends on os2.h
			# Yes, that way of coding it is ugly.
			if test "x${ac_cv_header_os2_h}" = xyes
			then
				# We mimick exactly the way how the header will be used.
				# It seems to be picky...
				AC_CHECK_HEADERS([os2me.h], [], [], [#define INCL_OS2MM
#define INCL_DOS
#define INCL_VIO
#define INCL_KBD
#include <os2.h>
#])
			fi
			if test "x${ac_cv_header_os2_h}" = xyes \
			   -a   "x${ac_cv_header_os2me_h}" = xyes
			then
				output_modules="$output_modules os2"
				HAVE_OS2=yes
			else
				check_failed=yes
			fi
		;;
# from here on only forced tests, untested code
		hp)
			# What's the deal with that and alib?
			UNSUPPORTED_AUDIO=yes
			AC_CHECK_HEADER([sys/audio.h], [output_modules="$output_modules hp" HAVE_HP=yes], [check_failed=yes])
		;;
		alib)
			UNSUPPORTED_AUDIO=yes
			# ALIB_CFLAGS="-I/opt/audio/include"
			ALIB_LIBS=-lAlib
			# These headers may not be all about audio but they are used.
			AC_CHECK_HEADERS([ Alib.h CUlib.h netdb.h netinet/in.h netinet/tcp.h])
			if test "x${ac_cv_header_Alib_h}" = xyes \
			   -a   "x${ac_cv_header_CUlib_h}" = xyes \
			   -a   "x${ac_cv_header_netdb_h}" = xyes \
			   -a   "x${ac_cv_header_netinet_in_h}" = xyes \
			   -a   "x${ac_cv_header_netinet_tcp_h}" = xyes
			then
				output_modules="$output_modules alib"
				HAVE_ALIB=yes
			else
				check_failed=yes
			fi
		;;
		mint)
			UNSUPPORTED_AUDIO=yes
			AC_CHECK_HEADERS([audios.h], [output_modules="$output_modules mint" HAVE_MINT=yes], [check_failes=yes])
		;;
		aix)
			UNSUPPORTED_AUDIO=yes
			AC_CHECK_HEADERS([sys/audio.h], [output_modules="$output_modules aix" HAVE_AIX=yes], [check_failed=yes])
		;;
		sgi)
			UNSUPPORTED_AUDIO=yes
			SGI_LIBS=-laudio
			AC_CHECK_HEADER([dmedia/audio.h], [output_modules="$output_modules sgi" HAVE_SGI=yes], [check_failed=yes])
		;;
		*)
			AC_MSG_ERROR([Unsupported/-known output '$m' demanded!])
		;;
	esac
done

if test "x$check_forced" = xyes -a "x$UNSUPPORTED_AUDIO" = xyes; then
	AC_MSG_WARN([You requested bulding of an unsupported audio module. Be prepared for happy hacking and please tell us about your experience!])
fi

if test "x$check_forced" = xyes -a "x$check_failed" = "xyes"; then
	AC_MSG_ERROR([One/some of your requested audio modules failed the test!])
fi

# When you extend check_modules, you should extend this:
#for i in alsa qsa oss coreaudio sndio sun win32 win32_wasapi esd jack portaudio pulse sdl nas aix alib arts hp os2 sgi mint openal dummy
#do echo $i; done |
#perl -ne 'chomp; $big = uc($_); print <<EOT;
#AC_SUBST(${big}_LIBS)
#AC_SUBST(${big}_LDFLAGS)
#AC_SUBST(${big}_CFLAGS)
#AM_CONDITIONAL( [HAVE_$big], [test "x\$HAVE_$big" = xyes] )
#EOT
#'
AC_SUBST(TINYALSA_LIBS)
AC_SUBST(TINYALSA_LDFLAGS)
AC_SUBST(TINYALSA_CFLAGS)
AM_CONDITIONAL( [HAVE_TINYALSA], [test "x$HAVE_TINYALSA" = xyes] )
AC_SUBST(ALSA_LIBS)
AC_SUBST(ALSA_LDFLAGS)
AC_SUBST(ALSA_CFLAGS)
AM_CONDITIONAL( [HAVE_ALSA], [test "x$HAVE_ALSA" = xyes] )
AC_SUBST(QSA_LIBS)
AC_SUBST(QSA_LDFLAGS)
AC_SUBST(QSA_CFLAGS)
AM_CONDITIONAL( [HAVE_QSA], [test "x$HAVE_QSA" = xyes] )
AC_SUBST(OSS_LIBS)
AC_SUBST(OSS_LDFLAGS)
AC_SUBST(OSS_CFLAGS)
AM_CONDITIONAL( [HAVE_OSS], [test "x$HAVE_OSS" = xyes] )
AC_SUBST(COREAUDIO_LIBS)
AC_SUBST(COREAUDIO_LDFLAGS)
AC_SUBST(COREAUDIO_CFLAGS)
AM_CONDITIONAL( [HAVE_COREAUDIO], [test "x$HAVE_COREAUDIO" = xyes] )
AC_SUBST(SNDIO_LIBS)
AC_SUBST(SNDIO_LDFLAGS)
AC_SUBST(SNDIO_CFLAGS)
AM_CONDITIONAL( [HAVE_SNDIO], [test "x$HAVE_SNDIO" = xyes] )
AC_SUBST(SUN_LIBS)
AC_SUBST(SUN_LDFLAGS)
AC_SUBST(SUN_CFLAGS)
AM_CONDITIONAL( [HAVE_SUN], [test "x$HAVE_SUN" = xyes] )
AC_SUBST(WIN32_LIBS)
AC_SUBST(WIN32_LDFLAGS)
AC_SUBST(WIN32_CFLAGS)
AM_CONDITIONAL( [HAVE_WIN32], [test "x$HAVE_WIN32" = xyes] )
AC_SUBST(WIN32_WASAPI_LIBS)
AC_SUBST(WIN32_WASAPI_LDFLAGS)
AC_SUBST(WIN32_WASAPI_CFLAGS)
AM_CONDITIONAL( [HAVE_WIN32_WASAPI], [test "x$HAVE_WIN32_WASAPI" = xyes] )
AC_SUBST(ESD_LIBS)
AC_SUBST(ESD_LDFLAGS)
AC_SUBST(ESD_CFLAGS)
AM_CONDITIONAL( [HAVE_ESD], [test "x$HAVE_ESD" = xyes] )
AC_SUBST(JACK_LIBS)
AC_SUBST(JACK_LDFLAGS)
AC_SUBST(JACK_CFLAGS)
AM_CONDITIONAL( [HAVE_JACK], [test "x$HAVE_JACK" = xyes] )
AC_SUBST(PORTAUDIO_LIBS)
AC_SUBST(PORTAUDIO_LDFLAGS)
AC_SUBST(PORTAUDIO_CFLAGS)
AM_CONDITIONAL( [HAVE_PORTAUDIO], [test "x$HAVE_PORTAUDIO" = xyes] )
AC_SUBST(PULSE_LIBS)
AC_SUBST(PULSE_LDFLAGS)
AC_SUBST(PULSE_CFLAGS)
AM_CONDITIONAL( [HAVE_PULSE], [test "x$HAVE_PULSE" = xyes] )
AC_SUBST(SDL_LIBS)
AC_SUBST(SDL_LDFLAGS)
AC_SUBST(SDL_CFLAGS)
AM_CONDITIONAL( [HAVE_SDL], [test "x$HAVE_SDL" = xyes] )
AC_SUBST(NAS_LIBS)
AC_SUBST(NAS_LDFLAGS)
AC_SUBST(NAS_CFLAGS)
AM_CONDITIONAL( [HAVE_NAS], [test "x$HAVE_NAS" = xyes] )
AC_SUBST(AIX_LIBS)
AC_SUBST(AIX_LDFLAGS)
AC_SUBST(AIX_CFLAGS)
AM_CONDITIONAL( [HAVE_AIX], [test "x$HAVE_AIX" = xyes] )
AC_SUBST(ALIB_LIBS)
AC_SUBST(ALIB_LDFLAGS)
AC_SUBST(ALIB_CFLAGS)
AM_CONDITIONAL( [HAVE_ALIB], [test "x$HAVE_ALIB" = xyes] )
AC_SUBST(ARTS_LIBS)
AC_SUBST(ARTS_LDFLAGS)
AC_SUBST(ARTS_CFLAGS)
AM_CONDITIONAL( [HAVE_ARTS], [test "x$HAVE_ARTS" = xyes] )
AC_SUBST(HP_LIBS)
AC_SUBST(HP_LDFLAGS)
AC_SUBST(HP_CFLAGS)
AM_CONDITIONAL( [HAVE_HP], [test "x$HAVE_HP" = xyes] )
AC_SUBST(OS2_LIBS)
AC_SUBST(OS2_LDFLAGS)
AC_SUBST(OS2_CFLAGS)
AM_CONDITIONAL( [HAVE_OS2], [test "x$HAVE_OS2" = xyes] )
AC_SUBST(SGI_LIBS)
AC_SUBST(SGI_LDFLAGS)
AC_SUBST(SGI_CFLAGS)
AM_CONDITIONAL( [HAVE_SGI], [test "x$HAVE_SGI" = xyes] )
AC_SUBST(MINT_LIBS)
AC_SUBST(MINT_LDFLAGS)
AC_SUBST(MINT_CFLAGS)
AM_CONDITIONAL( [HAVE_MINT], [test "x$HAVE_MINT" = xyes] )
AC_SUBST(OPENAL_LIBS)
AC_SUBST(OPENAL_LDFLAGS)
AC_SUBST(OPENAL_CFLAGS)
AM_CONDITIONAL( [HAVE_OPENAL], [test "x$HAVE_OPENAL" = xyes] )
AC_SUBST(DUMMY_LIBS)
AC_SUBST(DUMMY_LDFLAGS)
AC_SUBST(DUMMY_CFLAGS)
AM_CONDITIONAL( [HAVE_DUMMY], [test "x$HAVE_DUMMY" = xyes] )
# Hackery to get rid of module .la files.
AC_SUBST(output_modules)
for f in $output_modules
do
  output_modules_la="$output_modules_la output_$f.la"
done
AC_SUBST(output_modules_la)

# Did user choose default audio subsystem ?
if test "x$with_default_audio" != "x"; then
	default_output_module=$with_default_audio
else
	default_output_module=`echo "$output_modules" | $AWK '{ print $1 }'`
fi

# That's (beginning of) the list for mpg123's internal default.
default_output_modules=$default_output_module

# Setup the static build.
# The conditionals always need to be defined by configure, even if
# HAVE_MODULES is FALSE!
# Here's a script for that tedious list, perhaps to be outsourced together with the one in #src/output/Makefile.am
#for i in dummy tinyalsa alsa qsa coreaudio esd jack nas oss portaudio pulse sdl sndio sun win32 win32_wasapi aix alib arts hp os2 sgi mint openal
#do echo $i; done |
#perl -ne 'chomp; $big = uc($_); print <<EOT;
#AM_CONDITIONAL([BUILD_${big}], [ test "$_" = \$default_output_module ])
#EOT
#'

AM_CONDITIONAL([BUILD_DUMMY], [ test "dummy" = $default_output_module ])
AM_CONDITIONAL([BUILD_TINYALSA], [ test "tinyalsa" = $default_output_module ])
AM_CONDITIONAL([BUILD_ALSA], [ test "alsa" = $default_output_module ])
AM_CONDITIONAL([BUILD_QSA], [ test "qsa" = $default_output_module ])
AM_CONDITIONAL([BUILD_COREAUDIO], [ test "coreaudio" = $default_output_module ])
AM_CONDITIONAL([BUILD_ESD], [ test "esd" = $default_output_module ])
AM_CONDITIONAL([BUILD_JACK], [ test "jack" = $default_output_module ])
AM_CONDITIONAL([BUILD_NAS], [ test "nas" = $default_output_module ])
AM_CONDITIONAL([BUILD_OSS], [ test "oss" = $default_output_module ])
AM_CONDITIONAL([BUILD_PORTAUDIO], [ test "portaudio" = $default_output_module ])
AM_CONDITIONAL([BUILD_PULSE], [ test "pulse" = $default_output_module ])
AM_CONDITIONAL([BUILD_SDL], [ test "sdl" = $default_output_module ])
AM_CONDITIONAL([BUILD_SNDIO], [ test "sndio" = $default_output_module ])
AM_CONDITIONAL([BUILD_SUN], [ test "sun" = $default_output_module ])
AM_CONDITIONAL([BUILD_WIN32], [ test "win32" = $default_output_module ])
AM_CONDITIONAL([BUILD_WIN32_WASAPI], [ test "win32_wasapi" = $default_output_module ])
AM_CONDITIONAL([BUILD_AIX], [ test "aix" = $default_output_module ])
AM_CONDITIONAL([BUILD_ALIB], [ test "alib" = $default_output_module ])
AM_CONDITIONAL([BUILD_ARTS], [ test "arts" = $default_output_module ])
AM_CONDITIONAL([BUILD_HP], [ test "hp" = $default_output_module ])
AM_CONDITIONAL([BUILD_OS2], [ test "os2" = $default_output_module ])
AM_CONDITIONAL([BUILD_SGI], [ test "sgi" = $default_output_module ])
AM_CONDITIONAL([BUILD_MINT], [ test "mint" = $default_output_module ])
AM_CONDITIONAL([BUILD_OPENAL], [ test "openal" = $default_output_module ])

if test "x$modules" = xenabled
then

  # Now make a comma-separated list again... eliminating the possible duplicate and dummy.
  for i in $output_modules
  do
    if test $i != $default_output_module && test $i != dummy; then
      default_output_modules=$default_output_modules,$i
    fi
  done
fi

AC_DEFINE_UNQUOTED( DEFAULT_OUTPUT_MODULE, "$default_output_modules", [The default audio output module(s) to use] )

dnl ############## Compiler Optimizations

CFLAGS="$ADD_CFLAGS $CFLAGS"
AM_CONDITIONAL([BUILD_BUFFER], [ test x"$buffer" = xenabled ])
if test x"$buffer" = xdisabled; then
	ADD_CPPFLAGS="$ADD_CPPFLAGS -DNOXFERMEM"
fi
if test x"$newoldwritesample" = xenabled; then
	ADD_CPPFLAGS="$ADD_CPPFLAGS -DNEWOLD_WRITE_SAMPLE"
fi
CPPFLAGS="$ADD_CPPFLAGS $CPPFLAGS"
LDFLAGS="$ADD_LDFLAGS $LDFLAGS"

# None chosen?
if test "x$with_optimization" = "x"; then
	if test x"$debugging" = xenabled; then
		with_optimization="0"
	else
		# enable (gcc specific) default opts only with gcc
		if test "x$GCC" = xyes; then
			with_optimization="2"
		else
			with_optimization="0"
		fi
	fi
fi

case $with_optimization in
  0)
    # No Optimizations
    CFLAGS="$CFLAGS"
  ;;
  1)
    CFLAGS="-O $CFLAGS"
  ;;
  2)
    CFLAGS="-O2 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math $CFLAGS"
  ;;
  3)
    CFLAGS="-O3 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math $CFLAGS"
  ;;
  4)
    CFLAGS="-O4 -fomit-frame-pointer -funroll-all-loops -finline-functions -ffast-math $CFLAGS"
  ;;
  *)
  	AC_MSG_ERROR([Unknown optimizations level '$with_optimization'])
  ;;
esac

dnl If using gcc, prevent a bad alignment option from breaking things.
dnl Let's default to safe 16-byte alignment for any special x86 or altivec stuff.
if test "x$GCC" = xyes && echo "$CFLAGS" | $GREP 'mpreferred-stack-boundary=' > /dev/null; then
	case $cpu_type in
		x86 | i586* | mmx | sse | 3dnow* | altivec)
			AC_MSG_WARN( [ You are trying to mess with stack alignment. I know better. ] )
			CFLAGS="$CFLAGS -mpreferred-stack-boundary=4"
		;;
	esac
fi

dnl ############## Seektable size

if test "x$with_seektable" = "x"; then
	seektable=1000
else
	seektable=$with_seektable
fi

if test "$seektable" -gt 0; then
	AC_DEFINE(FRAME_INDEX, 1, [ Define if frame index should be used. ])
fi
# Define that always... also without frame index enabled.
AC_DEFINE_UNQUOTED(INDEX_SIZE, $seektable, [size of the frame index seek table])

dnl ############## Win32 function checks
# Check if we want Unicode for Win32. Cygwin does not need _wopen
if test x$ac_cv_header_windows_h = xyes && test "x$host_os" != "xcygwin"; then
  win32_specific_codes=enabled
else
  win32_specific_codes=disabled
fi
win32_unicode=unneeded
win32_sockets=disabled
win32_sockets_working=no
win32_wide_working=no
win32_winver_bump=no

dnl We do not support non-unicode Windows.
if test "x$win32_specific_codes" = xenabled; then
#### Check for Wide functions
  AC_CHECK_FUNC([_wopen], [win32_unicode=enabled],[win32_unicode=disabled])
  AC_MSG_CHECKING([if we want Unicode File Open for Win32])
  if test "x$win32_unicode" = xenabled; then
    dnl We need to include the header for PathCombineW checking as
    dnl the actual symbol has a very funny name.
    oldlibs=$LIBS
    LIBS="$LIBS -lshlwapi"
    AC_MSG_RESULT([yes])
    AC_MSG_CHECKING([if Unicode functions working])
    AC_LINK_IFELSE([AC_LANG_SOURCE([
      #include <windows.h>
      #include <shlwapi.h>
      #include <stdlib.h>
      int main()
      {
        MultiByteToWideChar (0, 0, NULL, 0, NULL, 0);
        WideCharToMultiByte (0, 0, NULL, 0, NULL, 0, NULL, NULL);
        PathCombineW(0,0,0);
        return 0;
      }
    ])], [win32_wide_working=yes], [win32_winver_bump=yes])
    if test "x$win32_wide_working" = xno; then
      AC_LINK_IFELSE([AC_LANG_SOURCE([
        #define WINVER 0x501
        #define _WIN32_WINNT 0x501
        #include <windows.h>
        #include <shlwapi.h>
        #include <stdlib.h>
        int main()
        {
          MultiByteToWideChar (0, 0, NULL, 0, NULL, 0);
          WideCharToMultiByte (0, 0, NULL, 0, NULL, 0, NULL, NULL);
          PathCombineW(0,0,0);
          return 0;
        }
      ])], [win32_wide_working=yes], [AC_MSG_RESULT([no])])
    fi
    if test "x$win32_wide_working" = xyes; then
      AC_MSG_RESULT([yes])
      AC_DEFINE([WANT_WIN32_UNICODE], [1], [ Define to use Unicode for Windows ])
    else
      LIBS=$oldlibs
      AC_MSG_ERROR([Unicode support for Win32 not working])
    fi
  else
    AC_MSG_ERROR([Unicode File Open for Win32 not available])
  fi

#### Check for Network functions
  AC_CHECK_HEADERS([ws2tcpip.h], [win32_sockets=enabled], [AC_MSG_WARN([Please update your headers to support winsock 2.2.])])
  AC_MSG_CHECKING([if we want Win32 sockets])
  if test "x$win32_sockets" = "xenabled" && test "x$network" != "xdisabled"; then
  AC_MSG_RESULT([yes])
  AC_MSG_CHECKING([if winsock2 API is available])
    wsoldlibs="$LIBS"
    LIBS="$LIBS -lws2_32"
    AC_LINK_IFELSE([AC_LANG_SOURCE([
      #include <winsock2.h>
      #include <ws2tcpip.h>
      #include <stdlib.h>
      int main()
      {
        getaddrinfo(NULL, NULL, NULL, NULL);
        freeaddrinfo(NULL);
        return 0;
      }
    ])], [win32_sockets_working=yes], [win32_winver_bump=yes])
    if test "x$win32_sockets_working" = "xno"; then
      AC_MSG_CHECKING([deeper if winsock2 API is available])
      AC_LINK_IFELSE([AC_LANG_SOURCE([
        #define WINVER 0x501
        #define _WIN32_WINNT 0x501
        #include <winsock2.h>
        #include <ws2tcpip.h>
        #include <stdlib.h>
        int main()
        {
          getaddrinfo(NULL, NULL, NULL, NULL);
          freeaddrinfo(NULL);
          return 0;
        }
      ])], [win32_sockets_working=yes], [AC_MSG_RESULT([no])])
    fi
    if test "x$win32_sockets_working" = "xyes"; then
      AC_MSG_RESULT([yes])
      AC_DEFINE([WANT_WIN32_SOCKETS], [1], [ Define to use Win32 sockets ])
      network_type=Winsock2
      have_network=yes
      have_ipv6=yes
    else
      LIBS="$wsoldlibs"
      AC_MSG_WARN([Please update your headers to support winsock 2.2.])
    fi
  else
    AC_MSG_RESULT([no])
  fi

#### Check for Win32 Named Pipe functions
win32_fifo_working=no
if test x$win32_specific_codes = xenabled; then
  AC_MSG_CHECKING([if we have Named Pipes])
  if test "x$fifo" != "xdisabled"; then
    AC_LINK_IFELSE([AC_LANG_SOURCE([
    #include <windows.h>
      int main(){
        CreateNamedPipeA(NULL,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,PIPE_TYPE_BYTE,1,255,255,0,NULL);
        CreateNamedPipeW(NULL,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,PIPE_TYPE_BYTE,1,255,255,0,NULL);
        return 0;
      }])],[win32_fifo_working=yes],[win32_winver_bump=yes])
    if test "x$win32_fifo_working" != "xyes"; then
    AC_LINK_IFELSE([AC_LANG_SOURCE([
    #define WINVER 0x501
    #define _WIN32_WINNT 0x501
    #include <windows.h>
      int main(){
        CreateNamedPipeA(NULL,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,PIPE_TYPE_BYTE,1,255,255,0,NULL);
        CreateNamedPipeW(NULL,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,PIPE_TYPE_BYTE,1,255,255,0,NULL);
        return 0;
      }])],[win32_fifo_working=yes],[win32_fifo_working=no])
    fi
  fi
  have_mkfifo=$win32_fifo_working
  if test "x$win32_fifo_working" = "xyes"; then
    AC_MSG_RESULT([yes])
    AC_DEFINE([WANT_WIN32_FIFO], [1], [ Define to use Win32 named pipes ])
  else
    AC_MSG_RESULT([no])
  fi
fi

#### WINVER Bump
if test x$win32_specific_codes = xenabled; then
  AC_MSG_CHECKING([if WINVER and _WIN32_WINNT needs version bumps])
  if test "x$win32_winver_bump" = "xyes"; then
    AC_MSG_RESULT([yes])
    AC_DEFINE([WINVER], [0x0501], [ WinXP and above for ipv6 ])
    AC_DEFINE([_WIN32_WINNT], [0x0501], [ WinXP and above for ipv6 ])
    # getaddrinfo and freeaddrinfo are available for Win2K and above
    # Bug: MinGW.org w32api ws2tcpip.h incorrectly wants (_WIN32_WINNT >= 0x0501) for getaddrinfo and freeaddrinfo
    # MultiByteToWideChar and WideCharToMultiByte are available for Win2K and above
  else
    AC_MSG_RESULT([no])
  fi
fi

fi #END OF WIN32 CHECKS

#### Check mingw.org for EOVERFLOW
AC_MSG_CHECKING([if we have EOVERFLOW macro])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <errno.h>
int i = (EOVERFLOW) + 0;
])],[eoverflow_present=yes],[eoverflow_present=no])
AC_MSG_RESULT([$eoverflow_present])
AS_IF([test "x$eoverflow_present" = "xyes"],[],[AC_DEFINE([EOVERFLOW],[EFBIG],[Use EFBIG as substitude for EOVERFLOW, mingw.org may lack the latter])])

#### Use Win32 support codes
AM_CONDITIONAL([WIN32_CODES], [ test "x$win32_specific_codes" = xenabled ])

if test x"$network" = xauto; then
	if test x"$have_network" = xyes; then
		network=enabled
	else
		network=disabled
	fi
fi
if test x"$ipv6" = xauto; then
	if test x"$have_ipv6" = xyes; then
		ipv6=enabled
	else
		ipv6=disabled
	fi
fi

dnl ############## FIFO enable

if test x"$fifo" = xauto; then
	if test x"$have_mkfifo" = xyes; then
		fifo=enabled
	else
		fifo=disabled
	fi
fi

if test x"$fifo" = xenabled; then
	AC_DEFINE(FIFO, 1, [ Define if FIFO support is enabled. ])
	if test x"$have_mkfifo" = xno; then
		AC_MSG_WARN( [ You forced FIFO code while I think there is no mkfifo() available! ] )
	fi
fi

dnl ############## Network enable
if test x"$network" = xenabled; then
	AC_DEFINE(NETWORK, 1, [ Define if network support is enabled. ])
	if test x"$have_network" = xno; then
		AC_MSG_WARN( [ You forced network code while I think there is support missing! ] )
	fi
fi

if test x"$ipv6" = xenabled; then
	AC_DEFINE(IPV6, 1, [ Define if IPV6 support is enabled. ])
	if test x"$have_ipv6" = xno; then
		AC_MSG_WARN( [ You forced IPv6 code while I think there is no getaddrinfo() available! ] )
	fi
fi

dnl ############## Final Output

AC_CONFIG_FILES([
	Makefile
	libmpg123.pc
	libout123.pc
	mpg123.spec
	src/libmpg123/mpg123.h
	src/libout123/out123.h
])

AC_OUTPUT



dnl ############## Display Message

echo "
  $PACKAGE_NAME $PACKAGE_VERSION

  Install path ............ $prefix
  CPU Optimization ........ $cpu_type
  Compiler Optimization ... $with_optimization
  Gapless Support ......... $gapless
  Debugging ............... $debugging
  Seek table size ......... $seektable
  FIFO support ............ $fifo
  Buffer .................. $buffer
  Network (http streams) .. $network
  Network Sockets ......... $network_type
  IPv6 (getaddrinfo) ...... $ipv6"
if test x"$LARGEFILE_BITS" = x; then
  echo "  File offsets ............ default"
else
  echo "  File offsets ............ $LARGEFILE_BITS"
  echo "    The lib will (try to) support default offset size, too."
fi
echo "  LFS alias symbols ....... $lfs_alias ($LFS_ALIAS_BITS)"
echo "  LFS alias type .......... $lfs_alias_type"
if test x"$use_yasm_for_avx" = xyes; then
  echo "  Use yasm (for AVX only) . enabled"
else
  echo "  Use yasm (for AVX only) . disabled"
fi

echo "
  Core libmpg123 features:
  Layer I ................. $layer1
  Layer II ................ $layer2
  Layer III ............... $layer3
  NtoM resampling ......... $ntom
  downsampled decoding .... $downsample
  Feeder/buffered input ... $feeder
  ID3v2 parsing ........... $id3v2
  String API .............. $string
  ICY parsing/conversion .. $icy
  Error/warning messages .. $messages
  Win32 Unicode File Open.. $win32_unicode
  Feature Report Function.. $feature_report
  Output formats (nofpu will disable all but 16 or 8 bit!):
  8 bit integer ........... $int8
  16 bit integer .......... $int16
  32/24 bit integer ....... $int32
  real (32 bit float) ..... $real
  Equalizer ............... $equalizer
  Optimization detail:
  Integer conversion ...... $integers
  IEEE 754 hackery ........ $ieee
  New/old WRITE_SAMPLE .... $newoldwritesample
  new Huffman scheme ...... $newhuff

Note: Disabling core features is not commonly done and some combinations might not build/work. If you encounter such a case, help yourself (and provide a patch) or just poke the maintainers."
# just an empty line
echo

echo "  Modules ................. $modules"
echo "  Checked audio modules ... $check_modules
  Detected audio support ..$output_modules
  Default output module ... $default_output_module
"
if test x"$modules" = xdisabled; then
echo "The _single_ active output module is being statically linked in.
"
fi
if test x"$with_optimization" = x0; then
	echo "No optimization flags chosen, make sure you have something basic in your CFLAGS at least...
"
fi

if test x"$cpu_type" = xi486; then
  echo "WARNING: You selected the i486 decoder. This is not recommened for regular use."
  echo "It was designed for actual i486 CPUs a long time ago. Generic C code is likely"
  echo "to perform better nowadays. Also, the decoder misses features like volume"
  echo "scaling and has clipping issues. Continue if you are into software archeology"
  echo "only."
fi

echo
echo "  CPPFLAGS='$CPPFLAGS'"
echo "  CFLAGS='$CFLAGS'"
echo "  LIBS='$LIBS'"
echo
echo "Next type 'make' and then 'make install'."
