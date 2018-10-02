//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
short level_frame_types[2048];
int themeCount;
char nTransTable[2049];
//int dword_52D204;
int dMonster[MAXDUNX][MAXDUNY];
char dungeon[40][40];
char dObject[MAXDUNX][MAXDUNY];
void *pSpeedCels;
int nlevel_frames; // weak
char pdungeon[40][40];
char dDead[MAXDUNX][MAXDUNY];
short dpiece_defs_map_1[16][MAXDUNX][MAXDUNY];
char dTransVal2[MAXDUNX][MAXDUNY];
char TransVal; // weak
int dword_5A5594;
char dflags[40][40];
int dPiece[MAXDUNX][MAXDUNY];
char dTransVal[MAXDUNX][MAXDUNY];
int setloadflag_2; // weak
int tile_defs[2048];
void *pMegaTiles;
void *pLevelPieces;
int gnDifficulty; // idb
char block_lvid[2049];
//char byte_5B78EB;
char dung_map[MAXDUNX][MAXDUNY];
char nTrapTable[2049];
char leveltype; // weak
unsigned char currlevel; // idb
char TransList[256];
UCHAR nSolidTable[2049];
int level_frame_count[2048];
ScrollStruct ScrollInfo;
void *pDungeonCels;
int speed_cel_frame_num_from_light_index_frame_num[16][128];
THEME_LOC themeLoc[MAXTHEMES];
char dPlayer[MAXDUNX][MAXDUNY];
int dword_5C2FF8; // weak
int dword_5C2FFC; // weak
int scr_pix_width; // weak
int scr_pix_height; // weak
char dArch[MAXDUNX][MAXDUNY];
char nBlockTable[2049];
void *level_special_cel;
char dFlags[MAXDUNX][MAXDUNY];
char dItem[MAXDUNX][MAXDUNY];
char setlvlnum; // weak
int level_frame_sizes[2048];
char nMissileTable[2049];
char *pSetPiece_2;
char setlvltype; // weak
char setlevel; // weak
int LvlViewY; // weak
int LvlViewX; // weak
int dmaxx; // weak
int dmaxy; // weak
int setpc_h; // weak
int setpc_w; // weak
int setpc_x; // idb
int ViewX; // idb
int ViewY; // idb
int setpc_y; // idb
char dMissile[MAXDUNX][MAXDUNY];
int dminx; // weak
int dminy; // weak
short dpiece_defs_map_2[16][MAXDUNX][MAXDUNY];
#endif

void __cdecl FillSolidBlockTbls()
{
	unsigned char *v0; // eax
	char *v1; // ecx
	unsigned char *v2; // esi
	int v3; // edx
	unsigned char v4; // bl
	int size; // [esp+8h] [ebp-4h]

	memset(nBlockTable, 0, sizeof(nBlockTable));
	memset(nSolidTable, 0, sizeof(nSolidTable));
	memset(nTransTable, 0, sizeof(nTransTable));
	memset(nMissileTable, 0, sizeof(nMissileTable));
	memset(nTrapTable, 0, sizeof(nTrapTable));
	if ( leveltype != DTYPE_TOWN )
	{
		switch ( leveltype )
		{
			case DTYPE_CATHEDRAL:
				v1 = "Levels\\L1Data\\L1.SOL";
				break;
			case DTYPE_CATACOMBS:
				v1 = "Levels\\L2Data\\L2.SOL";
				break;
			case DTYPE_CAVES:
				v1 = "Levels\\L3Data\\L3.SOL";
				break;
			case DTYPE_HELL:
				v1 = "Levels\\L4Data\\L4.SOL";
				break;
			default:
				TermMsg("FillSolidBlockTbls");
				// v0 = (unsigned char *)size; /* check error */
				goto LABEL_13;
		}
	}
	else
	{
		v1 = "Levels\\TownData\\Town.SOL";
	}
	v0 = LoadFileInMem(v1, &size);
LABEL_13:
	v2 = v0;
	if ( (unsigned int)size >= 1 )
	{
		v3 = 0;
		do
		{
			v4 = *v2++;
			if ( v4 & 1 )
				nSolidTable[v3 + 1] = 1;
			if ( v4 & 2 )
				nBlockTable[v3 + 1] = 1;
			if ( v4 & 4 )
				nMissileTable[v3 + 1] = 1;
			if ( v4 & 8 )
				nTransTable[v3 + 1] = 1;
			if ( (v4 & 0x80u) != 0 )
				nTrapTable[v3 + 1] = 1;
			block_lvid[v3++ + 1] = (v4 >> 4) & 7;
		}
		while ( v3 + 1 <= (unsigned int)size );
	}
	mem_free_dbg(v0);
}
// 5BB1ED: using guessed type char leveltype;

