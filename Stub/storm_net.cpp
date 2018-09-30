#include <deque>
#include <string>

#include "../types.h"
#include "stubs.h"

struct StubMessage {
	int playerid;
	std::string data;

	StubMessage(const &StubMessage) = delete;
};

std::deque<StubMessage> messages;
StubMessage current_message{0, ""};

BOOL STORMAPI SNetReceiveMessage(int *senderplayerid, char **data, int *databytes)
{
	DUMMY_ONCE();
	if (messages.size() == 0) {
		SErrSetLastError(STORM_ERROR_NO_MESSAGES_WAITING);
		return FALSE;
	}

	current_message = std::move(messages.front());
	messages.pop_front();

	*senderplayerid = current_message.playerid;
	*data = const_cast<char *>(current_message.data.data());
	*databytes = current_message.data.size();

	return TRUE;
}

BOOL STORMAPI SNetSendMessage(int playerID, void *data, size_t databytes)
{
	DUMMY();
	messages.push_back(StubMessage{playerID, std::string((char *)data, databytes)});
	return TRUE;
}
