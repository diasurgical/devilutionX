/**
 * @file random.hpp
 *
 * Contains convenience functions for random number generation
 *
 * This includes specific engine/distribution functions for logic that needs to be compatible with the base game.
 */
#pragma once

#include <algorithm>
#include <cstdint>
#include <initializer_list>
#include <random>

namespace devilution {

class DiabloGenerator {
private:
	/** Borland C/C++ psuedo-random number generator needed for vanilla compatibility */
	std::linear_congruential_engine<uint32_t, 0x015A4E35, 1, 0> lcg;

public:
	/**
	 * @brief Set the state of the RandomNumberEngine used by the base game to the specific seed
	 * @param seed New engine state
	 */
	DiabloGenerator(uint32_t seed)
	{
		lcg.seed(seed);
	}

	/**
	 * @brief Advance the global RandomNumberEngine state by the specified number of rounds
	 *
	 * Only used to maintain vanilla compatibility until logic requiring reproducable random number generation is isolated.
	 * @param count How many values to discard
	 */
	void discardRandomValues(unsigned count)
	{
		lcg.discard(count);
	}

	/**
	 * @brief Generates a random non-negative integer (most of the time) using the vanilla RNG
	 *
	 * This advances the engine state then interprets the new engine state as a signed value and calls std::abs to try
	 * discard the high bit of the result. This usually returns a positive number but may very rarely return -2^31.
	 *
	 * This function is only used when the base game wants to store the seed used to generate an item or level, however
	 * as the returned value is transformed about 50% of values do not reflect the actual engine state. It would be more
	 * appropriate to use GetLCGEngineState() in these cases but that may break compatibility with the base game.
	 *
	 * @return A random number in the range [0,2^31) or -2^31
	 */
	int32_t advanceRndSeed()
	{
		const int32_t seed = static_cast<int32_t>(lcg());
		// since abs(INT_MIN) is undefined behavior, handle this value specially
		return seed == std::numeric_limits<int32_t>::min() ? std::numeric_limits<int32_t>::min() : std::abs(seed);
	}

	/**
	 * @brief Generates a random integer less than the given limit using the vanilla RNG
	 *
	 * If v is not a positive number this function returns 0 without calling the RNG.
	 *
	 * Limits between 32768 and 65534 should be avoided as a bug in vanilla means this function always returns a value
	 * less than 32768 for limits in that range.
	 *
	 * This can very rarely return a negative value in the range (-v, -1] due to the bug in AdvanceRndSeed()
	 *
	 * @see AdvanceRndSeed()
	 * @param v The upper limit for the return value
	 * @return A random number in the range [0, v) or rarely a negative value in (-v, -1]
	 */
	int32_t generateRnd(int32_t v)
	{
		if (v <= 0)
			return 0;
		if (v <= 0x7FFF) // use the high bits to correct for LCG bias
			return (advanceRndSeed() >> 16) % v;
		return advanceRndSeed() % v;
	}

	/**
	 * @brief Generates a random boolean value using the vanilla RNG
	 *
	 * This function returns true 1 in `frequency` of the time, otherwise false. For example the default frequency of 2
	 * represents a 50/50 chance.
	 *
	 * @param frequency odds of returning a true value
	 * @return A random boolean value
	 */
	bool flipCoin(unsigned frequency)
	{
		// Casting here because GenerateRnd takes a signed argument when it should take and yield unsigned.
		return generateRnd(static_cast<int32_t>(frequency)) == 0;
	}

	/**
	 * @brief Picks one of the elements in the list randomly.
	 *
	 * @param values The values to pick from
	 * @return A random value from the 'values' list.
	 */
	template <typename T>
	const T pickRandomlyAmong(const std::initializer_list<T> &values)
	{
		const auto index { std::max<int32_t>(generateRnd(static_cast<int32_t>(values.size())), 0) };

		return *(values.begin() + index);
	}

	/**
	 * @brief Generates a random non-negative integer
	 *
	 * Effectively the same as GenerateRnd but will never return a negative value
	 * @param v upper limit for the return value
	 * @return a value between 0 and v-1 inclusive, i.e. the range [0, v)
	 */
	inline int32_t randomIntLessThan(int32_t v)
	{
		return std::max<int32_t>(generateRnd(v), 0);
	}

	/**
	 * @brief Randomly chooses a value somewhere within the given range
	 * @param min lower limit, minumum possible value
	 * @param max upper limit, either the maximum possible value for a closed range (the default behaviour) or one greater than the maximum value for a half-open range
	 * @param halfOpen whether to use the limits as a half-open range or not
	 * @return a randomly selected integer
	 */
	inline int32_t randomIntBetween(int32_t min, int32_t max, bool halfOpen = false)
	{
		return randomIntLessThan(max - min + (halfOpen ? 0 : 1)) + min;
	}
};

/** Adapted from https://prng.di.unimi.it/xoshiro128plusplus.c */
class xoshiro128plusplus {
private:
	static uint32_t rotl(const uint32_t x, int k)
	{
		return (x << k) | (x >> (32 - k));
	}

	uint32_t s[4];

public:
	void seed(uint64_t value)
	{
		uint64_t rand1 = next_split(value);
		uint64_t rand2 = next_split(rand1);
		s[0] = static_cast<uint32_t>(rand1);
		s[1] = static_cast<uint32_t>(rand2);
		s[2] = static_cast<uint32_t>(rand1 >> 32);
		s[3] = static_cast<uint32_t>(rand2 >> 32);
	}

