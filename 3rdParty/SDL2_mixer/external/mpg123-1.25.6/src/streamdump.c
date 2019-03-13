/*
	streamdump: Dumping a copy of the input data.

	copyright 2010 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp
*/

#include "streamdump.h"
#include <fcntl.h>
#include <errno.h>
#include "debug.h"

/* Stream dump descriptor. */
static int dump_fd = -1;

/* Read data from input, write copy to dump file. */
static ssize_t dump_read(int fd, void *buf, size_t count)
{
	ssize_t ret = read(fd, buf, count);
	if(ret > 0 && dump_fd > -1)
	{
		write(dump_fd, buf, ret);
	}
	return ret;
}

/* Also mirror seeks, to prevent messed up dumps of seekable streams. */
static off_t dump_seek(int fd, off_t pos, int whence)
{
	off_t ret = lseek(fd, pos, whence);
	if(ret >= 0 && dump_fd > -1)
	{
		lseek(dump_fd, pos, whence);
	}
	return ret;
}

/* External API... open and close. */
int dump_open(mpg123_handle *mh)
{
	int ret;

	if(param.streamdump == NULL) return 0;

	if(!param.quiet) fprintf(stderr, "Note: Dumping stream to %s\n", param.streamdump);

	dump_fd = compat_open(param.streamdump, O_CREAT|O_TRUNC|O_RDWR);
	if(dump_fd < 0)
	{
		error1("Failed to open dump file: %s\n", strerror(errno));
		return -1;
	}

#ifdef WIN32
	_setmode(dump_fd, _O_BINARY);
#endif

	ret = mpg123_replace_reader(mh, dump_read, dump_seek);
	if(ret != MPG123_OK)
	{
		error1("Unable to replace reader for stream dump: %s\n", mpg123_strerror(mh));
		dump_close();
		return -1;
	}
	else return 0;
}

void dump_close(void)
{
	if(dump_fd > -1) compat_close(dump_fd);

	dump_fd = -1;
}
