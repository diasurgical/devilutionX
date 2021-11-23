#include "platform/switch/docking.h"

#include <SDL.h>
#include <switch.h>

#include "diablo.h"
#include "utils/display.h"

namespace devilution {

static int currently_docked = -1; // keep track of docked or handheld mode

/**
 * @brief Do a manual window resize when docking/undocking the Switch
 */
void HandleDocking()
{
	int docked;
	switch (appletGetOperationMode()) {
	case AppletOperationMode_Handheld:
		docked = 0;
		break;
	case AppletOperationMode_Console:
		docked = 1;
		break;
	default:
		docked = 0;
	}

	int display_width;
	int display_height;
	if ((currently_docked == -1) || (docked && !currently_docked) || (!docked && currently_docked)) {
		// docked mode has changed, update window size
		if (docked) {
			display_width = 1920;
			display_height = 1080;
			currently_docked = 1;
		} else {
			display_width = 1280;
			display_height = 720;
			currently_docked = 0;
		}
		// remove leftover-garbage on screen
		for (int i = 0; i < 3; i++) {
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);
		}
		SDL_SetWindowSize(ghMainWnd, display_width, display_height);
	}
}

} // namespace devilution
