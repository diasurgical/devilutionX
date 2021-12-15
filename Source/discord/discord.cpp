#include "discord.h"

#include <discordsrc-src/cpp/discord.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <string>
#include <tuple>

#include <fmt/format.h>

#include "config.h"
#include "gendung.h"
#include "init.h"
#include "multi.h"
#include "panels/charpanel.hpp"
#include "player.h"
#include "setmaps.h"
#include "utils/language.h"

namespace devilution {
namespace discord_manager {

// App ID used for DevilutionX's Diablo (classic Diablo's is 496571953147150354)
constexpr discord::ClientId DiscordDevilutionxAppId = 795760213524742205;

constexpr auto IgnoreResult = [](discord::Result result) {};

discord::Core *discord_core = []() -> discord::Core * {
	discord::Core *core;
	discord::Result result = discord::Core::Create(DiscordDevilutionxAppId, DiscordCreateFlags_NoRequireDiscord, &core);
	if (result != discord::Result::Ok) {
		core = nullptr;
	}
	return core;
}();

struct PlayerData {
	dungeon_type dungeonArea;
	_setlevels questMap;
	Uint8 dungeonLevel;
	Sint8 playerLevel;
	int playerGfx;

	// Why??? This is POD
	bool operator!=(const PlayerData &other) const
	{
		return std::tie(dungeonArea, dungeonLevel, playerLevel, playerGfx) != std::tie(other.dungeonArea, other.dungeonLevel, other.playerLevel, other.playerGfx);
	}
};

bool want_menu_update = true;
PlayerData tracked_data;
Sint64 start_time = 0;

std::string GetLocationString()
{
	// Quest Level Name
	if (setlevel) {
		return _(QuestLevelNames[setlvlnum]);
	}

	// Dungeon Name
	constexpr std::array<const char *, DTYPE_LAST + 1> DungeonStrs = { N_("Town"), N_("Cathedral"), N_("Catacombs"), N_("Caves"), N_("Hell"), N_("Nest"), N_("Crypt") };
	const char *dungeonStr = _(/* TRANSLATORS: type of dungeon (i.e. Cathedral, Caves)*/ "None");
	if (tracked_data.dungeonArea != DTYPE_NONE) {
		dungeonStr = _(DungeonStrs[tracked_data.dungeonArea]);
	}

	// Dungeon Level
	if (tracked_data.dungeonLevel > 0) {
		int level = tracked_data.dungeonLevel;
		if (tracked_data.dungeonArea == DTYPE_NEST)
			level -= 16;
		else if (tracked_data.dungeonArea == DTYPE_CRYPT)
			level -= 20;

		return fmt::format(_(/* TRANSLATORS: dungeon type and floor number i.e. "Cathedral 3"*/ "{} {}"), dungeonStr, level);
	}
	return dungeonStr;
}

std::string GetCharacterString()
{
	const char *charClassStr = ClassStrTbl[static_cast<int>(Players[MyPlayerId]._pClass)];
	return fmt::format(_(/* TRANSLATORS: Discord character, i.e. "Lv 6 Warrior" */ "Lv {} {}"), tracked_data.playerLevel, charClassStr);
}

std::string GetDetailString()
{
	return fmt::format("{} - {}", GetCharacterString(), GetLocationString());
}

std::string GetStateString()
{
	constexpr std::array<const char *, 3> DifficultyStrs = { N_("Normal"), N_("Nightmare"), N_("Hell") };
	const char *difficultyStr = _(DifficultyStrs[sgGameInitInfo.nDifficulty]);
	return fmt::format(_(/* TRANSLATORS: Discord state i.e. "Nightmare difficulty" */ "{} difficulty"), difficultyStr);
}

std::string GetTooltipString()
{
	return fmt::format("{} - {}", Players[MyPlayerId]._pName, GetCharacterString());
}

std::string GetPlayerAssetString()
{
	char heroChar = CharChar[static_cast<int>(Players[MyPlayerId]._pClass)];
	char armourChar = ArmourChar[tracked_data.playerGfx >> 4];
	char wpnChar = WepChar[tracked_data.playerGfx & 0xF];

	std::string result = fmt::format("{}{}{}as", heroChar, armourChar, wpnChar);
	std::transform(std::begin(result), std::end(result), std::begin(result), [](char c) { return static_cast<char>(std::tolower(c)); });
	return result;
}

void ResetStartTime()
{
	start_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

const char *GetIconAsset()
{
	return gbIsHellfire ? "hellfire" : "icon";
}

void UpdateGame()
{
	if (discord_core == nullptr)
		return;

	auto newData = PlayerData {
		leveltype, setlvlnum, currlevel, Players[MyPlayerId]._pLevel, Players[MyPlayerId]._pgfxnum
	};
	if (newData != tracked_data) {
		tracked_data = newData;

		// Update status strings
		discord::Activity activity = {};
		activity.SetName(PROJECT_NAME);
		activity.SetState(GetStateString().c_str());
		activity.SetDetails(GetDetailString().c_str());
		activity.SetInstance(true);

		activity.GetTimestamps().SetStart(start_time);

		// Set image assets
		activity.GetAssets().SetLargeImage(GetPlayerAssetString().c_str());
		activity.GetAssets().SetLargeText(GetTooltipString().c_str());
		activity.GetAssets().SetSmallImage(GetIconAsset());
		activity.GetAssets().SetSmallText(gszProductName);

		discord_core->ActivityManager().UpdateActivity(activity, IgnoreResult);
	}
	discord_core->RunCallbacks();
}

void StartGame()
{
	tracked_data = PlayerData { dungeon_type::DTYPE_NONE, _setlevels::SL_NONE, 0, 0, 0 };
	want_menu_update = true;
	ResetStartTime();
}

void UpdateMenu(bool forced)
{
	if (discord_core == nullptr)
		return;

	if (want_menu_update || forced) {
		if (!forced) {
			ResetStartTime();
		}
		want_menu_update = false;

		discord::Activity activity = {};
		activity.SetName(PROJECT_NAME);
		activity.SetState(_(/* TRANSLATORS: Discord activity, not in game */ "In Menu"));

		activity.GetTimestamps().SetStart(start_time);

		activity.GetAssets().SetLargeImage(GetIconAsset());
		activity.GetAssets().SetLargeText(gszProductName);

		discord_core->ActivityManager().UpdateActivity(activity, IgnoreResult);
	}
	discord_core->RunCallbacks();
}

} // namespace discord_manager
} // namespace devilution
