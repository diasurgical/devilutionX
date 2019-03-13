/*
	mpg123.h: MPEG Audio Decoder library wrapper header for Xcode

	copyright 2012 by the mpg123 project - free software under the terms of the LGPL 2.1
	initially written by Patrick Dehne.
*/

#ifndef MPG123_XCODE_H
#define MPG123_XCODE_H

#include <stdlib.h>
#include <sys/types.h>

#define MPG123_NO_CONFIGURE
#include "../../src/libmpg123/mpg123.h.in" /* Yes, .h.in; we include the configure template! */

#endif
