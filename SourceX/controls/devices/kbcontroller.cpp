#include "controls/devices/kbcontroller.h"

#if HAS_KBCTRL == 1

#include "controls/controller_motion.h"
#include "sdl2_to_1_2_backports.h"
#include "sdl_compat.h"
#include "stubs.h"

namespace dvl {

namespace {

bool IsModifierKey()
{
#ifdef KBCTRL_MODIFIER_KEY
	return SDLC_GetKeyState()[KBCTRL_MODIFIER_KEY];
#else
	return false;
#endif
}

} // namespace

ControllerButton KbCtrlToControllerButton(const SDL_Event &event)
{
	switch (event.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
#ifdef KBCTRL_IGNORE_1
		case KBCTRL_IGNORE_1:
			return ControllerButton::IGNORE;
#endif
#ifdef KBCTRL_MODIFIER_KEY
		case KBCTRL_MODIFIER_KEY:
			return ControllerButton::IGNORE;
#endif
#ifdef KBCTRL_BUTTON_A
		case KBCTRL_BUTTON_A:
			return ControllerButton::BUTTON_A;
#endif
#ifdef KBCTRL_BUTTON_B
		case KBCTRL_BUTTON_B: // Right button
			if (IsModifierKey())
				return ControllerButton::BUTTON_RIGHTSTICK;
			return ControllerButton::BUTTON_B;
#endif
#ifdef KBCTRL_BUTTON_X
		case KBCTRL_BUTTON_X: // Left button
			if (IsModifierKey())
				return ControllerButton::BUTTON_BACK;
			return ControllerButton::BUTTON_X;
#endif
#ifdef KBCTRL_BUTTON_Y
		case KBCTRL_BUTTON_Y:
			return ControllerButton::BUTTON_Y;
#endif
#ifdef KBCTRL_BUTTON_LEFTSTICK
		case KBCTRL_BUTTON_LEFTSTICK:
			return ControllerButton::BUTTON_LEFTSTICK;
#endif
#ifdef KBCTRL_BUTTON_RIGHTSTICK
		case KBCTRL_BUTTON_RIGHTSTICK:
			return ControllerButton::BUTTON_RIGHTSTICK;
#endif
#ifdef KBCTRL_BUTTON_LEFTSHOULDER
		case KBCTRL_BUTTON_LEFTSHOULDER:
			if (IsModifierKey())
				return ControllerButton::AXIS_TRIGGERLEFT;
			return ControllerButton::BUTTON_LEFTSHOULDER;
#endif
#ifdef KBCTRL_BUTTON_RIGHTSHOULDER
		case KBCTRL_BUTTON_RIGHTSHOULDER:
			if (IsModifierKey())
				return ControllerButton::AXIS_TRIGGERRIGHT;
			return ControllerButton::BUTTON_RIGHTSHOULDER;
#endif
#ifdef KBCTRL_BUTTON_START
		case KBCTRL_BUTTON_START:
			return ControllerButton::BUTTON_START;
#endif
#ifdef KBCTRL_BUTTON_BACK
		case KBCTRL_BUTTON_BACK:
			return ControllerButton::BUTTON_BACK;
#endif
#ifdef KBCTRL_BUTTON_DPAD_UP
		case KBCTRL_BUTTON_DPAD_UP:
			if (IsModifierKey())
				return ControllerButton::IGNORE;
			return ControllerButton::BUTTON_DPAD_UP;
#endif
#ifdef KBCTRL_BUTTON_DPAD_DOWN
		case KBCTRL_BUTTON_DPAD_DOWN:
			if (IsModifierKey())
				return ControllerButton::IGNORE;
			return ControllerButton::BUTTON_DPAD_DOWN;
#endif
#ifdef KBCTRL_BUTTON_DPAD_LEFT
		case KBCTRL_BUTTON_DPAD_LEFT:
			if (IsModifierKey())
				return ControllerButton::IGNORE;
			return ControllerButton::BUTTON_DPAD_LEFT;
#endif
#ifdef KBCTRL_BUTTON_DPAD_RIGHT
		case KBCTRL_BUTTON_DPAD_RIGHT:
			if (IsModifierKey())
				return ControllerButton::IGNORE;
			return ControllerButton::BUTTON_DPAD_RIGHT;
#endif
		default:
			return ControllerButton::NONE;
		}
	default:
		return ControllerButton::NONE;
	}
}

