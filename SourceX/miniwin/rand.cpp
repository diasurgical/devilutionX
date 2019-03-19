#include <cstdint>

#include "devilution.h"

namespace dvl {

static uint32_t rand_state = 0;

int rand(void)
{
	rand_state = rand_state * 214013 + 2531011;
	return (rand_state >> 16) & 0x7FFF;
}

void srand(uint32_t seed)
{
	rand_state = seed;
}

}  // namespace dvl
