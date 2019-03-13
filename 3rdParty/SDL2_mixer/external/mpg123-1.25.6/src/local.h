#ifndef H_LOCAL
#define H_LOCAL
/*
	local: some stuff for localisation

	Currently, this is just about determining if we got UTF-8 locale.

	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis, based on a patch by Thorsten Glaser.
*/

/* Pulled in by mpg123app.h! */

/* This is 1 if check_locale found UTF-8, 0 if not. */
extern int utf8env;

/* Check/set locale, set the utf8env variable. */
void check_locale(void);

#endif
