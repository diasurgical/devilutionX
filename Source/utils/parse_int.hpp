#pragma once

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
    IntT max = std::numeric_limits<IntT>::max())
{
	IntT value;
	const std::from_chars_result result = std::from_chars(str.data(), str.data() + str.size(), value);
	if (result.ec == std::errc::invalid_argument)
		return tl::unexpected(ParseIntError::ParseError);
	if (result.ec == std::errc::result_out_of_range || value < min || value > max)
		return tl::unexpected(ParseIntError::OutOfRange);
	if (result.ec != std::errc())
		return tl::unexpected(ParseIntError::ParseError);
	return value;
}

} // namespace devilution
