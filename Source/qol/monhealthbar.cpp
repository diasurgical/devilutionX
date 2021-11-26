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
Art playerExpTags;

} // namespace

void InitMonsterHealthBar()
{
	if (!*sgOptions.Gameplay.enemyHealthBar)
		return;

	LoadMaskedArt("data\\healthbox.pcx", &healthBox, 1, 1);
	LoadArt("data\\health.pcx", &health);
	LoadMaskedArt("data\\resistance.pcx", &resistance, 6, 1);
	LoadMaskedArt("data\\monstertags.pcx", &playerExpTags, 5, 1);

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

void DrawMonsterHealthBar(const Surface &out)
{
	if (!*sgOptions.Gameplay.enemyHealthBar)
		return;

	assert(healthBox.surface != nullptr);
	assert(health.surface != nullptr);
	assert(resistance.surface != nullptr);

	if (currlevel == 0)
		return;
	if (pcursmonst == -1)
		return;

	const Monster &monster = Monsters[pcursmonst];

	const int width = healthBox.w();
	const int height = healthBox.h();
	Point position = { (gnScreenWidth - width) / 2, 18 };

	if (CanPanelsCoverView()) {
		if (invflag || sbookflag)
			position.x -= SPANEL_WIDTH / 2;
		if (chrflag || QuestLogIsOpen)
			position.x += SPANEL_WIDTH / 2;
	}

	const int border = 3;

	const int maxLife = std::max(monster._mmaxhp, monster._mhitpoints);

	DrawArt(out, position, &healthBox);
	DrawHalfTransparentRectTo(out, position.x + border, position.y + border, width - (border * 2), height - (border * 2));
	int barProgress = (width * monster._mhitpoints) / maxLife;
	if (barProgress != 0) {
		DrawArt(out, position + Displacement { border + 1, border + 1 }, &health, 0, barProgress, height - (border * 2) - 2);
	}

	constexpr auto getBorderColor = [](MonsterClass monsterClass) {
		switch (monsterClass) {
		case MonsterClass::Undead:
			return 248;

		case MonsterClass::Demon:
			return 232;

		case MonsterClass::Animal:
			return 150;

		default:
			app_fatal("Invalid monster class '%i'.", static_cast<int>(monsterClass));
		}
	};

	if (*sgOptions.Gameplay.showMonsterType) {
		Uint8 borderColor = getBorderColor(monster.MData->mMonstClass);
		int borderWidth = width - (border * 2);
		UnsafeDrawHorizontalLine(out, { position.x + border, position.y + border }, borderWidth, borderColor);
		UnsafeDrawHorizontalLine(out, { position.x + border, position.y + height - border - 1 }, borderWidth, borderColor);
		int borderHeight = height - (border * 2) - 2;
		UnsafeDrawVerticalLine(out, { position.x + border, position.y + border + 1 }, borderHeight, borderColor);
		UnsafeDrawVerticalLine(out, { position.x + width - border - 1, position.y + border + 1 }, borderHeight, borderColor);
	}

	UiFlags style = UiFlags::AlignCenter | UiFlags::VerticalCenter;
	DrawString(out, monster.mName, { position + Displacement { -1, 1 }, { width, height } }, style | UiFlags::ColorBlack);
	if (monster._uniqtype != 0)
		style |= UiFlags::ColorWhitegold;
	else if (monster.leader != 0)
		style |= UiFlags::ColorBlue;
	else
		style |= UiFlags::ColorWhite;
	DrawString(out, monster.mName, { position, { width, height } }, style);

	if (monster._uniqtype != 0 || MonsterKillCounts[monster.MType->mtype] >= 15) {
		monster_resistance immunes[] = { IMMUNE_MAGIC, IMMUNE_FIRE, IMMUNE_LIGHTNING };
		monster_resistance resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };

		int resOffset = 5;
		for (int i = 0; i < 3; i++) {
			if ((monster.mMagicRes & immunes[i]) != 0) {
				DrawArt(out, position + Displacement { resOffset, height - 6 }, &resistance, i * 2 + 1);
				resOffset += resistance.w() + 2;
			} else if ((monster.mMagicRes & resists[i]) != 0) {
				DrawArt(out, position + Displacement { resOffset, height - 6 }, &resistance, i * 2);
				resOffset += resistance.w() + 2;
			}
		}
	}

	int tagOffset = 5;
	for (int i = 0; i < MAX_PLRS; i++) {
		if (1 << i & monster.mWhoHit) {
			DrawArt(out, position + Displacement { tagOffset, height - 31 }, &playerExpTags, i + 1);
		} else if (Players[i].plractive) {
			DrawArt(out, position + Displacement { tagOffset, height - 31 }, &playerExpTags, 0);
		}
		tagOffset += playerExpTags.w();
	}
}

} // namespace devilution
