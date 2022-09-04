/**
 * @file multi.cpp
 *
 * Implementation of functions for keeping multiplaye games in sync.
 */

#include <SDL.h>
#include <config.h>

#include <ctime>
#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "diablo.h"
#include "engine/demomode.h"
#include "engine/point.hpp"
#include "engine/random.hpp"
#include "engine/world_tile.hpp"
#include "menu.h"
#include "nthread.h"
#include "options.h"
#include "pfile.h"
#include "plrmsg.h"
#include "qol/chatlog.h"
#include "storm/storm_net.hpp"
#include "sync.h"
#include "tmsg.h"
#include "utils/endian.hpp"
#include "utils/language.h"
#include "utils/stdcompat/cstddef.hpp"
#include "utils/stdcompat/string_view.hpp"
#include "utils/str_cat.hpp"

namespace devilution {

bool gbSomebodyWonGameKludge;
TBuffer sgHiPriBuf;
uint16_t sgwPackPlrOffsetTbl[MAX_PLRS];
bool sgbPlayerTurnBitTbl[MAX_PLRS];
bool sgbPlayerLeftGameTbl[MAX_PLRS];
bool gbShouldValidatePackage;
uint8_t gbActivePlayers;
bool gbGameDestroyed;
bool sgbSendDeltaTbl[MAX_PLRS];
GameData sgGameInitInfo;
bool gbSelectProvider;
int sglTimeoutStart;
int sgdwPlayerLeftReasonTbl[MAX_PLRS];
TBuffer sgLoPriBuf;
uint32_t sgdwGameLoops;
/**
 * Specifies the maximum number of players in a game, where 1
 * represents a single player game and 4 represents a multi player game.
 */
bool gbIsMultiplayer;
bool sgbTimeout;
std::string GameName;
std::string GamePassword;
bool PublicGame;
uint8_t gbDeltaSender;
bool sgbNetInited;
uint32_t player_state[MAX_PLRS];
Uint32 playerInfoTimers[MAX_PLRS];

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

void CopyPacket(TBuffer *buf, const byte *packet, size_t size)
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
		pBuf->dwNextWriteOffset += static_cast<uint32_t>(pBuf->bData - srcPtr);
		return body;
	}
	return body;
}

void NetReceivePlayerData(TPkt *pkt)
{
	const Player &myPlayer = *MyPlayer;
	const Point target = myPlayer.GetTargetPosition();

	pkt->hdr.wCheck = LoadBE32("\0\0ip");
	pkt->hdr.px = myPlayer.position.tile.x;
	pkt->hdr.py = myPlayer.position.tile.y;
	pkt->hdr.targx = target.x;
	pkt->hdr.targy = target.y;
	pkt->hdr.php = myPlayer._pHitPoints;
	pkt->hdr.pmhp = myPlayer._pMaxHP;
	pkt->hdr.mana = myPlayer._pMana;
	pkt->hdr.maxmana = myPlayer._pMaxMana;
	pkt->hdr.bstr = myPlayer._pBaseStr;
	pkt->hdr.bmag = myPlayer._pBaseMag;
	pkt->hdr.bdex = myPlayer._pBaseDex;
}

bool IsNetPlayerValid(const Player &player)
{
	return player._pLevel >= 1
	    && player._pLevel <= MaxCharacterLevel
	    && static_cast<uint8_t>(player._pClass) < enum_size<HeroClass>::value
	    && player.plrlevel < NUMLEVELS
	    && player.pDifficulty <= DIFF_LAST
	    && InDungeonBounds(player.position.tile)
	    && !string_view(player._pName).empty();
}

void CheckPlayerInfoTimeouts()
{
	for (size_t i = 0; i < Players.size(); i++) {
		Player &player = Players[i];
		if (&player == MyPlayer) {
			continue;
		}

		Uint32 &timerStart = playerInfoTimers[i];
		bool isPlayerConnected = (player_state[i] & PS_CONNECTED) != 0;
		bool isPlayerValid = isPlayerConnected && IsNetPlayerValid(player);
		if (isPlayerConnected && !isPlayerValid && timerStart == 0) {
			timerStart = SDL_GetTicks();
		}
		if (!isPlayerConnected || isPlayerValid) {
			timerStart = 0;
		}

		if (timerStart == 0) {
			continue;
		}

		// Time the player out after 15 seconds
		// if we do not receive valid player info
		if (SDL_GetTicks() - timerStart >= 15000) {
			SNetDropPlayer(i, LEAVE_DROP);
			timerStart = 0;
		}
	}
}

