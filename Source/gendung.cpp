#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

/*
address: 0x52BA00

level_frame_types specifies the CEL frame decoder type for each frame of the
level CEL (e.g. "levels/l1data/l1.cel"), Indexed by frame numbers starting
at 1.

The decoder type may be one of the following.
    0x0000 // cel.decodeType0
    0x1000 // cel.decodeType1
    0x2000 // cel.decodeType2
    0x3000 // cel.decodeType3
    0x4000 // cel.decodeType4
    0x5000 // cel.decodeType5
    0x6000 // cel.decodeType6
*/
WORD level_frame_types[MAXTILES];
/*
address: 0x52CA00

TODO: add docs.

PSX ref (SLPS-01416): 0x8011C14C
PSX def: int themeCount
alias: nthemes
*/
int themeCount;
/*
address: 0x52CA04

TODO: add docs.

PSX ref (easy-as-pie): 0x800EBA30
PSX def: unsigned char nTransTable[2049]
*/
BOOLEAN nTransTable[2049];
//int dword_52D204;
/*
address: 0x52D208

dMonster contains the NPC numbers of the map. The NPC number represents a
towner number (towners array index) in Tristram and a monster number
(monsters array index) in the dungeon.

PSX ref (SLPS-01416): 0x800E7A28
PSX def: struct map_info dung_map[112][112] // dMonster struct field
alias: npc_num_map
*/
int dMonster[MAXDUNX][MAXDUNY];
/*
address: 0x539608

dungeon contains the tile IDs of the map.

ref: graphics/l1/tiles/README.md#tileset-of-dungeon-layout-1

PSX ref (SLPS-01416): 0x800E40C4
PSX def: unsigned short dungeon[48][48]
alias: tile_id_map
*/
BYTE dungeon[DMAXX][DMAXY];
/*
address: 0x539C48

dObject contains the object numbers (objects array indices) of the map.

PSX ref (SLPS-01416): 0x800E7A28
PSX def: struct map_info dung_map[112][112] // dObject struct field
alias: object_num_map
*/
char dObject[MAXDUNX][MAXDUNY];
/*
address: 0x53CD48

TODO: add docs.
*/
BYTE *pSpeedCels;
/*
address: 0x53CD4C

nlevel_frames specifies the number of frames in the level cel (e.g.
"levels/l1data/l1.cel").
*/
int nlevel_frames;
/*
address: 0x53CD50

pdungeon contains a backup of the tile IDs of the map.

PSX ref (SLPS-01416): 0x800E52C4
PSX def: unsigned char pdungeon[40][40]
alias: tile_id_map_backup
*/
BYTE pdungeon[DMAXX][DMAXY];
/*
address: 0x53D390

dDead contains the dead numbers (deads array indices) and dead direction of
the map, encoded as specified by the pseudo-code below.

   dead_num  = dDead[x][y]&0x1F
   direction = dDead[x][y]>>5

PSX ref (SLPS-01416): 0x800E7A28
PSX def: struct map_info dung_map[112][112] // dDead struct field
alias: dead_map
*/
char dDead[MAXDUNX][MAXDUNY];
/*
address: 0x540490

dpiece_defs_map_1 specifies the dungeon piece information for a given
coordinate and block number.
*/
MICROS dpiece_defs_map_1[MAXDUNX * MAXDUNY];
/*
address: 0x5A2490

TODO: add docs.
*/
char dPreLight[MAXDUNX][MAXDUNY];
/*
address: 0x5A5590

TransVal specifies the current transparency category.

PSX ref (SLPS-01416): 0x8011C148
PSX def: char TransVal
alias: transparency_index
*/
char TransVal;
/*
address: 0x5A5594

TODO: add docs.

PSX ref (SLPS-01416): 0x8011C144
PSX def: int MicroTileLen
*/
int MicroTileLen;
/*
address: 0x5A5598

TODO: add docs.

PSX ref (easy-as-pie): 0x800EA3E8
PSX def: unsigned char dflags[40][40]
*/
char dflags[DMAXX][DMAXY];
/*
address: 0x5A5BD8

dPiece contains the piece IDs of each tile on the map.

PSX ref (SLPS-01416): 0x800E7A28
PSX def: struct map_info dung_map[112][112] // dPiece struct field
alias: piece_id_map
*/
int dPiece[MAXDUNX][MAXDUNY];
/*
address: 0x5B1FD8

TODO: add docs.
*/
char dLight[MAXDUNX][MAXDUNY];
/*
address: 0x5B50D8

setloadflag_2 specifies whether a single player quest DUN has been loaded.
alias: l2_l4_single_player_quest_dun_loaded
*/
int setloadflag_2;
/*
address: 0x5B50DC

TODO: add docs.
*/
int tile_defs[MAXTILES];
/*
address: 0x5B70DC

tile_defs specifies the tile definitions of the active dungeon type; (e.g.
levels/l1data/l1.til).

PSX ref (SLPS-01416): 0x800CECAC
PSX def: unsigned char pMegaTiles[2736]
*/
BYTE *pMegaTiles;
BYTE *pLevelPieces;
/*
address: 0x5B70E4

TODO: add docs.

PSX ref (SLPS-01416): 0x8011C108
PSX def: int gnDifficulty
*/
int gnDifficulty;
char block_lvid[2049];
//char byte_5B78EB;
/*
address: 0x5B78EC

dTransVal specifies the transparency at each coordinate of the map.

PSX ref (SLPS-01416): 0x800E7A28
PSX def: struct map_info dung_map[112][112] // dTransVal struct field
alias: transparency_map
*/
char dTransVal[MAXDUNX][MAXDUNY];
/*
address: 0x5BA9EC

TODO: add docs.

PSX ref (SLPS-01416): 0x800E7110
PSX def: unsigned char nTrapTable[2049]
*/
BOOLEAN nTrapTable[2049];
/*
address: 0x5BB1ED

leveltype specifies the active dungeon type of the current game.

PSX ref (SLPS-01416): 0x8011C10D
PSX def: unsigned char leveltype
alias: dtype
*/
BYTE leveltype;
/*
address: 0x5BB1EE

currlevel specifies the active dungeon level of the current game.

PSX ref (SLPS-01416): 0x8011C10C
PSX def: unsigned char currlevel
alias: dlvl
*/
BYTE currlevel;
/*
address: 0x5BB1F0

TransList specifies the active transparency indices.

PSX ref (SLPS-01416): 0x800E7928
PSX def: unsigned char TransList[256]
alias: transparency_active
*/
BOOLEAN TransList[256];
/*
address: 0x5BB2F0

TODO: add docs.

PSX ref (SLPS-01416): 0x800E6108
PSX def: unsigned char nSolidTable[2049]
*/
BOOLEAN nSolidTable[2049];
/*
address: 0x5BBAF4

level_frame_count specifies the CEL frame occurrence for each frame of the
level CEL (e.g. "levels/l1data/l1.cel").
*/
int level_frame_count[MAXTILES];
ScrollStruct ScrollInfo;
BYTE *pDungeonCels;
/*
address: 0x5BDB10

SpeedFrameTbl returns the frame number of the speed CEL, an in memory
decoding of level CEL frames, based on original frame number and light
index.

Note, given light index 0, the original frame number is returned.
alias: speed_cel_frame_num_from_light_index_frame_num
*/
int SpeedFrameTbl[128][16];
THEME_LOC themeLoc[MAXTHEMES];
/*
address: 0x5BFEF8

dPlayer contains the player numbers (players array indices) of the map.

PSX ref (SLPS-01416): 0x800E7A28
PSX def: struct map_info dung_map[112][112] // dPlayer struct field
alias: player_num_map
*/
char dPlayer[MAXDUNX][MAXDUNY];
int dword_5C2FF8;
int dword_5C2FFC;
int scr_pix_width;
int scr_pix_height;
/*
address: 0x5C3008

dArch contains the arch frame numbers of the map from the special tileset
(e.g. "levels/l1data/l1s.cel"). Note, the special tileset of Tristram (i.e.
"levels/towndata/towns.cel") contains trees rather than arches.
alias: arch_num_map
*/
char dArch[MAXDUNX][MAXDUNY];
/*
address: 0x5C6108

TODO: add docs.

PSX ref (SLPS-01416): 0x800E5904
PSX def: unsigned char nBlockTable[2049]
*/
BOOLEAN nBlockTable[2049];
BYTE *pSpecialCels;
char dFlags[MAXDUNX][MAXDUNY];
/*
address: 0x5C9A10

dItem contains the item numbers (items array indices) of the map.

PSX ref (SLPS-01416): 0x800E7A28
PSX def: struct map_info dung_map[112][112] // dItem struct field
alias: item_num_map
*/
char dItem[MAXDUNX][MAXDUNY];
/*
address: 0x5CCB10

setlvlnum specifies the active quest level of the current game.

PSX ref (SLPS-01416): 0x8011C10F
PSX def: unsigned char setlvlnum
alias: quest_lvl
*/
BYTE setlvlnum;
/*
address: 0x5CCB10

level_frame_sizes specifies the size of each frame of the level cel (e.g.
"levels/l1data/l1.cel"). Indexed by frame numbers starting at 1.
*/
int level_frame_sizes[MAXTILES];
/*
address: 0x5CEB14

TODO: add docs.

PSX ref (SLPS-01416): 0x800E690C
PSX def: unsigned char nMissileTable[2049]
*/
BOOLEAN nMissileTable[2049];
/*
address: 0x5CF318

pSetPiece_2 contains the contents of the single player quest DUN file.
alias: l2_l4_single_player_quest_dun
*/
char *pSetPiece_2;
/*
address: 0x5CF31C

TODO: add docs.

PSX ref (SLPS-01416): 0x8011C110
PSX def: unsigned char setlvltype
*/
char setlvltype;
/*
address: 0x5CF31D

TODO: add docs.

PSX ref (SLPS-01416): 0x8011C10E
PSX def: unsigned char setlevel
alias: is_set_level
*/
BOOLEAN setlevel;
/*
address: 0x5CF320

LvlViewY specifies the level viewpoint Y-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C130
PSX def: int LvlViewY
alias: lvl_view_y
*/
int LvlViewY;
/*
address: 0x5CF324

LvlViewX specifies the level viewpoint X-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C12C
PSX def: int LvlViewX
alias: lvl_view_x
*/
int LvlViewX;
/*
address: 0x5CF328

dmaxx specifies the maximum X-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C100
PSX def: int dmaxx
*/
int dmaxx;
/*
address: 0x5CF32C

dmaxy specifies the maximum Y-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C104
PSX def: int dmaxy
*/
int dmaxy;
/*
address: 0x5CF330

setpc_h specifies the height of the active set level of the map.

PSX ref (SLPS-01416): 0x8011C0F0
PSX def: int setpc_h
alias: set_level_height
*/
int setpc_h;
/*
address: 0x5CF334

setpc_w specifies the width of the active set level of the map.

PSX ref (SLPS-01416): 0x8011C0EC
PSX def: int setpc_w
alias: set_level_width
*/
int setpc_w;
/*
address: 0x5CF338

setpc_x specifies the active set level X-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C0E4
PSX def: int setpc_x
alias: set_xx
*/
int setpc_x;
/*
address: 0x5CF33C

ViewX specifies the player viewpoint X-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C114
PSX def: int ViewX
alias: view_x
*/
int ViewX;
/*
address: 0x5CF340

ViewY specifies the player viewpoint Y-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C118
PSX def: int ViewY
alias: view_y
*/
int ViewY;
/*
address: 0x5CF344

setpc_y specifies the active set level Y-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C0E8
PSX def: int setpc_y
alias: set_yy
*/
int setpc_y;
/*
address: 0x5CF350

dMissile contains the missile numbers (missiles array indices) of the map.

PSX ref (SLPS-01416): 0x800E7A28
PSX def: struct map_info dung_map[112][112] // dMissile struct field
alias: missile_num_map
*/
char dMissile[MAXDUNX][MAXDUNY];
/*
address: 0x5D2458

dminx specifies the minimum X-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C0F8
PSX def: int dminx
*/
int dminx;
/*
address: 0x5D245C

dminy specifies the minimum Y-coordinate of the map.

PSX ref (SLPS-01416): 0x8011C0FC
PSX def: int dminy
*/
int dminy;
/*
address: 0x5D2460

dpiece_defs_map_2 specifies the dungeon piece information for a given
coordinate and block number.
*/
MICROS dpiece_defs_map_2[MAXDUNX][MAXDUNY];

