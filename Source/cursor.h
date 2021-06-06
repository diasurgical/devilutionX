/**
 * @file cursor.h
 *
 * Interface of cursor tracking functionality.
 */
#pragma once

#include <cstdint>
#include <utility>

#include "engine.h"
#include "miniwin/miniwin.h"
#include "utils/stdcompat/optional.hpp"

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

inline bool IsItemSprite(int pcurs)
{
	return pcurs >= CURSOR_FIRSTITEM;
}

void CelDrawCursor(const CelOutputBuffer &out, Point position, int pcurs);

/** Returns the sprite for the given inventory index. */
const CelSprite &GetInvItemSprite(int i);

/** Returns the CEL frame index for the given inventory index. */
int GetInvItemFrame(int i);

/** Returns the width and height for an inventory index. */
Size GetInvItemSize(int i);

} // namespace devilution
