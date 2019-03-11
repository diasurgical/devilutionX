/*
	buffer.h: output buffer

	copyright 1999-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Daniel Kobras / Oliver Fromme
*/

/*
 * Application specific interaction between main and buffer
 * process. This is much less generic than the functions in
 * xfermem so I chose to put it in buffer.[hc].
 * 01/28/99 [dk]
 */

#ifndef _MPG123_BUFFER_H_
#define _MPG123_BUFFER_H_

#include "out123_int.h"
#include "compat.h"

int  buffer_init(out123_handle *ao, size_t bytes);
void buffer_exit(out123_handle *ao);

/* Messages with payload. */

int buffer_sync_param(out123_handle *ao);
int buffer_open(out123_handle *ao, const char* driver, const char* device);
int buffer_encodings(out123_handle *ao);
int buffer_formats( out123_handle *ao, const long *rates, int ratecount
                  , int minchannels, int maxchannels
                  , struct mpg123_fmt **fmtlist );
int buffer_start(out123_handle *ao);
void buffer_ndrain(out123_handle *ao, size_t bytes);

/* Simple messages to be deal with after playback. */

void buffer_stop(out123_handle *ao);
void buffer_close(out123_handle *ao);
void buffer_continue(out123_handle *ao);
/* Still undecided if that one is to be used anywhere. */
void buffer_ignore_lowmem(out123_handle *ao);
void buffer_drain(out123_handle *ao);
void buffer_end(out123_handle *ao);

/* Simple messages with interruption of playback. */

void buffer_pause(out123_handle *ao);
void buffer_drop(out123_handle *ao);

/* The actual work: Hand over audio data. */
size_t buffer_write(out123_handle *ao, void *buffer, size_t bytes);

/* Thin wrapper over xfermem giving the current buffer fill. */
size_t buffer_fill(out123_handle *ao);

/* Special handler to safely read values from command channel with
   an additional buffer handed in. Exported for read_parameters(). */
int read_buf(int fd, void *addr, size_t size
,	byte *prebuf, int *preoff, int presize);

/* Read/write strings from/to command channel. 0 on success. */
int xfer_write_string(out123_handle *ao, int who, const char *buf);
int xfer_read_string(out123_handle *ao, int who, char* *buf);

#endif
