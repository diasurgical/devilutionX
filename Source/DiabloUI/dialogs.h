#pragma once

#include <cstddef>

#include "DiabloUI/ui_item.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

void UiErrorOkDialog(string_view text, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind);
void UiErrorOkDialog(string_view caption, string_view text, const std::vector<std::unique_ptr<UiItemBase>> &renderBehind);

} // namespace devilution
