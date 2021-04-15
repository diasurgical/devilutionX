/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#pragma once

namespace devilution {

extern BYTE movie_playing;
extern bool loop_movie;

void play_movie(const char *pszMovie, bool user_can_close);

}
