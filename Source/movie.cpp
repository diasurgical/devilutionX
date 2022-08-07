/**
 * @file movie.cpp
 *
 * Implementation of video playback.
 */

#include "controls/plrctrls.h"
#include "diablo.h"
#include "effects.h"
#include "engine/demomode.h"
#include "engine/sound.h"
#include "hwcursor.hpp"
#include "miniwin/misc_msg.h"
#include "storm/storm_svid.h"
#include "utils/display.h"

namespace devilution {

/** Should the movie continue playing. */
bool movie_playing;
/** Should the movie play in a loop. */
bool loop_movie;

void play_movie(const char *pszMovie, bool userCanClose)
{
	if (demo::IsRunning())
		return;

	movie_playing = true;

	sound_disable_music(true);
	stream_stop();
	effects_play_sound(SFX_SILENCE);

	if (IsHardwareCursorEnabled() && ControlDevice == ControlTypes::KeyboardAndMouse) {
		SetHardwareCursorVisible(false);
	}

	if (SVidPlayBegin(pszMovie, loop_movie ? 0x100C0808 : 0x10280808)) {
		SDL_Event event;
		uint16_t modState;
		while (movie_playing) {
			while (movie_playing && FetchMessage(&event, &modState)) {
				switch (event.type) {
				case SDL_KEYDOWN:
				case SDL_MOUSEBUTTONUP:
					if (userCanClose || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
						movie_playing = false;
					break;
				case SDL_QUIT:
					SVidPlayEnd();
					diablo_quit(0);
				}
			}
			if (!SVidPlayContinue())
				break;
		}
		SVidPlayEnd();
	}

	sound_disable_music(false);

	movie_playing = false;

	SDL_GetMouseState(&MousePosition.x, &MousePosition.y);
	OutputToLogical(&MousePosition.x, &MousePosition.y);
}

void PlayInGameMovie(const char *pszMovie)
{
	PaletteFadeOut(8);
	play_movie(pszMovie, false);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen();
	PaletteFadeIn(8);
	force_redraw = 255;
}

} // namespace devilution
