/**
 * @file multi.cpp
 *
 * Implementation of functions for keeping multiplaye games in sync.
 */

#include <SDL.h>
#include <config.h>

#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "diablo.h"
#include "dthread.h"
#include "engine/point.hpp"
#include "engine/random.hpp"
#include "menu.h"
#include "nthread.h"
#include "options.h"
#include "pfile.h"
#include "plrmsg.h"
#include "storm/storm_net.hpp"
#include "sync.h"
#include "tmsg.h"
#include "utils/endian.hpp"
#include "utils/language.h"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

bool gbSomebodyWonGameKludge;
TBuffer sgHiPriBuf;
char szPlayerDescript[128];
uint16_t sgwPackPlrOffsetTbl[MAX_PLRS];
bool sgbPlayerTurnBitTbl[MAX_PLRS];
bool sgbPlayerLeftGameTbl[MAX_PLRS];
bool gbShouldValidatePackage;
BYTE gbActivePlayers;
bool gbGameDestroyed;
bool sgbSendDeltaTbl[MAX_PLRS];
GameData sgGameInitInfo;
bool gbSelectProvider;
int sglTimeoutStart;
int sgdwPlayerLeftReasonTbl[MAX_PLRS];
TBuffer sgLoPriBuf;
DWORD sgdwGameLoops;
/**
 * Specifies the maximum number of players in a game, where 1
 * represents a single player game and 4 represents a multi player game.
 */
bool gbIsMultiplayer;
bool sgbTimeout;
char szPlayerName[128];
bool PublicGame;
BYTE gbDeltaSender;
bool sgbNetInited;
uint32_t player_state[MAX_PLRS];

/**
 * Contains the set of supported event types supported by the multiplayer
 * event handler.
 */
const event_type EventTypes[3] = {
	EVENT_TYPE_PLAYER_LEAVE_GAME,
	EVENT_TYPE_PLAYER_CREATE_GAME,
	EVENT_TYPE_PLAYER_MESSAGE
};

