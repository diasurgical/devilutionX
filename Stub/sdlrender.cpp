#include "../types.h"
#include "stubs.h"
#include "miniwin_sdl.h"


#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "SDL_FontCache.h"

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

int LogoWidth;
int LogoHeight;


SDL_Texture *DiablologoAnimT;
FC_Font* font;

SDL_Rect textureRect;
SDL_Rect windowRect;
SDL_Rect CusorLocation;

 SDL_Rect SinglePlrBox;
 SDL_Rect MultiPlrBox;
 SDL_Rect ReplayIntroBox;
 SDL_Rect ShowCreditsBox;
 SDL_Rect ExitBox;
 SDL_Rect SinglePlayerMenuCancelBox;

SDL_Surface* DiabloTitle;
SDL_Event input;
SDL_Texture  * CursorTexture;


bool SinglePlayerMenuItemsLoaded = 0;
bool DiabloImageLoaded = 0;
bool DiabloMainMenuListLoaded = 0;
SDL_Texture  * MenuSelectNewHeroTexture;



struct timespec ts;



//DiabloMenu Items
SDL_Surface*  MainMenuItemsSurface;
SDL_Texture*  MainMenuItemsTexture;
SDL_Rect      MainMenuItemsWRect;
SDL_Rect      MainMenuItemsTRect;





uint32_t XgetTick() {
	//struct timespec ts;
	unsigned theTick = 0U;
	printf("This is supposed to replace GitTicks()");
//	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		//error
//	}
	return theTick;
}




void SDLCreateDiabloCursor() {
	int x, y;
	SDL_GetMouseState(&x, &y);

	CusorLocation.x = x;
	CusorLocation.y = y;
	CusorLocation.h = 29;
	CusorLocation.w = 33;


	char LDirectory[FILENAME_MAX];
	GetCurrentDir(LDirectory, FILENAME_MAX);

	char * filename = "/Xresources/cursor.png";

	strcat(LDirectory, filename);


	SDL_Surface  * CursorImg = IMG_Load(LDirectory);
	CursorTexture = SDL_CreateTextureFromSurface(renderer, CursorImg);
	SDL_UpdateWindowSurface(window);
}







void SDL_MAGICAL_RENDER(int x, int y, int w, int h) {

	 printf("SDL_MAGICAL RENDER I Am Not Implemented\n");
	// if (window != 0) {

	// 	int depth = 12;

	// 	SDL_Surface  * image = SDL_CreateRGBSurfaceFrom(destmemarea, SCREEN_WIDTH / 2, SCREEN_HEIGHT, depth, SCREEN_WIDTH, 0, 0, 0, 0);
	// 	SDL_Texture  * texture = SDL_CreateTextureFromSurface(renderer, image);
	// 	SDL_RenderCopy(renderer, texture, NULL, NULL);
	// 	SDL_RenderPresent(renderer);
	// 	SDL_UpdateWindowSurface(window);
	// }



}




