#include "utils/display.h"

#ifdef __vita__
#include <psp2/power.h>
#endif

#ifdef __3DS__
#include "platform/ctr/display.hpp"
#endif

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

#ifdef USE_SDL1
#ifndef SDL1_VIDEO_MODE_BPP
#define SDL1_VIDEO_MODE_BPP 0
#endif
#ifndef SDL1_VIDEO_MODE_FLAGS
#define SDL1_VIDEO_MODE_FLAGS SDL_SWSURFACE
#endif
#endif

namespace devilution {

extern SDLSurfaceUniquePtr RendererTextureSurface; /** defined in dx.cpp */

Uint16 gnScreenWidth;
Uint16 gnScreenHeight;
Uint16 gnViewportHeight;

Uint16 GetScreenWidth()
{
	return gnScreenWidth;
}

Uint16 GetScreenHeight()
{
	return gnScreenHeight;
}

Uint16 GetViewportHeight()
{
	return gnViewportHeight;
}

#ifdef USE_SDL1
void SetVideoMode(int width, int height, int bpp, uint32_t flags)
{
	Log("Setting video mode {}x{} bpp={} flags=0x{:08X}", width, height, bpp, flags);
	ghMainWnd = SDL_SetVideoMode(width, height, bpp, flags);
	if (ghMainWnd == nullptr) {
		ErrSdl();
	}
	const SDL_VideoInfo &current = *SDL_GetVideoInfo();
	Log("Video mode is now {}x{} bpp={} flags=0x{:08X}",
	    current.current_w, current.current_h, current.vfmt->BitsPerPixel, SDL_GetVideoSurface()->flags);
}

void SetVideoModeToPrimary(bool fullscreen, int width, int height)
{
	int flags = SDL1_VIDEO_MODE_FLAGS | SDL_HWPALETTE;
	if (fullscreen)
		flags |= SDL_FULLSCREEN;
#ifdef __3DS__
	flags &= ~SDL_FULLSCREEN;
	flags |= Get3DSScalingFlag(sgOptions.Graphics.bFitToScreen, width, height);
#endif
	SetVideoMode(width, height, SDL1_VIDEO_MODE_BPP, flags);
	if (OutputRequiresScaling())
		Log("Using software scaling");
}
#endif

bool IsFullScreen()
{
#ifdef USE_SDL1
	return (SDL_GetVideoSurface()->flags & SDL_FULLSCREEN) != 0;
#else
	return (SDL_GetWindowFlags(ghMainWnd) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
#endif
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
#ifdef USE_SDL1
	const SDL_VideoInfo &best = *SDL_GetVideoInfo();
	Log("Best video mode reported as: {}x{} bpp={} hw_available={}",
	    best.current_w, best.current_h, best.vfmt->BitsPerPixel, best.hw_available);
#else
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
#endif
}

bool SpawnWindow(const char *lpWindowName)
{
#ifdef __vita__
	scePowerSetArmClockFrequency(444);
#endif

#if SDL_VERSION_ATLEAST(2, 0, 6) && defined(__vita__)
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
#endif

#if defined(_WIN32) && !defined(USE_SDL1)
	// The default WASAPI backend causes distortions
	// https://github.com/diasurgical/devilutionX/issues/1434
	SDL_setenv("SDL_AUDIODRIVER", "winmm", /*overwrite=*/false);
#endif

	int initFlags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#ifndef NOSOUND
	initFlags |= SDL_INIT_AUDIO;
#endif
#ifndef USE_SDL1
	initFlags |= SDL_INIT_GAMECONTROLLER;

	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif
	if (SDL_Init(initFlags) <= -1) {
		ErrSdl();
	}

#ifndef USE_SDL1
	if (sgOptions.Controller.szMapping[0] != '\0') {
		SDL_GameControllerAddMapping(sgOptions.Controller.szMapping);
	}
#endif

#ifdef USE_SDL1
	SDL_EnableUNICODE(1);
#endif
#ifdef USE_SDL1
	// On SDL 1, there are no ADDED/REMOVED events.
	// Always try to initialize the first joystick.
	Joystick::Add(0);
#ifdef __SWITCH__
	// TODO: There is a bug in SDL2 on Switch where it does not report controllers on startup (Jan 1, 2020)
	GameController::Add(0);
#endif
#endif

	int width = sgOptions.Graphics.nWidth;
	int height = sgOptions.Graphics.nHeight;

	if (sgOptions.Graphics.bUpscale && sgOptions.Graphics.bFitToScreen) {
		CalculatePreferdWindowSize(width, height);
	}
	AdjustToScreenGeometry(width, height);

#ifdef USE_SDL1
	SDL_WM_SetCaption(lpWindowName, WINDOW_ICON_NAME);
	SetVideoModeToPrimary(!gbForceWindowed && sgOptions.Graphics.bFullscreen, width, height);
	if (*sgOptions.Gameplay.grabInput)
		SDL_WM_GrabInput(SDL_GRAB_ON);
	atexit(SDL_VideoQuit); // Without this video mode is not restored after fullscreen.
#else
	int flags = 0;
	if (sgOptions.Graphics.bUpscale) {
		if (!gbForceWindowed && sgOptions.Graphics.bFullscreen) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		flags |= SDL_WINDOW_RESIZABLE;

		auto quality = fmt::format("{}", static_cast<int>(*sgOptions.Graphics.scaleQuality));
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, quality.c_str());
	} else if (!gbForceWindowed && sgOptions.Graphics.bFullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (*sgOptions.Gameplay.grabInput) {
		flags |= SDL_WINDOW_INPUT_GRABBED;
	}

	ghMainWnd = SDL_CreateWindow(lpWindowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
#endif
	if (ghMainWnd == nullptr) {
		ErrSdl();
	}

	int refreshRate = 60;
#ifndef USE_SDL1
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, 0, &mode);
	if (mode.refresh_rate != 0) {
		refreshRate = mode.refresh_rate;
	}
#endif
	refreshDelay = 1000000 / refreshRate;

	if (sgOptions.Graphics.bUpscale) {
#ifndef USE_SDL1
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
#endif
	} else {
#ifdef USE_SDL1
		const SDL_VideoInfo &current = *SDL_GetVideoInfo();
		width = current.current_w;
		height = current.current_h;
#else
		SDL_GetWindowSize(ghMainWnd, &width, &height);
#endif
		AdjustToScreenGeometry(width, height);
	}

	return ghMainWnd != nullptr;
}

SDL_Surface *GetOutputSurface()
{
#ifdef USE_SDL1
	SDL_Surface *ret = SDL_GetVideoSurface();
	if (ret == nullptr)
		ErrSdl();
	return ret;
#else
	if (renderer != nullptr)
		return RendererTextureSurface.get();
	SDL_Surface *ret = SDL_GetWindowSurface(ghMainWnd);
	if (ret == nullptr)
		ErrSdl();
	return ret;
#endif
}

bool OutputRequiresScaling()
{
#ifdef USE_SDL1
	return gnScreenWidth != GetOutputSurface()->w || gnScreenHeight != GetOutputSurface()->h;
#else // SDL2, scaling handled by renderer.
	return false;
#endif
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

#ifdef USE_SDL1
namespace {

SDLSurfaceUniquePtr CreateScaledSurface(SDL_Surface *src)
{
	SDL_Rect stretched_rect = { 0, 0, static_cast<Uint16>(src->w), static_cast<Uint16>(src->h) };
	ScaleOutputRect(&stretched_rect);
	SDLSurfaceUniquePtr stretched = SDLWrap::CreateRGBSurface(
	    SDL_SWSURFACE, stretched_rect.w, stretched_rect.h, src->format->BitsPerPixel,
	    src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
	if (SDL_HasColorKey(src)) {
		SDL_SetColorKey(stretched.get(), SDL_SRCCOLORKEY, src->format->colorkey);
		if (src->format->palette != NULL)
			SDL_SetPalette(stretched.get(), SDL_LOGPAL, src->format->palette->colors, 0, src->format->palette->ncolors);
	}
	if (SDL_SoftStretch((src), NULL, stretched.get(), &stretched_rect) < 0)
		ErrSdl();
	return stretched;
}

} // namespace
#endif // USE_SDL1

SDLSurfaceUniquePtr ScaleSurfaceToOutput(SDLSurfaceUniquePtr surface)
{
#ifdef USE_SDL1
	if (OutputRequiresScaling())
		return CreateScaledSurface(surface.get());
#endif
	return surface;
}

} // namespace devilution
