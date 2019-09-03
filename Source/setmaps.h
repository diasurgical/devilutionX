//HEADER_GOES_HERE
#ifndef __SETMAPS_H__
#define __SETMAPS_H__

int ObjIndex(int x, int y);
void AddSKingObjs();
void AddSChamObjs();
void AddVileObjs();
void DRLG_SetMapTrans(char *sFileName);
void LoadSetMap();

/* rdata */
extern BYTE SkelKingTrans1[8];
extern BYTE SkelKingTrans2[8];
extern BYTE SkelKingTrans3[20];
extern BYTE SkelKingTrans4[28];
extern BYTE SkelChamTrans1[20];
extern BYTE SkelChamTrans2[8];
extern BYTE SkelChamTrans3[36];
extern char *quest_level_names[];

#endif /* __SETMAPS_H__ */
