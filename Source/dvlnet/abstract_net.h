#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "storm/storm_net.hpp"

namespace devilution {
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
	virtual int create(std::string addrstr) = 0;
	virtual int join(std::string addrstr) = 0;
	virtual bool SNetReceiveMessage(int *sender, void **data, uint32_t *size) = 0;
	virtual bool SNetSendMessage(int dest, void *data, unsigned int size) = 0;
	virtual bool SNetReceiveTurns(char **data, size_t *size, uint32_t *status) = 0;
	virtual bool SNetSendTurn(char *data, unsigned int size) = 0;
	virtual void SNetGetProviderCaps(struct _SNETCAPS *caps) = 0;
	virtual bool SNetRegisterEventHandler(event_type evtype, SEVTHANDLER func) = 0;
	virtual bool SNetUnregisterEventHandler(event_type evtype) = 0;
	virtual bool SNetLeaveGame(int type) = 0;
	virtual bool SNetDropPlayer(int playerid, uint32_t flags) = 0;
	virtual bool SNetGetOwnerTurnsWaiting(uint32_t *turns) = 0;
	virtual bool SNetGetTurnsInTransit(uint32_t *turns) = 0;
	virtual void setup_gameinfo(buffer_t info) = 0;
	virtual ~abstract_net() = default;

	virtual std::string make_default_gamename() = 0;

	virtual void setup_password(std::string passwd)
	{
	}

	virtual void clear_password()
	{
	}

	virtual bool send_info_request()
	{
		return true;
	}

	virtual void clear_gamelist()
	{
	}

	virtual std::vector<std::string> get_gamelist()
	{
		return std::vector<std::string>();
	}

	static std::unique_ptr<abstract_net> MakeNet(provider_t provider);
};

} // namespace net
} // namespace devilution
