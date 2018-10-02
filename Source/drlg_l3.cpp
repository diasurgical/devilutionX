//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
char lavapool; // weak
int abyssx; // weak
int lockoutcnt; // weak
char lockout[40][40];
#endif

const unsigned char L3ConvTbl[16] = { 8, 11, 3, 10, 1, 9, 12, 12, 6, 13, 4, 13, 2, 14, 5, 7 };
const unsigned char L3UP[20] = { 3, 3, 8, 8, 0, 10, 10, 0, 7, 7, 0, 51, 50, 0, 48, 49, 0, 0, 0, 0 };
const unsigned char L3DOWN[20] = { 3, 3, 8, 9, 7, 8, 9, 7, 0, 0, 0, 0, 47, 0, 0, 46, 0, 0, 0, 0 };
const unsigned char L3HOLDWARP[20] = { 3, 3, 8, 8, 0, 10, 10, 0, 7, 7, 0, 125, 125, 0, 125, 125, 0, 0, 0, 0 };
const unsigned char L3TITE1[34] = { 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 57, 58, 0, 0, 56, 55, 0, 0, 0, 0, 0 };
const unsigned char L3TITE2[34] = { 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 61, 62, 0, 0, 60, 59, 0, 0, 0, 0, 0 };
const unsigned char L3TITE3[34] = { 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 65, 66, 0, 0, 64, 63, 0, 0, 0, 0, 0 };
const unsigned char L3TITE6[42] = { 5, 4, 7, 7, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 77, 78, 0, 0, 0, 76, 74, 75, 0, 0, 0, 0, 0, 0 };
const unsigned char L3TITE7[42] = { 4, 5, 7, 7, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 83, 0, 0, 0, 82, 80, 0, 0, 81, 79, 0, 0, 0, 0, 0 };
const unsigned char L3TITE8[20] = { 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 52, 0, 0, 0, 0 };
const unsigned char L3TITE9[20] = { 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 53, 0, 0, 0, 0 };
const unsigned char L3TITE10[20] = { 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 54, 0, 0, 0, 0 };
const unsigned char L3TITE11[20] = { 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 67, 0, 0, 0, 0 };
const unsigned char L3TITE12[6] = { 2u, 1u, 9u, 7u, 68u, 0u };
const unsigned char L3TITE13[6] = { 1u, 2u, 10u, 7u, 69u, 0u };
const unsigned char L3CREV1[6] = { 2u, 1u, 8u, 7u, 84u, 85u };
const unsigned char L3CREV2[6] = { 2u, 1u, 8u, 11u, 86u, 87u };
const unsigned char L3CREV3[6] = { 1u, 2u, 8u, 10u, 89u, 88u };
const unsigned char L3CREV4[6] = { 2u, 1u, 8u, 7u, 90u, 91u };
const unsigned char L3CREV5[6] = { 1u, 2u, 8u, 11u, 92u, 93u };
const unsigned char L3CREV6[6] = { 1u, 2u, 8u, 10u, 95u, 94u };
const unsigned char L3CREV7[6] = { 2u, 1u, 8u, 7u, 96u, 101u };
const unsigned char L3CREV8[6] = { 1u, 2u, 2u, 8u, 102u, 97u };
const unsigned char L3CREV9[6] = { 2u, 1u, 3u, 8u, 103u, 98u };
const unsigned char L3CREV10[6] = { 2u, 1u, 4u, 8u, 104u, 99u };
const unsigned char L3CREV11[6] = { 1u, 2u, 6u, 8u, 105u, 100u };
const unsigned char L3ISLE1[14] = { 2u, 3u, 5u, 14u, 4u, 9u, 13u, 12u, 7u, 7u, 7u, 7u, 7u, 7u };
const unsigned char L3ISLE2[14] = { 3u, 2u, 5u, 2u, 14u, 13u, 10u, 12u, 7u, 7u, 7u, 7u, 7u, 7u };
const unsigned char L3ISLE3[14] = { 2u, 3u, 5u, 14u, 4u, 9u, 13u, 12u, 29u, 30u, 25u, 28u, 31u, 32u };
const unsigned char L3ISLE4[14] = { 3u, 2u, 5u, 2u, 14u, 13u, 10u, 12u, 29u, 26u, 30u, 31u, 27u, 32u };
const unsigned char L3ISLE5[10] = { 2u, 2u, 5u, 14u, 13u, 12u, 7u, 7u, 7u, 7u };
const unsigned char L3XTRA1[4] = { 1u, 1u, 7u, 106u };
const unsigned char L3XTRA2[4] = { 1u, 1u, 7u, 107u };
const unsigned char L3XTRA3[4] = { 1u, 1u, 7u, 108u };
const unsigned char L3XTRA4[4] = { 1u, 1u, 9u, 109u };
const unsigned char L3XTRA5[4] = { 1u, 1u, 10u, 110u };
const unsigned char L3ANVIL[244] =
{
   11,  11,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
	7,   7,   7,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,  29,  26,  26,  26,
   26,  26,  30,   0,   0,   0,  29,  34,  33,  33,
   37,  36,  33,  35,  30,   0,   0,  25,  33,  37,
   27,  32,  31,  36,  33,  28,   0,   0,  25,  37,
   32,   7,   7,   7,  31,  27,  32,   0,   0,  25,
   28,   7,   7,   7,   7,   2,   2,   2,   0,   0,
   25,  35,  30,   7,   7,   7,  29,  26,  30,   0,
	0,  25,  33,  35,  26,  30,  29,  34,  33,  28,
	0,   0,  31,  36,  33,  33,  35,  34,  33,  37,
   32,   0,   0,   0,  31,  27,  27,  27,  27,  27,
   32,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0
};
const unsigned char L3SpawnTbl1[15] = { 0u, 10u, 67u, 5u, 44u, 6u, 9u, 0u, 0u, 28u, 131u, 6u, 9u, 10u, 5u }; /* local spawntable? */
const unsigned char L3SpawnTbl2[15] = { 0u, 10u, 3u, 5u, 12u, 6u, 9u, 0u, 0u, 12u, 3u, 6u, 9u, 10u, 5u }; /* local spawntable? */
const unsigned char L3PoolSub[15] = { 0u, 35u, 26u, 36u, 25u, 29u, 34u, 7u, 33u, 28u, 27u, 37u, 32u, 31u, 30u }; /* local poolsub? */

