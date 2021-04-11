/**
 * @file movie.cpp
 *
 * Implementation of video playback.
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../SourceX/display.h"

namespace devilution {

/** Should the movie continue playing. */
BYTE movie_playing;
/** Should the movie play in a loop. */
bool loop_movie;

/**
 * @brief Start playback of a given video.
 * @param pszMovie The file name of the video
 * @param user_can_close Set to false to make the video unskippable.
 */
void play_movie(const char *pszMovie, bool user_can_close)
{
	HANDLE video_stream;

	movie_playing = true;
	sound_disable_music(true);
	stream_stop();
	effects_play_sound("Sfx\\Misc\\blank.wav");

	SVidPlayBegin(pszMovie, 0, 0, 0, 0, loop_movie ? 0x100C0808 : 0x10280808, &video_stream);
	MSG Msg;
	while (video_stream && movie_playing) {
		while (movie_playing && FetchMessage(&Msg)) {
			switch (Msg.message) {
			case DVL_WM_KEYDOWN:
			case DVL_WM_LBUTTONDOWN:
			case DVL_WM_RBUTTONDOWN:
				if (user_can_close || (Msg.message == DVL_WM_KEYDOWN && Msg.wParam == DVL_VK_ESCAPE))
					movie_playing = false;
				break;
			case DVL_WM_QUIT:
				SVidPlayEnd(video_stream);
				diablo_quit(0);
				break;
			}
		}
		if (!SVidPlayContinue())
			break;
	}
	if (video_stream)
		SVidPlayEnd(video_stream);
	sound_disable_music(false);
	movie_playing = false;
	SDL_GetMouseState(&MouseX, &MouseY);
	OutputToLogical(&MouseX, &MouseY);
}

} // namespace devilution
