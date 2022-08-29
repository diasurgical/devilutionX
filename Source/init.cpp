/**
 * @file init.cpp
 *
 * Implementation of routines for initializing the environment, disable screen saver, load MPQ.
 */
#include <SDL.h>
#include <config.h>
#include <string>
#include <vector>

#if (defined(_WIN64) || defined(_WIN32)) && !defined(__UWP__) && !defined(NXDK)
#include <find_steam_game.h>
#endif

#include "DiabloUI/diabloui.h"
#include "engine/assets.hpp"
#include "engine/dx.h"
#include "hwcursor.hpp"
#include "miniwin/misc_msg.h"
#include "mpq/mpq_reader.hpp"
#include "options.h"
#include "pfile.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/str_split.hpp"
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
EventHandler CurrentEventHandler;
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

std::optional<MpqArchive> LoadMPQ(const std::vector<std::string> &paths, string_view mpqName)
{
	std::optional<MpqArchive> archive;
	std::string mpqAbsPath;
	std::int32_t error = 0;
	for (const auto &path : paths) {
		mpqAbsPath = path + mpqName.data();
		if ((archive = MpqArchive::Open(mpqAbsPath.c_str(), error))) {
			LogVerbose("  Found: {} in {}", mpqName, path);
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

#if defined(__unix__) && !defined(__ANDROID__)
	// `XDG_DATA_HOME` is usually the root path of `paths::PrefPath()`, so we only
	// add `XDG_DATA_DIRS`.
	const char *xdgDataDirs = std::getenv("XDG_DATA_DIRS");
	if (xdgDataDirs != nullptr) {
		for (const string_view path : SplitByChar(xdgDataDirs, ':')) {
			std::string fullPath(path);
			if (!path.empty() && path.back() != '/')
				fullPath += '/';
			fullPath.append("diasurgical/devilutionx/");
			paths.push_back(std::move(fullPath));
		}
	} else {
		paths.emplace_back("/usr/local/share/diasurgical/devilutionx/");
		paths.emplace_back("/usr/share/diasurgical/devilutionx/");
	}
#elif defined(NXDK)
	paths.emplace_back("D:\\");
#elif (defined(_WIN64) || defined(_WIN32)) && !defined(__UWP__) && !defined(NXDK)
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
			message.append(fmt::format("\n{:6d}. '{}'", i + 1, paths[i]));
		}
		LogVerbose("MPQ search paths:{}", message);
	}

	return paths;
}

} // namespace

void init_cleanup()
{
	if (gbIsMultiplayer && gbRunGame) {
		pfile_write_hero(/*writeGameData=*/false);
		sfile_write_stash();
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

void LoadCoreArchives()
{
	auto paths = GetMPQSearchPaths();

#if !defined(__ANDROID__) && !defined(__APPLE__) && !defined(__3DS__) && !defined(__SWITCH__)
	// Load devilutionx.mpq first to get the font file for error messages
	devilutionx_mpq = LoadMPQ(paths, "devilutionx.mpq");
#endif
	font_mpq = LoadMPQ(paths, "fonts.mpq"); // Extra fonts
}

void LoadLanguageArchive()
{
	lang_mpq = std::nullopt;

	string_view code = *sgOptions.Language.code;
	if (code != "en") {
		std::string langMpqName { code };
		langMpqName.append(".mpq");

		auto paths = GetMPQSearchPaths();
		lang_mpq = LoadMPQ(paths, langMpqName);
	}
}

void LoadGameArchives()
{
	auto paths = GetMPQSearchPaths();

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
	if (!HeadlessMode) {
		SDL_RWops *handle = OpenAsset("ui_art\\title.pcx");
		if (handle == nullptr) {
			LogError("{}", SDL_GetError());
			InsertCDDlg(_("diabdat.mpq or spawn.mpq"));
		}
		SDL_RWclose(handle);
	}

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
		diablo_quit(1);
	}
}

void init_create_window()
{
	if (!SpawnWindow(PROJECT_NAME))
		app_fatal(_("Unable to create main window"));
	dx_init();
	gbActive = true;
#ifndef USE_SDL1
	SDL_DisableScreenSaver();
#endif
}

void MainWndProc(const SDL_Event &event)
{
#ifndef USE_SDL1
	if (event.type != SDL_WINDOWEVENT)
		return;
	switch (event.window.event) {
	case SDL_WINDOWEVENT_HIDDEN:
		gbActive = false;
		break;
	case SDL_WINDOWEVENT_SHOWN:
		gbActive = false;
		force_redraw = 255;
		break;
	case SDL_WINDOWEVENT_EXPOSED:
		force_redraw = 255;
		break;
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		ReinitializeHardwareCursor();
		break;
	case SDL_WINDOWEVENT_LEAVE:
		sgbMouseDown = CLICK_NONE;
		LastMouseButtonAction = MouseActionType::None;
		force_redraw = 255;
		break;
	case SDL_WINDOWEVENT_CLOSE:
		diablo_quit(0);
		break;
	case SDL_WINDOWEVENT_FOCUS_LOST:
		diablo_focus_pause();
		break;
	case SDL_WINDOWEVENT_FOCUS_GAINED:
		diablo_focus_unpause();
		break;
	default:
		LogVerbose("Unhandled SDL_WINDOWEVENT event: ", event.window.event);
		break;
	}
#else
	if (event.type != SDL_ACTIVEEVENT)
		return;
	if ((event.active.state & SDL_APPINPUTFOCUS) != 0) {
		if (event.active.gain == 0)
			diablo_focus_pause();
		else
			diablo_focus_unpause();
	}
#endif
}

EventHandler SetEventHandler(EventHandler eventHandler)
{
	EventHandler previousHandler = CurrentEventHandler;
	CurrentEventHandler = eventHandler;
	return previousHandler;
}

} // namespace devilution
