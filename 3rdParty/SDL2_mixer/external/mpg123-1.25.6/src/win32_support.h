/*
	Win32 support helper file

	This file is only for use with the mpg123 frontend.
	win32 support helpers for libmpg123 are in src/libmpg123/compat.h
*/
#ifndef MPG123_WIN32_SUPPORT_H
#define MPG123_WIN32_SUPPORT_H

#include "config.h"
#include "mpg123.h"
#include "httpget.h"
#ifdef HAVE_WINDOWS_H

#define WIN32_LEAN_AND_MEAN 1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
#include <winnls.h>
#include <shellapi.h>
#include <mmsystem.h>

#if defined (HAVE_WS2TCPIP_H) && !defined (__CYGWIN__)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#if defined (WANT_WIN32_SOCKETS) /*conflict with gethostname and select in select.h and unistd.h */
/* 
Note: Do not treat return values as valid file/socket handles, they only indicate success/failure.
file descriptors are ignored, only the local ws.local_socket is used for storing socket handle,
so the socket handle is always associated with the last call to win32_net_http_open
*/

/**
 * Opens an http URL
 * @param[in] url URL to open
 * @param[out] hd http data info
 * @return -1 for failure, 1 for success
 */
int win32_net_http_open(char* url, struct httpdata *hd);

/**
 * Reads from network socket
 * @param[in] filedes Value is ignored, last open connection is used.
 * @param[out] buf buffer to store data.
 * @param[in] nbyte bytes to read.
 * @return bytes read successfully from socket
 */
ssize_t win32_net_read (int fildes, void *buf, size_t nbyte);

/**
 * Writes to network socket
 * @param[in] filedes Value is ignored, last open connection is used.
 * @param[in] buf buffer to read data from.
 * @param[in] nbyte bytes to write.
 * @return bytes written successfully to socket
 */
ssize_t win32_net_write (int fildes, const void *buf, size_t nbyte);

/**
 * Similar to fgets - get a string from a stream
 * @param[out] s buffer to Write to
 * @param[in] n bytes of data to read.
 * @param[in] stream ignored for compatiblity, last open connection is used.
 * @return pointer to s if successful, NULL if failture
 */
char *win32_net_fgets(char *s, int n, int stream);

/**
 * Initialize Winsock 2.2.
 */
void win32_net_init (void);

/**
 * Shutdown all win32 sockets.
 */
void win32_net_deinit (void);

/**
 * Close last open socket.
 * @param[in] sock value is ignored.
 */
void win32_net_close (int sock);

/**
 * Set reader callback for mpg123_open_fd
 * @param[in] fr pointer to a mpg123_handle struct.
 */
void win32_net_replace (mpg123_handle *fr);
#endif

#ifdef WANT_WIN32_UNICODE
/**
 * Put the windows command line into argv / argc, encoded in UTF-8.
 * You are supposed to free up resources by calling win32_cmdline_free with the values you got from this one.
 * @return 0 on success, -1 on error */
int win32_cmdline_utf8(int * argc, char *** argv);

/**
 * Free up cmdline memory (the argv itself, theoretically hidden resources, too).
 */
void win32_cmdline_free(int argc, char **argv);

#endif /* WIN32_WANT_UNICODE */

/**
 * Set process priority
 * @param arg -2: Idle, -1, bellow normal, 0, normal (ignored), 1 above normal, 2 highest, 3 realtime
 */
void win32_set_priority (const int arg);

#ifdef WANT_WIN32_FIFO
/**
 * win32_fifo_mkfifo
 * Creates a named pipe of path.
 * Should be closed with win32_fifo_close.
 * @param[in] path Path of pipe, should be in form of "\\.\pipe\pipename".
 * @return -1 on failure, 0 otherwise.
 * @see win32_fifo_close
 */
int win32_fifo_mkfifo(const char *path);

/**
 *win32_fifo_close
 * Closes previously open pipe
 */
void win32_fifo_close(void);

/**
 * win32_fifo_read_peek
 * Checks how many bytes in fifo is pending read operation
 * Only the tv_sec member in timeval is evaluated! No microsecond precision.
 * @param[in] tv contains information on block duration
 * @return bytes available
 */
DWORD win32_fifo_read_peek(struct timeval *tv);

/***
 * win32_fifo_read
 * Read up to nbyte of data from open pipe into buf
 * @param[in] buf Pointer to buffer.
 * @param[in] nbyte Number of bytes to read up to.
 * @return Number of bytes actually read.
 */
ssize_t win32_fifo_read(void *buf, size_t nbyte);
#endif /* #ifdef WANT_WIN32_FIFO */

#endif /* HAVE_WINDOWS_H */
#endif /* MPG123_WIN32_SUPPORT_H */

