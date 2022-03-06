/**
 * @file chatlog.cpp
 *
 * Implementation of the in-game chat log.
 */
#include <fmt/format.h>
#include <string>
#include <vector>

#include "DiabloUI/ui_flags.hpp"
#include "automap.h"
#include "chatlog.h"
#include "control.h"
#include "doom.h"
#include "engine/render/text_render.hpp"
#include "error.h"
#include "gamemenu.h"
#include "help.h"
#include "init.h"
#include "inv.h"
#include "minitext.h"
#include "stores.h"
#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

namespace {

struct ColoredText {
	std::string text;
	UiFlags color;
};

struct MultiColoredText {
	std::string text;
	std::vector<ColoredText> colors;
	int offset = 0;
};

bool UnreadFlag = false;
unsigned int SkipLines;
unsigned int MessageCounter = 0;

std::vector<MultiColoredText> ChatLogLines;

constexpr int PaddingTop = 32;
constexpr int PaddingLeft = 32;

constexpr int PanelHeight = 297;
constexpr int ContentTextWidth = 577;

int LineHeight()
{
	return IsSmallFontTall() ? 18 : 14;
}

int BlankLineHeight()
{
	return 12;
}

int DividerLineMarginY()
{
	return BlankLineHeight() / 2;
}

int HeaderHeight()
{
	return PaddingTop + LineHeight() + 2 * BlankLineHeight() + DividerLineMarginY();
}

int ContentPaddingY()
{
	return BlankLineHeight();
}

int ContentsTextHeight()
{
	return PanelHeight - HeaderHeight() - DividerLineMarginY() - 2 * ContentPaddingY() - BlankLineHeight();
}

int NumVisibleLines()
{
	return (ContentsTextHeight() - 1) / LineHeight() + 1; // Ceil
}

} // namespace

bool ChatLogFlag = false;

void ToggleChatLog()
{
	if (ChatLogFlag) {
		ChatLogFlag = false;
	} else {
		stextflag = STORE_NONE;
		invflag = false;
		chrflag = false;
		sbookflag = false;
		spselflag = false;
		if (qtextflag && leveltype == DTYPE_TOWN) {
			qtextflag = false;
			stream_stop();
		}
		QuestLogIsOpen = false;
		HelpFlag = false;
		AutomapActive = false;
		CancelCurrentDiabloMsg();
		gamemenu_off();
		SkipLines = 0;
		ChatLogFlag = true;
		doom_close();
	}
}

void AddMessageToChatLog(const std::string &message, Player *player, UiFlags flags)
{
	MessageCounter++;
	time_t tm = time(nullptr);
	std::string timestamp = fmt::format("[#{:d}] {:02}:{:02}:{:02}", MessageCounter, localtime(&tm)->tm_hour, localtime(&tm)->tm_min, localtime(&tm)->tm_sec);
	int oldSize = ChatLogLines.size();
	ChatLogLines.emplace_back(MultiColoredText { "", { {} } });
	if (player == nullptr) {
		ChatLogLines.emplace_back(MultiColoredText { "{0} {1}", { { timestamp, UiFlags::ColorRed }, { message, flags } } });
	} else {
		std::string playerInfo = fmt::format(_("{:s} (lvl {:d}): {:s}"), player->_pName, player->_pLevel, "");
		ChatLogLines.emplace_back(MultiColoredText { message, { {} }, 20 });
		UiFlags nameColor = player == &Players[MyPlayerId] ? UiFlags::ColorWhitegold : UiFlags::ColorBlue;
		ChatLogLines.emplace_back(MultiColoredText { "{0} - {1}", { { timestamp, UiFlags::ColorRed }, { playerInfo, nameColor } } });
	}

	unsigned int diff = ChatLogLines.size() - oldSize;
	// only autoscroll when on top of the log
	if (SkipLines != 0) {
		SkipLines += diff;
		UnreadFlag = true;
	}
}

void DrawChatLog(const Surface &out)
{
	DrawSTextHelp();
	DrawQTextBack(out);

	if (SkipLines == 0) {
		UnreadFlag = false;
	}

	const int lineHeight = LineHeight();
	const int blankLineHeight = BlankLineHeight();
	const int sx = PANEL_X + PaddingLeft;
	const int sy = UI_OFFSET_Y;

	DrawString(out, fmt::format(_("Chat History (Messages: {:d})"), MessageCounter),
	    { { sx, sy + PaddingTop + blankLineHeight }, { ContentTextWidth, lineHeight } },
	    (UnreadFlag ? UiFlags::ColorRed : UiFlags::ColorWhitegold) | UiFlags::AlignCenter);

	time_t tm = time(nullptr);
	std::string timestamp = fmt::format("{:02}:{:02}:{:02}", localtime(&tm)->tm_hour, localtime(&tm)->tm_min, localtime(&tm)->tm_sec);
	DrawString(out, timestamp, { { sx, sy + PaddingTop + blankLineHeight }, { ContentTextWidth, lineHeight } }, UiFlags::ColorWhitegold);

	const int titleBottom = sy + HeaderHeight();
	DrawSLine(out, titleBottom);

	const int numLines = NumVisibleLines();
	const int contentY = titleBottom + DividerLineMarginY() + ContentPaddingY();
	for (int i = 0; i < numLines; i++) {
		if (i + SkipLines >= ChatLogLines.size())
			break;
		MultiColoredText &text = ChatLogLines[ChatLogLines.size() - (i + SkipLines + 1)];
		const string_view line = text.text;

		std::vector<DrawStringFormatArg> args;
		for (auto &x : text.colors) {
			args.emplace_back(DrawStringFormatArg { x.text, x.color });
		}
		DrawStringWithColors(out, line, args, { { (sx + text.offset), contentY + i * lineHeight }, { ContentTextWidth - text.offset * 2, lineHeight } }, UiFlags::ColorWhite, /*spacing=*/1, lineHeight);
	}

	DrawString(out, _("Press ESC to end or the arrow keys to scroll."),
	    { { sx, contentY + ContentsTextHeight() + ContentPaddingY() + blankLineHeight }, { ContentTextWidth, lineHeight } },
	    UiFlags::ColorWhitegold | UiFlags::AlignCenter);
}

void ChatLogScrollUp()
{
	if (SkipLines > 0)
		SkipLines--;
}

void ChatLogScrollDown()
{
	if (SkipLines + NumVisibleLines() < ChatLogLines.size())
		SkipLines++;
}

void ChatLogScrollTop()
{
	SkipLines = 0;
}

void ChatLogScrollBottom()
{
	SkipLines = ChatLogLines.size() - NumVisibleLines();
}

} // namespace devilution
