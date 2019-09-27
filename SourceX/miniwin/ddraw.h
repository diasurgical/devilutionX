#include "devilution.h"
#include <SDL.h>

#if defined(__AMIGA__) // Add other systems that require an 8bit screen here
#define D_BPP 8
#else
#define D_BPP 16
#endif

namespace dvl {

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;

extern SDL_Surface *surface;
extern SDL_Palette *palette;
extern SDL_Surface *pal_surface;
extern unsigned int pal_surface_palette_version;
extern bool bufferUpdated;

} // namespace dvl
