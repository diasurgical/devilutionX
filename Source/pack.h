//HEADER_GOES_HERE
#ifndef __PACK_H__
#define __PACK_H__

void __fastcall PackPlayer(PkPlayerStruct *pPack, int pnum, BOOL manashield);
void __fastcall PackItem(PkItemStruct *id, ItemStruct *is);
void __fastcall VerifyGoldSeeds(PlayerStruct *pPlayer);
void __fastcall UnPackPlayer(PkPlayerStruct *pPack, int pnum, BOOL killok);
void __fastcall UnPackItem(PkItemStruct *is, ItemStruct *id);

/* rdata */

#endif /* __PACK_H__ */
