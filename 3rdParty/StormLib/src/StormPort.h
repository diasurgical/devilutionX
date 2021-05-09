/*****************************************************************************/
/* StormPort.h                           Copyright (c) Marko Friedemann 2001 */
/*---------------------------------------------------------------------------*/
/* Portability module for the StormLib library. Contains a wrapper symbols   */
/* to make the compilation under Linux work                                  */
/*                                                                           */
/* Author: Marko Friedemann <marko.friedemann@bmx-chemnitz.de>               */
/* Created at: Mon Jan 29 18:26:01 CEST 2001                                 */
/* Computer: whiplash.flachland-chemnitz.de                                  */
/* System: Linux 2.4.0 on i686                                               */
/*                                                                           */
/* Author: Sam Wilkins <swilkins1337@gmail.com>                              */
/* System: Mac OS X and port to big endian processor                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 29.01.01  1.00  Mar  Created                                              */
/* 24.03.03  1.01  Lad  Some cosmetic changes                                */
/* 12.11.03  1.02  Dan  Macintosh compatibility                              */
/* 24.07.04  1.03  Sam  Mac OS X compatibility                               */
/* 22.11.06  1.04  Sam  Mac OS X compatibility (for StormLib 6.0)            */
/* 31.12.06  1.05  XPinguin  Full GNU/Linux compatibility		             */
/* 17.10.12  1.05  Lad  Moved error codes so they don't overlap with errno.h */
/*****************************************************************************/

#ifndef __STORMPORT_H__
#define __STORMPORT_H__

#ifndef __cplusplus
  #define bool char
  #define true 1
  #define false 0
#endif

//-----------------------------------------------------------------------------
// Defines for Windows

#if !defined(STORMLIB_PLATFORM_DEFINED) && defined(_WIN32)

  // In MSVC 8.0, there are some functions declared as deprecated.
  #if _MSC_VER >= 1400
  #define _CRT_SECURE_NO_DEPRECATE
  #define _CRT_NON_CONFORMING_SWPRINTFS
  #endif

  #include <tchar.h>
  #include <assert.h>
  #include <ctype.h>
  #include <stdio.h>

  // Suppress definitions of `min` and `max` macros by <windows.h>:
  #define NOMINMAX 1
  #include <windows.h>

  #include <wininet.h>
  #define STORMLIB_LITTLE_ENDIAN

  #ifdef _WIN64
    #define STORMLIB_64BIT
  #else
    #define STORMLIB_32BIT
  #endif

  #define STORMLIB_CDECL __cdecl

  #define STORMLIB_WINDOWS
  #define STORMLIB_PLATFORM_DEFINED                 // The platform is known now

#endif

//-----------------------------------------------------------------------------
// Defines for Mac

#if !defined(STORMLIB_PLATFORM_DEFINED) && defined(__APPLE__)  // Mac BSD API

  // Macintosh
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <sys/mman.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include <ctype.h>
  #include <assert.h>
  #include <errno.h>

  // Support for PowerPC on Max OS X
  #if (__ppc__ == 1) || (__POWERPC__ == 1) || (_ARCH_PPC == 1)
    #include <stdint.h>
    #include <CoreFoundation/CFByteOrder.h>
  #endif

  #define    PKEXPORT
  #define    __SYS_ZLIB
  #define    __SYS_BZLIB

  #ifndef __BIG_ENDIAN__
    #define STORMLIB_LITTLE_ENDIAN
  #endif

  #define STORMLIB_MAC
  #define STORMLIB_HAS_MMAP                         // Indicate that we have mmap support
  #define STORMLIB_PLATFORM_DEFINED                 // The platform is known now

#endif

//-----------------------------------------------------------------------------
// Defines for HAIKU platform

