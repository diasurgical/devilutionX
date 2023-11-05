#ifdef _DEBUG
#include "lua/modules/dev.hpp"

#include <sol/sol.hpp>

#include "lua/metadoc.hpp"
#include "lua/modules/dev/display.hpp"
#include "lua/modules/dev/items.hpp"
#include "lua/modules/dev/level.hpp"
#include "lua/modules/dev/monsters.hpp"
#include "lua/modules/dev/player.hpp"
#include "lua/modules/dev/quests.hpp"
#include "lua/modules/dev/search.hpp"
#include "lua/modules/dev/towners.hpp"

namespace devilution {

sol::table LuaDevModule(sol::state_view &lua)
{
	sol::table table = lua.create_table();
	SetDocumented(table, "display", "", "Debugging HUD and rendering commands.", LuaDevDisplayModule(lua));
	SetDocumented(table, "items", "", "Item-related commands.", LuaDevItemsModule(lua));
	SetDocumented(table, "level", "", "Level-related commands.", LuaDevLevelModule(lua));
	SetDocumented(table, "monsters", "", "Monster-related commands.", LuaDevMonstersModule(lua));
	SetDocumented(table, "player", "", "Player-related commands.", LuaDevPlayerModule(lua));
	SetDocumented(table, "quests", "", "Quest-related commands.", LuaDevQuestsModule(lua));
	SetDocumented(table, "search", "", "Search the map for monsters / items / objects.", LuaDevSearchModule(lua));
	SetDocumented(table, "towners", "", "Town NPC commands.", LuaDevTownersModule(lua));
	return table;
}

} // namespace devilution
#endif // _DEBUG
