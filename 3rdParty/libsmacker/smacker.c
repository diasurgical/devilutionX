/**
	libsmacker - A C library for decoding .smk Smacker Video files
	Copyright (C) 2012-2021 Greg Kennedy

	See smacker.h for more information.

	smacker.c
		Main implementation file of libsmacker.
		Open, close, query, render, advance and seek an smk
*/

#include "smacker.h"

#include "smk_malloc.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ************************************************************************* */
/* BITSTREAM Structure */
/* ************************************************************************* */
/* Wraps a block of memory and adds functions to read 1 or 8 bits at a time */
struct smk_bit_t {
	const unsigned char * buffer, * end;
	unsigned int bit_num;
};

/* ************************************************************************* */
/* BITSTREAM Functions */
/* ************************************************************************* */
/** Initialize a bitstream wrapper */
static void smk_bs_init(struct smk_bit_t * const bs, const unsigned char * const b, const size_t size)
{
	/* null check */
	assert(bs);
	assert(b);
	/* set up the pointer to bitstream start and end, and set the bit pointer to 0 */
	bs->buffer = b;
	bs->end = b + size;
	bs->bit_num = 0;
}

/* Reads a bit
	Returns -1 if error encountered */
static int smk_bs_read_1(struct smk_bit_t * const bs)
{
	int ret;
	/* null check */
	assert(bs);

	/* don't die when running out of bits, but signal */
	if (bs->buffer >= bs->end) {
		fputs("libsmacker::smk_bs_read_1(): ERROR: bitstream exhausted.\n", stderr);
		return -1;
	}

	/* get next bit and store for return */
	ret = (*bs->buffer >> bs->bit_num) & 1;

	/* advance to next bit */
	if (bs->bit_num >= 7) {
		/* Out of bits in this byte: next! */
		bs->buffer ++;
		bs->bit_num = 0;
	} else
		bs->bit_num ++;

	return ret;
}

/* Reads a byte
	Returns -1 if error. */
static int smk_bs_read_8(struct smk_bit_t * const bs)
{
	/* null check */
	assert(bs);

	/* don't die when running out of bits, but signal */
	if (bs->buffer + (bs->bit_num > 0) >= bs->end) {
		fputs("libsmacker::smk_bs_read_8(): ERROR: bitstream exhausted.\n", stderr);
		return -1;
	}

	if (bs->bit_num) {
		/* unaligned read */
		int ret = *bs->buffer >> bs->bit_num;
		bs->buffer ++;
		return ret | (*bs->buffer << (8 - bs->bit_num) & 0xFF);
	}

	/* aligned read */
	return *bs->buffer++;
}

/* ************************************************************************* */
/* HUFF8 Structure */
/* ************************************************************************* */
#define SMK_HUFF8_BRANCH 0x8000
#define SMK_HUFF8_LEAF_MASK 0x7FFF

struct smk_huff8_t {
	/* Unfortunately, smk files do not store the alloc size of a small tree.
		511 entries is the pessimistic case (N codes and N-1 branches,
		with N=256 for 8 bits) */
	size_t size;
	unsigned short tree[511];
};

/* ************************************************************************* */
/* HUFF8 Functions */
/* ************************************************************************* */
/* Recursive sub-func for building a tree into an array. */
static int _smk_huff8_build_rec(struct smk_huff8_t * const t, struct smk_bit_t * const bs)
{
	int bit, value;
	assert(t);
	assert(bs);

	/* Make sure we aren't running out of bounds */
	if (t->size >= 511) {
		fputs("libsmacker::_smk_huff8_build_rec() - ERROR: size exceeded\n", stderr);
		return 0;
	}

	/* Read the next bit */
	if ((bit = smk_bs_read_1(bs)) < 0) {
		fputs("libsmacker::_smk_huff8_build_rec() - ERROR: get_bit returned -1\n", stderr);
		return 0;
	}

	if (bit) {
		/* Bit set: this forms a Branch node.
			what we have to do is build the left-hand branch,
			assign the "jump" address,
			then build the right hand branch from there.
		*/
		/* track the current index */
		value = t->size ++;

		/* go build the left branch */
		if (! _smk_huff8_build_rec(t, bs)) {
			fputs("libsmacker::_smk_huff8_build_rec() - ERROR: failed to build left sub-tree\n", stderr);
			return 0;
		}

		/* now go back to our current location, and
			mark our location as a "jump" */
		t->tree[value] = SMK_HUFF8_BRANCH | t->size;

		/* continue building the right side */
		if (! _smk_huff8_build_rec(t, bs)) {
			fputs("libsmacker::_smk_huff8_build_rec() - ERROR: failed to build right sub-tree\n", stderr);
			return 0;
		}
	} else {
		/* Bit unset signifies a Leaf node. */
		/* Attempt to read value */
		if ((value = smk_bs_read_8(bs)) < 0) {
			fputs("libsmacker::_smk_huff8_build_rec() - ERROR: get_byte returned -1\n", stderr);
			return 0;
		}

		/* store to tree */
		t->tree[t->size ++] = value;
	}

	return 1;
}

/**
	Build an 8-bit Hufftree out of a Bitstream.
*/
static int smk_huff8_build(struct smk_huff8_t * const t, struct smk_bit_t * const bs)
{
	int bit;
	/* null check */
	assert(t);
	assert(bs);

	/* Smacker huff trees begin with a set-bit. */
	if ((bit = smk_bs_read_1(bs)) < 0) {
		fputs("libsmacker::smk_huff8_build() - ERROR: initial get_bit returned -1\n", stderr);
		return 0;
	}

	/* OK to fill out the struct now */
	t->size = 0;

	/* First bit indicates whether a tree is present or not. */
	/*  Very small or audio-only files may have no tree. */
	if (bit) {
		if (! _smk_huff8_build_rec(t, bs)) {
			fputs("libsmacker::smk_huff8_build() - ERROR: tree build failed\n", stderr);
			return 0;
		}
	} else
		t->tree[0] = 0;

	/* huff trees end with an unset-bit */
	if ((bit = smk_bs_read_1(bs)) < 0) {
		fputs("libsmacker::smk_huff8_build() - ERROR: final get_bit returned -1\n", stderr);
		return 0;
	}

	/* a 0 is expected here, a 1 generally indicates a problem! */
	if (bit) {
		fputs("libsmacker::smk_huff8_build() - ERROR: final get_bit returned 1\n", stderr);
		return 0;
	}

	return 1;
}

/* Look up an 8-bit value from a basic huff tree.
	Return -1 on error. */
static int smk_huff8_lookup(const struct smk_huff8_t * const t, struct smk_bit_t * const bs)
{
	int bit, index = 0;
	/* null check */
	assert(t);
	assert(bs);

	while (t->tree[index] & SMK_HUFF8_BRANCH) {
		if ((bit = smk_bs_read_1(bs)) < 0) {
			fputs("libsmacker::smk_huff8_lookup() - ERROR: get_bit returned -1\n", stderr);
			return -1;
		}

		if (bit) {
			/* take the right branch */
			index = t->tree[index] & SMK_HUFF8_LEAF_MASK;
		} else {
			/* take the left branch */
			index ++;
		}
	}

	/* at leaf node.  return the value at this point. */
	return t->tree[index];
}

