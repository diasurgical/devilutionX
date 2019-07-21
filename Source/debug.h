//HEADER_GOES_HERE
#ifndef __DEBUG_H__
#define __DEBUG_H__

extern BYTE *pSquareCel;
extern char dMonsDbg[NUMLEVELS][MAXDUNX][MAXDUNY];
extern char dFlagDbg[NUMLEVELS][MAXDUNX][MAXDUNY];

void LoadDebugGFX();
void FreeDebugGFX();
#ifdef _DEBUG
void init_seed_desync();
void seed_desync_index_get();
void seed_desync_index_set();
void seed_desync_check(int seed);
#endif
void CheckDungeonClear();
#ifdef _DEBUG
void GiveGoldCheat();
void StoresCheat();
void TakeGoldCheat();
void MaxSpellsCheat();
void SetSpellLevelCheat(char spl, int spllvl);
void SetAllSpellsCheat();
void PrintDebugPlayer(BOOL bNextPlayer);
void PrintDebugQuest();
void PrintDebugMonster(int m);
void GetDebugMonster();
void NextDebugMonster();
#endif

#endif /* __DEBUG_H__ */
