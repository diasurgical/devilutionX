#pragma once

#include <string_view>

#include <expected.hpp>
#include <sol/forward.hpp>

namespace devilution {

void LuaInitialize();
void LuaShutdown();
void LuaEvent(std::string_view name);
sol::state &GetLuaState();

} // namespace devilution
