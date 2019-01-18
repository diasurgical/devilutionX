#pragma once

#include <assert.h>
#include <vector>

#include <SDL2/SDL.h>
//#include <SDL_image.h>
#include <SDL_mixer.h>
//#include "SDL_FontCache.h"


#define SDL_CHECK(e) assert(e == 0)

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;
extern SDL_Surface *surface;
extern SDL_Palette *palette;
extern SDL_Surface *pal_surface;


void sdl_present_surface();

#ifdef __WINDOWS__
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif



extern int gdwFontWidth;
extern int gdwFontHeight;
extern void *pPcxFontImage;
extern unsigned char *pFont;


void sdl_update_entire_surface();

//My SDL inclusions // 

extern bool SorcerorCreateSelected;
extern bool WarriorCreateSelected;
extern bool RogueCreateSelected;


extern int totalFrames;
//extern SDL_Texture* spriteSheet;
extern SDL_Surface* DiabloTitle;
extern SDL_Event input;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int TotalPlayers;


extern SDL_Rect textureRect;
extern SDL_Rect windowRect;


//Menu0 //Main Menu rects
extern SDL_Rect SinglePlrBox;
extern SDL_Rect MultiPlrBox;
extern SDL_Rect ReplayIntroBox;
extern SDL_Rect ShowCreditsBox;
extern SDL_Rect ExitBox;

//

//CEL Gobals ?? Doesn't work?
extern void *BigTGold_cel;
extern void * pTitlqtxtCel;
extern void * pDiabfrCel ;
extern void *BigTGold_cel;
extern int gdwLogoWidth;
extern int gdwLogoHeight;
extern void *pPcxLogoImage;
extern void *pPanelText;
extern void * pTitlgrayCel_sgpBackCel;
extern int gdwTitleWidth;
extern int gdwTitleHeight;
extern void *pPcxTitleImage;
extern int gdwCursorHeight;
extern int gdwCursorWidth;
extern void * pPcxCursorImage;
extern int gdwHeroHeight;
extern int gdwHeroWidth;
extern void * pPcxHeroImage;
extern int gdwSHeroHeight;
extern int gdwSHeroWidth;
extern void * pPcxSHeroImage;
extern void * pMedTextCels;

extern void * pPcxGameImage;
extern void * pPcxCreditsImage;
extern int gdwCreditsWidth;
extern int gdwCreditsHeight;


extern int gdwFont2Width;
extern int gdwFont2Height;
extern void *pPcxFont2Image;
extern unsigned char *pFont2;

extern int gdwFont3Width;
extern int gdwFont3Height;
extern void *pPcxFont3Image;
extern unsigned char *pFont3;

extern char HeroUndecidedName[17];
extern bool StartNewGame;
extern bool CreateSinglePlayerChar;
extern int HeroChosen;

extern void * TitleMenuText; 

extern void * MenuPentegram;

//Menu2 // Single player menu rects
//extern static std::vector<_uiheroinfo> hero_infos;
extern SDL_Rect SinglePlayerMenuCancelBox;
extern SDL_Rect CreateHeroBox;
void CreateMenuDialogBox();
void CreateDiabloMainMenuz();
void SdlDiabloMainWindow();
void SDL_RenderDiabloMainPage();
char *GetWorkingDirectory();
void  CreateMainDiabloMenu();
void SDLCreateDiabloCursor();
void SDL_RenderDiabloSinglePlayerPage();
void ShowCredts();
void RenderCharNames();
void SDL_Diablo_UI();
void FreeMenuItems();
void DrawMouse();
void DrawCursor(int mx, int my);
void CreateHeroMenu();
void DrawNewHeroKartinka(int image , int ShowClasses);
void ConstantButtons();
void RenderUndecidedHeroName();
void LoadHeroStats();
void RenderDefaultStats(int HeroChosen);
void DrawPreGameOptions(int image , int ShowClasses);
void DrawPreGameDifficultySelection(int image, int ShowClasses);




typedef unsigned char BYTE;
void DrawPCXString(int x, int y, int w, int h, char *str, BYTE *font, void *pBuff);
void DrawArtWithMask(int SX, int SY, int SW, int SH, int nFrame, BYTE bMask, void *pBuffer);