void SendPacket(int playerId, const byte *packet, size_t size)
{
	TPkt pkt;

	NetReceivePlayerData(&pkt);
	pkt.hdr.wLen = static_cast<uint16_t>(size + sizeof(pkt.hdr));
	memcpy(pkt.body, packet, size);
	if (!SNetSendMessage(playerId, &pkt.hdr, pkt.hdr.wLen))
		nthread_terminate_game("SNetSendMessage0");
}

void MonsterSeeds()
{
	sgdwGameLoops++;
	const uint32_t seed = (sgdwGameLoops >> 8) | (sgdwGameLoops << 24);
	for (size_t i = 0; i < MaxMonsters; i++)
		Monsters[i].aiSeed = seed + i;
}

void HandleTurnUpperBit(size_t pnum)
{
	size_t i;

	for (i = 0; i < Players.size(); i++) {
		if ((player_state[i] & PS_CONNECTED) != 0 && i != pnum)
			break;
	}

	if (MyPlayerId == i) {
		sgbSendDeltaTbl[pnum] = true;
	} else if (pnum == MyPlayerId) {
		gbDeltaSender = i;
	}
}

void ParseTurn(size_t pnum, uint32_t turn)
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
	Player &player = Players[pnum];

	if (&player == MyPlayer)
		return;
	if (!player.plractive)
		return;

	FixPlrWalkTags(player);
	RemovePortalMissile(pnum);
	DeactivatePortal(pnum);
	delta_close_portal(pnum);
	RemovePlrMissiles(player);
	if (left) {
		string_view pszFmt = _("Player '{:s}' just left the game");
		switch (sgdwPlayerLeftReasonTbl[pnum]) {
		case LEAVE_ENDING:
			pszFmt = _("Player '{:s}' killed Diablo and left the game!");
			gbSomebodyWonGameKludge = true;
			break;
		case LEAVE_DROP:
			pszFmt = _("Player '{:s}' dropped due to timeout");
			break;
		}
		EventPlrMsg(fmt::format(fmt::runtime(pszFmt), player._pName));
	}
	player.plractive = false;
	player._pName[0] = '\0';
	ResetPlayerGFX(player);
	gbActivePlayers--;
}