namespace {

uint32_t sgbSentThisCycle;

void BufferInit(TBuffer *pBuf)
{
	pBuf->dwNextWriteOffset = 0;
	pBuf->bData[0] = byte { 0 };
}

void CopyPacket(TBuffer *buf, const byte *packet, uint8_t size)
{
	if (buf->dwNextWriteOffset + size + 2 > 0x1000) {
		return;
	}

	byte *p = &buf->bData[buf->dwNextWriteOffset];
	buf->dwNextWriteOffset += size + 1;
	*p = static_cast<byte>(size);
	p++;
	memcpy(p, packet, size);
	p[size] = byte { 0 };
}

byte *ReceivePacket(TBuffer *pBuf, byte *body, size_t *size)
{
	if (pBuf->dwNextWriteOffset != 0) {
		byte *srcPtr = pBuf->bData;
		while (true) {
			auto chunkSize = static_cast<uint8_t>(*srcPtr);
			if (chunkSize == 0)
				break;
			if (chunkSize > *size)
				break;
			srcPtr++;
			memcpy(body, srcPtr, chunkSize);
			body += chunkSize;
			srcPtr += chunkSize;
			*size -= chunkSize;
		}
		memcpy(pBuf->bData, srcPtr, (pBuf->bData - srcPtr) + pBuf->dwNextWriteOffset + 1);
		pBuf->dwNextWriteOffset += (pBuf->bData - srcPtr);
		return body;
	}
	return body;
}

void NetReceivePlayerData(TPkt *pkt)
{
	const auto &myPlayer = Players[MyPlayerId];
	const Point target = myPlayer.GetTargetPosition();

	pkt->hdr.wCheck = LoadBE32("\0\0ip");
	pkt->hdr.px = myPlayer.position.tile.x;
	pkt->hdr.py = myPlayer.position.tile.y;
	pkt->hdr.targx = target.x;
	pkt->hdr.targy = target.y;
	pkt->hdr.php = myPlayer._pHitPoints;
	pkt->hdr.pmhp = myPlayer._pMaxHP;
	pkt->hdr.bstr = myPlayer._pBaseStr;
	pkt->hdr.bmag = myPlayer._pBaseMag;
	pkt->hdr.bdex = myPlayer._pBaseDex;
}

void SendPacket(int playerId, const byte *packet, size_t size)
{
	TPkt pkt;

	NetReceivePlayerData(&pkt);
	pkt.hdr.wLen = size + sizeof(pkt.hdr);
	memcpy(pkt.body, packet, size);
	if (!SNetSendMessage(playerId, &pkt.hdr, pkt.hdr.wLen))
		nthread_terminate_game("SNetSendMessage0");
}

void MonsterSeeds()
{
	sgdwGameLoops++;
	uint32_t l = (sgdwGameLoops >> 8) | (sgdwGameLoops << 24); // _rotr(sgdwGameLoops, 8)
	for (int i = 0; i < MAXMONSTERS; i++)
		Monsters[i]._mAISeed = l + i;
}

void HandleTurnUpperBit(int pnum)
{
	int i;

	for (i = 0; i < MAX_PLRS; i++) {
		if ((player_state[i] & PS_CONNECTED) != 0 && i != pnum)
			break;
	}

	if (MyPlayerId == i) {
		sgbSendDeltaTbl[pnum] = true;
	} else if (MyPlayerId == pnum) {
		gbDeltaSender = i;
	}
}

void ParseTurn(int pnum, uint32_t turn)
{
	if ((turn & 0x80000000) != 0)
		HandleTurnUpperBit(pnum);
	uint32_t absTurns = turn & 0x7FFFFFFF;
	if (sgbSentThisCycle < gdwTurnsInTransit + absTurns) {
		if (absTurns >= 0x7FFFFFFF)
			absTurns &= 0xFFFF;
		sgbSentThisCycle = absTurns + gdwTurnsInTransit;
		sgdwGameLoops = 4 * absTurns * sgbNetUpdateRate;
	}
}

void PlayerLeftMsg(int pnum, bool left)
{
	auto &player = Players[pnum];

	if (!player.plractive) {
		return;
	}

	RemovePlrFromMap(pnum);
	RemovePortalMissile(pnum);
	DeactivatePortal(pnum);
	delta_close_portal(pnum);
	RemovePlrMissiles(pnum);
	if (left) {
		const char *pszFmt = _("Player '{:s}' just left the game");
		switch (sgdwPlayerLeftReasonTbl[pnum]) {
		case LEAVE_ENDING:
			pszFmt = _("Player '{:s}' killed Diablo and left the game!");
			gbSomebodyWonGameKludge = true;
			break;
		case LEAVE_DROP:
			pszFmt = _("Player '{:s}' dropped due to timeout");
			break;
		}
		EventPlrMsg(fmt::format(pszFmt, player._pName).c_str());
	}
	player.plractive = false;
	player._pName[0] = '\0';
	ResetPlayerGFX(player);
	gbActivePlayers--;
}

void ClearPlayerLeftState()
{
	for (int i = 0; i < MAX_PLRS; i++) {
		if (sgbPlayerLeftGameTbl[i]) {
			if (gbBufferMsgs == 1)
				msg_send_drop_pkt(i, sgdwPlayerLeftReasonTbl[i]);
			else
				PlayerLeftMsg(i, true);

			sgbPlayerLeftGameTbl[i] = false;
			sgdwPlayerLeftReasonTbl[i] = 0;
		}
	}
}

void CheckDropPlayer()
{
	for (int i = 0; i < MAX_PLRS; i++) {
		if ((player_state[i] & PS_ACTIVE) == 0 && (player_state[i] & PS_CONNECTED) != 0) {
			SNetDropPlayer(i, LEAVE_DROP);
		}
	}
}

void BeginTimeout()
{
	if (!sgbTimeout) {
		return;
	}
#ifdef _DEBUG
	if (DebugDisableNetworkTimeout) {
		return;
	}
#endif

	uint32_t nTicks = SDL_GetTicks() - sglTimeoutStart;
	if (nTicks > 20000) {
		gbRunGame = false;
		return;
	}
	if (nTicks < 10000) {
		return;
	}

	int nLowestActive = -1;
	int nLowestPlayer = -1;
	uint8_t bGroupPlayers = 0;
	uint8_t bGroupCount = 0;
	for (int i = 0; i < MAX_PLRS; i++) {
		uint32_t nState = player_state[i];
		if ((nState & PS_CONNECTED) != 0) {
			if (nLowestPlayer == -1) {
				nLowestPlayer = i;
			}
			if ((nState & PS_ACTIVE) != 0) {
				bGroupPlayers++;
				if (nLowestActive == -1) {
					nLowestActive = i;
				}
			} else {
				bGroupCount++;
			}
		}
	}

	assert(bGroupPlayers);
	assert(nLowestActive != -1);
	assert(nLowestPlayer != -1);

	if (bGroupPlayers < bGroupCount) {
		gbGameDestroyed = true;
	} else if (bGroupPlayers == bGroupCount) {
		if (nLowestPlayer != nLowestActive) {
			gbGameDestroyed = true;
		} else if (nLowestActive == MyPlayerId) {
			CheckDropPlayer();
		}
	} else if (nLowestActive == MyPlayerId) {
		CheckDropPlayer();
	}
}

void HandleAllPackets(int pnum, const byte *data, size_t size)
{
	for (unsigned offset = 0; offset < size;) {
		int messageSize = ParseCmd(pnum, reinterpret_cast<const TCmd *>(&data[offset]));
		if (messageSize == 0) {
			break;
		}
		offset += messageSize;
	}
}

void ProcessTmsgs()
{
	while (true) {
		std::unique_ptr<byte[]> msg;
		uint8_t size = tmsg_get(&msg);
		if (size == 0)
			break;

		HandleAllPackets(MyPlayerId, msg.get(), size);
	}
}

void SendPlayerInfo(int pnum, _cmd_id cmd)
{
	static_assert(alignof(PlayerPack) == 1, "Fix pkplr alignment");
	std::unique_ptr<byte[]> pkplr { new byte[sizeof(PlayerPack)] };

	PackPlayer(reinterpret_cast<PlayerPack *>(pkplr.get()), Players[MyPlayerId], true);
	dthread_send_delta(pnum, cmd, std::move(pkplr), sizeof(PlayerPack));
}

dungeon_type InitLevelType(int l)
{
	if (l == 0)
		return DTYPE_TOWN;
	if (l >= 1 && l <= 4)
		return DTYPE_CATHEDRAL;
	if (l >= 5 && l <= 8)
		return DTYPE_CATACOMBS;
	if (l >= 9 && l <= 12)
		return DTYPE_CAVES;
	if (l >= 13 && l <= 16)
		return DTYPE_HELL;
	if (l >= 21 && l <= 24)
		return DTYPE_CATHEDRAL; // Crypt
	if (l >= 17 && l <= 20)
		return DTYPE_CAVES; // Hive

	return DTYPE_CATHEDRAL;
}

void SetupLocalPositions()
{
	currlevel = 0;
	leveltype = DTYPE_TOWN;
	setlevel = false;

	int x = 75;
	int y = 68;

	x += plrxoff[MyPlayerId];
	y += plryoff[MyPlayerId];

	auto &myPlayer = Players[MyPlayerId];

	myPlayer.position.tile = { x, y };
	myPlayer.position.future = { x, y };
	myPlayer.plrlevel = currlevel;
	myPlayer._pLvlChanging = true;
	myPlayer.pLvlLoad = 0;
	myPlayer._pmode = PM_NEWLVL;
	myPlayer.destAction = ACTION_NONE;
}

void HandleEvents(_SNETEVENT *pEvt)
{
	DWORD leftReason;

	switch (pEvt->eventid) {
	case EVENT_TYPE_PLAYER_CREATE_GAME: {
		auto *gameData = (GameData *)pEvt->data;
		if (gameData->size != sizeof(GameData))
			app_fatal("Invalid size of game data: %i", gameData->size);
		sgGameInitInfo = *gameData;
		sgbPlayerTurnBitTbl[pEvt->playerid] = true;
		break;
	}
	case EVENT_TYPE_PLAYER_LEAVE_GAME:
		sgbPlayerLeftGameTbl[pEvt->playerid] = true;
		sgbPlayerTurnBitTbl[pEvt->playerid] = false;

		leftReason = 0;
		if (pEvt->data != nullptr && pEvt->databytes >= sizeof(DWORD))
			leftReason = *(DWORD *)pEvt->data;
		sgdwPlayerLeftReasonTbl[pEvt->playerid] = leftReason;
		if (leftReason == LEAVE_ENDING)
			gbSomebodyWonGameKludge = true;

		sgbSendDeltaTbl[pEvt->playerid] = false;
		dthread_remove_player(pEvt->playerid);

		if (gbDeltaSender == pEvt->playerid)
			gbDeltaSender = MAX_PLRS;
		break;
	case EVENT_TYPE_PLAYER_MESSAGE:
		ErrorPlrMsg((char *)pEvt->data);
		break;
	}
}

void EventHandler(bool add)
{
	for (auto eventType : EventTypes) {
		if (add) {
			if (!SNetRegisterEventHandler(eventType, HandleEvents)) {
				app_fatal("SNetRegisterEventHandler:\n%s", SDL_GetError());
			}
		} else {
			SNetUnregisterEventHandler(eventType);
		}
	}
}

bool InitSingle(GameData *gameData)
{
	if (!SNetInitializeProvider(SELCONN_LOOPBACK, gameData)) {
		return false;
	}

	int unused = 0;
	if (!SNetCreateGame("local", "local", (char *)&sgGameInitInfo, sizeof(sgGameInitInfo), &unused)) {
		app_fatal("SNetCreateGame1:\n%s", SDL_GetError());
	}

	MyPlayerId = 0;
	MyPlayer = &Players[MyPlayerId];
	gbIsMultiplayer = false;

	return true;
}

bool InitMulti(GameData *gameData)
{
	int playerId;

	while (true) {
		if (gbSelectProvider && !UiSelectProvider(gameData)) {
			return false;
		}

		EventHandler(true);
		if (UiSelectGame(gameData, &playerId))
			break;

		gbSelectProvider = true;
	}

	if ((DWORD)playerId >= MAX_PLRS) {
		return false;
	}
	MyPlayerId = playerId;
	MyPlayer = &Players[MyPlayerId];
	gbIsMultiplayer = true;

	pfile_read_player_from_save(gSaveNumber, *MyPlayer);

	return true;
}

} // namespace