#if !defined(STORMLIB_PLATFORM_DEFINED) && defined(__HAIKU__)

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <sys/mman.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  #include <ctype.h>
  #include <assert.h>
  #include <errno.h>

  #ifndef __BIG_ENDIAN__
    #define STORMLIB_LITTLE_ENDIAN
  #endif

  #define STORMLIB_MAC                              // Use Mac compatible code
  #define STORMLIB_HAIKU
  #define STORMLIB_PLATFORM_DEFINED                 // The platform is known now

#endif

//-----------------------------------------------------------------------------
// Defines for AMIGA platform

#if !defined(STORMLIB_PLATFORM_DEFINED) && defined(__AMIGA__)

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  #include <ctype.h>
  #include <assert.h>
  #include <errno.h>

  #ifndef __BIG_ENDIAN__
    #define STORMLIB_LITTLE_ENDIAN
  #endif

  #define STORMLIB_MAC                              // Use Mac compatible code
  #define STORMLIB_AMIGA
  #define STORMLIB_PLATFORM_DEFINED

#endif

//-----------------------------------------------------------------------------
// Defines for Switch platform

#if !defined(STORMLIB_PLATFORM_DEFINED) && defined(__SWITCH__)

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  #include <strings.h>
  #include <ctype.h>
  #include <assert.h>
  #include <errno.h>

  #ifndef __BIG_ENDIAN__
    #define STORMLIB_LITTLE_ENDIAN
  #endif

  #define STORMLIB_MAC                              // Use Mac compatible code
  #define STORMLIB_SWITCH
  #define STORMLIB_PLATFORM_DEFINED

#endif

//-----------------------------------------------------------------------------
// Defines for 3DS platform

#if !defined(STORMLIB_PLATFORM_DEFINED) && defined(__3DS__)

  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  #include <strings.h>
  #include <ctype.h>
  #include <assert.h>
  #include <errno.h>

  #define STORMLIB_LITTLE_ENDIAN

  #define STORMLIB_MAC                              // Use Mac compatible code
  #define STORMLIB_CTR
  #define STORMLIB_PLATFORM_DEFINED

#endif

//-----------------------------------------------------------------------------
// Defines for PSP platform

#if !defined(STORMLIB_PLATFORM_DEFINED) && defined(__PSP__)

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  #include <strings.h>
  #include <ctype.h>
  #include <assert.h>
  #include <errno.h>

  #define STORMLIB_LITTLE_ENDIAN

  #define STORMLIB_PLATFORM_DEFINED

#endif

//-----------------------------------------------------------------------------
// Defines for Vita platform

#if !defined(STORMLIB_PLATFORM_DEFINED) && defined(__vita__)

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  #include <strings.h>
  #include <ctype.h>
  #include <assert.h>
  #include <errno.h>

  #ifndef __BIG_ENDIAN__
    #define STORMLIB_LITTLE_ENDIAN
  #endif

  #define STORMLIB_MAC                              // Use Mac compatible code
  #define STORMLIB_VITA
  #define STORMLIB_PLATFORM_DEFINED

#endif

//-----------------------------------------------------------------------------
// Assumption: If the platform is not defined, assume a Linux-like platform

#if !defined(STORMLIB_PLATFORM_DEFINED)

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <stdint.h>
  #include <stdlib.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  #include <strings.h>
  #include <ctype.h>
  #include <assert.h>
  #include <errno.h>

  #ifndef __BIG_ENDIAN__
    #define STORMLIB_LITTLE_ENDIAN
  #endif

  #define STORMLIB_LINUX

  // Platforms with mmap support
  #if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    #include <sys/mman.h>
    #define STORMLIB_HAS_MMAP
  #endif

  #define STORMLIB_PLATFORM_DEFINED

#endif

//-----------------------------------------------------------------------------
// Definition of Windows-specific types for non-Windows platforms

