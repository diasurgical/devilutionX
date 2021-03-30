#pragma once

#include "all.h"

namespace dvl {
void UiSelOkDialog(const char *title, const char *body, bool background);
void selok_Free();
void selok_Select(int value);
void selok_Esc();

} // namespace dvl
