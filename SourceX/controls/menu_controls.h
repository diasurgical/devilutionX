#pragma once

#include "all.h"

namespace dvl {
namespace MenuActionNS {
enum MenuAction {
	NONE = 0,
	SELECT,
	BACK,
	MADELETE,

	UP,
	DOWN,
	LEFT,
	RIGHT,

	PAGE_UP,
	PAGE_DOWN,
};
}

MenuActionNS::MenuAction GetMenuAction(const SDL_Event &event);

} // namespace dvl
