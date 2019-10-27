#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"
#ifdef VITA
#ifdef USE_SDL1
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#else
#include <SDL.h>
#endif

namespace dvl {

WINBOOL SetCursorPos(int X, int Y)
{

#ifndef VITA
	assert(window);

#ifndef USE_SDL1
	if (renderer) {
		SDL_Rect view;
		SDL_RenderGetViewport(renderer, &view);
		X += view.x;
		Y += view.y;

		float scaleX;
		SDL_RenderGetScale(renderer, &scaleX, NULL);
		X *= scaleX;
		Y *= scaleX;
	}
#endif

	SDL_WarpMouseInWindow(window, X, Y);
#endif
	return true;
}

} // namespace dvl
