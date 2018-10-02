//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int trigflag[MAXTRIGGERS];
TriggerStruct trigs[MAXTRIGGERS];
int TWarpFrom; // weak
#endif

int TownDownList[11] = { 716, 715, 719, 720, 721, 723, 724, 725, 726, 727, -1 };
int TownWarp1List[13] =
{
  1171,
  1172,
  1173,
  1174,
  1175,
  1176,
  1177,
  1178,
  1179,
  1181,
  1183,
  1185,
  -1
};
int L1UpList[12] = { 127, 129, 130, 131, 132, 133, 135, 137, 138, 139, 140, -1 };
int L1DownList[10] = { 106, 107, 108, 109, 110, 112, 114, 115, 118, -1 };
int L2UpList[3] = { 266, 267, -1 };
int L2DownList[5] = { 269, 270, 271, 272, -1 };
int L2TWarpUpList[3] = { 558, 559, -1 };
int L3UpList[15] =
{
  170,
  171,
  172,
  173,
  174,
  175,
  176,
  177,
  178,
  179,
  180,
  181,
  182,
  183,
  -1
};
int L3DownList[9] = { 162, 163, 164, 165, 166, 167, 168, 169, -1 };
int L3TWarpUpList[14] = { 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, -1 };
int L4UpList[4] = { 82, 83, 90, -1 };
int L4DownList[6] = { 120, 130, 131, 132, 133, -1 };
int L4TWarpUpList[4] = { 421, 422, 429, -1 };
int L4PentaList[33] =
{
  353,
  354,
  355,
  356,
  357,
  358,
  359,
  360,
  361,
  362,
  363,
  364,
  365,
  366,
  367,
  368,
  369,
  370,
  371,
  372,
  373,
  374,
  375,
  376,
  377,
  378,
  379,
  380,
  381,
  382,
  383,
  384,
  -1
};

void __cdecl InitNoTriggers()
{
	trigflag[4] = 0;
	trigflag[3] = 0;
}

void __cdecl InitTownTriggers()
{
	char v0; // bl
	int v1; // eax
	int v2; // eax

	trigs[0]._tx = 25;
	trigs[0]._ty = 29;
	trigs[0]._tmsg = WM_DIABNEXTLVL;
	trigflag[4] = 1;
	if ( gbMaxPlayers == 4 )
	{
		trigs[1]._tx = 49;
		trigflag[0] = 1;
		trigflag[1] = 1;
		trigflag[2] = 1;
		trigs[1]._ty = 21;
		trigs[1]._tmsg = WM_DIABTOWNWARP;
		trigs[1]._tlvl = 5;
		trigs[2]._tx = 17;
		trigs[2]._ty = 69;
		trigs[2]._tmsg = WM_DIABTOWNWARP;
		trigs[2]._tlvl = 9;
		trigs[3]._tx = 41;
		trigs[3]._ty = 80;
		trigs[3]._tmsg = WM_DIABTOWNWARP;
		trigs[3]._tlvl = 13;
		trigflag[4] = 4;
	}
	else
	{
		trigflag[0] = 0;
		trigflag[1] = 0;
		trigflag[2] = 0;
		v0 = plr[myplr].pTownWarps;
		if ( v0 & 1 )
		{
			trigs[1]._tx = 49;
			trigs[1]._ty = 21;
			trigs[1]._tmsg = WM_DIABTOWNWARP;
			trigs[1]._tlvl = 5;
			trigflag[4] = 2;
			trigflag[0] = 1;
		}
		if ( v0 & 2 )
		{
			trigflag[1] = 1;
			v1 = trigflag[4]++;
			trigs[v1]._tx = 17;
			trigs[v1]._ty = 69;
			trigs[v1]._tmsg = WM_DIABTOWNWARP;
			trigs[v1]._tlvl = 9;
		}
		if ( v0 & 4 )
		{
			trigflag[2] = 1;
			v2 = trigflag[4]++;
			trigs[v2]._tx = 41;
			trigs[v2]._ty = 80;
			trigs[v2]._tmsg = WM_DIABTOWNWARP;
			trigs[v2]._tlvl = 13;
		}
	}
	trigflag[3] = 0;
}
// 679660: using guessed type char gbMaxPlayers;

