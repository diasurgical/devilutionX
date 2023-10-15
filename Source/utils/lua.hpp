#pragma once

#include <string_view>

#include <expected.hpp>

namespace devilution {

void LuaInitialize();
void LuaShutdown();
void LuaEvent(std::string_view name);
tl::expected<std::string, std::string> RunLua(std::string_view code);

} // namespace devilution
