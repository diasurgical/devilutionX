#pragma once

// Processes and stores mouse and joystick motion.

#include <SDL.h>

#include "./axis_direction.h"
#include "./controller.h"

namespace devilution {

// Whether we're currently simulating the mouse with SELECT + D-Pad.
extern bool SimulatingMouseWithSelectAndDPad;

// Raw axis values.
extern float leftStickXUnscaled, leftStickYUnscaled, rightStickXUnscaled, rightStickYUnscaled;

// Axis values scaled to [-1, 1] range and clamped to a deadzone.
extern float leftStickX, leftStickY, rightStickX, rightStickY;

// Whether stick positions have been updated and need rescaling.
extern bool leftStickNeedsScaling, rightStickNeedsScaling;

// Updates motion state for mouse and joystick sticks.
bool ProcessControllerMotion(const SDL_Event &event, ControllerButtonEvent ctrlEvent);

// Returns direction of the left thumb stick or DPad (if allow_dpad = true).
AxisDirection GetLeftStickOrDpadDirection(bool allowDpad = true);

} // namespace devilution
