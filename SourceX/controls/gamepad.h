#pragma once

namespace dvl {

class Gamepad {
public:
	// Raw axis values.
	float leftStickX, leftStickY, rightStickX, rightStickY;
	// Axis values scaled to [-1, 1] range and clamped to a deadzone.
	float leftStickXUnscaled, leftStickYUnscaled, rightStickXUnscaled, rightStickYUnscaled;
	// Whether stick positions have been updated and need rescaling.
	bool leftStickNeedsScaling, rightStickNeedsScaling;
};

} // namespace dvl
