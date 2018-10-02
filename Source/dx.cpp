//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
void *sgpBackBuf;
#endif
int dx_cpp_init_value; // weak
#ifndef NO_GLOBALS
IDirectDraw *lpDDInterface;
IDirectDrawPalette *lpDDPalette; // idb
int sgdwLockCount;
Screen *gpBuffer;
IDirectDrawSurface *lpDDSBackBuf;
IDirectDrawSurface *lpDDSPrimary;
static CRITICAL_SECTION sgMemCrit;
char gbBackBuf; // weak
char gbEmulate; // weak
HMODULE ghDiabMod; // idb
#endif

int dx_inf = 0x7F800000; // weak

struct dx_cpp_init_1
{
	dx_cpp_init_1()
	{
		dx_cpp_init_value = dx_inf;
	}
} _dx_cpp_init_1;
// 47A464: using guessed type int dx_inf;
// 52A514: using guessed type int dx_cpp_init_value;

struct dx_cpp_init_2
{
	dx_cpp_init_2()
	{
		dx_init_mutex();
		dx_cleanup_mutex_atexit();
	}
} _dx_cpp_init_2;

void __cdecl dx_init_mutex()
{
	InitializeCriticalSection(&sgMemCrit);
}

void __cdecl dx_cleanup_mutex_atexit()
{
	atexit(dx_cleanup_mutex);
}

void __cdecl dx_cleanup_mutex()
{
	DeleteCriticalSection(&sgMemCrit);
}

void __fastcall dx_init(HWND hWnd)
{
	HWND v1; // esi
	GUID *v2; // ecx
	int v3; // eax
	int v4; // eax
	//int v5; // ecx
	int v6; // edi
	int v7; // eax
	int v8; // eax
	HWND hWnda; // [esp+1Ch] [ebp-4h]

	v1 = hWnd;
	hWnda = hWnd;
	SetFocus(hWnd);
	ShowWindow(v1, SW_SHOWNORMAL);
	v2 = NULL;
	if ( gbEmulate )
		v2 = (GUID *)DDCREATE_EMULATIONONLY;
	v3 = dx_DirectDrawCreate(v2, &lpDDInterface, NULL);
	if ( v3 )
		ErrDlg(IDD_DIALOG1, v3, "C:\\Src\\Diablo\\Source\\dx.cpp", 149);
	fullscreen = 1;
	v4 = lpDDInterface->SetCooperativeLevel(v1, DDSCL_EXCLUSIVE|DDSCL_ALLOWREBOOT|DDSCL_FULLSCREEN);
	if ( v4 == DDERR_EXCLUSIVEMODEALREADYSET )
	{
		MI_Dummy(0); // v5
	}
	else if ( v4 )
	{
		ErrDlg(IDD_DIALOG1, v4, "C:\\Src\\Diablo\\Source\\dx.cpp", 170);
	}
	if ( lpDDInterface->SetDisplayMode(640, 480, 8) )
	{
		v6 = GetSystemMetrics(SM_CXSCREEN);
		v7 = GetSystemMetrics(SM_CYSCREEN);
		v8 = lpDDInterface->SetDisplayMode(v6, v7, 8);
		if ( v8 )
			ErrDlg(IDD_DIALOG1, v8, "C:\\Src\\Diablo\\Source\\dx.cpp", 183);
	}
	dx_create_primary_surface();
	palette_init();
	GdiSetBatchLimit(1);
	dx_create_back_buffer();
	SDrawManualInitialize(hWnda, lpDDInterface, lpDDSPrimary, 0, 0, lpDDSBackBuf, lpDDPalette, 0);
}
// 484364: using guessed type int fullscreen;
// 52A549: using guessed type char gbEmulate;

void __cdecl dx_create_back_buffer()
{
	int v0; // eax
	int v1; // eax
	int v2; // eax
	int v3; // eax
	DDSURFACEDESC v4; // [esp+Ch] [ebp-70h]
	DDSCAPS v5; // [esp+78h] [ebp-4h]

	v0 = lpDDSPrimary->GetCaps(&v5);
	if ( v0 )
		DDErrMsg(v0, 59, "C:\\Src\\Diablo\\Source\\dx.cpp");
	if ( !gbBackBuf )
	{
		v4.dwSize = 108;
		v1 = lpDDSPrimary->Lock(NULL, &v4, DDLOCK_WRITEONLY|DDLOCK_WAIT, NULL);
		if ( !v1 )
		{
			lpDDSPrimary->Unlock(NULL);
			sgpBackBuf = DiabloAllocPtr(0x7B000);
			return;
		}
		if ( v1 != DDERR_CANTLOCKSURFACE )
			ErrDlg(IDD_DIALOG1, v1, "C:\\Src\\Diablo\\Source\\dx.cpp", 81);
	}
	memset(&v4, 0, sizeof(v4));
	v4.dwWidth = 768;
	v4.lPitch = 768;
	v4.dwSize = sizeof(v4);
	v4.dwFlags = DDSD_PIXELFORMAT|DDSD_PITCH|DDSD_WIDTH|DDSD_HEIGHT|DDSD_CAPS;
	v4.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY|DDSCAPS_OFFSCREENPLAIN;
	v4.dwHeight = 656;
	v4.ddpfPixelFormat.dwSize = 32;
	v2 = lpDDSPrimary->GetPixelFormat(&v4.ddpfPixelFormat);
	if ( v2 )
		ErrDlg(IDD_DIALOG1, v2, "C:\\Src\\Diablo\\Source\\dx.cpp", 94);
	v3 = lpDDInterface->CreateSurface(&v4, &lpDDSBackBuf, NULL);
	if ( v3 )
		ErrDlg(IDD_DIALOG1, v3, "C:\\Src\\Diablo\\Source\\dx.cpp", 96);
}
// 52A548: using guessed type char gbBackBuf;

