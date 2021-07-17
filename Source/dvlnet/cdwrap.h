#pragma once

#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "dvlnet/abstract_net.h"

namespace devilution {
namespace net {

template <class T>
class cdwrap : public abstract_net {
private:
	std::unique_ptr<abstract_net> dvlnet_wrap;
	std::map<event_type, SEVTHANDLER> registered_handlers;
	buffer_t game_init_info;

	void reset();

public:
	virtual int create(std::string addrstr, std::string passwd);
	virtual int join(std::string addrstr, std::string passwd);
	virtual bool SNetReceiveMessage(int *sender, void **data, uint32_t *size);
	virtual bool SNetSendMessage(int dest, void *data, unsigned int size);
	virtual bool SNetReceiveTurns(char **data, size_t *size, uint32_t *status);
	virtual bool SNetSendTurn(char *data, unsigned int size);
	virtual void SNetGetProviderCaps(struct _SNETCAPS *caps);
	virtual bool SNetRegisterEventHandler(event_type evtype,
	    SEVTHANDLER func);
	virtual bool SNetUnregisterEventHandler(event_type evtype);
	virtual bool SNetLeaveGame(int type);
	virtual bool SNetDropPlayer(int playerid, uint32_t flags);
	virtual bool SNetGetOwnerTurnsWaiting(uint32_t *turns);
	virtual bool SNetGetTurnsInTransit(uint32_t *turns);
	virtual void setup_gameinfo(buffer_t info);
	virtual std::string make_default_gamename();

	cdwrap();
	virtual ~cdwrap() = default;
};

template <class T>
cdwrap<T>::cdwrap()
{
	reset();
}

template <class T>
void cdwrap<T>::reset()
{
	dvlnet_wrap.reset(new T);
	dvlnet_wrap->setup_gameinfo(game_init_info);

	for (const auto &pair : registered_handlers)
		dvlnet_wrap->SNetRegisterEventHandler(pair.first, pair.second);
}

template <class T>
int cdwrap<T>::create(std::string addrstr, std::string passwd)
{
	reset();
	return dvlnet_wrap->create(addrstr, passwd);
}

template <class T>
int cdwrap<T>::join(std::string addrstr, std::string passwd)
{
	game_init_info = buffer_t();
	reset();
	return dvlnet_wrap->join(addrstr, passwd);
}

template <class T>
void cdwrap<T>::setup_gameinfo(buffer_t info)
{
	game_init_info = std::move(info);
	if (dvlnet_wrap)
		dvlnet_wrap->setup_gameinfo(game_init_info);
}

template <class T>
bool cdwrap<T>::SNetReceiveMessage(int *sender, void **data, uint32_t *size)
{
	return dvlnet_wrap->SNetReceiveMessage(sender, data, size);
}

template <class T>
bool cdwrap<T>::SNetSendMessage(int playerID, void *data, unsigned int size)
{
	return dvlnet_wrap->SNetSendMessage(playerID, data, size);
}

template <class T>
bool cdwrap<T>::SNetReceiveTurns(char **data, size_t *size, uint32_t *status)
{
	return dvlnet_wrap->SNetReceiveTurns(data, size, status);
}

template <class T>
bool cdwrap<T>::SNetSendTurn(char *data, unsigned int size)
{
	return dvlnet_wrap->SNetSendTurn(data, size);
}

template <class T>
void cdwrap<T>::SNetGetProviderCaps(struct _SNETCAPS *caps)
{
	dvlnet_wrap->SNetGetProviderCaps(caps);
}

template <class T>
bool cdwrap<T>::SNetUnregisterEventHandler(event_type evtype)
{
	registered_handlers.erase(evtype);
	if (dvlnet_wrap)
		return dvlnet_wrap->SNetUnregisterEventHandler(evtype);
	else
		return true;
}

template <class T>
bool cdwrap<T>::SNetRegisterEventHandler(event_type evtype, SEVTHANDLER func)
{
	registered_handlers[evtype] = func;
	if (dvlnet_wrap)
		return dvlnet_wrap->SNetRegisterEventHandler(evtype, func);
	else
		return true;
}

template <class T>
bool cdwrap<T>::SNetLeaveGame(int type)
{
	return dvlnet_wrap->SNetLeaveGame(type);
}

template <class T>
bool cdwrap<T>::SNetDropPlayer(int playerid, uint32_t flags)
{
	return dvlnet_wrap->SNetDropPlayer(playerid, flags);
}

template <class T>
bool cdwrap<T>::SNetGetOwnerTurnsWaiting(uint32_t *turns)
{
	return dvlnet_wrap->SNetGetOwnerTurnsWaiting(turns);
}

template <class T>
bool cdwrap<T>::SNetGetTurnsInTransit(uint32_t *turns)
{
	return dvlnet_wrap->SNetGetTurnsInTransit(turns);
}

template <class T>
std::string cdwrap<T>::make_default_gamename()
{
	return dvlnet_wrap->make_default_gamename();
}

} // namespace net
} // namespace devilution
