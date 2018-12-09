//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
void *sgpBackCel;
float interfac_cpp_init_value;
int sgdwProgress;
int progress_id; // idb
#endif

const int interfac_inf = 0x7F800000; // weak
const unsigned char progress_bar_colours[3] = { 138u, 43u, 254u };
const int progress_bar_screen_pos[3][2] = { { 53, 37 }, { 53, 421 }, { 53, 37 } };

struct interfac_cpp_init
{
	interfac_cpp_init()
	{
		interfac_cpp_init_value = interfac_inf;
	}
} _interfac_cpp_init;
// 47AE40: using guessed type int interfac_inf;

void __cdecl interface_msg_pump()
{
	MSG Msg; // [esp+8h] [ebp-1Ch]

	while ( PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) )
	{
		if ( Msg.message != WM_QUIT )
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
}

bool __cdecl IncProgress()
{
	interface_msg_pump();
	sgdwProgress += 15;
	if ( (unsigned int)sgdwProgress > 0x216 )
		sgdwProgress = 534;
	if ( sgpBackCel )
		DrawCutscene();
	return (unsigned int)sgdwProgress >= 0x216;
}

void __cdecl DrawCutscene()
{
	unsigned int v0; // esi

	lock_buf_priv();
	CelDecodeOnly(64, 639, sgpBackCel, 1, 640);
	v0 = 0;
	if ( sgdwProgress )
	{
		do
			DrawProgress(
				progress_bar_screen_pos[progress_id][0] + v0++ + 64,
				progress_bar_screen_pos[progress_id][1] + 160,
				progress_id);
		while ( v0 < sgdwProgress );
	}
	

	unlock_buf_priv();
	drawpanflag = 255;
	scrollrt_draw_game_screen(0);
}
// 52571C: using guessed type int drawpanflag;

void __fastcall DrawProgress(int screen_x, int screen_y, int progress_id)
{
	_BYTE *v3; // eax
	signed int v4; // ecx

	v3 = (unsigned char *)gpBuffer + screen_y_times_768[screen_y] + screen_x;
	v4 = 22;
	do
	{
		*v3 = progress_bar_colours[progress_id];
		v3 += 768;
		--v4;
	}
	while ( v4 );
}

