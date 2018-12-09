#include "../types.h"
#include "miniwin_sdl.h"
#include "stubs.h"

#include "SDL_FontCache.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

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

FC_Font *Subfont = FC_CreateFont();

SDL_Texture *CreditsTexture;
SDL_Texture *MainMenuItemsTexture;
SDL_Texture *DiablologoAnimT;
SDL_Texture *CursorTexture;
SDL_Texture *MenuSelectNewHeroTexture;
SDL_Texture *CreateHeroDialogTextureW;
SDL_Texture *CreateHeroDialogTextureR;
SDL_Texture *CreateHeroDialogTextureS;

SDL_Surface *CreditsSurface;
SDL_Surface *MainMenuItemsSurface;
SDL_Surface *MenuSelectNewHeroSurface;
SDL_Surface *CreateHeroDialogSurface;
SDL_Surface *CursorImg;
SDL_Surface *DiabloTitle;

FC_Font *font;
////////////////
//new vars

int gb_Lfont_str_len;
int gb_Lfont_pix_width;
int the_pcx_frame;









/////////////////////////////////////////

void FreeMenuItems()
{

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

}

void SdlDiabloMainWindow()
{

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Diablo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
	                          SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);
	printf("Window And Renderer Created!\n");

	const int pitch = 640 + 64 + 64;

	surface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	assert(surface);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
	assert(texture);

	palette = SDL_AllocPalette(256);

	lock_buf_priv();

}



void DrawArtImage(int SX, int SY, int SW, int SH, int nFrame, void *pBuffer)
{
	BYTE *src = (BYTE *)pBuffer+(SW * SH * nFrame);
	BYTE *dst = (BYTE *)&gpBuffer->row[SY].pixels[SX];

	for(int i = 0; i < SH; i++, src += SW, dst += 768)
	{
		for(int j = 0; j < SW; j++)
		{
			dst[j] = src[j];
		}
	}
}




void __fastcall print_title_str_large(int a1, int a2, char *a3)
{
	assert(BigTGold_cel);
	int screen_y; // [esp+Ch] [ebp-14h]
	int screen_x; // [esp+10h] [ebp-10h]
	char v5; // [esp+14h] [ebp-Ch]
	signed int i; // [esp+18h] [ebp-8h]
	signed int v7; // [esp+1Ch] [ebp-4h]

	screen_y = a2;
	screen_x = a1;
	v7 = strlen(a3);
	for ( i = 0; v7 > i; ++i )
	{
		v5 = lfontframe[fontidx[a3[i]]];
		if ( v5 )
			CelDecodeOnly(screen_x, screen_y, BigTGold_cel, v5, 46);
		screen_x += lfontkern[v5] + 2;
	}
	gb_Lfont_pix_width = screen_x;
	gb_Lfont_str_len = v7;
}


void __fastcall print_title_str_small(int a1, int a2, char *a3)
{
	int screen_y; // [esp+Ch] [ebp-14h]
	int screen_x; // [esp+10h] [ebp-10h]
	char v5; // [esp+14h] [ebp-Ch]
	signed int i; // [esp+18h] [ebp-8h]
	signed int v7; // [esp+1Ch] [ebp-4h]

	screen_y = a2;
	screen_x = a1;
	v7 = strlen(a3);
	for ( i = 0; i < v7; ++i )
	{
		v5 = mfontframe[fontidx[a3[i]]];
		if ( v5 )
			CelDecodeOnly(screen_x, screen_y, pMedTextCels, v5, 22);
		screen_x += mfontkern[v5] + 2;
	}
}






void LoadDiabloMenuLogoImage()
{




}

void DiabloMainMenuItemsLoaded()
{


}

void CreateMainDiabloMenu()
{



}



void DrawArtWithMask(int SX, int SY, int SW, int SH, int nFrame, BYTE bMask, void *pBuffer)
{
	BYTE *src = (BYTE *)pBuffer+(SW * SH * nFrame);
	BYTE *dst = (BYTE *)&gpBuffer->row[SY].pixels[SX];

	for(int i = 0; i < SH; i++, src += SW, dst += 768)
	{
		for(int j = 0; j < SW; j++)
		{
			if(src[j] != bMask)
				dst[j] = src[j];
		}
	}
}