void __cdecl gendung_418D91()
{
	signed int v0; // edx
	short (*v1)[112][112]; // edi
	short (*v2)[112][112]; // esi
	signed int v3; // ebx
	int i; // edx
	short v5; // ax
	int v6; // ecx
	signed int v7; // edx
	int v8; // eax
	int v9; // edi
	char *v10; // esi
	int j; // ecx
	unsigned char v12; // al
	unsigned char *v13; // esi
	int v14; // ecx
	signed int v15; // edx
	int v16; // eax
	int v17; // ecx
	unsigned char v18; // al
	signed int v19; // ecx
	int v20; // edi
	int v21; // edx
	int v22; // edi
	int v23; // eax
	int v24; // eax
	bool v25; // zf
	int v26; // edx
	char *v27; // esi
	char *v28; // edi
	int k; // ecx
	char *v33; // esi
	char *v34; // edi
	int v36; // ecx
	signed int v37; // edx
	int v38; // eax
	int v39; // ecx
	short (*v42)[112][112]; // esi
	short v43; // ax
	unsigned short v44; // dx
	short v45; // ax
	int v46; // [esp-4h] [ebp-38h]
	int v47; // [esp-4h] [ebp-38h]
	int v48; // [esp+Ch] [ebp-28h]
	int (*v49)[128]; // [esp+10h] [ebp-24h]
	int (*v50)[112]; // [esp+10h] [ebp-24h]
	int v51; // [esp+14h] [ebp-20h]
	short (*v52)[112][112]; // [esp+14h] [ebp-20h]
	signed int v53; // [esp+18h] [ebp-1Ch]
	int v54; // [esp+18h] [ebp-1Ch]
	short (*v55)[112][112]; // [esp+18h] [ebp-1Ch]
	int v56; // [esp+1Ch] [ebp-18h]
	int (*v57)[112]; // [esp+1Ch] [ebp-18h]
	signed int v58; // [esp+20h] [ebp-14h]
	int v59; // [esp+20h] [ebp-14h]
	int v60; // [esp+24h] [ebp-10h]
	signed int v61; // [esp+24h] [ebp-10h]
	int v62; // [esp+28h] [ebp-Ch]
	int v63; // [esp+2Ch] [ebp-8h]
	signed int v64; // [esp+30h] [ebp-4h]
	signed int v65; // [esp+30h] [ebp-4h]
	int _EAX;
	char *_EBX;

	v0 = 0;
	memset(level_frame_types, 0, sizeof(level_frame_types));
	memset(level_frame_count, 0, sizeof(level_frame_count));
	do
	{
		tile_defs[v0] = v0;
		++v0;
	}
	while ( v0 < 2048 );
	v1 = dpiece_defs_map_2;
	v48 = 2 * (leveltype == DTYPE_HELL) + 10;
	do
	{
		v2 = v1;
		v3 = 112;
		do
		{
			for ( i = 0; i < v48; ++i )
			{
				v5 = (*v2)[0][i];
				if ( (*v2)[0][i] )
				{
					v6 = v5 & 0xFFF;
					++level_frame_count[v6];
					level_frame_types[v6] = v5 & 0x7000;
				}
			}
			v2 = (short (*)[112][112])((char *)v2 + 3584);
			--v3;
		}
		while ( v3 );
		v1 = (short (*)[112][112])((char *)v1 + 32);
	}
	while ( (signed int)v1 < (signed int)dpiece_defs_map_2[0][16] ); /* check */
	v7 = 1;
	nlevel_frames = *(_DWORD *)pDungeonCels & 0xFFFF;
	v8 = nlevel_frames;
	if ( nlevel_frames > 1 )
	{
		do
		{
			level_frame_sizes[v7] = (*((_DWORD *)pDungeonCels + v7 + 1) - *((_DWORD *)pDungeonCels + v7)) & 0xFFFF;
			v8 = nlevel_frames;
			++v7;
		}
		while ( v7 < nlevel_frames );
	}
	v9 = 0;
	level_frame_sizes[0] = 0;
	if ( leveltype == DTYPE_HELL && v8 > 0 )
	{
		do
		{
			if ( !v9 )
				level_frame_count[0] = 0;
			v53 = 1;
			if ( level_frame_count[v9] )
			{
				if ( level_frame_types[v9] == 4096 )
				{
					v13 = (unsigned char *)pDungeonCels + *((_DWORD *)pDungeonCels + v9);
					v14 = 32;
					do
					{
						v46 = v14;
						v15 = 32;
						do
						{
							while ( 1 )
							{
								v16 = *v13++;
								if ( (v16 & 0x80u) == 0 )
									break;
								_LOBYTE(v16) = -(char)v16;
								v15 -= v16;
								if ( !v15 )
									goto LABEL_36;
							}
							v15 -= v16;
							v17 = v16;
							do
							{
								v18 = *v13++;
								if ( v18 && v18 < 0x20u )
									v53 = 0;
								--v17;
							}
							while ( v17 );
						}
						while ( v15 );
LABEL_36:
						v14 = v46 - 1;
					}
					while ( v46 != 1 );
				}
				else
				{
					v10 = (char *)pDungeonCels + *((_DWORD *)pDungeonCels + v9);
					for ( j = level_frame_sizes[v9]; j; --j )
					{
						v12 = *v10++;
						if ( v12 && v12 < 0x20u )
							v53 = 0;
					}
				}
				if ( !v53 )
					level_frame_count[v9] = 0;
			}
			++v9;
		}
		while ( v9 < nlevel_frames );
	}
	gendung_4191BF(2047);
	v19 = 0;
	v20 = 0;
	if ( light4flag )
	{
		do
		{
			v21 = level_frame_sizes[v20++];
			v19 += 2 * v21;
		}
		while ( v19 < 0x100000 );
	}
	else
	{
		do
			v19 += 14 * level_frame_sizes[v20++];
		while ( v19 < 0x100000 );
	}
	v22 = v20 - 1;
	v58 = v22;
	if ( v22 > 128 )
	{
		v58 = 128;
		v22 = 128;
	}
	v23 = -(light4flag != 0);
	v63 = 0;
	_LOBYTE(v23) = v23 & 0xF4;
	v54 = 0;
	v60 = v23 + 15;
	if ( v22 > 0 )
	{
		v56 = 0;
		v49 = speed_cel_frame_num_from_light_index_frame_num;
		do
		{
			v24 = v54;
			v25 = level_frame_types[v54] == 4096;
			v62 = tile_defs[v54];
			(*v49)[0] = v62;
			if ( v25 )
			{
				v65 = 1;
				if ( v60 > 1 )
				{
					do
					{
						speed_cel_frame_num_from_light_index_frame_num[0][v65 + v56] = v63;
						v33 = (char *)pDungeonCels + *((_DWORD *)pDungeonCels + v62);
						v34 = (char *)pSpeedCels + v63;
						_EBX = &pLightTbl[256 * v65];
						v36 = 32;
						do
						{
							v47 = v36;
							v37 = 32;
							do
							{
								while ( 1 )
								{
									v38 = (unsigned char)*v33++;
									*v34++ = v38;
									if ( (v38 & 0x80u) == 0 )
										break;
									_LOBYTE(v38) = -(char)v38;
									v37 -= v38;
									if ( !v37 )
										goto LABEL_63;
								}
								v37 -= v38;
								v39 = v38;
								do
								{
									_EAX = *v33++;
									ASM_XLAT(_EAX,_EBX);
									*v34++ = _EAX;
									--v39;
								}
								while ( v39 );
							}
							while ( v37 );
LABEL_63:
							v36 = v47 - 1;
						}
						while ( v47 != 1 );
						v63 += level_frame_sizes[v54];
						++v65;
					}
					while ( v65 < v60 );
					goto LABEL_65;
				}
			}
			else
			{
				v26 = level_frame_sizes[v24];
				v51 = level_frame_sizes[v24];
				v64 = 1;
				if ( v60 > 1 )
				{
					do
					{
						speed_cel_frame_num_from_light_index_frame_num[0][v64 + v56] = v63;
						v27 = (char *)pDungeonCels + *((_DWORD *)pDungeonCels + v62);
						v28 = (char *)pSpeedCels + v63;
						_EBX = &pLightTbl[256 * v64];
						for ( k = v51; k; --k )
						{
							_EAX = *v27++;
							ASM_XLAT(_EAX,_EBX);
							*v28++ = _EAX;
						}
						v63 += v26;
						++v64;
					}
					while ( v64 < v60 );
LABEL_65:
					v22 = v58;
					goto LABEL_66;
				}
			}
LABEL_66:
			++v54;
			v49 = (int (*)[128])((char *)v49 + 64);
			v56 += 16;
		}
		while ( v54 < v22 );
	}
	v57 = dPiece;
	v55 = dpiece_defs_map_2;
	do
	{
		v61 = 112;
		v52 = v55;
		v50 = v57;
		do
		{
			if ( (*v50)[0] && v48 > 0 )
			{
				v42 = v52;
				v59 = v48;
				do
				{
					v43 = *(_WORD *)v42;
					if ( *(_WORD *)v42 )
					{
						v44 = 0;
						if ( v22 > 0 )
						{
							do
							{
								if ( (v43 & 0xFFF) == tile_defs[v44] )
								{
									v45 = v44 + level_frame_types[v44];
									v44 = v22;
									v43 = v45 + -32768;
								}
								++v44;
							}
							while ( v44 < v22 );
							*(_WORD *)v42 = v43;
						}
					}
					v42 = (short (*)[112][112])((char *)v42 + 2);
					--v59;
				}
				while ( v59 );
			}
			++v50;
			v52 = (short (*)[112][112])((char *)v52 + 3584);
			--v61;
		}
		while ( v61 );
		v55 = (short (*)[112][112])((char *)v55 + 32);
		v57 = (int (*)[112])((char *)v57 + 4);
	}
	while ( (signed int)v55 < (signed int)dpiece_defs_map_2[0][16] ); /* check */
}
// 525728: using guessed type int light4flag;
// 53CD4C: using guessed type int nlevel_frames;
// 5BB1ED: using guessed type char leveltype;

