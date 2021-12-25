/**
 * @file cursor.h
 *
 * Interface of cursor tracking functionality.
 */
#pragma once

#include <cstdint>
#include <utility>

#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "miniwin/miniwin.h"
#include "utils/attributes.h"
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

extern DVL_API_FOR_TEST Size cursSize;
extern int pcursmonst;
extern DVL_API_FOR_TEST Size icursSize28;
extern DVL_API_FOR_TEST Size icursSize;
extern int8_t pcursinvitem;
extern int8_t pcursitem;
extern int8_t pcursobj;
extern int8_t pcursplr;
extern Point cursPosition;
extern DVL_API_FOR_TEST int pcurs;

void InitCursor();
void FreeCursor();
void SetICursor(int cursId);
void ResetCursor();
void NewCursor(int cursId);
void InitLevelCursor();
void CheckRportal();
void CheckTown();
void CheckCursMove();

inline bool IsItemSprite(int cursId)
{
	return cursId >= CURSOR_FIRSTITEM;
}

void CelDrawCursor(const Surface &out, Point position, int cursId);

/** Returns the sprite for the given inventory index. */
const CelSprite &GetInvItemSprite(int i);

/** Returns the CEL frame index for the given inventory index. */
int GetInvItemFrame(int i);

/** Returns the width and height for an inventory index. */
Size GetInvItemSize(int cursId);

} // namespace devilution
