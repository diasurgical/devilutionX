#include "utils/lua.hpp"

#include <optional>
#include <string_view>

#include <fmt/args.h>
#include <fmt/format.h>
#include <sol/sol.hpp>
#include <sol/utility/to_string.hpp>

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

void LuaPanic(sol::optional<std::string> message)
{
	LogError("Lua is in a panic state and will now abort() the application:\n",
	    message.value_or("unknown error"));
}

void LuaLogMessage(LogPriority priority, std::string_view fmt, sol::variadic_args args)
{
	std::string formatted;
	FMT_TRY
	{
		fmt::dynamic_format_arg_store<fmt::format_context> store;
		for (const sol::stack_proxy arg : args) {
			switch (arg.get_type()) {
			case sol::type::boolean:
				store.push_back(arg.as<bool>());
				break;
			case sol::type::number:
				if (lua_isinteger(arg.lua_state(), arg.stack_index())) {
					store.push_back(lua_tointeger(arg.lua_state(), arg.stack_index()));
				} else {
					store.push_back(lua_tonumber(arg.lua_state(), arg.stack_index()));
				}
				break;
			case sol::type::string:
				store.push_back(arg.as<std::string>());
				break;
			default:
				store.push_back(sol::utility::to_string(sol::stack_object(arg)));
				break;
			}
		}
		formatted = fmt::vformat(fmt, store);
	}
	FMT_CATCH(const fmt::format_error &e)
	{
#if FMT_EXCEPTIONS
		// e.what() is undefined if exceptions are disabled, so we wrap the whole block
		// with an `FMT_EXCEPTIONS` check.
		std::string error = StrCat("Format error, fmt: ", fmt, " error: ", e.what());
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", error.c_str());
		return;
#endif
	}
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, static_cast<SDL_LogPriority>(priority), "%s", formatted.c_str());
}

void LuaLog(std::string_view fmt, sol::variadic_args args)
{
	LuaLogMessage(LogPriority::Info, fmt, std::move(args));
}
void LuaLogVerbose(std::string_view fmt, sol::variadic_args args)
{
	LuaLogMessage(LogPriority::Verbose, fmt, std::move(args));
}
void LuaLogDebug(std::string_view fmt, sol::variadic_args args)
{
	LuaLogMessage(LogPriority::Debug, fmt, std::move(args));
}
void LuaLogWarn(std::string_view fmt, sol::variadic_args args)
{
	LuaLogMessage(LogPriority::Warn, fmt, std::move(args));
}
void LuaLogError(std::string_view fmt, sol::variadic_args args)
{
	LuaLogMessage(LogPriority::Error, fmt, std::move(args));
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
	    "message", [](std::string_view text) { EventPlrMsg(text, UiFlags::ColorRed); },
	    "drawString", [](std::string_view text, int x, int y) { DrawString(GlobalBackBuffer(), text, { x, y }); },
	    "log", LuaLog,
	    "logVerbose", LuaLogVerbose,
	    "logDebug", LuaLogDebug,
	    "logWarn", LuaLogWarn,
	    "logError", LuaLogError);

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

tl::expected<std::string, std::string> RunLua(std::string_view code)
{
	sol::state &lua = *luaState;
	const sol::protected_function_result result = lua.safe_script(code);
	const bool valid = result.valid();
	if (!valid) {
		if (result.get_type() == sol::type::string) {
			return tl::make_unexpected(result.get<std::string>());
		}
		return tl::make_unexpected("Unknown Lua error");
	}

	return sol::utility::to_string(sol::stack_object(result));
}

} // namespace devilution
