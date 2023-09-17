#pragma once

#include <algorithm>
#include <charconv>
#include <string_view>
#include <system_error>

#include <expected.hpp>

namespace devilution {

enum class ParseIntError {
	ParseError = 1,
	OutOfRange
};

template <typename IntT>
using ParseIntResult = tl::expected<IntT, ParseIntError>;

template <typename IntT>
ParseIntResult<IntT> ParseInt(
    std::string_view str, IntT min = std::numeric_limits<IntT>::min(),
    IntT max = std::numeric_limits<IntT>::max(), const char **endOfParse = nullptr)
{
	IntT value;
	const std::from_chars_result result = std::from_chars(str.data(), str.data() + str.size(), value);
	if (endOfParse != nullptr) {
		*endOfParse = result.ptr;
	}
	if (result.ec == std::errc::invalid_argument)
		return tl::unexpected(ParseIntError::ParseError);
	if (result.ec == std::errc::result_out_of_range || value < min || value > max)
		return tl::unexpected(ParseIntError::OutOfRange);
	if (result.ec != std::errc())
		return tl::unexpected(ParseIntError::ParseError);
	return value;
}

inline uint8_t ParseFixed6Fraction(std::string_view str, const char **endOfParse = nullptr)
{
	unsigned numDigits = 0;
	uint32_t decimalFraction = 0;

	// Read at most 7 digits, at that threshold we're able to determine an exact rounding for 6 bit fixed point numbers
	while (!str.empty() && numDigits < 7) {
		if (str[0] < '0' || str[0] > '9') {
			break;
		}
		decimalFraction = decimalFraction * 10 + str[0] - '0';
		++numDigits;
		str.remove_prefix(1);
	}
	if (endOfParse != nullptr) {
		// to mimic the behaviour of std::from_chars consume all remaining digits in case the value was overly precise.
		*endOfParse = std::find_if_not(str.data(), str.data() + str.size(), [](char character) { return character >= '0' && character <= '9'; });
	}
	// to ensure rounding to nearest we normalise all values to 7 decimal places
	while (numDigits < 7) {
		decimalFraction *= 10;
		++numDigits;
	}
	// we add half the step between representable values to use integer truncation as a substitute for rounding to nearest.
	return (decimalFraction + 78125) / 156250;
}

template <typename IntT>
ParseIntResult<IntT> ParseFixed6(std::string_view str, const char **endOfParse = nullptr)
{
	if (endOfParse != nullptr) {
		// To allow for early returns we set the end pointer to the start of the string, which is the common case for errors.
		*endOfParse = str.data();
	}

	if (str.empty()) {
		return tl::unexpected { ParseIntError::ParseError };
	}

	constexpr IntT minIntegerValue = std::numeric_limits<IntT>::min() >> 6;
	constexpr IntT maxIntegerValue = std::numeric_limits<IntT>::max() >> 6;

	const char *currentChar; // will be set by the call to parseInt
	ParseIntResult<IntT> integerParseResult = ParseInt(str, minIntegerValue, maxIntegerValue, &currentChar);

	bool isNegative = std::is_signed_v<IntT> && str[0] == '-';
	bool haveDigits = integerParseResult.has_value() || integerParseResult.error() == ParseIntError::OutOfRange;
	if (haveDigits) {
		str.remove_prefix(static_cast<size_t>(std::distance(str.data(), currentChar)));
	} else if (isNegative) {
		str.remove_prefix(1);
	}

	// if the string has no leading digits we still need to try parse the fraction part
	uint8_t fractionPart = 0;
	if (!str.empty() && str[0] == '.') {
		// got a fractional part to read too
		str.remove_prefix(1); // skip past the decimal point

		fractionPart = ParseFixed6Fraction(str, &currentChar);
		haveDigits = haveDigits || str.data() != currentChar;
	}

	if (!haveDigits) {
		// early return in case we got a string like "-.abc", don't want to set the end pointer in this case
		return tl::unexpected { ParseIntError::ParseError };
	}

	if (endOfParse != nullptr) {
		*endOfParse = currentChar;
	}

	if (!integerParseResult.has_value() && integerParseResult.error() == ParseIntError::OutOfRange) {
		// if the integer parsing gave us an out of range value then we've done a bit of unnecessary
		//  work parsing the fraction part, but it saves duplicating code.
		return integerParseResult;
	}
	// integerParseResult could be a ParseError at this point because of a string like ".123" or "-.1"
	//  so we need to default to 0 (and use the result of the minus sign check when it's relevant)
	IntT integerPart = integerParseResult.value_or(0);

	// rounding could give us a value of 64 for the fraction part (e.g. 0.993 rounds to 1.0) so we need to ensure this doesn't overflow
	if (fractionPart >= 64 && (integerPart >= maxIntegerValue || (std::is_signed_v<IntT> && integerPart <= minIntegerValue))) {
		return tl::unexpected { ParseIntError::OutOfRange };
	} else {
		IntT fixedValue = integerPart << 6;
		if (isNegative) {
			fixedValue -= fractionPart;
		} else {
			fixedValue += fractionPart;
		}
		return fixedValue;
	}
}

} // namespace devilution
