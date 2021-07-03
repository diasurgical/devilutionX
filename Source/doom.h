/**
 * @file doom.h
 *
 * Interface of the map of the stars quest.
 */
#pragma once

#include "engine.h"

namespace devilution {

extern bool DoomFlag;
extern int DoomQuestState;

int doom_get_frame_from_time();
void doom_init();
void doom_close();
void doom_draw(const Surface &out);

} // namespace devilution
