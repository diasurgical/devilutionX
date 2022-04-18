#include "controls/controller_motion.h"

#include <cmath>

#include "controls/controller.h"
#ifndef USE_SDL1
#include "controls/devices/game_controller.h"
#endif
#include "controls/devices/joystick.h"
#include "controls/devices/kbcontroller.h"
#include "controls/game_controls.h"
#include "controls/plrctrls.h"
#include "controls/touch/gamepad.h"
#include "options.h"
#include "utils/log.hpp"

namespace devilution {

bool SimulatingMouseWithSelectAndDPad;

namespace {

void ScaleJoystickAxes(float *x, float *y, float deadzone)
{
	// radial and scaled dead_zone
	// https://web.archive.org/web/20200130014626/www.third-helix.com:80/2013/04/12/doing-thumbstick-dead-zones-right.html
	// input values go from -32767.0...+32767.0, output values are from -1.0 to 1.0;

	if (deadzone == 0) {
		return;
	}
	if (deadzone >= 1.0) {
		*x = 0;
		*y = 0;
		return;
	}

	const float maximum = 32767.0;
	float analogX = *x;
	float analogY = *y;
	float deadZone = deadzone * maximum;

	float magnitude = std::sqrt(analogX * analogX + analogY * analogY);
	if (magnitude >= deadZone) {
		// find scaled axis values with magnitudes between zero and maximum
		float scalingFactor = 1.F / magnitude * (magnitude - deadZone) / (maximum - deadZone);
		analogX = (analogX * scalingFactor);
		analogY = (analogY * scalingFactor);

		// clamp to ensure results will never exceed the max_axis value
		float clampingFactor = 1.F;
		float absAnalogX = std::fabs(analogX);
		float absAnalogY = std::fabs(analogY);
		if (absAnalogX > 1.0 || absAnalogY > 1.0) {
			if (absAnalogX > absAnalogY) {
				clampingFactor = 1.F / absAnalogX;
			} else {
				clampingFactor = 1.F / absAnalogY;
			}
		}
		*x = (clampingFactor * analogX);
		*y = (clampingFactor * analogY);
	} else {
		*x = 0;
		*y = 0;
	}
}

void SetSimulatingMouseWithDpad(bool value)
{
	if (SimulatingMouseWithSelectAndDPad == value)
		return;
	SimulatingMouseWithSelectAndDPad = value;
	if (value) {
		LogVerbose("Control: begin simulating mouse with D-Pad");
	} else {
		LogVerbose("Control: end simulating mouse with D-Pad");
	}
}

// SELECT + D-Pad to simulate right stick movement.
bool SimulateRightStickWithDpad(ControllerButtonEvent ctrlEvent)
{
	if (sgOptions.Controller.bDpadHotkeys)
		return false;
	if (ctrlEvent.button == ControllerButton_NONE || ctrlEvent.button == ControllerButton_IGNORE)
		return false;
	if (ctrlEvent.button == ControllerButton_BUTTON_BACK) {
		if (SimulatingMouseWithSelectAndDPad) {
			if (ctrlEvent.up) {
				rightStickX = rightStickY = 0;
			}
			return true;
		}
		return false;
	}

	if (!IsControllerButtonPressed(ControllerButton_BUTTON_BACK)) {
		SetSimulatingMouseWithDpad(false);
		return false;
	}
	switch (ctrlEvent.button) {
	case ControllerButton_BUTTON_DPAD_LEFT:
		if (ctrlEvent.up) {
			rightStickX = 0;
		} else {
			rightStickX = -1.F;
			SetSimulatingMouseWithDpad(true);
		}
		break;
	case ControllerButton_BUTTON_DPAD_RIGHT:
		if (ctrlEvent.up) {
			rightStickX = 0;
		} else {
			rightStickX = 1.F;
			SetSimulatingMouseWithDpad(true);
		}
		break;
	case ControllerButton_BUTTON_DPAD_UP:
		if (ctrlEvent.up) {
			rightStickY = 0;
		} else {
			rightStickY = 1.F;
			SetSimulatingMouseWithDpad(true);
		}
		break;
	case ControllerButton_BUTTON_DPAD_DOWN:
		if (ctrlEvent.up) {
			rightStickY = 0;
		} else {
			rightStickY = -1.F;
			SetSimulatingMouseWithDpad(true);
		}
		break;
	default:
		if (!IsSimulatedMouseClickBinding(ctrlEvent)) {
			SetSimulatingMouseWithDpad(false);
		}
		return false;
	}

	return true;
}

} // namespace

float leftStickX, leftStickY, rightStickX, rightStickY;
float leftStickXUnscaled, leftStickYUnscaled, rightStickXUnscaled, rightStickYUnscaled;
bool leftStickNeedsScaling, rightStickNeedsScaling;

namespace {

void ScaleJoysticks()
{
	const float rightDeadzone = sgOptions.Controller.fDeadzone;
	const float leftDeadzone = sgOptions.Controller.fDeadzone;

	if (leftStickNeedsScaling) {
		leftStickX = leftStickXUnscaled;
		leftStickY = leftStickYUnscaled;
		ScaleJoystickAxes(&leftStickX, &leftStickY, leftDeadzone);
		leftStickNeedsScaling = false;
	}

	if (rightStickNeedsScaling) {
		rightStickX = rightStickXUnscaled;
		rightStickY = rightStickYUnscaled;
		ScaleJoystickAxes(&rightStickX, &rightStickY, rightDeadzone);
		rightStickNeedsScaling = false;
	}
}

} // namespace

// Updates motion state for mouse and joystick sticks.
bool ProcessControllerMotion(const SDL_Event &event, ControllerButtonEvent ctrlEvent)
{
#ifndef USE_SDL1
	GameController *const controller = GameController::Get(event);
	if (controller != nullptr && devilution::GameController::ProcessAxisMotion(event)) {
		ScaleJoysticks();
		SetSimulatingMouseWithDpad(false);
		return true;
	}
#endif
	Joystick *const joystick = Joystick::Get(event);
	if (joystick != nullptr && devilution::Joystick::ProcessAxisMotion(event)) {
		ScaleJoysticks();
		SetSimulatingMouseWithDpad(false);
		return true;
	}
#if HAS_KBCTRL == 1
	if (ProcessKbCtrlAxisMotion(event)) {
		SetSimulatingMouseWithDpad(false);
		return true;
	}
#endif
	return SimulateRightStickWithDpad(ctrlEvent);
}

AxisDirection GetLeftStickOrDpadDirection(bool allowDpad)
{
	const float stickX = leftStickX;
	const float stickY = leftStickY;

	AxisDirection result { AxisDirectionX_NONE, AxisDirectionY_NONE };

	allowDpad = allowDpad && !IsControllerButtonPressed(ControllerButton_BUTTON_START);

	bool isUpPressed = stickY >= 0.5 || (allowDpad && IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_UP));
	bool isDownPressed = stickY <= -0.5 || (allowDpad && IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_DOWN));
	bool isLeftPressed = stickX <= -0.5 || (allowDpad && IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_LEFT));
	bool isRightPressed = stickX >= 0.5 || (allowDpad && IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_RIGHT));

#ifndef USE_SDL1
	if (ControlMode == ControlTypes::VirtualGamepad) {
		isUpPressed |= VirtualGamepadState.isActive && VirtualGamepadState.directionPad.isUpPressed;
		isDownPressed |= VirtualGamepadState.isActive && VirtualGamepadState.directionPad.isDownPressed;
		isLeftPressed |= VirtualGamepadState.isActive && VirtualGamepadState.directionPad.isLeftPressed;
		isRightPressed |= VirtualGamepadState.isActive && VirtualGamepadState.directionPad.isRightPressed;
	}
#endif

	if (isUpPressed) {
		result.y = AxisDirectionY_UP;
	} else if (isDownPressed) {
		result.y = AxisDirectionY_DOWN;
	}

	if (isLeftPressed) {
		result.x = AxisDirectionX_LEFT;
	} else if (isRightPressed) {
		result.x = AxisDirectionX_RIGHT;
	}

	return result;
}

} // namespace devilution