void __fastcall ShowProgress(int uMsg)
{
	WNDPROC saveProc; // edi
	BOOL v3; // cl
	int v4; // eax
	int v5; // edx
	signed int v7; // [esp-4h] [ebp-10h]

	gbSomebodyWonGameKludge = 0;
	plrmsg_delay(1);
	saveProc = SetWindowProc(DisableInputWndProc);
	interface_msg_pump();
	ClearScreenBuffer();
	scrollrt_draw_game_screen(1);
	InitCutscene(uMsg);
	BlackPalette();
	DrawCutscene();
	PaletteFadeIn(8);
	IncProgress();
	stream_update(); // What is this?
	IncProgress();
	switch ( uMsg )
	{
		case WM_DIABNEXTLVL:
			IncProgress();
			if ( gbMaxPlayers == 1 )
				SaveLevel();
			else
				DeltaSaveLevel();
			FreeGameMem();
			v4 = ++currlevel;
			goto LABEL_38;
		case WM_DIABPREVLVL:
			IncProgress();
			if ( gbMaxPlayers == 1 )
				SaveLevel();
			else
				DeltaSaveLevel();
			IncProgress();
			FreeGameMem();
			leveltype = gnLevelTypeTbl[--currlevel];
			IncProgress();
			v5 = 1;
			goto LABEL_33;
		case WM_DIABRTNLVL:
			if ( gbMaxPlayers == 1 )
				SaveLevel();
			else
				DeltaSaveLevel();
			setlevel = 0;
			FreeGameMem();
			IncProgress();
			GetReturnLvlPos();
			v7 = 3;
			goto LABEL_32;
		case WM_DIABSETLVL:
			SetReturnLvlPos();
			if ( gbMaxPlayers == 1 )
				SaveLevel();
			else
				DeltaSaveLevel();
			setlevel = 1;
			leveltype = setlvltype;
			FreeGameMem();
			IncProgress();
			v7 = 2;
			goto LABEL_32;
		case WM_DIABWARPLVL:
			IncProgress();
			if ( gbMaxPlayers == 1 )
				SaveLevel();
			else
				DeltaSaveLevel();
			FreeGameMem();
			GetPortalLevel();
			IncProgress();
			v7 = 5;
			goto LABEL_32;
		case WM_DIABTOWNWARP:
			IncProgress();
			if ( gbMaxPlayers == 1 )
				SaveLevel();
			else
				DeltaSaveLevel();
			FreeGameMem();
			currlevel = plr[myplr].plrlevel;
			leveltype = gnLevelTypeTbl[currlevel];
			IncProgress();
			v7 = 6;
			goto LABEL_32;
		case WM_DIABTWARPUP:
			IncProgress();
			if ( gbMaxPlayers == 1 )
				SaveLevel();
			else
				DeltaSaveLevel();
			FreeGameMem();
			currlevel = plr[myplr].plrlevel;
			leveltype = gnLevelTypeTbl[currlevel];
			IncProgress();
			v7 = 7;
LABEL_32:
			v5 = v7;
LABEL_33:
			v3 = FALSE;
			goto LABEL_40;
		case WM_DIABRETOWN:
			IncProgress();
			if ( gbMaxPlayers == 1 )
				SaveLevel();
			else
				DeltaSaveLevel();
			FreeGameMem();
			currlevel = plr[myplr].plrlevel;
			v4 = currlevel;
LABEL_38:
			leveltype = gnLevelTypeTbl[v4];
			IncProgress();
			v3 = FALSE;
			goto LABEL_39;
		case WM_DIABNEWGAME:
			IncProgress();
			FreeGameMem();
			IncProgress();
			pfile_remove_temp_files();
			v3 = TRUE;
LABEL_39:
			v5 = 0;
LABEL_40:
			LoadGameLevel(v3, v5);
			goto LABEL_41;
		case WM_DIABLOADGAME:
			IncProgress();
			LoadGame(TRUE);
LABEL_41:
			IncProgress();
			break;
		default:
			break;
	}
	PaletteFadeOut(8);
	FreeInterface();
	SetWindowProc(saveProc);
	NetSendCmdLocParam1(1u, CMD_PLAYER_JOINLEVEL, plr[myplr].WorldX, plr[myplr].WorldY, plr[myplr].plrlevel);
	plrmsg_delay(0);
	ResetPal();
	if ( gbSomebodyWonGameKludge && plr[myplr].plrlevel == 16 )
		PrepDoEnding();
	gbSomebodyWonGameKludge = 0;
}
// 5BB1ED: using guessed type char leveltype;
// 5CF31C: using guessed type char setlvltype;
// 5CF31D: using guessed type char setlevel;
// 6761B8: using guessed type char gbSomebodyWonGameKludge;
// 679660: using guessed type char gbMaxPlayers;

void __cdecl FreeInterface()
{
	void *v0; // ecx

	v0 = sgpBackCel;
	sgpBackCel = 0;
	mem_free_dbg(v0);
}

