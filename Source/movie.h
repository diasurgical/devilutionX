/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#ifndef __MOVIE_H__
#define __MOVIE_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern BYTE movie_playing;
extern BOOL loop_movie;

void play_movie(const char *pszMovie, BOOL user_can_close);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MOVIE_H__ */