void __fastcall gendung_4191BF(int frames)
{
	int v1; // edi
	signed int v2; // eax
	int i; // esi

	v1 = frames;
	v2 = 0;
	while ( v1 > 0 && !v2 )
	{
		v2 = 1;
		for ( i = 0; i < v1; ++i )
		{
			if ( level_frame_count[i] < level_frame_count[i + 1] )
			{
				gendung_4191FB(i, i + 1);
				v2 = 0;
			}
		}
		--v1;
	}
}

void __fastcall gendung_4191FB(int a1, int a2)
{
	int v2; // esi
	int *v3; // edi
	short *v4; // edx
	int v5; // ST10_4
	int *v6; // edi
	int *v7; // eax
	int v8; // ST10_4
	short *v9; // ecx
	int v10; // edx

	v2 = a2;
	v3 = &level_frame_count[a1];
	v4 = &level_frame_types[a2];
	v2 *= 4;
	v5 = *v3;
	*v3 = *(int *)((char *)level_frame_count + v2);
	v6 = &tile_defs[a1];
	*(int *)((char *)level_frame_count + v2) = v5;
	v7 = &level_frame_sizes[a1];
	v8 = *v6;
	*v6 = *(int *)((char *)tile_defs + v2);
	*(int *)((char *)tile_defs + v2) = v8;
	v9 = &level_frame_types[a1];
	_LOWORD(v6) = *v9;
	*v9 = *v4;
	*v4 = (signed short)v6;
	v10 = *v7;
	*v7 = *(int *)((char *)level_frame_sizes + v2);
	*(int *)((char *)level_frame_sizes + v2) = v10;
}

