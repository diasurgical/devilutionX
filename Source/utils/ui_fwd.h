#pragma once

#include <SDL.h>

#include "utils/attributes.h"

namespace devilution {

extern DVL_API_FOR_TEST Uint16 gnScreenWidth;
extern DVL_API_FOR_TEST Uint16 gnScreenHeight;
extern DVL_API_FOR_TEST Uint16 gnViewportHeight;

Uint16 GetScreenWidth();
Uint16 GetScreenHeight();
Uint16 GetViewportHeight();

float GetDpiScalingFactor();
bool SpawnWindow(const char *lpWindowName);
void ReinitializeTexture();
void ReinitializeRenderer();
void ResizeWindow();
void UiErrorOkDialog(const char *caption, const char *text, bool error = true);

} // namespace devilution
