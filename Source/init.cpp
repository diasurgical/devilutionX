/**
 * @file init.cpp
 *
 * Implementation of routines for initializing the environment, disable screen saver, load MPQ.
 */
#include "all.h"

#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include <SDL.h>
#include <config.h>

DEVILUTION_BEGIN_NAMESPACE

_SNETVERSIONDATA fileinfo;
int gbActive;
char diablo_exe_path[MAX_PATH];
HANDLE hellfire_mpq;
char patch_rt_mpq_path[MAX_PATH];
WNDPROC CurrentProc;
HANDLE diabdat_mpq = NULL;
char diabdat_mpq_path[MAX_PATH];
HANDLE patch_rt_mpq;

/* data */

char gszVersionNumber[MAX_PATH] = "internal version unknown";
char gszProductName[MAX_PATH] = "Diablo v1.09";

void init_cleanup()
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

	NetClose();
}

void init_create_window()
{
	if (!SpawnWindow(PROJECT_NAME, SCREEN_WIDTH, SCREEN_HEIGHT))
		app_fatal("Unable to create main window");
	dx_init(NULL);
	was_window_init = true;
	gbActive = true;
	gpBufStart = &gpBuffer[BUFFER_WIDTH * SCREEN_Y];
	gpBufEnd = (BYTE *)(BUFFER_WIDTH * (SCREEN_HEIGHT + SCREEN_Y));
	SDL_DisableScreenSaver();
}

void init_archives()
{
	HANDLE fh;
	memset(&fileinfo, 0, sizeof(fileinfo));
	fileinfo.size = sizeof(fileinfo);
	fileinfo.versionstring = gszVersionNumber;
	fileinfo.executablefile = diablo_exe_path;
	fileinfo.originalarchivefile = diabdat_mpq_path;
	fileinfo.patcharchivefile = patch_rt_mpq_path;
	init_get_file_info();
#ifdef SPAWN
		diabdat_mpq = init_test_access(diabdat_mpq_path, "spawn.mpq", "DiabloSpawn", 1000, FS_PC);
#else
	diabdat_mpq = init_test_access(diabdat_mpq_path, "diabdat.mpq", "DiabloCD", 1000, FS_CD);
#endif
	if (!SFileOpenFile("ui_art\\title.pcx", &fh))
#ifdef SPAWN
		InsertCDDlg("spawn.mpq");
#else
		InsertCDDlg("diabdat.mpq");
#endif
	SFileCloseFile(fh);
#ifdef SPAWN
	patch_rt_mpq = init_test_access(patch_rt_mpq_path, "patch_sh.mpq", "DiabloSpawn", 2000, FS_PC);
#else
	patch_rt_mpq = init_test_access(patch_rt_mpq_path, "patch_rt.mpq", "DiabloInstall", 2000, FS_PC);
#endif
}

HANDLE init_test_access(char *mpq_path, char *mpq_name, char *reg_loc, int dwPriority, int fs)
{
	char Buffer[2][MAX_PATH];
	HANDLE archive;

	GetBasePath(Buffer[0], MAX_PATH);
	GetPrefPath(Buffer[1], MAX_PATH);

	for (int i = 0; i < 2; i++) {
		snprintf(mpq_path, MAX_PATH, "%s%s", Buffer[i], mpq_name);
#if !defined(__SWITCH__) && !defined(__AMIGA__)
		if (SFileOpenArchive(mpq_path, dwPriority, MPQ_FLAG_READ_ONLY, &archive)) {
#else
		if (SFileOpenArchive(mpq_path, dwPriority, 0, &archive)) {
#endif
			SFileSetBasePath(Buffer[i]);
			return archive;

		}
	}

	return NULL;
}

void init_get_file_info()
{
	snprintf(gszProductName, MAX_PATH, "%s v%s", PROJECT_NAME, PROJECT_VERSION);
	snprintf(gszVersionNumber, MAX_PATH, "version %s", PROJECT_VERSION);
}

LRESULT MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
	case DVL_WM_ERASEBKGND:
		return 0;
	case DVL_WM_PAINT:
		force_redraw = 255;
		break;
	case DVL_WM_CLOSE:
		return 0;
	case DVL_WM_QUERYNEWPALETTE:
		return 1;
	case DVL_WM_QUERYENDSESSION:
		diablo_quit(0);
	}

	return 0;
}

WNDPROC SetWindowProc(WNDPROC NewProc)
{
	WNDPROC OldProc;

	OldProc = CurrentProc;
	CurrentProc = NewProc;
	return OldProc;
}

DEVILUTION_END_NAMESPACE
