
#include "devilution.h"
#include "miniwin/ddraw.h"
#include "stubs.h"
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>

#include "DiabloUI/diabloui.h"

namespace dvl {
	 SDL_Surface * JoyStickS;
	 SDL_Texture * JoyStickT;
	 SDL_Surface * AJoyStickS;
	 SDL_Texture * AJoyStickT;
	 SDL_Surface * CJoyStickS;
	 SDL_Texture * CJoyStickT;
	 SDL_Surface * ShiftStickS;
	 SDL_Texture * ShiftStickT;

	 SDL_Surface * DemoSqS;
	 SDL_Texture * DemoSqT;

	 SDL_Surface * Fog  = NULL;
	 SDL_Texture * gFog = NULL;



void FocusOnCharInfo()
{
	if (invflag || plr[myplr]._pStatPts <= 0)
		return;

	// Find the first incrementable stat.
	int pc = plr[myplr]._pClass;
	int stat = -1;
	for (int i = 4; i >= 0; --i) {
		switch (i) {
		case ATTRIB_STR:
			if (plr[myplr]._pBaseStr >= MaxStats[pc][ATTRIB_STR])
				continue;
			break;
		case ATTRIB_MAG:
			if (plr[myplr]._pBaseMag >= MaxStats[pc][ATTRIB_MAG])
				continue;
			break;
		case ATTRIB_DEX:
			if (plr[myplr]._pBaseDex >= MaxStats[pc][ATTRIB_DEX])
				continue;
			break;
		case ATTRIB_VIT:
			if (plr[myplr]._pBaseVit >= MaxStats[pc][ATTRIB_VIT])
				continue;
			break;
		default:
			continue;
		}
		stat = i;
	}
	if (stat == -1)
		return;
	const auto &rect = ChrBtnsRect[stat];
	SetCursorPos(rect.x + (rect.w / 2), rect.y + (rect.h / 2));
}



WINBOOL SetCursorPos(int X, int Y)
{
	assert(window);

	if (renderer) {
		SDL_Rect view;
		SDL_RenderGetViewport(renderer, &view);
		X += view.x;
		Y += view.y;

		float scaleX;
		SDL_RenderGetScale(renderer, &scaleX, NULL);
		X *= scaleX;
		Y *= scaleX;
	}

	SDL_WarpMouseInWindow(window, X, Y);
	return true;
}




androidcoords androidspeedspellscoords[50];
bool ShiftButtonPressed = 0;
bool DemoModeEnabled=false;

 
 
 //x , y , w, h 
SDL_Rect PotGameUIMenu ={195,350,245,50};
SDL_Rect RGameUIMenu   ={555,350,85,130};
SDL_Rect LGameUIMenu   ={0,350,85,130};
SDL_Rect Arect         ={520,250,100,95};
SDL_Rect Crect         ={560,180,90,80};
SDL_Rect Shiftrect     ={58,257,57,52};
SDL_Rect Jrect         ={1,200,170,170};

//x , y , w, h 
SDL_Rect DemoN ={68,202,57,52}; 
SDL_Rect DemoE ={130,257,67,52};   
SDL_Rect DemoW ={0,257,67,52};		
SDL_Rect DemoS ={68,315,57,52};  

//x , y , w, h 
SDL_Rect DemoNW={0,204,67,52};
SDL_Rect DemoNE={130,204,67,52};	
SDL_Rect DemoSW={0,315,67,52};
SDL_Rect DemoSE={130,315,67,52};


SDL_Rect DemonHealth={100,350,85,130};
SDL_Rect AngelMana={460,350,85,130};



bool AttackButtonPressed;
bool CastButtonPressed;
bool gbAndroidInterfaceLoaded = 0;	   
bool showJoystick = true;
bool newCurHidden = false;

bool DemoMode = false;
//			SDL_BlitSurface(surface, &source_rect, temp_surface, NULL);
            // Figure out transparency.
           


