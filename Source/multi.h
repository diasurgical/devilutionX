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
	Uint8 nDifficulty;
	Uint8 nTickRate;
	Uint8 bRunInTown;
	Uint8 bTheoQuest;
	Uint8 bCowQuest;
	Uint8 bFriendlyFire;
} GameData;

extern BOOLEAN gbSomebodyWonGameKludge;
extern char szPlayerDescript[128];
extern WORD sgwPackPlrOffsetTbl[MAX_PLRS];
extern BYTE gbActivePlayers;
extern BOOLEAN gbGameDestroyed;
extern BOOLEAN gbSelectProvider;
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
void multi_send_zero_packet(int pnum, BYTE bCmd, BYTE *pbSrc, DWORD dwLen);
void NetClose();
BOOL NetInit(BOOL bSinglePlayer, BOOL *pfExitProgram);
BOOL multi_init_single(GameData *gameData);
BOOL multi_init_multi(GameData *gameData, BOOL *pfExitProgram);
void recv_plrinfo(int pnum, TCmdPlrInfoHdr *p, BOOL recv);

#ifdef __cplusplus
}
#endif

}
