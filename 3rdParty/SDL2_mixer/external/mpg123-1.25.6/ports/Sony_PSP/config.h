/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if your architecture wants/needs/can use attribute_align_arg and
   alignment checks. It's for 32bit x86... */
#define ABI_ALIGN_FUN 1

/* Define to use proper rounding. */
/* #undef ACCURATE_ROUNDING */

/* Define if .align takes 3 for alignment of 2^3=8 bytes instead of 8. */
/* #undef ASMALIGN_EXP */

/* Define if __attribute__((aligned(16))) shall be used */
/* #undef CCALIGN */

/* Define if checking of stack alignment is wanted. */
#define CHECK_ALIGN 1

/* Define if debugging is enabled. */
/* #undef DEBUG */

/* The default audio output module(s) to use */
#define DEFAULT_OUTPUT_MODULE "alsa,oss,esd,sdl,nas,arts"

/* Define if building with dynamcally linked libmpg123 */
#define DYNAMIC_BUILD 1

/* Define if FIFO support is enabled. */
#define FIFO 1

/* Define if frame index should be used. */
#define FRAME_INDEX 1

/* Define if gapless is enabled. */
#define GAPLESS 1

/* Define to 1 if you have the <Alib.h> header file. */
/* #undef HAVE_ALIB_H */

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <asm/audioio.h> header file. */
/* #undef HAVE_ASM_AUDIOIO_H */

/* Define to 1 if you have the `atoll' function. */
#define HAVE_ATOLL 1

/* Define to 1 if you have the <audios.h> header file. */
/* #undef HAVE_AUDIOS_H */

/* Define to 1 if you have the <AudioToolbox/AudioToolbox.h> header file. */
/* #undef HAVE_AUDIOTOOLBOX_AUDIOTOOLBOX_H */

/* Define to 1 if you have the <AudioUnit/AudioUnit.h> header file. */
/* #undef HAVE_AUDIOUNIT_AUDIOUNIT_H */

/* Define to 1 if you have the <CoreServices/CoreServices.h> header file. */
/* #undef HAVE_CORESERVICES_CORESERVICES_H */

/* Define to 1 if you have the <CUlib.h> header file. */
/* #undef HAVE_CULIB_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the `getuid' function. */
#define HAVE_GETUID 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <langinfo.h> header file. */
#define HAVE_LANGINFO_H 1

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define to 1 if you have the `mx' library (-lmx). */
/* #undef HAVE_LIBMX */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <linux/soundcard.h> header file. */
#define HAVE_LINUX_SOUNDCARD_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define if libltdl is available */
#define HAVE_LTDL 1

/* Define to 1 if you have the <machine/soundcard.h> header file. */
/* #undef HAVE_MACHINE_SOUNDCARD_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkfifo' function. */
#define HAVE_MKFIFO 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <netinet/tcp.h> header file. */
/* #undef HAVE_NETINET_TCP_H */

/* Define to 1 if you have the `nl_langinfo' function. */
#define HAVE_NL_LANGINFO 1

/* Define to 1 if you have the <os2me.h> header file. */
/* #undef HAVE_OS2ME_H */

/* Define to 1 if you have the <os2.h> header file. */
/* #undef HAVE_OS2_H */

/* Define to 1 if you have the `random' function. */
#define HAVE_RANDOM 1

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define to 1 if you have the `sched_setscheduler' function. */
#define HAVE_SCHED_SETSCHEDULER 1

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the `setpriority' function. */
#define HAVE_SETPRIORITY 1

/* Define to 1 if you have the `setuid' function. */
#define HAVE_SETUID 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define to 1 if you have the <sndio.h> header file. */
/* #undef HAVE_SNDIO_H */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sun/audioio.h> header file. */
/* #undef HAVE_SUN_AUDIOIO_H */

/* Define to 1 if you have the <sys/audioio.h> header file. */
/* #undef HAVE_SYS_AUDIOIO_H */

/* Define to 1 if you have the <sys/audio.h> header file. */
/* #undef HAVE_SYS_AUDIO_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/signal.h> header file. */
#define HAVE_SYS_SIGNAL_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/soundcard.h> header file. */
#define HAVE_SYS_SOUNDCARD_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define this if you have the POSIX termios library */
#define HAVE_TERMIOS 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <windows.h> header file. */
/* #undef HAVE_WINDOWS_H */

/* Define to indicate that float storage follows IEEE754. */
#define IEEE_FLOAT 1

/* size of the frame index seek table */
#define INDEX_SIZE 1000

/* Define if IPV6 support is enabled. */
#define IPV6 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define if network support is enabled. */
/* #define NETWORK 1 */

/* Define to disable 16 bit integer output. */
/* #undef NO_16BIT */

/* Define to disable 32 bit integer output. */
/* define NO_32BIT 1 */

/* Define to disable 8 bit integer output. */
/* #undef NO_8BIT */

/* Define to disable downsampled decoding. */
/* #undef NO_DOWNSAMPLE */

/* Define to disable error messages in combination with a return value (the
   return is left intact). */
/* #undef NO_ERETURN */

/* Define to disable error messages. */
/* #undef NO_ERRORMSG */

/* Define to disable feeder and buffered readers. */
/* #undef NO_FEEDER */

/* Define to disable ICY handling. */
/* #undef NO_ICY */

/* Define to disable ID3v2 parsing. */
/* #undef NO_ID3V2 */

/* Define to disable layer I. */
/* #undef NO_LAYER1 */

/* Define to disable layer II. */
/* #undef NO_LAYER2 */

/* Define to disable layer III. */
/* #undef NO_LAYER3 */

/* Define to disable ntom resampling. */
/* #undef NO_NTOM */

/* Define to disable real output. */
/* define NO_REAL 1 */

/* Define to disable string functions. */
/* #undef NO_STRING */

/* Define to disable warning messages. */
/* #undef NO_WARNING */

/* Name of package */
#define PACKAGE "mpg123"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "mpg123-devel@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "mpg123"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "mpg123 1.8rc5"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "mpg123"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.8rc5"

/* Define if portaudio v18 API is wanted. */
/* #undef PORTAUDIO18 */

/* The size of `int32_t', as computed by sizeof. */
#define SIZEOF_INT32_T 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `off_t', as computed by sizeof. */
#define SIZEOF_OFF_T 4

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 4

/* The size of `ssize_t', as computed by sizeof. */
#define SIZEOF_SSIZE_T 4

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if modules are enabled */
#define USE_MODULES 1

/* Version number of package */
#define VERSION "1.8rc5"

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `short' if <sys/types.h> does not define. */
/* #undef int16_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef int32_t */

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned long' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef ssize_t */

/* Define to `unsigned short' if <sys/types.h> does not define. */
/* #undef uint16_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef uint32_t */
