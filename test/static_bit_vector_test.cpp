#include <gtest/gtest.h>

#include "utils/static_bit_vector.hpp"

namespace devilution {
namespace {

TEST(StaticBitVectorTest, SetSingleTest)
{
	StaticBitVector<500> v(100);
	EXPECT_FALSE(v.test(97));
	v.set(97);
	EXPECT_TRUE(v.test(97));
}

TEST(StaticBitVectorTest, SetRangeTest)
{
	StaticBitVector<500> v(100);
	EXPECT_FALSE(v.test(97));
	v.set(50, 50);
	for (size_t i = 0; i < 50; ++i) {
		EXPECT_FALSE(v.test(i)) << i;
	}
	for (size_t i = 50; i < 100; ++i) {
		EXPECT_TRUE(v.test(i)) << i;
	}
}

} // namespace
} // namespace devilution
