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
#ifdef __cplusplus
static CCritSect sgMemCrit;
#endif
char gbBackBuf;    // weak
char gbEmulate;    // weak
HMODULE ghDiabMod; // idb

void dx_init(HWND hWnd)
{
	HRESULT hDDVal;
	int winw, winh;
	BOOL bSuccess;
	GUID *lpGUID;

	/// ASSERT: assert(! gpBuffer);
	/// ASSERT: assert(! sgdwLockCount);
	/// ASSERT: assert(! sgpBackBuf);

	SetFocus(hWnd);
	ShowWindow(hWnd, 1);

	lpGUID = NULL;
	if(gbEmulate) {
		lpGUID = NULL;
	}
	hDDVal = dx_DirectDrawCreate(lpGUID, &lpDDInterface, NULL);
	if (hDDVal != DVL_S_OK) {
		ERR_DLG(IDD_DIALOG1, hDDVal);
	}

#ifdef COLORFIX
#ifdef __DDRAWI_INCLUDED__
	((LPDDRAWI_DIRECTDRAW_INT)lpDDInterface)->lpLcl->dwAppHackFlags |= 0x800;
#else
	((DWORD **)lpDDInterface)[1][18] |= 0x800;
#endif
#endif

#ifndef _DEBUG
	fullscreen = true;
#endif
	if (!fullscreen) {
#ifdef __cplusplus
		hDDVal = lpDDInterface->SetCooperativeLevel(hWnd, 0 | 0);
#else
		hDDVal = lpDDInterface->lpVtbl->SetCooperativeLevel(lpDDInterface, hWnd, DDSCL_NORMAL | DDSCL_ALLOWREBOOT);
#endif
		if (hDDVal == 1) {
			TriggerBreak();
		} else if (hDDVal != DVL_S_OK) {
			ERR_DLG(IDD_DIALOG1, hDDVal);
		}
		SetWindowPos(hWnd, 0, 0, 0, 0, 0, 0 | 0 | 0);
	} else {
#ifdef __cplusplus
		hDDVal = lpDDInterface->SetCooperativeLevel(hWnd, 0 | 0 | 0);
#else
		hDDVal = lpDDInterface->lpVtbl->SetCooperativeLevel(lpDDInterface, hWnd, DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN);
#endif
		if (hDDVal == 1) {
			TriggerBreak();
		} else if (hDDVal != DVL_S_OK) {
			ERR_DLG(IDD_DIALOG1, hDDVal);
		}
#ifdef __cplusplus
		hDDVal = lpDDInterface->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);
#else
		hDDVal = lpDDInterface->lpVtbl->SetDisplayMode(lpDDInterface, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);
#endif
		if (hDDVal != DVL_S_OK) {
			winw = GetSystemMetrics(DVL_SM_CXSCREEN);
			winh = GetSystemMetrics(DVL_SM_CYSCREEN);
#ifdef __cplusplus
			hDDVal = lpDDInterface->SetDisplayMode(winw, winh, SCREEN_BPP);
#else
			hDDVal = lpDDInterface->lpVtbl->SetDisplayMode(lpDDInterface, winw, winh, SCREEN_BPP);
#endif
		}
		if (hDDVal != DVL_S_OK) {
			ERR_DLG(IDD_DIALOG1, hDDVal);
		}
	}

	dx_create_primary_surface();
	palette_init();
	GdiSetBatchLimit(1);
	dx_create_back_buffer();
	bSuccess = SDrawManualInitialize(hWnd, lpDDInterface, lpDDSPrimary, NULL, NULL, lpDDSBackBuf, lpDDPalette, NULL);
	/// ASSERT: assert(bSuccess);
}
// 52A549: using guessed type char gbEmulate;

