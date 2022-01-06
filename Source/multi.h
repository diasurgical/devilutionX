/**
 * @file multi.h
 *
 * Interface of functions for keeping multiplayer games in sync.
 */
#pragma once

#include <cstdint>

#include "msg.h"
#include "utils/attributes.h"

namespace devilution {

// must be unsigned to generate unsigned comparisons with pnum
#define MAX_PLRS 4

struct GameData {
	int32_t size;
	/** Used to initialise the seed table for dungeon levels so players in multiplayer games generate the same layout */
	uint32_t dwSeed;
	uint32_t programid;
	uint8_t versionMajor;
	uint8_t versionMinor;
	uint8_t versionPatch;
	_difficulty nDifficulty;
	uint8_t nTickRate;
	uint8_t bRunInTown;
	uint8_t bTheoQuest;
	uint8_t bCowQuest;
	uint8_t bFriendlyFire;
};

extern bool gbSomebodyWonGameKludge;
extern char szPlayerDescript[128];
extern uint16_t sgwPackPlrOffsetTbl[MAX_PLRS];
extern BYTE gbActivePlayers;
extern bool gbGameDestroyed;
extern GameData sgGameInitInfo;
extern bool gbSelectProvider;
extern DVL_API_FOR_TEST bool gbIsMultiplayer;
extern char szPlayerName[128];
extern bool PublicGame;
extern BYTE gbDeltaSender;
extern uint32_t player_state[MAX_PLRS];

void InitGameInfo();
void NetSendLoPri(int playerId, const byte *data, size_t size);
void NetSendHiPri(int playerId, const byte *data, size_t size);
void multi_send_msg_packet(uint32_t pmask, const byte *data, size_t size);
void multi_msg_countdown();
void multi_player_left(int pnum, int reason);
void multi_net_ping();

/**
 * @return Always true for singleplayer
 */
bool multi_handle_delta();
void multi_process_network_packets();
void multi_send_zero_packet(int pnum, _cmd_id bCmd, const byte *data, size_t size);
void NetClose();
bool NetInit(bool bSinglePlayer);
void recv_plrinfo(int pnum, const TCmdPlrInfoHdr &header, bool recv);

} // namespace devilution
