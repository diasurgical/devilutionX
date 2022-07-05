/**
 * @file monhealthbar.cpp
 *
 * Adds monster health bar QoL feature
 */

#include <fmt/format.h>

#include "DiabloUI/art_draw.h"
#include "control.h"
#include "cursor.h"
#include "options.h"
#include "utils/language.h"
#include "utils/str_cat.hpp"

namespace devilution {
namespace {

Art healthBox;
Art resistance;
Art health;
Art healthBlue;
Art playerExpTags;

} // namespace

void InitMonsterHealthBar()
{
	if (!*sgOptions.Gameplay.enemyHealthBar)
		return;

	LoadMaskedArt("data\\healthbox.pcx", &healthBox, 1, 1);
	LoadArt("data\\health.pcx", &health);
	std::array<uint8_t, 256> data;
	data[234] = 185;
	data[235] = 186;
	data[236] = 187;
	LoadMaskedArt("data\\health.pcx", &healthBlue, 1, 1, &data);
	LoadMaskedArt("data\\resistance.pcx", &resistance, 6, 1);
	LoadMaskedArt("data\\monstertags.pcx", &playerExpTags, 5, 1);

	if ((healthBox.surface == nullptr)
	    || (health.surface == nullptr)
	    || (resistance.surface == nullptr)) {
		app_fatal(_("Failed to load UI resources.\n"
		            "\n"
		            "Make sure devilutionx.mpq is in the game folder and that it is up to date."));
	}
}

void FreeMonsterHealthBar()
{
	healthBox.Unload();
	health.Unload();
	healthBlue.Unload();
	resistance.Unload();
}

void DrawMonsterHealthBar(const Surface &out)
{
	if (!*sgOptions.Gameplay.enemyHealthBar)
		return;

	assert(healthBox.surface != nullptr);
	assert(health.surface != nullptr);
	assert(healthBlue.surface != nullptr);
	assert(resistance.surface != nullptr);

	if (leveltype == DTYPE_TOWN)
		return;
	if (pcursmonst == -1)
		return;

	const Monster &monster = Monsters[pcursmonst];

	const int width = healthBox.w();
	const int barWidth = health.w();
	const int height = healthBox.h();
	Point position = { (gnScreenWidth - width) / 2, 18 };

	if (CanPanelsCoverView()) {
		if (IsRightPanelOpen())
			position.x -= SidePanelSize.width / 2;
		if (IsLeftPanelOpen())
			position.x += SidePanelSize.width / 2;
	}

	const int border = 3;

	int multiplier = 0;
	int currLife = monster.hitPoints;
	// lifestealing monsters can reach HP exceeding their max
	if (monster.hitPoints > monster.maxHitPoints) {
		multiplier = monster.hitPoints / monster.maxHitPoints;
		currLife = monster.hitPoints - monster.maxHitPoints * multiplier;
		if (currLife == 0 && multiplier > 0) {
			multiplier--;
			currLife = monster.maxHitPoints;
		}
	}

	DrawArt(out, position, &healthBox);
	DrawHalfTransparentRectTo(out, position.x + border, position.y + border, width - (border * 2), height - (border * 2));
	int barProgress = (barWidth * currLife) / monster.maxHitPoints;
	if (barProgress != 0) {
		DrawArt(out, position + Displacement { border + 1, border + 1 }, multiplier > 0 ? &healthBlue : &health, 0, barProgress, height - (border * 2) - 2);
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
			app_fatal(StrCat("Invalid monster class: ", static_cast<int>(monsterClass)));
		}
	};

	if (*sgOptions.Gameplay.showMonsterType) {
		Uint8 borderColor = getBorderColor(monster.data().mMonstClass);
		int borderWidth = width - (border * 2);
		UnsafeDrawHorizontalLine(out, { position.x + border, position.y + border }, borderWidth, borderColor);
		UnsafeDrawHorizontalLine(out, { position.x + border, position.y + height - border - 1 }, borderWidth, borderColor);
		int borderHeight = height - (border * 2) - 2;
		UnsafeDrawVerticalLine(out, { position.x + border, position.y + border + 1 }, borderHeight, borderColor);
		UnsafeDrawVerticalLine(out, { position.x + width - border - 1, position.y + border + 1 }, borderHeight, borderColor);
	}

	UiFlags style = UiFlags::AlignCenter | UiFlags::VerticalCenter;
	DrawString(out, monster.name, { position + Displacement { -1, 1 }, { width, height } }, style | UiFlags::ColorBlack);
	if (monster.uniqType != 0)
		style |= UiFlags::ColorWhitegold;
	else if (monster.leader != 0)
		style |= UiFlags::ColorBlue;
	else
		style |= UiFlags::ColorWhite;
	DrawString(out, monster.name, { position, { width, height } }, style);

	if (multiplier > 0)
		DrawString(out, StrCat("x", multiplier), { position, { width - 2, height } }, UiFlags::ColorWhite | UiFlags::AlignRight | UiFlags::VerticalCenter);
	if (monster.uniqType != 0 || MonsterKillCounts[monster.type().type] >= 15) {
		monster_resistance immunes[] = { IMMUNE_MAGIC, IMMUNE_FIRE, IMMUNE_LIGHTNING };
		monster_resistance resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };

		int resOffset = 5;
		for (int i = 0; i < 3; i++) {
			if ((monster.magicResistance & immunes[i]) != 0) {
				DrawArt(out, position + Displacement { resOffset, height - 6 }, &resistance, i * 2 + 1);
				resOffset += resistance.w() + 2;
			} else if ((monster.magicResistance & resists[i]) != 0) {
				DrawArt(out, position + Displacement { resOffset, height - 6 }, &resistance, i * 2);
				resOffset += resistance.w() + 2;
			}
		}
	}

	int tagOffset = 5;
	for (int i = 0; i < MAX_PLRS; i++) {
		if (1 << i & monster.whoHit) {
			DrawArt(out, position + Displacement { tagOffset, height - 31 }, &playerExpTags, i + 1);
		} else if (Players[i].plractive) {
			DrawArt(out, position + Displacement { tagOffset, height - 31 }, &playerExpTags, 0);
		}
		tagOffset += playerExpTags.w();
	}
}

} // namespace devilution
