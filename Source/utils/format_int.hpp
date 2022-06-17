#pragma once

#include <string>

#include <fmt/format.h>

#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

/**
 * @brief Formats integer with thousands separator.
 */
inline std::string FormatInteger(int n)
{
	std::string number = fmt::format("{:d}", n);
	std::string out = "";

	int length = number.length();
	int mlength = length % 3;
	if (mlength == 0)
		mlength = 3;
	out.append(number.substr(0, mlength));
	for (int i = mlength; i < length; i += 3) {
		AppendStrView(out, _(/* TRANSLATORS: Thousands separator */ ","));
		out.append(number.substr(i, 3));
	}

	return out;
}

} //namespace devilution
