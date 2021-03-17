/**
 * @file capture.cpp
 *
 * Implementation of the screenshot function.
 */
#include <fstream>

#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "paths.h"
#include "file_util.h"

DEVILUTION_BEGIN_NAMESPACE

/**
 * @brief Write the PCX-file header
 * @param width Image width
 * @param height Image height
 * @param out File stream to write to
 * @return True on success
 */
static BOOL CaptureHdr(short width, short height, std::ofstream *out)
{
	PCXHEADER Buffer;

	memset(&Buffer, 0, sizeof(Buffer));
	Buffer.Manufacturer = 10;
	Buffer.Version = 5;
	Buffer.Encoding = 1;
	Buffer.BitsPerPixel = 8;
	Buffer.Xmax = SDL_SwapLE16(width - 1);
	Buffer.Ymax = SDL_SwapLE16(height - 1);
	Buffer.HDpi = SDL_SwapLE16(width);
	Buffer.VDpi = SDL_SwapLE16(height);
	Buffer.NPlanes = 1;
	Buffer.BytesPerLine = SDL_SwapLE16(width);

	out->write(reinterpret_cast<const char *>(&Buffer), sizeof(Buffer));
	return !out->fail();
}

/**
 * @brief Write the current ingame palette to the PCX file
 * @param palette Current palette
 * @param out File stream for the PCX file.
 * @return True if successful, else false
 */
static BOOL CapturePal(SDL_Color *palette, std::ofstream *out)
{
	BYTE pcx_palette[1 + 256 * 3];
	int i;

	pcx_palette[0] = 12;
	for (i = 0; i < 256; i++) {
		pcx_palette[1 + 3 * i + 0] = palette[i].r;
		pcx_palette[1 + 3 * i + 1] = palette[i].g;
		pcx_palette[1 + 3 * i + 2] = palette[i].b;
	}

	out->write(reinterpret_cast<const char *>(pcx_palette), sizeof(pcx_palette));
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
			if (!width)
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
	} while (width);

	return dst;
}

/**
 * @brief Write the pixel data to the PCX file
 * @param buf Buffer
 * @return True if successful, else false
 */
static bool CapturePix(CelOutputBuffer buf, std::ofstream *out)
{
	int width = buf.w();
	int height = buf.h();
	BYTE *pBuffer = (BYTE *)DiabloAllocPtr(2 * width);
	BYTE *pixels = buf.begin();
	while (height--) {
		const BYTE *pBufferEnd = CaptureEnc(pixels, pBuffer, width);
		pixels += buf.pitch();
		out->write(reinterpret_cast<const char *>(pBuffer), pBufferEnd - pBuffer);
		if (out->fail())
			return false;
	}
	mem_free_dbg(pBuffer);
	return true;
}

/**
 * Returns a pointer because in GCC < 5 ofstream itself is not moveable due to a bug.
 */
static std::ofstream *CaptureFile(std::string *dst_path)
{
	char filename[sizeof("screen00.PCX") / sizeof(char)];
	for (int i = 0; i <= 99; ++i) {
		snprintf(filename, sizeof(filename) / sizeof(char), "screen%02d.PCX", i);
		*dst_path = GetPrefPath() + filename;
		if (!FileExists(dst_path->c_str())) {
			return new std::ofstream(*dst_path, std::ios::binary | std::ios::trunc);
		}
	}
	return NULL;
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
	SDL_Rect SrcRect = {
		BUFFER_BORDER_LEFT,
		BUFFER_BORDER_TOP,
		gnScreenWidth,
		gnScreenHeight,
	};
	BltFast(&SrcRect, NULL);
	RenderPresent();
}

/**
 * @brief Save the current screen to a screen??.PCX (00-99) in file if available, then make the screen red for 200ms.

 */
void CaptureScreen()
{
	SDL_Color palette[256];
	std::string FileName;
	BOOL success;

	std::ofstream *out_stream = CaptureFile(&FileName);
	if (out_stream == NULL)
		return;
	DrawAndBlit();
	PaletteGetEntries(256, palette);
	RedPalette();

	lock_buf(2);
	CelOutputBuffer buf = GlobalBackBuffer();
	success = CaptureHdr(buf.w(), buf.h(), out_stream);
	if (success) {
		success = CapturePix(buf, out_stream);
	}
	if (success) {
		success = CapturePal(palette, out_stream);
	}
	unlock_buf(2);
	out_stream->close();

	if (!success) {
		SDL_Log("Failed to save screenshot at %s", FileName.c_str());
		RemoveFile(FileName.c_str());
	} else {
		SDL_Log("Screenshot saved at %s", FileName.c_str());
	}
	SDL_Delay(300);
	for (int i = 0; i < 256; i++) {
		system_palette[i] = palette[i];
	}
	palette_update();
	force_redraw = 255;
	delete out_stream;
}

DEVILUTION_END_NAMESPACE
