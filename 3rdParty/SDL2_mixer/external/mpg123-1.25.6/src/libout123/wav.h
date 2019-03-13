/*
	wav.c: write wav/au/cdr files (and headerless raw)

	copyright ?-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially extracted of out123_int.h, formerly audio.h, by Thomas Orgis
*/

#ifndef _MPG123_WAV_H_
#define _MPG123_WAV_H_

/* Could get away without any header, as only pointers declared. */
#include "out123.h"

/* Interfaces from wav.c, variants of file writing, to be combined into
   fake modules by the main library code.  */

int au_open(out123_handle *);
int cdr_open(out123_handle *);
int raw_open(out123_handle *);
int wav_open(out123_handle *);
int wav_write(out123_handle *, unsigned char *buf, int len);
int wav_close(out123_handle *);
int au_close(out123_handle *);
int raw_close(out123_handle *);
int cdr_formats(out123_handle *);
int au_formats(out123_handle *);
int raw_formats(out123_handle *);
int wav_formats(out123_handle *);
void wav_drain(out123_handle *);

#endif