/* ************************************************************************* */
/* HUFF16 Structure */
/* ************************************************************************* */
#define SMK_HUFF16_BRANCH    0x80000000
#define SMK_HUFF16_CACHE     0x40000000
#define SMK_HUFF16_LEAF_MASK 0x3FFFFFFF

struct smk_huff16_t {
	unsigned int * tree;
	size_t size;

	/* recently-used values cache */
	unsigned short cache[3];
};

/* ************************************************************************* */
/* HUFF16 Functions */
/* ************************************************************************* */
/* Recursive sub-func for building a tree into an array. */
static int _smk_huff16_build_rec(struct smk_huff16_t * const t, struct smk_bit_t * const bs, const struct smk_huff8_t * const low8, const struct smk_huff8_t * const hi8, const size_t limit)
{
	int bit, value;
	assert(t);
	assert(bs);
	assert(low8);
	assert(hi8);

	/* Make sure we aren't running out of bounds */
	if (t->size >= limit) {
		fputs("libsmacker::_smk_huff16_build_rec() - ERROR: size exceeded\n", stderr);
		return 0;
	}

	/* Read the first bit */
	if ((bit = smk_bs_read_1(bs)) < 0) {
		fputs("libsmacker::_smk_huff16_build_rec() - ERROR: get_bit returned -1\n", stderr);
		return 0;
	}

	if (bit) {
		/* See tree-in-array explanation for HUFF8 above */
		/* track the current index */
		value = t->size ++;

		/* go build the left branch */
		if (! _smk_huff16_build_rec(t, bs, low8, hi8, limit)) {
			fputs("libsmacker::_smk_huff16_build_rec() - ERROR: failed to build left sub-tree\n", stderr);
			return 0;
		}

		/* now go back to our current location, and
			mark our location as a "jump" */
		t->tree[value] = SMK_HUFF16_BRANCH | t->size;

		/* continue building the right side */
		if (! _smk_huff16_build_rec(t, bs, low8, hi8, limit)) {
			fputs("libsmacker::_smk_huff16_build_rec() - ERROR: failed to build right sub-tree\n", stderr);
			return 0;
		}
	} else {
		/* Bit unset signifies a Leaf node. */
		/* Attempt to read LOW value */
		if ((value = smk_huff8_lookup(low8, bs)) < 0) {
			fputs("libsmacker::_smk_huff16_build_rec() - ERROR: get LOW value returned -1\n", stderr);
			return 0;
		}

		t->tree[t->size] = value;

		/* now read HIGH value */
		if ((value = smk_huff8_lookup(hi8, bs)) < 0) {
			fputs("libsmacker::_smk_huff16_build_rec() - ERROR: get HIGH value returned -1\n", stderr);
			return 0;
		}

		/* Looks OK: we got low and hi values. Return a new LEAF */
		t->tree[t->size] |= (value << 8);

		/* Last: when building the tree, some Values may correspond to cache positions.
			Identify these values and set the Escape code byte accordingly. */
		if (t->tree[t->size] == t->cache[0])
			t->tree[t->size] = SMK_HUFF16_CACHE;
		else if (t->tree[t->size] == t->cache[1])
			t->tree[t->size] = SMK_HUFF16_CACHE | 1;
		else if (t->tree[t->size] == t->cache[2])
			t->tree[t->size] = SMK_HUFF16_CACHE | 2;

		t->size ++;
	}

	return 1;
}

/* Entry point for building a big 16-bit tree. */
static int smk_huff16_build(struct smk_huff16_t * const t, struct smk_bit_t * const bs, const unsigned int alloc_size)
{
	struct smk_huff8_t low8, hi8;
	size_t limit;
	int value, i, bit;
	/* null check */
	assert(t);
	assert(bs);

	/* Smacker huff trees begin with a set-bit. */
	if ((bit = smk_bs_read_1(bs)) < 0) {
		fputs("libsmacker::smk_huff16_build() - ERROR: initial get_bit returned -1\n", stderr);
		return 0;
	}

	t->size = 0;

	/* First bit indicates whether a tree is present or not. */
	/*  Very small or audio-only files may have no tree. */
	if (bit) {
		/* build low-8-bits tree */
		if (! smk_huff8_build(&low8, bs)) {
			fputs("libsmacker::smk_huff16_build() - ERROR: failed to build LOW tree\n", stderr);
			return 0;
		}

		/* build hi-8-bits tree */
		if (! smk_huff8_build(&hi8, bs)) {
			fputs("libsmacker::smk_huff16_build() - ERROR: failed to build HIGH tree\n", stderr);
			return 0;
		}

		/* Init the escape code cache. */
		for (i = 0; i < 3; i ++) {
			if ((value = smk_bs_read_8(bs)) < 0) {
				fprintf(stderr, "libsmacker::smk_huff16_build() - ERROR: get LOW value for cache %d returned -1\n", i);
				return 0;
			}

			t->cache[i] = value;

			/* now read HIGH value */
			if ((value = smk_bs_read_8(bs)) < 0) {
				fprintf(stderr, "libsmacker::smk_huff16_build() - ERROR: get HIGH value for cache %d returned -1\n", i);
				return 0;
			}

			t->cache[i] |= (value << 8);
		}

		/* Everything looks OK so far. Time to malloc structure. */
		if (alloc_size < 12 || alloc_size % 4) {
			fprintf(stderr, "libsmacker::smk_huff16_build() - ERROR: illegal value %u for alloc_size\n", alloc_size);
			return 0;
		}

		limit = (alloc_size - 12) / 4;

		if ((t->tree = malloc(limit * sizeof(unsigned int))) == NULL) {
			perror("libsmacker::smk_huff16_build() - ERROR: failed to malloc() huff16 tree");
			return 0;
		}

		/* Finally, call recursive function to retrieve the Bigtree. */
		if (! _smk_huff16_build_rec(t, bs, &low8, &hi8, limit)) {
			fputs("libsmacker::smk_huff16_build() - ERROR: failed to build huff16 tree\n", stderr);
			free(t->tree);
			t->tree = NULL;
			return 0;
		}

		/* check that we completely filled the tree */
		if (limit != t->size) {
			fputs("libsmacker::smk_huff16_build() - ERROR: failed to completely decode huff16 tree\n", stderr);
			free(t->tree);
			t->tree = NULL;
			return 0;
		}
	} else {
		if ((t->tree = malloc(sizeof(unsigned int))) == NULL) {
			perror("libsmacker::smk_huff16_build() - ERROR: failed to malloc() huff16 tree");
			return 0;
		}

		t->tree[0] = 0;
		//t->cache[0] = t->cache[1] = t->cache[2] = 0;
	}

	/* Check final end tag. */
	if ((bit = smk_bs_read_1(bs)) < 0) {
		fputs("libsmacker::smk_huff16_build() - ERROR: final get_bit returned -1\n", stderr);
		free(t->tree);
		t->tree = NULL;
		return 0;
	}

	/* a 0 is expected here, a 1 generally indicates a problem! */
	if (bit) {
		fputs("libsmacker::smk_huff16_build() - ERROR: final get_bit returned 1\n", stderr);
		free(t->tree);
		t->tree = NULL;
		return 0;
	}

	return 1;
}

