#pragma once

#include <cstddef>

#include "DiabloUI/ui_item.h"

namespace dvl {

void UiErrorOkDialog(const char *text, vUiItemBase renderBehind, std::size_t renderBehindSize);
void UiErrorOkDialog(const char *text, const char *caption, vUiItemBase render_behind, std::size_t render_behind_size);
void UiErrorOkDialog(const char *text, const char *caption);
void UiOkDialog(const char *text, const char *caption, bool error, vUiItemBase render_behind, std::size_t render_behind_size);

} // namespace dvl
