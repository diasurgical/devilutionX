#pragma once

#include <cstddef>

#include "DiabloUI/ui_item.h"

namespace dvl {

void UiErrorOkDialog(const char *text, UiItem *renderBehind, std::size_t renderBehindSize);
void UiErrorOkDialog(const char *text, const char *caption, UiItem *render_behind, std::size_t render_behind_size);
void UiOkDialog(const char *text, const char *caption, bool error, UiItem *render_behind, std::size_t render_behind_size);

} // namespace dvl