#ifndef STORMLIB_WINDOWS
  #if __LP64__
    #define STORMLIB_64BIT
  #else
    #define STORMLIB_32BIT
  #endif

  // __cdecl meand nothing on non-Windows
  #define STORMLIB_CDECL /* */

  // Typedefs for ANSI C
  typedef unsigned char  BYTE;
  typedef unsigned short USHORT;
  typedef int            LONG;
  typedef unsigned int   DWORD;
  typedef unsigned long  DWORD_PTR;
  typedef long           LONG_PTR;
  typedef long           INT_PTR;
  typedef long long      LONGLONG;
  typedef unsigned long long ULONGLONG;
  typedef void         * HANDLE;
  typedef void         * LPOVERLAPPED; // Unsupported on Linux and Mac
  typedef char           TCHAR;
  typedef unsigned int   LCID;
  typedef LONG         * PLONG;
  typedef DWORD        * LPDWORD;
  typedef BYTE         * LPBYTE;
  typedef const char   * LPCTSTR;
  typedef const char   * LPCSTR;
  typedef char         * LPTSTR;
  typedef char         * LPSTR;

  #ifdef STORMLIB_32BIT
    #define _LZMA_UINT32_IS_ULONG
  #endif

  // Some Windows-specific defines
  #ifndef MAX_PATH
    #define MAX_PATH 1024
  #endif

  #define WINAPI

  #define FILE_BEGIN    SEEK_SET
  #define FILE_CURRENT  SEEK_CUR
  #define FILE_END      SEEK_END

  #define _T(x)     x
  #define _tcslen   strlen
  #define _tcscpy   strcpy
  #define _tcscat   strcat
  #define _tcschr   strchr
  #define _tcsrchr  strrchr
  #define _tcsstr   strstr
  #define _tcsnicmp strncasecmp
  #define _tprintf  printf
  #define _stprintf sprintf
  #define _tremove  remove
  #define _tmain    main

  #define _stricmp  strcasecmp
  #define _strnicmp strncasecmp
  #define _tcsicmp  strcasecmp
  #define _tcsnicmp strncasecmp

#endif // !STORMLIB_WINDOWS

// 64-bit calls are supplied by "normal" calls on Mac
#if defined(STORMLIB_MAC)
  #define stat64  stat
  #define fstat64 fstat
  #define lseek64 lseek
  #define ftruncate64 ftruncate
  #define off64_t off_t
  #define O_LARGEFILE 0
#endif

// Platform-specific error codes for UNIX-based platforms
#if defined(STORMLIB_MAC) || defined(STORMLIB_LINUX) || defined(__PSP__)
  #define ERROR_SUCCESS                  0
  #define ERROR_FILE_NOT_FOUND           ENOENT
  #define ERROR_ACCESS_DENIED            EPERM
  #define ERROR_INVALID_HANDLE           EBADF
  #define ERROR_NOT_ENOUGH_MEMORY        ENOMEM
  #define ERROR_NOT_SUPPORTED            ENOTSUP
  #define ERROR_INVALID_PARAMETER        EINVAL
  #define ERROR_NEGATIVE_SEEK            ESPIPE
  #define ERROR_DISK_FULL                ENOSPC
  #define ERROR_ALREADY_EXISTS           EEXIST
  #define ERROR_INSUFFICIENT_BUFFER      ENOBUFS
  #define ERROR_BAD_FORMAT               1000        // No such error code under Linux
  #define ERROR_NO_MORE_FILES            1001        // No such error code under Linux
  #define ERROR_HANDLE_EOF               1002        // No such error code under Linux
  #define ERROR_CAN_NOT_COMPLETE         1003        // No such error code under Linux
  #define ERROR_FILE_CORRUPT             1004        // No such error code under Linux
#endif

#ifndef _countof
  #define _countof(x)  (sizeof(x) / sizeof(x[0]))
#endif

//-----------------------------------------------------------------------------
// Swapping functions

