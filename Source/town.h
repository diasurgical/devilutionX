/**
 * @file town.h
 *
 * Interface of functionality for rendering the town, towners and calling other render routines.
 */
#pragma once

#include "gendung.h"
#include "interfac.h"

namespace devilution {

/**
* @brief Update the map to show the open hive
*/
void TownOpenHive();

/** 
* @brief Update the map to show the open grave
*/
void TownOpenGrave();

/**
* @brief Initialize town level
* @param entry Method of entry
*/
void CreateTown(lvl_entry entry);

} // namespace devilution