 void LoadAndroidImages(){
	 IMG_Init(IMG_INIT_PNG);

	//https://image.flaticon.com/icons/png/512/54/54528.png
	//Loading Attack buttons
	 AJoyStickS = IMG_Load("/sdcard/devilutionx/input_attack.png");
     AJoyStickT = SDL_CreateTextureFromSurface(renderer, AJoyStickS);
	 SDL_SetTextureBlendMode(AJoyStickT, SDL_BLENDMODE_BLEND);
	 SDL_SetTextureAlphaMod(AJoyStickT, 150);

	 //Load CastButtons.
	 CJoyStickS = IMG_Load("/sdcard/devilutionx/input_cast.png");
     CJoyStickT = SDL_CreateTextureFromSurface(renderer, CJoyStickS);
	 SDL_SetTextureBlendMode(CJoyStickT, SDL_BLENDMODE_BLEND);
	 SDL_SetTextureAlphaMod(CJoyStickT, 150);
	 
	 //https://upload.wikimedia.org/wikipedia/commons/thumb/9/98/Crossed_circle.svg/1200px-Crossed_circle.svg.png
	 ShiftStickS = IMG_Load("/sdcard/devilutionx/shift.png");
     ShiftStickT = SDL_CreateTextureFromSurface(renderer, ShiftStickS);
	 SDL_SetTextureBlendMode(ShiftStickT, SDL_BLENDMODE_BLEND);
	 SDL_SetTextureAlphaMod(ShiftStickT, 255);


	 //Loading Walking Joystick.
	 JoyStickS = IMG_Load("/sdcard/devilutionx/dpad.png");
     JoyStickT = SDL_CreateTextureFromSurface(renderer, JoyStickS);
	 SDL_SetTextureBlendMode(JoyStickT, SDL_BLENDMODE_BLEND);
	 SDL_SetTextureAlphaMod(JoyStickT, 75);


	DemoSqS = IMG_Load("/sdcard/devilutionx/demosq.png");
	DemoSqT = SDL_CreateTextureFromSurface(renderer, DemoSqS);
	SDL_SetTextureBlendMode(DemoSqT, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(DemoSqT, 255);

    bool gbAndroidInterfaceLoaded = true;
 }



void HideCursor()
{
	if (pcurs >= CURSOR_FIRSTITEM) // if we don't drop the item on cursor, it will be destroyed
		DropItemBeforeTrig();
	SetCursorPos(320, 180);
	SetCursor_(CURSOR_NONE); // works?
	newCurHidden = true;
}


void DrawJoyStick(int MouseX, int MouseY, bool flag){



		 //TX 181, TY 309 MX 181 MY 309
	SDL_RenderCopy(renderer, JoyStickT, NULL, &Jrect);
	if(DemoModeEnabled){
	


	//Dpad
	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemoE);
	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemoW); 
	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemoS);
	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemoN); 

	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemoNE);
	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemoNW);
	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemoSE);
	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemoSW);

	SDL_RenderCopy(renderer, DemoSqT, NULL, &LGameUIMenu);
	SDL_RenderCopy(renderer, DemoSqT, NULL, &RGameUIMenu);
	SDL_RenderCopy(renderer, DemoSqT, NULL, &PotGameUIMenu);

	SDL_RenderCopy(renderer, DemoSqT, NULL, &DemonHealth);
	SDL_RenderCopy(renderer, DemoSqT, NULL, &AngelMana);
	}

//LGameUIMenu

}


void walkInDir(int dir)
{
	if (invflag || spselflag || chrflag) // don't walk if inventory, speedbook or char info windows are open
		return;
	// ticks = GetTickCount();
	// if (ticks - invmove < 370) {
	// 	return;
	// }
	//invmove = ticks;
	ClrPlrPath(myplr);                   // clear nodes
	plr[myplr].destAction = ACTION_NONE; // stop attacking, etc.
//	HideCursor();
//	SetCursor_(0);
	plr[myplr].walkpath[0] = dir;
}


androidcoords checkNearbyObjs(int x, int y, int diff)
{
	int diff_x = abs(plr[myplr]._px - x);
	int diff_y = abs(plr[myplr]._py - y);

	if (diff_x <= diff && diff_y <= diff) {
		androidcoords cm = { diff_x, diff_y };
		//sprintf(tempstr, "N-DIFF X:%i Y:%i", diff_x, diff_y);
		//NetSendCmdString(1 << myplr, tempstr);
		return cm;
	}
	return { -1, -1 };
}




void __fastcall checkTownersNearby(bool interact)
{
	for (int i = 0; i < 16; i++) {
		if (checkNearbyObjs(towner[i]._tx, towner[i]._ty, 2).x != -1) {

			if (towner[i]._ttype == -1)
				continue;
			pcursmonst = i;
			if (interact) {
				plr[myplr].destAction = ACTION_NONE;
				TalkToTowner(myplr, i);
			}
			break;
		}
	}
}


