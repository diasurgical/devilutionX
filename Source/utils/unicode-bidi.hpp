#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace devilution {

std::u32string ConvertLogicalToVisual(std::u32string_view input);

} // namespace devilution
