/**
 * @file plrmsg.h
 *
 * Interface of functionality for printing the ingame chat messages.
 */
#pragma once

#include "engine.h"

namespace devilution {

struct _plrmsg {
	Uint32 time;
	Uint8 player;
	char str[144];
};

void plrmsg_delay(bool delay);
void ErrorPlrMsg(const char *pszMsg);
size_t EventPlrMsg(const char *pszFmt, ...);
void SendPlrMsg(int pnum, const char *pszStr);
void ClearPlrMsg();
void InitPlrMsg();
void DrawPlrMsg(CelOutputBuffer out);

}
