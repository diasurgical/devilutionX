/**
 * @file spells.h
 *
 * Interface of functionality for casting player spells.
 */
#pragma once

#include "player.h"

namespace devilution {

enum class SpellCheckResult : uint8_t {
	Success,
	Fail_NoMana,
	Fail_Level0,
	Fail_Busy,
};

bool IsWallSpell(spell_id spl);
int GetManaAmount(Player &player, spell_id sn);
void UseMana(int id, spell_id sn);
SpellCheckResult CheckSpell(int id, spell_id sn, spell_type st, bool manaonly);

/**
 * @brief Ensures the player's current readied spell is a valid selection for the character. If the current selection is
 * incompatible with the player's items and spell (for example, if the player does not currently have access to the spell),
 * the selection is cleared.
 * @note Will force a UI redraw in case the values actually change, so that the new spell reflects on the bottom panel.
 * @param player The player whose readied spell is to be checked.
 */
void EnsureValidReadiedSpell(Player &player);
void CastSpell(int id, spell_id spl, int sx, int sy, int dx, int dy, int spllvl);

/**
 * @param pnum player index
 * @param rid target player index
 */
void DoResurrect(int pnum, uint16_t rid);
void DoHealOther(int pnum, uint16_t rid);
int GetSpellBookLevel(spell_id s);
int GetSpellStaffLevel(spell_id s);

/**
 * @brief Gets a value that represents the specified spellID in 64bit bitmask format.
 * For example:
 *  - spell ID  1: 0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000.0001
 *  - spell ID 43: 0000.0000.0000.0000.0000.0100.0000.0000.0000.0000.0000.0000.0000.0000.0000.0000
 * @param spellId The id of the spell to get a bitmask for.
 * @return A 64bit bitmask representation for the specified spell.
 */
constexpr uint64_t GetSpellBitmask(int spellId)
{
	return 1ULL << (spellId - 1);
}

} // namespace devilution
