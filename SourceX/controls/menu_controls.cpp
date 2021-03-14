#include "controls/menu_controls.h"

#include "controls/controller.h"
#include "controls/controller_motion.h"
#include "controls/remap_keyboard.h"
#include "DiabloUI/diabloui.h"

namespace dvl {

MenuAction GetStickMenuAction()
{
	const float stickY = leftStickY;
	const int minIntervalMs = 200;
	if (stickY >= 0.5) {
		static int last_up = 0;
		const int now = SDL_GetTicks();
		if (now - last_up >= minIntervalMs) {
			last_up = now;
			return MenuAction_UP;
		}
	} else if (stickY <= -0.5) {
		static int last_down = 0;
		const int now = SDL_GetTicks();
		if (now - last_down >= minIntervalMs) {
			last_down = now;
			return MenuAction_DOWN;
		}
	}
	return MenuAction_NONE;
}

MenuAction GetMenuAction(const SDL_Event &event)
{
	const ControllerButtonEvent ctrl_event = ToControllerButtonEvent(event);

	if (ProcessControllerMotion(event, ctrl_event)) {
		sgbControllerActive = true;
		return GetStickMenuAction();
	}

	if (ctrl_event.button != ControllerButton_NONE)
		sgbControllerActive = true;

	if (!ctrl_event.up) {
		switch (ctrl_event.button) {
		case ControllerButton_IGNORE:
			return MenuAction_NONE;
		case ControllerButton_BUTTON_B: // Right button
		case ControllerButton_BUTTON_START:
			return MenuAction_SELECT;
		case ControllerButton_BUTTON_BACK:
		case ControllerButton_BUTTON_A: // Bottom button
			return MenuAction_BACK;
		case ControllerButton_BUTTON_X: // Left button
			return MenuAction_DELETE;
		case ControllerButton_BUTTON_DPAD_UP:
			return MenuAction_UP;
		case ControllerButton_BUTTON_DPAD_DOWN:
			return MenuAction_DOWN;
		case ControllerButton_BUTTON_DPAD_LEFT:
			return MenuAction_LEFT;
		case ControllerButton_BUTTON_DPAD_RIGHT:
			return MenuAction_RIGHT;
		case ControllerButton_BUTTON_LEFTSHOULDER:
			return MenuAction_PAGE_UP;
		case ControllerButton_BUTTON_RIGHTSHOULDER:
			return MenuAction_PAGE_DOWN;
		default:
			break;
		}
	}

#if HAS_KBCTRL == 0
	if (event.type >= SDL_KEYDOWN && event.type < SDL_JOYAXISMOTION)
		sgbControllerActive = false;

	if (event.type == SDL_KEYDOWN) {
		auto sym = event.key.keysym.sym;
		remap_keyboard_key(&sym);
		switch (sym) {
		case SDLK_UP:
			return MenuAction_UP;
		case SDLK_DOWN:
			return MenuAction_DOWN;
		case SDLK_TAB:
			if (SDL_GetModState() & KMOD_SHIFT)
				return MenuAction_UP;
			else
				return MenuAction_DOWN;
		case SDLK_PAGEUP:
			return MenuAction_PAGE_UP;
		case SDLK_PAGEDOWN:
			return MenuAction_PAGE_DOWN;
		case SDLK_RETURN: {
			const Uint8 *state = SDLC_GetKeyState();
			if (!state[SDLC_KEYSTATE_LALT] && !state[SDLC_KEYSTATE_RALT]) {
				return MenuAction_SELECT;
			}
			break;
		}
		case SDLK_KP_ENTER:
			return MenuAction_SELECT;
		case SDLK_SPACE:
			if (!textInputActive) {
				return MenuAction_SELECT;
			}
			break;
		case SDLK_DELETE:
			return MenuAction_DELETE;
		case SDLK_LEFT:
			return MenuAction_LEFT;
		case SDLK_RIGHT:
			return MenuAction_RIGHT;
		case SDLK_ESCAPE:
			return MenuAction_BACK;
		default:
			break;
		}
	}
#endif

	return MenuAction_NONE;
} // namespace dvl

} // namespace dvl
