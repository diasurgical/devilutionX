#pragma once

#include <charconv>
#include <system_error>

#include "utils/stdcompat/string_view.hpp"

namespace devilution {

enum class ParseIntStatus {
	Ok,
	ParseError,
	OutOfRange
};

template <typename IntT>
struct ParseIntResult {
	ParseIntStatus status;
	IntT value = 0;

	[[nodiscard]] bool ok() const
	{
		return status == ParseIntStatus::Ok;
	}

	template <typename T>
	[[nodiscard]] IntT value_or(T defaultValue) const // NOLINT(readability-identifier-naming)
	{
		return ok() ? value : static_cast<IntT>(defaultValue);
	}
};

template <typename IntT>
ParseIntResult<IntT> ParseInt(
    string_view str, IntT min = std::numeric_limits<IntT>::min(),
    IntT max = std::numeric_limits<IntT>::max())
{
	IntT value;
	const std::from_chars_result result = std::from_chars(str.data(), str.data() + str.size(), value);
	if (result.ec == std::errc::invalid_argument)
		return ParseIntResult<IntT> { ParseIntStatus::ParseError };
	if (result.ec == std::errc::result_out_of_range || value < min || value > max)
		return ParseIntResult<IntT> { ParseIntStatus::OutOfRange };
	if (result.ec != std::errc())
		return ParseIntResult<IntT> { ParseIntStatus::ParseError };
	return ParseIntResult<IntT> { ParseIntStatus::Ok, value };
}

} // namespace devilution
