/**
 * @file init.h
 *
 * Interface of routines for initializing the environment, disable screen saver, load MPQ.
 */
#pragma once

#include "utils/attributes.h"
#include "utils/stdcompat/optional.hpp"

#ifdef UNPACKED_MPQS
#include <string>
#else
#include "mpq/mpq_reader.hpp"
#endif

#include <SDL.h>

namespace devilution {

extern bool gbActive;
extern DVL_API_FOR_TEST bool gbIsSpawn;
extern DVL_API_FOR_TEST bool gbIsHellfire;
extern DVL_API_FOR_TEST bool gbVanilla;
extern bool forceHellfire;

#ifdef UNPACKED_MPQS
extern DVL_API_FOR_TEST std::optional<std::string> spawn_data_path;
extern DVL_API_FOR_TEST std::optional<std::string> diabdat_data_path;
extern std::optional<std::string> hellfire_data_path;
extern std::optional<std::string> font_data_path;
extern std::optional<std::string> lang_data_path;
#else
/** A handle to the spawn.mpq archive. */
extern DVL_API_FOR_TEST std::optional<MpqArchive> spawn_mpq;
/** A handle to the diabdat.mpq archive. */
extern DVL_API_FOR_TEST std::optional<MpqArchive> diabdat_mpq;
/** A handle to an hellfire.mpq archive. */
extern std::optional<MpqArchive> hellfire_mpq;
extern std::optional<MpqArchive> hfmonk_mpq;
extern std::optional<MpqArchive> hfbard_mpq;
extern std::optional<MpqArchive> hfbarb_mpq;
extern std::optional<MpqArchive> hfmusic_mpq;
extern std::optional<MpqArchive> hfvoice_mpq;
extern std::optional<MpqArchive> font_mpq;
extern std::optional<MpqArchive> lang_mpq;
extern std::optional<MpqArchive> devilutionx_mpq;
#endif

extern char font_mpq_version[];

inline bool HaveSpawn()
{
#ifdef UNPACKED_MPQS
	return bool(spawn_data_path);
#else
	return bool(spawn_mpq);
#endif
}

inline bool HaveDiabdat()
{
#ifdef UNPACKED_MPQS
	return bool(diabdat_data_path);
#else
	return bool(diabdat_mpq);
#endif
}

inline bool HaveHellfire()
{
#ifdef UNPACKED_MPQS
	return bool(hellfire_data_path);
#else
	return bool(hellfire_mpq);
#endif
}

inline bool HaveExtraFonts()
{
#ifdef UNPACKED_MPQS
	return bool(font_data_path);
#else
	return bool(font_mpq);
#endif
}

void init_cleanup();
void LoadCoreArchives();
void LoadLanguageArchive();
void LoadGameArchives();
void init_create_window();
void MainWndProc(const SDL_Event &event);

} // namespace devilution