void NetSendLoPri(int playerId, const byte *data, size_t size)
{
	if (data != nullptr && size != 0) {
		CopyPacket(&sgLoPriBuf, data, size);
		SendPacket(playerId, data, size);
	}
}

void NetSendHiPri(int playerId, const byte *data, size_t size)
{
	if (data != nullptr && size != 0) {
		CopyPacket(&sgHiPriBuf, data, size);
		SendPacket(playerId, data, size);
	}
	if (!gbShouldValidatePackage) {
		gbShouldValidatePackage = true;
		TPkt pkt;
		NetReceivePlayerData(&pkt);
		size_t msgSize = gdwNormalMsgSize - sizeof(TPktHdr);
		byte *hipriBody = ReceivePacket(&sgHiPriBuf, pkt.body, &msgSize);
		byte *lowpriBody = ReceivePacket(&sgLoPriBuf, hipriBody, &msgSize);
		msgSize = sync_all_monsters(lowpriBody, msgSize);
		size_t len = gdwNormalMsgSize - msgSize;
		pkt.hdr.wLen = len;
		if (!SNetSendMessage(SNPLAYER_OTHERS, &pkt.hdr, len))
			nthread_terminate_game("SNetSendMessage");
	}
}

void multi_send_msg_packet(uint32_t pmask, const byte *data, size_t size)
{
	TPkt pkt;
	NetReceivePlayerData(&pkt);
	size_t t = size + sizeof(pkt.hdr);
	pkt.hdr.wLen = t;
	memcpy(pkt.body, data, size);
	size_t p = 0;
	for (size_t v = 1; p < MAX_PLRS; p++, v <<= 1) {
		if ((v & pmask) != 0) {
			if (!SNetSendMessage(p, &pkt.hdr, t) && SErrGetLastError() != STORM_ERROR_INVALID_PLAYER) {
				nthread_terminate_game("SNetSendMessage");
				return;
			}
		}
	}
}

