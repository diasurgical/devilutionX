/**
 * @file init.cpp
 *
 * Implementation of routines for initializing the environment, disable screen saver, load MPQ.
 */
#include <string>

#include "all.h"
#include "paths.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include <SDL.h>
#include <config.h>

DEVILUTION_BEGIN_NAMESPACE

_SNETVERSIONDATA fileinfo;
/** True if the game is the current active window */
int gbActive;
/** A handle to an unused MPQ archive. */
HANDLE hellfire_mpq;
/** The current input handler function */
WNDPROC CurrentProc;
/** A handle to the spawn.mpq archive. */
HANDLE spawn_mpq;
/** A handle to the diabdat.mpq archive. */
HANDLE diabdat_mpq;
/** A handle to the patch_rt.mpq archive. */
HANDLE patch_rt_mpq;
/** Indicate if we only have access to demo data */
bool gbIsSpawn;
/** Indicate if we have loaded the Hellfire expansion data */
bool gbIsHellfire;
HANDLE hfmonk_mpq;
HANDLE hfbard_mpq;
HANDLE hfbarb_mpq;
HANDLE hfmusic_mpq;
HANDLE hfvoice_mpq;
HANDLE hfopt1_mpq;
HANDLE hfopt2_mpq;
HANDLE devilutionx_mpq;

namespace {

HANDLE init_test_access(const char *mpq_name, const char *reg_loc, int dwPriority, int fs)
{
	HANDLE archive;
	const std::string *paths[2] = { &GetBasePath(), &GetPrefPath() };
	std::string mpq_abspath;
	DWORD mpq_flags = 0;
#if !defined(__SWITCH__) && !defined(__AMIGA__)
	mpq_flags |= MPQ_FLAG_READ_ONLY;
#endif
	for (int i = 0; i < 2; i++) {
		mpq_abspath = *paths[i] + mpq_name;
		if (SFileOpenArchive(mpq_abspath.c_str(), dwPriority, mpq_flags, &archive)) {
			SFileSetBasePath(paths[i]->c_str());
			return archive;
		}
	}

	return NULL;
}

} // namespace

/* data */

char gszVersionNumber[260] = "internal version unknown";
char gszProductName[260] = "Diablo v1.09";

void init_cleanup()
{
	pfile_flush_W();

	if (spawn_mpq) {
		SFileCloseArchive(spawn_mpq);
		spawn_mpq = NULL;
	}
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
	if (hfmonk_mpq) {
		SFileCloseArchive(hfmonk_mpq);
		hfmonk_mpq = NULL;
	}
	if (hfbard_mpq) {
		SFileCloseArchive(hfbard_mpq);
		hfbard_mpq = NULL;
	}
	if (hfbarb_mpq) {
		SFileCloseArchive(hfbarb_mpq);
		hfbarb_mpq = NULL;
	}
	if (hfmusic_mpq) {
		SFileCloseArchive(hfmusic_mpq);
		hfmusic_mpq = NULL;
	}
	if (hfvoice_mpq) {
		SFileCloseArchive(hfvoice_mpq);
		hfvoice_mpq = NULL;
	}
	if (hfopt1_mpq) {
		SFileCloseArchive(hfopt1_mpq);
		hfopt1_mpq = NULL;
	}
	if (hfopt2_mpq) {
		SFileCloseArchive(hfopt2_mpq);
		hfopt2_mpq = NULL;
	}
	if (devilutionx_mpq) {
		SFileCloseArchive(patch_rt_mpq);
		patch_rt_mpq = NULL;
	}

	NetClose();
}

static void init_get_file_info()
{
	snprintf(gszProductName, sizeof(gszProductName) / sizeof(char), "%s v%s", PROJECT_NAME, PROJECT_VERSION);
	snprintf(gszVersionNumber, sizeof(gszVersionNumber) / sizeof(char), "version %s", PROJECT_VERSION);
}

void init_archives()
{
	HANDLE fh = NULL;
	memset(&fileinfo, 0, sizeof(fileinfo));
	fileinfo.size = sizeof(fileinfo);
	fileinfo.versionstring = gszVersionNumber;
	fileinfo.executablefile = "";
	fileinfo.originalarchivefile = "";
	fileinfo.patcharchivefile = "";
	init_get_file_info();

	diabdat_mpq = init_test_access("diabdat.mpq", "DiabloCD", 1000, FS_CD);
	if (diabdat_mpq == NULL) {
		spawn_mpq = init_test_access("spawn.mpq", "DiabloSpawn", 1000, FS_PC);
		if (spawn_mpq != NULL)
			gbIsSpawn = true;
	}
	if (!SFileOpenFile("ui_art\\title.pcx", &fh))
		InsertCDDlg();
	SFileCloseFile(fh);

	patch_rt_mpq = init_test_access("patch_rt.mpq", "DiabloInstall", 2000, FS_PC);
	if (patch_rt_mpq == NULL)
		patch_rt_mpq = init_test_access("patch_sh.mpq", "DiabloSpawn", 2000, FS_PC);

	hellfire_mpq = init_test_access("hellfire.mpq", "DiabloInstall", 8000, FS_PC);
	if (hellfire_mpq != NULL)
		gbIsHellfire = true;
	hfmonk_mpq = init_test_access("hfmonk.mpq", "DiabloInstall", 8100, FS_PC);
	hfbard_mpq = init_test_access("hfbard.mpq", "DiabloInstall", 8110, FS_PC);
	hfbarb_mpq = init_test_access("hfbarb.mpq", "DiabloInstall", 8120, FS_PC);
	hfmusic_mpq = init_test_access("hfmusic.mpq", "DiabloInstall", 8200, FS_PC);
	hfvoice_mpq = init_test_access("hfvoice.mpq", "DiabloInstall", 8500, FS_PC);
	hfopt1_mpq = init_test_access("hfopt1.mpq", "DiabloInstall", 8600, FS_PC);
	hfopt2_mpq = init_test_access("hfopt2.mpq", "DiabloInstall", 8610, FS_PC);
	devilutionx_mpq = init_test_access("devilutionx.mpq", "DiabloInstall", 9000, FS_PC);
}

void init_create_window()
{
	if (!SpawnWindow(PROJECT_NAME))
		app_fatal("Unable to create main window");
	dx_init(NULL);
	gbActive = true;
	gpBufStart = &gpBuffer[BUFFER_WIDTH * SCREEN_Y];
	gpBufEnd = (BYTE *)(BUFFER_WIDTH * (SCREEN_HEIGHT + SCREEN_Y));
	SDL_DisableScreenSaver();
}

void MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg) {
	case DVL_WM_PAINT:
		force_redraw = 255;
		break;
	case DVL_WM_QUERYENDSESSION:
		diablo_quit(0);
		break;
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
