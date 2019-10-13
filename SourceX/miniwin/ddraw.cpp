#include "miniwin/ddraw.h"

namespace dvl {

extern SDL_Surface *renderer_texture_surface; // defined in dx.cpp

SDL_Surface *GetOutputSurface() {
#ifdef USE_SDL1
	return SDL_GetVideoSurface();
#else
	if (renderer)
		return renderer_texture_surface;
	return SDL_GetWindowSurface(window);
#endif
}

} // namespace dvl
