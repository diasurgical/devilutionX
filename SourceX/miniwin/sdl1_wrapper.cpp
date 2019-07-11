#include <SDL/SDL.h>
#include "sdl1_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

struct SDL_Window {
    const void *magic;
    Uint32 id;
    char *title;
    SDL_Surface *icon;
    int x, y;
    int w, h;
    int min_w, min_h;
    int max_w, max_h;
    Uint32 flags;
    Uint32 last_fullscreen_flags;

    /* Stored position and size for windowed mode */
    SDL_Rect windowed;

    SDL_DisplayMode fullscreen_mode;

    float brightness;
    Uint16 *gamma;
    Uint16 *saved_gamma;        /* (just offset into gamma) */

    SDL_Surface *surface;
    SDL_bool surface_valid;

    SDL_bool is_hiding;
    SDL_bool is_destroying;

    SDL_WindowShaper *shaper;

    SDL_HitTest hit_test;
    void *hit_test_data;

    SDL_WindowUserData *data;

    void *driverdata;

    SDL_Window *prev;
    SDL_Window *next;
};

struct SDL_WindowShaper {
    /* The window associated with the shaper */
    SDL_Window *window;

    /* The user's specified coordinates for the window, for once we give it a shape. */
    Uint32 userx,usery;

    /* The parameters for shape calculation. */
    SDL_WindowShapeMode mode;

    /* Has this window been assigned a shape? */
    SDL_bool hasshape;

    void *driverdata;
};

SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, Uint32 flags)
{
	SDL_Window* window;

	window->surface = SDL_SetVideoMode(w, h, 16,  (SDL_SWSURFACE | SDL_FULLSCREEN) );
		
	return window;
}

void SDL_DestroyWindow(SDL_Window* window)
{
	SDL_FreeSurface(window->surface);
}

SDL_Surface* SDL_GetWindowSurface(SDL_Window* window)
{
	if (window)		
		return window->surface;
	else
		return NULL;
}

int SDL_UpdateWindowSurface(SDL_Window* window)
{
	SDL_Flip(window->surface);
}

SDL_Renderer* SDL_CreateRenderer(SDL_Window* window,
                                 int         index,
                                 Uint32      flags)
{
	return window->surface;
}

int SDL_SetRenderDrawColor(SDL_Renderer* renderer,
                           Uint8         r,
                           Uint8         g,
                           Uint8         b,
                           Uint8         a)
{
	SDL_FillRect( renderer, NULL, SDL_MapRGBA( renderer->format, r, g, b, a) );
}

SDL_Texture* SDL_CreateTextureFromSurface(SDL_Renderer* renderer,
                                          SDL_Surface*  surface)
{
	if( renderer != NULL )
    {
        //Create an optimized image
        renderer = SDL_DisplayFormat( surface );
        		
		return renderer;
    }
}											  

void SDL_DestroyTexture(SDL_Texture* texture)
{
	SDL_FreeSurface(texture);
}

void SDL_DestroyRenderer(SDL_Renderer* Renderer)
{
	SDL_FreeSurface(Renderer);
}

int SDL_RenderClear(SDL_Renderer* renderer)
{
	return 0;
}

int SDL_RenderCopy(SDL_Renderer*   renderer,
                   SDL_Texture*    texture,
                   const SDL_Rect* srcrect,
                   const SDL_Rect* dstrect)
{
   //Blit the surface
    SDL_BlitSurface( texture, NULL, renderer, NULL );
}

void SDL_RenderPresent(SDL_Renderer* renderer)
{
	SDL_Flip(renderer);
}

void SDL_Log(const char *fmt, ...) {

	printf("%s\n", fmt);
}

SDL_Palette *
SDL_AllocPalette(int ncolors)
{
    SDL_Palette *palette;

    /* Input validation */
    if (ncolors < 1) {
      SDL_InvalidParamError("ncolors");
      return NULL;
    }

    palette = (SDL_Palette *) SDL_malloc(sizeof(*palette));
    if (!palette) {
        SDL_OutOfMemory();
        return NULL;
    }
    palette->colors =
        (SDL_Color *) SDL_malloc(ncolors * sizeof(*palette->colors));
    if (!palette->colors) {
        SDL_free(palette);
        return NULL;
    }
    palette->ncolors = ncolors;
    palette->version = 1;
    palette->refcount = 1;

    SDL_memset(palette->colors, 0xFF, ncolors * sizeof(*palette->colors));

    return palette;
}

