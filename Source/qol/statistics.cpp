#include "qol/statistics.h"

namespace devilution {

Statistics MyPlayerStatistics;
std::map<std::string, std::string> StatisticsFile;

void LoadMonsterKillCount();
void SaveMonsterKillCount();
std::string GetMonsterName(uint16_t monsterID);
void AddMonsterWeapon(std::string &monsterName, uint16_t monsterID);

void InitializePlayerStatistics()
{
	MyPlayerStatistics = {};
	std::fill_n(MonsterKillCounts, MAXMONSTERS, 0);
	StatisticsFile.clear();
}

void LoadStatisticsFromMap()
{
	if (StatisticsFile.find("deathCount") != StatisticsFile.end())
		MyPlayerStatistics.deathCount = std::stoul(StatisticsFile.at("deathCount"));

	if (StatisticsFile.find("ingameTime") != StatisticsFile.end())
		MyPlayerStatistics.ingameTime = std::stoul(StatisticsFile.at("ingameTime"));
	LoadMonsterKillCount();
}

void SaveStatisticsToMap()
{
	StatisticsFile["deathCount"] = std::to_string(MyPlayerStatistics.deathCount);
	StatisticsFile["ingameTime"] = std::to_string(MyPlayerStatistics.ingameTime);
	SaveMonsterKillCount();
}

void CalculateInGameTime()
{
	uint32_t ticksNow = SDL_GetTicks();
	MyPlayerStatistics.ingameTime += (ticksNow - MyPlayerStatistics.ticksSubtrahend);
	MyPlayerStatistics.ticksSubtrahend = ticksNow;
}

void LoadMonsterKillCount()
{
	for (uint16_t monsterID = 0; monsterID < _monster_id::NUM_MTYPES; monsterID++) {
		std::string monsterName = GetMonsterName(monsterID);
		if (StatisticsFile.find("killed." + monsterName) != StatisticsFile.end())
			MonsterKillCounts[monsterID] = std::stoi(StatisticsFile.at("killed." + monsterName));
	}
}

void SaveMonsterKillCount()
{
	for (uint16_t monsterID = 0; monsterID < _monster_id::NUM_MTYPES; monsterID++) {
		std::string monsterName = GetMonsterName(monsterID);
		StatisticsFile["killed." + monsterName] = std::to_string(MonsterKillCounts[monsterID]);
	}
}

std::string GetMonsterName(uint16_t monsterID)
{
	std::string monsterName = MonstersData[monsterID].mName;
	std::string remChars = "- ";
	monsterName.erase(std::remove_if(monsterName.begin(), monsterName.end(), [remChars](const std::string::value_type &x) { return remChars.find(x) != std::string::npos; }), monsterName.end());
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
