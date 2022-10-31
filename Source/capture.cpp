/**
 * @file capture.cpp
 *
 * Implementation of the screenshot function.
 */
#include <cstdint>
#include <fmt/chrono.h>
#include <fstream>

#include "DiabloUI/diabloui.h"
#include "engine/dx.h"
#include "engine/palette.h"
#include "utils/file_util.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/pcx.hpp"
#include "utils/str_cat.hpp"
#include "utils/ui_fwd.h"

namespace devilution {
namespace {

/**
 * @brief Write the PCX-file header
 * @param width Image width
 * @param height Image height
 * @param out File stream to write to
 * @return True on success
 */
bool CaptureHdr(int16_t width, int16_t height, std::ofstream *out)
{
	PCXHeader buffer;

	memset(&buffer, 0, sizeof(buffer));
	buffer.Manufacturer = 10;
	buffer.Version = 5;
	buffer.Encoding = 1;
	buffer.BitsPerPixel = 8;
	buffer.Xmax = SDL_SwapLE16(width - 1);
	buffer.Ymax = SDL_SwapLE16(height - 1);
	buffer.HDpi = SDL_SwapLE16(width);
	buffer.VDpi = SDL_SwapLE16(height);
	buffer.NPlanes = 1;
	buffer.BytesPerLine = SDL_SwapLE16(width);

	out->write(reinterpret_cast<const char *>(&buffer), sizeof(buffer));
	return !out->fail();
}

/**
 * @brief Write the current in-game palette to the PCX file
 * @param palette Current palette
 * @param out File stream for the PCX file.
 * @return True if successful, else false
 */
bool CapturePal(SDL_Color *palette, std::ofstream *out)
{
	uint8_t pcxPalette[1 + 256 * 3];

	pcxPalette[0] = 12;
	for (int i = 0; i < 256; i++) {
		pcxPalette[1 + 3 * i + 0] = palette[i].r;
		pcxPalette[1 + 3 * i + 1] = palette[i].g;
		pcxPalette[1 + 3 * i + 2] = palette[i].b;
	}

	out->write(reinterpret_cast<const char *>(pcxPalette), sizeof(pcxPalette));
	return !out->fail();
}

/**
 * @brief RLE compress the pixel data
 * @param src Raw pixel buffer
 * @param dst Output buffer
 * @param width Width of pixel buffer

 * @return Output buffer
 */
uint8_t *CaptureEnc(uint8_t *src, uint8_t *dst, int width)
{
	int rleLength;

	do {
		uint8_t rlePixel = *src;
		src++;
		rleLength = 1;

		width--;

		while (rlePixel == *src) {
			if (rleLength >= 63)
				break;
			if (width == 0)
				break;
			rleLength++;

			width--;
			src++;
		}

		if (rleLength > 1 || rlePixel > 0xBF) {
			*dst = rleLength | 0xC0;
			dst++;
		}

		*dst = rlePixel;
		dst++;
	} while (width > 0);

	return dst;
}

/**
 * @brief Write the pixel data to the PCX file
 *
 * @param buf Pixel data
 * @param out File stream for the PCX file.
 * @return True if successful, else false
 */
bool CapturePix(const Surface &buf, std::ofstream *out)
{
	int width = buf.w();
	std::unique_ptr<uint8_t[]> pBuffer { new uint8_t[2 * width] };
	uint8_t *pixels = buf.begin();
	for (int height = buf.h(); height > 0; height--) {
		const uint8_t *pBufferEnd = CaptureEnc(pixels, pBuffer.get(), width);
		pixels += buf.pitch();
		out->write(reinterpret_cast<const char *>(pBuffer.get()), pBufferEnd - pBuffer.get());
		if (out->fail())
			return false;
	}
	return true;
}

std::ofstream CaptureFile(std::string *dstPath)
{
	std::time_t tt = std::time(nullptr);
	std::tm *tm = std::localtime(&tt);
	std::string filename = fmt::format("Screenshot from {:%Y-%m-%d %H-%M-%S}", *tm);
	*dstPath = StrCat(paths::PrefPath(), filename, ".pcx");
	int i = 0;
	while (FileExists(dstPath->c_str())) {
		i++;
		*dstPath = StrCat(paths::PrefPath(), filename, "-", i, ".pcx");
	}
	return std::ofstream(*dstPath, std::ios::binary | std::ios::trunc);
}

/**
 * @brief Make a red version of the given palette and apply it to the screen.
 */
void RedPalette()
{
	for (int i = 0; i < 256; i++) {
		system_palette[i].g = 0;
		system_palette[i].b = 0;
	}
	palette_update();
	BltFast(nullptr, nullptr);
	RenderPresent();
}
} // namespace

void CaptureScreen()
{
	SDL_Color palette[256];
	std::string fileName;
	bool success;

	std::ofstream outStream = CaptureFile(&fileName);
	if (!outStream.is_open())
		return;
	DrawAndBlit();
	PaletteGetEntries(256, palette);
	RedPalette();

	const Surface &buf = GlobalBackBuffer();
	success = CaptureHdr(buf.w(), buf.h(), &outStream);
	if (success) {
		success = CapturePix(buf, &outStream);
	}
	if (success) {
		success = CapturePal(palette, &outStream);
	}
	outStream.close();

	if (!success) {
		Log("Failed to save screenshot at {}", fileName);
		RemoveFile(fileName.c_str());
	} else {
		Log("Screenshot saved at {}", fileName);
	}
	SDL_Delay(300);
	for (int i = 0; i < 256; i++) {
		system_palette[i] = palette[i];
	}
	palette_update();
	force_redraw = 255;
}

} // namespace devilution
