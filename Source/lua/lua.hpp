#pragma once

#include <string_view>

#include <expected.hpp>

namespace sol {
class state;
} // namespace sol

namespace devilution {

void LuaInitialize();
void LuaShutdown();
void LuaEvent(std::string_view name);
sol::state &GetLuaState();

} // namespace devilution