void multi_msg_countdown()
{
	for (int i = 0; i < MAX_PLRS; i++) {
		if ((player_state[i] & PS_TURN_ARRIVED) != 0) {
			if (gdwMsgLenTbl[i] == 4)
				ParseTurn(i, *(DWORD *)glpMsgTbl[i]);
		}
	}
}

void multi_player_left(int pnum, int reason)
{
	sgbPlayerLeftGameTbl[pnum] = true;
	sgdwPlayerLeftReasonTbl[pnum] = reason;
	ClearPlayerLeftState();
}

void multi_net_ping()
{
	sgbTimeout = true;
	sglTimeoutStart = SDL_GetTicks();
}

bool multi_handle_delta()
{
	if (gbGameDestroyed) {
		gbRunGame = false;
		return false;
	}

	for (int i = 0; i < MAX_PLRS; i++) {
		if (sgbSendDeltaTbl[i]) {
			sgbSendDeltaTbl[i] = false;
			DeltaExportData(i);
		}
	}

	sgbSentThisCycle = nthread_send_and_recv_turn(sgbSentThisCycle, 1);
	bool received;
	if (!nthread_recv_turns(&received)) {
		BeginTimeout();
		return false;
	}

	sgbTimeout = false;
	if (received) {
		if (!gbShouldValidatePackage) {
			NetSendHiPri(MyPlayerId, nullptr, 0);
			gbShouldValidatePackage = false;
		} else {
			gbShouldValidatePackage = false;
			if (sgHiPriBuf.dwNextWriteOffset != 0)
				NetSendHiPri(MyPlayerId, nullptr, 0);
		}
	}
	MonsterSeeds();

	return true;
}

