#pragma once

#include <array>

#include "engine/cel_sprite.hpp"
#include "engine/surface.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

extern std::optional<CelSprite> pChrButtons;
extern const std::array<const char *const, 6> ClassStrTbl;

void DrawChr(const Surface &);
void LoadCharPanel();
void FreeCharPanel();

} // namespace devilution
