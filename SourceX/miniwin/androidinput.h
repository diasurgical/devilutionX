#pragma once
#include <SDL.h>
#include <SDL_image.h>


namespace dvl {

	/* START ANDROID INPUT FUNKTIONS */
	#ifdef __ANDROID__
	void DisplayJoyStick(int x , int y, int h, int w, SDL_Surface * SrcSurface, SDL_Surface * DstSurface);
	void LoadAndroidImages();
	void DrawDPAD(int MouseX, int MouseY, bool flag);
	void walkInDir(int dir);
	void __fastcall checkTownersNearby(bool interact);
	void DrawAndroidUI();
	bool __fastcall checkMonstersNearby(bool attack, bool spellcast);
	void convert_touch_xy_to_game_xy(float touch_x, float touch_y, int *game_x, int *game_y);
	//void __fastcall checkItemsNearby(bool interact);
	void AutoPickGold(int pnum);
	void AutoPickItem(int pnum);
	void ActivateObject(bool interact);
	void __fastcall checkItemsNearby(int pnum);
	void useBeltPotion(bool mana);
	

	extern bool showJoystick;
	extern bool JoyStickInitalPressSet;
	extern bool JoyStickCTRL;
	extern int TouchX;
	extern int TouchY;
	extern bool DemoModeEnabled;
	extern void PerformDPADMovement();

	extern SDL_Surface * JoyStickS;
	extern SDL_Texture * JoyStickT;
	extern SDL_Surface * AJoyStickS;
	extern SDL_Texture * AJoyStickT;
	extern SDL_Surface * ShiftStickS;
	extern SDL_Texture * ShiftStickT;
	extern SDL_Surface * DemoSqS;
	extern SDL_Texture * DemoSqT;
	extern SDL_Surface * Fog ;
	extern SDL_Texture * gFog ;
	extern SDL_Texture * JoystickCircleT;
	extern bool ShiftButtonPressed ;
	extern bool AttackButtonPressed;
	extern bool CastButtonPressed;
	extern bool change_controlPressed;

	//extern bool JoyStickCTRL;
	
// Defined in plrctrls.cpp / h	
	typedef struct androidcoords {
	int x;
	int y;
} androidcoords;
extern androidcoords androidspeedspellscoords[50];

extern SDL_Rect Arect;
extern SDL_Rect Shiftrect;
extern SDL_Rect LGameUIMenu;
extern SDL_Rect RGameUIMenu;
extern SDL_Rect PotGameUIMenu;
extern SDL_Rect Crect;

extern SDL_Rect DemoN;
extern SDL_Rect DemoS;
extern SDL_Rect DemoE;
extern SDL_Rect DemoW;

extern SDL_Rect DemoNW;
extern SDL_Rect DemoNE;	
extern SDL_Rect DemoSW;
extern SDL_Rect DemoSE;

extern SDL_Rect DemonHealth;
extern SDL_Rect AngelMana;

extern SDL_Rect JoyStickBox;
extern SDL_Rect Change_controlsRect;
extern SDL_Rect JoystickCircleRect;
void RenderJoyStick(int X , int Y  );

#endif

	/*Finish */

} // namespace dv