void multi_process_network_packets()
{
	ClearPlayerLeftState();
	ProcessTmsgs();

	int dwID = -1;
	TPktHdr *pkt;
	uint32_t dwMsgSize = 0;
	while (SNetReceiveMessage(&dwID, (void **)&pkt, &dwMsgSize)) {
		dwRecCount++;
		ClearPlayerLeftState();
		if (dwMsgSize < sizeof(TPktHdr))
			continue;
		if (dwID < 0 || dwID >= MAX_PLRS)
			continue;
		if (pkt->wCheck != LoadBE32("\0\0ip"))
			continue;
		if (pkt->wLen != dwMsgSize)
			continue;
		auto &player = Players[dwID];
		player.position.last = { pkt->px, pkt->py };
		if (dwID != MyPlayerId) {
			assert(gbBufferMsgs != 2);
			player._pHitPoints = pkt->php;
			player._pMaxHP = pkt->pmhp;
			bool cond = gbBufferMsgs == 1;
			player._pBaseStr = pkt->bstr;
			player._pBaseMag = pkt->bmag;
			player._pBaseDex = pkt->bdex;
			if (!cond && player.plractive && player._pHitPoints != 0) {
				if (currlevel == player.plrlevel && !player._pLvlChanging) {
					int dx = abs(player.position.tile.x - pkt->px);
					int dy = abs(player.position.tile.y - pkt->py);
					if ((dx > 3 || dy > 3) && dPlayer[pkt->px][pkt->py] == 0) {
						FixPlrWalkTags(dwID);
						player.position.old = player.position.tile;
						FixPlrWalkTags(dwID);
						player.position.tile = { pkt->px, pkt->py };
						player.position.future = { pkt->px, pkt->py };
						dPlayer[player.position.tile.x][player.position.tile.y] = dwID + 1;
					}
					dx = abs(player.position.future.x - player.position.tile.x);
					dy = abs(player.position.future.y - player.position.tile.y);
					if (dx > 1 || dy > 1) {
						player.position.future = player.position.tile;
					}
					MakePlrPath(player, { pkt->targx, pkt->targy }, true);
				} else {
					player.position.tile = { pkt->px, pkt->py };
					player.position.future = { pkt->px, pkt->py };
				}
			}
		}
		HandleAllPackets(dwID, (const byte *)(pkt + 1), dwMsgSize - sizeof(TPktHdr));
	}
	if (SErrGetLastError() != STORM_ERROR_NO_MESSAGES_WAITING)
		nthread_terminate_game("SNetReceiveMsg");
}

