#include "lua/modules/render.hpp"

#include <sol/sol.hpp>

#include "effects.h"

namespace devilution {

namespace {

bool IsValidSfx(int16_t psfx)
{
	return psfx >= 0 && psfx <= static_cast<int16_t>(SfxID::LAST);
}

} // namespace

sol::table LuaAudioModule(sol::state_view &lua)
{
	return lua.create_table_with(
	    "playSfx", [](int psfx) { if (IsValidSfx(psfx)) PlaySFX(static_cast<SfxID>(psfx)); },
	    "playSfxLoc", [](int psfx, int x, int y) { if (IsValidSfx(psfx)) PlaySfxLoc(static_cast<SfxID>(psfx), { x, y }); });
}

} // namespace devilution
