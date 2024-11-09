#pragma once
#if defined(_DEBUG) && defined(DEVILUTIONX_RESOURCE_TRACKING_ENABLED)
#include <sol/sol.hpp>

namespace devilution {

sol::table LuaDevResourcesModule(sol::state_view &lua);

} // namespace devilution
#endif // defined(_DEBUG) && defined(DEVILUTIONX_RESOURCE_TRACKING_ENABLED)
