/**
 * @file items/validation.h
 *
 * Interface of functions for validation of player and item data.
 */
#pragma once

#include <cstdint>

namespace devilution {

bool IsCreationFlagComboValid(uint16_t iCreateInfo);
bool IsTownItemValid(uint16_t iCreateInfo, uint8_t maxCharacterLevel);
bool IsUniqueMonsterItemValid(uint16_t iCreateInfo, uint32_t dwBuff);
bool IsDungeonItemValid(uint16_t iCreateInfo, uint32_t dwBuff);

} // namespace devilution
