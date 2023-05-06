#include "engine/random.hpp"

#include <limits>
#include <random>

#include "utils/stdcompat/abs.hpp"

namespace devilution {

/** Current game seed */
uint32_t sglGameSeed;

/** Borland C/C++ psuedo-random number generator needed for vanilla compatibility */
std::linear_congruential_engine<uint32_t, 0x015A4E35, 1, 0> diabloGenerator;

void SetRndSeed(uint32_t seed)
{
	diabloGenerator.seed(seed);
	sglGameSeed = seed;
}

uint32_t GetLCGEngineState()
{
	return sglGameSeed;
}

void DiscardRandomValues(unsigned count)
{
	while (count != 0) {
		GenerateSeed();
		count--;
	}
}

uint32_t GenerateSeed()
{
	sglGameSeed = diabloGenerator();
	return sglGameSeed;
}

int32_t AdvanceRndSeed()
{
	const int32_t seed = static_cast<int32_t>(GenerateSeed());
	// since abs(INT_MIN) is undefined behavior, handle this value specially
	return seed == std::numeric_limits<int32_t>::min() ? std::numeric_limits<int32_t>::min() : abs(seed);
}

int32_t GenerateRnd(int32_t v)
{
	if (v <= 0)
		return 0;
	if (v <= 0x7FFF) // use the high bits to correct for LCG bias
		return (AdvanceRndSeed() >> 16) % v;
	return AdvanceRndSeed() % v;
}

bool FlipCoin(unsigned frequency)
{
	// Casting here because GenerateRnd takes a signed argument when it should take and yield unsigned.
	return GenerateRnd(static_cast<int32_t>(frequency)) == 0;
}

} // namespace devilution
