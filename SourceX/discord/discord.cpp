#include "discord.h"

#include <Discord/cpp/discord.h>
#include <algorithm>
#include <array>
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

namespace {
constexpr int NUM_HELLFIRE_CLASSES = 6;

constexpr std::array<const char *, NUM_DIFFICULTIES> DIFFICULTIES = { "Normal", "Nightmare", "Hell" };
constexpr std::array<const char *, NUM_HELLFIRE_CLASSES> CLASSES = { "Warrior", "Rogue", "Sorcerer", "Monk", "Bard", "Barbarian" };
constexpr std::array<const char *, DTYPE_CRYPT + 1> DUNGEONS = { "Town", "Cathedral", "Catacombs", "Caves", "Hell", "Nest", "Crypt" };
} // namespace

DiscordManager::DiscordManager()
{
	discord::Core::Create(DISCORD_DEVIX_APPID, DiscordCreateFlags_NoRequireDiscord, &core);
	//core->ActivityManager().RegisterCommand(...); // TODO: Add path to .exe here to allow launch after first run
}

std::string DiscordManager::GetLocationString()
{
	std::ostringstream result;
	if (dungeon_type == DTYPE_NONE)
		result << "None";
	else
		result << DUNGEONS.at(dungeon_type);

	if (dungeon_level > 0) {
		int level = dungeon_level;
		if (dungeon_type == DTYPE_NEST)
			level -= 16;
		else if (dungeon_type == DTYPE_CRYPT)
			level -= 20;

		result << " " << level;
	}

	return result.str();
}

std::string DiscordManager::GetCharacterString()
{
	return std::string("Lv ") + std::to_string(player_level) + " " + CLASSES.at(player_class);
}

std::string DiscordManager::GetDetailString()
{
	return GetCharacterString() + " - " + GetLocationString();
}

std::string DiscordManager::GetStateString()
{
	return std::string(DIFFICULTIES.at(difficulty)) + " difficulty";
}

std::string DiscordManager::GenerateId(int length)
{
	static constexpr char chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	static const std::mt19937 rng(std::random_device {}());
	static const std::uniform_int_distribution<> dist(0, sizeof(chars)-1);
	static const auto generator = []() { return chars[dist(rng)]; };

	std::string result(length, '\0');
	std::generate_n(result.begin(), length, generator);
	return result;
}

std::string DiscordManager::GetPlayerAssetString()
{
	return std::string {
		"wrsmrw"[player_class],
		"lmh"[player_gfx >> 4],			// armour class
		"nusdbamht"[player_gfx & 0xF],	// weapon configuration
		'a', 's', '\0'
	};
}

void DiscordManager::UpdateGame(int new_num_players, int new_player_level, int new_dungeon_type, int new_dungeon_level)
{
	if (core == nullptr)
		return;

	auto new_data = std::make_tuple(new_num_players, new_player_level, new_dungeon_type, new_dungeon_level, plr[myplr]._pgfxnum);
	auto tracked_data = std::tie(num_players, player_level, dungeon_type, dungeon_level, player_gfx);
	if (new_data != tracked_data)
	{
		tracked_data = new_data;

		// Update status strings
		discord::Activity activity = {};
		activity.SetState(GetStateString().c_str());
		activity.SetDetails(GetDetailString().c_str());
		activity.SetInstance(true);

		// Set party info (whether others can be invited to the game)
		if (max_players > 1) {
			discord::PartySize &party = activity.GetParty().GetSize();
			party.SetCurrentSize(num_players);
			party.SetMaxSize(max_players);

			activity.GetParty().SetId(game_id.c_str());
			activity.GetSecrets().SetJoin(game_password);
		}

		activity.GetTimestamps().SetStart(start_time);

		// Set image assets
		std::string asset_tooltip = character_name + " - " + GetCharacterString();
		activity.GetAssets().SetLargeImage(GetPlayerAssetString().c_str());
		activity.GetAssets().SetLargeText(asset_tooltip.c_str());

		core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
	}
	core->RunCallbacks();
}

void DiscordManager::StartGame(int difficulty, int max_players, int plyr_class, const char *char_name)
{
	this->difficulty = difficulty;
	this->max_players = max_players;
	this->player_class = plyr_class;
	this->character_name = char_name;

	game_id = GenerateId();

	// TODO: Generate for discord lobby API instead
	unsigned int written;
	SNetGetGameInfo(GAMEINFO_PASSWORD, game_password, sizeof(game_password), &written);
	game_password[sizeof(game_password) - 1] = '\0';

	start_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void DiscordManager::UpdateMenu()
{
	if (core == nullptr)
		return;

	discord::Activity activity = {};
	activity.SetState("In Menu");

	core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {});
	core->RunCallbacks();
}

}