/* Look up a 16-bit value from a large huff tree.
	Return -1 on error.
	Note that this also updates the recently-used-values cache. */
static int smk_huff16_lookup(struct smk_huff16_t * const t, struct smk_bit_t * const bs)
{
	int bit, value, index = 0;
	/* null check */
	assert(t);
	assert(bs);

	while (t->tree[index] & SMK_HUFF16_BRANCH) {
		if ((bit = smk_bs_read_1(bs)) < 0) {
			fputs("libsmacker::smk_huff16_lookup() - ERROR: get_bit returned -1\n", stderr);
			return -1;
		}

		if (bit) {
			/* take the right branch */
			index = t->tree[index] & SMK_HUFF16_LEAF_MASK;
		} else {
			/* take the left branch */
			index ++;
		}
	}

	/* Get the value at this point */
	value = t->tree[index];

	if (value & SMK_HUFF16_CACHE) {
		/* uses cached value instead of actual value */
		value = t->cache[value & SMK_HUFF16_LEAF_MASK];
	}

	if (t->cache[0] != value) {
		/* Update the cache, by moving val to the front of the queue,
			if it isn't already there. */
		t->cache[2] = t->cache[1];
		t->cache[1] = t->cache[0];
		t->cache[0] = value;
	}

	return value;
}

/* ************************************************************************* */
/* SMACKER Structure */
/* ************************************************************************* */
/* tree processing order */
#define SMK_TREE_MMAP	0
#define SMK_TREE_MCLR	1
#define SMK_TREE_FULL	2
#define SMK_TREE_TYPE	3

struct smk_t {
	/* meta-info */
	/* file mode: see flags, smacker.h */
	unsigned char	mode;

	/* microsec per frame - stored as a double to handle scaling
		(large positive millisec / frame values may overflow a ul) */
	double	usf;

	/* total frames */
	unsigned long	f;
	/* does file have a ring frame? (in other words, does file loop?) */
	unsigned char	ring_frame;

	/* Index of current frame */
	unsigned long	cur_frame;

	/* SOURCE union.
		Where the data is going to be read from (or be stored),
		depending on the file mode. */
	union {
		struct {
			/* on-disk mode */
			FILE * fp;
			unsigned long * chunk_offset;
		} file;

		/* in-memory mode: unprocessed chunks */
		unsigned char ** chunk_data;
	} source;

	/* shared array of "chunk sizes"*/
	unsigned long * chunk_size;

	/* Holds per-frame flags (i.e. 'keyframe') */
	unsigned char * keyframe;
	/* Holds per-frame type mask (e.g. 'audio track 3, 2, and palette swap') */
	unsigned char * frame_type;

	/* video and audio structures */
	/* Video data type: enable/disable decode switch,
		video info and flags,
		pointer to last-decoded-palette */
	struct smk_video_t {
		/* enable/disable decode switch */
		unsigned char enable;

		/* video info */
		unsigned long	w;
		unsigned long	h;
		/* Y scale mode (constants defined in smacker.h)
			0: unscaled
			1: doubled
			2: interlaced */
		unsigned char	y_scale_mode;

		/* version ('2' or '4') */
		unsigned char	v;

		/* Huffman trees */
		unsigned long tree_size[4];
		struct smk_huff16_t tree[4];

		/* Palette data type: pointer to last-decoded-palette */
		unsigned char palette[256][3];
		/* Last-unpacked frame */
		unsigned char * frame;
	} video;

	/* audio structure */
	struct smk_audio_t {
		/* set if track exists in file */
		unsigned char exists;

		/* enable/disable switch (per track) */
		unsigned char enable;

		/* Info */
		unsigned char	channels;
		unsigned char	bitdepth;
		unsigned long	rate;
		long	max_buffer;

		/* compression type
			0: raw PCM
			1: SMK DPCM
			2: Bink (Perceptual), unsupported */
		unsigned char	compress;

		/* pointer to last-decoded-audio-buffer */
		void * buffer;
		unsigned long	buffer_size;
	} audio[7];
};

union smk_read_t {
	FILE * file;
	unsigned char * ram;
};

/* ************************************************************************* */
/* SMACKER Functions */
/* ************************************************************************* */
/* An fread wrapper: consumes N bytes, or returns -1
	on failure (when size doesn't match expected) */
static char smk_read_file(void * buf, const size_t size, FILE * fp)
{
	/* don't bother checking buf or fp, fread does it for us */
	size_t bytesRead = fread(buf, 1, size, fp);

	if (bytesRead != size) {
		fprintf(stderr, "libsmacker::smk_read_file(buf,%lu,fp) - ERROR: Short read, %lu bytes returned\n", (unsigned long)size, (unsigned long)bytesRead);
		perror("\tReason");
		return -1;
	}

	return 0;
}

/* A memcpy wrapper: consumes N bytes, or returns -1
	on failure (when size too low) */
static char smk_read_memory(void * buf, const unsigned long size, unsigned char ** p, unsigned long * p_size)
{
	if (size > *p_size) {
		fprintf(stderr, "libsmacker::smk_read_memory(buf,%lu,p,%lu) - ERROR: Short read\n", (unsigned long)size, (unsigned long)*p_size);
		return -1;
	}

	memcpy(buf, *p, size);
	*p += size;
	*p_size -= size;
	return 0;
}

/* Helper functions to do the reading, plus
	byteswap from LE to host order */
/* read n bytes from (source) into ret */
#define smk_read(ret,n) \
{ \
	if (m) \
	{ \
		r = (smk_read_file(ret,n,fp.file)); \
	} \
	else \
	{ \
		r = (smk_read_memory(ret,n,&fp.ram,&size)); \
	} \
	if (r < 0) \
	{ \
		fprintf(stderr,"libsmacker::smk_read(...) - Errors encountered on read, bailing out (file: %s, line: %lu)\n", __FILE__, (unsigned long)__LINE__); \
		goto error; \
	} \
}

/* Calls smk_read, but returns a ul */
#define smk_read_ul(p) \
{ \
	smk_read(buf,4); \
	p = ((unsigned long) buf[3] << 24) | \
		((unsigned long) buf[2] << 16) | \
		((unsigned long) buf[1] << 8) | \
		((unsigned long) buf[0]); \
}

