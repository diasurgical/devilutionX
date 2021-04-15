/**
 * @file sync.h
 *
 * Interface of functionality for syncing game state with other players.
 */
#pragma once

namespace devilution {

Uint32 sync_all_monsters(const Uint8 *pbBuf, Uint32 dwMaxLen);
Uint32 sync_update(int pnum, const Uint8 *pbBuf);
void sync_init();

}