void ClearPlayerLeftState()
{
	for (size_t i = 0; i < Players.size(); i++) {
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
	for (size_t i = 0; i < Players.size(); i++) {
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

	CheckDropPlayer();
}

void HandleAllPackets(size_t pnum, const byte *data, size_t size)
{
	for (unsigned offset = 0; offset < size;) {
		size_t messageSize = ParseCmd(pnum, reinterpret_cast<const TCmd *>(&data[offset]));
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
	PlayerPack pPack;
	Player &myPlayer = *MyPlayer;
	PackPlayer(&pPack, myPlayer, true, true);
	pPack.friendlyMode = myPlayer.friendlyMode ? 1 : 0;
	pPack.isOnSetLevel = myPlayer.plrIsOnSetLevel;
	multi_send_zero_packet(pnum, cmd, reinterpret_cast<byte *>(&pPack), sizeof(PlayerPack));
}

void SetupLocalPositions()
{
	currlevel = 0;
	leveltype = DTYPE_TOWN;
	setlevel = false;

	const auto x = static_cast<WorldTileCoord>(75 + plrxoff[MyPlayerId]);
	const auto y = static_cast<WorldTileCoord>(68 + plryoff[MyPlayerId]);

	Player &myPlayer = *MyPlayer;

	myPlayer.position.tile = WorldTilePosition { x, y };
	myPlayer.position.future = WorldTilePosition { x, y };
	myPlayer.setLevel(currlevel);
	myPlayer._pLvlChanging = true;
	myPlayer.pLvlLoad = 0;
	myPlayer._pmode = PM_NEWLVL;
	myPlayer.destAction = ACTION_NONE;
}

void HandleEvents(_SNETEVENT *pEvt)
{
	switch (pEvt->eventid) {
	case EVENT_TYPE_PLAYER_CREATE_GAME: {
		auto *gameData = (GameData *)pEvt->data;
		if (gameData->size != sizeof(GameData))
			app_fatal(StrCat("Invalid size of game data: ", gameData->size));
		sgGameInitInfo = *gameData;
		sgbPlayerTurnBitTbl[pEvt->playerid] = true;
		break;
	}
	case EVENT_TYPE_PLAYER_LEAVE_GAME: {
		sgbPlayerLeftGameTbl[pEvt->playerid] = true;
		sgbPlayerTurnBitTbl[pEvt->playerid] = false;

		int leftReason = 0;
		if (pEvt->data != nullptr && pEvt->databytes >= sizeof(leftReason))
			leftReason = *(int *)pEvt->data;
		sgdwPlayerLeftReasonTbl[pEvt->playerid] = leftReason;
		if (leftReason == LEAVE_ENDING)
			gbSomebodyWonGameKludge = true;

		sgbSendDeltaTbl[pEvt->playerid] = false;

		if (gbDeltaSender == pEvt->playerid)
			gbDeltaSender = MAX_PLRS;
	} break;
	case EVENT_TYPE_PLAYER_MESSAGE:
		EventPlrMsg((char *)pEvt->data);
		break;
	}
}

void RegisterNetEventHandlers()
{
	for (auto eventType : EventTypes) {
		if (!SNetRegisterEventHandler(eventType, HandleEvents)) {
			app_fatal(StrCat("SNetRegisterEventHandler:\n", SDL_GetError()));
		}
	}
}

void UnregisterNetEventHandlers()
{
	for (auto eventType : EventTypes) {
		SNetUnregisterEventHandler(eventType);
	}
}

bool InitSingle(GameData *gameData)
{
	Players.resize(1);

	if (!SNetInitializeProvider(SELCONN_LOOPBACK, gameData)) {
		return false;
	}

	int unused = 0;
	if (!SNetCreateGame("local", "local", (char *)&sgGameInitInfo, sizeof(sgGameInitInfo), &unused)) {
		app_fatal(StrCat("SNetCreateGame1:\n", SDL_GetError()));
	}

	MyPlayerId = 0;
	MyPlayer = &Players[MyPlayerId];
	gbIsMultiplayer = false;

	pfile_read_player_from_save(gSaveNumber, *MyPlayer);

	return true;
}

bool InitMulti(GameData *gameData)
{
	Players.resize(MAX_PLRS);

	int playerId;

	while (true) {
		if (gbSelectProvider && !UiSelectProvider(gameData)) {
			return false;
		}

		RegisterNetEventHandlers();
		if (UiSelectGame(gameData, &playerId))
			break;

		gbSelectProvider = true;
	}

	if (static_cast<size_t>(playerId) >= Players.size()) {
		return false;
	}
	MyPlayerId = playerId;
	MyPlayer = &Players[MyPlayerId];
	gbIsMultiplayer = true;

	pfile_read_player_from_save(gSaveNumber, *MyPlayer);

	return true;
}

} // namespace

void InitGameInfo()
{
	sgGameInitInfo.size = sizeof(sgGameInitInfo);
	sgGameInitInfo.dwSeed = static_cast<uint32_t>(time(nullptr));
	sgGameInitInfo.programid = GAME_ID;
	sgGameInitInfo.versionMajor = PROJECT_VERSION_MAJOR;
	sgGameInitInfo.versionMinor = PROJECT_VERSION_MINOR;
	sgGameInitInfo.versionPatch = PROJECT_VERSION_PATCH;
	sgGameInitInfo.nTickRate = *sgOptions.Gameplay.tickRate;
	sgGameInitInfo.bRunInTown = *sgOptions.Gameplay.runInTown ? 1 : 0;
	sgGameInitInfo.bTheoQuest = *sgOptions.Gameplay.theoQuest ? 1 : 0;
	sgGameInitInfo.bCowQuest = *sgOptions.Gameplay.cowQuest ? 1 : 0;
	sgGameInitInfo.bFriendlyFire = *sgOptions.Gameplay.friendlyFire ? 1 : 0;
}

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
		pkt.hdr.wLen = static_cast<uint16_t>(len);
		if (!SNetSendMessage(SNPLAYER_OTHERS, &pkt.hdr, static_cast<unsigned>(len)))
			nthread_terminate_game("SNetSendMessage");
	}
}