void FillSolidBlockTbls()
{
	BYTE bv;
	DWORD dwTiles;
	BYTE *pSBFile, *pTmp;
	int i;

	memset(nBlockTable, 0, sizeof(nBlockTable));
	memset(nSolidTable, 0, sizeof(nSolidTable));
	memset(nTransTable, 0, sizeof(nTransTable));
	memset(nMissileTable, 0, sizeof(nMissileTable));
	memset(nTrapTable, 0, sizeof(nTrapTable));

	switch (leveltype) {
	case DTYPE_TOWN:
		pSBFile = LoadFileInMem("Levels\\TownData\\Town.SOL", &dwTiles);
		break;
	case DTYPE_CATHEDRAL:
		pSBFile = LoadFileInMem("Levels\\L1Data\\L1.SOL", &dwTiles);
		break;
	case DTYPE_CATACOMBS:
		pSBFile = LoadFileInMem("Levels\\L2Data\\L2.SOL", &dwTiles);
		break;
	case DTYPE_CAVES:
		pSBFile = LoadFileInMem("Levels\\L3Data\\L3.SOL", &dwTiles);
		break;
	case DTYPE_HELL:
		pSBFile = LoadFileInMem("Levels\\L4Data\\L4.SOL", &dwTiles);
		break;
	default:
		app_fatal("FillSolidBlockTbls");
	}

	pTmp = pSBFile;

	for (i = 1; i <= dwTiles; i++) {
		bv = *pTmp++;
		if (bv & 1)
			nSolidTable[i] = 1;
		if (bv & 2)
			nBlockTable[i] = TRUE;
		if (bv & 4)
			nMissileTable[i] = TRUE;
		if (bv & 8)
			nTransTable[i] = TRUE;
		if (bv & 0x80)
			nTrapTable[i] = 1;
		block_lvid[i] = (bv & 0x70) >> 4; /* beta: (bv >> 4) & 7 */
	}

	mem_free_dbg(pSBFile);
}

