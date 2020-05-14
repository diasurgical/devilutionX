#include "controls/menu_controls.h"

#include "controls/controller.h"
#include "controls/remap_keyboard.h"

namespace dvl {

MenuActionNS::MenuAction GetMenuAction(const SDL_Event &event)
{
	const ControllerButtonEvent ctrl_event = ToControllerButtonEvent(event);
	if (ctrl_event.button != ControllerButtonNS::NONE)
		sgbControllerActive = true;

	if (!ctrl_event.up) {
		switch (ctrl_event.button) {
		case ControllerButtonNS::CBIGNORE:
			return MenuActionNS::NONE;
		case ControllerButtonNS::BUTTON_B: // Right button
		case  ControllerButtonNS::BUTTON_START:
			return MenuActionNS::SELECT;
		case  ControllerButtonNS::BUTTON_BACK:
		case  ControllerButtonNS::BUTTON_A: // Bottom button
			return MenuActionNS::BACK;
		case  ControllerButtonNS::BUTTON_X: // Left button
			return MenuActionNS::MADELETE;
		case  ControllerButtonNS::BUTTON_DPAD_UP:
			return MenuActionNS::UP;
		case  ControllerButtonNS::BUTTON_DPAD_DOWN:
			return MenuActionNS::DOWN;
		case  ControllerButtonNS::BUTTON_DPAD_LEFT:
			return MenuActionNS::LEFT;
		case  ControllerButtonNS::BUTTON_DPAD_RIGHT:
			return MenuActionNS::RIGHT;
		case  ControllerButtonNS::BUTTON_LEFTSHOULDER:
			return MenuActionNS::PAGE_UP;
		case  ControllerButtonNS::BUTTON_RIGHTSHOULDER:
			return MenuActionNS::PAGE_DOWN;
		default:
			break;
		}
	}

#if HAS_KBCTRL == 0
	if (event.type >= SDL_KEYDOWN && event.type < SDL_JOYAXISMOTION)
		sgbControllerActive = false;

	if (event.type == SDL_KEYDOWN) {
#ifdef USE_SDL1
		SDLKey sym = event.key.keysym.sym;
#else
		int sym = event.key.keysym.sym;
#endif
		remap_keyboard_key(&sym);
		switch (sym) {
		case SDLK_UP:
			return MenuActionNS::UP;
		case SDLK_DOWN:
			return MenuActionNS::DOWN;
		case SDLK_TAB:
			if (SDL_GetModState() & KMOD_SHIFT)
				return MenuActionNS::UP;
			else
				return MenuActionNS::DOWN;
		case SDLK_PAGEUP:
			return MenuActionNS::PAGE_UP;
		case SDLK_PAGEDOWN:
			return MenuActionNS::PAGE_DOWN;
		case SDLK_RETURN: {
			const Uint8 *state = SDLC_GetKeyState();
			if (!state[SDLC_KEYSTATE_LALT] && !state[SDLC_KEYSTATE_RALT]) {
				return MenuActionNS::SELECT;
			}
			break;
		}
		case SDLK_KP_ENTER:
			return MenuActionNS::SELECT;
		case SDLK_SPACE:
			if (!SDL_IsTextInputActive()) {
				return MenuActionNS::SELECT;
			}
			break;
		case SDLK_DELETE:
			return MenuActionNS::MADELETE;
		case SDLK_LEFT:
			return MenuActionNS::LEFT;
		case SDLK_RIGHT:
			return MenuActionNS::RIGHT;
		case SDLK_ESCAPE:
			return MenuActionNS::BACK;
		default:
			break;
		}
	}
#endif

	return MenuActionNS::NONE;
} // namespace dvl

} // namespace dvl
