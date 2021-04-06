/**
 * @file setmaps.cpp
 *
 * Interface of functionality for the special quest dungeons.
 */
#ifndef __SETMAPS_H__
#define __SETMAPS_H__

namespace dvl {

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

#endif /* __SETMAPS_H__ */
