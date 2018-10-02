/** @file
 *
 * A minimal implementation of the Storm network stack necessary for local play.
 */
#include <deque>
#include <string>

#include "../types.h"
#include "stubs.h"

struct StubMessage {
	int playerid;
	std::string data;
};

/** A queue of messages waiting to be processed. */
static std::deque<StubMessage> snet_messages;
/**
 * Last message returned from SNetReceiveMessage().
 * Must always be kept alive because the caller will read the data afterwards.
 */
static StubMessage snet_current_message;

BOOL STORMAPI SNetReceiveMessage(int *senderplayerid, char **data, int *databytes)
{
	DUMMY_ONCE();

	if (snet_messages.empty()) {
		SErrSetLastError(STORM_ERROR_NO_MESSAGES_WAITING);
		return FALSE;
	}

	snet_current_message = std::move(snet_messages.front());
	snet_messages.pop_front();

	*senderplayerid = snet_current_message.playerid;
	*data = const_cast<char *>(snet_current_message.data.data());
	*databytes = snet_current_message.data.size();

	return TRUE;
}

BOOL STORMAPI SNetSendMessage(int playerID, void *data, size_t databytes)
{
	DUMMY();

	snet_messages.push_back(StubMessage{playerID, std::string((char *)data, databytes)});
	return TRUE;
}
