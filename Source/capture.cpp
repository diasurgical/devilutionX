/**
 * @file capture.cpp
 *
 * Implementation of the screenshot function.
 */
#include <fstream>

#include "DiabloUI/diabloui.h"
#include "dx.h"
#include "palette.h"
#include "storm/storm.h"
#include "utils/file_util.h"
#include "utils/paths.h"
#include "utils/ui_fwd.h"
#include "utils/log.hpp"

namespace devilution {

/**
 * @brief Write the PCX-file header
 * @param width Image width
 * @param height Image height
 * @param out File stream to write to
 * @return True on success
 */
static bool CaptureHdr(short width, short height, std::ofstream *out)
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
 * @brief Write the current ingame palette to the PCX file
 * @param palette Current palette
 * @param out File stream for the PCX file.
 * @return True if successful, else false
 */
static bool CapturePal(SDL_Color *palette, std::ofstream *out)
{
	BYTE pcxPalette[1 + 256 * 3];
	int i;

	pcxPalette[0] = 12;
	for (i = 0; i < 256; i++) {
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
static BYTE *CaptureEnc(BYTE *src, BYTE *dst, int width)
{
	int rleLength;

	do {
		BYTE rlePixel = *src;
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
 * @param buf Buffer
 * @return True if successful, else false
 */
static bool CapturePix(const CelOutputBuffer &buf, std::ofstream *out)
{
	int width = buf.w();
	std::unique_ptr<BYTE[]> pBuffer { new BYTE[2 * width] };
	BYTE *pixels = buf.begin();
	for (int height = buf.h(); height > 0; height--) {
		const BYTE *pBufferEnd = CaptureEnc(pixels, pBuffer.get(), width);
		pixels += buf.pitch();
		out->write(reinterpret_cast<const char *>(pBuffer.get()), pBufferEnd - pBuffer.get());
		if (out->fail())
			return false;
	}
	return true;
}

/**
 * Returns a pointer because in GCC < 5 ofstream itself is not moveable due to a bug.
 */
static std::ofstream *CaptureFile(std::string *dstPath)
{
	char filename[sizeof("screen00.PCX") / sizeof(char)];
	for (int i = 0; i <= 99; ++i) {
		snprintf(filename, sizeof(filename) / sizeof(char), "screen%02d.PCX", i);
		*dstPath = paths::PrefPath() + filename;
		if (!FileExists(dstPath->c_str())) {
			return new std::ofstream(*dstPath, std::ios::binary | std::ios::trunc);
		}
	}
	return nullptr;
}

/**
 * @brief Make a red version of the given palette and apply it to the screen.
 */
static void RedPalette()
{
	for (int i = 0; i < 255; i++) {
		system_palette[i].g = 0;
		system_palette[i].b = 0;
	}
	palette_update();
	BltFast(nullptr, nullptr);
	RenderPresent();
}

/**
 * @brief Save the current screen to a screen??.PCX (00-99) in file if available, then make the screen red for 200ms.

 */
void CaptureScreen()
{
	SDL_Color palette[256];
	std::string fileName;
	bool success;

	std::ofstream *outStream = CaptureFile(&fileName);
	if (outStream == nullptr)
		return;
	DrawAndBlit();
	PaletteGetEntries(256, palette);
	RedPalette();

	lock_buf(2);
	const CelOutputBuffer &buf = GlobalBackBuffer();
	success = CaptureHdr(buf.w(), buf.h(), outStream);
	if (success) {
		success = CapturePix(buf, outStream);
	}
	if (success) {
		success = CapturePal(palette, outStream);
	}
	unlock_buf(2);
	outStream->close();

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
	delete outStream;
}

} // namespace devilution
