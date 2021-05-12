/**
 * @file sync.h
 *
 * Interface of functionality for syncing game state with other players.
 */
#pragma once

#include <cstdint>

#include "engine.h"

namespace devilution {

uint32_t sync_all_monsters(const byte *pbBuf, uint32_t dwMaxLen);
uint32_t sync_update(int pnum, const byte *pbBuf);
void sync_init();

} // namespace devilution
