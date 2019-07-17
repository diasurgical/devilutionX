#include <memory>

#include "devilution.h"
#include "stubs.h"
#include "dvlnet/abstract_net.h"

namespace dvl {

static std::unique_ptr<net::abstract_net> dvlnet_inst;

BOOL SNetReceiveMessage(int *senderplayerid, char **data, int *databytes)
{
	if (!dvlnet_inst->SNetReceiveMessage(senderplayerid, data, databytes)) {
		SErrSetLastError(STORM_ERROR_NO_MESSAGES_WAITING);
		return false;
	}
	return true;
}

BOOL SNetSendMessage(int playerID, void *data, unsigned int databytes)
{
	return true;//dvlnet_inst->SNetSendMessage(playerID, data, databytes); //arczi
}

BOOL SNetReceiveTurns(int a1, int arraysize, char **arraydata, unsigned int *arraydatabytes,
    DWORD *arrayplayerstatus)
{
	if (a1 != 0)
		UNIMPLEMENTED();
	if (arraysize != MAX_PLRS)
		UNIMPLEMENTED();
	//if (!dvlnet_inst->SNetReceiveTurns(arraydata, arraydatabytes, arrayplayerstatus)) {
	//	SErrSetLastError(STORM_ERROR_NO_MESSAGES_WAITING);
		//return false;
	//} //arczi
	return true;
}

BOOL SNetSendTurn(char *data, unsigned int databytes)
{
	return true;//dvlnet_inst->SNetSendTurn(data, databytes); //arczi
}

int SNetGetProviderCaps(struct _SNETCAPS *caps)
{
	//return dvlnet_inst->SNetGetProviderCaps(caps); //arczi
	
	caps->size = 0;                  // engine writes only ?!?
	caps->flags = 0;                 // unused
	caps->maxmessagesize = 512;      // capped to 512; underflow if < 24
	caps->maxqueuesize = 0;          // unused
	caps->maxplayers = MAX_PLRS;     // capped to 4
	caps->bytessec = 1000000;        // ?
	caps->latencyms = 0;             // unused
	caps->defaultturnssec = 10;      // ?
	caps->defaultturnsintransit = 1; // maximum acceptable number
	                                 // of turns in queue?
	return 1;
}

BOOL SNetUnregisterEventHandler(int evtype, SEVTHANDLER func)
{
	return true;//dvlnet_inst->SNetUnregisterEventHandler(*(event_type *)&evtype, func); //arczi
}

BOOL SNetRegisterEventHandler(int evtype, SEVTHANDLER func)
{
	return dvlnet_inst->SNetRegisterEventHandler(*(event_type *)&evtype, func);
}

BOOL SNetDestroy()
{
	DUMMY();
	return true;
}

BOOL SNetDropPlayer(int playerid, DWORD flags)
{
	return dvlnet_inst->SNetDropPlayer(playerid, flags);
}

BOOL SNetGetGameInfo(int type, void *dst, unsigned int length, unsigned int *byteswritten)
{
	DUMMY();
	return true;
}

BOOL SNetLeaveGame(int type)
{
	return true;//dvlnet_inst->SNetLeaveGame(type); //arczi
}

BOOL SNetSendServerChatCommand(const char *command)
{
	DUMMY();
	return true;
}

/**
 * @brief Called by engine for single, called by ui for multi
 * @param provider BNET, IPXN, MODM, SCBL or UDPN
 * @param fileinfo Ignore
 */
int SNetInitializeProvider(unsigned long provider, struct _SNETPROGRAMDATA *client_info,
    struct _SNETPLAYERDATA *user_info, struct _SNETUIDATA *ui_info,
    struct _SNETVERSIONDATA *fileinfo)
{
	//dvlnet_inst = net::abstract_net::make_net(provider);
	return ui_info->selectnamecallback(client_info, user_info, ui_info, fileinfo, provider, NULL, 0, NULL, 0, NULL);
}

/**
 * @brief Called by engine for single, called by ui for multi
 */
BOOL SNetCreateGame(const char *pszGameName, const char *pszGamePassword, const char *pszGameStatString,
    DWORD dwGameType, char *GameTemplateData, int GameTemplateSize, int playerCount,
    char *creatorName, char *a11, int *playerID)
{
	/*if (GameTemplateSize != 8)
		ABORT();
	net::buffer_t game_init_info(GameTemplateData, GameTemplateData + GameTemplateSize);
	dvlnet_inst->setup_gameinfo(std::move(game_init_info));

	char addrstr[129] = "0.0.0.0";
	SRegLoadString("dvlnet", "bindaddr", 0, addrstr, 128);
	*playerID = dvlnet_inst->create(addrstr, pszGamePassword);
	return *playerID != -1;
	*/
	return true;
}

BOOL SNetJoinGame(int id, char *pszGameName, char *pszGamePassword, char *playerName, char *userStats, int *playerID)
{
	*playerID = dvlnet_inst->join(pszGameName, pszGamePassword);
	return *playerID != -1;
}

/**
 * @brief Is this the mirror image of SNetGetTurnsInTransit?
 */
BOOL SNetGetOwnerTurnsWaiting(DWORD *turns)
{
	return dvlnet_inst->SNetGetOwnerTurnsWaiting(turns);
}

BOOL SNetGetTurnsInTransit(int *turns)
{
	*turns = 0; //arczi
	return true;//dvlnet_inst->SNetGetTurnsInTransit(turns);
}

/**
 * @brief engine calls this only once with argument 1
 */
BOOLEAN SNetSetBasePlayer(int)
{
	return true;
}

/**
 * @brief since we never signal STORM_ERROR_REQUIRES_UPGRADE the engine will not call this function
 */
BOOL SNetPerformUpgrade(DWORD *upgradestatus)
{
	UNIMPLEMENTED();
}

/**
 * @brief not called from engine
 */
BOOL SNetSetGameMode(DWORD modeFlags, bool makePublic)
{
	UNIMPLEMENTED();
	return true;
}

}
