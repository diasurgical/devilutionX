#include "utils/display.h"

#include "DiabloUI/diabloui.h"
#include "control.h"
#include "controls/controller.h"
#include "controls/devices/game_controller.h"
#include "controls/devices/joystick.h"
#include "controls/devices/kbcontroller.h"
#include "controls/game_controls.h"
#include "options.h"
#include "utils/log.hpp"
#include "utils/sdl_wrap.h"

namespace devilution {

extern SDLSurfaceUniquePtr RendererTextureSurface; /** defined in dx.cpp */

Uint16 gnScreenWidth;
Uint16 gnScreenHeight;
Uint16 gnViewportHeight;

bool IsFullScreen()
{
	return (SDL_GetWindowFlags(ghMainWnd) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
}

void AdjustToScreenGeometry(int width, int height)
{
	gnScreenWidth = width;
	gnScreenHeight = height;

	gnViewportHeight = gnScreenHeight;
	if (gnScreenWidth <= PANEL_WIDTH) {
		// Part of the screen is fully obscured by the UI
		gnViewportHeight -= PANEL_HEIGHT;
	}
}

void CalculatePreferdWindowSize(int &width, int &height)
{
	SDL_DisplayMode mode;
	if (SDL_GetDesktopDisplayMode(0, &mode) != 0) {
		ErrSdl();
	}

	if (!sgOptions.Graphics.bIntegerScaling) {
		float wFactor = (float)mode.w / width;
		float hFactor = (float)mode.h / height;

		if (wFactor > hFactor) {
			width = mode.w * height / mode.h;
		} else {
			height = mode.h * width / mode.w;
		}
		return;
	}

	int wFactor = mode.w / width;
	int hFactor = mode.h / height;

	if (wFactor > hFactor) {
		width = mode.w / hFactor;
		height = mode.h / hFactor;
	} else {
		width = mode.w / wFactor;
		height = mode.h / wFactor;
	}
}

bool SpawnWindow(const char *lpWindowName)
{
#ifdef _WIN32
	// The default WASAPI backend causes distortions
	// https://github.com/diasurgical/devilutionX/issues/1434
	SDL_setenv("SDL_AUDIODRIVER", "winmm", /*overwrite=*/false);
#endif

	int initFlags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#ifndef NOSOUND
	initFlags |= SDL_INIT_AUDIO;
#endif
	initFlags |= SDL_INIT_GAMECONTROLLER;

	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
	if (SDL_Init(initFlags) <= -1) {
		ErrSdl();
	}

	if (sgOptions.Controller.szMapping[0] != '\0') {
		SDL_GameControllerAddMapping(sgOptions.Controller.szMapping);
	}

	int width = sgOptions.Graphics.nWidth;
	int height = sgOptions.Graphics.nHeight;

	if (sgOptions.Graphics.bUpscale && sgOptions.Graphics.bFitToScreen) {
		CalculatePreferdWindowSize(width, height);
	}
	AdjustToScreenGeometry(width, height);

	int flags = 0;
	if (sgOptions.Graphics.bUpscale) {
		if (!gbForceWindowed && sgOptions.Graphics.bFullscreen) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		flags |= SDL_WINDOW_RESIZABLE;

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, sgOptions.Graphics.szScaleQuality);
	} else if (!gbForceWindowed && sgOptions.Graphics.bFullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (sgOptions.Gameplay.bGrabInput) {
		flags |= SDL_WINDOW_INPUT_GRABBED;
	}

	ghMainWnd = SDL_CreateWindow(lpWindowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
	if (ghMainWnd == nullptr) {
		ErrSdl();
	}

	int refreshRate = 60;
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, 0, &mode);
	if (mode.refresh_rate != 0) {
		refreshRate = mode.refresh_rate;
	}
	refreshDelay = 1000000 / refreshRate;

	if (sgOptions.Graphics.bUpscale) {
		Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;

		if (sgOptions.Graphics.bVSync) {
			rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
		}

		renderer = SDL_CreateRenderer(ghMainWnd, -1, rendererFlags);
		if (renderer == nullptr) {
			ErrSdl();
		}

		texture = SDLWrap::CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);

		if (sgOptions.Graphics.bIntegerScaling && SDL_RenderSetIntegerScale(renderer, SDL_TRUE) < 0) {
			ErrSdl();
		}

		if (SDL_RenderSetLogicalSize(renderer, width, height) <= -1) {
			ErrSdl();
		}
	} else {
		SDL_GetWindowSize(ghMainWnd, &width, &height);
		AdjustToScreenGeometry(width, height);
	}

	return ghMainWnd != nullptr;
}

SDL_Surface *GetOutputSurface()
{
	if (renderer != nullptr)
		return RendererTextureSurface.get();
	SDL_Surface *ret = SDL_GetWindowSurface(ghMainWnd);
	if (ret == nullptr)
		ErrSdl();
	return ret;
}

bool OutputRequiresScaling()
{
	return false;
}

void ScaleOutputRect(SDL_Rect *rect)
{
	if (!OutputRequiresScaling())
		return;
	const SDL_Surface *surface = GetOutputSurface();
	rect->x = rect->x * surface->w / gnScreenWidth;
	rect->y = rect->y * surface->h / gnScreenHeight;
	rect->w = rect->w * surface->w / gnScreenWidth;
	rect->h = rect->h * surface->h / gnScreenHeight;
}

SDLSurfaceUniquePtr ScaleSurfaceToOutput(SDLSurfaceUniquePtr surface)
{
	return surface;
}

} // namespace devilution
