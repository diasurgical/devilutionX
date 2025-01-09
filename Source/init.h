/**
 * @file init.h
 *
 * Interface of routines for initializing the environment, disable screen saver, load MPQ.
 */
#pragma once

#include <optional>

#include "engine/assets.hpp"
#include "utils/attributes.h"

#ifdef UNPACKED_MPQS
#include <string>
#else
#include "mpq/mpq_reader.hpp"
#endif

namespace devilution {

/** True if the game is the current active window */
extern bool gbActive;

#ifdef UNPACKED_MPQS
bool AreExtraFontsOutOfDate(const std::string &path);
#else
bool AreExtraFontsOutOfDate(MpqArchive &archive);
#endif

inline bool AreExtraFontsOutOfDate()
{
#ifdef UNPACKED_MPQS
	return font_data_path && AreExtraFontsOutOfDate(*font_data_path);
#else
	return font_mpq && AreExtraFontsOutOfDate(*font_mpq);
#endif
}

#ifndef UNPACKED_MPQS
bool IsDevilutionXMpqOutOfDate(MpqArchive &archive);
#endif

inline bool IsDevilutionXMpqOutOfDate()
{
#ifdef UNPACKED_MPQS
	return false;
#else
	return devilutionx_mpq.has_value() && IsDevilutionXMpqOutOfDate(*devilutionx_mpq);
#endif
}

void init_cleanup();
void init_create_window();
void MainWndProc(const SDL_Event &event);

} // namespace devilution