void DrawAndroidUI(){
	if(!invflag && !spselflag && !chrflag && !stextflag && !questlog && !helpflag && !talkflag && !qtextflag && !sgpCurrentMenu && gbRunGame){
		//if(showJoystick){
			DrawJoyStick(MouseX, MouseY, true);
			


			if (AttackButtonPressed){
				SDL_SetTextureColorMod(ShiftStickT, 255, 0, 0);
				SDL_RenderCopy(renderer, AJoyStickT, NULL, &Arect);
			}
			else{
				SDL_SetTextureColorMod(ShiftStickT, 255, 0, 0);
				SDL_RenderCopy(renderer, AJoyStickT, NULL, &Arect);
				if (DemoModeEnabled){
					    // attack
						SDL_RenderCopy(renderer, DemoSqT, NULL, &Arect);
				}
			}


			if(CastButtonPressed){
				SDL_SetTextureColorMod(ShiftStickT, 255, 0, 0);
				SDL_RenderCopy(renderer, CJoyStickT, NULL, &Crect);

			}
			else{
				SDL_RenderCopy(renderer, CJoyStickT, NULL, &Crect);
				if (DemoModeEnabled){
					    // attack
						SDL_RenderCopy(renderer, DemoSqT, NULL, &Crect);
				}
				
			}





		if (ShiftButtonPressed){
			SDL_SetTextureColorMod(ShiftStickT, 255, 0, 0);
			SDL_RenderCopy(renderer, ShiftStickT, NULL, &Shiftrect);
		}
		else{
			SDL_SetTextureColorMod(ShiftStickT, 220,220,220);
			SDL_RenderCopy(renderer, ShiftStickT, NULL, &Shiftrect);
			if(DemoModeEnabled){
				SDL_RenderCopy(renderer, DemoSqT, NULL, &Shiftrect);
			}
			
		}
	}	
}



bool __fastcall checkMonstersNearby(bool attack,bool spellcast)
{
	int rspell, sd, sl;
	sl = GetSpellLevel(myplr, plr[myplr]._pRSpell);


	int closest = 0;                 // monster ID who is closest
	androidcoords objDistLast = { 135, 135 }; // previous obj distance
	for (int i = 0; i < MAXMONSTERS; i++) {
		int d_monster = dMonster[monster[i]._mx][monster[i]._my];
		if (monster[i]._mFlags & MFLAG_HIDDEN || monster[i]._mhitpoints <= 0) // monster is hiding or dead, skip
			continue;
		if (d_monster && dFlags[monster[i]._mx][monster[i]._my] & BFLAG_LIT) {                                                                          // is monster visible
			if (monster[i].MData->mSelFlag & 1 || monster[i].MData->mSelFlag & 2 || monster[i].MData->mSelFlag & 3 || monster[i].MData->mSelFlag & 4) { // is monster selectable
				androidcoords objDist = checkNearbyObjs(monster[i]._mx, monster[i]._my, 6);
				if (objDist.x > -1 && objDist.x <= objDistLast.x && objDist.y <= objDistLast.y) {
					closest = i;
					objDistLast = objDist;
				}
			}
		}
	}
	if (closest > 0) { // did we find a monster
		pcursmonst = closest;
		//sprintf(tempstr, "NEARBY MONSTER WITH HP:%i", monster[closest]._mhitpoints);
		//NetSendCmdString(1 << myplr, tempstr);
	} else if (closest > 0) { // found monster, but we don't want to attack it
		return true;
	} else {
		checkItemsNearby(true);
	//	LeftMouseCmd(true);// I would like SHIFT ATTACK To exist...
		pcursmonst = -1;
		return false;
	}
	if (attack) {
		// if (ticks - attacktick > 100) { // prevent accidental double attacks
		// 	attacktick = ticks;
		if(ShiftButtonPressed){
			if(spellcast){
			sd = GetDirection(plr[myplr].WorldX, plr[myplr].WorldY, monster[pcursmonst]._mx, monster[pcursmonst]._my);
			sl = GetSpellLevel(myplr, plr[myplr]._pRSpell);
			NetSendCmdLocParam3(true, CMD_SPELLXYD, monster[pcursmonst]._mx, monster[pcursmonst]._my, plr[myplr]._pRSpell, sd, sl); //CAST SPELL
			}
			else{
			LeftMouseCmd(true);
			}
			
		}else{
			if(spellcast){
			sd = GetDirection(plr[myplr].WorldX, plr[myplr].WorldY, monster[pcursmonst]._mx, monster[pcursmonst]._my);
			sl = GetSpellLevel(myplr, plr[myplr]._pRSpell);
			NetSendCmdLocParam3(true, CMD_SPELLXYD, monster[pcursmonst]._mx, monster[pcursmonst]._my, plr[myplr]._pRSpell, sd, sl); //CAST SPELL
			}
			else{
			LeftMouseCmd(false);
			}
		}

				
		// }
		return true;
	} else {
		return true;
	}
	pcursmonst = -1;
	return false;
}


