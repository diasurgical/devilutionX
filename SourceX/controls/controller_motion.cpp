#include "controls/controller_motion.h"

#include <cmath>

#include "controls/devices/game_controller.h"
#include "controls/devices/joystick.h"
#include "controls/devices/kbcontroller.h"
#include "controls/controller.h"
#include "controls/game_controls.h"

namespace dvl {

Controller controller;

namespace {

// SELECT + D-Pad to simulate right stick movement.
bool SimulateRightStickWithDpad(const SDL_Event &event, ControllerButtonEvent ctrl_event)
{
	if (dpad_hotkeys)
		return false;
	static bool simulating = false;
	if (ctrl_event.button == ControllerButton_BUTTON_BACK) {
		if (ctrl_event.up && simulating) {
			controller.rightStickX = controller.rightStickY = 0;
			simulating = false;
		}
		return false;
	}
	if (!IsControllerButtonPressed(ControllerButton_BUTTON_BACK))
		return false;
	switch (ctrl_event.button) {
	case ControllerButton_BUTTON_DPAD_LEFT:
		controller.rightStickX = ctrl_event.up ? 0 : -1;
		break;
	case ControllerButton_BUTTON_DPAD_RIGHT:
		controller.rightStickX = ctrl_event.up ? 0 : 1;
		break;
	case ControllerButton_BUTTON_DPAD_UP:
		controller.rightStickY = ctrl_event.up ? 0 : 1;
		break;
	case ControllerButton_BUTTON_DPAD_DOWN:
		controller.rightStickY = ctrl_event.up ? 0 : -1;
		break;
	default:
		return false;
	}
	simulating = !(controller.rightStickX == 0 && controller.rightStickY == 0);

	return true;
}

} // namespace

// Updates motion state for mouse and joystick sticks.
bool ProcessControllerMotion(const SDL_Event &event, ControllerButtonEvent ctrl_event)
{
#ifndef USE_SDL1
	GameController *const controller = GameController::Get(event);
	if (controller != NULL && controller->ProcessAxisMotion(event)) {
		controller->ScaleJoysticks();
		return true;
	}
#endif
	Joystick *const joystick = Joystick::Get(event);
	if (joystick != NULL && joystick->ProcessAxisMotion(event)) {
		joystick->ScaleJoysticks();
		return true;
	}
#if HAS_KBCTRL == 1
	KeyboardController *const keyboardController = KeyboardController::Get(event);
	if (keyboardController != NULL && keyboardController->ProcessAxisMotion(event)) {
		keyboardController->ScaleJoysticks();
		return true;
	}
#endif
	return SimulateRightStickWithDpad(event, ctrl_event);
}

} // namespace dvl
