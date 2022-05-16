#include "itemlabels.h"

#include <deque>
#include <fmt/format.h>
#include <string>
#include <unordered_map>

#include "diablo.h"
#include "engine/render/text_render.hpp"
#include "floatingnumbers.h"
#include "utils/ui_fwd.h"

namespace devilution {

namespace {

struct FloatingNumber {
	bool myDmg;
	Point startPos;
	Displacement endOffset;
	std::string text;
	uint32_t time;
	uint32_t lastMerge;
	UiFlags style;
	FloatingType type;
	int value;
	int index;
};

constexpr int Lifetime = 2500; // in milliseconds;
constexpr int MergeTime = 150; // how long after the number got spawned new numbers will get merged into it (merging resets timer)
constexpr double ScreenPercentToTravel = 30;

std::deque<FloatingNumber> FloatingQueue;
std::unordered_map<int, Point> FloatingCoordsMap;

void ClearExpiredNumbers()
{
	while (!FloatingQueue.empty()) {
		FloatingNumber &num = FloatingQueue.front();
		if (num.time <= SDL_GetTicks())
			FloatingQueue.pop_front();
		else
			break;
	}
}

UiFlags GetFontSizeByDamage(int value)
{
	value >>= 6;
	if (value >= 500)
		return UiFlags::FontSize30;
	if (value >= 100)
		return UiFlags::FontSize24;
	return UiFlags::FontSize12;
}

void UpdateFloatingData(FloatingNumber &num)
{
	num.style &= ~(UiFlags::FontSize12 | UiFlags::FontSize24 | UiFlags::FontSize30);
	num.text = fmt::format("{:d}", num.value >> 6);
	switch (num.type) {
	case FloatingType::Experience:
		num.style |= UiFlags::ColorWhite | UiFlags::FontSize12;
		num.text = fmt::format("{:d} XP", num.value);
		break;
	case FloatingType::DamagePhysical:
		num.style |= UiFlags::ColorGold | GetFontSizeByDamage(num.value);
		break;
	case FloatingType::DamageFire:
		num.style |= UiFlags::ColorRed | GetFontSizeByDamage(num.value);
		break;
	case FloatingType::DamageLightning:
		num.style |= UiFlags::ColorBlue | GetFontSizeByDamage(num.value);
		break;
	case FloatingType::DamageMagic:
		num.style |= UiFlags::ColorWhitegold | GetFontSizeByDamage(num.value);
		break;
	case FloatingType::DamageAcid:
		num.style |= UiFlags::ColorDialogYellow | GetFontSizeByDamage(num.value);
		break;
	}
}

} // namespace

void AddFloatingNumber(bool isMyPlayer, Point pos, FloatingType type, int value, int index, UiFlags style)
{
	double distanceX = gnScreenWidth * ScreenPercentToTravel / 100;
	double distanceY = gnScreenHeight * ScreenPercentToTravel / 100;
	double angle = (500 + rand() % 500) * 0.0062831853;
	int xCoord = cos(angle) * distanceX;
	int yCoord = sin(angle) * distanceY;
	Displacement endOffset = { xCoord, yCoord };

	ClearExpiredNumbers();
	for (auto &num : FloatingQueue) {
		if (index != -1 && num.myDmg == isMyPlayer && num.type == type && num.index == index && (SDL_GetTicks() - (int)num.lastMerge <= MergeTime)) {
			num.value += value;
			num.lastMerge = SDL_GetTicks();
			UpdateFloatingData(num);
			return;
		}
	}
	FloatingNumber num = FloatingNumber {
		isMyPlayer, pos, endOffset, "", SDL_GetTicks() + Lifetime, SDL_GetTicks(), style, type, value, index
	};
	UpdateFloatingData(num);
	num.style |= style; // for handling styles from debug command
	FloatingQueue.push_back(num);
}

void DrawFloatingNumbers(const Surface &out)
{
	ClearExpiredNumbers();
	for (auto &floatingNum : FloatingQueue) {
		Point pos = floatingNum.startPos;
		pos = FloatingCoordsMap[pos.x + pos.y * MAXDUNX];
		if (pos == Point { 0, 0 })
			continue;
		constexpr int tileAndHalf = TILE_HEIGHT * 1.5;
		pos += { TILE_WIDTH / 2, -tileAndHalf };

		if (!zoomflag) {
			pos.x *= 2;
			pos.y *= 2;
		}
		pos.x -= GetLineWidth(floatingNum.text) / 2;
		uint32_t timeLeft = floatingNum.time - SDL_GetTicks();
		float mul = 1 - (timeLeft / (float)Lifetime);
		Point endPos = pos + floatingNum.endOffset * mul;
		if (endPos.x < 0 || endPos.x >= gnScreenWidth || endPos.y < 0 || endPos.y >= gnScreenHeight)
			continue;
		constexpr UiFlags allSizes = UiFlags::FontSize12 | UiFlags::FontSize24 | UiFlags::FontSize30 | UiFlags::FontSize42 | UiFlags::FontSize46;
		UiFlags shadowStyle = (floatingNum.style & allSizes) | UiFlags::ColorBlack;
		DrawString(out, floatingNum.text, endPos + Displacement { -1, -1 }, shadowStyle);
		DrawString(out, floatingNum.text, endPos + Displacement { 1, 1 }, shadowStyle);
		DrawString(out, floatingNum.text, endPos + Displacement { -1, 1 }, shadowStyle);
		DrawString(out, floatingNum.text, endPos + Displacement { 1, -1 }, shadowStyle);
		DrawString(out, floatingNum.text, endPos, floatingNum.style);
	}
}

void UpdateFloatingNumbersCoordsMap(Point dungeon, Point screen)
{
	FloatingCoordsMap[dungeon.x + dungeon.y * MAXDUNX] = screen;
}

void ClearFloatingNumbersCoordsMap()
{
	FloatingCoordsMap.clear();
}

FloatingType GetFloatingNumberTypeFromMissile(missile_resistance mir)
{
	switch (mir) {
	case MISR_FIRE:
		return FloatingType::DamageFire;
	case MISR_LIGHTNING:
		return FloatingType::DamageLightning;
	case MISR_MAGIC:
		return FloatingType::DamageMagic;
	case MISR_ACID:
		return FloatingType::DamageAcid;
	}
	return FloatingType::DamagePhysical;
}

} // namespace devilution
