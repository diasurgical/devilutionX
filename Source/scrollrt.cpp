//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
int light_table_index; // weak
int screen_y_times_768[1024];
int scrollrt_cpp_init_value; // weak
unsigned int sgdwCursWdtOld; // idb
int sgdwCursX; // idb
int sgdwCursY; // idb
unsigned char *gpBufEnd; // weak
int sgdwCursHgt;
int level_cel_block; // weak
int sgdwCursXOld; // idb
int sgdwCursYOld; // idb
char arch_draw_type; // weak
DDSURFACEDESC DDS_desc;
int cel_transparency_active; // weak
int level_piece_id; // weak
int sgdwCursWdt;
int (__fastcall *DrawPlrProc)(int player_num, int x, int y, int screen_x, int screen_y, void *cl2_buf, int frame, int frame_width, int a9, int a10);
char sgSaveBack[8192];
int draw_monster_num; // weak
int sgdwCursHgtOld; // idb
#endif

const int scrollrt_inf = 0x7F800000; // weak

/* data */

/* used in 1.00 debug */
char *szMonModeAssert[18] =
{
	"standing",
	"walking (1)",
	"walking (2)",
	"walking (3)",
	"attacking",
	"getting hit",
	"dying",
	"attacking (special)",
	"fading in",
	"fading out",
	"attacking (ranged)",
	"standing (special)",
	"attacking (special ranged)",
	"delaying",
	"charging",
	"stoned",
	"healing",
	"talking"
};

char *szPlrModeAssert[12] =
{
	"standing",
	"walking (1)",
	"walking (2)",
	"walking (3)",
	"attacking (melee)",
	"attacking (ranged)",
	"blocking",
	"getting hit",
	"dying",
	"casting a spell",
	"changing levels",
	"quitting"
};

struct scrollrt_cpp_init
{
	scrollrt_cpp_init()
	{
		scrollrt_cpp_init_value = scrollrt_inf;
	}
} _scrollrt_cpp_init;
// 47F238: using guessed type int scrollrt_inf;
// 69CEFC: using guessed type int scrollrt_cpp_init_value;

void __cdecl ClearCursor() // CODE_FIX: this was supposed to be in cursor.cpp
{
	sgdwCursWdt = 0;
	sgdwCursWdtOld = 0;
}

void __fastcall DrawMissile(int x, int y, int sx, int sy, int a5, int a6, int del_flag)
{
	int v7; // ebx
	char v8; // al
	int v9; // eax
	int v10; // eax
	MissileStruct *v11; // eax
	char *v12; // edi
	int v13; // edx
	int v14; // esi
	int v15; // ecx
	MissileStruct *v16; // eax
	char *v17; // edi
	int v18; // edx
	int v19; // esi
	int v20; // ecx
	int v21; // [esp-10h] [ebp-28h]
	int v22; // [esp-10h] [ebp-28h]
	int v23; // [esp-Ch] [ebp-24h]
	int v24; // [esp-Ch] [ebp-24h]
	int v25; // [esp+Ch] [ebp-Ch]
	int v26; // [esp+10h] [ebp-8h]
	int i; // [esp+14h] [ebp-4h]

	v26 = x;
	v7 = y;
	v8 = dMissile[x][y];
	v25 = y;
	if ( v8 == -1 )
	{
		v9 = 0;
		for ( i = 0; i < nummissiles; v9 = i++ + 1 )
		{
			v10 = missileactive[v9];
			if ( v10 >= MAXMISSILES )
				break;
			v11 = &missile[v10];
			if ( v11->_mix == v26 && v11->_miy == v7 && v11->_miPreFlag == del_flag && v11->_miDrawFlag )
			{
				v12 = (char *)v11->_miAnimData;
				if ( !v12 )
					return;
				v13 = v11->_miAnimFrame;
				if ( v13 < 1 || (unsigned int)*v12 > 0x32 || v13 > *v12 )
					return;
				v14 = sy + v11->_miyoff;
				v15 = sx + v11->_mixoff - v11->_miAnimWidth2;
				if ( v11->_miUniqTrans )
				{
					Cl2DecodeFrm3(v15, v14, v12, v13, v11->_miAnimWidth, a5, a6, _LOBYTE(v11->_miUniqTrans) + 3);
					v7 = v25;
				}
				else
				{
					v23 = v11->_miAnimWidth;
					v21 = v11->_miAnimFrame;
					if ( v11->_miLightFlag )
						Cl2DecodeLightTbl(v15, v14, v12, v21, v23, a5, a6);
					else
						Cl2DecodeFrm1(v15, v14, v12, v21, v23, a5, a6);
				}
			}
		}
	}
	else
	{
		v16 = &missile[v8-1];
		if ( v16->_miPreFlag == del_flag )
		{
			if ( v16->_miDrawFlag )
			{
				v17 = (char *)v16->_miAnimData;
				if ( v17 )
				{
					v18 = v16->_miAnimFrame;
					if ( v18 >= 1 && (unsigned int)*v17 <= 0x32 && v18 <= *v17 )
					{
						v19 = sy + v16->_miyoff;
						v20 = sx + v16->_mixoff - v16->_miAnimWidth2;
						if ( v16->_miUniqTrans )
						{
							Cl2DecodeFrm3(v20, v19, v17, v18, v16->_miAnimWidth, a5, a6, LOBYTE(v16->_miUniqTrans) + 3);
						}
						else
						{
							v24 = v16->_miAnimWidth;
							v22 = v16->_miAnimFrame;
							if ( v16->_miLightFlag )
								Cl2DecodeLightTbl(v20, v19, v17, v22, v24, a5, a6);
							else
								Cl2DecodeFrm1(v20, v19, v17, v22, v24, a5, a6);
						}
					}
				}
			}
		}
	}
}

void __fastcall DrawClippedMissile(int x, int y, int sx, int sy, int a5, int a6, int a7)
{
	int v7; // ebx
	char v8; // al
	int v9; // eax
	int v10; // eax
	MissileStruct *v11; // eax
	char *v12; // edi
	int v13; // edx
	int v14; // esi
	int v15; // ecx
	MissileStruct *v16; // eax
	char *v17; // edi
	int v18; // edx
	int v19; // esi
	int v20; // ecx
	int v21; // [esp-10h] [ebp-28h]
	int v22; // [esp-10h] [ebp-28h]
	int v23; // [esp-Ch] [ebp-24h]
	int v24; // [esp-Ch] [ebp-24h]
	int v25; // [esp+Ch] [ebp-Ch]
	int v26; // [esp+10h] [ebp-8h]
	int i; // [esp+14h] [ebp-4h]

	v26 = x;
	v7 = y;
	v8 = dMissile[x][y];
	v25 = y;
	if ( v8 == -1 )
	{
		v9 = 0;
		for ( i = 0; i < nummissiles; v9 = i++ + 1 )
		{
			v10 = missileactive[v9];
			if ( v10 >= MAXMISSILES )
				break;
			v11 = &missile[v10];
			if ( v11->_mix == v26 && v11->_miy == v7 && v11->_miPreFlag == a7 && v11->_miDrawFlag )
			{
				v12 = (char *)v11->_miAnimData;
				if ( !v12 )
					return;
				v13 = v11->_miAnimFrame;
				if ( v13 < 1 || (unsigned int)*v12 > 0x32 || v13 > *v12 )
					return;
				v14 = sy + v11->_miyoff;
				v15 = sx + v11->_mixoff - v11->_miAnimWidth2;
				if ( v11->_miUniqTrans )
				{
					Cl2DecodeFrm5(v15, v14, v12, v13, v11->_miAnimWidth, a5, a6, _LOBYTE(v11->_miUniqTrans) + 3);
					v7 = v25;
				}
				else
				{
					v23 = v11->_miAnimWidth;
					v21 = v11->_miAnimFrame;
					if ( v11->_miLightFlag )
						Cl2DecodeFrm6(v15, v14, v12, v21, v23, a5, a6);
					else
						Cl2DecodeFrm4(v15, v14, v12, v21, v23, a5, a6);
				}
			}
		}
	}
	else
	{
		v16 = &missile[v8-1];
		if ( v16->_miPreFlag == a7 )
		{
			if ( v16->_miDrawFlag )
			{
				v17 = (char *)v16->_miAnimData;
				if ( v17 )
				{
					v18 = v16->_miAnimFrame;
					if ( v18 >= 1 && (unsigned int)*v17 <= 0x32 && v18 <= *v17 )
					{
						v19 = sy + v16->_miyoff;
						v20 = sx + v16->_mixoff - v16->_miAnimWidth2;
						if ( v16->_miUniqTrans )
						{
							Cl2DecodeFrm5(v20, v19, v17, v18, v16->_miAnimWidth, a5, a6, LOBYTE(v16->_miUniqTrans) + 3);
						}
						else
						{
							v24 = v16->_miAnimWidth;
							v22 = v16->_miAnimFrame;
							if ( v16->_miLightFlag )
								Cl2DecodeFrm6(v20, v19, v17, v22, v24, a5, a6);
							else
								Cl2DecodeFrm4(v20, v19, v17, v22, v24, a5, a6);
						}
					}
				}
			}
		}
	}
}

void __fastcall DrawDeadPlayer(int x, int y, int sx, int sy, int a5, int a6, bool clipped)
{
	int v7; // ebx
	int *v8; // esi
	char *v9; // edi
	int *v10; // eax
	int v11; // ecx
	int xa; // [esp+Ch] [ebp-4h]
	int player_num; // [esp+28h] [ebp+18h]

	v7 = y;
	xa = x;
	DrawPlrProc = (int (__fastcall *)(int, int, int, int, int, void *, int, int, int, int))DrawClippedPlayer;
	if ( !clipped )
		DrawPlrProc = (int (__fastcall *)(int, int, int, int, int, void *, int, int, int, int))DrawPlayer;
	v8 = &plr[0]._pHitPoints;
	v9 = &dFlags[x][y];
	*v9 &= ~DFLAG_DEAD_PLAYER;
	player_num = 0;
	do
	{
		if ( !*((_BYTE *)v8 - 379) || *v8 || *(v8 - 89) != currlevel || *(v8 - 88) != xa || *(v8 - 87) != v7 )
			goto LABEL_14;
		v10 = (int *)*(v8 - 71);
		if ( !v10 )
			break;
		v11 = *(v8 - 67);
		if ( v11 < 1 || (unsigned int)*v10 > 0x32 || v11 > *v10 )
			break;
		*v9 |= DFLAG_DEAD_PLAYER;
		DrawPlrProc(player_num, xa, v7, sx + *(v8 - 78) - *(v8 - 65), sy + *(v8 - 77), v10, v11, *(v8 - 66), a5, a6);
LABEL_14:
		++player_num;
		v8 += 5430;
	}
	while ( (signed int)v8 < (signed int)&plr[4]._pHitPoints );
}

void __fastcall DrawPlayer(int pnum, int x, int y, int px, int py, unsigned char *animdata, int animframe, int animwidth, int a9, int a10)
{
	char *v10; // edx
	int v11; // eax
	char *v12; // ecx
	int v13; // [esp+Ch] [ebp-4h]
	int ya; // [esp+18h] [ebp+8h]
	int animdataa; // [esp+24h] [ebp+14h]

	v10 = &dFlags[x][y];
	v11 = myplr;
	v13 = pnum;
	ya = (int)v10;
	if ( *v10 & DFLAG_LIT || plr[myplr]._pInfraFlag || !setlevel && !currlevel )
	{
		v12 = (char *)animdata;
		if ( animdata )
		{
			if ( animframe >= 1 && *(_DWORD *)animdata <= 0x32u && animframe <= *(_DWORD *)animdata )
			{
				if ( v13 == pcursplr )
				{
					Cl2DecodeFrm2(165, px, py, (char *)animdata, animframe, animwidth, a9, a10);
					v11 = myplr;
					v12 = (char *)animdata;
					v10 = (char *)ya;
				}
				if ( v13 == v11 )
				{
					Cl2DecodeFrm1(px, py, v12, animframe, animwidth, a9, a10);
					if ( plr[v13].pManaShield )
						Cl2DecodeFrm1(
							px + plr[v13]._pAnimWidth2 - misfiledata[9].mAnimWidth2[0],
							py,
							(char *)misfiledata[9].mAnimData[0],
							1,
							misfiledata[9].mAnimWidth[0],
							a9,
							a10);
				}
				else if ( !(*v10 & DFLAG_LIT) || plr[v11]._pInfraFlag && light_table_index > 8 )
				{
					Cl2DecodeFrm3(px, py, v12, animframe, animwidth, a9, a10, 1);
					if ( plr[v13].pManaShield )
						Cl2DecodeFrm3(
							px + plr[v13]._pAnimWidth2 - misfiledata[9].mAnimWidth2[0],
							py,
							(char *)misfiledata[9].mAnimData[0],
							1,
							misfiledata[9].mAnimWidth[0],
							a9,
							a10,
							1);
				}
				else
				{
					animdataa = light_table_index;
					if ( light_table_index >= 5 )
						light_table_index -= 5;
					else
						light_table_index = 0;
					Cl2DecodeLightTbl(px, py, v12, animframe, animwidth, a9, a10);
					if ( plr[v13].pManaShield )
						Cl2DecodeLightTbl(
							px + plr[v13]._pAnimWidth2 - misfiledata[9].mAnimWidth2[0],
							py,
							(char *)misfiledata[9].mAnimData[0],
							1,
							misfiledata[9].mAnimWidth[0],
							a9,
							a10);
					light_table_index = animdataa;
				}
			}
		}
	}
}
// 4B8CC2: using guessed type char pcursplr;
// 5CF31D: using guessed type char setlevel;
// 69BEF8: using guessed type int light_table_index;

