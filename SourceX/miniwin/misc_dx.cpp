#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"
#include <SDL.h>

namespace dvl {

WINBOOL SetCursorPos(int X, int Y)
{
#ifndef SWITCH
	assert(window);

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

	SDL_WarpMouseInWindow(window, X, Y);
#endif
	return true;
}

int ShowCursor(WINBOOL bShow)
{
#ifndef SWITCH
	if (SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE) <= -1) {
		SDL_Log(SDL_GetError());
	}
#endif

	return bShow;
}

WINBOOL TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
	DUMMY_ONCE();

#ifndef SWITCH
	assert(window);
	SDL_SetWindowTitle(window, lpString);
#endif

	return true;
}

} // namespace dvl
