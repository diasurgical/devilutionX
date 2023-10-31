#ifdef _DEBUG
#pragma once

#include <SDL.h>

#include "engine/surface.hpp"

namespace devilution {

void OpenConsole();
bool ConsoleHandleEvent(const SDL_Event &event);
void DrawConsole(const Surface &out);

} // namespace devilution
#endif // _DEBUG