/* PUBLIC FUNCTIONS */
/* open an smk (from a generic Source) */
static smk smk_open_generic(const unsigned char m, union smk_read_t fp, unsigned long size, const unsigned char process_mode)
{
	/* Smacker structure we intend to work on / return */
	smk s;
	/* Temporary variables */
	long temp_l;
	unsigned long temp_u;
	/* r is used by macros above for return code */
	char r;
	unsigned char buf[4] = {'\0'};
	/* video hufftrees are stored as a large chunk (bitstream)
		these vars are used to load, then decode them */
	unsigned char * hufftree_chunk = NULL;
	unsigned long tree_size;
	/* a bitstream struct */
	struct smk_bit_t bs;

	/** **/
	/* safe malloc the structure */
	if ((s = calloc(1, sizeof(struct smk_t))) == NULL) {
		perror("libsmacker::smk_open_generic() - ERROR: failed to malloc() smk structure");
		return NULL;
	}

	/* Check for a valid signature */
	smk_read(buf, 3);

	if (buf[0] != 'S' || buf[1] != 'M' || buf[2] != 'K') {
		fprintf(stderr, "libsmacker::smk_open_generic - ERROR: invalid SMKn signature (got: %s)\n", buf);
		goto error;
	}

	/* Read .smk file version */
	smk_read(&s->video.v, 1);

	if (s->video.v != '2' && s->video.v != '4') {
		fprintf(stderr, "libsmacker::smk_open_generic - Warning: invalid SMK version %c (expected: 2 or 4)\n", s->video.v);

		/* take a guess */
		if (s->video.v < '4')
			s->video.v = '2';
		else
			s->video.v = '4';

		fprintf(stderr, "\tProcessing will continue as type %c\n", s->video.v);
	}

	/* width, height, total num frames */
	smk_read_ul(s->video.w);
	smk_read_ul(s->video.h);
	smk_read_ul(s->f);
	/* frames per second calculation */
	smk_read_ul(temp_u);
	temp_l = (int)temp_u;

	if (temp_l > 0) {
		/* millisec per frame */
		s->usf = temp_l * 1000;
	} else if (temp_l < 0) {
		/* 10 microsec per frame */
		s->usf = temp_l * -10;
	} else {
		/* defaults to 10 usf (= 100000 microseconds) */
		s->usf = 100000;
	}

	/* Video flags follow.
		Ring frame is important to libsmacker.
		Y scale / Y interlace go in the Video flags.
		The user should scale appropriately. */
	smk_read_ul(temp_u);

	if (temp_u & 0x01)
		s->ring_frame = 1;

	if (temp_u & 0x02)
		s->video.y_scale_mode = SMK_FLAG_Y_DOUBLE;

	if (temp_u & 0x04) {
		if (s->video.y_scale_mode == SMK_FLAG_Y_DOUBLE)
			fputs("libsmacker::smk_open_generic - Warning: SMK file specifies both Y-Double AND Y-Interlace.\n", stderr);

		s->video.y_scale_mode = SMK_FLAG_Y_INTERLACE;
	}

	/* Max buffer size for each audio track - used to pre-allocate buffers */
	for (temp_l = 0; temp_l < 7; temp_l ++)
		smk_read_ul(s->audio[temp_l].max_buffer);

	/* Read size of "hufftree chunk" - save for later. */
	smk_read_ul(tree_size);

	/* "unpacked" sizes of each huff tree */
	for (temp_l = 0; temp_l < 4; temp_l ++)
		smk_read_ul(s->video.tree_size[temp_l]);

	/* read audio rate data */
	for (temp_l = 0; temp_l < 7; temp_l ++) {
		smk_read_ul(temp_u);

		if (temp_u & 0x40000000) {
			/* Audio track specifies "exists" flag, malloc structure and copy components. */
			s->audio[temp_l].exists = 1;
			/* and for all audio tracks */
			smk_malloc(s->audio[temp_l].buffer, s->audio[temp_l].max_buffer);

			if (temp_u & 0x80000000)
				s->audio[temp_l].compress = 1;

			s->audio[temp_l].bitdepth = ((temp_u & 0x20000000) ? 16 : 8);
			s->audio[temp_l].channels = ((temp_u & 0x10000000) ? 2 : 1);

			if (temp_u & 0x0c000000) {
				fprintf(stderr, "libsmacker::smk_open_generic - Warning: audio track %ld is compressed with Bink (perceptual) Audio Codec: this is currently unsupported by libsmacker\n", temp_l);
				s->audio[temp_l].compress = 2;
			}

			/* Bits 25 & 24 are unused. */
			s->audio[temp_l].rate = (temp_u & 0x00FFFFFF);
		}
	}

	/* Skip over Dummy field */
	smk_read_ul(temp_u);
	/* FrameSizes and Keyframe marker are stored together. */
	smk_malloc(s->keyframe, (s->f + s->ring_frame));
	smk_malloc(s->chunk_size, (s->f + s->ring_frame) * sizeof(unsigned long));

	for (temp_u = 0; temp_u < (s->f + s->ring_frame); temp_u ++) {
		smk_read_ul(s->chunk_size[temp_u]);

		/* Set Keyframe */
		if (s->chunk_size[temp_u] & 0x01)
			s->keyframe[temp_u] = 1;

		/* Bits 1 is used, but the purpose is unknown. */
		s->chunk_size[temp_u] &= 0xFFFFFFFC;
	}

	/* That was easy... Now read FrameTypes! */
	smk_malloc(s->frame_type, (s->f + s->ring_frame));

	for (temp_u = 0; temp_u < (s->f + s->ring_frame); temp_u ++)
		smk_read(&s->frame_type[temp_u], 1);

	/* HuffmanTrees
		We know the sizes already: read and assemble into
		something actually parse-able at run-time */
	smk_malloc(hufftree_chunk, tree_size);
	smk_read(hufftree_chunk, tree_size);
	/* set up a Bitstream */
	smk_bs_init(&bs, hufftree_chunk, tree_size);

	/* create some tables */
	for (temp_u = 0; temp_u < 4; temp_u ++) {
		if (! smk_huff16_build(&s->video.tree[temp_u], &bs, s->video.tree_size[temp_u])) {
			fprintf(stderr, "libsmacker::smk_open_generic - ERROR: failed to create huff16 tree %lu\n", temp_u);
			goto error;
		}
	}

	/* clean up */
	smk_free(hufftree_chunk);
	/* Go ahead and malloc storage for the video frame */
	smk_malloc(s->video.frame, s->video.w * s->video.h);
	/* final processing: depending on ProcessMode, handle what to do with rest of file data */
	s->mode = process_mode;

	/* Handle the rest of the data.
		For MODE_MEMORY, read the chunks and store */
	if (s->mode == SMK_MODE_MEMORY) {
		smk_malloc(s->source.chunk_data, (s->f + s->ring_frame) * sizeof(unsigned char *));

		for (temp_u = 0; temp_u < (s->f + s->ring_frame); temp_u ++) {
			smk_malloc(s->source.chunk_data[temp_u], s->chunk_size[temp_u]);
			smk_read(s->source.chunk_data[temp_u], s->chunk_size[temp_u]);
		}
	} else {
		/* MODE_STREAM: don't read anything now, just precompute offsets.
			use fseek to verify that the file is "complete" */
		smk_malloc(s->source.file.chunk_offset, (s->f + s->ring_frame) * sizeof(unsigned long));

		for (temp_u = 0; temp_u < (s->f + s->ring_frame); temp_u ++) {
			s->source.file.chunk_offset[temp_u] = ftell(fp.file);

			if (fseek(fp.file, s->chunk_size[temp_u], SEEK_CUR)) {
				fprintf(stderr, "libsmacker::smk_open - ERROR: fseek to frame %lu not OK.\n", temp_u);
				perror("\tError reported was");
				goto error;
			}
		}
	}

	return s;
error:
	smk_free(hufftree_chunk);
	smk_close(s);
	return NULL;
}

