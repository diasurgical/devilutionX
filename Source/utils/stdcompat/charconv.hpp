#pragma once

#if __cplusplus >= 201703L
#include <charconv>
#else
#include <system_error>
#include <type_traits>
#endif

namespace devilution {
#if __cplusplus >= 201703L
using std::from_chars;
using std::from_chars_result;
#else
struct from_chars_result {
	const char *ptr;
	std::errc ec;
};

template <typename T>
from_chars_result from_chars(const char *first, const char *last, T &value, int base = 10)
{
	static_assert(std::is_integral<T>::value, "devilution::from_char is only defined for integral types");

	if (base != 10 || first == last)
		return { first, std::errc::invalid_argument };

	const char *current = first;
	bool isNegative = false;
	if (std::is_signed<T>::value) {
		if (*current == '-') {
			isNegative = true;
			++current;
			if (current == last)
				return { first, std::errc::invalid_argument };
		}
	}

	using U = typename std::make_unsigned<T>::type;
	U workingValue = 0;

	if (*current < '0' || *current > '9') {
		return { first, std::errc::invalid_argument };
	}

	bool hasOverflown = false;

	do {
		if (!hasOverflown) {
			U testValue = workingValue * base;
			testValue += *current - '0';

			if (testValue < workingValue)
				hasOverflown = true;
			else
				workingValue = testValue;
		}

		++current;
	} while (current != last && *current >= '0' && *current <= '9');

	if (hasOverflown) {
		return { current, std::errc::result_out_of_range };
	}

	if (isNegative) {
		using S = typename std::make_signed<T>::type;
		if (workingValue > static_cast<U>(std::numeric_limits<S>::min())) {
			return { current, std::errc::result_out_of_range };
		} else if (workingValue == static_cast<U>(std::numeric_limits<S>::min())) {
			value = std::numeric_limits<S>::min();
		} else {
			value = -static_cast<S>(workingValue);
		}
	} else {
		if (workingValue > static_cast<U>(std::numeric_limits<T>::max())) {
			return { current, std::errc::result_out_of_range };
		}
		value = static_cast<T>(workingValue);
	}

	return { current };
}
#endif
} // namespace devilution
