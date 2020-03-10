#pragma once

#include "all.h"

namespace dvl {

enum class MenuAction {
	NONE = 0,
	SELECT,
	BACK,
	DELETE,

	UP,
	DOWN,
	LEFT,
	RIGHT,

	PAGE_UP,
	PAGE_DOWN,
};

MenuAction GetMenuAction(const SDL_Event &event);

} // namespace dvl
