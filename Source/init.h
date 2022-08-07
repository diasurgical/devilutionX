/**
 * @file init.h
 *
 * Interface of routines for initializing the environment, disable screen saver, load MPQ.
 */
#pragma once

#include "miniwin/misc_msg.h"
#include "mpq/mpq_reader.hpp"
#include "utils/attributes.h"

namespace devilution {

extern bool gbActive;
extern std::optional<MpqArchive> hellfire_mpq;
extern EventHandler CurrentEventHandler;
extern DVL_API_FOR_TEST std::optional<MpqArchive> spawn_mpq;
extern DVL_API_FOR_TEST std::optional<MpqArchive> diabdat_mpq;
extern DVL_API_FOR_TEST bool gbIsSpawn;
extern DVL_API_FOR_TEST bool gbIsHellfire;
extern DVL_API_FOR_TEST bool gbVanilla;
extern bool forceHellfire;
extern std::optional<MpqArchive> hfmonk_mpq;
extern std::optional<MpqArchive> hfbard_mpq;
extern std::optional<MpqArchive> hfbarb_mpq;
extern std::optional<MpqArchive> hfmusic_mpq;
extern std::optional<MpqArchive> hfvoice_mpq;
extern std::optional<MpqArchive> font_mpq;
extern std::optional<MpqArchive> lang_mpq;
extern std::optional<MpqArchive> devilutionx_mpq;

void init_cleanup();
void LoadCoreArchives();
void LoadLanguageArchive();
void LoadGameArchives();
void init_create_window();
void MainWndProc(const SDL_Event &event);
EventHandler SetEventHandler(EventHandler NewProc);

} // namespace devilution
