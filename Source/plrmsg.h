/**
 * @file plrmsg.h
 *
 * Interface of functionality for printing the ingame chat messages.
 */
#pragma once

#include "engine.h"

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _plrmsg {
	Uint32 time;
	Uint8 player;
	char str[144];
} _plrmsg;

void plrmsg_delay(BOOL delay);
void ErrorPlrMsg(const char *pszMsg);
size_t EventPlrMsg(const char *pszFmt, ...);
void SendPlrMsg(int pnum, const char *pszStr);
void ClearPlrMsg();
void InitPlrMsg();
void DrawPlrMsg(CelOutputBuffer out);

#ifdef __cplusplus
}
#endif

}
