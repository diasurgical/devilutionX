/**
 * @file sync.h
 *
 * Interface of functionality for syncing game state with other players.
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace devilution {

size_t sync_all_monsters(std::byte *pbBuf, size_t dwMaxLen);
uint32_t OnSyncData(const TCmd *pCmd, const Player &player);
void sync_init();

} // namespace devilution
