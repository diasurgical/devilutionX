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

	void updateGame(int newNumPlayers, int newPlayerLevel, int newDungeonType, int newDungeonLevel);
	void startGame(int difficulty, int maxPlayers, int plyrClass, const char *charName);

	void setInMenuActivity();

private:
	std::string getLocationString();
	std::string getCharacterString();
	std::string getDetailString();
	std::string getStateString();

private:
	discord::Core *core = nullptr;

	int numPlayers = 0;
	int maxPlayers = 0;
	int playerClass = 0;
	int playerLevel = 0;
	int difficulty = 0;
	int dungeonType = 0;
	int dungeonLevel = 0;
	int plyrGfx = 0;

	std::string characterName;

	std::int64_t startTime = 0;
	std::string gameId;
	std::string gamePassword;
};

}

#endif