void MakeSpeedCels()
{
	int i, j, x, y, mt, t, z;
	int total_frames, blocks, total_size, frameidx, blk_cnt, nDataSize;
	WORD m;
	BOOL blood_flag;
	DWORD *pFrameTable;
	MICROS *pMap;
	int l, k;
	BYTE width, pix;
	BYTE *src, *dst, *tbl;

	for (i = 0; i < MAXTILES; i++) {
		tile_defs[i] = i;
		level_frame_count[i] = 0;
		level_frame_types[i] = 0;
	}

	if (leveltype != DTYPE_HELL)
		blocks = 10;
	else
		blocks = 12;

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			for (i = 0; i < blocks; i++) {
				pMap = &dpiece_defs_map_2[x][y];
				mt = pMap->mt[i];
				if (mt) {
					level_frame_count[pMap->mt[i] & 0xFFF]++;
					level_frame_types[pMap->mt[i] & 0xFFF] = mt & 0x7000;
				}
			}
		}
	}

	pFrameTable = (DWORD *)pDungeonCels;
	nDataSize = pFrameTable[0];
	nlevel_frames = nDataSize & 0xFFFF;

	for (i = 1; i < nlevel_frames; i++) {
		z = i;
		nDataSize = pFrameTable[i + 1] - pFrameTable[i];
		level_frame_sizes[i] = nDataSize & 0xFFFF;
	}

	level_frame_sizes[0] = 0;

	if (leveltype == DTYPE_HELL) {
		for (i = 0; i < nlevel_frames; i++) {
			if (i == 0)
				level_frame_count[0] = 0;
			z = i;
			blood_flag = TRUE;
			if (level_frame_count[i] != 0) {
				if (level_frame_types[i] != 0x1000) {
					src = &pDungeonCels[pFrameTable[i]];
					for (j = level_frame_sizes[i]; j; j--) {
						pix = *src++;
						if (pix && pix < 32)
							blood_flag = FALSE;
					}
				} else {
					src = &pDungeonCels[pFrameTable[i]];
					for (k = 32; k; k--) {
						for (l = 32; l;) {
							width = *src++;
							if (!(width & 0x80)) {
								l -= width;
								while (width) {
									pix = *src++;
									if (pix && pix < 32)
										blood_flag = FALSE;
									width--;
								}
							} else {
								width = -(char)width;
								l -= width;
							}
						}
					}
				}
				if (!blood_flag)
					level_frame_count[i] = 0;
			}
		}
	}

	SortTiles(MAXTILES - 1);
	total_size = 0;
	total_frames = 0;

	if (light4flag) {
		while (total_size < 0x100000) {
			total_size += level_frame_sizes[total_frames] << 1;
			total_frames++;
		}
	} else {
		while (total_size < 0x100000) {
			total_size += (level_frame_sizes[total_frames] << 4) - (level_frame_sizes[total_frames] << 1);
			total_frames++;
		}
	}

	total_frames--;
	if (total_frames > 128)
		total_frames = 128;

	frameidx = 0; /* move into loop ? */

	if (light4flag)
		blk_cnt = 3;
	else
		blk_cnt = 15;

	for (i = 0; i < total_frames; i++) {
		z = tile_defs[i];
		SpeedFrameTbl[i][0] = z;
		if (level_frame_types[i] != 0x1000) {
			t = level_frame_sizes[i];
			for (j = 1; j < blk_cnt; j++) {
				SpeedFrameTbl[i][j] = frameidx;
				src = &pDungeonCels[pFrameTable[z]];
				dst = &pSpeedCels[frameidx];
				tbl = &pLightTbl[256 * j];
				for (k = t; k; k--) {
					*dst++ = tbl[*src++];
				}
				frameidx += t;
			}
		} else {
			for (j = 1; j < blk_cnt; j++) {
				SpeedFrameTbl[i][j] = frameidx;
				src = &pDungeonCels[pFrameTable[z]];
				dst = &pSpeedCels[frameidx];
				tbl = &pLightTbl[256 * j];
				for (k = 32; k; k--) {
					for (l = 32; l;) {
						width = *src++;
						*dst++ = width;
						if (!(width & 0x80)) {
							l -= width;
							while (width) {
								*dst++ = tbl[*src++];
								width--;
							}
						} else {
							width = -(char)width;
							l -= width;
						}
					}
				}
				frameidx += level_frame_sizes[i];
			}
		}
	}

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			if (dPiece[x][y]) {
				pMap = &dpiece_defs_map_2[x][y];
				for (i = 0; i < blocks; i++) {
					if (pMap->mt[i]) {
						for (m = 0; m < total_frames; m++) {
							if ((pMap->mt[i] & 0xFFF) == tile_defs[m]) {
								pMap->mt[i] = m + level_frame_types[m] + 0x8000;
								m = total_frames;
							}
						}
					}
				}
			}
		}
	}
}

