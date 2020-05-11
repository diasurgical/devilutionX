#pragma once

#include <cstddef>

#include "DiabloUI/ui_item.h"

namespace dvl {

void UiErrorOkDialog(const char *text, vUiItemBase renderBehind);
void UiErrorOkDialog(const char *text, const char *caption, vUiItemBase renderBehind);
void UiErrorOkDialog(const char *text, const char *caption);
void UiOkDialog(const char *text, const char *caption, bool error, vUiItemBase renderBehind);

} // namespace dvl
