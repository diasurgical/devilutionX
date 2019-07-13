#ifndef SWITCH_TOUCH_H
#define SWITCH_TOUCH_H

#include <SDL.h>
#include <stdbool.h>

void switch_handle_touch(SDL_Event *event, int current_mouse_x, int current_mouse_y);
void switch_finish_simulated_mouse_clicks(int current_mouse_x, int current_mouse_y);
#endif /* SWITCH_TOUCH_H */
