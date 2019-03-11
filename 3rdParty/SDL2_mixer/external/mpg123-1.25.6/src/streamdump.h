/*
	streamdump: Dumping a copy of the input data.

	copyright 2010 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#ifndef STREAMDUMP_H
#define STREAMDUMP_H

#include "mpg123app.h"

/* Open dump stream, if requested, and replace readers.
   Return value is 0 for no error, -1 when bad. */
int dump_open(mpg123_handle *mh);
/* Just close... */
void dump_close(void);

#endif
