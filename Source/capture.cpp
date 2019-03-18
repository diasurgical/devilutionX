//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

void __cdecl CaptureScreen()
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

		j_lock_buf_priv(2);
		success = CaptureHdr(hObject, 640, 480);
		if (success) {
			success = CapturePix(hObject, 640, 480, 768, &gpBuffer[SCREENXY(0, 0)]);
			if (success) {
				success = CapturePal(hObject, palette);
			}
		}
		j_unlock_buf_priv(2);
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

BOOL __fastcall CaptureHdr(HANDLE hFile, short width, short height)
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

BOOL __fastcall CapturePal(HANDLE hFile, PALETTEENTRY *palette)
{
	char *v3;
	char Buffer[769];
	int i;
	DWORD lpNumBytes;

	Buffer[0] = 12;
	v3 = &Buffer[1];
	for (i = 256; i != 0; --i) {
		v3[0] = palette->peRed;
		v3[1] = palette->peGreen;
		v3[2] = palette->peBlue;

		palette++;
		v3 += 3;
	}

	return WriteFile(hFile, Buffer, sizeof(Buffer), &lpNumBytes, NULL) && lpNumBytes == sizeof(Buffer);
}

BOOL __fastcall CapturePix(HANDLE hFile, WORD width, WORD height, WORD stride, BYTE *pixels)
{
	int writeSize;
	DWORD lpNumBytes;
	BYTE *pBuffer, *pBufferEnd;

	pBuffer = (BYTE *)DiabloAllocPtr(2 * width);
	do {
		if (!height) {
			mem_free_dbg(pBuffer);
			return TRUE;
		}
		height--;
		pBufferEnd = CaptureEnc(pixels, pBuffer, width);
		pixels += stride;
		writeSize = pBufferEnd - pBuffer;
	} while (WriteFile(hFile, pBuffer, writeSize, &lpNumBytes, 0) && lpNumBytes == writeSize);

	return FALSE;
}

BYTE *__fastcall CaptureEnc(BYTE *src, BYTE *dst, int width)
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

HANDLE __fastcall CaptureFile(char *dst_path)
{
	BOOLEAN num_used[100];
	int free_num, hFind;
	struct _finddata_t finder;

	memset(num_used, FALSE, sizeof(num_used));
	hFind = _findfirst("screen??.PCX", &finder);
	if (hFind != -1) {
		do {
			if (isdigit(finder.name[6]) && isdigit(finder.name[7])) {
				free_num = 10 * (finder.name[6] - '0');
				free_num += (finder.name[7] - '0');
				num_used[free_num] = TRUE;
			}
		} while (_findnext(hFind, &finder) == 0);
	}

	for (free_num = 0; free_num < 100; free_num++) {
		if (!num_used[free_num]) {
			sprintf(dst_path, "screen%02d.PCX", free_num);
			return CreateFile(dst_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
	}

	return INVALID_HANDLE_VALUE;
}

void __fastcall RedPalette(PALETTEENTRY *pal)
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
