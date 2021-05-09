/**
 * @file init.cpp
 *
 * Implementation of routines for initializing the environment, disable screen saver, load MPQ.
 */
#include <SDL.h>
#include <config.h>
#include <string>
#include <vector>

#include "DiabloUI/diabloui.h"
#include "dx.h"
#include "pfile.h"
#include "storm/storm.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/ui_fwd.h"

#ifdef __vita__
// increase default allowed heap size on Vita
int _newlib_heap_size_user = 100 * 1024 * 1024;
#endif

namespace devilution {

/** True if the game is the current active window */
bool gbActive;
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
	for (const auto &path : paths) {
		mpq_abspath = path + mpq_name;
		if (SFileOpenArchive(mpq_abspath.c_str(), 0, MPQ_OPEN_READ_ONLY, &archive)) {
			LogVerbose("  Found: {} in {}", mpq_name, path);
			SFileSetBasePath(path.c_str());
			return archive;
		}
		if (SErrGetLastError() != STORM_ERROR_FILE_NOT_FOUND) {
			LogError("Open error {}: {}", SErrGetLastError(), mpq_abspath);
		}
	}
	if (SErrGetLastError() == STORM_ERROR_FILE_NOT_FOUND) {
		LogVerbose("Missing: {}", mpq_name);
	}

	return nullptr;
}

} // namespace

void init_cleanup()
{
	if (gbIsMultiplayer && gbRunGame) {
		pfile_write_hero(/*write_game_data=*/false, /*clear_tables=*/true);
	}

	if (spawn_mpq != nullptr) {
		SFileCloseArchive(spawn_mpq);
		spawn_mpq = nullptr;
	}
	if (diabdat_mpq != nullptr) {
		SFileCloseArchive(diabdat_mpq);
		diabdat_mpq = nullptr;
	}
	if (patch_rt_mpq != nullptr) {
		SFileCloseArchive(patch_rt_mpq);
		patch_rt_mpq = nullptr;
	}
	if (hellfire_mpq != nullptr) {
		SFileCloseArchive(hellfire_mpq);
		hellfire_mpq = nullptr;
	}
	if (hfmonk_mpq != nullptr) {
		SFileCloseArchive(hfmonk_mpq);
		hfmonk_mpq = nullptr;
	}
	if (hfbard_mpq != nullptr) {
		SFileCloseArchive(hfbard_mpq);
		hfbard_mpq = nullptr;
	}
	if (hfbarb_mpq != nullptr) {
		SFileCloseArchive(hfbarb_mpq);
		hfbarb_mpq = nullptr;
	}
	if (hfmusic_mpq != nullptr) {
		SFileCloseArchive(hfmusic_mpq);
		hfmusic_mpq = nullptr;
	}
	if (hfvoice_mpq != nullptr) {
		SFileCloseArchive(hfvoice_mpq);
		hfvoice_mpq = nullptr;
	}
	if (hfopt1_mpq != nullptr) {
		SFileCloseArchive(hfopt1_mpq);
		hfopt1_mpq = nullptr;
	}
	if (hfopt2_mpq != nullptr) {
		SFileCloseArchive(hfopt2_mpq);
		hfopt2_mpq = nullptr;
	}
	if (devilutionx_mpq != nullptr) {
		SFileCloseArchive(patch_rt_mpq);
		patch_rt_mpq = nullptr;
	}

	NetClose();
}

void init_archives()
{
	std::vector<std::string> paths;
	paths.reserve(5);
#ifdef __ANDROID__
	paths.push_back(std::string(getenv("EXTERNAL_STORAGE")) + "/devilutionx/");
#else
	paths.push_back(paths::BasePath());
#endif
	paths.push_back(paths::PrefPath());
	if (paths[0] == paths[1])
		paths.pop_back();

#ifdef __ANDROID__
	if (getenv("SECONDARY_STORAGE") != nullptr)
		paths.emplace_back(std::string(getenv("SECONDARY_STORAGE")) + "/devilutionx/");
	if (getenv("EXTERNAL_SDCARD_STORAGE") != nullptr)
		paths.emplace_back(std::string(getenv("EXTERNAL_SDCARD_STORAGE")) + "/devilutionx/");
#elif defined(__linux__)
	paths.emplace_back("/usr/share/diasurgical/devilutionx/");
	paths.emplace_back("/usr/local/share/diasurgical/devilutionx/");
#elif defined(__3DS__)
	paths.emplace_back("romfs:/");
#endif

	paths.emplace_back(""); // PWD

	if (SDL_LOG_PRIORITY_VERBOSE >= SDL_LogGetPriority(SDL_LOG_CATEGORY_APPLICATION)) {
		std::string message;
		for (std::size_t i = 0; i < paths.size(); ++i) {
			char prefix[32];
			std::snprintf(prefix, sizeof(prefix), "\n%6u. '", static_cast<unsigned>(i + 1));
			message.append(prefix);
			message.append(paths[i]);
			message += '\'';
		}
		LogVerbose("MPQ search paths:{}", message);
	}

	// Load devilutionx.mpq first to get the font file for error messages
	devilutionx_mpq = init_test_access(paths, "devilutionx.mpq");

	diabdat_mpq = init_test_access(paths, "DIABDAT.MPQ");
	if (diabdat_mpq == nullptr) {
		// DIABDAT.MPQ is uppercase on the original CD and the GOG version.
		diabdat_mpq = init_test_access(paths, "diabdat.mpq");
	}

	if (diabdat_mpq == nullptr) {
		spawn_mpq = init_test_access(paths, "spawn.mpq");
		if (spawn_mpq != nullptr)
			gbIsSpawn = true;
	}
	HANDLE fh = nullptr;
	if (!SFileOpenFile("ui_art\\title.pcx", &fh))
		InsertCDDlg();
	SFileCloseFileThreadSafe(fh);

	patch_rt_mpq = init_test_access(paths, "patch_rt.mpq");
	if (patch_rt_mpq == nullptr)
		patch_rt_mpq = init_test_access(paths, "patch_sh.mpq");

	hellfire_mpq = init_test_access(paths, "hellfire.mpq");
	if (hellfire_mpq != nullptr)
		gbIsHellfire = true;
	hfmonk_mpq = init_test_access(paths, "hfmonk.mpq");
	hfbard_mpq = init_test_access(paths, "hfbard.mpq");
	if (hfbard_mpq != nullptr)
		gbBard = true;
	hfbarb_mpq = init_test_access(paths, "hfbarb.mpq");
	if (hfbarb_mpq != nullptr)
		gbBarbarian = true;
	hfmusic_mpq = init_test_access(paths, "hfmusic.mpq");
	hfvoice_mpq = init_test_access(paths, "hfvoice.mpq");
	hfopt1_mpq = init_test_access(paths, "hfopt1.mpq");
	hfopt2_mpq = init_test_access(paths, "hfopt2.mpq");

	if (gbIsHellfire && (hfmonk_mpq == nullptr || hfmusic_mpq == nullptr || hfvoice_mpq == nullptr)) {
		UiErrorOkDialog(_("Some Hellfire MPQs are missing"), _("Not all Hellfire MPQs were found.\nPlease copy all the hf*.mpq files."));
		app_fatal(nullptr);
	}
}

void init_create_window()
{
	if (!SpawnWindow(PROJECT_NAME))
		app_fatal("%s", _("Unable to create main window"));
	dx_init();
	gbActive = true;
#ifndef USE_SDL1
	SDL_DisableScreenSaver();
#endif
}

void MainWndProc(uint32_t Msg)
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
