/**
 * @file qol.cpp
 *
 * Quality of life features
 */
#include "all.h"
#include "options.h"
#include "DiabloUI/art_draw.h"

DEVILUTION_BEGIN_NAMESPACE
namespace {

struct QolArt {
	Art healthBox;
	Art resistance;
	Art health;
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
	if (sgOptions.Gameplay.bEnemyHealthBar) {
		qolArt = new QolArt();
		LoadMaskedArt("data\\healthbox.pcx", &qolArt->healthBox, 1, 1);
		LoadArt("data\\health.pcx", &qolArt->health);
		LoadMaskedArt("data\\resistance.pcx", &qolArt->resistance, 6, 1);

		if ((qolArt->healthBox.surface == nullptr)
		    || (qolArt->health.surface == nullptr)
		    || (qolArt->resistance.surface == nullptr)) {
			app_fatal("Failed to load UI resources. Is devilutionx.mpq accessible and up to date?");
		}
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

	int barWidth = 306;
	int barHeight = 5;
	int yPos = gnScreenHeight - 9;                 // y position of xp bar
	int xPos = (gnScreenWidth - barWidth) / 2 + 5; // x position of xp bar
	int dividerHeight = 3;
	int numDividers = 10;
	int barColor = 198;
	int emptyBarColor = 0;
	int frameColor = 196;
	bool space = true; // add 1 pixel separator on top/bottom of the bar

	PrintGameStr(out, xPos - 22, yPos + 6, "XP", COL_WHITE);
	int charLevel = plr[myplr]._pLevel;
	if (charLevel == MAXCHARLEVEL - 1)
		return;

	int prevXp = ExpLvlsTbl[charLevel - 1];
	if (plr[myplr]._pExperience < prevXp)
		return;

	Uint64 prevXpDelta_1 = plr[myplr]._pExperience - prevXp;
	int prevXpDelta = ExpLvlsTbl[charLevel] - prevXp;
	int visibleBar = barWidth * prevXpDelta_1 / prevXpDelta;

	FillRect(out, xPos, yPos, barWidth, barHeight, emptyBarColor);
	FastDrawHorizLine(out, xPos - 1, yPos - 1, barWidth + 2, frameColor);
	FastDrawHorizLine(out, xPos - 1, yPos + barHeight, barWidth + 2, frameColor);
	FastDrawVertLine(out, xPos - 1, yPos - 1, barHeight + 2, frameColor);
	FastDrawVertLine(out, xPos + barWidth, yPos - 1, barHeight + 2, frameColor);
	for (int i = 1; i < numDividers; i++)
		FastDrawVertLine(out, xPos - 1 + (barWidth * i / numDividers), yPos - dividerHeight + 3, barHeight, 245);

	FillRect(out, xPos, yPos + (space ? 1 : 0), visibleBar, barHeight - (space ? 2 : 0), barColor);
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
			if (item[itemIndex]._itype == ITYPE_GOLD) {
				NetSendCmdGItem(TRUE, CMD_REQUESTAGITEM, pnum, pnum, itemIndex);
				item[itemIndex]._iRequest = TRUE;
				PlaySFX(IS_IGRAB);
			}
		}
	}
}

DEVILUTION_END_NAMESPACE
