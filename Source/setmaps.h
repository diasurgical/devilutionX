/**
 * @file setmaps.cpp
 *
 * Interface of functionality for the special quest dungeons.
 */
#pragma once

namespace devilution {

int ObjIndex(int x, int y);
void LoadSetMap();

/* rdata */
extern const char *const QuestLevelNames[];

} // namespace devilution
