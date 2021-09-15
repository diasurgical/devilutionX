/**
	libsmacker - A C library for decoding .smk Smacker Video files
	Copyright (C) 2012-2017 Greg Kennedy

	See smacker.h for more information.

	smk_malloc.h
		"Safe" implementations of malloc and free.
		Verbose implementation of assert.
*/

#ifndef SMK_MALLOC_H
#define SMK_MALLOC_H

/* assert */
#include <assert.h>
/* calloc */
#include <stdlib.h>
/* fprintf */
#include <stdio.h>

/* Error messages from calloc */
#include <errno.h>
#include <string.h>

/**
	Safe free: attempts to prevent double-free by setting pointer to NULL.
		Optionally warns on attempts to free a NULL pointer.
*/
#define smk_free(p) \
{ \
	assert (p); \
	free(p); \
	p = NULL; \
}

/**
	Safe malloc: exits if calloc() returns NULL.
		Also initializes blocks to 0.
	Optionally warns on attempts to malloc over an existing pointer.
	Ideally, one should not exit() in a library. However, if you cannot
		calloc(), you probably have bigger problems.
*/
#define smk_malloc(p, x) \
{ \
	assert (p == NULL); \
	p = calloc(1, x); \
	if (!p) \
	{ \
		fprintf(stderr, "libsmacker::smk_malloc(" #p ", %lu) - ERROR: calloc() returned NULL (file: %s, line: %lu)\n\tReason: [%d] %s\n", \
			(unsigned long) (x), __FILE__, (unsigned long)__LINE__, errno, strerror(errno)); \
		exit(EXIT_FAILURE); \
	} \
}

#endif
