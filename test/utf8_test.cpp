
#include <gtest/gtest.h>

#include "utils/utf8.hpp"

namespace devilution {
namespace {

TEST(AppendUtf8Test, OneByteCodePoint)
{
	std::string s = "x";
	AppendUtf8(U'a', s);
	EXPECT_EQ(s, "xa");
}

TEST(AppendUtf8Test, TwoByteCodePoint)
{
	std::string s = "x";
	AppendUtf8(U'Á', s);
	EXPECT_EQ(s, "xÁ");
}

TEST(AppendUtf8Test, ThreeByteCodePoint)
{
	std::string s;
	AppendUtf8(U'€', s);
	EXPECT_EQ(s, "€");
}

TEST(AppendUtf8Test, FourByteCodePoint)
{
	std::string s;
	AppendUtf8(U'あ', s);
	EXPECT_EQ(s, "あ");
}

TEST(Utf8CodeUnits, ValidCodePoints)
{
	// Working backwards on this loop to avoid triggering signed integer overflow on platforms where char has an
	// underlying type of signed char
	for (char x = '\x7F'; x >= '\x00' && x <= '\x7F'; x--) {
		EXPECT_FALSE(IsTrailUtf8CodeUnit(x)) << "Basic Latin and ASCII Control characters are not trail code units";
	}

	for (char x = '\x80'; x >= '\x80' && x <= '\xBF'; x++) {
		EXPECT_TRUE(IsTrailUtf8CodeUnit(x)) << "Bytes in the range 0x80 to 0xBF are potentially valid trail code units";
	}

	for (char x = '\xC2'; x >= '\xC2' && x <= '\xF4'; x++) {
		EXPECT_FALSE(IsTrailUtf8CodeUnit(x)) << "Bytes in the range 0xC2 to 0xF4 are never valid trail code units";
	}
}

TEST(Utf8CodeUnits, InvalidCodePoints)
{
	for (char x = '\xC0'; x >= '\xC0' && x <= '\xC1'; x++) {
		EXPECT_FALSE(IsTrailUtf8CodeUnit(x)) << "Bytes in the range 0xC0 to oxC1 are not trail code units";
	}

	for (char x = '\xF5'; x >= '\xF5' && x <= '\xFF'; x++) {
		EXPECT_FALSE(IsTrailUtf8CodeUnit(x)) << "Bytes in the range 0xF5 to 0xFF are not trail code units";
	}
}

} // namespace
} // namespace devilution
