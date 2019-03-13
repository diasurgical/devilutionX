/*
	playlist: playlist logic

	copyright 1995-2007 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp, outsourced/reorganized by Thomas Orgis
*/
#ifndef MPG123_PLAYLIST_H
#define MPG123_PLAYLIST_H

#include "mpg123app.h"

/* create playlist form argv including reading of playlist file */
void prepare_playlist(int argc, char** argv);
/* returns the next url to play or NULL when there is none left */
char *get_next_file(void);
/* Get current track number, optionally the total count and loop counter. */
size_t playlist_pos(size_t *total, long *loop);
/* frees memory that got allocated in prepare_playlist */
void free_playlist(void);
/* Print out the playlist, with optional position indicator. */
void print_playlist(FILE* out, int showpos);
/* This prepares a jump to be executed on next get_next_file(). */
void playlist_jump(ssize_t incr);
/* Aim for the next directory (just trigger next track for random play). */
void playlist_next_dir(void);
/* Same for previous one. */
void playlist_prev_dir(void);

#endif
