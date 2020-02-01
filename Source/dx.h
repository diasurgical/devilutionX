//HEADER_GOES_HERE
#ifndef __DX_H__
#define __DX_H__

extern BYTE *gpBuffer;
extern char gbBackBuf;
extern char gbEmulate;
extern HMODULE ghDiabMod;

void dx_init(HWND hWnd);
void lock_buf(BYTE idx);
void unlock_buf(BYTE idx);
void dx_cleanup();
void dx_reinit();

void CreatePalette();
void BltFast(SDL_Rect *src_rect, SDL_Rect *dst_rect);
void RenderPresent();
void PaletteGetEntries(DWORD dwNumEntries, SDL_Color *lpEntries);
void PaletteSetEntries(DWORD dwCount, SDL_Color *lpEntries);

/* data */

#endif /* __DX_H__ */
