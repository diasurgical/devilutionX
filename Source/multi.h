/**
 * @file multi.h
 *
 * Interface of functions for keeping multiplayer games in sync.
 */
#pragma once

#include <cstdint>

#include "msg.h"

namespace devilution {

// must be unsigned to generate unsigned comparisons with pnum
#define MAX_PLRS 4

enum event_type : uint8_t {
	EVENT_TYPE_PLAYER_CREATE_GAME,
	EVENT_TYPE_PLAYER_LEAVE_GAME,
	EVENT_TYPE_PLAYER_MESSAGE,
};

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
extern bool gbIsMultiplayer;
extern char szPlayerName[128];
extern BYTE gbDeltaSender;
extern uint32_t player_state[MAX_PLRS];

void multi_msg_add(byte *pbMsg, BYTE bLen);
void NetSendLoPri(int playerId, byte *pbMsg, BYTE bLen);
void NetSendHiPri(int playerId, byte *pbMsg, BYTE bLen);
void multi_send_msg_packet(uint32_t pmask, byte *src, BYTE len);
void multi_msg_countdown();
void multi_player_left(int pnum, int reason);
void multi_net_ping();
bool multi_handle_delta();
void multi_process_network_packets();
void multi_send_zero_packet(int pnum, _cmd_id bCmd, byte *pbSrc, DWORD dwLen);
void NetClose();
bool NetInit(bool bSinglePlayer);
bool multi_init_single(GameData *gameData);
bool multi_init_multi(GameData *gameData);
void recv_plrinfo(int pnum, TCmdPlrInfoHdr *p, bool recv);

} // namespace devilution