void __fastcall DrawClippedPlayer(int pnum, int x, int y, int px, int py, unsigned char *animdata, int animframe, int animwidth, int a9, int a10)
{
	char *v10; // edx
	int v11; // eax
	char *v12; // ecx
	int v13; // [esp+Ch] [ebp-4h]
	int ya; // [esp+18h] [ebp+8h]
	int animdataa; // [esp+24h] [ebp+14h]

	v10 = &dFlags[x][y];
	v11 = myplr;
	v13 = pnum;
	ya = (int)v10;
	if ( *v10 & DFLAG_LIT || plr[myplr]._pInfraFlag )
	{
		v12 = (char *)animdata;
		if ( animdata )
		{
			if ( animframe >= 1 && *(_DWORD *)animdata <= 0x32u && animframe <= *(_DWORD *)animdata )
			{
				if ( v13 == pcursplr )
				{
					Cl2DecodeClrHL(165, px, py, (char *)animdata, animframe, animwidth, a9, a10);
					v11 = myplr;
					v12 = (char *)animdata;
					v10 = (char *)ya;
				}
				if ( v13 == v11 )
				{
					Cl2DecodeFrm4(px, py, v12, animframe, animwidth, a9, a10);
					if ( plr[v13].pManaShield )
						Cl2DecodeFrm4(
							px + plr[v13]._pAnimWidth2 - misfiledata[9].mAnimWidth2[0],
							py,
							(char *)misfiledata[9].mAnimData[0],
							1,
							misfiledata[9].mAnimWidth[0],
							a9,
							a10);
				}
				else if ( !(*v10 & DFLAG_LIT) || plr[v11]._pInfraFlag && light_table_index > 8 )
				{
					Cl2DecodeFrm5(px, py, v12, animframe, animwidth, a9, a10, 1);
					if ( plr[v13].pManaShield )
						Cl2DecodeFrm5(
							px + plr[v13]._pAnimWidth2 - misfiledata[9].mAnimWidth2[0],
							py,
							(char *)misfiledata[9].mAnimData[0],
							1,
							misfiledata[9].mAnimWidth[0],
							a9,
							a10,
							1);
				}
				else
				{
					animdataa = light_table_index;
					if ( light_table_index >= 5 )
						light_table_index -= 5;
					else
						light_table_index = 0;
					Cl2DecodeFrm6(px, py, v12, animframe, animwidth, a9, a10);
					if ( plr[v13].pManaShield )
						Cl2DecodeFrm6(
							px + plr[v13]._pAnimWidth2 - misfiledata[9].mAnimWidth2[0],
							py,
							(char *)misfiledata[9].mAnimData[0],
							1,
							misfiledata[9].mAnimWidth[0],
							a9,
							a10);
					light_table_index = animdataa;
				}
			}
		}
	}
}
// 4B8CC2: using guessed type char pcursplr;
// 69BEF8: using guessed type int light_table_index;

void __fastcall DrawView(int StartX, int StartY)
{
	if ( zoomflag )
		DrawGame(StartX, StartY);
	else
		DrawZoom(StartX, StartY);

	if ( automapflag )
		DrawAutomap();

	if ( invflag )
		DrawInv();
	else if ( sbookflag )
		DrawSpellBook();

	DrawDurIcon();

	if ( chrflag )
		DrawChr();
	else if ( questlog )
		DrawQuestLog();
	else if ( plr[myplr]._pStatPts && !spselflag )
		DrawLevelUpIcon();

	if ( uitemflag )
		DrawUniqueInfo();
	if ( qtextflag )
		DrawQText();
	if ( spselflag )
		DrawSpellList();
	if ( dropGoldFlag )
		DrawGoldSplit(dropGoldValue);
	if ( helpflag )
		DrawHelp();
	if ( msgflag )
		DrawDiabloMsg();

	if ( deathflag )
		RedBack();
	else if ( PauseMode )
		gmenu_draw_pause();

	DrawPlrMsg();
	gmenu_draw();
	doom_draw();
	DrawInfoBox();
	DrawLifeFlask();
	DrawManaFlask();
}
// 4B84DC: using guessed type int dropGoldFlag;
// 4B8968: using guessed type int sbookflag;
// 4B8C98: using guessed type int spselflag;
// 52569C: using guessed type int zoomflag;
// 525740: using guessed type int PauseMode;
// 52B9F1: using guessed type char msgflag;
// 646D00: using guessed type char qtextflag;
// 69BD04: using guessed type int questlog;

void __fastcall DrawGame(int x, int y)
{
	int v2; // esi
	int v3; // ebx
	int v4; // edi
	int v5; // edi
	int v6; // esi
	int v7; // edi
	int v8; // esi
	int v9; // edi
	int v10; // esi
	signed int v11; // [esp+Ch] [ebp-10h]
	signed int a6; // [esp+10h] [ebp-Ch]
	signed int a6a; // [esp+10h] [ebp-Ch]
	signed int a5; // [esp+14h] [ebp-8h]
	int ya; // [esp+18h] [ebp-4h]

	dword_5C2FF8 = 10;
	v2 = ScrollInfo._sxoff + 64;
	v3 = x - 10;
	ya = y - 1;
	a5 = 10;
	v4 = ScrollInfo._syoff + 175;
	scr_pix_width = 640;
	scr_pix_height = 352;
	dword_5C2FFC = 11;
	v11 = 8;
	if ( chrflag || questlog )
	{
		ya = y - 3;
		v3 += 2;
		v2 = ScrollInfo._sxoff + 352;
		a5 = 6;
	}
	if ( invflag || sbookflag )
	{
		ya -= 2;
		v3 += 2;
		v2 -= 32;
		a5 = 6;
	}
	switch ( ScrollInfo._sdir )
	{
		case DIR_SW:
			goto LABEL_9;
		case DIR_W:
			++a5;
LABEL_9:
			v4 = ScrollInfo._syoff + 143;
			--v3;
			--ya;
			goto LABEL_15;
		case DIR_NW:
			goto LABEL_13;
		case DIR_N:
			v11 = 9;
			goto LABEL_13;
		case DIR_NE:
			goto LABEL_15;
		case DIR_E:
			v11 = 9;
			goto LABEL_12;
		case DIR_SE:
LABEL_12:
			v2 -= 64;
			--v3;
			++ya;
LABEL_13:
			++a5;
			break;
		case DIR_OMNI:
			v2 -= 64;
			v4 = ScrollInfo._syoff + 143;
			v3 -= 2;
			++a5;
LABEL_15:
			v11 = 9;
			break;
		default:
			break;
	}
	a6 = 0;
	gpBufEnd = (unsigned char *)gpBuffer + screen_y_times_768[160];
	do
	{
		scrollrt_draw_upper(v3, ya++, v2, v4, a5, a6, 0);
		v5 = v4 + 16;
		v6 = v2 - 32;
		scrollrt_draw_upper(v3++, ya, v6, v5, a5, a6, 1);
		v2 = v6 + 32;
		v4 = v5 + 16;
		++a6;
	}
	while ( a6 < 4 );
	gpBufEnd = (unsigned char *)gpBuffer + screen_y_times_768[512];
	if ( v11 > 0 )
	{
		do
		{
			scrollrt_draw_lower(v3, ya++, v2, v4, a5, 0);
			v7 = v4 + 16;
			v8 = v2 - 32;
			scrollrt_draw_lower(v3++, ya, v8, v7, a5, 1);
			v2 = v8 + 32;
			v4 = v7 + 16;
			--v11;
		}
		while ( v11 );
	}
	arch_draw_type = 0;
	a6a = 0;
	do
	{
		scrollrt_draw_lower_2(v3, ya++, v2, v4, a5, a6a, 0);
		v9 = v4 + 16;
		v10 = v2 - 32;
		scrollrt_draw_lower_2(v3++, ya, v10, v9, a5, a6a, 1);
		v2 = v10 + 32;
		v4 = v9 + 16;
		++a6a;
	}
	while ( a6a < 4 );
}
// 4B8968: using guessed type int sbookflag;
// 5C2FF8: using guessed type int dword_5C2FF8;
// 5C2FFC: using guessed type int dword_5C2FFC;
// 5C3000: using guessed type int scr_pix_width;
// 5C3004: using guessed type int scr_pix_height;
// 69BD04: using guessed type int questlog;
// 69CF0C: using guessed type int gpBufEnd;
// 69CF20: using guessed type char arch_draw_type;

