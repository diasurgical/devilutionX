/**
 * @file trn.hpp
 *
 * Contains most of trn logic
 */
#pragma once

#include <cstdint>
#include <optional>

#include "player.h"

namespace devilution {

uint8_t *GetInfravisionTRN();
uint8_t *GetStoneTRN();
uint8_t *GetPauseTRN();
std::optional<std::array<uint8_t, 256>> GetClassTRN(Player &player);
std::optional<std::array<uint8_t, 256>> GetPlayerGraphicTRN(const char *pszName);

} // namespace devilution
