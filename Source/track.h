/**
 * @file track.h
 *
 * Interface of functionality tracking what the mouse cursor is pointing at.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

void track_process();
void track_repeat_walk(bool rep);
bool track_isscrolling();

#ifdef __cplusplus
}
#endif

}
