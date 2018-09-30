//HEADER_GOES_HERE

#include "../types.h"

int movie_cpp_init_value; // weak
char movie_playing; // weak
BOOL loop_movie; // weak

const int movie_inf = 0x7F800000; // weak

struct movie_cpp_init
{
	movie_cpp_init()
	{
		movie_cpp_init_value = movie_inf;
	}
} _movie_cpp_init;
// 47F144: using guessed type int movie_inf;
// 659AF4: using guessed type int movie_cpp_init_value;

void __fastcall play_movie(char *pszMovie, BOOL user_can_close)
{
	char *v2; // esi
	WNDPROC saveProc; // edi
	//int v4; // eax
	MSG Msg; // [esp+8h] [ebp-24h]
	BOOL v6; // [esp+24h] [ebp-8h]
	void *video_stream; // [esp+28h] [ebp-4h]

	v6 = user_can_close;
	v2 = pszMovie;
	if ( gbActive )
	{
		saveProc = SetWindowProc(MovieWndProc);
		InvalidateRect(ghMainWnd, 0, 0);
		UpdateWindow(ghMainWnd);
		movie_playing = 1;
		sound_disable_music(1);
		sfx_stop();
		effects_play_sound("Sfx\\Misc\\blank.wav");
		SVidPlayBegin(v2, 0, 0, 0, 0, loop_movie != 0 ? 0x100C0808 : 0x10280808, &video_stream);
		if ( video_stream )
		{
			do
			{
				if ( !gbActive || v6 && !movie_playing )
					break;
				while ( PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) )
				{
					if ( Msg.message != WM_QUIT )
					{
						TranslateMessage(&Msg);
						DispatchMessage(&Msg);
					}
				}
				//_LOBYTE(v4) = SVidPlayContinue();
				if ( !SVidPlayContinue() )
					break;
			}
			while ( video_stream );
			if ( video_stream )
				SVidPlayEnd(video_stream);
		}
		SetWindowProc(saveProc);
		sound_disable_music(0);
	}
}
// 634980: using guessed type int gbActive;
// 659AF8: using guessed type int movie_playing;
// 659AFC: using guessed type int loop_movie;

LRESULT __stdcall MovieWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if ( Msg == WM_KEYFIRST || Msg == WM_CHAR )
	{
LABEL_6:
		movie_playing = 0;
		return MainWndProc(hWnd, Msg, wParam, lParam);
	}
	if ( Msg != WM_SYSCOMMAND )
	{
		if ( Msg != WM_LBUTTONDOWN && Msg != WM_RBUTTONDOWN )
			return MainWndProc(hWnd, Msg, wParam, lParam);
		goto LABEL_6;
	}
	if ( wParam != SC_CLOSE )
		return MainWndProc(hWnd, Msg, wParam, lParam);
	movie_playing = 0;
	return 0;
}
// 659AF8: using guessed type int movie_playing;
