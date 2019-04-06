//HEADER_GOES_HERE

#include "../types.h"

DEVILUTION_BEGIN_NAMESPACE

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
static CRITICAL_SECTION sgMemCrit;
char gbBackBuf;    // weak
char gbEmulate;    // weak
HMODULE ghDiabMod; // idb

#ifndef _MSC_VER
__attribute__((constructor))
#endif
static void
dx_c_init(void)
{
	dx_init_mutex();
	dx_cleanup_mutex_atexit();
}

SEG_ALLOCATE(SEGMENT_C_INIT)
_PVFV dx_c_init_funcs[] = { &dx_c_init };

void __cdecl dx_init_mutex()
{
	InitializeCriticalSection(&sgMemCrit);
}

void __cdecl dx_cleanup_mutex_atexit()
{
	atexit(dx_cleanup_mutex);
}

void __cdecl dx_cleanup_mutex(void)
{
	DeleteCriticalSection(&sgMemCrit);
}

void __fastcall dx_init(HWND hWnd)
{
	HWND v1;  // esi
	GUID *v2; // ecx
	int v3;   // eax
	int v4;   // eax
	//int v5; // ecx
	int v6;     // edi
	int v7;     // eax
	int v8;     // eax
	HWND hWnda; // [esp+1Ch] [ebp-4h]

	v1 = hWnd;
	hWnda = hWnd;
	SetFocus(hWnd);
	ShowWindow(v1, SW_SHOWNORMAL);
	v2 = NULL;
	if (gbEmulate)
		v2 = (GUID *)DDCREATE_EMULATIONONLY;
	v3 = dx_DirectDrawCreate(v2, &lpDDInterface, NULL);
	if (v3)
		ErrDlg(IDD_DIALOG1, v3, "C:\\Src\\Diablo\\Source\\dx.cpp", 149);
	fullscreen = 1;
#ifdef __cplusplus
	v4 = lpDDInterface->SetCooperativeLevel(v1, DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN);
#else
	v4 = lpDDInterface->lpVtbl->SetCooperativeLevel(lpDDInterface, v1, DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_FULLSCREEN);
#endif
	if (v4 == DDERR_EXCLUSIVEMODEALREADYSET) {
		MI_Dummy(0); // v5
	} else if (v4) {
		ErrDlg(IDD_DIALOG1, v4, "C:\\Src\\Diablo\\Source\\dx.cpp", 170);
	}
#ifdef __cplusplus
	if (lpDDInterface->SetDisplayMode(640, 480, 8)) {
#else
	if (lpDDInterface->lpVtbl->SetDisplayMode(lpDDInterface, 640, 480, 8)) {
#endif
		v6 = GetSystemMetrics(SM_CXSCREEN);
		v7 = GetSystemMetrics(SM_CYSCREEN);
#ifdef __cplusplus
		v8 = lpDDInterface->SetDisplayMode(v6, v7, 8);
#else
		v8 = lpDDInterface->lpVtbl->SetDisplayMode(lpDDInterface, v6, v7, 8);
#endif
		if (v8)
			ErrDlg(IDD_DIALOG1, v8, "C:\\Src\\Diablo\\Source\\dx.cpp", 183);
	}
	dx_create_primary_surface();
	palette_init();
	GdiSetBatchLimit(1);
	dx_create_back_buffer();
	SDrawManualInitialize(hWnda, lpDDInterface, lpDDSPrimary, 0, 0, lpDDSBackBuf, lpDDPalette, 0);
}
// 52A549: using guessed type char gbEmulate;

void __cdecl dx_create_back_buffer()
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
			sgpBackBuf = (BYTE *)DiabloAllocPtr(656 * 768);
			return;
		}
		if (error_code != DDERR_CANTLOCKSURFACE)
			ErrDlg(IDD_DIALOG1, error_code, "C:\\Src\\Diablo\\Source\\dx.cpp", 81);
	}

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwWidth = 768;
	ddsd.lPitch = 768;
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_PIXELFORMAT | DDSD_PITCH | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwHeight = 656;
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
// 52A548: using guessed type char gbBackBuf;

void __cdecl dx_create_primary_surface()
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

HRESULT __fastcall dx_DirectDrawCreate(LPGUID guid, LPDIRECTDRAW *lplpDD, LPUNKNOWN pUnkOuter)
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

void __fastcall j_lock_buf_priv(BYTE idx)
{
#ifdef _DEBUG
	++locktbl[idx];
#endif
	lock_buf_priv();
}

void __cdecl lock_buf_priv()
{
	DDSURFACEDESC ddsd;
	HRESULT error_code;

	EnterCriticalSection(&sgMemCrit);
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

	gpBufEnd += (int)ddsd.lpSurface;
	gpBuffer = (BYTE *)ddsd.lpSurface;
	sgdwLockCount++;
}

void __fastcall j_unlock_buf_priv(BYTE idx)
{
#ifdef _DEBUG
	if (!locktbl[idx])
		app_fatal("Draw lock underflow: 0x%x", idx);
	--locktbl[idx];
#endif
	unlock_buf_priv();
}

void __cdecl unlock_buf_priv()
{
	HRESULT error_code;

	if (sgdwLockCount == 0)
		app_fatal("draw main unlock error");
	if (!gpBuffer)
		app_fatal("draw consistency error");

	sgdwLockCount--;
	if (sgdwLockCount == 0) {
		gpBufEnd -= (int)gpBuffer;
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
	LeaveCriticalSection(&sgMemCrit);
}

void __cdecl dx_cleanup()
{
	BYTE *v0; // ecx

	if (ghMainWnd)
		ShowWindow(ghMainWnd, SW_HIDE);
	SDrawDestroy();
	EnterCriticalSection(&sgMemCrit);
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
		lpDDSBackBuf = NULL;
	}
	sgdwLockCount = 0;
	gpBuffer = 0;
	LeaveCriticalSection(&sgMemCrit);
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

void __cdecl dx_reinit()
{
	int lockCount;

	EnterCriticalSection(&sgMemCrit);
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

	LeaveCriticalSection(&sgMemCrit);
}

DEVILUTION_END_NAMESPACE
