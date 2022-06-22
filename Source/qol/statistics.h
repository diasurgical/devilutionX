#pragma once

#include <map>
#include <string>

#include "player.h"

namespace devilution {

struct Statistics {
	uint32_t deathCount = 0;
	uint64_t ingameTime = 0;
	uint32_t ticksSubtrahend = SDL_GetTicks();
};

extern std::map<std::string, std::string> StatisticsFile;
extern Statistics MyPlayerStatistics;

void InitializePlayerStatistics();
void LoadStatisticsFromMap();
void SaveStatisticsToMap();
void CalculateInGameTime();

} // namespace devilution
