/*
	SFIFO 1.3 Simple portable lock-free FIFO

	(c) 2000-2002, David Olofson - free software under the terms of the LGPL 2.1
*/


/*
 * Platform support:
 *	gcc / Linux / x86:		Works
 *	gcc / Linux / x86 kernel:	Works
 *	gcc / FreeBSD / x86:		Works
 *	gcc / NetBSD / x86:		Works
 *	gcc / Mac OS X / PPC:		Works
 *	gcc / Win32 / x86:		Works
 *	Borland C++ / DOS / x86RM:	Works
 *	Borland C++ / Win32 / x86PM16:	Untested
 *	? / Various Un*ces / ?:		Untested
 *	? / Mac OS / PPC:		Untested
 *	gcc / BeOS / x86:		Untested
 *	gcc / BeOS / PPC:		Untested
 *	? / ? / Alpha:			Untested
 *
 * 1.2: Max buffer size halved, to avoid problems with
 *	the sign bit...
 *
 * 1.3:	Critical buffer allocation bug fixed! For certain
 *	requested buffer sizes, older version would
 *	allocate a buffer of insufficient size, which
 *	would result in memory thrashing. (Amazing that
 *	I've manage to use this to the extent I have
 *	without running into this... *heh*)
 */

#ifndef	_SFIFO_H_
#define	_SFIFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>

/* Defining SFIFO_STATIC and then including the sfifo.c will result in local code. */
#ifdef SFIFO_STATIC
#define SFIFO_SCOPE static
#else
#define SFIFO_SCOPE
#endif

/*------------------------------------------------
	"Private" stuff
------------------------------------------------*/
/*
 * Porting note:
 *	Reads and writes of a variable of this type in memory
 *	must be *atomic*! 'int' is *not* atomic on all platforms.
 *	A safe type should be used, and  sfifo should limit the
 *	maximum buffer size accordingly.
 */
typedef int sfifo_atomic_t;
#ifdef __TURBOC__
#	define	SFIFO_MAX_BUFFER_SIZE	0x7fff
#else /* Kludge: Assume 32 bit platform */
#	define	SFIFO_MAX_BUFFER_SIZE	0x7fffffff
#endif

typedef struct sfifo_t
{
	char *buffer;
	int size;			/* Number of bytes */
	sfifo_atomic_t readpos;		/* Read position */
	sfifo_atomic_t writepos;	/* Write position */
} sfifo_t;

#define SFIFO_SIZEMASK(x)	((x)->size - 1)


/*------------------------------------------------
	API
------------------------------------------------*/
SFIFO_SCOPE int sfifo_init(sfifo_t *f, int size);
SFIFO_SCOPE void sfifo_close(sfifo_t *f);
SFIFO_SCOPE void sfifo_flush(sfifo_t *f);
SFIFO_SCOPE int sfifo_write(sfifo_t *f, const void *buf, int len);
SFIFO_SCOPE int sfifo_read(sfifo_t *f, void *buf, int len);
#define sfifo_used(x)	(((x)->writepos - (x)->readpos) & SFIFO_SIZEMASK(x))
#define sfifo_space(x)	((x)->size - 1 - sfifo_used(x))
#define sfifo_size(x)	((x)->size - 1)


#ifdef __cplusplus
};
#endif
#endif
