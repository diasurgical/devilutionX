#include <dirent.h>
#include <string>

#include "../types.h"
#include "stubs.h"

#ifndef NO_GLOBALS
void *diabdat_mpq;
void *patch_rt_mpq;
void *prealpha_mpq;

_SNETVERSIONDATA fileinfo;
WNDPROC CurrentProc;
int gbActive;
#endif

char gszVersionNumber[260];
char gszProductName[260];
char HeroUndecidedName[17] = {0};
bool CreateSinglePlayerChar;
int HeroChosen = 0;

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
			printf("%s \n", path.c_str());
			return path;
		}
	}

	TermMsg("Required file %s not found", file);
}

void __fastcall init_create_window(int nCmdShow)
{
	DUMMY();

	init_get_file_info();

	// pfile_init_save_directory();

	dx_init(NULL);
	snd_init(NULL);

	init_archives();
	gmenu_init_menu();
	SDL_Diablo_UI();
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
	// I don' think this works, like I would intend.
	SFileOpenArchive(find_file_in_std_directories("prealpha.mpq").c_str(), 1000, 0, &prealpha_mpq);
	assert(prealpha_mpq);
}

void __cdecl init_get_file_info()
{
	strcpy(gszVersionNumber, "0.1");
	strcpy(gszProductName, "devil-test");
}

