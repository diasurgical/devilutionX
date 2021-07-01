/**
 * @file random.hpp
 *
 * Contains convenience functions for random number generation
 *
 * This includes specific engine/distribution functions for logic that needs to be compatible with the base game.
 */
#pragma once

#include <cstdint>
#include <initializer_list>
#include <random>

namespace devilution {

inline namespace randomV1 {
/**
 * @brief Generates a random unsigned integer value
 *
 * This is intended to be used to set the initial seed for a new RandomEngine.
 *
 * @return A random number in the range [0, 2^32)
 */
uint32_t GetRandomEngineSeed();

/**
 * @brief Generates a random number in the range [0, limit)
 * @param limit The upper bound of the desired range
 * @return A random number v where v >= 0 and v < limit
 */
int RandomLessThan(int limit);

/**
 * @brief Generates a random number in the range [min, max]
 * @param min Lower bound of the range
 * @param max Upper bound of the range
 * @return A random number v where v >= min and v <= max
 */
int RandomInRange(int min, int max);

/**
 * @brief Generates a true/false value where a true result has the given odds
 *
 * For example, RandomBoolean(0.75) will generate true about 75% of the time.
 *
 * @param chance How likely a true result should be (in the range [0, 1]). Defaults to 50%
 * @return true/false at random
 */
bool RandomBoolean(double chance = 0.5);

/**
 * @brief Returns one of a list of choices at random, where each option has equal chance of being selected.
 * @tparam T A common type that all items share
 * @param choices The list to choose from
 * @return One of the list chosen at random
 */
template <typename T>
T RandomChoice(std::initializer_list<T> choices)
{
	return *(choices.begin() + RandomLessThan(choices.size()));
}

/**
 * @brief Combines a RandomNumberEngine and helper functions using appropriate RandomNumberDistributions
 *
 * This allows generating a repeatable sequence of values from a known seed.
 */
class RandomEngine {
public:
	/**
	 * @brief Constructs a random number generator with the given initial state.
	 *
	 * The created object will then give repeatable results when the various functions are called. For example if
	 * created with a starting seed of 5, calling three Random* functions then calling RandomLessThan(23) might
	 * return 16. If you then create another object with seed 5 and call RandomLessThan(23) four times, the fourth
	 * call will return the same value (16).
	 *
	 * @param seed The starting state of the engine
	 */
	RandomEngine(uint32_t seed)
	    : initialState(seed)
	    , engine(seed)
	{
	}

	/**
	 * @brief stores a copy of the initial state of the engine, so the sequence can be repeated if desired.
	 */
	const uint32_t initialState;

	/**
	 * @brief Advances the engine and returns a random value
	 * @return A random number in the range [0, 2^32)
	 */
	uint32_t operator()()
	{
		return engine();
	}

	/**
	 * @brief Advances the engine state by the generating and discarding the given number of values
	 *
	 * Provided as a convenience for bringing engines in sync when unnecessary calls have been removed but the total
	 * number of calls needs to stay the same for future use of the RNG.
	 *
	 * @param rounds How many values to discard.
	 */
	void Discard(uint32_t rounds = 1)
	{
		engine.discard(rounds);
	}

	/**
	 * @brief Generates a random number in the range [0, limit)
	 * @param limit The upper bound of the desired range
	 * @return A random number less than the limit
	 */
	int RandomLessThan(int limit)
	{
		return RandomInRange(0, limit - 1);
	}

	/**
	 * @brief Generates a random number in the range [min, max]
	 * @param min Lower bound of the range
	 * @param max Upper bound of the range
	 * @return A random number v where v >= min and v <= max
	 */
	int RandomInRange(int min, int max);

	/**
	 * @brief Generates a true/false value where a true result has the given odds
	 *
	 * For example, RandomBoolean(0.75) will generate true about 75% of the time.
	 *
	 * @param chance How likely a true result should be (in the range [0, 1]). Defaults to 50%
	 * @return true/false at random
	 */
	bool RandomBoolean(double chance = 0.5);

	/**
	 * @brief Returns one of a list of choices at random, where each option has equal chance of being selected.
	 * @tparam T A common type that all items share
	 * @param choices The list to choose from
	 * @return One of the list chosen at random
	 */
	template <typename T>
	T RandomChoice(std::initializer_list<T> choices)
	{
		return *(choices.begin() + RandomLessThan(choices.size()));
	}

private:
	std::mt19937 engine;
};
} // namespace randomV1

namespace vanilla {
/**
 * @brief Set the state of the RandomNumberEngine used by the base game to the specific seed
 * 
 * @deprecated This sets the state of the global RNG used by vanilla code. It is provided purely to maintain
 * compatibility with Diablo/Hellfire.
 *
 * @param seed New engine state
 */
void SetRndSeed(uint32_t seed);

/**
 * @brief Returns the current state of the RandomNumberEngine used by the base game
 *
 * @deprecated This is only exposed to allow for debugging vanilla code and testing. Using this engine for new code is
 * discouraged due to the poor randomness and bugs in the implementation that need to be retained for compatibility.
 *
 * @return The current engine state
 */
uint32_t GetLCGEngineState();

/**
 * @brief Generates a random non-negative integer (most of the time) using the vanilla RNG.
 *
 * This function is only used when the base game wants to store the seed used to generate an item or level, however
 * as the returned value is transformed about 50% of values do not reflect the actual engine state. It would be more
 * appropriate to use GetLCGEngineState() in these cases but that may break compatibility with the base game.
 *
 * @deprecated This function relies on implementation defined behaviour and can occasionally return a negative value
 *
 * @return A random number in the range [0,2^31) or -2^31
 */
int32_t AdvanceRndSeed();

/**
 * @brief Advances the global engine state by the generating and discarding the given number of values
 *
 * Provided as a convenience for vanilla code that uses the global RNG which need to advance the state manually.
 *
 * @param rounds How many values to discard.
 */
void Discard(uint32_t rounds = 1);

/**
 * @brief Generates a random integer less than the given limit using the vanilla RNG
 *
 * If v is not a positive number this function returns 0 without calling the RNG.
 *
 * @deprecated Limits between 32768 and 65534 should be avoided as a bug in vanilla means this function always returns
 * a value less than 32768 for limits in that range. This function can very rarely return a negative value in the range
 * (-v, -1] due to the bug in AdvanceRndSeed()
 *
 * @see AdvanceRndSeed()
 * @param v The upper limit for the return value
 * @return A random number in the range [0, v) or rarely a negative value in (-v, -1]
 */
int32_t GenerateRnd(int32_t v);

/**
 * @brief Combines a RandomNumberEngine and helper functions using appropriate RandomNumberDistributions
 *
 * This allows generating a repeatable sequence of values from a known seed.
 *
 * @deprecated This class maintains the bugs present in the base Diablo/Hellfire RNG code so that cross compatibility
 * is preserved. Use a RandomEngine from a later version if you are writing new code.
 *
 * @see randomV1::RandomEngine
 */
class RandomEngine {
public:
	/**
	 * @brief Constructs a random number generator with the given initial state.
	 *
	 * The created object will then give repeatable results when the various functions are called. For example if
	 * created with a starting seed of 5, calling three Random* functions then calling RandomLessThan(23) might
	 * return 16. If you then create another object with seed 5 and call RandomLessThan(23) four times, the fourth
	 * call will return the same value (16).
	 *
	 * @param seed The starting state of the engine
	 */
	RandomEngine(uint32_t seed)
	    : initialState(seed)
	    , engine(seed)
	{
	}

