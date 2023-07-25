#include <gtest/gtest.h>

#include "utils/stdcompat/charconv.hpp"

namespace devilution {

TEST(StdCompatTest, FromChars_Unsigned)
{
	unsigned value = 0;

	const char *empty = "";
	// zero length character sequence
	auto result = from_chars(empty, empty, value);
	EXPECT_EQ(result.ec, std::errc::invalid_argument);

	// sequence of only a null character
	result = from_chars(empty, empty + sizeof(empty), value);
	EXPECT_EQ(result.ec, std::errc::invalid_argument);

	const char digits[] = "0123";

#if __cplusplus < 201703L
	// the devilution::from_chars shim only supports base10
	result = from_chars(digits, digits + sizeof(digits), value, 12);
	EXPECT_EQ(result.ec, std::errc::invalid_argument);
#endif

	// valid value, pattern stops at the null character
	result = from_chars(digits, digits + sizeof(digits), value);
	EXPECT_EQ(value, 123) << "Value should be parsed as a base 10 number";

	const char negativeDigits[] = "-0123";
	value = 0;
	// invalid value for unsigned types, pattern stops at the minus character
	result = from_chars(negativeDigits, negativeDigits + sizeof(negativeDigits), value);
	EXPECT_EQ(result.ec, std::errc::invalid_argument);
	EXPECT_EQ(result.ptr, negativeDigits) << "from_chars should stop at the minus sign when using unsigned types";
	EXPECT_EQ(value, 0) << "Value should not be modified";

	const char tooLargeDigits[] = "99999999999999999999999999";
	value = 444;
	result = from_chars(tooLargeDigits, tooLargeDigits + sizeof(tooLargeDigits), value);
	EXPECT_EQ(result.ec, std::errc::result_out_of_range) << "from_chars should indicate the sequence cannot be represented in the given type";
	EXPECT_EQ(result.ptr, tooLargeDigits + sizeof(tooLargeDigits) - 1) << "from_chars should consume all digit characters even if the result is out of representable range";
	EXPECT_EQ(value, 444) << "Value should not be modified if the result is out of range";
}

TEST(StdCompatTest, FromChars_Signed)
{
	int value = 0;

	const char empty[] = "";
	// zero length character sequence
	auto result = from_chars(empty, empty, value);
	EXPECT_EQ(result.ec, std::errc::invalid_argument);

	// sequence of only a null character
	result = from_chars(empty, empty + sizeof(empty), value);
	EXPECT_EQ(result.ec, std::errc::invalid_argument);

	const char digits[] = "0123";

#if __cplusplus < 201703L
	// unsupported base
	result = from_chars(digits, digits + sizeof(digits), value, 12);
	EXPECT_EQ(result.ec, std::errc::invalid_argument);
#endif

	// valid value, pattern stops at the null character
	result = from_chars(digits, digits + sizeof(digits), value);
	EXPECT_EQ(value, 123) << "Value should be parsed as a base 10 number";

	const char negativeDigits[] = "-0123";
	value = 0;
	// valid value, pattern stops at the null character
	result = from_chars(negativeDigits, negativeDigits + sizeof(negativeDigits), value);
	EXPECT_EQ(value, -123) << "Value should be parsed as a negative base 10 number";

	const char tooLargeDigits[] = "99999999999999999999999999";
	value = 444;
	result = from_chars(tooLargeDigits, tooLargeDigits + sizeof(tooLargeDigits), value);
	EXPECT_EQ(result.ec, std::errc::result_out_of_range) << "from_chars should indicate the sequence cannot be represented in the given type";
	EXPECT_EQ(result.ptr, tooLargeDigits + sizeof(tooLargeDigits) - 1) << "from_chars should consume all digit characters even if the result is out of representable range";
	EXPECT_EQ(value, 444) << "Value should not be modified if the result is out of range";

	const char tooLargeNegativeDigits[] = "-99999999999999999999999999";
	value = 444;
	result = from_chars(tooLargeNegativeDigits, tooLargeNegativeDigits + sizeof(tooLargeNegativeDigits), value);
	EXPECT_EQ(result.ec, std::errc::result_out_of_range);
	EXPECT_EQ(result.ptr, tooLargeNegativeDigits + sizeof(tooLargeNegativeDigits) - 1) << "from_chars should consume all digit characters even if the result is out of representable range";
	EXPECT_EQ(value, 444) << "Value should not be modified if the result is out of range";
}
} // namespace devilution
