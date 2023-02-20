#pragma once

#include <memory>
#include <vector>

#include "DiabloUI/diabloui.h"
#include "engine/point.hpp"

namespace devilution {

struct PlayerInfo {
	std::string name;
	HeroClass heroClass;
	uint8_t level;
	uint8_t diabloKillLevel;
	uint32_t gameMode;
	int latency;
};

extern std::vector<std::unique_ptr<UiItemBase>> vecHubMainDialog;
extern OptionalOwnedClxSpriteList Layout;

void selgame_GameSelection_Init();
void selgame_GameSelection_Focus(int value);
void selgame_GameSelection_Esc();
void selgame_Diff_Select(int value);
void LoadHubScrollBar();
void UiHubPlacePlayerIcon(Point position, uint32_t gameMode, const PlayerInfo &player);
void UiHubPlaceLatencyMeter(int latency, Point position);

} // namespace devilution
