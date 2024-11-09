#pragma once

#include <string>

#include <expected.hpp>

#include "engine/clx_sprite.hpp"
#include "engine/surface.hpp"

namespace devilution {

tl::expected<void, std::string> InitSpellBook();
void FreeSpellBook();
void CheckSBook();
void DrawSpellBook(const Surface &out);

} // namespace devilution
