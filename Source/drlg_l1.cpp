//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
char L5dungeon[80][80];
char mydflags[40][40];
int setloadflag; // weak
int HR1;
int HR2;
int HR3;
int VR1;
int VR2;
int VR3;
void *pSetPiece; // idb
#endif

const ShadowStruct SPATS[37] =
{
  { 7, 13, 0, 13, 144, 0, 142 },
  { 16, 13, 0, 13, 144, 0, 142 },
  { 15, 13, 0, 13, 145, 0, 142 },
  { 5, 13, 13, 13, 152, 140, 139 },
  { 5, 13, 1, 13, 143, 146, 139 },
  { 5, 13, 13, 2, 143, 140, 148 },
  { 5, 0, 1, 2, 0, 146, 148 },
  { 5, 13, 11, 13, 143, 147, 139 },
  { 5, 13, 13, 12, 143, 140, 149 },
  { 5, 13, 11, 12, 150, 147, 149 },
  { 5, 13, 1, 12, 143, 146, 149 },
  { 5, 13, 11, 2, 143, 147, 148 },
  { 9, 13, 13, 13, 144, 140, 142 },
  { 9, 13, 1, 13, 144, 146, 142 },
  { 9, 13, 11, 13, 151, 147, 142 },
  { 8, 13, 0, 13, 144, 0, 139 },
  { 8, 13, 0, 12, 143, 0, 149 },
  { 8, 0, 0, 2, 0, 0, 148 },
  { 11, 0, 0, 13, 0, 0, 139 },
  { 11, 13, 0, 13, 139, 0, 139 },
  { 11, 2, 0, 13, 148, 0, 139 },
  { 11, 12, 0, 13, 149, 0, 139 },
  { 11, 13, 11, 12, 139, 0, 149 },
  { 14, 0, 0, 13, 0, 0, 139 },
  { 14, 13, 0, 13, 139, 0, 139 },
  { 14, 2, 0, 13, 148, 0, 139 },
  { 14, 12, 0, 13, 149, 0, 139 },
  { 14, 13, 11, 12, 139, 0, 149 },
  { 10, 0, 13, 0, 0, 140, 0 },
  { 10, 13, 13, 0, 140, 140, 0 },
  { 10, 0, 1, 0, 0, 146, 0 },
  { 10, 13, 11, 0, 140, 147, 0 },
  { 12, 0, 13, 0, 0, 140, 0 },
  { 12, 13, 13, 0, 140, 140, 0 },
  { 12, 0, 1, 0, 0, 146, 0 },
  { 12, 13, 11, 0, 140, 147, 0 },
  { 3, 13, 11, 12, 150, 0, 0 }
};
const unsigned char BSTYPES[206] =
{
	0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
   10,  11,  12,  13,  14,  15,  16,  17,   0,   0,
	0,   0,   0,   0,   0,   1,   2,  10,   4,   5,
	6,   7,   8,   9,  10,  11,  12,  14,   5,  14,
   10,   4,  14,   4,   5,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   1,
	2,   3,   4,   1,   6,   7,  16,  17,   2,   1,
	1,   2,   2,   1,   1,   2,   2,   2,   2,   2,
	1,   1,  11,   1,  13,  13,  13,   1,   2,   1,
	2,   1,   2,   1,   2,   2,   2,   2,  12,   0,
	0,  11,   1,  11,   1,  13,   0,   0,   0,   0,
	0,   0,   0,  13,  13,  13,  13,  13,  13,  13,
   13,  13,  13,  13,  13,  13,   1,  11,   2,  12,
   13,  13,  13,  12,   2,   1,   2,   2,   4,  14,
	4,  10,  13,  13,   4,   4,   1,   1,   4,   2,
	2,  13,  13,  13,  13,  25,  26,  28,  30,  31,
   41,  43,  40,  41,  42,  43,  25,  41,  43,  28,
   28,   1,   2,  25,  26,  22,  22,  25,  26,   0,
	0,   0,   0,   0,   0,   0
};
const unsigned char L5BTYPES[206] =
{
	0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
   10,  11,  12,  13,  14,  15,  16,  17,   0,   0,
	0,   0,   0,   0,   0,  25,  26,   0,  28,   0,
   30,  31,   0,   0,   0,   0,   0,   0,   0,   0,
   40,  41,  42,  43,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,  79,
   80,   0,  82,   0,   0,   0,   0,   0,   0,  79,
	0,  80,   0,   0,  79,  80,   0,   2,   2,   2,
	1,   1,  11,  25,  13,  13,  13,   1,   2,   1,
	2,   1,   2,   1,   2,   2,   2,   2,  12,   0,
	0,  11,   1,  11,   1,  13,   0,   0,   0,   0,
	0,   0,   0,  13,  13,  13,  13,  13,  13,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0
};
const unsigned char STAIRSUP[] = { 4, 4, 13, 13, 13, 13, 2, 2, 2, 2, 13, 13, 13, 13, 13, 13, 13, 13, 0, 66, 6, 0, 63, 64, 65, 0, 0, 67, 68, 0, 0, 0, 0, 0 };
const unsigned char L5STAIRSUP[] = { 4, 4, 22, 22, 22, 22, 2, 2, 2, 2, 13, 13, 13, 13, 13, 13, 13, 13, 0, 66, 23, 0, 63, 64, 65, 0, 0, 67, 68, 0, 0, 0, 0, 0 };
const unsigned char STAIRSDOWN[] = { 4, 3, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 62, 57, 58, 0, 61, 59, 60, 0, 0, 0, 0, 0 };
const unsigned char LAMPS[] = { 2, 2, 13, 0, 13, 13, 129, 0, 130, 128 };
const unsigned char PWATERIN[] = { 6, 6, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0, 0, 0, 0, 0, 0, 0, 202, 200, 200, 84, 0, 0, 199, 203, 203, 83, 0, 0, 85, 206, 80, 81, 0, 0, 0, 134, 135, 0, 0, 0, 0, 0, 0, 0, 0 };

/* data */
unsigned char L5ConvTbl[16] = { 22u, 13u, 1u, 13u, 2u, 13u, 13u, 13u, 4u, 13u, 1u, 13u, 2u, 13u, 16u, 13u };

void __cdecl DRLG_Init_Globals()
{
	char v0; // al

	memset(dFlags, 0, 0x3100u);
	memset(dPlayer, 0, 0x3100u);
	memset(dMonster, 0, 0xC400u);
	memset(dDead, 0, 0x3100u);
	memset(dObject, 0, 0x3100u);
	memset(dItem, 0, 0x3100u);
	memset(dMissile, 0, 0x3100u);
	memset(dArch, 0, 0x3100u);
	if ( lightflag )
		v0 = 0;
	else
		v0 = light4flag == 0 ? 15 : 3;
	memset(dTransVal, v0, 0x3100u);
}
// 525728: using guessed type int light4flag;
// 646A28: using guessed type int lightflag;

void __fastcall LoadL1Dungeon(char *sFileName, int vx, int vy)
{
	char *v3; // esi
	unsigned char *v4; // esi
	signed int v5; // ecx
	signed int v6; // eax
	signed int v7; // edx
	int v8; // edi
	int v9; // ebx
	char *v10; // eax
	int v11; // ecx
	char v12; // dl
	int v13; // [esp+Ch] [ebp-Ch]
	int v14; // [esp+10h] [ebp-8h]
	int v15; // [esp+14h] [ebp-4h]

	v13 = vx;
	dminx = 16;
	dminy = 16;
	v3 = sFileName;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
	v4 = LoadFileInMem(v3, 0);
	v5 = 0;
	do
	{
		v6 = v5;
		v7 = 40;
		do
		{
			mydflags[0][v6] = 0;
			dungeon[0][v6] = 22;
			v6 += 40;
			--v7;
		}
		while ( v7 );
		++v5;
	}
	while ( v5 < 40 );
	v15 = 0;
	v8 = *v4;
	v9 = v4[2];
	v10 = (char *)(v4 + 4);
	if ( v9 > 0 )
	{
		do
		{
			if ( v8 > 0 )
			{
				v11 = v15;
				v14 = v8;
				do
				{
					v12 = *v10;
					if ( *v10 )
					{
						mydflags[0][v11] |= 0x80u;
						dungeon[0][v11] = v12;
					}
					else
					{
						dungeon[0][v11] = 13;
					}
					v11 += 40;
					v10 += 2;
					--v14;
				}
				while ( v14 );
			}
			++v15;
		}
		while ( v15 < v9 );
	}
	DRLG_L1Floor();
	ViewX = v13;
	ViewY = vy;
	DRLG_L1Pass3();
	DRLG_Init_Globals();
	DRLG_InitL1Vals();
	SetMapMonsters(v4, 0, 0);
	SetMapObjects(v4, 0, 0);
	mem_free_dbg(v4);
}
// 5CF328: using guessed type int dmaxx;
// 5CF32C: using guessed type int dmaxy;
// 5D2458: using guessed type int dminx;
// 5D245C: using guessed type int dminy;

void __cdecl DRLG_L1Floor()
{
	signed int i; // edi
	signed int v1; // esi
	signed int j; // ebx
	int rv; // eax

	i = 0;
	do
	{
		v1 = i;
		j = 40;
		do
		{
			if ( !mydflags[0][v1] && dungeon[0][v1] == 13 )
			{
				rv = random(0, 3);
				if ( rv == 1 )
					dungeon[0][v1] = -94;
				if ( rv == 2 )
					dungeon[0][v1] = -93;
			}
			v1 += 40;
			--j;
		}
		while ( j );
		++i;
	}
	while ( i < 40 );
}

