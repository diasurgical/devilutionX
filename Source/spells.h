/**
 * @file spells.h
 *
 * Interface of functionality for casting player spells.
 */
#pragma once

#include "player.h"

namespace devilution {

int GetManaAmount(int id, spell_id sn);
void UseMana(int id, spell_id sn);
bool CheckSpell(int id, spell_id sn, spell_type st, bool manaonly);
void EnsureValidReadiedSpell(PlayerStruct &player);
void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int spllvl);
void DoResurrect(int pnum, int rid);
void DoHealOther(int pnum, int rid);
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