void multi_send_zero_packet(int pnum, _cmd_id bCmd, const byte *data, size_t size)
{
	assert(pnum != MyPlayerId);
	assert(data != nullptr);
	assert(size <= 0x0ffff);

	for (size_t offset = 0; offset < size;) {
		TPkt pkt {};
		pkt.hdr.wCheck = LoadBE32("\0\0ip");
		auto &message = *reinterpret_cast<TCmdPlrInfoHdr *>(pkt.body);
		message.bCmd = bCmd;
		message.wOffset = offset;

		size_t dwBody = gdwLargestMsgSize - sizeof(pkt.hdr) - sizeof(message);
		dwBody = std::min(dwBody, size - offset);
		assert(dwBody <= 0x0ffff);
		message.wBytes = dwBody;

		memcpy(&pkt.body[sizeof(message)], &data[offset], message.wBytes);

		size_t dwMsg = sizeof(pkt.hdr);
		dwMsg += sizeof(message);
		dwMsg += message.wBytes;
		pkt.hdr.wLen = dwMsg;

		if (!SNetSendMessage(pnum, &pkt, dwMsg)) {
			nthread_terminate_game("SNetSendMessage2");
			return;
		}

		offset += message.wBytes;
	}
}

void NetClose()
{
	if (!sgbNetInited) {
		return;
	}

	sgbNetInited = false;
	nthread_cleanup();
	DThreadCleanup();
	tmsg_cleanup();
	EventHandler(false);
	SNetLeaveGame(3);
	if (gbIsMultiplayer)
		SDL_Delay(2000);
}

bool NetInit(bool bSinglePlayer)
{
	while (true) {
		SetRndSeed(0);
		sgGameInitInfo.size = sizeof(sgGameInitInfo);
		sgGameInitInfo.dwSeed = time(nullptr);
		sgGameInitInfo.programid = GAME_ID;
		sgGameInitInfo.versionMajor = PROJECT_VERSION_MAJOR;
		sgGameInitInfo.versionMinor = PROJECT_VERSION_MINOR;
		sgGameInitInfo.versionPatch = PROJECT_VERSION_PATCH;
		sgGameInitInfo.nTickRate = sgOptions.Gameplay.nTickRate;
		sgGameInitInfo.bRunInTown = *sgOptions.Gameplay.runInTown ? 1 : 0;
		sgGameInitInfo.bTheoQuest = *sgOptions.Gameplay.theoQuest ? 1 : 0;
		sgGameInitInfo.bCowQuest = *sgOptions.Gameplay.cowQuest ? 1 : 0;
		sgGameInitInfo.bFriendlyFire = *sgOptions.Gameplay.friendlyFire ? 1 : 0;
		memset(sgbPlayerTurnBitTbl, 0, sizeof(sgbPlayerTurnBitTbl));
		gbGameDestroyed = false;
		memset(sgbPlayerLeftGameTbl, 0, sizeof(sgbPlayerLeftGameTbl));
		memset(sgdwPlayerLeftReasonTbl, 0, sizeof(sgdwPlayerLeftReasonTbl));
		memset(sgbSendDeltaTbl, 0, sizeof(sgbSendDeltaTbl));
		for (auto &player : Players) {
			player.Reset();
		}
		memset(sgwPackPlrOffsetTbl, 0, sizeof(sgwPackPlrOffsetTbl));
		SNetSetBasePlayer(0);
		if (bSinglePlayer) {
			if (!InitSingle(&sgGameInitInfo))
				return false;
		} else {
			if (!InitMulti(&sgGameInitInfo))
				return false;
		}
		sgbNetInited = true;
		sgbTimeout = false;
		delta_init();
		InitPlrMsg();
		BufferInit(&sgHiPriBuf);
		BufferInit(&sgLoPriBuf);
		gbShouldValidatePackage = false;
		sync_init();
		nthread_start(sgbPlayerTurnBitTbl[MyPlayerId]);
		dthread_start();
		tmsg_start();
		sgdwGameLoops = 0;
		sgbSentThisCycle = 0;
		gbDeltaSender = MyPlayerId;
		gbSomebodyWonGameKludge = false;
		nthread_send_and_recv_turn(0, 0);
		SetupLocalPositions();
		SendPlayerInfo(SNPLAYER_OTHERS, CMD_SEND_PLRINFO);

		auto &myPlayer = Players[MyPlayerId];
		ResetPlayerGFX(myPlayer);
		myPlayer.plractive = true;
		gbActivePlayers = 1;

		if (!sgbPlayerTurnBitTbl[MyPlayerId] || msg_wait_resync())
			break;
		NetClose();
		gbSelectProvider = false;
	}
	SetRndSeed(sgGameInitInfo.dwSeed);
	gnTickDelay = 1000 / sgGameInitInfo.nTickRate;

	for (int i = 0; i < NUMLEVELS; i++) {
		glSeedTbl[i] = AdvanceRndSeed();
		gnLevelTypeTbl[i] = InitLevelType(i);
	}
	if (!SNetGetGameInfo(GAMEINFO_NAME, szPlayerName, 128))
		nthread_terminate_game("SNetGetGameInfo1");
	if (!SNetGetGameInfo(GAMEINFO_PASSWORD, szPlayerDescript, 128))
		nthread_terminate_game("SNetGetGameInfo2");
	PublicGame = DvlNet_IsPublicGame();

	return true;
}