/* open an smk (from a memory buffer) */
smk smk_open_memory(const unsigned char * buffer, const unsigned long size)
{
	smk s = NULL;
	union smk_read_t fp;

	if (buffer == NULL) {
		fputs("libsmacker::smk_open_memory() - ERROR: buffer pointer is NULL\n", stderr);
		return NULL;
	}

	/* set up the read union for Memory mode */
	fp.ram = (unsigned char *)buffer;

	if (!(s = smk_open_generic(0, fp, size, SMK_MODE_MEMORY)))
		fprintf(stderr, "libsmacker::smk_open_memory(buffer,%lu) - ERROR: Fatal error in smk_open_generic, returning NULL.\n", size);

	return s;
}

/* open an smk (from a file) */
smk smk_open_filepointer(FILE * file, const unsigned char mode)
{
	smk s = NULL;
	union smk_read_t fp;

	if (file == NULL) {
		fputs("libsmacker::smk_open_filepointer() - ERROR: file pointer is NULL\n", stderr);
		return NULL;
	}

	/* Copy file ptr to internal union */
	fp.file = file;

	if (!(s = smk_open_generic(1, fp, 0, mode))) {
		fprintf(stderr, "libsmacker::smk_open_filepointer(file,%u) - ERROR: Fatal error in smk_open_generic, returning NULL.\n", mode);
		fclose(fp.file);
		goto error;
	}

	if (mode == SMK_MODE_MEMORY)
		fclose(fp.file);
	else
		s->source.file.fp = fp.file;

	/* fall through, return s or null */
error:
	return s;
}

/* open an smk (from a file) */
smk smk_open_file(const char * filename, const unsigned char mode)
{
	FILE * fp;

	if (filename == NULL) {
		fputs("libsmacker::smk_open_file() - ERROR: filename is NULL\n", stderr);
		return NULL;
	}

	if (!(fp = fopen(filename, "rb"))) {
		fprintf(stderr, "libsmacker::smk_open_file(%s,%u) - ERROR: could not open file\n", filename, mode);
		perror("\tError reported was");
		goto error;
	}

	/* kick processing to smk_open_filepointer */
	return smk_open_filepointer(fp, mode);
	/* fall through, return s or null */
error:
	return NULL;
}

/* close out an smk file and clean up memory */
void smk_close(smk s)
{
	unsigned long u;

	if (s == NULL) {
		fputs("libsmacker::smk_close() - ERROR: smk is NULL\n", stderr);
		return;
	}

	/* free video sub-components */
	for (u = 0; u < 4; u ++) {
		if (s->video.tree[u].tree) free(s->video.tree[u].tree);
	}

	smk_free(s->video.frame);

	/* free audio sub-components */
	for (u = 0; u < 7; u++) {
		if (s->audio[u].buffer)
			smk_free(s->audio[u].buffer);
	}

	smk_free(s->keyframe);
	smk_free(s->frame_type);

	if (s->mode == SMK_MODE_DISK) {
		/* disk-mode */
		if (s->source.file.fp)
			fclose(s->source.file.fp);

		smk_free(s->source.file.chunk_offset);
	} else {
		/* mem-mode */
		if (s->source.chunk_data != NULL) {
			for (u = 0; u < (s->f + s->ring_frame); u++)
				smk_free(s->source.chunk_data[u]);

			smk_free(s->source.chunk_data);
		}
	}

	smk_free(s->chunk_size);
	smk_free(s);
}

/* tell some info about the file */
char smk_info_all(const smk object, unsigned long * frame, unsigned long * frame_count, double * usf)
{
	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_info_all() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	if (!frame && !frame_count && !usf) {
		fputs("libsmacker::smk_info_all(object,frame,frame_count,usf) - ERROR: Request for info with all-NULL return references\n", stderr);
		goto error;
	}

	if (frame)
		*frame = (object->cur_frame % object->f);

	if (frame_count)
		*frame_count = object->f;

	if (usf)
		*usf = object->usf;

	return 0;
error:
	return -1;
}

char smk_info_video(const smk object, unsigned long * w, unsigned long * h, unsigned char * y_scale_mode)
{
	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_info_video() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	if (!w && !h && !y_scale_mode) {
		fputs("libsmacker::smk_info_all(object,w,h,y_scale_mode) - ERROR: Request for info with all-NULL return references\n", stderr);
		return -1;
	}

	if (w)
		*w = object->video.w;

	if (h)
		*h = object->video.h;

	if (y_scale_mode)
		*y_scale_mode = object->video.y_scale_mode;

	return 0;
}

char smk_info_audio(const smk object, unsigned char * track_mask, unsigned char channels[7], unsigned char bitdepth[7], unsigned long audio_rate[7])
{
	unsigned char i;

	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_info_audio() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	if (!track_mask && !channels && !bitdepth && !audio_rate) {
		fputs("libsmacker::smk_info_audio(object,track_mask,channels,bitdepth,audio_rate) - ERROR: Request for info with all-NULL return references\n", stderr);
		return -1;
	}

	if (track_mask) {
		*track_mask = ((object->audio[0].exists) |
				((object->audio[1].exists) << 1) |
				((object->audio[2].exists) << 2) |
				((object->audio[3].exists) << 3) |
				((object->audio[4].exists) << 4) |
				((object->audio[5].exists) << 5) |
				((object->audio[6].exists) << 6));
	}

	if (channels) {
		for (i = 0; i < 7; i ++)
			channels[i] = object->audio[i].channels;
	}

	if (bitdepth) {
		for (i = 0; i < 7; i ++)
			bitdepth[i] = object->audio[i].bitdepth;
	}

	if (audio_rate) {
		for (i = 0; i < 7; i ++)
			audio_rate[i] = object->audio[i].rate;
	}

	return 0;
}

/* Enable-disable switches */
char smk_enable_all(smk object, const unsigned char mask)
{
	unsigned char i;

	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_enable_all() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	/* set video-enable */
	object->video.enable = (mask & 0x80);

	for (i = 0; i < 7; i ++) {
		if (object->audio[i].exists)
			object->audio[i].enable = (mask & (1 << i));
	}

	return 0;
}

char smk_enable_video(smk object, const unsigned char enable)
{
	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_enable_video() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	object->video.enable = enable;
	return 0;
}

char smk_enable_audio(smk object, const unsigned char track, const unsigned char enable)
{
	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_enable_audio() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	object->audio[track].enable = enable;
	return 0;
}

const unsigned char * smk_get_palette(const smk object)
{
	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_get_palette() - ERROR: smk is NULL\n", stderr);
		return NULL;
	}

	return (unsigned char *)object->video.palette;
}
const unsigned char * smk_get_video(const smk object)
{
	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_get_video() - ERROR: smk is NULL\n", stderr);
		return NULL;
	}

	return object->video.frame;
}
const unsigned char * smk_get_audio(const smk object, const unsigned char t)
{
	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_get_audio() - ERROR: smk is NULL\n", stderr);
		return NULL;
	}

	return object->audio[t].buffer;
}
unsigned long smk_get_audio_size(const smk object, const unsigned char t)
{
	/* null check */
	if (object == NULL) {
		fputs("libsmacker::smk_get_audio_size() - ERROR: smk is NULL\n", stderr);
		return 0;
	}

	return object->audio[t].buffer_size;
}

