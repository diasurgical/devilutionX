#include "../types.h"
#include "miniwin_sdl.h"
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
	init_archives();

	encrypt_init_lookup_table();
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

void LoadCharNamesintoMemory(int start, int end)
{
	PkPlayerStruct pkplr;
	int unused;
	void *CharFile;
	char *p_hero_names = *hero_names;// Not sure if this is correct

	memset(hero_names, 0, 0x140u);

	while (start != end) {
		CharFile = pfile_open_save_archive(&unused, start);
		if (CharFile) {
			if (pfile_read_hero(CharFile, &pkplr)) {
				strcpy(p_hero_names, pkplr.pName);
				UnPackPlayer(&pkplr, 0, 0);
				pfile_archive_contains_game(CharFile);
			}
			pfile_SFileCloseArchive(CharFile);
		}

		p_hero_names += 32;
		start++;
	}
	// memcpy(shero_names, hero_names, sizeof(hero_names));
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

void HideCursor()
{
Uint8 l_data[1];
Uint8 l_mask[1];

l_data[0] = 0;
l_mask[0] = 0;

SDL_Cursor * g_cursor = SDL_CreateCursor(l_data, l_mask, 1, 1, 0, 0);
SDL_SetCursor(g_cursor);
}



void SDL_Diablo_UI() // I anticipate to move this later.
{
	snd_init();
	//music_start(5);
	printf("This is Run And We need a Wile Loop");

	int menu = 0;
	SDL_Event event;
	int x, y;
	bool quit = false;
	int CharsLoaded = 0;
	/* Comment out these functions */
	printf("Main Menu Init\n");
	//SDL_ShowCursor(SDL_DISABLE);//Doesn't really work... Use HideCursor() instead.
	SdlDiabloMainWindow();

	SDLCreateDiabloCursor();
	HideCursor();
	// GetWorkingDirectory();
	/*comment out this*/

	// a2 = option; // not used

	//	mainmenu_refresh_music();
//	do {

	

		while (1 && quit == false) {

			if (menu == 0) {

				CreateMainDiabloMenu();
				SDL_RenderDiabloMainPage();
				// printf("I am 0\n");
			}

			if (menu == 2) {

				if (CharsLoaded == 0) {
					LoadCharNamesintoMemory(0, 10);
					CharsLoaded = 1;
				}

				RenderCharNames();
				SDL_RenderDiabloSinglePlayerPage();
				gbMaxPlayers = 1;
			}

			if (SDL_PollEvent(&event)) {

				if (event.type == SDL_QUIT) {

					quit = true;
					SDL_Quit();
				//	goto LABEL_16;
				}

				if (event.type == SDL_MOUSEMOTION) {
					// Get the mouse offsets
					x = event.motion.x;
					y = event.motion.y;
				}

				// If a key was pressed
				if (event.type == SDL_MOUSEBUTTONDOWN) {

					if (event.button.button == SDL_BUTTON_LEFT) {
						x = event.button.x;
						y = event.button.y;
						printf("X %d , Y %d\n", x, y);

						if (menu == 0) {

							if ((x > SinglePlrBox.x) && (y > SinglePlrBox.y) && (x < SinglePlrBox.w) &&
							    (y < SinglePlrBox.h)) { // Single clicked
								printf("SinglePlayer Diablo\n");
								// effects_play_sound("Sfx\\Items\\Titlslct.wav");
								// a2 = 2;
								menu = 2;
							}

							if ((x > MultiPlrBox.x) && (y > MultiPlrBox.y) && (x < MultiPlrBox.w) &&
							    (y < MultiPlrBox.h)) { // MultiBox clicked
								printf("MultiPlayer Diablo\n");
								// effects_play_sound("Sfx\\Items\\Titlslct.wav");
								// v1 = mainmenu_multi_player();
								//	goto LABEL_15;
							}
							if ((x > ReplayIntroBox.x) && (y > ReplayIntroBox.y) && (x < ReplayIntroBox.w) &&
							    (y < ReplayIntroBox.h)) { // Reply Intro clicked
								printf("Replay Intro\n");
								// effects_play_sound("Sfx\\Items\\Titlslct.wav");
								//	goto LABEL_10;
							}
							if ((x > ShowCreditsBox.x) && (y > ShowCreditsBox.y) && (x < ShowCreditsBox.w) &&
							    (y < ShowCreditsBox.h)) { // ShowCredits clicked
								printf("Show Credits\n");
								// effects_play_sound("Sfx\\Items\\Titlslct.wav");
								UiCreditsDialog(16);
								
							}

							if ((x > ExitBox.x) && (y > ExitBox.y) && (x < ExitBox.w) &&
							    (y < ExitBox.h)) { // ExitBox clicked
								printf("Exiting Diablo\n");
								// effects_play_sound("Sfx\\Items\\Titlslct.wav");

								quit = true;
								SDL_Quit();
								exit(0);
								//goto LABEL_16;
							}

						} // End of this Menu0

						else if (menu == 2) {

							//	int x = 364;
							//  int y = 240;
							int ClickListStart = 250;
							int sizeOfBox = 30;
							int WidthOfBox = 400;
							int ClickListEnd = 343;

							// Render Clicks
							if (TotalPlayers >= 1 && (x > ClickListStart) && (y > ClickListStart) &&
							    (x < ClickListStart + WidthOfBox) &&
							    (y < ClickListStart + sizeOfBox)) { // MultiBox clicked
								printf("Player %s\n", hero_names[0]);
								effects_play_sound("Sfx\\Items\\Titlslct.wav");
								strcpy(chr_name_str, hero_names[0]);
								printf("Player %s\n", chr_name_str);
								break;
							}

							if (TotalPlayers >= 2 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox)) &&
							    (x < ClickListStart + WidthOfBox) &&
							    (y < ClickListStart + (sizeOfBox) + sizeOfBox)) { // MultiBox clicked
								printf("Player 2 Diablo\n");
								effects_play_sound("Sfx\\Items\\Titlslct.wav");
								strcpy(chr_name_str, hero_names[1]);
								printf("Player %s\n", chr_name_str);
								//XStartGame();
								break;
							}
							if (TotalPlayers >= 3 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 2)) &&
							    (x < ClickListStart + WidthOfBox) &&
							    (y < ClickListStart + (sizeOfBox * 2) + sizeOfBox)) { // MultiBox clicked
								printf("Player 3 Diablo\n");
								effects_play_sound("Sfx\\Items\\Titlslct.wav");
								strcpy(chr_name_str, hero_names[2]);
								printf("Player %s\n", chr_name_str);
								break;
								//XStartGame();
							}
							if (TotalPlayers >= 4 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 3)) &&
							    (x < ClickListStart + WidthOfBox) &&
							    (y < ClickListStart + (sizeOfBox * 3) + sizeOfBox)) { // MultiBox clicked
								printf("Player 4 Diablo\n");
								effects_play_sound("Sfx\\Items\\Titlslct.wav");
								strcpy(chr_name_str, hero_names[3]);
								printf("Player %s\n", chr_name_str);
								break;
								//XStartGame();
							}
							if (TotalPlayers >= 5 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 4)) &&
							    (x < ClickListStart + WidthOfBox) &&
							    (y < ClickListStart + (sizeOfBox * 4) + sizeOfBox)) { // MultiBox clicked
								printf("Player 5 Diablo\n");
								effects_play_sound("Sfx\\Items\\Titlslct.wav");
								strcpy(chr_name_str, hero_names[4]);
								printf("Player %s\n", chr_name_str);
								
								break;
								//XStartGame();
							}
							if (TotalPlayers >= 6 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 5)) &&
							    (x < ClickListStart + WidthOfBox) &&
							    (y < ClickListStart + (sizeOfBox * 5) + sizeOfBox)) { // MultiBox clicked
								printf("Player 6 Diablo\n");
								effects_play_sound("Sfx\\Items\\Titlslct.wav");
								strcpy(chr_name_str, hero_names[5]);
								printf("Player %s\n", chr_name_str);
								break;
								//XStartGame();
							}

							if ((x > SinglePlayerMenuCancelBox.x) && (y > SinglePlayerMenuCancelBox.y) &&
							    (x < SinglePlayerMenuCancelBox.w) &&
							    (y < SinglePlayerMenuCancelBox.h)) { // ExitBox clicked
								printf("CancelBox Diablo\n");
								effects_play_sound("Sfx\\Items\\Titlslct.wav");

								//	quit = true;
								//	goto LABEL_16;
								menu = 0;
							}
						}
					}
				}
			}
		}
	}

int main(int argc, char **argv)
{
	music_start(5);


	SDL_Diablo_UI();
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