void RenderDiabloLogo()
{
	int	diablogo_cel_frame = 1;

		
	
	int MyPcxDelay = 60;
	int MyPcxFRAME  = (SDL_GetTicks() / MyPcxDelay) % 15;
		if ( ++MyPcxFRAME == 15 ){
		MyPcxFRAME = 1;
	}

	DrawArtWithMask(320-(gdwLogoWidth/2), 0, gdwLogoWidth, gdwLogoHeight, MyPcxFRAME, 250, pPcxLogoImage);


	if ( ++diablogo_cel_frame == 29 ){
		diablogo_cel_frame = 1;

	}
		




}


void DrawCursor(int mx, int my){
	SDL_GetMouseState(&mx, &my);

int lines = gdwCursorHeight;



				


DrawArtWithMask(mx, my, gdwCursorWidth, lines, 0, 0, pPcxCursorImage);
unlock_buf_priv();

}


void DrawMouse()
{
	
	int lines = gdwCursorHeight;
	// if(MouseY > 480-gdwCursorHeight)
	// 	lines -= (MouseY - (480-gdwCursorHeight));
	// int mx = MouseX;
	// if(mx < 0) mx = 0;
	// if(mx >639) mx = 639;
	// int my = MouseY;
	// if(my < 0) my = 0;
	// if(my > 480) my = 480;

	

	lock_buf_priv();
	DrawArtWithMask(MouseX, MouseY, gdwCursorWidth, lines, 0, 0, pPcxCursorImage);
	unlock_buf_priv();
}



void SDL_RenderDiabloMainPage()
{
	DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);

	int totalPentFrames = 9;
	int PentdelayPerFrame = 60;
	int Pentframe = (SDL_GetTicks() / PentdelayPerFrame) % totalPentFrames;

	if ( ++Pentframe == 9 ){
		Pentframe = 1;
	}
				


	int PentPositionX1 =  (SCREEN_WIDTH  / 2) - (SCREEN_WIDTH *0.28) ;  
	int PentPositionY1 = ( SCREEN_HEIGHT   /2 )+ ( SCREEN_HEIGHT  * 0.1);
	int PentPositionX2 =  (SCREEN_WIDTH  / 2) - (SCREEN_WIDTH *0.75) ;
	int PentPositionY2 = ( SCREEN_HEIGHT   /2 )+ ( SCREEN_HEIGHT  * 0.1);

	

	//scrollrt_draw_cursor_back_buffer(); // Doesn't work?


	RenderDiabloLogo();



	//CelDecodeOnly(236, 282, pDiabfrCel, 1, 296);//CelDecodeOnly(64, 377, pDiabfrCel, diablogo_cel_frame, 640);
	//CelDecodeOnly(PentPositionX1, PentPositionY1, pTitlgrayCel_sgpBackCel ,1,  48);

	CelDecodeOnly(64, 639, pTitlgrayCel_sgpBackCel, 1, 640);
	gmenu_print_text( (SCREEN_WIDTH  / 2) - (SCREEN_WIDTH *0.2), ( SCREEN_HEIGHT   /2 )+ ( SCREEN_HEIGHT  * 0.1), "Single Player");
	gmenu_print_text( (SCREEN_WIDTH  / 2) - (SCREEN_WIDTH *0.2), ( SCREEN_HEIGHT   /2 )+ ( SCREEN_HEIGHT  * 0.2), "Multi Player");
	gmenu_print_text( (SCREEN_WIDTH  / 2) - (SCREEN_WIDTH *0.2), ( SCREEN_HEIGHT   /2 )+ ( SCREEN_HEIGHT  * 0.3), "Show Replay");
	gmenu_print_text( (SCREEN_WIDTH  / 2) - (SCREEN_WIDTH *0.2), ( SCREEN_HEIGHT   /2 )+ ( SCREEN_HEIGHT  * 0.4), "Play Credits");
	gmenu_print_text( (SCREEN_WIDTH  / 2) - (SCREEN_WIDTH *0.2), ( SCREEN_HEIGHT   /2 )+ ( SCREEN_HEIGHT  * 0.5), "Exit  Diablo");

	CelDecodeOnly(PentPositionX1, PentPositionY1, PentSpin_cel, (int ) Pentframe, 48);
	CelDecodeOnly(PentPositionX2, PentPositionY2, PentSpin_cel, (int ) Pentframe, 48);
	
	ADD_PlrStringXY(0, 600-150, 640, "DedicaTed To David Brevik, Erich Schaefer, Max Schaefer,", COL_GOLD);
	ADD_PlrStringXY(0, 600-130, 640, " MaTT Uelman, and The Blizzard North Team ThaT Gave Us A Childhood.", COL_GOLD);


	
	
	

}

