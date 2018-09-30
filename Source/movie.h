//HEADER_GOES_HERE
#ifndef __MOVIE_H__
#define __MOVIE_H__

extern int movie_cpp_init_value; // weak
extern char movie_playing; // weak
extern BOOL loop_movie; // weak

void __cdecl movie_cpp_init();
void __fastcall play_movie(char *pszMovie, BOOL user_can_close);
LRESULT __stdcall MovieWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

/* rdata */

extern const int movie_inf; // weak

#endif /* __MOVIE_H__ */
