/**
 * @file cursor.h
 *
 * Interface of cursor tracking functionality.
 */
#pragma once

#include <stdint.h>

namespace devilution {

enum cursor_id : uint8_t {
	CURSOR_NONE,
	CURSOR_HAND,
	CURSOR_IDENTIFY,
	CURSOR_REPAIR,
	CURSOR_RECHARGE,
	CURSOR_DISARM,
	CURSOR_OIL,
	CURSOR_TELEKINESIS,
	CURSOR_RESURRECT,
	CURSOR_TELEPORT,
	CURSOR_HEALOTHER,
	CURSOR_HOURGLASS,
	CURSOR_FIRSTITEM,
};

extern int cursW;
extern int cursH;
extern int pcursmonst;
extern int icursW28;
extern int icursH28;
extern BYTE *pCursCels;
extern BYTE *pCursCels2;
extern int icursH;
extern char pcursinvitem;
extern int icursW;
extern char pcursitem;
extern char pcursobj;
extern char pcursplr;
extern int cursmx;
extern int cursmy;
extern int pcurs;

void InitCursor();
void FreeCursor();
void SetICursor(int i);
void NewCursor(int i);
void InitLevelCursor();
void CheckRportal();
void CheckTown();
void CheckCursMove();

/* rdata */
extern const int InvItemWidth[];
extern const int InvItemHeight[];

}
