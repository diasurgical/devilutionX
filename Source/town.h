/**
 * @file town.h
 *
 * Interface of functionality for rendering the town, towners and calling other render routines.
 */
#ifndef __TOWN_H__
#define __TOWN_H__

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

void TownOpenHive();
void TownOpenGrave();
void CreateTown(int entry);

#ifdef __cplusplus
}
#endif

}

#endif /* __TOWN_H__ */
