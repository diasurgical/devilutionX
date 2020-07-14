#pragma once

#include <SDL.h>

namespace dvl {

#ifndef USE_SDL1
void handle_touch(SDL_Event *event, int current_mouse_x, int current_mouse_y);
#endif

} // namespace dvl
