#pragma once

#include <map>
#include <string>

#include "player.h"

namespace devilution {

struct Statistics {
	uint32_t deathCount = 0;
	uint64_t ingameTime = 0;
	uint64_t ticksSubstrahend = SDL_GetTicks64();
};

extern std::map<std::string, std::string> statisticsFile;

void InitializePlayerStatistics(Player &player);
void LoadStatisticsFromMap();
void SaveStatisticsToMap();
void CalculateInGameTime();

} // namespace devilution
