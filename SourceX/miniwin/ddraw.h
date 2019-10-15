#include "devilution.h"
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

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;

extern SDL_Palette *palette;
extern SDL_Surface *pal_surface;
extern unsigned int pal_surface_palette_version;
extern bool bufferUpdated;

// Returns:
// SDL1: Video surface.
// SDL2, no upscale: Window surface.
// SDL2, upscale: Renderer texture surface.
SDL_Surface *GetOutputSurface();

} // namespace dvl
