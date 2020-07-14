
#include "controls/devices/joystick.h"

#include "controls/controller_motion.h"
#include "stubs.h"

namespace dvl {

ControllerButton JoyButtonToControllerButton(const SDL_Event &event)
{
	switch (event.type) {
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		switch (event.jbutton.button) {
#ifdef JOY_BUTTON_A
		case JOY_BUTTON_A:
			return ControllerButton::BUTTON_A;
#endif
#ifdef JOY_BUTTON_B
		case JOY_BUTTON_B:
			return ControllerButton::BUTTON_B;
#endif
#ifdef JOY_BUTTON_X
		case JOY_BUTTON_X:
			return ControllerButton::BUTTON_X;
#endif
#ifdef JOY_BUTTON_Y
		case JOY_BUTTON_Y:
			return ControllerButton::BUTTON_Y;
#endif
#ifdef JOY_BUTTON_LEFTSTICK
		case JOY_BUTTON_LEFTSTICK:
			return ControllerButton::BUTTON_LEFTSTICK;
#endif
#ifdef JOY_BUTTON_RIGHTSTICK
		case JOY_BUTTON_RIGHTSTICK:
			return ControllerButton::BUTTON_RIGHTSTICK;
#endif
#ifdef JOY_BUTTON_LEFTSHOULDER
		case JOY_BUTTON_LEFTSHOULDER:
			return ControllerButton::BUTTON_LEFTSHOULDER;
#endif
#ifdef JOY_BUTTON_RIGHTSHOULDER
		case JOY_BUTTON_RIGHTSHOULDER:
			return ControllerButton::BUTTON_RIGHTSHOULDER;
#endif
#ifdef JOY_BUTTON_TRIGGERLEFT
		case JOY_BUTTON_TRIGGERLEFT:
			return ControllerButton::AXIS_TRIGGERLEFT;
#endif
#ifdef JOY_BUTTON_TRIGGERRIGHT
		case JOY_BUTTON_TRIGGERRIGHT:
			return ControllerButton::AXIS_TRIGGERRIGHT;
#endif
#ifdef JOY_BUTTON_START
		case JOY_BUTTON_START:
			return ControllerButton::BUTTON_START;
#endif
#ifdef JOY_BUTTON_BACK
		case JOY_BUTTON_BACK:
			return ControllerButton::BUTTON_BACK;
#endif
#ifdef JOY_BUTTON_DPAD_LEFT
		case JOY_BUTTON_DPAD_LEFT:
			return ControllerButton::BUTTON_DPAD_LEFT;
#endif
#ifdef JOY_BUTTON_DPAD_UP
		case JOY_BUTTON_DPAD_UP:
			return ControllerButton::BUTTON_DPAD_UP;
#endif
#ifdef JOY_BUTTON_DPAD_RIGHT
		case JOY_BUTTON_DPAD_RIGHT:
			return ControllerButton::BUTTON_DPAD_RIGHT;
#endif
#ifdef JOY_BUTTON_DPAD_DOWN
		case JOY_BUTTON_DPAD_DOWN:
			return ControllerButton::BUTTON_DPAD_DOWN;
#endif
		default:
			break;
		}
		break;
	case SDL_JOYHATMOTION:
#if defined(JOY_HAT_DPAD_UP_HAT) && defined(JOY_HAT_DPAD_UP)
		if (event.jhat.hat == JOY_HAT_DPAD_UP_HAT && (event.jhat.value & JOY_HAT_DPAD_UP) != 0)
			return ControllerButton::BUTTON_DPAD_UP;
#endif
#if defined(JOY_HAT_DPAD_DOWN_HAT) && defined(JOY_HAT_DPAD_DOWN)
		if (event.jhat.hat == JOY_HAT_DPAD_DOWN_HAT && (event.jhat.value & JOY_HAT_DPAD_DOWN) != 0)
			return ControllerButton::BUTTON_DPAD_DOWN;
#endif
#if defined(JOY_HAT_DPAD_LEFT_HAT) && defined(JOY_HAT_DPAD_LEFT)
		if (event.jhat.hat == JOY_HAT_DPAD_LEFT_HAT && (event.jhat.value & JOY_HAT_DPAD_LEFT) != 0)
			return ControllerButton::BUTTON_DPAD_LEFT;
#endif
#if defined(JOY_HAT_DPAD_RIGHT_HAT) && defined(JOY_HAT_DPAD_RIGHT)
		if (event.jhat.hat == JOY_HAT_DPAD_RIGHT_HAT && (event.jhat.value & JOY_HAT_DPAD_RIGHT) != 0)
			return ControllerButton::BUTTON_DPAD_RIGHT;
#endif
		return ControllerButton::IGNORE;
		break;
	default:
		break;
	}
	return ControllerButton::NONE;
}

namespace {

int JoyButtonToControllerButton(ControllerButton button)
{
	if (button == ControllerButton::AXIS_TRIGGERLEFT || button == ControllerButton::AXIS_TRIGGERRIGHT)
		UNIMPLEMENTED();
	switch (button) {
#ifdef JOY_BUTTON_A
	case ControllerButton::BUTTON_A:
		return JOY_BUTTON_A;
#endif
#ifdef JOY_BUTTON_B
	case ControllerButton::BUTTON_B:
		return JOY_BUTTON_B;
#endif
#ifdef JOY_BUTTON_X
	case ControllerButton::BUTTON_X:
		return JOY_BUTTON_X;
#endif
#ifdef JOY_BUTTON_Y
	case ControllerButton::BUTTON_Y:
		return JOY_BUTTON_Y;
#endif
#ifdef JOY_BUTTON_BACK
	case ControllerButton::BUTTON_BACK:
		return JOY_BUTTON_BACK;
#endif
#ifdef JOY_BUTTON_START
	case ControllerButton::BUTTON_START:
		return JOY_BUTTON_START;
#endif
#ifdef JOY_BUTTON_LEFTSTICK
	case ControllerButton::BUTTON_LEFTSTICK:
		return JOY_BUTTON_LEFTSTICK;
#endif
#ifdef JOY_BUTTON_RIGHTSTICK
	case ControllerButton::BUTTON_RIGHTSTICK:
		return JOY_BUTTON_RIGHTSTICK;
#endif
#ifdef JOY_BUTTON_LEFTSHOULDER
	case ControllerButton::BUTTON_LEFTSHOULDER:
		return JOY_BUTTON_LEFTSHOULDER;
#endif
#ifdef JOY_BUTTON_RIGHTSHOULDER
	case ControllerButton::BUTTON_RIGHTSHOULDER:
		return JOY_BUTTON_RIGHTSHOULDER;
#endif
#ifdef JOY_BUTTON_DPAD_UP
	case ControllerButton::BUTTON_DPAD_UP:
		return JOY_BUTTON_DPAD_UP;
#endif
#ifdef JOY_BUTTON_DPAD_DOWN
	case ControllerButton::BUTTON_DPAD_DOWN:
		return JOY_BUTTON_DPAD_DOWN;
#endif
#ifdef JOY_BUTTON_DPAD_LEFT
	case ControllerButton::BUTTON_DPAD_LEFT:
		return JOY_BUTTON_DPAD_LEFT;
#endif
#ifdef JOY_BUTTON_DPAD_RIGHT
	case ControllerButton::BUTTON_DPAD_RIGHT:
		return JOY_BUTTON_DPAD_RIGHT;
#endif
	default:
		return -1;
	}
}

bool IsJoystickHatButtonPressed(ControllerButton button)
{
	switch (button) {
#if defined(JOY_HAT_DPAD_UP_HAT) && defined(JOY_HAT_DPAD_UP)
	case ControllerButton::BUTTON_DPAD_UP:
		return (SDL_JoystickGetHat(CurrentJoystick(), JOY_HAT_DPAD_UP_HAT) & JOY_HAT_DPAD_UP) != 0;
#endif
#if defined(JOY_HAT_DPAD_DOWN_HAT) && defined(JOY_HAT_DPAD_DOWN)
	case ControllerButton::BUTTON_DPAD_DOWN:
		return (SDL_JoystickGetHat(CurrentJoystick(), JOY_HAT_DPAD_DOWN_HAT) & JOY_HAT_DPAD_DOWN) != 0;
#endif
#if defined(JOY_HAT_DPAD_LEFT_HAT) && defined(JOY_HAT_DPAD_LEFT)
	case ControllerButton::BUTTON_DPAD_LEFT:
		return (SDL_JoystickGetHat(CurrentJoystick(), JOY_HAT_DPAD_LEFT_HAT) & JOY_HAT_DPAD_LEFT) != 0;
#endif
#if defined(JOY_HAT_DPAD_RIGHT_HAT) && defined(JOY_HAT_DPAD_RIGHT)
	case ControllerButton::BUTTON_DPAD_RIGHT:
		return (SDL_JoystickGetHat(CurrentJoystick(), JOY_HAT_DPAD_RIGHT_HAT) & JOY_HAT_DPAD_RIGHT) != 0;
#endif
	default:
		return false;
	}
}

} // namespace

bool IsJoystickButtonPressed(ControllerButton button)
{
	if (CurrentJoystick() == nullptr)
		return false;
	if (IsJoystickHatButtonPressed(button))
		return true;
	const int joy_button = JoyButtonToControllerButton(button);
	return joy_button != -1 && SDL_JoystickGetButton(CurrentJoystick(), joy_button);
}

bool ProcessJoystickAxisMotion(const SDL_Event &event)
{
	if (event.type != SDL_JOYAXISMOTION)
		return false;
	switch (event.jaxis.axis) {
#ifdef JOY_AXIS_LEFTX
	case JOY_AXIS_LEFTX:
		leftStickXUnscaled = event.jaxis.value;
		leftStickNeedsScaling = true;
		break;
#endif
#ifdef JOY_AXIS_LEFTY
	case JOY_AXIS_LEFTY:
		leftStickYUnscaled = -event.jaxis.value;
		leftStickNeedsScaling = true;
		break;
#endif
#ifdef JOY_AXIS_RIGHTX
	case JOY_AXIS_RIGHTX:
		rightStickXUnscaled = event.jaxis.value;
		rightStickNeedsScaling = true;
		break;
#endif
#ifdef JOY_AXIS_RIGHTY
	case JOY_AXIS_RIGHTY:
		rightStickYUnscaled = -event.jaxis.value;
		rightStickNeedsScaling = true;
		break;
#endif
	default:
		return false;
	}
	return true;
}

static SDL_Joystick *current_joystick = nullptr;

SDL_Joystick *CurrentJoystick()
{
	return current_joystick;
}

static int current_joystick_index = -1;

int CurrentJoystickIndex()
{
	return current_joystick_index;
}

void InitJoystick()
{
	if (SDL_NumJoysticks() == 0)
		return;

	// Get the first available controller.
	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
#ifndef USE_SDL1
		if (!SDL_IsGameController(i))
			continue;
#endif
		SDL_Log("Initializing joystick %d: %s", i, SDL_JoystickNameForIndex(i));
		current_joystick = SDL_JoystickOpen(i);
		if (current_joystick == nullptr) {
			SDL_Log(SDL_GetError());
			continue;
		}
		current_joystick_index = i;
		break;
	}
}

} // namespace dvl
