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
int msgs;

/** Maps from player_num to text color, as used in chat messages. */
const UiFlags TextColorFromPlayerId[MAX_PLRS + 1] = { UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhite, UiFlags::ColorWhitegold };

int GetChatLines(int width, char *textPtr)
{
	const size_t length = strlen(textPtr);
	const string_view text { textPtr, length };
	std::string lineCountString = WordWrapString(text, width);
	int lines = std::count(lineCountString.begin(), lineCountString.end(), '\n');
	lines++;
	return lines;
}

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

	for (msgs = PMSG_COUNT - 2; msgs >= 0; msgs--) {
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

	for (msgs = PMSG_COUNT - 2; msgs >= 0; msgs--) {
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

	for (msgs = PMSG_COUNT - 2; msgs >= 0; msgs--) {
		plr_msgs[msgs + 1] = plr_msgs[msgs];
	}

	plr_msg_slot = plr_msg_slot & (PMSG_COUNT - 1);
	pMsg->player = pnum;
	pMsg->time = SDL_GetTicks();
	auto &player = Players[pnum];
	assert(strlen(player._pName) < PLR_NAME_LEN);
	assert(strlen(pszStr) < MAX_SEND_STR_LEN);

	CopyUtf8(pMsg->str, fmt::format(_("{:s} (lvl {:d}): {:s}"), player._pName, player._pLevel, pszStr), sizeof(pMsg->str));
	CopyUtf8(pMsg->name, fmt::format(_("{:s} (lvl {:d}):"), player._pName, player._pLevel), sizeof(pMsg->name));
}

void InitPlrMsg()
{
	memset(plr_msgs, 0, sizeof(plr_msgs));
	plr_msg_slot = 0;
}

void DrawPlrMsg(const Surface &out)
{
	int x = 10;
	int y = PANEL_TOP - 32;
	int width = gnScreenWidth - 20;
	int vislines;
	int chatlines;
	_plrmsg *pMsg;

	int oneLine = 15;
	int twoLine = 30;
	int threeLine = 45;

	if (!talkflag && (chrflag || QuestLogIsOpen/* || stashflag*/)) {
		x += GetLeftPanel().position.x + GetLeftPanel().size.width;
		width -= GetLeftPanel().size.width;
	}
	if (!talkflag && (invflag || sbookflag))
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
		if (pMsg->str[0] != '\0') {
			if (talkflag) {
				if (GetChatLines(width, pMsg->str) == 1) {
					DrawHalfTransparentRectTo(out, x - 3, y, width + 6, oneLine);
				} else if (GetChatLines(width, pMsg->str) == 2) {
					DrawHalfTransparentRectTo(out, x - 3, y, width + 6, twoLine);
				} else {
					DrawHalfTransparentRectTo(out, x - 3, y, width + 6, threeLine);
				}
				PrintChatMessage(out, x, y, width, pMsg->str, TextColorFromPlayerId[pMsg->player]);
				if (pMsg->player != MAX_PLRS)
					PrintChatMessage(out, x, y, width, pMsg->name, TextColorFromPlayerId[MAX_PLRS]);
			} else if (SDL_GetTicks() - pMsg->time < 10000) {
				if (GetChatLines(width, pMsg->str) == 1) {
					DrawHalfTransparentRectTo(out, x - 3, y, width + 6, oneLine);
				} else if (GetChatLines(width, pMsg->str) == 2) {
					DrawHalfTransparentRectTo(out, x - 3, y, width + 6, twoLine);
				} else {
					DrawHalfTransparentRectTo(out, x - 3, y, width + 6, threeLine);
				}
				PrintChatMessage(out, x, y, width, pMsg->str, TextColorFromPlayerId[pMsg->player]);
				if (pMsg->player != MAX_PLRS)
					PrintChatMessage(out, x, y, width, pMsg->name, TextColorFromPlayerId[MAX_PLRS]);
			}
			pMsg++;
		}
		chatlines = GetChatLines(width, pMsg->str);

		if (chatlines == 1) {
			y -= oneLine;
		} else if (chatlines == 2) {
			y -= twoLine;
		} else {
			y -= threeLine;
		}
	}
}

} // namespace devilution
