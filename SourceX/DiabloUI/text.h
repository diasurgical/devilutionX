#pragma once

#include <cstddef>

#include "DiabloUI/fonts.h"

namespace dvl {

enum TXT_JUST {
	JustLeft = 0,
	JustCentre = 1,
	JustRight = 2,
};

std::size_t GetStrWidth(const char *str, std::size_t size);
void WordWrap(char *text, std::size_t width);

} // namespace dvl
