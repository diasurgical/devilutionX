/**
 * @file qol.cpp
 *
 * Quality of life features
 */

#include "control.h"
#include "cursor.h"
#include "DiabloUI/art_draw.h"
#include "options.h"

namespace devilution {
namespace {

namespace xpbar {
constexpr int barWidth = 307;
constexpr int barHeight = 5;

constexpr int goldGradient[] = { 0xCF, 0xCE, 0xCD, 0xCC, 0xCB, 0xCA, 0xC9, 0xC8, 0xC7, 0xC6, 0xC5, 0xC4 };
constexpr int goldGrades = SDL_arraysize(goldGradient);

constexpr int whiteGradient[] = { 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3 };
constexpr int whiteGrades = SDL_arraysize(whiteGradient);

constexpr int backWidth = 313;
constexpr int backHeight = 9;
} // namespace xpbar

struct QolArt {
	Art healthBox;
	Art resistance;
	Art health;
	Art xpbar;
};

QolArt *qolArt = nullptr;

int GetTextWidth(const char *s)
{
	int l = 0;
	while (*s) {
		l += fontkern[fontframe[gbFontTransTbl[(BYTE)*s++]]] + 1;
	}
	return l;
}

/**
 * @brief Prints integer into buffer, using ',' as thousands separator.
 * @param out Destination buffer
 * @param n Number to print
 * @return Address of first character after printed number
*/
char *PrintWithSeparator(char *out, long long n)
{
	if (n < 1000) {
		return out + sprintf(out, "%lld", n);
	}

	char *append = PrintWithSeparator(out, n / 1000);
	return append + sprintf(append, ",%03lld", n % 1000);
}

void FastDrawHorizLine(CelOutputBuffer out, int x, int y, int width, BYTE col)
{
	memset(out.at(x, y), col, width);
}

void FastDrawVertLine(CelOutputBuffer out, int x, int y, int height, BYTE col)
{
	BYTE *p = out.at(x, y);
	for (int j = 0; j < height; j++) {
		*p = col;
		p += out.pitch();
	}
}

void FillRect(CelOutputBuffer out, int x, int y, int width, int height, BYTE col)
{
	for (int j = 0; j < height; j++) {
		FastDrawHorizLine(out, x, y + j, width, col);
	}
}

} // namespace

void FreeQol()
{
	delete qolArt;
	qolArt = nullptr;
}

void InitQol()
{
	const bool needsQolArt = (sgOptions.Gameplay.bEnemyHealthBar || sgOptions.Gameplay.bExperienceBar);

	if (needsQolArt)
		qolArt = new QolArt();

	if (sgOptions.Gameplay.bEnemyHealthBar) {
		LoadMaskedArt("data\\healthbox.pcx", &qolArt->healthBox, 1, 1);
		LoadArt("data\\health.pcx", &qolArt->health);
		LoadMaskedArt("data\\resistance.pcx", &qolArt->resistance, 6, 1);

		if ((qolArt->healthBox.surface == nullptr)
		    || (qolArt->health.surface == nullptr)
		    || (qolArt->resistance.surface == nullptr)) {
			app_fatal("Failed to load UI resources. Is devilutionx.mpq accessible and up to date?");
		}
	}

	if (sgOptions.Gameplay.bExperienceBar) {
		LoadMaskedArt("data\\xpbar.pcx", &qolArt->xpbar, 1, 1);
	}
}

void DrawMonsterHealthBar(CelOutputBuffer out)
{
	if (!sgOptions.Gameplay.bEnemyHealthBar)
		return;
	assert(qolArt != nullptr);
	assert(qolArt->healthBox.surface != nullptr);
	assert(qolArt->health.surface != nullptr);
	assert(qolArt->resistance.surface != nullptr);
	if (currlevel == 0)
		return;
	if (pcursmonst == -1)
		return;

	MonsterStruct *mon = &monster[pcursmonst];

	Sint32 width = qolArt->healthBox.w();
	Sint32 height = qolArt->healthBox.h();
	Sint32 xPos = (gnScreenWidth - width) / 2;

	if (PANELS_COVER) {
		if (invflag || sbookflag)
			xPos -= SPANEL_WIDTH / 2;
		if (chrflag || questlog)
			xPos += SPANEL_WIDTH / 2;
	}

	Sint32 yPos = 18;
	Sint32 border = 3;

	Sint32 maxLife = mon->_mmaxhp;
	if (mon->_mhitpoints > maxLife)
		maxLife = mon->_mhitpoints;

	DrawArt(out, xPos, yPos, &qolArt->healthBox);
	DrawHalfTransparentRectTo(out, xPos + border, yPos + border, width - (border * 2), height - (border * 2));
	int barProgress = (width * mon->_mhitpoints) / maxLife;
	if (barProgress) {
		DrawArt(out, xPos + border + 1, yPos + border + 1, &qolArt->health, 0, barProgress, height - (border * 2) - 2);
	}

	if (sgOptions.Gameplay.bShowMonsterType) {
		Uint8 borderColors[] = { 248 /*undead*/, 232 /*demon*/, 150 /*beast*/ };
		Uint8 borderColor = borderColors[mon->MData->mMonstClass];
		Sint32 borderWidth = width - (border * 2);
		FastDrawHorizLine(out, xPos + border, yPos + border, borderWidth, borderColor);
		FastDrawHorizLine(out, xPos + border, yPos + height - border - 1, borderWidth, borderColor);
		Sint32 borderHeight = height - (border * 2) - 2;
		FastDrawVertLine(out, xPos + border, yPos + border + 1, borderHeight, borderColor);
		FastDrawVertLine(out, xPos + width - border - 1, yPos + border + 1, borderHeight, borderColor);
	}

	Sint32 barLableX = xPos + width / 2 - GetTextWidth(mon->mName) / 2;
	Sint32 barLableY = yPos + 10 + (height - 11) / 2;
	PrintGameStr(out, barLableX - 1, barLableY + 1, mon->mName, COL_BLACK);
	text_color color = COL_WHITE;
	if (mon->_uniqtype != 0)
		color = COL_GOLD;
	else if (mon->leader != 0)
		color = COL_BLUE;
	PrintGameStr(out, barLableX, barLableY, mon->mName, color);

	if (mon->_uniqtype != 0 || monstkills[mon->MType->mtype] >= 15) {
		monster_resistance immunes[] = { IMMUNE_MAGIC, IMMUNE_FIRE, IMMUNE_LIGHTNING };
		monster_resistance resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };

		Sint32 resOffset = 5;
		for (Sint32 i = 0; i < 3; i++) {
			if (mon->mMagicRes & immunes[i]) {
				DrawArt(out, xPos + resOffset, yPos + height - 6, &qolArt->resistance, i * 2 + 1);
				resOffset += qolArt->resistance.w() + 2;
			} else if (mon->mMagicRes & resists[i]) {
				DrawArt(out, xPos + resOffset, yPos + height - 6, &qolArt->resistance, i * 2);
				resOffset += qolArt->resistance.w() + 2;
			}
		}
	}
}

