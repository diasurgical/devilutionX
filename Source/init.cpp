#include "diablo.h"

#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include <SDL.h>

DEVILUTION_BEGIN_NAMESPACE

_SNETVERSIONDATA fileinfo;
int gbActive;
char diablo_exe_path[MAX_PATH];
HANDLE hellfire_mpq;
char patch_rt_mpq_path[MAX_PATH];
WNDPROC CurrentProc;
HANDLE diabdat_mpq;
char diabdat_mpq_path[MAX_PATH];
HANDLE patch_rt_mpq;

/* data */

char gszVersionNumber[MAX_PATH] = "internal version unknown";
char gszProductName[MAX_PATH] = "Diablo v1.09";

void init_cleanup(BOOL show_cursor)
{
	pfile_flush_W();

	if (diabdat_mpq) {
		SFileCloseArchive(diabdat_mpq);
		diabdat_mpq = NULL;
	}
	if (patch_rt_mpq) {
		SFileCloseArchive(patch_rt_mpq);
		patch_rt_mpq = NULL;
	}
	if (hellfire_mpq) {
		SFileCloseArchive(hellfire_mpq);
		hellfire_mpq = NULL;
	}

	UiDestroy();
	effects_cleanup_sfx();
	sound_cleanup();
	NetClose();
	dx_cleanup();
	engine_debug_trap(show_cursor);

	if (show_cursor)
		ShowCursor(TRUE);
}

void init_create_window(int nCmdShow)
{
	int nWidth, nHeight;
	HWND hWnd;

	pfile_init_save_directory();

	if (GetSystemMetrics(SM_CXSCREEN) < SCREEN_WIDTH)
		nWidth = SCREEN_WIDTH;
	else
		nWidth = GetSystemMetrics(SM_CXSCREEN);
	if (GetSystemMetrics(SM_CYSCREEN) < SCREEN_HEIGHT)
		nHeight = SCREEN_HEIGHT;
	else
		nHeight = GetSystemMetrics(SM_CYSCREEN);
	hWnd = CreateWindowEx(0, "DIABLO", "DIABLO", WS_POPUP, 0, 0, nWidth, nHeight, NULL, NULL, ghInst, NULL);
	if (!hWnd)
		app_fatal("Unable to create main window");
	ShowWindow(hWnd, SW_SHOWNORMAL); // nCmdShow used only in beta: ShowWindow(hWnd, nCmdShow)
	UpdateWindow(hWnd);
	dx_init(hWnd);
	BlackPalette();
	snd_init(hWnd);
	init_archives();
	SDL_DisableScreenSaver();
}

void init_archives()
{
	HANDLE fh;
#ifdef COPYPROT
	int result;
#endif
	memset(&fileinfo, 0, sizeof(fileinfo));
	fileinfo.size = sizeof(fileinfo);
	fileinfo.versionstring = gszVersionNumber;
	fileinfo.executablefile = diablo_exe_path;
	fileinfo.originalarchivefile = diabdat_mpq_path;
	fileinfo.patcharchivefile = patch_rt_mpq_path;
	init_get_file_info();
#ifdef SPAWN
		diabdat_mpq = init_test_access(diabdat_mpq_path, "spawn.mpq", "DiabloSpawn", MPQ_FLAG_READ_ONLY, FS_PC);
#else
		diabdat_mpq = init_test_access(diabdat_mpq_path, "diabdat.mpq", "DiabloCD", MPQ_FLAG_READ_ONLY, FS_PC);
#endif
	if (!WOpenFile("ui_art\\title.pcx", &fh, TRUE))
#ifdef SPAWN
		FileErrDlg("Main program archive: spawn.mpq");
#else
		FileErrDlg("Main program archive: diabdat.mpq");
#endif
	WCloseFile(fh);
#ifdef SPAWN
	patch_rt_mpq = init_test_access(patch_rt_mpq_path, "patch_sh.mpq", "DiabloSpawn", MPQ_FLAG_READ_ONLY, FS_PC);
#else
	patch_rt_mpq = init_test_access(patch_rt_mpq_path, "patch_rt.mpq", "DiabloInstall", MPQ_FLAG_READ_ONLY, FS_PC);
#endif
}

HANDLE init_test_access(char *mpq_path, char *mpq_name, char *reg_loc, int flags, int fs)
{
	char Buffer[2][MAX_PATH];
	HANDLE archive;

	GetCurrentDirectory(MAX_PATH, Buffer[0]); // Package
	GetModuleFileName(NULL, Buffer[1], MAX_PATH); // Preferences

	for (int i = 0; i < 2; i++) {
		strcpy(mpq_path, Buffer[i]);
		strcat(mpq_path, mpq_name);
		if (SFileOpenArchive(mpq_path, 0, flags, &archive)) {
			SFileSetBasePath(Buffer[i]);
			return archive;
		}
	}

	return NULL;
}

void init_get_file_info()
{
	DWORD dwLen;
	void *pBlock;
	unsigned int uBytes;
	DWORD dwHandle;
	VS_FIXEDFILEINFO *lpBuffer;

	if (GetModuleFileName(ghInst, diablo_exe_path, sizeof(diablo_exe_path))) {
		dwLen = GetFileVersionInfoSize(diablo_exe_path, &dwHandle);
		if (dwLen) {
			pBlock = DiabloAllocPtr(dwLen);
			if (GetFileVersionInfo(diablo_exe_path, 0, dwLen, pBlock)) {
				if (VerQueryValue(pBlock, "\\", (LPVOID *)&lpBuffer, &uBytes))
					sprintf(
					    gszVersionNumber,
					    "version %d.%d.%d.%d",
					    lpBuffer->dwProductVersionMS >> 16,
					    lpBuffer->dwProductVersionMS & 0xFFFF,
					    lpBuffer->dwProductVersionLS >> 16,
					    lpBuffer->dwProductVersionLS & 0xFFFF);
			}
			mem_free_dbg(pBlock);
		}
	}
}

LRESULT __stdcall MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
	case WM_ERASEBKGND:
		return 0;
	case WM_CREATE:
		ghMainWnd = hWnd;
		break;
	case WM_DESTROY:
		init_cleanup(1);
		ghMainWnd = 0;
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		drawpanflag = 255;
		break;
	case WM_CLOSE:
		return 0;
	case WM_ACTIVATEAPP:
		init_activate_window(hWnd, wParam);
		break;
#ifdef _DEBUG
	case WM_SYSKEYUP:
		if (wParam == VK_RETURN) {
			fullscreen = !fullscreen;
			dx_reinit();
			return 0;
		}
		break;
#endif
	case WM_QUERYNEWPALETTE:
		return 1;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

void init_activate_window(HWND hWnd, BOOL bActive)
{
	gbActive = bActive;
	UiAppActivate(bActive);

	if (gbActive) {
		drawpanflag = 255;
		ResetPal();
	}
}

WNDPROC SetWindowProc(WNDPROC NewProc)
{
	WNDPROC OldProc;

	OldProc = CurrentProc;
	CurrentProc = NewProc;
	return OldProc;
}

DEVILUTION_END_NAMESPACE
