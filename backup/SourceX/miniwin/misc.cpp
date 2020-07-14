#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"
#include <SDL.h>
#include <string>

#include "controls/controller.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/dialogs.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

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
#endif

namespace dvl {

DWORD last_error;

DWORD GetLastError()
{
	return last_error;
}

void SetLastError(DWORD dwErrCode)
{
	last_error = dwErrCode;
}

int wsprintfA(LPSTR dest, LPCSTR format, ...)
{
	va_list args;
	va_start(args, format);
	return vsprintf(dest, format, args);
}

int wvsprintfA(LPSTR dest, LPCSTR format, va_list arglist)
{
	return vsnprintf(dest, 256, format, arglist);
}

int _strcmpi(const char *_Str1, const char *_Str2)
{
	return strcasecmp(_Str1, _Str2);
}

int _strnicmp(const char *_Str1, const char *_Str2, size_t n)
{
	return strncasecmp(_Str1, _Str2, n);
}

char *_itoa(int _Value, char *_Dest, int _Radix)
{
	switch (_Radix) {
	case 8:
		sprintf(_Dest, "%o", _Value);
		break;
	case 10:
		sprintf(_Dest, "%d", _Value);
		break;
	case 16:
		sprintf(_Dest, "%x", _Value);
		break;
	default:
		UNIMPLEMENTED();
		break;
	}

	return _Dest;
}

DWORD GetTickCount()
{
	return SDL_GetTicks();
}

void Sleep(DWORD dwMilliseconds)
{
	SDL_Delay(dwMilliseconds);
}

WINBOOL GetComputerNameA(LPSTR lpBuffer, LPDWORD nSize)
{
	DUMMY();
	strncpy(lpBuffer, "localhost", *nSize);
	*nSize = strlen(lpBuffer);
	return true;
}

WINBOOL DeleteFileA(LPCSTR lpFileName)
{
	char name[DVL_MAX_PATH];
	TranslateFileName(name, sizeof(name), lpFileName);

	FILE *f = fopen(name, "r+");
	if (f) {
		fclose(f);
		remove(name);
		f = NULL;
		eprintf("Removed file: %s\n", name);
	} else {
		eprintf("Failed to remove file: %s\n", name);
	}

	return true;
}

bool SpawnWindow(LPCSTR lpWindowName, int nWidth, int nHeight)
{
	if (SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_HAPTIC) <= -1) {
		ErrSdl();
	}

	atexit(SDL_Quit);

#ifdef USE_SDL1
	SDL_EnableUNICODE(1);
#endif

	InitController();

	int upscale = 1;
	DvlIntSetting("upscale", &upscale);
	DvlIntSetting("fullscreen", (int *)&fullscreen);

	int grabInput = 1;
	DvlIntSetting("grab input", &grabInput);

#ifdef USE_SDL1
	int flags = SDL1_VIDEO_MODE_FLAGS | SDL_HWPALETTE;
	if (fullscreen)
		flags |= SDL_FULLSCREEN;
	SDL_WM_SetCaption(lpWindowName, WINDOW_ICON_NAME);
#ifndef RETROFW
	SDL_SetVideoMode(nWidth, nHeight, SDL1_VIDEO_MODE_BPP, flags);
#else // RETROFW
	// JZ4760 IPU scaler (e.g. on RG-300 v2/3) - automatic high-quality scaling.
	if (access("/proc/jz/ipu_ratio", F_OK) == 0) {
		SDL_SetVideoMode(nWidth, nHeight, SDL1_VIDEO_MODE_BPP, flags);
	} else {
		// Other RetroFW devices have 320x480 screens with non-square pixels.
		SDL_SetVideoMode(320, 480, SDL1_VIDEO_MODE_BPP, flags);
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
	refreshDelay = 16; // rougly 60hz
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
	} else {
#ifndef USE_SDL1
		SDL_DisplayMode mode;
		SDL_GetDisplayMode(0, 0, &mode);
		refreshDelay = 1000 / mode.refresh_rate;
#endif
	}

	return window != NULL;
}
} // namespace dvl
