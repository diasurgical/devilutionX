#pragma once
#ifdef _DEBUG
#include <sol/sol.hpp>

namespace devilution {

sol::table LuaDevModule(sol::state_view &lua);

} // namespace devilution
#endif // _DEBUG