void DrawXPBar(CelOutputBuffer out)
{
	if (!sgOptions.Gameplay.bExperienceBar)
		return;

	const PlayerStruct &player = plr[myplr];

	const int backX = PANEL_LEFT + PANEL_WIDTH / 2 - 155;
	const int backY = PANEL_TOP + PANEL_HEIGHT - 11;

	const int xPos = backX + 3;
	const int yPos = backY + 2;

	DrawArt(out, backX, backY, &qolArt->xpbar);

	const int charLevel = player._pLevel;

	if (charLevel == MAXCHARLEVEL - 1) {
		// Draw a nice golden bar for max level characters.
		FastDrawHorizLine(out, xPos, yPos + 1, xpbar::barWidth, xpbar::goldGradient[xpbar::goldGrades * 3 / 4 - 1]);
		FastDrawHorizLine(out, xPos, yPos + 2, xpbar::barWidth, xpbar::goldGradient[xpbar::goldGrades - 1]);
		FastDrawHorizLine(out, xPos, yPos + 3, xpbar::barWidth, xpbar::goldGradient[xpbar::goldGrades / 2 - 1]);

		return;
	}

	const int prevXp = ExpLvlsTbl[charLevel - 1];
	if (player._pExperience < prevXp)
		return;

	Uint64 prevXpDelta_1 = player._pExperience - prevXp;
	Uint64 prevXpDelta = ExpLvlsTbl[charLevel] - prevXp;
	Uint64 fullBar = xpbar::barWidth * prevXpDelta_1 / prevXpDelta;

	// Figure out how much to fill the last pixel of the XP bar, to make it gradually appear with gained XP
	Uint64 onePx = prevXpDelta / xpbar::barWidth;
	Uint64 lastFullPx = fullBar * prevXpDelta / xpbar::barWidth;

	const Uint64 fade = (prevXpDelta_1 - lastFullPx) * (xpbar::whiteGrades - 1) / onePx;

	// Draw beginning of bar full brightness
	FastDrawHorizLine(out, xPos, yPos + 1, fullBar, xpbar::whiteGradient[xpbar::whiteGrades * 3 / 4 - 1]);
	FastDrawHorizLine(out, xPos, yPos + 2, fullBar, xpbar::whiteGradient[xpbar::whiteGrades - 1]);
	FastDrawHorizLine(out, xPos, yPos + 3, fullBar, xpbar::whiteGradient[xpbar::whiteGrades / 2 - 1]);

	// End pixels appear gradually
	SetPixel(out, xPos + fullBar, yPos + 1, xpbar::whiteGradient[fade * 3 / 4]);
	SetPixel(out, xPos + fullBar, yPos + 2, xpbar::whiteGradient[fade]);
	SetPixel(out, xPos + fullBar, yPos + 3, xpbar::whiteGradient[fade / 2]);
}

