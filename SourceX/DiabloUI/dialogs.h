#pragma once

#include <cstddef>

#include "DiabloUI/ui_item.h"

namespace dvl {

void UiErrorOkDialog(const char *text, UiItem *renderBehind, std::size_t renderBehindSize);
void UiErrorOkDialog(const char *text, const char *caption, UiItem *renderBehind, std::size_t renderBehind_size);
void UiOkDialog(const char *text, const char *caption, bool error, UiItem *renderBehind, std::size_t renderBehind_size);

} // namespace dvl
