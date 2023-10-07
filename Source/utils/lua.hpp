#pragma once

#include <string>

namespace devilution {

void LuaInitialize();
void LuaShutdown();
void LuaEvent(std::string name);

} // namespace devilution
