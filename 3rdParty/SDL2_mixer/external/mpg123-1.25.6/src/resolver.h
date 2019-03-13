/*
	resolver.c: TCP network stuff, for IPv4 and IPv6

	copyright 2008 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written Thomas Orgis (based on httpget.c)
*/

#ifndef MPG123_RESOLVER_H
#define MPG123_RESOLVER_H

#include "mpg123app.h"
/*
	Split an URL into parts of user:password, hostname, port, path on host.
	There is no name resolution going on here, also no numeric conversion.
	The URL string is supposed to be stripped of all \r and \n.
	Return code 1 (TRUE) is fine, 0 (FALSE) is bad.
*/
int split_url(mpg123_string *url, mpg123_string *auth, mpg123_string *host, mpg123_string *port, mpg123_string *path);
/*
	Open a connection to specified server and port.
	The arguments are plain strings (hostname or IP, port number as string); any network specific data types are hidden.
*/
int open_connection(mpg123_string *host, mpg123_string *port);

#endif
