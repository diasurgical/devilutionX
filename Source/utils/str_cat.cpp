#include "utils/str_cat.hpp"

#include <fmt/format.h>

namespace devilution {

char *BufCopy(char *out, long long value)
{
	const fmt::format_int formatted { value };
	std::memcpy(out, formatted.data(), formatted.size());
	return out + formatted.size();
}
char *BufCopy(char *out, unsigned long long value)
{
	const fmt::format_int formatted { value };
	std::memcpy(out, formatted.data(), formatted.size());
	return out + formatted.size();
}

void StrAppend(std::string &out, long long value)
{
	const fmt::format_int formatted { value };
	out.append(formatted.data(), formatted.size());
}
void StrAppend(std::string &out, unsigned long long value)
{
	const fmt::format_int formatted { value };
	out.append(formatted.data(), formatted.size());
}

} // namespace devilution
