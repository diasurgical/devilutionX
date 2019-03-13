/*
	scan: Estimate length (sample count) of a mpeg file and compare to length from exact scan.

	copyright 2007 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Thomas Orgis
*/

/* Note the lack of error checking here.
   While it would be nicer to inform the user about troubles, libmpg123 is designed _not_ to bite you on operations with invalid handles , etc.
  You just jet invalid results on invalid operations... */

#include <mpg123.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	mpg123_handle *m;
	int i;
	if(argc < 2)
	{
		fprintf(stderr, "\nI will give you the estimated and exact sample lengths of MPEG audio files.\n");
		fprintf(stderr, "\nUsage: %s <mpeg audio file list>\n\n", argv[0]);
		return -1;
	}
	mpg123_init();
	m = mpg123_new(NULL, NULL);
	mpg123_param(m, MPG123_RESYNC_LIMIT, -1, 0); /* New in library version 0.0.1 . */
	for(i = 1; i < argc; ++i)
	{
		off_t a, b;
		
		mpg123_open(m, argv[i]);

		a = mpg123_length(m);		
		mpg123_scan(m);
		b = mpg123_length(m);

		mpg123_close(m);

		printf("File %i: estimated %li vs. scanned %li\n", i, (long)a, (long)b);
	}

	mpg123_delete(m);
	mpg123_exit();
	return 0;
}
