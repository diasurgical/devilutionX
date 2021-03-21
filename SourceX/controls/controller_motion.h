#pragma once

// Processes and stores mouse and joystick motion.

#include <SDL.h>

#include "./axis_direction.h"
#include "./controller.h"

namespace dvl {

extern struct Controller controller;

// Updates motion state for mouse and joystick sticks.
bool ProcessControllerMotion(const SDL_Event &event, ControllerButtonEvent ctrl_event);

// Returns direction of the left thumb stick or DPad (if allow_dpad = true).
AxisDirection GetLeftStickOrDpadDirection(bool allow_dpad = true);

} // namespace dvl
