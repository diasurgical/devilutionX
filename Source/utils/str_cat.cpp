#include "utils/str_cat.hpp"

#include <cstddef>

#include <fmt/compile.h>
#include <fmt/core.h>

namespace devilution {

namespace {

template <typename T>
constexpr size_t MaxDecimalDigits = 241 * sizeof(T) / 100 + 1;

} // namespace

char *BufCopy(char *out, int value)
{
	return fmt::format_to(out, FMT_COMPILE("{}"), value);
}

void StrAppend(std::string &out, int value)
{
	char buf[MaxDecimalDigits<int> + 1];
	out.append(&buf[0], BufCopy(buf, value) - &buf[0]);
}

} // namespace devilution
