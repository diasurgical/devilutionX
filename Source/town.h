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
 * @brief Check if hive can be opened by dropping rune bomb on a tile
 * @param position The position of the tile
 * @return True if the bomb would open hive
 */
bool OpensHive(Point position);

/**
 * @brief Check if grave can be opened by dropping cathedral map on a tile
 * @param position The position of the tile
 * @return True if the map would open the grave
 */
bool OpensGrave(Point position);

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
