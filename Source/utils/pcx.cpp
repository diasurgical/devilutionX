#include "utils/pcx.hpp"

#include <SDL_endian.h>
#include <cstring>
#include <memory>

#include "appfat.h"

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

namespace devilution {
namespace {
constexpr unsigned NumPaletteColors = 256;
} // namespace

bool LoadPcxMeta(SDL_RWops *handle, int &width, int &height, uint8_t &bpp)
{
	PCXHeader pcxhdr;
	if (SDL_RWread(handle, &pcxhdr, PcxHeaderSize, 1) == 0) {
		return false;
	}
	width = SDL_SwapLE16(pcxhdr.Xmax) - SDL_SwapLE16(pcxhdr.Xmin) + 1;
	height = SDL_SwapLE16(pcxhdr.Ymax) - SDL_SwapLE16(pcxhdr.Ymin) + 1;
	bpp = pcxhdr.BitsPerPixel;
	return true;
}

} // namespace devilution
