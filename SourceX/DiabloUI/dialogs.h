#pragma once

#include <cstddef>

#include "DiabloUI/ui_item.h"

namespace dvl {

void UiErrorOkDialog(const char *text, std::vector<UiItemBase *> renderBehind);
void UiErrorOkDialog(const char *text, const char *caption, std::vector<UiItemBase *> renderBehind);
void UiOkDialog(const char *text, const char *caption, bool error, std::vector<UiItemBase *> renderBehind);

} // namespace dvl