void SortTiles(int frames)
{
	int i;
	BOOL doneflag;

	doneflag = FALSE;
	while (frames > 0 && !doneflag) {
		doneflag = TRUE;
		for (i = 0; i < frames; i++) {
			if (level_frame_count[i] < level_frame_count[i + 1]) {
				SwapTile(i, i + 1);
				doneflag = FALSE;
			}
		}
		frames--;
	}
}

void SwapTile(int f1, int f2)
{
	int swap;

	swap = level_frame_count[f1];
	level_frame_count[f1] = level_frame_count[f2];
	level_frame_count[f2] = swap;
	swap = tile_defs[f1];
	tile_defs[f1] = tile_defs[f2];
	tile_defs[f2] = swap;
	swap = level_frame_types[f1];
	level_frame_types[f1] = level_frame_types[f2];
	level_frame_types[f2] = swap;
	swap = level_frame_sizes[f1];
	level_frame_sizes[f1] = level_frame_sizes[f2];
	level_frame_sizes[f2] = swap;
}

int IsometricCoord(int x, int y)
{
	if (x < MAXDUNY - y)
		return (y + y * y + x * (x + 2 * y + 3)) / 2;

	x = MAXDUNX - x - 1;
	y = MAXDUNY - y - 1;
	return MAXDUNX * MAXDUNY - ((y + y * y + x * (x + 2 * y + 3)) / 2) - 1;
}

