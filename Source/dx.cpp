#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE *sgpBackBuf;
LPDIRECTDRAW lpDDInterface;
IDirectDrawPalette *lpDDPalette;
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
char gbBackBuf;
char gbEmulate;
HMODULE ghDiabMod;

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
	ShowWindow(hWnd, SW_SHOWNORMAL);

	lpGUID = NULL;
	if (gbEmulate) {
		lpGUID = (GUID *)DDCREATE_EMULATIONONLY;
	}
	hDDVal = dx_DirectDrawCreate(lpGUID, &lpDDInterface, NULL);
	if (hDDVal != DD_OK) {
		ErrDlg(IDD_DIALOG1, hDDVal, "C:\\Src\\Diablo\\Source\\dx.cpp", 149);
	}

#ifdef COLORFIX
#ifdef __DDRAWI_INCLUDED__
	((LPDDRAWI_DIRECTDRAW_INT)lpDDInterface)->lpLcl->dwAppHackFlags |= 0x800;
#else
	((DWORD **)lpDDInterface)[1][18] |= 0x800;
#endif
#endif

#ifndef _DEBUG
	fullscreen = TRUE;
#endif
	if (!fullscreen) {
#ifdef __cplusplus
		hDDVal = lpDDInterface->SetCooperativeLevel(hWnd, DDSCL_NORMAL | DDSCL_ALLOWREBOOT);
#else
		hDDVal = lpDDInterface->lpVtbl->SetCooperativeLevel(lpDDInterface, hWnd, DDSCL_NORMAL | DDSCL_ALLOWREBOOT);
#endif
		if (hDDVal == DDERR_EXCLUSIVEMODEALREADYSET) {
			TriggerBreak();
		} else if (hDDVal != DD_OK) {
			ErrDlg(IDD_DIALOG1, hDDVal, "C:\\Diablo\\Direct\\dx.cpp", 155);
		}
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	} else {
#ifdef __cplusplus
		hDDVal = lpDDInterface->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN);
#else
		hDDVal = lpDDInterface->lpVtbl->SetCooperativeLevel(lpDDInterface, hWnd, DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN);
#endif
		if (hDDVal == DDERR_EXCLUSIVEMODEALREADYSET) {
			TriggerBreak();
		} else if (hDDVal != DD_OK) {
			ErrDlg(IDD_DIALOG1, hDDVal, "C:\\Src\\Diablo\\Source\\dx.cpp", 170);
		}
#ifdef __cplusplus
		hDDVal = lpDDInterface->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);
#else
		hDDVal = lpDDInterface->lpVtbl->SetDisplayMode(lpDDInterface, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);
#endif
		if (hDDVal != DD_OK) {
			winw = GetSystemMetrics(SM_CXSCREEN);
			winh = GetSystemMetrics(SM_CYSCREEN);
#ifdef __cplusplus
			hDDVal = lpDDInterface->SetDisplayMode(winw, winh, SCREEN_BPP);
#else
			hDDVal = lpDDInterface->lpVtbl->SetDisplayMode(lpDDInterface, winw, winh, SCREEN_BPP);
#endif
		}
		if (hDDVal != DD_OK) {
			ErrDlg(IDD_DIALOG1, hDDVal, "C:\\Src\\Diablo\\Source\\dx.cpp", 183);
		}
	}

	dx_create_primary_surface();
	palette_init();
	GdiSetBatchLimit(1);
	dx_create_back_buffer();
	bSuccess = SDrawManualInitialize(hWnd, lpDDInterface, lpDDSPrimary, NULL, NULL, lpDDSBackBuf, lpDDPalette, NULL);
	/// ASSERT: assert(bSuccess);
}

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
	if (error_code != DD_OK)
		DDErrMsg(error_code, 59, "C:\\Src\\Diablo\\Source\\dx.cpp");

	if (!gbBackBuf) {
		ddsd.dwSize = sizeof(ddsd);
#ifdef __cplusplus
		error_code = lpDDSPrimary->Lock(NULL, &ddsd, DDLOCK_WRITEONLY | DDLOCK_WAIT, NULL);
#else
		error_code = lpDDSPrimary->lpVtbl->Lock(lpDDSPrimary, NULL, &ddsd, DDLOCK_WRITEONLY | DDLOCK_WAIT, NULL);
#endif
		if (error_code == DD_OK) {
#ifdef __cplusplus
			lpDDSPrimary->Unlock(NULL);
#else
			lpDDSPrimary->lpVtbl->Unlock(lpDDSPrimary, NULL);
#endif
			sgpBackBuf = (BYTE *)DiabloAllocPtr(BUFFER_HEIGHT * BUFFER_WIDTH);
			return;
		}
		if (error_code != DDERR_CANTLOCKSURFACE)
			ErrDlg(IDD_DIALOG1, error_code, "C:\\Src\\Diablo\\Source\\dx.cpp", 81);
	}

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwWidth = BUFFER_WIDTH;
	ddsd.lPitch = BUFFER_WIDTH;
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_PIXELFORMAT | DDSD_PITCH | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwHeight = BUFFER_HEIGHT;
	ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
