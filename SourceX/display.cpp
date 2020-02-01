#include "display.h"
#include "DiabloUI/diabloui.h"
#include "controls/controller.h"

#if defined(USE_SDL1) && defined(RETROFW)
#include <unistd.h>
#endif

#ifdef USE_SDL1
#ifndef SDL1_VIDEO_MODE_BPP
#define SDL1_VIDEO_MODE_BPP 0
#endif
#ifndef SDL1_VIDEO_MODE_FLAGS
#define SDL1_VIDEO_MODE_FLAGS SDL_SWSURFACE
#endif
#ifndef SDL1_VIDEO_MODE_WIDTH
#define SDL1_VIDEO_MODE_WIDTH nWidth
#endif
#ifndef SDL1_VIDEO_MODE_HEIGHT
#define SDL1_VIDEO_MODE_HEIGHT nHeight
#endif
#endif

namespace dvl {

extern SDL_Surface *renderer_texture_surface; // defined in dx.cpp

namespace {

#ifdef USE_SDL1
void InitVideoMode(int width, int height, int bpp, std::uint32_t flags)
{
	const auto &best = *SDL_GetVideoInfo();
	SDL_Log("Best video mode reported as: %dx%d bpp=%d hw_available=%u",
	    best.current_w, best.current_h, best.vfmt->BitsPerPixel, best.hw_available);
	SDL_Log("Setting video mode %dx%d bpp=%u flags=0x%08X", width, height, bpp, flags);
	SDL_SetVideoMode(width, height, bpp, flags);
	const auto &current = *SDL_GetVideoInfo();
	SDL_Log("Video mode is now %dx%d bpp=%u",
	    current.current_w, current.current_h, current.vfmt->BitsPerPixel);
}
#endif

} // namespace

bool SpawnWindow(const char *lpWindowName, int nWidth, int nHeight)
{
	if (SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_HAPTIC) <= -1) {
		ErrSdl();
	}

	atexit(SDL_Quit);

#ifdef USE_SDL1
	SDL_EnableUNICODE(1);
#endif
#if defined(USE_SDL1) || defined(__SWITCH__)
	InitController();
#endif

	int upscale = 1;
	DvlIntSetting("upscale", &upscale);
	if (fullscreen)
		DvlIntSetting("fullscreen", (int *)&fullscreen);

	int grabInput = 1;
	DvlIntSetting("grab input", &grabInput);

#ifdef USE_SDL1
	int flags = SDL1_VIDEO_MODE_FLAGS | SDL_HWPALETTE;
	if (fullscreen)
		flags |= SDL_FULLSCREEN;
	SDL_WM_SetCaption(lpWindowName, WINDOW_ICON_NAME);
#ifndef RETROFW
	InitVideoMode(SDL1_VIDEO_MODE_WIDTH, SDL1_VIDEO_MODE_HEIGHT, SDL1_VIDEO_MODE_BPP, flags);
#else // RETROFW
	// JZ4760 IPU scaler (e.g. on RG-300 v2/3) - automatic high-quality scaling.
	if (access("/proc/jz/ipu", F_OK) == 0 || access("/proc/jz/ipu_ratio", F_OK) == 0) {
		InitVideoMode(SDL1_VIDEO_MODE_WIDTH, SDL1_VIDEO_MODE_HEIGHT, SDL1_VIDEO_MODE_BPP, flags);
	} else {
		// Other RetroFW devices have 320x480 screens with non-square pixels.
		InitVideoMode(320, 480, SDL1_VIDEO_MODE_BPP, flags);
	}
#endif
	window = SDL_GetVideoSurface();
	SDL_Log("Output surface: %dx%d sw-scaling=%d bpp=%d", window->w, window->h, OutputRequiresScaling(), window->format->BitsPerPixel);
	if (grabInput)
		SDL_WM_GrabInput(SDL_GRAB_ON);
	atexit(SDL_VideoQuit); // Without this video mode is not restored after fullscreen.
#else
	int flags = 0;
	if (upscale) {
		flags |= fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_RESIZABLE;

		char scaleQuality[2] = "2";
		DvlStringSetting("scaling quality", scaleQuality, 2);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaleQuality);
	} else if (fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (grabInput) {
		flags |= SDL_WINDOW_INPUT_GRABBED;
	}

	window = SDL_CreateWindow(lpWindowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, nWidth, nHeight, flags);
#endif
	if (window == NULL) {
		ErrSdl();
	}

#ifdef USE_SDL1
	refreshDelay = 1000000 / 60; // 60hz
#else
	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, 0, &mode);
	refreshDelay = 1000000 / mode.refresh_rate;
#endif

	if (upscale) {
#ifdef USE_SDL1
		SDL_Log("upscaling not supported with USE_SDL1");
#else
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
		if (renderer == NULL) {
			ErrSdl();
		}

		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, nWidth, nHeight);
		if (texture == NULL) {
			ErrSdl();
		}

		if (SDL_RenderSetLogicalSize(renderer, nWidth, nHeight) <= -1) {
			ErrSdl();
		}
#endif
	}

	return window != NULL;
}

SDL_Surface *GetOutputSurface()
{
#ifdef USE_SDL1
	return SDL_GetVideoSurface();
#else
	if (renderer)
		return renderer_texture_surface;
	return SDL_GetWindowSurface(window);
#endif
}

bool OutputRequiresScaling()
{
#ifdef USE_SDL1
	return SCREEN_WIDTH != GetOutputSurface()->w || SCREEN_HEIGHT != GetOutputSurface()->h;
#else // SDL2, scaling handled by renderer.
	return false;
#endif
}

void ScaleOutputRect(SDL_Rect *rect)
{
	if (!OutputRequiresScaling())
		return;
	const auto *surface = GetOutputSurface();
	rect->x = rect->x * surface->w / SCREEN_WIDTH;
	rect->y = rect->y * surface->h / SCREEN_HEIGHT;
	rect->w = rect->w * surface->w / SCREEN_WIDTH;
	rect->h = rect->h * surface->h / SCREEN_HEIGHT;
}

} // namespace dvl