	/**
	 * @brief stores a copy of the initial state of the engine, so the sequence can be repeated if desired.
	 */
	const uint32_t initialState;

	/**
	 * @brief Advances the engine and returns a random value
	 * @return A random number in the range [0, 2^32)
	*/
	uint32_t operator()()
	{
		return engine();
	}

	/**
	 * @brief Advances the engine state by the generating and discarding the given number of values
	 *
	 * Provided as a convenience for bringing engines in sync when unnecessary calls have been removed but the total
	 * number of calls needs to stay the same for future use of the engine.
	 *
	 * @param rounds How many values to discard.
	*/
	void Discard(uint32_t rounds = 1)
	{
		engine.discard(rounds);
	}

	/**
	 * @brief Generates a random positive number or rarely -2^31.
	 *
	 * This advances the engine state then interprets the new engine state as a signed value and calls std::abs to try
	 * discard the high bit of the result. This usually returns a positive number but may very rarely return -2^31.
	 *
	 * This function is only used when the base game wants to store the seed used to generate an item or level, however
	 * as the returned value is transformed about 50% of values do not reflect the actual engine state.
	 *
	 * @deprecated This method was only used to seed another RandomEngine. Use randomV1::GetRandomEngineSeed() instead.
	 * @return a value in the range [0, 2^31) or -2^31
	*/
	int RandomInt()
	{
		return abs(static_cast<int32_t>((*this)()));
	}

	/**
	 * @brief Generates a random number in the range [0, limit) or rarely a negative value in (-v, -1]
	 *
	 * If v is not a positive number this function returns 0 without calling the RNG.
	 *
	 * @deprecated Limits between 32768 and 65534 should be avoided as this function always returns a value less than
	 * 32768 for limits in that range. This function can very rarely return a negative value in the range (-v, -1] due
	 * to the bug in RandomInt()
	 *
	 * @see vanilla::RandomEngine::RandomInt()
	 * @param limit The upper bound of the desired range
	 * @return A random number in the range [0, limit) or rarely a negative value in (-limit, -1]
	 */
	int RandomLessThan(int limit)
	{
		if (limit <= 0)
			return 0;

		int value = RandomInt();

		if (limit < 0xFFFF)
			value >>= 16;

		return value % limit;
	}

	/**
	 * @brief Generates a random number usually in the range [min, max].
	 *
	 * This can occasionally return a result below the minimum bound due to the bug in RandomLessThan()
	 *
	 * @see vanilla::RandomEngine:RandomLessThan()
	 * @param min Lower bound of the range
	 * @param max Upper bound of the range
	 * @return A random number which is often in the range [min, max], occasionally a value in [min-max, min)
	 */
	int RandomInRange(int min, int max)
	{
		return RandomLessThan((max - min) + 1) + min;
	}

	/**
	 * @brief Generates a true/false value where a true result has one chance of occuring and false has n - 1 chances.
	 *
	 * For example, RandomBoolean(4) will generate true about 1 in 4 times (25%). This has slight bias due to the bug
	 * in RandomInt() and bias in RandomLessThan()
	 *
	 * @see vanilla::RandomEngine::RandomLessThan()
	 * @param outcomes The total number of outcomes. Defaults to 1 in 2, ~50%
	 * @return true/false at random
	 */
	bool RandomBoolean(int outcomes = 2)
	{
		return RandomLessThan(outcomes) == 0;
	}

private:
	/**
	 * @brief A LCG using Borland C++ constants.
	*/
	std::linear_congruential_engine<uint32_t, 0x015A4E35, 1, 0> engine;
};
} // namespace vanilla

} // namespace devilution
