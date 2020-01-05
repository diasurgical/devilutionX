#pragma once
// Unifies joystick, gamepad, and keyboard controller APIs.

#include "devilution.h"

namespace dvl {

// NOTE: A, B, X, Y refer to physical positions on an XBox 360 controller.
// A<->B and X<->Y are reversed on a Nintendo controller.
enum class ControllerButton {
	NONE = 0,
	IGNORE,
	AXIS_TRIGGERLEFT,  // ZL (aka L2)
	AXIS_TRIGGERRIGHT, // ZR (aka R2)
	BUTTON_A,          // Bottom button
	BUTTON_B,          // Right button
	BUTTON_X,          // Left button
	BUTTON_Y,          // TOP button
	BUTTON_LEFTSTICK,
	BUTTON_RIGHTSTICK,
	BUTTON_LEFTSHOULDER,
	BUTTON_RIGHTSHOULDER,
	BUTTON_START,
	BUTTON_BACK,
	BUTTON_DPAD_UP,
	BUTTON_DPAD_DOWN,
	BUTTON_DPAD_LEFT,
	BUTTON_DPAD_RIGHT
};

inline bool IsDPadButton(ControllerButton button)
{
	return button == ControllerButton::BUTTON_DPAD_UP
	    || button == ControllerButton::BUTTON_DPAD_DOWN
	    || button == ControllerButton::BUTTON_DPAD_LEFT
	    || button == ControllerButton::BUTTON_DPAD_RIGHT;
}

} // namespace dvl
