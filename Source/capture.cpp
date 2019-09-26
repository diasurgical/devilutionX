#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

void CaptureScreen()
{
	HANDLE hObject;
	PALETTEENTRY palette[256];
	char FileName[MAX_PATH];
	BOOL success;

	hObject = CaptureFile(FileName);
	if (hObject != INVALID_HANDLE_VALUE) {
		DrawAndBlit();
		PaletteGetEntries(256, palette);
		RedPalette(palette);

		lock_buf(2);
		success = CaptureHdr(hObject, SCREEN_WIDTH, SCREEN_HEIGHT);
		if (success) {
			success = CapturePix(hObject, SCREEN_WIDTH, SCREEN_HEIGHT, BUFFER_WIDTH, &gpBuffer[SCREENXY(0, 0)]);
			if (success) {
				success = CapturePal(hObject, palette);
			}
		}
		unlock_buf(2);
		CloseHandle(hObject);

		if (!success)
			DeleteFile(FileName);

		Sleep(300);
		PaletteGetEntries(256, palette);
	}
}

BOOL CaptureHdr(HANDLE hFile, short width, short height)
{
	DWORD lpNumBytes;
	PCXHEADER Buffer;

	memset(&Buffer, 0, sizeof(Buffer));
	Buffer.Manufacturer = 10;
	Buffer.Version = 5;
	Buffer.Encoding = 1;
	Buffer.BitsPerPixel = 8;
	Buffer.Xmax = width - 1;
	Buffer.Ymax = height - 1;
	Buffer.HDpi = width;
	Buffer.VDpi = height;
	Buffer.NPlanes = 1;
	Buffer.BytesPerLine = width;

	return WriteFile(hFile, &Buffer, sizeof(Buffer), &lpNumBytes, NULL) && lpNumBytes == sizeof(Buffer);
}

BOOL CapturePal(HANDLE hFile, PALETTEENTRY *palette)
{
	DWORD NumberOfBytesWritten;
	BYTE pcx_palette[769];
	int i;

	pcx_palette[0] = 12;
	for (i = 0; i < 256; i++) {
		pcx_palette[1 + 3 * i + 0] = palette[i].peRed;
		pcx_palette[1 + 3 * i + 1] = palette[i].peGreen;
		pcx_palette[1 + 3 * i + 2] = palette[i].peBlue;
	}

	return WriteFile(hFile, pcx_palette, 769, &NumberOfBytesWritten, 0) && NumberOfBytesWritten == 769;
}

BOOL CapturePix(HANDLE hFile, WORD width, WORD height, WORD stride, BYTE *pixels)
{
	int writeSize;
	DWORD lpNumBytes;
	BYTE *pBuffer, *pBufferEnd;

	pBuffer = (BYTE *)DiabloAllocPtr(2 * width);
	while (height != 0) {
		height--;
		pBufferEnd = CaptureEnc(pixels, pBuffer, width);
		pixels += stride;
		writeSize = pBufferEnd - pBuffer;
		if (!(WriteFile(hFile, pBuffer, writeSize, &lpNumBytes, 0) && lpNumBytes == writeSize)) {
			return FALSE;
		}
	}
	mem_free_dbg(pBuffer);
	return TRUE;
}

BYTE *CaptureEnc(BYTE *src, BYTE *dst, int width)
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

HANDLE CaptureFile(char *dst_path)
{
	char path[MAX_PATH];

	GetPrefPath(dst_path, MAX_PATH);

	for (int i = 0; i <= 99; i++) {
		snprintf(dst_path, MAX_PATH, "%sscreen%02d.PCX", path, i);
		FILE *file = fopen(dst_path, "r");

		if (file == NULL) {
			return CreateFile(dst_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}

		fclose(file);
	}

	return INVALID_HANDLE_VALUE;
}

void RedPalette(PALETTEENTRY *pal)
{
	PALETTEENTRY red[256];
	int i;

	for (i = 0; i < 256; i++) {
		red[i].peRed = pal[i].peRed;
		red[i].peGreen = 0;
		red[i].peBlue = 0;
		red[i].peFlags = 0;
	}

	PaletteGetEntries(256, red);
}

DEVILUTION_END_NAMESPACE
