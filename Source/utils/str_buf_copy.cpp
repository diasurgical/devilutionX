#include "utils/str_buf_copy.hpp"

#include <fmt/compile.h>
#include <fmt/core.h>

namespace devilution {

char *BufCopy(char *out, int value)
{
	return fmt::format_to(out, FMT_COMPILE("{}"), value);
}

} // namespace devilution
