/*
	stringlists: creation of paired string lists for one-time consumption

	copyright 2015 by the mpg123 project
	free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis
*/

#ifndef MPG123_H_STRINGLISTS
#define MPG123_H_STRINGLISTS

int stringlists_add( char ***alist, char ***blist
                   , const char *atext, const char *btext, int *count);

#endif
