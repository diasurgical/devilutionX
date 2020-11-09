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

DWORD sync_all_monsters(const BYTE *pbBuf, DWORD dwMaxLen);
DWORD sync_update(int pnum, const BYTE *pbBuf);
void sync_monster(int pnum, const TSyncMonster *p);
void sync_init();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __SYNC_H__ */
