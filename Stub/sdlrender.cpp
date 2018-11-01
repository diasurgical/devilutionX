#include "../types.h"
#include "miniwin_sdl.h"
#include "stubs.h"

#include "SDL_FontCache.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 500;

int LogoWidth;
int LogoHeight;



SDL_Rect textureRect;
SDL_Rect windowRect;
SDL_Rect CusorLocation;

SDL_Rect SinglePlrBox;
SDL_Rect MultiPlrBox;
SDL_Rect ReplayIntroBox;
SDL_Rect ShowCreditsBox;
SDL_Rect ExitBox;


// DiabloMenu Items

SDL_Rect MainMenuItemsWRect;
SDL_Rect MainMenuItemsTRect;
SDL_Rect CreateHeroBox;
SDL_Rect SinglePlayerMenuCancelBox;



SDL_Event input;


bool SinglePlayerMenuItemsLoaded = 0;
bool DiabloImageLoaded = 0;
bool DiabloMainMenuListLoaded = 0;

struct timespec ts;



//////////////////////////////////////
// things I need to unload...

int FontLoaded = 0;


FC_Font     *  Subfont = FC_CreateFont();

SDL_Texture *  MainMenuItemsTexture;
SDL_Texture *  DiablologoAnimT;
SDL_Texture *  CursorTexture;
SDL_Texture *  MenuSelectNewHeroTexture;
SDL_Texture *  CreateHeroDialogTextureW;
SDL_Texture *  CreateHeroDialogTextureR;
SDL_Texture *  CreateHeroDialogTextureS;

SDL_Surface *  MainMenuItemsSurface;
SDL_Surface *  MenuSelectNewHeroSurface;
SDL_Surface *  CreateHeroDialogSurface;
SDL_Surface *  CursorImg ;
SDL_Surface *  DiabloTitle;

FC_Font     *  font;

/////////////////////////////////////////


void FreeMenuItems(){

SDL_FreeSurface(MainMenuItemsSurface);
SDL_FreeSurface(MenuSelectNewHeroSurface);
SDL_FreeSurface(CreateHeroDialogSurface);
SDL_FreeSurface(CreateHeroDialogSurface);
SDL_FreeSurface(CursorImg);
SDL_FreeSurface(DiabloTitle);


SDL_DestroyTexture(MainMenuItemsTexture);
SDL_DestroyTexture(DiablologoAnimT);
SDL_DestroyTexture(CursorTexture);
SDL_DestroyTexture(MenuSelectNewHeroTexture);
SDL_DestroyTexture(CreateHeroDialogTextureW);
SDL_DestroyTexture(CreateHeroDialogTextureR);
SDL_DestroyTexture(CreateHeroDialogTextureS);


}







char gLDirectory[FILENAME_MAX];
void GetWorkingLocationOfFile(char *RelativeFile)
{
	GetCurrentDir(gLDirectory, FILENAME_MAX);
	strcat(gLDirectory, RelativeFile);
}

uint32_t XgetTick()
{

	unsigned theTick = 0U;
	printf("This is supposed to replace GitTicks()");
	return theTick;
}







void SDLCreateDiabloCursor()
{
	int x, y;
	SDL_GetMouseState(&x, &y);

	CusorLocation.x = x;
	CusorLocation.y = y;
	CusorLocation.h = 29;
	CusorLocation.w = 33;

	GetWorkingLocationOfFile("/Xresources/cursor.png");
	CursorImg = IMG_Load(gLDirectory);
	CursorTexture = SDL_CreateTextureFromSurface(renderer, CursorImg);
	SDL_UpdateWindowSurface(window);
}