void SetSpeedCels()
{
	int x, y;

	for (x = 0; x < MAXDUNX; x++) {
		for (y = 0; y < MAXDUNY; y++) {
			dpiece_defs_map_1[IsometricCoord(x, y)] = dpiece_defs_map_2[x][y];
		}
	}
}

void SetDungeonMicros()
{
	int i, x, y, lv, blocks;
	WORD *pPiece;
	MICROS *pMap;

	if (leveltype != DTYPE_HELL) {
		MicroTileLen = 10;
		blocks = 10;
	} else {
		MicroTileLen = 12;
		blocks = 16;
	}

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			lv = dPiece[x][y];
			pMap = &dpiece_defs_map_2[x][y];
			if (lv) {
				lv--;
				if (leveltype != DTYPE_HELL)
					pPiece = (WORD *)&pLevelPieces[20 * lv];
				else
					pPiece = (WORD *)&pLevelPieces[32 * lv];
				for (i = 0; i < blocks; i++)
					pMap->mt[i] = pPiece[(i & 1) + blocks - 2 - (i & 0xE)];
			} else {
				for (i = 0; i < blocks; i++)
					pMap->mt[i] = 0;
			}
		}
	}

	MakeSpeedCels();
	SetSpeedCels();

	if (zoomflag) {
		scr_pix_width = SCREEN_WIDTH;
		scr_pix_height = VIEWPORT_HEIGHT;
		dword_5C2FF8 = SCREEN_WIDTH / 64;
		dword_5C2FFC = VIEWPORT_HEIGHT / 32;
	} else {
		scr_pix_width = ZOOM_WIDTH;
		scr_pix_height = ZOOM_HEIGHT;
		dword_5C2FF8 = ZOOM_WIDTH / 64;
		dword_5C2FFC = ZOOM_HEIGHT / 32;
	}
}