void AutoPickGold(int pnum) {
	PlayerStruct& player = plr[pnum];
	if (invflag) return;
	for (int orient = 0; orient < 9; ++orient) {
		int row = player.WorldX + pathxdir[orient];
		int col = player.WorldY + pathydir[orient];
		int itemIndex = dItem[row][col] - 1;
		if (itemIndex > -1) {
			char* pcursitem = (char*)&item;
			ItemStruct* it = &(item[itemIndex]);
			if (it->_itype == ITYPE_GOLD) {
				NetSendCmdGItem(1u, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
				item[itemIndex]._iRequest = 1;
				//dItem[row][col] = 0;
				PlaySFX(68);
				//AddPanelString("Item Picked Up! ", true);
				  char tempstr[255] = {0};
   				  sprintf(tempstr, "Item Picked Up! %s",it->_iName );
				  DrawInvMsg(tempstr);

			}
		}
	}
}

void AutoPickItem(int pnum){
//if (GetConfigBoolVariable("autoPickGold") == false) { return; }
	PlayerStruct& player = plr[pnum];
	if (invflag) return;
	for (int orient = 0; orient < 9; ++orient) {
		int row = player.WorldX; //+ pathxdir[orient];
		int col = player.WorldY; //+ pathydir[orient];

		int itemIndex = dItem[row][col] - 1;
		if (itemIndex > -1) {
			char* pcursitem = (char*)&item;
			ItemStruct* it = &(item[itemIndex]);
			if (it->_itype != ITYPE_NONE || it->_itype != ITYPE_0E ) {
				NetSendCmdGItem(1u, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
				item[itemIndex]._iRequest = 1;
				//dItem[row][col] = 0;
				PlaySFX(68);
     			//AddPanelString("Item Picked up! ", true);

				  char tempstr[255] = {0};
   				  sprintf(tempstr, "Item Picked Up! %s",it->_iName );
				  DrawInvMsg(tempstr);

			}
		}
	}
}


void useBeltPotion(bool mana)
{
	int invNum = 0;
	// if (ticks - menuopenslow < 300) {
	// 	return;
	// }
	//menuopenslow = ticks;
	for (int i = 0; i < MAXBELTITEMS; i++) {
		if ((AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_HEAL && mana == false) || 
		   (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_FULLHEAL && mana == false) || 
		   (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_REJUV && mana == false) || 
		   (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_FULLREJUV && mana == false) || 

		   (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_MANA && mana == true) || 
		   (AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_FULLMANA && mana == true) 
		   //(AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_REJUV && AllItemsList[plr[myplr].SpdList[i].IDidx].iMiscId == IMISC_FULLREJUV)
		   
		   ) {
			if (plr[myplr].SpdList[i]._itype > -1) {
				invNum = i + INVITEM_BELT_FIRST;
				UseInvItem(myplr, invNum);
				break;
			}
		}
	}
}


/*

typedef enum direction {
	DIR_S    = 0x0,
	DIR_SW   = 0x1,
	DIR_W    = 0x2,
	DIR_NW   = 0x3,
	DIR_N    = 0x4,
	DIR_NE   = 0x5,
	DIR_E    = 0x6,
	DIR_SE   = 0x7,
	DIR_OMNI = 0x8,
} direction;
 */

void ActivateObjectz(bool interact){
	for (int i = 0; i < MAXOBJECTS; i++) {
		if (checkNearbyObjs(object[i]._ox, object[i]._oy, 1).x != -1 && object[i]._oSelFlag > 0 && object[i]._otype > -1 && currlevel) { // make sure we're in the dungeon to scan for objs
			pcursobj = i;
			if (interact) {
				LeftMouseCmd(false);
			}
		}
	}
}

/**
 * if (monster[i].MData->mSelFlag & 1 || monster[i].MData->mSelFlag & 2 || monster[i].MData->mSelFlag & 3 || monster[i].MData->mSelFlag & 4) { // is monster selectable
				coords objDist = checkNearbyObjs(monster[i]._mx, monster[i]._my, 6);
				if (objDist.x > -1 && objDist.x <= objDistLast.x && objDist.y <= objDistLast.y) {
					closest = i;
					objDistLast = objDist;
				}
			}

 */

//coords objDist = checkNearbyObjs(monster[i]._mx, monster[i]._my, 6);



int diff_ms(timeval t1, timeval t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) + 
            (t1.tv_usec - t2.tv_usec))/1000;
}