void __fastcall scrollrt_draw_lower(int x, int y, int sx, int sy, int a5, int some_flag)
{
	unsigned int v6; // edi
	unsigned int v7; // ebx
	unsigned short *v8; // esi
	unsigned int v9; // ebx
	int v10; // eax
	int v11; // ecx
	int v12; // edx
	char *v13; // edx
	int v14; // edi
	int v15; // eax
	unsigned char *v16; // edi
	unsigned char *v17; // edi
	unsigned char *v18; // edi
	unsigned char *v19; // edi
	int v20; // eax
	int v21; // edi
	int v22; // ecx
	int v23; // ecx
	int v24; // eax
	int *v25; // ebx
	int v26; // ecx
	int v27; // eax
	int v28; // edi
	unsigned char *v29; // edi
	int v30; // eax
	int v31; // eax
	int v32; // eax
	int v33; // ecx
	int v34; // eax
	int *v35; // edi
	int v36; // ecx
	int v37; // eax
	char *v38; // edi
	unsigned char *v39; // edi
	int v40; // eax
	unsigned char *v41; // edi
	unsigned char *v42; // edi
	unsigned char *v43; // edi
	unsigned char *v44; // edi
	int v45; // eax
	int v46; // [esp+Ch] [ebp-10h]
	int v47; // [esp+10h] [ebp-Ch]
	signed int sya; // [esp+14h] [ebp-8h]
	unsigned int sxa; // [esp+18h] [ebp-4h]
	signed int i; // [esp+2Ch] [ebp+10h]
	int *v51; // [esp+2Ch] [ebp+10h]

	v6 = y;
	v7 = x;
	sya = y;
	sxa = x;
	v8 = (unsigned short *)((char *)dpiece_defs_map_1 + 32 * gendung_get_dpiece_num_from_coord(x, y));
	if ( some_flag )
	{
		if ( v6 < 0x70 && v7 < 0x70 )
		{
			v9 = v7;
			v10 = dPiece[0][v9 * 112 + v6];
			light_table_index = dTransVal[v9][v6];
			level_piece_id = v10;
			if ( v10 )
			{
				v11 = (unsigned char)(nTransTable[v10] & TransList[dung_map[v9][v6]]);
				arch_draw_type = 2;
				v12 = screen_y_times_768[sy];
				cel_transparency_active = v11;
				v13 = (char *)gpBuffer + v12;
				level_cel_block = v8[1];
				v14 = (int)&v13[sx + 32];
				if ( level_cel_block )
					drawLowerScreen((unsigned char *)&v13[sx + 32]);
				v15 = v8[3];
				arch_draw_type = 0;
				v16 = (unsigned char *)(v14 - 24576);
				level_cel_block = v15;
				if ( v15 )
					drawLowerScreen(v16);
				v17 = v16 - 24576;
				level_cel_block = v8[5];
				if ( level_cel_block )
					drawLowerScreen(v17);
				v18 = v17 - 24576;
				level_cel_block = v8[7];
				if ( level_cel_block )
					drawLowerScreen(v18);
				v19 = v18 - 24576;
				level_cel_block = v8[9];
				if ( level_cel_block )
					drawLowerScreen(v19);
				v20 = v8[11];
				level_cel_block = v8[11];
				if ( v20 && leveltype == DTYPE_HELL )
					drawLowerScreen(v19 - 24576);
				v21 = sy;
				scrollrt_draw_clipped_dungeon((char *)gpBuffer + screen_y_times_768[sy] + sx, sxa, sya, sx, sy, 0);
				goto LABEL_21;
			}
			world_draw_black_tile((unsigned char *)gpBuffer + screen_y_times_768[sy] + sx);
		}
		v21 = sy;
LABEL_21:
		++sxa;
		--sya;
		sx += 64;
		v8 += 16;
		--a5;
		goto LABEL_23;
	}
	v21 = sy;
LABEL_23:
	v46 = a5;
	if ( a5 )
	{
		v22 = 112 * sxa;
		v47 = 112 * sxa;
		do
		{
			--v46;
			if ( sya < 0 || v22 >= 12544 )
				break;
			if ( sya < 112 && v22 >= 0 )
			{
				v23 = sya + v22;
				v24 = dPiece[0][v23];
				light_table_index = dTransVal[0][v23];
				level_piece_id = v24;
				if ( v24 )
				{
					v25 = &screen_y_times_768[v21];
					v26 = (unsigned char)(nTransTable[v24] & TransList[dung_map[0][v23]]);
					v27 = *v8;
					v28 = *v25;
					cel_transparency_active = v26;
					arch_draw_type = 1;
					level_cel_block = v27;
					v29 = (unsigned char *)gpBuffer + v28 + sx;
					if ( v27 )
						drawLowerScreen(v29);
					v30 = v8[1];
					arch_draw_type = 2;
					level_cel_block = v30;
					if ( v30 )
						drawLowerScreen(v29 + 32);
					arch_draw_type = 0;
					v31 = 2;
					for ( i = 2; i < dword_5A5594; i += 2 )
					{
						v29 -= 24576;
						level_cel_block = v8[v31];
						if ( level_cel_block )
							drawLowerScreen(v29);
						v32 = v8[i + 1];
						level_cel_block = v8[i + 1];
						if ( v32 )
							drawLowerScreen(v29 + 32);
						v31 = i + 2;
					}
					scrollrt_draw_clipped_dungeon((char *)gpBuffer + *v25 + sx, sxa, sya, sx, sy, 1);
					v21 = sy;
				}
				else
				{
					world_draw_black_tile((unsigned char *)gpBuffer + screen_y_times_768[v21] + sx);
				}
				v22 = v47;
			}
			++sxa;
			sx += 64;
			v22 += 112;
			--sya;
			v8 += 16;
			v47 = v22;
		}
		while ( v46 );
	}
	if ( some_flag && (unsigned int)sya < 0x70 && sxa < 0x70 )
	{
		v33 = sya + 112 * sxa;
		v34 = dPiece[0][v33];
		light_table_index = dTransVal[0][v33];
		level_piece_id = v34;
		if ( v34 )
		{
			v35 = &screen_y_times_768[v21];
			v36 = (unsigned char)(nTransTable[v34] & TransList[dung_map[0][v33]]);
			v37 = *v8;
			v51 = v35;
			v38 = (char *)gpBuffer + *v35;
			cel_transparency_active = v36;
			arch_draw_type = 1;
			level_cel_block = v37;
			v39 = (unsigned char *)&v38[sx];
			if ( v37 )
				drawLowerScreen(v39);
			v40 = v8[2];
			arch_draw_type = 0;
			v41 = v39 - 24576;
			level_cel_block = v40;
			if ( v40 )
				drawLowerScreen(v41);
			v42 = v41 - 24576;
			level_cel_block = v8[4];
			if ( level_cel_block )
				drawLowerScreen(v42);
			v43 = v42 - 24576;
			level_cel_block = v8[6];
			if ( level_cel_block )
				drawLowerScreen(v43);
			v44 = v43 - 24576;
			level_cel_block = v8[8];
			if ( level_cel_block )
				drawLowerScreen(v44);
			v45 = v8[10];
			level_cel_block = v8[10];
			if ( v45 )
			{
				if ( leveltype == DTYPE_HELL )
					drawLowerScreen(v44 - 24576);
			}
			scrollrt_draw_clipped_dungeon((char *)gpBuffer + *v51 + sx, sxa, sya, sx, sy, 0);
		}
		else
		{
			world_draw_black_tile((unsigned char *)gpBuffer + screen_y_times_768[v21] + sx);
		}
	}
}
// 5BB1ED: using guessed type char leveltype;
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF20: using guessed type char arch_draw_type;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void __fastcall scrollrt_draw_clipped_dungeon(char *a1, int sx, int sy, int a4, int a5, int a6)
{
	int v6; // eax
	char v7; // bl
	char v8; // cl
	char v9; // dl
	int *v10; // eax
	DeadStruct *v11; // eax
	int *v12; // esi
	int v13; // ecx
	int v14; // edx
	char v15; // bl
	ItemStruct *v16; // esi
	char *v17; // eax
	signed int v18; // ebx
	int v19; // ebx
	unsigned int v20; // ecx
	PlayerStruct *v21; // esi
	int v22; // esi
	int v23; // eax
	MonsterStruct *v24; // esi
	CMonster *v25; // ecx
	int v26; // ebx
	int v27; // edi
	unsigned int v28; // ecx
	PlayerStruct *v29; // esi
	int v30; // esi
	int v31; // eax
	MonsterStruct *v32; // esi
	CMonster *v33; // ecx
	int v34; // ebx
	int v35; // edi
	ItemStruct *v36; // esi
	char *v37; // ecx
	signed int v38; // ebx
	int v39; // ebx
	int v40; // [esp+Ch] [ebp-18h]
	int v41; // [esp+10h] [ebp-14h]
	char *dst_buf; // [esp+14h] [ebp-10h]
	int a1a; // [esp+18h] [ebp-Ch]
	char v44; // [esp+1Dh] [ebp-7h]
	char v45; // [esp+1Eh] [ebp-6h]
	char v46; // [esp+1Fh] [ebp-5h]
	char v47; // [esp+20h] [ebp-4h]
	char v48; // [esp+21h] [ebp-3h]
	char v49; // [esp+22h] [ebp-2h]
	char v50; // [esp+23h] [ebp-1h]

	a1a = sx;
	dst_buf = a1;
	v6 = 112 * sx + sy;
	v7 = dDead[0][v6];
	v50 = dFlags[0][v6];
	v47 = dObject[0][v6];
	v49 = dItem[0][v6];
	v8 = dPlayer[0][v6 - 1];
	v48 = dPlayer[0][v6];
	v46 = dArch[0][v6];
	v9 = dung_map[0][v6];
	v10 = (int *)((char *)dMonster + 4 * v6);
	v44 = v9;
	v45 = v8;
	v40 = *v10;
	v41 = *(v10 - 1);
	if ( visiondebug && v50 & DFLAG_LIT )
		Cel2DecodeHdrOnly(dst_buf, (char *)pSquareCel, 1, 64, 0, 8);
	if ( MissilePreFlag && v50 & DFLAG_MISSILE )
		DrawClippedMissile(a1a, sy, a4, a5, 0, 8, 1);
	if ( light_table_index < lightmax )
	{
		if ( v7 )
		{
			v11 = &dead[(v7 & 0x1F) - 1];
			v12 = (int *)v11->_deadData[(v7 >> 5) & 7];
			v13 = a4 - v11->_deadWidth2;
			if ( v12 )
			{
				v14 = v11->_deadFrame;
				if ( v14 >= 1 && (unsigned int)*v12 <= 0x32 && v14 <= *v12 )
				{
					v15 = v11->_deadtrans;
					if ( v15 )
						Cl2DecodeFrm5(v13, a5, (char *)v12, v14, v11->_deadWidth, 0, 8, v15);
					else
						Cl2DecodeFrm6(v13, a5, (char *)v12, v14, v11->_deadWidth, 0, 8);
				}
			}
		}
		if ( v47 )
			DrawClippedObject(a1a, sy, a4, a5, 1, 0, 8);
	}
	if ( v49 )
	{
		v16 = &item[v49-1];
		if ( !v16->_iPostDraw && (unsigned char)v49 <= MAXITEMS )
		{
			v17 = (char *)v16->_iAnimData;
			if ( v17 )
			{
				v18 = v16->_iAnimFrame;
				if ( v18 >= 1 && *(_DWORD *)v17 <= 0x32u && v18 <= *(_DWORD *)v17 )
				{
					v19 = a4 - v16->_iAnimWidth2;
					if ( v49 - 1 == pcursitem )
						CelDrawHdrClrHL(181, v19, a5, v17, v16->_iAnimFrame, v16->_iAnimWidth, 0, 8);
					Cel2DecodeHdrLight(v19, a5, (char *)v16->_iAnimData, v16->_iAnimFrame, v16->_iAnimWidth, 0, 8);
				}
			}
		}
	}
	if ( v50 & DFLAG_PLAYER )
	{
		v20 = -1 - v45;
		if ( v20 < 4 )
		{
			v21 = &plr[v20];
			DrawClippedPlayer(
				v20,
				a1a,
				sy - 1,
				a4 + v21->_pxoff - v21->_pAnimWidth2,
				a5 + v21->_pyoff,
				v21->_pAnimData,
				v21->_pAnimFrame,
				v21->_pAnimWidth,
				0,
				8);
			if ( a6 )
			{
				v22 = v21->_peflag;
				if ( v22 )
				{
					if ( v22 == 2 )
						scrollrt_draw_clipped_e_flag(dst_buf - 12384, a1a - 2, sy + 1, a4 - 96, a5 - 16);
					scrollrt_draw_clipped_e_flag(dst_buf - 64, a1a - 1, sy + 1, a4 - 64, a5);
				}
			}
		}
	}
	if ( v50 & DFLAG_MONSTER && (v50 & DFLAG_LIT || plr[myplr]._pInfraFlag) && v41 < 0 )
	{
		v23 = -1 - v41;
		draw_monster_num = -1 - v41;
		if ( (unsigned int)(-1 - v41) < MAXMONSTERS )
		{
			v24 = &monster[v23];
			if ( !(v24->_mFlags & 1) )
			{
				v25 = v24->MType;
				if ( v25 )
				{
					v26 = a5 + v24->_myoff;
					v27 = a4 + v24->_mxoff - v25->flags_2;
					if ( v23 == pcursmonst )
					{
						Cl2DecodeClrHL(233, v27, v26, (char *)v24->_mAnimData, v24->_mAnimFrame, v25->flags_1, 0, 8);
						v23 = draw_monster_num;
					}
					DrawClippedMonster(a1a, sy, v27, v26, v23, 0, 8);
					if ( a6 && v24->_meflag )
						scrollrt_draw_clipped_e_flag(dst_buf - 64, a1a - 1, sy + 1, a4 - 64, a5);
				}
			}
		}
	}
	if ( v50 & 4 )
		DrawDeadPlayer(a1a, sy, a4, a5, 0, 8, 1);
	if ( v48 > 0 )
	{
		v28 = v48 - 1;
		if ( v28 < 4 )
		{
			v29 = &plr[v28];
			DrawClippedPlayer(
				v28,
				a1a,
				sy,
				a4 + v29->_pxoff - v29->_pAnimWidth2,
				a5 + v29->_pyoff,
				v29->_pAnimData,
				v29->_pAnimFrame,
				v29->_pAnimWidth,
				0,
				8);
			if ( a6 )
			{
				v30 = v29->_peflag;
				if ( v30 )
				{
					if ( v30 == 2 )
						scrollrt_draw_clipped_e_flag(dst_buf - 12384, a1a - 2, sy + 1, a4 - 96, a5 - 16);
					scrollrt_draw_clipped_e_flag(dst_buf - 64, a1a - 1, sy + 1, a4 - 64, a5);
				}
			}
		}
	}
	if ( v40 > 0 && (v50 & 0x40 || plr[myplr]._pInfraFlag) )
	{
		v31 = v40 - 1;
		draw_monster_num = v40 - 1;
		if ( (unsigned int)(v40 - 1) < MAXMONSTERS )
		{
			v32 = &monster[v31];
			if ( !(v32->_mFlags & 1) )
			{
				v33 = v32->MType;
				if ( v33 )
				{
					v34 = a5 + v32->_myoff;
					v35 = a4 + v32->_mxoff - v33->flags_2;
					if ( v31 == pcursmonst )
					{
						Cl2DecodeClrHL(233, v35, v34, (char *)v32->_mAnimData, v32->_mAnimFrame, v33->flags_1, 0, 8);
						v31 = draw_monster_num;
					}
					DrawClippedMonster(a1a, sy, v35, v34, v31, 0, 8);
					if ( a6 && v32->_meflag )
						scrollrt_draw_clipped_e_flag(dst_buf - 64, a1a - 1, sy + 1, a4 - 64, a5);
				}
			}
		}
	}
	if ( v50 & 1 )
		DrawClippedMissile(a1a, sy, a4, a5, 0, 8, 0);
	if ( v47 && light_table_index < lightmax )
		DrawClippedObject(a1a, sy, a4, a5, 0, 0, 8);
	if ( v49 )
	{
		v36 = &item[v49-1];
		if ( v36->_iPostDraw )
		{
			if ( (unsigned char)v49 <= MAXITEMS )
			{
				v37 = (char *)v36->_iAnimData;
				if ( v37 )
				{
					v38 = v36->_iAnimFrame;
					if ( v38 >= 1 && *(_DWORD *)v37 <= 0x32u && v38 <= *(_DWORD *)v37 )
					{
						v39 = a4 - v36->_iAnimWidth2;
						if ( v49 - 1 == pcursitem )
							CelDrawHdrClrHL(181, v39, a5, v37, v36->_iAnimFrame, v36->_iAnimWidth, 0, 8);
						Cel2DecodeHdrLight(
							v39,
							a5,
							(char *)v36->_iAnimData,
							v36->_iAnimFrame,
							v36->_iAnimWidth,
							0,
							8);
					}
				}
			}
		}
	}
	if ( v46 )
	{
		cel_transparency_active = (unsigned char)TransList[v44];
		Cel2DecodeLightTrans(dst_buf, (char *)level_special_cel, v46, 64, 0, 8);
	}
}
// 4B8CC0: using guessed type char pcursitem;
// 525720: using guessed type int visiondebug;
// 642A14: using guessed type char lightmax;
// 64CCD4: using guessed type int MissilePreFlag;
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;
// 69EFA4: using guessed type int draw_monster_num;

