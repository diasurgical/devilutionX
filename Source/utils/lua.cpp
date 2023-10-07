#include "utils/lua.hpp"

#include <string_view>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "engine/assets.hpp"
#include "plrmsg.h"
#include "utils/console.h"

namespace devilution {

namespace {

lua_State *LuaState;

int LuaPrint(lua_State *state)
{
	int nargs = lua_gettop(state);
	if (nargs >= 1 && lua_isstring(state, 1)) {
		std::string msg = lua_tostring(state, 1);
		msg += "\n";
		printInConsole(msg);
	}

	return 0;
}

int LuaPlayerMessage(lua_State *state)
{
	int nargs = lua_gettop(state);
	if (nargs >= 1 && lua_isstring(state, 1)) {
		std::string_view msg = lua_tostring(state, 1);
		EventPlrMsg(msg, UiFlags::ColorRed);
	}

	return 0;
}

void RunScript(std::string_view path)
{
	AssetRef ref = FindAsset(path);
	if (!ref.ok())
		return;

	const size_t size = ref.size();
	std::unique_ptr<char[]> luaScript { new char[size + 1] };

	AssetHandle handle = OpenAsset(std::move(ref));
	if (!handle.ok())
		return;

	if (size > 0 && !handle.read(luaScript.get(), size))
		return;

	luaScript[size] = '\0'; // Terminate string

	int status = luaL_loadstring(LuaState, luaScript.get());
	if (status == LUA_OK)
		status = lua_pcall(LuaState, 0, 0, 0);
	if (status != LUA_OK)
		SDL_Log("%s", lua_tostring(LuaState, -1));
}

} // namespace

void LuaInitialize()
{
	LuaState = luaL_newstate();

	// Load libraries
	luaL_requiref(LuaState, LUA_GNAME, luaopen_base, 1);
	lua_pop(LuaState, 1);
	luaL_requiref(LuaState, LUA_LOADLIBNAME, luaopen_package, 1);
	lua_pop(LuaState, 1);
	luaL_requiref(LuaState, LUA_COLIBNAME, luaopen_coroutine, 1);
	lua_pop(LuaState, 1);
	luaL_requiref(LuaState, LUA_TABLIBNAME, luaopen_table, 1);
	lua_pop(LuaState, 1);
	luaL_requiref(LuaState, LUA_STRLIBNAME, luaopen_string, 1);
	lua_pop(LuaState, 1);
	luaL_requiref(LuaState, LUA_MATHLIBNAME, luaopen_math, 1);
	lua_pop(LuaState, 1);
	luaL_requiref(LuaState, LUA_UTF8LIBNAME, luaopen_utf8, 1);
	lua_pop(LuaState, 1);

#ifdef _DEBUG
	luaL_requiref(LuaState, LUA_DBLIBNAME, luaopen_debug, 1);
	lua_pop(LuaState, 1);
#endif

	// Registering globals
	lua_register(LuaState, "print", LuaPrint);
	lua_pushstring(LuaState, LUA_VERSION);
	lua_setglobal(LuaState, "_VERSION");

	// Registering devilutionx object table
	lua_newtable(LuaState);
	lua_pushcfunction(LuaState, LuaPlayerMessage);
	lua_setfield(LuaState, -2, "message");
	lua_setglobal(LuaState, "devilutionx");

	RunScript("lua/init.lua");
	RunScript("lua/user.lua");

	LuaEvent("OnGameBoot");
}

void LuaShutdown()
{
	if (LuaState == nullptr)
		return;

	lua_close(LuaState);
}

void LuaEvent(std::string name)
{
	lua_getglobal(LuaState, "Events");
	if (!lua_istable(LuaState, -1)) {
		lua_pop(LuaState, 1);
		SDL_Log("Events table missing!");
		return;
	}
	lua_getfield(LuaState, -1, name.c_str());
	if (!lua_istable(LuaState, -1)) {
		lua_pop(LuaState, 2);
		SDL_Log("Events.%s event not registered", name.c_str());
		return;
	}
	lua_getfield(LuaState, -1, "Trigger");
	if (!lua_isfunction(LuaState, -1)) {
		lua_pop(LuaState, 3);
		SDL_Log("Events.%s.Trigger is not a function", name.c_str());
		return;
	}
	if (lua_pcall(LuaState, 0, 0, 0) != LUA_OK) {
		SDL_Log("%s", lua_tostring(LuaState, -1));
	}
	lua_pop(LuaState, 2);
}

} // namespace devilution
