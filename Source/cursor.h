/**
 * @file cursor.h
 *
 * Interface of cursor tracking functionality.
 */
#pragma once

#include <stdint.h>
#include <string_view>

#include "miniwin/miniwin.h"

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

constexpr std::string_view toString(cursor_id value)
{
	switch(value) {
	case CURSOR_NONE:
		return "None";
	case CURSOR_HAND:
		return "Hand";
	case CURSOR_IDENTIFY:
		return "Identify";
	case CURSOR_REPAIR:
		return "Repair";
	case CURSOR_RECHARGE:
		return "Recharge";
	case CURSOR_DISARM:
		return "Disarm";
	case CURSOR_OIL:
		return "Oil";
	case CURSOR_TELEKINESIS:
		return "Telekinesis";
	case CURSOR_RESURRECT:
		return "Resurrect";
	case CURSOR_TELEPORT:
		return "Teleport";
	case CURSOR_HEALOTHER:
		return "Healother";
	case CURSOR_HOURGLASS:
		return "Hourglass";
	case CURSOR_FIRSTITEM:
		return "Firstitem";
	}
}

extern int cursW;
extern int cursH;
extern int pcursmonst;
extern int icursW28;
extern int icursH28;
extern BYTE *pCursCels;
extern BYTE *pCursCels2;
extern int icursH;
extern int8_t pcursinvitem;
extern int icursW;
extern int8_t pcursitem;
extern int8_t pcursobj;
extern int8_t pcursplr;
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

} // namespace devilution
