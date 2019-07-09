#include <SDL/SDL.h>

struct SDL_Window
{
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
struct SDL_Renderer {};
struct SDL_Texture {};


extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;

/*
 * klaus
 * make sdl1_wrapper.h & .cpp
 * make structs for sdl2 datatypes and
 * redefine functions
 * and link the include to everywhere needed
 */
int SDL_WarpMouseInWindow(*window);
int SDL_RenderGetViewport(*renderer );
int SDL_RenderGetScale(*renderer );
int SDL_Log(*renderer);
int SDL_SetWindowTitle(*renderer);

