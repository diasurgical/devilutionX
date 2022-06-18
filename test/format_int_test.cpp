
#include <gtest/gtest.h>

#include "utils/format_int.hpp"

namespace devilution {
namespace {

TEST(FormatIntegerTest, OneDigit)
{
	EXPECT_EQ(FormatInteger(1), "1");
	EXPECT_EQ(FormatInteger(-1), "-1");
}

TEST(FormatIntegerTest, TwoDigits)
{
	EXPECT_EQ(FormatInteger(12), "12");
	EXPECT_EQ(FormatInteger(-12), "-12");
}

TEST(FormatIntegerTest, ThreeDigits)
{
	EXPECT_EQ(FormatInteger(123), "123");
	EXPECT_EQ(FormatInteger(-123), "-123");
}

TEST(FormatIntegerTest, FourDigits)
{
	EXPECT_EQ(FormatInteger(1234), "1,234");
	EXPECT_EQ(FormatInteger(-1234), "-1,234");
}

TEST(FormatIntegerTest, SevenDigits)
{
	EXPECT_EQ(FormatInteger(1234567), "1,234,567");
	EXPECT_EQ(FormatInteger(-1234567), "-1,234,567");
}

} // namespace
} // namespace devilution
