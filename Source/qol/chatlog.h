/**
* @file chatlog.h
*
* Adds ChatLog QoL feature
*/
#pragma once
#include <ctime>
#include <string>
#include <vector>

#include "engine.h"
#include "player.h"

namespace devilution {

struct PlayerMessage {
	std::string sender;
	std::string text;
	int level;
	bool myMessage;
	time_t timestamp;
	bool systemMessage;
};

extern std::vector<PlayerMessage> ChatLogMessages;

bool IsChatLogEnabled();
void ToggleChatLog();
void AddMessageToChatLog(int pnum, const char *message, bool systemMessage = false);

} // namespace devilution
