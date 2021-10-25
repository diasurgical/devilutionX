#include "chatlog.h"

#include <string>
#include <vector>

#include "stores.h"

namespace devilution {

std::vector<PlayerMessage> ChatLogMessages;

bool IsChatLogEnabled()
{
	return stextflag == STORE_CHATLOG;
}

void ToggleChatLog()
{
	if (stextflag == STORE_CHATLOG) {
		StoreESC();
	} else if (!ChatLogMessages.empty())
		StartStore(STORE_CHATLOG);
}

void AddMessageToChatLog(int pnum, const char *message, bool systemMessage)
{
	auto &player = Players[pnum];

	PlayerMessage msg;
	msg.sender = player._pName;
	msg.text = message;
	msg.level = player._pLevel;
	msg.myMessage = pnum == MyPlayerId;
	msg.timestamp = time(nullptr);
	msg.systemMessage = systemMessage;
	ChatLogMessages.push_back(msg);
}

} // namespace devilution
