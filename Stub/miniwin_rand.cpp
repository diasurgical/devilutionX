/** @file
 *
 * An implementation of the MSVCRT random number generator for codec_encode().
 */
#include "miniwin.h"

#include "stubs.h"

unsigned int rand_state = 1;

int rand_miniwin(void)
{
	rand_state = rand_state * 214013 + 2531011;
	return (rand_state >> 16) & 0x7FFF;
}

void srand_miniwin(unsigned int seed)
{
	rand_state = seed;
}
