/*
	audio: audio output interface

	This is what is left after separating out libout123.

	copyright ?-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

/* 
 * Audio 'LIB' defines
 */


#ifndef _MPG123_AUDIO_H_
#define _MPG123_AUDIO_H_

#include "compat.h"
#include "mpg123.h"
#include "out123.h"

#define pitch_rate(rate)	(param.pitch == 0 ? (rate) : (long) ((param.pitch+1.0)*(rate)))

mpg123_string* audio_enclist(void);

void audio_capabilities(out123_handle *ao, mpg123_handle *mh);
void print_capabilities(out123_handle *ao, mpg123_handle *mh);

/*
	Twiddle audio output rate to yield speedup/down (pitch) effect.
	The actually achieved pitch value is stored in param.pitch.
	Returns 1 if pitch setting succeeded, 0 otherwise.
*/
int set_pitch(mpg123_handle *fr, out123_handle *ao, double new_pitch);

#endif

