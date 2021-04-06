/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#ifndef __MOVIE_H__
#define __MOVIE_H__

namespace dvl {

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

#endif /* __MOVIE_H__ */
