/**
 * @file sync.h
 *
 * Interface of functionality for syncing game state with other players.
 */
#ifndef __SYNC_H__
#define __SYNC_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

Uint32 sync_all_monsters(const Uint8 *pbBuf, Uint32 dwMaxLen);
Uint32 sync_update(int pnum, const Uint8 *pbBuf);
void sync_init();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __SYNC_H__ */
