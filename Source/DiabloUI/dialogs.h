#pragma once

#include <cstddef>

#include "DiabloUI/ui_item.h"

namespace devilution {

void UiErrorOkDialog(const char *text, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind);
void UiErrorOkDialog(const char *caption, const char *text, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind);

} // namespace devilution
