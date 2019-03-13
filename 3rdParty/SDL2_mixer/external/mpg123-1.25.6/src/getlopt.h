/*
	getlopt: command line option/parameter parsing

	copyright ?-2006 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written Oliver Fromme
	old timestamp: Tue Apr  8 07:13:39 MET DST 1997
*/

#include <stdlib.h>
#include <string.h>


#ifndef _MPG123_GETLOPT_H_
#define _MPG123_GETLOPT_H_

extern int loptind;	/* index in argv[] */
extern int loptchr;	/* index in argv[loptind] */
extern char *loptarg;	/* points to argument if present, else to option */

typedef struct {
	char sname;	/* short option name, can be 0 */
	char *lname;	/* long option name, can be 0 */
	int flags;	/* see below */
	void (*func)(char *);	/* called if != 0 (after setting of var) */
	void *var;	/* type is *long, *char or **char, see below */
	long value;
} topt;

/* ThOr: make this clear; distict long from int (since this is != on my Alpha) and really use a flag for every case (spare the 0 case 
for .... no flag) */
#define GLO_ARG  1
#define GLO_CHAR 2
#define GLO_INT  4
#define GLO_LONG 8
#define GLO_DOUBLE 16

/* flags:
 *	bit 0 = 0 - no argument
 *		if var != NULL
 *			*var := value or (char)value [see bit 1]
 *		else
 *			loptarg = &option
 *			return ((value != 0) ? value : sname)
 *	bit 0 = 1 - argument required
 *		if var != NULL
 *			*var := atoi(arg) or strdup(arg) [see bit 1]
 *		else
 *			loptarg = &arg
 *			return ((value != 0) ? value : sname)
 *
 *	bit 1 = 1 - var is a pointer to a char (or string),
 *			and value is interpreted as char
 *	bit 2 = 1 - var is a pointer to int
 *	bit 3 = 1 - var is a pointer to long
 *
 * Note: The options definition is terminated by a topt
 *	 containing only zeroes.
 */

#define GLO_END		0
#define GLO_UNKNOWN	-1
#define GLO_NOARG	-2
#define GLO_CONTINUE	-3

int getlopt (int argc, char *argv[], topt *opts);

/* return values:
 *	GLO_END		(0)	end of options
 *	GLO_UNKNOWN	(-1)	unknown option *loptarg
 *	GLO_NOARG	(-2)	missing argument
 *	GLO_CONTINUE	(-3)	(reserved for internal use)
 *	else - return value according to flags (see above)
 */


#endif
