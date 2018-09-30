//HEADER_GOES_HERE

#include "../types.h"

void __cdecl CaptureScreen()
{
	PALETTEENTRY palette[256];
	char FileName[MAX_PATH];

	HANDLE hObject = CaptureFile(FileName);
	if ( hObject != INVALID_HANDLE_VALUE)
	{
		DrawAndBlit();
		lpDDPalette->GetEntries(0, 0, 256, palette);
		RedPalette(palette);

		lock_buf_priv();
		bool success = CaptureHdr(hObject, 640, 480);
		if (success)
		{
			success = CapturePix(hObject, 640, 480, 768, (BYTE*)gpBuffer->row[0].pixels);
			if (success)
			{
				success = CapturePal(hObject, palette);
			}
		}
		unlock_buf_priv();
		CloseHandle(hObject);

		if (!success)
			DeleteFile(FileName);

		Sleep(300);
		lpDDPalette->SetEntries(0, 0, 256, palette);
	}
}

bool __fastcall CaptureHdr(HANDLE hFile, short width, short height)
{
	PCXHeader Buffer;
	memset(&Buffer, 0, sizeof(Buffer));

	Buffer.xmax = width - 1;
	Buffer.vertRes = height;
	Buffer.manufacturer = 10;
	Buffer.version = 5;
	Buffer.encoding = 1;
	Buffer.bitsPerPixel = 8;
	Buffer.ymax = height - 1;
	Buffer.horzRes = width;
	Buffer.numColorPlanes = 1;
	Buffer.bytesPerScanLine = width;

	DWORD lpNumBytes;
	return WriteFile(hFile, &Buffer, sizeof(Buffer), &lpNumBytes, NULL) && lpNumBytes == sizeof(Buffer);
}

bool __fastcall CapturePal(HANDLE hFile, PALETTEENTRY *palette)
{
	char *v3;
	char Buffer[769];

	Buffer[0] = 12;
	v3 = &Buffer[1];
	for (int i = 256; i != 0; --i)
	{
		v3[0] = palette->peRed;
		v3[1] = palette->peGreen;
		v3[2] = palette->peBlue;

		palette++;
		v3 += 3;
	}

	DWORD lpNumBytes;
	return WriteFile(hFile, Buffer, sizeof(Buffer), &lpNumBytes, NULL) && lpNumBytes == sizeof(Buffer);
}

bool __fastcall CapturePix(HANDLE hFile, WORD width, WORD height, WORD stride, BYTE *pixels)
{
	int writeSize;
	DWORD lpNumBytes;

	BYTE *pBuffer = (BYTE *)DiabloAllocPtr(2 * width);
	do
	{
		if ( !height )
		{
			mem_free_dbg(pBuffer);
			return 1;
		}
		height--;
		BYTE *pBufferEnd = CaptureEnc(pixels, pBuffer, width);
		pixels += stride;
		writeSize = pBufferEnd - pBuffer;
	}
	while (WriteFile(hFile, pBuffer, writeSize, &lpNumBytes, 0) && lpNumBytes == writeSize);
	return 0;
}

BYTE *__fastcall CaptureEnc(BYTE *src, BYTE *dst, int width)
{
	do
	{
		BYTE rlePixel = *src++;
		--width;

		int rleLength = 1;
		while (rlePixel == *src)
		{
			if (rleLength >= 63)
				break;
			if (!width)
				break;
			++rleLength;

			--width;
			++src;
		}

		if (rlePixel > 0xBF || rleLength > 1)
		{
			*dst++ = rleLength | 0xC0;
		}
		*dst++ = rlePixel;
	} while (width);
	return dst;
}

HANDLE __fastcall CaptureFile(char *dst_path)
{
	bool num_used[100] = { false };

	_finddata_t finder;
	int hFind = _findfirst("screen??.PCX", &finder);
	if (hFind != -1)
	{
		do
		{
			if (isdigit(finder.name[6]) && isdigit(finder.name[7]))
			{
				num_used[10 * (finder.name[6] - '0') + (finder.name[7] - '0')] = true;
			}
		}
		while (_findnext(hFind, &finder) == 0);
	}

	int free_num = 0;
	while (num_used[free_num])
	{
		++free_num;
		if (free_num >= 100)
			return INVALID_HANDLE_VALUE;
	}

	sprintf(dst_path, "screen%02d.PCX", free_num);
	return CreateFile(dst_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void __fastcall RedPalette(PALETTEENTRY *pal)
{
	PALETTEENTRY red[256];

	for(int i = 0; i < 256; i++)
	{
		red[i].peRed = pal[i].peRed;
		red[i].peGreen = 0;
		red[i].peBlue = 0;
		red[i].peFlags = 0;
	}

	lpDDPalette->SetEntries(0, 0, 256, red);
}