#ifdef STORMLIB_LITTLE_ENDIAN
    #define    BSWAP_INT16_UNSIGNED(a)          (a)
    #define    BSWAP_INT16_SIGNED(a)            (a)
    #define    BSWAP_INT32_UNSIGNED(a)          (a)
    #define    BSWAP_INT32_SIGNED(a)            (a)
    #define    BSWAP_INT64_SIGNED(a)            (a)
    #define    BSWAP_INT64_UNSIGNED(a)          (a)
    #define    BSWAP_ARRAY16_UNSIGNED(a,b)      {}
    #define    BSWAP_ARRAY32_UNSIGNED(a,b)      {}
    #define    BSWAP_ARRAY64_UNSIGNED(a,b)      {}
    #define    BSWAP_PART_HEADER(a)             {}
    #define    BSWAP_TMPQHEADER(a,b)            {}
    #define    BSWAP_TMPKHEADER(a)              {}
#else

#ifdef __cplusplus
  extern "C" {
#endif
    int16_t  SwapInt16(uint16_t);
    uint16_t SwapUInt16(uint16_t);
    int32_t  SwapInt32(uint32_t);
    uint32_t SwapUInt32(uint32_t);
    int64_t  SwapInt64(uint64_t);
    uint64_t SwapUInt64(uint64_t);
    void ConvertUInt16Buffer(void * ptr, size_t length);
    void ConvertUInt32Buffer(void * ptr, size_t length);
    void ConvertUInt64Buffer(void * ptr, size_t length);
    void ConvertTMPQUserData(void *userData);
    void ConvertTMPQHeader(void *header, uint16_t wPart);
    void ConvertTMPKHeader(void *header);
#ifdef __cplusplus
  }
#endif
    #define    BSWAP_INT16_SIGNED(a)            SwapInt16((a))
    #define    BSWAP_INT16_UNSIGNED(a)          SwapUInt16((a))
    #define    BSWAP_INT32_SIGNED(a)            SwapInt32((a))
    #define    BSWAP_INT32_UNSIGNED(a)          SwapUInt32((a))
    #define    BSWAP_INT64_SIGNED(a)            SwapInt64((a))
    #define    BSWAP_INT64_UNSIGNED(a)          SwapUInt64((a))
    #define    BSWAP_ARRAY16_UNSIGNED(a,b)      ConvertUInt16Buffer((a),(b))
    #define    BSWAP_ARRAY32_UNSIGNED(a,b)      ConvertUInt32Buffer((a),(b))
    #define    BSWAP_ARRAY64_UNSIGNED(a,b)      ConvertUInt64Buffer((a),(b))
    #define    BSWAP_TMPQHEADER(a,b)            ConvertTMPQHeader((a),(b))
    #define    BSWAP_TMPKHEADER(a)              ConvertTMPKHeader((a))
#endif

//-----------------------------------------------------------------------------
// Macro for deprecated symbols

/*
#ifdef _MSC_VER
  #if _MSC_FULL_VER >= 140050320
    #define STORMLIB_DEPRECATED(_Text) __declspec(deprecated(_Text))
  #else
    #define STORMLIB_DEPRECATED(_Text) __declspec(deprecated)
  #endif
#else
  #ifdef __GNUC__
    #define STORMLIB_DEPRECATED(_Text) __attribute__((deprecated))
  #else
    #define STORMLIB_DEPRECATED(_Text) __attribute__((deprecated(_Text)))
  #endif
#endif

// When a flag is deprecated, use this macro
#ifndef _STORMLIB_NO_DEPRECATE
  #define STORMLIB_DEPRECATED_FLAG(type, oldflag, newflag)    \
    const STORMLIB_DEPRECATED(#oldflag " is deprecated. Use " #newflag ". To supress this warning, define _STORMLIB_NO_DEPRECATE") static type oldflag = (type)newflag;
#else
#define STORMLIB_DEPRECATED_FLAG(type, oldflag, newflag) static type oldflag = (type)newflag;
#endif
*/

#endif // __STORMPORT_H__
