#pragma once

#include "engine/cel_sprite.hpp"
#include "engine/surface.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

extern std::optional<CelSprite> pChrButtons;
extern const char *const ClassStrTbl[];

void DrawChr(const Surface &);
void LoadCharPanel();
void FreeCharPanel();

} // namespace devilution
