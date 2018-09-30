#pragma once

#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "SDL_FontCache.h"


#define SDL_CHECK(e) assert(e == 0)

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *texture;


#ifdef __WINDOWS__
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif



//My SDL inclusions // 

extern int totalFrames;
//extern SDL_Texture* spriteSheet;
extern SDL_Surface* DiabloTitle;
extern SDL_Event input;
extern FC_Font* font;
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

//Menu2 // Single player menu rects
extern SDL_Rect SinglePlayerMenuCancelBox;


void CreateDiabloMainMenuz();
void SdlDiabloMainWindow();
void SDL_RenderDiabloMainPage();
char *GetWorkingDirectory();
void  CreateMainDiabloMenu();
void SDLCreateDiabloCursor();
void SDL_RenderDiabloSinglePlayerPage();
void RenderCharNames();