void dx_create_back_buffer()
{
	DDSCAPS caps;
	HRESULT error_code;
	DDSURFACEDESC ddsd;

#ifdef __cplusplus
	error_code = lpDDSPrimary->GetCaps(&caps);
#else
	error_code = lpDDSPrimary->lpVtbl->GetCaps(lpDDSPrimary, &caps);
#endif
	if (error_code != DVL_S_OK)
		DD_ERR_MSG(error_code);

	gbBackBuf = 1;
	if (!gbBackBuf) {
		ddsd.dwSize = sizeof(ddsd);
#ifdef __cplusplus
		error_code = lpDDSPrimary->Lock(NULL, &ddsd, 0 | 0, NULL);
#else
		error_code = lpDDSPrimary->lpVtbl->Lock(lpDDSPrimary, NULL, &ddsd, DDLOCK_WRITEONLY | DDLOCK_WAIT, NULL);
#endif
		if (error_code == DVL_S_OK) {
#ifdef __cplusplus
			lpDDSPrimary->Unlock(NULL);
#else
			lpDDSPrimary->lpVtbl->Unlock(lpDDSPrimary, NULL);
#endif
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
#ifdef __cplusplus
	error_code = lpDDSPrimary->GetPixelFormat(&ddsd.ddpfPixelFormat);
#else
	error_code = lpDDSPrimary->lpVtbl->GetPixelFormat(lpDDSPrimary, &ddsd.ddpfPixelFormat);
#endif
	if (error_code != DVL_S_OK)
		ERR_DLG(IDD_DIALOG1, error_code);
#ifdef __cplusplus
	error_code = lpDDInterface->CreateSurface(&ddsd, &lpDDSBackBuf, NULL);
#else
	error_code = lpDDInterface->lpVtbl->CreateSurface(lpDDInterface, &ddsd, &lpDDSBackBuf, NULL);
#endif
	if (error_code != DVL_S_OK)
		ERR_DLG(IDD_DIALOG1, error_code);
}
// 52A548: using guessed type char gbBackBuf;

void dx_create_primary_surface()
{
	DDSURFACEDESC ddsd;
	HRESULT error_code;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = 0;
	ddsd.ddsCaps.dwCaps = 0;
#ifdef __cplusplus
	error_code = lpDDInterface->CreateSurface(&ddsd, &lpDDSPrimary, NULL);
#else
	error_code = lpDDInterface->lpVtbl->CreateSurface(lpDDInterface, &ddsd, &lpDDSPrimary, NULL);
#endif
	if (error_code != DVL_S_OK)
		ERR_DLG(IDD_DIALOG1, error_code);
}

HRESULT dx_DirectDrawCreate(LPGUID guid, LPDIRECTDRAW *lplpDD, LPUNKNOWN pUnkOuter)
{
	if (ghDiabMod == NULL) {
		ghDiabMod = NULL; //ghDiabMod = LoadLibrary("ddraw.dll");
		if (ghDiabMod == NULL) {
			// ERR_DLG(IDD_DIALOG4, GetLastError());
		}
	}

	*lplpDD = new StubDraw();

	return DVL_S_OK;
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

#ifdef __cplusplus
	sgMemCrit.Enter();
#endif
	if (sgpBackBuf != NULL) {
		gpBuffer = sgpBackBuf;
		sgdwLockCount++;
		return;
	}

	if (lpDDSBackBuf == NULL) {
		Sleep(20000);
		app_fatal("lock_buf_priv");
		sgdwLockCount++;
		return;
	}

	if (sgdwLockCount != 0) {
		sgdwLockCount++;
		return;
	}
	ddsd.dwSize = sizeof(ddsd);
#ifdef __cplusplus
	error_code = lpDDSBackBuf->Lock(NULL, &ddsd, 0, NULL);
#else
	error_code = lpDDSBackBuf->lpVtbl->Lock(lpDDSBackBuf, NULL, &ddsd, DDLOCK_WAIT, NULL);
#endif
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
#ifdef __cplusplus
			error_code = lpDDSBackBuf->Unlock(NULL);
#else
			error_code = lpDDSBackBuf->lpVtbl->Unlock(lpDDSBackBuf, NULL);
#endif
			if (error_code != DVL_S_OK)
				DD_ERR_MSG(error_code);
		}
	}
#ifdef __cplusplus
	sgMemCrit.Leave();
#endif
}

void dx_cleanup()
{
	BYTE *v0; // ecx

	if (ghMainWnd)
		ShowWindow(ghMainWnd, 0);
	SDrawDestroy();
#ifdef __cplusplus
	sgMemCrit.Enter();
#endif
	if (sgpBackBuf != NULL) {
		v0 = sgpBackBuf;
		sgpBackBuf = 0;
		mem_free_dbg(v0);
	} else if (lpDDSBackBuf != NULL) {
#ifdef __cplusplus
		lpDDSBackBuf->Release();
#else
		lpDDSBackBuf->lpVtbl->Release(lpDDSBackBuf);
#endif
		delete lpDDSBackBuf;
		lpDDSBackBuf = NULL;
	}
	sgdwLockCount = 0;
	gpBuffer = 0;
#ifdef __cplusplus
	sgMemCrit.Leave();
#endif
	if (lpDDSPrimary) {
#ifdef __cplusplus
		lpDDSPrimary->Release();
#else
		lpDDSPrimary->lpVtbl->Release(lpDDSPrimary);
#endif
		delete lpDDSPrimary;
		lpDDSPrimary = NULL;
	}
	if (lpDDPalette) {
#ifdef __cplusplus
		lpDDPalette->Release();
#else
		lpDDPalette->lpVtbl->Release(lpDDPalette);
#endif
		lpDDPalette = NULL;
	}
	if (lpDDInterface) {
#ifdef __cplusplus
		lpDDInterface->Release();
#else
		lpDDInterface->lpVtbl->Release(lpDDInterface);
#endif
		lpDDInterface = NULL;
	}
}

void dx_reinit()
{
	int lockCount;

#ifdef __cplusplus
	sgMemCrit.Enter();
#endif
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

#ifdef __cplusplus
	sgMemCrit.Leave();
#endif
}

} // namespace dvl
