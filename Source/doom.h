/**
 * @file doom.h
 *
 * Interface of the map of the stars quest.
 */
#pragma once

#include "engine/surface.hpp"

namespace devilution {

extern bool DoomFlag;
void doom_init();
void doom_close();
void doom_draw(const Surface &out);

} // namespace devilution
