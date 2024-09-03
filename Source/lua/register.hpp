#pragma once

#include <sol/forward.hpp>

#include "lua/lua.hpp"

namespace devilution {

void RegisterAllBindings(sol::state &lua);

} // namespace devilution
