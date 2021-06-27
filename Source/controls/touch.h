#pragma once

#include <SDL.h>

namespace devilution {

#ifndef USE_SDL1
void handle_touch(SDL_Event *event, int currentMouseX, int currentMouseY);
#endif

} // namespace devilution
