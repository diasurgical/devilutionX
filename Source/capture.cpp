#include "diablo.h"

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
#ifdef __cplusplus
		lpDDPalette->GetEntries(0, 0, 256, palette);
#else
		lpDDPalette->lpVtbl->GetEntries(lpDDPalette, 0, 0, 256, palette);
#endif
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
#ifdef __cplusplus
		lpDDPalette->SetEntries(0, 0, 256, palette);
#else
		lpDDPalette->lpVtbl->SetEntries(lpDDPalette, 0, 0, 256, palette);
#endif
	}
}

BOOL CaptureHdr(HANDLE hFile, short width, short height)
{
	DWORD lpNumBytes;
	PCXHeader Buffer;

	memset(&Buffer, 0, sizeof(Buffer));
	Buffer.manufacturer = 10;
	Buffer.version = 5;
	Buffer.encoding = 1;
	Buffer.bitsPerPixel = 8;
	Buffer.xmax = width - 1;
	Buffer.ymax = height - 1;
	Buffer.horzRes = width;
	Buffer.vertRes = height;
	Buffer.numColorPlanes = 1;
	Buffer.bytesPerScanLine = width;

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
	int len = GetModuleFileNameA(ghInst, dst_path, MAX_PATH);

	for (int i = 0; i <= 99; i++) {
		sprintf(&dst_path[len], "screen%02d.PCX", i);
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

#ifdef __cplusplus
	lpDDPalette->SetEntries(0, 0, 256, red);
#else
	lpDDPalette->lpVtbl->SetEntries(lpDDPalette, 0, 0, 256, red);
#endif
}

DEVILUTION_END_NAMESPACE
