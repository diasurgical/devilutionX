#pragma once

// Joystick mappings for SDL1 and additional buttons on SDL2.

#include <SDL.h>
#include "controls/controller_buttons.h"

namespace dvl {

ControllerButtonNS::ControllerButton JoyButtonToControllerButton(const SDL_Event &event);

bool IsJoystickButtonPressed(ControllerButtonNS::ControllerButton button);

bool ProcessJoystickAxisMotion(const SDL_Event &event);

SDL_Joystick *CurrentJoystick();
int CurrentJoystickIndex();

void InitJoystick();

} // namespace dvl
