#pragma once

#include <cstdarg>
#include <cstddef>

#include "utils/attributes.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

void printInConsole(string_view str);
void printNewlineInConsole();
void printfInConsole(const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(1, 2);
void vprintfInConsole(const char *fmt, std::va_list ap) DVL_PRINTF_ATTRIBUTE(1, 0);

} // namespace devilution
