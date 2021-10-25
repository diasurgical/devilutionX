/**
 * @file plrmsg.h
 *
 * Interface of functionality for printing the ingame chat messages.
 */
#pragma once

#include "SDL.h"
#include <cstdint>

#include "engine.h"

namespace devilution {

struct _plrmsg {
	Uint32 time;
	uint8_t player;
	char str[144];
};

void plrmsg_delay(bool delay);
void ErrorPlrMsg(const char *pszMsg);
size_t EventPlrMsg(int pnum, const char *pszFmt, ...);
void SendPlrMsg(int pnum, const char *pszStr);
void ClearPlrMsg();
void InitPlrMsg();
void DrawPlrMsg(const Surface &out);

} // namespace devilution
