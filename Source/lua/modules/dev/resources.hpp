#pragma once
#ifdef _DEBUG
#include <sol/sol.hpp>

namespace devilution {

sol::table LuaDevResourcesModule(sol::state_view &lua);

} // namespace devilution
#endif // _DEBUG
