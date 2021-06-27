#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "engine/random.hpp"

namespace devilution {

// These tests use ASSERT_EQ as the PRNG is expected to depend on state from the last call, so one failing assertion
// means the rest of the results can't be trusted.
TEST(RandomTest, RandomEngineParams)
{
	// The core diablo random number generator is an LCG with Borland constants.
	// This RNG must be available for network/save compatibility for things such as level generation.

	constexpr uint32_t multiplicand = 22695477;
	constexpr uint32_t increment = 1;

	vanilla::SetRndSeed(0);

	// Starting from a seed of 0 means the multiplicand is dropped and the state advances by increment only
	vanilla::AdvanceRndSeed();
	ASSERT_EQ(vanilla::GetLCGEngineState(), increment) << "Increment factor is incorrect";

	// LCGs use a formula of mult * seed + inc. Using a long form in the code to document the expected factors.
	vanilla::AdvanceRndSeed();
	ASSERT_EQ(vanilla::GetLCGEngineState(), (multiplicand * 1) + increment) << "Multiplicand factor is incorrect";

	// C++11 defines the default seed for a LCG engine as 1. The ten thousandth value is commonly used for sanity checking
	// a sequence, so as we've had one round since state 1 we need to discard another 9999 values to get to the 10000th state.
	// This loop has an off by one error, so test the 9999th value as well as 10000th
	for (auto i = 2; i < 10000; i++)
		vanilla::AdvanceRndSeed();
	uint32_t expectedState = 3495122800U;
	ASSERT_EQ(vanilla::GetLCGEngineState(), expectedState) << "Wrong engine state after 9999 invocations";
	vanilla::AdvanceRndSeed();
	expectedState = 3007658545U;
	ASSERT_EQ(vanilla::GetLCGEngineState(), expectedState) << "Wrong engine state after 10000 invocations";
}

TEST(RandomTest, AbsDistribution)
{
	// The default distribution for RNG calls is the absolute value of the generated value interpreted as a signed int
	// This relies on undefined behaviour when called on std::numeric_limits<int_t>::min(). See C17 7.22.6.1
	// The current behaviour is this returns the same value (the most negative number of the type).
	vanilla::SetRndSeed(1457187811); // yields -2147483648
	ASSERT_EQ(vanilla::AdvanceRndSeed(), std::numeric_limits<int32_t>::min()) << "Invalid distribution";
	vanilla::SetRndSeed(3604671459U); // yields 0
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 0) << "Invalid distribution";

	vanilla::SetRndSeed(0); // yields +1
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 1) << "Invalid distribution";
	vanilla::SetRndSeed(2914375622U); // yields -1
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 1) << "Invalid distribution";

	vanilla::SetRndSeed(3604671460U); // yields +22695477
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 22695477) << "Invalid distribution";
	vanilla::SetRndSeed(3604671458U); // yields -22695477
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 22695477) << "Invalid distribution";

	vanilla::SetRndSeed(1902003768); // yields +429496729
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 429496729) << "Invalid distribution";
	vanilla::SetRndSeed(1012371854); // yields -429496729
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 429496729) << "Invalid distribution";

	vanilla::SetRndSeed(189776845); // yields +1212022642
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 1212022642) << "Invalid distribution";
	vanilla::SetRndSeed(2724598777U); // yields -1212022642
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 1212022642) << "Invalid distribution";

	vanilla::SetRndSeed(76596137); // yields +2147483646
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 2147483646) << "Invalid distribution";
	vanilla::SetRndSeed(2837779485U); // yields -2147483646
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 2147483646) << "Invalid distribution";

	vanilla::SetRndSeed(766891974); // yields +2147483647
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 2147483647) << "Invalid distribution";
	vanilla::SetRndSeed(2147483648U); // yields -2147483647
	ASSERT_EQ(vanilla::AdvanceRndSeed(), 2147483647) << "Invalid distribution";
}