void __cdecl InitL1Triggers()
{
	int v0; // edi
	signed int v1; // esi
	int *v2; // edx
	int *v3; // ecx
	TriggerStruct *v4; // eax
	int (*v5)[112]; // [esp+Ch] [ebp-8h]
	int (*v6)[112]; // [esp+10h] [ebp-4h]

	v0 = 0;
	trigflag[4] = 0;
	v5 = dPiece;
	do
	{
		v1 = 0;
		v6 = v5;
		v2 = &trigs[trigflag[4]]._tmsg;
		v3 = &trigs[trigflag[4]]._ty;
		v4 = &trigs[trigflag[4]];
		do
		{
			if ( (*v6)[0] == 129 )
			{
				++trigflag[4];
				v4->_tx = v1;
				*v3 = v0;
				*v2 = WM_DIABPREVLVL;
				++v4;
				v3 += 4;
				v2 += 4;
			}
			if ( (*v6)[0] == 115 )
			{
				++trigflag[4];
				v4->_tx = v1;
				*v3 = v0;
				*v2 = WM_DIABNEXTLVL;
				++v4;
				v3 += 4;
				v2 += 4;
			}
			++v6;
			++v1;
		}
		while ( v1 < 112 );
		v5 = (int (*)[112])((char *)v5 + 4);
		++v0;
	}
	while ( (signed int)v5 < (signed int)dPiece[1] );
	trigflag[3] = 0;
}

void __cdecl InitL2Triggers()
{
	signed int v0; // edi
	int *v1; // esi
	int *v2; // edx
	TriggerStruct *v3; // ecx
	int *v4; // eax
	int (*v5)[112]; // [esp+Ch] [ebp-10h]
	int (*v6)[112]; // [esp+10h] [ebp-Ch]
	int v7; // [esp+14h] [ebp-8h]
	int *v8; // [esp+18h] [ebp-4h]

	trigflag[4] = 0;
	v7 = 0;
	v5 = dPiece;
	do
	{
		v0 = 0;
		v1 = &trigs[trigflag[4]]._tmsg;
		v2 = &trigs[trigflag[4]]._ty;
		v3 = &trigs[trigflag[4]];
		v8 = &trigs[trigflag[4]]._tlvl;
		v6 = v5;
		do
		{
			if ( (*v6)[0] == 267 && (v0 != quests[QTYPE_BONE]._qtx || v7 != quests[QTYPE_BONE]._qty) )
			{
				++trigflag[4];
				v8 += 4;
				v3->_tx = v0;
				*v2 = v7;
				*v1 = WM_DIABPREVLVL;
				++v3;
				v2 += 4;
				v1 += 4;
			}
			if ( (*v6)[0] == 559 )
			{
				v3->_tx = v0;
				*v2 = v7;
				v4 = v8;
				v8 += 4;
				*v1 = WM_DIABTWARPUP;
				*v4 = 0;
				++trigflag[4];
				++v3;
				v2 += 4;
				v1 += 4;
			}
			if ( (*v6)[0] == 271 )
			{
				++trigflag[4];
				v8 += 4;
				v3->_tx = v0;
				*v2 = v7;
				*v1 = WM_DIABNEXTLVL;
				++v3;
				v2 += 4;
				v1 += 4;
			}
			++v6;
			++v0;
		}
		while ( v0 < 112 );
		v5 = (int (*)[112])((char *)v5 + 4);
		++v7;
	}
	while ( (signed int)v5 < (signed int)dPiece[1] );
	trigflag[3] = 0;
}

void __cdecl InitL3Triggers()
{
	int v0; // edi
	signed int v1; // esi
	int *v2; // edx
	int *v3; // ecx
	TriggerStruct *v4; // eax
	int (*v5)[112]; // [esp+Ch] [ebp-8h]
	int (*v6)[112]; // [esp+10h] [ebp-4h]

	v0 = 0;
	trigflag[4] = 0;
	v5 = dPiece;
	do
	{
		v1 = 0;
		v6 = v5;
		v2 = &trigs[trigflag[4]]._tmsg;
		v3 = &trigs[trigflag[4]]._ty;
		v4 = &trigs[trigflag[4]];
		do
		{
			if ( (*v6)[0] == 171 )
			{
				++trigflag[4];
				v4->_tx = v1;
				*v3 = v0;
				*v2 = WM_DIABPREVLVL;
				++v4;
				v3 += 4;
				v2 += 4;
			}
			if ( (*v6)[0] == 168 )
			{
				++trigflag[4];
				v4->_tx = v1;
				*v3 = v0;
				*v2 = WM_DIABNEXTLVL;
				++v4;
				v3 += 4;
				v2 += 4;
			}
			if ( (*v6)[0] == 549 )
			{
				++trigflag[4];
				v4->_tx = v1;
				*v3 = v0;
				*v2 = WM_DIABTWARPUP;
				++v4;
				v3 += 4;
				v2 += 4;
			}
			++v6;
			++v1;
		}
		while ( v1 < 112 );
		v5 = (int (*)[112])((char *)v5 + 4);
		++v0;
	}
	while ( (signed int)v5 < (signed int)dPiece[1] );
	trigflag[3] = 0;
}

