#include <SDL/SDL.h>
// #include <SDL/SDL_shape.h>

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
struct SDL_DisplayMode {int h; int w;};
//struct SDL_WindowShaper {};
struct SDL_HitTest {};
struct SDL_WindowUserData {};
struct SDL_Renderer {};
struct SDL_Texture {};


// done:
// =====
typedef struct SDL_WindowShaper SDL_WindowShaper;
typedef struct SDL_ShapeDriver SDL_ShapeDriver;
typedef struct SDL_VideoDisplay SDL_VideoDisplay;
typedef struct SDL_VideoDevice SDL_VideoDevice;
typedef union {
    /** \brief A cutoff alpha value for binarization of the window shape's alpha channel. */
    Uint8 binarizationCutoff;
    SDL_Color colorKey;
} SDL_WindowShapeParams;
typedef enum {
    /** \brief The default mode, a binarized alpha cutoff of 1. */
    ShapeModeDefault,
    /** \brief A binarized alpha cutoff with a given integer value. */
    ShapeModeBinarizeAlpha,
    /** \brief A binarized alpha cutoff with a given integer value, but with the opposite comparison. */
    ShapeModeReverseBinarizeAlpha,
    /** \brief A color key is applied. */
    ShapeModeColorKey
} WindowShapeMode;

typedef struct SDL_WindowShapeMode {
    /** \brief The mode of these window-shape parameters. */
    WindowShapeMode mode;
    /** \brief Window-shape parameters. */
    SDL_WindowShapeParams parameters;
} SDL_WindowShapeMode;

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


// todo functions:
// ==============

void SDL_RenderGetViewport(SDL_Renderer* renderer, SDL_Rect* rect);

void SDL_RenderGetScale(SDL_Renderer* renderer, float* scaleX, float* scaleY);

void SDL_WarpMouseInWindow(SDL_Window* window, int x, int y);

void SDL_Log(const char* fmt);

void SDL_SetWindowTitle(SDL_Window* window, const char* title);

char* SDL_GetPrefPath(const char* org, const char* app);

const Uint8* SDL_GetKeyboardState(int* numkeys);

typedef int (SDLCALL * SDL_ThreadFunction) (void *data);

SDL_Thread* SDL_CreateThread(SDL_ThreadFunction fn, const char* name, void* data);

char* SDL_GetBasePath(void);

void SDL_EnableScreenSaver(void);

void SDL_DisableScreenSaver(void);

int SDL_SetWindowInputFocus(SDL_Window* window);

SDL_bool SDL_SetHint(const char* name, const char* value);

void SDL_ShowWindow(SDL_Window* window);

int SDL_GetCurrentDisplayMode(int displayIndex, SDL_DisplayMode* mode);

void SDL_GetWindowPosition(SDL_Window* window,  int* x, int* y);

int SDL_ShowSimpleMessageBox(Uint32      flags, const char* title, const char* message, SDL_Window* window);

SDL_bool SDL_IsScreenSaverEnabled(void);

void SDL_HideWindow(SDL_Window* window);

SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, Uint32 flags);

void SDL_FreePalette(SDL_Palette* palette);

void SDL_DestroyTexture(SDL_Texture* texture);

void SDL_DestroyRenderer(SDL_Renderer* renderer);

void SDL_DestroyWindow(SDL_Window* window);

SDL_Palette* SDL_AllocPalette(int ncolors);

int SDL_GetRendererOutputSize(SDL_Renderer* renderer, int* w, int* h);

SDL_Surface* SDL_CreateRGBSurfaceWithFormat(Uint32 flags, int width, int height, int depth, Uint32 format);

void SDL_GetWindowSize(SDL_Window* window, int* w, int* h);

SDL_Surface* SDL_GetWindowSurface(SDL_Window* window);

int SDL_SetSurfacePalette(SDL_Surface* surface, SDL_Palette* palette);

int SDL_UpdateTexture(SDL_Texture* texture, const SDL_Rect* rect, const void* pixels, int pitch);

int SDL_RenderClear(SDL_Renderer* renderer);

int SDL_RenderCopy(SDL_Renderer*   renderer, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect);

void SDL_RenderPresent(SDL_Renderer* renderer);

int SDL_UpdateWindowSurface(SDL_Window* window);

SDL_Renderer* SDL_CreateRenderer(SDL_Window* window, int index, Uint32 flags);

SDL_Texture* SDL_CreateTexture(SDL_Renderer* renderer, Uint32 format, int access, int w, int h);

int SDL_RenderSetLogicalSize(SDL_Renderer* renderer, int w, int h);





