int __fastcall gendung_get_dpiece_num_from_coord(int x, int y)
{
	__int64 v3; // rax

	if ( x < 112 - y )
		return (y * (y + 1) + x * (x + 2 * y + 3)) / 2;
	v3 = (111 - y) * (111 - y + 1) + (111 - x) * (111 - x + 2 * (111 - y) + 3);
	return 12543 - (((signed int)v3 - HIDWORD(v3)) >> 1);
}

void __cdecl gendung_4192C2()
{
	short (*v0)[112][112]; // ebx
	int v1; // ebp
	short (*v2)[112][112]; // esi
	char *v3; // edi
	int x; // [esp+10h] [ebp-4h]

	x = 0;
	v0 = dpiece_defs_map_2;
	do
	{
		v1 = 0;
		do
		{
			v2 = v0;
			v3 = (char *)dpiece_defs_map_1 + 32 * gendung_get_dpiece_num_from_coord(x, v1++);
			v0 = (short (*)[112][112])((char *)v0 + 32);
			qmemcpy(v3, v2, 0x20u);
		}
		while ( v1 < 112 );
		++x;
	}
	while ( (signed int)v0 < (signed int)&dpiece_defs_map_2[16][0][0] );
}

void __cdecl SetDungeonMicros()
{
	signed int v0; // esi
	short (*v1)[112][112]; // edx
	int (*v2)[112]; // ebp
	int v3; // eax
	char *v4; // eax
	signed int i; // ecx
	_WORD *v6; // edi
	int j; // ecx
	short (*v8)[112][112]; // [esp+8h] [ebp-Ch]
	int (*v9)[112]; // [esp+Ch] [ebp-8h]
	signed int v10; // [esp+10h] [ebp-4h]

	if ( leveltype == DTYPE_HELL )
	{
		dword_5A5594 = 12;
		v0 = 16;
	}
	else
	{
		dword_5A5594 = 10;
		v0 = 10;
	}
	v9 = dPiece;
	v8 = dpiece_defs_map_2;
	do
	{
		v1 = v8;
		v2 = v9;
		v10 = 112;
		do
		{
			if ( (*v2)[0] )
			{
				v3 = (*v2)[0] - 1;
				if ( leveltype == DTYPE_HELL )
					v4 = (char *)pLevelPieces + 32 * v3;
				else
					v4 = (char *)pLevelPieces + 20 * v3;
				for ( i = 0; i < v0; ++i )
					(*v1)[0][i] = *(_WORD *)&v4[2 * (v0 + (i & 1) - (i & 0xE)) - 4];
			}
			else if ( v0 > 0 )
			{
				memset(v1, 0, 4 * ((unsigned int)v0 >> 1));
				v6 = (_WORD *)((char *)v1 + 4 * ((unsigned int)v0 >> 1));
				for ( j = v0 & 1; j; --j )
				{
					*v6 = 0;
					++v6;
				}
			}
			++v2;
			v1 = (short (*)[112][112])((char *)v1 + 3584);
			--v10;
		}
		while ( v10 );
		v8 = (short (*)[112][112])((char *)v8 + 32);
		v9 = (int (*)[112])((char *)v9 + 4);
	}
	while ( (signed int)v8 < (signed int)dpiece_defs_map_2[0][16] ); /* check */
	gendung_418D91();
	gendung_4192C2();
	if ( zoomflag )
	{
		scr_pix_width = 640;
		scr_pix_height = 352;
		dword_5C2FF8 = 10;
		dword_5C2FFC = 11;
	}
	else
	{
		scr_pix_width = 384;
		scr_pix_height = 224;
		dword_5C2FF8 = 6;
		dword_5C2FFC = 7;
	}
}
// 52569C: using guessed type int zoomflag;
// 5BB1ED: using guessed type char leveltype;
// 5C2FF8: using guessed type int dword_5C2FF8;
// 5C2FFC: using guessed type int dword_5C2FFC;
// 5C3000: using guessed type int scr_pix_width;
// 5C3004: using guessed type int scr_pix_height;

void __cdecl DRLG_InitTrans()
{
	memset(dung_map, 0, sizeof(dung_map));
	memset(TransList, 0, sizeof(TransList));
	TransVal = 1;
}
// 5A5590: using guessed type char TransVal;

void __fastcall DRLG_MRectTrans(int x1, int y1, int x2, int y2)
{
	int v4; // esi
	int v5; // edi
	int i; // eax
	char *v7; // edx
	int j; // ecx
	int ty_enda; // [esp+10h] [ebp+8h]

	v4 = 2 * x1 + 17;
	v5 = 2 * x2 + 16;
	i = 2 * y1 + 17;
	for ( ty_enda = 2 * y2 + 16; i <= ty_enda; ++i )
	{
		if ( v4 <= v5 )
		{
			v7 = &dung_map[v4][i];
			j = v5 - v4 + 1;
			do
			{
				*v7 = TransVal;
				v7 += 112;
				--j;
			}
			while ( j );
		}
	}
	++TransVal;
}
// 5A5590: using guessed type char TransVal;