/* Decompresses a palette-frame. */
static char smk_render_palette(struct smk_video_t * s, unsigned char * p, unsigned long size)
{
	/* Index into palette */
	unsigned short i = 0;
	/* Helper variables */
	unsigned short count, src;
	static unsigned char oldPalette[256][3];
	/* Smacker palette map: smk colors are 6-bit, this table expands them to 8. */
	const unsigned char palmap[64] = {
		0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C,
		0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C,
		0x41, 0x45, 0x49, 0x4D, 0x51, 0x55, 0x59, 0x5D,
		0x61, 0x65, 0x69, 0x6D, 0x71, 0x75, 0x79, 0x7D,
		0x82, 0x86, 0x8A, 0x8E, 0x92, 0x96, 0x9A, 0x9E,
		0xA2, 0xA6, 0xAA, 0xAE, 0xB2, 0xB6, 0xBA, 0xBE,
		0xC3, 0xC7, 0xCB, 0xCF, 0xD3, 0xD7, 0xDB, 0xDF,
		0xE3, 0xE7, 0xEB, 0xEF, 0xF3, 0xF7, 0xFB, 0xFF
	};
	/* null check */
	assert(s);
	assert(p);
	/* Copy palette to old palette */
	memcpy(oldPalette, s->palette, 256 * 3);

	/* Loop until palette is complete, or we are out of bytes to process */
	while ((i < 256) && (size > 0)) {
		if ((*p) & 0x80) {
			/* 0x80: Skip block
				(preserve C+1 palette entries from previous palette) */
			count = ((*p) & 0x7F) + 1;
			p ++;
			size --;

			/* check for overflow condition */
			if (i + count > 256) {
				fprintf(stderr, "libsmacker::palette_render(s,p,size) - ERROR: overflow, 0x80 attempt to skip %d entries from %d\n", count, i);
				goto error;
			}

			/* finally: advance the index. */
			i += count;
		} else if ((*p) & 0x40) {
			/* 0x40: Color-shift block
				Copy (c + 1) color entries of the previous palette,
				starting from entry (s),
				to the next entries of the new palette. */
			if (size < 2) {
				fputs("libsmacker::palette_render(s,p,size) - ERROR: 0x40 ran out of bytes for copy\n", stderr);
				goto error;
			}

			/* pick "count" items to copy */
			count = ((*p) & 0x3F) + 1;
			p ++;
			size --;
			/* start offset of old palette */
			src = *p;
			p ++;
			size --;

			/* overflow: see if we write/read beyond 256colors, or overwrite own palette */
			if (i + count > 256 || src + count > 256 ||
				(src < i && src + count > i)) {
				fprintf(stderr, "libsmacker::palette_render(s,p,size) - ERROR: overflow, 0x40 attempt to copy %d entries from %d to %d\n", count, src, i);
				goto error;
			}

			/* OK!  Copy the color-palette entries. */
			memmove(&s->palette[i][0], &oldPalette[src][0], count * 3);
			i += count;
		} else {
			/* 0x00: Set Color block
				Direct-set the next 3 bytes for palette index */
			if (size < 3) {
				fprintf(stderr, "libsmacker::palette_render - ERROR: 0x3F ran out of bytes for copy, size=%lu\n", size);
				goto error;
			}

			for (count = 0; count < 3; count ++) {
				if (*p > 0x3F) {
					fprintf(stderr, "libsmacker::palette_render - ERROR: palette index exceeds 0x3F (entry [%u][%u])\n", i, count);
					goto error;
				}

				s->palette[i][count] = palmap[*p];
				p++;
				size --;
			}

			i ++;
		}
	}

	if (i < 256) {
		fprintf(stderr, "libsmacker::palette_render - ERROR: did not completely fill palette (idx=%u)\n", i);
		goto error;
	}

	return 0;
error:
	/* Error, return -1
		The new palette probably has errors but is preferrable to a black screen */
	return -1;
}

