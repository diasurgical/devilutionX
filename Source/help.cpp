/**
 * @file help.cpp
 *
 * Implementation of the in-game help text.
 */
#include <string>
#include <vector>

#include "control.h"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "minitext.h"
#include "stores.h"
#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

unsigned int SkipLines;
bool helpflag;

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
	N_("$Gold"),
	N_("You can select a specific amount of gold to drop by"
	   "right-clicking on a pile of gold in your inventory."),
	"",
	N_("$Skills & Spells:"),
	N_("You can access your list of skills and spells by left-clicking on "
	   "the 'SPELLS' button in the interface bar. Memorized spells and "
	   "those available through staffs are listed here. Left-clicking on "
	   "the spell you wish to cast will ready the spell. A readied spell "
	   "may be cast by simply right-clicking in the play area."),
	"",
	N_("$Using the Speedbook for Spells"),
	N_("Left-clicking on the 'readied spell' button will open the 'Speedbook' "
	   "which allows you to select a skill or spell for immediate use.  "
	   "To use a readied skill or spell, simply right-click in the main play "
	   "area."),
	N_("Shift + Left-clicking on the 'select current spell' button will clear the readied spell"),
	"",
	N_("$Setting Spell Hotkeys"),
	N_("You can assign up to four Hotkeys for skills, spells or scrolls.  "
	   "Start by opening the 'speedbook' as described in the section above. "
	   "Press the F5, F6, F7 or F8 keys after highlighting the spell you "
	   "wish to assign."),
	"",
	N_("$Spell Books"),
	N_("Reading more than one book increases your knowledge of that "
	   "spell, allowing you to cast the spell more effectively."),
};

std::vector<std::string> HelpTextLines;

void InitHelp()
{
	helpflag = false;
	char tempstr[512];

	for (const auto *text : HelpText) {
		strcpy(tempstr, _(text));

		WordWrapGameString(tempstr, 577);
		const string_view paragraph = tempstr;

		size_t previous = 0;
		while (true) {
			size_t next = paragraph.find('\n', previous);
			HelpTextLines.emplace_back(paragraph.substr(previous, next));
			if (next == std::string::npos)
				break;
			previous = next + 1;
		}
	}
}

void DrawHelp(const Surface &out)
{
	DrawSTextHelp();
	DrawQTextBack(out);

	const char *title;
	if (gbIsHellfire)
		title = gbIsSpawn ? _("Shareware Hellfire Help") : _("Hellfire Help");
	else
		title = gbIsSpawn ? _("Shareware Diablo Help") : _("Diablo Help");
	PrintSString(out, 0, 2, title, UIS_GOLD | UIS_CENTER);

	DrawSLine(out, 5);

	const int sx = PANEL_X + 32;
	const int sy = UI_OFFSET_Y + 51;

	for (int i = 7; i < 22; i++) {
		const char *line = HelpTextLines[i - 7 + SkipLines].c_str();
		if (line[0] == '\0') {
			continue;
		}

		int offset = 0;
		uint16_t style = UIS_SILVER;
		if (line[0] == '$') {
			offset = 1;
			style = UIS_RED;
		}

		DrawString(out, &line[offset], { sx, sy + i * 12, 577, 12 }, style);
	}

	PrintSString(out, 0, 23, _("Press ESC to end or the arrow keys to scroll."), UIS_GOLD | UIS_CENTER);
}

void DisplayHelp()
{
	SkipLines = 0;
	helpflag = true;
}

void HelpScrollUp()
{
	if (SkipLines > 0)
		SkipLines--;
}

void HelpScrollDown()
{
	if (SkipLines < HelpTextLines.size() - 15)
		SkipLines++;
}

} // namespace devilution
