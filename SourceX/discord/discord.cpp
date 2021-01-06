#include "discord.h"

#include <Discord/cpp/discord.h>
#include <chrono>
#include <string>
#include <sstream>

#include "../types.h"
#include "../Source/player.h"

namespace dvl {

DiscordManager::DiscordManager()
{
	discord::Core::Create(DISCORD_DEVIX_APPID, DiscordCreateFlags_NoRequireDiscord, &core);
}

const char *difficulties[] = { "Normal", "Nightmare", "Hell" };
const char *DifficultyToStr(int difficulty)
{
	if (difficulty < 0 || difficulty > NUM_DIFFICULTIES)
		return "Unknown";
	return difficulties[difficulty];
}

const char *PlayerClassToStr(int plyrClass)
{
	if (plyrClass < 0 || plyrClass >= NUM_CLASSES)
		return "Unknown";
	return ClassStrTbl[plyrClass];
}

const char *dungeonTypes[] = { "Town", "Cathedral", "Catacombs", "Caves", "Hell", "Nest", "Crypt" };
const char *DungeonTypeToStr(int dungeonType)
{
	if (dungeonType == DTYPE_NONE)
		return "None";
	if (dungeonType < 0 || dungeonType > DTYPE_CRYPT)
		return "Unknown";
	return dungeonTypes[dungeonType];
}

std::string DiscordManager::getPlayerInfoString()
{
	std::ostringstream result;
	result << characterName << ": Lv " << playerLevel << " " << PlayerClassToStr(playerClass);
	return result.str();
}

std::string DiscordManager::getLocationString()
{
	std::ostringstream result;
	result << DungeonTypeToStr(dungeonType);

	if (dungeonLevel > 0) {
		int level = dungeonLevel;
		if (dungeonType == DTYPE_NEST)
			level -= 16;
		else if (dungeonType == DTYPE_CRYPT)
			level -= 20;

		result << " " << level;
	}

	return result.str();
}

std::string DiscordManager::getDetailString()
{
	std::ostringstream result;
	result << getPlayerInfoString();
	return result.str();
}

std::string DiscordManager::getStateString()
{
	std::ostringstream result;
	result << getLocationString() << " - " << DifficultyToStr(difficulty) << " difficulty";
	return result.str();
}

void DiscordManager::updateGame(int newNumPlayers, int newPlayerLevel, int newDungeonType, int newDungeonLevel)
{
	if (core == nullptr)
		return;

	if (newNumPlayers != numPlayers ||
		newPlayerLevel != playerLevel ||
		newDungeonType != dungeonType ||
		newDungeonLevel != dungeonLevel)
	{
		numPlayers = newNumPlayers;
		playerLevel = newPlayerLevel;
		dungeonType = newDungeonType;
		dungeonLevel = newDungeonLevel;

		std::string details = getDetailString();
		std::string state = getStateString();

		discord::Activity activity = {};
		activity.SetState(state.c_str());
		activity.SetDetails(details.c_str());
		activity.SetInstance(true);

		if (maxPlayers > 1) {
			discord::PartySize &party = activity.GetParty().GetSize();
			party.SetCurrentSize(numPlayers);
			party.SetMaxSize(maxPlayers);
		}

		activity.GetTimestamps().SetStart(startTime);

		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
	}
	core->RunCallbacks();
}

void DiscordManager::startGame(int difficulty, int maxPlayers, int plyrClass, const char *charName)
{
	this->difficulty = difficulty;
	this->maxPlayers = maxPlayers;
	this->playerClass = plyrClass;
	this->characterName = charName;

	startTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}


void DiscordManager::setInMenuActivity()
{
	if (core == nullptr)
		return;

	discord::Activity activity = {};
	activity.SetState("In Menu");

	core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
	core->RunCallbacks();
}

}