void GetAvailableHeroes() {}

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
				printf("Player Strength %d\n", (int)pkplr.pBaseStr);
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
	// WNDPROC saveProc;

	snd_init(0);
	music_start(5);

	signed int NewHeroNameIndex = 0;

	int menu = 0;
	SDL_Event event;
	int x, y;
	bool quit = false;
	int CharsLoaded = 0;
	int HeroPortrait = 3;

	printf("Main Menu Init\n");
	// SDL_ShowCursor(SDL_DISABLE);//Doesn't really work... Use HideCursor() instead.
	SdlDiabloMainWindow();

	ClearScreenBuffer();
	//LoadPalette("gendata\\charscrn.pal"); // Uncomenting this fixes the the PCXs...
	LoadPalette("Gendata\\Title.pal"); 

	const Uint8 *state = SDL_GetKeyboardState(NULL);

	// static std::deque<MSG> message_queue;

	while (1 && quit == false) {
		// DrawMouse();
		PaletteFadeIn(8);

		if (menu == 0) {
			//	CreateMainDiabloMenu();
			SDL_RenderDiabloMainPage();
			DrawMouse();
			
		}

		if (menu == 2) {

			if (CharsLoaded == 0) {
				LoadCharNamesintoMemory(0, 7);
				//	LoadHeroStats();
				CharsLoaded = 1;
			}
			SDL_RenderDiabloSinglePlayerPage();
			gbMaxPlayers = 1;
			DrawMouse(); // Not accurate for some reason. It adds too much  and I am not sure why.
			ConstantButtons();
		}

		if (menu == 3) {
			// HeroPortrait = 3;
			CreateHeroMenu();

			DrawNewHeroKartinka(HeroPortrait, 1);
			ConstantButtons();
			DrawMouse();
		}

		int m4Loaded = 0;
		if (menu == 4) {

			DrawNewHeroKartinka(HeroPortrait, 0);
			RenderDefaultStats(HeroPortrait);
			RenderUndecidedHeroName();
			ConstantButtons();
			DrawMouse();
		}
		if (menu == 10) {
			ShowCredts();
		}

		if (SDL_PollEvent(&event)) { // Technically if this is a while look then it would probably work fine...
			                         // but my code above wouldnt escape

			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					menu = 0;
					break;

				case SDLK_BACKSPACE:
					if (NewHeroNameIndex > 0) {
						HeroUndecidedName[NewHeroNameIndex - 1] = 0;
						--NewHeroNameIndex;
					}
					break;

				case SDLK_RETURN:

				default:
					char letter = event.key.keysym.sym; // This is just a way to ensure alpha keys are pressed only
					//printf("%d", event.key.keysym.sym);
					if (int(letter) > 96 && int(letter) < 123 || int(letter) == 32)

						if (NewHeroNameIndex < 17) {
							HeroUndecidedName[NewHeroNameIndex] = letter;
							NewHeroNameIndex++;
						//	printf("%s\n ", HeroUndecidedName);
						}

					break;
				}

			case SDL_KEYUP:
				//printf("Key release detected\n");
				break;

			default:
				//	printf("%c", event.key.keysym.sym);
				break;
			}

			if (event.type == SDL_QUIT) {

				quit = true;
				SDL_Quit();
				exit(0); // SDL quit doesn't always work.
			}

			if (event.type == SDL_MOUSEMOTION) {
				// Get the mouse offsets
				x = event.motion.x;
				y = event.motion.y;
				MouseX = x;
				MouseY = y;
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

						int ClickListStart = 195;
						int sizeOfBox = 60;
						int WidthOfBox = 400;
						int ClickListEnd = 343;

						if ((x > ClickListStart) && (y > ClickListStart) && (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + sizeOfBox)) { // Single clicked
							printf("SinglePlayer Diablo\n");
							// effects_play_sound("Sfx\\Items\\Titlslct.wav");
							menu = 2;
						}

						if ((x > ClickListStart) && (y > ClickListStart + (sizeOfBox)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox) + sizeOfBox)) { // MultiBox clicked
							printf("MultiPlayer Diablo\n");
						}
						if ((x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 2)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 2) + sizeOfBox)) { // Reply Intro clicked
							printf("Replay Intro\n");
						}
						if ((x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 4)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 4) + sizeOfBox)) { // ShowCredits clicked
							printf("Show Credits\n");
							menu = 10;
							// effects_play_sound("Sfx\\Items\\Titlslct.wav");
							// UiCreditsDialog(16);
						}

						if ((x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 5)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 5) + sizeOfBox)) { // ExitBox clicked
							printf("Exiting Diablo\n");
							// effects_play_sound("Sfx\\Items\\Titlslct.wav");

							quit = true;
							SDL_Quit();
							exit(0);
							// goto LABEL_16;
						}

					} // End of this Menu0

					if (menu == 2) { // Yes, I know. Skipped 1 and I going to hell for it.
						//	int x = 440;
						//	int y = 430;
						int ClickListStart = 315;
						int sizeOfBox = 30;
						int WidthOfBox = 400;
						int ClickListEnd = 343;

						int CreateHeroY = 555;
						int CreateHeroX = 305;

						SDL_Rect CreateHeroCancelBox;
						CreateHeroCancelBox.y = 550;
						CreateHeroCancelBox.x = 675;
						CreateHeroCancelBox.w = CreateHeroCancelBox.x + 100;
						CreateHeroCancelBox.h = CreateHeroCancelBox.y + 30;

						// Render Clicks
						if (TotalPlayers >= 1 && (x > ClickListStart) && (y > ClickListStart) &&
						    (x < ClickListStart + WidthOfBox) && (y < ClickListStart + sizeOfBox)) { // MultiBox clicked
							strcpy(chr_name_str, hero_names[0]);
							printf("Player %s\n", chr_name_str);
							break;
						}

						if (TotalPlayers >= 2 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox) + sizeOfBox)) { // MultiBox clicked
							printf("Player 2 Diablo\n");
							strcpy(chr_name_str, hero_names[1]);
							printf("Player %s\n", chr_name_str);
							break;
						}

						if (TotalPlayers >= 3 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 2)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 2) + sizeOfBox)) { // MultiBox clicked
							printf("Player 3 Diablo\n");
							//							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[2]);
							printf("Player %s\n", chr_name_str);
							break;
						}

						if (TotalPlayers >= 4 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 3)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 3) + sizeOfBox)) { // MultiBox clicked
							printf("Player 4 Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[3]);
							printf("Player %s\n", chr_name_str);
							break;
						}

						if (TotalPlayers >= 5 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 4)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 4) + sizeOfBox)) { // MultiBox clicked
							printf("Player 5 Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[4]);
							printf("Player %s\n", chr_name_str);
							break;
						}

						if (TotalPlayers >= 6 && (x > ClickListStart) && (y > ClickListStart + (sizeOfBox * 5)) &&
						    (x < ClickListStart + WidthOfBox) &&
						    (y < ClickListStart + (sizeOfBox * 5) + sizeOfBox)) { // MultiBox clicked
							printf("Player 6 Diablo\n");
							effects_play_sound("Sfx\\Items\\Titlslct.wav");
							strcpy(chr_name_str, hero_names[5]);
							printf("Player %s\n", chr_name_str);
							break;
						}

						if ((x > CreateHeroCancelBox.x) && (y > CreateHeroCancelBox.y) && (x < CreateHeroCancelBox.w) &&
						    (y < CreateHeroCancelBox.h)) {
							HeroPortrait = 3;

							printf("Cancel\n\n\n");
							menu = 0;
						}
						if ((x > CreateHeroX) && (y > CreateHeroY) && (x < CreateHeroX + WidthOfBox) &&
						    (y < CreateHeroY + sizeOfBox)) {
							printf("Clicked Create Hero Box\n");
							menu = 3;
						}
					}
					if (menu == 3) {
						// SinglePlayerMenuItemsLoaded = 0;
						printf("\n\nmenu3 X%d Y%d \n ", x, y);

						int WarriorSelectBoxY = 430;
						int WarriorSelectBoxW = 100;
						int WarriorSelectBoxS = 100;

						int RogueSelectBoxX = 450;
						int RogueSelectBoxY = 392;
						int SorcerorSelectBoxX = 383;
						int SorcerorSelectBoxY = 365;
						int CreateHeroOkBoxX = 330;
						int CreateHeroOkBoxY = 441;
						int CreateHeroCanBBoxX = 445;
						int CreateHeroCanBBoxY = 473;

						SDL_Rect WarriorSelectBox;
						WarriorSelectBox.y = 350;
						WarriorSelectBox.x = 450;
						WarriorSelectBox.w = WarriorSelectBox.x + 100;
						WarriorSelectBox.h = WarriorSelectBox.y + 30;

						SDL_Rect RogueSelectBox;
						RogueSelectBox.y = 392;
						RogueSelectBox.x = 450;
						RogueSelectBox.w = RogueSelectBox.x + 100;
						RogueSelectBox.h = RogueSelectBox.y + 30;
						// X450 Y 392 ;

						SDL_Rect SorcerorSelectBox;
						SorcerorSelectBox.y = 428;
						SorcerorSelectBox.x = 450;
						SorcerorSelectBox.w = SorcerorSelectBox.x + 100;
						SorcerorSelectBox.h = SorcerorSelectBox.y + 30;
						// X 447 Y 428

						SDL_Rect CreateHeroCancelBox;
						CreateHeroCancelBox.y = 550;
						CreateHeroCancelBox.x = 675;
						CreateHeroCancelBox.w = CreateHeroCancelBox.x + 100;
						CreateHeroCancelBox.h = CreateHeroCancelBox.y + 30;

						if ((x > WarriorSelectBox.x) && (y > WarriorSelectBox.y) && (x < WarriorSelectBox.w) &&
						    (y < WarriorSelectBox.h)) {

							printf(" warrior I was hit\n\n\n");
							HeroPortrait = 0;
							HeroChosen = 0;
							menu = 4;
						}
						if ((x > RogueSelectBox.x) && (y > RogueSelectBox.y) && (x < RogueSelectBox.w) &&
						    (y < RogueSelectBox.h)) {

							printf(" rogue I was hit\n\n\n");
							HeroPortrait = 1;
							HeroChosen = 1;
							menu = 4;
						}
						if ((x > SorcerorSelectBox.x) && (y > SorcerorSelectBox.y) && (x < SorcerorSelectBox.w) &&
						    (y < SorcerorSelectBox.h)) {
							HeroPortrait = 2;
							printf("sorceror I was hit\n\n\n");
							HeroChosen = 2;
							menu = 4;
						}

						if ((x > CreateHeroCancelBox.x) && (y > CreateHeroCancelBox.y) && (x < CreateHeroCancelBox.w) &&
						    (y < CreateHeroCancelBox.h)) {
							HeroPortrait = 3;

							printf("Cancel\n\n\n");
							--menu;
						}
					}
					if (menu == 4) {
						printf("sozdat geroya");
						SDL_Rect CreateHeroCancelBox;
						CreateHeroCancelBox.y = 550;
						CreateHeroCancelBox.x = 675;
						CreateHeroCancelBox.w = CreateHeroCancelBox.x + 100;
						CreateHeroCancelBox.h = CreateHeroCancelBox.y + 30;

						// X 549 , Y 551
						SDL_Rect ClickOkBox;
						ClickOkBox.y = 550;
						ClickOkBox.x = 550;
						ClickOkBox.w = ClickOkBox.x + 30;
						ClickOkBox.h = ClickOkBox.y + 30;

						if ((x > CreateHeroCancelBox.x) && (y > CreateHeroCancelBox.y) && (x < CreateHeroCancelBox.w) &&
						    (y < CreateHeroCancelBox.h)) {
							memset(HeroUndecidedName, 0, 17);

							printf("Cancel\n\n\n");
							HeroPortrait = 3;
							menu = 3;
						}

						if ((x > ClickOkBox.x) && (y > ClickOkBox.y) && (x < ClickOkBox.w) && (y < ClickOkBox.h)) {

							printf("Ok\n");
							CreateSinglePlayerChar = 1;
							const char *test_name = HeroUndecidedName;
							printf("%s\n", test_name);
							break;
						}
					}
				}
			}
		}
	}
}