bool CheckXPBarInfo()
{
	if (!sgOptions.Gameplay.bExperienceBar)
		return false;

	const int backX = PANEL_LEFT + PANEL_WIDTH / 2 - 155;
	const int backY = PANEL_TOP + PANEL_HEIGHT - 11;

	if (MouseX < backX || MouseX >= backX + xpbar::backWidth || MouseY < backY || MouseY >= backY + xpbar::backHeight)
		return false;

	const PlayerStruct &player = plr[myplr];

	const int charLevel = player._pLevel;

	char tempstr[64];

	if (charLevel == MAXCHARLEVEL - 1) {
		// Show a maximum level indicator for max level players.
		infoclr = COL_GOLD;

		sprintf(tempstr, "Level %d", charLevel);
		AddPanelString(tempstr, true);

		sprintf(tempstr, "Experience: ");
		PrintWithSeparator(tempstr + SDL_arraysize("Experience: ") - 1, ExpLvlsTbl[charLevel - 1]);
		AddPanelString(tempstr, true);

		AddPanelString("Maximum Level", true);

		return true;
	}

	infoclr = COL_WHITE;

	sprintf(tempstr, "Level %d", charLevel);
	AddPanelString(tempstr, true);

	sprintf(tempstr, "Experience: ");
	PrintWithSeparator(tempstr + SDL_arraysize("Experience: ") - 1, player._pExperience);
	AddPanelString(tempstr, true);

	sprintf(tempstr, "Next Level: ");
	PrintWithSeparator(tempstr + SDL_arraysize("Next Level: ") - 1, ExpLvlsTbl[charLevel]);
	AddPanelString(tempstr, true);

	sprintf(PrintWithSeparator(tempstr, ExpLvlsTbl[charLevel] - player._pExperience), " to Level %d", charLevel + 1);
	AddPanelString(tempstr, true);

	return true;
}

bool HasRoomForGold()
{
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		int idx = plr[myplr].InvGrid[i];

		// Secondary item cell. No need to check those as we'll go through the main item cells anyway.
		if (idx < 0)
			continue;

		// Empty cell. 1x1 space available.
		if (idx == 0)
			return true;

		// Main item cell. Potentially a gold pile so check it.
		auto item = plr[myplr].InvList[idx - 1];
		if (item._itype == ITYPE_GOLD && item._ivalue < MaxGold)
			return true;
	}

	return false;
}

void AutoGoldPickup(int pnum)
{
	if (!sgOptions.Gameplay.bAutoGoldPickup)
		return;
	if (pnum != myplr)
		return;
	if (leveltype == DTYPE_TOWN)
		return;
	if (!HasRoomForGold())
		return;

	for (int dir = 0; dir < 8; dir++) {
		int x = plr[pnum]._px + pathxdir[dir];
		int y = plr[pnum]._py + pathydir[dir];
		if (dItem[x][y] != 0) {
			int itemIndex = dItem[x][y] - 1;
			if (items[itemIndex]._itype == ITYPE_GOLD) {
				NetSendCmdGItem(true, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
				items[itemIndex]._iRequest = true;
				PlaySFX(IS_IGRAB);
			}
		}
	}
}

} // namespace devilution
