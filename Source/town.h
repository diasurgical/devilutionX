/**
 * @file town.h
 *
 * Interface of functionality for rendering the town, towners and calling other render routines.
 */
#pragma once

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
