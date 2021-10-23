#pragma once

#include <cstdint>

#include <SDL.h>

#include "utils/mpq.hpp"

namespace devilution {

SDL_RWops *SDL_RWops_FromMpqFile(MpqArchive &mpqArchive, uint32_t fileNumber, const char *filename, bool threadsafe);

} // namespace devilution
