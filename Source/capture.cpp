#include "Capture.h"
#include "DiabloUI/diabloui.h"
#include "engine/render/text_render.hpp"
#include "engine/dx.h"
#include "options.h"
#include "utils/png.h"
#include "utils/sdl_geometry.h"
#include "utils/sdl_ptrs.h"
#include "controls/touch/renderers.h"
#include <SDL_image.h> 

#include <cstddef>
#include <cstdint>
#include <memory>

#ifndef USE_SDL1
#include <SDL_image.h>
#else
#include "stubs.h"
#endif
#include "utils/image_utils.hpp"


namespace devilution {

SDLSurfaceUniquePtr ScaleSurfaceToOutput(Surface srcSurface)
{
	const SDL_Rect outputViewport = Renderer::GetOutputViewport();

	int w = outputViewport.w;
	int h = outputViewport.h;

	SDLSurfaceUniquePtr dstSurface(SDLWrap::CreateRGBSurfaceWithFormat(0, w, h, 8, SDL_PIXELFORMAT_INDEX8));

	if (SDL_BlitScaled(srcSurface.getSDLSurface(), nullptr, dstSurface.get(), nullptr) <= -1) {
		ErrSdl();
	}

	return dstSurface;
}

void CaptureScreenshot(std::string path)
{
	Surface srcSurface = GlobalBackBuffer();

	if (*sgOptions.Graphics.upscale) {
		SDLSurfaceUniquePtr dstSurface = ScaleSurfaceToOutput(srcSurface);

		if (SDL_SavePNG(dstSurface.get(), path.c_str()) <= -1) {
			ErrSdl();
		}
	} else {
		if (SDL_SavePNG(srcSurface.getSDLSurface(), path.c_str()) <= -1) {
			ErrSdl();
		}
	}
}

} // namespace devilution
