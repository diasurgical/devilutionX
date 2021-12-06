#include "utils/display.h"

#include <algorithm>

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
#include "dx.h"
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

namespace {

#ifndef USE_SDL1
void CalculatePreferdWindowSize(int &width, int &height)
{
	SDL_DisplayMode mode;
	if (SDL_GetDesktopDisplayMode(0, &mode) != 0) {
		ErrSdl();
	}

	if (mode.w < mode.h) {
		std::swap(mode.w, mode.h);
	}

	if (*sgOptions.Graphics.integerScaling) {
		int factor = std::min(mode.w / width, mode.h / height);
		width = mode.w / factor;
		height = mode.h / factor;
		return;
	}

	float wFactor = (float)mode.w / width;
	float hFactor = (float)mode.h / height;

	if (wFactor > hFactor) {
		width = mode.w * height / mode.h;
	} else {
		height = mode.h * width / mode.w;
	}
}
#endif

void AdjustToScreenGeometry(Size windowSize)
{
	gnScreenWidth = windowSize.width;
	gnScreenHeight = windowSize.height;

	gnViewportHeight = gnScreenHeight;
	if (gnScreenWidth <= PANEL_WIDTH) {
		// Part of the screen is fully obscured by the UI
		gnViewportHeight -= PANEL_HEIGHT;
	}
}

Size GetPreferredWindowSize()
{
	Size windowSize = *sgOptions.Graphics.resolution;

#ifndef USE_SDL1
	if (*sgOptions.Graphics.upscale && *sgOptions.Graphics.fitToScreen) {
		CalculatePreferdWindowSize(windowSize.width, windowSize.height);
	}
#endif
	AdjustToScreenGeometry(windowSize);
	return windowSize;
}
} // namespace

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
	flags |= Get3DSScalingFlag(*sgOptions.Graphics.fitToScreen, width, height);
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

	Size windowSize = GetPreferredWindowSize();

#ifdef USE_SDL1
	SDL_WM_SetCaption(lpWindowName, WINDOW_ICON_NAME);
	SetVideoModeToPrimary(!gbForceWindowed && *sgOptions.Graphics.fullscreen, windowSize.width, windowSize.height);
	if (*sgOptions.Gameplay.grabInput)
		SDL_WM_GrabInput(SDL_GRAB_ON);
	atexit(SDL_VideoQuit); // Without this video mode is not restored after fullscreen.
#else
	int flags = SDL_WINDOW_ALLOW_HIGHDPI;
	if (*sgOptions.Graphics.upscale) {
		if (!gbForceWindowed && *sgOptions.Graphics.fullscreen) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		flags |= SDL_WINDOW_RESIZABLE;
	} else if (!gbForceWindowed && *sgOptions.Graphics.fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (*sgOptions.Gameplay.grabInput) {
		flags |= SDL_WINDOW_INPUT_GRABBED;
	}

	ghMainWnd = SDL_CreateWindow(lpWindowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowSize.width, windowSize.height, flags);
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

	ReinitializeRenderer();

	return ghMainWnd != nullptr;
}

void ReinitializeRenderer()
{
	if (ghMainWnd == nullptr)
		return;

#ifdef USE_SDL1
	const SDL_VideoInfo &current = *SDL_GetVideoInfo();
	Size windowSize = { current.current_w, current.current_h };
	AdjustToScreenGeometry(windowSize);
#else
	if (texture)
		texture.reset();

	if (renderer != nullptr) {
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}

	if (*sgOptions.Graphics.upscale) {
		Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;

		if (*sgOptions.Graphics.vSync) {
			rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
		}

		renderer = SDL_CreateRenderer(ghMainWnd, -1, rendererFlags);
		if (renderer == nullptr) {
			ErrSdl();
		}

		auto quality = fmt::format("{}", static_cast<int>(*sgOptions.Graphics.scaleQuality));
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, quality.c_str());

		texture = SDLWrap::CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, gnScreenWidth, gnScreenHeight);

		if (*sgOptions.Graphics.integerScaling && SDL_RenderSetIntegerScale(renderer, SDL_TRUE) < 0) {
			ErrSdl();
		}

		if (SDL_RenderSetLogicalSize(renderer, gnScreenWidth, gnScreenHeight) <= -1) {
			ErrSdl();
		}

		Uint32 format;
		if (SDL_QueryTexture(texture.get(), &format, nullptr, nullptr, nullptr) < 0)
			ErrSdl();
		RendererTextureSurface = SDLWrap::CreateRGBSurfaceWithFormat(0, gnScreenWidth, gnScreenHeight, SDL_BITSPERPIXEL(format), format);
	} else {
		Size windowSize = {};
		SDL_GetWindowSize(ghMainWnd, &windowSize.width, &windowSize.height);
		AdjustToScreenGeometry(windowSize);
	}
#endif
}

void ResizeWindow()
{
	if (ghMainWnd == nullptr)
		return;

	Size windowSize = GetPreferredWindowSize();

#ifdef USE_SDL1
	SetVideoModeToPrimary(!gbForceWindowed && *sgOptions.Graphics.fullscreen, windowSize.width, windowSize.height);
#else
	int flags = 0;
	if (*sgOptions.Graphics.upscale) {
		if (!gbForceWindowed && *sgOptions.Graphics.fullscreen) {
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		SDL_SetWindowResizable(ghMainWnd, SDL_TRUE);
	} else if (!gbForceWindowed && *sgOptions.Graphics.fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
		SDL_SetWindowResizable(ghMainWnd, SDL_FALSE);
	}
	SDL_SetWindowFullscreen(ghMainWnd, flags);

	SDL_SetWindowSize(ghMainWnd, windowSize.width, windowSize.height);
#endif

	ReinitializeRenderer();
	dx_resize();
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
