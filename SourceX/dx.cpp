#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "miniwin/ddraw.h"

namespace dvl {

BYTE *sgpBackBuf;
LPDIRECTDRAW lpDDInterface;
IDirectDrawPalette *lpDDPalette; // idb
int sgdwLockCount;
BYTE *gpBuffer;
IDirectDrawSurface *lpDDSBackBuf;
IDirectDrawSurface *lpDDSPrimary;
#ifdef _DEBUG
int locktbl[256];
#endif
static CCritSect sgMemCrit;
char gbBackBuf;
char gbEmulate;
HMODULE ghDiabMod;

void dx_init(HWND hWnd)
{
	SetFocus(hWnd);
	SDL_ShowWindow(window);

	lpDDInterface = new StubDraw();

	dx_create_primary_surface();
	palette_init();
	dx_create_back_buffer();
}

void dx_create_back_buffer()
{
	DDSCAPS caps;
	HRESULT error_code;
	DDSURFACEDESC ddsd;

	error_code = lpDDSPrimary->GetCaps(&caps);
	if (error_code != DVL_S_OK)
		DD_ERR_MSG(error_code);

	gbBackBuf = 1;
	if (!gbBackBuf) {
		ddsd.dwSize = sizeof(ddsd);
		error_code = lpDDSPrimary->Lock(NULL, &ddsd, 0 | 0, NULL);
		if (error_code == DVL_S_OK) {
			lpDDSPrimary->Unlock(NULL);
			sgpBackBuf = (BYTE *)DiabloAllocPtr(BUFFER_HEIGHT * BUFFER_WIDTH);
			return;
		}
		if (error_code != 2)
			ERR_DLG(IDD_DIALOG1, error_code);
	}

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwWidth = BUFFER_WIDTH;
	ddsd.lPitch = BUFFER_WIDTH;
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = 0 | 0 | 0 | 0 | 0;
	ddsd.ddsCaps.dwCaps = 0 | 0;
	ddsd.dwHeight = BUFFER_HEIGHT;
	ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
	error_code = lpDDSPrimary->GetPixelFormat(&ddsd.ddpfPixelFormat);
	if (error_code != DVL_S_OK)
		ERR_DLG(IDD_DIALOG1, error_code);
	error_code = lpDDInterface->CreateSurface(&ddsd, &lpDDSBackBuf, NULL);
	if (error_code != DVL_S_OK)
		ERR_DLG(IDD_DIALOG1, error_code);
}

void dx_create_primary_surface()
{
	DDSURFACEDESC ddsd;
	HRESULT error_code;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = 0;
	ddsd.ddsCaps.dwCaps = 0;
	error_code = lpDDInterface->CreateSurface(&ddsd, &lpDDSPrimary, NULL);
	if (error_code != DVL_S_OK)
		ERR_DLG(IDD_DIALOG1, error_code);
}

void lock_buf(BYTE idx)
{
#ifdef _DEBUG
	++locktbl[idx];
#endif
	lock_buf_priv();
}

void lock_buf_priv()
{
	DDSURFACEDESC ddsd;
	HRESULT error_code;

	sgMemCrit.Enter();
	if (sgpBackBuf != NULL) {
		gpBuffer = sgpBackBuf;
		sgdwLockCount++;
		return;
	}

	if (sgdwLockCount != 0) {
		sgdwLockCount++;
		return;
	}
	ddsd.dwSize = sizeof(ddsd);
	error_code = lpDDSBackBuf->Lock(NULL, &ddsd, 0, NULL);
	if (error_code != DVL_S_OK)
		DD_ERR_MSG(error_code);

	gpBufEnd += (uintptr_t)ddsd.lpSurface;
	gpBuffer = (BYTE *)ddsd.lpSurface;
	sgdwLockCount++;
}

void unlock_buf(BYTE idx)
{
#ifdef _DEBUG
	if (!locktbl[idx])
		app_fatal("Draw lock underflow: 0x%x", idx);
	--locktbl[idx];
#endif
	unlock_buf_priv();
}

void unlock_buf_priv()
{
	HRESULT error_code;

	if (sgdwLockCount == 0)
		app_fatal("draw main unlock error");
	if (!gpBuffer)
		app_fatal("draw consistency error");

	sgdwLockCount--;
	if (sgdwLockCount == 0) {
		gpBufEnd -= (uintptr_t)gpBuffer;
		//gpBuffer = NULL; unable to return to menu
		if (sgpBackBuf == NULL) {
			error_code = lpDDSBackBuf->Unlock(NULL);
			if (error_code != DVL_S_OK)
				DD_ERR_MSG(error_code);
		}
	}
	sgMemCrit.Leave();
}

void dx_cleanup()
{
	BYTE *v0; // ecx

	if (ghMainWnd)
		ShowWindow(ghMainWnd, 0);
	sgMemCrit.Enter();
	if (sgpBackBuf != NULL) {
		v0 = sgpBackBuf;
		sgpBackBuf = 0;
		mem_free_dbg(v0);
	} else if (lpDDSBackBuf != NULL) {
		lpDDSBackBuf->Release();
		delete lpDDSBackBuf;
		lpDDSBackBuf = NULL;
	}
	sgdwLockCount = 0;
	gpBuffer = 0;
	sgMemCrit.Leave();
	if (lpDDSPrimary) {
		lpDDSPrimary->Release();
		delete lpDDSPrimary;
		lpDDSPrimary = NULL;
	}
	if (lpDDPalette) {
		lpDDPalette->Release();
		lpDDPalette = NULL;
	}
	if (lpDDInterface) {
		lpDDInterface->Release();
		lpDDInterface = NULL;
	}
}

void dx_reinit()
{
	int lockCount;

	sgMemCrit.Enter();
	ClearCursor();
	lockCount = sgdwLockCount;

	while (sgdwLockCount != 0)
		unlock_buf_priv();

	dx_cleanup();

	drawpanflag = 255;

	dx_init(ghMainWnd);

	while (lockCount != 0) {
		lock_buf_priv();
		lockCount--;
	}

	sgMemCrit.Leave();
}

} // namespace dvl
