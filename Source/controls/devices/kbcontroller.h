#pragma once

// Keyboard keys acting like gamepad buttons
#ifndef HAS_KBCTRL
#define HAS_KBCTRL 0
#endif

#if HAS_KBCTRL == 1
#include "controls/controller_buttons.h"
#include <SDL.h>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {

ControllerButton KbCtrlToControllerButton(const SDL_Event &event);

SDL_Keycode ControllerButtonToKbCtrlKeyCode(ControllerButton button);

bool IsKbCtrlButtonPressed(ControllerButton button);

bool ProcessKbCtrlAxisMotion(const SDL_Event &event);

} // namespace devilution
#endif