void __cdecl DRLG_L1Pass3()
{
	int v0; // eax
	int *v1; // edx
	int *v2; // eax
	signed int v3; // ecx
	signed int v4; // ebx
	int *v5; // ecx
	unsigned char *v6; // edx
	unsigned short *v7; // esi
	unsigned short v8; // ax
	int v9; // eax
	int v10; // ST24_4
	int v11; // ST20_4
	int v12; // ST1C_4
	signed int v13; // [esp+Ch] [ebp-1Ch]
	int *v14; // [esp+10h] [ebp-18h]
	int v15; // [esp+14h] [ebp-14h]
	int v16; // [esp+18h] [ebp-10h]
	int v17; // [esp+1Ch] [ebp-Ch]
	int v18; // [esp+20h] [ebp-8h]

	v0 = *((unsigned short *)pMegaTiles + 84) + 1;
	v18 = *((unsigned short *)pMegaTiles + 84) + 1;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 85);
	v17 = ++v0;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 86);
	v16 = ++v0;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 87);
	v15 = v0 + 1;
	v1 = dPiece[1];
	do
	{
		v2 = v1;
		v3 = 56;
		do
		{
			*(v2 - 112) = v18;
			*v2 = v17;
			*(v2 - 111) = v16;
			v2[1] = v15;
			v2 += 224;
			--v3;
		}
		while ( v3 );
		v1 += 2;
	}
	while ( (signed int)v1 < (signed int)dPiece[2] );
	v4 = 0;
	v14 = &dPiece[17][16]; /* check */
	do
	{
		v5 = v14;
		v6 = (unsigned char *)dungeon + v4;
		v13 = 40;
		do
		{
			v7 = (unsigned short *)((char *)pMegaTiles + 8 * (*v6 - 1));
			v8 = *v7;
			++v7;
			v9 = v8 + 1;
			v10 = v9;
			_LOWORD(v9) = *v7;
			++v7;
			v11 = ++v9;
			_LOWORD(v9) = *v7;
			v12 = ++v9;
			_LOWORD(v9) = v7[1];
			v6 += 40;
			*(v5 - 112) = v10;
			*v5 = v11;
			*(v5 - 111) = v12;
			v5[1] = v9 + 1;
			v5 += 224;
			--v13;
		}
		while ( v13 );
		v14 += 2;
		++v4;
	}
	while ( v4 < 40 );
}

void __cdecl DRLG_InitL1Vals()
{
	int v0; // esi
	int (*v1)[112]; // edx
	char *v2; // ecx
	int v3; // eax
	char v4; // al
	char v5; // [esp-4h] [ebp-18h]
	signed int v6; // [esp+Ch] [ebp-8h]
	int (*v7)[112]; // [esp+10h] [ebp-4h]

	v0 = 0;
	v7 = dPiece;
	do
	{
		v1 = v7;
		v2 = (char *)dArch + v0;
		v6 = 112;
		do
		{
			v3 = (*v1)[0];
			if ( (*v1)[0] != 12 )
			{
				if ( v3 == 11 )
					goto LABEL_21;
				if ( v3 != 71 )
				{
					if ( v3 == 259 )
					{
						v5 = 5;
LABEL_9:
						v4 = v5;
						goto LABEL_22;
					}
					if ( v3 == 249 || v3 == 325 )
						goto LABEL_21;
					if ( v3 != 321 )
					{
						if ( v3 == 255 )
						{
							v5 = 4;
							goto LABEL_9;
						}
						if ( v3 != 211 )
						{
							if ( v3 == 344 )
								goto LABEL_21;
							if ( v3 != 341 )
							{
								if ( v3 == 331 )
									goto LABEL_21;
								if ( v3 != 418 )
								{
									if ( v3 != 421 )
										goto LABEL_23;
LABEL_21:
									v4 = 2;
									goto LABEL_22;
								}
							}
						}
					}
				}
			}
			v4 = 1;
LABEL_22:
			*v2 = v4;
LABEL_23:
			++v1;
			v2 += 112;
			--v6;
		}
		while ( v6 );
		v7 = (int (*)[112])((char *)v7 + 4);
		++v0;
	}
	while ( (signed int)v7 < (signed int)dPiece[1] );
}

void __fastcall LoadPreL1Dungeon(char *sFileName, int vx, int vy)
{
	unsigned char *v3; // ebx
	signed int v4; // ecx
	signed int v5; // eax
	signed int v6; // edx
	int v7; // esi
	int v8; // edi
	char *v9; // eax
	int v10; // ecx
	char v11; // dl
	signed int v12; // esi
	signed int v13; // eax
	signed int v14; // edi
	int v15; // [esp+Ch] [ebp-8h]
	int v16; // [esp+10h] [ebp-4h]

	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	v3 = LoadFileInMem(sFileName, 0);
	v4 = 0;
	do
	{
		v5 = v4;
		v6 = 40;
		do
		{
			mydflags[0][v5] = 0;
			dungeon[0][v5] = 22;
			v5 += 40;
			--v6;
		}
		while ( v6 );
		++v4;
	}
	while ( v4 < 40 );
	v16 = 0;
	v7 = *v3;
	v8 = v3[2];
	v9 = (char *)(v3 + 4);
	if ( v8 > 0 )
	{
		do
		{
			if ( v7 > 0 )
			{
				v10 = v16;
				v15 = v7;
				do
				{
					v11 = *v9;
					if ( *v9 )
					{
						mydflags[0][v10] |= 0x80u;
						dungeon[0][v10] = v11;
					}
					else
					{
						dungeon[0][v10] = 13;
					}
					v10 += 40;
					v9 += 2;
					--v15;
				}
				while ( v15 );
			}
			++v16;
		}
		while ( v16 < v8 );
	}
	DRLG_L1Floor();
	v12 = 0;
	do
	{
		v13 = v12;
		v14 = 40;
		do
		{
			pdungeon[0][v13] = dungeon[0][v13];
			v13 += 40;
			--v14;
		}
		while ( v14 );
		++v12;
	}
	while ( v12 < 40 );
	mem_free_dbg(v3);
}
// 5CF328: using guessed type int dmaxx;
// 5CF32C: using guessed type int dmaxy;
// 5D2458: using guessed type int dminx;
// 5D245C: using guessed type int dminy;

void __fastcall CreateL5Dungeon(int rseed, int entry)
{
	int v2; // esi

	v2 = entry;
	SetRndSeed(rseed);
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_LoadL1SP();
	DRLG_L5(v2);
	DRLG_L1Pass3();
	DRLG_FreeL1SP();
	DRLG_InitL1Vals();
	DRLG_SetPC();
}
// 5CF328: using guessed type int dmaxx;
// 5CF32C: using guessed type int dmaxy;
// 5D2458: using guessed type int dminx;
// 5D245C: using guessed type int dminy;

void __cdecl DRLG_LoadL1SP()
{
	setloadflag = 0;
	if ( QuestStatus(6) )
	{
		pSetPiece = LoadFileInMem("Levels\\L1Data\\rnd6.DUN", 0);
		setloadflag = 1;
	}
	if ( QuestStatus(12) && gbMaxPlayers == 1 )
	{
		pSetPiece = LoadFileInMem("Levels\\L1Data\\SKngDO.DUN", 0);
		setloadflag = 1;
	}
	if ( QuestStatus(7) )
	{
		pSetPiece = LoadFileInMem("Levels\\L1Data\\Banner2.DUN", 0);
		setloadflag = 1;
	}
}
// 5276A4: using guessed type int setloadflag;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl DRLG_FreeL1SP()
{
	void *v0; // ecx

	v0 = pSetPiece;
	pSetPiece = 0;
	mem_free_dbg(v0);
}

void __fastcall DRLG_L5(int entry)
{
	signed int v1; // esi
	signed int v2; // edi
	int v5; // eax
	int v6; // ebx
	int v7; // edi
	int v8; // edi
	int v9; // ebp
	_BYTE *v10; // ebx
	signed int v11; // eax
	signed int v12; // ecx
	int v13; // [esp+10h] [ebp-8h]
	int v14; // [esp+10h] [ebp-8h]
	int v15; // [esp+14h] [ebp-4h]
	_BYTE *v16; // [esp+14h] [ebp-4h]

	v13 = entry;
	if ( currlevel == 1 )
	{
		v15 = 533;
	}
	else if ( currlevel == 2 )
	{
		v15 = 693;
	}
	else if ( currlevel > 2u && currlevel <= 4u )
	{
		v15 = 761;
	}
	v1 = 0;
	while ( 1 )
	{
		DRLG_InitTrans();
		do
		{
			InitL5Dungeon();
			L5firstRoom();
		}
		while ( L5GetArea() < v15 );
		L5makeDungeon();
		L5makeDmt();
		L5FillChambers();
		L5tileFix();
		L5AddWall();
		L5ClearFlags();
		DRLG_L5FloodTVal();
		v2 = 1;
		if ( QuestStatus(13) )
		{
			if ( v13 )
			{
				if ( DRLG_PlaceMiniSet(PWATERIN, 1, 1, 0, 0, 0, -1, 0) < 0 )
					v2 = 0;
				--ViewY;
			}
			else if ( DRLG_PlaceMiniSet(PWATERIN, 1, 1, 0, 0, 1, -1, 0) < 0 )
			{
				v2 = 0;
			}
		}
		if ( QuestStatus(7) )
		{
			if ( !v13 )
			{
				v5 = DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 1, -1, 0);
				goto LABEL_21;
			}
			if ( DRLG_PlaceMiniSet(STAIRSUP, 1, 1, 0, 0, 0, -1, 0) < 0 )
				v2 = 0;
			if ( v13 == 1 )
			{
				ViewX = 2 * setpc_x + 20;
				ViewY = 2 * setpc_y + 28;
				goto LABEL_34;
			}
LABEL_33:
			--ViewY;
			goto LABEL_34;
		}
		if ( v13 )
		{
			if ( DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 0, -1, 0) < 0
			  || DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 1, -1, 1) < 0 )
			{
				v2 = 0;
			}
			goto LABEL_33;
		}
		if ( DRLG_PlaceMiniSet(L5STAIRSUP, 1, 1, 0, 0, 1, -1, 0) >= 0 )
		{
			v5 = DRLG_PlaceMiniSet(STAIRSDOWN, 1, 1, 0, 0, 0, -1, 1);
LABEL_21:
			if ( v5 < 0 )
				v2 = 0;
LABEL_34:
			if ( v2 )
				break;
		}
	}
	v14 = 0;
	v6 = 16;
	do
	{
		v7 = 16;
		v16 = (unsigned char *)dungeon + v14;
		do
		{
			if ( *v16 == 64 )
			{
				DRLG_CopyTrans(v7, v6 + 1, v7, v6);
				DRLG_CopyTrans(v7 + 1, v6 + 1, v7 + 1, v6);
			}
			v16 += 40;
			v7 += 2;
		}
		while ( v7 < 96 );
		++v14;
		v6 += 2;
	}
	while ( v6 < 96 );
	DRLG_L5TransFix();
	DRLG_L5DirtFix();
	DRLG_L5CornerFix();
	v8 = 0;
	do
	{
		v9 = 0;
		v10 = (unsigned char *)mydflags + v8;
		do
		{
			if ( *v10 & 0x7F )
				DRLG_PlaceDoor(v9, v8);
			++v9;
			v10 += 40;
		}
		while ( v9 < 40 );
		++v8;
	}
	while ( v8 < 40 );
	DRLG_L5Subs();
	DRLG_L1Shadows();
	DRLG_PlaceMiniSet(LAMPS, 5, 10, 0, 0, 0, -1, 4);
	DRLG_L1Floor();
	do
	{
		v11 = v1;
		v12 = 40;
		do
		{
			pdungeon[0][v11] = dungeon[0][v11];
			v11 += 40;
			--v12;
		}
		while ( v12 );
		++v1;
	}
	while ( v1 < 40 );
	DRLG_Init_Globals();
	DRLG_CheckQuests(setpc_x, setpc_y);
}