// The bounded distribution function used by Diablo performs a modulo operation on the result of the AbsDistribution
// tested above, with a shift operation applied before the mod when the bound is < 65535.
//
// The current implementation does not allow testing these operations independantly, hopefully this can be changed
// with confidence from these tests.
//
// The result of a mod b when a is negative was implementation defined before C++11, current behaviour is to
// preserve the sign of a. See C++17 [expr.mul] and C++03 TR1 [expr.mul]
TEST(RandomTest, ModDistributionInvalidRange)
{
	// Calling the modulo distribution with an upper bound <= 0 returns 0 without advancing the engine
	auto initialSeed = 44444444;
	vanilla::SetRndSeed(initialSeed);
	EXPECT_EQ(vanilla::GenerateRnd(0), 0) << "A distribution with an upper bound of 0 must return 0";
	EXPECT_EQ(vanilla::GetLCGEngineState(), initialSeed) << "Distribution with invalid range must not advance the engine state";
	EXPECT_EQ(vanilla::GenerateRnd(-1), 0) << "A distribution with a negative upper bound must return 0";
	EXPECT_EQ(vanilla::GetLCGEngineState(), initialSeed) << "Distribution with invalid range must not advance the engine state";
	EXPECT_EQ(vanilla::GenerateRnd(std::numeric_limits<int32_t>::min()), 0)
	    << "A distribution with a negative upper bound must return 0";
	EXPECT_EQ(vanilla::GetLCGEngineState(), initialSeed) << "Distribution with invalid range must not advance the engine state";
}

TEST(RandomTest, ShiftModDistributionSingleRange)
{
	// All results mod 1 = 0;
	auto initialSeed = ::testing::UnitTest::GetInstance()->random_seed();
	vanilla::SetRndSeed(initialSeed);
	for (auto i = 0; i < 100; i++)
		ASSERT_EQ(vanilla::GenerateRnd(1), 0) << "Interval [0, 1) must return 0";
	ASSERT_NE(vanilla::GetLCGEngineState(), initialSeed) << "Interval of 1 element must still advance the engine state";

	// Just in case -0 has a distinct representation? Shouldn't be possible but cheap to test.
	vanilla::SetRndSeed(1457187811);
	ASSERT_EQ(vanilla::GenerateRnd(1), 0) << "Interval [0, 1) must return 0 when AbsDistribution yields INT_MIN";
}

// When called with an upper bound less than 0xFFFF this distribution function discards the low 16 bits of the output
// from the default distribution by performing a shift right of 16 bits. This relies on implementation defined
// behaviour for negative numbers, the expectation is shift right uses sign carry. See C++17 [expr.shift]
TEST(RandomTest, ShiftModDistributionSignCarry)
{
	// This distribution is used when the upper bound is a value in [1, 65535)
	// Using an upper bound of 1 means the result always maps to 0, see RandomTest_ShiftModDistributionSingleRange

	// The only negative value return from AbsDistribution is -2147483648
	// A sign-preserving shift right of 16 bits gives a result of -32768.
	vanilla::SetRndSeed(1457187811); // Test mod with no division
	ASSERT_EQ(vanilla::GenerateRnd(65535 - 1), -32768) << "Distribution must map negative numbers using sign carry shifts";
	vanilla::SetRndSeed(1457187811); // Test mod when a division occurs
	ASSERT_EQ(vanilla::GenerateRnd(32768 - 1), -1) << "Distribution must map negative numbers using sign carry shifts";
}

