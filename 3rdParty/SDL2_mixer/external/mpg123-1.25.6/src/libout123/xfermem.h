/*
	xfermem: unidirectional fast pipe

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Oliver Fromme
	old timestamp: Sat Mar 29 04:41:34 MET 1997

	This is a stand-alone module which implements a unidirectional,
	fast pipe using mmap().  Its primary use is to transfer large
	amounts of data from a parent process to its child process,
	with a buffer in between which decouples blocking conditions
	on both sides.  Control information is transferred between the
	processes through a socketpair.  See xftest.c for an example on
	how to use this module.

	note: xftest not there anymore
*/

#ifndef _XFERMEM_H_
#define _XFERMEM_H_

#include "compat.h"

typedef struct {
	size_t freeindex;	/* [W] next free index */
	size_t readindex;	/* [R] next index to read */
	int fd[2];
	char *data;
	char *metadata;
	size_t size;
	size_t metasize;
} txfermem;
/*
 *   [W] -- May be written to by the writing process only!
 *   [R] -- May be written to by the reading process only!
 *   All other entries are initialized once.
 */

void xfermem_init (txfermem **xf, size_t bufsize, size_t msize, size_t skipbuf);
void xfermem_init_writer (txfermem *xf);
void xfermem_init_reader (txfermem *xf);

size_t xfermem_get_freespace (txfermem *xf);
size_t xfermem_get_usedspace (txfermem *xf);

/* Unless otherwise noted, each command demands a reponse if issued from the
   writer. The reader does not expect responses, only orders. */
enum xf_cmd_code
{
	XF_CMD_PING = 1  /**< Wake up and give a response, not changing any state. */
,	XF_CMD_PONG      /**< The response to a ping. */
,	XF_CMD_DATA      /**< Re-check the amount of data available without response. */
,	XF_CMD_TERMINATE /**< Stop operation. */
,	XF_CMD_DROP      /**< Drop current buffer contents. */
,	XF_CMD_DRAIN     /**< Consume current buffer contents now. */
,	XF_CMD_PAUSE     /**< Pause operation, wait for next command. */
,	XF_CMD_CONTINUE  /**< Continue operation. */
,	XF_CMD_IGNLOW    /**< Ignore situation with low buffer fill. */
,	XF_CMD_OK        /**< Response from reader: Operation succeeded. */
,	XF_CMD_ERROR     /**< Response from reader: Operation failed.  */
,	XF_CMD_CUSTOM1   /**< Some custom command to be filled with meaning. */
,	XF_CMD_CUSTOM2   /**< Some custom command to be filled with meaning. */
,	XF_CMD_CUSTOM3   /**< Some custom command to be filled with meaning. */
,	XF_CMD_CUSTOM4   /**< Some custom command to be filled with meaning. */
,	XF_CMD_CUSTOM5   /**< Some custom command to be filled with meaning. */
,	XF_CMD_CUSTOM6   /**< Some custom command to be filled with meaning. */
,	XF_CMD_CUSTOM7   /**< Some custom command to be filled with meaning. */
,	XF_CMD_CUSTOM8   /**< Some custom command to be filled with meaning. */
};

#define XF_WRITER 0
#define XF_READER 1
int xfermem_getcmd(int fd, int block);
int xfermem_getcmds(int fd, int block, byte* cmds, int count);
int xfermem_putcmd(int fd, byte cmd);
int xfermem_writer_block(txfermem *xf);
/* returns TRUE for being interrupted */
int xfermem_write(txfermem *xf, void *buffer, size_t bytes);

void xfermem_done (txfermem *xf);
#define xfermem_done_writer xfermem_init_reader
#define xfermem_done_reader xfermem_init_writer


#endif 
