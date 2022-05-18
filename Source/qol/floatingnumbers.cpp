#include "itemlabels.h"

#include <deque>
#include <fmt/format.h>
#include <string>
#include <unordered_map>

#include "diablo.h"
#include "engine/render/text_render.hpp"
#include "floatingnumbers.h"
#include "options.h"
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
	if (*sgOptions.FloatingNumbers.bigThreshold != -1 && value >= *sgOptions.FloatingNumbers.bigThreshold)
		return UiFlags::FontSize30;
	if (*sgOptions.FloatingNumbers.mediumThreshold != -1 && value >= *sgOptions.FloatingNumbers.mediumThreshold)
		return UiFlags::FontSize24;
	return UiFlags::FontSize12;
}

UiFlags IndexToFlag(int index)
{
	return static_cast<UiFlags>(1ULL << index);
}

void UpdateFloatingData(FloatingNumber &num)
{
	num.style &= ~(UiFlags::FontSize12 | UiFlags::FontSize24 | UiFlags::FontSize30);
	num.text = fmt::format("{:d}", num.value >> 6);
	switch (num.type) {
	case FloatingType::Experience:
		num.style |= IndexToFlag(*sgOptions.FloatingNumbers.expGainColor) | UiFlags::FontSize12;
		num.text = fmt::format("{:d} XP", num.value);
		break;
	case FloatingType::DamagePhysical:
		num.style |= IndexToFlag(*sgOptions.FloatingNumbers.physicalDamageColor) | GetFontSizeByDamage(num.value);
		break;
	case FloatingType::DamageFire:
		num.style |= IndexToFlag(*sgOptions.FloatingNumbers.fireDamageColor) | GetFontSizeByDamage(num.value);
		break;
	case FloatingType::DamageLightning:
		num.style |= IndexToFlag(*sgOptions.FloatingNumbers.lightningDamageColor) | GetFontSizeByDamage(num.value);
		break;
	case FloatingType::DamageMagic:
		num.style |= IndexToFlag(*sgOptions.FloatingNumbers.magicDamageColor) | GetFontSizeByDamage(num.value);
		break;
	case FloatingType::DamageAcid:
		num.style |= IndexToFlag(*sgOptions.FloatingNumbers.acidDamageColor) | GetFontSizeByDamage(num.value);
		break;
	}
}

} // namespace

void AddFloatingNumber(bool isMyPlayer, Point pos, FloatingType type, int value, int index, UiFlags style)
{
	if (!*sgOptions.FloatingNumbers.enableFloatingNumbers)
		return;
	if (!*sgOptions.FloatingNumbers.floatingNumbersFromOthers && !isMyPlayer)
		return;
	double distanceX = gnScreenWidth * *sgOptions.FloatingNumbers.maxHorizontalDistance / 100.0;
	double distanceY = gnScreenHeight * *sgOptions.FloatingNumbers.maxVerticalDistance / 100.0;
	constexpr double PI = 3.14159265358979323846;
	constexpr double mul = PI * 2 / 1000.0;
	double angle = (500 + rand() % 500) * mul;
	int xCoord = cos(angle) * distanceX;
	int yCoord = sin(angle) * distanceY;
	Displacement endOffset = { xCoord, yCoord };

	ClearExpiredNumbers();
	for (auto &num : FloatingQueue) {
		if (index != -1 && num.myDmg == isMyPlayer && num.type == type && num.index == index && (SDL_GetTicks() - (int)num.lastMerge <= *sgOptions.FloatingNumbers.mergeTime)) {
			num.value += value;
			num.lastMerge = SDL_GetTicks();
			UpdateFloatingData(num);
			return;
		}
	}
	FloatingNumber num = FloatingNumber {
		isMyPlayer, pos, endOffset, "", SDL_GetTicks() + *sgOptions.FloatingNumbers.floatingNumbersLifetime, SDL_GetTicks(), style, type, value, index
	};
	UpdateFloatingData(num);
	num.style |= style; // for handling styles from debug command
	FloatingQueue.push_back(num);
}

void DrawFloatingNumbers(const Surface &out)
{
	if (!*sgOptions.FloatingNumbers.enableFloatingNumbers)
		return;
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
		float mul = 1 - (timeLeft / (float)*sgOptions.FloatingNumbers.floatingNumbersLifetime);
		Point endPos = pos + floatingNum.endOffset * mul;
		if (endPos.x < 0 || endPos.x >= gnScreenWidth || endPos.y < 0 || endPos.y >= gnScreenHeight)
			continue;
		DrawString(out, floatingNum.text, endPos, floatingNum.style, 1, -1, 0);
	}
}

void UpdateFloatingNumbersCoordsMap(Point dungeon, Point screen)
{
	if (!*sgOptions.FloatingNumbers.enableFloatingNumbers)
		return;
	FloatingCoordsMap[dungeon.x + dungeon.y * MAXDUNX] = screen;
}

void ClearFloatingNumbersCoordsMap()
{
	if (!*sgOptions.FloatingNumbers.enableFloatingNumbers)
		return;
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
