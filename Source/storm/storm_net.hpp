#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace devilution {

enum game_info : uint8_t {
	GAMEINFO_NAME,
	GAMEINFO_PASSWORD,
};

enum conn_type : uint8_t {
	SELCONN_ZT,
	SELCONN_TCP,
	SELCONN_LOOPBACK,
};

enum event_type : uint8_t {
	EVENT_TYPE_PLAYER_CREATE_GAME,
	EVENT_TYPE_PLAYER_LEAVE_GAME,
	EVENT_TYPE_PLAYER_MESSAGE,
};

extern const char *ConnectionNames[];

struct _SNETCAPS {
	uint32_t size;
	uint32_t flags;
	uint32_t maxmessagesize;
	uint32_t maxqueuesize;
	uint32_t maxplayers;
	uint32_t bytessec;
	uint32_t latencyms;
	uint32_t defaultturnssec;
	uint32_t defaultturnsintransit;
};

struct _SNETEVENT {
	uint32_t eventid;
	uint32_t playerid;
	void *data;
	uint32_t databytes;
};

// Game states
#define GAMESTATE_PRIVATE 0x01
#define GAMESTATE_FULL 0x02
#define GAMESTATE_ACTIVE 0x04
#define GAMESTATE_STARTED 0x08
#define GAMESTATE_REPLAY 0x80

#define PS_CONNECTED 0x10000
#define PS_TURN_ARRIVED 0x20000
#define PS_ACTIVE 0x40000

#define LEAVE_ENDING 0x40000004
#define LEAVE_DROP 0x40000006

bool SNetCreateGame(const char *pszGameName, const char *pszGamePassword, char *GameTemplateData, int GameTemplateSize, int *playerID);
bool SNetDestroy();

/*  SNetDropPlayer @ 106
 *
 *  Drops a player from the current game.
 *
 *  playerid:     The player ID for the player to be dropped.
 *  flags:
 *
 *  Returns true if the function was called successfully and false otherwise.
 */
bool SNetDropPlayer(int playerid, uint32_t flags);

/*  SNetGetGameInfo @ 107
 *
 *  Retrieves specific game information from Storm, such as name, password,
 *  stats, mode, game template, and players.
 *
 *  type:         The type of data to retrieve. See GAMEINFO_ flags.
 *  dst:          The destination buffer for the data.
 *  length:       The maximum size of the destination buffer.
 *
 *  Returns true if the function was called successfully and false otherwise.
 */
bool SNetGetGameInfo(game_info type, void *dst, unsigned int length);

/*  SNetGetTurnsInTransit @ 115
 *
 *  Retrieves the number of turns (buffers) that have been queued
 *  before sending them over the network.
 *
 *  turns: A pointer to an integer that will receive the value.
 *
 *  Returns true if the function was called successfully and false otherwise.
 */
bool SNetGetTurnsInTransit(uint32_t *turns);

bool SNetJoinGame(char *gameName, char *gamePassword, int *playerid);

/*  SNetLeaveGame @ 119
 *
 *  Notifies Storm that the player has left the game. Storm will
 *  notify all connected peers through the network provider.
 *
 *  type: The leave type. It doesn't appear to be important, no documentation available.
 *
 *  Returns true if the function was called successfully and false otherwise.
 */
bool SNetLeaveGame(int type);

bool SNetReceiveMessage(int *senderplayerid, void **data, uint32_t *databytes);
bool SNetReceiveTurns(int arraysize, char **arraydata, size_t *arraydatabytes, uint32_t *arrayplayerstatus);

typedef void (*SEVTHANDLER)(struct _SNETEVENT *);

/*  SNetSendMessage @ 127
 *
 *  Sends a message to a player given their player ID. Network message
 *  is sent using class 01 and is retrieved by the other client using
 *  SNetReceiveMessage().
 *
 *  playerID:   The player index of the player to receive the data.
 *              Conversely, this field can be one of the following constants:
 *                  SNPLAYER_ALL      | Sends the message to all players, including oneself.
 *                  SNPLAYER_OTHERS   | Sends the message to all players, except for oneself.
 *  data:       A pointer to the data.
 *  databytes:  The amount of bytes that the data pointer contains.
 *
 *  Returns true if the function was called successfully and false otherwise.
 */
bool SNetSendMessage(int playerID, void *data, unsigned int databytes);

// Macro values to target specific players
#define SNPLAYER_ALL -1
#define SNPLAYER_OTHERS -2

/*  SNetSendTurn @ 128
 *
 *  Sends a turn (data packet) to all players in the game. Network data
 *  is sent using class 02 and is retrieved by the other client using
 *  SNetReceiveTurns().
 *
 *  data:       A pointer to the data.
 *  databytes:  The amount of bytes that the data pointer contains.
 *
 *  Returns true if the function was called successfully and false otherwise.
 */
bool SNetSendTurn(char *data, unsigned int databytes);

uint32_t SErrGetLastError();
void SErrSetLastError(uint32_t dwErrCode);

// Values for dwErrCode
#define STORM_ERROR_GAME_TERMINATED 0x85100069
#define STORM_ERROR_INVALID_PLAYER 0x8510006a
#define STORM_ERROR_NO_MESSAGES_WAITING 0x8510006b
#define STORM_ERROR_NOT_IN_GAME 0x85100070

bool SNetGetOwnerTurnsWaiting(uint32_t *);
bool SNetUnregisterEventHandler(event_type);
bool SNetRegisterEventHandler(event_type, SEVTHANDLER);
bool SNetSetBasePlayer(int);
bool SNetInitializeProvider(uint32_t provider, struct GameData *gameData);
void SNetGetProviderCaps(struct _SNETCAPS *);

bool DvlNet_SendInfoRequest();
void DvlNet_ClearGamelist();
std::vector<std::string> DvlNet_GetGamelist();
void DvlNet_SetPassword(std::string pw);
void DvlNet_ClearPassword();
bool DvlNet_IsPublicGame();

} // namespace devilution
