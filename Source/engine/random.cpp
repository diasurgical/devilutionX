#include "engine/random.hpp"

#include <limits>
#include <random>

#include "engine/random_distribution.hpp"

namespace devilution {

namespace {
/** Borland C/C++ psuedo-random number generator needed for vanilla compatibility */
using BorlandEngine = std::linear_congruential_engine<uint32_t, 0x015A4E35, 1, 0>;

class DiabloGenerator {
public:
	using result_type = uint32_t;

	constexpr result_type min() const
	{
		return std::numeric_limits<uint32_t>::min();
	}

	constexpr result_type max() const
	{
		return std::numeric_limits<uint32_t>::max();
	}

	result_type operator()()
	{
		seed_ = engine_();
		return seed_;
	}

	void seed(result_type seed)
	{
		engine_.seed(seed);
		seed_ = seed;
	}

	result_type seed() const
	{
		return seed_;
	}

private:
	BorlandEngine engine_;
	result_type seed_; /** Current game seed, for debugging purposes */
};

DiabloGenerator generator;
} // namespace

void SetRndSeed(uint32_t seed)
{
	generator.seed(seed);
}

uint32_t GetLCGEngineState()
{
	return generator.seed();
}

int32_t AdvanceRndSeed()
{
	return AbsoluteDistribution(true)(generator);
}

int32_t GenerateRnd(int32_t v)
{
	return DiabloDistribution(v, true)(generator);
}

bool FlipCoin(unsigned frequency)
{
	// Casting here because GenerateRnd takes a signed argument when it should take and yield unsigned.
	return GenerateRnd(static_cast<int32_t>(frequency)) == 0;
}

int32_t RandomIntLessThan(int32_t v)
{
	return DiabloDistribution(v)(generator);
}

int32_t RandomIntBetween(int32_t min, int32_t max, bool halfOpen)
{
	return DiabloDistribution(min, max + (halfOpen ? 0 : 1))(generator);
}

} // namespace devilution
