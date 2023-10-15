#pragma once

#include <string_view>

namespace devilution {

void LuaInitialize();
void LuaShutdown();
void LuaEvent(std::string_view name);

} // namespace devilution
