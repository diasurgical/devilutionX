/**
 * @file setmaps.cpp
 *
 * Interface of functionality for the special quest dungeons.
 */
#ifndef __SETMAPS_H__
#define __SETMAPS_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

int ObjIndex(int x, int y);
void AddSKingObjs();
void AddSChamObjs();
void AddVileObjs();
void DRLG_SetMapTrans(const char *sFileName);
void LoadSetMap();

/* rdata */
extern BYTE SkelKingTrans1[8];
extern BYTE SkelKingTrans2[8];
extern BYTE SkelKingTrans3[20];
extern BYTE SkelKingTrans4[28];
extern BYTE SkelChamTrans1[20];
extern BYTE SkelChamTrans2[8];
extern BYTE SkelChamTrans3[36];
extern const char *const quest_level_names[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __SETMAPS_H__ */
