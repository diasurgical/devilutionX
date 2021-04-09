/**
 * @file multi.h
 *
 * Interface of functions for keeping multiplayer games in sync.
 */
#pragma once

#include "msg.h"

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GameData {
	Sint32 size;
	Sint32 dwSeed;
	Uint32 programid;
	Uint8 versionMajor;
	Uint8 versionMinor;
	Uint8 versionPatch;
	_difficulty nDifficulty;
	Uint8 nTickRate;
	Uint8 bRunInTown;
	Uint8 bTheoQuest;
	Uint8 bCowQuest;
	Uint8 bFriendlyFire;
} GameData;

extern bool gbSomebodyWonGameKludge;
extern char szPlayerDescript[128];
extern WORD sgwPackPlrOffsetTbl[MAX_PLRS];
extern BYTE gbActivePlayers;
extern bool gbGameDestroyed;
extern bool gbSelectProvider;
extern bool gbIsMultiplayer;
extern char szPlayerName[128];
extern BYTE gbDeltaSender;
extern int player_state[MAX_PLRS];

void multi_msg_add(BYTE *pbMsg, BYTE bLen);
void NetSendLoPri(BYTE *pbMsg, BYTE bLen);
void NetSendHiPri(BYTE *pbMsg, BYTE bLen);
void multi_send_msg_packet(int pmask, BYTE *src, BYTE len);
void multi_msg_countdown();
void multi_player_left(int pnum, int reason);
void multi_net_ping();
int multi_handle_delta();
void multi_process_network_packets();
void multi_send_zero_packet(int pnum, _cmd_id bCmd, BYTE *pbSrc, DWORD dwLen);
void NetClose();
bool NetInit(bool bSinglePlayer, bool *pfExitProgram);
bool multi_init_single(GameData *gameData);
bool multi_init_multi(GameData *gameData, bool *pfExitProgram);
void recv_plrinfo(int pnum, TCmdPlrInfoHdr *p, bool recv);

#ifdef __cplusplus
}
#endif

}
