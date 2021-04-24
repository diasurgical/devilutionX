#pragma once

#include <SDL.h>
#include <stdint.h>
#include <string_view>

namespace devilution {

enum MenuAction : uint8_t {
	MenuAction_NONE,
	MenuAction_SELECT,
	MenuAction_BACK,
	MenuAction_DELETE,

	MenuAction_UP,
	MenuAction_DOWN,
	MenuAction_LEFT,
	MenuAction_RIGHT,

	MenuAction_PAGE_UP,
	MenuAction_PAGE_DOWN,
};

constexpr std::string_view toString(MenuAction value)
{
	switch(value) {
	case MenuAction_NONE:
		return "None";
	case MenuAction_SELECT:
		return "Select";
	case MenuAction_BACK:
		return "Back";
	case MenuAction_DELETE:
		return "Delete";
	case MenuAction_UP:
		return "Up";
	case MenuAction_DOWN:
		return "Down";
	case MenuAction_LEFT:
		return "Left";
	case MenuAction_RIGHT:
		return "Right";
	case MenuAction_PAGE_UP:
		return "Page Up";
	case MenuAction_PAGE_DOWN:
		return "Page Down";
	}
}

MenuAction GetMenuAction(const SDL_Event &event);

/** Menu action from holding the left stick or DPad. */
MenuAction GetMenuHeldUpDownAction();

} // namespace devilution
