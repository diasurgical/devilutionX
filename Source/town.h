/**
 * @file town.h
 *
 * Interface of functionality for rendering the town, towners and calling other render routines.
 */
#ifndef __TOWN_H__
#define __TOWN_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

void T_FillSector(BYTE *P3Tiles, BYTE *pSector, int xi, int yi, int w, int h);
void T_FillTile(BYTE *P3Tiles, int xx, int yy, int t);
void T_Pass3();
void town_4751C6();
void town_475379();
void town_47552C();
void town_475595();
void CreateTown(int entry);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __TOWN_H__ */
