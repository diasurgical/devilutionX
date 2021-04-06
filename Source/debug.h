/**
 * @file debug.h
 *
 * Interface of debug functions.
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__

namespace dvl {

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

#endif /* __DEBUG_H__ */
