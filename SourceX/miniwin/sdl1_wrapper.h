#include <SDL/SDL.h>


/*
 klaus
 make sdl1_wrapper.h & .cpp
 redefine sdl2 datatypes with sdl1 structs,
 redefine sdl2 functions in sdl1
 and link the include to everywhere needed.


 needed datatypes:
 ----------------
 SDL_Window
 SDL_Renderer
 SDL_Texture
 SDL_WindowUserData
 SDL_WindowShaper
 SDL_DisplayMode
 SDL_HitTest


 needed functions:
 -----------------
 SDL_WarpMouseInWindow(window, X, Y);
 SDL_RenderGetViewport(*renderer, SDL_Rect );
 SDL_RenderGetScale(*renderer, float, NULL );
 SDL_Log(SDL_GetError());
 SDL_SetWindowTitle(window, const *char);
 */


// todo structs:
// ============
struct SDL_DisplayMode {};
struct SDL_WindowShaper {};
struct SDL_HitTest {};
struct SDL_WindowUserData {};
struct SDL_Renderer {};
struct SDL_Texture {};


// done:
// =====
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



// todo functions:
// ==============
extern SDL_Window *dummy;
int SDL_WarpMouseInWindow(*dummy);
int SDL_RenderGetViewport(*dummy );
int SDL_RenderGetScale(*dummy );
int SDL_Log(*dummy);
int SDL_SetWindowTitle(*dummy);




