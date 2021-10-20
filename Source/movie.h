/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#pragma once

namespace devilution {

extern bool movie_playing;
extern bool loop_movie;

/**
 * @brief Start playback of a given video.
 * @param pszMovie The file name of the video
 * @param user_can_close Set to false to make the video unskippable.
 */
void play_movie(const char *pszMovie, bool user_can_close);

/**
 * @brief Fade to black and play a video
 * @param pszMovie file path of movie
 */
void PlayInGameMovie(const char *pszMovie);

} // namespace devilution