// The Diablo LCG implementation attempts to improve the quality of generated numbers that would only use the low
// bits of the LCG output but due to applying this after taking the absolute value this introduces bias. This may
// be an inconsistency with the implementation in devilutionx, see the comment for RandomTest_ShiftModDistributionHighBits
TEST(RandomTest, ShiftModDistributionLowBits)
{
	// All the following seeds generate values less than 2^16, so after shifting they give a 0 value
	constexpr auto maxBound = 65534;

	vanilla::SetRndSeed(3604671459U); // yields 0
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(0); // yields 1
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(2914375622U); // yields -1
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(538964771); // yields 64
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(2375410851U); // yields -64
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(1229260608); // yields 65
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(1685115014); // yields -65
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(1768225379); // yields 128
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(1146150243); // yields -128
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(1480523688); // yields 7625
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(1433851934); // yields -7625
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(2382565573U); // yields 32458
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(531810049); // yields -32458
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(1625910243); // yields 32768
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(1288465379); // yields -32768
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";

	// -1 from the max bound for the next two to ensure it's not due to a mod with no remainder
	vanilla::SetRndSeed(2561524649U); // yields 65534
	ASSERT_EQ(vanilla::GenerateRnd(maxBound - 1), 0) << "Invalid distribution";
	vanilla::SetRndSeed(352850973U); // yields -65534
	ASSERT_EQ(vanilla::GenerateRnd(maxBound - 1), 0) << "Invalid distribution";

	vanilla::SetRndSeed(3251820486U); // yields 65535
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
	vanilla::SetRndSeed(3957522432U); // yields -65535
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 0) << "Invalid distribution";
}

// The highest value GenerateRnd can return is 32767 (0x7FFF). I suspect this is the Borland rand() implementation
// This may mean that devilutionx is using an incorrect version as that function was implemented as the following:
// uint seed = mult * seed + inc
// return (seed >> 16) & 0x7FFF
// i.e., no cast from unsigned to signed, no modulo when building the return value.
TEST(RandomTest, ShiftModDistributionHighBits)
{
	constexpr auto maxBound = 65534;
	vanilla::SetRndSeed(3267226595U); // yields 65536
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 1) << "Invalid distribution";
	vanilla::SetRndSeed(3942116323U); // yields -65536
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 1) << "Invalid distribution";
	vanilla::SetRndSeed(4279561187U); // yields 131072
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 2) << "Invalid distribution";
	vanilla::SetRndSeed(2929781731U); // yields -131072
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 2) << "Invalid distribution";
	vanilla::SetRndSeed(659483619); // yields 262144
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 4) << "Invalid distribution";
	vanilla::SetRndSeed(2254892003U); // yields -262144
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 4) << "Invalid distribution";
	vanilla::SetRndSeed(3604671458U); // yields 22695477
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 346) << "Invalid distribution";
	vanilla::SetRndSeed(3604671460U); // yields -22695477
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 346) << "Invalid distribution";
	vanilla::SetRndSeed(1012371854); // yields 429496729
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 6553) << "Invalid distribution";
	vanilla::SetRndSeed(1902003768); // yields -429496729
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 6553) << "Invalid distribution";
	vanilla::SetRndSeed(189776845); // yields 1212022642
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 18493) << "Invalid distribution";
	vanilla::SetRndSeed(2724598777); // yields -1212022642
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 18493) << "Invalid distribution";
	vanilla::SetRndSeed(76596137); // yields 2147483646
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 32767) << "Invalid distribution";
	vanilla::SetRndSeed(2837779485); // yields -2147483646
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 32767) << "Invalid distribution";
	vanilla::SetRndSeed(766891974); // yields 2147483647
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 32767) << "Invalid distribution";
	vanilla::SetRndSeed(2147483648); // yields -2147483647
	ASSERT_EQ(vanilla::GenerateRnd(maxBound), 32767) << "Invalid distribution";
}

TEST(RandomTest, ModDistributionSignPreserving)
{
	// This distribution is used when the upper bound is a value in [65535, 2147483647]

	// Sign preserving modulo when no division is performed cannot be tested in isolation with the current implementation.
	vanilla::SetRndSeed(1457187811);
	ASSERT_EQ(vanilla::GenerateRnd(65535), -32768) << "Distribution must map negative numbers using sign preserving modulo";
	vanilla::SetRndSeed(1457187811);
	ASSERT_EQ(vanilla::GenerateRnd(std::numeric_limits<int32_t>::max()), -1)
	    << "Distribution must map negative numbers using sign preserving modulo";
}

