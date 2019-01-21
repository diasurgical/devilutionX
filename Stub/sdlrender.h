#pragma once

#include "miniwin.h"

extern unsigned char *pFont16;
extern int gdwFont16Width;
extern int gdwFont16Height;
extern void *pPcxFont16sImage;
extern void *pPcxFont16gImage;

extern int gdwFont24Width;
extern int gdwFont24Height;
extern void *pPcxFont24sImage;
extern void *pPcxFont24gImage;
extern unsigned char *pFont24;

extern unsigned char *pFont30;
extern int gdwFont30Width;
extern int gdwFont30Height;
extern void *pPcxFont30sImage;
extern void *pPcxFont30gImage;

extern unsigned char *pFont42;
extern int gdwFont42Width;
extern int gdwFont42Height;
extern void *pPcxFont42gImage;

extern void *pTitlqtxtCel;
extern void *pDiabfrCel;

extern void *pPcxLogoImage;
extern int gdwLogoWidth;
extern int gdwLogoHeight;
extern void *pPcxLogoSmImage;
extern int gdwLogoSmWidth;
extern int gdwLogoSmHeight;

extern int gdwTitleWidth;
extern int gdwTitleHeight;
extern void *pPcxTitleImage;
extern int gdwCursorHeight;
extern int gdwCursorWidth;
extern void *pPcxCursorImage;
extern int gdwHeroHeight;
extern int gdwHeroWidth;
extern void *pPcxHeroImage;
extern int gdwSHeroHeight;
extern int gdwSHeroWidth;
extern void *pPcxSHeroImage;
extern void *pMedTextCels;

extern void *pPcxGameImage;
extern void *pPcxCreditsImage;
extern int gdwCreditsWidth;
extern int gdwCreditsHeight;

extern bool TitleImageLoaded;

extern int totalFrames;
//extern SDL_Texture* spriteSheet;
extern SDL_Surface *DiabloTitle;
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

extern bool StartNewGame;
extern bool CreateSinglePlayerChar;
extern int HeroChosen;

extern void *TitleMenuText;

extern void *MenuPentegram16;
extern void *MenuPentegram;
extern void *MenuPentegram42;

extern char HeroUndecidedName[17];

extern SDL_Rect SinglePlayerMenuCancelBox;
extern SDL_Rect CreateHeroBox;
void CreateMenuDialogBox();
void CreateDiabloMainMenuz();
void SdlDiabloMainWindow();
void RenderDiabloSplashPage();
void RenderDiabloMainPage();
char *GetWorkingDirectory();
void CreateMainDiabloMenu();
void RenderDiabloSinglePlayerPage();
void ShowCredts();
void RenderCharNames();
void FreeMenuItems();
BOOL __cdecl LoadArtImage(char *pszFile, void **pBuffer, int frames, DWORD *data);
void DrawMouse();
void DrawCursor(int mx, int my);
void CreateHeroMenu();
void RenderUndecidedHeroName(int HeroClass);
void LoadHeroStats();
void RenderDefaultStats(int HeroClass);
void DrawPreGameOptions(int ShowClasses);
void DrawPreGameDifficultySelection(int HeroClass, int ShowClasses);
int GetCenterOffset(int w, int bw = 0);
void DrawPCXString(int x, int y, int w, int h, char *str, BYTE *font, void *pBuff);
void DrawArtWithMask(int SX, int SY, int SW, int SH, int nFrame, BYTE bMask, void *pBuffer);
