/**
 * @file help.cpp
 *
 * Implementation of the in-game help text.
 */

#include "control.h"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "minitext.h"
#include "stores.h"
#include "utils/language.h"

namespace devilution {

int help_select_line;
bool helpflag;
int HelpTop;

const char gszHelpText[] = {
	// TRANSLATORS: HUGE KEY. There might be changes in a future version where redundant keys between the shareware help and the full game help will be consolidated. 
	N_("$Keyboard Shortcuts:|"
	   "F1:    Open Help Screen|"
	   "Esc:   Display Main Menu|"
	   "Tab:   Display Auto-map|"
	   "Space: Hide all info screens|"
	   "S: Open Speedbook|"
	   "B: Open Spellbook|"
	   "I: Open Inventory screen|"
	   "C: Open Character screen|"
	   "Q: Open Quest log|"
	   "F: Reduce screen brightness|"
	   "G: Increase screen brightness|"
	   "Z: Zoom Game Screen|"
	   "+ / -: Zoom Automap|"
	   "1 - 8: Use Belt item|"
	   "F5, F6, F7, F8:     Set hotkey for skill or spell|"
	   "Shift + Left Mouse Button: Attack without moving|"
	   "Shift + Left Mouse Button (on character screen): Assign all stat points|"
	   "Shift + Left Mouse Button (on inventory): Move item to belt or equip/unequip item|"
	   "Shift + Left Mouse Button (on belt): Move item to inventory|"
	   "|"
	   "$Movement:|"
	   "If you hold the mouse button down while moving, the character "
	   "will continue to move in that direction.|"
	   "|"
	   "$Combat:|"
	   "Holding down the shift key and then left-clicking allows the "
	   "character to attack without moving.|"
	   "|"
	   "$Auto-map:|"
	   "To access the auto-map, click the 'MAP' button on the "
	   "Information Bar or press 'TAB' on the keyboard. Zooming in and "
	   "out of the map is done with the + and - keys. Scrolling the map "
	   "uses the arrow keys.|"
	   "|"
	   "$Picking up Objects:|"
	   "Useable items that are small in size, such as potions or scrolls, "
	   "are automatically placed in your 'belt' located at the top of "
	   "the Interface bar . When an item is placed in the belt, a small "
	   "number appears in that box. Items may be used by either pressing "
	   "the corresponding number or right-clicking on the item.|"
	   "|"
	   "$Gold|"
	   "You can select a specific amount of gold to drop by"
	   "right-clicking on a pile of gold in your inventory.|"
	   "|"
	   "$Skills & Spells:|"
	   "You can access your list of skills and spells by left-clicking on "
	   "the 'SPELLS' button in the interface bar. Memorized spells and "
	   "those available through staffs are listed here. Left-clicking on "
	   "the spell you wish to cast will ready the spell. A readied spell "
	   "may be cast by simply right-clicking in the play area.|"
	   "|"
	   "$Using the Speedbook for Spells|"
	   "Left-clicking on the 'readied spell' button will open the 'Speedbook' "
	   "which allows you to select a skill or spell for immediate use.  "
	   "To use a readied skill or spell, simply right-click in the main play "
	   "area.|"
	   "Shift + Left-clicking on the 'select current spell' button will clear the readied spell|"
	   "|"
	   "$Setting Spell Hotkeys|"
	   "You can assign up to four Hotkeys for skills, spells or scrolls.  "
	   "Start by opening the 'speedbook' as described in the section above. "
	   "Press the F5, F6, F7 or F8 keys after highlighting the spell you "
	   "wish to assign.|"
	   "|"
	   "$Spell Books|"
	   "Reading more than one book increases your knowledge of that "
	   "spell, allowing you to cast the spell more effectively.|"
	   "&")
};

void InitHelp()
{
	helpflag = false;
}

static void DrawHelpLine(const CelOutputBuffer &out, int x, int y, char *text, uint16_t style)
{
	const int sx = x + 32 + PANEL_X;
	const int sy = y * 12 + 44 + UI_OFFSET_Y;
	DrawString(out, text, { sx, sy, 577, 0 }, style);
}

void DrawHelp(const CelOutputBuffer &out)
{
	int i, c, w;
	const char *s;

	DrawSTextHelp();
	DrawQTextBack(out);
	PrintSString(out, 0, 2, gbIsHellfire ? _("Hellfire Help") : _("Diablo Help"), UIS_GOLD | UIS_CENTER);
	DrawSLine(out, 5);

	s = _(&gszHelpText[0]);

	for (i = 0; i < help_select_line; i++) {
		c = 0;
		w = 0;
		while (*s == '\0') {
			s++;
		}
		if (*s == '$') {
			s++;
		}
		if (*s == '&') {
			continue;
		}
		while (*s != '|' && w < 577) {
			while (*s == '\0') {
				s++;
			}
			tempstr[c] = *s;
			w += fontkern[GameFontSmall][fontframe[GameFontSmall][gbFontTransTbl[(BYTE)tempstr[c]]]] + 1;
			c++;
			s++;
		}
		if (w >= 577) {
			c--;
			while (tempstr[c] != ' ') {
				s--;
				c--;
			}
		}
		if (*s == '|') {
			s++;
		}
	}
	for (i = 7; i < 22; i++) {
		c = 0;
		w = 0;
		while (*s == '\0') {
			s++;
		}
		uint16_t style = UIS_SILVER;
		if (*s == '$') {
			s++;
			style = UIS_RED;
		}
		if (*s == '&') {
			HelpTop = help_select_line;
			continue;
		}
		while (*s != '|' && w < 577) {
			while (*s == '\0') {
				s++;
			}
			tempstr[c] = *s;
			BYTE tc = gbFontTransTbl[(BYTE)tempstr[c]];
			w += fontkern[GameFontSmall][fontframe[GameFontSmall][tc]] + 1;
			c++;
			s++;
		}
		if (w >= 577) {
			c--;
			while (tempstr[c] != ' ') {
				s--;
				c--;
			}
		}
		if (c != 0) {
			tempstr[c] = '\0';
			DrawHelpLine(out, 0, i, tempstr, style);
		}
		if (*s == '|') {
			s++;
		}
	}

	PrintSString(out, 0, 23, _("Press ESC to end or the arrow keys to scroll."), UIS_GOLD | UIS_CENTER);
}

void DisplayHelp()
{
	help_select_line = 0;
	helpflag = true;
	HelpTop = 5000;
}

void HelpScrollUp()
{
	if (help_select_line > 0)
		help_select_line--;
}

void HelpScrollDown()
{
	if (help_select_line < HelpTop)
		help_select_line++;
}

} // namespace devilution
