#include "lua/modules/player.hpp"

#include <sol/sol.hpp>

#include "engine/point.hpp"
#include "lua/metadoc.hpp"
#include "player.h"

namespace devilution {

sol::table LuaPlayerModule(sol::state_view &lua)
{
	sol::table table = lua.create_table();
	SetDocumented(table, "walk_to", "(x: integer, y: integer)",
	    "Walk to the given coordinates",
	    [](int x, int y) {
		    NetSendCmdLoc(MyPlayerId, true, CMD_WALKXY, Point { x, y });
	    });
	return table;
}

} // namespace devilution
