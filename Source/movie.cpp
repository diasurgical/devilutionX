#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE movie_playing;
BOOL loop_movie;

void play_movie(char *pszMovie, BOOL user_can_close)
{
	WNDPROC saveProc;
	HANDLE video_stream;

#ifndef SWITCH
	if (!gbActive) {
		return;
	}
#endif

	saveProc = SetWindowProc(MovieWndProc);
	movie_playing = TRUE;
	sound_disable_music(TRUE);
	sfx_stop();
	effects_play_sound("Sfx\\Misc\\blank.wav");

	SVidPlayBegin(pszMovie, 0, 0, 0, 0, loop_movie ? 0x100C0808 : 0x10280808, &video_stream);
	if (video_stream) {
		MSG Msg;
		while (video_stream) {
			if (!gbActive || user_can_close && !movie_playing)
				break;
			while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
				if (Msg.message != WM_QUIT) {
					TranslateMessage(&Msg);
					DispatchMessage(&Msg);
				}
			}
			if (!SVidPlayContinue())
				break;
		}
		if (video_stream)
			SVidPlayEnd(video_stream);
	}
	SetWindowProc(saveProc);
	sound_disable_music(FALSE);
}

LRESULT __stdcall MovieWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
	case WM_KEYFIRST:
	case WM_CHAR:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		movie_playing = FALSE;
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE) {
			movie_playing = FALSE;
			return 0;
		}
		break;
	}

	return MainWndProc(hWnd, Msg, wParam, lParam);
}

DEVILUTION_END_NAMESPACE
