#include "controls/menu_controls.h"

#include "controls/controller.h"

namespace dvl {

MenuAction GetMenuAction(const SDL_Event &event)
{
	const ControllerButtonEvent ctrl_event = ToControllerButtonEvent(event);
	sgbControllerActive = true;
	if (!ctrl_event.up) {
		switch (ctrl_event.button) {
		case ControllerButton::IGNORE:
			return MenuAction::NONE;
		case ControllerButton::BUTTON_B: // Right button
		case ControllerButton::BUTTON_START:
			return MenuAction::SELECT;
		case ControllerButton::BUTTON_BACK:
		case ControllerButton::BUTTON_A: // Bottom button
			return MenuAction::BACK;
		case ControllerButton::BUTTON_X: // Left button
			return MenuAction::DELETE;
		case ControllerButton::BUTTON_DPAD_UP:
			return MenuAction::UP;
		case ControllerButton::BUTTON_DPAD_DOWN:
			return MenuAction::DOWN;
		case ControllerButton::BUTTON_DPAD_LEFT:
			return MenuAction::LEFT;
		case ControllerButton::BUTTON_DPAD_RIGHT:
			return MenuAction::RIGHT;
		case ControllerButton::BUTTON_LEFTSHOULDER:
			return MenuAction::PAGE_UP;
		case ControllerButton::BUTTON_RIGHTSHOULDER:
			return MenuAction::PAGE_DOWN;
		default:
			break;
		}
	}
	if (ctrl_event.button == ControllerButton::NONE && (event.type < SDL_JOYAXISMOTION || event.type >= 0x700))
		sgbControllerActive = false;

#if HAS_KBCTRL == 0
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_UP:
			return MenuAction::UP;
		case SDLK_DOWN:
			return MenuAction::DOWN;
		case SDLK_TAB:
			if (SDL_GetModState() & KMOD_SHIFT)
				return MenuAction::UP;
			else
				return MenuAction::DOWN;
		case SDLK_PAGEUP:
			return MenuAction::PAGE_UP;
		case SDLK_PAGEDOWN:
			return MenuAction::PAGE_DOWN;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_SPACE:
			return MenuAction::SELECT;
		case SDLK_DELETE:
			return MenuAction::DELETE;
		case SDLK_LEFT:
			return MenuAction::LEFT;
		case SDLK_RIGHT:
			return MenuAction::RIGHT;
		case SDLK_ESCAPE:
			return MenuAction::BACK;
		default:
			break;
		}
	}
#endif

	return MenuAction::NONE;
} // namespace dvl

} // namespace dvl
