/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern BYTE movie_playing;
extern BOOL loop_movie;

void play_movie(const char *pszMovie, BOOL user_can_close);

#ifdef __cplusplus
}
#endif

}
