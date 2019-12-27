#pragma once

#include "devilution.h"
#include <SDL.h>
#include <type_traits>

namespace dvl {

	/* START ANDROID INPUT FUNKTIONS */
	#ifdef __ANDROID__
	void DisplayJoyStick(int x , int y, int h, int w, SDL_Surface * SrcSurface, SDL_Surface * DstSurface);
	void LoadAndroidImages();
	void DrawJoyStick(int MouseX, int MouseY, bool flag);
	void walkInDir(int dir);
	void __fastcall checkTownersNearby(bool interact);
	void DrawAndroidUI();
	bool __fastcall checkMonstersNearby(bool attack, bool spellcast);
	void convert_touch_xy_to_game_xy(float touch_x, float touch_y, int *game_x, int *game_y);
	//void __fastcall checkItemsNearby(bool interact);
	void AutoPickGold(int pnum);
	void AutoPickItem(int pnum);
	void ActivateObject(bool interact);
	void __fastcall checkItemsNearby(int pnum);
	void useBeltPotion(bool mana);

	extern bool showJoystick;
	extern int TouchX;
	extern int TouchY;

	extern SDL_Surface * JoyStickS;
	extern SDL_Texture * JoyStickT;
	extern SDL_Surface * AJoyStickS;
	extern SDL_Texture * AJoyStickT;
	extern SDL_Surface * ShiftStickS;
	extern SDL_Texture * ShiftStickT;
	extern SDL_Surface * DemoSqS;
	extern SDL_Texture * DemoSqT;
	extern SDL_Surface * Fog ;
	extern SDL_Texture * gFog ;
	extern bool ShiftButtonPressed ;
	extern bool AttackButtonPressed;
	extern bool CastButtonPressed;
	
// Defined in plrctrls.cpp / h	
	typedef struct androidcoords {
	int x;
	int y;
} androidcoords;
extern androidcoords androidspeedspellscoords[50];

extern SDL_Rect Arect;
extern SDL_Rect Shiftrect;
extern SDL_Rect LGameUIMenu;
extern SDL_Rect RGameUIMenu;
extern SDL_Rect PotGameUIMenu;
extern SDL_Rect Crect;

extern SDL_Rect DemoN;
extern SDL_Rect DemoS;
extern SDL_Rect DemoE;
extern SDL_Rect DemoW;

extern SDL_Rect DemoNW;
extern SDL_Rect DemoNE;	
extern SDL_Rect DemoSW;
extern SDL_Rect DemoSE;

extern SDL_Rect DemonHealth;
extern SDL_Rect AngelMana;

#endif

	/*Finish */



extern int refreshDelay;
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

// Whether the output surface requires software scaling.
// Always returns false on SDL2.
bool OutputRequiresScaling();

// Scales rect if necessary.
void ScaleOutputRect(SDL_Rect *rect);

// Convert from output coordinates to logical (resolution-independent) coordinates.
template <
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
void OutputToLogical(T *x, T *y)
{
#ifndef USE_SDL1
	if (!renderer)
		return;
	float scaleX;
	SDL_RenderGetScale(renderer, &scaleX, NULL);
	*x /= scaleX;
	*y /= scaleX;

	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	*x -= view.x;
	*y -= view.y;
#else
	if (!OutputRequiresScaling())
		return;
	const auto *surface = GetOutputSurface();
	*x = *x * SCREEN_WIDTH / surface->w;
	*y = *y * SCREEN_HEIGHT / surface->h;
#endif
}

template <
    typename T,
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
void LogicalToOutput(T *x, T *y)
{
#ifndef USE_SDL1
	if (!renderer)
		return;
	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	*x += view.x;
	*y += view.y;

	float scaleX;
	SDL_RenderGetScale(renderer, &scaleX, NULL);
	*x *= scaleX;
	*y *= scaleX;
#else
	if (!OutputRequiresScaling())
		return;
	const auto *surface = GetOutputSurface();
	*x = *x * surface->w / SCREEN_WIDTH;
	*y = *y * surface->h / SCREEN_HEIGHT;
#endif
}

} // namespace dvl