void __fastcall DrawClippedMonster(int x, int y, int a3, int a4, int mon_id, int a6, int a7)
{
	int v7; // eax
	char *v8; // esi
	signed int v9; // ebx
	char v10; // cl
	CMonster *v11; // eax
	char mon_ida; // [esp+1Ch] [ebp+10h]

	if ( (unsigned int)mon_id < MAXMONSTERS )
	{
		v7 = mon_id;
		v8 = (char *)monster[mon_id]._mAnimData;
		if ( v8 )
		{
			v9 = monster[v7]._mAnimFrame;
			if ( v9 >= 1 && (unsigned int)*v8 <= 0x32 && v9 <= *v8 )
			{
				if ( dFlags[x][y] & DFLAG_LIT )
				{
					v10 = 0;
					mon_ida = 0;
					if ( monster[v7]._uniqtype )
					{
						v10 = monster[v7]._uniqtrans + 4;
						mon_ida = monster[v7]._uniqtrans + 4;
					}
					if ( monster[v7]._mmode == MM_STONE )
					{
						v10 = 2;
						mon_ida = 2;
					}
					if ( plr[myplr]._pInfraFlag && light_table_index > 8 )
					{
						v10 = 1;
						mon_ida = 1;
					}
					v11 = monster[v7].MType;
					if ( v10 )
						Cl2DecodeFrm5(a3, a4, v8, v9, v11->flags_1, a6, a7, mon_ida);
					else
						Cl2DecodeFrm6(a3, a4, v8, v9, v11->flags_1, a6, a7);
				}
				else
				{
					Cl2DecodeFrm5(a3, a4, v8, v9, monster[v7].MType->flags_1, a6, a7, 1);
				}
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall DrawClippedObject(int x, int y, int a3, int a4, int pre_flag, int a6, int dir)
{
	int v7; // edi
	char v8; // al
	unsigned char v9; // dl
	int v10; // esi
	int v11; // ebx
	int v12; // edi
	int v13; // eax
	int v14; // ecx
	char *v15; // eax
	signed int v16; // ecx
	char *v17; // [esp-14h] [ebp-24h]
	int v18; // [esp-10h] [ebp-20h]
	int v19; // [esp-Ch] [ebp-1Ch]
	char v20; // [esp+Fh] [ebp-1h]

	v7 = y;
	v8 = dObject[x][y];
	if ( v8 <= 0 )
	{
		v9 = -1 - v8;
		v10 = (char)(-1 - v8);
		v20 = -1 - v8;
		if ( object[v10]._oPreFlag != pre_flag )
			return;
		dir = 8;
		v13 = object[v10]._ox - x;
		v14 = object[v10]._oy - v7;
		v12 = a3 + 32 * v13 - object[v10]._oAnimWidth2 - 32 * v14;
		v11 = a4 + 16 * (v14 + v13);
		a6 = 0;
	}
	else
	{
		v9 = v8 - 1;
		v10 = (char)(v8 - 1);
		v20 = v8 - 1;
		if ( object[v10]._oPreFlag != pre_flag )
			return;
		v11 = a4;
		v12 = a3 - object[v10]._oAnimWidth2;
	}
	if ( v9 < MAXOBJECTS )
	{
		v15 = (char *)object[v10]._oAnimData;
		if ( v15 )
		{
			v16 = object[v10]._oAnimFrame;
			if ( v16 >= 1 && *(_DWORD *)v15 <= 0x32u && v16 <= *(_DWORD *)v15 )
			{
				if ( v20 == pcursobj )
					CelDrawHdrClrHL(194, v12, v11, v15, v16, object[v10]._oAnimWidth, a6, dir);
				v19 = object[v10]._oAnimWidth;
				v18 = object[v10]._oAnimFrame;
				v17 = (char *)object[v10]._oAnimData;
				if ( object[v10]._oLight )
					Cel2DecodeHdrLight(v12, v11, v17, v18, v19, a6, dir);
				else
					Cel2DrawHdrOnly(v12, v11, v17, v18, v19, a6, dir);
			}
		}
	}
}
// 4B8CC1: using guessed type char pcursobj;

void __fastcall scrollrt_draw_clipped_e_flag(char *buffer, int x, int y, int a4, int a5)
{
	int v5; // eax
	int v6; // ebx
	int v7; // ecx
	int v8; // esi
	int v9; // eax
	int v10; // edi
	int v11; // eax
	int v12; // eax
	unsigned short *v13; // esi
	int v14; // eax
	int v15; // eax
	int v16; // eax
	int v17; // eax
	int v18; // [esp+Ch] [ebp-14h]
	int xa; // [esp+10h] [ebp-10h]
	int i; // [esp+14h] [ebp-Ch]
	unsigned char *a1; // [esp+18h] [ebp-8h]
	unsigned char *pbDst; // [esp+1Ch] [ebp-4h]

	xa = x;
	v18 = level_piece_id;
	v5 = y + 112 * x;
	a1 = (unsigned char *)buffer;
	v6 = cel_transparency_active;
	v7 = dPiece[0][v5];
	v8 = dTransVal[0][v5];
	v9 = dung_map[0][v5];
	v10 = light_table_index;
	level_piece_id = v7;
	v11 = (unsigned char)(nTransTable[v7] & TransList[v9]);
	light_table_index = v8;
	cel_transparency_active = v11;
	v12 = gendung_get_dpiece_num_from_coord(x, y);
	arch_draw_type = 1;
	v13 = (unsigned short *)((char *)dpiece_defs_map_1 + 32 * v12);
	v14 = *v13;
	level_cel_block = *v13;
	if ( v14 )
		drawLowerScreen(a1);
	v15 = v13[1];
	arch_draw_type = 2;
	level_cel_block = v15;
	if ( v15 )
		drawLowerScreen(a1 + 32);
	arch_draw_type = 0;
	pbDst = a1;
	v16 = 2;
	for ( i = 2; i < dword_5A5594; i += 2 )
	{
		pbDst -= 24576;
		level_cel_block = v13[v16];
		if ( level_cel_block )
			drawLowerScreen(pbDst);
		v17 = v13[i + 1];
		level_cel_block = v13[i + 1];
		if ( v17 )
			drawLowerScreen(pbDst + 32);
		v16 = i + 2;
	}
	scrollrt_draw_clipped_dungeon((char *)a1, xa, y, a4, a5, 0);
	light_table_index = v10;
	cel_transparency_active = v6;
	level_piece_id = v18;
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF20: using guessed type char arch_draw_type;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void __fastcall scrollrt_draw_lower_2(int x, int y, int sx, int sy, int a5, int a6, int some_flag)
{
	signed int v7; // ebx
	int v8; // edi
	int v9; // ecx
	int v10; // eax
	int v11; // eax
	int v12; // eax
	int v13; // ecx
	int v14; // ecx
	int v15; // eax
	unsigned char *v16; // ebx
	int v17; // eax
	int v18; // eax
	int v19; // ecx
	int v20; // eax
	int v21; // eax
	int v22; // eax
	int v23; // [esp+Ch] [ebp-14h]
	unsigned short *v24; // [esp+10h] [ebp-10h]
	int v25; // [esp+10h] [ebp-10h]
	int a1; // [esp+14h] [ebp-Ch]
	unsigned char *a1a; // [esp+14h] [ebp-Ch]
	unsigned char *a1b; // [esp+14h] [ebp-Ch]
	char *v29; // [esp+18h] [ebp-8h]
	signed int xa; // [esp+1Ch] [ebp-4h]
	int a6a; // [esp+28h] [ebp+8h]
	int a6b; // [esp+28h] [ebp+8h]
	int a6c; // [esp+28h] [ebp+8h]
	unsigned short *a5a; // [esp+30h] [ebp+10h]
	unsigned short *a5b; // [esp+30h] [ebp+10h]

	v7 = y;
	a1 = y;
	xa = x;
	v8 = sx;
	v29 = (char *)dpiece_defs_map_1 + 32 * gendung_get_dpiece_num_from_coord(x, y);
	if ( some_flag )
	{
		if ( v7 >= 0 && v7 < 112 && xa >= 0 && xa < 112 )
		{
			v9 = 112 * xa + v7;
			v10 = dPiece[0][v9];
			light_table_index = dTransVal[0][v9];
			level_piece_id = v10;
			if ( v10 )
			{
				a6a = 0;
				cel_transparency_active = (unsigned char)(nTransTable[v10] & TransList[dung_map[0][v9]]);
				a1a = (unsigned char *)gpBuffer + screen_y_times_768[sy] + v8 - 24544;
				if ( (dword_5A5594 >> 1) - 1 > 0 )
				{
					v24 = (unsigned short *)(v29 + 6);
					do
					{
						if ( a6 <= a6a )
						{
							v11 = *v24;
							level_cel_block = *v24;
							if ( v11 )
								drawLowerScreen(a1a);
						}
						a1a -= 24576;
						++a6a;
						v24 += 2;
					}
					while ( a6a < (dword_5A5594 >> 1) - 1 );
				}
				v12 = 2 * a6 + 2;
				if ( v12 < 8 )
					scrollrt_draw_clipped_dungeon_2(
						(char *)gpBuffer + screen_y_times_768[sy] - 12288 * v12 + v8,
						xa,
						v7,
						a6,
						2 * a6 + 2,
						v8,
						sy,
						0);
			}
		}
		++xa;
		--v7;
		v8 += 64;
		--a5;
		v29 += 32;
		a1 = v7;
	}
	v25 = a5;
	if ( a5 )
	{
		v13 = 112 * xa;
		v23 = 112 * xa;
		do
		{
			--v25;
			if ( v13 >= 12544 || v7 < 0 )
				break;
			if ( v7 < 112 && v13 >= 0 )
			{
				v14 = v7 + v13;
				v15 = dPiece[0][v14];
				light_table_index = dTransVal[0][v14];
				level_piece_id = v15;
				if ( v15 )
				{
					a6b = 0;
					cel_transparency_active = (unsigned char)(nTransTable[v15] & TransList[dung_map[0][v14]]);
					v16 = (unsigned char *)gpBuffer + screen_y_times_768[sy] + v8 - 24576;
					if ( (dword_5A5594 >> 1) - 1 > 0 )
					{
						a5a = (unsigned short *)(v29 + 6);
						do
						{
							if ( a6 <= a6b )
							{
								v17 = *(a5a - 1);
								level_cel_block = *(a5a - 1);
								if ( v17 )
									drawLowerScreen(v16);
								v18 = *a5a;
								level_cel_block = *a5a;
								if ( v18 )
									drawLowerScreen(v16 + 32);
							}
							++a6b;
							a5a += 2;
							v16 -= 24576;
						}
						while ( a6b < (dword_5A5594 >> 1) - 1 );
					}
					if ( 2 * a6 + 2 < 8 )
						scrollrt_draw_clipped_dungeon_2(
							(char *)gpBuffer + screen_y_times_768[sy] - ((3 * a6 + 3) << 13) + v8,
							xa,
							a1,
							a6,
							2 * a6 + 2,
							v8,
							sy,
							1);
					v7 = a1;
				}
			}
			++xa;
			v29 += 32;
			v13 = v23 + 112;
			--v7;
			v8 += 64;
			v23 += 112;
			a1 = v7;
		}
		while ( v25 );
	}
	if ( some_flag )
	{
		if ( (unsigned int)v7 < 0x70 && (unsigned int)xa < 0x70 )
		{
			v19 = 112 * xa + v7;
			v20 = dPiece[0][v19];
			light_table_index = dTransVal[0][v19];
			level_piece_id = v20;
			if ( v20 )
			{
				a6c = 0;
				cel_transparency_active = (unsigned char)(nTransTable[v20] & TransList[dung_map[0][v19]]);
				a1b = (unsigned char *)gpBuffer + screen_y_times_768[sy] + v8 - 24576;
				if ( (dword_5A5594 >> 1) - 1 > 0 )
				{
					a5b = (unsigned short *)(v29 + 4);
					do
					{
						if ( a6 <= a6c )
						{
							v21 = *a5b;
							level_cel_block = *a5b;
							if ( v21 )
								drawLowerScreen(a1b);
						}
						a1b -= 24576;
						++a6c;
						a5b += 2;
					}
					while ( a6c < (dword_5A5594 >> 1) - 1 );
				}
				v22 = 2 * a6 + 2;
				if ( v22 < 8 )
					scrollrt_draw_clipped_dungeon_2(
						(char *)gpBuffer + screen_y_times_768[sy] - 12288 * v22 + v8,
						xa,
						v7,
						a6,
						2 * a6 + 2,
						v8,
						sy,
						0);
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void __fastcall scrollrt_draw_clipped_dungeon_2(char *buffer, int x, int y, int a4, int a5, int sx, int sy, int me_flag)
{
	int v8; // eax
	char v9; // bl
	char v10; // cl
	char v11; // dl
	int *v12; // eax
	int v13; // edi
	DeadStruct *v14; // eax
	int *v15; // esi
	int v16; // ecx
	int v17; // edx
	char v18; // bl
	ItemStruct *v19; // esi
	char *v20; // eax
	signed int v21; // ebx
	int v22; // ebx
	unsigned int v23; // ecx
	PlayerStruct *v24; // esi
	int v25; // esi
	int v26; // eax
	MonsterStruct *v27; // esi
	CMonster *v28; // ecx
	int v29; // ebx
	int v30; // edi
	unsigned int v31; // ecx
	PlayerStruct *v32; // esi
	int v33; // esi
	int v34; // eax
	MonsterStruct *v35; // esi
	CMonster *v36; // ecx
	int v37; // ebx
	int v38; // edi
	ItemStruct *v39; // esi
	char *v40; // eax
	int v41; // ecx
	int v42; // edi
	int v43; // [esp+Ch] [ebp-18h]
	int v44; // [esp+10h] [ebp-14h]
	char *dst_buf; // [esp+14h] [ebp-10h]
	int a1; // [esp+18h] [ebp-Ch]
	char v47; // [esp+1Dh] [ebp-7h]
	char v48; // [esp+1Eh] [ebp-6h]
	char v49; // [esp+1Fh] [ebp-5h]
	char v50; // [esp+20h] [ebp-4h]
	char v51; // [esp+21h] [ebp-3h]
	char v52; // [esp+22h] [ebp-2h]
	char v53; // [esp+23h] [ebp-1h]

	a1 = x;
	dst_buf = buffer;
	v8 = 112 * x + y;
	v9 = dDead[0][v8];
	v53 = dFlags[0][v8];
	v50 = dObject[0][v8];
	v52 = dItem[0][v8];
	v10 = dPlayer[0][v8 - 1];
	v51 = dPlayer[0][v8];
	v49 = dArch[0][v8];
	v11 = dung_map[0][v8];
	v12 = (int *)((char *)dMonster + 4 * v8);
	v47 = v11;
	v48 = v10;
	v43 = *v12;
	v44 = *(v12 - 1);
	if ( visiondebug && v53 & DFLAG_LIT )
		Cel2DecodeHdrOnly(dst_buf, (char *)pSquareCel, 1, 64, a5, 8);
	if ( MissilePreFlag && v53 & DFLAG_MISSILE )
	{
		v13 = sx;
		DrawClippedMissile(a1, y, sx, sy, a5, 8, 1);
	}
	else
	{
		v13 = sx;
	}
	if ( light_table_index < lightmax )
	{
		if ( v9 )
		{
			v14 = &dead[(v9 & 0x1F) - 1];
			v15 = (int *)v14->_deadData[(v9 >> 5) & 7];
			v16 = v13 - v14->_deadWidth2;
			if ( v15 )
			{
				v17 = v14->_deadFrame;
				if ( v17 >= 1 && (unsigned int)*v15 <= 0x32 && v17 <= *v15 )
				{
					v18 = v14->_deadtrans;
					if ( v18 )
						Cl2DecodeFrm5(v16, sy, (char *)v15, v17, v14->_deadWidth, a5, 8, v18);
					else
						Cl2DecodeFrm6(v16, sy, (char *)v15, v17, v14->_deadWidth, a5, 8);
				}
			}
		}
		if ( v50 )
			DrawClippedObject(a1, y, v13, sy, 1, a5, 8);
	}
	if ( v52 )
	{
		v19 = &item[v52-1];
		if ( !v19->_iPostDraw && (unsigned char)v52 <= MAXITEMS )
		{
			v20 = (char *)v19->_iAnimData;
			if ( v20 )
			{
				v21 = v19->_iAnimFrame;
				if ( v21 >= 1 && *(_DWORD *)v20 <= 0x32u && v21 <= *(_DWORD *)v20 )
				{
					v22 = v13 - v19->_iAnimWidth2;
					if ( v52 - 1 == pcursitem )
						CelDrawHdrClrHL(181, v22, sy, v20, v19->_iAnimFrame, v19->_iAnimWidth, a5, 8);
					Cel2DecodeHdrLight(v22, sy, (char *)v19->_iAnimData, v19->_iAnimFrame, v19->_iAnimWidth, a5, 8);
				}
			}
		}
	}
	if ( v53 & DFLAG_PLAYER )
	{
		v23 = -1 - v48;
		if ( v23 < 4 )
		{
			v24 = &plr[v23];
			DrawClippedPlayer(
				v23,
				a1,
				y - 1,
				v13 + v24->_pxoff - v24->_pAnimWidth2,
				sy + v24->_pyoff,
				v24->_pAnimData,
				v24->_pAnimFrame,
				v24->_pAnimWidth,
				a5,
				8);
			if ( me_flag )
			{
				v25 = v24->_peflag;
				if ( v25 )
				{
					if ( v25 == 2 )
						scrollrt_draw_clipped_e_flag_2(dst_buf - 12384, a1 - 2, y + 1, a4, a5, v13 - 96, sy - 16);
					scrollrt_draw_clipped_e_flag_2(dst_buf - 64, a1 - 1, y + 1, a4, a5, v13 - 64, sy);
				}
			}
		}
	}
	if ( v53 & DFLAG_MONSTER && (v53 & DFLAG_LIT || plr[myplr]._pInfraFlag) && v44 < 0 )
	{
		v26 = -1 - v44;
		draw_monster_num = -1 - v44;
		if ( (unsigned int)(-1 - v44) < MAXMONSTERS )
		{
			v27 = &monster[v26];
			if ( !(v27->_mFlags & 1) )
			{
				v28 = v27->MType;
				if ( v28 )
				{
					v29 = sy + v27->_myoff;
					v30 = sx + v27->_mxoff - v28->flags_2;
					if ( v26 == pcursmonst )
					{
						Cl2DecodeClrHL(233, v30, v29, (char *)v27->_mAnimData, v27->_mAnimFrame, v28->flags_1, a5, 8);
						v26 = draw_monster_num;
					}
					DrawClippedMonster(a1, y, v30, v29, v26, a5, 8);
					if ( me_flag && !v27->_meflag )
						scrollrt_draw_clipped_e_flag_2(dst_buf - 64, a1 - 1, y + 1, a4, a5, sx - 64, sy);
					v13 = sx;
				}
			}
		}
	}
	if ( v53 & DFLAG_DEAD_PLAYER )
		DrawDeadPlayer(a1, y, v13, sy, a5, 8, 1);
	if ( v51 > 0 )
	{
		v31 = v51 - 1;
		if ( v31 < 4 )
		{
			v32 = &plr[v31];
			DrawClippedPlayer(
				v31,
				a1,
				y,
				v13 + v32->_pxoff - v32->_pAnimWidth2,
				sy + v32->_pyoff,
				v32->_pAnimData,
				v32->_pAnimFrame,
				v32->_pAnimWidth,
				a5,
				8);
			if ( me_flag )
			{
				v33 = v32->_peflag;
				if ( v33 )
				{
					if ( v33 == 2 )
						scrollrt_draw_clipped_e_flag_2(dst_buf - 12384, a1 - 2, y + 1, a4, a5, v13 - 96, sy - 16);
					scrollrt_draw_clipped_e_flag_2(dst_buf - 64, a1 - 1, y + 1, a4, a5, v13 - 64, sy);
				}
			}
		}
	}
	if ( v43 > 0 && (v53 & DFLAG_LIT || plr[myplr]._pInfraFlag) )
	{
		v34 = v43 - 1;
		draw_monster_num = v43 - 1;
		if ( (unsigned int)(v43 - 1) < MAXMONSTERS )
		{
			v35 = &monster[v34];
			if ( !(v35->_mFlags & 1) )
			{
				v36 = v35->MType;
				if ( v36 )
				{
					v37 = sy + v35->_myoff;
					v38 = sx + v35->_mxoff - v36->flags_2;
					if ( v34 == pcursmonst )
					{
						Cl2DecodeClrHL(233, v38, v37, (char *)v35->_mAnimData, v35->_mAnimFrame, v36->flags_1, a5, 8);
						v34 = draw_monster_num;
					}
					DrawClippedMonster(a1, y, v38, v37, v34, a5, 8);
					if ( me_flag && !v35->_meflag )
						scrollrt_draw_clipped_e_flag_2(dst_buf - 64, a1 - 1, y + 1, a4, a5, sx - 64, sy);
					v13 = sx;
				}
			}
		}
	}
	if ( v53 & DFLAG_MISSILE )
		DrawClippedMissile(a1, y, v13, sy, a5, 8, 0);
	if ( v50 && light_table_index < lightmax )
		DrawClippedObject(a1, y, v13, sy, 0, a5, 8);
	if ( v52 )
	{
		v39 = &item[v52-1];
		if ( v39->_iPostDraw )
		{
			if ( (unsigned char)v52 <= MAXITEMS )
			{
				v40 = (char *)v39->_iAnimData;
				if ( v40 )
				{
					v41 = v39->_iAnimFrame;
					if ( v41 >= 1 && *(_DWORD *)v40 <= 0x32u && v41 <= *(_DWORD *)v40 )
					{
						v42 = v13 - v39->_iAnimWidth2;
						if ( v52 - 1 == pcursitem )
							CelDrawHdrClrHL(181, v42, sy, v40, v41, v39->_iAnimWidth, a5, 8);
						Cel2DecodeHdrLight(
							v42,
							sy,
							(char *)v39->_iAnimData,
							v39->_iAnimFrame,
							v39->_iAnimWidth,
							a5,
							8);
					}
				}
			}
		}
	}
	if ( v49 )
	{
		cel_transparency_active = (unsigned char)TransList[v47];
		Cel2DecodeLightTrans(dst_buf, (char *)level_special_cel, v49, 64, a5, 8);
	}
}
// 4B8CC0: using guessed type char pcursitem;
// 525720: using guessed type int visiondebug;
// 642A14: using guessed type char lightmax;
// 64CCD4: using guessed type int MissilePreFlag;
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;
// 69EFA4: using guessed type int draw_monster_num;

void __fastcall scrollrt_draw_clipped_e_flag_2(char *buffer, int x, int y, int a4, signed int a5, int sx, int sy)
{
	int v7; // eax
	int v8; // ecx
	int v9; // esi
	int v10; // eax
	unsigned char *v11; // edi
	int v12; // eax
	unsigned short *v13; // esi
	int v14; // eax
	int v15; // eax
	int v16; // eax
	int v17; // eax
	unsigned char *v18; // edi
	int v19; // eax
	int v20; // [esp+Ch] [ebp-14h]
	int v21; // [esp+10h] [ebp-10h]
	int v22; // [esp+14h] [ebp-Ch]
	char *a1; // [esp+18h] [ebp-8h]
	int xa; // [esp+1Ch] [ebp-4h]

	xa = x;
	v22 = light_table_index;
	v21 = cel_transparency_active;
	v20 = level_piece_id;
	v7 = y + 112 * x;
	a1 = buffer;
	v8 = dPiece[0][v7];
	v9 = dTransVal[0][v7];
	v10 = dung_map[0][v7];
	level_piece_id = v8;
	v11 = (unsigned char *)&a1[24576 * a4];
	v12 = (unsigned char)(nTransTable[v8] & TransList[v10]);
	light_table_index = v9;
	cel_transparency_active = v12;
	v13 = (unsigned short *)((char *)dpiece_defs_map_1 + 32 * gendung_get_dpiece_num_from_coord(x, y));
	if ( !a4 )
	{
		v14 = v13[2];
		level_cel_block = v13[2];
		if ( v14 )
			drawLowerScreen(v11);
		v15 = v13[3];
		level_cel_block = v13[3];
		if ( v15 )
			drawLowerScreen(v11 + 32);
		goto LABEL_10;
	}
	if ( a4 == 1 )
	{
LABEL_10:
		v11 -= 24576;
		level_cel_block = v13[4];
		if ( level_cel_block )
			drawLowerScreen(v11);
		v16 = v13[5];
		level_cel_block = v13[5];
		if ( v16 )
			drawLowerScreen(v11 + 32);
		goto LABEL_14;
	}
	if ( a4 != 2 )
	{
		if ( a4 != 3 )
			goto LABEL_22;
		goto LABEL_18;
	}
LABEL_14:
	v11 -= 24576;
	level_cel_block = v13[6];
	if ( level_cel_block )
		drawLowerScreen(v11);
	v17 = v13[7];
	level_cel_block = v13[7];
	if ( v17 )
		drawLowerScreen(v11 + 32);
LABEL_18:
	v18 = v11 - 24576;
	level_cel_block = v13[8];
	if ( level_cel_block )
		drawLowerScreen(v18);
	v19 = v13[9];
	level_cel_block = v13[9];
	if ( v19 )
		drawLowerScreen(v18 + 32);
LABEL_22:
	if ( a5 < 8 )
		scrollrt_draw_clipped_dungeon_2(a1, xa, y, a4, a5, sx, sy, 0);
	light_table_index = v22;
	cel_transparency_active = v21;
	level_piece_id = v20;
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void __fastcall scrollrt_draw_upper(int x, int y, int sx, int sy, int a5, int a6, int some_flag)
{
	int v7; // edi
	int v8; // esi
	unsigned short *v9; // ebx
	int v10; // ecx
	int v11; // eax
	char *v12; // edx
	int v13; // edi
	int v14; // eax
	unsigned char *v15; // edi
	int v16; // eax
	unsigned char *v17; // edi
	int v18; // eax
	int v19; // eax
	int v20; // esi
	int v21; // eax
	int v22; // ecx
	int v23; // ecx
	int v24; // eax
	int v25; // esi
	unsigned char *v26; // esi
	int v27; // eax
	int v28; // eax
	int v29; // eax
	bool v30; // zf
	int v31; // ecx
	int v32; // eax
	unsigned char *v33; // esi
	int v34; // eax
	unsigned char *v35; // esi
	int v36; // eax
	unsigned char *v37; // esi
	int v38; // eax
	int v39; // eax
	int v40; // [esp+Ch] [ebp-14h]
	int v41; // [esp+10h] [ebp-10h]
	int a5a; // [esp+14h] [ebp-Ch]
	int ya; // [esp+18h] [ebp-8h]
	signed int xa; // [esp+1Ch] [ebp-4h]
	int i; // [esp+30h] [ebp+10h]

	v7 = y;
	v8 = x;
	ya = y;
	xa = x;
	v9 = (unsigned short *)((char *)dpiece_defs_map_1 + 32 * gendung_get_dpiece_num_from_coord(x, y));
	a5a = 2 * a6 + 2;
	if ( a5a > 8 )
		a5a = 8;
	if ( some_flag )
	{
		if ( v7 >= 0 && v7 < 112 && v8 >= 0 && v8 < 112 )
		{
			v10 = 112 * v8 + v7;
			v11 = dPiece[0][v10];
			light_table_index = dTransVal[0][v10];
			level_piece_id = v11;
			if ( v11 )
			{
				cel_transparency_active = (unsigned char)(nTransTable[v11] & TransList[dung_map[0][v10]]);
				v12 = (char *)gpBuffer + screen_y_times_768[sy];
				v13 = (int)&v12[sx + 32];
				if ( a6 >= 0 )
				{
					v14 = v9[1];
					level_cel_block = v9[1];
					if ( v14 )
					{
						arch_draw_type = 2;
						drawUpperScreen((unsigned char *)&v12[sx + 32]);
						arch_draw_type = 0;
					}
				}
				v15 = (unsigned char *)(v13 - 24576);
				if ( a6 >= 1 )
				{
					v16 = v9[3];
					level_cel_block = v9[3];
					if ( v16 )
						drawUpperScreen(v15);
				}
				v17 = v15 - 24576;
				if ( a6 >= 2 )
				{
					v18 = v9[5];
					level_cel_block = v9[5];
					if ( v18 )
						drawUpperScreen(v17);
				}
				if ( a6 >= 3 )
				{
					v19 = v9[7];
					level_cel_block = v9[7];
					if ( v19 )
						drawUpperScreen(v17 - 24576);
				}
				v7 = ya;
				scrollrt_draw_dungeon((char *)gpBuffer + screen_y_times_768[sy] + sx, xa, ya, a6, a5a, sx, sy, 0);
			}
			else
			{
				world_draw_black_tile((unsigned char *)gpBuffer + screen_y_times_768[sy] + sx);
			}
		}
		sx += 64;
		v8 = xa + 1;
		--v7;
		--a5;
		++xa;
		ya = v7;
		v9 += 16;
	}
	if ( a5 > 0 )
	{
		v20 = 112 * v8;
		v41 = v20;
		v40 = a5;
		do
		{
			if ( v7 >= 0 && v7 < 112 && v20 >= 0 && v20 < 12544 )
			{
				v21 = dPiece[0][v20 + v7];
				light_table_index = dTransVal[0][v20 + v7];
				level_piece_id = v21;
				if ( v21 )
				{
					v22 = dung_map[0][v20 + v7];
					arch_draw_type = 1;
					v23 = (unsigned char)(nTransTable[v21] & TransList[v22]);
					v24 = *v9;
					v25 = screen_y_times_768[sy];
					cel_transparency_active = v23;
					level_cel_block = v24;
					v26 = (unsigned char *)gpBuffer + v25 + sx;
					if ( v24 )
						drawUpperScreen(v26);
					v27 = v9[1];
					arch_draw_type = 2;
					level_cel_block = v27;
					if ( v27 )
						drawUpperScreen(v26 + 32);
					arch_draw_type = 0;
					for ( i = 1; i < (dword_5A5594 >> 1) - 1; ++i )
					{
						v26 -= 24576;
						if ( a6 >= i )
						{
							v28 = v9[2 * i];
							level_cel_block = v9[2 * i];
							if ( v28 )
								drawUpperScreen(v26);
							v29 = v9[2 * i + 1];
							level_cel_block = v9[2 * i + 1];
							if ( v29 )
								drawUpperScreen(v26 + 32);
						}
					}
					scrollrt_draw_dungeon((char *)gpBuffer + screen_y_times_768[sy] + sx, xa, ya, a6, a5a, sx, sy, 1);
					v7 = ya;
					v20 = v41;
				}
				else
				{
					world_draw_black_tile((unsigned char *)gpBuffer + screen_y_times_768[sy] + sx);
				}
			}
			++xa;
			sx += 64;
			v20 += 112;
			--v7;
			v9 += 16;
			v30 = v40-- == 1;
			v41 = v20;
			ya = v7;
		}
		while ( !v30 );
	}
	if ( some_flag && v7 >= 0 && v7 < 112 && xa >= 0 && xa < 112 )
	{
		v31 = 112 * xa + v7;
		v32 = dPiece[0][v31];
		light_table_index = dTransVal[0][v31];
		level_piece_id = v32;
		if ( v32 )
		{
			arch_draw_type = 1;
			cel_transparency_active = (unsigned char)(nTransTable[v32] & TransList[dung_map[0][v31]]);
			v33 = (unsigned char *)gpBuffer + screen_y_times_768[sy] + sx;
			if ( a6 >= 0 )
			{
				v34 = *v9;
				level_cel_block = *v9;
				if ( v34 )
					drawUpperScreen(v33);
			}
			arch_draw_type = 0;
			v35 = v33 - 24576;
			if ( a6 >= 1 )
			{
				v36 = v9[2];
				level_cel_block = v9[2];
				if ( v36 )
					drawUpperScreen(v35);
			}
			v37 = v35 - 24576;
			if ( a6 >= 2 )
			{
				v38 = v9[4];
				level_cel_block = v9[4];
				if ( v38 )
					drawUpperScreen(v37);
			}
			if ( a6 >= 3 )
			{
				v39 = v9[6];
				level_cel_block = v9[6];
				if ( v39 )
					drawUpperScreen(v37 - 24576);
			}
			scrollrt_draw_dungeon((char *)gpBuffer + screen_y_times_768[sy] + sx, xa, ya, a6, a5a, sx, sy, 0);
		}
		else
		{
			world_draw_black_tile((unsigned char *)gpBuffer + screen_y_times_768[sy] + sx);
		}
	}
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF20: using guessed type char arch_draw_type;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void __fastcall scrollrt_draw_dungeon(char *buffer, int x, int y, int a4, int a5, int sx, int sy, int me_flag)
{
	int v8; // eax
	char v9; // bl
	char v10; // cl
	char v11; // dl
	int *v12; // eax
	DeadStruct *v13; // eax
	int *v14; // esi
	int v15; // ecx
	int v16; // edx
	char v17; // bl
	ItemStruct *v18; // esi
	char *v19; // eax
	signed int v20; // ebx
	int v21; // ebx
	unsigned int v22; // ecx
	PlayerStruct *v23; // esi
	int v24; // esi
	int v25; // eax
	MonsterStruct *v26; // esi
	CMonster *v27; // ecx
	int v28; // ebx
	int v29; // edi
	unsigned int v30; // ecx
	PlayerStruct *v31; // esi
	int v32; // esi
	int v33; // eax
	MonsterStruct *v34; // esi
	CMonster *v35; // ecx
	int v36; // ebx
	int v37; // edi
	ItemStruct *v38; // esi
	char *v39; // ecx
	signed int v40; // ebx
	int v41; // ebx
	int v42; // [esp+Ch] [ebp-18h]
	int v43; // [esp+10h] [ebp-14h]
	char *dst_buf; // [esp+14h] [ebp-10h]
	int xa; // [esp+18h] [ebp-Ch]
	char v46; // [esp+1Dh] [ebp-7h]
	char v47; // [esp+1Eh] [ebp-6h]
	char v48; // [esp+1Fh] [ebp-5h]
	char v49; // [esp+20h] [ebp-4h]
	char v50; // [esp+21h] [ebp-3h]
	char v51; // [esp+22h] [ebp-2h]
	char v52; // [esp+23h] [ebp-1h]

	xa = x;
	dst_buf = buffer;
	v8 = 112 * x + y;
	v9 = dDead[0][v8];
	v52 = dFlags[0][v8];
	v49 = dObject[0][v8];
	v51 = dItem[0][v8];
	v10 = dPlayer[0][v8 - 1];
	v50 = dPlayer[0][v8];
	v48 = dArch[0][v8];
	v11 = dung_map[0][v8];
	v12 = (int *)((char *)dMonster + 4 * v8);
	v46 = v11;
	v47 = v10;
	v42 = *v12;
	v43 = *(v12 - 1);
	if ( visiondebug && v52 & DFLAG_LIT )
		CelDecodeHdrOnly(dst_buf, (char *)pSquareCel, 1, 64, 0, a5);
	if ( MissilePreFlag && v52 & DFLAG_MISSILE )
		DrawMissile(xa, y, sx, sy, 0, a5, 1);
	if ( light_table_index < lightmax )
	{
		if ( v9 )
		{
			v13 = &dead[(v9 & 0x1F) - 1];
			v14 = (int *)v13->_deadData[(v9 >> 5) & 7];
			v15 = sx - v13->_deadWidth2;
			if ( v14 )
			{
				v16 = v13->_deadFrame;
				if ( v16 >= 1 && (unsigned int)*v14 <= 0x32 && v16 <= *v14 )
				{
					v17 = v13->_deadtrans;
					if ( v17 )
						Cl2DecodeFrm3(v15, sy, (char *)v14, v16, v13->_deadWidth, 0, a5, v17);
					else
						Cl2DecodeLightTbl(v15, sy, (char *)v14, v16, v13->_deadWidth, 0, a5);
				}
			}
		}
		if ( v49 )
			DrawObject(xa, y, sx, sy, 1, 0, a5);
	}
	if ( v51 )
	{
		v18 = &item[v51-1];
		if ( !v18->_iPostDraw && (unsigned char)v51 <= MAXITEMS )
		{
			v19 = (char *)v18->_iAnimData;
			if ( v19 )
			{
				v20 = v18->_iAnimFrame;
				if ( v20 >= 1 && *(_DWORD *)v19 <= 0x32u && v20 <= *(_DWORD *)v19 )
				{
					v21 = sx - v18->_iAnimWidth2;
					if ( v51 - 1 == pcursitem )
						CelDecodeClr(181, v21, sy, v19, v18->_iAnimFrame, v18->_iAnimWidth, 0, a5);
					CelDecodeHdrLightOnly(v21, sy, (char *)v18->_iAnimData, v18->_iAnimFrame, v18->_iAnimWidth, 0, a5);
				}
			}
		}
	}
	if ( v52 & DFLAG_PLAYER )
	{
		v22 = -1 - v47;
		if ( v22 < 4 )
		{
			v23 = &plr[v22];
			DrawPlayer(
				v22,
				xa,
				y - 1,
				sx + v23->_pxoff - v23->_pAnimWidth2,
				sy + v23->_pyoff,
				v23->_pAnimData,
				v23->_pAnimFrame,
				v23->_pAnimWidth,
				0,
				a5);
			if ( me_flag )
			{
				v24 = v23->_peflag;
				if ( v24 )
				{
					if ( v24 == 2 )
						scrollrt_draw_e_flag(dst_buf - 12384, xa - 2, y + 1, a4, a5, sx - 96, sy - 16);
					scrollrt_draw_e_flag(dst_buf - 64, xa - 1, y + 1, a4, a5, sx - 64, sy);
				}
			}
		}
	}
	if ( v52 & DFLAG_MONSTER && (v52 & DFLAG_LIT || plr[myplr]._pInfraFlag) && v43 < 0 )
	{
		v25 = -1 - v43;
		draw_monster_num = -1 - v43;
		if ( (unsigned int)(-1 - v43) < MAXMONSTERS )
		{
			v26 = &monster[v25];
			if ( !(v26->_mFlags & 1) )
			{
				v27 = v26->MType;
				if ( v27 )
				{
					v28 = sy + v26->_myoff;
					v29 = sx + v26->_mxoff - v27->flags_2;
					if ( v25 == pcursmonst )
					{
						Cl2DecodeFrm2(233, v29, v28, (char *)v26->_mAnimData, v26->_mAnimFrame, v27->flags_1, 0, a5);
						v25 = draw_monster_num;
					}
					DrawMonster(xa, y, v29, v28, v25, 0, a5);
					if ( me_flag && !v26->_meflag )
						scrollrt_draw_e_flag(dst_buf - 64, xa - 1, y + 1, a4, a5, sx - 64, sy);
				}
			}
		}
	}
	if ( v52 & DFLAG_DEAD_PLAYER )
		DrawDeadPlayer(xa, y, sx, sy, 0, a5, 0);
	if ( v50 > 0 )
	{
		v30 = v50 - 1;
		if ( v30 < 4 )
		{
			v31 = &plr[v30];
			DrawPlayer(
				v30,
				xa,
				y,
				sx + v31->_pxoff - v31->_pAnimWidth2,
				sy + v31->_pyoff,
				v31->_pAnimData,
				v31->_pAnimFrame,
				v31->_pAnimWidth,
				0,
				a5);
			if ( me_flag )
			{
				v32 = v31->_peflag;
				if ( v32 )
				{
					if ( v32 == 2 )
						scrollrt_draw_e_flag(dst_buf - 12384, xa - 2, y + 1, a4, a5, sx - 96, sy - 16);
					scrollrt_draw_e_flag(dst_buf - 64, xa - 1, y + 1, a4, a5, sx - 64, sy);
				}
			}
		}
	}
	if ( v42 > 0 && (v52 & DFLAG_LIT || plr[myplr]._pInfraFlag) )
	{
		v33 = v42 - 1;
		draw_monster_num = v42 - 1;
		if ( (unsigned int)(v42 - 1) < MAXMONSTERS )
		{
			v34 = &monster[v33];
			if ( !(v34->_mFlags & 1) )
			{
				v35 = v34->MType;
				if ( v35 )
				{
					v36 = sy + v34->_myoff;
					v37 = sx + v34->_mxoff - v35->flags_2;
					if ( v33 == pcursmonst )
					{
						Cl2DecodeFrm2(233, v37, v36, (char *)v34->_mAnimData, v34->_mAnimFrame, v35->flags_1, 0, a5);
						v33 = draw_monster_num;
					}
					DrawMonster(xa, y, v37, v36, v33, 0, a5);
					if ( me_flag && !v34->_meflag )
						scrollrt_draw_e_flag(dst_buf - 64, xa - 1, y + 1, a4, a5, sx - 64, sy);
				}
			}
		}
	}
	if ( v52 & DFLAG_MISSILE )
		DrawMissile(xa, y, sx, sy, 0, a5, 0);
	if ( v49 && light_table_index < lightmax )
		DrawObject(xa, y, sx, sy, 0, 0, a5);
	if ( v51 )
	{
		v38 = &item[v51-1];
		if ( v38->_iPostDraw )
		{
			if ( (unsigned char)v51 <= MAXITEMS )
			{
				v39 = (char *)v38->_iAnimData;
				if ( v39 )
				{
					v40 = v38->_iAnimFrame;
					if ( v40 >= 1 && *(_DWORD *)v39 <= 0x32u && v40 <= *(_DWORD *)v39 )
					{
						v41 = sx - v38->_iAnimWidth2;
						if ( v51 - 1 == pcursitem )
							CelDecodeClr(181, v41, sy, v39, v38->_iAnimFrame, v38->_iAnimWidth, 0, a5);
						CelDecodeHdrLightOnly(
							v41,
							sy,
							(char *)v38->_iAnimData,
							v38->_iAnimFrame,
							v38->_iAnimWidth,
							0,
							a5);
					}
				}
			}
		}
	}
	if ( v48 )
	{
		cel_transparency_active = (unsigned char)TransList[v46];
		CelDecodeHdrLightTrans(dst_buf, (char *)level_special_cel, v48, 64, 0, a5);
	}
}
// 4B8CC0: using guessed type char pcursitem;
// 525720: using guessed type int visiondebug;
// 642A14: using guessed type char lightmax;
// 64CCD4: using guessed type int MissilePreFlag;
// 69BEF8: using guessed type int light_table_index;
// 69CF94: using guessed type int cel_transparency_active;
// 69EFA4: using guessed type int draw_monster_num;

void __fastcall DrawMonster(int x, int y, int a3, int a4, int mon_id, int a6, int a7)
{
	int v7; // eax
	char *v8; // esi
	signed int v9; // ebx
	char v10; // cl
	CMonster *v11; // eax
	char mon_ida; // [esp+1Ch] [ebp+10h]

	if ( (unsigned int)mon_id < MAXMONSTERS )
	{
		v7 = mon_id;
		v8 = (char *)monster[mon_id]._mAnimData;
		if ( v8 )
		{
			v9 = monster[v7]._mAnimFrame;
			if ( v9 >= 1 && (unsigned int)*v8 <= 0x32 && v9 <= *v8 )
			{
				if ( dFlags[x][y] & DFLAG_LIT )
				{
					v10 = 0;
					mon_ida = 0;
					if ( monster[v7]._uniqtype )
					{
						v10 = monster[v7]._uniqtrans + 4;
						mon_ida = monster[v7]._uniqtrans + 4;
					}
					if ( monster[v7]._mmode == MM_STONE )
					{
						v10 = 2;
						mon_ida = 2;
					}
					if ( plr[myplr]._pInfraFlag && light_table_index > 8 )
					{
						v10 = 1;
						mon_ida = 1;
					}
					v11 = monster[v7].MType;
					if ( v10 )
						Cl2DecodeFrm3(a3, a4, v8, v9, v11->flags_1, a6, a7, mon_ida);
					else
						Cl2DecodeLightTbl(a3, a4, v8, v9, v11->flags_1, a6, a7);
				}
				else
				{
					Cl2DecodeFrm3(a3, a4, v8, v9, monster[v7].MType->flags_1, a6, a7, 1);
				}
			}
		}
	}
}
// 69BEF8: using guessed type int light_table_index;

void __fastcall DrawObject(int x, int y, int a3, int a4, int pre_flag, int a6, int dir)
{
	int v7; // edi
	char v8; // al
	unsigned char v9; // dl
	int v10; // esi
	int v11; // ebx
	int v12; // edi
	int v13; // eax
	int v14; // ecx
	char *v15; // eax
	signed int v16; // ecx
	char *v17; // eax
	char v18; // [esp+Fh] [ebp-1h]

	v7 = y;
	v8 = dObject[x][y];
	if ( v8 <= 0 )
	{
		v9 = -1 - v8;
		v10 = (char)(-1 - v8);
		v18 = -1 - v8;
		if ( object[v10]._oPreFlag != pre_flag )
			return;
		dir = 8;
		v13 = object[v10]._ox - x;
		v14 = object[v10]._oy - v7;
		v12 = a3 + 32 * v13 - object[v10]._oAnimWidth2 - 32 * v14;
		v11 = a4 + 16 * (v14 + v13);
		a6 = 0;
	}
	else
	{
		v9 = v8 - 1;
		v10 = (char)(v8 - 1);
		v18 = v8 - 1;
		if ( object[v10]._oPreFlag != pre_flag )
			return;
		v11 = a4;
		v12 = a3 - object[v10]._oAnimWidth2;
	}
	if ( v9 < MAXOBJECTS )
	{
		v15 = (char *)object[v10]._oAnimData;
		if ( v15 )
		{
			v16 = object[v10]._oAnimFrame;
			if ( v16 >= 1 && *(_DWORD *)v15 <= 0x32u && v16 <= *(_DWORD *)v15 )
			{
				if ( v18 == pcursobj )
					CelDecodeClr(194, v12, v11, v15, v16, object[v10]._oAnimWidth, a6, dir);
				if ( object[v10]._oLight )
				{
					CelDecodeHdrLightOnly(
						v12,
						v11,
						(char *)object[v10]._oAnimData,
						object[v10]._oAnimFrame,
						object[v10]._oAnimWidth,
						a6,
						dir);
				}
				else
				{
					v17 = (char *)object[v10]._oAnimData;
					if ( v17 )
						CelDrawHdrOnly(v12, v11, v17, object[v10]._oAnimFrame, object[v10]._oAnimWidth, a6, dir);
				}
			}
		}
	}
}
// 4B8CC1: using guessed type char pcursobj;

void __fastcall scrollrt_draw_e_flag(char *buffer, int x, int y, int a4, int a5, int sx, int sy)
{
	int v7; // eax
	char *v8; // esi
	int v9; // ecx
	int v10; // ebx
	int v11; // edx
	int v12; // eax
	int v13; // eax
	int v14; // ecx
	int v15; // edi
	int v16; // eax
	unsigned short *v17; // esi
	int v18; // eax
	int v19; // eax
	int v20; // eax
	int v21; // eax
	int v22; // [esp+Ch] [ebp-14h]
	int xa; // [esp+10h] [ebp-10h]
	unsigned char *a1; // [esp+14h] [ebp-Ch]
	unsigned char *v25; // [esp+18h] [ebp-8h]
	int i; // [esp+1Ch] [ebp-4h]

	xa = x;
	v22 = level_piece_id;
	v7 = 112 * x + y;
	v8 = buffer;
	v9 = dPiece[0][v7];
	v10 = cel_transparency_active;
	v11 = dTransVal[0][v7];
	v12 = dung_map[0][v7];
	level_piece_id = v9;
	v13 = (unsigned char)TransList[v12];
	v14 = (unsigned char)nTransTable[v9];
	v15 = light_table_index;
	light_table_index = v11;
	a1 = (unsigned char *)v8;
	v25 = (unsigned char *)v8;
	cel_transparency_active = v14 & v13;
	v16 = gendung_get_dpiece_num_from_coord(xa, y);
	arch_draw_type = 1;
	v17 = (unsigned short *)((char *)dpiece_defs_map_1 + 32 * v16);
	v18 = *v17;
	level_cel_block = *v17;
	if ( v18 )
		drawUpperScreen(a1);
	v19 = v17[1];
	arch_draw_type = 2;
	level_cel_block = v19;
	if ( v19 )
		drawUpperScreen(a1 + 32);
	arch_draw_type = 0;
	for ( i = 1; i < (dword_5A5594 >> 1) - 1; ++i )
	{
		v25 -= 24576;
		if ( a4 >= i )
		{
			v20 = v17[2 * i];
			level_cel_block = v17[2 * i];
			if ( v20 )
				drawUpperScreen(v25);
			v21 = v17[2 * i + 1];
			level_cel_block = v17[2 * i + 1];
			if ( v21 )
				drawUpperScreen(v25 + 32);
		}
	}
	scrollrt_draw_dungeon((char *)a1, xa, y, a4, a5, sx, sy, 0);
	light_table_index = v15;
	cel_transparency_active = v10;
	level_piece_id = v22;
}
// 69BEF8: using guessed type int light_table_index;
// 69CF14: using guessed type int level_cel_block;
// 69CF20: using guessed type char arch_draw_type;
// 69CF94: using guessed type int cel_transparency_active;
// 69CF98: using guessed type int level_piece_id;

void __fastcall DrawZoom(int x, int y)
{
	int v2; // edi
	int v3; // ebx
	int v4; // esi
	int v5; // esi
	int v6; // edi
	int v7; // esi
	int v8; // edi
	int v9; // esi
	int v10; // edi
	_WORD *v11; // edi
	char *v12; // esi
	char *v13; // ebx
	signed int v14; // edx
	signed int v15; // ecx
	short v16; // ax
	int v17; // eax
	signed int v18; // [esp+Ch] [ebp-10h]
	signed int v19; // [esp+Ch] [ebp-10h]
	signed int a5; // [esp+10h] [ebp-Ch]
	int a5a; // [esp+10h] [ebp-Ch]
	signed int a6; // [esp+14h] [ebp-8h]
	signed int a6a; // [esp+14h] [ebp-8h]
	int a6b; // [esp+14h] [ebp-8h]
	int ya; // [esp+18h] [ebp-4h]

	v2 = ScrollInfo._sxoff + 64;
	dword_5C2FF8 = 6;
	dword_5C2FFC = 6;
	v3 = x - 6;
	ya = y - 1;
	a5 = 6;
	v4 = ScrollInfo._syoff + 143;
	scr_pix_width = 384;
	scr_pix_height = 192;
	v18 = 3;
	switch ( ScrollInfo._sdir )
	{
		case DIR_SW:
			goto LABEL_3;
		case DIR_W:
			a5 = 7;
LABEL_3:
			v4 = ScrollInfo._syoff + 111;
			v3 = x - 7;
			ya = y - 2;
			goto LABEL_9;
		case DIR_NW:
			goto LABEL_7;
		case DIR_N:
			v18 = 4;
			goto LABEL_7;
		case DIR_NE:
			goto LABEL_9;
		case DIR_E:
			v18 = 4;
			goto LABEL_6;
		case DIR_SE:
LABEL_6:
			v2 = ScrollInfo._sxoff;
			v3 = x - 7;
			ya = y;
LABEL_7:
			a5 = 7;
			break;
		case DIR_OMNI:
			v2 = ScrollInfo._sxoff;
			v4 = ScrollInfo._syoff + 111;
			a5 = 7;
			v3 = x - 8;
LABEL_9:
			v18 = 4;
			break;
		default:
			break;
	}
	a6 = 0;
	gpBufEnd = (unsigned char *)gpBuffer + screen_y_times_768[143];
	do
	{
		scrollrt_draw_upper(v3, ya++, v2, v4, a5, a6, 0);
		v5 = v4 + 16;
		v6 = v2 - 32;
		scrollrt_draw_upper(v3++, ya, v6, v5, a5, a6, 1);
		v2 = v6 + 32;
		v4 = v5 + 16;
		++a6;
	}
	while ( a6 < 4 );
	gpBufEnd = (unsigned char *)gpBuffer + screen_y_times_768[320];
	if ( v18 > 0 )
	{
		do
		{
			scrollrt_draw_lower(v3, ya++, v2, v4, a5, 0);
			v7 = v4 + 16;
			v8 = v2 - 32;
			scrollrt_draw_lower(v3++, ya, v8, v7, a5, 1);
			v2 = v8 + 32;
			v4 = v7 + 16;
			--v18;
		}
		while ( v18 );
	}
	arch_draw_type = 0;
	a6a = 0;
	do
	{
		scrollrt_draw_lower_2(v3, ya++, v2, v4, a5, a6a, 0);
		v9 = v4 + 16;
		v10 = v2 - 32;
		scrollrt_draw_lower_2(v3++, ya, v10, v9, a5, a6a, 1);
		v2 = v10 + 32;
		v4 = v9 + 16;
		++a6a;
	}
	while ( a6a < 4 );
	if ( chrflag || questlog )
	{
		a6b = 392064;
		goto LABEL_23;
	}
	if ( invflag || sbookflag )
	{
		a6b = 391744;
LABEL_23:
		a5a = 245168;
		v19 = 160;
		goto LABEL_24;
	}
	a5a = 245088;
	a6b = 391744;
	v19 = 320;
LABEL_24:
	v11 = (_WORD *)((char *)gpBuffer + a6b);
	v12 = (char *)gpBuffer + a5a;
	v13 = &gpBuffer->row_unused_1[1].col_unused_1[a6b];
	v14 = 176;
	do
	{
		v15 = v19;
		do
		{
			_LOBYTE(v16) = *v12++;
			_HIBYTE(v16) = v16;
			*v11 = v16;
			*(_WORD *)v13 = v16;
			++v11;
			v13 += 2;
			--v15;
		}
		while ( v15 );
		v12 += -v19 - 768;
		v17 = 2 * (v19 + 768);
		v13 -= v17;
		v11 = (_WORD *)((char *)v11 - v17);
		--v14;
	}
	while ( v14 );
}
// 4B8968: using guessed type int sbookflag;
// 5C2FF8: using guessed type int dword_5C2FF8;
// 5C2FFC: using guessed type int dword_5C2FFC;
// 5C3000: using guessed type int scr_pix_width;
// 5C3004: using guessed type int scr_pix_height;
// 69BD04: using guessed type int questlog;
// 69CF0C: using guessed type int gpBufEnd;
// 69CF20: using guessed type char arch_draw_type;

void __cdecl ClearScreenBuffer()
{
	int i; // edx

	lock_buf_priv();

	for(i = 0; i < 480; i++)
		memset(gpBuffer->row[i].pixels, 0, 640);

	unlock_buf_priv();
}

#ifdef _DEBUG
void __cdecl ScrollView()
{
	signed int v0; // esi
	int v1; // edi
	int v2; // edx

	if ( pcurs < 12 )
	{
		v0 = 0;
		if ( MouseX >= 20 )
		{
			v2 = ViewX;
			v1 = ViewY;
		}
		else
		{
			v1 = ViewY;
			v2 = ViewX;
			if ( ViewY >= dmaxy - 1 || dminx >= ViewX )
			{
				if ( ViewY < dmaxy - 1 )
				{
					v1 = ViewY + 1;
					v0 = 1;
				}
				if ( dminx < ViewX )
				{
					v2 = ViewX - 1;
					v0 = 1;
				}
			}
			else
			{
				v1 = ViewY + 1;
				v2 = ViewX - 1;
				v0 = 1;
			}
		}
		if ( MouseX > 620 )
		{
			if ( dmaxx - 1 > v2 )
			{
				if ( dminy < v1 )
				{
					--v1;
					++v2;
					v0 = 1;
					goto LABEL_19;
				}
				if ( dmaxx - 1 > v2 )
				{
					++v2;
					v0 = 1;
				}
			}
			if ( dminy < v1 )
			{
				--v1;
				v0 = 1;
			}
		}
LABEL_19:
		if ( MouseY >= 20 )
			goto LABEL_28;
		if ( dminy < v1 )
		{
			if ( dminx < v2 )
			{
				--v2;
				--v1;
				goto LABEL_27;
			}
			if ( dminy < v1 )
			{
				--v1;
				v0 = 1;
			}
		}
		if ( dminx >= v2 )
			goto LABEL_28;
		--v2;
LABEL_27:
		v0 = 1;
LABEL_28:
		ViewX = v2;
		ViewY = v1;
		if ( MouseY > 460 )
		{
			if ( v1 >= dmaxy - 1 || dmaxx - 1 <= v2 )
			{
				ViewY = v1;
				if ( v1 < dmaxy - 1 )
				{
					v0 = 1;
					ViewY = v1 + 1;
				}
				ViewX = v2;
				if ( dmaxx - 1 <= v2 )
					goto LABEL_37;
				ViewX = v2 + 1;
			}
			else
			{
				ViewX = v2 + 1;
				ViewY = v1 + 1;
			}
			v0 = 1;
		}
LABEL_37:
		if ( v0 )
			ScrollInfo._sdir = 0;
	}
}

void __cdecl EnableFrameCount()
{
	frameflag = frameflag == 0;
	framestart = GetTickCount();
}
#endif

void __fastcall scrollrt_draw_game_screen(bool draw_cursor)
{
	int dwHgt; // edi

	if ( drawpanflag == 255 )
	{
		drawpanflag = 0;
		dwHgt = 480;
	}
	else
	{
		dwHgt = 0;
	}
	if ( draw_cursor )
	{
		lock_buf_priv();
		scrollrt_draw_cursor_item();
		unlock_buf_priv();
	}
	DrawMain(dwHgt, 0, 0, 0, 0, 0);
	if ( draw_cursor )
	{
		lock_buf_priv();
		scrollrt_draw_cursor_back_buffer();
		unlock_buf_priv();
	}
}
// 52571C: using guessed type int drawpanflag;

void __cdecl scrollrt_draw_cursor_back_buffer()
{
	int v0; // edx
	int v1; // eax
	char *v2; // edi
	char *v3; // esi
	int v4; // ecx
	int v5; // ebx

	v0 = sgdwCursWdt;
	if ( sgdwCursWdt )
	{
		v1 = sgdwCursY;
		v2 = sgSaveBack;
		v3 = &gpBuffer->row[sgdwCursY].pixels[sgdwCursX];
		v4 = sgdwCursHgt;
		if ( sgdwCursHgt )
		{
			v5 = sgdwCursHgt;
			do
			{
				memcpy(v3, v2, v0);
				v0 = sgdwCursWdt;
				v2 += sgdwCursWdt;
				v3 += 768;
				--v5;
			}
			while ( v5 );
			v1 = sgdwCursY;
			v4 = sgdwCursHgt;
		}
		sgdwCursWdt = 0;
		sgdwCursXOld = sgdwCursX;
		sgdwCursYOld = v1;
		sgdwCursWdtOld = v0;
		sgdwCursHgtOld = v4;
	}
}

void __cdecl scrollrt_draw_cursor_item()
{
	int v0; // ebp
	int v1; // edx
	int v2; // edi
	int v3; // esi
	unsigned int v4; // eax
	unsigned int v5; // eax
	int v6; // eax
	char *v7; // ebx
	int v8; // ebp
	int v9; // edi
	int v10; // esi
	signed int v11; // ebx
	int v12; // edi
	int v13; // edx
	char *v14; // [esp+10h] [ebp-4h]

	if ( pcurs > 0 )
	{
		v0 = cursW;
		if ( cursW )
		{
			v1 = cursH;
			if ( cursH )
			{
				v2 = MouseX - 1;
				if ( MouseX - 1 >= 0 )
				{
					if ( v2 > 639 )
						return;
				}
				else
				{
					v2 = 0;
				}
				v3 = MouseY - 1;
				if ( MouseY - 1 >= 0 )
				{
					if ( v3 > 479 )
						return;
				}
				else
				{
					v3 = 0;
				}
				v4 = v2 + cursW + 1;
				if ( v4 > 0x27F )
					v4 = 639;
				_LOBYTE(v4) = v4 | 3;
				sgdwCursY = v3;
				sgdwCursX = v2 & 0xFFFFFFFC;
				sgdwCursWdt = v4 - (v2 & 0xFFFFFFFC) + 1;
				v5 = cursH + v3 + 1;
				if ( v5 > 0x1DF )
					v5 = 479;
				v14 = sgSaveBack;
				v6 = 1 - v3 + v5;
				sgdwCursHgt = v6;
				v7 = &gpBuffer->row[v3].pixels[v2 & 0xFFFFFFFC];
				if ( v6 )
				{
					v8 = v6;
					do
					{
						memcpy(v14, v7, sgdwCursWdt);
						v14 += sgdwCursWdt;
						v7 += 768;
						--v8;
					}
					while ( v8 );
					v0 = cursW;
					v1 = cursH;
				}
				v9 = v2 + 1;
				v10 = v3 + 1;
				gpBufEnd = (unsigned char *)gpBuffer + screen_y_times_768[640] - v0 - 2;
				if ( pcurs < 12 )
				{
					Cel2DrawHdrOnly(v9 + 64, v1 + v10 + 159, (char *)pCursCels, pcurs, v0, 0, 8);
				}
				else
				{
					v11 = 197;
					if ( plr[myplr].HoldItem._iMagical )
						v11 = 181;
					if ( !plr[myplr].HoldItem._iStatFlag )
						v11 = 229;
					v12 = v9 + 64;
					CelDrawHdrClrHL(v11, v12, v1 + v10 + 159, (char *)pCursCels, pcurs, v0, 0, 8);
					v13 = cursH + v10 + 159;
					if ( v11 == 229 )
						Cel2DrawHdrLightRed(v12, v13, (char *)pCursCels, pcurs, cursW, 0, 8, 1);
					else
						Cel2DrawHdrOnly(v12, v13, (char *)pCursCels, pcurs, cursW, 0, 8);
				}
			}
		}
	}
}
// 4B8C9C: using guessed type int cursH;
// 69CF0C: using guessed type int gpBufEnd;

void __fastcall DrawMain(int dwHgt, int draw_desc, int draw_hp, int draw_mana, int draw_sbar, int draw_btn)
{
	signed int v6; // ebp
	DWORD v7; // ebx
	int v8; // esi
	int v9; // eax
	signed int a4; // [esp+1Ch] [ebp-8h]

	a4 = dwHgt;
	if ( gbActive && lpDDSPrimary )
	{
		if ( lpDDSPrimary->IsLost() == DDERR_SURFACELOST )
		{
			if ( lpDDSPrimary->Restore() )
				return;
			ResetPal();
			a4 = 480;
		}
		if ( !lpDDSBackBuf )
		{
			v6 = 1;
LABEL_8:
			v7 = GetTickCount();
			while ( 1 )
			{
				DDS_desc.dwSize = 108;
				v8 = lpDDSPrimary->Lock(NULL, &DDS_desc, DDLOCK_WRITEONLY|DDLOCK_WAIT, NULL);
				if ( !v8 )
					break;
				if ( v7 - GetTickCount() > 5000 )
					goto LABEL_17;
				Sleep(1u);
				if ( v8 == DDERR_SURFACELOST )
					return;
				if ( v8 != DDERR_WASSTILLDRAWING && v8 != DDERR_SURFACEBUSY )
				{
					if ( v6 && v8 == E_FAIL )
					{
						v6 = 0;
						dx_reinit();
						a4 = 480;
						goto LABEL_8;
					}
LABEL_17:
					if ( v8 != DDERR_SURFACELOST && v8 != DDERR_WASSTILLDRAWING && v8 != DDERR_SURFACEBUSY )
					{
						DDErrMsg(v8, 3707, "C:\\Src\\Diablo\\Source\\SCROLLRT.CPP");
						break;
					}
					return;
				}
			}
		}
		if ( a4 > 0 )
			DoBlitScreen(0, 0, 640, a4);
		if ( a4 < 480 )
		{
			if ( draw_sbar )
				DoBlitScreen(204, 357, 232, 28);
			if ( draw_desc )
				DoBlitScreen(176, 398, 288, 60);
			if ( draw_mana )
			{
				DoBlitScreen(460, 352, 88, 72);
				DoBlitScreen(564, 416, 56, 56);
			}
			if ( draw_hp )
				DoBlitScreen(96, 352, 88, 72);
			if ( draw_btn )
			{
				DoBlitScreen(8, 357, 72, 119);
				DoBlitScreen(556, 357, 72, 48);
				if ( (unsigned char)gbMaxPlayers > 1u )
				{
					DoBlitScreen(84, 443, 36, 32);
					DoBlitScreen(524, 443, 36, 32);
				}
			}
			if ( sgdwCursWdtOld )
				DoBlitScreen(sgdwCursXOld, sgdwCursYOld, sgdwCursWdtOld, sgdwCursHgtOld);
			if ( sgdwCursWdt )
				DoBlitScreen(sgdwCursX, sgdwCursY, sgdwCursWdt, sgdwCursHgt);
		}
		if ( !lpDDSBackBuf )
		{
			v9 = lpDDSPrimary->Unlock(NULL);
			if ( v9 != DDERR_SURFACELOST )
			{
				if ( v9 )
					DDErrMsg(v9, 3779, "C:\\Src\\Diablo\\Source\\SCROLLRT.CPP");
			}
		}
#ifdef _DEBUG
		DrawFPS();
#endif
	}
}
// 634980: using guessed type int gbActive;
// 679660: using guessed type char gbMaxPlayers;

#ifdef _DEBUG
void __cdecl DrawFPS()
{
	DWORD v0; // eax
	int v1; // esi
	char String[12]; // [esp+8h] [ebp-10h]
	HDC hdc; // [esp+14h] [ebp-4h]

	if ( frameflag && gbActive )
	{
		++frameend;
		v0 = GetTickCount();
		v1 = v0 - framestart;
		if ( v0 - framestart >= 1000 )
		{
			framestart = v0;
			framerate = 1000 * frameend / v1;
			frameend = 0;
		}
		if ( framerate > 99 )
			framerate = 99;
		wsprintf(String, "%2d", framerate);
		if ( !lpDDSPrimary->GetDC(&hdc) )
		{
			TextOut(hdc, 0, 400, String, strlen(String));
			lpDDSPrimary->ReleaseDC(hdc);
		}
	}
}
#endif

void __fastcall DoBlitScreen(int dwX, int dwY, int dwWdt, int dwHgt)
{
	int v4; // esi
	int v5; // edi
	int v6; // ecx
	char *v7; // esi
	char *v8; // edi
	int v9; // edx
	RECT Rect; // [esp+Ch] [ebp-20h]
	int v14; // [esp+1Ch] [ebp-10h]
	LONG v15; // [esp+20h] [ebp-Ch]
	int v16; // [esp+24h] [ebp-8h]
	LONG v17; // [esp+28h] [ebp-4h]
	HRESULT error_code; // [esp+34h] [ebp+8h]
	int error_codea; // [esp+34h] [ebp+8h]
	int a4; // [esp+38h] [ebp+Ch]

	v4 = dwY;
	v5 = dwX;
	if ( lpDDSBackBuf )
	{
		Rect.left = dwX + 64;
		Rect.right = dwX + 64 + dwWdt - 1;
		Rect.top = dwY + 160;
		Rect.bottom = dwY + 160 + dwHgt - 1;
		a4 = GetTickCount();
		while ( 1 )
		{
			error_code = lpDDSPrimary->BltFast(v5, v4, lpDDSBackBuf, &Rect, DDBLTFAST_WAIT);
			if ( !error_code )
				break;
			if ( a4 - GetTickCount() <= 5000 )
			{
				Sleep(1u);
				if ( error_code == DDERR_SURFACELOST )
					return;
				if ( error_code == DDERR_WASSTILLDRAWING || error_code == DDERR_SURFACEBUSY )
					continue;
			}
			if ( error_code != DDERR_SURFACELOST && error_code != DDERR_WASSTILLDRAWING && error_code != DDERR_SURFACEBUSY )
				DDErrMsg(error_code, 3596, "C:\\Src\\Diablo\\Source\\SCROLLRT.CPP");
			return;
		}
	}
	else
	{
		v14 = 768 * dwY + dwX + 0x1E040;
		v17 = DDS_desc.lPitch - dwWdt;
		v15 = dwX + dwY * DDS_desc.lPitch;
		v6 = 768 - dwWdt;
		error_codea = (unsigned int)dwWdt >> 2;
		v16 = v6;
		lock_buf_priv();
		v7 = (char *)gpBuffer + v14;
		v8 = (char *)DDS_desc.lpSurface + v15;
		v9 = dwHgt;
		do
		{
			qmemcpy(v8, v7, 4 * error_codea);
			v7 += 4 * error_codea + v16;
			v8 += 4 * error_codea + v17;
			--v9;
		}
		while ( v9 );
		unlock_buf_priv();
	}
}

void __cdecl DrawAndBlit()
{
	bool ddsdesc; // ebp
	bool ctrlPan; // esi
	int dwHgt; // edi

	if ( gbRunGame )
	{
		if ( drawpanflag == 255 )
		{
			drawhpflag = TRUE;
			drawmanaflag = TRUE;
			drawbtnflag = 1;
			drawsbarflag = 1;
			ddsdesc = 0;
			ctrlPan = 1;
			dwHgt = 480;
		}
		else
		{
			if ( drawpanflag != 1 )
				return;
			ddsdesc = 1;
			ctrlPan = 0;
			dwHgt = 352;
		}
		drawpanflag = 0;
		lock_buf_priv();
		if ( leveltype )
			DrawView(ViewX, ViewY);
		else
			T_DrawView(ViewX, ViewY);
		if ( ctrlPan )
			ClearCtrlPan();
		if ( drawhpflag )
			UpdateLifeFlask();
		if ( drawmanaflag )
			UpdateManaFlask();
		if ( drawbtnflag )
			DrawCtrlPan();
		if ( drawsbarflag )
			DrawInvBelt();
		if ( talkflag )
		{
			DrawTalkPan();
			dwHgt = 480;
		}
		scrollrt_draw_cursor_item();
		unlock_buf_priv();
		DrawMain(dwHgt, ddsdesc, drawhpflag, drawmanaflag, drawsbarflag, drawbtnflag);
		lock_buf_priv();
		scrollrt_draw_cursor_back_buffer();
		unlock_buf_priv();
		drawhpflag = FALSE;
		drawmanaflag = FALSE;
		drawbtnflag = 0;
		drawsbarflag = 0;
	}
}
// 4B8960: using guessed type int talkflag;
// 525650: using guessed type int gbRunGame;
// 52571C: using guessed type int drawpanflag;
// 5BB1ED: using guessed type char leveltype;