void SdlDiabloMainWindow()
{

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	window = SDL_CreateWindow("Diablo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);
	printf("Window And Renderer Created!\n");
}

void LoadDiabloMenuLogoImage()
{
	int LogoWidth;
	int LogoHeight;

	GetWorkingLocationOfFile("/Xresources/Diablo_Logo.png");
	DiabloTitle     = IMG_Load(gLDirectory);
	DiablologoAnimT = SDL_CreateTextureFromSurface(renderer, DiabloTitle);

	// SDL_QueryTexture() method gets the width and height of the texture
	// Now, LogoWidth  and Logo Height are filled
	// with respective dimensions of the image/texture
	SDL_QueryTexture(DiablologoAnimT, NULL, NULL, &LogoWidth, &LogoHeight);
	SDL_FreeSurface(DiabloTitle);
}




void DiabloMainMenuItemsLoaded()
{

	GetWorkingLocationOfFile("/Xresources/DiabloMainMenu.png");
	SDL_Surface *MainMenuItemsSurface = IMG_Load(gLDirectory);
	MainMenuItemsTexture = SDL_CreateTextureFromSurface(renderer, MainMenuItemsSurface);
}

void CreateMainDiabloMenu()
{

	int totalFrames = 4;

	//'windowRect' defines the dimensions of the rendering sprite on window
	// SDL_Rect windowRect;
	windowRect.x = 120; // 140 ///(SCREEN_WIDTH - LogoWidth / totalFrames) / 2;
	windowRect.y = 0;
	windowRect.w = 450; // 320
	windowRect.h = 150;

	//'textureRect' defines the dimensions of the rendering sprite on texture
	// SDL_Rect textureRect;
	textureRect.x = 0;
	textureRect.y = 0;
	textureRect.w = 868;
	textureRect.h = 150;

	if (DiabloImageLoaded == 0) {

		LoadDiabloMenuLogoImage();
		DiabloImageLoaded = 1;
	}



	// get the width of a frame by dividing with 4
	textureRect.w /= totalFrames;
	// Height for each frame is the same as for the whole sheet/texture

	// Load The Main Menu List Single Player, Multi Player,  foo , bar , exit Diablo

	int MainMenuItemsW;
	int MainMenuItemsH;

	if (DiabloMainMenuListLoaded == 0) {

		DiabloMainMenuItemsLoaded();
		DiabloMainMenuListLoaded = 1;
	}

	MainMenuItemsWRect.x = 0;
	MainMenuItemsWRect.y = 0;
	MainMenuItemsWRect.w = 470;
	MainMenuItemsWRect.h = 320;

	MainMenuItemsTRect.x = 150;
	MainMenuItemsTRect.y = 150;
	MainMenuItemsTRect.w = 350;
	MainMenuItemsTRect.h = 250;

	SDL_QueryTexture(MainMenuItemsTexture, NULL, NULL, &MainMenuItemsW, &MainMenuItemsH);
}

void RenderDiabloLogo()
{
	int totalFrames = 4;
	int delayPerFrame = 100;
	int frame = (SDL_GetTicks() / delayPerFrame) % totalFrames;
	textureRect.x = frame * textureRect.w;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_RenderClear(renderer);
	int column = (SCREEN_WIDTH / 4);
	// SDL_RenderCopy(renderer, MainMenuItemsTexture, &MainMenuItemsWRect, &MainMenuItemsTRect);
}



void SDL_RenderDiabloMainPage()
{
	SDL_RenderClear(renderer);

	SinglePlrBox.x = 169;
	SinglePlrBox.y = 163;
	SinglePlrBox.w = 491;
	SinglePlrBox.h = 193;

	MultiPlrBox.x = 166;
	MultiPlrBox.y = 216;
	MultiPlrBox.w = 484;
	MultiPlrBox.h = 240;

	ReplayIntroBox.x = 171;
	ReplayIntroBox.y = 264;
	ReplayIntroBox.w = 483;
	ReplayIntroBox.h = 291;

	ShowCreditsBox.x = 161;
	ShowCreditsBox.y = 310;
	ShowCreditsBox.w = 487;
	ShowCreditsBox.h = 348;

	ExitBox.x = 187;
	ExitBox.y = 363;
	ExitBox.w = 460;
	ExitBox.h = 399;

	RenderDiabloLogo();

	int cx, cy;
	SDL_GetMouseState(&cx, &cy);

	CusorLocation.x = cx;
	CusorLocation.y = cy;
	CusorLocation.h = 29;
	CusorLocation.w = 33;

	SDL_RenderCopy(renderer, DiablologoAnimT, &textureRect, &windowRect);
	SDL_RenderCopy(renderer, MainMenuItemsTexture, &MainMenuItemsWRect, &MainMenuItemsTRect);
	SDL_RenderCopy(renderer, CursorTexture, NULL, &CusorLocation);

	char LDirectory[FILENAME_MAX];
	GetCurrentDir(LDirectory, FILENAME_MAX);
	char *filename = "/fonts/Exocet.ttf";
	strcat(LDirectory, filename);

	if (FontLoaded == 0) {

		FC_LoadFont(Subfont, renderer, LDirectory, 13, FC_MakeColor(112, 106, 70, 255), TTF_STYLE_NORMAL);

		printf("LoadFont\n\n");
		FontLoaded = 1;
	}
	FC_Draw(Subfont, renderer, 10, SCREEN_HEIGHT - 40, "DedicaTed To David Brevik, Erich Schaefer, Max Schaefer, MaTT Uelman and");
	FC_Draw(Subfont, renderer, 10, SCREEN_HEIGHT - 25, "The Blizzard North Team ThaT Gave Us A Childhood.");

	SDL_RenderPresent(renderer);
}

void CreateNewHeroClickBox() {}




void LoadSinglePlayerMenuItems()
{

	CreateNewHeroClickBox();

	GetWorkingLocationOfFile("/Xresources/MenuSelectNewHero.png");
	MenuSelectNewHeroSurface = IMG_Load(gLDirectory);
	MenuSelectNewHeroTexture = SDL_CreateTextureFromSurface(renderer, MenuSelectNewHeroSurface);
	SinglePlayerMenuItemsLoaded = 1;
}

void SDL_RenderDiabloSinglePlayerPage()
{
	RenderDiabloLogo();
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	int cx, cy;
	SDL_GetMouseState(&cx, &cy);

	CusorLocation.x = cx;
	CusorLocation.y = cy;
	CusorLocation.h = 29;
	CusorLocation.w = 33;


	if (SinglePlayerMenuItemsLoaded == 0) {

		LoadSinglePlayerMenuItems();
	}

	SinglePlayerMenuCancelBox.x = 520;
	SinglePlayerMenuCancelBox.y = 454;
	SinglePlayerMenuCancelBox.w = 640;
	SinglePlayerMenuCancelBox.h = 480;

	CreateHeroBox.x = 55;
	CreateHeroBox.y = 444;
	CreateHeroBox.w = 163;
	CreateHeroBox.h = 476;

	int MenuSelectNewHeroW = 907;
	int MenuSelectNewHeroH = 469;
	// W 907 H 469

	SDL_Rect MenuSelectNewHeroWRect;
	MenuSelectNewHeroWRect.x = 0;
	MenuSelectNewHeroWRect.y = 140;
	MenuSelectNewHeroWRect.w = 640;
	MenuSelectNewHeroWRect.h = 350;

	SDL_Rect MenuSelectNewHeroTRect;
	MenuSelectNewHeroTRect.x = 0;
	MenuSelectNewHeroTRect.y = 0;
	MenuSelectNewHeroTRect.w = 907;
	MenuSelectNewHeroTRect.h = 500;

	SDL_RenderCopy(renderer, MenuSelectNewHeroTexture, &MenuSelectNewHeroTRect, &MenuSelectNewHeroWRect);
	SDL_RenderCopy(renderer, DiablologoAnimT, &textureRect, &windowRect);
	SDL_RenderCopy(renderer, CursorTexture, NULL, &CusorLocation);

	// SDL_RenderPresent(renderer);
}

void LoadFont()
{
	GetWorkingLocationOfFile("/fonts/Exocet.ttf");

	font = FC_CreateFont();
	FC_LoadFont(font, renderer, gLDirectory, 35, FC_MakeColor(112, 106, 70, 255), TTF_STYLE_NORMAL);
}

void LoadClickBoxes(int numberofchars)
{
	SDL_Rect Charpos1;
	Charpos1.x = -1;
	Charpos1.y = -1;
	Charpos1.h = -1;
	Charpos1.w = -1;

	SDL_Rect Charpos2;
	Charpos2.x = -1;
	Charpos2.y = -1;
	Charpos2.h = -1;
	Charpos2.w = -1;

	SDL_Rect Charpos3;
	Charpos3.x = -1;
	Charpos3.y = -1;
	Charpos3.h = -1;
	Charpos3.w = -1;

	SDL_Rect Charpos4;
	Charpos4.x = -1;
	Charpos4.y = -1;
	Charpos4.h = -1;
	Charpos4.w = -1;

	SDL_Rect Charpos5;
	Charpos5.x = -1;
	Charpos5.y = -1;
	Charpos5.h = -1;
	Charpos5.w = -1;

	SDL_Rect Charpos6;
	Charpos6.x = -1;
	Charpos6.y = -1;
	Charpos6.h = -1;
	Charpos6.w = -1;

	// The menu doesn't fit past 6 chars.
	SDL_Rect Charpos7;
	SDL_Rect Charpos8;
	SDL_Rect Charpos9;
}

int LoadedFont = 0;
int TotalPlayers = 0;
void RenderCharNames()
{
	if (LoadedFont == 0) {
		LoadFont();
		LoadedFont = 1;
	}

	//	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	// X 355, Y 269
	int x = 340;
	int y = 240;
	TotalPlayers = 0;
	for (int i = 0; i < 6; i++) {
		if (hero_names[i][0] != 0) {
			// Checking if section of array is empty.
			// if array has something in this then draw name.
			for (int j = 0; j < 32; j++) {
				hero_names[i][j] = toupper(hero_names[i][j]); // making all the names uppercase..
			}
			const char *name = hero_names[i]; // getting the index of the name and drawing it to screen
			FC_Draw(font, renderer, x, y, name);

			y += 30;
			TotalPlayers++;
		}
	}

	SDL_RenderPresent(renderer);
}



bool LoadCreateHeroDialogImages = 0;
bool SorcerorCreateSelected     = 0;
bool RogueCreateSelected        = 0;
bool WarriorCreateSelected      = 1;  

void LoadCreateHeroDialogMenu(){
		GetWorkingLocationOfFile("/Xresources/warriorcreate.bmp");

		CreateHeroDialogSurface = IMG_Load(gLDirectory);
		CreateHeroDialogTextureW = SDL_CreateTextureFromSurface(renderer, CreateHeroDialogSurface);

		GetWorkingLocationOfFile("/Xresources/roguecreate.bmp");
		CreateHeroDialogSurface = IMG_Load(gLDirectory);
		CreateHeroDialogTextureR = SDL_CreateTextureFromSurface(renderer, CreateHeroDialogSurface);

		GetWorkingLocationOfFile("/Xresources/sorcerorcreate.bmp");
		CreateHeroDialogSurface = IMG_Load(gLDirectory);
		CreateHeroDialogTextureS = SDL_CreateTextureFromSurface(renderer, CreateHeroDialogSurface);


		LoadCreateHeroDialogImages = 1;



}
//Have this load the function above and then render it in the main menu.
//Cnacel box is also needed.
 void CreateMenuDialogBox() {

	RenderDiabloLogo();

	int cx, cy;
	SDL_GetMouseState(&cx, &cy);

	CusorLocation.x = cx;
	CusorLocation.y = cy;
	CusorLocation.h = 29;
	CusorLocation.w = 33;

	if(LoadCreateHeroDialogImages == 0){
		LoadCreateHeroDialogMenu();

	}

	SDL_Rect MenuSelectNewHeroWRect;
	MenuSelectNewHeroWRect.x = 0;
	MenuSelectNewHeroWRect.y = 140;
	MenuSelectNewHeroWRect.w = 640;
	MenuSelectNewHeroWRect.h = 350;

	SDL_Rect MenuSelectNewHeroTRect;
	MenuSelectNewHeroTRect.x = 0;
	MenuSelectNewHeroTRect.y = 0;
	MenuSelectNewHeroTRect.w = 907;
	MenuSelectNewHeroTRect.h = 500;

	if (WarriorCreateSelected){
		SDL_RenderCopy(renderer, CreateHeroDialogTextureW, &MenuSelectNewHeroTRect, NULL);
	//	printf("WarriorCreateSelected\n");
	}
	if (RogueCreateSelected){
		SDL_RenderCopy(renderer, CreateHeroDialogTextureR, &MenuSelectNewHeroTRect, NULL);
	//	printf("RogueCreateSelected\n");
	}
	if(SorcerorCreateSelected){
		SDL_RenderCopy(renderer, CreateHeroDialogTextureS, &MenuSelectNewHeroTRect, NULL);
		//printf("SorcerorCreateSelected\n");

	}


SDL_RenderCopy(renderer, DiablologoAnimT, &textureRect, &windowRect);
SDL_RenderCopy(renderer, CursorTexture, NULL, &CusorLocation);
SDL_RenderPresent(renderer);
	
 }