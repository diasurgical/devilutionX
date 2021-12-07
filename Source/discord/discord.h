#pragma once

#ifdef DISCORD

#include <cstdint>
#include <string>

#include "../gendung.h"
#include "../player.h"

namespace discord {
class Core;
} // namespace discord

namespace devilution {

class DiscordManager {
public:
	DiscordManager();
	DiscordManager(const DiscordManager &) = delete;

	DiscordManager &operator=(const DiscordManager &) = delete;

	void UpdateGame();
	void StartGame(int difficulty);

	void UpdateMenu();

	static DiscordManager &Instance();

private:
	std::string GetPlayerAssetString() const;
	std::string GetLocationString() const;
	std::string GetCharacterString() const;
	std::string GetDetailString() const;
	std::string GetStateString() const;
	std::string GetTooltipString() const;

	char HeroClassChar() const;
	char ArmourClassChar() const;
	char WpnConfigChar() const;

	void ResetStartTime();

	discord::Core *core_ = nullptr;
	bool good_ = true;
	bool want_menu_update_ = true;

	HeroClass player_class_ = HeroClass::Warrior;
	dungeon_type dungeon_area_ = dungeon_type::DTYPE_NONE;
	Sint8 player_level_ = 0;
	Uint8 dungeon_level_ = 0;
	int difficulty_ = 0;
	int player_gfx_ = 0;

	std::string character_name_;

	Sint64 start_time_ = 0;
};

} // namespace devilution

#endif
