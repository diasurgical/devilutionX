/**
 * @file setmaps.cpp
 *
 * Interface of functionality for the special quest dungeons.
 */
#pragma once

#include "engine.h"

namespace devilution {

/**
 * @brief Find the index of an object given a point in map coordinates
 *
 * @param position The map coordinate to test
 * @return The index of the object at the given position
 */
int ObjIndex(Point position);
void LoadSetMap();

/* rdata */
extern const char *const QuestLevelNames[];

} // namespace devilution
