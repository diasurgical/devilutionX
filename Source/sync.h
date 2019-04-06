//HEADER_GOES_HERE
#ifndef __SYNC_H__
#define __SYNC_H__

extern WORD sync_word_6AA708[MAXMONSTERS];
extern int sgnMonsters;
extern WORD sgwLRU[MAXMONSTERS];
extern int sgnSyncItem;

DWORD __fastcall sync_all_monsters(const BYTE *pbBuf, DWORD dwMaxLen);
void __cdecl sync_one_monster();
BOOL __fastcall sync_monster_active(TSyncMonster *p);
void __fastcall sync_monster_pos(TSyncMonster *p, int ndx);
BOOL __fastcall sync_monster_active2(TSyncMonster *p);
void __fastcall SyncPlrInv(TSyncHeader *pHdr);
DWORD __fastcall sync_update(int pnum, const BYTE *pbBuf);
void __fastcall sync_monster(int pnum, const TSyncMonster *p);
void __cdecl sync_init();

#endif /* __SYNC_H__ */
