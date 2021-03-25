#pragma once

#include <SDL.h>

#include "all.h"
#include "./axis_direction.h"
#include "./controller_buttons.h"

namespace dvl {

struct ControllerButtonEvent {
	ControllerButton button;
	bool up;
};

class Controller {
public:
	// Raw axis values.
	float leftStickX = 0, leftStickY = 0, rightStickX = 0, rightStickY = 0;
	// Axis values scaled to [-1, 1] range and clamped to a deadzone.
	float leftStickXUnscaled = 0, leftStickYUnscaled = 0, rightStickXUnscaled = 0, rightStickYUnscaled = 0;
	// Whether stick positions have been updated and need rescaling.
	bool leftStickNeedsScaling = false, rightStickNeedsScaling = false;

	// Returns direction of the left thumb stick or DPad (if allow_dpad = true).
	AxisDirection GetLeftStickOrDpadDirection(bool allow_dpad = true);

	// Normalize joystick values
	void ScaleJoysticks();

	// NOTE: Not idempotent because of how it handles axis triggers.
	// Must be called exactly once for each SDL input event.
	ControllerButton ToControllerButton(const SDL_Event &event) const;

private:
	void ScaleJoystickAxes(float *x, float *y, float deadzone);
};

// NOTE: Not idempotent because of how it handles axis triggers.
// Must be called exactly once per SDL input event.
ControllerButtonEvent ToControllerButtonEvent(const SDL_Event &event);

bool IsControllerButtonPressed(ControllerButton button);

bool HandleControllerAddedOrRemovedEvent(const SDL_Event &event);

} // namespace dvl
