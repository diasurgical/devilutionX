#include "utils/lua.hpp"

#include <optional>
#include <string_view>

#include <sol/sol.hpp>

#include "engine/assets.hpp"
#include "engine/dx.h"
#include "engine/render/text_render.hpp"
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
	    "message", [](std::string_view text) { EventPlrMsg(text, UiFlags::ColorRed); }
	    // TODO: Re-enable once https://github.com/bebbo/amiga-gcc/issues/363 is fixed.
	    // "drawString", [](std::string_view text, int x, int y) { DrawString(GlobalBackBuffer(), text, { x, y }); }
	);

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
	const sol::state &lua = *luaState;
	const auto trigger = lua.traverse_get<std::optional<sol::object>>("Events", name, "Trigger");
	if (!trigger.has_value() || !trigger->is<sol::protected_function>()) {
		LogError("Events.{}.Trigger is not a function", name);
		return;
	}
	const sol::protected_function fn = trigger->as<sol::protected_function>();
	CheckResult(fn());
}

} // namespace devilution
