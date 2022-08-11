/**
 * @file trn.hpp
 *
 * Contains most of trn logic
 */
#pragma once

#include "player.h"
#include "utils/stdcompat/optional.hpp"

namespace devilution {

uint8_t *GetInfravisionTRN();
uint8_t *GetStoneTRN();
uint8_t *GetPauseTRN();
std::optional<std::array<uint8_t, 256>> GetClassTRN(Player &player);

} // namespace devilution
