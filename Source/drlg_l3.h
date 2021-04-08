/**
 * @file drlg_l3.h
 *
 * Interface of the caves level generation algorithms.
 */
#pragma once

namespace devilution {

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

}
