#pragma once

#include "engine/cel_sprite.hpp"
#include "engine/surface.hpp"

namespace devilution {

extern OptionalOwnedCelSprite pChrButtons;
extern const char *const ClassStrTbl[];

void DrawChr(const Surface &);
void LoadCharPanel();
void FreeCharPanel();

} // namespace devilution
