#pragma once

#include "../player.h"

#include <fmt/format.h>

namespace fmt {

template <>
struct formatter<devilution::PLR_MODE> : formatter<string_view> {
	template <typename FormatContext>
	auto format(devilution::PLR_MODE mode, FormatContext &ctx)
	{
		string_view name = "unknown";
		switch (mode) {
		case devilution::PM_STAND:
			name = "stand";
			break;
		case devilution::PM_WALK:
			name = "walk (N, NW, or NE)";
			break;
		case devilution::PM_WALK2:
			name = "walk (S, SW, or SE)";
			break;
		case devilution::PM_WALK3:
			name = "walk (W or E)";
			break;
		case devilution::PM_ATTACK:
			name = "attack";
			break;
		case devilution::PM_RATTACK:
			name = "ranged attack";
			break;
		case devilution::PM_BLOCK:
			name = "block";
			break;
		case devilution::PM_GOTHIT:
			name = "got hit";
			break;
		case devilution::PM_DEATH:
			name = "death";
			break;
		case devilution::PM_SPELL:
			name = "spell";
			break;
		case devilution::PM_NEWLVL:
			name = "new level";
			break;
		case devilution::PM_QUIT:
			name = "quit";
			break;
		}
		return formatter<string_view>::format(name, ctx);
	}
};

}
