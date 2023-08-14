#pragma once

#include <cstddef>
#include <string_view>

#include "DiabloUI/ui_item.h"

namespace devilution {

void UiErrorOkDialog(std::string_view text, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind);
void UiErrorOkDialog(std::string_view caption, std::string_view text, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind);

} // namespace devilution
