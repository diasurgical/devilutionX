
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

} // namespace
} // namespace devilution
