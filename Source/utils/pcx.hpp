#pragma once

#include <cstdint>

namespace devilution {

struct PCXHeader {
	uint8_t Manufacturer;
	uint8_t Version;
	uint8_t Encoding;
	uint8_t BitsPerPixel;
	uint16_t Xmin;
	uint16_t Ymin;
	uint16_t Xmax;
	uint16_t Ymax;
	uint16_t HDpi;
	uint16_t VDpi;
	uint8_t Colormap[48];
	uint8_t Reserved;
	uint8_t NPlanes;
	uint16_t BytesPerLine;
	uint16_t PaletteInfo;
	uint16_t HscreenSize;
	uint16_t VscreenSize;
	uint8_t Filler[54];
};

} // namespace devilution
