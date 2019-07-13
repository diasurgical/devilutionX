#ifndef TOUCH_H
#define TOUCH_H

#include <SDL.h>
#include <stdbool.h>

void handle_touch(SDL_Event *event, int current_mouse_x, int current_mouse_y);
void finish_simulated_mouse_clicks(int current_mouse_x, int current_mouse_y);
#endif
