#include "lua/modules/render.hpp"

#include <sol/sol.hpp>

#include "engine/dx.h"
#include "engine/render/text_render.hpp"

namespace devilution {

sol::table LuaRenderModule(sol::state_view &lua)
{
	return lua.create_table_with(
	    "string", [](std::string_view text, int x, int y) { DrawString(GlobalBackBuffer(), text, { x, y }); });
}

} // namespace devilution
