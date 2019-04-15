#pragma once

#include <SDL2/SDL.h>

namespace dvl {

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;
extern SDL_Surface *surface;
extern SDL_Palette *palette;
extern SDL_Surface *pal_surface;

void sdl_present_surface();

void sdl_update_entire_surface();

}  // namespace dvl
