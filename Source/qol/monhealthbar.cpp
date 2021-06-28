/**
* @file monhealthbar.cpp
*
* Adds monster health bar QoL feature
*/

#include "DiabloUI/art_draw.h"
#include "control.h"
#include "cursor.h"
#include "options.h"
#include "qol/common.h"
#include "utils/language.h"

namespace devilution {
namespace {

Art healthBox;
Art resistance;
Art health;

} // namespace

void InitMonsterHealthBar()
{
	if (!sgOptions.Gameplay.bEnemyHealthBar)
		return;

	LoadMaskedArt("data\\healthbox.pcx", &healthBox, 1, 1);
	LoadArt("data\\health.pcx", &health);
	LoadMaskedArt("data\\resistance.pcx", &resistance, 6, 1);

	if ((healthBox.surface == nullptr)
	    || (health.surface == nullptr)
	    || (resistance.surface == nullptr)) {
		app_fatal("%s", _("Failed to load UI resources.\n"
		                  "\n"
		                  "Make sure devilutionx.mpq is in the game folder and that it is up to date."));
	}
}

void FreeMonsterHealthBar()
{
	healthBox.Unload();
	health.Unload();
	resistance.Unload();
}

void DrawMonsterHealthBar(const CelOutputBuffer &out)
{
	if (!sgOptions.Gameplay.bEnemyHealthBar)
		return;

	assert(healthBox.surface != nullptr);
	assert(health.surface != nullptr);
	assert(resistance.surface != nullptr);

	if (currlevel == 0)
		return;
	if (pcursmonst == -1)
		return;

	const MonsterStruct &mon = monster[pcursmonst];

	const int width = healthBox.w();
	const int height = healthBox.h();
	Point position = { (gnScreenWidth - width) / 2, 18 };

	if (CanPanelsCoverView()) {
		if (invflag || sbookflag)
			position.x -= SPANEL_WIDTH / 2;
		if (chrflag || questlog)
			position.x += SPANEL_WIDTH / 2;
	}

	const int border = 3;

	const int maxLife = std::max(mon._mmaxhp, mon._mhitpoints);

	DrawArt(out, position, &healthBox);
	DrawHalfTransparentRectTo(out, position.x + border, position.y + border, width - (border * 2), height - (border * 2));
	int barProgress = (width * mon._mhitpoints) / maxLife;
	if (barProgress != 0) {
		DrawArt(out, position + Point { border + 1, border + 1 }, &health, 0, barProgress, height - (border * 2) - 2);
	}

	if (sgOptions.Gameplay.bShowMonsterType) {
		Uint8 borderColors[] = { 248 /*undead*/, 232 /*demon*/, 150 /*beast*/ };
		Uint8 borderColor = borderColors[mon.MData->mMonstClass];
		int borderWidth = width - (border * 2);
		UnsafeDrawHorizontalLine(out, { position.x + border, position.y + border }, borderWidth, borderColor);
		UnsafeDrawHorizontalLine(out, { position.x + border, position.y + height - border - 1 }, borderWidth, borderColor);
		int borderHeight = height - (border * 2) - 2;
		UnsafeDrawVerticalLine(out, { position.x + border, position.y + border + 1 }, borderHeight, borderColor);
		UnsafeDrawVerticalLine(out, { position.x + width - border - 1, position.y + border + 1 }, borderHeight, borderColor);
	}

	int barLabelY = position.y + 10 + (height - 11) / 2;
	DrawString(out, mon.mName, { position.x - 1, barLabelY + 1, width, height }, UIS_CENTER | UIS_BLACK);
	uint16_t style = UIS_SILVER;
	if (mon._uniqtype != 0)
		style = UIS_GOLD;
	else if (mon.leader != 0)
		style = UIS_BLUE;
	DrawString(out, mon.mName, { position.x, barLabelY, width, height }, UIS_CENTER | style);

	if (mon._uniqtype != 0 || monstkills[mon.MType->mtype] >= 15) {
		monster_resistance immunes[] = { IMMUNE_MAGIC, IMMUNE_FIRE, IMMUNE_LIGHTNING };
		monster_resistance resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };

		int resOffset = 5;
		for (int i = 0; i < 3; i++) {
			if ((mon.mMagicRes & immunes[i]) != 0) {
				DrawArt(out, position + Size { resOffset, height - 6 }, &resistance, i * 2 + 1);
				resOffset += resistance.w() + 2;
			} else if ((mon.mMagicRes & resists[i]) != 0) {
				DrawArt(out, position + Size { resOffset, height - 6 }, &resistance, i * 2);
				resOffset += resistance.w() + 2;
			}
		}
	}
}

} // namespace devilution
