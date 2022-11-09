#pragma once

#include "engine/clx_sprite.hpp"
#include "engine/point.hpp"
#include "engine/surface.hpp"
#include "spelldat.h"

#define SPLICONLENGTH 56

namespace devilution {

/** Maps from spell_id to spelicon.cel frame number. */
extern const char SpellITbl[];

/**
 * Draw spell icon onto the given buffer.
 * @param out Output buffer.
 * @param position Buffer coordinates.
 * @param nCel Index of the cel frame to draw. 0 based.
 */
void DrawSpellCel(const Surface &out, Point position, int nCel);

/**
 * Draw spell icon onto the given buffer.
 * @param out Output buffer.
 * @param position Buffer coordinates.
 * @param sprite Icons sprite sheet.
 */
#ifdef UNPACKED_MPQS
void DrawSpellCel(const Surface &out, Point position, ClxSprite sprite, ClxSprite background);
#else
void DrawSpellCel(const Surface &out, Point position, ClxSprite sprite);
#endif

void DrawSpellBorder(const Surface &out, Point position, ClxSprite sprite);

void SetSpellTrans(spell_type t);

void LoadSpellIcons();
void FreeSpellIcons();

} // namespace devilution
