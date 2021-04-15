/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#pragma once

#include "miniwin/miniwin.h"

namespace devilution {

extern BYTE movie_playing;
extern bool loop_movie;

void play_movie(const char *pszMovie, bool user_can_close);

}