namespace {

int ControllerButtonToKbCtrlKeyCode(ControllerButton button)
{
	if (button == ControllerButton::AXIS_TRIGGERLEFT || button == ControllerButton::AXIS_TRIGGERRIGHT)
		UNIMPLEMENTED();
	switch (button) {
#ifdef KBCTRL_BUTTON_A
	case ControllerButton::BUTTON_A:
		return KBCTRL_BUTTON_A;
#endif
#ifdef KBCTRL_BUTTON_B
	case ControllerButton::BUTTON_B:
		return KBCTRL_BUTTON_B;
#endif
#ifdef KBCTRL_BUTTON_X
	case ControllerButton::BUTTON_X:
		return KBCTRL_BUTTON_X;
#endif
#ifdef KBCTRL_BUTTON_Y
	case ControllerButton::BUTTON_Y:
		return KBCTRL_BUTTON_Y;
#endif
#ifdef KBCTRL_BUTTON_BACK
	case ControllerButton::BUTTON_BACK:
		return KBCTRL_BUTTON_BACK;
#endif
#ifdef KBCTRL_BUTTON_START
	case ControllerButton::BUTTON_START:
		return KBCTRL_BUTTON_START;
#endif
#ifdef KBCTRL_BUTTON_LEFTSTICK
	case ControllerButton::BUTTON_LEFTSTICK:
		return KBCTRL_BUTTON_LEFTSTICK;
#endif
#ifdef KBCTRL_BUTTON_RIGHTSTICK
	case ControllerButton::BUTTON_RIGHTSTICK:
		return KBCTRL_BUTTON_RIGHTSTICK;
#endif
#ifdef KBCTRL_BUTTON_LEFTSHOULDER
	case ControllerButton::BUTTON_LEFTSHOULDER:
		return KBCTRL_BUTTON_LEFTSHOULDER;
#endif
#ifdef KBCTRL_BUTTON_RIGHTSHOULDER
	case ControllerButton::BUTTON_RIGHTSHOULDER:
		return KBCTRL_BUTTON_RIGHTSHOULDER;
#endif
#ifdef KBCTRL_BUTTON_DPAD_UP
	case ControllerButton::BUTTON_DPAD_UP:
		return KBCTRL_BUTTON_DPAD_UP;
#endif
#ifdef KBCTRL_BUTTON_DPAD_DOWN
	case ControllerButton::BUTTON_DPAD_DOWN:
		return KBCTRL_BUTTON_DPAD_DOWN;
#endif
#ifdef KBCTRL_BUTTON_DPAD_LEFT
	case ControllerButton::BUTTON_DPAD_LEFT:
		return KBCTRL_BUTTON_DPAD_LEFT;
#endif
#ifdef KBCTRL_BUTTON_DPAD_RIGHT
	case ControllerButton::BUTTON_DPAD_RIGHT:
		return KBCTRL_BUTTON_DPAD_RIGHT;
#endif
	default:
		return -1;
	}
}

bool IsButtonPressed(ControllerButton button)
{
	int key_code = ControllerButtonToKbCtrlKeyCode(button);
	if (key_code == -1)
		return false;
#ifndef USE_SDL1
	return SDL_GetKeyboardState(nullptr)[SDL_GetScancodeFromKey(key_code)];
#else
	return SDL_GetKeyState(nullptr)[key_code];
#endif
}

} // namespace

bool IsKbCtrlButtonPressed(ControllerButton button)
{
	if (IsModifierKey() && (button == ControllerButton::BUTTON_DPAD_UP || button == ControllerButton::BUTTON_DPAD_DOWN || button == ControllerButton::BUTTON_DPAD_LEFT || button == ControllerButton::BUTTON_DPAD_RIGHT))
		return false;
	return IsButtonPressed(button);
}

bool ProcessKbCtrlAxisMotion(const SDL_Event &event)
{
	if (!IsModifierKey()) {
		rightStickX = 0;
		rightStickY = 0;
		return false;
	}
	if (event.type != SDL_KEYUP && event.type != SDL_KEYDOWN)
		return false;
	const auto sym = event.key.keysym.sym;
	if (sym != KBCTRL_BUTTON_DPAD_UP && sym != KBCTRL_BUTTON_DPAD_DOWN && sym != KBCTRL_BUTTON_DPAD_LEFT && sym != KBCTRL_BUTTON_DPAD_RIGHT)
		return false;
	if (IsButtonPressed(ControllerButton::BUTTON_DPAD_LEFT)) {
		rightStickX = -1;
	} else if (IsButtonPressed(ControllerButton::BUTTON_DPAD_RIGHT)) {
		rightStickX = 1;
	} else {
		rightStickX = 0;
	}
	if (IsButtonPressed(ControllerButton::BUTTON_DPAD_UP)) {
		rightStickY = 1;
	} else if (IsButtonPressed(ControllerButton::BUTTON_DPAD_DOWN)) {
		rightStickY = -1;
	} else {
		rightStickY = 0;
	}
	return true;
}


} // namespace dvl
#endif
