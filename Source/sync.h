/**
 * @file sync.h
 *
 * Interface of functionality for syncing game state with other players.
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace devilution {

uint32_t sync_all_monsters(std::byte *pbBuf, uint32_t dwMaxLen);
uint32_t OnSyncData(const TCmd *pCmd, size_t pnum);
void sync_init();

} // namespace devilution
