#pragma once

#ifdef __vita__

#include <SDL.h>

namespace devilution {

void handle_touch(SDL_Event *event, int currentMouseX, int currentMouseY);
void finish_simulated_mouse_clicks(int currentMouseX, int currentMouseY);

} // namespace devilution

#endif
