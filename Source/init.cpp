/**
 * @file init.cpp
 *
 * Implementation of routines for initializing the environment, disable screen saver, load MPQ.
 */
#include <string>
#include <vector>

#include "all.h"
#include "paths.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../DiabloUI/diabloui.h"
#include <SDL.h>
#include <config.h>

#ifdef __vita__
// increase default allowed heap size on Vita
int _newlib_heap_size_user = 100 * 1024 * 1024;
#endif

namespace devilution {

/** True if the game is the current active window */
int gbActive;
/** A handle to an hellfire.mpq archive. */
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
/** Indicate if we want vanilla savefiles */
bool gbVanilla;
HANDLE hfmonk_mpq;
HANDLE hfbard_mpq;
HANDLE hfbarb_mpq;
HANDLE hfmusic_mpq;
HANDLE hfvoice_mpq;
HANDLE hfopt1_mpq;
HANDLE hfopt2_mpq;
HANDLE devilutionx_mpq;

namespace {

HANDLE init_test_access(const std::vector<std::string> &paths, const char *mpq_name)
{
	HANDLE archive;
	std::string mpq_abspath;
	for (int i = 0; i < paths.size(); i++) {
		mpq_abspath = paths[i] + mpq_name;
		if (SFileOpenArchive(mpq_abspath.c_str(), 0, MPQ_FLAG_READ_ONLY, &archive)) {
			SFileSetBasePath(paths[i].c_str());
			return archive;
		}
	}

	return NULL;
}

} // namespace

/* data */

char gszVersionNumber[64] = "internal version unknown";
char gszProductName[64] = "DevilutionX vUnknown";

void init_cleanup()
{
	if (gbIsMultiplayer && gbRunGame) {
		pfile_write_hero();
	}
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
	init_get_file_info();

	std::vector<std::string> paths;
	paths.reserve(5);
	paths.push_back(GetBasePath());
	paths.push_back(GetPrefPath());
	if (paths[0] == paths[1])
		paths.pop_back();

#ifdef __linux__
	paths.push_back("/usr/share/diasurgical/devilutionx/");
	paths.push_back("/usr/local/share/diasurgical/devilutionx/");
#endif

	paths.push_back(""); // PWD

	diabdat_mpq = init_test_access(paths, "DIABDAT.MPQ");
	if (diabdat_mpq == NULL) {
		// DIABDAT.MPQ is uppercase on the original CD and the GOG version.
		diabdat_mpq = init_test_access(paths, "diabdat.mpq");
	}

	if (diabdat_mpq == NULL) {
		spawn_mpq = init_test_access(paths, "spawn.mpq");
		if (spawn_mpq != NULL)
			gbIsSpawn = true;
	}
	HANDLE fh = NULL;
	if (!SFileOpenFile("ui_art\\title.pcx", &fh))
		InsertCDDlg();
	SFileCloseFile(fh);

	patch_rt_mpq = init_test_access(paths, "patch_rt.mpq");
	if (patch_rt_mpq == NULL)
		patch_rt_mpq = init_test_access(paths, "patch_sh.mpq");

	hellfire_mpq = init_test_access(paths, "hellfire.mpq");
	if (hellfire_mpq != NULL)
		gbIsHellfire = true;
	hfmonk_mpq = init_test_access(paths, "hfmonk.mpq");
	hfbard_mpq = init_test_access(paths, "hfbard.mpq");
	if (hfbard_mpq != NULL)
		gbBard = true;
	hfbarb_mpq = init_test_access(paths, "hfbarb.mpq");
	if (hfbarb_mpq != NULL)
		gbBarbarian = true;
	hfmusic_mpq = init_test_access(paths, "hfmusic.mpq");
	hfvoice_mpq = init_test_access(paths, "hfvoice.mpq");
	hfopt1_mpq = init_test_access(paths, "hfopt1.mpq");
	hfopt2_mpq = init_test_access(paths, "hfopt2.mpq");

	if (gbIsHellfire && (hfmonk_mpq == NULL || hfmusic_mpq == NULL || hfvoice_mpq == NULL)) {
		UiErrorOkDialog("Some Hellfire MPQs are missing", "Not all Hellfire MPQs were found.\nPlease copy all the hf*.mpq files.");
		app_fatal(NULL);
	}

	devilutionx_mpq = init_test_access(paths, "devilutionx.mpq");
}

void init_create_window()
{
	if (!SpawnWindow(PROJECT_NAME))
		app_fatal("Unable to create main window");
	dx_init();
	gbActive = true;
	SDL_DisableScreenSaver();
}

void MainWndProc(UINT Msg, WPARAM wParam, LPARAM lParam)
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

} // namespace devilution
