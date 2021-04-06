/**
 * @file debug.h
 *
 * Interface of debug functions.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern BYTE *pSquareCel;

void FreeDebugGFX();
void CheckDungeonClear();
void LoadDebugGFX();
void GiveGoldCheat();
void TakeGoldCheat();
void MaxSpellsCheat();
void SetAllSpellsCheat();
void PrintDebugPlayer(BOOL bNextPlayer);
void PrintDebugQuest();
void GetDebugMonster();
void NextDebugMonster();

#ifdef __cplusplus
}
#endif

}
