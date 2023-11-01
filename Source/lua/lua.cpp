#include "lua/lua.hpp"

#include <optional>
#include <string_view>

#include <sol/sol.hpp>

#include "appfat.h"
#include "engine/assets.hpp"
#include "lua/modules/log.hpp"
#include "lua/modules/render.hpp"
#include "plrmsg.h"
#include "utils/console.h"
#include "utils/log.hpp"

namespace devilution {

namespace {

std::optional<sol::state> luaState;

int LuaPrint(lua_State *state)
{
	const int n = lua_gettop(state);
	for (int i = 1; i <= n; i++) {
		size_t l;
		const char *s = luaL_tolstring(state, i, &l);
		if (i > 1)
			printInConsole("\t");
		printInConsole(std::string_view(s, l));
		lua_pop(state, 1);
	}
	printNewlineInConsole();
	return 0;
}

bool CheckResult(sol::protected_function_result result, bool optional)
{
	const bool valid = result.valid();
	if (!valid) {
		const std::string error = result.get_type() == sol::type::string
		    ? StrCat("Lua error: ", result.get<std::string>())
		    : "Unknown Lua error";
		if (!optional)
			app_fatal(error);
		LogError(error);
	}
	return valid;
}

void RunScript(std::string_view path, bool optional)
{
	AssetRef ref = FindAsset(path);
	if (!ref.ok()) {
		if (!optional)
			app_fatal(StrCat("Asset not found: ", path));
		return;
	}

	const size_t size = ref.size();
	std::unique_ptr<char[]> luaScript { new char[size] };

	AssetHandle handle = OpenAsset(std::move(ref));
	if (!handle.ok()) {
		app_fatal(StrCat("Failed to open asset: ", path, "\n", handle.error()));
		return;
	}

	if (size > 0 && !handle.read(luaScript.get(), size)) {
		app_fatal(StrCat("Read failed: ", path, "\n", handle.error()));
		return;
	}

	const std::string_view luaScriptStr(luaScript.get(), size);
	CheckResult(luaState->safe_script(luaScriptStr), optional);
}

void LuaPanic(sol::optional<std::string> message)
{
	LogError("Lua is in a panic state and will now abort() the application:\n",
	    message.value_or("unknown error"));
}

} // namespace

void Sol2DebugPrintStack(lua_State *state)
{
	LogDebug("{}", sol::detail::debug::dump_types(state));
}

void Sol2DebugPrintSection(const std::string &message, lua_State *state)
{
	LogDebug("-- {} -- [ {} ]", message, sol::detail::debug::dump_types(state));
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
	lua.set(
	    "print", LuaPrint,
	    "_VERSION", LUA_VERSION);

	// Registering devilutionx object table
	lua.create_named_table(
	    "devilutionx",
	    "log", LuaLogModule(lua),
	    "render", LuaRenderModule(lua),
	    "message", [](std::string_view text) { EventPlrMsg(text, UiFlags::ColorRed); });

	RunScript("lua/init.lua", /*optional=*/false);
	RunScript("lua/user.lua", /*optional=*/true);

	LuaEvent("OnGameBoot");
}

void LuaShutdown()
{
	luaState = std::nullopt;
}

void LuaEvent(std::string_view name)
{
	const sol::state &lua = *luaState;
	const auto trigger = lua.traverse_get<std::optional<sol::object>>("Events", name, "Trigger");
	if (!trigger.has_value() || !trigger->is<sol::protected_function>()) {
		LogError("Events.{}.Trigger is not a function", name);
		return;
	}
	const sol::protected_function fn = trigger->as<sol::protected_function>();
	CheckResult(fn(), /*optional=*/true);
}

sol::state &LuaState()
{
	return *luaState;
}

} // namespace devilution
