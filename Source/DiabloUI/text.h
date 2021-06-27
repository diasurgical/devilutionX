#pragma once

#include <cstddef>

#include "DiabloUI/fonts.h"

namespace devilution {

std::size_t GetArtStrWidth(const char *str, std::size_t size);
void WordWrapArtStr(char *text, std::size_t width, std::size_t size = AFT_SMALL);

} // namespace devilution