void __cdecl dx_create_primary_surface()
{
	int v0; // eax
	DDSURFACEDESC v1; // [esp+0h] [ebp-6Ch]

	memset(&v1, 0, sizeof(v1));
	v1.dwSize = sizeof(v1);
	v1.dwFlags = DDSD_CAPS;
	v1.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	v0 = lpDDInterface->CreateSurface(&v1, &lpDDSPrimary, NULL);
	if ( v0 )
		ErrDlg(IDD_DIALOG1, v0, "C:\\Src\\Diablo\\Source\\dx.cpp", 109);
}

HRESULT __fastcall dx_DirectDrawCreate(GUID *guid, IDirectDraw **DD, void *unknown)
{
	IDirectDraw **v3; // ebp
	int v4; // eax
	FARPROC v5; // ebx
	int v6; // eax
	GUID *v8; // [esp+10h] [ebp-4h]

	v3 = DD;
	v8 = guid;
	if ( !ghDiabMod )
	{
		ghDiabMod = LoadLibrary("ddraw.dll");
		if ( !ghDiabMod )
		{
			v4 = GetLastError();
			ErrDlg(IDD_DIALOG4, v4, "C:\\Src\\Diablo\\Source\\dx.cpp", 122);
		}
	}
	v5 = GetProcAddress(ghDiabMod, "DirectDrawCreate");
	if ( !v5 )
	{
		v6 = GetLastError();
		ErrDlg(IDD_DIALOG4, v6, "C:\\Src\\Diablo\\Source\\dx.cpp", 127);
	}
	return ((int (__stdcall *)(GUID *, IDirectDraw **, void *))v5)(v8, v3, unknown);
}

void __cdecl lock_buf_priv()
{
	Screen *v0; // eax
	int v1; // eax
	DDSURFACEDESC v2; // [esp+0h] [ebp-6Ch]

	EnterCriticalSection(&sgMemCrit);
	v0 = (Screen *)sgpBackBuf;
	if ( sgpBackBuf )
		goto LABEL_8;
	if ( lpDDSBackBuf )
	{
		if ( sgdwLockCount )
			goto LABEL_9;
		v2.dwSize = sizeof(v2);
		v1 = lpDDSBackBuf->Lock(NULL, &v2, DDLOCK_WAIT, NULL);
		if ( v1 )
			DDErrMsg(v1, 235, "C:\\Src\\Diablo\\Source\\dx.cpp");
		v0 = (Screen *)v2.lpSurface;
		gpBufEnd += (unsigned int)v2.lpSurface;
LABEL_8:
		gpBuffer = v0;
		goto LABEL_9;
	}
	Sleep(20000);
	TermMsg("lock_buf_priv");
LABEL_9:
	++sgdwLockCount;
}
// 69CF0C: using guessed type int gpBufEnd;

void __cdecl unlock_buf_priv()
{
	Screen *v0; // eax
	int v1; // eax

	if ( !sgdwLockCount )
		TermMsg("draw main unlock error");
	if ( !gpBuffer )
		TermMsg("draw consistency error");
	if ( !--sgdwLockCount )
	{
		v0 = gpBuffer;
		gpBuffer = 0;
		gpBufEnd -= (signed int)v0;
		if ( !sgpBackBuf )
		{
			v1 = lpDDSBackBuf->Unlock(NULL);
			if ( v1 )
				DDErrMsg(v1, 273, "C:\\Src\\Diablo\\Source\\dx.cpp");
		}
	}
	LeaveCriticalSection(&sgMemCrit);
}
// 69CF0C: using guessed type int gpBufEnd;

void __cdecl dx_cleanup()
{
	void *v0; // ecx

	if ( ghMainWnd )
		ShowWindow(ghMainWnd, SW_HIDE);
	SDrawDestroy();
	EnterCriticalSection(&sgMemCrit);
	if ( sgpBackBuf )
	{
		v0 = sgpBackBuf;
		sgpBackBuf = 0;
		mem_free_dbg(v0);
	}
	else if ( lpDDSBackBuf )
	{
		lpDDSBackBuf->Release();
		lpDDSBackBuf = 0;
	}
	sgdwLockCount = 0;
	gpBuffer = 0;
	LeaveCriticalSection(&sgMemCrit);
	if ( lpDDSPrimary )
	{
		lpDDSPrimary->Release();
		lpDDSPrimary = 0;
	}
	if ( lpDDPalette )
	{
		lpDDPalette->Release();
		lpDDPalette = 0;
	}
	if ( lpDDInterface )
	{
		lpDDInterface->Release();
		lpDDInterface = 0;
	}
}

void __cdecl dx_reinit()
{
	int v0; // esi

	EnterCriticalSection(&sgMemCrit);
	ClearCursor();
	v0 = sgdwLockCount;
	while ( sgdwLockCount )
		unlock_buf_priv();
	dx_cleanup();
	drawpanflag = 255;
	dx_init(ghMainWnd);
	for ( ; v0; --v0 )
		lock_buf_priv();
	LeaveCriticalSection(&sgMemCrit);
}
// 52571C: using guessed type int drawpanflag;
