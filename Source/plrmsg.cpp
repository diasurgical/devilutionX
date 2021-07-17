/**
 * @file plrmsg.cpp
 *
 * Implementation of functionality for printing the ingame chat messages.
 */
#include "plrmsg.h"

#include <fmt/format.h>

#include "control.h"
#include "engine/render/text_render.hpp"
#include "inv.h"
#include "utils/language.h"

namespace devilution {

#define PMSG_COUNT 8

static BYTE plr_msg_slot;
_plrmsg plr_msgs[PMSG_COUNT];

/** Maps from player_num to text color, as used in chat messages. */
const UiFlags TextColorFromPlayerId[MAX_PLRS + 1] = { UIS_SILVER, UIS_SILVER, UIS_SILVER, UIS_SILVER, UIS_GOLD };

void plrmsg_delay(bool delay)
{
	static uint32_t plrmsgTicks;

	if (delay) {
		plrmsgTicks = -SDL_GetTicks();
		return;
	}

	plrmsgTicks += SDL_GetTicks();
	_plrmsg *pMsg = plr_msgs;
	for (int i = 0; i < PMSG_COUNT; i++, pMsg++)
		pMsg->time += plrmsgTicks;
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
	assert(strlen(Players[pnum]._pName) < PLR_NAME_LEN);
	assert(strlen(pszStr) < MAX_SEND_STR_LEN);
	strcpy(pMsg->str, fmt::format(_(/* TRANSLATORS: Shown if player presses "v" button. {:s} is player name, {:d} is level, {:s} is location */ "{:s} (lvl {:d}): {:s}"), Players[pnum]._pName, Players[pnum]._pLevel, pszStr).c_str());
}

void ClearPlrMsg()
{
	_plrmsg *pMsg = plr_msgs;
	uint32_t tick = SDL_GetTicks();

	for (int i = 0; i < PMSG_COUNT; i++, pMsg++) {
		if ((int)(tick - pMsg->time) > 10000)
			pMsg->str[0] = '\0';
	}
}

void InitPlrMsg()
{
	memset(plr_msgs, 0, sizeof(plr_msgs));
	plr_msg_slot = 0;
}

static void PrintPlrMsg(const Surface &out, int x, int y, int width, char *text, uint16_t style)
{
	int length = strlen(text);
	for (int i = 0; i < length; i++) {
		if (text[i] == '\n')
			text[i] = ' ';
	}
	WordWrapGameString(text, width);
	DrawString(out, text, { { x, y }, { width, 0 } }, style, 1, 10);
}

void DrawPlrMsg(const Surface &out)
{
	DWORD x = 10;
	DWORD y = 70;
	DWORD width = gnScreenWidth - 20;
	_plrmsg *pMsg;

	if (chrflag || QuestLogIsOpen) {
		x += SPANEL_WIDTH;
		width -= SPANEL_WIDTH;
	}
	if (invflag || sbookflag)
		width -= SPANEL_WIDTH;

	if (width < 300)
		return;

	pMsg = plr_msgs;
	for (int i = 0; i < PMSG_COUNT; i++) {
		if (pMsg->str[0] != '\0')
			PrintPlrMsg(out, x, y, width, pMsg->str, TextColorFromPlayerId[pMsg->player]);
		pMsg++;
		y += 35;
	}
}

} // namespace devilution
