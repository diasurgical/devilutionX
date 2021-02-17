/**
 * @file drlg_l3.h
 *
 * Interface of the caves level generation algorithms.
 */
#ifndef __DRLG_L3_H__
#define __DRLG_L3_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

void AddFenceDoors();
void CreateL3Dungeon(DWORD rseed, int entry);
void LoadL3Dungeon(const char *sFileName, int vx, int vy);
void LoadPreL3Dungeon(const char *sFileName, int vx, int vy);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L3_H__ */
