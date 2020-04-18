/**
 * @file movie.h
 *
 * Interface of video playback.
 */
#ifndef __MOVIE_H__
#define __MOVIE_H__

DEVILUTION_BEGIN_NAMESPACE

extern BYTE movie_playing;
extern BOOL loop_movie;

void play_movie(char *pszMovie, BOOL user_can_close);
LRESULT MovieWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

/* rdata */

DEVILUTION_END_NAMESPACE

#endif /* __MOVIE_H__ */