void recv_plrinfo(int pnum, const TCmdPlrInfoHdr &header, bool recv)
{
	static PlayerPack PackedPlayerBuffer[MAX_PLRS];

	if (MyPlayerId == pnum) {
		return;
	}
	assert(pnum >= 0 && pnum < MAX_PLRS);
	auto &player = Players[pnum];
	auto &packedPlayer = PackedPlayerBuffer[pnum];

	if (sgwPackPlrOffsetTbl[pnum] != header.wOffset) {
		sgwPackPlrOffsetTbl[pnum] = 0;
		if (header.wOffset != 0) {
			return;
		}
	}
	if (!recv && sgwPackPlrOffsetTbl[pnum] == 0) {
		SendPlayerInfo(pnum, CMD_ACK_PLRINFO);
	}

	memcpy(reinterpret_cast<uint8_t *>(&packedPlayer) + header.wOffset, reinterpret_cast<const uint8_t *>(&header) + sizeof(header), header.wBytes);

	sgwPackPlrOffsetTbl[pnum] += header.wBytes;
	if (sgwPackPlrOffsetTbl[pnum] != sizeof(packedPlayer)) {
		return;
	}
	sgwPackPlrOffsetTbl[pnum] = 0;

	PlayerLeftMsg(pnum, false);
	if (!UnPackPlayer(&packedPlayer, player, true)) {
		return;
	}

	if (!recv) {
		return;
	}

	ResetPlayerGFX(player);
	player.plractive = true;
	gbActivePlayers++;

	const char *szEvent;
	if (sgbPlayerTurnBitTbl[pnum]) {
		szEvent = _("Player '{:s}' (level {:d}) just joined the game");
	} else {
		szEvent = _("Player '{:s}' (level {:d}) is already in the game");
	}
	EventPlrMsg(fmt::format(szEvent, player._pName, player._pLevel).c_str());

	SyncInitPlr(pnum);

	if (player.plrlevel != currlevel) {
		return;
	}

	if (player._pHitPoints >> 6 > 0) {
		StartStand(pnum, Direction::South);
		return;
	}

	player._pgfxnum = 0;
	player._pmode = PM_DEATH;
	NewPlrAnim(player, player_graphic::Death, Direction::South, player._pDFrames, 1);
	player.AnimInfo.CurrentFrame = player.AnimInfo.NumberOfFrames - 1;
	dFlags[player.position.tile.x][player.position.tile.y] |= DungeonFlag::DeadPlayer;
}

} // namespace devilution
