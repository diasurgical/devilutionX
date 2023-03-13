#include "utils/str_cat.hpp"

#include <fmt/format.h>

namespace devilution {

char *BufCopy(char *out, int value)
{
	const fmt::format_int formatted { value };
	std::memcpy(out, formatted.data(), formatted.size());
	return out + formatted.size();
}

void StrAppend(std::string &out, int value)
{
	const fmt::format_int formatted { value };
	out.append(formatted.data(), formatted.size());
}

} // namespace devilution
