/*
	dump_seekindex: Scan a mpeg file and dump its seek index.

	copyright 2010 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Patrick Dehne
*/

#include <mpg123.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	mpg123_handle *m;
	off_t* offsets;
	off_t step;
	size_t fill, i;

	if(argc != 2)
	{
		fprintf(stderr, "\nI will dump the frame index of an MPEG audio file.\n");
		fprintf(stderr, "\nUsage: %s <mpeg audio file>\n\n", argv[0]);
		return -1;
	}
	mpg123_init();
	m = mpg123_new(NULL, NULL);
	mpg123_param(m, MPG123_RESYNC_LIMIT, -1, 0);
	mpg123_param(m, MPG123_INDEX_SIZE, -1, 0);
	mpg123_open(m, argv[1]);
	mpg123_scan(m);

	mpg123_index(m, &offsets, &step, &fill);
	for(i=0; i<fill;i++) {
		printf("Frame number %d: file offset %d\n", i * step, offsets[i]);
	}

	mpg123_close(m);
	mpg123_delete(m);
	mpg123_exit();
	return 0;
}
