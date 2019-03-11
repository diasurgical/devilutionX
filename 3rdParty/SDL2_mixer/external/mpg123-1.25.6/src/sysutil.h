/*
	sysutil: generic utilities to interact with the OS (signals, paths)

	copyright ?-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp (dissected/renamed by Thomas Orgis)
*/

#ifndef _MPG123_SYSUTIL_H_
#define _MPG123_SYSUTIL_H_

int split_dir_file(const char *path, char **dname, char **fname);
/* Length of directory part in given path. */
size_t dir_length(const char *path);

#endif