void DRLG_InitTrans()
{
	memset(dTransVal, 0, sizeof(dTransVal));
	memset(TransList, 0, sizeof(TransList));
	TransVal = 1;
}

void DRLG_MRectTrans(int x1, int y1, int x2, int y2)
{
	int i, j;

	x1 = 2 * x1 + 17;
	y1 = 2 * y1 + 17;
	x2 = 2 * x2 + 16;
	y2 = 2 * y2 + 16;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			dTransVal[i][j] = TransVal;
		}
	}

	TransVal++;
}

void DRLG_RectTrans(int x1, int y1, int x2, int y2)
{
	int i, j;

	for (j = y1; j <= y2; j++) {
		for (i = x1; i <= x2; i++) {
			dTransVal[i][j] = TransVal;
		}
	}
	TransVal++;
}

void DRLG_CopyTrans(int sx, int sy, int dx, int dy)
{
	dTransVal[dx][dy] = dTransVal[sx][sy];
}

#ifndef SPAWN
void DRLG_ListTrans(int num, BYTE *List)
{
	int i;
	BYTE x1, x2, y1, y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
	}
}

void DRLG_AreaTrans(int num, BYTE *List)
{
	int i;
	BYTE x1, x2, y1, y2;

	for (i = 0; i < num; i++) {
		x1 = *List++;
		y1 = *List++;
		x2 = *List++;
		y2 = *List++;
		DRLG_RectTrans(x1, y1, x2, y2);
		TransVal--;
	}
	TransVal++;
}
#endif

void DRLG_InitSetPC()
{
	setpc_x = 0;
	setpc_y = 0;
	setpc_w = 0;
	setpc_h = 0;
}

void DRLG_SetPC()
{
	int i, j, x, y, w, h;

	w = 2 * setpc_w;
	h = 2 * setpc_h;
	x = 2 * setpc_x + 16;
	y = 2 * setpc_y + 16;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			dFlags[i + x][j + y] |= 8;
		}
	}
}

#ifndef SPAWN
void Make_SetPC(int x, int y, int w, int h)
{
	int i, j, dx, dy, dh, dw;

	dw = 2 * w;
	dh = 2 * h;
	dx = 2 * x + 16;
	dy = 2 * y + 16;

	for (j = 0; j < dh; j++) {
		for (i = 0; i < dw; i++) {
			dFlags[i + dx][j + dy] |= 8;
		}
	}
}

