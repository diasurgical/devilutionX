#pragma once
// Unifies joystick, gamepad, and keyboard controller APIs.

#include "all.h"

namespace dvl {

namespace ControllerButtonNS {
// NOTE: A, B, X, Y refer to physical positions on an XBox 360 controller.
// A<->B and X<->Y are reversed on a Nintendo controller.
enum ControllerButton {
	NONE = 0,
	CBIGNORE,
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
}

inline bool IsDPadButton(ControllerButtonNS::ControllerButton button)
{
	return button == ControllerButtonNS::BUTTON_DPAD_UP
	    || button == ControllerButtonNS::BUTTON_DPAD_DOWN
	    || button == ControllerButtonNS::BUTTON_DPAD_LEFT
	    || button == ControllerButtonNS::BUTTON_DPAD_RIGHT;
}

} // namespace dvl