void __fastcall DRLG_PlaceDoor(int x, int y)
{
	int v2; // edi
	char *v3; // eax
	char v4; // al
	char v5; // dl
	char *v6; // eax
	char v7; // bl
	char *v8; // [esp+Ch] [ebp-8h]

	v2 = y;
	v3 = &mydflags[x][y];
	v8 = v3;
	v4 = *v3;
	if ( v4 < 0 )
		goto LABEL_57;
	v5 = v4 & 0x7F;
	v6 = &dungeon[x][v2];
	v7 = *v6;
	if ( v5 == 1 )
	{
		if ( v2 != 1 )
		{
			if ( v7 == 2 )
				*v6 = 26;
			if ( v7 == 7 )
				*v6 = 31;
			if ( v7 == 14 )
				*v6 = 42;
			if ( v7 == 4 )
				*v6 = 43;
		}
		if ( x == 1 )
			goto LABEL_57;
		if ( v7 == 1 )
			*v6 = 25;
		if ( v7 == 10 )
			*v6 = 40;
		if ( v7 != 6 )
			goto LABEL_57;
		*v6 = 30;
	}
	if ( v5 != 2 )
		goto LABEL_36;
	if ( x != 1 )
	{
		if ( v7 == 1 )
			*v6 = 25;
		if ( v7 == 6 )
			*v6 = 30;
		if ( v7 == 10 )
			*v6 = 40;
		if ( v7 == 4 )
			*v6 = 41;
	}
	if ( v2 != 1 )
	{
		if ( v7 == 2 )
			*v6 = 26;
		if ( v7 == 14 )
			*v6 = 42;
		if ( v7 == 7 )
		{
			*v6 = 31;
LABEL_36:
			if ( v5 == 3 )
			{
				if ( x != 1 )
				{
					if ( v2 != 1 && v7 == 4 )
						*v6 = 28;
					if ( v7 == 10 )
						*v6 = 40;
				}
				if ( v2 != 1 )
				{
					if ( v7 == 14 )
						*v6 = 42;
					if ( v7 == 2 )
						*v6 = 26;
				}
				if ( x != 1 && v7 == 1 )
					*v6 = 25;
				if ( v2 != 1 && v7 == 7 )
					*v6 = 31;
				if ( x != 1 && v7 == 6 )
					*v6 = 30;
			}
			goto LABEL_57;
		}
	}
LABEL_57:
	*v8 = -128;
}

void __cdecl DRLG_L1Shadows()
{
	signed int v0; // ebx
	char *v1; // eax
	signed int v2; // edx
	unsigned char *v3; // esi
	signed int v4; // edi
	char v5; // cl
	char v6; // cl
	char v7; // cl
	char v8; // cl
	char v9; // cl
	signed int v10; // edi
	signed int v11; // eax
	signed int v12; // esi
	char v13; // cl
	char v14; // dl
	char v15; // cl
	char v16; // dl
	char v17; // cl
	char v18; // dl
	unsigned char sd[2][2];

	v0 = 1;
	do
	{
		v1 = &dungeon[0][v0 + 39];
		v2 = 1;
		do
		{
			v3 = (unsigned char *)&SPATS[0].s1;
			sd[0][0] = BSTYPES[(unsigned char)v1[1]];
			sd[1][0] = BSTYPES[(unsigned char)*(v1 - 39)];
			sd[0][1] = BSTYPES[(unsigned char)*v1];
			sd[1][1] = BSTYPES[(unsigned char)*(v1 - 40)];
			do
			{
				if ( *(v3 - 1) == sd[0][0] )
				{
					v4 = 1;
					if ( *v3 && *v3 != sd[1][1] )
						v4 = 0;
					v5 = v3[1];
					if ( v5 && v5 != sd[0][1] )
						v4 = 0;
					v6 = v3[2];
					if ( v6 && v6 != sd[1][0] )
						v4 = 0;
					if ( v4 == 1 )
					{
						v7 = v3[3];
						if ( v7 && !mydflags[v2 - 1][v0 - 1] ) /* !L5dungeon[79][v2 + 39 + v0] ) */
							*(v1 - 40) = v7;
						v8 = v3[4];
						if ( v8 && !mydflags[v2][v0 - 1] ) /* !L5dungeon[79][v2 + 79 + v0] ) */
							*v1 = v8;
						v9 = v3[5];
						if ( v9 && !mydflags[v2 - 1][v0] ) /* !L5dungeon[79][v2 + 40 + v0] ) */
							*(v1 - 39) = v9;
					}
				}
				v3 += 7;
			}
			while ( (signed int)v3 < (signed int)&SPATS[37].s1 );
			++v2;
			v1 += 40;
		}
		while ( v2 < 40 );
		++v0;
	}
	while ( v0 < 40 );
	v10 = 1;
	do
	{
		v11 = v10;
		v12 = 39;
		do
		{
			if ( dungeon[0][v11] == -117 && !mydflags[0][v11] )
			{
				v13 = dungeon[1][v11];
				v14 = -117;
				if ( v13 == 29 )
					v14 = -115;
				if ( v13 == 32 )
					v14 = -115;
				if ( v13 == 35 )
					v14 = -115;
				if ( v13 == 37 )
					v14 = -115;
				if ( v13 == 38 )
					v14 = -115;
				if ( v13 == 39 )
					v14 = -115;
				dungeon[0][v11] = v14;
			}
			if ( dungeon[0][v11] == -107 && !mydflags[0][v11] )
			{
				v15 = dungeon[1][v11];
				v16 = -107;
				if ( v15 == 29 )
					v16 = -103;
				if ( v15 == 32 )
					v16 = -103;
				if ( v15 == 35 )
					v16 = -103;
				if ( v15 == 37 )
					v16 = -103;
				if ( v15 == 38 )
					v16 = -103;
				if ( v15 == 39 )
					v16 = -103;
				dungeon[0][v11] = v16;
			}
			if ( dungeon[0][v11] == -108 && !mydflags[0][v11] )
			{
				v17 = dungeon[1][v11];
				v18 = -108;
				if ( v17 == 29 )
					v18 = -102;
				if ( v17 == 32 )
					v18 = -102;
				if ( v17 == 35 )
					v18 = -102;
				if ( v17 == 37 )
					v18 = -102;
				if ( v17 == 38 )
					v18 = -102;
				if ( v17 == 39 )
					v18 = -102;
				dungeon[0][v11] = v18;
			}
			v11 += 40;
			--v12;
		}
		while ( v12 );
		++v10;
	}
	while ( v10 < 40 );
}

