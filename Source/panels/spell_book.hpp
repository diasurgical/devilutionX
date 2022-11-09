#pragma once

#include "engine/clx_sprite.hpp"
#include "engine/surface.hpp"

namespace devilution {

#ifdef UNPACKED_MPQS
extern OptionalOwnedClxSpriteList pSBkIconsBackground;
extern OptionalOwnedClxSpriteList pSBkIconsForeground;
#else
extern OptionalOwnedClxSpriteList pSBkIconCels;
#endif

void InitSpellBook();
void FreeSpellBook();
void CheckSBook();
void DrawSpellBook(const Surface &out);

} // namespace devilution
