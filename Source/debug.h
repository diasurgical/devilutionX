/**
 * @file debug.h
 *
 * Interface of debug functions.
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__

extern BYTE *pSquareCel;
extern char dMonsDbg[NUMLEVELS][MAXDUNX][MAXDUNY];
extern char dFlagDbg[NUMLEVELS][MAXDUNX][MAXDUNY];

void LoadDebugGFX();
void FreeDebugGFX();
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
