#pragma once
#ifdef _DEBUG

#include <string>
#include <string_view>

#include <expected.hpp>

namespace devilution {

tl::expected<std::string, std::string> RunLuaReplLine(std::string_view code);

} // namespace devilution
#endif // _DEBUG
