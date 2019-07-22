//HEADER_GOES_HERE
#ifndef __DX_H__
#define __DX_H__

extern IDirectDraw *lpDDInterface;
extern IDirectDrawPalette *lpDDPalette;
extern BYTE *gpBuffer;
extern IDirectDrawSurface *lpDDSBackBuf;
extern IDirectDrawSurface *lpDDSPrimary;
extern char gbBackBuf;
extern char gbEmulate;
extern HMODULE ghDiabMod;

void dx_init(HWND hWnd);
void dx_create_back_buffer();
void dx_create_primary_surface();
HRESULT dx_DirectDrawCreate(LPGUID guid, LPDIRECTDRAW *lplpDD, LPUNKNOWN pUnkOuter);
void lock_buf(BYTE idx);
void lock_buf_priv();
void unlock_buf(BYTE idx);
void unlock_buf_priv();
void dx_cleanup();
void dx_reinit();
void j_dx_reinit();

/* data */

#endif /* __DX_H__ */
