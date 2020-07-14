#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE movie_playing;
BOOL loop_movie;

void play_movie(char *pszMovie, BOOL user_can_close)
{
	HANDLE video_stream;

	movie_playing = TRUE;
	sound_disable_music(TRUE);
	sfx_stop();
	effects_play_sound("Sfx\\Misc\\blank.wav");

	SVidPlayBegin(pszMovie, 0, 0, 0, 0, loop_movie ? 0x100C0808 : 0x10280808, &video_stream);
	MSG Msg;
	while (video_stream && movie_playing) {
		while (movie_playing && PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Msg);
			switch (Msg.message) {
			case WM_KEYDOWN:
			case WM_CHAR:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				if (user_can_close || (Msg.message == WM_CHAR && Msg.wParam == VK_ESCAPE))
					movie_playing = FALSE;
				break;
			case WM_QUIT:
				movie_playing = FALSE;
				break;
			}
		}
		if (!SVidPlayContinue())
			break;
	}
	if (video_stream)
		SVidPlayEnd(video_stream);
	sound_disable_music(FALSE);
	movie_playing = FALSE;
}

DEVILUTION_END_NAMESPACE
