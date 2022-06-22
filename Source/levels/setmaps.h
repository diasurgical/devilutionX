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
 * @brief Load a quest map, the given map is specified via the global setlvlnum
 */
void LoadSetMap();

/* rdata */
extern const char *const QuestLevelNames[];

} // namespace devilution