static char smk_render_video(struct smk_video_t * s, unsigned char * p, unsigned int size)
{
	unsigned char * t = s->frame;
	unsigned char s1, s2;
	unsigned short temp;
	unsigned long i, j, k, row, col, skip;
	/* used for video decoding */
	struct smk_bit_t bs;
	/* results from a tree lookup */
	int unpack;
	/* unpack, broken into pieces */
	unsigned char type;
	unsigned char blocklen;
	unsigned char typedata;
	char bit;
	const unsigned short sizetable[64] = {
		1,	 2,	3,	4,	5,	6,	7,	8,
		9,	10,	11,	12,	13,	14,	15,	16,
		17,	18,	19,	20,	21,	22,	23,	24,
		25,	26,	27,	28,	29,	30,	31,	32,
		33,	34,	35,	36,	37,	38,	39,	40,
		41,	42,	43,	44,	45,	46,	47,	48,
		49,	50,	51,	52,	53,	54,	55,	56,
		57,	58,	59,	128,	256,	512,	1024,	2048
	};
	/* null check */
	assert(s);
	assert(p);
	row = 0;
	col = 0;
	/* Set up a bitstream for video unpacking */
	smk_bs_init(&bs, p, size);

	/* Reset the cache on all bigtrees */
	for (i = 0; i < 4; i++)
		memset(&s->tree[i].cache, 0, 3 * sizeof(unsigned short));

	while (row < s->h) {
		if ((unpack = smk_huff16_lookup(&s->tree[SMK_TREE_TYPE], &bs)) < 0) {
			fputs("libsmacker::smk_render_video() - ERROR: failed to lookup from TYPE tree.\n", stderr);
			return -1;
		}

		type = ((unpack & 0x0003));
		blocklen = ((unpack & 0x00FC) >> 2);
		typedata = ((unpack & 0xFF00) >> 8);

		/* support for v4 full-blocks */
		if (type == 1 && s->v == '4') {
			bit = smk_bs_read_1(&bs);

			if (bit)
				type = 4;
			else {
				bit = smk_bs_read_1(&bs);

				if (bit)
					type = 5;
			}
		}

		for (j = 0; (j < sizetable[blocklen]) && (row < s->h); j ++) {
			skip = (row * s->w) + col;

			switch (type) {
			case 0:
				if ((unpack = smk_huff16_lookup(&s->tree[SMK_TREE_MCLR], &bs)) < 0) {
					fputs("libsmacker::smk_render_video() - ERROR: failed to lookup from MCLR tree.\n", stderr);
					return -1;
				}

				s1 = (unpack & 0xFF00) >> 8;
				s2 = (unpack & 0x00FF);

				if ((unpack = smk_huff16_lookup(&s->tree[SMK_TREE_MMAP], &bs)) < 0) {
					fputs("libsmacker::smk_render_video() - ERROR: failed to lookup from MMAP tree.\n", stderr);
					return -1;
				}

				temp = 0x01;

				for (k = 0; k < 4; k ++) {
					for (i = 0; i < 4; i ++) {
						if (unpack & temp)
							t[skip + i] = s1;
						else
							t[skip + i] = s2;

						temp = temp << 1;
					}

					skip += s->w;
				}

				break;

			case 1: /* FULL BLOCK */
				for (k = 0; k < 4; k ++) {
					if ((unpack = smk_huff16_lookup(&s->tree[SMK_TREE_FULL], &bs)) < 0) {
						fputs("libsmacker::smk_render_video() - ERROR: failed to lookup from FULL tree.\n", stderr);
						return -1;
					}

					t[skip + 3] = ((unpack & 0xFF00) >> 8);
					t[skip + 2] = (unpack & 0x00FF);

					if ((unpack = smk_huff16_lookup(&s->tree[SMK_TREE_FULL], &bs)) < 0) {
						fputs("libsmacker::smk_render_video() - ERROR: failed to lookup from FULL tree.\n", stderr);
						return -1;
					}

					t[skip + 1] = ((unpack & 0xFF00) >> 8);
					t[skip] = (unpack & 0x00FF);
					skip += s->w;
				}

				break;

			case 2: /* VOID BLOCK */
				/* break;
				if (s->frame)
				{
					memcpy(&t[skip], &s->frame[skip], 4);
					skip += s->w;
					memcpy(&t[skip], &s->frame[skip], 4);
					skip += s->w;
					memcpy(&t[skip], &s->frame[skip], 4);
					skip += s->w;
					memcpy(&t[skip], &s->frame[skip], 4);
				} */
				break;

			case 3: /* SOLID BLOCK */
				memset(&t[skip], typedata, 4);
				skip += s->w;
				memset(&t[skip], typedata, 4);
				skip += s->w;
				memset(&t[skip], typedata, 4);
				skip += s->w;
				memset(&t[skip], typedata, 4);
				break;

			case 4: /* V4 DOUBLE BLOCK */
				for (k = 0; k < 2; k ++) {
					if ((unpack = smk_huff16_lookup(&s->tree[SMK_TREE_FULL], &bs)) < 0) {
						fputs("libsmacker::smk_render_video() - ERROR: failed to lookup from FULL tree.\n", stderr);
						return -1;
					}

					for (i = 0; i < 2; i ++) {
						memset(&t[skip + 2], (unpack & 0xFF00) >> 8, 2);
						memset(&t[skip], (unpack & 0x00FF), 2);
						skip += s->w;
					}
				}

				break;

			case 5: /* V4 HALF BLOCK */
				for (k = 0; k < 2; k ++) {
					if ((unpack = smk_huff16_lookup(&s->tree[SMK_TREE_FULL], &bs)) < 0) {
						fputs("libsmacker::smk_render_video() - ERROR: failed to lookup from FULL tree.\n", stderr);
						return -1;
					}

					t[skip + 3] = ((unpack & 0xFF00) >> 8);
					t[skip + 2] = (unpack & 0x00FF);
					t[skip + s->w + 3] = ((unpack & 0xFF00) >> 8);
					t[skip + s->w + 2] = (unpack & 0x00FF);

					if ((unpack = smk_huff16_lookup(&s->tree[SMK_TREE_FULL], &bs)) < 0) {
						fputs("libsmacker::smk_render_video() - ERROR: failed to lookup from FULL tree.\n", stderr);
						return -1;
					}

					t[skip + 1] = ((unpack & 0xFF00) >> 8);
					t[skip] = (unpack & 0x00FF);
					t[skip + s->w + 1] = ((unpack & 0xFF00) >> 8);
					t[skip + s->w] = (unpack & 0x00FF);
					skip += (s->w << 1);
				}

				break;
			}

			col += 4;

			if (col >= s->w) {
				col = 0;
				row += 4;
			}
		}
	}

	return 0;
}

/* Decompress audio track i. */
static char smk_render_audio(struct smk_audio_t * s, unsigned char * p, unsigned long size)
{
	unsigned int j, k;
	unsigned char * t = s->buffer;
	struct smk_bit_t bs;
	char bit;
	short unpack, unpack2;
	/* used for audio decoding */
	struct smk_huff8_t aud_tree[4];
	/* null check */
	assert(s);
	assert(p);

	if (!s->compress) {
		/* Raw PCM data, update buffer size and perform copy */
		s->buffer_size = size;
		memcpy(t, p, size);
	} else if (s->compress == 1) {
		/* SMACKER DPCM compression */
		/* need at least 4 bytes to process */
		if (size < 4) {
			fputs("libsmacker::smk_render_audio() - ERROR: need 4 bytes to get unpacked output buffer size.\n", stderr);
			goto error;
		}

		/* chunk is compressed (huff-compressed dpcm), retrieve unpacked buffer size */
		s->buffer_size = ((unsigned int) p[3] << 24) |
			((unsigned int) p[2] << 16) |
			((unsigned int) p[1] << 8) |
			((unsigned int) p[0]);
		p += 4;
		size -= 4;
		/* Compressed audio: must unpack here */
		/*  Set up a bitstream */
		smk_bs_init(&bs, p, size);
		bit = smk_bs_read_1(&bs);

		if (!bit) {
			fputs("libsmacker::smk_render_audio - ERROR: initial get_bit returned 0\n", stderr);
			goto error;
		}

		bit = smk_bs_read_1(&bs);

		if (s->channels != (bit == 1 ? 2 : 1))
			fputs("libsmacker::smk_render - ERROR: mono/stereo mismatch\n", stderr);

		bit = smk_bs_read_1(&bs);

		if (s->bitdepth != (bit == 1 ? 16 : 8))
			fputs("libsmacker::smk_render - ERROR: 8-/16-bit mismatch\n", stderr);

		/* build the trees */
		smk_huff8_build(&aud_tree[0], &bs);
		j = 1;
		k = 1;

		if (s->bitdepth == 16) {
			smk_huff8_build(&aud_tree[1], &bs);
			k = 2;
		}

		if (s->channels == 2) {
			smk_huff8_build(&aud_tree[2], &bs);
			j = 2;
			k = 2;

			if (s->bitdepth == 16) {
				smk_huff8_build(&aud_tree[3], &bs);
				k = 4;
			}
		}

		/* read initial sound level */
		if (s->channels == 2) {
			unpack = smk_bs_read_8(&bs);

			if (s->bitdepth == 16) {
				((short *)t)[1] = smk_bs_read_8(&bs);
				((short *)t)[1] |= (unpack << 8);
			} else
				((unsigned char *)t)[1] = (unsigned char)unpack;
		}

		unpack = smk_bs_read_8(&bs);

		if (s->bitdepth == 16) {
			((short *)t)[0] = smk_bs_read_8(&bs);
			((short *)t)[0] |= (unpack << 8);
		} else
			((unsigned char *)t)[0] = (unsigned char)unpack;

		/* All set: let's read some DATA! */
		while (k < s->buffer_size) {
			if (s->bitdepth == 8) {
				unpack = smk_huff8_lookup(&aud_tree[0], &bs);
				((unsigned char *)t)[j] = (char)unpack + ((unsigned char *)t)[j - s->channels];
				j ++;
				k++;
			} else {
				unpack = smk_huff8_lookup(&aud_tree[0], &bs);
				unpack2 = smk_huff8_lookup(&aud_tree[1], &bs);
				((short *)t)[j] = (short)(unpack | (unpack2 << 8)) + ((short *)t)[j - s->channels];
				j ++;
				k += 2;
			}

			if (s->channels == 2) {
				if (s->bitdepth == 8) {
					unpack = smk_huff8_lookup(&aud_tree[2], &bs);
					((unsigned char *)t)[j] = (char)unpack + ((unsigned char *)t)[j - 2];
					j ++;
					k++;
				} else {
					unpack = smk_huff8_lookup(&aud_tree[2], &bs);
					unpack2 = smk_huff8_lookup(&aud_tree[3], &bs);
					((short *)t)[j] = (short)(unpack | (unpack2 << 8)) + ((short *)t)[j - 2];
					j ++;
					k += 2;
				}
			}
		}
	}

	return 0;
error:
	return -1;
}

