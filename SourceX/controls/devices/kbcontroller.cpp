#include "controls/devices/kbcontroller.h"

#if HAS_KBCTRL == 1

#include "controls/controller_motion.h"
#include "sdl_compat.h"
#include "stubs.h"

namespace dvl {

ControllerButton KbCtrlToControllerButton(const SDL_Event &event)
{
	switch (event.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
#ifdef KBCTRL_IGNORE_1
		case KBCTRL_IGNORE_1:
			return ControllerButton_IGNORE;
#endif
#ifdef KBCTRL_BUTTON_A
		case KBCTRL_BUTTON_A:
			return ControllerButton_BUTTON_A;
#endif
#ifdef KBCTRL_BUTTON_B
		case KBCTRL_BUTTON_B: // Right button
			return ControllerButton_BUTTON_B;
#endif
#ifdef KBCTRL_BUTTON_X
		case KBCTRL_BUTTON_X: // Left button
			return ControllerButton_BUTTON_X;
#endif
#ifdef KBCTRL_BUTTON_Y
		case KBCTRL_BUTTON_Y:
			return ControllerButton_BUTTON_Y;
#endif
#ifdef KBCTRL_BUTTON_LEFTSTICK
		case KBCTRL_BUTTON_LEFTSTICK:
			return ControllerButton_BUTTON_LEFTSTICK;
#endif
#ifdef KBCTRL_BUTTON_RIGHTSTICK
		case KBCTRL_BUTTON_RIGHTSTICK:
			return ControllerButton_BUTTON_RIGHTSTICK;
#endif
#ifdef KBCTRL_BUTTON_LEFTSHOULDER
		case KBCTRL_BUTTON_LEFTSHOULDER:
			return ControllerButton_BUTTON_LEFTSHOULDER;
#endif
#ifdef KBCTRL_BUTTON_RIGHTSHOULDER
		case KBCTRL_BUTTON_RIGHTSHOULDER:
			return ControllerButton_BUTTON_RIGHTSHOULDER;
#endif
#ifdef KBCTRL_BUTTON_START
		case KBCTRL_BUTTON_START:
			return ControllerButton_BUTTON_START;
#endif
#ifdef KBCTRL_BUTTON_BACK
		case KBCTRL_BUTTON_BACK:
			return ControllerButton_BUTTON_BACK;
#endif
#ifdef KBCTRL_BUTTON_DPAD_UP
		case KBCTRL_BUTTON_DPAD_UP:
			return ControllerButton_BUTTON_DPAD_UP;
#endif
#ifdef KBCTRL_BUTTON_DPAD_DOWN
		case KBCTRL_BUTTON_DPAD_DOWN:
			return ControllerButton_BUTTON_DPAD_DOWN;
#endif
#ifdef KBCTRL_BUTTON_DPAD_LEFT
		case KBCTRL_BUTTON_DPAD_LEFT:
			return ControllerButton_BUTTON_DPAD_LEFT;
#endif
#ifdef KBCTRL_BUTTON_DPAD_RIGHT
		case KBCTRL_BUTTON_DPAD_RIGHT:
			return ControllerButton_BUTTON_DPAD_RIGHT;
#endif
		default:
			return ControllerButton_NONE;
		}
	default:
		return ControllerButton_NONE;
	}
}

namespace {

int ControllerButtonToKbCtrlKeyCode(ControllerButton button)
{
	if (button == ControllerButton_AXIS_TRIGGERLEFT || button == ControllerButton_AXIS_TRIGGERRIGHT)
		UNIMPLEMENTED();
	switch (button) {
#ifdef KBCTRL_BUTTON_A
	case ControllerButton_BUTTON_A:
		return KBCTRL_BUTTON_A;
#endif
#ifdef KBCTRL_BUTTON_B
	case ControllerButton_BUTTON_B:
		return KBCTRL_BUTTON_B;
#endif
#ifdef KBCTRL_BUTTON_X
	case ControllerButton_BUTTON_X:
		return KBCTRL_BUTTON_X;
#endif
#ifdef KBCTRL_BUTTON_Y
	case ControllerButton_BUTTON_Y:
		return KBCTRL_BUTTON_Y;
#endif
#ifdef KBCTRL_BUTTON_BACK
	case ControllerButton_BUTTON_BACK:
		return KBCTRL_BUTTON_BACK;
#endif
#ifdef KBCTRL_BUTTON_START
	case ControllerButton_BUTTON_START:
		return KBCTRL_BUTTON_START;
#endif
#ifdef KBCTRL_BUTTON_LEFTSTICK
	case ControllerButton_BUTTON_LEFTSTICK:
		return KBCTRL_BUTTON_LEFTSTICK;
#endif
#ifdef KBCTRL_BUTTON_RIGHTSTICK
	case ControllerButton_BUTTON_RIGHTSTICK:
		return KBCTRL_BUTTON_RIGHTSTICK;
#endif
#ifdef KBCTRL_BUTTON_LEFTSHOULDER
	case ControllerButton_BUTTON_LEFTSHOULDER:
		return KBCTRL_BUTTON_LEFTSHOULDER;
#endif
#ifdef KBCTRL_BUTTON_RIGHTSHOULDER
	case ControllerButton_BUTTON_RIGHTSHOULDER:
		return KBCTRL_BUTTON_RIGHTSHOULDER;
#endif
#ifdef KBCTRL_BUTTON_DPAD_UP
	case ControllerButton_BUTTON_DPAD_UP:
		return KBCTRL_BUTTON_DPAD_UP;
#endif
#ifdef KBCTRL_BUTTON_DPAD_DOWN
	case ControllerButton_BUTTON_DPAD_DOWN:
		return KBCTRL_BUTTON_DPAD_DOWN;
#endif
#ifdef KBCTRL_BUTTON_DPAD_LEFT
	case ControllerButton_BUTTON_DPAD_LEFT:
		return KBCTRL_BUTTON_DPAD_LEFT;
#endif
#ifdef KBCTRL_BUTTON_DPAD_RIGHT
	case ControllerButton_BUTTON_DPAD_RIGHT:
		return KBCTRL_BUTTON_DPAD_RIGHT;
#endif
	default:
		return -1;
	}
}

} // namespace

bool IsKbCtrlButtonPressed(ControllerButton button)
{
	int key_code = ControllerButtonToKbCtrlKeyCode(button);
	if (key_code == -1)
		return false;
#ifndef USE_SDL1
	return SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(key_code)];
#else
	return SDL_GetKeyState(NULL)[key_code];
#endif
}

bool ProcessKbCtrlAxisMotion(const SDL_Event &event)
{
	// Mapping keyboard to right stick axis not implemented.
	return false;
}

} // namespace dvl
#endif
