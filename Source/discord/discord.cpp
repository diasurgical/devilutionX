#include "discord.h"

#include <Discord/cpp/discord.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <random>
#include <sstream>
#include <string>
#include <tuple>

#include <fmt/format.h>

#include "../gendung.h"
#include "../panels/charpanel.hpp"
#include "../player.h"
#include "../utils/language.h"

// App ID used when someone launches classic Diablo
constexpr discord::ClientId DiscordDiabloAppId = 496571953147150354;

// App ID used for DevilutionX's Diablo
constexpr discord::ClientId DiscordDevilutionxAppId = 795760213524742205;

namespace devilution {

namespace {
constexpr std::array<const char *, 3> DifficultyStrs = { N_("Normal"), N_("Nightmare"), N_("Hell") };
constexpr std::array<const char *, DTYPE_LAST + 1> DungeonStrs = { N_("Town"), N_("Cathedral"), N_("Catacombs"), N_("Caves"), N_("Hell"), N_("Nest"), N_("Crypt") };

constexpr auto IgnoreResult = [](discord::Result result) {};
} // namespace

DiscordManager::DiscordManager()
{
	discord::Result result = discord::Core::Create(DiscordDevilutionxAppId, DiscordCreateFlags_NoRequireDiscord, &core_);
	good_ = result == discord::Result::Ok;
}

DiscordManager &DiscordManager::Instance()
{
	static DiscordManager instance {};
	return instance;
}

std::string DiscordManager::GetLocationString() const
{
	const char *dungeonStr = _(/* TRANSLATORS: type of dungeon (i.e. Cathedral, Caves)*/ "None");
	if (dungeon_area_ != DTYPE_NONE) {
		dungeonStr = _(DungeonStrs.at(dungeon_area_));
	}

	if (dungeon_level_ > 0) {
		int level = dungeon_level_;
		if (dungeon_area_ == DTYPE_NEST)
			level -= 16;
		else if (dungeon_area_ == DTYPE_CRYPT)
			level -= 20;

		return fmt::format(_(/* TRANSLATORS: dungeon type and floor number i.e. "Cathedral 3"*/ "{} {}"), dungeonStr, level);
	}
	return fmt::format("{}", dungeonStr);
}

std::string DiscordManager::GetCharacterString() const
{
	const char *charClassStr = ClassStrTbl.at(static_cast<int>(player_class_));
	return fmt::format(_(/* TRANSLATORS: Discord character, i.e. "Lv 6 Warrior" */ "Lv {} {}"), player_level_, charClassStr);
}

std::string DiscordManager::GetDetailString() const
{
	return fmt::format("{} - {}", GetCharacterString(), GetLocationString());
}

std::string DiscordManager::GetStateString() const
{
	return fmt::format(_(/* TRANSLATORS: Discord state i.e. "Nightmare difficulty" */ "{} difficulty"), _(DifficultyStrs.at(difficulty_)));
}

std::string DiscordManager::GetTooltipString() const
{
	return fmt::format("{} - {}", character_name_, GetCharacterString());
}

char DiscordManager::HeroClassChar() const
{
	char chr = CharChar.at(static_cast<int>(player_class_));
	return static_cast<char>(std::tolower(chr));
}

char DiscordManager::ArmourClassChar() const
{
	char chr = ArmourChar.at(player_gfx_ >> 4);
	return static_cast<char>(std::tolower(chr));
}

char DiscordManager::WpnConfigChar() const
{
	char chr = WepChar.at(player_gfx_ & 0xF);
	return static_cast<char>(std::tolower(chr));
}

std::string DiscordManager::GetPlayerAssetString() const
{
	return fmt::format("{}{}{}as", HeroClassChar(), ArmourClassChar(), WpnConfigChar());
}

void DiscordManager::UpdateGame()
{
	if (core_ == nullptr || !good_)
		return;

	auto newData = std::make_tuple(Players[MyPlayerId]._pLevel, leveltype, currlevel, Players[MyPlayerId]._pgfxnum);
	auto trackedData = std::tie(player_level_, dungeon_area_, dungeon_level_, player_gfx_);
	if (newData != trackedData) {
		trackedData = newData;

		// Update status strings
		discord::Activity activity = {};
		activity.SetName(gszProductName);
		activity.SetState(GetStateString().c_str());
		activity.SetDetails(GetDetailString().c_str());
		activity.SetInstance(true);

		activity.GetTimestamps().SetStart(start_time_);

		// Set image assets
		activity.GetAssets().SetLargeImage(GetPlayerAssetString().c_str());
		activity.GetAssets().SetLargeText(GetTooltipString().c_str());

		core_->ActivityManager().UpdateActivity(activity, IgnoreResult);
	}
	core_->RunCallbacks();
}

void DiscordManager::ResetStartTime()
{
	start_time_ = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void DiscordManager::StartGame(int difficulty)
{
	difficulty_ = difficulty;
	player_class_ = Players[MyPlayerId]._pClass;
	character_name_ = Players[MyPlayerId]._pName;
	want_menu_update_ = true;
	ResetStartTime();
}

void DiscordManager::UpdateMenu()
{
	if (core_ == nullptr || !good_)
		return;

	if (want_menu_update_) {
		want_menu_update_ = false;
		ResetStartTime();

		discord::Activity activity = {};
		activity.SetName(gszProductName);
		activity.SetState(_(/* TRANSLATORS: Discord activity, not in game */ "In Menu"));

		activity.GetTimestamps().SetStart(start_time_);

		activity.GetAssets().SetLargeImage("icon");
		activity.GetAssets().SetLargeText(gszProductName);

		core_->ActivityManager().UpdateActivity(activity, IgnoreResult);
	}
	core_->RunCallbacks();
}

} // namespace devilution
