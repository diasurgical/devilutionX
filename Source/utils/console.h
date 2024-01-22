#pragma once

#include <cstdarg>
#include <cstddef>
#include <string_view>

#include "utils/attributes.h"

namespace devilution {

void printInConsole(std::string_view str);
void printNewlineInConsole();
void printfInConsole(const char *fmt, ...) DVL_PRINTF_ATTRIBUTE(1, 2);
void vprintfInConsole(const char *fmt, std::va_list ap) DVL_PRINTF_ATTRIBUTE(1, 0);

} // namespace devilution
