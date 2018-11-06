#include <dirent.h>
#include <string>

#include "../types.h"
#include "stubs.h"

#ifndef NO_GLOBALS
void *diabdat_mpq;
void *patch_rt_mpq;

_SNETVERSIONDATA fileinfo;
WNDPROC CurrentProc;
int gbActive;
#endif

char gszVersionNumber[260];
char gszProductName[260];

/**
 * Case insensitive search for a file name in a directory.
 * @return Empty string when not found.
 */
static std::string find_file_in_directory(const char *dirpath, const char *file)
{
	DIR *dir = opendir(dirpath);
	if (!dir) {
		return "";
	}

	struct dirent *entry;
	std::string result;
	while ((entry = readdir(dir)) != NULL) {
		if (strcasecmp(entry->d_name, file) == 0) {
			result = std::string(dirpath) + "/" + entry->d_name;
			break;
		}
	}
	closedir(dir);

	return result;
}

static std::string find_file_in_std_directories(const char *file)
{
	for (auto dir : {".", "..", "../.."}) {
		auto path = find_file_in_directory(dir, file);
		if (!path.empty()) {
			return path;
		}
	}

	TermMsg("Required file %s not found", file);
}

void __fastcall init_create_window(int nCmdShow)
{
	DUMMY();
	SDL_Diablo_UI();

	init_get_file_info();

	// pfile_init_save_directory();

	dx_init(NULL);
	snd_init(NULL);

	init_archives();
}

LRESULT __stdcall MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	UNIMPLEMENTED();
}

WNDPROC __fastcall SetWindowProc(WNDPROC NewProc)
{
	WNDPROC OldProc = CurrentProc;
	CurrentProc = NewProc;
	return OldProc;
}

void __cdecl init_archives()
{
	DUMMY();

	SFileOpenArchive(find_file_in_std_directories("diabdat.mpq").c_str(), 1000, 0, &diabdat_mpq);
	assert(diabdat_mpq);

	SFileOpenArchive(find_file_in_std_directories("patch_rt.mpq").c_str(), 1000, 0, &patch_rt_mpq);
	assert(patch_rt_mpq);
}

void __cdecl init_get_file_info()
{
	strcpy(gszVersionNumber, "0.1");
	strcpy(gszProductName, "devil-test");
}

