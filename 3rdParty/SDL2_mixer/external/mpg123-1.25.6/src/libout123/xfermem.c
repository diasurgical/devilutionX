/*
	xfermem: unidirectional fast pipe

	copyright ?-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Oliver Fromme
	old timestamp: Sun Apr  6 02:26:26 MET DST 1997

	See xfermem.h for documentation/description.
*/

#include "config.h"
#include "compat.h"
#include "xfermem.h"
#include <string.h>
#include <errno.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <fcntl.h>

#ifndef HAVE_MMAP
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include "debug.h"

#if defined (HAVE_MMAP) && defined(MAP_ANONYMOUS) && !defined(MAP_ANON)
#define MAP_ANON MAP_ANONYMOUS
#endif

void xfermem_init (txfermem **xf, size_t bufsize, size_t msize, size_t skipbuf)
{
	size_t regsize = bufsize + msize + skipbuf + sizeof(txfermem);

#ifdef HAVE_MMAP
#  ifdef MAP_ANON
	if ((*xf = (txfermem *) mmap(0, regsize, PROT_READ | PROT_WRITE,
			MAP_ANON | MAP_SHARED, -1, 0)) == (txfermem *) -1) {
		perror ("mmap()");
		exit (1);
	}
#  else
	int devzero;
	if ((devzero = open("/dev/zero", O_RDWR, 0)) == -1) {
		perror ("open(/dev/zero)");
		exit (1);
	}
	if ((*xf = (txfermem *) mmap(0, regsize, PROT_READ | PROT_WRITE,
			MAP_SHARED, devzero, 0)) == (txfermem *) -1) {
		perror ("mmap()");
		exit (1);
	}
	close (devzero);
#  endif
#else
	struct shmid_ds shmemds;
	int shmemid;
	if ((shmemid = shmget(IPC_PRIVATE, regsize, IPC_CREAT | 0600)) == -1) {
		perror ("shmget()");
		exit (1);
	}
	if ((*xf = (txfermem *) shmat(shmemid, 0, 0)) == (txfermem *) -1) {
		perror ("shmat()");
		shmctl (shmemid, IPC_RMID, &shmemds);
		exit (1);
	}
	if (shmctl(shmemid, IPC_RMID, &shmemds) == -1) {
		perror ("shmctl()");
		xfermem_done (*xf);
		exit (1);
	}
#endif
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, (*xf)->fd) < 0) {
		perror ("socketpair()");
		xfermem_done (*xf);
		exit (1);
	}
	(*xf)->freeindex = (*xf)->readindex = 0;
	(*xf)->data = ((char *) *xf) + sizeof(txfermem) + msize;
	(*xf)->metadata = ((char *) *xf) + sizeof(txfermem);
	(*xf)->size = bufsize;
	(*xf)->metasize = msize + skipbuf;
}

void xfermem_done (txfermem *xf)
{
	if(!xf)
		return;
#ifdef HAVE_MMAP
	/* Here was a cast to (caddr_t) ... why? Was this needed for SunOS?
	   Casting to (void*) should silence compilers in case of funny
	   prototype for munmap(). */
	munmap ( (void*)xf, xf->size + xf->metasize + sizeof(txfermem));
#else
	if (shmdt((void *) xf) == -1) {
		perror ("shmdt()");
		exit (1);
	}
#endif
}

void xfermem_init_writer (txfermem *xf)
{
	if(xf)
		close (xf->fd[XF_READER]);
	debug1("xfermem writer fd=%i", xf->fd[XF_WRITER]);
}

void xfermem_init_reader (txfermem *xf)
{
	if(xf)
		close (xf->fd[XF_WRITER]);
	debug1("xfermem reader fd=%i", xf->fd[XF_READER]);
}

size_t xfermem_get_freespace (txfermem *xf)
{
	size_t freeindex, readindex;

	if(!xf)
		return 0;

	if ((freeindex = xf->freeindex) < 0
			|| (readindex = xf->readindex) < 0)
		return (0);
	if (readindex > freeindex)
		return ((readindex - freeindex) - 1);
	else
		return ((xf->size - (freeindex - readindex)) - 1);
}

size_t xfermem_get_usedspace (txfermem *xf)
{
	size_t freeindex, readindex;

	if(!xf)
		return 0;

	if ((freeindex = xf->freeindex) < 0
			|| (readindex = xf->readindex) < 0)
		return (0);
	if (freeindex >= readindex)
		return (freeindex - readindex);
	else
		return (xf->size - (readindex - freeindex));
}

