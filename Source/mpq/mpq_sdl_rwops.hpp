#pragma once

#include <cstdint>
#include <string_view>

#include <SDL.h>

#include "mpq/mpq_reader.hpp"

namespace devilution {

SDL_RWops *SDL_RWops_FromMpqFile(MpqArchive &mpqArchive, uint32_t fileNumber, std::string_view filename, bool threadsafe);

} // namespace devilution