BOOL DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height)
{
	int ii, xx, yy;
	int xSmallest, ySmallest;
	int xArray[20], yArray[20];
	int xCount, yCount;
	BOOL yFlag, xFlag;

	yFlag = TRUE;
	xFlag = TRUE;
	xCount = 0;
	yCount = 0;

	// BUGFIX: change '&&' to '||'
	if (x > DMAXX - maxSize && y > DMAXY - maxSize) {
		return FALSE;
	}
	if (!SkipThemeRoom(x, y)) {
		return FALSE;
	}

	memset(xArray, 0, sizeof(xArray));
	memset(yArray, 0, sizeof(yArray));

	for (ii = 0; ii < maxSize; ii++) {
		if (xFlag) {
			for (xx = x; xx < x + maxSize; xx++) {
				if (dungeon[xx][y + ii] != floor) {
					if (xx >= minSize) {
						break;
					}
					xFlag = FALSE;
				} else {
					xCount++;
				}
			}
			if (xFlag) {
				xArray[ii] = xCount;
				xCount = 0;
			}
		}
		if (yFlag) {
			for (yy = y; yy < y + maxSize; yy++) {
				if (dungeon[x + ii][yy] != floor) {
					if (yy >= minSize) {
						break;
					}
					yFlag = FALSE;
				} else {
					yCount++;
				}
			}
			if (yFlag) {
				yArray[ii] = yCount;
				yCount = 0;
			}
		}
	}

	for (ii = 0; ii < minSize; ii++) {
		if (xArray[ii] < minSize || yArray[ii] < minSize) {
			return FALSE;
		}
	}

	xSmallest = xArray[0];
	ySmallest = yArray[0];

	for (ii = 0; ii < maxSize; ii++) {
		if (xArray[ii] < minSize || yArray[ii] < minSize) {
			break;
		}
		if (xArray[ii] < xSmallest) {
			xSmallest = xArray[ii];
		}
		if (yArray[ii] < ySmallest) {
			ySmallest = yArray[ii];
		}
	}

	*width = xSmallest - 2;
	*height = ySmallest - 2;
	return TRUE;
}

void DRLG_CreateThemeRoom(int themeIndex)
{
	int xx, yy;

	for (yy = themeLoc[themeIndex].y; yy < themeLoc[themeIndex].y + themeLoc[themeIndex].height; yy++) {
		for (xx = themeLoc[themeIndex].x; xx < themeLoc[themeIndex].x + themeLoc[themeIndex].width; xx++) {
			if (leveltype == DTYPE_CATACOMBS) {
				if (yy == themeLoc[themeIndex].y
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width
				    || yy == themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width) {
					dungeon[xx][yy] = 2;
				} else if (xx == themeLoc[themeIndex].x
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height
				    || xx == themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height) {
					dungeon[xx][yy] = 1;
				} else {
					dungeon[xx][yy] = 3;
				}
			}
			if (leveltype == DTYPE_CAVES) {
				if (yy == themeLoc[themeIndex].y
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width
				    || yy == themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width) {
					dungeon[xx][yy] = 134;
				} else if (xx == themeLoc[themeIndex].x
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height
				    || xx == themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height) {
					dungeon[xx][yy] = 137;
				} else {
					dungeon[xx][yy] = 7;
				}
			}
			if (leveltype == DTYPE_HELL) {
				if (yy == themeLoc[themeIndex].y
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width
				    || yy == themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1
				        && xx >= themeLoc[themeIndex].x
				        && xx <= themeLoc[themeIndex].x + themeLoc[themeIndex].width) {
					dungeon[xx][yy] = 2;
				} else if (xx == themeLoc[themeIndex].x
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height
				    || xx == themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1
				        && yy >= themeLoc[themeIndex].y
				        && yy <= themeLoc[themeIndex].y + themeLoc[themeIndex].height) {
					dungeon[xx][yy] = 1;
				} else {
					dungeon[xx][yy] = 6;
				}
			}
		}
	}

	if (leveltype == DTYPE_CATACOMBS) {
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y] = 8;
		dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y] = 7;
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 9;
		dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 6;
	}
	if (leveltype == DTYPE_CAVES) {
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y] = 150;
		dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y] = 151;
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 152;
		dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 138;
	}
	if (leveltype == DTYPE_HELL) {
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y] = 9;
		dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y] = 16;
		dungeon[themeLoc[themeIndex].x][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 15;
		dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 12;
	}

	if (leveltype == DTYPE_CATACOMBS) {
		switch (random(0, 2)) {
		case 0:
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2] = 4;
			break;
		case 1:
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 5;
			break;
		}
	}
	if (leveltype == DTYPE_CAVES) {
		switch (random(0, 2)) {
		case 0:
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2] = 147;
			break;
		case 1:
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 146;
			break;
		}
	}
	if (leveltype == DTYPE_HELL) {
		switch (random(0, 2)) {
		case 0:
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2 - 1] = 53;
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2] = 6;
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2 + 1] = 52;
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width - 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height / 2 - 1] = 54;
			break;
		case 1:
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2 - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 57;
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 6;
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2 + 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 1] = 56;
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 2] = 59;
			dungeon[themeLoc[themeIndex].x + themeLoc[themeIndex].width / 2 - 1][themeLoc[themeIndex].y + themeLoc[themeIndex].height - 2] = 58;
			break;
		}
	}
}

void DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize)
{
	int i, j;
	int themeW, themeH;
	int rv2, min, max;

	themeCount = 0;
	memset(themeLoc, 0, sizeof(*themeLoc));
	for (j = 0; j < DMAXY; j++) {
		for (i = 0; i < DMAXX; i++) {
			if (dungeon[i][j] == floor && !random(0, freq) && DRLG_WillThemeRoomFit(floor, i, j, minSize, maxSize, &themeW, &themeH)) {
				if (rndSize) {
					min = minSize - 2;
					max = maxSize - 2;
					rv2 = min + random(0, random(0, themeW - min + 1));
					if (rv2 >= min && rv2 <= max)
						themeW = rv2;
					else
						themeW = min;
					rv2 = min + random(0, random(0, themeH - min + 1));
					if (rv2 >= min && rv2 <= max)
						themeH = rv2;
					else
						themeH = min;
				}
				themeLoc[themeCount].x = i + 1;
				themeLoc[themeCount].y = j + 1;
				themeLoc[themeCount].width = themeW;
				themeLoc[themeCount].height = themeH;
				if (leveltype == DTYPE_CAVES)
					DRLG_RectTrans(2 * i + 20, 2 * j + 20, 2 * (i + themeW) + 15, 2 * (j + themeH) + 15);
				else
					DRLG_MRectTrans(i + 1, j + 1, i + themeW, j + themeH);
				themeLoc[themeCount].ttval = TransVal - 1;
				DRLG_CreateThemeRoom(themeCount);
				themeCount++;
			}
		}
	}
}
#endif

void DRLG_HoldThemeRooms()
{
	int i, x, y, xx, yy;

	for (i = 0; i < themeCount; i++) {
		for (y = themeLoc[i].y; y < themeLoc[i].y + themeLoc[i].height - 1; y++) {
			for (x = themeLoc[i].x; x < themeLoc[i].x + themeLoc[i].width - 1; x++) {
				xx = 2 * x + 16;
				yy = 2 * y + 16;
				dFlags[xx][yy] |= BFLAG_POPULATED;
				dFlags[xx + 1][yy] |= BFLAG_POPULATED;
				dFlags[xx][yy + 1] |= BFLAG_POPULATED;
				dFlags[xx + 1][yy + 1] |= BFLAG_POPULATED;
			}
		}
	}
}

BOOL SkipThemeRoom(int x, int y)
{
	int i;

	for (i = 0; i < themeCount; i++) {
		if (x >= themeLoc[i].x - 2 && x <= themeLoc[i].x + themeLoc[i].width + 2
		    && y >= themeLoc[i].y - 2 && y <= themeLoc[i].y + themeLoc[i].height + 2)
			return 0;
	}

	return 1;
}

void InitLevels()
{
	if (!leveldebug) {
		currlevel = 0;
		leveltype = DTYPE_TOWN;
		setlevel = FALSE;
	}
}

DEVILUTION_END_NAMESPACE

