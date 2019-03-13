/*
	term: terminal control

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#ifndef _MPG123_TERM_H_
#define _MPG123_TERM_H_

#include "mpg123app.h"
#include "audio.h"

#ifdef HAVE_TERMIOS

#define LOOP_CYCLES	0.500000	/* Loop time in sec */

/* 
 * Defines the keybindings in term.c - change to your 
 * own preferences.
 */

#define MPG123_HELP_KEY	'h'
#define MPG123_BACK_KEY	'b'
#define MPG123_NEXT_KEY	'f'
#define MPG123_PAUSE_KEY	'p'
#define MPG123_QUIT_KEY	'q'
/* space bar is alias for that */
#define MPG123_STOP_KEY	's'
#define MPG123_REWIND_KEY	','
#define MPG123_FORWARD_KEY	'.'
/* This is convenient on QWERTZ-keyboards. */
#define MPG123_FAST_REWIND_KEY ';'
#define MPG123_FAST_FORWARD_KEY ':'
#define MPG123_FINE_REWIND_KEY '<'
#define MPG123_FINE_FORWARD_KEY '>'
/* You probably want to use the following bindings instead
 * on a standard QWERTY-keyboard:
 */
 
/* #define MPG123_FAST_REWIND_KEY '<' */
/* #define MPG123_FAST_FORWARD_KEY '>' */
/* #define MPG123_FINE_REWIND_KEY ';' */
/* #define MPG123_FINE_FORWARD_KEY ':' */

#define MPG123_VOL_UP_KEY '+'
#define MPG123_VOL_DOWN_KEY '-'
#define MPG123_VERBOSE_KEY 'v'
#define MPG123_RVA_KEY 'r'
#define MPG123_PLAYLIST_KEY 'l'
#define MPG123_PREV_KEY 'd'
#define MPG123_MPEG_KEY 'm'
#define MPG123_TAG_KEY  't'
#define MPG123_PITCH_UP_KEY    'c'
#define MPG123_PITCH_BUP_KEY   'C'
#define MPG123_PITCH_DOWN_KEY  'x'
#define MPG123_PITCH_BDOWN_KEY 'X'
#define MPG123_PITCH_ZERO_KEY  'w'
#define MPG123_BOOKMARK_KEY    'k'
/* This counts as "undocumented" and can disappear */
#define MPG123_FRAME_INDEX_KEY 'i'
#define MPG123_VARIOUS_INFO_KEY 'I'

#define MPG123_PREV_DIR_KEY '['
#define MPG123_NEXT_DIR_KEY ']'

/* The normal and big pitch adjustment done on key presses. */
#define MPG123_PITCH_VAL 0.001
#define MPG123_PITCH_BVAL 0.01

#define MPG123_PAUSED_STRING	"Paused. \b\b\b\b\b\b\b\b"
#define MPG123_STOPPED_STRING	"Stopped.\b\b\b\b\b\b\b\b"
#define MPG123_EMPTY_STRING	"        \b\b\b\b\b\b\b\b"

/* Need it as string for the param struct, change according to the above. */
#define MPG123_TERM_USR1 "s"
#define MPG123_TERM_USR2 "f"


void term_init(void);
void term_exit(void);
off_t term_control(mpg123_handle *mh, out123_handle *ao);
void term_hint(void); /* Print a message hinting at terminal usage. */

#endif

#endif
