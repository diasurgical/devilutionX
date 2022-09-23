#pragma once

#include "engine/clx_sprite.hpp"
#include "engine/surface.hpp"
#include "spelldat.h"

namespace devilution {

void InitSpellBook();
void FreeSpellBook();
void CheckSBook();
void DrawSpellBook(const Surface &out);
/** Maps from spellbook page number and position to spell_id. */
extern spell_id SpellPages[6][7];

} // namespace devilution
