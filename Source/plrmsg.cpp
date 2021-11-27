/**
 * @file plrmsg.cpp
 *
 * Implementation of functionality for printing the ingame chat messages.
 */
#include "plrmsg.h"

#include <algorithm>

#include <fmt/format.h>

#include "DiabloUI/ui_flags.hpp"
#include "control.h"
#include "engine/render/text_render.hpp"
#include "inv.h"
#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"
#include "utils/utf8.hpp"

namespace devilution {

namespace {

#define PMSG_COUNT 8

uint8_t plr_msg_slot;
_plrmsg plr_msgs[PMSG_COUNT];

/** Maps from player_num to text color, as used in chat messages. */
const UiFlags TextColorFromPlayerId[MAX_PLRS + 1] = { UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhitegold };

void PrintChatMessage(const Surface &out, int x, int y, int width, char *textPtr, UiFlags style)
{
	const size_t length = strlen(textPtr);
	std::replace(textPtr, textPtr + length, '\n', ' ');
	const string_view text { textPtr, length };
	DrawString(out, WordWrapString(text, width), { { x, y }, { width, 0 } }, style, 1, 18);
}

} // namespace

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
	CopyUtf8(pMsg->str, pszMsg, sizeof(pMsg->str));
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
	auto &player = Players[pnum];
	assert(strlen(player._pName) < PLR_NAME_LEN);
	assert(strlen(pszStr) < MAX_SEND_STR_LEN);
	CopyUtf8(pMsg->str, fmt::format(_("{:s} (lvl {:d}): {:s}"), player._pName, player._pLevel, pszStr), sizeof(pMsg->str));
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

void DrawPlrMsg(const Surface &out)
{
	int x = 10;
	int y = 58;
	int width = gnScreenWidth - 20;
	_plrmsg *pMsg;

	if (chrflag || QuestLogIsOpen) {
		x += GetLeftPanel().position.x + GetLeftPanel().size.width;
		width -= GetLeftPanel().size.width;
	}
	if (invflag || sbookflag)
		width -= gnScreenWidth - GetRightPanel().position.x;

	if (width < 300)
		return;

	pMsg = plr_msgs;
	for (int i = 0; i < PMSG_COUNT; i++) {
		if (pMsg->str[0] != '\0')
			PrintChatMessage(out, x, y, width, pMsg->str, TextColorFromPlayerId[pMsg->player]);
		pMsg++;
		y += 35;
	}
}

} // namespace devilution
