#include <SDL.h>

#include "devilution.h"
#include "dx.h"
#include "stubs.h"

namespace dvl {

WINBOOL SetCursorPos(int X, int Y)
{
	assert(renderer);
	assert(window);

	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	X += view.x;
	Y += view.y;

	float scaleX;
	SDL_RenderGetScale(renderer, &scaleX, NULL);
	X *= scaleX;
	Y *= scaleX;

	SDL_WarpMouseInWindow(window, X, Y);
	return true;
}

int ShowCursor(WINBOOL bShow)
{
	SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);

	return bShow;
}

WINBOOL TextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
	DUMMY_ONCE();

	assert(window);
	SDL_SetWindowTitle(window, lpString);

	return true;
}

}  // namespace dvl
