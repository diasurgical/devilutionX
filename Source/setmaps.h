/**
 * @file setmaps.cpp
 *
 * Interface of functionality for the special quest dungeons.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

int ObjIndex(int x, int y);
void LoadSetMap();

/* rdata */
extern const char *const quest_level_names[];

#ifdef __cplusplus
}
#endif

}
