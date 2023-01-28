#pragma once

#include "engine/clx_sprite.hpp"
#include "engine/point.hpp"
#include "engine/surface.hpp"
#include "spelldat.h"

#define SPLICONLENGTH 56

namespace devilution {

/**
 * Draw a large (56x56) spell icon onto the given buffer.
 *
 * @param out Output buffer.
 * @param position Buffer coordinates (bottom-left).
 * @param spell Spell ID.
 */
void DrawLargeSpellIcon(const Surface &out, Point position, spell_id spell);

/**
 * Draw a small (37x38) spell icon onto the given buffer.
 *
 * @param out Output buffer.
 * @param position Buffer coordinates (bottom-left).
 * @param spell Spell ID.
 */
void DrawSmallSpellIcon(const Surface &out, Point position, spell_id spell);

/**
 * Draw an inset 2px border for a large (56x56) spell icon.
 *
 * @param out Output buffer.
 * @param position Buffer coordinates (bottom-left).
 * @param spell Spell ID.
 */
void DrawLargeSpellIconBorder(const Surface &out, Point position, uint8_t color);

/**
 * Draw an inset 2px border for a small (37x38) spell icon.
 *
 * @param out Output buffer.
 * @param position Buffer coordinates (bottom-left).
 * @param spell Spell ID.
 */
void DrawSmallSpellIconBorder(const Surface &out, Point position);

/**
 * @brief Set the color mapping for the `Draw(Small|Large)SpellIcon(Border)` calls.
 */
void SetSpellTrans(SpellType t);

void LoadLargeSpellIcons();
void FreeLargeSpellIcons();

void LoadSmallSpellIcons();
void FreeSmallSpellIcons();

} // namespace devilution
