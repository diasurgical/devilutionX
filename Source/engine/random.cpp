#include "engine/random.hpp"

#include "utils/stdcompat/abs.hpp"

namespace devilution {

/** Current game seed */
uint32_t sglGameSeed;

/**
 * Specifies the increment used in the Borland C/C++ pseudo-random.
 */
const uint32_t RndInc = 1;

/**
 * Specifies the multiplier used in the Borland C/C++ pseudo-random number generator algorithm.
 */
const uint32_t RndMult = 0x015A4E35;

/**
 * @brief Set the RNG seed
 * @param s RNG seed
 */
void SetRndSeed(uint32_t s)
{
	sglGameSeed = s;
}

/**
 * @brief Advance the internal RNG seed and return the new value
 * @return RNG seed
 */
int32_t AdvanceRndSeed()
{
	sglGameSeed = (RndMult * sglGameSeed) + RndInc;
	return GetRndSeed();
}

/**
 * @brief Get the current RNG seed
 * @return RNG seed
 */
int32_t GetRndSeed()
{
	return abs(static_cast<int32_t>(sglGameSeed));
}

uint32_t GetLCGEngineState()
{
	return sglGameSeed;
}

/**
 * @brief Main RNG function
 * @param v The upper limit for the return value
 * @return A random number from 0 to (v-1)
 */
int32_t GenerateRnd(int32_t v)
{
	if (v <= 0)
		return 0;
	if (v < 0xFFFF)
		return (AdvanceRndSeed() >> 16) % v;
	return AdvanceRndSeed() % v;
}

}
