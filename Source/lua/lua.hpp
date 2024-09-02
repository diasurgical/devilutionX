#pragma once

#include <ankerl/unordered_dense.h>
#include <optional>
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <string_view>

namespace devilution {

// Forward declaration of LuaState struct.
struct LuaState {
	sol::state sol = {};
	sol::table commonPackages = {};
	ankerl::unordered_dense::segmented_map<std::string, sol::bytecode> compiledScripts = {};
	sol::environment sandbox = {};
	sol::table events = {};
	std::unordered_map<std::string, sol::function> preHooks;
	std::unordered_map<std::string, sol::function> postHooks;
	std::unordered_map<std::string, sol::function> functionOverrides;
};

// Declare CurrentLuaState as external within the devilution namespace.
extern std::optional<LuaState> CurrentLuaState;

// Lua initialization and shutdown
void LuaInitialize();
void LuaShutdown();

// Lua event handling
void LuaEvent(std::string_view name);

// Lua state management
sol::state &GetLuaState();
sol::environment CreateLuaSandbox();
sol::object SafeCallResult(sol::protected_function_result result, bool optional);

template <typename... Args>
sol::object SafeCallLuaFunction(sol::function luaFunction, Args &&...args)
{
	sol::protected_function_result result = luaFunction(std::forward<Args>(args)...);
	return SafeCallResult(std::move(result), /*optional=*/true);
}

// Lua hook handling
template <typename... Args>
bool LuaPreHook(const std::string &functionName, Args &&...args)
{
	auto &luaState = *CurrentLuaState;

	// Check for function override first
	if (luaState.functionOverrides.find(functionName) != luaState.functionOverrides.end()) {
		SafeCallLuaFunction(luaState.functionOverrides[functionName], std::forward<Args>(args)...);
		return true; // Skip original logic
	}

	// Check for pre-hook
	if (luaState.preHooks.find(functionName) != luaState.preHooks.end()) {
		SafeCallLuaFunction(luaState.preHooks[functionName], std::forward<Args>(args)...);
	}

	return false; // Continue with original logic after pre-hook
}

template <typename... Args>
void LuaPostHook(const std::string &functionName, Args &&...args)
{
	auto &luaState = *CurrentLuaState;

	// Check for post-hook
	if (luaState.postHooks.find(functionName) != luaState.postHooks.end()) {
		SafeCallLuaFunction(luaState.postHooks[functionName], std::forward<Args>(args)...);
	}
}

// Lua function override registration
void RegisterPreHook(std::string_view functionName, sol::function hook);
void RegisterPostHook(std::string_view functionName, sol::function hook);
void RegisterFunctionOverride(std::string_view functionName, sol::function overrideFunction);

// Lua mod loading from mods directory
void LoadLuaMods();

} // namespace devilution
