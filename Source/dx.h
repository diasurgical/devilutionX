//HEADER_GOES_HERE
#ifndef __DX_H__
#define __DX_H__

extern IDirectDraw *lpDDInterface;
extern IDirectDrawPalette *lpDDPalette; // idb
extern BYTE *gpBuffer;
extern IDirectDrawSurface *lpDDSBackBuf;
extern IDirectDrawSurface *lpDDSPrimary;
extern char gbBackBuf;    // weak
extern char gbEmulate;    // weak
extern HMODULE ghDiabMod; // idb

void __cdecl dx_init_mutex();
void __cdecl dx_cleanup_mutex_atexit();
void __cdecl dx_cleanup_mutex(void);
void __fastcall dx_init(HWND hWnd);
void __cdecl dx_create_back_buffer();
void __cdecl dx_create_primary_surface();
HRESULT __fastcall dx_DirectDrawCreate(LPGUID guid, LPDIRECTDRAW *lplpDD, LPUNKNOWN pUnkOuter);
void __fastcall j_lock_buf_priv(BYTE idx);
void __cdecl lock_buf_priv();
void __fastcall j_unlock_buf_priv(BYTE idx);
void __cdecl unlock_buf_priv();
void __cdecl dx_cleanup();
void __cdecl dx_reinit();
void __cdecl j_dx_reinit();

/* data */

extern int dx_inf; // weak

#endif /* __DX_H__ */