int __fastcall DRLG_PlaceMiniSet(const unsigned char *miniset, int tmin, int tmax, int cx, int cy, bool setview, int noquad, int ldir)
{
	unsigned char *v8; // ebx
	int v9; // edi
	int v10; // esi
	int v11; // edx
	int v12; // esi
	int v13; // edi
	int v14; // ebx
	signed int v15; // edx
	int v16; // eax
	unsigned char v17; // cl
	int v18; // ebx
	int result; // eax
	int v20; // eax
	char *v21; // ecx
	char v22; // dl
	char v23; // bl
	bool v24; // zf
	bool v25; // sf
	unsigned char v26; // of
	int v27; // [esp-4h] [ebp-34h]
	int v28; // [esp+Ch] [ebp-24h]
	const unsigned char *v29; // [esp+10h] [ebp-20h]
	int v30; // [esp+14h] [ebp-1Ch]
	int v31; // [esp+18h] [ebp-18h]
	int v32; // [esp+1Ch] [ebp-14h]
	int v33; // [esp+20h] [ebp-10h]
	int max; // [esp+24h] [ebp-Ch]
	int v35; // [esp+28h] [ebp-8h]
	int v36; // [esp+2Ch] [ebp-4h]
	int tmaxa; // [esp+38h] [ebp+8h]
	int tmaxb; // [esp+38h] [ebp+8h]

	v8 = (unsigned char *)miniset;
	v9 = *miniset;
	v10 = tmin;
	v11 = tmax - tmin;
	v29 = miniset;
	v35 = *miniset;
	v36 = miniset[1];
	if ( v11 )
		v30 = v10 + random(0, v11);
	else
		v30 = 1;
	v31 = 0;
	if ( v30 > 0 )
	{
		max = 40 - v9;
		v28 = 40 - v36;
		while ( 1 )
		{
			v12 = random(0, max);
			v32 = 0;
			v13 = random(0, v28);
			while ( 1 )
			{
				tmaxa = 1;
				if ( cx != -1 && v12 >= cx - v35 && v12 <= cx + 12 )
				{
					++v12;
					tmaxa = 0;
				}
				if ( cy != -1 && v13 >= cy - v36 && v13 <= cy + 12 )
				{
					++v13;
					tmaxa = 0;
				}
				v14 = 0;
				switch ( noquad )
				{
					case 0:
						if ( v12 >= cx )
							goto LABEL_29;
						goto LABEL_27;
					case 1:
						if ( v12 <= cx )
							goto LABEL_29;
LABEL_27:
						if ( v13 >= cy )
							goto LABEL_29;
LABEL_28:
						tmaxa = 0;
						goto LABEL_29;
					case 2:
						if ( v12 >= cx )
							goto LABEL_29;
LABEL_22:
						if ( v13 <= cy )
							goto LABEL_29;
						goto LABEL_28;
				}
				if ( noquad == 3 && v12 > cx )
					goto LABEL_22;
LABEL_29:
				v15 = 2;
				if ( v36 > 0 )
				{
					do
					{
						if ( tmaxa != 1 )
							break;
						v33 = 0;
						if ( v35 > 0 )
						{
							v16 = v13 + v14 + 40 * v12;
							do
							{
								if ( tmaxa != 1 )
									break;
								v17 = v29[v15];
								if ( v17 && dungeon[0][v16] != v17 )
									tmaxa = 0;
								if ( mydflags[0][v16] )
									tmaxa = 0;
								++v15;
								++v33;
								v16 += 40;
							}
							while ( v33 < v35 );
						}
						++v14;
					}
					while ( v14 < v36 );
				}
				v18 = 0;
				if ( tmaxa )
					break;
				if ( ++v12 == max )
				{
					v12 = 0;
					if ( ++v13 == v28 )
						v13 = 0;
				}
				if ( ++v32 > 4000 )
					return -1;
			}
			v20 = v35 * v36 + 2;
			if ( v36 > 0 )
			{
				do
				{
					if ( v35 > 0 )
					{
						tmaxb = v35;
						v21 = &dungeon[v12][v18 + v13];
						do
						{
							v22 = v29[v20];
							if ( v22 )
								*v21 = v22;
							++v20;
							v21 += 40;
							--tmaxb;
						}
						while ( tmaxb );
					}
					++v18;
				}
				while ( v18 < v36 );
			}
			if ( ++v31 >= v30 )
			{
				v8 = (unsigned char *)v29;
				goto LABEL_57;
			}
		}
	}
	v12 = cx;
	v13 = cx;
LABEL_57:
	if ( v8 == PWATERIN )
	{
		v23 = TransVal;
		TransVal = 0;
		DRLG_MRectTrans(v12, v13 + 2, v12 + 5, v13 + 4);
		TransVal = v23;
		quests[13]._qtx = 2 * v12 + 21;
		quests[13]._qty = 2 * v13 + 22;
	}
	result = 1;
	if ( setview == 1 )
	{
		ViewX = 2 * v12 + 19;
		ViewY = 2 * v13 + 20;
	}
	if ( !ldir )
	{
		LvlViewX = 2 * v12 + 19;
		LvlViewY = 2 * v13 + 20;
	}
	v26 = __OFSUB__(v12, cx);
	v24 = v12 == cx;
	v25 = v12 - cx < 0;
	if ( v12 < cx )
	{
		if ( v13 < cy )
			return 0;
		v26 = __OFSUB__(v12, cx);
		v24 = v12 == cx;
		v25 = v12 - cx < 0;
	}
	if ( (unsigned char)(v25 ^ v26) | v24 || v13 >= cy )
	{
		if ( v12 >= cx || v13 <= cy )
			v27 = 3;
		else
			v27 = 2;
		result = v27;
	}
	return result;
}
// 5A5590: using guessed type char TransVal;
// 5CF320: using guessed type int LvlViewY;
// 5CF324: using guessed type int LvlViewX;

