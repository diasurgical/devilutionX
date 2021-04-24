/**
 * @file sync.h
 *
 * Interface of functionality for syncing game state with other players.
 */
#pragma once

#include <cstdint>

#include "miniwin/miniwin.h"

namespace devilution {

uint32_t sync_all_monsters(const BYTE *pbBuf, uint32_t dwMaxLen);
uint32_t sync_update(int pnum, const BYTE *pbBuf);
void sync_init();

} // namespace devilution
