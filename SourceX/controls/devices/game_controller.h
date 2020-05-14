
#pragma once

#include <SDL.h>
#include "controls/controller_buttons.h"

#ifndef USE_SDL1
namespace dvl {

ControllerButtonNS::ControllerButton GameControllerToControllerButton(const SDL_Event &event);

bool IsGameControllerButtonPressed(ControllerButtonNS::ControllerButton button);

bool ProcessGameControllerAxisMotion(const SDL_Event &event);

SDL_GameController *CurrentGameController();

// Must be called after InitJoystick().
void InitGameController();

} // namespace dvl
#endif
