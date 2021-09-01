/**
 * @file town.h
 *
 * Interface of functionality for rendering the town, towners and calling other render routines.
 *
 * Function : FillSector
 * @brief Load level data into dPiece
 * @param path Path of dun file
 * @param xi upper left destination
 * @param yy upper left destination
 *
 * Function: FillTile.
 * @brief Load a tile in to dPiece
 * @param megas Map from mega tiles to macro tiles
 * @param xx upper left destination
 * @param yy upper left destination
 * @param t tile id
 *
 * Function: TownCloseHive(). 
 * @brief Update the map to show the closed hive
 *
 * Function: TownCloseGrave().
 * @brief Update the map to show the closed grave
 *
 * Function : DrlgTPass3().
 * @brief Initialize all of the levels data
 *
 * Function: TownOpenHive().
 * @brief Update the map to show the open hive
 *
 * Function: TownOpenGrave().
 * @brief Update the map to show the open grave
 *
 * Function: CreateTown.
 * @brief Initialize town level
 * @param entry Methode of entry
 */
 
#pragma once

#include "gendung.h"
#include "interfac.h"

namespace devilution {

void TownOpenHive();
void TownOpenGrave();
void CreateTown(lvl_entry entry);

} // namespace devilution
