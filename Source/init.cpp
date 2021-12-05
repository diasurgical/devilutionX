/**
 * @file init.cpp
 *
 * Implementation of routines for initializing the environment, disable screen saver, load MPQ.
 */
#include <SDL.h>
#include <config.h>
#include <string>
#include <vector>

#if defined(_WIN64) || defined(_WIN32)
#include <find_steam_game.h>
#endif

#include "DiabloUI/diabloui.h"
#include "dx.h"
#include "engine/assets.hpp"
#include "mpq/mpq_reader.hpp"
#include "options.h"
#include "pfile.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/ui_fwd.h"
#include "utils/utf8.hpp"

#ifdef __vita__
// increase default allowed heap size on Vita
int _newlib_heap_size_user = 100 * 1024 * 1024;
#endif

namespace devilution {

/** True if the game is the current active window */
bool gbActive;
/** A handle to an hellfire.mpq archive. */
std::optional<MpqArchive> hellfire_mpq;
/** The current input handler function */
WNDPROC CurrentProc;
/** A handle to the spawn.mpq archive. */
std::optional<MpqArchive> spawn_mpq;
/** A handle to the diabdat.mpq archive. */
std::optional<MpqArchive> diabdat_mpq;
/** Indicate if we only have access to demo data */
bool gbIsSpawn;
/** Indicate if we have loaded the Hellfire expansion data */
bool gbIsHellfire;
/** Indicate if we want vanilla savefiles */
bool gbVanilla;
/** Whether the Hellfire mode is required (forced). */
bool forceHellfire;
std::optional<MpqArchive> hfmonk_mpq;
std::optional<MpqArchive> hfbard_mpq;
std::optional<MpqArchive> hfbarb_mpq;
std::optional<MpqArchive> hfmusic_mpq;
std::optional<MpqArchive> hfvoice_mpq;
std::optional<MpqArchive> devilutionx_mpq;
std::optional<MpqArchive> lang_mpq;
std::optional<MpqArchive> font_mpq;

namespace {

std::optional<MpqArchive> LoadMPQ(const std::vector<std::string> &paths, const char *mpqName)
{
	std::optional<MpqArchive> archive;
	std::string mpqAbsPath;
	std::int32_t error = 0;
	for (const auto &path : paths) {
		mpqAbsPath = path + mpqName;
		if ((archive = MpqArchive::Open(mpqAbsPath.c_str(), error))) {
			LogVerbose("  Found: {} in {}", mpqName, path);
			paths::SetMpqDir(path);
			return archive;
		}
		if (error != 0) {
			LogError("Error {}: {}", MpqArchive::ErrorMessage(error), mpqAbsPath);
		}
	}
	if (error == 0) {
		LogVerbose("Missing: {}", mpqName);
	}

	return std::nullopt;
}

std::vector<std::string> GetMPQSearchPaths()
{
	std::vector<std::string> paths;
	paths.push_back(paths::BasePath());
	paths.push_back(paths::PrefPath());
	if (paths[0] == paths[1])
		paths.pop_back();

#if defined(__linux__) && !defined(__ANDROID__)
	paths.emplace_back("/usr/share/diasurgical/devilutionx/");
	paths.emplace_back("/usr/local/share/diasurgical/devilutionx/");
#elif defined(__3DS__)
	paths.emplace_back("romfs:/");
#elif defined(_WIN64) || defined(_WIN32)
	char gogpath[_FSG_PATH_MAX];
	fsg_get_gog_game_path(gogpath, "1412601690");
	if (strlen(gogpath) > 0) {
		paths.emplace_back(std::string(gogpath) + "/");
		paths.emplace_back(std::string(gogpath) + "/hellfire/");
	}
#endif

	paths.emplace_back(""); // PWD

	if (SDL_LOG_PRIORITY_VERBOSE >= SDL_LogGetPriority(SDL_LOG_CATEGORY_APPLICATION)) {
		LogVerbose("Paths:\n    base: {}\n    pref: {}\n  config: {}\n  assets: {}",
		    paths::BasePath(), paths::PrefPath(), paths::ConfigPath(), paths::AssetsPath());

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

	return paths;
}

void init_language_archives(const std::vector<std::string> &paths)
{
	string_view code = *sgOptions.Language.code;
	if (code != "en") {
		char langMpqName[10] = {};
		CopyUtf8(langMpqName, code.data(), sizeof(langMpqName));

		strncat(langMpqName, ".mpq", sizeof(langMpqName) - strlen(langMpqName) - 1);
		lang_mpq = LoadMPQ(paths, langMpqName);
	}
}

} // namespace

void init_cleanup()
{
	if (gbIsMultiplayer && gbRunGame) {
		pfile_write_hero(/*writeGameData=*/false, /*clearTables=*/true);
	}

	spawn_mpq = std::nullopt;
	diabdat_mpq = std::nullopt;
	hellfire_mpq = std::nullopt;
	hfmonk_mpq = std::nullopt;
	hfbard_mpq = std::nullopt;
	hfbarb_mpq = std::nullopt;
	hfmusic_mpq = std::nullopt;
	hfvoice_mpq = std::nullopt;
	lang_mpq = std::nullopt;
	font_mpq = std::nullopt;
	devilutionx_mpq = std::nullopt;

	NetClose();
}

void init_archives()
{
	auto paths = GetMPQSearchPaths();

#if !defined(__ANDROID__) && !defined(__APPLE__)
	// Load devilutionx.mpq first to get the font file for error messages
	devilutionx_mpq = LoadMPQ(paths, "devilutionx.mpq");
#endif
	font_mpq = LoadMPQ(paths, "fonts.mpq"); // Extra fonts

	init_language_archives(paths);

	diabdat_mpq = LoadMPQ(paths, "DIABDAT.MPQ");
	if (!diabdat_mpq) {
		// DIABDAT.MPQ is uppercase on the original CD and the GOG version.
		diabdat_mpq = LoadMPQ(paths, "diabdat.mpq");
	}

	if (!diabdat_mpq) {
		spawn_mpq = LoadMPQ(paths, "spawn.mpq");
		if (spawn_mpq)
			gbIsSpawn = true;
	}
	SDL_RWops *handle = OpenAsset("ui_art\\title.pcx");
	if (handle == nullptr) {
		LogError("{}", SDL_GetError());
		InsertCDDlg(_("diabdat.mpq or spawn.mpq"));
	}
	SDL_RWclose(handle);

	hellfire_mpq = LoadMPQ(paths, "hellfire.mpq");
	if (hellfire_mpq)
		gbIsHellfire = true;
	if (forceHellfire && !hellfire_mpq)
		InsertCDDlg("hellfire.mpq");

	hfmonk_mpq = LoadMPQ(paths, "hfmonk.mpq");
	hfbard_mpq = LoadMPQ(paths, "hfbard.mpq");
	if (hfbard_mpq)
		gbBard = true;
	hfbarb_mpq = LoadMPQ(paths, "hfbarb.mpq");
	if (hfbarb_mpq)
		gbBarbarian = true;
	hfmusic_mpq = LoadMPQ(paths, "hfmusic.mpq");
	hfvoice_mpq = LoadMPQ(paths, "hfvoice.mpq");

	if (gbIsHellfire && (!hfmonk_mpq || !hfmusic_mpq || !hfvoice_mpq)) {
		UiErrorOkDialog(_("Some Hellfire MPQs are missing"), _("Not all Hellfire MPQs were found.\nPlease copy all the hf*.mpq files."));
		app_fatal(nullptr);
	}
}

void init_language_archives()
{
	lang_mpq = std::nullopt;
	init_language_archives(GetMPQSearchPaths());
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

void MainWndProc(uint32_t msg)
{
	switch (msg) {
	case DVL_WM_PAINT:
		force_redraw = 255;
		break;
	case DVL_WM_QUERYENDSESSION:
		diablo_quit(0);
	}
}

WNDPROC SetWindowProc(WNDPROC newProc)
{
	WNDPROC oldProc;

	oldProc = CurrentProc;
	CurrentProc = newProc;
	return oldProc;
}

} // namespace devilution
