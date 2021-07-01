#include "engine/random.hpp"

#include "utils/stdcompat/abs.hpp"

namespace devilution {

inline namespace randomV1 {
/**
 * @brief Randomly initialise the engine used for non-instanced RNG calls
*/
std::random_device rd;

/**
 * @brief The random number generator used for non-instanced calls.
 *
 * Callers who wish to use repeatable sequences are expected to create their own RandomEngine object, to motivate that
 * the seed of this engine instance is deliberately not exposed and can not be set manually.
*/
RandomEngine engine(rd());

uint32_t GetRandomEngineSeed()
{
	return engine();
}

int RandomLessThan(int limit)
{
	return engine.RandomLessThan(limit);
}

int RandomInRange(int min, int max)
{
	return engine.RandomInRange(min, max);
}

bool RandomBoolean(double chance)
{
	return engine.RandomBoolean(chance);
}

int RandomEngine::RandomInRange(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);

	return dist(engine);
}

bool RandomEngine::RandomBoolean(double chance)
{
	std::bernoulli_distribution dist(chance);

	return dist(engine);
}
} // namespace randomV1

namespace vanilla {
/** Current game seed */
uint32_t sglGameSeed;

/**
 * Specifies the increment used in the Borland C/C++ pseudo-random number generator algorithm.
 */
const uint32_t RndInc = 1;

/**
 * Specifies the multiplier used in the Borland C/C++ pseudo-random number generator algorithm.
 */
const uint32_t RndMult = 0x015A4E35;

void SetRndSeed(uint32_t seed)
{
	sglGameSeed = seed;
}

uint32_t GetLCGEngineState()
{
	return sglGameSeed;
}

int32_t GetRndSeed()
{
	return abs(static_cast<int32_t>(sglGameSeed));
}

int32_t AdvanceRndSeed()
{
	sglGameSeed = (RndMult * sglGameSeed) + RndInc;
	return GetRndSeed();
}

void Discard(uint32_t rounds)
{
	for (uint32_t i = 0; i < rounds; i++)
		AdvanceRndSeed();
}

int32_t GenerateRnd(int32_t v)
{
	if (v <= 0)
		return 0;
	if (v < 0xFFFF)
		return (AdvanceRndSeed() >> 16) % v;
	return AdvanceRndSeed() % v;
}

} // namespace vanilla

} // namespace devilution
