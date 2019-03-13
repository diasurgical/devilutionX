/*
	SFIFO 1.3 Simple portable lock-free FIFO

	(c) 2000-2002, David Olofson - free software under the terms of the LGPL 2.1
*/


/*
-----------------------------------------------------------
TODO:
	* Is there a way to avoid losing one byte of buffer
	  space to avoid extra variables or locking?

	* Test more compilers and environments.
-----------------------------------------------------------
 */

#include	<string.h>
#include	<stdlib.h>

#include "sfifo.h"
#include "debug.h"

/*
 * Alloc buffer, init FIFO etc...
 */
SFIFO_SCOPE int sfifo_init(sfifo_t *f, int size)
{
	memset(f, 0, sizeof(sfifo_t));

	if(size > SFIFO_MAX_BUFFER_SIZE)
		return -EINVAL;

	/*
	 * Set sufficient power-of-2 size.
	 *
	 * No, there's no bug. If you need
	 * room for N bytes, the buffer must
	 * be at least N+1 bytes. (The fifo
	 * can't tell 'empty' from 'full'
	 * without unsafe index manipulations
	 * otherwise.)
	 */
	f->size = 1;
	for(; f->size <= size; f->size <<= 1)
		;

	/* Get buffer */
	if( 0 == (f->buffer = (void *)malloc(f->size)) )
		return -ENOMEM;

	return 0;
}

/*
 * Dealloc buffer etc...
 */
SFIFO_SCOPE void sfifo_close(sfifo_t *f)
{
	if(f->buffer) {
		free(f->buffer);
		f->buffer = NULL;	/* Prevent double free */
	}
}

/*
 * Empty FIFO buffer
 */
SFIFO_SCOPE void sfifo_flush(sfifo_t *f)
{
	debug("sfifo_flush()");
	/* Reset positions */
	f->readpos = 0;
	f->writepos = 0;
}

/*
 * Write bytes to a FIFO
 * Return number of bytes written, or an error code
 */
SFIFO_SCOPE int sfifo_write(sfifo_t *f, const void *_buf, int len)
{
	int total;
	int i;
	const char *buf = (const char *)_buf;

	if(!f->buffer)
		return -ENODEV;	/* No buffer! */

	/* total = len = min(space, len) */
	total = sfifo_space(f);
	debug1("sfifo_space() = %d",total);
	if(len > total)
		len = total;
	else
		total = len;
	debug1("sfifo_write() = %d", total);

	i = f->writepos;
	if(i + len > f->size)
	{
		memcpy(f->buffer + i, buf, f->size - i);
		buf += f->size - i;
		len -= f->size - i;
		i = 0;
	}
	memcpy(f->buffer + i, buf, len);
	f->writepos = i + len;

	return total;
}


/*
 * Read bytes from a FIFO
 * Return number of bytes read, or an error code
 */
SFIFO_SCOPE int sfifo_read(sfifo_t *f, void *_buf, int len)
{
	int total;
	int i;
	char *buf = (char *)_buf;

	if(!f->buffer)
		return -ENODEV;	/* No buffer! */

	/* total = len = min(used, len) */
	total = sfifo_used(f);
	debug1("sfifo_used() = %d",total);
	if(len > total)
		len = total;
	else
		total = len;
	debug1("sfifo_read() = %d", total);

	i = f->readpos;
	if(i + len > f->size)
	{
		memcpy(buf, f->buffer + i, f->size - i);
		buf += f->size - i;
		len -= f->size - i;
		i = 0;
	}
	memcpy(buf, f->buffer + i, len);
	f->readpos = i + len;

	return total;
}