void __cdecl InitL4Triggers()
{
	signed int v0; // edi
	int *v1; // esi
	int *v2; // edx
	TriggerStruct *v3; // ecx
	int *v4; // eax
	int v5; // edx
	int (*v6)[112]; // edi
	signed int v7; // ecx
	int *v8; // eax
	int (*v9)[112]; // [esp+Ch] [ebp-Ch]
	int (*v10)[112]; // [esp+Ch] [ebp-Ch]
	int v11; // [esp+10h] [ebp-8h]
	int (*v12)[112]; // [esp+14h] [ebp-4h]

	trigflag[4] = 0;
	v11 = 0;
	v9 = dPiece;
	do
	{
		v0 = 0;
		v12 = v9;
		v1 = &trigs[trigflag[4]]._tmsg;
		v2 = &trigs[trigflag[4]]._ty;
		v3 = &trigs[trigflag[4]];
		v4 = &trigs[trigflag[4]]._tlvl;
		do
		{
			if ( (*v12)[0] == 83 )
			{
				++trigflag[4];
				v3->_tx = v0;
				*v2 = v11;
				*v1 = WM_DIABPREVLVL;
				v4 += 4;
				++v3;
				v2 += 4;
				v1 += 4;
			}
			if ( (*v12)[0] == 422 )
			{
				v3->_tx = v0;
				*v2 = v11;
				*v1 = WM_DIABTWARPUP;
				*v4 = 0;
				++trigflag[4];
				v4 += 4;
				++v3;
				v2 += 4;
				v1 += 4;
			}
			if ( (*v12)[0] == 120 )
			{
				++trigflag[4];
				v3->_tx = v0;
				*v2 = v11;
				*v1 = WM_DIABNEXTLVL;
				v4 += 4;
				++v3;
				v2 += 4;
				v1 += 4;
			}
			++v12;
			++v0;
		}
		while ( v0 < 112 );
		v9 = (int (*)[112])((char *)v9 + 4);
		++v11;
	}
	while ( (signed int)v9 < (signed int)dPiece[1] );
	v5 = 0;
	v10 = dPiece;
	do
	{
		v6 = v10;
		v7 = 0;
		v8 = &trigs[trigflag[4]]._ty;
		do
		{
			if ( (*v6)[0] == 370 && quests[QTYPE_VB]._qactive == 3 )
			{
				++trigflag[4];
				*(v8 - 1) = v7;
				*v8 = v5;
				v8[1] = WM_DIABNEXTLVL;
				v8 += 4;
			}
			++v7;
			++v6;
		}
		while ( v7 < 112 );
		v10 = (int (*)[112])((char *)v10 + 4);
		++v5;
	}
	while ( (signed int)v10 < (signed int)dPiece[1] );
	trigflag[3] = 0;
}

