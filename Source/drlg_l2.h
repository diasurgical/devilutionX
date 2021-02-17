/**
 * @file drlg_l2.h
 *
 * Interface of the catacombs level generation algorithms.
 */
#ifndef __DRLG_L2_H__
#define __DRLG_L2_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern BYTE predungeon[DMAXX][DMAXY];

void InitDungeon();
void LoadL2Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL2Dungeon(const char *sFileName, int vx, int vy);
void CreateL2Dungeon(DWORD rseed, int entry);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L2_H__ */
