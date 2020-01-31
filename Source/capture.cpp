#include <fstream>

#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "file_util.h"

DEVILUTION_BEGIN_NAMESPACE

static BOOL CaptureHdr(short width, short height, std::ofstream *out)
{
	DWORD lpNumBytes;
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

	out->write(reinterpret_cast<const char*>(&Buffer), sizeof(Buffer));
	return !out->fail();
}

static BOOL CapturePal(SDL_Color *palette, std::ofstream *out)
{
	BYTE pcx_palette[769];
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

static BYTE *CaptureEnc(BYTE *src, BYTE *dst, int width)
{
	int rleLength;

	do {
		BYTE rlePixel = *src;
		*src++;
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
			*dst++;
		}

		*dst = rlePixel;
		*dst++;
	} while (width);

	return dst;
}

static bool CapturePix(WORD width, WORD height, WORD stride, BYTE *pixels, std::ofstream *out)
{
	int writeSize;
	DWORD lpNumBytes;
	BYTE *pBuffer, *pBufferEnd;

	pBuffer = (BYTE *)DiabloAllocPtr(2 * width);
	while (height--) {
		pBufferEnd = CaptureEnc(pixels, pBuffer, width);
		pixels += stride;
		writeSize = pBufferEnd - pBuffer;
		out->write(reinterpret_cast<const char *>(pBuffer), writeSize);
		if (out->fail()) return false;
	}
	mem_free_dbg(pBuffer);
	return true;
}

// Returns a pointer because in GCC < 5 ofstream itself is not moveable due to a bug.
static std::ofstream *CaptureFile(char *dst_path)
{
	char path[MAX_PATH];

	GetPrefPath(path, MAX_PATH);

	for (int i = 0; i <= 99; i++) {
		snprintf(dst_path, MAX_PATH, "%sscreen%02d.PCX", path, i);
		if (!FileExists(dst_path))
			return new std::ofstream(dst_path, std::ios::binary | std::ios::trunc);
	}

	return nullptr;
}

static void RedPalette(SDL_Color *pal)
{
	SDL_Color red[256];
	int i;

	for (i = 0; i < 256; i++) {
		red[i].r = pal[i].r;
		red[i].g = 0;
		red[i].b = 0;
	}

	PaletteGetEntries(256, red);
}

void CaptureScreen()
{
	SDL_Color palette[256];
	char FileName[MAX_PATH];
	BOOL success;

	std::ofstream *out = CaptureFile(FileName);
	if (out == nullptr) return;
	DrawAndBlit();
	PaletteGetEntries(256, palette);
	RedPalette(palette);

	lock_buf(2);
	success = CaptureHdr(SCREEN_WIDTH, SCREEN_HEIGHT, out);
	if (success) {
		success = CapturePix(SCREEN_WIDTH, SCREEN_HEIGHT, BUFFER_WIDTH, &gpBuffer[SCREENXY(0, 0)], out);
	}
	if (success) {
		success = CapturePal(palette, out);
	}
	unlock_buf(2);
	out->close();

	if (!success) {
		SDL_Log("Failed to save screenshot at %s", FileName);
		RemoveFile(FileName);
	} else {
		SDL_Log("Screenshot saved at %s", FileName);
	}

	SDL_Delay(300);
	PaletteGetEntries(256, palette);
	delete out;
}

DEVILUTION_END_NAMESPACE
