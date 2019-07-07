#ifndef __PLRCTRLS_H__
#define __PLRCTRLS_H__

void checkTownersNearby(bool interact);
void checkItemsNearby(bool interact);
void keyboardExpansion(int vikey);
void charMovement();
void movements(int key);
bool checkMonstersNearby(bool attack);
extern bool newCurHidden;
void invMove(int key);
void HideCursor();
void useBeltPotion(bool mana);
typedef struct coords {
	int x;
	int y;
} coords;
extern coords speedspellscoords[50];
extern int speedspellcount;
extern const InvXY InvRect[73]; // wasn't made public, so I'll add this here from inv.cpp

extern DWORD talkwait;
extern DWORD talktick;
extern DWORD castwait;

extern bool inmainmenu;

#define INV_TOP 240;
#define INV_LEFT 350;
#define INV_HEIGHT 320;
#define DISPLAY_HEIGHT 360;
#define DISPLAY_WIDTH 640;

#endif
