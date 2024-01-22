#pragma once

#include <string>
#include <string_view>

namespace devilution {

void AsciiStrToLower(std::string &str);

[[nodiscard]] inline std::string AsciiStrToLower(std::string_view str)
{
	std::string copy { str.data(), str.size() };
	AsciiStrToLower(copy);
	return copy;
}

} // namespace devilution