static int xfermem_getcmd_raw (int fd, int block, byte *cmds, int count)
{
	fd_set selfds;
	int ret;

	for (;;) {
		struct timeval selto = {0, 0};

		FD_ZERO (&selfds);
		FD_SET (fd, &selfds);
#ifdef HPUX
		switch (select(FD_SETSIZE, (int *) &selfds, NULL, NULL, block ? NULL : &selto))
#else
		switch (select(FD_SETSIZE, &selfds, NULL, NULL, block ? NULL : &selto))
#endif
		{
			case 0:
				if (!block)
					return (0);
				continue;
			case -1:
				if (errno == EINTR)
					continue;
				return (-2);
			case 1:
				if (FD_ISSET(fd, &selfds))
					switch((ret=read(fd, cmds, count)))
					{
						case 0: /* EOF */
							return (-1);
						case -1:
							if (errno == EINTR)
								continue;
							return (-3);
						default:
							return ret;
					}
				else /* ?!? */
					return (-5);
			default: /* ?!? */
				return (-6);
		}
	}
}

/* Verbose variant for debugging communication. */
int xfermem_getcmd(int fd, int block)
{
	byte cmd;
	int res = xfermem_getcmd_raw(fd, block, &cmd, 1);
	debug3("xfermem_getcmd(%i, %i) = %i", fd, block, res == 1 ? cmd : res);
	return res == 1 ? cmd : res;
}

int xfermem_getcmds(int fd, int block, byte *cmds, int count)
{
	int res = xfermem_getcmd_raw(fd, block, cmds, count);
	debug5("xfermem_getcmds(%i, %i, %p, %i) = %i"
	,	fd, block, (void*)cmds, count
	,	res);
	return res;
}


int xfermem_putcmd (int fd, byte cmd)
{
	for (;;) {
		switch (write(fd, &cmd, 1)) {
			case 1:
				debug2("xfermem_putcmd(%i, %i) = 1", fd, cmd);
				return (1);
			case -1:
				if (errno != EINTR)
				{
					debug3("xfermem_putcmd(%i, %i) = -1 (%s)"
					,	fd, cmd, strerror(errno));
					return (-1);
				}
		}
	}
}

/*
	There is a basic assumetry between reader and writer:
	The reader does work in periodic pieces and can be relied upon to
	eventually answer a call. It is important that it does not block
	for a significant duration unless it has really nothing to do.

	The writer is more undefined in its behaviour, it is controlled by
	external agents. You cannot rely on it answering synchronization
	requests in a timely manner. But on the other hand, it can be left
	hanging for a while. The critical side is that of the reader.

	Because of that, it is only sensible to provide a voluntary
	xfermem_writer_block() here. The reader does not need such a function.
	Only if it has nothing else to do, it will simply block on
	xfermem_getcmd(), and the writer promises to xfermem_putcmd() when
	something happens.

	The writer always sends a wakeup command to the reader since the latter
	could be in the process of putting itself to sleep right now, without
	a flag indicating so being set yet.

	The reader periodically reads from its file descriptor so that it does
	not get clogged up with pending messages. It will only (and always) send
	a wakeup call in response to a received command.
*/

/* Wait a bit to get a sign of life from the reader.
   Returns -1 if even that did not work. */
int xfermem_writer_block(txfermem *xf)
{
	int myfd = xf->fd[XF_WRITER];
	int result;

	xfermem_putcmd(myfd, XF_CMD_PING);
	result = xfermem_getcmd(myfd, TRUE);
	/* Only a pong to my ping is the expected good answer.
	   Everything else is a problem to be communicated. */
	return (result == XF_CMD_PONG) ? 0 : result;
}

/* Return: 0 on success, -1 on communication error, > 0 for
   error on buffer side, some special return code from buffer to be
   evaluated. */
int xfermem_write(txfermem *xf, void *buffer, size_t bytes)
{
	if(buffer == NULL || bytes < 1) return 0;

	/* You weren't so braindead not allocating enough space at all, right? */
	while (xfermem_get_freespace(xf) < bytes)
	{
		int cmd = xfermem_writer_block(xf);
		if(cmd) /* Non-successful wait. */
			return cmd;
	}
	/* Now we have enough space. copy the memory, possibly with the wrap. */
	if(xf->size - xf->freeindex >= bytes)
	{	/* one block of free memory */
		memcpy(xf->data+xf->freeindex, buffer, bytes);
	}
	else
	{ /* two blocks */
		size_t endblock = xf->size - xf->freeindex;
		memcpy(xf->data+xf->freeindex, buffer, endblock);
		memcpy(xf->data, (char*)buffer + endblock, bytes-endblock);
	}
	/* Advance the free space pointer, including the wrap. */
	xf->freeindex = (xf->freeindex + bytes) % xf->size;
	/* Always notify the buffer process. */
	debug("write waking");
	return xfermem_putcmd(xf->fd[XF_WRITER], XF_CMD_DATA) < 0
	?	-1
	:	0;
}
