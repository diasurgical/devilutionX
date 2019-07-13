#ifndef TOUCH_H
#define TOUCH_H

#include <SDL.h>
#include <stdbool.h>

#ifndef USE_SDL1
void handle_touch(SDL_Event *event, int current_mouse_x, int current_mouse_y);
void finish_simulated_mouse_clicks(int current_mouse_x, int current_mouse_y);
#endif

#endif
