/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#pragma once

namespace devilution {

extern bool movie_playing;
extern bool loop_movie;

void play_movie(const char *pszMovie, bool user_can_close);
void PlayInGameMovie(const char *pszMovie);

} // namespace devilution
