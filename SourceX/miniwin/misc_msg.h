#pragma once

#include <SDL.h>

namespace dvl {

int translate_sdl_key(SDL_Keysym key);
SDL_Keycode translate_dvl_key(int key);

}
