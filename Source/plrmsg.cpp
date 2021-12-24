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
//#include "stash.h"
#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"
#include "utils/utf8.hpp"

namespace devilution {

namespace {

#define PMSG_COUNT 8

uint8_t plr_msg_slot;
_plrmsg plr_msgs[PMSG_COUNT];
int msgs;

/** Maps from player_num to text color, as used in chat messages. */
const UiFlags TextColorFromPlayerId[MAX_PLRS + 1] = { UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhitegold };

void PrintChatMessage(const Surface &out, int x, int y, int width, char *textPtr, UiFlags style)
{
	const size_t length = strlen(textPtr);
	std::replace(textPtr, textPtr + length, '\n', ' ');
	const string_view text { textPtr, length };
	DrawString(out, WordWrapString(text, width), { { x, y }, { width, 0 } }, style, 1, 15);
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

	for (msgs = PMSG_COUNT - 2; msgs > -1; msgs--) {
		plr_msgs[msgs + 1] = plr_msgs[msgs];
	}

	plr_msg_slot = plr_msg_slot & (PMSG_COUNT - 1);
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

	for (msgs = PMSG_COUNT - 2; msgs > -1; msgs--) {
		plr_msgs[msgs + 1] = plr_msgs[msgs];
	}

	plr_msg_slot = plr_msg_slot & (PMSG_COUNT - 1);
	pMsg->player = MAX_PLRS;
	pMsg->time = SDL_GetTicks();
	vsprintf(pMsg->str, pszFmt, va);
	va_end(va);
	return strlen(pMsg->str);
}

void SendPlrMsg(int pnum, const char *pszStr)
{
	_plrmsg *pMsg = &plr_msgs[plr_msg_slot];

	for (msgs = PMSG_COUNT - 2; msgs > -1; msgs--) {
		plr_msgs[msgs + 1] = plr_msgs[msgs];
	}

	plr_msg_slot = plr_msg_slot & (PMSG_COUNT - 1);
	pMsg->player = pnum;
	pMsg->time = SDL_GetTicks();
	auto &player = Players[pnum];
	assert(strlen(player._pName) < PLR_NAME_LEN);
	assert(strlen(pszStr) < MAX_SEND_STR_LEN);

	CopyUtf8(pMsg->str, fmt::format(_("<{:s} (lvl {:d})> {:s}"), player._pName, player._pLevel, pszStr), sizeof(pMsg->str));
	CopyUtf8(pMsg->name, fmt::format(_("<{:s} (lvl {:d})>"), player._pName, player._pLevel), sizeof(pMsg->name));
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
	int y = PANEL_TOP - 20;
	int width = gnScreenWidth - 20;
	int vislines;
	_plrmsg *pMsg;

	if (chrflag || QuestLogIsOpen/* || stashflag*/) {
		x += GetLeftPanel().position.x + GetLeftPanel().size.width;
		width -= GetLeftPanel().size.width;
	}
	if (invflag || sbookflag)
		width -= gnScreenWidth - GetRightPanel().position.x;

	if (width < 300)
		return;

	if (talkflag) {
		vislines = PMSG_COUNT;
	} else {
		vislines = 3;
	}

	pMsg = plr_msgs;

	for (int i = 0; i < vislines; i++) {
		if (talkflag && pMsg->str[0] != '\0') {
			DrawHalfTransparentRectTo(out, x, y, width, 15);
			PrintChatMessage(out, x, y, width, pMsg->str, TextColorFromPlayerId[pMsg->player]);
			if (pMsg->player != MAX_PLRS)
				PrintChatMessage(out, x, y, width, pMsg->name, TextColorFromPlayerId[4]);

		} else if (SDL_GetTicks() - pMsg->time < 5000) {
			DrawHalfTransparentRectTo(out, x, y, width, 15);
			PrintChatMessage(out, x, y, width, pMsg->str, TextColorFromPlayerId[pMsg->player]);
			if (pMsg->player != MAX_PLRS)
				PrintChatMessage(out, x, y, width, pMsg->name, TextColorFromPlayerId[4]);
		}

		pMsg++;

		std::string text = pMsg->str;
		auto count = std::count(text.begin(), text.end(), '\n');

		if (count == 0) {
			y -= 15;
		} else if (count == 1) {
			y -= 30;
		} else {
			y -= 45;
		}
	}

	
}

} // namespace devilution
