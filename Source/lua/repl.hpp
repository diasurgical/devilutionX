#pragma once
#ifdef _DEBUG

#include <string>
#include <string_view>

#include <expected.hpp>
#include <sol/forward.hpp>

namespace devilution {

tl::expected<std::string, std::string> RunLuaReplLine(std::string_view code);

sol::environment &GetLuaReplEnvironment();

void LuaReplShutdown();

} // namespace devilution
#endif // _DEBUG
