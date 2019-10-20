//HEADER_GOES_HERE
#ifndef __TOWN_H__
#define __TOWN_H__

void SetTownMicros();
void T_FillSector(BYTE *P3Tiles, BYTE *pSector, int xi, int yi, int w, int h);
void T_FillTile(BYTE *P3Tiles, int xx, int yy, int t);
void T_Pass3();
void CreateTown(int entry);

#endif /* __TOWN_H__ */