void multi_send_msg_packet(uint32_t pmask, const byte *data, size_t size)
{
	TPkt pkt;
	NetReceivePlayerData(&pkt);
	size_t len = size + sizeof(pkt.hdr);
	pkt.hdr.wLen = static_cast<uint16_t>(len);
	memcpy(pkt.body, data, size);
	size_t playerID = 0;
	for (size_t v = 1; playerID < Players.size(); playerID++, v <<= 1) {
		if ((v & pmask) != 0) {
			if (!SNetSendMessage(playerID, &pkt.hdr, len) && SErrGetLastError() != STORM_ERROR_INVALID_PLAYER) {
				nthread_terminate_game("SNetSendMessage");
				return;
			}
		}
	}
}

void multi_msg_countdown()
{
	for (size_t i = 0; i < Players.size(); i++) {
		if ((player_state[i] & PS_TURN_ARRIVED) != 0) {
			if (gdwMsgLenTbl[i] == sizeof(int32_t))
				ParseTurn(i, *(int32_t *)glpMsgTbl[i]);
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

	for (size_t i = 0; i < Players.size(); i++) {
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

	uint8_t playerId = std::numeric_limits<uint8_t>::max();
	TPktHdr *pkt;
	uint32_t dwMsgSize = 0;
	while (SNetReceiveMessage(&playerId, (void **)&pkt, &dwMsgSize)) {
		dwRecCount++;
		ClearPlayerLeftState();
		if (dwMsgSize < sizeof(TPktHdr))
			continue;
		if (playerId >= Players.size())
			continue;
		if (pkt->wCheck != LoadBE32("\0\0ip"))
			continue;
		if (pkt->wLen != dwMsgSize)
			continue;
		Player &player = Players[playerId];
		if (!IsNetPlayerValid(player)) {
			_cmd_id cmd = *(const _cmd_id *)(pkt + 1);
			if (gbBufferMsgs == 0 && IsNoneOf(cmd, CMD_SEND_PLRINFO, CMD_ACK_PLRINFO)) {
				// Distrust all messages until
				// player info is received
				continue;
			}
		}
		Point syncPosition = { pkt->px, pkt->py };
		player.position.last = syncPosition;
		if (&player != MyPlayer) {
			assert(gbBufferMsgs != 2);
			player._pHitPoints = pkt->php;
			player._pMaxHP = pkt->pmhp;
			player._pMana = pkt->mana;
			player._pMaxMana = pkt->maxmana;
			bool cond = gbBufferMsgs == 1;
			player._pBaseStr = pkt->bstr;
			player._pBaseMag = pkt->bmag;
			player._pBaseDex = pkt->bdex;
			if (!cond && player.plractive && player._pHitPoints != 0) {
				if (player.isOnActiveLevel() && !player._pLvlChanging) {
					if (player.position.tile.WalkingDistance(syncPosition) > 3 && dPlayer[pkt->px][pkt->py] == 0) {
						// got out of sync, clear the tiles around where we last thought the player was located
						FixPlrWalkTags(player);

						player.position.old = player.position.tile;
						// then just in case clear the tiles around the current position (probably unnecessary)
						FixPlrWalkTags(player);
						player.position.tile = syncPosition;
						player.position.future = syncPosition;
						if (player.IsWalking())
							player.position.temp = syncPosition;
						dPlayer[player.position.tile.x][player.position.tile.y] = playerId + 1;
					}
					if (player.position.future.WalkingDistance(player.position.tile) > 1) {
						player.position.future = player.position.tile;
					}
					MakePlrPath(player, { pkt->targx, pkt->targy }, true);
				} else {
					player.position.tile = syncPosition;
					player.position.future = syncPosition;
				}
			}
		}
		HandleAllPackets(playerId, (const byte *)(pkt + 1), dwMsgSize - sizeof(TPktHdr));
	}
	if (SErrGetLastError() != STORM_ERROR_NO_MESSAGES_WAITING)
		nthread_terminate_game("SNetReceiveMsg");
	CheckPlayerInfoTimeouts();
}

void multi_send_zero_packet(size_t pnum, _cmd_id bCmd, const byte *data, size_t size)
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
	tmsg_cleanup();
	UnregisterNetEventHandlers();
	SNetLeaveGame(3);
	if (gbIsMultiplayer)
		SDL_Delay(2000);
	if (!demo::IsRunning()) {
		Players.clear();
		MyPlayer = nullptr;
	}
}

bool NetInit(bool bSinglePlayer)
{
	while (true) {
		SetRndSeed(0);
		InitGameInfo();
		memset(sgbPlayerTurnBitTbl, 0, sizeof(sgbPlayerTurnBitTbl));
		gbGameDestroyed = false;
		memset(sgbPlayerLeftGameTbl, 0, sizeof(sgbPlayerLeftGameTbl));
		memset(sgdwPlayerLeftReasonTbl, 0, sizeof(sgdwPlayerLeftReasonTbl));
		memset(sgbSendDeltaTbl, 0, sizeof(sgbSendDeltaTbl));
		Players.clear();
		MyPlayer = nullptr;
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
		tmsg_start();
		sgdwGameLoops = 0;
		sgbSentThisCycle = 0;
		gbDeltaSender = MyPlayerId;
		gbSomebodyWonGameKludge = false;
		nthread_send_and_recv_turn(0, 0);
		SetupLocalPositions();
		SendPlayerInfo(SNPLAYER_OTHERS, CMD_SEND_PLRINFO);

		Player &myPlayer = *MyPlayer;
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
	}
	PublicGame = DvlNet_IsPublicGame();

	Player &myPlayer = *MyPlayer;
	// separator for marking messages from a different game
	AddMessageToChatLog(_("New Game"), nullptr, UiFlags::ColorRed);
	AddMessageToChatLog(fmt::format(fmt::runtime(_("Player '{:s}' (level {:d}) just joined the game")), myPlayer._pName, myPlayer._pLevel));

	return true;
}

void recv_plrinfo(int pnum, const TCmdPlrInfoHdr &header, bool recv)
{
	static PlayerPack PackedPlayerBuffer[MAX_PLRS];

	assert(pnum >= 0 && pnum < MAX_PLRS);
	Player &player = Players[pnum];
	if (&player == MyPlayer) {
		return;
	}
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
	player.friendlyMode = packedPlayer.friendlyMode != 0;
	player.plrIsOnSetLevel = packedPlayer.isOnSetLevel != 0;

	if (!recv) {
		return;
	}

	ResetPlayerGFX(player);
	player.plractive = true;
	gbActivePlayers++;

	string_view szEvent;
	if (sgbPlayerTurnBitTbl[pnum]) {
		szEvent = _("Player '{:s}' (level {:d}) just joined the game");
	} else {
		szEvent = _("Player '{:s}' (level {:d}) is already in the game");
	}
	EventPlrMsg(fmt::format(fmt::runtime(szEvent), player._pName, player._pLevel));

	SyncInitPlr(player);

	if (!player.isOnActiveLevel()) {
		return;
	}

	if (player._pHitPoints >> 6 > 0) {
		StartStand(player, Direction::South);
		return;
	}

	player._pgfxnum &= ~0xF;
	player._pmode = PM_DEATH;
	NewPlrAnim(player, player_graphic::Death, Direction::South);
	player.AnimInfo.currentFrame = player.AnimInfo.numberOfFrames - 2;
	dFlags[player.position.tile.x][player.position.tile.y] |= DungeonFlag::DeadPlayer;
}

} // namespace devilution
