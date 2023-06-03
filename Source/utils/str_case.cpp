#include "utils/str_case.hpp"

namespace devilution {

void AsciiStrToLower(std::string &str)
{
	for (char &c : str) { // NOLINT(readability-identifier-length)
		if (c >= 'A' && c <= 'Z')
			c += ('a' - 'A');
	}
}

} // namespace devilution
