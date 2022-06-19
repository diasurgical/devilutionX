#pragma once

#include <map>
#include <string>

#include "player.h"

namespace devilution {

struct Statistics {
	uint32_t deathCount = 0;
	uint64_t ingameTime = 0;
	uint32_t ticksSubstrahend = SDL_GetTicks();
};

extern std::map<std::string, std::string> statisticsFile;
extern Statistics myPlayerStatistics;

void InitializePlayerStatistics();
void LoadStatisticsFromMap();
void SaveStatisticsToMap();
void CalculateInGameTime();

} // namespace devilution
