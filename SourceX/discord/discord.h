#pragma once

#ifdef DISCORD

#include <cstdint>
#include <string>

namespace discord {
class Core;
}

namespace dvl {

class DiscordManager {
public:
	DiscordManager();

	void UpdateGame(int new_num_players, int new_player_level, int new_dungeon_type, int new_dungeon_level);
	void StartGame(int difficulty, int max_players, int plyr_class, const char *char_name);

	void UpdateMenu();

private:
	std::string GetPlayerAssetString();
	std::string GetLocationString();
	std::string GetCharacterString();
	std::string GetDetailString();
	std::string GetStateString();

	static std::string GenerateId(int length = 32);

private:
	discord::Core *core = nullptr;

	int num_players = 0;
	int max_players = 0;
	int player_class = 0;
	int player_level = 0;
	int difficulty = 0;
	int dungeon_type = 0;
	int dungeon_level = 0;
	int player_gfx = 0;

	std::string character_name;

	std::int64_t start_time = 0;
	std::string game_id;
	char game_password[128];
};

}

#endif
