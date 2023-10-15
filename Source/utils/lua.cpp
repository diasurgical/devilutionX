#include "utils/lua.hpp"

#include <optional>
#include <string_view>

#include <sol/sol.hpp>

#include "engine/assets.hpp"
#include "plrmsg.h"
#include "utils/console.h"
#include "utils/log.hpp"

namespace devilution {

namespace {

std::optional<sol::state> luaState;

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

bool CheckResult(sol::protected_function_result result)
{
	const bool valid = result.valid();
	if (!valid) {
		if (result.get_type() == sol::type::string) {
			LogError("Lua error: {}", result.get<std::string>());
		} else {
			LogError("Unknown Lua error");
		}
	}
	return valid;
}

void RunScript(std::string_view path)
{
	AssetRef ref = FindAsset(path);
	if (!ref.ok())
		return;

	const size_t size = ref.size();
	std::unique_ptr<char[]> luaScript { new char[size] };

	AssetHandle handle = OpenAsset(std::move(ref));
	if (!handle.ok())
		return;

	if (size > 0 && !handle.read(luaScript.get(), size))
		return;

	const std::string_view luaScriptStr(luaScript.get(), size);
	CheckResult(luaState->safe_script(luaScriptStr));
}

void LuaPanic(sol::optional<std::string> maybe_msg)
{
	LogError("Lua is in a panic state and will now abort() the application:\n",
	    maybe_msg.value_or("unknown error"));
}

} // namespace

void Sol2DebugPrintStack(lua_State *L)
{
	LogDebug("{}", sol::detail::debug::dump_types(L));
}

void Sol2DebugPrintSection(const std::string &message, lua_State *L)
{
	LogDebug("-- {} -- [ {} ]", message, sol::detail::debug::dump_types(L));
}

void LuaInitialize()
{
	luaState.emplace(sol::c_call<decltype(&LuaPanic), &LuaPanic>);
	sol::state &lua = *luaState;
	lua.open_libraries(
	    sol::lib::base,
	    sol::lib::package,
	    sol::lib::coroutine,
	    sol::lib::table,
	    sol::lib::string,
	    sol::lib::math,
	    sol::lib::utf8);

#ifdef _DEBUG
	lua.open_libraries(sol::lib::debug);
#endif

	// Registering globals
	lua["print"] = LuaPrint;
	lua["_VERSION"] = LUA_VERSION;

	// Registering devilutionx object table
	sol::table devilutionx(lua, sol::create);
	devilutionx["message"] = LuaPlayerMessage;
	lua["devilutionx"] = devilutionx;

	RunScript("lua/init.lua");
	RunScript("lua/user.lua");

	LuaEvent("OnGameBoot");
}

void LuaShutdown()
{
	luaState = std::nullopt;
}

void LuaEvent(std::string_view name)
{
	sol::state &lua = *luaState;
	const sol::object events = lua["Events"];
	if (!events.is<sol::table>()) {
		LogError("Events table missing!");
		return;
	}
	const sol::object event = events.as<sol::table>()[name];
	if (!event.is<sol::table>()) {
		LogError("Events.{} event not registered", name);
		return;
	}
	const sol::object trigger = event.as<sol::table>()["Trigger"];
	if (!trigger.is<sol::function>()) {
		LogError("Events.{}.Trigger is not a function", name);
		return;
	}
	const sol::protected_function fn = trigger.as<sol::protected_function>();
	CheckResult(fn());
}

} // namespace devilution