	void seed(uint32_t value)
	{
		s[0] = next_split(value);
		s[1] = next_split(s[0]);
		s[2] = next_split(s[1]);
		s[3] = next_split(s[2]);
	}

	uint32_t next()
	{
		const uint32_t result = rotl(s[0] + s[3], 7) + s[0];

		const uint32_t t = s[1] << 9;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];

		s[2] ^= t;

		s[3] = rotl(s[3], 11);

		return result;
	}

private:
	/** Adapted from https://prng.di.unimi.it/splitmix64.c */
	static uint64_t next_split(uint64_t x)
	{
		uint64_t z = (x += 0x9e3779b97f4a7c15);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
		z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
		return z ^ (z >> 31);
	}

	/** Adapted from https://prng.di.unimi.it/splitmix64.c using fmix32 from MurmurHash3 */
	static uint32_t next_split(uint32_t x)
	{
		uint32_t z = (x += 0x9e3779b9);
		z = (z ^ (z >> 16)) * 0x85ebca6b;
		z = (z ^ (z >> 13)) * 0xc2b2ae35;
		return z ^ (z >> 16);
	}
};

/**
 * @brief Advances the global seed generator state and returns the new value
 */
void ResetSeedGenerator(uint64_t seed);

/**
 * @brief Advances the global seed generator state and returns the new value
 */
uint32_t GenerateSeed();

/**
 * @brief Set the state of the RandomNumberEngine used by the base game to the specific seed
 * @param seed New engine state
 */
void SetRndSeed(uint32_t seed);

/**
 * @brief Returns the current state of the RandomNumberEngine used by the base game
 *
 * This is only exposed to allow for debugging vanilla code and testing. Using this engine for new code is discouraged
 * due to the poor randomness and bugs in the implementation that need to be retained for compatibility.
 *
 * @return The current engine state
 */
uint32_t GetLCGEngineState();

/**
 * @brief Advance the global RandomNumberEngine state by the specified number of rounds
 *
 * Only used to maintain vanilla compatibility until logic requiring reproducable random number generation is isolated.
 * @param count How many values to discard
 */
void DiscardRandomValues(unsigned count);

/**
 * @brief Advances the global RandomNumberEngine state and returns the new value
 */
uint32_t GenerateRandomNumber();

/**
 * @brief Generates a random non-negative integer (most of the time) using the vanilla RNG
 *
 * This advances the engine state then interprets the new engine state as a signed value and calls std::abs to try
 * discard the high bit of the result. This usually returns a positive number but may very rarely return -2^31.
 *
 * This function is only used when the base game wants to store the seed used to generate an item or level, however
 * as the returned value is transformed about 50% of values do not reflect the actual engine state. It would be more
 * appropriate to use GetLCGEngineState() in these cases but that may break compatibility with the base game.
 *
 * @return A random number in the range [0,2^31) or -2^31
 */
[[nodiscard]] int32_t AdvanceRndSeed();

/**
 * @brief Generates a random integer less than the given limit using the vanilla RNG
 *
 * If v is not a positive number this function returns 0 without calling the RNG.
 *
 * Limits between 32768 and 65534 should be avoided as a bug in vanilla means this function always returns a value
 * less than 32768 for limits in that range.
 *
 * This can very rarely return a negative value in the range (-v, -1] due to the bug in AdvanceRndSeed()
 *
 * @see AdvanceRndSeed()
 * @param v The upper limit for the return value
 * @return A random number in the range [0, v) or rarely a negative value in (-v, -1]
 */
int32_t GenerateRnd(int32_t v);

/**
 * @brief Generates a random boolean value using the vanilla RNG
 *
 * This function returns true 1 in `frequency` of the time, otherwise false. For example the default frequency of 2
 * represents a 50/50 chance.
 *
 * @param frequency odds of returning a true value
 * @return A random boolean value
 */
bool FlipCoin(unsigned frequency = 2);

/**
 * @brief Picks one of the elements in the list randomly.
 *
 * @param values The values to pick from
 * @return A random value from the 'values' list.
 */
template <typename T>
const T PickRandomlyAmong(const std::initializer_list<T> &values)
{
	const auto index { std::max<int32_t>(GenerateRnd(static_cast<int32_t>(values.size())), 0) };

	return *(values.begin() + index);
}

/**
 * @brief Generates a random non-negative integer
 *
 * Effectively the same as GenerateRnd but will never return a negative value
 * @param v upper limit for the return value
 * @return a value between 0 and v-1 inclusive, i.e. the range [0, v)
 */
inline int32_t RandomIntLessThan(int32_t v)
{
	return std::max<int32_t>(GenerateRnd(v), 0);
}

/**
 * @brief Randomly chooses a value somewhere within the given range
 * @param min lower limit, minumum possible value
 * @param max upper limit, either the maximum possible value for a closed range (the default behaviour) or one greater than the maximum value for a half-open range
 * @param halfOpen whether to use the limits as a half-open range or not
 * @return a randomly selected integer
 */
inline int32_t RandomIntBetween(int32_t min, int32_t max, bool halfOpen = false)
{
	return RandomIntLessThan(max - min + (halfOpen ? 0 : 1)) + min;
}

} // namespace devilution