int
SDL_SetPixelFormatPalette(SDL_PixelFormat * format, SDL_Palette *palette)
{
    if (!format) {
        return SDL_SetError("SDL_SetPixelFormatPalette() passed NULL format");
    }

    if (palette && palette->ncolors > (1 << format->BitsPerPixel)) {
        return SDL_SetError("SDL_SetPixelFormatPalette() passed a palette that doesn't match the format");
    }

    if (format->palette == palette) {
        return 0;
    }

    if (format->palette) {
        SDL_FreePalette(format->palette);
    }

    format->palette = palette;

    if (format->palette) {
        ++format->palette->refcount;
    }

    return 0;
}

int
SDL_SetPaletteColors(SDL_Palette * palette, const SDL_Color * colors,
                     int firstcolor, int ncolors)
{
    int status = 0;

    /* Verify the parameters */
    if (!palette) {
        return -1;
    }
    if (ncolors > (palette->ncolors - firstcolor)) {
        ncolors = (palette->ncolors - firstcolor);
        status = -1;
    }

    if (colors != (palette->colors + firstcolor)) {
        SDL_memcpy(palette->colors + firstcolor, colors,
                   ncolors * sizeof(*colors));
    }
    ++palette->version;
    if (!palette->version) {
        palette->version = 1;
    }

    return status;
}

void
SDL_FreePalette(SDL_Palette * palette)
{
    if (!palette) {
        SDL_InvalidParamError("palette");
        return;
    }
    if (--palette->refcount > 0) {
        return;
    }
    SDL_free(palette->colors);
    SDL_free(palette);
}


	  
/*
 * Create an empty RGB surface of the appropriate depth using the given
 * enum SDL_PIXELFORMAT_* format
 */
SDL_Surface *
SDL_CreateRGBSurfaceWithFormat(Uint32 flags, int width, int height, int depth,
                               Uint32 format)
{
    SDL_Surface *surface;

    /* The flags are no longer used, make the compiler happy */
    (void)flags;

    /* Allocate the surface */
    surface = (SDL_Surface *) SDL_calloc(1, sizeof(*surface));
    if (surface == NULL) {
        SDL_OutOfMemory();
        return NULL;
    }

    surface->format = SDL_AllocFormat(format);
    if (!surface->format) {
        SDL_FreeSurface(surface);
        return NULL;
    }
    surface->w = width;
    surface->h = height;
    surface->pitch = SDL_CalculatePitch(format, width);
    SDL_SetClipRect(surface, NULL);

    if (SDL_ISPIXELFORMAT_INDEXED(surface->format->format)) {
        SDL_Palette *palette =
            SDL_AllocPalette((1 << surface->format->BitsPerPixel));
        if (!palette) {
            SDL_FreeSurface(surface);
            return NULL;
        }
        if (palette->ncolors == 2) {
            /* Create a black and white bitmap palette */
            palette->colors[0].r = 0xFF;
            palette->colors[0].g = 0xFF;
            palette->colors[0].b = 0xFF;
            palette->colors[1].r = 0x00;
            palette->colors[1].g = 0x00;
            palette->colors[1].b = 0x00;
        }
        SDL_SetSurfacePalette(surface, palette);
        SDL_FreePalette(palette);
    }

    /* Get the pixels */
    if (surface->w && surface->h) {
        /* Assumptions checked in surface_size_assumptions assert above */
        Sint64 size = ((Sint64)surface->h * surface->pitch);
        if (size < 0 || size > SDL_MAX_SINT32) {
            /* Overflow... */
            SDL_FreeSurface(surface);
            SDL_OutOfMemory();
            return NULL;
        }

        surface->pixels = SDL_malloc((size_t)size);
        if (!surface->pixels) {
            SDL_FreeSurface(surface);
            SDL_OutOfMemory();
            return NULL;
        }
        /* This is important for bitmaps */
        SDL_memset(surface->pixels, 0, surface->h * surface->pitch);
    }

    /* Allocate an empty mapping */
    surface->map = SDL_AllocBlitMap();
    if (!surface->map) {
        SDL_FreeSurface(surface);
        return NULL;
    }

    /* By default surface with an alpha mask are set up for blending */
    if (surface->format->Amask) {
        //SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);  //arczi
    }

    /* The surface is ready to go */
    surface->refcount = 1;
    return surface;
}
	
char* SDL_GetPrefPath(const char* org, const char* app) { return org; }

#ifdef __cplusplus
}
#endif
