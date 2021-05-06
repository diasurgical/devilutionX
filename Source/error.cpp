/**
 * @file error.cpp
 *
 * Implementation of in-game message functions.
 */
#include "error.h"

#include "control.h"
#include "engine/render/cel_render.hpp"
#include "stores.h"
#include "utils/language.h"

namespace devilution {

diablo_message msgtable[MAX_SEND_STR_LEN];
DWORD msgdelay;
diablo_message msgflag;
uint8_t msgcnt;

/** Maps from error_id to error message. */
const char *const MsgStrings[] = {
	"",
	N_("No automap available in town"),
	N_("No multiplayer functions in demo"),
	N_("Direct Sound Creation Failed"),
	N_("Not available in shareware version"),
	N_("Not enough space to save"),
	N_("No Pause in town"),
	N_("Copying to a hard disk is recommended"),
	N_("Multiplayer sync problem"),
	N_("No pause in multiplayer"),
	N_("Loading..."),
	N_("Saving..."),
	N_("Some are weakened as one grows strong"),
	N_("New strength is forged through destruction"),
	N_("Those who defend seldom attack"),
	N_("The sword of justice is swift and sharp"),
	N_("While the spirit is vigilant the body thrives"),
	N_("The powers of mana refocused renews"),
	N_("Time cannot diminish the power of steel"),
	N_("Magic is not always what it seems to be"),
	N_("What once was opened now is closed"),
	N_("Intensity comes at the cost of wisdom"),
	N_("Arcane power brings destruction"),
	N_("That which cannot be held cannot be harmed"),
	N_("Crimson and Azure become as the sun"),
	N_("Knowledge and wisdom at the cost of self"),
	N_("Drink and be refreshed"),
	N_("Wherever you go, there you are"),
	N_("Energy comes at the cost of wisdom"),
	N_("Riches abound when least expected"),
	N_("Where avarice fails, patience gains reward"),
	N_("Blessed by a benevolent companion!"),
	N_("The hands of men may be guided by fate"),
	N_("Strength is bolstered by heavenly faith"),
	N_("The essence of life flows from within"),
	N_("The way is made clear when viewed from above"),
	N_("Salvation comes at the cost of wisdom"),
	N_("Mysteries are revealed in the light of reason"),
	N_("Those who are last may yet be first"),
	N_("Generosity brings its own rewards"),
	N_("You must be at least level 8 to use this."),
	N_("You must be at least level 13 to use this."),
	N_("You must be at least level 17 to use this."),
	N_("Arcane knowledge gained!"),
	N_("That which does not kill you..."),
	N_("Knowledge is power."),
	N_("Give and you shall receive."),
	N_("Some experience is gained by touch."),
	N_("There's no place like home."),
	N_("Spiritual energy is restored."),
	N_("You feel more agile."),
	N_("You feel stronger."),
	N_("You feel wiser."),
	N_("You feel refreshed."),
	N_("That which can break will."),
};

void InitDiabloMsg(diablo_message e)
{
	int i;

	if (msgcnt >= sizeof(msgtable))
		return;

	for (i = 0; i < msgcnt; i++) {
		if (msgtable[i] == e)
			return;
	}

	msgtable[msgcnt] = e; // BUGFIX: missing out-of-bounds check (fixed)
	msgcnt++;

	msgflag = msgtable[0];
	msgdelay = SDL_GetTicks();
}

void ClrDiabloMsg()
{
	for (auto &msg : msgtable)
		msg = EMSG_NONE;

	msgflag = EMSG_NONE;
	msgcnt = 0;
}

#define DIALOG_Y ((gnScreenHeight - PANEL_HEIGHT) / 2 - 18)

void DrawDiabloMsg(const CelOutputBuffer &out)
{
	int i, len, width, sx, sy;
	BYTE c;

	CelDrawTo(out, PANEL_X + 101, DIALOG_Y, *pSTextSlidCels, 1);
	CelDrawTo(out, PANEL_X + 527, DIALOG_Y, *pSTextSlidCels, 4);
	CelDrawTo(out, PANEL_X + 101, DIALOG_Y + 48, *pSTextSlidCels, 2);
	CelDrawTo(out, PANEL_X + 527, DIALOG_Y + 48, *pSTextSlidCels, 3);

	sx = PANEL_X + 109;
	for (i = 0; i < 35; i++) {
		CelDrawTo(out, sx, DIALOG_Y, *pSTextSlidCels, 5);
		CelDrawTo(out, sx, DIALOG_Y + 48, *pSTextSlidCels, 7);
		sx += 12;
	}
	sy = DIALOG_Y + 12;
	for (i = 0; i < 3; i++) {
		CelDrawTo(out, PANEL_X + 101, sy, *pSTextSlidCels, 6);
		CelDrawTo(out, PANEL_X + 527, sy, *pSTextSlidCels, 8);
		sy += 12;
	}

	DrawHalfTransparentRectTo(out, PANEL_X + 104, DIALOG_Y - 8, 432, 54);

	strcpy(tempstr, _(MsgStrings[msgflag]));
	sx = PANEL_X + 101;
	sy = DIALOG_Y + 24;
	len = strlen(tempstr);
	width = 0;

	for (i = 0; i < len; i++) {
		width += fontkern[fontframe[gbFontTransTbl[(BYTE)tempstr[i]]]] + 1;
	}

	if (width < 442) {
		sx += (442 - width) / 2;
	}

	for (i = 0; i < len; i++) {
		c = fontframe[gbFontTransTbl[(BYTE)tempstr[i]]];
		if (c != '\0') {
			PrintChar(out, sx, sy, c, COL_GOLD);
		}
		sx += fontkern[c] + 1;
	}

	if (msgdelay > 0 && msgdelay <= SDL_GetTicks() - 3500) {
		msgdelay = 0;
	}
	if (msgdelay == 0) {
		msgcnt--;
		if (msgcnt == 0) {
			msgflag = EMSG_NONE;
		} else {
			msgflag = msgtable[msgcnt];
			msgdelay = SDL_GetTicks();
		}
	}
}

} // namespace devilution
