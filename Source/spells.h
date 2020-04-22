//HEADER_GOES_HERE
#ifndef __SPELLS_H__
#define __SPELLS_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

int GetManaAmount(int id, int sn);
void UseMana(int id, int sn);
BOOL CheckSpell(int id, int sn, char st, BOOL manaonly);
void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int caster, int spllvl);
void DoResurrect(int pnum, int rid);
void DoHealOther(int pnum, int rid);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __SPELLS_H__ */
