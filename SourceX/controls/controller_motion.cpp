#include "controls/controller_motion.h"

#include "controls/devices/game_controller.h"
#include "controls/devices/joystick.h"
#include "controls/devices/kbcontroller.h"
#include "controls/controller.h"

namespace dvl {

namespace {

void ScaleJoystickAxes(float *x, float *y, float deadzone)
{
	//radial and scaled dead_zone
	//http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
	//input values go from -32767.0...+32767.0, output values are from -1.0 to 1.0;

	if (deadzone == 0) {
		return;
	}
	if (deadzone >= 1.0) {
		*x = 0;
		*y = 0;
		return;
	}

	const float maximum = 32767.0f;
	float analog_x = *x;
	float analog_y = *y;
	float dead_zone = deadzone * maximum;

	float magnitude = sqrtf(analog_x * analog_x + analog_y * analog_y);
	if (magnitude >= dead_zone) {
		// find scaled axis values with magnitudes between zero and maximum
		float scalingFactor = 1.0 / magnitude * (magnitude - dead_zone) / (maximum - dead_zone);
		analog_x = (analog_x * scalingFactor);
		analog_y = (analog_y * scalingFactor);

		// clamp to ensure results will never exceed the max_axis value
		float clamping_factor = 1.0f;
		float abs_analog_x = fabs(analog_x);
		float abs_analog_y = fabs(analog_y);
		if (abs_analog_x > 1.0 || abs_analog_y > 1.0) {
			if (abs_analog_x > abs_analog_y) {
				clamping_factor = 1 / abs_analog_x;
			} else {
				clamping_factor = 1 / abs_analog_y;
			}
		}
		*x = (clamping_factor * analog_x);
		*y = (clamping_factor * analog_y);
	} else {
		*x = 0;
		*y = 0;
	}
}

} // namespace

float leftStickX, leftStickY, rightStickX, rightStickY;
float leftStickXUnscaled, leftStickYUnscaled, rightStickXUnscaled, rightStickYUnscaled;
bool leftStickNeedsScaling, rightStickNeedsScaling;

void ScaleJoysticks()
{
	const float rightDeadzone = 0.07;
	const float leftDeadzone = 0.07;

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

// Updates motion state for mouse and joystick sticks.
bool ProcessControllerMotion(const SDL_Event &event)
{
#ifndef USE_SDL1
	if (ProcessGameControllerAxisMotion(event))
		return true;
#endif
	if (ProcessJoystickAxisMotion(event))
		return true;
#if HAS_KBCTRL == 1
	if (ProcessKbCtrlAxisMotion(event))
		return true;
#endif

	// SELECT + D-Pad simulating mouse movement.
	if (!IsControllerButtonPressed(ControllerButton_BUTTON_BACK)) {
		rightStickX = 0;
		rightStickY = 0;
		return false;
	}

	const ControllerButtonEvent ctrl_event = ToControllerButtonEvent(event);
	if (!IsDPadButton(ctrl_event.button))
		return false;
	if (IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_LEFT)) {
		rightStickX = -1;
	} else if (IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_RIGHT)) {
		rightStickX = 1;
	} else {
		rightStickX = 0;
	}
	if (IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_UP)) {
		rightStickY = 1;
	} else if (IsControllerButtonPressed(ControllerButton_BUTTON_DPAD_DOWN)) {
		rightStickY = -1;
	} else {
		rightStickY = 0;
	}
	return true;
}

} // namespace dvl
