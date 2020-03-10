//HEADER_GOES_HERE
#ifndef __SPELLS_H__
#define __SPELLS_H__

int GetManaAmount(int id, int sn);
void UseMana(int id, int sn);
BOOL CheckSpell(int id, int sn, char st, BOOL manaonly);
void CastSpell(int id, int spl, int sx, int sy, int dx, int dy, int caster, int spllvl);
void DoResurrect(int pnum, int rid);
void DoHealOther(int pnum, int rid);

#endif /* __SPELLS_H__ */
