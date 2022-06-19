#include "qol/statistics.h"

namespace devilution {

Statistics myPlayerStatistics;
std::map<std::string, std::string> statisticsFile;

void SaveMonsterKillCount();
void LoadMonsterKillCount();
std::string GetMonsterName(uint16_t monsterID);
void AddMonsterWeapon(std::string &monsterName, uint16_t monsterID);

void InitializePlayerStatistics()
{
	myPlayerStatistics = {};
	std::fill_n(MonsterKillCounts, MAXMONSTERS, 0);
}

void LoadStatisticsFromMap()
{
	if (statisticsFile.find("deathCount") != statisticsFile.end())
		myPlayerStatistics.deathCount = std::stoul(statisticsFile.at("deathCount"));

	if (statisticsFile.find("ingameTime") != statisticsFile.end())
		myPlayerStatistics.ingameTime = std::stoul(statisticsFile.at("ingameTime"));
	LoadMonsterKillCount();
}

void SaveStatisticsToMap()
{
	statisticsFile["deathCount"] = std::to_string(myPlayerStatistics.deathCount);
	statisticsFile["ingameTime"] = std::to_string(myPlayerStatistics.ingameTime);
	SaveMonsterKillCount();
}

void CalculateInGameTime()
{
	uint32_t ticksNow = SDL_GetTicks();
	myPlayerStatistics.ingameTime += (ticksNow - myPlayerStatistics.ticksSubstrahend);
	myPlayerStatistics.ticksSubstrahend = ticksNow;
}

void LoadMonsterKillCount()
{
	for (uint16_t monsterID = 0; monsterID < _monster_id::NUM_MTYPES; monsterID++) {
		std::string monsterName = GetMonsterName(monsterID);
		if (statisticsFile.find("killed." + monsterName) != statisticsFile.end())
			MonsterKillCounts[monsterID] = std::stoi(statisticsFile.at("killed." + monsterName));
	}
}

void SaveMonsterKillCount()
{
	for (uint16_t monsterID = 0; monsterID < _monster_id::NUM_MTYPES; monsterID++) {
		std::string monsterName = GetMonsterName(monsterID);
		statisticsFile["killed." + monsterName] = std::to_string(MonsterKillCounts[monsterID]);
	}
}

std::string GetMonsterName(uint16_t monsterID)
{
	std::string monsterName = MonstersData[monsterID].mName;
	monsterName.erase(std::remove_if(monsterName.begin(), monsterName.end(), [](unsigned char x) { return std::isspace(x); }), monsterName.end());
	AddMonsterWeapon(monsterName, monsterID);
	return monsterName;
}

void AddMonsterWeapon(std::string &monsterName, uint16_t monsterID)
{
	switch (monsterID) {
	case MT_WSKELBW:
	case MT_RSKELBW:
	case MT_XSKELBW:
	case MT_NGOATBW:
	case MT_BGOATBW:
	case MT_RGOATBW:
	case MT_GGOATBW:
		monsterName += "Archer";
		break;
	case MT_RFALLSP:
	case MT_DFALLSP:
	case MT_YFALLSP:
	case MT_BFALLSP:
		monsterName += "WithSpear";
		break;
	case MT_RFALLSD:
	case MT_DFALLSD:
	case MT_YFALLSD:
	case MT_BFALLSD:
		monsterName += "WithSword";
		break;
	default: // do nothing;
		break;
	}
}

} // namespace devilution