void __fastcall DRLG_RectTrans(int x1, int y1, int x2, int y2)
{
	int i; // esi
	char *v5; // edx
	int j; // eax

	for ( i = y1; i <= y2; ++i )
	{
		if ( x1 <= x2 )
		{
			v5 = &dung_map[x1][i];
			j = x2 - x1 + 1;
			do
			{
				*v5 = TransVal;
				v5 += 112;
				--j;
			}
			while ( j );
		}
	}
	++TransVal;
}
// 5A5590: using guessed type char TransVal;

void __fastcall DRLG_CopyTrans(int sx, int sy, int dx, int dy)
{
	dung_map[dx][dy] = dung_map[sx][sy];
}

void __fastcall DRLG_ListTrans(int num, unsigned char *List)
{
	unsigned char *v2; // esi
	int v3; // edi
	unsigned char v4; // al
	unsigned char *v5; // esi
	unsigned char v6; // cl
	unsigned char v7; // dl
	unsigned char v8; // bl

	v2 = List;
	if ( num > 0 )
	{
		v3 = num;
		do
		{
			v4 = *v2;
			v5 = v2 + 1;
			v6 = *v5++;
			v7 = *v5++;
			v8 = *v5;
			v2 = v5 + 1;
			DRLG_RectTrans(v4, v6, v7, v8);
			--v3;
		}
		while ( v3 );
	}
}

void __fastcall DRLG_AreaTrans(int num, unsigned char *List)
{
	unsigned char *v2; // esi
	int v3; // edi
	unsigned char v4; // al
	unsigned char *v5; // esi
	unsigned char v6; // cl
	unsigned char v7; // dl
	unsigned char v8; // bl

	v2 = List;
	if ( num > 0 )
	{
		v3 = num;
		do
		{
			v4 = *v2;
			v5 = v2 + 1;
			v6 = *v5++;
			v7 = *v5++;
			v8 = *v5;
			v2 = v5 + 1;
			DRLG_RectTrans(v4, v6, v7, v8);
			--TransVal;
			--v3;
		}
		while ( v3 );
	}
	++TransVal;
}
// 5A5590: using guessed type char TransVal;