/* "Renders" (unpacks) the frame at cur_frame
	Preps all the image and audio pointers */
static char smk_render(smk s)
{
	unsigned long i, size;
	unsigned char * buffer = NULL, * p, track;
	/* null check */
	assert(s);

	/* Retrieve current chunk_size for this frame. */
	if (!(i = s->chunk_size[s->cur_frame])) {
		fprintf(stderr, "libsmacker::smk_render(s) - Warning: frame %lu: chunk_size is 0.\n", s->cur_frame);
		goto error;
	}

	if (s->mode == SMK_MODE_DISK) {
		/* Skip to frame in file */
		if (fseek(s->source.file.fp, s->source.file.chunk_offset[s->cur_frame], SEEK_SET)) {
			fprintf(stderr, "libsmacker::smk_render(s) - ERROR: fseek to frame %lu (offset %lu) failed.\n", s->cur_frame, s->source.file.chunk_offset[s->cur_frame]);
			perror("\tError reported was");
			goto error;
		}

		/* In disk-streaming mode: make way for our incoming chunk buffer */
		if ((buffer = malloc(i)) == NULL) {
			perror("libsmacker::smk_render() - ERROR: failed to malloc() buffer");
			return -1;
		}

		/* Read into buffer */
		if (smk_read_file(buffer, s->chunk_size[s->cur_frame], s->source.file.fp) < 0) {
			fprintf(stderr, "libsmacker::smk_render(s) - ERROR: frame %lu (offset %lu): smk_read had errors.\n", s->cur_frame, s->source.file.chunk_offset[s->cur_frame]);
			goto error;
		}
	} else {
		/* Just point buffer at the right place */
		if (!s->source.chunk_data[s->cur_frame]) {
			fprintf(stderr, "libsmacker::smk_render(s) - ERROR: frame %lu: memory chunk is a NULL pointer.\n", s->cur_frame);
			goto error;
		}

		buffer = s->source.chunk_data[s->cur_frame];
	}

	p = buffer;

	/* Palette record first */
	if (s->frame_type[s->cur_frame] & 0x01) {
		/* need at least 1 byte to process */
		if (!i) {
			fprintf(stderr, "libsmacker::smk_render(s) - ERROR: frame %lu: insufficient data for a palette rec.\n", s->cur_frame);
			goto error;
		}

		/* Byte 1 in block, times 4, tells how many
			subsequent bytes are present */
		size = 4 * (*p);

		/* If video rendering enabled, kick this off for decode. */
		if (s->video.enable)
			smk_render_palette(&(s->video), p + 1, size - 1);

		p += size;
		i -= size;
	}

	/* Unpack audio chunks */
	for (track = 0; track < 7; track ++) {
		if (s->frame_type[s->cur_frame] & (0x02 << track)) {
			/* need at least 4 byte to process */
			if (i < 4) {
				fprintf(stderr, "libsmacker::smk_render(s) - ERROR: frame %lu: insufficient data for audio[%u] rec.\n", s->cur_frame, track);
				goto error;
			}

			/* First 4 bytes in block tell how many
				subsequent bytes are present */
			size = (((unsigned int) p[3] << 24) |
					((unsigned int) p[2] << 16) |
					((unsigned int) p[1] << 8) |
					((unsigned int) p[0]));

			/* If audio rendering enabled, kick this off for decode. */
			if (s->audio[track].enable)
				smk_render_audio(&s->audio[track], p + 4, size - 4);

			p += size;
			i -= size;
		} else
			s->audio[track].buffer_size = 0;
	}

	/* Unpack video chunk */
	if (s->video.enable) {
		if (smk_render_video(&(s->video), p, i) < 0) {
			fprintf(stderr, "libsmacker::smk_render(s) - ERROR: frame %lu: failed to render video.\n", s->cur_frame);
			goto error;
		}
	}

	if (s->mode == SMK_MODE_DISK) {
		/* Remember that buffer we allocated?  Trash it */
		smk_free(buffer);
	}

	return 0;
error:

	if (s->mode == SMK_MODE_DISK) {
		/* Remember that buffer we allocated?  Trash it */
		smk_free(buffer);
	}

	return -1;
}

/* rewind to first frame and unpack */
char smk_first(smk s)
{
	/* null check */
	if (s == NULL) {
		fputs("libsmacker::smk_first() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	s->cur_frame = 0;

	if (smk_render(s) < 0) {
		fprintf(stderr, "libsmacker::smk_first(s) - Warning: frame %lu: smk_render returned errors.\n", s->cur_frame);
		return -1;
	}

	if (s->f == 1) return SMK_LAST;

	return SMK_MORE;
}

/* advance to next frame */
char smk_next(smk s)
{
	/* null check */
	if (s == NULL) {
		fputs("libsmacker::smk_next() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	if (s->cur_frame + 1 < (s->f + s->ring_frame)) {
		s->cur_frame ++;

		if (smk_render(s) < 0) {
			fprintf(stderr, "libsmacker::smk_next(s) - Warning: frame %lu: smk_render returned errors.\n", s->cur_frame);
			return -1;
		}

		if (s->cur_frame + 1 == (s->f + s->ring_frame))
			return SMK_LAST;

		return SMK_MORE;
	} else if (s->ring_frame) {
		s->cur_frame = 1;

		if (smk_render(s) < 0) {
			fprintf(stderr, "libsmacker::smk_next(s) - Warning: frame %lu: smk_render returned errors.\n", s->cur_frame);
			return -1;
		}

		if (s->cur_frame + 1 == (s->f + s->ring_frame))
			return SMK_LAST;

		return SMK_MORE;
	}

	return SMK_DONE;
}

/* seek to a keyframe in an smk */
char smk_seek_keyframe(smk s, unsigned long f)
{
	/* null check */
	if (s == NULL) {
		fputs("libsmacker::smk_seek_keyframe() - ERROR: smk is NULL\n", stderr);
		return -1;
	}

	/* rewind (or fast forward!) exactly to f */
	s->cur_frame = f;

	/* roll back to previous keyframe in stream, or 0 if no keyframes exist */
	while (s->cur_frame > 0 && !(s->keyframe[s->cur_frame]))
		s->cur_frame --;

	/* render the frame: we're ready */
	if (smk_render(s) < 0) {
		fprintf(stderr, "libsmacker::smk_seek_keyframe(s,%lu) - Warning: frame %lu: smk_render returned errors.\n", f, s->cur_frame);
		return -1;
	}

	return 0;
}
