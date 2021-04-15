/**
 * @file track.h
 *
 * Interface of functionality tracking what the mouse cursor is pointing at.
 */
#pragma once

namespace devilution {

void track_process();
void track_repeat_walk(bool rep);
bool track_isscrolling();

}
