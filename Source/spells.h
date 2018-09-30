//HEADER_GOES_HERE
#ifndef __SPELLS_H__
#define __SPELLS_H__

int __fastcall GetManaAmount(int id, int sn);
void __fastcall UseMana(int id, int sn);
BOOL __fastcall CheckSpell(int id, int sn, BYTE st, BOOL manaonly);
void __fastcall CastSpell(int id, int spl, int sx, int sy, int dx, int dy, BOOL caster, int spllvl);
void __fastcall DoResurrect(int pnum, int rid);
void __fastcall PlacePlayer(int pnum);
void __fastcall DoHealOther(int pnum, int rid);

/* rdata */

extern SpellData spelldata[37];

#endif /* __SPELLS_H__ */
