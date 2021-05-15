/**
 * @file plrmsg.cpp
 *
 * Implementation of functionality for printing the ingame chat messages.
 */
#include "plrmsg.h"

#include "control.h"
#include "engine/render/text_render.hpp"
#include "inv.h"
#include "utils/language.h"

namespace devilution {

#define PMSG_COUNT 8

static BYTE plr_msg_slot;
_plrmsg plr_msgs[PMSG_COUNT];

/** Maps from player_num to text color, as used in chat messages. */
const text_color text_color_from_player_num[MAX_PLRS + 1] = { COL_WHITE, COL_WHITE, COL_WHITE, COL_WHITE, COL_GOLD };

void plrmsg_delay(bool delay)
{
	int i;
	_plrmsg *pMsg;
	static DWORD plrmsg_ticks;

	if (delay) {
		plrmsg_ticks = -SDL_GetTicks();
		return;
	}

	plrmsg_ticks += SDL_GetTicks();
	pMsg = plr_msgs;
	for (i = 0; i < PMSG_COUNT; i++, pMsg++)
		pMsg->time += plrmsg_ticks;
}

void ErrorPlrMsg(const char *pszMsg)
{
	_plrmsg *pMsg = &plr_msgs[plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & (PMSG_COUNT - 1);
	pMsg->player = MAX_PLRS;
	pMsg->time = SDL_GetTicks();
	strncpy(pMsg->str, pszMsg, sizeof(pMsg->str));
	pMsg->str[sizeof(pMsg->str) - 1] = '\0';
}

size_t EventPlrMsg(const char *pszFmt, ...)
{
	_plrmsg *pMsg;
	va_list va;

	va_start(va, pszFmt);
	pMsg = &plr_msgs[plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & (PMSG_COUNT - 1);
	pMsg->player = MAX_PLRS;
	pMsg->time = SDL_GetTicks();
	vsprintf(pMsg->str, pszFmt, va);
	va_end(va);
	return strlen(pMsg->str);
}

void SendPlrMsg(int pnum, const char *pszStr)
{
	_plrmsg *pMsg = &plr_msgs[plr_msg_slot];
	plr_msg_slot = (plr_msg_slot + 1) & (PMSG_COUNT - 1);
	pMsg->player = pnum;
	pMsg->time = SDL_GetTicks();
	assert(strlen(plr[pnum]._pName) < PLR_NAME_LEN);
	assert(strlen(pszStr) < MAX_SEND_STR_LEN);
	sprintf(pMsg->str, _( /* TRANSLATORS: Shown if player presses "v" button. %s is player name, %i is level, %s is location */ "%s (lvl %i): %s"), plr[pnum]._pName, plr[pnum]._pLevel, pszStr);
}

void ClearPlrMsg()
{
	int i;
	_plrmsg *pMsg = plr_msgs;
	DWORD tick = SDL_GetTicks();

	for (i = 0; i < PMSG_COUNT; i++, pMsg++) {
		if ((int)(tick - pMsg->time) > 10000)
			pMsg->str[0] = '\0';
	}
}

void InitPlrMsg()
{
	memset(plr_msgs, 0, sizeof(plr_msgs));
	plr_msg_slot = 0;
}

static void PrintPlrMsg(const CelOutputBuffer &out, DWORD x, DWORD y, DWORD width, const char *str, text_color col)
{
	int line = 0;

	while (*str) {
		BYTE c;
		int sx = x;
		DWORD len = 0;
		const char *sstr = str;
		const char *endstr = sstr;

		while (true) {
			if (*sstr) {
				c = gbFontTransTbl[(BYTE)*sstr++];
				c = fontframe[GameFontSmall][c];
				len += fontkern[GameFontSmall][c] + 1;
				if (!c) // allow wordwrap on blank glyph
					endstr = sstr;
				else if (len >= width)
					break;
			} else {
				endstr = sstr;
				break;
			}
		}

		while (str < endstr) {
			c = gbFontTransTbl[(BYTE)*str++];
			c = fontframe[GameFontSmall][c];
			if (c)
				PrintChar(out, sx, y, c, col);
			sx += fontkern[GameFontSmall][c] + 1;
		}

		y += 10;
		line++;
		if (line == 3)
			break;
	}
}

void DrawPlrMsg(const CelOutputBuffer &out)
{
	int i;
	DWORD x = 10;
	DWORD y = 70;
	DWORD width = gnScreenWidth - 20;
	_plrmsg *pMsg;

	if (chrflag || questlog) {
		x += SPANEL_WIDTH;
		width -= SPANEL_WIDTH;
	}
	if (invflag || sbookflag)
		width -= SPANEL_WIDTH;

	if (width < 300)
		return;

	pMsg = plr_msgs;
	for (i = 0; i < PMSG_COUNT; i++) {
		if (pMsg->str[0])
			PrintPlrMsg(out, x, y, width, pMsg->str, text_color_from_player_num[pMsg->player]);
		pMsg++;
		y += 35;
	}
}

} // namespace devilution
