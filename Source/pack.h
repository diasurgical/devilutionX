//HEADER_GOES_HERE
#ifndef __PACK_H__
#define __PACK_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

void PackPlayer(PkPlayerStruct *pPack, int pnum, BOOL manashield);
void VerifyGoldSeeds(PlayerStruct *pPlayer);
void UnPackPlayer(PkPlayerStruct *pPack, int pnum, BOOL killok);

/* rdata */
#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __PACK_H__ */
