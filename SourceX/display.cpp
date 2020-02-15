#include "display.h"
#include "DiabloUI/diabloui.h"
#include "controls/controller.h"

#ifdef USE_SDL1
#include <SDL_rotozoom.h>
#ifndef SDL1_VIDEO_MODE_BPP
#define SDL1_VIDEO_MODE_BPP 0
#endif
#ifndef SDL1_VIDEO_MODE_FLAGS
#define SDL1_VIDEO_MODE_FLAGS SDL_SWSURFACE
#endif
#ifndef SDL1_VIDEO_MODE_WIDTH
#define SDL1_VIDEO_MODE_WIDTH SCREEN_WIDTH
#endif
#ifndef SDL1_VIDEO_MODE_HEIGHT
#define SDL1_VIDEO_MODE_HEIGHT SCREEN_HEIGHT
#endif
#endif

namespace dvl {

extern SDL_Surface *renderer_texture_surface; // defined in dx.cpp

#ifdef USE_SDL1
void SetVideoMode(int width, int height, int bpp, std::uint32_t flags) {
	SDL_Log("Setting video mode %dx%d bpp=%u flags=0x%08X", width, height, bpp, flags);
	SDL_SetVideoMode(width, height, bpp, flags);
	const auto &current = *SDL_GetVideoInfo();
	SDL_Log("Video mode is now %dx%d bpp=%u flags=0x%08X",
	    current.current_w, current.current_h, current.vfmt->BitsPerPixel, SDL_GetVideoSurface()->flags);
	window = SDL_GetVideoSurface();
}

void SetVideoModeToPrimary(bool fullscreen) {
	int flags = SDL1_VIDEO_MODE_FLAGS | SDL_HWPALETTE;
	if (fullscreen)
		flags |= SDL_FULLSCREEN;
	SetVideoMode(SDL1_VIDEO_MODE_WIDTH, SDL1_VIDEO_MODE_HEIGHT, SDL1_VIDEO_MODE_BPP, flags);
	if (OutputRequiresScaling())
		SDL_Log("Using software scaling");
}

bool IsFullScreen() {
	return (SDL_GetVideoSurface()->flags & SDL_FULLSCREEN) != 0;
}
#endif

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
	SDL_WM_SetCaption(lpWindowName, WINDOW_ICON_NAME);
	const auto &best = *SDL_GetVideoInfo();
	SDL_Log("Best video mode reported as: %dx%d bpp=%d hw_available=%u",
	    best.current_w, best.current_h, best.vfmt->BitsPerPixel, best.hw_available);
	SetVideoModeToPrimary(fullscreen);
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

#ifdef USE_SDL1
namespace {

bool GetIntegerOutputDownscalingFactors(int *shrinkx, int *shrinky) {
	const auto *screen = GetOutputSurface();
	if (screen->w <= SCREEN_WIDTH && (SCREEN_WIDTH % screen->w) == 0 && screen->h <= SCREEN_HEIGHT && (SCREEN_HEIGHT % screen->h) == 0) {
		*shrinkx = SCREEN_WIDTH / screen->w;
		*shrinky = SCREEN_HEIGHT / screen->h;
		return true;
	}
	return false;
}

} // namespace

SDL_Surface *CreateOutputScaledSurface(SDL_Surface *src)
{
	const auto *screen = GetOutputSurface();
	SDL_Surface *result = nullptr;

	// We use the optimized down-scaling routine, shrinkSurface, when possible.
	int shrinkx, shrinky;
	if (!SDL_HasColorKey(src) && GetIntegerOutputDownscalingFactors(&shrinkx, &shrinky)) {
		if (src->format->BitsPerPixel == 8) {
			SDL_Surface *tmp = SDL_ConvertSurface(src, screen->format, 0);
			result = shrinkSurface(tmp, shrinkx, shrinky);
			SDL_FreeSurface(tmp);
		} else {
			result = shrinkSurface(src, shrinkx, shrinky);
		}
	} else {
		const double zoomx = static_cast<double>(screen->w) / SCREEN_WIDTH;
		const double zoomy = static_cast<double>(screen->h) / SCREEN_HEIGHT;
		result = zoomSurface(src, zoomx, zoomy, /*smooth=*/SMOOTHING_ON);
	}
	if (result == nullptr)
		ErrSdl();
	return result;
}
#endif // USE_SDL1

void ScaleSurfaceToOutput(SDL_Surface **surface)
{
#ifdef USE_SDL1
	if (!OutputRequiresScaling())
		return;
	SDL_Surface *stretched = CreateOutputScaledSurface(*surface);
	SDL_FreeSurface((*surface));
	*surface = stretched;
#endif
}

} // namespace dvl