TEST(RandomTest, VanillaEngine)
{
	uint32_t seed = ::testing::UnitTest::GetInstance()->random_seed();
	auto engine1 = vanilla::RandomEngine(seed);
	auto engine2 = vanilla::RandomEngine(seed);

	for (auto i = 0; i < 10; i++)
		ASSERT_EQ(engine1.RandomInt(), engine2.RandomInt()) << "vanilla::RandomEngines created from the same seed must generate the same sequence";

	auto engine3 = vanilla::RandomEngine(engine1.initialState);

	engine3.Discard(10);
	ASSERT_EQ(engine3.RandomInRange(5, 24), engine1.RandomInRange(5, 24))
	    << "vanilla::RandomEngines created from the same seed must produce the same value at the same point in the sequence even if prior calls used different distributions";

	auto engine4 = vanilla::RandomEngine(seed);
	vanilla::SetRndSeed(seed);

	ASSERT_EQ(abs(static_cast<int32_t>(engine4())), vanilla::AdvanceRndSeed())
	    << "vanilla::RandomEngines must use the same base distribution as the global RNG";

	for (auto i = 0; i < 10; i++) {
		ASSERT_EQ(engine4.RandomLessThan(6435), vanilla::GenerateRnd(6435))
		    << "vanilla::RandomEngines must generate the same sequence as the global RNG from the same starting seed";
	}
}

TEST(RandomTest, V1Choices)
{
	auto choice = randomV1::RandomChoice({ -1, 1 });
	EXPECT_THAT(choice, ::testing::AnyOf(-1, 1)) << "RandomChoice must pick from the listed elements";
}

TEST(RandomTest, V1Engine)
{
	uint32_t seed = ::testing::UnitTest::GetInstance()->random_seed();
	auto engine1 = randomV1::RandomEngine(seed);
	// Can't set a specific seed on the global RNG

	{
		auto engine2 = randomV1::RandomEngine(seed);

		for (auto i = 0; i < 10; i++)
			ASSERT_EQ(engine1(), engine2()) << "randomV1::RandomEngines created from the same seed must generate the same sequence";
		// It is not guaranteed that the global RNG gives a repeatable sequence as the seed cannot be controlled.
	}

	{
		auto engine3 = randomV1::RandomEngine(engine1.initialState);

		engine3.Discard(10);
		ASSERT_EQ(engine1.RandomInRange(5, 24), engine3.RandomInRange(5, 24))
		    << "randomV1::RandomEngines created from the same seed must produce the same InRange value at the same point in the sequence";

		ASSERT_EQ(engine1.RandomLessThan(24234), engine3.RandomLessThan(24234))
		    << "randomV1::RandomEngines created from the same seed must produce the same LessThan value at the same point in the sequence";
		// It is not guaranteed that the global RNG produces the same value at any point as the seed cannot be controlled.
	}

	{
		auto engine4 = randomV1::RandomEngine(seed);

		for (auto i = 0; i < 12; i++)
			engine4.RandomLessThan(6435); // Purely to show the next call does not depend on any specific prior distributions

		for (auto i = 0; i < 5; i++) {
			ASSERT_EQ(engine1.RandomBoolean(0.5), engine4.RandomBoolean(0.5))
			    << "randomV1::RandomEngines created from the same seed must generate the same Boolean value at the same point in the sequence";
		}

		ASSERT_EQ(engine1.RandomChoice({ 5, 2, -1, -8 }), engine4.RandomChoice({ 5, 2, -1, -8 }))
		    << "randomV1::RandomEngines created from the same seed must choose the same option at the same point in the sequence";
		// It is not guaranteed that the global RNG will choose the same option as the seed cannot be controlled.
	}
}

} // namespace devilution
