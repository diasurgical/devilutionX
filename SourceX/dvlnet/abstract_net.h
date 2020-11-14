#pragma once

#include <vector>
#include <memory>
#include <string>
#include <exception>

#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

namespace dvl {
namespace net {

typedef std::vector<unsigned char> buffer_t;
typedef unsigned long provider_t;
class dvlnet_exception : public std::exception {
public:
	const char *what() const throw() override
	{
		return "Network error";
	}
};

class abstract_net {
public:
	virtual int create(std::string addrstr, std::string passwd) = 0;
	virtual int join(std::string addrstr, std::string passwd) = 0;
	virtual bool SNetReceiveMessage(int *sender, char **data,
	    int *size)
	    = 0;
	virtual bool SNetSendMessage(int dest, void *data,
	    unsigned int size)
	    = 0;
	virtual bool SNetReceiveTurns(char **data, unsigned int *size,
	    DWORD *status)
	    = 0;
	virtual bool SNetSendTurn(char *data, unsigned int size) = 0;
	virtual int SNetGetProviderCaps(struct _SNETCAPS *caps) = 0;
	virtual bool SNetRegisterEventHandler(event_type evtype,
	    SEVTHANDLER func)
	    = 0;
	virtual bool SNetUnregisterEventHandler(event_type evtype,
	    SEVTHANDLER func)
	    = 0;
	virtual bool SNetLeaveGame(int type) = 0;
	virtual bool SNetDropPlayer(int playerid, DWORD flags) = 0;
	virtual bool SNetGetOwnerTurnsWaiting(DWORD *turns) = 0;
	virtual bool SNetGetTurnsInTransit(DWORD *turns) = 0;
	virtual void setup_gameinfo(buffer_t info) = 0;
	virtual ~abstract_net() = default;

	static std::unique_ptr<abstract_net> make_net(provider_t provider);
};

} // namespace net
} // namespace dvl