void SdlDiabloMainWindow() {

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	window = SDL_CreateWindow("Diablo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);
	printf("Window And Renderer Created!\n");
	



}



char *GetWorkingDirectory() {
	char *buff= "wo work";
	//printf("Current working dir: ");

	return buff;
}



void LoadDiabloMenuLogoImage() {
	int LogoWidth;
	int LogoHeight;


	char LDirectory[FILENAME_MAX];
	GetCurrentDir(LDirectory, FILENAME_MAX);

	char * filename = "/Xresources/Diablo_Logo.png";

	strcat(LDirectory, filename);


	//spriteSheet = NULL;
	DiabloTitle = IMG_Load(LDirectory);
	DiablologoAnimT = SDL_CreateTextureFromSurface(renderer, DiabloTitle);

	//SDL_QueryTexture() method gets the width and height of the texture
	//Now, LogoWidth  and Logo Height are filled
	//with respective dimensions of the image/texture
	SDL_QueryTexture(DiablologoAnimT, NULL, NULL, &LogoWidth, &LogoHeight);
	SDL_FreeSurface(DiabloTitle);


}


void DiabloMainMenuItemsLoaded() {
	char LRDirectory[FILENAME_MAX];
	GetCurrentDir(LRDirectory, FILENAME_MAX);

	char * Lfilename = "/Xresources/DiabloMainMenu.png";

	strcat(LRDirectory, Lfilename);

	SDL_Surface*  MainMenuItemsSurface = IMG_Load(LRDirectory);
	MainMenuItemsTexture = SDL_CreateTextureFromSurface(renderer, MainMenuItemsSurface);

}


void CreateMainDiabloMenu(){


	int totalFrames = 4;



	//'windowRect' defines the dimensions of the rendering sprite on window
	//SDL_Rect windowRect;
	windowRect.x = 120;// 140 ///(SCREEN_WIDTH - LogoWidth / totalFrames) / 2;
	windowRect.y = 0;
	windowRect.w = 450;//320
	windowRect.h = 150;

	//'textureRect' defines the dimensions of the rendering sprite on texture
	//SDL_Rect textureRect;
	textureRect.x = 0;
	textureRect.y = 0;
	textureRect.w = 868;
	textureRect.h = 150;


	if (DiabloImageLoaded == 0) {

		LoadDiabloMenuLogoImage();
		DiabloImageLoaded = 1;


	}

  //  font = FC_CreateFont();
  //	FC_LoadFont(font, renderer, "C:/Users/Krash/Desktop/devilutionX/fonts/Exocet.ttf", 40, FC_MakeColor(112, 106, 70, 255), TTF_STYLE_NORMAL);

	
	//get the width of a frame by dividing with 4
	textureRect.w /= totalFrames;
	//Height for each frame is the same as for the whole sheet/texture


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

void RenderDiabloLogo() {
	int totalFrames = 4;
	int delayPerFrame = 100;
	int frame = (SDL_GetTicks() / delayPerFrame) % totalFrames; 
	textureRect.x = frame * textureRect.w;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_RenderClear(renderer);
	int column = (SCREEN_WIDTH / 4);
	//SDL_RenderCopy(renderer, MainMenuItemsTexture, &MainMenuItemsWRect, &MainMenuItemsTRect);

}


int FontLoaded = 0;
FC_Font* Subfont = FC_CreateFont();

void SDL_RenderDiabloMainPage() {
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
	char * filename = "/fonts/Exocet.ttf";
	strcat(LDirectory, filename);

	
	

	if (FontLoaded == 0){
		
				FC_LoadFont(Subfont, renderer, LDirectory, 12, FC_MakeColor(112, 106, 70, 255), TTF_STYLE_NORMAL);

		    printf("LoadFont\n\n");
			FontLoaded = 1;
	}
	FC_Draw(Subfont, renderer, 10, SCREEN_HEIGHT - 40, "DedicaTed To David Brevik, Erich Schaefer, Max Schaefer, MaTT Uelman and");
	FC_Draw(Subfont, renderer, 10, SCREEN_HEIGHT - 25, "The Blizzard North Team ThaT Gave Us A Childhood.");

	
	
	SDL_RenderPresent(renderer);

}



void LoadSinglePlayerMenuItems() {

	


	char LZDirectory[FILENAME_MAX];
	GetCurrentDir(LZDirectory, FILENAME_MAX);

	char * filename = "/Xresources/MenuSelectNewHero.png";

	strcat(LZDirectory, filename);

	SDL_Surface  * MenuSelectNewHeroSurface = IMG_Load(LZDirectory);
	MenuSelectNewHeroTexture = SDL_CreateTextureFromSurface(renderer, MenuSelectNewHeroSurface);
	SinglePlayerMenuItemsLoaded = 1;



}



void SDL_RenderDiabloSinglePlayerPage() {
	RenderDiabloLogo();
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);



	int cx, cy;
	SDL_GetMouseState(&cx, &cy);

	CusorLocation.x = cx;
	CusorLocation.y = cy;
	CusorLocation.h = 29;
	CusorLocation.w = 33;

	if (SinglePlayerMenuItemsLoaded == 0)
	{

		LoadSinglePlayerMenuItems();

	}

	SinglePlayerMenuCancelBox.x = 520;
	SinglePlayerMenuCancelBox.y = 454;
	SinglePlayerMenuCancelBox.w = 640;
	SinglePlayerMenuCancelBox.h = 480;
	/*

	X 520 , Y 454
	X 615 , Y 471
*/



	int MenuSelectNewHeroW  = 907;
	int MenuSelectNewHeroH  = 469;
	//W 907 H 469

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

	//SDL_QueryTexture(MenuSelectNewHeroTexture, NULL, NULL, &MenuSelectNewHeroW, &MenuSelectNewHeroH);

//	printf("W %d H %d \n", MenuSelectNewHeroW, MenuSelectNewHeroH);




	SDL_RenderCopy(renderer, MenuSelectNewHeroTexture, &MenuSelectNewHeroTRect, &MenuSelectNewHeroWRect);
	SDL_RenderCopy(renderer, DiablologoAnimT, &textureRect, &windowRect);
	SDL_RenderCopy(renderer, CursorTexture, NULL, &CusorLocation);
	
	//SDL_RenderPresent(renderer);

}

void LoadFont() {
	char LDirectory[FILENAME_MAX];
	GetCurrentDir(LDirectory, FILENAME_MAX);
	char * filename = "/fonts/Exocet.ttf";
	strcat(LDirectory, filename);


	font = FC_CreateFont();
	FC_LoadFont(font, renderer, LDirectory, 30, FC_MakeColor(112, 106, 70, 255), TTF_STYLE_NORMAL);

}

void LoadClickBoxes(int numberofchars) {
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

	//The menu doesn't fit past 6 chars.
	SDL_Rect Charpos7;
	SDL_Rect Charpos8;
	SDL_Rect Charpos9;


}





int LoadedFont = 0;
int TotalPlayers = 0;
void RenderCharNames() {
	if (LoadedFont == 0) {
		LoadFont();
		LoadedFont = 1;
	}

	
//	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	//X 355, Y 269
	int x = 364;
	int y = 240;
	TotalPlayers = 0; 
	for (int i = 0; i < 6; i++) {
		if (hero_names[i][0] != 0) {
			// Checking if section of array is empty. 
			//if array has something in this then draw name.
			for (int j = 0; j < 32; j++) {
				hero_names[i][j] = toupper(hero_names[i][j]);// making all the names uppercase...
				
			}



			const char * name = hero_names[i]; // getting the index of the name and drawing it to screen
			FC_Draw(font, renderer, x, y, name);

			
			y += 30;
			TotalPlayers++;
		}
		

	}
	
	
SDL_RenderPresent(renderer);


}
