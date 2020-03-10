#pragma once

#include <cstddef>

#include "DiabloUI/fonts.h"

namespace dvl {

std::size_t GetArtStrWidth(const char *str, std::size_t size);
void WordWrapArtStr(char *text, std::size_t width);

} // namespace dvl
