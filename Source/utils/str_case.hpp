#pragma once

#include <string>

#include "utils/stdcompat/string_view.hpp"

namespace devilution {

void AsciiStrToLower(std::string &str);

[[nodiscard]] inline std::string AsciiStrToLower(string_view str)
{
	std::string copy { str.data(), str.size() };
	AsciiStrToLower(copy);
	return copy;
}

} // namespace devilution