void __cdecl DRLG_InitSetPC()
{
	setpc_x = 0;
	setpc_y = 0;
	setpc_w = 0;
	setpc_h = 0;
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __cdecl DRLG_SetPC()
{
	int v0; // ebx
	int v1; // edx
	int v2; // ecx
	int v3; // esi
	int i; // eax
	int v5; // ebp
	char *v6; // edi

	v0 = 0;
	v1 = 2 * setpc_w;
	v2 = 2 * setpc_h;
	v3 = 2 * setpc_x + 16;
	for ( i = 2 * setpc_y + 16; v0 < v2; ++v0 )
	{
		if ( v1 > 0 )
		{
			v5 = v1;
			v6 = &dFlags[v3][v0 + i];
			do
			{
				*v6 |= DFLAG_POPULATED;
				v6 += 112;
				--v5;
			}
			while ( v5 );
		}
	}
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __fastcall Make_SetPC(int x, int y, int w, int h)
{
	int v4; // eax
	int v5; // esi
	int v6; // ebx
	int i; // eax
	int v8; // edx
	char *v9; // ecx
	int wa; // [esp+14h] [ebp+8h]

	v4 = w;
	wa = 0;
	v5 = 2 * v4;
	v6 = 2 * x + 16;
	for ( i = 2 * y + 16; wa < 2 * h; ++wa )
	{
		if ( v5 > 0 )
		{
			v8 = v5;
			v9 = &dFlags[v6][wa + i];
			do
			{
				*v9 |= DFLAG_POPULATED;
				v9 += 112;
				--v8;
			}
			while ( v8 );
		}
	}
}

bool __fastcall DRLG_WillThemeRoomFit(int floor, int x, int y, int minSize, int maxSize, int *width, int *height)
{
	int v7; // esi
	int v8; // edi
	int v10; // ebx
	int v11; // edx
	unsigned char *v12; // eax
	int v13; // eax
	int i; // eax
	int v15; // eax
	int v16; // esi
	int v17; // eax
	int v18; // edx
	int v19; // ecx
	int v21; // eax
	int v22; // esi
	int yArray[20]; // [esp+8h] [ebp-BCh]
	int xArray[20]; // [esp+58h] [ebp-6Ch]
	int v25; // [esp+A8h] [ebp-1Ch]
	int v26; // [esp+ACh] [ebp-18h]
	int v27; // [esp+B0h] [ebp-14h]
	int v28; // [esp+B4h] [ebp-10h]
	char *v29; // [esp+B8h] [ebp-Ch]
	int v30; // [esp+BCh] [ebp-8h]
	int v31; // [esp+C0h] [ebp-4h]

	v28 = 1;
	v27 = 1;
	v7 = x;
	v8 = 0;
	v25 = floor;
	v31 = 0;
	v30 = 0;
	if ( x > 40 - maxSize && y > 40 - maxSize )
		return 0;
	if ( !SkipThemeRoom(x, y) )
		return 0;
	memset(xArray, 0, sizeof(xArray));
	memset(yArray, 0, sizeof(yArray));
	if ( maxSize > 0 )
	{
		v10 = 40 * v7;
		v26 = 40 * v7;
		v29 = dungeon[v7];
		do
		{
			if ( v27 )
			{
				v11 = v7;
				if ( v7 < v7 + maxSize )
				{
					v12 = (unsigned char *)dungeon + v8 + v10 + y;
					do
					{
						if ( *v12 == v25 )
						{
							++v31;
						}
						else
						{
							if ( v11 >= minSize )
								break;
							v27 = 0;
						}
						++v11;
						v12 += 40;
					}
					while ( v11 < v7 + maxSize );
					v10 = v26;
				}
				if ( v27 )
				{
					v13 = v31;
					v31 = 0;
					xArray[v8] = v13;
				}
			}
			if ( v28 )
			{
				for ( i = y; i < y + maxSize; ++i )
				{
					if ( (unsigned char)v29[i] == v25 )
					{
						++v30;
					}
					else
					{
						if ( i >= minSize )
							break;
						v28 = 0;
					}
				}
				if ( v28 )
				{
					v15 = v30;
					v30 = 0;
					yArray[v8] = v15;
				}
			}
			v29 += 40;
			++v8;
		}
		while ( v8 < maxSize );
		v8 = 0;
	}
	v16 = minSize;
	v17 = 0;
	if ( minSize > 0 )
	{
		while ( xArray[v17] >= minSize && yArray[v17] >= minSize )
		{
			if ( ++v17 >= minSize )
				goto LABEL_32;
		}
		return 0;
	}
LABEL_32:
	v18 = xArray[0];
	v19 = yArray[0];
	if ( maxSize > 0 )
	{
		while ( 1 )
		{
			v21 = xArray[v8];
			if ( v21 < v16 )
				break;
			v22 = yArray[v8];
			if ( v22 < minSize )
				break;
			if ( v21 < v18 )
				v18 = xArray[v8];
			if ( v22 < v19 )
				v19 = yArray[v8];
			if ( ++v8 >= maxSize )
				break;
			v16 = minSize;
		}
	}
	*width = v18 - 2;
	*height = v19 - 2;
	return 1;
}
// 41965B: using guessed type int var_6C[20];
// 41965B: using guessed type int var_BC[20];

void __fastcall DRLG_CreateThemeRoom(int themeIndex)
{
	int v1; // esi
	int v2; // eax
	int v3; // edi
	int v4; // ebx
	int v5; // ecx
	int v6; // ecx
	int v7; // ebx
	int v8; // edx
	int v9; // ebx
	int v10; // edx
	int v11; // ebx
	int v12; // edx
	int v13; // eax
	int v14; // eax
	int v15; // eax
	int v16; // ecx
	char *v17; // eax
	int v18; // ecx
	char *v19; // eax
	int v20; // [esp+Ch] [ebp-8h]
	char *v21; // [esp+10h] [ebp-4h]

	v1 = themeIndex;
	v2 = themeLoc[themeIndex].y;
	v3 = themeLoc[themeIndex].height;
	v4 = v2;
	v5 = v3 + v2;
	if ( v2 < v3 + v2 )
	{
		v20 = themeLoc[v1].x + themeLoc[v1].width;
		while ( 1 )
		{
			v6 = themeLoc[v1].x;
			if ( v6 < v20 )
				break;
LABEL_52:
			++v4;
			v5 = v3 + v2;
			if ( v4 >= v3 + v2 )
				goto LABEL_53;
		}
		v21 = &dungeon[v6][v4];
		while ( 1 )
		{
			if ( leveltype != DTYPE_CATACOMBS )
				goto LABEL_21;
			if ( v4 == v2 && v6 >= themeLoc[v1].x && v6 <= v20 )
				goto LABEL_12;
			if ( v4 != v3 + v2 - 1 )
				goto LABEL_13;
			if ( v6 >= themeLoc[v1].x )
				break;
LABEL_16:
			if ( v6 == v20 - 1 && v4 >= v2 && v4 <= v3 + v2 )
				goto LABEL_19;
			*v21 = 3;
LABEL_21:
			if ( leveltype == DTYPE_CAVES )
			{
				if ( v4 == v2 && v6 >= themeLoc[v1].x && v6 <= v20 )
				{
LABEL_28:
					*v21 = -122;
					goto LABEL_51;
				}
				if ( v4 == v3 + v2 - 1 )
				{
					if ( v6 >= themeLoc[v1].x )
					{
						if ( v6 <= v20 )
							goto LABEL_28;
						goto LABEL_29;
					}
				}
				else
				{
LABEL_29:
					if ( v6 == themeLoc[v1].x && v4 >= v2 && v4 <= v3 + v2 )
					{
LABEL_35:
						*v21 = -119;
						goto LABEL_51;
					}
				}
				if ( v6 == v20 - 1 && v4 >= v2 && v4 <= v3 + v2 )
					goto LABEL_35;
				*v21 = 7;
			}
			if ( leveltype != DTYPE_HELL )
				goto LABEL_51;
			if ( v4 != v2 || v6 < themeLoc[v1].x || v6 > v20 )
			{
				if ( v4 != v3 + v2 - 1 )
					goto LABEL_44;
				if ( v6 < themeLoc[v1].x )
					goto LABEL_47;
				if ( v6 > v20 )
				{
LABEL_44:
					if ( v6 != themeLoc[v1].x || v4 < v2 || v4 > v3 + v2 )
					{
LABEL_47:
						if ( v6 != v20 - 1 || v4 < v2 || v4 > v3 + v2 )
						{
							*v21 = 6;
							goto LABEL_51;
						}
					}
LABEL_19:
					*v21 = 1;
					goto LABEL_51;
				}
			}
LABEL_12:
			*v21 = 2;
LABEL_51:
			v21 += 40;
			if ( ++v6 >= v20 )
				goto LABEL_52;
		}
		if ( v6 <= v20 )
			goto LABEL_12;
LABEL_13:
		if ( v6 == themeLoc[v1].x && v4 >= v2 && v4 <= v3 + v2 )
			goto LABEL_19;
		goto LABEL_16;
	}
LABEL_53:
	if ( leveltype == DTYPE_CATACOMBS )
	{
		v7 = themeLoc[v1].x;
		v8 = 10 * (v7 + themeLoc[v1].width);
		dungeon[v7][v2] = 8;
		v5 = v3 + 40 * v7;
		dungeon[-1][v8 * 4 + v2] = 7; // *((_BYTE *)&dMonster[111][v8 + 102] + v2) = 7; /* check */
		dungeon[0][v5 + v2 - 1] = 9; // *((_BYTE *)&dMonster[111][111] + v5 + v2 + 3) = 9;
		dungeon[-1][v3 + v8 * 4 + v2 - 1] = 6; // *((_BYTE *)&dMonster[111][101] + v3 + v8 * 4 + v2 + 3) = 6;
	}
	if ( leveltype == DTYPE_CAVES )
	{
		v9 = themeLoc[v1].x;
		v10 = 10 * (v9 + themeLoc[v1].width);
		dungeon[v9][v2] = 150;
		v5 = v3 + 40 * v9;
		dungeon[-1][v10 * 4 + v2] = 151; // *((_BYTE *)&dMonster[111][v10 + 102] + v2) = -105;
		dungeon[0][v5 + v2 - 1] = 152; // *((_BYTE *)&dMonster[111][111] + v5 + v2 + 3) = -104;
		dungeon[-1][v3 + v10 * 4 + v2 - 1] = 138; // *((_BYTE *)&dMonster[111][101] + v3 + v10 * 4 + v2 + 3) = -118;
	}
	if ( leveltype == DTYPE_HELL )
	{
		v11 = themeLoc[v1].x;
		v12 = 10 * (v11 + themeLoc[v1].width);
		dungeon[v11][v2] = 9;
		v5 = v3 + 40 * v11;
		dungeon[-1][v12 * 4 + v2] = 16; // *((_BYTE *)&dMonster[111][v12 + 102] + v2) = 16;
		dungeon[0][v5 + v2 - 1] = 15; // *((_BYTE *)&dMonster[111][111] + v5 + v2 + 3) = 15;
		dungeon[-1][v3 + v12 * 4 + v2 - 1] = 12; // *((_BYTE *)&dMonster[111][101] + v3 + v12 * 4 + v2 + 3) = 12;
	}
	if ( leveltype == DTYPE_CATACOMBS )
	{
		v13 = random(0, 2);
		if ( v13 )
		{
			if ( v13 == 1 )
			{
				v5 = themeLoc[v1].height;
				dungeon[themeLoc[v1].x + themeLoc[v1].width / 2][themeLoc[v1].y + v5 - 1] = 5; /*
				*((_BYTE *)&dMonster[111][10 * (themeLoc[v1].x + themeLoc[v1].width / 2) + 111] + themeLoc[v1].y
																								+ v5
																								+ 3) = 5; */
			}
		}
		else
		{
			v5 = themeLoc[v1].y;
			dungeon[themeLoc[v1].x + themeLoc[v1].width -1][themeLoc[v1].height / 2 + v5] = 4;
			// *((_BYTE *)&dMonster[111][10 * (themeLoc[v1].x + themeLoc[v1].width) + 102] + themeLoc[v1].height / 2 + v5) = 4;
		}
	}
	if ( leveltype == DTYPE_CAVES )
	{
		v14 = random(0, 2);
		if ( v14 )
		{
			if ( v14 == 1 )
			{
				v5 = themeLoc[v1].height;
				dungeon[themeLoc[v1].x + themeLoc[v1].width / 2][themeLoc[v1].y + v5 - 1] = 146; /*
				*((_BYTE *)&dMonster[111][10 * (themeLoc[v1].x + themeLoc[v1].width / 2) + 111] + themeLoc[v1].y
																								+ v5
																								+ 3) = -110; */
			}
		}
		else
		{
			v5 = themeLoc[v1].y;
			dungeon[themeLoc[v1].x + themeLoc[v1].width -1][themeLoc[v1].height / 2 + v5] = 147;
			// *((_BYTE *)&dMonster[111][10 * (themeLoc[v1].x + themeLoc[v1].width) + 102] + themeLoc[v1].height / 2 + v5) = -109;
		}
	}
	if ( leveltype == DTYPE_HELL )
	{
		v15 = random(0, 2);
		if ( v15 )
		{
			if ( v15 == 1 )
			{
				v16 = themeLoc[v1].y + 40 * (themeLoc[v1].x + themeLoc[v1].width / 2) + themeLoc[v1].height;
				v17 = (char *)dungeon + v16;
				*(v17 - 41) = 57;
				*(v17 - 1) = 6;
				dungeon[0][v16 + 39] = 56;
				*(v17 - 2) = 59;
				*(v17 - 42) = 58;
			}
		}
		else
		{
			v18 = themeLoc[v1].height / 2 + 40 * (themeLoc[v1].x + themeLoc[v1].width) + themeLoc[v1].y;
			v19 = (char *)dungeon + v18;
			*(v19 - 41) = 53;
			*(v19 - 40) = 6;
			dungeon[0][v18 - 39] = 52; // *((_BYTE *)&dMonster[111][102] + v18 + 1) = 52;
			*(v19 - 81) = 54;
		}
	}
}
// 5BB1ED: using guessed type char leveltype;

void __fastcall DRLG_PlaceThemeRooms(int minSize, int maxSize, int floor, int freq, int rndSize)
{
	int v5; // ebx
	//int v7; // eax
	int v8; // esi
	int v9; // edi
	int v10; // eax
	int v12; // eax
	int v14; // eax
	int v16; // eax
	int v17; // edi
	int v18; // esi
	int v19; // ecx
	int v20; // ecx
	int v21; // eax
	int minSize2; // [esp+10h] [ebp-1Ch]
	int maxSize2; // [esp+14h] [ebp-18h]
	unsigned char *v24; // [esp+18h] [ebp-14h]
	signed int x_start; // [esp+1Ch] [ebp-10h]
	int x; // [esp+20h] [ebp-Ch]
	int width; // [esp+24h] [ebp-8h]
	int height; // [esp+28h] [ebp-4h]

	v5 = 0;
	maxSize2 = maxSize;
	minSize2 = minSize;
	themeCount = 0;
	memset(themeLoc, 0, sizeof(*themeLoc));
	do
	{
		x = 0;
		x_start = 20;
		v24 = (unsigned char *)dungeon + v5;
		do
		{
			if ( *v24 == floor )
			{
				if ( !random(0, freq) )
				{
					//_LOBYTE(v7) = DRLG_WillThemeRoomFit(floor, x, v5, minSize2, maxSize2, &width, &height);
					if ( DRLG_WillThemeRoomFit(floor, x, v5, minSize2, maxSize2, &width, &height) )
					{
						if ( rndSize )
						{
							v8 = minSize2 - 2;
							v9 = maxSize2 - 2;
							v10 = random(0, width - (minSize2 - 2) + 1);
							v12 = minSize2 - 2 + random(0, v10);
							if ( v12 < minSize2 - 2 || (width = v12, v12 > v9) )
								width = minSize2 - 2;
							v14 = random(0, height - v8 + 1);
							v16 = v8 + random(0, v14);
							if ( v16 < v8 || v16 > v9 )
								v16 = minSize2 - 2;
							height = v16;
						}
						else
						{
							v16 = height;
						}
						v17 = themeCount;
						v18 = themeCount;
						themeLoc[v18].x = x + 1;
						themeLoc[v18].y = v5 + 1;
						v19 = width;
						themeLoc[v18].width = width;
						themeLoc[v18].height = v16;
						v20 = x + v19;
						v21 = v5 + v16;
						if ( leveltype == DTYPE_CAVES )
							DRLG_RectTrans(x_start, 2 * v5 + 20, 2 * v20 + 15, 2 * v21 + 15);
						else
							DRLG_MRectTrans(x + 1, v5 + 1, v20, v21);
						themeLoc[v18].ttval = TransVal - 1;
						DRLG_CreateThemeRoom(v17);
						++themeCount;
					}
				}
			}
			x_start += 2;
			++x;
			v24 += 40;
		}
		while ( x_start < 100 );
		++v5;
	}
	while ( v5 < 40 );
}
// 5A5590: using guessed type char TransVal;
// 5BB1ED: using guessed type char leveltype;

void __cdecl DRLG_HoldThemeRooms()
{
	int *v0; // esi
	int v1; // edi
	int v2; // edx
	int v3; // ebx
	int v4; // edi
	int v5; // ecx
	int v6; // eax
	int v7; // [esp+0h] [ebp-Ch]
	int v8; // [esp+4h] [ebp-8h]
	int v9; // [esp+8h] [ebp-4h]

	if ( themeCount > 0 )
	{
		v0 = &themeLoc[0].height;
		v8 = themeCount;
		do
		{
			v1 = *(v0 - 3);
			if ( v1 < v1 + *v0 - 1 )
			{
				v2 = *(v0 - 4);
				v3 = 2 * v1 + 16;
				v7 = v2 + *(v0 - 1) - 1;
				v9 = *v0 - 1;
				do
				{
					if ( v2 < v7 )
					{
						v4 = 224 * (v2 + 8);
						v5 = v7 - v2;
						do
						{
							v6 = v3 + v4;
							v4 += 224;
							dFlags[0][v6] |= DFLAG_POPULATED;
							dFlags[1][v6] |= DFLAG_POPULATED;
							dFlags[0][v6 + 1] |= DFLAG_POPULATED;
							dFlags[1][v6 + 1] |= DFLAG_POPULATED;
							--v5;
						}
						while ( v5 );
					}
					v3 += 2;
					--v9;
				}
				while ( v9 );
			}
			v0 += 5;
			--v8;
		}
		while ( v8 );
	}
}

bool __fastcall SkipThemeRoom(int x, int y)
{
	int i; // ebx
	THEME_LOC *v3; // eax
	int v4; // esi

	i = 0;
	if ( themeCount <= 0 )
		return 1;
	v3 = themeLoc;
	while ( 1 )
	{
		if ( x >= v3->x - 2 && x <= v3->x + v3->width + 2 )
		{
			v4 = v3->y;
			if ( y >= v4 - 2 && y <= v4 + v3->height + 2 )
				break;
		}
		++i;
		++v3;
		if ( i >= themeCount )
			return 1;
	}
	return 0;
}

void __cdecl InitLevels()
{
	if ( !leveldebug )
	{
		currlevel = 0;
		leveltype = 0;
		setlevel = 0;
	}
}
// 52572C: using guessed type int leveldebug;
// 5BB1ED: using guessed type char leveltype;
// 5CF31D: using guessed type char setlevel;
