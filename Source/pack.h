/**
 * @file pack.h
 *
 * Interface of functions for minifying player data structure.
 */
#ifndef __PACK_H__
#define __PACK_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

void PackPlayer(PkPlayerStruct *pPack, int pnum, BOOL manashield);
void UnPackPlayer(PkPlayerStruct *pPack, int pnum, BOOL killok);
void PackItem(PkItemStruct *id, ItemStruct *is);
void UnPackItem(PkItemStruct *is, ItemStruct *id);

/* rdata */
#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __PACK_H__ */
