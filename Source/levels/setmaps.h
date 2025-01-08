/**
 * @file setmaps.cpp
 *
 * Interface of functionality for the special quest dungeons.
 */
#pragma once

namespace devilution {

/**
 * @brief Load a quest map, the given map is specified via the global setlvlnum
 */
void LoadSetMap();

/* rdata */
extern const char *const QuestLevelNames[];

} // namespace devilution
