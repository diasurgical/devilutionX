/**
 * @file help.cpp
 *
 * Implementation of the in-game help text.
 */
#include <string>
#include <vector>

#include "DiabloUI/ui_flags.hpp"
#include "control.h"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "minitext.h"
#include "stores.h"
#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

bool HelpFlag;

namespace {

unsigned int SkipLines;

const char *const HelpText[] = {
	N_("$Keyboard Shortcuts:"),
	N_("F1:    Open Help Screen"),
	N_("Esc:   Display Main Menu"),
	N_("Tab:   Display Auto-map"),
	N_("Space: Hide all info screens"),
	N_("S: Open Speedbook"),
	N_("B: Open Spellbook"),
	N_("I: Open Inventory screen"),
	N_("C: Open Character screen"),
	N_("Q: Open Quest log"),
	N_("F: Reduce screen brightness"),
	N_("G: Increase screen brightness"),
	N_("Z: Zoom Game Screen"),
	N_("+ / -: Zoom Automap"),
	N_("1 - 8: Use Belt item"),
	N_("F5, F6, F7, F8:     Set hotkey for skill or spell"),
	N_("Shift + Left Mouse Button: Attack without moving"),
	N_("Shift + Left Mouse Button (on character screen): Assign all stat points"),
	N_("Shift + Left Mouse Button (on inventory): Move item to belt or equip/unequip item"),
	N_("Shift + Left Mouse Button (on belt): Move item to inventory"),
	"",
	N_("$Movement:"),
	N_("If you hold the mouse button down while moving, the character "
	   "will continue to move in that direction."),
	"",
	N_("$Combat:"),
	N_("Holding down the shift key and then left-clicking allows the "
	   "character to attack without moving."),
	"",
	N_("$Auto-map:"),
	N_("To access the auto-map, click the 'MAP' button on the "
	   "Information Bar or press 'TAB' on the keyboard. Zooming in and "
	   "out of the map is done with the + and - keys. Scrolling the map "
	   "uses the arrow keys."),
	"",
	N_("$Picking up Objects:"),
	N_("Useable items that are small in size, such as potions or scrolls, "
	   "are automatically placed in your 'belt' located at the top of "
	   "the Interface bar . When an item is placed in the belt, a small "
	   "number appears in that box. Items may be used by either pressing "
	   "the corresponding number or right-clicking on the item."),
	"",
	N_("$Gold:"),
	N_("You can select a specific amount of gold to drop by "
	   "right-clicking on a pile of gold in your inventory."),
	"",
	N_("$Skills & Spells:"),
	N_("You can access your list of skills and spells by left-clicking on "
	   "the 'SPELLS' button in the interface bar. Memorized spells and "
	   "those available through staffs are listed here. Left-clicking on "
	   "the spell you wish to cast will ready the spell. A readied spell "
	   "may be cast by simply right-clicking in the play area."),
	"",
	N_("$Using the Speedbook for Spells:"),
	N_("Left-clicking on the 'readied spell' button will open the 'Speedbook' "
	   "which allows you to select a skill or spell for immediate use. "
	   "To use a readied skill or spell, simply right-click in the main play "
	   "area."),
	N_("Shift + Left-clicking on the 'select current spell' button will clear the readied spell."),
	"",
	N_("$Setting Spell Hotkeys:"),
	N_("You can assign up to four Hotkeys for skills, spells or scrolls. "
	   "Start by opening the 'speedbook' as described in the section above. "
	   "Press the F5, F6, F7 or F8 keys after highlighting the spell you "
	   "wish to assign."),
	"",
	N_("$Spell Books:"),
	N_("Reading more than one book increases your knowledge of that "
	   "spell, allowing you to cast the spell more effectively."),
};

std::vector<std::string> HelpTextLines;

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

void InitHelp()
{
	static bool Initialized = false;
	if (Initialized)
		return;

	HelpFlag = false;
	char tempString[1024];

	for (const auto *text : HelpText) {
		strcpy(tempString, _(text));

		const std::string paragraph = WordWrapString(tempString, 577);

		size_t previous = 0;
		while (true) {
			size_t next = paragraph.find('\n', previous);
			HelpTextLines.emplace_back(paragraph.substr(previous, next - previous));
			if (next == std::string::npos)
				break;
			previous = next + 1;
		}
	}

	Initialized = true;
}

void DrawHelp(const Surface &out)
{
	DrawSTextHelp();
	DrawQTextBack(out);

	const int lineHeight = LineHeight();
	const int blankLineHeight = BlankLineHeight();

	const char *title;
	if (gbIsHellfire)
		title = gbIsSpawn ? _("Shareware Hellfire Help") : _("Hellfire Help");
	else
		title = gbIsSpawn ? _("Shareware Diablo Help") : _("Diablo Help");

	const int sx = PANEL_X + PaddingLeft;
	const int sy = UI_OFFSET_Y;

	DrawString(out, title,
	    { { sx, sy + PaddingTop + blankLineHeight }, { ContentTextWidth, lineHeight } },
	    UiFlags::ColorWhitegold | UiFlags::AlignCenter);

	const int titleBottom = sy + HeaderHeight();
	DrawSLine(out, titleBottom);

	const int numLines = NumVisibleLines();
	const int contentY = titleBottom + DividerLineMarginY() + ContentPaddingY();
	for (int i = 0; i < numLines; i++) {
		const string_view line = HelpTextLines[i + SkipLines];
		if (line.empty()) {
			continue;
		}

		int offset = 0;
		UiFlags style = UiFlags::ColorWhite;
		if (line[0] == '$') {
			offset = 1;
			style = UiFlags::ColorBlue;
		}

		DrawString(out, line.substr(offset), { { sx, contentY + i * lineHeight }, { ContentTextWidth, lineHeight } }, style, /*spacing=*/1, lineHeight);
	}

	DrawString(out, _("Press ESC to end or the arrow keys to scroll."),
	    { { sx, contentY + ContentsTextHeight() + ContentPaddingY() + blankLineHeight }, { ContentTextWidth, lineHeight } },
	    UiFlags::ColorWhitegold | UiFlags::AlignCenter);
}

void DisplayHelp()
{
	SkipLines = 0;
	HelpFlag = true;
}

void HelpScrollUp()
{
	if (SkipLines > 0)
		SkipLines--;
}

void HelpScrollDown()
{
	if (SkipLines + NumVisibleLines() < HelpTextLines.size())
		SkipLines++;
}

} // namespace devilution
