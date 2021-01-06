#include "discord.h"

#include <Discord/cpp/discord.h>
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>
#include <string>
#include <tuple>

#include "../Source/all.h"
#include "../3rdParty/Storm/Source/storm.h"
#include "../Source/player.h"

#define DISCORD_DIABLO_APPID 496571953147150354
#define DISCORD_DEVIX_APPID 795760213524742205

namespace dvl {

DiscordManager::DiscordManager()
{
	discord::Core::Create(DISCORD_DEVIX_APPID, DiscordCreateFlags_NoRequireDiscord, &core);
	//core->ActivityManager().RegisterCommand(...); // TODO: Add path to .exe here to allow launch after first run
}

const char *difficulties[] = { "Normal", "Nightmare", "Hell" };
const char *DifficultyToStr(int difficulty)
{
	if (difficulty < 0 || difficulty > NUM_DIFFICULTIES)
		return "Unknown";
	return difficulties[difficulty];
}

const char *playerClasses[] = { "Warrior", "Rogue", "Sorcerer", "Monk", "Bard", "Barbarian" };
const char *PlayerClassToStr(int plyrClass)
{
	if (plyrClass < 0 || plyrClass >= NUM_CLASSES)
		return "Unknown";
	return playerClasses[plyrClass];
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

std::string DiscordManager::getCharacterString()
{
	return std::string("Lv ") + std::to_string(playerLevel) + " " + PlayerClassToStr(playerClass);
}

std::string DiscordManager::getDetailString()
{
	return getCharacterString() + " - " + getLocationString();
}

std::string DiscordManager::getStateString()
{
	return std::string(DifficultyToStr(difficulty)) + " difficulty";
}

std::string GenerateId(int length = 32)
{
	static constexpr char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	static std::random_device device;
	static std::mt19937 rng(device());
	static std::uniform_int_distribution<> dist(0, sizeof(chars)-1);

	std::string result(length, '\0');
	std::generate_n(result.begin(), length, []() { return chars[dist(rng)]; });
	return result;
}

std::string GetGamePassword()
{
	char gamePassword[128];
	unsigned int written;
	SNetGetGameInfo(GAMEINFO_PASSWORD, gamePassword, sizeof(gamePassword), &written);
	gamePassword[sizeof(gamePassword) - 1] = '\0';
	return std::string(gamePassword);
}

void DiscordManager::updateGame(int newNumPlayers, int newPlayerLevel, int newDungeonType, int newDungeonLevel)
{
	if (core == nullptr)
		return;

	auto newData = std::make_tuple(newNumPlayers, newPlayerLevel, newDungeonType, newDungeonLevel, plr[myplr]._pgfxnum);
	auto trackedData = std::tie(numPlayers, playerLevel, dungeonType, dungeonLevel, plyrGfx);
	if (newData != trackedData)
	{
		trackedData = newData;

		// Update status strings
		std::string details = getDetailString();
		std::string state = getStateString();

		discord::Activity activity = {};
		activity.SetState(state.c_str());
		activity.SetDetails(details.c_str());
		activity.SetInstance(true);

		// Set party info (whether others can be invited to the game)
		if (maxPlayers > 1) {
			discord::PartySize &party = activity.GetParty().GetSize();
			party.SetCurrentSize(numPlayers);
			party.SetMaxSize(maxPlayers);

			activity.GetParty().SetId(gameId.c_str());
			activity.GetSecrets().SetJoin(gamePassword.c_str());
		}

		activity.GetTimestamps().SetStart(startTime);

		// Set image assets
		const char charImg[] = {
			"wrsmrw"[playerClass],
			"lmh"[plr[myplr]._pgfxnum >> 4],
			"nusdbamht"[plr[myplr]._pgfxnum & 0xF],
			'a', 's', '\0'
		};

		std::string assetTooltip = characterName + " - " + getCharacterString();
		activity.GetAssets().SetLargeImage(charImg);
		activity.GetAssets().SetLargeText(assetTooltip.c_str());

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

	gameId = GenerateId();
	gamePassword = GetGamePassword();

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