void __fastcall InitCutscene(int uMsg)
{
	int v1; // eax
	int v2; // eax
	int v3; // eax
	int v4; // eax
	unsigned char *v5; // eax
	char *v6; // ecx
	int *v7; // eax
	int v8; // eax
	int v9; // eax
	int v10; // eax
	int v11; // eax
	int v12; // eax
	int v13; // eax
	int v14; // eax

	switch ( uMsg )
	{
		case WM_DIABNEXTLVL:
			v1 = gnLevelTypeTbl[currlevel];
			if ( !v1 )
				goto LABEL_31;
			v2 = v1 - 1;
			if ( !v2 )
				goto LABEL_10;
			v3 = v2 - 1;
			if ( !v3 )
				goto LABEL_9;
			v4 = v3 - 1;
			if ( !v4 )
				goto LABEL_29;
			if ( v4 != 1 )
				goto LABEL_10;
			if ( currlevel < 0xFu )
				goto LABEL_28;
			v5 = LoadFileInMem("Gendata\\Cutgate.CEL", 0);
			v6 = "Gendata\\Cutgate.pal";
			goto LABEL_30;
		case WM_DIABPREVLVL:
			v7 = &gnLevelTypeTbl[currlevel];
			if ( !*(v7 - 1) )
				goto LABEL_31;
			v8 = *v7;
			if ( !v8 )
				goto LABEL_31;
			v9 = v8 - 1;
			if ( !v9 )
				goto LABEL_10;
			v10 = v9 - 1;
			if ( !v10 )
			{
LABEL_9:
				sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", 0);
				LoadPalette("Gendata\\Cut2.pal");
				progress_id = 2;
				goto LABEL_33;
			}
			v11 = v10 - 1;
			if ( !v11 )
				goto LABEL_29;
			if ( v11 == 1 )
				goto LABEL_28;
LABEL_10:
			sgpBackCel = LoadFileInMem("Gendata\\Cutl1d.CEL", 0);
			LoadPalette("Gendata\\Cutl1d.pal");
			progress_id = 0;
			goto LABEL_33;
		case WM_DIABRTNLVL:
		case WM_DIABSETLVL:
			if ( setlvlnum == SL_BONECHAMB )
				goto LABEL_21;
			if ( setlvlnum != SL_VILEBETRAYER )
				goto LABEL_10;
			v5 = LoadFileInMem("Gendata\\Cutportr.CEL", 0);
			v6 = "Gendata\\Cutportr.pal";
			goto LABEL_30;
		case WM_DIABWARPLVL:
			v5 = LoadFileInMem("Gendata\\Cutportl.CEL", 0);
			v6 = "Gendata\\Cutportl.pal";
			goto LABEL_30;
		case WM_DIABTOWNWARP:
		case WM_DIABTWARPUP:
			v12 = gnLevelTypeTbl[plr[myplr].plrlevel];
			if ( !v12 )
				goto LABEL_31;
			v13 = v12 - 2;
			if ( !v13 )
			{
LABEL_21:
				sgpBackCel = LoadFileInMem("Gendata\\Cut2.CEL", 0);
				LoadPalette("Gendata\\Cut2.pal");
				progress_id = SL_BONECHAMB;
				goto LABEL_33;
			}
			v14 = v13 - 1;
			if ( v14 )
			{
				if ( v14 != 1 )
					goto LABEL_33;
LABEL_28:
				v5 = LoadFileInMem("Gendata\\Cut4.CEL", 0);
				v6 = "Gendata\\Cut4.pal";
			}
			else
			{
LABEL_29:
				v5 = LoadFileInMem("Gendata\\Cut3.CEL", 0);
				v6 = "Gendata\\Cut3.pal";
			}
LABEL_30:
			sgpBackCel = v5;
			LoadPalette(v6);
			progress_id = 1;
LABEL_33:
			sgdwProgress = 0;
			return;
		case WM_DIABRETOWN:
LABEL_31:
			v5 = LoadFileInMem("Gendata\\Cuttt.CEL", 0);
			v6 = "Gendata\\Cuttt.pal";
			goto LABEL_30;
		case WM_DIABNEWGAME:
		case WM_DIABLOADGAME:
			v5 = LoadFileInMem("Gendata\\Cutstart.CEL", 0);
			v6 = "Gendata\\Cutstart.pal";
			goto LABEL_30;
		default:
			TermMsg("Unknown progress mode");
			goto LABEL_33;
	}
}
// 5CCB10: using guessed type char setlvlnum;
