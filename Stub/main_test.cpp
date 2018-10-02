#include "../types.h"

#include "stubs.h"

#include <stddef.h>

static_assert(sizeof(plr) == 0x15360u, "sizeof(PlayerStruct) is wrong");

extern "C" {
const char *__asan_default_options()
{
	return "halt_on_error=0:print_legend=0";
}
}

static void init()
{
	//init_archives();
	


	//encrypt_init_lookup_table();
	diablo_init_screen();

	InitLightTable();

	gdwNormalMsgSize = 512;
}

static void test_levelgen()
{
	init();

	SetRndSeed(123);

	currlevel = 2;
	leveltype = DTYPE_CATACOMBS;

	// DRLG_Init_Globals();
	// LoadLvlGFX();
	// CreateLevel(0);

	// LoadGameLevel(TRUE, 0);
}

static void test_newgame()
{
	init();

	start_game(WM_DIABNEWGAME);
}



void XStartGame()
{
	printf("Not Implemented\n");


	// pfile_read_player_from_save();

	// int v2; // esi
	// int v3; // edi
	// int v4; // ecx
	// int pfExitProgram; // [esp+Ch] [ebp-4h]

	// XNetInit(1, &pfExitProgram);
	// while (1)
	// {

	// 	printf("V2 STUFF : %i bSinglePlayer : %i", v2, &pfExitProgram);
	// 	pfExitProgram = 0;
	// 	dword_5256E8 = 0;
	// 	byte_678640 = 0;
	// 	InitLevels();
	// 	InitQuests();
	// 	InitPortals();
	// 	InitDungMsgs(myplr);
	// 	v4 = WM_DIABLOADGAME;
	// 	run_game_loop(v4);
	// 	NetClose();

	// }
}




int main(int argc, char **argv)
{
	//printf("Start\n\n");
	
	//music_start(5);


	
	//#define O(f) fprintf(stderr, "offsetof(%s) = %d\n", #f, offsetof(PlayerStruct, f))
	//
	//	O(_pRSplType);
	//	O(_pSBkSplType);
	//	O(_pSplLvl);
	//	O(_pMemSpells64);
	//	O(_pAblSpells64);
	//	O(_pSpellFlags);
	//	O(_pClass);
	//	O(_pVar8);
	//	O(_pIBonusDamMod);
	//	O(_pISpells64);
	//	O(_pBData);
	//
	//	printf("s %d\n", sizeof(UINT64));

	// test_levelgen();
	// test_newgame();

	
	WinMain(NULL, NULL, argc > 1 ? argv[1] : "", 0);

	eprintf("Done!\n");

	return 0;
}