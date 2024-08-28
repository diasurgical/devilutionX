
#include <gtest/gtest.h>

#include "utils/utf8.hpp"
#include "utils/unicode-bidi.hpp"

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
	for (char x = '\x7F'; static_cast<signed char>(x) >= '\x00'; x--) {
		EXPECT_FALSE(IsTrailUtf8CodeUnit(x)) << "Basic Latin and ASCII Control characters are not trail code units";
	}

	for (char x = '\x80'; x <= '\xBF'; x++) {
		EXPECT_TRUE(IsTrailUtf8CodeUnit(x)) << "Bytes in the range 0x80 to 0xBF are potentially valid trail code units";
	}

	for (char x = '\xC2'; x <= '\xF4'; x++) {
		EXPECT_FALSE(IsTrailUtf8CodeUnit(x)) << "Bytes in the range 0xC2 to 0xF4 are never valid trail code units";
	}
}

TEST(Utf8CodeUnits, InvalidCodePoints)
{
	for (char x = '\xC0'; x <= '\xC1'; x++) {
		EXPECT_FALSE(IsTrailUtf8CodeUnit(x)) << "Bytes in the range 0xC0 to oxC1 are not trail code units";
	}

	for (char x = '\xFF'; x >= '\xF5'; x--) {
		EXPECT_FALSE(IsTrailUtf8CodeUnit(x)) << "Bytes in the range 0xF5 to 0xFF are not trail code units";
	}
}

TEST(Utf8CodeUnits, BasicLatin)
{
	for (char x = '\x00'; x < '\x20'; x++) {
		EXPECT_FALSE(IsBasicLatin(x)) << "ASCII Control characters are not Basic Latin symbols";
	}

	for (char x = '\x20'; x <= '\x7E'; x++) {
		EXPECT_TRUE(IsBasicLatin(x)) << "Basic Latin symbols are denoted by the range 0x20 to 0x7E inclusive";
	}

	EXPECT_FALSE(IsBasicLatin('\x7F')) << "ASCII Control character DEL is not a Basic Latin symbol";

	// Tests '\xFF' separately to avoid infinite loop on platforms with unsigned char.
	for (char x = '\x80'; x < '\xFF'; x++) {
		EXPECT_FALSE(IsBasicLatin(x)) << "Multibyte Utf8 code units are not Basic Latin symbols";
	}
	EXPECT_FALSE(IsBasicLatin('\xFF')) << "Multibyte Utf8 code units are not Basic Latin symbols";
}

TEST(ConvertUtf8ToUtf32Test, EmptyString)
{
	std::string_view input;
	auto result = ConvertUtf8ToUtf32(input);
	EXPECT_TRUE(result.empty());
}

TEST(ConvertUtf8ToUtf32Test, BasicLatin)
{
	std::string_view input = "Hello, world!";
	auto result = ConvertUtf8ToUtf32(input);
	EXPECT_EQ(result, U"Hello, world!");
}

TEST(ConvertUtf8ToUtf32Test, MultibyteUtf8)
{
	std::string_view input = "こんにちは、世界！";
	auto result = ConvertUtf8ToUtf32(input);
	EXPECT_EQ(result, U"こんにちは、世界！");
}

TEST(ConvertUtf32ToUtf8Test, EmptyString)
{
	std::u32string_view input;
	auto result = ConvertUtf32ToUtf8(input);
	EXPECT_TRUE(result.empty());
}

TEST(ConvertUtf32ToUtf8Test, BasicLatin)
{
	std::u32string_view input = U"Hello, world!";
	auto result = ConvertUtf32ToUtf8(input);
	EXPECT_EQ(result, "Hello, world!");
}

TEST(ConvertUtf32ToUtf8Test, MultibyteUtf8)
{
	std::u32string_view input = U"こんにちは、世界！";
	auto result = ConvertUtf32ToUtf8(input);
	EXPECT_EQ(result, "こんにちは、世界！");
}

TEST(ConvertUtf32ToUtf8Test, Inverse)
{
	std::u32string_view input = U"こんにちは、世界！";
	auto utf8 = ConvertUtf32ToUtf8(input);
	auto utf32 = ConvertUtf8ToUtf32(utf8);
	EXPECT_EQ(input, utf32);
}

TEST(ConvertUtf32ToUtf8Test, InverseInverse)
{
	std::string_view input = "こんにちは、世界！";
	auto utf32 = ConvertUtf8ToUtf32(input);
	auto utf8 = ConvertUtf32ToUtf8(utf32);
	EXPECT_EQ(input, utf8);
}

TEST(ConvertLogicalToVisualTest, EmptyString)
{
	std::u32string_view input;
	auto result = ConvertLogicalToVisual(input);
	EXPECT_TRUE(result.empty());
}

TEST(ConvertLogicalToVisualTest, BasicLatin)
{
	std::u32string_view input = U"Hello, world!";
	auto result = ConvertLogicalToVisual(input);
	EXPECT_EQ(result, U"Hello, world!");
}

TEST(ConvertLogicalToVisualTest, Hebrew)
{
	std::u32string_view input = U"שלום, עולם!";
	auto result = ConvertLogicalToVisual(input);
	EXPECT_EQ(result, U"!םלוע ,םולש");
}

// TEST(ConvertLogicalToVisualTest, MultiLineString)
// {
// 	std::u32string_view input = U"שלום\nכיתה א!";
// 	auto result = ConvertLogicalToVisual(input);
// 	EXPECT_EQ(ConvertUtf32ToUtf8(result), "םולש\n!א התיכ");
// }

} // namespace
} // namespace devilution