#ifdef __cplusplus
	error_code = lpDDSPrimary->GetPixelFormat(&ddsd.ddpfPixelFormat);
#else
	error_code = lpDDSPrimary->lpVtbl->GetPixelFormat(lpDDSPrimary, &ddsd.ddpfPixelFormat);
#endif
	if (error_code != DD_OK)
		ErrDlg(IDD_DIALOG1, error_code, "C:\\Src\\Diablo\\Source\\dx.cpp", 94);
#ifdef __cplusplus
	error_code = lpDDInterface->CreateSurface(&ddsd, &lpDDSBackBuf, NULL);
#else
	error_code = lpDDInterface->lpVtbl->CreateSurface(lpDDInterface, &ddsd, &lpDDSBackBuf, NULL);
#endif
	if (error_code != DD_OK)
		ErrDlg(IDD_DIALOG1, error_code, "C:\\Src\\Diablo\\Source\\dx.cpp", 96);
}

void dx_create_primary_surface()
{
	DDSURFACEDESC ddsd;
	HRESULT error_code;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
#ifdef __cplusplus
	error_code = lpDDInterface->CreateSurface(&ddsd, &lpDDSPrimary, NULL);
#else
	error_code = lpDDInterface->lpVtbl->CreateSurface(lpDDInterface, &ddsd, &lpDDSPrimary, NULL);
#endif
	if (error_code != DD_OK)
		ErrDlg(IDD_DIALOG1, error_code, "C:\\Src\\Diablo\\Source\\dx.cpp", 109);
}

HRESULT dx_DirectDrawCreate(LPGUID guid, LPDIRECTDRAW *lplpDD, LPUNKNOWN pUnkOuter)
{
	HRESULT(WINAPI * DirectDrawCreate)
	(LPGUID lpGuid, LPDIRECTDRAW * lplpDD, LPUNKNOWN pUnkOuter);

	if (ghDiabMod == NULL) {
		ghDiabMod = LoadLibrary("ddraw.dll");
		if (ghDiabMod == NULL) {
			ErrDlg(IDD_DIALOG4, GetLastError(), "C:\\Src\\Diablo\\Source\\dx.cpp", 122);
		}
	}

	DirectDrawCreate = (HRESULT(WINAPI *)(LPGUID, LPDIRECTDRAW *, LPUNKNOWN))GetProcAddress(ghDiabMod, "DirectDrawCreate");
	if (DirectDrawCreate == NULL) {
		ErrDlg(IDD_DIALOG4, GetLastError(), "C:\\Src\\Diablo\\Source\\dx.cpp", 127);
	}
	return DirectDrawCreate(guid, lplpDD, pUnkOuter);
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
	error_code = lpDDSBackBuf->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
#else
	error_code = lpDDSBackBuf->lpVtbl->Lock(lpDDSBackBuf, NULL, &ddsd, DDLOCK_WAIT, NULL);
#endif
	if (error_code != DD_OK)
		DDErrMsg(error_code, 235, "C:\\Src\\Diablo\\Source\\dx.cpp");

	gpBufEnd += (size_t)ddsd.lpSurface;
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
	if (gpBuffer == NULL)
		app_fatal("draw consistency error");

	sgdwLockCount--;
	if (sgdwLockCount == 0) {
		gpBufEnd -= (size_t)gpBuffer;
		gpBuffer = NULL;
		if (sgpBackBuf == NULL) {
#ifdef __cplusplus
			error_code = lpDDSBackBuf->Unlock(NULL);
#else
			error_code = lpDDSBackBuf->lpVtbl->Unlock(lpDDSBackBuf, NULL);
#endif
			if (error_code != DD_OK)
				DDErrMsg(error_code, 273, "C:\\Src\\Diablo\\Source\\dx.cpp");
		}
	}
#ifdef __cplusplus
	sgMemCrit.Leave();
#endif
}

void dx_cleanup()
{
	if (ghMainWnd)
		ShowWindow(ghMainWnd, SW_HIDE);
	SDrawDestroy();
#ifdef __cplusplus
	sgMemCrit.Enter();
#endif
	if (sgpBackBuf != NULL) {
		MemFreeDbg(sgpBackBuf);
	} else if (lpDDSBackBuf != NULL) {
#ifdef __cplusplus
		lpDDSBackBuf->Release();
#else
		lpDDSBackBuf->lpVtbl->Release(lpDDSBackBuf);
#endif
		lpDDSBackBuf = NULL;
	}
	sgdwLockCount = 0;
	gpBuffer = NULL;
#ifdef __cplusplus
	sgMemCrit.Leave();
#endif
	if (lpDDSPrimary) {
#ifdef __cplusplus
		lpDDSPrimary->Release();
#else
		lpDDSPrimary->lpVtbl->Release(lpDDSPrimary);
#endif
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

DEVILUTION_END_NAMESPACE
