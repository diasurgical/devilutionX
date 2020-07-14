/**
	libsmacker - A C library for decoding .smk Smacker Video files
	Copyright (C) 2012-2017 Greg Kennedy

	See smacker.h for more information.

	smk_hufftree.h
		SMK huffmann trees.  There are two types:
		- a basic 8-bit tree, and
		- a "big" 16-bit tree which includes a cache for recently
			searched values.
*/

#ifndef SMK_HUFFTREE_H
#define SMK_HUFFTREE_H

#include "smk_bitstream.h"

/** Tree node structures - Forward declaration */
struct smk_huff8_t;
struct smk_huff16_t;

/*********************** 8-BIT HUFF-TREE FUNCTIONS ***********************/
/** This macro checks return code from _smk_huff8_build and
	jumps to error label if problems occur. */
#define smk_huff8_build(bs,t) \
{ \
	if (!(t = _smk_huff8_build(bs))) \
	{ \
		fprintf(stderr, "libsmacker::smk_huff8_build(" #bs ", " #t ") - ERROR (file: %s, line: %lu)\n", __FILE__, (unsigned long)__LINE__); \
		goto error; \
	} \
}
/** Build an 8-bit tree from a bitstream */
struct smk_huff8_t* _smk_huff8_build(struct smk_bit_t* bs);

/** This macro checks return code from _smk_huff8_lookup and
	jumps to error label if problems occur. */
#define smk_huff8_lookup(bs,t,s) \
{ \
	if ((short)(s = _smk_huff8_lookup(bs, t)) < 0) \
	{ \
		fprintf(stderr, "libsmacker::smk_huff8_lookup(" #bs ", " #t ", " #s ") - ERROR (file: %s, line: %lu)\n", __FILE__, (unsigned long)__LINE__); \
		goto error; \
	} \
}
/** Look up an 8-bit value in the referenced tree by following a bitstream
	returns -1 on error */
short _smk_huff8_lookup(struct smk_bit_t* bs, const struct smk_huff8_t* t);

/** function to recursively delete an 8-bit huffman tree */
void smk_huff8_free(struct smk_huff8_t* t);

/************************ 16-BIT HUFF-TREE FUNCTIONS ************************/
/** This macro checks return code from _smk_huff16_build and
	jumps to error label if problems occur. */
#define smk_huff16_build(bs,t) \
{ \
	if (!(t = _smk_huff16_build(bs))) \
	{ \
		fprintf(stderr, "libsmacker::smk_huff16_build(" #bs ", " #t ") - ERROR (file: %s, line: %lu)\n", __FILE__, (unsigned long)__LINE__); \
		goto error; \
	} \
}
/** Build a 16-bit tree from a bitstream */
struct smk_huff16_t* _smk_huff16_build(struct smk_bit_t* bs);

/** This macro checks return code from smk_huff16_lookup and
	jumps to error label if problems occur. */
#define smk_huff16_lookup(bs,t,s) \
{ \
	if ((s = _smk_huff16_lookup(bs, t)) < 0) \
	{ \
		fprintf(stderr, "libsmacker::smk_huff16_lookup(" #bs ", " #t ", " #s ") - ERROR (file: %s, line: %lu)\n", __FILE__, (unsigned long)__LINE__); \
		goto error; \
	} \
}
/** Look up a 16-bit value in the bigtree by following a bitstream
	returns -1 on error */
long _smk_huff16_lookup(struct smk_bit_t* bs, struct smk_huff16_t* big);

/** Reset the cache in a 16-bit tree */
void smk_huff16_reset(struct smk_huff16_t* big);

/** function to recursively delete a 16-bit huffman tree */
void smk_huff16_free(struct smk_huff16_t* big);

#endif