void __cdecl AddFenceDoors()
{
	signed int v0; // esi
	char *v1; // eax
	signed int v2; // ebx
	unsigned char v3; // cl
	char v4; // cl
	char v5; // cl

	v0 = 0;
	do
	{
		v1 = &dungeon[-1][v0 + 39];
		v2 = 40;
		do
		{
			if ( v1[1] == 7 )
			{
				v3 = *(v1 - 39);
				if ( v3 > 0x98u
				  || v3 < 0x82u
				  || (v4 = v1[41], (unsigned char)v4 > 0x98u)
				  || (unsigned char)v4 < 0x82u )
				{
					if ( (unsigned char)*v1 <= 0x98u && (unsigned char)*v1 >= 0x82u )
					{
						v5 = v1[2];
						if ( (unsigned char)v5 <= 0x98u && (unsigned char)v5 >= 0x82u )
							v1[1] = -109;
					}
				}
				else
				{
					v1[1] = -110;
				}
			}
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl FenceDoorFix()
{
	signed int v0; // edi
	char *v1; // eax
	signed int v2; // esi
	char v3; // bl
	char v4; // cl
	unsigned char v5; // cl
	char v6; // dl
	char v7; // cl
	char v8; // cl
	char v9; // dl

	v0 = 0;
	do
	{
		v1 = &dungeon[-1][v0 + 39];
		v2 = 40;
		do
		{
			v3 = v1[1];
			if ( v3 == -110
			  && ((v4 = v1[41], (unsigned char)v4 > 0x98u)
			   || (unsigned char)v4 < 0x82u
			   || (v5 = *(v1 - 39), v5 > 0x98u)
			   || v5 < 0x82u
			   || (v6 = v1[41], v6 != -126)
			   && v5 != -126
			   && v6 != -124
			   && v5 != -124
			   && v6 != -123
			   && v5 != -123
			   && v6 != -122
			   && v5 != -122
			   && v6 != -120
			   && v5 != -120
			   && v6 != -118
			   && v5 != -118
			   && v6 != -116
			   && v5 != -116)
			  || v3 == -109
			  && ((v7 = v1[2], (unsigned char)v7 > 0x98u)
			   || (unsigned char)v7 < 0x82u
			   || (v8 = *v1, (unsigned char)*v1 > 0x98u)
			   || (unsigned char)v8 < 0x82u
			   || (v9 = v1[2], v9 != -125)
			   && v8 != -125
			   && v9 != -124
			   && v8 != -124
			   && v9 != -123
			   && v8 != -123
			   && v9 != -121
			   && v8 != -121
			   && v9 != -119
			   && v8 != -119
			   && v9 != -118
			   && v8 != -118
			   && v9 != -117
			   && v8 != -117) )
			{
				v1[1] = 7;
			}
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

int __cdecl DRLG_L3Anvil()
{
	int v0; // esi
	signed int v1; // edi
	signed int v2; // ebx
	signed int v3; // eax
	int v4; // ecx
	unsigned char v5; // dl
	signed int v7; // ebx
	int v8; // edi
	int v9; // ecx
	signed int v10; // eax
	unsigned char v11; // dl
	signed int v12; // [esp+Ch] [ebp-Ch]
	signed int v13; // [esp+Ch] [ebp-Ch]
	signed int v14; // [esp+10h] [ebp-8h]
	int v15; // [esp+14h] [ebp-4h]

	v0 = random(0, 29);
	v1 = 0;
	v15 = random(0, 29);
	v12 = 0;
	while ( 1 )
	{
		if ( v12 >= 200 )
			return 1;
		++v12;
		v14 = 1;
		v2 = 2;
		do
		{
			if ( v14 != 1 )
				break;
			v3 = 0;
			v4 = v15 + v1 + 40 * v0;
			do
			{
				if ( v14 != 1 )
					break;
				v5 = L3ANVIL[v2];
				if ( v5 && dungeon[0][v4] != v5 )
					v14 = 0;
				if ( dflags[0][v4] )
					v14 = 0;
				++v2;
				++v3;
				v4 += 40;
			}
			while ( v3 < 11 );
			++v1;
		}
		while ( v1 < 11 );
		v1 = 0;
		if ( v14 )
			break;
		if ( ++v0 == 29 )
		{
			v0 = 0;
			if ( ++v15 == 29 )
				v15 = 0;
		}
	}
	if ( v12 >= 200 )
		return 1;
	v13 = 11;
	v7 = 123;
	v8 = v15 + 40 * v0;
	do
	{
		v9 = v8;
		v10 = 11;
		do
		{
			v11 = L3ANVIL[v7];
			if ( v11 )
				dungeon[0][v9] = v11;
			dflags[0][v9] |= DFLAG_EXPLORED;
			++v7;
			v9 += 40;
			--v10;
		}
		while ( v10 );
		++v8;
		--v13;
	}
	while ( v13 );
	setpc_y = v15;
	setpc_w = 11;
	setpc_h = 11;
	setpc_x = v0;
	return 0;
}
// 5CF330: using guessed type int setpc_h;
// 5CF334: using guessed type int setpc_w;

void __cdecl FixL3Warp()
{
	int v0; // ecx
	signed int v1; // esi
	char *v2; // eax
	char v3; // dl
	signed int v4; // eax

	v0 = 0;
	while ( 2 )
	{
		v1 = 0;
		v2 = &dungeon[1][v0 + 1];
		do
		{
			v3 = *(v2 - 41);
			if ( v3 == 125 && *(v2 - 1) == 125 && *(v2 - 40) == 125 && *v2 == 125 )
			{
				v4 = v1;
				dungeon[v4][v0] = -100;
				dungeon[v4 + 1][v0] = -101;
				dungeon[v4][v0 + 1] = -103;
				dungeon[v4 + 1][v0 + 1] = -102;
				return;
			}
			if ( v3 == 5 && *v2 == 7 )
				*(v2 - 41) = 7;
			++v1;
			v2 += 40;
		}
		while ( v1 < 40 );
		if ( ++v0 < 40 )
			continue;
		break;
	}
}

void __cdecl FixL3HallofHeroes()
{
	signed int v0; // ecx
	char *v1; // eax
	signed int v2; // edx
	signed int v3; // ecx
	char *v4; // eax
	signed int v5; // edx

	v0 = 0;
	do
	{
		v1 = (char *)dungeon + v0;
		v2 = 40;
		do
		{
			if ( *v1 == 5 && v1[41] == 7 )
				*v1 = 7;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
	v3 = 0;
	do
	{
		v4 = (char *)dungeon + v3;
		v5 = 40;
		do
		{
			if ( *v4 == 5 )
			{
				if ( v4[41] == 12 && v4[40] == 7 )
				{
					*v4 = 7;
					v4[1] = 7;
					v4[41] = 7;
				}
				if ( *v4 == 5 && v4[41] == 12 && v4[1] == 7 )
				{
					*v4 = 7;
					v4[40] = 7;
					v4[41] = 7;
				}
			}
			v4 += 40;
			--v5;
		}
		while ( v5 );
		++v3;
	}
	while ( v3 < 40 );
}

void __fastcall DRLG_L3LockRec(int x, int y)
{
	int v2; // esi
	int v3; // edi
	char *v4; // eax
	char *v5; // ebp

	v2 = x;
	v3 = y;
	v4 = &lockout[x][y];
	if ( *v4 )
	{
		v5 = &lockout[x][y];
		do
		{
			*v4 = 0;
			++lockoutcnt;
			DRLG_L3LockRec(v2, v3 - 1);
			DRLG_L3LockRec(v2, v3 + 1);
			DRLG_L3LockRec(v2 - 1, v3);
			v5 += 40;
			++v2;
			v4 = v5;
		}
		while ( *v5 );
	}
}
// 528380: using guessed type int lockoutcnt;

bool __cdecl DRLG_L3Lockout()
{
	int v0; // esi
	signed int v1; // edx
	signed int v2; // ecx
	signed int v3; // eax
	int x; // [esp+4h] [ebp-8h]
	int y; // [esp+8h] [ebp-4h]

	v0 = 0;
	v1 = 0;
	do
	{
		v2 = 0;
		v3 = v1;
		do
		{
			if ( dungeon[0][v3] )
			{
				lockout[0][v3] = 1;
				x = v2;
				y = v1;
				++v0;
			}
			else
			{
				lockout[0][v3] = 0;
			}
			++v2;
			v3 += 40;
		}
		while ( v2 < 40 );
		++v1;
	}
	while ( v1 < 40 );
	lockoutcnt = 0;
	DRLG_L3LockRec(x, y);
	return v0 == lockoutcnt;
}
// 528380: using guessed type int lockoutcnt;

void __fastcall CreateL3Dungeon(int rseed, int entry)
{
	int v2; // esi
	int v3; // edi
	int v4; // esi
	signed int v5; // eax
	signed int *v6; // [esp+8h] [ebp-8h]
	int (*v7)[112]; // [esp+Ch] [ebp-4h]

	v2 = entry;
	SetRndSeed(rseed);
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
	DRLG_InitSetPC();
	DRLG_L3(v2);
	DRLG_L3Pass3();
	v3 = 0;
	v7 = dPiece;
	do
	{
		v4 = 0;
		v6 = (signed int *)v7;
		do
		{
			v5 = *v6;
			if ( *v6 >= 56 && v5 <= 147 || v5 >= 154 && v5 <= 161 || v5 == 150 || v5 == 152 )
				DoLighting(v4, v3, 7, -1);
			v6 += 112;
			++v4;
		}
		while ( v4 < 112 );
		v7 = (int (*)[112])((char *)v7 + 4);
		++v3;
	}
	while ( (signed int)v7 < (signed int)dPiece[1] );
	DRLG_SetPC();
}
// 5CF328: using guessed type int dmaxx;
// 5CF32C: using guessed type int dmaxy;
// 5D2458: using guessed type int dminx;
// 5D245C: using guessed type int dminy;

void __fastcall DRLG_L3(int entry)
{
	int x1; // esi
	int y1; // eax
	int x2; // edi
	int y2; // eax
	int found; // eax
	int genok; // eax
	signed int i; // ecx
	signed int j; // esi
	bool v24; // [esp-8h] [ebp-20h]

	lavapool = 0;
	do
	{
		do
		{
			do
			{
				InitL3Dungeon();
				x1 = random(0, 20) + 10;
				y1 = random(0, 20);
				DRLG_L3FillRoom(x1, y1 + 10, x1 + 2, y1 + 12);
				DRLG_L3CreateBlock(x1, y1 + 10, 2, 0);
				DRLG_L3CreateBlock(x1 + 2, y1 + 10, 2, 1);
				DRLG_L3CreateBlock(x1, y1 + 12, 2, 2);
				DRLG_L3CreateBlock(x1, y1 + 10, 2, 3);

				if ( QuestStatus(QTYPE_ANVIL) )
				{
					x2 = random(0, 10) + 10;
					y2 = random(0, 10);
					DRLG_L3FloorArea(x2, y2 + 10, x2 + 12, y2 + 22);
				}
				DRLG_L3FillDiags();
				DRLG_L3FillSingles();
				DRLG_L3FillStraights();
				DRLG_L3FillDiags();
				DRLG_L3Edges();
				if ( DRLG_L3GetFloorArea() < 600 )
					found = 0;
				else
					found = DRLG_L3Lockout();
			}
			while ( !found );
			DRLG_L3MakeMegas();
			if ( !entry )
			{
				genok = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, 1, 0);
				if ( genok )
					continue;
				genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 0, 1);
				if ( genok )
					continue;
				if ( currlevel != 9 )
					goto LABEL_24;
				genok = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, 0, 6);
				goto LABEL_23;
			}
			genok = DRLG_L3PlaceMiniSet(L3UP, 1, 1, -1, -1, 0, 0);
			if ( entry == 1 )
			{
				if ( genok )
					continue;
				genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 1, 1);
				ViewX += 2;
				ViewY -= 2;
				if ( genok )
					continue;
				if ( currlevel != 9 )
					goto LABEL_24;
				v24 = 0;
LABEL_22:
				genok = DRLG_L3PlaceMiniSet(L3HOLDWARP, 1, 1, -1, -1, v24, 6);
LABEL_23:
				if ( genok )
					continue;
				goto LABEL_24;
			}
			if ( genok )
				continue;
			genok = DRLG_L3PlaceMiniSet(L3DOWN, 1, 1, -1, -1, 0, 1);
			if ( genok )
				continue;
			if ( currlevel == 9 )
			{
				v24 = 1;
				goto LABEL_22;
			}
LABEL_24:
			if ( !QuestStatus(QTYPE_ANVIL) )
				break;
			genok = DRLG_L3Anvil();
		}
		while ( genok == 1 );
		DRLG_L3Pool();
	}
	while ( !lavapool );
	DRLG_L3PoolFix();
	FixL3Warp();
	DRLG_L3PlaceRndSet(L3ISLE1, 70);
	DRLG_L3PlaceRndSet(L3ISLE2, 70);
	DRLG_L3PlaceRndSet(L3ISLE3, 30);
	DRLG_L3PlaceRndSet(L3ISLE4, 30);
	DRLG_L3PlaceRndSet(L3ISLE1, 100);
	DRLG_L3PlaceRndSet(L3ISLE2, 100);
	DRLG_L3PlaceRndSet(L3ISLE5, 90);
	FixL3HallofHeroes();
	DRLG_L3River();

	if ( QuestStatus(QTYPE_ANVIL) )
	{
		dungeon[setpc_x + 7][setpc_y + 5] = 7;
		dungeon[setpc_x + 8][setpc_y + 5] = 7;
		dungeon[setpc_x + 9][setpc_y + 5] = 7;
		if ( dungeon[setpc_x + 10][setpc_y + 5] == 17
		  || dungeon[setpc_x + 10][setpc_y + 5] == 18 )
			dungeon[setpc_x + 10][setpc_y + 5] = 45;
	}

	DRLG_PlaceThemeRooms(5, 10, 7, 0, 0);
	DRLG_L3Wood();
	DRLG_L3PlaceRndSet(L3TITE1, 10);
	DRLG_L3PlaceRndSet(L3TITE2, 10);
	DRLG_L3PlaceRndSet(L3TITE3, 10);
	DRLG_L3PlaceRndSet(L3TITE6, 20);
	DRLG_L3PlaceRndSet(L3TITE7, 20);
	DRLG_L3PlaceRndSet(L3TITE8, 20);
	DRLG_L3PlaceRndSet(L3TITE9, 20);
	DRLG_L3PlaceRndSet(L3TITE10, 20);
	DRLG_L3PlaceRndSet(L3TITE11, 30);
	DRLG_L3PlaceRndSet(L3TITE12, 20);
	DRLG_L3PlaceRndSet(L3TITE13, 20);
	DRLG_L3PlaceRndSet(L3CREV1, 30);
	DRLG_L3PlaceRndSet(L3CREV2, 30);
	DRLG_L3PlaceRndSet(L3CREV3, 30);
	DRLG_L3PlaceRndSet(L3CREV4, 30);
	DRLG_L3PlaceRndSet(L3CREV5, 30);
	DRLG_L3PlaceRndSet(L3CREV6, 30);
	DRLG_L3PlaceRndSet(L3CREV7, 30);
	DRLG_L3PlaceRndSet(L3CREV8, 30);
	DRLG_L3PlaceRndSet(L3CREV9, 30);
	DRLG_L3PlaceRndSet(L3CREV10, 30);
	DRLG_L3PlaceRndSet(L3CREV11, 30);
	DRLG_L3PlaceRndSet(L3XTRA1, 25);
	DRLG_L3PlaceRndSet(L3XTRA2, 25);
	DRLG_L3PlaceRndSet(L3XTRA3, 25);
	DRLG_L3PlaceRndSet(L3XTRA4, 25);
	DRLG_L3PlaceRndSet(L3XTRA5, 25);

	for(i = 0; i < 40; i++)
	{
		for(j = 0; j < 40; j++)
			pdungeon[i][j] = dungeon[i][j];
	}

	DRLG_Init_Globals();
}
// 528378: using guessed type char lavapool;

void __cdecl InitL3Dungeon()
{
	int i; // edx
	int j; // ecx

	memset(dungeon, 0, sizeof(dungeon));

	for(i = 0; i < 40; i++)
	{
		for(j = 0; j < 40; j++)
		{
			dungeon[i][j] = 0;
			dflags[i][j] = 0;
		}
	}
}

int __fastcall DRLG_L3FillRoom(int x1, int y1, int x2, int y2)
{
	int v4; // esi
	int v5; // eax
	int v6; // edi
	int v7; // edx
	int v8; // ecx
	char *v9; // ecx
	int v10; // eax
	int v11; // ebx
	char *v12; // edx
	int v13; // eax
	int i; // ebx
	int v16; // ecx
	char *v17; // ebx
	int v18; // edi
	int v21; // [esp+Ch] [ebp-4h]
	int x2a; // [esp+18h] [ebp+8h]
	int y2a; // [esp+1Ch] [ebp+Ch]

	v4 = x1;
	v5 = y1;
	v21 = y1;
	if ( x1 <= 1 )
		return 0;
	v6 = x2;
	if ( x2 >= 34 || y1 <= 1 || y2 >= 38 )
		return 0;
	v7 = 0;
	v8 = v5;
	x2a = v5;
	if ( v5 <= y2 )
	{
		do
		{
			if ( v4 <= v6 )
			{
				v9 = &dungeon[v4][v8];
				v10 = v6 - v4 + 1;
				do
				{
					v7 += (unsigned char)*v9;
					v9 += 40;
					--v10;
				}
				while ( v10 );
			}
			v8 = x2a++ + 1;
		}
		while ( x2a <= y2 );
		if ( !v7 )
		{
			v5 = v21;
			goto LABEL_12;
		}
		return 0;
	}
LABEL_12:
	v11 = v5 + 1;
	if ( v5 + 1 < y2 )
	{
		v8 = v4 + 1;
		do
		{
			if ( v8 < v6 )
			{
				v12 = &dungeon[v8][v11];
				v13 = v6 - v8;
				do
				{
					*v12 = 1;
					v12 += 40;
					--v13;
				}
				while ( v13 );
			}
			++v11;
		}
		while ( v11 < y2 );
		v5 = v21;
	}
	for ( i = v5; i <= y2; ++i )
	{
		if ( random(0, 2) )
			dungeon[v4][i] = 1;
		if ( random(0, 2) )
			dungeon[v6][i] = 1;
	}
	if ( v4 <= v6 )
	{
		v16 = y2;
		v17 = &dungeon[v4][y2];
		v18 = v6 - v4 + 1;
		y2a = v21 - y2;
		do
		{
			if ( random(0, 2) )
				v17[y2a] = 1;
			if ( random(0, 2) )
				*v17 = 1;
			v17 += 40;
			--v18;
		}
		while ( v18 );
	}
	return 1;
}

void __fastcall DRLG_L3CreateBlock(int x, int y, int obs, int dir)
{
	int v4; // esi
	int v5; // edi
	int v6; // eax
	int v9; // ebx
	bool v10; // zf
	bool v11; // zf
	int y2; // [esp+Ch] [ebp-14h]
	int x2; // [esp+10h] [ebp-10h]
	int i; // [esp+14h] [ebp-Ch]
	int v16; // [esp+18h] [ebp-8h]
	int max; // [esp+1Ch] [ebp-4h]

	v4 = obs;
	v5 = obs;
	v16 = y;
	for ( i = x; ; i = v4 )
	{
		v6 = random(0, 2);
		max = v6 + 3;
		v9 = random(0, 2) + 3;
		if ( !dir )
		{
			y2 = v16 - 1;
			v5 = v16 - 1 - v9;
			if ( max < obs )
			{
				v4 = i + random(0, max);
			}
			if ( max == obs )
				v4 = i;
			if ( max > obs )
			{
				v4 = i - random(0, max);
			}
			x2 = v4 + max;
		}
		if ( dir == 3 )
		{
			x2 = i - 1;
			v4 = i - 1 - max;
			v10 = v9 == obs;
			if ( v9 < obs )
			{
				v5 = v16 + random(0, v9);
				v10 = v9 == obs;
			}
			if ( v10 )
				v5 = v16;
			if ( v9 > obs )
			{
				v5 = v16 - random(0, v9);
			}
			y2 = v5 + v9;
		}
		if ( dir == 2 )
		{
			v5 = v16 + 1;
			y2 = v16 + 1 + v9;
			if ( max < obs )
			{
				v4 = i + random(0, max);
			}
			if ( max == obs )
				v4 = i;
			if ( max > obs )
			{
				v4 = i - random(0, max);
			}
			x2 = v4 + max;
		}
		if ( dir == 1 )
		{
			v4 = i + 1;
			v11 = v9 == obs;
			x2 = i + 1 + max;
			if ( v9 < obs )
			{
				v5 = v16 + random(0, v9);
				v11 = v9 == obs;
			}
			if ( v11 )
				v5 = v16;
			if ( v9 > obs )
			{
				v5 = v16 - random(0, v9);
			}
			y2 = v5 + v9;
		}
		if ( DRLG_L3FillRoom(v4, v5, x2, y2) != 1 )
			break;
		if ( !random(0, 4) )
			break;
		if ( dir != 2 )
			DRLG_L3CreateBlock(v4, v5, v9, 0);
		if ( dir != 3 )
			DRLG_L3CreateBlock(x2, v5, max, 1);
		if ( dir )
			DRLG_L3CreateBlock(v4, y2, v9, 2);
		if ( dir == 1 )
			break;
		dir = 3;
		obs = max;
		v16 = v5;
	}
}

void __fastcall DRLG_L3FloorArea(int x1, int y1, int x2, int y2)
{
	int i; // esi
	char *v5; // edx
	int v6; // eax

	for ( i = y1; i <= y2; ++i )
	{
		if ( x1 <= x2 )
		{
			v5 = &dungeon[x1][i];
			v6 = x2 - x1 + 1;
			do
			{
				*v5 = 1;
				v5 += 40;
				--v6;
			}
			while ( v6 );
		}
	}
}

void __cdecl DRLG_L3FillDiags()
{
	signed int v0; // ebx
	char *v1; // esi
	signed int v2; // ebp
	int v3; // ecx
	int v4; // edi

	v0 = 0;
	do
	{
		v1 = &dungeon[1][v0 + 1];
		v2 = 39;
		do
		{
			v3 = (unsigned char)*v1;
			v4 = v3
			   + 2 * ((unsigned char)*(v1 - 40) + 2 * ((unsigned char)*(v1 - 1) + 2 * (unsigned char)*(v1 - 41)));
			if ( v4 == 6 )
			{
				if ( !random(0, 2) )
				{
					*(v1 - 41) = 1;
					goto LABEL_11;
				}
				*v1 = 1;
			}
			if ( v4 == 9 )
			{
				if ( random(0, 2) )
					*(v1 - 40) = 1;
				else
					*(v1 - 1) = 1;
			}
LABEL_11:
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 39 );
}

void __cdecl DRLG_L3FillSingles()
{
	signed int v0; // ecx
	char *v1; // eax
	signed int v2; // edx

	v0 = 1;
	do
	{
		v1 = &dungeon[0][v0 + 39];
		v2 = 38;
		do
		{
			if ( !v1[1]
			  && (unsigned char)*v1 + (unsigned char)v1[40] + (unsigned char)*(v1 - 40) == 3
			  && (unsigned char)*(v1 - 39) + (unsigned char)v1[41] == 2
			  && (unsigned char)v1[2] + (unsigned char)*(v1 - 38) + (unsigned char)v1[42] == 3 )
			{
				v1[1] = 1;
			}
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 39 );
}

void __cdecl DRLG_L3FillStraights()
{
	int v0; // esi
	char *v1; // ecx
	signed int v2; // eax
	char *v3; // ebx
	int v4; // edi
	int v5; // ebx
	char v6; // al
	char *v7; // ecx
	signed int v8; // eax
	char *v9; // ebx
	int v10; // edi
	int v11; // ebx
	char v12; // al
	signed int v13; // ebx
	signed int v14; // eax
	signed int v15; // esi
	signed int i; // edi
	signed int v17; // ebx
	signed int v18; // eax
	signed int v19; // esi
	signed int j; // edi
	//int v21; // [esp+Ch] [ebp-14h]
	char *v22; // [esp+Ch] [ebp-14h]
	char *v23; // [esp+Ch] [ebp-14h]
	char *v24; // [esp+10h] [ebp-10h]
	signed int v25; // [esp+14h] [ebp-Ch]
	signed int v26; // [esp+14h] [ebp-Ch]
	signed int v27; // [esp+18h] [ebp-8h]
	signed int v28; // [esp+18h] [ebp-8h]
	int v29; // [esp+1Ch] [ebp-4h]
	int v30; // [esp+1Ch] [ebp-4h]

	v27 = 0;
	v0 = 0; /* v21 */
	do
	{
		v1 = (char *)v27;
		v2 = 0;
		v29 = 0;
		v3 = &dungeon[0][v27 + 1];
		v4 = 40 * v0;
		v25 = 0;
		v22 = &dungeon[0][v27 + 1];
		do
		{
			if ( *(v3 - 1) || *v3 != 1 )
			{
				if ( v2 > 3 )
				{
					if ( random(0, 2) )
					{
						if ( v0 < v29 )
						{
							v5 = v29 - v0;
							v24 = (char *)dungeon + v4 + v27;
							do
							{
								v6 = random(0, 2);
								v1 = v24;
								v24 += 40;
								--v5;
								*v1 = v6;
							}
							while ( v5 );
							v3 = v22;
						}
					}
				}
				v2 = 0;
			}
			else
			{
				if ( !v2 )
				{
					v0 = v29;
					v4 = v25;
				}
				++v2;
			}
			v25 += 40;
			++v29;
			v3 += 40;
			v22 = v3;
		}
		while ( v25 < 1480 );
		++v27;
	}
	while ( v27 < 39 );
	v28 = 0;
	do
	{
		v7 = (char *)v28;
		v8 = 0;
		v30 = 0;
		v9 = &dungeon[0][v28 + 1];
		v26 = 0;
		v10 = 40 * v0;
		v23 = &dungeon[0][v28 + 1];
		do
		{
			if ( *(v9 - 1) != 1 || *v9 )
			{
				if ( v8 > 3 )
				{
					if ( random(0, 2) )
					{
						if ( v0 < v30 )
						{
							v11 = v30 - v0;
							v24 = &dungeon[0][v10 + 1 + v28];
							do
							{
								v12 = random(0, 2);
								v7 = v24;
								v24 += 40;
								--v11;
								*v7 = v12;
							}
							while ( v11 );
							v9 = v23;
						}
					}
				}
				v8 = 0;
			}
			else
			{
				if ( !v8 )
				{
					v0 = v30;
					v10 = v26;
				}
				++v8;
			}
			v26 += 40;
			++v30;
			v9 += 40;
			v23 = v9;
		}
		while ( v26 < 1480 );
		++v28;
	}
	while ( v28 < 39 );
	v13 = 0;
	do
	{
		v14 = 0;
		v15 = 0;
		do
		{
			if ( dungeon[v13][v15] || dungeon[v13 + 1][v15] != 1 )
			{
				if ( v14 > 3 )
				{
					if ( random(0, 2) )
					{
						for ( i = (signed int)v24; i < v15; ++i )
						{
							dungeon[v13][i] = random(0, 2);
						}
					}
				}
				v14 = 0;
			}
			else
			{
				if ( !v14 )
					v24 = (char *)v15;
				++v14;
			}
			++v15;
		}
		while ( v15 < 37 );
		++v13;
	}
	while ( v13 < 39 );
	v17 = 0;
	do
	{
		v18 = 0;
		v19 = 0;
		do
		{
			if ( dungeon[v17][v19] != 1 || dungeon[v17 + 1][v19] )
			{
				if ( v18 > 3 )
				{
					if ( random(0, 2) )
					{
						for ( j = (signed int)v24; j < v19; ++j )
						{
							dungeon[v17 + 1][j] = random(0, 2);
						}
					}
				}
				v18 = 0;
			}
			else
			{
				if ( !v18 )
					v24 = (char *)v19;
				++v18;
			}
			++v19;
		}
		while ( v19 < 37 );
		++v17;
	}
	while ( v17 < 39 );
}

void __cdecl DRLG_L3Edges()
{
	char *v0; // eax

	memset(dungeon[39], 0, sizeof(char[40]));
	v0 = &dungeon[0][39];
	do
	{
		*v0 = 0;
		v0 += 40;
	}
	while ( (signed int)v0 < (signed int)&dungeon[40][39] );
}

int __cdecl DRLG_L3GetFloorArea()
{
	int gfa; // eax
	int i; // edx
	int j; // esi

	gfa = 0;

	for(i = 0; i < 40; i++)
	{
		for(j = 0; j < 40; j++)
			gfa += dungeon[i][j];
	}

	return gfa;
}

void __cdecl DRLG_L3MakeMegas()
{
	signed int v0; // edi
	char *v1; // esi
	signed int v2; // ebp
	int v3; // ecx
	int v4; // eax
	char *v5; // eax

	v0 = 0;
	do
	{
		v1 = &dungeon[0][v0 + 1];
		v2 = 39;
		do
		{
			v3 = (unsigned char)v1[40];
			v4 = v3 + 2 * ((unsigned char)*v1 + 2 * ((unsigned char)v1[39] + 2 * (unsigned char)*(v1 - 1)));
			if ( v4 == 6 )
			{
				if ( !random(0, 2) )
				{
					v4 = 12;
					goto LABEL_9;
				}
				v4 = 5;
			}
			if ( v4 == 9 )
			{
				v4 = (random(0, 2) != 0) + 13;
			}
LABEL_9:
			--v2;
			*(v1 - 1) = L3ConvTbl[v4];
			v1 += 40;
		}
		while ( v2 );
		dungeon[39][v0++] = 8;
	}
	while ( v0 < 39 );
	v5 = &dungeon[0][39];
	do
	{
		*v5 = 8;
		v5 += 40;
	}
	while ( (signed int)v5 < (signed int)&dungeon[40][39] );
}

void __cdecl DRLG_L3River()
{
	signed int v0; // ebx
	int v1; // esi
	int v2; // edi
	char v3; // al
	char v4; // al
	signed int v5; // edx
	int v6; // eax
	int v7; // ebx
	unsigned char v8; // al
	unsigned char v9; // al
	int v10; // eax
	char *v11; // eax
	signed int v12; // eax
	int v13; // ecx
	bool v14; // zf
	int v15; // eax
	signed int v16; // eax
	int v17; // eax
	signed int v18; // eax
	signed int v19; // eax
	signed int v20; // edi
	int v21; // eax
	int v22; // eax
	int v23; // edx
	int v24; // ecx
	int v25; // ecx
	int v26; // esi
	int v27; // ecx
	int v28; // edx
	int v29; // ecx
	int v30; // edx
	int v31; // ecx
	int v32; // edx
	bool v33; // sf
	unsigned char v34; // of
	int river[3][100]; // [esp+Ch] [ebp-4E8h]
	int v36; // [esp+4BCh] [ebp-38h]
	int v37; // [esp+4C0h] [ebp-34h]
	int v38; // [esp+4C4h] [ebp-30h]
	int v39; // [esp+4C8h] [ebp-2Ch]
	int v40; // [esp+4CCh] [ebp-28h]
	int v41; // [esp+4D0h] [ebp-24h]
	int v42; // [esp+4D4h] [ebp-20h]
	int v43; // [esp+4D8h] [ebp-1Ch]
	int v44; // [esp+4DCh] [ebp-18h]
	int v45; // [esp+4E0h] [ebp-14h]
	int v46; // [esp+4E4h] [ebp-10h]
	int v47; // [esp+4E8h] [ebp-Ch]
	int v48; // [esp+4ECh] [ebp-8h]
	int max; // [esp+4F0h] [ebp-4h]

	v0 = 0;
	v39 = 0;
	v41 = 0;
	while ( v39 < 4 )
	{
		v47 = 0;
		do
		{
			if ( v41 >= 200 )
			{
				v5 = max;
				break;
			}
			++v41;
			v1 = 0;
			v2 = 0;
			while ( 1 )
			{
				v3 = dungeon[v1][v2];
				if ( (unsigned char)v3 >= 0x19u && (unsigned char)v3 <= 0x1Cu )
					break;
				if ( v0 >= 100 )
					return;
				v1 = random(0, 40);
				v2 = random(0, 40);
				++v0;
				while ( 1 )
				{
					v4 = dungeon[v1][v2];
					if ( (unsigned char)v4 >= 0x19u && (unsigned char)v4 <= 0x1Cu )
						break;
					if ( v2 >= 40 )
						break;
					if ( ++v1 >= 40 )
					{
						v1 = 0;
						++v2;
					}
				}
			}
			if ( v0 >= 100 )
				return;
			switch ( dungeon[v1][v2] )
			{
				case 0x19:
					v48 = 3;
					v42 = 2;
					river[2][0] = 40;
					break;
				case 0x1A:
					v48 = 0;
					v42 = 1;
					river[2][0] = 38;
					break;
				case 0x1B:
					v42 = 0;
					v48 = 1;
					river[2][0] = 41;
					break;
				case 0x1C:
					v48 = 2;
					v42 = 3;
					river[2][0] = 39;
					break;
			}
			v43 = 0;
			max = 1;
			v5 = 1;
			river[0][0] = v1;
			river[1][0] = v2;
			v46 = 4;
			v45 = 40 * v1;
			while ( v5 < 100 )
			{
				v38 = v1;
				v36 = v45;
				v37 = v2;
				if ( v43 )
				{
					v48 = ((_BYTE)v48 + 1) & 3;
					v7 = v48;
				}
				else
				{
					v6 = random(0, 4);
					v5 = max;
					v7 = v6;
					v48 = v6;
				}
				while ( 1 )
				{
					++v43;
					if ( v7 != v42 && v7 != v46 )
						break;
					v7 = ((_BYTE)v7 + 1) & 3;
				}
				v48 = v7;
				if ( !v7 )
				{
					if ( v2 <= 0 )
						goto LABEL_44;
					--v2;
				}
				if ( v7 == 1 )
				{
					if ( v2 >= 40 )
						goto LABEL_44;
					++v2;
				}
				if ( v7 != 2 )
					goto LABEL_41;
				if ( v1 < 40 )
				{
					++v1;
					v45 += 40;
LABEL_41:
					if ( v7 == 3 && v1 > 0 )
					{
						--v1;
						v45 -= 40;
					}
				}
LABEL_44:
				if ( dungeon[0][v45 + v2] == 7 )
				{
					v43 = 0;
					if ( v7 < 2 )
					{
						v8 = random(0, 2);
						v5 = max;
						river[2][max] = v8 + 17;
					}
					if ( v7 > 1 )
					{
						v9 = random(0, 2);
						v5 = max;
						river[2][max] = v9 + 15;
					}
					v10 = v40;
					river[0][v5] = v1;
					river[1][v5++] = v2;
					max = v5;
					if ( v7 || v10 != 2 )
					{
						if ( v7 != 3 )
							goto LABEL_58;
						if ( v10 != 1 )
							goto LABEL_70;
					}
					if ( v5 > 2 )
						river[1][v5 + 98] = 22;
					if ( !v7 )
					{
						v46 = 1;
LABEL_59:
						if ( v10 == 3 )
							goto LABEL_62;
						goto LABEL_60;
					}
					v46 = 2;
LABEL_58:
					if ( !v7 )
						goto LABEL_59;
LABEL_60:
					if ( v7 != 2 )
						goto LABEL_67;
					if ( v10 != 1 )
						goto LABEL_79;
LABEL_62:
					if ( v5 > 2 )
						river[1][v5 + 98] = 21;
					if ( !v7 )
					{
						v46 = 1;
						goto LABEL_83;
					}
					v46 = 3;
LABEL_67:
					if ( v7 != 1 || v10 != 2 )
					{
						if ( v7 != 3 )
							goto LABEL_76;
LABEL_70:
						if ( v10 )
							goto LABEL_83;
					}
					if ( v5 > 2 )
						river[1][v5 + 98] = 20;
					if ( v7 == 1 )
					{
						v46 = 0;
						goto LABEL_77;
					}
					v46 = 2;
LABEL_76:
					if ( v7 != 1 )
						goto LABEL_78;
LABEL_77:
					if ( v10 != 3 )
					{
LABEL_78:
						if ( v7 != 2 )
							goto LABEL_83;
LABEL_79:
						if ( v10 )
							goto LABEL_83;
					}
					if ( v5 > 2 )
						river[1][v5 + 98] = 19;
					v46 = v7 != 1 ? 3 : 0;
LABEL_83:
					v40 = v7;
				}
				else
				{
					v1 = v38;
					v2 = v37;
					v45 = v36;
					if ( v43 >= 4 )
						break;
				}
			}
			if ( v48 )
			{
				v13 = v40;
				goto LABEL_94;
			}
			v11 = &dungeon[v1][v2];
			if ( *(v11 - 1) == 10 && *(v11 - 2) == 8 )
			{
				v12 = v5;
				river[1][v12] = v2 - 1;
				v13 = v40;
				v14 = v40 == 2;
				river[0][v12] = v1;
				river[2][v12] = 24;
				if ( v14 )
					river[1][v12 + 99] = 22;
				if ( v13 == 3 )
					river[1][v12 + 99] = 21;
				v47 = 1;
LABEL_94:
				if ( v48 == 1 )
				{
					v15 = v2 + 40 * v1;
					if ( dungeon[0][v15 + 1] == 2 && dungeon[0][v15 + 2] == 8 )
					{
						v16 = v5;
						river[0][v16] = v1;
						river[1][v16] = v2 + 1;
						river[2][v16] = 42;
						if ( v13 == 2 )
							river[1][v16 + 99] = 20;
						if ( v13 == 3 )
							river[1][v16 + 99] = 19;
						v47 = 1;
						goto LABEL_102;
					}
				}
				else
				{
LABEL_102:
					if ( v48 == 2 )
					{
						v17 = v2 + 40 * v1;
						if ( dungeon[1][v17] != 4 || dungeon[2][v17] != 8 )
							goto LABEL_118;
						v18 = v5;
						river[0][v18] = v1 + 1;
						river[1][v18] = v2;
						river[2][v18] = 43;
						if ( !v13 )
							river[1][v18 + 99] = 19;
						if ( v13 == 1 )
							river[1][v18 + 99] = 21;
						v47 = 1;
					}
					if ( v48 == 3
					  && dungeon[v1-1][v2] == 9 // *((_BYTE *)&dMonster[111][10 * v1 + 102] + v2) == 9 /* check */
					  && dungeon[0][8 * (5 * v1 - 10) + v2] == 8 )
					{
						v19 = v5;
						river[0][v19] = v1 - 1;
						river[1][v19] = v2;
						river[2][v19] = 23;
						if ( !v13 )
							river[1][v19 + 99] = 20;
						if ( v13 == 1 )
							river[1][v19 + 99] = 22;
						v47 = 1;
					}
				}
			}
LABEL_118:
			v0 = 0;
		}
		while ( !v47 );
		if ( v47 == 1 && v5 >= 7 )
		{
			v20 = 0;
			v47 = 0;
LABEL_124:
			while ( v47 < 30 )
			{
				++v47;
				v21 = random(0, max);
				v44 = v21;
				v22 = v21;
				v23 = river[2][v22];
				if ( v23 == 15 || v23 == 16 )
				{
					v24 = river[1][v22] + 40 * river[0][v22];
					if ( dungeon[0][v24 - 1] == 7 && dungeon[0][v24 + 1] == 7 ) /*  *((_BYTE *)&dMonster[111][111] + v24 + 3)  check */
						v20 = 1;
				}
				if ( v23 == 17 || v23 == 18 )
				{
					v25 = river[1][v22] + 40 * river[0][v22];
					if ( dungeon[-1][v25] == 7 && dungeon[1][v25] == 7 ) /* *((_BYTE *)&dMonster[111][102] + v25)  check */
						v20 = 2;
				}
				v26 = 0;
				if ( max > 0 )
				{
					while ( 1 )
					{
						if ( !v20 )
							goto LABEL_124;
						if ( v20 != 1 )
							goto LABEL_142;
						v27 = river[1][v22];
						v28 = river[1][v26];
						if ( (v27 - 1 == v28 || v27 + 1 == v28) && river[0][v22] == river[0][v26] )
							break;
LABEL_147:
						if ( ++v26 >= max )
							goto LABEL_148;
					}
					v20 = 0;
LABEL_142:
					if ( v20 == 2 )
					{
						v29 = river[0][v22];
						v30 = river[0][v26];
						if ( (v29 - 1 == v30 || v29 + 1 == v30) && river[1][v22] == river[1][v26] )
							v20 = 0;
					}
					goto LABEL_147;
				}
LABEL_148:
				if ( v20 )
					break;
			}
			v0 = 0;
			if ( v20 )
			{
				river[2][v44] = v20 == 1 ? 44 : 45;
				v31 = max;
				++v39;
				v44 = 0;
				if ( max >= 0 )
				{
					do
					{
						v32 = v44++;
						v34 = __OFSUB__(v44, v31);
						v14 = v44 == v31;
						v33 = v44 - v31 < 0;
						dungeon[river[0][v32]][river[1][v32]] = river[2][v32];
					}
					while ( (unsigned char)(v33 ^ v34) | v14 );
				}
			}
		}
		if ( v41 >= 200 )
			return;
	}
}
// 410FAD: using guessed type int var_1C8[100];
// 410FAD: using guessed type int var_4E8[100];
// 410FAD: using guessed type int var_358[98];

void __cdecl DRLG_L3Pool()
{
	int v0; // ebx
	_BYTE *v1; // ecx
	int v2; // esi
	int v3; // ecx
	signed int v4; // eax
	signed int v5; // eax
	signed int v6; // eax
	int v7; // eax
	int v8; // edi
	int v9; // ecx
	int v10; // eax
	int v11; // esi
	char *v12; // edx
	unsigned char v13; // al
	unsigned char v14; // al
	signed int v15; // [esp+Ch] [ebp-18h]
	char *v16; // [esp+10h] [ebp-14h]
	signed int v17; // [esp+14h] [ebp-10h]
	int v18; // [esp+18h] [ebp-Ch]
	int totarea; // [esp+1Ch] [ebp-8h]
	int x; // [esp+20h] [ebp-4h]

	v0 = 0;
	v18 = 0;
	do
	{
		x = 0;
		v1 = (unsigned char *)dungeon + v0;
		v16 = (char *)dungeon + v0;
		do
		{
			if ( *v1 == 8 )
			{
				*v1 = -120;
				v2 = x - 1;
				totarea = 1;
				v3 = x - 1 + 2;
				v4 = v3 >= 40 ? 1 : DRLG_L3SpawnEdge(v3, v0, &totarea);
				v5 = v2 <= 0 || v4 ? 1 : DRLG_L3SpawnEdge(v2, v0, &totarea);
				v6 = v0 + 1 >= 40 || v5 ? 1 : DRLG_L3SpawnEdge(x, v0 + 1, &totarea);
				v17 = v0 - 1 <= 0 || v6 ? 1 : DRLG_L3SpawnEdge(x, v0 - 1, &totarea);
				v7 = random(0, 100);
				v8 = totarea;
				v15 = v7;
				v9 = v0 - totarea;
				if ( v0 - totarea < totarea + v0 )
				{
					totarea = x - totarea;
					v10 = v8 + x;
					do
					{
						v11 = totarea;
						if ( totarea < v10 )
						{
							v12 = &dungeon[totarea][v9];
							do
							{
								if ( *v12 < 0 && v9 >= 0 && v9 < 40 && v11 >= 0 && v11 < 40 )
								{
									v13 = *v12 & 0x7F;
									*v12 = v13;
									if ( v8 > 4 )
									{
										if ( v15 < 25 && !v17 )
										{
											v14 = L3PoolSub[v13];
											if ( v14 )
											{
												if ( v14 <= 0x25u )
													*v12 = v14;
											}
											lavapool = 1;
										}
										v0 = v18;
									}
								}
								++v11;
								v10 = v8 + x;
								v12 += 40;
							}
							while ( v11 < v8 + x );
						}
						++v9;
					}
					while ( v9 < v8 + v0 );
				}
			}
			++x;
			v1 = (unsigned char *)v16 + 40;
			v16 += 40;
		}
		while ( x < 40 );
		v18 = ++v0;
	}
	while ( v0 < 40 );
}
// 528378: using guessed type char lavapool;

int __fastcall DRLG_L3SpawnEdge(int x, int y, int *totarea)
{
	int *v3; // ebp
	int v4; // edi
	int v5; // esi
	char *v6; // ecx
	int *v7; // eax
	int v8; // eax
	int *totareaa; // [esp+14h] [ebp+4h]

	v3 = totarea;
	v4 = y;
	v5 = x;
	if ( *totarea <= 40 && x >= 0 && y >= 0 && x < 40 && y < 40 )
	{
		v6 = &dungeon[x][y];
		_LOBYTE(v7) = *v6;
		if ( *v6 < 0 )
			return 0;
		if ( (unsigned char)v7 <= 0xFu )
		{
			*v6 = (unsigned char)v7 | 0x80;
			++*totarea;
			if ( (_BYTE)v7 == 8 )
			{
				if ( DRLG_L3SpawnEdge(v5 + 1, y, totarea) == 1
				  || DRLG_L3SpawnEdge(v5 - 1, v4, totarea) == 1
				  || DRLG_L3SpawnEdge(v5, v4 + 1, totarea) == 1 )
				{
					return 1;
				}
				v8 = DRLG_L3SpawnEdge(v5, v4 - 1, totarea);
LABEL_24:
				if ( v8 == 1 )
					return 1;
				return 0;
			}
			v7 = (int *)(unsigned char)v7;
			totareaa = v7;
			if ( L3SpawnTbl2[(unsigned char)v7] & 8 )
			{
				if ( DRLG_L3Spawn(v5, y - 1, v3) == 1 )
					return 1;
				v7 = totareaa;
			}
			if ( L3SpawnTbl2[(_DWORD)v7] & 4 )
			{
				if ( DRLG_L3Spawn(v5, v4 + 1, v3) == 1 )
					return 1;
				v7 = totareaa;
			}
			if ( !(L3SpawnTbl2[(_DWORD)v7] & 2) )
				goto LABEL_18;
			if ( DRLG_L3Spawn(v5 + 1, v4, v3) != 1 )
			{
				v7 = totareaa;
LABEL_18:
				if ( L3SpawnTbl2[(_DWORD)v7] & 1 )
				{
					v8 = DRLG_L3Spawn(v5 - 1, v4, v3);
					goto LABEL_24;
				}
				return 0;
			}
			return 1;
		}
	}
	return 1;
}

int __fastcall DRLG_L3Spawn(int x, int y, int *totarea)
{
	int v3; // edi
	int v4; // esi
	char *v5; // eax
	unsigned char v6; // cl
	int v7; // ebx
	int result; // eax

	v3 = y;
	v4 = x;
	result = 1;
	if ( *totarea <= 40 && x >= 0 && y >= 0 && x < 40 && y < 40 )
	{
		v5 = &dungeon[x][y];
		v6 = *v5;
		if ( *v5 < 0
		  || v6 <= 0xFu
		  && ((v7 = v6, *v5 = v6 | 0x80, ++*totarea, !(L3SpawnTbl1[v6] & 8)) || DRLG_L3Spawn(v4, y - 1, totarea) != 1)
		  && (!(L3SpawnTbl1[v7] & 4) || DRLG_L3Spawn(v4, v3 + 1, totarea) != 1)
		  && (!(L3SpawnTbl1[v7] & 2) || DRLG_L3Spawn(v4 + 1, v3, totarea) != 1)
		  && (!(L3SpawnTbl1[v7] & 1) || DRLG_L3Spawn(v4 - 1, v3, totarea) != 1)
		  && ((L3SpawnTbl1[v7] & 0x80u) == 0 || DRLG_L3SpawnEdge(v4, v3 - 1, totarea) != 1)
		  && (!(L3SpawnTbl1[v7] & 0x40) || DRLG_L3SpawnEdge(v4, v3 + 1, totarea) != 1)
		  && (!(L3SpawnTbl1[v7] & 0x20) || DRLG_L3SpawnEdge(v4 + 1, v3, totarea) != 1)
		  && (!(L3SpawnTbl1[v7] & 0x10) || DRLG_L3SpawnEdge(v4 - 1, v3, totarea) != 1) )
		{
			result = 0;
		}
	}
	return result;
}

void __cdecl DRLG_L3PoolFix()
{
	signed int v0; // esi
	char *v1; // eax
	char *v2; // edi
	unsigned char v3; // cl
	char v4; // cl
	char v5; // cl
	char v6; // cl
	char v7; // cl
	char v8; // cl
	char v9; // al
	bool v10; // zf
	signed int v11; // [esp+10h] [ebp-4h]

	v0 = 0;
	do
	{
		v1 = &dungeon[-1][v0];
		v11 = 40;
		do
		{
			v2 = v1 + 40;
			if ( v1[40] == 8 )
			{
				v3 = *(v1 - 1);
				if ( v3 >= 0x19u && v3 <= 0x29u && (unsigned char)*v1 >= 0x19u && (unsigned char)*v1 <= 0x29u )
				{
					v4 = v1[1];
					if ( (unsigned char)v4 >= 0x19u && (unsigned char)v4 <= 0x29u )
					{
						v5 = v1[39];
						if ( (unsigned char)v5 >= 0x19u && (unsigned char)v5 <= 0x29u )
						{
							v6 = v1[41];
							if ( (unsigned char)v6 >= 0x19u && (unsigned char)v6 <= 0x29u )
							{
								v7 = v1[79];
								if ( (unsigned char)v7 >= 0x19u && (unsigned char)v7 <= 0x29u )
								{
									v8 = v1[80];
									if ( (unsigned char)v8 >= 0x19u && (unsigned char)v8 <= 0x29u )
									{
										v9 = v1[81];
										if ( (unsigned char)v9 >= 0x19u && (unsigned char)v9 <= 0x29u )
											*v2 = 33;
									}
								}
							}
						}
					}
				}
			}
			v10 = v11-- == 1;
			v1 = v2;
		}
		while ( !v10 );
		++v0;
	}
	while ( v0 < 40 );
}

int __fastcall DRLG_L3PlaceMiniSet(const unsigned char *miniset, int tmin, int tmax, int cx, int cy, bool setview, int ldir)
{
	int v7; // ebx
	int v8; // esi
	int v9; // edi
	int v10; // edx
	int v11; // esi
	signed int v12; // ebx
	int v13; // edi
	signed int i; // eax
	int v15; // ecx
	unsigned char v16; // dl
	int v17; // eax
	int v18; // ecx
	int v19; // edi
	char *v20; // edx
	char v21; // bl
	const unsigned char *v23; // [esp+Ch] [ebp-24h]
	int v24; // [esp+10h] [ebp-20h]
	int v25; // [esp+14h] [ebp-1Ch]
	int v26; // [esp+18h] [ebp-18h]
	signed int v27; // [esp+1Ch] [ebp-14h]
	int v28; // [esp+20h] [ebp-10h]
	int v29; // [esp+24h] [ebp-Ch]
	int v30; // [esp+28h] [ebp-8h]
	int max; // [esp+2Ch] [ebp-4h]

	v7 = miniset[1];
	v8 = tmin;
	v9 = *miniset;
	v23 = miniset;
	v10 = tmax - tmin;
	v28 = *miniset;
	v29 = miniset[1];
	if ( v10 )
		v24 = v8 + random(0, v10);
	else
		v24 = 1;
	v25 = 0;
	if ( v24 <= 0 )
	{
		v11 = tmax;
	}
	else
	{
		max = 40 - v9;
		v30 = 40 - v7;
		do
		{
			v11 = random(0, max);
			v27 = 0;
			tmax = random(0, v30);
			while ( 1 )
			{
				if ( v27 >= 200 )
					return 1;
				++v27;
				v12 = 1;
				if ( cx != -1 && v11 >= cx - v28 && v11 <= cx + 12 )
				{
					v11 = random(0, max);
					tmax = random(0, v30);
					v12 = 0;
				}
				if ( cy != -1 && tmax >= cy - v29 && tmax <= cy + 12 )
				{
					v11 = random(0, max);
					tmax = random(0, v30);
					v12 = 0;
				}
				v13 = 0;
				for ( i = 2; v13 < v29; ++v13 )
				{
					if ( v12 != 1 )
						break;
					v26 = 0;
					if ( v28 > 0 )
					{
						v15 = tmax + v13 + 40 * v11;
						do
						{
							if ( v12 != 1 )
								break;
							v16 = v23[i];
							if ( v16 && dungeon[0][v15] != v16 )
								v12 = 0;
							if ( dflags[0][v15] )
								v12 = 0;
							++i;
							++v26;
							v15 += 40;
						}
						while ( v26 < v28 );
					}
				}
				v17 = 0;
				if ( v12 )
					break;
				if ( ++v11 == max )
				{
					v11 = 0;
					if ( ++tmax == v30 )
						tmax = 0;
				}
			}
			if ( v27 >= 200 )
				return 1;
			v18 = v28 * v29 + 2;
			if ( v29 > 0 )
			{
				do
				{
					v19 = v28;
					if ( v28 > 0 )
					{
						v20 = &dungeon[v11][v17 + tmax];
						do
						{
							v21 = v23[v18];
							if ( v21 )
								*v20 = v21;
							++v18;
							v20 += 40;
							--v19;
						}
						while ( v19 );
					}
					++v17;
				}
				while ( v17 < v29 );
			}
			++v25;
		}
		while ( v25 < v24 );
	}
	if ( setview == 1 )
	{
		ViewX = 2 * v11 + 17;
		ViewY = 2 * tmax + 19;
	}
	if ( !ldir )
	{
		LvlViewX = 2 * v11 + 17;
		LvlViewY = 2 * tmax + 19;
	}
	return 0;
}
// 5CF320: using guessed type int LvlViewY;
// 5CF324: using guessed type int LvlViewX;

void __fastcall DRLG_L3PlaceRndSet(const unsigned char *miniset, int rndper)
{
	const unsigned char *v2; // ebx
	int v3; // ecx
	int v4; // eax
	char *v5; // ecx
	int v6; // esi
	signed int i; // edx
	int v8; // edi
	int v9; // eax
	unsigned char v10; // cl
	int v11; // edi
	unsigned char v12; // al
	char v13; // al
	int j; // edx
	int v15; // esi
	unsigned char *v16; // eax
	unsigned char v17; // cl
	bool v18; // zf
	int v19; // [esp+8h] [ebp-30h]
	int v20; // [esp+10h] [ebp-28h]
	char *v21; // [esp+14h] [ebp-24h]
	int v22; // [esp+18h] [ebp-20h]
	int v23; // [esp+1Ch] [ebp-1Ch]
	int v24; // [esp+20h] [ebp-18h]
	int v25; // [esp+24h] [ebp-14h]
	int v26; // [esp+28h] [ebp-10h]
	int v27; // [esp+2Ch] [ebp-Ch]
	int v28; // [esp+30h] [ebp-8h]
	signed int v29; // [esp+34h] [ebp-4h]

	v2 = miniset;
	v19 = rndper;
	v3 = miniset[1];
	v4 = 0;
	v23 = 40 - v3;
	v27 = *v2;
	v28 = v3;
	v24 = 0;
	if ( 40 - v3 > 0 )
	{
		v22 = 40 - *v2;
		v21 = dungeon[-1];
		while ( v22 <= 0 )
		{
LABEL_44:
			v4 = v24++ + 1;
			if ( v24 >= v23 )
				return;
		}
		v26 = 0;
		v20 = v22;
		v5 = &v21[v4];
		v25 = (int)&v21[v4];
		while ( 1 )
		{
			v6 = 0;
			v29 = 1;
			for ( i = 2; v6 < v28; ++v6 )
			{
				if ( v29 != 1 )
					break;
				v8 = 0;
				if ( v27 > 0 )
				{
					v9 = v24 + v6 + v26;
					do
					{
						if ( v29 != 1 )
							break;
						v10 = v2[i];
						if ( v10 && dungeon[0][v9] != v10 )
							v29 = 0;
						if ( dflags[0][v9] )
							v29 = 0;
						++i;
						++v8;
						v9 += 40;
					}
					while ( v8 < v27 );
					v5 = (char *)v25;
				}
			}
			v11 = v27 * v28 + 2;
			v12 = v2[v11];
			if ( v12 < 0x54u || v12 > 0x64u )
				goto LABEL_33;
			if ( v29 == 1 )
				break;
LABEL_43:
			v26 += 40;
			v5 += 40;
			v18 = v20-- == 1;
			v25 = (int)v5;
			if ( v18 )
				goto LABEL_44;
		}
		v13 = *v5;
		if ( (unsigned char)*v5 >= 0x54u && (unsigned char)v13 <= 0x64u )
			v29 = 0;
		if ( (unsigned char)v5[80] >= 0x54u && (unsigned char)v13 <= 0x64u )
			v29 = 0;
		if ( (unsigned char)v5[41] >= 0x54u && (unsigned char)v13 <= 0x64u )
			v29 = 0;
		if ( (unsigned char)v5[39] >= 0x54u && (unsigned char)v13 <= 0x64u )
			v29 = 0;
LABEL_33:
		if ( v29 == 1 )
		{
			if ( random(0, 100) < v19 )
			{
				for ( j = 0; j < v28; ++j )
				{
					v15 = v27;
					if ( v27 > 0 )
					{
						v16 = (unsigned char *)dungeon + j + v26 + v24;
						do
						{
							v17 = v2[v11];
							if ( v17 )
								*v16 = v17;
							++v11;
							v16 += 40;
							--v15;
						}
						while ( v15 );
					}
				}
			}
			v5 = (char *)v25;
		}
		goto LABEL_43;
	}
}

void __cdecl DRLG_L3Wood()
{
	char *v0; // edi
	int v1; // edx
	_BYTE *v2; // eax
	int v3; // edx
	_BYTE *v4; // ebx
	int v5; // esi
	int v6; // esi
	int v7; // ebx
	int v8; // ebx
	signed int v9; // esi
	_BYTE *v10; // eax
	int v11; // esi
	int v12; // ebx
	int v13; // eax
	_BYTE *v14; // ecx
	int v15; // ecx
	int v16; // eax
	int v17; // esi
	int v18; // esi
	//int v19; // eax
	int v20; // edi
	int v21; // esi
	int i; // edx
	int v23; // esi
	int v24; // edi
	signed int v25; // ecx
	int v26; // ebx
	char *v27; // esi
	int v28; // ecx
	int v29; // edi
	int v30; // ecx
	int v31; // edi
	int v32; // ebx
	int v33; // ebx
	char *v34; // esi
	signed int v35; // ecx
	int v36; // [esp+Ch] [ebp-14h]
	int v37; // [esp+10h] [ebp-10h]
	int v38; // [esp+10h] [ebp-10h]
	int v39; // [esp+10h] [ebp-10h]
	int v40; // [esp+10h] [ebp-10h]
	int v41; // [esp+10h] [ebp-10h]
	int x; // [esp+14h] [ebp-Ch]
	int xa; // [esp+14h] [ebp-Ch]
	signed int v44; // [esp+18h] [ebp-8h]
	signed int v45; // [esp+18h] [ebp-8h]
	int y; // [esp+1Ch] [ebp-4h]
	signed int ya; // [esp+1Ch] [ebp-4h]

	y = 0;
	do
	{
		x = 0;
		v44 = 1;
		v0 = (char *)dungeon + y;
		do
		{
			if ( *v0 == 10 && random(0, 2) )
			{
				v1 = v44 - 1;
				if ( *v0 == 10 )
				{
					v2 = (unsigned char *)v0;
					do
					{
						v2 += 40;
						++v1;
					}
					while ( *v2 == 10 );
				}
				v3 = v1 - 1;
				v37 = v3;
				if ( v3 - (v44 - 1) > 0 )
				{
					*v0 = 127;
					if ( v44 < v3 )
					{
						v4 = (unsigned char *)v0 + 40;
						v5 = v3 - v44;
						do
						{
							*v4 = random(0, 2) != 0 ? 126 : -127;
							v4 += 40;
							--v5;
						}
						while ( v5 );
					}
					dungeon[v37][y] = -128;
				}
			}
			if ( *v0 == 9 && random(0, 2) )
			{
				v6 = y;
				v7 = y;
				if ( *v0 == 9 )
				{
					do
						++v7;
					while ( dungeon[x][v7] == 9 );
				}
				v8 = v7 - 1;
				if ( v8 - y > 0 )
				{
					*v0 = 123;
					while ( ++v6 < v8 )
					{
						if ( random(0, 2) )
							dungeon[x][v6] = 121;
						else
							dungeon[x][v6] = 124;
					}
					dungeon[x][v8] = 122;
				}
			}
			if ( *v0 == 11 && v0[40] == 10 && v0[1] == 9 && random(0, 2) )
			{
				v9 = v44;
				*v0 = 125;
				if ( v0[40] == 10 )
				{
					v10 = (unsigned char *)v0 + 40;
					do
					{
						v10 += 40;
						++v9;
					}
					while ( *v10 == 10 );
				}
				v11 = v9 - 1;
				if ( v44 < v11 )
				{
					v38 = (int)(v0 + 40);
					v12 = v11 - v44;
					do
					{
						v13 = random(0, 2);
						v14 = (_BYTE *)v38;
						v38 += 40;
						--v12;
						*v14 = v13 != 0 ? 126 : -127;
					}
					while ( v12 );
				}
				v15 = v11;
				v16 = y + 1;
				v17 = v16;
				for ( dungeon[v15][v16 - 1] = 128; dungeon[x][v17] == 9; ++v17 ) /* check  *((_BYTE *)&dMonster[111][2 * v15 + 111] + v16 + 3) */
					;
				v18 = v17 - 1;
				v39 = y + 1;
				if ( v16 < v18 )
				{
					do
					{
						if ( random(0, 2) )
							dungeon[x][v39] = 121;
						else
							dungeon[x][v39] = 124;
						++v39;
					}
					while ( v39 < v18 );
				}
				dungeon[x][v18] = 122;
			}
			++v44;
			++x;
			v0 += 40;
		}
		while ( v44 - 1 < 39 );
		++y;
	}
	while ( y < 39 );
	ya = 0;
	do
	{
		xa = 0;
		v45 = 0;
		do
		{
			if ( dungeon[v45][ya] != 7 )
				goto LABEL_112;
			if ( random(0, 1) )
				goto LABEL_112;
			//_LOBYTE(v19) = SkipThemeRoom(xa, ya);
			if ( !SkipThemeRoom(xa, ya) )
				goto LABEL_112;
			v36 = random(0, 2);
			if ( !v36 )
			{
				v20 = ya;
				v21 = ya;
				for ( i = ya; WoodVertU(xa, i); i = v21 )
					--v21;
				v23 = v21 + 1;
				while ( WoodVertD(xa, v20) )
					++v20;
				v24 = v20 - 1;
				v25 = 1;
				if ( dungeon[v45][v23] == 7 )
					v25 = 0;
				if ( dungeon[v45][v24] == 7 )
					v25 = 0;
				if ( v24 - v23 <= 1 )
					goto LABEL_112;
				if ( !v25 )
					goto LABEL_112;
				v40 = random(0, v24 - v23 - 1) + v23 + 1;
				v26 = v23;
				if ( v23 > v24 )
					goto LABEL_112;
				do
				{
					if ( v26 != v40 )
					{
						v27 = &dungeon[v45][v26];
						if ( *v27 == 7 )
							*v27 = random(0, 2) != 0 ? -121 : -119;
						if ( *v27 == 10 )
							*v27 = -125;
						if ( *v27 == 126 )
							*v27 = -123;
						if ( *v27 == -127 )
							*v27 = -123;
						if ( *v27 == 2 )
							*v27 = -117;
						if ( *v27 == -122 )
							*v27 = -118;
						if ( *v27 == -120 )
							*v27 = -118;
					}
					++v26;
				}
				while ( v26 <= v24 );
			}
			if ( v36 == 1 )
			{
				v28 = xa;
				v29 = xa;
				while ( WoodHorizL(v28, ya) )
					v28 = --v29;
				v30 = xa;
				v31 = v29 + 1;
				v32 = xa;
				while ( WoodHorizR(v30, ya) )
					v30 = ++v32;
				v33 = v32 - 1;
				v34 = &dungeon[v31][ya];
				v35 = 1;
				if ( *v34 == 7 )
					v35 = 0;
				if ( dungeon[v33][ya] == 7 )
					v35 = 0;
				if ( v33 - v31 > 1 && v35 )
				{
					v41 = random(0, v33 - v31 - 1) + v31 + 1;
					while ( 1 )
					{
						if ( v31 > v33 )
							break;
						if ( v31 != v41 )
						{
							if ( *v34 == 7 )
							{
								if ( random(0, 2) )
								{
									*v34 = -122;
									goto LABEL_110;
								}
								*v34 = -120;
							}
							if ( *v34 == 9 )
								*v34 = -126;
							if ( *v34 == 121 )
								*v34 = -124;
							if ( *v34 == 124 )
								*v34 = -124;
							if ( *v34 == 4 )
								*v34 = -116;
							if ( *v34 == -121 )
								*v34 = -118;
							if ( *v34 == -119 )
								*v34 = -118;
						}
LABEL_110:
						++v31;
						v34 += 40;
					}
				}
			}
LABEL_112:
			++v45;
			++xa;
		}
		while ( v45 < 40 );
		++ya;
	}
	while ( ya < 40 );
	AddFenceDoors();
	FenceDoorFix();
}

bool __fastcall WoodVertU(int i, int y)
{
	int v2; // eax
	char v3; // cl
	char *v4; // eax
	unsigned char v5; // cl
	char v6; // al
	bool result; // eax

	v2 = i;
	v3 = dungeon[i + 1][y];
	result = 0;
	if ( (unsigned char)v3 > 0x98u || (unsigned char)v3 < 0x82u )
	{
		v4 = &dungeon[v2][y];
		v5 = *(v4 - 40);
		if ( v5 > 0x98u || v5 < 0x82u )
		{
			v6 = *v4;
			if ( v6 == 7 || v6 == 10 || v6 == 126 || v6 == -127 || v6 == -122 || v6 == -120 )
				result = 1;
		}
	}
	return result;
}

bool __fastcall WoodVertD(int i, int y)
{
	int v2; // eax
	char v3; // cl
	char *v4; // eax
	unsigned char v5; // cl
	char v6; // al
	bool result; // eax

	v2 = i;
	v3 = dungeon[i + 1][y];
	result = 0;
	if ( (unsigned char)v3 > 0x98u || (unsigned char)v3 < 0x82u )
	{
		v4 = &dungeon[v2][y];
		v5 = *(v4 - 40);
		if ( v5 > 0x98u || v5 < 0x82u )
		{
			v6 = *v4;
			if ( v6 == 7 || v6 == 2 || v6 == -122 || v6 == -120 )
				result = 1;
		}
	}
	return result;
}

bool __fastcall WoodHorizL(int x, int j)
{
	int v2; // eax
	char v3; // cl
	char *v4; // eax
	unsigned char v5; // cl
	char v6; // al
	bool result; // eax

	v2 = x;
	v3 = dungeon[x][j + 1];
	result = 0;
	if ( (unsigned char)v3 > 0x98u || (unsigned char)v3 < 0x82u )
	{
		v4 = &dungeon[v2][j];
		v5 = *(v4 - 1);
		if ( v5 > 0x98u || v5 < 0x82u )
		{
			v6 = *v4;
			if ( v6 == 7 || v6 == 9 || v6 == 121 || v6 == 124 || v6 == -121 || v6 == -119 )
				result = 1;
		}
	}
	return result;
}

bool __fastcall WoodHorizR(int x, int j)
{
	int v2; // eax
	char v3; // cl
	char *v4; // eax
	unsigned char v5; // cl
	char v6; // al
	bool result; // eax

	v2 = x;
	v3 = dungeon[x][j + 1];
	result = 0;
	if ( (unsigned char)v3 > 0x98u || (unsigned char)v3 < 0x82u )
	{
		v4 = &dungeon[v2][j];
		v5 = *(v4 - 1);
		if ( v5 > 0x98u || v5 < 0x82u )
		{
			v6 = *v4;
			if ( v6 == 7 || v6 == 4 || v6 == -121 || v6 == -119 )
				result = 1;
		}
	}
	return result;
}

void __cdecl DRLG_L3Pass3()
{
	int v0; // eax
	int *v1; // esi
	int *v2; // eax
	signed int v3; // ecx
	signed int v4; // ebx
	int *v5; // ecx
	unsigned char *v6; // edi
	unsigned short *v7; // esi
	unsigned short v8; // ax
	int v9; // eax
	signed int v10; // [esp+Ch] [ebp-1Ch]
	int *v11; // [esp+10h] [ebp-18h]
	int v12; // [esp+14h] [ebp-14h]
	int v13; // [esp+18h] [ebp-10h]
	int v14; // [esp+18h] [ebp-10h]
	int v15; // [esp+1Ch] [ebp-Ch]
	int v16; // [esp+1Ch] [ebp-Ch]
	int v17; // [esp+20h] [ebp-8h]
	int v18; // [esp+20h] [ebp-8h]
	int v19; // [esp+24h] [ebp-4h]
	int v20; // [esp+24h] [ebp-4h]

	v0 = *((unsigned short *)pMegaTiles + 28) + 1;
	v19 = *((unsigned short *)pMegaTiles + 28) + 1;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 29);
	v17 = ++v0;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 30);
	v15 = ++v0;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 31);
	v13 = v0 + 1;
	v1 = dPiece[1];
	do
	{
		v2 = v1;
		v3 = 56;
		do
		{
			*(v2 - 112) = v19;
			*v2 = v17;
			*(v2 - 111) = v15;
			v2[1] = v13;
			v2 += 224;
			--v3;
		}
		while ( v3 );
		v1 += 2;
	}
	while ( (signed int)v1 < (signed int)dPiece[2] );
	v4 = 0;
	v11 = &dPiece[17][16];
	do
	{
		v5 = v11;
		v6 = (unsigned char *)dungeon + v4;
		v10 = 40;
		do
		{
			v12 = *v6 - 1;
			if ( v12 < 0 )
			{
				v20 = 0;
				v18 = 0;
				v16 = 0;
				v14 = 0;
			}
			else
			{
				v7 = (unsigned short *)((char *)pMegaTiles + 8 * v12);
				v8 = *v7;
				++v7;
				v9 = v8 + 1;
				v20 = v9;
				_LOWORD(v9) = *v7;
				++v7;
				v18 = ++v9;
				_LOWORD(v9) = *v7;
				v16 = ++v9;
				_LOWORD(v9) = v7[1];
				v14 = v9 + 1;
			}
			v6 += 40;
			*(v5 - 112) = v20;
			*v5 = v18;
			*(v5 - 111) = v16;
			v5[1] = v14;
			v5 += 224;
			--v10;
		}
		while ( v10 );
		v11 += 2;
		++v4;
	}
	while ( v4 < 40 );
}

void __fastcall LoadL3Dungeon(char *sFileName, int vx, int vy)
{
	char *v3; // esi
	unsigned char *v4; // eax
	char *v5; // esi
	int v6; // edi
	int v7; // ecx
	_BYTE *v8; // eax
	_BYTE *v9; // edx
	int v10; // ebx
	signed int v11; // ecx
	_BYTE *v12; // eax
	signed int v13; // edx
	int v14; // edi
	signed int v15; // eax
	int v16; // [esp+Ch] [ebp-8h]
	signed int *v17; // [esp+Ch] [ebp-8h]
	int v18; // [esp+10h] [ebp-4h]
	int (*v19)[112]; // [esp+10h] [ebp-4h]

	v3 = sFileName;
	InitL3Dungeon();
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	DRLG_InitTrans();
	v4 = LoadFileInMem(v3, 0);
	v5 = (char *)v4;
	v18 = 0;
	v6 = *v4;
	v4 += 2;
	v7 = *v4;
	v8 = v4 + 2;
	if ( v7 > 0 )
	{
		do
		{
			if ( v6 > 0 )
			{
				v16 = v6;
				v9 = (unsigned char *)dungeon + v18;
				do
				{
					if ( *v8 )
						*v9 = *v8;
					else
						*v9 = 7;
					v9 += 40;
					v8 += 2;
					--v16;
				}
				while ( v16 );
			}
			++v18;
		}
		while ( v18 < v7 );
	}
	v10 = 0;
	v11 = 0;
	do
	{
		v12 = (unsigned char *)dungeon + v11;
		v13 = 40;
		do
		{
			if ( !*v12 )
				*v12 = 8;
			v12 += 40;
			--v13;
		}
		while ( v13 );
		++v11;
	}
	while ( v11 < 40 );
	abyssx = 112;
	DRLG_L3Pass3();
	DRLG_Init_Globals();
	ViewX = 31;
	ViewY = 83;
	SetMapMonsters((unsigned char *)v5, 0, 0);
	SetMapObjects((unsigned char *)v5, 0, 0);
	v19 = dPiece;
	do
	{
		v14 = 0;
		v17 = (signed int *)v19;
		do
		{
			v15 = *v17;
			if ( *v17 >= 56 && v15 <= 147 || v15 >= 154 && v15 <= 161 || v15 == 150 || v15 == 152 )
				DoLighting(v14, v10, 7, -1);
			v17 += 112;
			++v14;
		}
		while ( v14 < 112 );
		v19 = (int (*)[112])((char *)v19 + 4);
		++v10;
	}
	while ( (signed int)v19 < (signed int)dPiece[1] );
	mem_free_dbg(v5);
}
// 52837C: using guessed type int abyssx;
// 5CF328: using guessed type int dmaxx;
// 5CF32C: using guessed type int dmaxy;
// 5D2458: using guessed type int dminx;
// 5D245C: using guessed type int dminy;

void __fastcall LoadPreL3Dungeon(char *sFileName, int vx, int vy)
{
	char *v3; // esi
	unsigned char *v4; // eax
	unsigned char *v5; // esi
	int v6; // edx
	int v7; // edi
	_BYTE *v8; // eax
	_BYTE *v9; // ecx
	signed int v10; // ecx
	_BYTE *v11; // eax
	signed int v12; // edx
	int v13; // [esp+8h] [ebp-8h]
	int v14; // [esp+Ch] [ebp-4h]

	v3 = sFileName;
	InitL3Dungeon();
	DRLG_InitTrans();
	v4 = LoadFileInMem(v3, 0);
	v5 = v4;
	v14 = 0;
	v6 = *v4;
	v4 += 2;
	v7 = *v4;
	v8 = v4 + 2;
	if ( v7 > 0 )
	{
		do
		{
			if ( v6 > 0 )
			{
				v13 = v6;
				v9 = (unsigned char *)dungeon + v14;
				do
				{
					if ( *v8 )
						*v9 = *v8;
					else
						*v9 = 7;
					v9 += 40;
					v8 += 2;
					--v13;
				}
				while ( v13 );
			}
			++v14;
		}
		while ( v14 < v7 );
	}
	v10 = 0;
	do
	{
		v11 = (unsigned char *)dungeon + v10;
		v12 = 40;
		do
		{
			if ( !*v11 )
				*v11 = 8;
			v11 += 40;
			--v12;
		}
		while ( v12 );
		++v10;
	}
	while ( v10 < 40 );
	memcpy(pdungeon, dungeon, 0x640u);
	mem_free_dbg(v5);
}
