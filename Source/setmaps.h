/**
 * @file setmaps.cpp
 *
 * Interface of functionality for the special quest dungeons.
 */
#pragma once

#include "engine.h"
#include "engine/point.hpp"
#include "objects.h"

namespace devilution {

/**
 * @brief Find an object given a point in map coordinates
 *
 * @param position The map coordinate to test
 * @return A reference to the object
 */
Object &ObjectAtPosition(Point position);

/**
 * @brief Load a quest map, the given map is specified via the global setlvlnum
 */
void LoadSetMap();

/* rdata */
extern const char *const QuestLevelNames[];

} // namespace devilution
