/*
	mpg123_msvc: MPEG Audio Decoder library wrapper header for MS VC++ 2005

	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	initially written by Patrick Dehne and Thomas Orgis.
*/
#ifndef MPG123_MSVC_H
#define MPG123_MSVC_H

#include <tchar.h>
#include <stdlib.h>
#include <sys/types.h>

// Needed for Visual Studio versions before VS 2010.
#if (_MSC_VER < 1600)
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
#define PRIiMAX "I64i"
typedef __int64 intmax_t;
#else
#include <stdint.h>
#include <inttypes.h>
#endif

// ftell returns long, _ftelli64 returns __int64
// off_t is long, not __int64, use ftell
#define ftello ftell

#define MPG123_NO_CONFIGURE
#include "mpg123.h.in" /* Yes, .h.in; we include the configure template! */

#ifdef __cplusplus
extern "C" {
#endif

	// Wrapper around mpg123_open that supports path names with unicode
	// characters
	MPG123_EXPORT int mpg123_topen(mpg123_handle *fr, const _TCHAR *path);
	MPG123_EXPORT int mpg123_tclose(mpg123_handle *fr);

#ifdef __cplusplus
}
#endif

#endif
