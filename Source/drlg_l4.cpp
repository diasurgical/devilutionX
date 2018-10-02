//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int diabquad1x; // weak
int diabquad1y; // weak
int diabquad3x; // idb
int diabquad3y; // idb
int diabquad2x; // idb
int diabquad2y; // idb
int diabquad4x; // idb
int diabquad4y; // idb
int hallok[20];
int l4holdx; // weak
int l4holdy; // weak
int SP4x1; // idb
int SP4x2; // weak
int SP4y1; // idb
int SP4y2; // weak
char L4dungeon[80][80];
char dung[20][20];
//int dword_52A4DC; // weak
#endif

const unsigned char L4ConvTbl[16] = { 30u, 6u, 1u, 6u, 2u, 6u, 6u, 6u, 9u, 6u, 1u, 6u, 2u, 6u, 3u, 6u };
const unsigned char L4USTAIRS[42] =
{
  4u,
  5u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  0u,
  0u,
  0u,
  0u,
  36u,
  38u,
  35u,
  0u,
  37u,
  34u,
  33u,
  32u,
  0u,
  0u,
  31u,
  0u,
  0u,
  0u,
  0u,
  0u
};
const unsigned char L4TWARP[42] =
{
  4u,
  5u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  0u,
  0u,
  0u,
  0u,
  134u,
  136u,
  133u,
  0u,
  135u,
  132u,
  131u,
  130u,
  0u,
  0u,
  129u,
  0u,
  0u,
  0u,
  0u,
  0u
};
const unsigned char L4DSTAIRS[52] =
{
  5u,
  5u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  0u,
  0u,
  0u,
  0u,
  0u,
  0u,
  0u,
  45u,
  41u,
  0u,
  0u,
  44u,
  43u,
  40u,
  0u,
  0u,
  46u,
  42u,
  39u,
  0u,
  0u,
  0u,
  0u,
  0u,
  0u
};
const unsigned char L4PENTA[52] =
{
  5u,
  5u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  0u,
  0u,
  0u,
  0u,
  0u,
  0u,
  98u,
  100u,
  103u,
  0u,
  0u,
  99u,
  102u,
  105u,
  0u,
  0u,
  101u,
  104u,
  106u,
  0u,
  0u,
  0u,
  0u,
  0u,
  0u
};
const unsigned char L4PENTA2[52] =
{
  5u,
  5u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  6u,
  0u,
  0u,
  0u,
  0u,
  0u,
  0u,
  107u,
  109u,
  112u,
  0u,
  0u,
  108u,
  111u,
  114u,
  0u,
  0u,
  110u,
  113u,
  115u,
  0u,
  0u,
  0u,
  0u,
  0u,
  0u
};
const unsigned char L4BTYPES[140] =
{
	0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
   10,  11,  12,  13,  14,  15,  16,  17,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   6,
	6,   6,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   1,   2,   1,   2,   1,   2,   1,   1,   2,
	2,   0,   0,   0,   0,   0,   0,  15,  16,   9,
   12,   4,   5,   7,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

void __cdecl DRLG_LoadL4SP()
{
	setloadflag_2 = 0;
	if ( QuestStatus(QTYPE_WARLRD) )
	{
		pSetPiece_2 = (char *)LoadFileInMem("Levels\\L4Data\\Warlord.DUN", 0);
		setloadflag_2 = 1;
	}
	if ( currlevel == 15 && gbMaxPlayers != 1 )
	{
		pSetPiece_2 = (char *)LoadFileInMem("Levels\\L4Data\\Vile1.DUN", 0);
		setloadflag_2 = 1;
	}
}
// 5B50D8: using guessed type int setloadflag_2;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl DRLG_FreeL4SP()
{
	char *v0; // ecx

	v0 = pSetPiece_2;
	pSetPiece_2 = 0;
	mem_free_dbg(v0);
}

void __fastcall DRLG_L4SetSPRoom(int rx1, int ry1)
{
	int v2; // edi
	int v3; // esi
	int v4; // eax
	char v5; // bl
	int v6; // [esp+8h] [ebp-Ch]
	char *v7; // [esp+Ch] [ebp-8h]
	int v8; // [esp+10h] [ebp-4h]

	v8 = 0;
	v2 = (unsigned char)pSetPiece_2[2];
	v3 = (unsigned char)*pSetPiece_2;
	setpc_x = rx1;
	setpc_y = ry1;
	setpc_w = v3;
	setpc_h = v2;
	v7 = pSetPiece_2 + 4;
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
						dflags[0][v4] |= DFLAG_EXPLORED;
						dungeon[0][v4] = v5;
					}
					else
					{
						dungeon[0][v4] = 6;
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

void __cdecl L4SaveQuads()
{
	char *v0; // esi
	char *v1; // edx
	char *v2; // edi
	char *v3; // eax
	char *v4; // ecx
	char *v5; // ebx
	signed int v6; // [esp+Ch] [ebp-14h]
	signed int v7; // [esp+10h] [ebp-10h]
	char *v8; // [esp+14h] [ebp-Ch]
	char *v9; // [esp+18h] [ebp-8h]
	char *v10; // [esp+1Ch] [ebp-4h]

	v0 = &dflags[39][l4holdy - 40 * l4holdx]; /* check */
	v1 = &dflags[39][-l4holdy + 39 + -40 * l4holdx];
	v9 = &dflags[l4holdx][l4holdy];
	v8 = &dflags[0][40 * l4holdx - l4holdy + 39];
	v6 = 14;
	do
	{
		v2 = v1;
		v10 = v8;
		v3 = v9;
		v4 = v0;
		v7 = 14;
		do
		{
			v5 = v10;
			v10 += 40;
			*v3 = 1;
			*v4 = 1;
			*v5 = 1;
			*v2 = 1;
			v4 -= 40;
			v2 -= 40;
			v3 += 40;
			--v7;
		}
		while ( v7 );
		++v9;
		--v8;
		--v1;
		++v0;
		--v6;
	}
	while ( v6 );
}
// 528A34: using guessed type int l4holdx;
// 528A38: using guessed type int l4holdy;

void __fastcall DRLG_L4SetRoom(unsigned char *pSetPiece, int rx1, int ry1)
{
	int v3; // ebx
	int v4; // edi
	unsigned char *v5; // esi
	int v6; // eax
	char v7; // cl
	int v8; // [esp+Ch] [ebp-8h]
	int v9; // [esp+10h] [ebp-4h]

	v3 = *pSetPiece;
	v4 = 0;
	v8 = pSetPiece[2];
	v5 = pSetPiece + 4;
	if ( v8 > 0 )
	{
		do
		{
			if ( v3 > 0 )
			{
				v9 = v3;
				v6 = ry1 + v4 + 40 * rx1;
				do
				{
					v7 = *v5;
					if ( *v5 )
					{
						dflags[0][v6] |= DFLAG_EXPLORED;
						dungeon[0][v6] = v7;
					}
					else
					{
						dungeon[0][v6] = 6;
					}
					v6 += 40;
					v5 += 2;
					--v9;
				}
				while ( v9 );
			}
			++v4;
		}
		while ( v4 < v8 );
	}
}

void __fastcall DRLG_LoadDiabQuads(bool preflag)
{
	bool v1; // esi
	unsigned char *v2; // edi
	char *v3; // ecx
	unsigned char *v4; // edi
	char *v5; // ecx
	unsigned char *v6; // edi
	char *v7; // ecx
	unsigned char *v8; // esi

	v1 = preflag;
	v2 = LoadFileInMem("Levels\\L4Data\\diab1.DUN", 0);
	diabquad1x = l4holdx + 4;
	diabquad1y = l4holdy + 4;
	DRLG_L4SetRoom(v2, l4holdx + 4, l4holdy + 4);
	mem_free_dbg(v2);
	v3 = "Levels\\L4Data\\diab2b.DUN";
	if ( !v1 )
		v3 = "Levels\\L4Data\\diab2a.DUN";
	v4 = LoadFileInMem(v3, 0);
	diabquad2y = l4holdy + 1;
	diabquad2x = 27 - l4holdx;
	DRLG_L4SetRoom(v4, 27 - l4holdx, l4holdy + 1);
	mem_free_dbg(v4);
	v5 = "Levels\\L4Data\\diab3b.DUN";
	if ( !v1 )
		v5 = "Levels\\L4Data\\diab3a.DUN";
	v6 = LoadFileInMem(v5, 0);
	diabquad3x = l4holdx + 1;
	diabquad3y = 27 - l4holdy;
	DRLG_L4SetRoom(v6, l4holdx + 1, 27 - l4holdy);
	mem_free_dbg(v6);
	v7 = "Levels\\L4Data\\diab4b.DUN";
	if ( !v1 )
		v7 = "Levels\\L4Data\\diab4a.DUN";
	v8 = LoadFileInMem(v7, 0);
	diabquad4y = 28 - l4holdy;
	diabquad4x = 28 - l4holdx;
	DRLG_L4SetRoom(v8, 28 - l4holdx, 28 - l4holdy);
	mem_free_dbg(v8);
}
// 5289C4: using guessed type int diabquad1x;
// 5289C8: using guessed type int diabquad1y;
// 528A34: using guessed type int l4holdx;
// 528A38: using guessed type int l4holdy;

bool __fastcall IsDURWall(char d)
{
	bool result; // al

	if ( d == 25 || d == 28 )
		result = 1;
	else
		result = d == 23;
	return result;
}

bool __fastcall IsDLLWall(char dd)
{
	bool result; // al

	if ( dd == 27 || dd == 26 )
		result = 1;
	else
		result = dd == 22;
	return result;
}

void __cdecl L4FixRim()
{
	char (*v0)[20]; // eax

	v0 = dung;
	do
	{
		*(_BYTE *)v0 = 0;
		++v0;
	}
	while ( (signed int)v0 < (signed int)&dung[20][0] );
	*(_DWORD *)&dung[0][0] = 0;
	*(_DWORD *)&dung[0][4] = 0;
	*(_DWORD *)&dung[0][8] = 0;
	*(_DWORD *)&dung[0][12] = 0;
	*(_DWORD *)&dung[0][16] = 0;
}
// 52A4DC: using guessed type int dword_52A4DC;

void __cdecl DRLG_L4GeneralFix()
{
	signed int v0; // ecx
	char *v1; // eax
	signed int v2; // esi

	v0 = 0;
	do
	{
		v1 = (char *)dungeon + v0;
		v2 = 39;
		do
		{
			if ( (*v1 == 24 || *v1 == 122) && v1[40] == 2 && v1[1] == 5 )
				*v1 = 17;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 39 );
}

void __fastcall CreateL4Dungeon(int rseed, int entry)
{
	int v2; // esi

	v2 = entry;
	SetRndSeed(rseed);
	dminx = 16;
	dminy = 16;
	dmaxx = 96;
	dmaxy = 96;
	ViewX = 40;
	ViewY = 40;
	DRLG_InitSetPC();
	DRLG_LoadL4SP();
	DRLG_L4(v2);
	DRLG_L4Pass3();
	DRLG_FreeL4SP();
	DRLG_SetPC();
}
// 5CF328: using guessed type int dmaxx;
// 5CF32C: using guessed type int dmaxy;
// 5D2458: using guessed type int dminx;
// 5D245C: using guessed type int dminy;

void __fastcall DRLG_L4(int entry)
{
	signed int v1; // ebp
	//int v2; // eax
	int v3; // edx
	char *v4; // edi
	char v5; // bp
	unsigned int v6; // ecx
	char *v7; // edi
	int v8; // ecx
	//int v9; // eax
	int v10; // eax
	unsigned char *v11; // ecx
	unsigned char *v12; // ecx
	//int v13; // eax
	signed int v14; // eax
	signed int v15; // ecx
	int v16; // ebx
	int v17; // edi
	char *v18; // ebp
	signed int v19; // ecx
	signed int v20; // eax
	signed int v21; // esi
	int v22; // [esp-8h] [ebp-20h]
	int v23; // [esp+10h] [ebp-8h]
	int v24; // [esp+14h] [ebp-4h]

	v1 = 0;
	v23 = entry;
	do
	{
		DRLG_InitTrans();
		do
		{
			InitL4Dungeon();
			L4firstRoom();
			L4FixRim();
		}
		while ( GetArea() < 173 );
		uShape();
		L4makeDungeon();
		L4makeDmt();
		L4tileFix();
		if ( currlevel == 16 )
			L4SaveQuads();
		//_LOBYTE(v2) = QuestStatus(QTYPE_WARLRD);
		if ( (QuestStatus(QTYPE_WARLRD) || currlevel == quests[QTYPE_VB]._qlevel && gbMaxPlayers != 1) && SP4x1 < SP4x2 )
		{
			v3 = SP4x1;
			v24 = SP4x2 - SP4x1;
			do
			{
				if ( SP4y1 < SP4y2 )
				{
					v4 = &dflags[v3][SP4y1];
					v5 = SP4y2 - SP4y1;
					v6 = (unsigned int)(SP4y2 - SP4y1) >> 2;
					memset(v4, 1u, 4 * v6);
					v7 = &v4[4 * v6];
					v8 = v5 & 3;
					v1 = 0;
					memset(v7, 1, v8);
				}
				++v3;
				--v24;
			}
			while ( v24 );
		}
		L4AddWall();
		DRLG_L4FloodTVal();
		DRLG_L4TransFix();
		if ( setloadflag_2 )
			DRLG_L4SetSPRoom(SP4x1, SP4y1);
		if ( currlevel == 16 )
			DRLG_LoadDiabQuads(1);
		//_LOBYTE(v9) = QuestStatus(QTYPE_WARLRD);
		if ( !QuestStatus(QTYPE_WARLRD) )
		{
			if ( currlevel == 15 )
			{
				if ( !v23 )
				{
					v10 = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
					if ( v10 )
					{
						if ( gbMaxPlayers != 1 || (v11 = (unsigned char *)L4PENTA, quests[QTYPE_MOD]._qactive == 2) )
							v11 = (unsigned char *)L4PENTA2;
						v10 = DRLG_L4PlaceMiniSet(v11, 1, 1, -1, -1, 0, 1);
					}
					goto LABEL_35;
				}
				v10 = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if ( v10 )
				{
					if ( gbMaxPlayers != 1 || (v12 = (unsigned char *)L4PENTA, quests[QTYPE_MOD]._qactive == 2) )
						v12 = (unsigned char *)L4PENTA2;
					v10 = DRLG_L4PlaceMiniSet(v12, 1, 1, -1, -1, 1, 1);
				}
			}
			else
			{
				if ( !v23 )
				{
					v10 = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
					if ( v10 )
					{
						if ( currlevel != 16 )
							v10 = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 0, 1);
						goto LABEL_31;
					}
LABEL_35:
					++ViewX;
					continue;
				}
				v10 = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
				if ( v23 != 1 )
				{
					if ( v10 )
					{
						if ( currlevel != 16 )
							v10 = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 0, 1);
LABEL_46:
						if ( v10 )
						{
							if ( currlevel == 13 )
							{
								v22 = 1;
LABEL_34:
								v10 = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, v22, 6);
								goto LABEL_35;
							}
						}
					}
					goto LABEL_35;
				}
				if ( v10 )
				{
					if ( currlevel != 16 )
						v10 = DRLG_L4PlaceMiniSet(L4DSTAIRS, 1, 1, -1, -1, 1, 1);
					if ( v10 && currlevel == 13 )
						v10 = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
				}
			}
			++ViewY;
			continue;
		}
		if ( !v23 )
		{
			v10 = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 1, 0);
LABEL_31:
			if ( !v10 || currlevel != 13 )
				goto LABEL_35;
			v22 = 0;
			goto LABEL_34;
		}
		v10 = DRLG_L4PlaceMiniSet(L4USTAIRS, 1, 1, -1, -1, 0, 0);
		if ( v23 != 1 )
			goto LABEL_46;
		if ( v10 && currlevel == 13 )
			v10 = DRLG_L4PlaceMiniSet(L4TWARP, 1, 1, -1, -1, 0, 6);
		ViewX = 2 * setpc_x + 22;
		ViewY = 2 * setpc_y + 22;
	}
	while ( !v10 );
	DRLG_L4GeneralFix();
	if ( currlevel != 16 )
		DRLG_PlaceThemeRooms(7, 10, 6, 8, 1);
	DRLG_L4Shadows();
	DRLG_L4Corners();
	DRLG_L4Subs();
	DRLG_Init_Globals();
	//_LOBYTE(v13) = QuestStatus(QTYPE_WARLRD);
	if ( QuestStatus(QTYPE_WARLRD) )
	{
		do
		{
			v14 = v1;
			v15 = 40;
			do
			{
				pdungeon[0][v14] = dungeon[0][v14];
				v14 += 40;
				--v15;
			}
			while ( v15 );
			++v1;
		}
		while ( v1 < 40 );
	}
	DRLG_CheckQuests(SP4x1, SP4y1);
	if ( currlevel == 15 )
	{
		v16 = -1;
		do
		{
			v17 = -1;
			v18 = &dungeon[0][v16 + 1];
			do
			{
				if ( *v18 == 98 )
					Make_SetPC(v17, v16, 5, 5);
				if ( *v18 == 107 )
					Make_SetPC(v17, v16, 5, 5);
				v18 += 40;
				++v17;
			}
			while ( v17 + 1 < 40 );
			++v16;
		}
		while ( v16 < 39 );
	}
	if ( currlevel == 16 )
	{
		v19 = 0;
		do
		{
			v20 = v19;
			v21 = 40;
			do
			{
				pdungeon[0][v20] = dungeon[0][v20];
				v20 += 40;
				--v21;
			}
			while ( v21 );
			++v19;
		}
		while ( v19 < 40 );
		DRLG_LoadDiabQuads(0);
	}
}
// 528A40: using guessed type int SP4x2;
// 528A48: using guessed type int SP4y2;
// 5B50D8: using guessed type int setloadflag_2;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl DRLG_L4Shadows()
{
	signed int v0; // esi
	char *v1; // eax
	signed int v2; // edi
	char v3; // dl
	signed int v4; // ecx

	v0 = 1;
	do
	{
		v1 = &dungeon[1][v0];
		v2 = 39;
		do
		{
			v3 = *v1;
			v4 = 0;
			if ( *v1 == 3 )
				v4 = 1;
			if ( v3 == 4 )
				v4 = 1;
			if ( v3 == 8 )
				v4 = 1;
			if ( v3 == 15 )
				v4 = 1;
			if ( v4 )
			{
				if ( *(v1 - 40) == 6 )
					*(v1 - 40) = 47;
				if ( *(v1 - 41) == 6 )
					*(v1 - 41) = 48;
			}
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl InitL4Dungeon()
{
	signed int v0; // edx
	signed int v1; // eax
	signed int v2; // ecx

	memset(dung, 0, sizeof(dung));
	memset(L4dungeon, 0, sizeof(L4dungeon));
	v0 = 0;
	do
	{
		v1 = v0;
		v2 = 40;
		do
		{
			dflags[0][v1] = 0;
			dungeon[0][v1] = 30;
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl L4makeDmt()
{
	signed int v0; // ecx
	char (*v1)[40]; // ebp
	char (*v2)[40]; // esi
	char *v3; // eax
	signed int v4; // edi
	int v5; // edx
	int v6; // ebx

	v0 = 1;
	v1 = dungeon;
	do
	{
		v2 = v1;
		v3 = &L4dungeon[1][v0 + 1];
		v4 = 39;
		do
		{
			v5 = (unsigned char)v3[80];
			v6 = (unsigned char)*v3;
			v3 += 160;
			*(_BYTE *)v2 = L4ConvTbl[2 * ((unsigned char)*(v3 - 81) + 2 * (v6 + 2 * v5))
								   + (unsigned char)*(v3 - 161)];
			++v2;
			--v4;
		}
		while ( v4 );
		v1 = (char (*)[40])((char *)v1 + 1);
		v0 += 2;
	}
	while ( v0 <= 77 );
}

void __cdecl L4AddWall()
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
	int v9; // eax
	int v10; // eax
	int v11; // eax
	int v12; // eax
	int v13; // eax
	int v14; // eax
	int v15; // eax
	int v16; // eax
	int v17; // eax

	v0 = 0;
	do
	{
		v1 = 0;
		v2 = v0;
		do
		{
			if ( !dflags[0][v2] )
			{
				if ( dungeon[0][v2] == 10 && random(0, 100) < 100 )
				{
					v3 = L4HWallOk(v1, v0);
					if ( v3 != -1 )
						L4HorizWall(v1, v0, v3);
				}
				if ( dungeon[0][v2] == 12 && random(0, 100) < 100 )
				{
					v4 = L4HWallOk(v1, v0);
					if ( v4 != -1 )
						L4HorizWall(v1, v0, v4);
				}
				if ( dungeon[0][v2] == 13 && random(0, 100) < 100 )
				{
					v5 = L4HWallOk(v1, v0);
					if ( v5 != -1 )
						L4HorizWall(v1, v0, v5);
				}
				if ( dungeon[0][v2] == 15 && random(0, 100) < 100 )
				{
					v6 = L4HWallOk(v1, v0);
					if ( v6 != -1 )
						L4HorizWall(v1, v0, v6);
				}
				if ( dungeon[0][v2] == 16 && random(0, 100) < 100 )
				{
					v7 = L4HWallOk(v1, v0);
					if ( v7 != -1 )
						L4HorizWall(v1, v0, v7);
				}
				if ( dungeon[0][v2] == 21 && random(0, 100) < 100 )
				{
					v8 = L4HWallOk(v1, v0);
					if ( v8 != -1 )
						L4HorizWall(v1, v0, v8);
				}
				if ( dungeon[0][v2] == 22 && random(0, 100) < 100 )
				{
					v9 = L4HWallOk(v1, v0);
					if ( v9 != -1 )
						L4HorizWall(v1, v0, v9);
				}
				if ( dungeon[0][v2] == 8 && random(0, 100) < 100 )
				{
					v10 = L4VWallOk(v1, v0);
					if ( v10 != -1 )
						L4VertWall(v1, v0, v10);
				}
				if ( dungeon[0][v2] == 9 && random(0, 100) < 100 )
				{
					v11 = L4VWallOk(v1, v0);
					if ( v11 != -1 )
						L4VertWall(v1, v0, v11);
				}
				if ( dungeon[0][v2] == 11 && random(0, 100) < 100 )
				{
					v12 = L4VWallOk(v1, v0);
					if ( v12 != -1 )
						L4VertWall(v1, v0, v12);
				}
				if ( dungeon[0][v2] == 14 && random(0, 100) < 100 )
				{
					v13 = L4VWallOk(v1, v0);
					if ( v13 != -1 )
						L4VertWall(v1, v0, v13);
				}
				if ( dungeon[0][v2] == 15 && random(0, 100) < 100 )
				{
					v14 = L4VWallOk(v1, v0);
					if ( v14 != -1 )
						L4VertWall(v1, v0, v14);
				}
				if ( dungeon[0][v2] == 16 && random(0, 100) < 100 )
				{
					v15 = L4VWallOk(v1, v0);
					if ( v15 != -1 )
						L4VertWall(v1, v0, v15);
				}
				if ( dungeon[0][v2] == 21 && random(0, 100) < 100 )
				{
					v16 = L4VWallOk(v1, v0);
					if ( v16 != -1 )
						L4VertWall(v1, v0, v16);
				}
				if ( dungeon[0][v2] == 23 && random(0, 100) < 100 )
				{
					v17 = L4VWallOk(v1, v0);
					if ( v17 != -1 )
						L4VertWall(v1, v0, v17);
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

int __fastcall L4HWallOk(int i, int j)
{
	int v2; // esi
	int v3; // edi
	char *v4; // ebx
	int result; // eax
	signed int v6; // esi
	char v7; // dl
	int v8; // [esp+8h] [ebp-4h]

	v2 = 8 * (5 * i + 5);
	v8 = 1;
	if ( dungeon[0][v2 + j] == 6 )
	{
		v3 = 8 * (5 * i + 5);
		v4 = &dungeon[i + 1][j];
		do
		{
			if ( dflags[0][v3 + j] )
				break;
			if ( dungeon[0][v3 + j - 1] != 6 ) // *((_BYTE *)&dMonster[111][111] + v3 + j + 3) != 6 ) /* check */
				break;
			if ( dungeon[0][v3 + 1 + j] != 6 )
				break;
			++v8;
			v4 += 40;
			v2 += 40;
			v3 = v2;
		}
		while ( *v4 == 6 );
	}
	result = v8;
	v6 = 0;
	v7 = dungeon[v8 + i][j];
	if ( v7 == 10 )
		v6 = 1;
	if ( v7 == 12 )
		v6 = 1;
	if ( v7 == 13 )
		v6 = 1;
	if ( v7 == 15 )
		v6 = 1;
	if ( v7 == 16 )
		v6 = 1;
	if ( v7 == 21 )
		v6 = 1;
	if ( v7 == 22 )
		v6 = 1;
	if ( v8 <= 3 )
		v6 = 0;
	if ( !v6 )
		result = -1;
	return result;
}

int __fastcall L4VWallOk(int i, int j)
{
	int v2; // ecx
	int result; // eax
	char *v4; // esi
	signed int v5; // esi
	char v6; // dl

	v2 = i;
	result = 1;
	if ( dungeon[v2][j + 1] == 6 )
	{
		do
		{
			if ( dflags[v2][j + result] )
				break;
			v4 = &dungeon[v2][j];
			if ( v4[result - 40] != 6 )
				break;
			if ( dungeon[v2 + 1][result + j] != 6 )
				break;
			++result;
		}
		while ( v4[result] == 6 );
	}
	v5 = 0;
	v6 = dungeon[0][result + v2 * 40 + j];
	if ( v6 == 8 )
		v5 = 1;
	if ( v6 == 9 )
		v5 = 1;
	if ( v6 == 11 )
		v5 = 1;
	if ( v6 == 14 )
		v5 = 1;
	if ( v6 == 15 )
		v5 = 1;
	if ( v6 == 16 )
		v5 = 1;
	if ( v6 == 21 )
		v5 = 1;
	if ( v6 == 23 )
		v5 = 1;
	if ( result <= 3 )
		v5 = 0;
	if ( !v5 )
		result = -1;
	return result;
}

void __fastcall L4HorizWall(int i, int j, int dx)
{
	int v3; // esi
	int v4; // edi
	int v5; // eax
	int v6; // ecx
	char *v7; // eax
	int v8; // edx
	char *v9; // eax
	int v10; // eax
	bool v11; // zf
	char *v12; // eax

	v3 = i;
	v4 = j;
	v5 = j + 40 * i;
	if ( dungeon[0][v5] == 13 )
		dungeon[0][v5] = 17;
	if ( dungeon[0][v5] == 16 )
		dungeon[0][v5] = 11;
	if ( dungeon[0][v5] == 12 )
		dungeon[0][v5] = 14;
	v6 = dx;
	if ( dx > 1 )
	{
		v7 = &dungeon[1][v5];
		v8 = dx - 1;
		do
		{
			*v7 = 2;
			v7 += 40;
			--v8;
		}
		while ( v8 );
	}
	v9 = &dungeon[v3 + dx][v4];
	if ( *v9 == 15 )
		*v9 = 14;
	if ( *v9 == 10 )
		*v9 = 17;
	if ( *v9 == 21 )
		*v9 = 23;
	if ( *v9 == 22 )
		*v9 = 29;
	v10 = v4 + 40 * (v3 + random(0, dx - 3) + 1);
	dungeon[2][v10] = 56;
	dungeon[1][v10] = 60;
	v11 = dungeon[0][v10 - 1] == 6;
	dungeon[0][v10] = 57;
	if ( v11 )
		dungeon[0][v10 - 1] = 58;
	v12 = &dungeon[0][v10 + 39];
	if ( *v12 == 6 )
		*v12 = 59;
}

void __fastcall L4VertWall(int i, int j, int dy)
{
	int v3; // edi
	int v4; // esi
	int v5; // edx
	int v6; // ecx
	_BYTE *v7; // eax
	int v8; // edx
	int v9; // eax
	char *v10; // ecx
	bool v11; // zf
	int v12; // [esp+8h] [ebp-4h]

	v3 = j;
	v4 = 40 * i;
	v12 = j;
	v5 = 40 * i + j;
	if ( dungeon[0][v5] == 14 )
		dungeon[0][v5] = 17;
	if ( dungeon[0][v5] == 8 )
		dungeon[0][v5] = 9;
	if ( dungeon[0][v5] == 15 )
		dungeon[0][v5] = 10;
	v6 = dy;
	if ( dy > 1 )
	{
		memset(&dungeon[0][v5 + 1], 1u, dy - 1);
		v3 = v12;
		v6 = dy;
	}
	v7 = (unsigned char *)dungeon + v5 + v6;
	if ( *v7 == 11 )
		*v7 = 17;
	if ( *v7 == 9 )
		*v7 = 10;
	if ( *v7 == 16 )
		*v7 = 13;
	if ( *v7 == 21 )
		*v7 = 22;
	if ( *v7 == 23 )
		*v7 = 29;
	v8 = v6 - 3;
	v9 = random(0, v8) + 1 + v4 + v3;
	v10 = (char *)dungeon + v9;
	dungeon[0][v9 + 2] = 52;
	dungeon[0][v9 + 1] = 6;
	v11 = dungeon[-1][v9] == 6;
	dungeon[0][v9] = 53;
	if ( v11 )
		*(v10 - 40) = 54;
	if ( *(v10 - 41) == 6 )
		*(v10 - 41) = 55;
}

void __cdecl L4tileFix()
{
	signed int v0; // edx
	char *v1; // eax
	signed int v2; // esi
	char v3; // cl
	signed int v4; // edx
	char *v5; // eax
	signed int v6; // esi
	char v7; // cl
	signed int v8; // ecx
	int v9; // eax
	int v10; // eax
	char *v11; // esi
	char v12; // bl
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
	char *v34; // edx
	char *v35; // edx
	char *v36; // edx
	char *v37; // edx
	char *v38; // edx
	char *v39; // edx
	char *v40; // edx
	char *v41; // edx
	char *v42; // edx
	char *v43; // edx
	char *v44; // edx
	char *v45; // edx
	char *v46; // edx
	char *v47; // edx
	char *v48; // edx
	char *v49; // edx
	char *v50; // edx
	char *v51; // edx
	char *v52; // edx
	char *v53; // edx
	char *v54; // edx
	char *v55; // edx
	char *v56; // edx
	char *v57; // edx
	char *v58; // edx
	char *v59; // edx
	char *v60; // edx
	char *v61; // edx
	char *v62; // edx
	char *v63; // edx
	char *v64; // edx
	char *v65; // edx
	char *v66; // edx
	char *v67; // edx
	char *v68; // edx
	char *v69; // edx
	char *v70; // edx
	char *v71; // edx
	char *v72; // edx
	char *v73; // edx
	char *v74; // edx
	char *v75; // edx
	char *v76; // edx
	char *v77; // edx
	char *v78; // edx
	char *v79; // edx
	char *v80; // edx
	char *v81; // edx
	char *v82; // edx
	char *v83; // edx
	char *v84; // edx
	char *v85; // edx
	char *v86; // edx
	char *v87; // edx
	char *v88; // edx
	char *v89; // edx
	char *v90; // edx
	char *v91; // edx
	char *v92; // edx
	char *v93; // edx
	char *v94; // edx
	char *v95; // edx
	signed int v96; // ecx
	signed int v97; // edi
	signed int v98; // eax
	char *v99; // esi
	char v100; // bl
	char *v101; // edx
	char *v102; // edx
	char *v103; // edx
	char *v104; // edx
	char *v105; // edx
	char *v106; // edx
	char *v107; // edx
	char *v108; // edx
	char *v109; // edx
	char *v110; // edx
	char *v111; // edx
	char *v112; // edx
	char *v113; // edx
	char *v114; // edx
	char *v115; // edx
	char *v116; // edx
	char *v117; // edx
	char *v118; // edx
	char *v119; // edx
	char *v120; // edx
	char *v121; // edx
	char *v122; // edx
	char *v123; // edx
	char *v124; // edx
	char *v125; // edx
	char *v126; // edx
	char *v127; // edx
	char *v128; // edx
	char *v129; // edx
	char *v130; // edx
	signed int v131; // edx
	char *v132; // eax
	signed int v133; // esi
	char v134; // cl
	signed int v135; // edx
	char *v136; // eax
	signed int v137; // esi
	char v138; // cl
	signed int v139; // [esp+8h] [ebp-4h]

	v0 = 0;
	do
	{
		v1 = &dungeon[1][v0];
		v2 = 40;
		do
		{
			v3 = *(v1 - 40);
			if ( v3 == 2 )
			{
				if ( *v1 == 6 )
					*v1 = 5;
				if ( *v1 == 1 )
					*v1 = 13;
			}
			if ( v3 == 1 && *(v1 - 39) == 2 )
				*(v1 - 39) = 14;
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
		v5 = &dungeon[1][v4];
		v6 = 40;
		do
		{
			v7 = *(v5 - 40);
			if ( v7 == 2 )
			{
				if ( *v5 == 6 )
					*v5 = 2;
				if ( *v5 == 9 )
					*v5 = 11;
			}
			if ( v7 == 9 && *v5 == 6 )
				*v5 = 12;
			if ( v7 == 14 && *v5 == 1 )
				*v5 = 13;
			if ( v7 == 6 )
			{
				if ( *v5 == 14 )
					*v5 = 15;
				if ( *(v5 - 39) == 13 )
					*(v5 - 39) = 16;
			}
			if ( v7 == 1 && *(v5 - 39) == 9 )
				*(v5 - 39) = 10;
			if ( v7 == 6 && *(v5 - 41) == 1 )
				*(v5 - 41) = 1;
			v5 += 40;
			--v6;
		}
		while ( v6 );
		++v4;
	}
	while ( v4 < 40 );
	v8 = 0;
	do
	{
		v9 = 0;
		v139 = 0;
		do
		{
			v10 = v9;
			v11 = &dungeon[v10][v8];
			v12 = *v11;
			if ( *v11 == 13 )
			{
				v13 = &dungeon[v10][v8 + 1];
				if ( *v13 == 30 )
					*v13 = 27;
			}
			if ( v12 == 27 )
			{
				v14 = &dungeon[v10 + 1][v8];
				if ( *v14 == 30 )
					*v14 = 19;
			}
			if ( v12 == 1 )
			{
				v15 = &dungeon[v10][v8 + 1];
				if ( *v15 == 30 )
					*v15 = 27;
			}
			if ( v12 == 27 )
			{
				v16 = &dungeon[v10 + 1][v8];
				if ( *v16 == 1 )
					*v16 = 16;
			}
			if ( v12 == 19 )
			{
				v17 = &dungeon[v10 + 1][v8];
				if ( *v17 == 27 )
					*v17 = 26;
			}
			if ( v12 == 27 )
			{
				v18 = &dungeon[v10 + 1][v8];
				if ( *v18 == 30 )
					*v18 = 19;
			}
			if ( v12 == 2 )
			{
				v19 = &dungeon[v10 + 1][v8];
				if ( *v19 == 15 )
					*v19 = 14;
			}
			if ( v12 == 14 )
			{
				v20 = &dungeon[v10 + 1][v8];
				if ( *v20 == 15 )
					*v20 = 14;
			}
			if ( v12 == 22 )
			{
				v21 = &dungeon[v10 + 1][v8];
				if ( *v21 == 1 )
					*v21 = 16;
			}
			if ( v12 == 27 )
			{
				v22 = &dungeon[v10 + 1][v8];
				if ( *v22 == 1 )
					*v22 = 16;
			}
			if ( v12 == 6 )
			{
				v23 = &dungeon[v10 + 1][v8];
				if ( *v23 == 27 )
				{
					if ( dungeon[v10 + 1][v8 + 1] )
						*v23 = 22;
				}
			}
			if ( v12 == 22 )
			{
				v24 = &dungeon[v10 + 1][v8];
				if ( *v24 == 30 )
					*v24 = 19;
			}
			if ( v12 == 21 )
			{
				v25 = &dungeon[v10 + 1][v8];
				if ( *v25 == 1 && dungeon[v10][v8 + 39] == 1 )
					*v25 = 13;
			}
			if ( v12 == 14 )
			{
				v26 = &dungeon[v10 + 1][v8];
				if ( *v26 == 30 && dungeon[v10][v8 + 1] == 6 )
					*v26 = 28;
			}
			if ( v12 == 16 )
			{
				if ( dungeon[v10 + 1][v8] == 6 )
				{
					v27 = &dungeon[v10][v8 + 1];
					if ( *v27 == 30 )
						*v27 = 27;
				}
				v28 = &dungeon[v10][v8 + 1];
				if ( *v28 == 30 && dungeon[v10 + 1][v8 + 1] == 30 )
					*v28 = 27;
			}
			if ( v12 == 6 )
			{
				v29 = &dungeon[v10 + 1][v8];
				if ( *v29 == 30 && dungeon[v10][v8 + 39] == 6 )
					*v29 = 21;
			}
			if ( v12 == 2 )
			{
				v30 = &dungeon[v10 + 1][v8];
				if ( *v30 == 27 && dungeon[v10 + 1][v8 + 1] == 9 )
					*v30 = 29;
			}
			if ( v12 == 9 )
			{
				v31 = &dungeon[v10 + 1][v8];
				if ( *v31 == 15 )
					*v31 = 14;
			}
			if ( v12 == 15 )
			{
				v32 = &dungeon[v10 + 1][v8];
				if ( *v32 == 27 && dungeon[v10 + 1][v8 + 1] == 2 )
					*v32 = 29;
			}
			if ( v12 == 19 )
			{
				v33 = &dungeon[v10 + 1][v8];
				if ( *v33 == 18 )
					*v33 = 24;
			}
			if ( v12 == 9 )
			{
				v34 = &dungeon[v10 + 1][v8];
				if ( *v34 == 15 )
					*v34 = 14;
			}
			if ( v12 == 19 )
			{
				v35 = &dungeon[v10 + 1][v8];
				if ( *v35 == 19 && dungeon[v10][v8 + 39] == 30 )
					*v35 = 24;
			}
			if ( v12 == 24 && *(v11 - 1) == 30 && *(v11 - 2) == 6 )
				*(v11 - 1) = 21;
			if ( v12 == 2 )
			{
				v36 = &dungeon[v10 + 1][v8];
				if ( *v36 == 30 )
					*v36 = 28;
			}
			if ( v12 == 15 )
			{
				v37 = &dungeon[v10 + 1][v8];
				if ( *v37 == 30 )
					*v37 = 28;
			}
			if ( v12 == 28 )
			{
				v38 = &dungeon[v10][v8 + 1];
				if ( *v38 == 30 )
					*v38 = 18;
				v39 = &dungeon[v10][v8 + 1];
				if ( *v39 == 2 )
					*v39 = 15;
			}
			if ( v12 == 19 )
			{
				if ( dungeon[v10 + 2][v8] == 2 && dungeon[v10][v8 + 39] == 18 && dungeon[v10 + 1][v8 + 1] == 1 )
					dungeon[v10 + 1][v8] = 17;
				if ( dungeon[v10 + 2][v8] == 2 && dungeon[v10][v8 + 39] == 22 && dungeon[v10 + 1][v8 + 1] == 1 )
					dungeon[v10 + 1][v8] = 17;
				if ( dungeon[v10 + 2][v8] == 2 && dungeon[v10][v8 + 39] == 18 && dungeon[v10 + 1][v8 + 1] == 13 )
					dungeon[v10 + 1][v8] = 17;
			}
			if ( v12 == 21 )
			{
				if ( dungeon[v10 + 2][v8] == 2 && dungeon[v10][v8 + 39] == 18 && dungeon[v10 + 1][v8 + 1] == 1 )
					dungeon[v10 + 1][v8] = 17;
				if ( dungeon[v10 + 1][v8 + 1] == 1 && dungeon[v10][v8 + 39] == 22 && dungeon[v10 + 2][v8] == 3 )
					dungeon[v10 + 1][v8] = 17;
			}
			if ( v12 == 15 )
			{
				v40 = &dungeon[v10 + 1][v8];
				if ( *v40 == 28 && dungeon[v10 + 2][v8] == 30 && dungeon[v10][v8 + 39] == 6 )
					*v40 = 23;
			}
			if ( v12 == 14 )
			{
				v41 = &dungeon[v10 + 1][v8];
				if ( *v41 == 28 && dungeon[v10 + 2][v8] == 1 )
					*v41 = 23;
			}
			if ( v12 == 15 )
			{
				v42 = &dungeon[v10 + 1][v8];
				if ( *v42 == 27 && dungeon[v10 + 1][v8 + 1] == 30 )
					*v42 = 29;
			}
			if ( v12 == 28 )
			{
				v43 = &dungeon[v10][v8 + 1];
				if ( *v43 == 9 )
					*v43 = 15;
			}
			if ( v12 == 21 && dungeon[v10][v8 + 39] == 21 )
				dungeon[v10 + 1][v8] = 24;
			if ( v12 == 2 )
			{
				v44 = &dungeon[v10 + 1][v8];
				if ( *v44 == 27 && dungeon[v10 + 1][v8 + 1] == 30 )
					*v44 = 29;
				v45 = &dungeon[v10 + 1][v8];
				if ( *v45 == 18 )
					*v45 = 25;
			}
			if ( v12 == 21 )
			{
				v46 = &dungeon[v10 + 1][v8];
				if ( *v46 == 9 && dungeon[v10 + 2][v8] == 2 )
					*v46 = 11;
			}
			if ( v12 == 19 )
			{
				v47 = &dungeon[v10 + 1][v8];
				if ( *v47 == 10 )
					*v47 = 17;
			}
			if ( v12 == 15 )
			{
				v48 = &dungeon[v10][v8 + 1];
				if ( *v48 == 3 )
					*v48 = 4;
			}
			if ( v12 == 22 )
			{
				v49 = &dungeon[v10][v8 + 1];
				if ( *v49 == 9 )
					*v49 = 15;
			}
			if ( v12 == 18 )
			{
				v50 = &dungeon[v10][v8 + 1];
				if ( *v50 == 30 )
					*v50 = 18;
			}
			if ( v12 == 24 && *(v11 - 40) == 30 )
				*(v11 - 40) = 19;
			if ( v12 == 21 )
			{
				v51 = &dungeon[v10][v8 + 1];
				if ( *v51 == 2 )
					*v51 = 15;
				v52 = &dungeon[v10][v8 + 1];
				if ( *v52 == 9 )
					*v52 = 10;
			}
			if ( v12 == 22 )
			{
				v53 = &dungeon[v10][v8 + 1];
				if ( *v53 == 30 )
					*v53 = 18;
			}
			if ( v12 == 21 )
			{
				v54 = &dungeon[v10][v8 + 1];
				if ( *v54 == 30 )
					*v54 = 18;
			}
			if ( v12 == 16 )
			{
				v55 = &dungeon[v10][v8 + 1];
				if ( *v55 == 2 )
					*v55 = 15;
			}
			if ( v12 == 13 )
			{
				v56 = &dungeon[v10][v8 + 1];
				if ( *v56 == 2 )
					*v56 = 15;
			}
			if ( v12 == 22 )
			{
				v57 = &dungeon[v10][v8 + 1];
				if ( *v57 == 2 )
					*v57 = 15;
			}
			if ( v12 == 21 )
			{
				v58 = &dungeon[v10 + 1][v8];
				if ( *v58 == 18 && dungeon[v10 + 2][v8] == 30 )
					*v58 = 24;
				v59 = &dungeon[v10 + 1][v8];
				if ( *v59 == 9 && dungeon[v10 + 1][v8 + 1] == 1 )
					*v59 = 16;
			}
			if ( v12 == 2 )
			{
				v60 = &dungeon[v10 + 1][v8];
				if ( *v60 == 27 && dungeon[v10 + 1][v8 + 1] == 2 )
					*v60 = 29;
			}
			if ( v12 == 23 )
			{
				v61 = &dungeon[v10][v8 + 1];
				if ( *v61 == 2 )
					*v61 = 15;
				v62 = &dungeon[v10][v8 + 1];
				if ( *v62 == 9 )
					*v62 = 15;
			}
			if ( v12 == 25 )
			{
				v63 = &dungeon[v10][v8 + 1];
				if ( *v63 == 2 )
					*v63 = 15;
			}
			if ( v12 == 22 )
			{
				v64 = &dungeon[v10 + 1][v8];
				if ( *v64 == 9 )
					*v64 = 11;
			}
			if ( v12 == 23 )
			{
				v65 = &dungeon[v10 + 1][v8];
				if ( *v65 == 9 )
					*v65 = 11;
			}
			if ( v12 == 15 )
			{
				v66 = &dungeon[v10 + 1][v8];
				if ( *v66 == 1 )
					*v66 = 16;
			}
			if ( v12 == 11 )
			{
				v67 = &dungeon[v10 + 1][v8];
				if ( *v67 == 15 )
					*v67 = 14;
			}
			if ( v12 == 23 )
			{
				v68 = &dungeon[v10 + 1][v8];
				if ( *v68 == 1 )
					*v68 = 16;
			}
			if ( v12 == 21 )
			{
				v69 = &dungeon[v10 + 1][v8];
				if ( *v69 == 27 )
					*v69 = 26;
				v70 = &dungeon[v10 + 1][v8];
				if ( *v70 == 18 )
					*v70 = 24;
			}
			if ( v12 == 26 )
			{
				v71 = &dungeon[v10 + 1][v8];
				if ( *v71 == 1 )
					*v71 = 16;
			}
			if ( v12 == 29 )
			{
				v72 = &dungeon[v10 + 1][v8];
				if ( *v72 == 1 )
					*v72 = 16;
				v73 = &dungeon[v10][v8 + 1];
				if ( *v73 == 2 )
					*v73 = 15;
			}
			if ( v12 == 1 && *(v11 - 1) == 15 )
				*(v11 - 1) = 10;
			if ( v12 == 18 )
			{
				v74 = &dungeon[v10][v8 + 1];
				if ( *v74 == 2 )
					*v74 = 15;
			}
			if ( v12 == 23 )
			{
				v75 = &dungeon[v10][v8 + 1];
				if ( *v75 == 30 )
					*v75 = 18;
			}
			if ( v12 == 18 )
			{
				v76 = &dungeon[v10][v8 + 1];
				if ( *v76 == 9 )
					*v76 = 10;
			}
			if ( v12 == 14 )
			{
				v77 = &dungeon[v10 + 1][v8];
				if ( *v77 == 30 && dungeon[v10 + 1][v8 + 1] == 30 )
					*v77 = 23;
			}
			if ( v12 == 2 )
			{
				v78 = &dungeon[v10 + 1][v8];
				if ( *v78 == 28 && dungeon[v10][v8 + 39] == 6 )
					*v78 = 23;
			}
			if ( v12 == 23 )
			{
				v79 = &dungeon[v10 + 1][v8];
				if ( *v79 == 18 && *(v11 - 1) == 6 )
					*v79 = 24;
			}
			if ( v12 == 14 )
			{
				v80 = &dungeon[v10 + 1][v8];
				if ( *v80 == 23 && dungeon[v10 + 2][v8] == 30 )
					*v80 = 28;
				v81 = &dungeon[v10 + 1][v8];
				if ( *v81 == 28 && dungeon[v10 + 2][v8] == 30 && dungeon[v10][v8 + 39] == 6 )
					*v81 = 23;
			}
			if ( v12 == 23 )
			{
				v82 = &dungeon[v10 + 1][v8];
				if ( *v82 == 30 )
					*v82 = 19;
			}
			if ( v12 == 29 )
			{
				v83 = &dungeon[v10 + 1][v8];
				if ( *v83 == 30 )
					*v83 = 19;
				v84 = &dungeon[v10][v8 + 1];
				if ( *v84 == 30 )
					*v84 = 18;
			}
			if ( v12 == 19 )
			{
				v85 = &dungeon[v10 + 1][v8];
				if ( *v85 == 30 )
					*v85 = 19;
			}
			if ( v12 == 21 )
			{
				v86 = &dungeon[v10 + 1][v8];
				if ( *v86 == 30 )
					*v86 = 19;
			}
			if ( v12 == 26 )
			{
				v87 = &dungeon[v10 + 1][v8];
				if ( *v87 == 30 )
					*v87 = 19;
			}
			if ( v12 == 16 )
			{
				v88 = &dungeon[v10][v8 + 1];
				if ( *v88 == 30 )
					*v88 = 18;
			}
			if ( v12 == 13 )
			{
				v89 = &dungeon[v10][v8 + 1];
				if ( *v89 == 9 )
					*v89 = 10;
			}
			if ( v12 == 25 )
			{
				v90 = &dungeon[v10][v8 + 1];
				if ( *v90 == 30 )
					*v90 = 18;
			}
			if ( v12 == 18 )
			{
				v91 = &dungeon[v10][v8 + 1];
				if ( *v91 == 2 )
					*v91 = 15;
			}
			if ( v12 == 11 )
			{
				v92 = &dungeon[v10 + 1][v8];
				if ( *v92 == 3 )
					*v92 = 5;
			}
			if ( v12 == 19 )
			{
				v93 = &dungeon[v10 + 1][v8];
				if ( *v93 == 9 )
					*v93 = 11;
				v94 = &dungeon[v10 + 1][v8];
				if ( *v94 == 1 )
					*v94 = 13;
				v95 = &dungeon[v10 + 1][v8];
				if ( *v95 == 13 && dungeon[v10][v8 + 39] == 6 )
					*v95 = 16;
			}
			v9 = v139++ + 1;
		}
		while ( v139 < 40 );
		++v8;
	}
	while ( v8 < 40 );
	v96 = 0;
	do
	{
		v97 = 0;
		do
		{
			v98 = v97;
			v99 = &dungeon[v97][v96];
			v100 = *v99;
			if ( *v99 == 21 )
			{
				v101 = &dungeon[v98][v96 + 1];
				if ( *v101 == 24 && dungeon[v98][v96 + 2] == 1 )
					*v101 = 17;
			}
			if ( v100 == 15
			  && dungeon[v98 + 1][v96 + 1] == 9
			  && dungeon[v98][v96 + 39] == 1
			  && dungeon[v98 + 2][v96] == 16 )
			{
				dungeon[v98 + 1][v96] = 29;
			}
			if ( v100 == 2 && *(v99 - 40) == 6 )
				*(v99 - 40) = 8;
			if ( v100 == 1 && *(v99 - 1) == 6 )
				*(v99 - 1) = 7;
			if ( v100 == 6 )
			{
				v102 = &dungeon[v98 + 1][v96];
				if ( *v102 == 15 && dungeon[v98 + 1][v96 + 1] == 4 )
					*v102 = 10;
			}
			if ( v100 == 1 )
			{
				v103 = &dungeon[v98][v96 + 1];
				if ( *v103 == 3 )
					*v103 = 4;
				v104 = &dungeon[v98][v96 + 1];
				if ( *v104 == 6 )
					*v104 = 4;
			}
			if ( v100 == 9 )
			{
				v105 = &dungeon[v98][v96 + 1];
				if ( *v105 == 3 )
					*v105 = 4;
			}
			if ( v100 == 10 )
			{
				v106 = &dungeon[v98][v96 + 1];
				if ( *v106 == 3 )
					*v106 = 4;
			}
			if ( v100 == 13 )
			{
				v107 = &dungeon[v98][v96 + 1];
				if ( *v107 == 3 )
					*v107 = 4;
			}
			if ( v100 == 1 )
			{
				v108 = &dungeon[v98][v96 + 1];
				if ( *v108 == 5 )
					*v108 = 12;
				v109 = &dungeon[v98][v96 + 1];
				if ( *v109 == 16 )
					*v109 = 13;
			}
			if ( v100 == 6 )
			{
				v110 = &dungeon[v98][v96 + 1];
				if ( *v110 == 13 )
					*v110 = 16;
			}
			if ( v100 == 25 )
			{
				v111 = &dungeon[v98][v96 + 1];
				if ( *v111 == 9 )
					*v111 = 10;
			}
			if ( v100 == 13 )
			{
				v112 = &dungeon[v98][v96 + 1];
				if ( *v112 == 5 )
					*v112 = 12;
			}
			if ( v100 == 28 && *(v99 - 1) == 6 )
			{
				v113 = &dungeon[v98 + 1][v96];
				if ( *v113 == 1 )
					*v113 = 23;
			}
			if ( v100 == 19 )
			{
				v114 = &dungeon[v98 + 1][v96];
				if ( *v114 == 10 )
					*v114 = 17;
			}
			if ( v100 == 21 )
			{
				v115 = &dungeon[v98 + 1][v96];
				if ( *v115 == 9 )
					*v115 = 11;
			}
			if ( v100 == 11 )
			{
				v116 = &dungeon[v98 + 1][v96];
				if ( *v116 == 3 )
					*v116 = 5;
			}
			if ( v100 == 10 )
			{
				v117 = &dungeon[v98 + 1][v96];
				if ( *v117 == 4 )
					*v117 = 12;
			}
			if ( v100 == 14 )
			{
				v118 = &dungeon[v98 + 1][v96];
				if ( *v118 == 4 )
					*v118 = 12;
			}
			if ( v100 == 27 )
			{
				v119 = &dungeon[v98 + 1][v96];
				if ( *v119 == 9 )
					*v119 = 11;
			}
			if ( v100 == 15 )
			{
				v120 = &dungeon[v98 + 1][v96];
				if ( *v120 == 4 )
					*v120 = 12;
			}
			if ( v100 == 21 )
			{
				v121 = &dungeon[v98 + 1][v96];
				if ( *v121 == 1 )
					*v121 = 16;
			}
			if ( v100 == 11 )
			{
				v122 = &dungeon[v98 + 1][v96];
				if ( *v122 == 4 )
					*v122 = 12;
			}
			if ( v100 == 2 )
			{
				v123 = &dungeon[v98 + 1][v96];
				if ( *v123 == 3 )
					*v123 = 5;
			}
			if ( v100 == 9 )
			{
				v124 = &dungeon[v98 + 1][v96];
				if ( *v124 == 3 )
					*v124 = 5;
			}
			if ( v100 == 14 )
			{
				v125 = &dungeon[v98 + 1][v96];
				if ( *v125 == 3 )
					*v125 = 5;
			}
			if ( v100 == 15 )
			{
				v126 = &dungeon[v98 + 1][v96];
				if ( *v126 == 3 )
					*v126 = 5;
			}
			if ( v100 == 2 )
			{
				v127 = &dungeon[v98 + 1][v96];
				if ( *v127 == 5 && dungeon[v98][v96 + 39] == 16 )
					*v127 = 12;
				v128 = &dungeon[v98 + 1][v96];
				if ( *v128 == 4 )
					*v128 = 12;
			}
			if ( v100 == 9 )
			{
				v129 = &dungeon[v98 + 1][v96];
				if ( *v129 == 4 )
					*v129 = 12;
			}
			if ( v100 == 1 && *(v99 - 1) == 8 )
				*(v99 - 1) = 9;
			if ( v100 == 28 )
			{
				v130 = &dungeon[v98 + 1][v96];
				if ( *v130 == 23 && dungeon[v98 + 1][v96 + 1] == 3 )
					*v130 = 16;
			}
			++v97;
		}
		while ( v97 < 40 );
		++v96;
	}
	while ( v96 < 40 );
	v131 = 0;
	do
	{
		v132 = &dungeon[0][v131 + 1];
		v133 = 40;
		do
		{
			v134 = *(v132 - 1);
			if ( v134 == 21 && v132[39] == 10 )
				v132[39] = 17;
			if ( v134 == 17 && v132[39] == 4 )
				v132[39] = 12;
			if ( v134 == 10 && v132[39] == 4 )
				v132[39] = 12;
			if ( v134 == 17 && *v132 == 5 )
				*v132 = 12;
			if ( v134 == 29 && *v132 == 9 )
				*v132 = 10;
			if ( v134 == 13 && *v132 == 5 )
				*v132 = 12;
			if ( v134 == 9 && *v132 == 16 )
				*v132 = 13;
			if ( v134 == 10 && *v132 == 16 )
				*v132 = 13;
			if ( v134 == 16 && *v132 == 3 )
				*v132 = 4;
			if ( v134 == 11 && *v132 == 5 )
				*v132 = 12;
			if ( v134 == 10 && v132[39] == 3 && v132[38] == 16 )
				v132[39] = 12;
			if ( v134 == 16 && *v132 == 5 )
				*v132 = 12;
			if ( v134 == 1 && *v132 == 6 )
				*v132 = 4;
			if ( v134 == 21 && v132[39] == 13 && *v132 == 10 )
				v132[40] = 12;
			if ( v134 == 15 && v132[39] == 10 )
				v132[39] = 17;
			if ( v134 == 22 && *v132 == 11 )
				*v132 = 17;
			if ( v134 == 15 && v132[39] == 28 && v132[79] == 16 )
				v132[39] = 23;
			if ( v134 == 28 && v132[39] == 23 && v132[40] == 1 && v132[79] == 6 )
				v132[39] = 16;
			v132 += 40;
			--v133;
		}
		while ( v133 );
		++v131;
	}
	while ( v131 < 40 );
	v135 = 0;
	do
	{
		v136 = (char *)dungeon + v135;
		v137 = 40;
		do
		{
			v138 = *v136;
			if ( *v136 == 15 && v136[40] == 28 && v136[80] == 16 )
				v136[40] = 23;
			if ( v138 == 21 && v136[39] == 21 && v136[41] == 13 && v136[80] == 2 )
				v136[40] = 17;
			if ( v138 == 19 && v136[40] == 15 && v136[41] == 12 )
				v136[40] = 17;
			v136 += 40;
			--v137;
		}
		while ( v137 );
		++v135;
	}
	while ( v135 < 40 );
}

void __cdecl DRLG_L4Subs()
{
	signed int v0; // edi
	signed int v1; // esi
	signed int v2; // ebp
	unsigned char v3; // bl
	int v4; // eax
	signed int v5; // ecx
	signed int v6; // edi
	signed int v7; // esi
	signed int v8; // ebx

	v0 = 0;
	do
	{
		v1 = v0;
		v2 = 40;
		do
		{
			if ( !random(0, 3) )
			{
				v3 = L4BTYPES[(unsigned char)dungeon[0][v1]];
				if ( v3 )
				{
					if ( !dflags[0][v1] )
					{
						v4 = random(0, 16);
						v5 = -1;
						while ( v4 >= 0 )
						{
							if ( ++v5 == 140 )
								v5 = 0;
							if ( v3 == L4BTYPES[v5] )
								--v4;
						}
						dungeon[0][v1] = v5;
					}
				}
			}
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 40 );
	v6 = 0;
	do
	{
		v7 = v6;
		v8 = 40;
		do
		{
			if ( !random(0, 10) && L4BTYPES[(unsigned char)dungeon[0][v7]] == 6 && !dflags[0][v7] )
				dungeon[0][v7] = random(0, 3) + 95;
			v7 += 40;
			--v8;
		}
		while ( v8 );
		++v6;
	}
	while ( v6 < 40 );
}

void __cdecl L4makeDungeon()
{
	signed int v0; // ebx
	signed int v1; // esi
	char *v2; // edx
	char v3; // cl
	int v4; // eax
	int v5; // eax
	int v6; // ebx
	char *v7; // esi
	signed int v8; // edx
	char v9; // cl
	int v10; // eax
	int v11; // eax
	signed int v12; // ebx
	signed int v13; // esi
	char *v14; // edx
	char v15; // cl
	int v16; // eax
	int v17; // eax
	int v18; // ebx
	char *v19; // esi
	signed int v20; // edx
	char v21; // cl
	int v22; // eax
	int v23; // eax
	signed int v24; // [esp+Ch] [ebp-8h]
	char *v25; // [esp+10h] [ebp-4h]
	char *v26; // [esp+10h] [ebp-4h]

	v0 = 0;
	do
	{
		v1 = 0;
		v2 = (char *)dung + v0;
		do
		{
			v3 = *v2;
			v2 += 20;
			v4 = 160 * v1++;
			v5 = v4 + 2 * v0;
			L4dungeon[0][v5] = v3;
			L4dungeon[0][v5 + 1] = v3;
			L4dungeon[1][v5] = v3;
			L4dungeon[1][v5 + 1] = v3;
		}
		while ( v1 < 20 );
		++v0;
	}
	while ( v0 < 20 );
	v6 = 0;
	v25 = &dung[0][19];
	v24 = 20;
	do
	{
		v7 = v25;
		v8 = 0;
		do
		{
			v9 = *v7;
			v7 += 20;
			v10 = 160 * v8++;
			v11 = v10 + 2 * v6;
			L4dungeon[0][v11 + 40] = v9;
			L4dungeon[0][v11 + 41] = v9;
			L4dungeon[1][v11 + 40] = v9;
			L4dungeon[1][v11 + 41] = v9;
		}
		while ( v8 < 20 );
		++v6;
		--v25;
		--v24;
	}
	while ( v24 );
	v12 = 0;
	do
	{
		v13 = 0;
		v14 = &dung[19][v12];
		do
		{
			v15 = *v14;
			v14 -= 20;
			v16 = 160 * v13++;
			v17 = v16 + 2 * v12;
			L4dungeon[40][v17] = v15;
			L4dungeon[40][v17 + 1] = v15;
			L4dungeon[41][v17] = v15;
			L4dungeon[41][v17 + 1] = v15;
		}
		while ( v13 < 20 );
		++v12;
	}
	while ( v12 < 20 );
	v18 = 0;
	v26 = &dung[19][19];
	do
	{
		v19 = v26;
		v20 = 0;
		do
		{
			v21 = *v19;
			v19 -= 20;
			v22 = 160 * v20++;
			v23 = v22 + 2 * v18;
			L4dungeon[40][v23 + 40] = v21;
			L4dungeon[40][v23 + 41] = v21;
			L4dungeon[41][v23 + 40] = v21;
			L4dungeon[41][v23 + 41] = v21;
		}
		while ( v20 < 20 );
		++v18;
		--v26;
	}
	while ( (signed int)v26 > (signed int)&dung[18][19] );
}

void __cdecl uShape()
{
	int v0; // ecx
	signed int v1; // esi
	signed int v2; // eax
	char v3; // dl
	int v4; // eax
	signed int v5; // esi
	int v6; // ecx
	int v7; // ecx
	int *v8; // esi
	signed int v9; // eax
	char v10; // dl
	int v11; // eax
	signed int v12; // esi
	char *v13; // edx

	v0 = 19;
	do
	{
		v1 = 19;
		do
		{
			v2 = v1;
			v3 = dung[v1][v0];
			if ( v3 == 1 || (hallok[v0] = 0, v3 == 1) )
			{
				hallok[v0] = dung[v2][v0 + 1] == 1 && !dung[v2 + 1][v0 + 1];
				v1 = 0;
			}
			--v1;
		}
		while ( v1 >= 0 );
		--v0;
	}
	while ( v0 >= 0 );
	v4 = random(0, 19) + 1;
	do
	{
		if ( hallok[v4] )
		{
			v5 = 19;
			do
			{
				v6 = v4 + 20 * v5;
				if ( dung[0][v6] == 1 )
				{
					v5 = -1;
					v4 = 0;
				}
				else
				{
					dung[0][v6] = 1;
					dung[0][v6 + 1] = 1;
				}
				--v5;
			}
			while ( v5 >= 0 );
		}
		else if ( ++v4 == 20 )
		{
			v4 = 1;
		}
	}
	while ( v4 );
	v7 = 380;
	v8 = &hallok[19];
	do
	{
		v9 = 19;
		do
		{
			v10 = dung[0][v7 + v9];
			if ( v10 == 1 || (*v8 = 0, v10 == 1) )
			{
				*v8 = dung[1][v7 + v9] == 1 && !dung[1][v7 + 1 + v9];
				v9 = 0;
			}
			--v9;
		}
		while ( v9 >= 0 );
		--v8;
		v7 -= 20;
	}
	while ( (signed int)v8 >= (signed int)hallok );
	v11 = random(0, 19) + 1;
	do
	{
		if ( hallok[v11] )
		{
			v12 = 19;
			do
			{
				v13 = &dung[v11][v12];
				if ( *v13 == 1 )
				{
					v12 = -1;
					v11 = 0;
				}
				else
				{
					*v13 = 1;
					dung[v11 + 1][v12] = 1;
				}
				--v12;
			}
			while ( v12 >= 0 );
		}
		else if ( ++v11 == 20 )
		{
			v11 = 1;
		}
	}
	while ( v11 );
}

int __cdecl GetArea()
{
	int result; // eax
	signed int v1; // edx
	_BYTE *v2; // ecx
	signed int v3; // esi

	result = 0;
	v1 = 0;
	do
	{
		v2 = (unsigned char *)dung + v1;
		v3 = 20;
		do
		{
			if ( *v2 == 1 )
				++result;
			v2 += 20;
			--v3;
		}
		while ( v3 );
		++v1;
	}
	while ( v1 < 20 );
	return result;
}

void __cdecl L4firstRoom()
{
	int v0; // esi
	int v1; // edi
	int v2; // ebx
	int v3; // eax
	int v4; // eax
	int v5; // ebp
	//int v6; // eax
	int v7; // eax
	int v8; // eax
	signed int v9; // [esp-4h] [ebp-18h]

	if ( currlevel == 16 )
	{
		v9 = 14;
	}
	else
	{
		if ( (currlevel != quests[QTYPE_WARLRD]._qlevel || !quests[QTYPE_WARLRD]._qactive)
		  && (currlevel != quests[QTYPE_VB]._qlevel || gbMaxPlayers == 1) )
		{
			v0 = random(0, 5) + 2;
			v1 = random(0, 5) + 2;
			goto LABEL_10;
		}
		v9 = 11;
	}
	v1 = v9;
	v0 = v9;
LABEL_10:
	v2 = 20 - v0;
	v3 = ((20 - v0) >> 1) + random(0, 20 - ((20 - v0) >> 1) - v0);
	if ( v3 + v0 <= 19 )
		v2 = v3;
	v4 = ((20 - v1) >> 1) + random(0, 20 - ((20 - v1) >> 1) - v1);
	v5 = 20 - v1;
	if ( v4 + v1 <= 19 )
		v5 = v4;
	if ( currlevel == 16 )
	{
		l4holdx = v2;
		l4holdy = v5;
	}
	//_LOBYTE(v6) = QuestStatus(QTYPE_WARLRD);
	if ( QuestStatus(QTYPE_WARLRD) || currlevel == quests[QTYPE_VB]._qlevel && gbMaxPlayers != 1 )
	{
		SP4x1 = v2 + 1;
		SP4y1 = v5 + 1;
		v7 = v0 + v2 + 1;
		SP4y2 = v1 + v5 + 1;
	}
	else
	{
		v7 = 0;
		SP4x1 = 0;
		SP4y1 = 0;
		SP4y2 = 0;
	}
	SP4x2 = v7;
	L4drawRoom(v2, v5, v0, v1);
	v8 = random(0, 2);
	L4roomGen(v2, v5, v0, v1, v8);
}
// 528A34: using guessed type int l4holdx;
// 528A38: using guessed type int l4holdy;
// 528A40: using guessed type int SP4x2;
// 528A48: using guessed type int SP4y2;
// 679660: using guessed type char gbMaxPlayers;

void __fastcall L4drawRoom(int x, int y, int width, int height)
{
	int i; // esi
	int v5; // edi
	char *v6; // eax

	for ( i = 0; i < height; ++i )
	{
		if ( width > 0 )
		{
			v5 = width;
			v6 = &dung[x][i + y];
			do
			{
				*v6 = 1;
				v6 += 20;
				--v5;
			}
			while ( v5 );
		}
	}
}

void __fastcall L4roomGen(int x, int y, int w, int h, int dir)
{
	int v5; // eax
	int v6; // ecx
	int v7; // eax
	int v8; // ecx
	int v9; // eax
	int v11; // esi
	int v12; // edi
	int v13; // ebx
	int v14; // eax
	int v15; // eax
	int v17; // esi
	int v18; // edi
	int v19; // ebx
	int v20; // eax
	int ya; // [esp+Ch] [ebp-10h]
	int yb; // [esp+Ch] [ebp-10h]
	int v23; // [esp+10h] [ebp-Ch]
	int v24; // [esp+10h] [ebp-Ch]
	int xa; // [esp+14h] [ebp-8h]
	int xb; // [esp+14h] [ebp-8h]
	int v27; // [esp+18h] [ebp-4h]
	int wa; // [esp+24h] [ebp+8h]
	int ha; // [esp+28h] [ebp+Ch]
	int hb; // [esp+28h] [ebp+Ch]
	int hc; // [esp+28h] [ebp+Ch]
	int dira; // [esp+2Ch] [ebp+10h]
	int dirb; // [esp+2Ch] [ebp+10h]

	v27 = y;
	xa = x;
	while ( 1 )
	{
		while ( 1 )
		{
			v5 = random(0, 4);
			v6 = 0;
			_LOBYTE(v6) = dir == 1 ? v5 != 0 : v5 == 0;
			v7 = v6;
			v8 = 0;
			if ( !v7 )
				break;
			if ( v7 != 1 )
				return;
			dira = 0;
			wa = w / 2;
			do
			{
				v9 = random(0, 5);
				v11 = (v9 + 2) & 0xFFFFFFFE;
				v12 = (random(0, 5) + 2) & 0xFFFFFFFE;
				v13 = xa + wa - v11 / 2;
				ya = v27 - v12;
				v14 = L4checkRoom(v13 - 1, v27 - v12 - 1, v11 + 2, v12 + 1);
				++dira;
				v23 = v14;
			}
			while ( !v14 && dira < 20 );
			if ( v14 == 1 )
				L4drawRoom(v13, ya, v11, v12);
			xb = v27 + h;
			ha = L4checkRoom(v13 - 1, v27 + h, v11 + 2, v12 + 1);
			if ( ha == 1 )
				L4drawRoom(v13, xb, v11, v12);
			if ( v23 == 1 )
				L4roomGen(v13, ya, v11, v12, 0);
			if ( ha != 1 )
				return;
			dir = 0;
			h = v12;
			w = v11;
			v27 = xb;
			xa = v13;
		}
		dirb = 0;
		hb = h / 2;
		do
		{
			v15 = random(0, 5);
			v17 = (v15 + 2) & 0xFFFFFFFE;
			v18 = (random(0, 5) + 2) & 0xFFFFFFFE;
			v19 = v27 + hb - v18 / 2;
			yb = xa - v17;
			v20 = L4checkRoom(xa - v17 - 1, v19 - 1, v18 + 2, v17 + 1);
			++dirb;
			v24 = v20;
		}
		while ( !v20 && dirb < 20 );
		if ( v20 == 1 )
			L4drawRoom(yb, v19, v17, v18);
		xa += w;
		hc = L4checkRoom(xa, v19 - 1, v17 + 1, v18 + 2);
		if ( hc == 1 )
			L4drawRoom(xa, v19, v17, v18);
		if ( v24 == 1 )
			L4roomGen(yb, v19, v17, v18, 1);
		if ( hc != 1 )
			break;
		dir = 1;
		h = v18;
		w = v17;
		v27 = v19;
	}
}

bool __fastcall L4checkRoom(int x, int y, int width, int height)
{
	int v4; // esi
	int v5; // ebx
	char *v6; // edi
	int v8; // [esp+Ch] [ebp-4h]

	v4 = 0;
	if ( x > 0 && y > 0 )
	{
		if ( height <= 0 )
			return 1;
		while ( 1 )
		{
			v8 = 0;
			if ( width > 0 )
				break;
LABEL_12:
			if ( ++v4 >= height )
				return 1;
		}
		v5 = x;
		v6 = &dung[x][v4 + y];
		while ( v5 >= 0 && v5 < 20 && v4 + y >= 0 && v4 + y < 20 && !*v6 )
		{
			++v8;
			v6 += 20;
			++v5;
			if ( v8 >= width )
				goto LABEL_12;
		}
	}
	return 0;
}

bool __fastcall DRLG_L4PlaceMiniSet(const unsigned char *miniset, int tmin, int tmax, int cx, int cy, int setview, int ldir)
{
	int v7; // ebx
	int v8; // esi
	int v9; // edi
	int v10; // edx
	int v11; // esi
	int v12; // ebx
	int v13; // edi
	signed int i; // eax
	int v15; // ecx
	unsigned char v16; // dl
	int v17; // eax
	int j; // ecx
	int v19; // edi
	int v20; // edx
	char v21; // bl
	bool result; // al
	const unsigned char *v23; // [esp+Ch] [ebp-28h]
	int v24; // [esp+10h] [ebp-24h]
	int v25; // [esp+14h] [ebp-20h]
	int v26; // [esp+18h] [ebp-1Ch]
	signed int v27; // [esp+1Ch] [ebp-18h]
	int v28; // [esp+20h] [ebp-14h]
	int v29; // [esp+24h] [ebp-10h]
	int v30; // [esp+28h] [ebp-Ch]
	int max; // [esp+2Ch] [ebp-8h]
	//int v32; // [esp+30h] [ebp-4h]
	int v33; // [esp+30h] [ebp-4h]
	int tmaxa; // [esp+3Ch] [ebp+8h]

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
		v12 = 0; /* v32 */
	}
	else
	{
		max = 40 - v9;
		v30 = 40 - v7;
		do
		{
			v11 = random(0, max);
			v27 = 0;
			v12 = random(0, v30);
			v33 = v12;
			do
			{
				if ( v27 >= 200 )
					return 0;
				tmaxa = 1;
				if ( v11 >= SP4x1 && v11 <= SP4x2 && v12 >= SP4y1 && v12 <= SP4y2 )
					tmaxa = 0;
				if ( cx != -1 && v11 >= cx - v28 && v11 <= cx + 12 )
				{
					v11 = random(0, max);
					tmaxa = 0;
					v33 = random(0, v30);
					v12 = v33;
				}
				if ( cy != -1 && v12 >= cy - v29 && v12 <= cy + 12 )
				{
					v11 = random(0, max);
					tmaxa = 0;
					v33 = random(0, v30);
					v12 = v33;
				}
				v13 = 0;
				for ( i = 2; v13 < v29; ++v13 )
				{
					if ( tmaxa != 1 )
						break;
					v26 = 0;
					if ( v28 > 0 )
					{
						v15 = v12 + v13 + 40 * v11;
						do
						{
							if ( tmaxa != 1 )
								break;
							v16 = v23[i];
							if ( v16 && dungeon[0][v15] != v16 )
								tmaxa = 0;
							if ( dflags[0][v15] )
								tmaxa = 0;
							++i;
							++v26;
							v15 += 40;
						}
						while ( v26 < v28 );
					}
				}
				if ( !tmaxa && ++v11 == max )
				{
					v11 = 0;
					v33 = ++v12;
					if ( v12 == v30 )
					{
						v33 = 0;
						v12 = 0;
					}
				}
				++v27;
			}
			while ( !tmaxa );
			if ( v27 >= 200 )
				return 0;
			v17 = 0;
			for ( j = v28 * v29 + 2; v17 < v29; ++v17 )
			{
				v19 = v28;
				if ( v28 > 0 )
				{
					v20 = v12 + v17 + 40 * v11;
					do
					{
						v21 = v23[j];
						if ( v21 )
						{
							dflags[0][v20] |= DFLAG_POPULATED;
							dungeon[0][v20] = v21;
						}
						++j;
						v20 += 40;
						--v19;
					}
					while ( v19 );
					v12 = v33;
				}
			}
			++v25;
		}
		while ( v25 < v24 );
	}
	if ( currlevel == 15 )
	{
		quests[QTYPE_VB]._qtx = v11 + 1;
		quests[QTYPE_VB]._qty = v12 + 1;
	}
	result = 1;
	if ( setview == 1 )
	{
		ViewX = 2 * v11 + 21;
		ViewY = 2 * v12 + 22;
	}
	if ( !ldir )
	{
		LvlViewX = 2 * v11 + 21;
		LvlViewY = 2 * v12 + 22;
	}
	return result;
}
// 528A40: using guessed type int SP4x2;
// 528A48: using guessed type int SP4y2;
// 5CF320: using guessed type int LvlViewY;
// 5CF324: using guessed type int LvlViewX;

void __cdecl DRLG_L4FloodTVal()
{
	int v0; // ebx
	int v1; // esi
	char *v2; // edi
	_BYTE *v3; // [esp+Ch] [ebp-Ch]
	signed int x; // [esp+10h] [ebp-8h]
	signed int i; // [esp+14h] [ebp-4h]

	v0 = 16;
	v1 = 0;
	do
	{
		i = 0;
		x = 16;
		v2 = &dung_map[16][v0];
		v3 = (unsigned char *)dungeon + v1;
		do
		{
			if ( *v3 == 6 && !*v2 )
			{
				DRLG_L4FTVR(i, v1, x, v0, 0);
				++TransVal;
			}
			x += 2;
			v3 += 40;
			v2 += 224;
			++i;
		}
		while ( i < 40 );
		v0 += 2;
		++v1;
	}
	while ( v1 < 40 );
}
// 5A5590: using guessed type char TransVal;

void __fastcall DRLG_L4FTVR(int i, int j, int x, int y, int d)
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
	int k; // [esp+10h] [ebp-10h]
	int v21; // [esp+14h] [ebp-Ch]
	int ja; // [esp+18h] [ebp-8h]
	int ia; // [esp+1Ch] [ebp-4h]
	int ya; // [esp+2Ch] [ebp+Ch]

	v5 = x;
	v6 = y;
	v7 = j;
	v8 = i;
	v9 = 112 * x + y;
	ja = v7;
	v21 = v8;
	if ( !dung_map[0][v9] )
	{
		v19 = x;
		ia = v8 - 1;
		v10 = x - 2;
		v11 = 40 * v8;
		ya = v7 - 1;
		v12 = v6 - 2;
		for ( k = 40 * v8; dungeon[0][v11 + ja] == 6; v11 = k )
		{
			v13 = TransVal;
			dung_map[0][v9] = TransVal;
			dung_map[1][v9] = v13;
			dung_map[0][v9 + 1] = v13;
			dung_map[1][v9 + 1] = v13;
			DRLG_L4FTVR(ia + 2, ja, v10 + 4, v6, 1);
			DRLG_L4FTVR(ia, ja, v10, v6, 2);
			DRLG_L4FTVR(v21, ya + 2, x, v12 + 4, 3);
			DRLG_L4FTVR(v21, ya, x, v12, 4);
			DRLG_L4FTVR(ia, ya, v10, v12, 5);
			DRLG_L4FTVR(ia + 2, ya, v10 + 4, v12, 6);
			DRLG_L4FTVR(ia, ya + 2, v10, v12 + 4, 7);
			v19 += 2;
			k += 40;
			d = 8;
			x += 2;
			v6 += 2;
			v12 += 2;
			v10 += 2;
			++ja;
			++ya;
			++v21;
			++ia;
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

void __cdecl DRLG_L4TransFix()
{
	signed int v0; // ebx
	char *v1; // esi
	char *v2; // edi
	char v3; // al
	signed int v4; // [esp+Ch] [ebp-8h]
	char *v5; // [esp+10h] [ebp-4h]

	v0 = 0;
	v5 = &dung_map[16][16];
	do
	{
		v1 = v5;
		v2 = (char *)dungeon + v0;
		v4 = 40;
		do
		{
			if ( IsDURWall(*v2) && *(v2 - 1) == 18 )
			{
				v1[112] = *v1;
				v1[113] = *v1;
			}
			if ( IsDLLWall(*v2) && v2[40] == 19 )
			{
				v1[1] = *v1;
				v1[113] = *v1;
			}
			v3 = *v2;
			if ( *v2 == 18 )
			{
				v1[112] = *v1;
				v1[113] = *v1;
			}
			if ( v3 == 19 )
			{
				v1[1] = *v1;
				v1[113] = *v1;
			}
			if ( v3 == 24 )
			{
				v1[112] = *v1;
				v1[1] = *v1;
				v1[113] = *v1;
			}
			if ( v3 == 57 )
			{
				*(v1 - 112) = v1[1];
				*v1 = v1[1];
			}
			if ( v3 == 53 )
			{
				*(v1 - 1) = v1[112];
				*v1 = v1[112];
			}
			v1 += 224;
			v2 += 40;
			--v4;
		}
		while ( v4 );
		v5 += 2;
		++v0;
	}
	while ( v0 < 40 );
}

void __cdecl DRLG_L4Corners()
{
	signed int v0; // edx
	char *v1; // ecx
	signed int v2; // esi
	char v3; // al

	v0 = 1;
	do
	{
		v1 = &dungeon[1][v0];
		v2 = 38;
		do
		{
			v3 = *v1;
			if ( (unsigned char)*v1 >= 0x12u
			  && (unsigned char)v3 <= 0x1Eu
			  && ((unsigned char)v1[40] < 0x12u || (unsigned char)v1[1] < 0x12u) )
			{
				*v1 = v3 + 98;
			}
			v1 += 40;
			--v2;
		}
		while ( v2 );
		++v0;
	}
	while ( v0 < 39 );
}

void __cdecl DRLG_L4Pass3()
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

	v0 = *((unsigned short *)pMegaTiles + 116) + 1;
	v19 = *((unsigned short *)pMegaTiles + 116) + 1;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 117);
	v17 = ++v0;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 118);
	v15 = ++v0;
	_LOWORD(v0) = *((_WORD *)pMegaTiles + 119);
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
