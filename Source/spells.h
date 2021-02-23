/**
 * @file spells.h
 *
 * Interface of functionality for casting player spells.
 */
#ifndef __SPELLS_H__
#define __SPELLS_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

int GetManaAmount(int id, int sn);
void UseMana(int id, int sn);
BOOL CheckSpell(int id, int sn, char st, BOOL manaonly);
void EnsureValidReadiedSpell(PlayerStruct &player);
void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int spllvl);
void DoResurrect(int pnum, int rid);
void DoHealOther(int pnum, int rid);
int GetSpellBookLevel(int s);
int GetSpellStaffLevel(int s);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __SPELLS_H__ */
