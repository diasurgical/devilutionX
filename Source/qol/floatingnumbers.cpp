#include "itemlabels.h"

#include <deque>
#include <fmt/format.h>
#include <string>
#include <unordered_set>

#include "diablo.h"
#include "engine/render/text_render.hpp"
#include "floatingnumbers.h"
#include "utils/ui_fwd.h"

namespace devilution {

namespace {

struct FloatingNumber {
	Point startPos;
	Displacement endOffset;
	std::string text;
	uint32_t time;
	UiFlags color;
};

constexpr int Lifetime = 3000; // in milliseconds;
constexpr double ScreenPercentToTravel = 30;

std::deque<FloatingNumber> FloatingQueue;
std::unordered_map<int, Point> FloatingCoordsMap;

} // namespace

void AddFloatingNumber(bool isMyPlayer, Point pos, FloatingType type, int value)
{
	double distanceX = gnScreenWidth * ScreenPercentToTravel / 100;
	double distanceY = gnScreenHeight * ScreenPercentToTravel / 100;
	double angle = (rand() % 1000) * 0.0062831853;
	int xCoord = cos(angle) * distanceX;
	int yCoord = sin(angle) * distanceY;
	Displacement endOffset = { xCoord, yCoord };
	std::string text = fmt::format("{:d}", value);
	UiFlags color = UiFlags::ColorBlue;

	switch (type) {
	case FloatingType::Experience:
		color = UiFlags::ColorWhite;
		text = fmt::format("{:d} XP", value);
		break;
	case FloatingType::DamagePhysical:
		color = UiFlags::ColorGold;
		break;
	case FloatingType::DamageFire:
		color = UiFlags::ColorRed;
		break;
	case FloatingType::DamageLightning:
		color = UiFlags::ColorBlue;
		break;
	case FloatingType::DamageMagic:
		color = UiFlags::ColorWhitegold;
		break;
	case FloatingType::DamageAcid:
		color = UiFlags::ColorDialogYellow;
		break;
	}
	FloatingQueue.push_back(FloatingNumber { pos, endOffset, text, SDL_GetTicks() + Lifetime, color });
}

void DrawFloatingNumbers(const Surface &out)
{
	while (!FloatingQueue.empty()) {
		FloatingNumber &num = FloatingQueue.front();
		if (num.time <= SDL_GetTicks())
			FloatingQueue.pop_front();
		else
			break;
	}
	for (auto & floatingNum : FloatingQueue) {
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
		DrawString(out, floatingNum.text, endPos, floatingNum.color);
	}
}

void UpdateFloatingNumbersCoordsMap(Point dungeon, Point screen)
{
	FloatingCoordsMap[dungeon.x + dungeon.y * MAXDUNX] = screen;
}

} // namespace devilution