void CreateNewHeroClickBox() {}

void LoadSinglePlayerMenuItems()
{

}

void SDL_RenderDiabloSinglePlayerPage()
{

DrawArtImage(0, 0, gdwTitleWidth, gdwTitleHeight, 0, pPcxTitleImage);
DrawArtImage(0, 0, gdwSHeroWidth, gdwSHeroHeight, 0, pPcxGameImage);
RenderDiabloLogo();
RenderCharNames();




}

void LoadFont()
{

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
			// 	const char *hero_name = hero_infos[0].name;
			// DUMMY_PRINT("use hero: %s", hero_name);
			// strcpy(name, hero_name);
			// *dlgresult = 2;

	// X 355, Y 269
	int x = 440;
	int y = 430;
	TotalPlayers = 0;
	for (int i = 0; i < 6; i++) {
		if (hero_names[i][0] != 0) {
			// Checking if section of array is empty.
			// if array has something in this then draw name.
			print_title_str_small(x, y,  hero_names[i]);

			y += 35;
			TotalPlayers++;
		}
	}

	//SDL_RenderPresent(renderer);
}















bool LoadCreateHeroDialogImages = 0;
bool SorcerorCreateSelected = 0;
bool RogueCreateSelected = 0;
bool WarriorCreateSelected = 1;

void LoadCreateHeroDialogMenu()
{
	// GetWorkingLocationOfFile("/Xresources/warriorcreate.bmp");

	// CreateHeroDialogSurface = IMG_Load(gLDirectory);
	// CreateHeroDialogTextureW = SDL_CreateTextureFromSurface(renderer, CreateHeroDialogSurface);

	// GetWorkingLocationOfFile("/Xresources/roguecreate.bmp");
	// CreateHeroDialogSurface = IMG_Load(gLDirectory);
	// CreateHeroDialogTextureR = SDL_CreateTextureFromSurface(renderer, CreateHeroDialogSurface);

	// GetWorkingLocationOfFile("/Xresources/sorcerorcreate.bmp");
	// CreateHeroDialogSurface = IMG_Load(gLDirectory);
	// CreateHeroDialogTextureS = SDL_CreateTextureFromSurface(renderer, CreateHeroDialogSurface);

	// LoadCreateHeroDialogImages = 1;
}
// Have this load the function above and then render it in the main menu.
// Cnacel box is also needed.
void CreateMenuDialogBox()
{

	// RenderDiabloLogo();

	// int cx, cy;
	// SDL_GetMouseState(&cx, &cy);

	// CusorLocation.x = cx;
	// CusorLocation.y = cy;
	// CusorLocation.h = 29;
	// CusorLocation.w = 33;

	// if (LoadCreateHeroDialogImages == 0) {
	// 	LoadCreateHeroDialogMenu();
	// }

	// SDL_Rect MenuSelectNewHeroWRect;
	// MenuSelectNewHeroWRect.x = 0;
	// MenuSelectNewHeroWRect.y = 140;
	// MenuSelectNewHeroWRect.w = 640;
	// MenuSelectNewHeroWRect.h = 350;

	// SDL_Rect MenuSelectNewHeroTRect;
	// MenuSelectNewHeroTRect.x = 0;
	// MenuSelectNewHeroTRect.y = 0;
	// MenuSelectNewHeroTRect.w = 907;
	// MenuSelectNewHeroTRect.h = 500;

	// if (WarriorCreateSelected) {
	// 	SDL_RenderCopy(renderer, CreateHeroDialogTextureW, &MenuSelectNewHeroTRect, NULL);
	// 	//	printf("WarriorCreateSelected\n");
	// }
	// if (RogueCreateSelected) {
	// 	SDL_RenderCopy(renderer, CreateHeroDialogTextureR, &MenuSelectNewHeroTRect, NULL);
	// 	//	printf("RogueCreateSelected\n");
	// }
	// if (SorcerorCreateSelected) {
	// 	SDL_RenderCopy(renderer, CreateHeroDialogTextureS, &MenuSelectNewHeroTRect, NULL);
	// 	// printf("SorcerorCreateSelected\n");
	// }

	// SDL_RenderCopy(renderer, DiablologoAnimT, &textureRect, &windowRect);
	// SDL_RenderCopy(renderer, CursorTexture, NULL, &CusorLocation);
	// SDL_RenderPresent(renderer);
}