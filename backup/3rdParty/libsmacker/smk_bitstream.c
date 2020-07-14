/**
	libsmacker - A C library for decoding .smk Smacker Video files
	Copyright (C) 2012-2017 Greg Kennedy

	See smacker.h for more information.

	smk_bitstream.c
		Implements a bitstream structure, which can extract and
		return a bit at a time from a raw block of bytes.
*/

#include "smk_bitstream.h"

/* malloc and friends */
#include "smk_malloc.h"

/*
	Bitstream structure
	Pointer to raw block of data and a size limit.
	Maintains internal pointers to byte_num and bit_number.
*/
struct smk_bit_t
{
	const unsigned char* buffer;
	unsigned long size;

	unsigned long byte_num;
	char bit_num;
};

/* BITSTREAM Functions */
struct smk_bit_t* smk_bs_init(const unsigned char* b, const unsigned long size)
{
	struct smk_bit_t* ret = NULL;

	/* sanity check */
	smk_assert(b);

	/* allocate a bitstream struct */
	smk_malloc(ret, sizeof(struct smk_bit_t));

	/* set up the pointer to bitstream, and the size counter */
	ret->buffer = b;
	ret->size = size;

	/* point to initial byte: note, smk_malloc already sets these to 0 */
	/* ret->byte_num = 0;
	ret->bit_num = 0; */

	/* return ret or NULL if error : ) */
error:
	return ret;
}

/* Reads a bit
	Returns -1 if error encountered */
char _smk_bs_read_1(struct smk_bit_t* bs)
{
	unsigned char ret = -1;

	/* sanity check */
	smk_assert(bs);

	/* don't die when running out of bits, but signal */
	if (bs->byte_num >= bs->size)
	{
		fprintf(stderr, "libsmacker::_smk_bs_read_1(bs): ERROR: bitstream (length=%lu) exhausted.\n", bs->size);
		goto error;
	}

	/* get next bit and return */
	ret = (((bs->buffer[bs->byte_num]) & (1 << bs->bit_num)) != 0);

	/* advance to next bit */
	bs->bit_num ++;

	/* Out of bits in this byte: next! */
	if (bs->bit_num > 7)
	{
		bs->byte_num ++;
		bs->bit_num = 0;
	}

	/* return ret, or (default) -1 if error */
error:
	return ret;
}

/* Reads a byte
	Returns -1 if error. */
short _smk_bs_read_8(struct smk_bit_t* bs)
{
	unsigned char ret = -1;

	/* sanity check */
	smk_assert(bs);

	/* don't die when running out of bits, but signal */
	if (bs->byte_num + (bs->bit_num > 0) >= bs->size)
	{
		fprintf(stderr, "libsmacker::_smk_bs_read_8(bs): ERROR: bitstream (length=%lu) exhausted.\n", bs->size);
		goto error;
	}

	if (bs->bit_num)
	{
		/* unaligned read */
		ret = bs->buffer[bs->byte_num] >> bs->bit_num;
		bs->byte_num ++;
		ret |= (bs->buffer[bs->byte_num] << (8 - bs->bit_num));
	} else {
		/* aligned read */
		ret = bs->buffer[bs->byte_num ++];
	}

	/* return ret, or (default) -1 if error */
error:
	return ret;
}