void __cdecl InitL5Dungeon()
{
	signed int v0; // edx
	signed int v1; // eax
	signed int v2; // ecx

	v0 = 0;
	do
	{
		v1 = v0;
		v2 = 40;
		do
		{
			dungeon[0][v1] = 0;
			mydflags[0][v1] = 0;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl L5ClearFlags()
{
	signed int v0; // ecx
	_BYTE *v1; // eax
	signed int v2; // edx

	v0 = 0;
	do
	{
		v1 = (unsigned char *)mydflags + v0;
		v2 = 40;
		do
		{
			*v1 &= 0xBFu;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl L5firstRoom()
{
	signed int v0; // ebx
	signed int v1; // ebp
	signed int i; // ecx
	char *v3; // eax
	signed int v4; // ebp
	signed int v5; // ebx
	int v6; // ebp
	char *v7; // eax

	if ( random(0, 2) )
	{
		v4 = 39;
		v5 = 1;
		HR1 = random(0, 2);
		HR2 = random(0, 2);
		HR3 = random(0, 2);
		if ( HR1 + HR3 <= 1 )
			HR2 = 1;
		if ( HR1 )
			L5drawRoom(1, 15, 10, 10);
		else
			v5 = 18;
		if ( HR2 )
			L5drawRoom(15, 15, 10, 10);
		if ( HR3 )
			L5drawRoom(29, 15, 10, 10);
		else
			v4 = 22;
		if ( v5 < v4 )
		{
			v6 = v4 - v5;
			v7 = &dungeon[v5][18];
			do
			{
				*(v7 - 1) = 1;
				*v7 = 1;
				v7[1] = 1;
				v7[2] = 1;
				v7[3] = 1;
				v7[4] = 1;
				v7 += 40;
				--v6;
			}
			while ( v6 );
		}
		if ( HR1 )
			L5roomGen(1, 15, 10, 10, 1);
		if ( HR2 )
			L5roomGen(15, 15, 10, 10, 1);
		if ( HR3 )
			L5roomGen(29, 15, 10, 10, 1);
		VR3 = 0;
		VR2 = 0;
		VR1 = 0;
	}
	else
	{
		v0 = 39;
		v1 = 1;
		VR1 = random(0, 2);
		VR2 = random(0, 2);
		VR3 = random(0, 2);
		if ( VR1 + VR3 <= 1 )
			VR2 = 1;
		if ( VR1 )
			L5drawRoom(15, 1, 10, 10);
		else
			v1 = 18;
		if ( VR2 )
			L5drawRoom(15, 15, 10, 10);
		if ( VR3 )
			L5drawRoom(15, 29, 10, 10);
		else
			v0 = 22;
		for ( i = v1; i < v0; v3[160] = 1 )
		{
			v3 = &dungeon[18][i++];
			*(v3 - 40) = 1;
			*v3 = 1;
			v3[40] = 1;
			v3[80] = 1;
			v3[120] = 1;
		}
		if ( VR1 )
			L5roomGen(15, 1, 10, 10, 0);
		if ( VR2 )
			L5roomGen(15, 15, 10, 10, 0);
		if ( VR3 )
			L5roomGen(15, 29, 10, 10, 0);
		HR3 = 0;
		HR2 = 0;
		HR1 = 0;
	}
}

void __fastcall L5drawRoom(int x, int y, int w, int h)
{
	int i; // esi
	int v5; // edi
	char *v6; // eax

	for ( i = 0; i < h; ++i )
	{
		if ( w > 0 )
		{
			v5 = w;
			v6 = &dungeon[x][i + y];
			do
			{
				*v6 = 1;
				v6 += 40;
				--v5;
			}
			while ( v5 );
		}
	}
}

void __fastcall L5roomGen(int x, int y, int w, int h, BOOL dir)
{
	int v5; // eax
	BOOL v6; // ecx
	BOOL v7; // eax
	int v8; // ecx
	int v9; // eax
	int v11; // esi
	int v12; // edi
	int v13; // ebx
	int v14; // eax
	int v15; // eax
	int v16; // eax
	int v18; // esi
	int v19; // edi
	int v20; // ebx
	int v21; // eax
	int v22; // eax
	int tya; // [esp+Ch] [ebp-10h]
	int tyb; // [esp+Ch] [ebp-10h]
	int v25; // [esp+10h] [ebp-Ch]
	int v26; // [esp+10h] [ebp-Ch]
	int txa; // [esp+14h] [ebp-8h]
	int txb; // [esp+14h] [ebp-8h]
	int v29; // [esp+18h] [ebp-4h]
	int twa; // [esp+24h] [ebp+8h]
	int tha; // [esp+28h] [ebp+Ch]
	int thb; // [esp+28h] [ebp+Ch]
	int thc; // [esp+28h] [ebp+Ch]
	signed int dir_horiza; // [esp+2Ch] [ebp+10h]
	signed int dir_horizb; // [esp+2Ch] [ebp+10h]

	v29 = y;
	txa = x;
	while ( 1 )
	{
		while ( 1 )
		{
			v5 = random(0, 4);
			v6 = 0;
			v6 = dir == 1 ? v5 != 0 : v5 == 0;
			v7 = v6;
			v8 = 0;
			if ( !v7 )
				break;
			if ( v7 != 1 )
				return;
			dir_horiza = 0;
			twa = w / 2;
			do
			{
				v9 = random(0, 5);
				v11 = (v9 + 2) & 0xFFFFFFFE;
				v12 = (random(0, 5) + 2) & 0xFFFFFFFE;
				v13 = txa + twa - v11 / 2;
				tya = v29 - v12;
				v14 = L5checkRoom(v13 - 1, v29 - v12 - 1, v11 + 2, v12 + 1);
				++dir_horiza;
				v25 = v14;
			}
			while ( !v14 && dir_horiza < 20 );
			if ( v14 == 1 )
				L5drawRoom(v13, tya, v11, v12);
			txb = v29 + h;
			v15 = L5checkRoom(v13 - 1, v29 + h, v11 + 2, v12 + 1);
			tha = v15;
			if ( v15 == 1 )
				L5drawRoom(v13, txb, v11, v12);
			if ( v25 == 1 )
				L5roomGen(v13, tya, v11, v12, 0);
			if ( tha != 1 )
				return;
			dir = 0;
			h = v12;
			w = v11;
			v29 = txb;
			txa = v13;
		}
		dir_horizb = 0;
		thb = h / 2;
		do
		{
			v16 = random(0, 5);
			v18 = (v16 + 2) & 0xFFFFFFFE;
			v19 = (random(0, 5) + 2) & 0xFFFFFFFE;
			v20 = v29 + thb - v19 / 2;
			tyb = txa - v18;
			v21 = L5checkRoom(txa - v18 - 1, v20 - 1, v19 + 2, v18 + 1);
			++dir_horizb;
			v26 = v21;
		}
		while ( !v21 && dir_horizb < 20 );
		if ( v21 == 1 )
			L5drawRoom(tyb, v20, v18, v19);
		txa += w;
		v22 = L5checkRoom(txa, v20 - 1, v18 + 1, v19 + 2);
		thc = v22;
		if ( v22 == 1 )
			L5drawRoom(txa, v20, v18, v19);
		if ( v26 == 1 )
			L5roomGen(tyb, v20, v18, v19, 1);
		if ( thc != 1 )
			break;
		dir = 1;
		h = v19;
		w = v18;
		v29 = v20;
	}
}

bool __fastcall L5checkRoom(int x, int y, int width, int height)
{
	int v4; // eax
	int v5; // ebx
	char *v6; // edi
	int v8; // [esp+Ch] [ebp-4h]

	v4 = 0;
	if ( height <= 0 )
		return 1;
	while ( 1 )
	{
		v8 = 0;
		if ( width > 0 )
			break;
LABEL_10:
		if ( ++v4 >= height )
			return 1;
	}
	v5 = x;
	v6 = &dungeon[x][v4 + y];
	while ( v5 >= 0 && v5 < 40 && v4 + y >= 0 && v4 + y < 40 && !*v6 )
	{
		++v8;
		v6 += 40;
		++v5;
		if ( v8 >= width )
			goto LABEL_10;
	}
	return 0;
}

int __cdecl L5GetArea()
{
	int rv; // eax
	signed int i; // edx
	_BYTE *v2; // ecx
	signed int j; // esi

	rv = 0;
	i = 0;
	do
	{
		v2 = (unsigned char *)dungeon + i;
		j = 40;
		do
		{
			if ( *v2 == 1 )
				++rv;
			v2 += 40;
			--j;
		}
		while ( j );
		++i;
	}
	while ( i < 40 );
	return rv;
}

void __cdecl L5makeDungeon()
{
	signed int v0; // edi
	signed int v1; // esi
	char *v2; // edx
	char v3; // cl
	int v4; // eax
	int v5; // eax

	v0 = 0;
	do
	{
		v1 = 0;
		v2 = (char *)dungeon + v0;
		do
		{
			v3 = *v2;
			v2 += 40;
			v4 = 160 * v1++;
			v5 = v4 + 2 * v0;
			L5dungeon[0][v5] = v3;
			L5dungeon[0][v5 + 1] = v3;
			L5dungeon[1][v5] = v3;
			L5dungeon[1][v5 + 1] = v3;
		}
		while ( v1 < 40 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl L5makeDmt()
{
	signed int v0; // ecx
	_BYTE *v1; // eax
	signed int v2; // edx
	signed int v3; // esi
	char (*v4)[40]; // ecx
	char *v5; // eax
	signed int v6; // edi
	int v7; // edx
	int v8; // ebx
	char (*v9)[40]; // [esp+0h] [ebp-4h]

	v0 = 0;
	do
	{
		v1 = (unsigned char *)dungeon + v0;
		v2 = 40;
		do
		{
			*v1 = 22;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
	v3 = 1;
	v9 = dungeon;
	do
	{
		v4 = v9;
		v5 = &L5dungeon[1][v3 + 1];
		v6 = 39;
		do
		{
			v7 = (unsigned char)v5[80];
			v8 = (unsigned char)*v5;
			v5 += 160;
			*(_BYTE *)v4 = L5ConvTbl[2 * ((unsigned char)*(v5 - 81) + 2 * (v8 + 2 * v7))
								   + (unsigned char)*(v5 - 161)];
			++v4;
			--v6;
		}
		while ( v6 );
		v9 = (char (*)[40])((char *)v9 + 1);
		v3 += 2;
	}
	while ( v3 <= 77 );
}

void __cdecl L5AddWall()
{
	int v0; // edi
	int v1; // esi
	int v2; // ebx
	int v3; // eax
	int v4; // eax
	int v5; // eax
	int v6; // eax
	int v7; // eax
	int v8; // eax

	v0 = 0;
	do
	{
		v1 = 0;
		v2 = v0;
		do
		{
			if ( !mydflags[0][v2] )
			{
				if ( dungeon[0][v2] == 3 )
				{
					if ( random(0, 100) < 100 )
					{
						v3 = L5HWallOk(v1, v0);
						if ( v3 != -1 )
							L5HorizWall(v1, v0, 2, v3);
					}
					if ( dungeon[0][v2] == 3 && random(0, 100) < 100 )
					{
						v4 = L5VWallOk(v1, v0);
						if ( v4 != -1 )
							L5VertWall(v1, v0, 1, v4);
					}
				}
				if ( dungeon[0][v2] == 6 && random(0, 100) < 100 )
				{
					v5 = L5HWallOk(v1, v0);
					if ( v5 != -1 )
						L5HorizWall(v1, v0, 4, v5);
				}
				if ( dungeon[0][v2] == 7 && random(0, 100) < 100 )
				{
					v6 = L5VWallOk(v1, v0);
					if ( v6 != -1 )
						L5VertWall(v1, v0, 4, v6);
				}
				if ( dungeon[0][v2] == 2 && random(0, 100) < 100 )
				{
					v7 = L5HWallOk(v1, v0);
					if ( v7 != -1 )
						L5HorizWall(v1, v0, 2, v7);
				}
				if ( dungeon[0][v2] == 1 && random(0, 100) < 100 )
				{
					v8 = L5VWallOk(v1, v0);
					if ( v8 != -1 )
						L5VertWall(v1, v0, 1, v8);
				}
			}
			++v1;
			v2 += 40;
		}
		while ( v1 < 40 );
		++v0;
	}
	while ( v0 < 40 );
}

int __fastcall L5HWallOk(int i, int j)
{
	int v2; // esi
	char *v3; // edi
	int v4; // eax
	char *v5; // ebx
	signed int wallok; // eax
	char v7; // dl
	int result; // eax
	int v9; // [esp+8h] [ebp-4h]

	v2 = 8 * (5 * i + 5);
	v9 = 1;
	v3 = (char *)dungeon + v2 + j;
	if ( *v3 == 13 )
	{
		v4 = 8 * (5 * i + 5);
		v5 = &dungeon[i + 1][j];
		do
		{
			if ( *(v3 - 1) != 13 )
				break;
			if ( dungeon[0][v2 + 1 + j] != 13 )
				break;
			if ( mydflags[0][v2 + j] )
				break;
			++v9;
			v5 += 40;
			v4 += 40;
			v3 = v5;
			v2 = v4;
		}
		while ( *v5 == 13 );
	}
	wallok = 0;
	v7 = dungeon[v9 + i][j];
	if ( (unsigned char)v7 >= 3u && (unsigned char)v7 <= 7u )
		wallok = 1;
	if ( (unsigned char)v7 >= 0x10u && (unsigned char)v7 <= 0x18u )
		wallok = 1;
	if ( v7 == 22 )
		wallok = 0;
	if ( v9 == 1 )
		wallok = 0;
	if ( wallok )
		result = v9;
	else
		result = -1;
	return result;
}

int __fastcall L5VWallOk(int i, int j)
{
	int v2; // ecx
	int result; // eax
	char *v4; // esi
	signed int wallok; // esi
	char v6; // dl

	v2 = i;
	result = 1;
	if ( dungeon[v2][j + 1] == 13 )
	{
		v4 = &dungeon[v2][j];
		do
		{
			if ( v4[result - 40] != 13 )
				break;
			if ( dungeon[v2 + 1][result + j] != 13 )
				break;
			if ( mydflags[v2][result + j] )
				break;
			++result;
		}
		while ( v4[result] == 13 );
	}
	wallok = 0;
	v6 = dungeon[0][result + v2 * 40 + j];
	if ( (unsigned char)v6 >= 3u && (unsigned char)v6 <= 7u )
		wallok = 1;
	if ( (unsigned char)v6 >= 0x10u && (unsigned char)v6 <= 0x18u )
		wallok = 1;
	if ( v6 == 22 )
		wallok = 0;
	if ( result == 1 )
		wallok = 0;
	if ( !wallok )
		result = -1;
	return result;
}

void __fastcall L5HorizWall(int i, int j, char p, int dx)
{
	int v4; // edi
	int v5; // esi
	int v6; // eax
	char v8; // bl
	int v9; // eax
	int v10; // ecx
	char *v11; // edi
	int v12; // eax
	int v13; // eax
	int v14; // eax
	int v15; // [esp+8h] [ebp-8h]
	char v16; // [esp+Fh] [ebp-1h]

	v4 = j;
	v5 = i;
	v15 = j;
	v6 = random(0, 4);
	if ( v6 >= 0 )
	{
		if ( v6 <= 1 )
		{
			v16 = 2;
		}
		else if ( v6 == 2 )
		{
			v16 = 12;
			if ( p == 2 )
				_LOBYTE(p) = 12;
			if ( p == 4 )
				_LOBYTE(p) = 10;
		}
		else if ( v6 == 3 )
		{
			v16 = 36;
			if ( p == 2 )
				_LOBYTE(p) = 36;
			if ( p == 4 )
				_LOBYTE(p) = 27;
		}
	}
	v8 = random(0, 6) != 5 ? 26 : 12;
	if ( v16 == 12 )
		v8 = 12;
	v9 = v4 + 40 * v5;
	dungeon[0][v9] = p;
	v10 = dx;
	if ( dx > 1 )
	{
		v11 = &dungeon[1][v9];
		v12 = dx - 1;
		do
		{
			*v11 = v16;
			v11 += 40;
			--v12;
		}
		while ( v12 );
		v4 = v15;
	}
	v13 = random(0, dx - 1) + 1;
	if ( v8 == 12 )
	{
		dungeon[v5 + v13][v4] = 12;
	}
	else
	{
		v14 = v4 + 40 * (v5 + v13);
		mydflags[0][v14] |= 1u;
		dungeon[0][v14] = 2;
	}
}

void __fastcall L5VertWall(int i, int j, char p, int dy)
{
	int v4; // edi
	int v5; // esi
	int v6; // eax
	int v8; // eax
	int v9; // ebx
	int v10; // esi
	int v11; // ecx
	char *v12; // edi
	int v13; // eax
	unsigned int v14; // ecx
	int v15; // edx
	int v16; // eax
	int v17; // eax
	int v18; // [esp+8h] [ebp-8h]
	char v19; // [esp+Eh] [ebp-2h]
	char v20; // [esp+Fh] [ebp-1h]

	v4 = j;
	v5 = i;
	v18 = j;
	v6 = random(0, 4);
	if ( v6 >= 0 )
	{
		if ( v6 <= 1 )
		{
			v20 = 1;
		}
		else if ( v6 == 2 )
		{
			v20 = 11;
			if ( p == 1 )
				_LOBYTE(p) = 11;
			if ( p == 4 )
				_LOBYTE(p) = 14;
		}
		else if ( v6 == 3 )
		{
			v20 = 35;
			if ( p == 1 )
				_LOBYTE(p) = 35;
			if ( p == 4 )
				_LOBYTE(p) = 37;
		}
	}
	v8 = random(0, 6);
	v9 = 5 - v8;
	_LOBYTE(v9) = v8 != 5 ? 25 : 11;
	v19 = v8 != 5 ? 25 : 11;
	if ( v20 == 11 )
	{
		_LOBYTE(v9) = 11;
		v19 = 11;
	}
	v10 = v5;
	dungeon[v10][v4] = p;
	v11 = dy;
	if ( dy > 1 )
	{
		v12 = &dungeon[v10][v4 + 1];
		_LOBYTE(v9) = v20;
		BYTE1(v9) = v20;
		v13 = v9 << 16;
		_LOWORD(v13) = v9;
		_LOBYTE(v9) = v19;
		v14 = (unsigned int)(dy - 1) >> 2;
		memset32(v12, v13, v14);
		memset(&v12[4 * v14], v13, ((_BYTE)dy - 1) & 3);
		v11 = dy;
		v4 = v18;
	}
	v15 = v11 - 1;
	v16 = random(0, v15) + 1;
	if ( (_BYTE)v9 == 11 )
	{
		dungeon[0][v16 + v10 * 40 + v4] = 11;
	}
	else
	{
		v17 = v16 + v10 * 40 + v4;
		mydflags[0][v17] |= 2u;
		dungeon[0][v17] = 1;
	}
}

void __cdecl L5tileFix()
{
	signed int v0; // esi
	char *v1; // eax
	signed int v2; // edx
	char v3; // cl
	signed int v4; // ecx
	signed int v5; // edi
	signed int v6; // eax
	char *v7; // esi
	char v8; // bl
	char *v9; // edx
	char *v10; // edx
	char *v11; // edx
	char *v12; // edx
	char *v13; // edx
	char *v14; // edx
	char *v15; // edx
	char *v16; // edx
	char *v17; // edx
	char *v18; // edx
	char *v19; // edx
	char *v20; // edx
	char *v21; // edx
	char *v22; // edx
	char *v23; // edx
	char *v24; // edx
	char *v25; // edx
	char *v26; // edx
	char *v27; // edx
	char *v28; // edx
	char *v29; // edx
	char *v30; // edx
	char *v31; // edx
	char *v32; // edx
	char *v33; // edx
	char *v34; // eax
	signed int v35; // edx
	char *v36; // eax
	signed int v37; // esi
	char v38; // cl

	v0 = 0;
	do
	{
		v1 = &dungeon[1][v0];
		v2 = 40;
		do
		{
			v3 = *(v1 - 40);
			if ( v3 == 2 && *v1 == 22 )
				*v1 = 23;
			if ( v3 == 13 )
			{
				if ( *v1 == 22 )
					*v1 = 18;
				if ( *v1 == 2 )
					*v1 = 7;
			}
			if ( v3 == 6 && *v1 == 22 )
				*v1 = 24;
			if ( v3 == 1 && *(v1 - 39) == 22 )
				*(v1 - 39) = 24;
			if ( v3 == 13 )
			{
				if ( *(v1 - 39) == 1 )
					*(v1 - 39) = 6;
				if ( *(v1 - 39) == 22 )
					*(v1 - 39) = 19;
			}
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
	v4 = 0;
	do
	{
		v5 = 0;
		do
		{
			v6 = v5;
			v7 = &dungeon[v5][v4];
			v8 = *v7;
			if ( *v7 == 13 )
			{
				v9 = &dungeon[v6 + 1][v4];
				if ( *v9 == 19 )
					*v9 = 21;
				v10 = &dungeon[v6 + 1][v4];
				if ( *v10 == 22 )
					*v10 = 20;
			}
			if ( v8 == 7 )
			{
				v11 = &dungeon[v6 + 1][v4];
				if ( *v11 == 22 )
					*v11 = 23;
			}
			if ( v8 == 13 )
			{
				v12 = &dungeon[v6 + 1][v4];
				if ( *v12 == 24 )
					*v12 = 21;
			}
			if ( v8 == 19 )
			{
				v13 = &dungeon[v6 + 1][v4];
				if ( *v13 == 22 )
					*v13 = 20;
			}
			if ( v8 == 2 )
			{
				v14 = &dungeon[v6 + 1][v4];
				if ( *v14 == 19 )
					*v14 = 21;
			}
			if ( v8 == 19 )
			{
				v15 = &dungeon[v6 + 1][v4];
				if ( *v15 == 1 )
					*v15 = 6;
			}
			if ( v8 == 7 )
			{
				v16 = &dungeon[v6 + 1][v4];
				if ( *v16 == 19 )
					*v16 = 21;
			}
			if ( v8 == 2 )
			{
				v17 = &dungeon[v6 + 1][v4];
				if ( *v17 == 1 )
					*v17 = 6;
			}
			if ( v8 == 3 )
			{
				v18 = &dungeon[v6 + 1][v4];
				if ( *v18 == 22 )
					*v18 = 24;
			}
			if ( v8 == 21 )
			{
				v19 = &dungeon[v6 + 1][v4];
				if ( *v19 == 1 )
					*v19 = 6;
			}
			if ( v8 == 7 )
			{
				v20 = &dungeon[v6 + 1][v4];
				if ( *v20 == 1 )
					*v20 = 6;
				v21 = &dungeon[v6 + 1][v4];
				if ( *v21 == 24 )
					*v21 = 21;
			}
			if ( v8 == 4 )
			{
				v22 = &dungeon[v6 + 1][v4];
				if ( *v22 == 16 )
					*v22 = 17;
			}
			if ( v8 == 7 )
			{
				v23 = &dungeon[v6 + 1][v4];
				if ( *v23 == 13 )
					*v23 = 17;
			}
			if ( v8 == 2 )
			{
				v24 = &dungeon[v6 + 1][v4];
				if ( *v24 == 24 )
					*v24 = 21;
				v25 = &dungeon[v6 + 1][v4];
				if ( *v25 == 13 )
					*v25 = 17;
			}
			if ( v8 == 23 && *(v7 - 40) == 22 )
				*(v7 - 40) = 19;
			if ( v8 == 19 && *(v7 - 40) == 23 )
				*(v7 - 40) = 21;
			if ( v8 == 6 )
			{
				if ( *(v7 - 40) == 22 )
					*(v7 - 40) = 24;
				if ( *(v7 - 40) == 23 )
					*(v7 - 40) = 21;
			}
			if ( v8 == 1 )
			{
				v26 = &dungeon[v6][v4 + 1];
				if ( *v26 == 2 )
					*v26 = 7;
			}
			if ( v8 == 6 )
			{
				v27 = &dungeon[v6][v4 + 1];
				if ( *v27 == 18 )
					*v27 = 21;
			}
			if ( v8 == 18 )
			{
				v28 = &dungeon[v6][v4 + 1];
				if ( *v28 == 2 )
					*v28 = 7;
			}
			if ( v8 == 6 )
			{
				v29 = &dungeon[v6][v4 + 1];
				if ( *v29 == 2 )
					*v29 = 7;
			}
			if ( v8 == 21 )
			{
				v30 = &dungeon[v6][v4 + 1];
				if ( *v30 == 2 )
					*v30 = 7;
			}
			if ( v8 == 6 )
			{
				v31 = &dungeon[v6][v4 + 1];
				if ( *v31 == 22 )
					*v31 = 24;
				v32 = &dungeon[v6][v4 + 1];
				if ( *v32 == 13 )
					*v32 = 16;
			}
			if ( v8 == 1 )
			{
				v33 = &dungeon[v6][v4 + 1];
				if ( *v33 == 13 )
					*v33 = 16;
			}
			if ( v8 == 13 )
			{
				v34 = &dungeon[v6][v4 + 1];
				if ( *v34 == 16 )
					*v34 = 17;
			}
			if ( v8 == 6 )
			{
				if ( *(v7 - 1) == 22 )
					*(v7 - 1) = 7;
				if ( *(v7 - 1) == 22 )
					*(v7 - 1) = 24;
			}
			if ( v8 == 7 && *(v7 - 1) == 24 )
				*(v7 - 1) = 21;
			if ( v8 == 18 && *(v7 - 1) == 24 )
				*(v7 - 1) = 21;
			++v5;
		}
		while ( v5 < 40 );
		++v4;
	}
	while ( v4 < 40 );
	v35 = 0;
	do
	{
		v36 = (char *)dungeon + v35;
		v37 = 40;
		do
		{
			v38 = *v36;
			if ( *v36 == 4 && v36[1] == 2 )
				v36[1] = 7;
			if ( v38 == 2 && v36[40] == 19 )
				v36[40] = 21;
			if ( v38 == 18 && v36[1] == 22 )
				v36[1] = 20;
			v36 += 40;
			--v37;
		}
		while ( v37 );
		++v35;
	}
	while ( v35 < 40 );
}

void __cdecl DRLG_L5Subs()
{
	signed int v0; // edi
	int v1; // esi
	unsigned char v2; // bl
	int v3; // eax
	signed int v4; // ecx
	signed int v5; // [esp+Ch] [ebp-4h]

	v0 = 0;
	do
	{
		v1 = v0 - 1;
		v5 = 40;
		do
		{
			if ( !random(0, 4) )
			{
				v2 = L5BTYPES[(unsigned char)dungeon[0][v1 + 1]];
				if ( v2 )
				{
					if ( !mydflags[0][v1 + 1] )
					{
						v3 = random(0, 16);
						v4 = -1;
						if ( v3 >= 0 )
						{
							do
							{
								if ( ++v4 == 206 )
									v4 = 0;
								if ( v2 == L5BTYPES[v4] )
									--v3;
							}
							while ( v3 >= 0 );
							if ( v4 == 89 )
							{
								if ( L5BTYPES[(unsigned char)dungeon[0][v1]] == 79 && !mydflags[0][v1] )
								{
									dungeon[0][v1] = 90;
									goto LABEL_22;
								}
								v4 = 79;
							}
							if ( v4 == 91 )
							{
								if ( L5BTYPES[(unsigned char)dungeon[1][v1 + 1]] != 80 || mydflags[1][v1 + 1] )
									_LOBYTE(v4) = 80;
								else
									dungeon[1][v1 + 1] = 92;
							}
						}
LABEL_22:
						dungeon[0][v1 + 1] = v4;
						goto LABEL_23;
					}
				}
			}
LABEL_23:
			v1 += 40;
			--v5;
		}
		while ( v5 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl L5FillChambers()
{
	int v0; // edi
	int v1; // edi
	int v2; // edx
	int v3; // ecx
	int v4; // edi
	signed int v5; // [esp-4h] [ebp-10h]

	v0 = 1;
	if ( HR1 )
		DRLG_L5GChamber(0, 14, 0, 0, 0, 1);
	if ( !HR2 )
		goto LABEL_16;
	if ( HR1 )
	{
		if ( !HR3 )
			DRLG_L5GChamber(14, 14, 0, 0, 1, 0);
		if ( HR1 )
			goto LABEL_111;
	}
	if ( HR3 )
		DRLG_L5GChamber(14, 14, 0, 0, 0, 1);
	if ( HR1 )
	{
LABEL_111:
		if ( HR3 )
			DRLG_L5GChamber(14, 14, 0, 0, 1, 1);
		if ( HR1 )
		{
LABEL_16:
			if ( !HR3 )
				goto LABEL_18;
			goto LABEL_17;
		}
	}
	if ( !HR3 )
	{
		DRLG_L5GChamber(14, 14, 0, 0, 0, 0);
		goto LABEL_16;
	}
LABEL_17:
	DRLG_L5GChamber(28, 14, 0, 0, 1, 0);
LABEL_18:
	if ( HR1 )
	{
		if ( !HR2 )
			goto LABEL_24;
		DRLG_L5GHall(12, 18, 14, 18);
	}
	if ( HR2 && HR3 )
		DRLG_L5GHall(26, 18, 28, 18);
LABEL_24:
	if ( HR1 && !HR2 && HR3 )
		DRLG_L5GHall(12, 18, 28, 18);
	if ( VR1 )
		DRLG_L5GChamber(14, 0, 0, 1, 0, 0);
	if ( !VR2 )
		goto LABEL_43;
	if ( VR1 )
	{
		if ( !VR3 )
			DRLG_L5GChamber(14, 14, 1, 0, 0, 0);
		if ( VR1 )
			goto LABEL_112;
	}
	if ( VR3 )
		DRLG_L5GChamber(14, 14, 0, 1, 0, 0);
	if ( VR1 )
	{
LABEL_112:
		if ( VR3 )
			DRLG_L5GChamber(14, 14, 1, 1, 0, 0);
		if ( VR1 )
		{
LABEL_43:
			if ( !VR3 )
				goto LABEL_45;
			goto LABEL_44;
		}
	}
	if ( !VR3 )
	{
		DRLG_L5GChamber(14, 14, 0, 0, 0, 0);
		goto LABEL_43;
	}
LABEL_44:
	DRLG_L5GChamber(14, 28, 1, 0, 0, 0);
LABEL_45:
	if ( VR1 )
	{
		if ( !VR2 )
			goto LABEL_51;
		DRLG_L5GHall(18, 12, 18, 14);
	}
	if ( VR2 && VR3 )
		DRLG_L5GHall(18, 26, 18, 28);
LABEL_51:
	if ( VR1 && !VR2 && VR3 )
		DRLG_L5GHall(18, 12, 18, 28);
	if ( setloadflag )
	{
		if ( !VR1 && !VR2 && !VR3 )
		{
			if ( HR1 )
				goto LABEL_113;
			if ( HR2 && HR3 )
			{
				if ( random(0, 2) )
					v0 = 2;
				if ( HR1 )
				{
LABEL_113:
					if ( HR2 && !HR3 && random(0, 2) )
						v0 = 0;
					if ( HR1 )
					{
						if ( !HR2 && HR3 )
							v0 = random(0, 2) != 0 ? 0 : 2;
						if ( HR1 && HR2 )
						{
							if ( HR3 )
								v0 = random(0, 3);
						}
					}
				}
			}
			if ( !v0 )
			{
				v3 = 2;
				v2 = 16;
				goto LABEL_108;
			}
			v1 = v0 - 1;
			if ( v1 )
			{
				if ( v1 != 1 )
					return;
				v2 = 16;
				v5 = 30;
				goto LABEL_107;
			}
LABEL_81:
			v3 = 16;
			v2 = 16;
LABEL_108:
			DRLG_L5SetRoom(v3, v2);
			return;
		}
		if ( VR1 )
			goto LABEL_114;
		if ( VR2 && VR3 )
		{
			if ( random(0, 2) )
				v0 = 2;
			if ( VR1 )
			{
LABEL_114:
				if ( VR2 && !VR3 && random(0, 2) )
					v0 = 0;
				if ( VR1 )
				{
					if ( !VR2 && VR3 )
						v0 = random(0, 2) != 0 ? 0 : 2;
					if ( VR1 && VR2 && VR3 )
						v0 = random(0, 3);
				}
			}
		}
		if ( v0 )
		{
			v4 = v0 - 1;
			if ( !v4 )
				goto LABEL_81;
			if ( v4 != 1 )
				return;
			v2 = 30;
		}
		else
		{
			v2 = 2;
		}
		v5 = 16;
LABEL_107:
		v3 = v5;
		goto LABEL_108;
	}
}
// 5276A4: using guessed type int setloadflag;

void __fastcall DRLG_L5GChamber(int sx, int sy, bool topflag, bool bottomflag, bool leftflag, bool rightflag)
{
	int v6; // eax
	int v7; // edx
	int v8; // eax
	char *v9; // eax
	int v10; // eax
	int v11; // ecx
	int v12; // eax
	char *v13; // eax
	signed int v14; // edi
	int v15; // eax
	int v16; // edx
	int v17; // ecx
	signed int v18; // esi

	if ( topflag == 1 )
	{
		v6 = sy + 40 * sx;
		dungeon[2][v6] = 12;
		dungeon[3][v6] = 12;
		dungeon[4][v6] = 3;
		dungeon[7][v6] = 9;
		dungeon[8][v6] = 12;
		dungeon[9][v6] = 2;
	}
	if ( bottomflag == 1 )
	{
		v7 = sy + 11;
		v8 = v7 + 40 * sx;
		dungeon[2][v8] = 10;
		dungeon[3][v8] = 12;
		dungeon[4][v8] = 8;
		dungeon[7][v8] = 5;
		dungeon[8][v8] = 12;
		v9 = &dungeon[9][v8];
		if ( *v9 != 4 )
			*v9 = 21;
		sy = v7 - 11;
	}
	if ( leftflag == 1 )
	{
		v10 = sy + 40 * sx;
		dungeon[0][v10 + 2] = 11;
		dungeon[0][v10 + 3] = 11;
		dungeon[0][v10 + 4] = 3;
		dungeon[0][v10 + 7] = 8;
		dungeon[0][v10 + 8] = 11;
		dungeon[0][v10 + 9] = 1;
	}
	if ( rightflag == 1 )
	{
		v11 = sx + 11;
		v12 = sy + 40 * v11;
		dungeon[0][v12 + 2] = 14;
		dungeon[0][v12 + 3] = 11;
		dungeon[0][v12 + 4] = 9;
		dungeon[0][v12 + 7] = 5;
		dungeon[0][v12 + 8] = 11;
		v13 = &dungeon[0][v12 + 9];
		if ( *v13 != 4 )
			*v13 = 21;
		sx = v11 - 11;
	}
	v14 = 10;
	v15 = sy + 40 * sx;
	v16 = v15 + 1;
	do
	{
		v17 = v16;
		v18 = 10;
		do
		{
			mydflags[1][v17] |= 0x40u;
			dungeon[1][v17] = 13;
			v17 += 40;
			--v18;
		}
		while ( v18 );
		++v16;
		--v14;
	}
	while ( v14 );
	dungeon[4][v15 + 4] = 15;
	dungeon[7][v15 + 4] = 15;
	dungeon[4][v15 + 7] = 15;
	dungeon[7][v15 + 7] = 15;
}

void __fastcall DRLG_L5GHall(int x1, int y1, int x2, int y2)
{
	int v4; // eax
	char *v5; // edx
	int v6; // eax
	char *v7; // ecx

	if ( y1 == y2 )
	{
		if ( x1 < x2 )
		{
			v4 = x2 - x1;
			v5 = &dungeon[x1][y1 + 3];
			do
			{
				*(v5 - 3) = 12;
				*v5 = 12;
				v5 += 40;
				--v4;
			}
			while ( v4 );
		}
	}
	else
	{
		v6 = y1;
		if ( y1 < y2 )
		{
			v7 = dungeon[x1 + 3];
			do
			{
				v7[v6 - 120] = 11;
				v7[v6++] = 11;
			}
			while ( v6 < y2 );
		}
	}
}

void __fastcall DRLG_L5SetRoom(int rx1, int ry1)
{
	int v2; // edi
	int v3; // esi
	int v4; // eax
	char v5; // bl
	int v6; // [esp+8h] [ebp-Ch]
	char *v7; // [esp+Ch] [ebp-8h]
	int v8; // [esp+10h] [ebp-4h]

	v8 = 0;
	v2 = *((unsigned char *)pSetPiece + 2);
	v3 = *(unsigned char *)pSetPiece;
	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = v3;
	setpc_h = v2;
	v7 = (char *)pSetPiece + 4;
	if ( v2 > 0 )
	{
		do
		{
			if ( v3 > 0 )
			{
				v6 = v3;
				v4 = ry1 + v8 + 40 * rx1;
				do
				{
					v5 = *v7;
					if ( *v7 )
					{
						mydflags[0][v4] |= 0x80u;
						dungeon[0][v4] = v5;
					}
					else
					{
						dungeon[0][v4] = 13;
					}
					v7 += 2;
					v4 += 40;
					--v6;
				}
				while ( v6 );
			}
			++v8;
		}
		while ( v8 < v2 );
	}
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __cdecl DRLG_L5FloodTVal()
{
	int v0; // ebx
	int v1; // esi
	char *v2; // edi
	_BYTE *v3; // [esp+Ch] [ebp-Ch]
	signed int x; // [esp+10h] [ebp-8h]
	signed int tx; // [esp+14h] [ebp-4h]

	v0 = 16;
	v1 = 0;
	do
	{
		tx = 0;
		x = 16;
		v2 = &dung_map[16][v0];
		v3 = (unsigned char *)dungeon + v1;
		do
		{
			if ( *v3 == 13 && !*v2 )
			{
				DRLG_L5FTVR(tx, v1, x, v0, 0);
				++TransVal;
			}
			x += 2;
			v3 += 40;
			v2 += 224;
			++tx;
		}
		while ( tx < 40 );
		v0 += 2;
		++v1;
	}
	while ( v1 < 40 );
}
// 5A5590: using guessed type char TransVal;

void __fastcall DRLG_L5FTVR(int i, int j, int x, int y, int d)
{
	int v5; // ebx
	int v6; // esi
	int v7; // edi
	int v8; // edx
	int v9; // ecx
	int v10; // ebx
	int v11; // eax
	int v12; // edi
	char v13; // al
	char v14; // al
	int v15; // ecx
	int v16; // ecx
	int v17; // ecx
	int v18; // ecx
	int v19; // [esp+Ch] [ebp-14h]
	int v20; // [esp+10h] [ebp-10h]
	int v21; // [esp+14h] [ebp-Ch]
	int tya; // [esp+18h] [ebp-8h]
	int txa; // [esp+1Ch] [ebp-4h]
	int ya; // [esp+2Ch] [ebp+Ch]

	v5 = x;
	v6 = y;
	v7 = j;
	v8 = i;
	v9 = 112 * x + y;
	tya = v7;
	v21 = v8;
	if ( !dung_map[0][v9] )
	{
		v19 = x;
		txa = v8 - 1;
		v10 = x - 2;
		v11 = 40 * v8;
		ya = v7 - 1;
		v12 = v6 - 2;
		for ( v20 = 40 * v8; dungeon[0][v11 + tya] == 13; v11 = v20 )
		{
			v13 = TransVal;
			dung_map[0][v9] = TransVal;
			dung_map[1][v9] = v13;
			dung_map[0][v9 + 1] = v13;
			dung_map[1][v9 + 1] = v13;
			DRLG_L5FTVR(txa + 2, tya, v10 + 4, v6, 1);
			DRLG_L5FTVR(txa, tya, v10, v6, 2);
			DRLG_L5FTVR(v21, ya + 2, x, v12 + 4, 3);
			DRLG_L5FTVR(v21, ya, x, v12, 4);
			DRLG_L5FTVR(txa, ya, v10, v12, 5);
			DRLG_L5FTVR(txa + 2, ya, v10 + 4, v12, 6);
			DRLG_L5FTVR(txa, ya + 2, v10, v12 + 4, 7);
			v19 += 2;
			v20 += 40;
			d = 8;
			x += 2;
			v6 += 2;
			v12 += 2;
			v10 += 2;
			++tya;
			++ya;
			++v21;
			++txa;
			v9 = v19 * 112 + v6;
			if ( dung_map[v19][v6] )
				break;
		}
		v5 = x;
	}
	v14 = TransVal;
	if ( d == 1 )
	{
		v15 = v6 + 112 * v5;
		dung_map[0][v15] = TransVal;
		dung_map[0][v15 + 1] = v14;
	}
	if ( d == 2 )
	{
		v16 = v6 + 112 * v5;
		dung_map[1][v16] = v14;
		dung_map[1][v16 + 1] = v14;
	}
	if ( d == 3 )
	{
		v17 = v6 + 112 * v5;
		dung_map[0][v17] = v14;
		dung_map[1][v17] = v14;
	}
	if ( d == 4 )
	{
		v18 = v6 + 112 * v5;
		dung_map[0][v18 + 1] = v14;
		dung_map[1][v18 + 1] = v14;
	}
	if ( d == 5 )
		dung_map[v5 + 1][v6 + 1] = v14;
	if ( d == 6 )
		dung_map[v5][v6 + 1] = v14;
	if ( d == 7 )
		dung_map[v5 + 1][v6] = v14;
	if ( d == 8 )
		dung_map[v5][v6] = v14;
}
// 5A5590: using guessed type char TransVal;

void __cdecl DRLG_L5TransFix()
{
	signed int v0; // esi
	char *v1; // eax
	char *v2; // ecx
	signed int v3; // edi
	char v4; // bl
	char v5; // dl
	char v6; // dl
	char v7; // dl
	char v8; // dl
	char v9; // dl
	char *v10; // [esp+Ch] [ebp-4h]

	v0 = 0;
	v10 = &dung_map[16][16]; /* check */
	do
	{
		v1 = v10;
		v2 = (char *)dungeon + v0;
		v3 = 40;
		do
		{
			v4 = *v2;
			if ( *v2 == 23 && *(v2 - 1) == 18 )
			{
				v5 = *v1;
				v1[112] = *v1;
				v1[113] = v5;
			}
			if ( v4 == 24 && v2[40] == 19 )
			{
				v6 = *v1;
				v1[1] = *v1;
				v1[113] = v6;
			}
			if ( v4 == 18 )
			{
				v7 = *v1;
				v1[112] = *v1;
				v1[113] = v7;
			}
			if ( v4 == 19 )
			{
				v8 = *v1;
				v1[1] = *v1;
				v1[113] = v8;
			}
			if ( v4 == 20 )
			{
				v9 = *v1;
				v1[112] = *v1;
				v1[1] = v9;
				v1[113] = v9;
			}
			v1 += 224;
			v2 += 40;
			--v3;
		}
		while ( v3 );
		v10 += 2;
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl DRLG_L5DirtFix()
{
	signed int v0; // ecx
	char *v1; // eax
	signed int v2; // edx

	v0 = 0;
	do
	{
		v1 = (char *)dungeon + v0;
		v2 = 40;
		do
		{
			if ( *v1 == 21 && v1[40] != 19 )
				*v1 = -54;
			if ( *v1 == 19 && v1[40] != 19 )
				*v1 = -56;
			if ( *v1 == 24 && v1[40] != 19 )
				*v1 = -51;
			if ( *v1 == 18 && v1[1] != 18 )
				*v1 = -57;
			if ( *v1 == 21 && v1[1] != 18 )
				*v1 = -54;
			if ( *v1 == 23 && v1[1] != 18 )
				*v1 = -52;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl DRLG_L5CornerFix()
{
	signed int v0; // esi
	signed int v1; // eax
	signed int v2; // edi

	v0 = 1;
	do
	{
		v1 = v0;
		v2 = 38;
		do
		{
			if ( mydflags[1][v1] >= 0 && dungeon[1][v1] == 17 && dungeon[0][v1] == 13 && dungeon[0][v1 + 39] == 1 )
			{
				mydflags[0][v1 + 39] &= 0x80u;
				dungeon[1][v1] = 16;
			}
			if ( dungeon[1][v1] == -54 && dungeon[2][v1] == 13 && dungeon[1][v1 + 1] == 1 )
				dungeon[1][v1] = 8;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 39 );
}
