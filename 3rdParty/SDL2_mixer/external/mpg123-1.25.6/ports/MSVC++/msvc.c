/*
	msvc: libmpg123 add-ons for MSVC++

	copyright 1995-2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org

	originally written by Patrick Dehne (inspired by libmpg123/readers.c)

*/

#include "mpg123lib_intern.h"

#include <tchar.h>
#include <fcntl.h>
#include <io.h>

#include "debug.h"

/* mpg123_replace_reader expects size_t as count, _read unsigned int */
static ssize_t read_helper(int handle, void *dest, size_t count)
{
	return _read(handle, dest, (unsigned int) count);
}

int mpg123_topen(mpg123_handle *fr, const _TCHAR *path)
{
	int ret;
	int filept; /* descriptor of opened file/stream */

	ret = mpg123_replace_reader(fr, read_helper, _lseek);
	if(ret != MPG123_OK)
	{
		return ret;
	}

	if((filept = _topen(path, O_RDONLY|O_BINARY)) < 0)
	{
		/* Will not work with unicode path name
		   if(NOQUIET) error2("Cannot open file %s: %s", path, strerror(errno)); */

		if(NOQUIET) error1("Cannot open file: %s", strerror(errno));
		fr->err = MPG123_BAD_FILE;
		return filept; /* error... */
	}

	if(mpg123_open_fd(fr, filept) == MPG123_OK) {
		return MPG123_OK;
	}
	else
	{
		_close(filept);
		return MPG123_ERR;
	}
}

int mpg123_tclose(mpg123_handle *fr)
{
	int ret, filept;

	filept = fr->rdat.filept;
	ret = mpg123_close(fr);
	_close(filept);

	return ret;
}
