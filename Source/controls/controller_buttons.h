#pragma once
// Unifies joystick, gamepad, and keyboard controller APIs.

#include <cstdint>
#include <functional>
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
	FIRST = ControllerButton_NONE,
	LAST = ControllerButton_BUTTON_DPAD_RIGHT
};

struct ControllerButtonCombo {
	constexpr ControllerButtonCombo()
	    : modifier(ControllerButton_NONE)
	    , button(ControllerButton_NONE)
	{
	}

	constexpr ControllerButtonCombo(ControllerButton button)
	    : modifier(ControllerButton_NONE)
	    , button(button)
	{
	}

	constexpr ControllerButtonCombo(ControllerButton modifier, ControllerButton button)
	    : modifier(modifier)
	    , button(button)
	{
	}

	ControllerButton modifier;
	ControllerButton button;
};

struct ControllerButtonEvent {
	ControllerButtonEvent(ControllerButton button, bool up)
	    : button(button)
	    , up(up)
	{
	}

	ControllerButton button;
	bool up;
};

inline bool IsDPadButton(ControllerButton button)
{
	return button == ControllerButton_BUTTON_DPAD_UP
	    || button == ControllerButton_BUTTON_DPAD_DOWN
	    || button == ControllerButton_BUTTON_DPAD_LEFT
	    || button == ControllerButton_BUTTON_DPAD_RIGHT;
}

enum class GamepadLayout : uint8_t {
	Generic,
	Nintendo,
	PlayStation,
	Xbox,
};

[[nodiscard]] std::string_view ToString(GamepadLayout gamepadType, ControllerButton button);

} // namespace devilution
