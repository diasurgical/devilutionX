#pragma once

#include <cstdarg>
#include <cstddef>

#include "utils/attributes.h"

namespace devilution {

void printInConsole(const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(1, 2);
void printInConsoleV(const char *fmt, std::va_list ap) DVL_PRINTF_ATTRIBUTE(1, 0);

} // namespace devilution
