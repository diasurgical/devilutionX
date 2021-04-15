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
Uint64 GetSpellBitmask(int spellId);
bool CheckSpell(int id, spell_id sn, spell_type st, bool manaonly);
void EnsureValidReadiedSpell(PlayerStruct &player);
void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int spllvl);
void DoResurrect(int pnum, int rid);
void DoHealOther(int pnum, int rid);
int GetSpellBookLevel(spell_id s);
int GetSpellStaffLevel(spell_id s);

}
