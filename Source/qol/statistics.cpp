#include "qol/statistics.h"

namespace devilution {

Statistics myPlayerStatistics;
std::map<std::string, std::string> statisticsFile;

void InitializePlayerStatistics(Player &player)
{
	myPlayerStatistics = {};
	player.statistics = &myPlayerStatistics;
}

void LoadStatisticsFromMap()
{
	Player &myPlayer = *MyPlayer;
	if (statisticsFile.find("deathCount") != statisticsFile.end())
		myPlayer.statistics->deathCount = std::stoul(statisticsFile.at("deathCount"));

	if (statisticsFile.find("ingameTime") != statisticsFile.end())
		myPlayer.statistics->ingameTime = std::stoul(statisticsFile.at("ingameTime"));
}

void SaveStatisticsToMap()
{
	Player &myPlayer = *MyPlayer;
	statisticsFile["deathCount"] = std::to_string(myPlayer.statistics->deathCount);
	statisticsFile["ingameTime"] = std::to_string(myPlayer.statistics->ingameTime);
}

void CalculateInGameTime()
{
	Statistics *stats = MyPlayer->statistics;
	uint64_t ticksNow = SDL_GetTicks64();
	stats->ingameTime += (ticksNow - stats->ticksSubstrahend);
	stats->ticksSubstrahend = ticksNow;
}

} // namespace devilution
