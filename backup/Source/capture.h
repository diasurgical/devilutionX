//HEADER_GOES_HERE
#ifndef __CAPTURE_H__
#define __CAPTURE_H__

void CaptureScreen();
BOOL CaptureHdr(HANDLE hFile, short width, short height);
BOOL CapturePal(HANDLE hFile, PALETTEENTRY *palette);
BOOL CapturePix(HANDLE hFile, WORD width, WORD height, WORD stride, BYTE *pixels);
BYTE *CaptureEnc(BYTE *src, BYTE *dst, int width);
HANDLE CaptureFile(char *dst_path);
void RedPalette(PALETTEENTRY *pal);

#endif /* __CAPTURE_H__ */
