#pragma once

// Keyboard keys acting like gamepad buttons
#ifndef HAS_KBCTRL
#define HAS_KBCTRL 0
#endif

#if HAS_KBCTRL == 1
#include "controls/controller_buttons.h"
#include <SDL.h>

namespace devilution {

ControllerButton KbCtrlToControllerButton(const SDL_Event &event);

bool IsKbCtrlButtonPressed(ControllerButton button);

bool ProcessKbCtrlAxisMotion(const SDL_Event &event);

} // namespace devilution
#endif