void __cdecl InitSKingTriggers()
{
	trigflag[3] = 0;
	trigflag[4] = 1;
	trigs[0]._tx = 82;
	trigs[0]._ty = 42;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void __cdecl InitSChambTriggers()
{
	trigflag[3] = 0;
	trigflag[4] = 1;
	trigs[0]._tx = 70;
	trigs[0]._ty = 39;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void __cdecl InitPWaterTriggers()
{
	trigflag[3] = 0;
	trigflag[4] = 1;
	trigs[0]._tx = 30;
	trigs[0]._ty = 83;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

void __cdecl InitVPTriggers()
{
	trigflag[3] = 0;
	trigflag[4] = 1;
	trigs[0]._tx = 35;
	trigs[0]._ty = 32;
	trigs[0]._tmsg = WM_DIABRTNLVL;
}

unsigned char __cdecl ForceTownTrig()
{
	int v0; // edx
	int *v1; // esi
	int v2; // edx
	int *v3; // esi
	signed int v4; // esi
	signed int v5; // edx

	v0 = TownDownList[0];
	if ( TownDownList[0] != -1 )
	{
		v1 = TownDownList;
		while ( dPiece[0][cursmy + 112 * cursmx] != v0 )
		{
			++v1;
			v0 = *v1;
			if ( *v1 == -1 )
				goto LABEL_5;
		}
		strcpy(infostr, "Down to dungeon");
		cursmx = 25;
		cursmy = 29;
		return 1;
	}
LABEL_5:
	if ( trigflag[0] )
	{
		v2 = TownWarp1List[0];
		if ( TownWarp1List[0] != -1 )
		{
			v3 = TownWarp1List;
			while ( dPiece[0][cursmy + 112 * cursmx] != v2 )
			{
				++v3;
				v2 = *v3;
				if ( *v3 == -1 )
					goto LABEL_13;
			}
			strcpy(infostr, "Down to catacombs");
			cursmx = 49;
			cursmy = 21;
			return 1;
		}
	}
LABEL_13:
	if ( trigflag[1] )
	{
		v4 = 1199;
		while ( dPiece[0][cursmy + 112 * cursmx] != v4 )
		{
			if ( ++v4 > 1220 )
				goto LABEL_17;
		}
		strcpy(infostr, "Down to caves");
		cursmx = 17;
		cursmy = 69;
		return 1;
	}
LABEL_17:
	if ( trigflag[2] )
	{
		v5 = 1240;
		while ( dPiece[0][cursmy + 112 * cursmx] != v5 )
		{
			if ( ++v5 > 1255 )
				return 0;
		}
		strcpy(infostr, "Down to hell");
		cursmx = 41;
		cursmy = 80;
		return 1;
	}
	return 0;
}

unsigned char __cdecl ForceL1Trig()
{
	int *v0; // eax
	int *v1; // esi
	int v2; // eax
	int *v3; // edx
	int *v4; // eax
	int *v5; // esi
	int *v6; // edx
	int v8; // eax
	int v9; // ecx

	if ( L1UpList[0] == -1 )
	{
LABEL_12:
		if ( L1DownList[0] == -1 )
			return 0;
		v4 = L1DownList;
		v5 = L1DownList;
		while ( 1 )
		{
			if ( dPiece[0][cursmy + 112 * cursmx] == *v4 )
			{
				sprintf(infostr, "Down to level %i", currlevel + 1);
				v2 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_19:
			++v5;
			v4 = v5;
			if ( *v5 == -1 )
				return 0;
		}
		v6 = &trigs[0]._tmsg;
		while ( *v6 != WM_DIABNEXTLVL )
		{
			++v2;
			v6 += 4;
			if ( v2 >= trigflag[4] )
				goto LABEL_19;
		}
	}
	else
	{
		v0 = L1UpList;
		v1 = L1UpList;
		while ( 1 )
		{
			if ( dPiece[0][cursmy + 112 * cursmx] == *v0 )
			{
				if ( currlevel <= 1u )
					strcpy(infostr, "Up to town");
				else
					sprintf(infostr, "Up to level %i", currlevel - 1);
				v2 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_11:
			++v1;
			v0 = v1;
			if ( *v1 == -1 )
				goto LABEL_12;
		}
		v3 = &trigs[0]._tmsg;
		while ( *v3 != WM_DIABPREVLVL )
		{
			++v2;
			v3 += 4;
			if ( v2 >= trigflag[4] )
				goto LABEL_11;
		}
	}
	v8 = v2;
	v9 = trigs[v8]._tx;
	cursmy = trigs[v8]._ty;
	cursmx = v9;
	return 1;
}

unsigned char __cdecl ForceL2Trig()
{
	int *v0; // eax
	int *v1; // ebp
	int v2; // edi
	TriggerStruct *v3; // esi
	int v4; // ebx
	int v5; // eax
	int *v6; // eax
	int *v7; // esi
	int v8; // eax
	int *v9; // ecx
	int v10; // eax
	int v11; // ecx
	int v12; // eax
	int *v13; // eax
	int *v14; // ebp
	TriggerStruct *v15; // esi
	int v16; // ebx
	int v17; // eax
	int v19; // edi

	if ( L2UpList[0] == -1 )
	{
LABEL_11:
		if ( L2DownList[0] != -1 )
		{
			v6 = L2DownList;
			v7 = L2DownList;
			while ( 1 )
			{
				if ( dPiece[0][cursmy + 112 * cursmx] == *v6 )
				{
					sprintf(infostr, "Down to level %i", currlevel + 1);
					v8 = 0;
					if ( trigflag[4] > 0 )
						break;
				}
LABEL_18:
				++v7;
				v6 = v7;
				if ( *v7 == -1 )
					goto LABEL_22;
			}
			v9 = &trigs[0]._tmsg;
			while ( *v9 != WM_DIABNEXTLVL )
			{
				++v8;
				v9 += 4;
				if ( v8 >= trigflag[4] )
					goto LABEL_18;
			}
			v10 = v8;
			v11 = trigs[v10]._tx;
			v12 = trigs[v10]._ty;
			cursmx = v11;
			goto LABEL_37;
		}
LABEL_22:
		if ( currlevel != 5 || L2TWarpUpList[0] == -1 )
			return 0;
		v13 = L2TWarpUpList;
		v14 = L2TWarpUpList;
		while ( 1 )
		{
			if ( dPiece[0][cursmy + 112 * cursmx] == *v13 )
			{
				v2 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_32:
			++v14;
			v13 = v14;
			if ( *v14 == -1 )
				return 0;
		}
		v15 = trigs;
		while ( 1 )
		{
			if ( v15->_tmsg == WM_DIABTWARPUP )
			{
				v16 = abs(v15->_tx - cursmx);
				v17 = abs(v15->_ty - cursmy);
				if ( v16 < 4 && v17 < 4 )
					break;
			}
			++v2;
			++v15;
			if ( v2 >= trigflag[4] )
				goto LABEL_32;
		}
		strcpy(infostr, "Up to town");
	}
	else
	{
		v0 = L2UpList;
		v1 = L2UpList;
		while ( 1 )
		{
			if ( dPiece[0][cursmy + 112 * cursmx] == *v0 )
			{
				v2 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_10:
			++v1;
			v0 = v1;
			if ( *v1 == -1 )
				goto LABEL_11;
		}
		v3 = trigs;
		while ( 1 )
		{
			if ( v3->_tmsg == WM_DIABPREVLVL )
			{
				v4 = abs(v3->_tx - cursmx);
				v5 = abs(v3->_ty - cursmy);
				if ( v4 < 4 && v5 < 4 )
					break;
			}
			++v2;
			++v3;
			if ( v2 >= trigflag[4] )
				goto LABEL_10;
		}
		sprintf(infostr, "Up to level %i", currlevel - 1);
	}
	v19 = v2;
	cursmx = trigs[v19]._tx;
	v12 = trigs[v19]._ty;
LABEL_37:
	cursmy = v12;
	return 1;
}

unsigned char __cdecl ForceL3Trig()
{
	int *v0; // eax
	int *v1; // esi
	int v2; // eax
	int *v3; // ecx
	int *v4; // ecx
	int *v5; // esi
	int v6; // ecx
	int v7; // eax
	int *v8; // ecx
	int *v9; // eax
	int *v10; // ebp
	int v11; // edi
	TriggerStruct *v12; // esi
	int v13; // ebx
	int v14; // eax
	int v15; // eax
	int v16; // ecx
	int v17; // eax
	int v18; // edi

	if ( L3UpList[0] != -1 )
	{
		v0 = L3UpList;
		v1 = L3UpList;
		while ( 1 )
		{
			if ( dPiece[0][cursmy + 112 * cursmx] == *v0 )
			{
				sprintf(infostr, "Up to level %i", currlevel - 1);
				v2 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_8:
			++v1;
			v0 = v1;
			if ( *v1 == -1 )
				goto LABEL_9;
		}
		v3 = &trigs[0]._tmsg;
		while ( *v3 != WM_DIABPREVLVL )
		{
			++v2;
			v3 += 4;
			if ( v2 >= trigflag[4] )
				goto LABEL_8;
		}
		goto LABEL_31;
	}
LABEL_9:
	if ( L3DownList[0] != -1 )
	{
		v4 = L3DownList;
		v5 = L3DownList;
		while ( 1 )
		{
			v6 = *v4;
			v7 = cursmy + 112 * cursmx;
			if ( dPiece[0][v7] == v6 || dPiece[1][v7] == v6 || dPiece[2][v7] == v6 )
			{
				sprintf(infostr, "Down to level %i", currlevel + 1);
				v2 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_18:
			++v5;
			v4 = v5;
			if ( *v5 == -1 )
				goto LABEL_19;
		}
		v8 = &trigs[0]._tmsg;
		while ( *v8 != WM_DIABNEXTLVL )
		{
			++v2;
			v8 += 4;
			if ( v2 >= trigflag[4] )
				goto LABEL_18;
		}
LABEL_31:
		v15 = v2;
		v16 = trigs[v15]._tx;
		v17 = trigs[v15]._ty;
		cursmx = v16;
LABEL_33:
		cursmy = v17;
		return 1;
	}
LABEL_19:
	if ( currlevel == 9 && L3TWarpUpList[0] != -1 )
	{
		v9 = L3TWarpUpList;
		v10 = L3TWarpUpList;
		while ( 1 )
		{
			if ( dPiece[0][cursmy + 112 * cursmx] == *v9 )
			{
				v11 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_29:
			++v10;
			v9 = v10;
			if ( *v10 == -1 )
				return 0;
		}
		v12 = trigs;
		while ( 1 )
		{
			if ( v12->_tmsg == WM_DIABTWARPUP )
			{
				v13 = abs(v12->_tx - cursmx);
				v14 = abs(v12->_ty - cursmy);
				if ( v13 < 4 && v14 < 4 )
					break;
			}
			++v11;
			++v12;
			if ( v11 >= trigflag[4] )
				goto LABEL_29;
		}
		strcpy(infostr, "Up to town");
		v18 = v11;
		cursmx = trigs[v18]._tx;
		v17 = trigs[v18]._ty;
		goto LABEL_33;
	}
	return 0;
}

unsigned char __cdecl ForceL4Trig()
{
	int *v0; // eax
	int *v1; // esi
	int v2; // eax
	int *v3; // ecx
	int *v4; // eax
	int *v5; // esi
	int *v6; // ecx
	int *v7; // eax
	int *v8; // ebp
	int v9; // edi
	TriggerStruct *v10; // esi
	int v11; // ebx
	int v12; // eax
	int *v13; // eax
	int *v14; // esi
	int *v15; // edx
	int v16; // edi
	int v17; // eax
	int v18; // eax
	int v19; // ecx

	if ( L4UpList[0] != -1 )
	{
		v0 = L4UpList;
		v1 = L4UpList;
		while ( 1 )
		{
			if ( dPiece[0][cursmy + 112 * cursmx] == *v0 )
			{
				sprintf(infostr, "Up to level %i", currlevel - 1);
				v2 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_8:
			++v1;
			v0 = v1;
			if ( *v1 == -1 )
				goto LABEL_9;
		}
		v3 = &trigs[0]._tmsg;
		while ( *v3 != WM_DIABPREVLVL )
		{
			++v2;
			v3 += 4;
			if ( v2 >= trigflag[4] )
				goto LABEL_8;
		}
		goto LABEL_39;
	}
LABEL_9:
	if ( L4DownList[0] != -1 )
	{
		v4 = L4DownList;
		v5 = L4DownList;
		while ( 1 )
		{
			if ( dPiece[0][cursmy + 112 * cursmx] == *v4 )
			{
				sprintf(infostr, "Down to level %i", currlevel + 1);
				v2 = 0;
				if ( trigflag[4] > 0 )
					break;
			}
LABEL_16:
			++v5;
			v4 = v5;
			if ( *v5 == -1 )
				goto LABEL_17;
		}
		v6 = &trigs[0]._tmsg;
		while ( *v6 != WM_DIABNEXTLVL )
		{
			++v2;
			v6 += 4;
			if ( v2 >= trigflag[4] )
				goto LABEL_16;
		}
		goto LABEL_39;
	}
LABEL_17:
	if ( currlevel == 13 )
	{
		if ( L4TWarpUpList[0] != -1 )
		{
			v7 = L4TWarpUpList;
			v8 = L4TWarpUpList;
			while ( 1 )
			{
				if ( dPiece[0][cursmy + 112 * cursmx] == *v7 )
				{
					v9 = 0;
					if ( trigflag[4] > 0 )
						break;
				}
LABEL_27:
				++v8;
				v7 = v8;
				if ( *v8 == -1 )
					goto LABEL_28;
			}
			v10 = trigs;
			while ( 1 )
			{
				if ( v10->_tmsg == WM_DIABTWARPUP )
				{
					v11 = abs(v10->_tx - cursmx);
					v12 = abs(v10->_ty - cursmy);
					if ( v11 < 4 && v12 < 4 )
						break;
				}
				++v9;
				++v10;
				if ( v9 >= trigflag[4] )
					goto LABEL_27;
			}
			strcpy(infostr, "Up to town");
			v16 = v9;
			cursmx = trigs[v16]._tx;
			v17 = trigs[v16]._ty;
			goto LABEL_40;
		}
	}
	else
	{
LABEL_28:
		if ( currlevel == 15 && L4PentaList[0] != -1 )
		{
			v13 = L4PentaList;
			v14 = L4PentaList;
			while ( 1 )
			{
				if ( dPiece[0][cursmy + 112 * cursmx] == *v13 )
				{
					strcpy(infostr, "Down to Diablo");
					v2 = 0;
					if ( trigflag[4] > 0 )
						break;
				}
LABEL_36:
				++v14;
				v13 = v14;
				if ( *v14 == -1 )
					return 0;
			}
			v15 = &trigs[0]._tmsg;
			while ( *v15 != WM_DIABNEXTLVL )
			{
				++v2;
				v15 += 4;
				if ( v2 >= trigflag[4] )
					goto LABEL_36;
			}
LABEL_39:
			v18 = v2;
			v19 = trigs[v18]._tx;
			v17 = trigs[v18]._ty;
			cursmx = v19;
LABEL_40:
			cursmy = v17;
			return 1;
		}
	}
	return 0;
}

void __cdecl Freeupstairs()
{
	int *v0; // ecx
	int v1; // ebx
	char *v2; // eax
	signed int v3; // edi
	char *v4; // edx
	signed int v5; // esi

	if ( trigflag[4] > 0 )
	{
		v0 = &trigs[0]._ty;
		v1 = trigflag[4];
		do
		{
			v2 = &dFlags[*(v0 - 1)-2][*v0-2]; /* v2 = &nBlockTable[112 * *(v0 - 1) + 1830 + *v0]; check */
			v3 = 5;
			do
			{
				v4 = v2;
				v5 = 5;
				do
				{
					*v4 |= DFLAG_POPULATED;
					v4 += 112;
					--v5;
				}
				while ( v5 );
				++v2;
				--v3;
			}
			while ( v3 );
			v0 += 4;
			--v1;
		}
		while ( v1 );
	}
}

unsigned char __cdecl ForceSKingTrig()
{
	int v0; // eax
	int *v1; // ecx

	v0 = L1UpList[0];
	if ( L1UpList[0] == -1 )
		return 0;
	v1 = L1UpList;
	while ( dPiece[0][cursmy + 112 * cursmx] != v0 )
	{
		++v1;
		v0 = *v1;
		if ( *v1 == -1 )
			return 0;
	}
	sprintf(infostr, "Back to Level %i", (unsigned char)quests[QTYPE_KING]._qlevel);
	cursmx = trigs[0]._tx;
	cursmy = trigs[0]._ty;
	return 1;
}

unsigned char __cdecl ForceSChambTrig()
{
	int v0; // eax
	int *v1; // ecx

	v0 = L2DownList[0];
	if ( L2DownList[0] == -1 )
		return 0;
	v1 = L2DownList;
	while ( dPiece[0][cursmy + 112 * cursmx] != v0 )
	{
		++v1;
		v0 = *v1;
		if ( *v1 == -1 )
			return 0;
	}
	sprintf(infostr, "Back to Level %i", (unsigned char)quests[QTYPE_BONE]._qlevel);
	cursmx = trigs[0]._tx;
	cursmy = trigs[0]._ty;
	return 1;
}

unsigned char __cdecl ForcePWaterTrig()
{
	int v0; // eax
	int *v1; // ecx

	v0 = L3DownList[0];
	if ( L3DownList[0] == -1 )
		return 0;
	v1 = L3DownList;
	while ( dPiece[0][cursmy + 112 * cursmx] != v0 )
	{
		++v1;
		v0 = *v1;
		if ( *v1 == -1 )
			return 0;
	}
	sprintf(infostr, "Back to Level %i", (unsigned char)quests[QTYPE_PW]._qlevel);
	cursmx = trigs[0]._tx;
	cursmy = trigs[0]._ty;
	return 1;
}

void __cdecl CheckTrigForce()
{
	int v0; // eax
	int v1; // eax

	trigflag[3] = 0;
	if ( MouseY <= 351 )
	{
		if ( setlevel )
		{
			switch ( setlvlnum )
			{
				case SL_SKELKING:
					v1 = ForceSKingTrig();
					break;
				case SL_BONECHAMB:
					v1 = ForceSChambTrig();
					break;
				case SL_POISONWATER:
					v1 = ForcePWaterTrig();
					break;
				default:
					return;
			}
			goto LABEL_23;
		}
		if ( leveltype )
		{
			switch ( leveltype )
			{
				case DTYPE_CATHEDRAL:
					v0 = ForceL1Trig();
					break;
				case DTYPE_CATACOMBS:
					v0 = ForceL2Trig();
					break;
				case DTYPE_CAVES:
					v0 = ForceL3Trig();
					break;
				case DTYPE_HELL:
					v0 = ForceL4Trig();
					break;
				default:
LABEL_14:
					if ( leveltype == DTYPE_TOWN )
						goto LABEL_24;
					if ( trigflag[3] )
					{
LABEL_25:
						ClearPanel();
						return;
					}
					v1 = ForceQuests();
LABEL_23:
					trigflag[3] = v1;
LABEL_24:
					if ( !trigflag[3] )
						return;
					goto LABEL_25;
			}
		}
		else
		{
			v0 = ForceTownTrig();
		}
		trigflag[3] = v0;
		goto LABEL_14;
	}
}
// 5BB1ED: using guessed type char leveltype;
// 5CCB10: using guessed type char setlvlnum;
// 5CF31D: using guessed type char setlevel;

void __cdecl CheckTriggers()
{
	int *v0; // edi
	int v1; // esi
	int v2; // ecx
	int v3; // eax
	int v4; // edx
	signed int v5; // edx
	int v6; // eax
	char v7; // al
	int v8; // ecx
	int v9; // [esp-4h] [ebp-20h]
	int x; // [esp+Ch] [ebp-10h]
	int y; // [esp+10h] [ebp-Ch]
	int v12; // [esp+14h] [ebp-8h]
	int error_id; // [esp+1Bh] [ebp-1h]

	if ( plr[myplr]._pmode )
		return;
	v12 = 0;
	if ( trigflag[4] <= 0 )
		return;
	v0 = &trigs[0]._tmsg;
	while ( 1 )
	{
		v1 = myplr;
		v2 = plr[myplr].WorldX;
		if ( v2 != *(v0 - 2) )
			goto LABEL_34;
		v3 = plr[v1].WorldY;
		if ( v3 != *(v0 - 1) )
			goto LABEL_34;
		v4 = *v0;
		if ( *v0 == WM_DIABNEXTLVL )
		{
			if ( pcurs >= CURSOR_FIRSTITEM && DropItemBeforeTrig() )
				return;
			v6 = currlevel + 1;
			goto LABEL_32;
		}
		if ( *v0 == WM_DIABPREVLVL )
		{
			if ( pcurs >= CURSOR_FIRSTITEM && DropItemBeforeTrig() )
				return;
			v6 = currlevel - 1;
LABEL_32:
			v9 = v6;
			goto LABEL_33;
		}
		if ( *v0 != WM_DIABRTNLVL )
			break;
		StartNewLvl(myplr, v4, ReturnLvl);
LABEL_34:
		++v12;
		v0 += 4;
		if ( v12 >= trigflag[4] )
			return;
	}
	if ( *v0 != WM_DIABTOWNWARP )
	{
		if ( *v0 == WM_DIABTWARPUP )
		{
			TWarpFrom = currlevel;
			StartNewLvl(myplr, v4, 0);
		}
		else
		{
			TermMsg("Unknown trigger msg");
		}
		goto LABEL_34;
	}
	if ( gbMaxPlayers == 1 )
		goto LABEL_46;
	v5 = 0;
	if ( v0[1] == 5 && plr[v1]._pLevel < 8 )
	{
		v5 = 1;
		x = plr[myplr].WorldX;
		_LOBYTE(y) = v3 + 1;
		_LOBYTE(error_id) = 40;
	}
	if ( v0[1] == 9 && plr[v1]._pLevel < 13 )
	{
		v5 = 1;
		_LOBYTE(x) = v2 + 1;
		y = plr[v1].WorldY;
		_LOBYTE(error_id) = 41;
	}
	if ( v0[1] == 13 && plr[v1]._pLevel < 17 )
	{
		x = plr[myplr].WorldX;
		v5 = 1;
		_LOBYTE(y) = v3 + 1;
		_LOBYTE(error_id) = 42;
	}
	if ( !v5 )
	{
LABEL_46:
		v9 = v0[1];
LABEL_33:
		StartNewLvl(myplr, *v0, v9);
		goto LABEL_34;
	}
	v7 = plr[myplr]._pClass;
	switch ( v7 )
	{
		case UI_WARRIOR:
			v8 = PS_WARR43;
			goto LABEL_42;
		case UI_ROGUE:
			v8 = PS_ROGUE43;
			goto LABEL_42;
		case UI_SORCERER:
			v8 = PS_MAGE43;
LABEL_42:
			PlaySFX(v8);
			break;
	}
	_LOBYTE(v2) = error_id;
	InitDiabloMsg(v2);
	NetSendCmdLoc(1u, CMD_WALKXY, x, y);
}
// 679660: using guessed type char gbMaxPlayers;
// 6ABB30: using guessed type int TWarpFrom;
