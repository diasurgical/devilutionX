#pragma once

#include <SDL.h>

#include "utils/attributes.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

extern DVL_API_FOR_TEST Uint16 gnScreenWidth;
extern DVL_API_FOR_TEST Uint16 gnScreenHeight;
extern DVL_API_FOR_TEST Uint16 gnViewportHeight;

Uint16 GetScreenWidth();
Uint16 GetScreenHeight();
Uint16 GetViewportHeight();

bool SpawnWindow(const char *lpWindowName);
void ReinitializeRenderer();
void ResizeWindow();

} // namespace devilution
