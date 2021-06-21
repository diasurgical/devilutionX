#include <gtest/gtest.h>

#include "engine.h"

namespace devilution {

// These tests use ASSERT_EQ as the PRNG is expected to depend on state from the last call, so one failing assertion
// means the rest of the results can't be trusted.
TEST(RandomTest, RandomEngineParams)
{
	// The core diablo random number generator is an LCG with Borland constants.
	// This RNG must be available for network/save compatibility for things such as level generation.

	constexpr uint32_t multiplicand = 22695477;
	constexpr uint32_t increment = 1;

	SetRndSeed(0);

	// Starting from a seed of 0 means the multiplicand is dropped and the state advances by increment only
	AdvanceRndSeed();
	ASSERT_EQ(GetLCGEngineState(), increment) << "Increment factor is incorrect";
	AdvanceRndSeed();

	// LCGs use a formula of mult * seed + inc. Using a long form in the code to document the expected factors.
	ASSERT_EQ(GetLCGEngineState(), (multiplicand * 1) + increment) << "Multiplicand factor is incorrect";

	// C++11 defines the default seed for a LCG engine as 1, we've had 1 call since then so starting at element 2
	for (auto i = 2; i < 10000; i++)
		AdvanceRndSeed();

	uint32_t expectedState = 3495122800U;
	ASSERT_EQ(GetLCGEngineState(), expectedState)
		<< "Wrong engine state after 10000 invocations";
}

TEST(RandomTest, DefaultDistribution)
{
	// The default distribution for RNG calls is the absolute value of the generated value interpreted as a signed int
	// This relies on undefined behaviour. abs(limit<int32_t>::min()) is larger than limit<int32_t>::max(). The
	// current behaviour is this returns limit<int32_t>::min().
	SetRndSeed(1457187811); // yields -2147483648
	ASSERT_EQ(AdvanceRndSeed(), -2147483648) << "Invalid distribution";
	SetRndSeed(3604671459U); // yields 0
	ASSERT_EQ(AdvanceRndSeed(), 0) << "Invalid distribution";

	SetRndSeed(0); // yields +1
	ASSERT_EQ(AdvanceRndSeed(), 1) << "Invalid distribution";
	SetRndSeed(2914375622U); // yields -1
	ASSERT_EQ(AdvanceRndSeed(), 1) << "Invalid distribution";

	SetRndSeed(3604671460U); // yields +22695477
	ASSERT_EQ(AdvanceRndSeed(), 22695477) << "Invalid distribution";
	SetRndSeed(3604671458U); // yields -22695477
	ASSERT_EQ(AdvanceRndSeed(), 22695477) << "Invalid distribution";

	SetRndSeed(1902003768); // yields +429496729
	ASSERT_EQ(AdvanceRndSeed(), 429496729) << "Invalid distribution";
	SetRndSeed(1012371854); // yields -429496729
	ASSERT_EQ(AdvanceRndSeed(), 429496729) << "Invalid distribution";

	SetRndSeed(189776845); // yields +1212022642
	ASSERT_EQ(AdvanceRndSeed(), 1212022642) << "Invalid distribution";
	SetRndSeed(2724598777U); // yields -1212022642
	ASSERT_EQ(AdvanceRndSeed(), 1212022642) << "Invalid distribution";

	SetRndSeed(76596137); // yields +2147483646
	ASSERT_EQ(AdvanceRndSeed(), 2147483646) << "Invalid distribution";
	SetRndSeed(2837779485U); // yields -2147483646
	ASSERT_EQ(AdvanceRndSeed(), 2147483646) << "Invalid distribution";

	SetRndSeed(766891974); // yields +2147483647
	ASSERT_EQ(AdvanceRndSeed(), 2147483647) << "Invalid distribution";
	SetRndSeed(2147483648U); // yields -2147483647
	ASSERT_EQ(AdvanceRndSeed(), 2147483647) << "Invalid distribution";
}

} // namespace devilution