void LoadCharNamesintoMemory(int start, int end)
{
	PkPlayerStruct pkplr;
	int unused;
	void *CharFile;
	char *p_hero_names = *hero_names; // Not sure if this is correct

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

void HideCursor()
{
	Uint8 l_data[1];
	Uint8 l_mask[1];

	l_data[0] = 0;
	l_mask[0] = 0;

	SDL_Cursor *g_cursor = SDL_CreateCursor(l_data, l_mask, 1, 1, 0, 0);
	SDL_SetCursor(g_cursor);
}

void SDL_Diablo_UI() // I anticipate to move this later.
{
	snd_init(0);
	init_archives();
	music_start(5);

	char NewHeroName[17] = {0};
	signed int NewHeroNameIndex = 0;
	int menu = 0;
	SDL_Event event;
	int x, y;
	bool quit = false;
	int CharsLoaded = 0;
	/* Comment out these functions */
	printf("Main Menu Init\n");
	// SDL_ShowCursor(SDL_DISABLE);//Doesn't really work... Use HideCursor() instead.
	SdlDiabloMainWindow();

	SDLCreateDiabloCursor();
	HideCursor();
	//ClearScreenBuffer();

	SDL_StartTextInput();
	const Uint8 *state = SDL_GetKeyboardState(NULL);

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

		if (menu == 3) {

			CreateMenuDialogBox();
		}
		if (menu == 4) {
			SDL_RenderDiabloSinglePlayerPage();
			SDL_RenderPresent(renderer);


			

							// NewHeroName[NewHeroNameIndex] = '\0';
				// memset(&NewHeroName[NewHeroNameIndex], 0, sizeof(char));
				// --NewHeroNameIndex;
				// printf("HeroName %s \n", NewHeroName);
		




			

			if (event.type == SDL_KEYDOWN && NewHeroNameIndex < 17) {

				// char *keyz = SDL_GetKeyName(event.key.keysym.sym);
				// printf("SDLKEY PRESSED %s\n", keyz);
				// NewHeroName[NewHeroNameIndex] = *keyz;
				// NewHeroNameIndex++;

				//	printf("HeroName %s", NewHeroName);
			}
		}

		if (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {

				quit = true;
				SDL_Quit();
			}

			if (event.type == SDL_MOUSEMOTION) {
				// Get the mouse offsets
				x = event.motion.x;
				y = event.motion.y;
			}

			// if (event.type == SDL_KEYDOWN && menu == 4 ){
			// 	char * keyz = SDL_GetKeyName(event.key.keysym.sym);
			// 	printf("SDLKEY PRESSED %s", keyz);

			// 	}

			// If a key was pressed
			if (event.type == SDL_MOUSEBUTTONDOWN /*&& event.button.clicks == 2*/) {

				if (event.button.button == SDL_BUTTON_LEFT) {
					x = event.button.x;
					y = event.button.y;
					printf("X %d , Y %d\n", x, y);

					if (menu == 0) {

						if ((x > SinglePlrBox.x) && (y > SinglePlrBox.y) && (x < SinglePlrBox.w) &&
						    (y < SinglePlrBox.h)) { // Single clicked
							printf("SinglePlayer Diablo\n");
							// effects_play_sound("Sfx\\Items\\Titlslct.wav");
							menu = 2;
						}

						if ((x > MultiPlrBox.x) && (y > MultiPlrBox.y) && (x < MultiPlrBox.w) &&
						    (y < MultiPlrBox.h)) { // MultiBox clicked
							printf("MultiPlayer Diablo\n");

						}
						if ((x > ReplayIntroBox.x) && (y > ReplayIntroBox.y) && (x < ReplayIntroBox.w) &&
						    (y < ReplayIntroBox.h)) { // Reply Intro clicked
							printf("Replay Intro\n");

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
							// goto LABEL_16;
						}

					} // End of this Menu0

					if (menu == 2) {

						//	int x = 364;
						//  int y = 240;
						int ClickListStart = 250;
						int sizeOfBox = 30;
						int WidthOfBox = 400;
						int ClickListEnd = 343;

						// Render Clicks
						if (TotalPlayers >= 1 && (x > ClickListStart) && (y > ClickListStart) &&
						    (x < ClickListStart + WidthOfBox) && (y < ClickListStart + sizeOfBox)) { // MultiBox clicked
							printf("Player %s\n", hero_names[0]);
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[0]);
							printf("Player %s\n", chr_name_str);
							FreeMenuItems();
							break;
						}

						if (TotalPlayers >= 2 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox) + sizeOfBox)) { // MultiBox clicked
							printf("Player 2 Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[1]);
							printf("Player %s\n", chr_name_str);
							FreeMenuItems();
							// XStartGame();
							break;
						}
						if (TotalPlayers >= 3 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 2)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 2) + sizeOfBox)) { // MultiBox clicked
							printf("Player 3 Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[2]);
							printf("Player %s\n", chr_name_str);
							FreeMenuItems();
							break;
							// XStartGame();
						}
						if (TotalPlayers >= 4 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 3)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 3) + sizeOfBox)) { // MultiBox clicked
							printf("Player 4 Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[3]);
							printf("Player %s\n", chr_name_str);
							FreeMenuItems();
							break;
							// XStartGame();
						}
						if (TotalPlayers >= 5 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 4)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 4) + sizeOfBox)) { // MultiBox clicked
							printf("Player 5 Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[4]);
							printf("Player %s\n", chr_name_str);
							FreeMenuItems();
							break;
							// XStartGame();
						}
						if (TotalPlayers >= 6 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 5)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 5) + sizeOfBox)) { // MultiBox clicked
							printf("Player 6 Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[5]);
							printf("Player %s\n", chr_name_str);
							FreeMenuItems();
							break;
							// XStartGame();
						}

						if ((x > SinglePlayerMenuCancelBox.x) && (y > SinglePlayerMenuCancelBox.y) &&
						    (x < SinglePlayerMenuCancelBox.w) && (y < SinglePlayerMenuCancelBox.h)) { // ExitBox clicked
							printf("CancelBox Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");

							menu = 0;
						}
						if ((x > CreateHeroBox.x) && (y > CreateHeroBox.y) && (x < CreateHeroBox.w) &&
						    (y < CreateHeroBox.h)) {
							printf("Clicked Create Hero Box\n");
							menu = 3;
						}
					}
					if (menu == 3) {
						// SinglePlayerMenuItemsLoaded = 0;
						printf("\n\nmenu3 X%d Y%d \n ", x, y);

						int WarriorSelectBoxY = 379;
						int WarriorSelectBoxW = 100;
						int WarriorSelectBoxS = 100;

						int RogueSelectBoxX = 360;
						int RogueSelectBoxY = 345;
						int SorcerorSelectBoxX = 383;
						int SorcerorSelectBoxY = 365;
						int CreateHeroOkBoxX = 330;
						int CreateHeroOkBoxY = 441;
						int CreateHeroCanBBoxX = 445;
						int CreateHeroCanBBoxY = 473;

						SDL_Rect WarriorSelectBox;
						WarriorSelectBox.y = 280;
						WarriorSelectBox.x = 375;
						WarriorSelectBox.w = WarriorSelectBox.x + 100;
						WarriorSelectBox.h = WarriorSelectBox.y + 30;

						SDL_Rect RogueSelectBox;
						RogueSelectBox.y = 320;
						RogueSelectBox.x = 400;
						RogueSelectBox.w = RogueSelectBox.x + 100;
						RogueSelectBox.h = RogueSelectBox.y + 30;

						SDL_Rect SorcerorSelectBox;
						SorcerorSelectBox.y = 360;
						SorcerorSelectBox.x = 380;
						SorcerorSelectBox.w = SorcerorSelectBox.x + 100;
						SorcerorSelectBox.h = SorcerorSelectBox.y + 30;

						SDL_Rect CreateHeroCancelBox;
						CreateHeroCancelBox.y = 450;
						CreateHeroCancelBox.x = 450;
						CreateHeroCancelBox.w = CreateHeroCancelBox.x + 100;
						CreateHeroCancelBox.h = CreateHeroCancelBox.y + 30;

						if ((x > WarriorSelectBox.x) && (y > WarriorSelectBox.y) && (x < WarriorSelectBox.w) &&
						    (y < WarriorSelectBox.h)) {

							printf("I was hit\n\n\n");
							WarriorCreateSelected = 1;
							RogueCreateSelected = 0;
							SorcerorCreateSelected = 0;
							menu = 4;
						}
						if ((x > RogueSelectBox.x) && (y > RogueSelectBox.y) && (x < RogueSelectBox.w) &&
						    (y < RogueSelectBox.h)) {

							printf("I was hit\n\n\n");
							WarriorCreateSelected = 0;
							RogueCreateSelected = 1;
							SorcerorCreateSelected = 0;
							menu = 4;
						}
						if ((x > SorcerorSelectBox.x) && (y > SorcerorSelectBox.y) && (x < SorcerorSelectBox.w) &&
						    (y < SorcerorSelectBox.h)) {

							printf("I was hit\n\n\n");
							WarriorCreateSelected = 0;
							RogueCreateSelected = 0;
							SorcerorCreateSelected = 1;
							menu = 4;
						}

						if ((x > CreateHeroCancelBox.x) && (y > CreateHeroCancelBox.y) && (x < CreateHeroCancelBox.w) &&
						    (y < CreateHeroCancelBox.h)) {

							printf("Cancel\n\n\n");
							menu = 2;
						}
					}
					if (menu == 4) {
						printf("sozdat geroya");

						// if (SDL_PollEvent(&event)) {
						// 	if (event.type == SDL_KEYDOWN && menu == 4) {
						// 		char *keyz = SDL_GetKeyName(event.key.keysym.sym);
						// 		printf("SDLKEY PRESSED %s\n", keyz);
						// 	}
						// }

						SDL_Rect CreateHeroCancelBox;
						CreateHeroCancelBox.y = 450;
						CreateHeroCancelBox.x = 450;
						CreateHeroCancelBox.w = CreateHeroCancelBox.x + 100;
						CreateHeroCancelBox.h = CreateHeroCancelBox.y + 30;

						if ((x > CreateHeroCancelBox.x) && (y > CreateHeroCancelBox.y) && (x < CreateHeroCancelBox.w) &&
						    (y < CreateHeroCancelBox.h)) {

							printf("Cancel\n\n\n");
							menu = 3;
						}
					}
				}
			}
		}
	}
}
