#pragma once
// Unifies joystick, gamepad, and keyboard controller APIs.

#include <stdint.h>
#include <string_view>

namespace devilution {

// NOTE: A, B, X, Y refer to physical positions on an XBox 360 controller.
// A<->B and X<->Y are reversed on a Nintendo controller.
enum ControllerButton : uint8_t {
	ControllerButton_NONE,
	ControllerButton_IGNORE,
	ControllerButton_AXIS_TRIGGERLEFT,  // ZL (aka L2)
	ControllerButton_AXIS_TRIGGERRIGHT, // ZR (aka R2)
	ControllerButton_BUTTON_A,          // Bottom button
	ControllerButton_BUTTON_B,          // Right button
	ControllerButton_BUTTON_X,          // Left button
	ControllerButton_BUTTON_Y,          // TOP button
	ControllerButton_BUTTON_LEFTSTICK,
	ControllerButton_BUTTON_RIGHTSTICK,
	ControllerButton_BUTTON_LEFTSHOULDER,
	ControllerButton_BUTTON_RIGHTSHOULDER,
	ControllerButton_BUTTON_START,
	ControllerButton_BUTTON_BACK,
	ControllerButton_BUTTON_DPAD_UP,
	ControllerButton_BUTTON_DPAD_DOWN,
	ControllerButton_BUTTON_DPAD_LEFT,
	ControllerButton_BUTTON_DPAD_RIGHT,
};

constexpr std::string_view toString(ControllerButton value)
{
	switch(value) {
	case ControllerButton_NONE:
		return "None";
	case ControllerButton_IGNORE:
		return "Ignore";
	case ControllerButton_AXIS_TRIGGERLEFT:
		return "Axis Triggerleft";
	case ControllerButton_AXIS_TRIGGERRIGHT:
		return "Axis Triggerright";
	case ControllerButton_BUTTON_A:
		return "Button A";
	case ControllerButton_BUTTON_B:
		return "Button B";
	case ControllerButton_BUTTON_X:
		return "Button X";
	case ControllerButton_BUTTON_Y:
		return "Button Y";
	case ControllerButton_BUTTON_LEFTSTICK:
		return "Button Leftstick";
	case ControllerButton_BUTTON_RIGHTSTICK:
		return "Button Rightstick";
	case ControllerButton_BUTTON_LEFTSHOULDER:
		return "Button Leftshoulder";
	case ControllerButton_BUTTON_RIGHTSHOULDER:
		return "Button Rightshoulder";
	case ControllerButton_BUTTON_START:
		return "Button Start";
	case ControllerButton_BUTTON_BACK:
		return "Button Back";
	case ControllerButton_BUTTON_DPAD_UP:
		return "Button Dpad Up";
	case ControllerButton_BUTTON_DPAD_DOWN:
		return "Button Dpad Down";
	case ControllerButton_BUTTON_DPAD_LEFT:
		return "Button Dpad Left";
	case ControllerButton_BUTTON_DPAD_RIGHT:
		return "Button Dpad Right";
	}
}

inline bool IsDPadButton(ControllerButton button)
{
	return button == ControllerButton_BUTTON_DPAD_UP
	    || button == ControllerButton_BUTTON_DPAD_DOWN
	    || button == ControllerButton_BUTTON_DPAD_LEFT
	    || button == ControllerButton_BUTTON_DPAD_RIGHT;
}

} // namespace devilution