void ActivateObject(bool interact){ // I think this function is dirty, but it does what I want.
	int closest = 0;     
	androidcoords objDistLast = { 10, 10 }; // previous obj distance
	for (int i = 0; i < MAXOBJECTS; i++) {
		
		if (checkNearbyObjs(object[i]._ox, object[i]._oy, 1).x != -1 && object[i]._oSelFlag > 0 && object[i]._otype > -1 && currlevel) { // make sure we're in the dungeon to scan for objs
			pcursobj = i;
			androidcoords objDist = checkNearbyObjs(object[i]._ox, object[i]._oy, 6);
			
			if (objDist.x > -1 && objDist.x <= objDistLast.x && objDist.y <= objDistLast.y) {
					closest = i;
					//objDistLast = objDist;
					if (interact) {
						NetSendCmdLocParam1(true, pcurs == CURSOR_DISARM ? CMD_DISARMXY : CMD_OPOBJXY,  object[closest]._ox, object[closest]._oy, pcursobj);
						Sleep(20); // might be good to remove this thing.
					}


				}

			
		}
	}
}



//ObjectStruct object[MAXOBJECTS];


void __fastcall checkItemsNearby(int pnum)
{
	PlayerStruct& player = plr[pnum];
	for (int orient = 0; orient < 9; ++orient) {
		int row = player.WorldX + pathxdir[orient];
		int col = player.WorldY + pathydir[orient];
		checkNearbyObjs(row,col,1);
				  //char tempstr[255] = {0};
   				  //sprintf(tempstr, "Activated!");
				  //DrawInvMsg(tempstr);
				  //NetSendCmdGItem(1u, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
				  //OperateObject(pnum, int i, BOOL TeleFlag)
				  SetCursorPos(item[orient]._ix, item[orient]._iy);
				  LeftMouseCmd(false);

			
		}
}

template<typename T> inline T CLIP(T v, T amin, T amax)
	{ if (v < amin) return amin; else if (v > amax) return amax; else return v; }


int TouchX = 0;
int TouchY = 0;
#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080
#define GAME_WIDTH 640
#define GAME_HEIGHT 480


void convert_touch_xy_to_game_xy(float touch_x, float touch_y, int *game_x, int *game_y) {

	const int screen_h = GAME_HEIGHT;
	const int screen_w = GAME_WIDTH;
	const int disp_w   = DISPLAY_WIDTH;
	const int disp_h   = DISPLAY_HEIGHT;

	int x, y, w, h;
	float sx, sy;

	h = disp_h;
	w = h * 16.0/9.0;

	x = (disp_w - w) / 2;
	y = (disp_h - h) / 2;

	sy = (float) h / (float) screen_h;
	sx = (float) w / (float) screen_w;

	// Find touch coordinates in terms of screen pixels
	float disp_touch_x = (touch_x * (float) disp_w);
	float disp_touch_y = (touch_y * (float) disp_h);

	*game_x = CLIP((int)((disp_touch_x - x) / sx), 0, (int) GAME_WIDTH);
	*game_y = CLIP((int)((disp_touch_y - y) / sy), 0, (int) GAME_HEIGHT);

	int foox = *game_x;
	int fooy = *game_y;
	TouchX = *game_x;
	TouchY = *game_y;
	
	//SDL_Log("GAME_X %d GAME_Y %d ", foox, fooy);
}



}


