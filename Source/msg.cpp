/**
 * @file msg.cpp
 *
 * Implementation of function for sending and reciving network messages.
 */
#include <climits>
#include <memory>

#include <fmt/format.h>
#include <list>

#include "DiabloUI/diabloui.h"
#include "automap.h"
#include "control.h"
#include "dead.h"
#include "drlg_l1.h"
#include "dthread.h"
#include "encrypt.h"
#include "engine/random.hpp"
#include "gamemenu.h"
#include "lighting.h"
#include "missiles.h"
#include "nthread.h"
#include "objects.h"
#include "options.h"
#include "pfile.h"
#include "plrmsg.h"
#include "spells.h"
#include "storm/storm_net.hpp"
#include "sync.h"
#include "tmsg.h"
#include "town.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"

namespace devilution {

bool deltaload;
BYTE gbBufferMsgs;
int dwRecCount;

namespace {

struct TMegaPkt {
	uint32_t spaceLeft;
	byte data[32000];

	TMegaPkt()
	    : spaceLeft(sizeof(data))
	{
	}
};

#define MAX_CHUNKS (NUMLEVELS + 4)

uint32_t sgdwOwnerWait;
uint32_t sgdwRecvOffset;
int sgnCurrMegaPlayer;
DLevel sgLevels[NUMLEVELS];
BYTE sbLastCmd;
byte sgRecvBuf[sizeof(DLevel) + 1];
_cmd_id sgbRecvCmd;
LocalLevel sgLocals[NUMLEVELS];
DJunk sgJunk;
bool sgbDeltaChanged;
BYTE sgbDeltaChunks;
std::list<TMegaPkt> MegaPktList;

void GetNextPacket()
{
	MegaPktList.emplace_back();
}

void FreePackets()
{
	MegaPktList.clear();
}

void PrePacket()
{
	uint8_t playerId = std::numeric_limits<uint8_t>::max();
	for (TMegaPkt &pkt : MegaPktList) {
		byte *data = pkt.data;
		size_t spaceLeft = sizeof(pkt.data);
		while (spaceLeft != pkt.spaceLeft) {
			auto cmdId = static_cast<_cmd_id>(*data);

			if (cmdId == FAKE_CMD_SETID) {
				auto *cmd = (TFakeCmdPlr *)data;
				data += sizeof(*cmd);
				spaceLeft -= sizeof(*cmd);
				playerId = cmd->bPlr;
				continue;
			}

			if (cmdId == FAKE_CMD_DROPID) {
				auto *cmd = (TFakeDropPlr *)data;
				data += sizeof(*cmd);
				spaceLeft -= sizeof(*cmd);
				multi_player_left(cmd->bPlr, cmd->dwReason);
				continue;
			}

			if (playerId >= MAX_PLRS) {
				Log("Missing source of network message");
				return;
			}

			uint32_t size = ParseCmd(playerId, (TCmd *)data);
			if (size == 0) {
				Log("Discarding bad network message");
				return;
			}
			uint32_t pktSize = size;
			data += pktSize;
			spaceLeft -= pktSize;
		}
	}
}

void SendPacket(int pnum, const void *packet, DWORD dwSize)
{
	TFakeCmdPlr cmd;

	if (pnum != sgnCurrMegaPlayer) {
		sgnCurrMegaPlayer = pnum;
		cmd.bCmd = FAKE_CMD_SETID;
		cmd.bPlr = pnum;
		SendPacket(pnum, &cmd, sizeof(cmd));
	}
	if (MegaPktList.back().spaceLeft < dwSize)
		GetNextPacket();

	TMegaPkt &currMegaPkt = MegaPktList.back();
	memcpy(currMegaPkt.data + sizeof(currMegaPkt.data) - currMegaPkt.spaceLeft, packet, dwSize);
	currMegaPkt.spaceLeft -= dwSize;
}

int WaitForTurns()
{
	DWORD turns;

	if (sgbDeltaChunks == 0) {
		nthread_send_and_recv_turn(0, 0);
		if (!SNetGetOwnerTurnsWaiting(&turns) && SErrGetLastError() == STORM_ERROR_NOT_IN_GAME)
			return 100;
		if (SDL_GetTicks() - sgdwOwnerWait <= 2000 && turns < gdwTurnsInTransit)
			return 0;
		sgbDeltaChunks++;
	}
	multi_process_network_packets();
	nthread_send_and_recv_turn(0, 0);
	if (nthread_has_500ms_passed()) {
		nthread_recv_turns();
	}

	if (gbGameDestroyed)
		return 100;
	if (gbDeltaSender >= MAX_PLRS) {
		sgbDeltaChunks = 0;
		sgbRecvCmd = CMD_DLEVEL_END;
		gbDeltaSender = MyPlayerId;
		nthread_set_turn_upper_bit();
	}
	if (sgbDeltaChunks == MAX_CHUNKS - 1) {
		sgbDeltaChunks = MAX_CHUNKS;
		return 99;
	}
	return 100 * sgbDeltaChunks / MAX_CHUNKS;
}

byte *DeltaExportItem(byte *dst, const TCmdPItem *src)
{
	for (int i = 0; i < MAXITEMS; i++, src++) {
		if (src->bCmd == CMD_INVALID) {
			*dst++ = byte { 0xFF };
		} else {
			memcpy(dst, src, sizeof(TCmdPItem));
			dst += sizeof(TCmdPItem);
		}
	}

	return dst;
}

size_t DeltaImportItem(const byte *src, TCmdPItem *dst)
{
	size_t size = 0;
	for (int i = 0; i < MAXITEMS; i++, dst++) {
		if (src[size] == byte { 0xFF }) {
			memset(dst, 0xFF, sizeof(TCmdPItem));
			size++;
		} else {
			memcpy(dst, &src[size], sizeof(TCmdPItem));
			size += sizeof(TCmdPItem);
		}
	}

	return size;
}

byte *DeltaExportObject(byte *dst, const DObjectStr *src)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return dst + sizeof(DObjectStr) * MAXOBJECTS;
}

size_t DeltaImportObject(const byte *src, DObjectStr *dst)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return sizeof(DObjectStr) * MAXOBJECTS;
}

byte *DeltaExportMonster(byte *dst, const DMonsterStr *src)
{
	for (int i = 0; i < MAXMONSTERS; i++, src++) {
		if (src->_mx == 0xFF) {
			*dst++ = byte { 0xFF };
		} else {
			memcpy(dst, src, sizeof(DMonsterStr));
			dst += sizeof(DMonsterStr);
		}
	}

	return dst;
}

void DeltaImportMonster(const byte *src, DMonsterStr *dst)
{
	size_t size = 0;
	for (int i = 0; i < MAXMONSTERS; i++, dst++) {
		if (src[size] == byte { 0xFF }) {
			memset(dst, 0xFF, sizeof(DMonsterStr));
			size++;
		} else {
			memcpy(dst, &src[size], sizeof(DMonsterStr));
			size += sizeof(DMonsterStr);
		}
	}
}

byte *DeltaExportJunk(byte *dst)
{
	for (auto &portal : sgJunk.portal) {
		if (portal.x == 0xFF) {
			*dst++ = byte { 0xFF };
		} else {
			memcpy(dst, &portal, sizeof(DPortal));
			dst += sizeof(DPortal);
		}
	}

	int q = 0;
	for (auto &quest : Quests) {
		if (!QuestsData[quest._qidx].isSinglePlayerOnly) {
			sgJunk.quests[q].qlog = quest._qlog ? 1 : 0;
			sgJunk.quests[q].qstate = quest._qactive;
			sgJunk.quests[q].qvar1 = quest._qvar1;
			memcpy(dst, &sgJunk.quests[q], sizeof(MultiQuests));
			dst += sizeof(MultiQuests);
			q++;
		}
	}

	return dst;
}

void DeltaImportJunk(const byte *src)
{
	for (int i = 0; i < MAXPORTAL; i++) {
		if (*src == byte { 0xFF }) {
			memset(&sgJunk.portal[i], 0xFF, sizeof(DPortal));
			src++;
			SetPortalStats(i, false, 0, 0, 0, DTYPE_TOWN);
		} else {
			memcpy(&sgJunk.portal[i], src, sizeof(DPortal));
			src += sizeof(DPortal);
			SetPortalStats(
			    i,
			    true,
			    sgJunk.portal[i].x,
			    sgJunk.portal[i].y,
			    sgJunk.portal[i].level,
			    (dungeon_type)sgJunk.portal[i].ltype);
		}
	}

	int q = 0;
	for (auto &quest : Quests) {
		if (!QuestsData[quest._qidx].isSinglePlayerOnly) {
			memcpy(&sgJunk.quests[q], src, sizeof(MultiQuests));
			src += sizeof(MultiQuests);
			quest._qlog = sgJunk.quests[q].qlog != 0;
			quest._qactive = sgJunk.quests[q].qstate;
			quest._qvar1 = sgJunk.quests[q].qvar1;
			q++;
		}
	}
}

DWORD CompressData(byte *buffer, byte *end)
{
	DWORD size = end - buffer - 1;
	DWORD pkSize = PkwareCompress(buffer + 1, size);

	*buffer = size != pkSize ? byte { 1 } : byte { 0 };

	return pkSize + 1;
}

void DeltaImportData(_cmd_id cmd, DWORD recvOffset)
{
	if (sgRecvBuf[0] != byte { 0 })
		PkwareDecompress(&sgRecvBuf[1], recvOffset, sizeof(sgRecvBuf) - 1);

	byte *src = &sgRecvBuf[1];
	if (cmd == CMD_DLEVEL_JUNK) {
		DeltaImportJunk(src);
	} else if (cmd >= CMD_DLEVEL_0 && cmd <= CMD_DLEVEL_24) {
		uint8_t i = cmd - CMD_DLEVEL_0;
		src += DeltaImportItem(src, sgLevels[i].item);
		src += DeltaImportObject(src, sgLevels[i].object);
		DeltaImportMonster(src, sgLevels[i].monster);
	} else {
		app_fatal("Unkown network message type: %i", cmd);
	}

	sgbDeltaChunks++;
	sgbDeltaChanged = true;
}

DWORD OnLevelData(int pnum, const TCmd *pCmd)
{
	const auto &message = *reinterpret_cast<const TCmdPlrInfoHdr *>(pCmd);

	if (gbDeltaSender != pnum) {
		if (message.bCmd != CMD_DLEVEL_END && (message.bCmd != CMD_DLEVEL_0 || message.wOffset != 0)) {
			return message.wBytes + sizeof(message);
		}

		gbDeltaSender = pnum;
		sgbRecvCmd = CMD_DLEVEL_END;
	}

	if (sgbRecvCmd == CMD_DLEVEL_END) {
		if (message.bCmd == CMD_DLEVEL_END) {
			sgbDeltaChunks = MAX_CHUNKS - 1;
			return message.wBytes + sizeof(message);
		}
		if (message.bCmd != CMD_DLEVEL_0 || message.wOffset != 0) {
			return message.wBytes + sizeof(message);
		}

		sgdwRecvOffset = 0;
		sgbRecvCmd = message.bCmd;
	} else if (sgbRecvCmd != message.bCmd) {
		DeltaImportData(sgbRecvCmd, sgdwRecvOffset);
		if (message.bCmd == CMD_DLEVEL_END) {
			sgbDeltaChunks = MAX_CHUNKS - 1;
			sgbRecvCmd = CMD_DLEVEL_END;
			return message.wBytes + sizeof(message);
		}
		sgdwRecvOffset = 0;
		sgbRecvCmd = message.bCmd;
	}

	assert(message.wOffset == sgdwRecvOffset);
	memcpy(&sgRecvBuf[message.wOffset], &message + 1, message.wBytes);
	sgdwRecvOffset += message.wBytes;
	return message.wBytes + sizeof(message);
}

void DeltaSyncGolem(const TCmdGolem &message, int pnum, uint8_t level)
{
	if (!gbIsMultiplayer)
		return;

	sgbDeltaChanged = true;
	DMonsterStr &monster = sgLevels[level].monster[pnum];
	monster._mx = message._mx;
	monster._my = message._my;
	monster._mactive = UINT8_MAX;
	monster._menemy = message._menemy;
	monster._mdir = message._mdir;
	monster._mhitpoints = message._mhitpoints;
}

void DeltaLeaveSync(BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;
	if (currlevel == 0)
		glSeedTbl[0] = AdvanceRndSeed();
	if (currlevel <= 0)
		return;

	for (int i = 0; i < ActiveMonsterCount; i++) {
		int ma = ActiveMonsters[i];
		auto &monster = Monsters[ma];
		if (monster._mhitpoints == 0)
			continue;
		sgbDeltaChanged = true;
		DMonsterStr &delta = sgLevels[bLevel].monster[ma];
		delta._mx = monster.position.tile.x;
		delta._my = monster.position.tile.y;
		delta._mdir = monster._mdir;
		delta._menemy = encode_enemy(monster);
		delta._mhitpoints = monster._mhitpoints;
		delta._mactive = monster._msquelch;
		delta.mWhoHit = monster.mWhoHit;
	}
	memcpy(&sgLocals[bLevel].automapsv, AutomapView, sizeof(AutomapView));
}

void DeltaSyncObject(int oi, _cmd_id bCmd, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;

	sgbDeltaChanged = true;
	sgLevels[bLevel].object[oi].bCmd = bCmd;
}

bool DeltaGetItem(const TCmdGItem &message, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return true;

	for (TCmdPItem &item : sgLevels[bLevel].item) {
		if (item.bCmd == CMD_INVALID || item.wIndx != message.wIndx || item.wCI != message.wCI || item.dwSeed != message.dwSeed)
			continue;

		if (item.bCmd == CMD_WALKXY) {
			return true;
		}
		if (item.bCmd == CMD_STAND) {
			sgbDeltaChanged = true;
			item.bCmd = CMD_WALKXY;
			return true;
		}
		if (item.bCmd == CMD_ACK_PLRINFO) {
			sgbDeltaChanged = true;
			item.bCmd = CMD_INVALID;
			return true;
		}

		app_fatal("delta:1");
	}

	if ((message.wCI & CF_PREGEN) == 0)
		return false;

	for (TCmdPItem &item : sgLevels[bLevel].item) {
		if (item.bCmd == CMD_INVALID) {
			sgbDeltaChanged = true;
			item.bCmd = CMD_WALKXY;
			item.x = message.x;
			item.y = message.y;
			item.wIndx = message.wIndx;
			item.wCI = message.wCI;
			item.dwSeed = message.dwSeed;
			item.bId = message.bId;
			item.bDur = message.bDur;
			item.bMDur = message.bMDur;
			item.bCh = message.bCh;
			item.bMCh = message.bMCh;
			item.wValue = message.wValue;
			item.dwBuff = message.dwBuff;
			item.wToHit = message.wToHit;
			item.wMaxDam = message.wMaxDam;
			item.bMinStr = message.bMinStr;
			item.bMinMag = message.bMinMag;
			item.bMinDex = message.bMinDex;
			item.bAC = message.bAC;
			break;
		}
	}
	return true;
}

void DeltaPutItem(const TCmdPItem &message, Point position, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;

	for (const TCmdPItem &item : sgLevels[bLevel].item) {
		if (item.bCmd != CMD_WALKXY
		    && item.bCmd != CMD_INVALID
		    && item.wIndx == message.wIndx
		    && item.wCI == message.wCI
		    && item.dwSeed == message.dwSeed) {
			if (item.bCmd == CMD_ACK_PLRINFO)
				return;
			app_fatal("%s", _("Trying to drop a floor item?"));
		}
	}

	for (TCmdPItem &item : sgLevels[bLevel].item) {
		if (item.bCmd == CMD_INVALID) {
			sgbDeltaChanged = true;
			memcpy(&item, &message, sizeof(TCmdPItem));
			item.bCmd = CMD_ACK_PLRINFO;
			item.x = position.x;
			item.y = position.y;
			return;
		}
	}
}

bool IOwnLevel(int nReqLevel)
{
	int i;

	for (i = 0; i < MAX_PLRS; i++) {
		if (!Players[i].plractive)
			continue;
		if (Players[i]._pLvlChanging)
			continue;
		if (Players[i].plrlevel != nReqLevel)
			continue;
		if (i == MyPlayerId && gbBufferMsgs != 0)
			continue;
		break;
	}
	return i == MyPlayerId;
}

void DeltaOpenPortal(int pnum, Point position, uint8_t bLevel, dungeon_type bLType, bool bSetLvl)
{
	sgbDeltaChanged = true;
	sgJunk.portal[pnum].x = position.x;
	sgJunk.portal[pnum].y = position.y;
	sgJunk.portal[pnum].level = bLevel;
	sgJunk.portal[pnum].ltype = bLType;
	sgJunk.portal[pnum].setlvl = bSetLvl ? 1 : 0;
}

void CheckUpdatePlayer(int pnum)
{
	if (gbIsMultiplayer && pnum == MyPlayerId)
		pfile_update(true);
}

void NetSendCmdGItem2(bool usonly, _cmd_id bCmd, uint8_t mast, uint8_t pnum, const TCmdGItem &item)
{
	TCmdGItem cmd;

	memcpy(&cmd, &item, sizeof(cmd));
	cmd.bPnum = pnum;
	cmd.bCmd = bCmd;
	cmd.bMaster = mast;

	if (!usonly) {
		cmd.dwTime = 0;
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
		return;
	}

	int ticks = SDL_GetTicks();
	if (cmd.dwTime == 0) {
		cmd.dwTime = ticks;
	} else if (ticks - cmd.dwTime > 5000) {
		return;
	}

	tmsg_add((byte *)&cmd, sizeof(cmd));
}

bool NetSendCmdReq2(_cmd_id bCmd, uint8_t mast, uint8_t pnum, const TCmdGItem &item)
{
	TCmdGItem cmd;

	memcpy(&cmd, &item, sizeof(cmd));
	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;

	int ticks = SDL_GetTicks();
	if (cmd.dwTime == 0)
		cmd.dwTime = ticks;
	else if (ticks - cmd.dwTime > 5000)
		return false;

	tmsg_add((byte *)&cmd, sizeof(cmd));

	return true;
}

void NetSendCmdExtra(const TCmdGItem &item)
{
	TCmdGItem cmd;

	memcpy(&cmd, &item, sizeof(cmd));
	cmd.dwTime = 0;
	cmd.bCmd = CMD_ITEMEXTRA;
	NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

DWORD OnWalk(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLoc *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position)) {
		ClrPlrPath(player);
		MakePlrPath(player, position, true);
		player.destAction = ACTION_NONE;
	}

	return sizeof(message);
}

DWORD OnAddStrength(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 <= 256)
		ModifyPlrStr(pnum, message.wParam1);

	return sizeof(message);
}

DWORD OnAddMagic(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 <= 256)
		ModifyPlrMag(pnum, message.wParam1);

	return sizeof(message);
}

DWORD OnAddDexterity(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 <= 256)
		ModifyPlrDex(pnum, message.wParam1);

	return sizeof(message);
}

DWORD OnAddVitality(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 <= 256)
		ModifyPlrVit(pnum, message.wParam1);

	return sizeof(message);
}

DWORD OnGotoGetItem(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam1 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position) && message.wParam1 < MAXITEMS + 1) {
		MakePlrPath(player, position, false);
		player.destAction = ACTION_PICKUPITEM;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

bool IsGItemValid(const TCmdGItem &message)
{
	if (message.bMaster >= MAX_PLRS)
		return false;
	if (message.bPnum >= MAX_PLRS)
		return false;
	if (message.bCursitem >= MAXITEMS + 1)
		return false;
	if (message.bLevel >= NUMLEVELS)
		return false;

	if (!InDungeonBounds({ message.x, message.y }))
		return false;

	return IsItemAvailable(message.wIndx);
}

bool IsPItemValid(const TCmdPItem &message)
{
	const Point position { message.x, message.y };

	if (!InDungeonBounds(position))
		return false;

	return IsItemAvailable(message.wIndx);
}

DWORD OnRequestGetItem(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdGItem *>(pCmd);

	if (gbBufferMsgs != 1 && IOwnLevel(player.plrlevel) && IsGItemValid(message)) {
		const Point position { message.x, message.y };
		if (GetItemRecord(message.dwSeed, message.wCI, message.wIndx)) {
			int ii = -1;
			if (InDungeonBounds(position)) {
				ii = abs(dItem[position.x][position.y]) - 1;
				if (ii >= 0 && !Items[ii].KeyAttributesMatch(message.dwSeed, static_cast<_item_indexes>(message.wIndx), message.wCI)) {
					ii = -1;
				}
			}

			if (ii == -1) {
				// No item at the target position or the key attributes don't match, so try find a matching item.
				int activeItemIndex = FindGetItem(message.dwSeed, message.wIndx, message.wCI);
				if (activeItemIndex != -1) {
					ii = ActiveItems[activeItemIndex];
				}
			}

			if (ii != -1) {
				NetSendCmdGItem2(false, CMD_GETITEM, MyPlayerId, message.bPnum, message);
				if (message.bPnum != MyPlayerId)
					SyncGetItem(position, message.dwSeed, message.wIndx, message.wCI);
				else
					InvGetItem(MyPlayerId, ii);
				SetItemRecord(message.dwSeed, message.wCI, message.wIndx);
			} else if (!NetSendCmdReq2(CMD_REQUESTGITEM, MyPlayerId, message.bPnum, message)) {
				NetSendCmdExtra(message);
			}
		}
	}

	return sizeof(message);
}

DWORD OnGetItem(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdGItem *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (IsGItemValid(message)) {
		const Point position { message.x, message.y };
		if (DeltaGetItem(message, message.bLevel)) {
			if ((currlevel == message.bLevel || message.bPnum == MyPlayerId) && message.bMaster != MyPlayerId) {
				if (message.bPnum == MyPlayerId) {
					if (currlevel != message.bLevel) {
						auto &player = Players[MyPlayerId];
						int ii = SyncPutItem(player, player.position.tile, message.wIndx, message.wCI, message.dwSeed, message.bId, message.bDur, message.bMDur, message.bCh, message.bMCh, message.wValue, message.dwBuff, message.wToHit, message.wMaxDam, message.bMinStr, message.bMinMag, message.bMinDex, message.bAC);
						if (ii != -1)
							InvGetItem(MyPlayerId, ii);
					} else {
						int activeItemIndex = FindGetItem(message.dwSeed, message.wIndx, message.wCI);
						InvGetItem(MyPlayerId, ActiveItems[activeItemIndex]);
					}
				} else {
					SyncGetItem(position, message.dwSeed, message.wIndx, message.wCI);
				}
			}
		} else {
			NetSendCmdGItem2(true, CMD_GETITEM, message.bMaster, message.bPnum, message);
		}
	}

	return sizeof(message);
}

DWORD OnGotoAutoGetItem(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam1 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position) && message.wParam1 < MAXITEMS + 1) {
		MakePlrPath(player, position, false);
		player.destAction = ACTION_PICKUPAITEM;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnRequestAutoGetItem(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdGItem *>(pCmd);

	if (gbBufferMsgs != 1 && IOwnLevel(player.plrlevel) && IsGItemValid(message)) {
		const Point position { message.x, message.y };
		if (GetItemRecord(message.dwSeed, message.wCI, message.wIndx)) {
			if (FindGetItem(message.dwSeed, message.wIndx, message.wCI) != -1) {
				NetSendCmdGItem2(false, CMD_AGETITEM, MyPlayerId, message.bPnum, message);
				if (message.bPnum != MyPlayerId)
					SyncGetItem(position, message.dwSeed, message.wIndx, message.wCI);
				else
					AutoGetItem(MyPlayerId, &Items[message.bCursitem], message.bCursitem);
				SetItemRecord(message.dwSeed, message.wCI, message.wIndx);
			} else if (!NetSendCmdReq2(CMD_REQUESTAGITEM, MyPlayerId, message.bPnum, message)) {
				NetSendCmdExtra(message);
			}
		}
	}

	return sizeof(message);
}

DWORD OnAutoGetItem(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdGItem *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (IsGItemValid(message)) {
		const Point position { message.x, message.y };
		if (DeltaGetItem(message, message.bLevel)) {
			if ((currlevel == message.bLevel || message.bPnum == MyPlayerId) && message.bMaster != MyPlayerId) {
				if (message.bPnum == MyPlayerId) {
					if (currlevel != message.bLevel) {
						auto &player = Players[MyPlayerId];
						int ii = SyncPutItem(player, player.position.tile, message.wIndx, message.wCI, message.dwSeed, message.bId, message.bDur, message.bMDur, message.bCh, message.bMCh, message.wValue, message.dwBuff, message.wToHit, message.wMaxDam, message.bMinStr, message.bMinMag, message.bMinDex, message.bAC);
						if (ii != -1)
							AutoGetItem(MyPlayerId, &Items[ii], ii);
					} else {
						AutoGetItem(MyPlayerId, &Items[message.bCursitem], message.bCursitem);
					}
				} else {
					SyncGetItem(position, message.dwSeed, message.wIndx, message.wCI);
				}
			}
		} else {
			NetSendCmdGItem2(true, CMD_AGETITEM, message.bMaster, message.bPnum, message);
		}
	}

	return sizeof(message);
}

DWORD OnItemExtra(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdGItem *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (IsGItemValid(message)) {
		DeltaGetItem(message, message.bLevel);
		if (currlevel == Players[pnum].plrlevel) {
			const Point position { message.x, message.y };
			SyncGetItem(position, message.dwSeed, message.wIndx, message.wCI);
		}
	}

	return sizeof(message);
}

DWORD OnPutItem(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdPItem *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (IsPItemValid(message)) {
		const Point position { message.x, message.y };
		if (currlevel == Players[pnum].plrlevel) {
			int ii;
			if (pnum == MyPlayerId)
				ii = InvPutItem(Players[pnum], position);
			else
				ii = SyncPutItem(Players[pnum], position, message.wIndx, message.wCI, message.dwSeed, message.bId, message.bDur, message.bMDur, message.bCh, message.bMCh, message.wValue, message.dwBuff, message.wToHit, message.wMaxDam, message.bMinStr, message.bMinMag, message.bMinDex, message.bAC);
			if (ii != -1) {
				PutItemRecord(message.dwSeed, message.wCI, message.wIndx);
				DeltaPutItem(message, Items[ii].position, Players[pnum].plrlevel);
				CheckUpdatePlayer(pnum);
			}
			return sizeof(message);
		} else {
			PutItemRecord(message.dwSeed, message.wCI, message.wIndx);
			DeltaPutItem(message, position, Players[pnum].plrlevel);
			CheckUpdatePlayer(pnum);
		}
	}

	return sizeof(message);
}

DWORD OnSyncPutItem(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdPItem *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (IsPItemValid(message)) {
		const Point position { message.x, message.y };
		if (currlevel == Players[pnum].plrlevel) {
			int ii = SyncPutItem(Players[pnum], position, message.wIndx, message.wCI, message.dwSeed, message.bId, message.bDur, message.bMDur, message.bCh, message.bMCh, message.wValue, message.dwBuff, message.wToHit, message.wMaxDam, message.bMinStr, message.bMinMag, message.bMinDex, message.bAC);
			if (ii != -1) {
				PutItemRecord(message.dwSeed, message.wCI, message.wIndx);
				DeltaPutItem(message, Items[ii].position, Players[pnum].plrlevel);
				CheckUpdatePlayer(pnum);
			}
			return sizeof(message);
		} else {
			PutItemRecord(message.dwSeed, message.wCI, message.wIndx);
			DeltaPutItem(message, position, Players[pnum].plrlevel);
			CheckUpdatePlayer(pnum);
		}
	}

	return sizeof(message);
}

DWORD OnRespawnItem(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdPItem *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (IsPItemValid(message)) {
		const Point position { message.x, message.y };
		auto &player = Players[pnum];
		int playerLevel = player.plrlevel;
		if (currlevel == playerLevel && pnum != MyPlayerId) {
			SyncPutItem(player, position, message.wIndx, message.wCI, message.dwSeed, message.bId, message.bDur, message.bMDur, message.bCh, message.bMCh, message.wValue, message.dwBuff, message.wToHit, message.wMaxDam, message.bMinStr, message.bMinMag, message.bMinDex, message.bAC);
		}
		PutItemRecord(message.dwSeed, message.wCI, message.wIndx);
		DeltaPutItem(message, position, playerLevel);
	}

	return sizeof(message);
}

DWORD OnAttackTile(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLoc *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position)) {
		MakePlrPath(player, position, false);
		player.destAction = ACTION_ATTACK;
		player.destParam1 = position.x;
		player.destParam2 = position.y;
	}

	return sizeof(message);
}

DWORD OnStandingAttackTile(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLoc *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position)) {
		ClrPlrPath(player);
		player.destAction = ACTION_ATTACK;
		player.destParam1 = position.x;
		player.destParam2 = position.y;
	}

	return sizeof(message);
}

DWORD OnRangedAttackTile(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLoc *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position)) {
		ClrPlrPath(player);
		player.destAction = ACTION_RATTACK;
		player.destParam1 = position.x;
		player.destParam2 = position.y;
	}

	return sizeof(message);
}

DWORD OnSpellWall(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam4 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs == 1)
		return sizeof(message);
	if (currlevel != player.plrlevel)
		return sizeof(message);
	if (!InDungeonBounds(position))
		return sizeof(message);
	if (message.wParam1 > SPL_LAST)
		return sizeof(message);
	if (message.wParam2 > RSPLTYPE_INVALID)
		return sizeof(message);

	auto spell = static_cast<spell_id>(message.wParam1);
	if (currlevel == 0 && !spelldata[spell].sTownSpell) {
		LogError(_("{:s} has cast an illegal spell."), player._pName);
		return sizeof(message);
	}

	ClrPlrPath(player);
	player.destAction = ACTION_SPELLWALL;
	player.destParam1 = position.x;
	player.destParam2 = position.y;
	player.destParam3 = message.wParam3;
	player.destParam4 = message.wParam4;
	player._pSpell = spell;
	player._pSplType = static_cast<spell_type>(message.wParam2);
	player._pSplFrom = 0;

	return sizeof(message);
}

DWORD OnSpellTile(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam3 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs == 1)
		return sizeof(message);
	if (currlevel != player.plrlevel)
		return sizeof(message);
	if (!InDungeonBounds(position))
		return sizeof(message);
	if (message.wParam1 > SPL_LAST)
		return sizeof(message);
	if (message.wParam2 > RSPLTYPE_INVALID)
		return sizeof(message);

	auto spell = static_cast<spell_id>(message.wParam1);
	if (currlevel == 0 && !spelldata[spell].sTownSpell) {
		LogError(_("{:s} has cast an illegal spell."), player._pName);
		return sizeof(message);
	}

	ClrPlrPath(player);
	player.destAction = ACTION_SPELL;
	player.destParam1 = position.x;
	player.destParam2 = position.y;
	player.destParam3 = message.wParam3;
	player._pSpell = spell;
	player._pSplType = static_cast<spell_type>(message.wParam2);

	return sizeof(message);
}

DWORD OnTargetSpellTile(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam2 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs == 1)
		return sizeof(message);
	if (currlevel != player.plrlevel)
		return sizeof(message);
	if (!InDungeonBounds(position))
		return sizeof(message);
	if (message.wParam1 > SPL_LAST)
		return sizeof(message);

	auto spell = static_cast<spell_id>(message.wParam1);
	if (currlevel == 0 && !spelldata[spell].sTownSpell) {
		LogError(_("{:s} has cast an illegal spell."), player._pName);
		return sizeof(message);
	}

	ClrPlrPath(player);
	player.destAction = ACTION_SPELL;
	player.destParam1 = position.x;
	player.destParam2 = position.y;
	player.destParam3 = message.wParam2;
	player._pSpell = spell;
	player._pSplType = RSPLTYPE_INVALID;
	player._pSplFrom = 2;

	return sizeof(message);
}

DWORD OnOperateObjectTile(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam1 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position) && message.wParam1 < MAXOBJECTS) {
		MakePlrPath(player, position, !Objects[message.wParam1]._oSolidFlag && !Objects[message.wParam1]._oDoorFlag);
		player.destAction = ACTION_OPERATE;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnDisarm(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam1 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position) && message.wParam1 < MAXOBJECTS) {
		MakePlrPath(player, position, !Objects[message.wParam1]._oSolidFlag && !Objects[message.wParam1]._oDoorFlag);
		player.destAction = ACTION_DISARM;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnOperateObjectTelekinesis(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && message.wParam1 < MAXOBJECTS) {
		player.destAction = ACTION_OPERATETK;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnAttackMonster(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && message.wParam1 < MAXMONSTERS) {
		Point position = Monsters[message.wParam1].position.future;
		if (player.position.tile.WalkingDistance(position) > 1)
			MakePlrPath(player, position, false);
		player.destAction = ACTION_ATTACKMON;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnAttackPlayer(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && message.wParam1 < MAX_PLRS) {
		MakePlrPath(player, Players[message.wParam1].position.future, false);
		player.destAction = ACTION_ATTACKPLR;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnRangedAttackMonster(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && message.wParam1 < MAXMONSTERS) {
		ClrPlrPath(player);
		player.destAction = ACTION_RATTACKMON;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnRangedAttackPlayer(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && message.wParam1 < MAX_PLRS) {
		ClrPlrPath(player);
		player.destAction = ACTION_RATTACKPLR;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnSpellMonster(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam4 *>(pCmd);

	if (gbBufferMsgs == 1)
		return sizeof(message);
	if (currlevel != player.plrlevel)
		return sizeof(message);
	if (message.wParam1 >= MAXMONSTERS)
		return sizeof(message);
	if (message.wParam2 > SPL_LAST)
		return sizeof(message);
	if (message.wParam3 > RSPLTYPE_INVALID)
		return sizeof(message);

	auto spell = static_cast<spell_id>(message.wParam2);
	if (currlevel == 0 && !spelldata[spell].sTownSpell) {
		LogError(_("{:s} has cast an illegal spell."), player._pName);
		return sizeof(message);
	}

	ClrPlrPath(player);
	player.destAction = ACTION_SPELLMON;
	player.destParam1 = message.wParam1;
	player.destParam2 = message.wParam4;
	player._pSpell = spell;
	player._pSplType = static_cast<spell_type>(message.wParam3);
	player._pSplFrom = 0;

	return sizeof(message);
}

DWORD OnSpellPlayer(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam4 *>(pCmd);

	if (gbBufferMsgs == 1)
		return sizeof(message);
	if (currlevel != player.plrlevel)
		return sizeof(message);
	if (message.wParam1 >= MAX_PLRS)
		return sizeof(message);
	if (message.wParam2 > SPL_LAST)
		return sizeof(message);
	if (message.wParam3 > RSPLTYPE_INVALID)
		return sizeof(message);

	auto spell = static_cast<spell_id>(message.wParam2);
	if (currlevel == 0 && !spelldata[spell].sTownSpell) {
		LogError(_("{:s} has cast an illegal spell."), player._pName);
		return sizeof(message);
	}

	ClrPlrPath(player);
	player.destAction = ACTION_SPELLPLR;
	player.destParam1 = message.wParam1;
	player.destParam2 = message.wParam4;
	player._pSpell = spell;
	player._pSplType = static_cast<spell_type>(message.wParam3);
	player._pSplFrom = 0;

	return sizeof(message);
}

DWORD OnTargetSpellMonster(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam3 *>(pCmd);

	if (gbBufferMsgs == 1)
		return sizeof(message);
	if (currlevel != player.plrlevel)
		return sizeof(message);
	if (message.wParam1 >= MAXMONSTERS)
		return sizeof(message);
	if (message.wParam2 > SPL_LAST)
		return sizeof(message);
	if (message.wParam3 > RSPLTYPE_INVALID)
		return sizeof(message);

	auto spell = static_cast<spell_id>(message.wParam2);
	if (currlevel == 0 && !spelldata[spell].sTownSpell) {
		LogError(_("{:s} has cast an illegal spell."), player._pName);
		return sizeof(message);
	}

	ClrPlrPath(player);
	player.destAction = ACTION_SPELLMON;
	player.destParam1 = message.wParam1;
	player.destParam2 = message.wParam3;
	player._pSpell = spell;
	player._pSplType = RSPLTYPE_INVALID;
	player._pSplFrom = 2;

	return sizeof(message);
}

DWORD OnTargetSpellPlayer(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam3 *>(pCmd);

	if (gbBufferMsgs == 1)
		return sizeof(message);
	if (currlevel != player.plrlevel)
		return sizeof(message);
	if (message.wParam1 >= MAX_PLRS)
		return sizeof(message);
	if (message.wParam2 > SPL_LAST)
		return sizeof(message);

	auto spell = static_cast<spell_id>(message.wParam2);
	if (currlevel == 0 && !spelldata[spell].sTownSpell) {
		LogError(_("{:s} has cast an illegal spell."), player._pName);
		return sizeof(message);
	}

	ClrPlrPath(player);
	player.destAction = ACTION_SPELLPLR;
	player.destParam1 = message.wParam1;
	player.destParam2 = message.wParam3;
	player._pSpell = spell;
	player._pSplType = RSPLTYPE_INVALID;
	player._pSplFrom = 2;

	return sizeof(message);
}

DWORD OnKnockback(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs != 1 && currlevel == Players[pnum].plrlevel && message.wParam1 < MAXMONSTERS) {
		M_GetKnockback(message.wParam1);
		M_StartHit(message.wParam1, pnum, 0);
	}

	return sizeof(message);
}

DWORD OnResurrect(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (message.wParam1 < MAX_PLRS) {
		DoResurrect(pnum, message.wParam1);
		CheckUpdatePlayer(pnum);
	}

	return sizeof(message);
}

DWORD OnHealOther(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs != 1 && currlevel == Players[pnum].plrlevel && message.wParam1 < MAX_PLRS)
		DoHealOther(pnum, message.wParam1);

	return sizeof(message);
}

DWORD OnTalkXY(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam1 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel && InDungeonBounds(position) && message.wParam1 < NUM_TOWNERS) {
		MakePlrPath(player, position, false);
		player.destAction = ACTION_TALK;
		player.destParam1 = message.wParam1;
	}

	return sizeof(message);
}

DWORD OnNewLevel(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam2 *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (pnum != MyPlayerId) {
		if (message.wParam1 < WM_FIRST || message.wParam1 > WM_LAST)
			return sizeof(message);

		auto mode = static_cast<interface_mode>(message.wParam1);

		int levelId = message.wParam2;
		if (mode == WM_DIABSETLVL) {
			if (levelId > SL_LAST)
				return sizeof(message);
		} else {
			if (levelId >= NUMLEVELS)
				return sizeof(message);
		}

		StartNewLvl(pnum, mode, levelId);
	}

	return sizeof(message);
}

DWORD OnWarp(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (message.wParam1 < MAXPORTAL) {
		StartWarpLvl(pnum, message.wParam1);
	}

	return sizeof(message);
}

DWORD OnMonstDeath(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam1 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (pnum != MyPlayerId && InDungeonBounds(position) && message.wParam1 < MAXMONSTERS) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			M_SyncStartKill(message.wParam1, position, pnum);
		delta_kill_monster(message.wParam1, position, playerLevel);
	}

	return sizeof(message);
}

DWORD OnKillGolem(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam1 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (pnum != MyPlayerId && InDungeonBounds(position) && message.wParam1 < NUMLEVELS) {
		if (currlevel == message.wParam1)
			M_SyncStartKill(pnum, position, pnum);
		delta_kill_monster(pnum, position, message.wParam1);
	}

	return sizeof(message);
}

DWORD OnAwakeGolem(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdGolem *>(pCmd);
	const Point position { message._mx, message._my };

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (InDungeonBounds(position)) {
		if (currlevel != Players[pnum].plrlevel) {
			DeltaSyncGolem(message, pnum, message._currlevel);
		} else if (pnum != MyPlayerId) {
			// Check if this player already has an active golem
			for (int i = 0; i < ActiveMissileCount; i++) {
				int mi = ActiveMissiles[i];
				auto &missile = Missiles[mi];
				if (missile._mitype == MIS_GOLEM && missile._misource == pnum) {
					return sizeof(message);
				}
			}

			AddMissile(Players[pnum].position.tile, position, message._mdir, MIS_GOLEM, TARGET_MONSTERS, pnum, 0, 1);
		}
	}

	return sizeof(message);
}

DWORD OnMonstDamage(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdMonDamage *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (pnum != MyPlayerId) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel && message.wMon < MAXMONSTERS) {
			auto &monster = Monsters[message.wMon];
			monster.mWhoHit |= 1 << pnum;
			if (monster._mhitpoints > 0) {
				monster._mhitpoints -= message.dwDam;
				if ((monster._mhitpoints >> 6) < 1)
					monster._mhitpoints = 1 << 6;
				delta_monster_hp(message.wMon, monster._mhitpoints, playerLevel);
			}
		}
	}

	return sizeof(message);
}

DWORD OnPlayerDeath(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (pnum != MyPlayerId)
		StartPlayerKill(pnum, message.wParam1);
	else
		CheckUpdatePlayer(pnum);

	return sizeof(message);
}

DWORD OnPlayerDamage(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdDamage *>(pCmd);

	if (message.bPlr == MyPlayerId && currlevel != 0 && gbBufferMsgs != 1) {
		if (currlevel == player.plrlevel && message.dwDam <= 192000 && Players[message.bPlr]._pHitPoints >> 6 > 0) {
			ApplyPlrDamage(message.bPlr, 0, 0, message.dwDam, 1);
		}
	}

	return sizeof(message);
}

DWORD OnOpenDoor(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (message.wParam1 < MAXOBJECTS) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncOpObject(pnum, CMD_OPENDOOR, message.wParam1);
		DeltaSyncObject(message.wParam1, CMD_OPENDOOR, playerLevel);
	}

	return sizeof(message);
}

DWORD OnCloseDoor(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (message.wParam1 < MAXOBJECTS) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncOpObject(pnum, CMD_CLOSEDOOR, message.wParam1);
		DeltaSyncObject(message.wParam1, CMD_CLOSEDOOR, playerLevel);
	}

	return sizeof(message);
}

DWORD OnOperateObject(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (message.wParam1 < MAXOBJECTS) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncOpObject(pnum, CMD_OPERATEOBJ, message.wParam1);
		DeltaSyncObject(message.wParam1, CMD_OPERATEOBJ, playerLevel);
	}

	return sizeof(message);
}

DWORD OnPlayerOperateObject(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam2 *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (message.wParam1 < MAX_PLRS && message.wParam2 < MAXOBJECTS) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncOpObject(message.wParam1, CMD_PLROPOBJ, message.wParam2);
		DeltaSyncObject(message.wParam2, CMD_PLROPOBJ, playerLevel);
	}

	return sizeof(message);
}

DWORD OnBreakObject(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam2 *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (message.wParam1 < MAX_PLRS && message.wParam2 < MAXOBJECTS) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel) {
			SyncBreakObj(message.wParam1, Objects[message.wParam2]);
		}
		DeltaSyncObject(message.wParam2, CMD_BREAKOBJ, playerLevel);
	}

	return sizeof(message);
}

DWORD OnChangePlayerItems(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdChItem *>(pCmd);
	auto &player = Players[pnum];

	if (message.bLoc >= NUM_INVLOC)
		return sizeof(message);

	auto bodyLocation = static_cast<inv_body_loc>(message.bLoc);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (pnum != MyPlayerId && message.wIndx <= IDI_LAST) {
		CheckInvSwap(player, bodyLocation, message.wIndx, message.wCI, message.dwSeed, message.bId != 0, message.dwBuff);
	}

	player.ReadySpellFromEquipment(bodyLocation);

	return sizeof(message);
}

DWORD OnDeletePlayerItems(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdDelItem *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (pnum != MyPlayerId && message.bLoc < NUM_INVLOC)
		inv_update_rem_item(Players[pnum], static_cast<inv_body_loc>(message.bLoc));

	return sizeof(message);
}

DWORD OnPlayerLevel(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 < MAXCHARLEVEL && pnum != MyPlayerId)
		Players[pnum]._pLevel = message.wParam1;

	return sizeof(message);
}

DWORD OnDropItem(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdPItem *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (IsPItemValid(message))
		DeltaPutItem(message, { message.x, message.y }, Players[pnum].plrlevel);

	return sizeof(message);
}

DWORD OnSendPlayerInfo(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdPlrInfoHdr *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, message.wBytes + sizeof(message));
	else
		recv_plrinfo(pnum, message, message.bCmd == CMD_ACK_PLRINFO);

	return message.wBytes + sizeof(message);
}

DWORD OnPlayerJoinLevel(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam1 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
		return sizeof(message);
	}

	int playerLevel = message.wParam1;
	if (playerLevel > NUMLEVELS || !InDungeonBounds(position)) {
		return sizeof(message);
	}

	auto &player = Players[pnum];

	player._pLvlChanging = false;
	if (player._pName[0] != '\0' && !player.plractive) {
		ResetPlayerGFX(player);
		player.plractive = true;
		gbActivePlayers++;
		EventPlrMsg(fmt::format(_("Player '{:s}' (level {:d}) just joined the game"), player._pName, player._pLevel).c_str());
	}

	if (player.plractive && MyPlayerId != pnum) {
		player.position.tile = position;
		player.plrlevel = playerLevel;
		ResetPlayerGFX(player);
		if (currlevel == player.plrlevel) {
			SyncInitPlr(pnum);
			if ((player._pHitPoints >> 6) > 0) {
				StartStand(pnum, Direction::South);
			} else {
				player._pgfxnum = 0;
				player._pmode = PM_DEATH;
				NewPlrAnim(player, player_graphic::Death, Direction::South, player._pDFrames, 1);
				player.AnimInfo.CurrentFrame = player.AnimInfo.NumberOfFrames - 1;
				dFlags[player.position.tile.x][player.position.tile.y] |= DungeonFlag::DeadPlayer;
			}

			player._pvid = AddVision(player.position.tile, player._pLightRad, pnum == MyPlayerId);
		}
	}

	return sizeof(message);
}

DWORD OnActivatePortal(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdLocParam3 *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else if (InDungeonBounds(position) && message.wParam1 < NUMLEVELS && message.wParam2 <= DTYPE_LAST) {
		int level = message.wParam1;
		auto dungeonType = static_cast<dungeon_type>(message.wParam2);
		bool isSetLevel = message.wParam3 != 0;

		ActivatePortal(pnum, position, level, dungeonType, isSetLevel);
		if (pnum != MyPlayerId) {
			if (currlevel == 0) {
				AddInTownPortal(pnum);
			} else if (currlevel == Players[pnum].plrlevel) {
				bool addPortal = true;
				for (int i = 0; i < ActiveMissileCount; i++) {
					int mi = ActiveMissiles[i];
					auto &missile = Missiles[mi];
					if (missile._mitype == MIS_TOWN && missile._misource == pnum) {
						addPortal = false;
						break;
					}
				}
				if (addPortal) {
					AddWarpMissile(pnum, position);
				}
			} else {
				RemovePortalMissile(pnum);
			}
		}
		DeltaOpenPortal(pnum, position, level, dungeonType, isSetLevel);
	}

	return sizeof(message);
}

DWORD OnDeactivatePortal(const TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs == 1) {
		SendPacket(pnum, pCmd, sizeof(*pCmd));
	} else {
		if (PortalOnLevel(pnum))
			RemovePortalMissile(pnum);
		DeactivatePortal(pnum);
		delta_close_portal(pnum);
	}

	return sizeof(*pCmd);
}

DWORD OnRestartTown(const TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs == 1) {
		SendPacket(pnum, pCmd, sizeof(*pCmd));
	} else {
		if (pnum == MyPlayerId) {
			MyPlayerIsDead = false;
			gamemenu_off();
		}
		RestartTownLvl(pnum);
	}

	return sizeof(*pCmd);
}

DWORD OnSetStrength(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 <= 750 && pnum != MyPlayerId)
		SetPlrStr(Players[pnum], message.wParam1);

	return sizeof(message);
}

DWORD OnSetDexterity(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 <= 750 && pnum != MyPlayerId)
		SetPlrDex(Players[pnum], message.wParam1);

	return sizeof(message);
}

DWORD OnSetMagic(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 <= 750 && pnum != MyPlayerId)
		SetPlrMag(Players[pnum], message.wParam1);

	return sizeof(message);
}

DWORD OnSetVitality(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, &message, sizeof(message));
	else if (message.wParam1 <= 750 && pnum != MyPlayerId)
		SetPlrVit(Players[pnum], message.wParam1);

	return sizeof(message);
}

DWORD OnString(const TCmd *pCmd, int pnum)
{
	auto *p = (TCmdString *)pCmd;

	int len = strlen(p->str);
	if (gbBufferMsgs == 0)
		SendPlrMsg(pnum, p->str);

	return len + 2; // length of string + nul terminator + sizeof(p->bCmd)
}

DWORD OnSyncQuest(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdQuest *>(pCmd);

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, &message, sizeof(message));
	} else {
		if (pnum != MyPlayerId && message.q < MAXQUESTS && message.qstate <= QUEST_HIVE_DONE)
			SetMultiQuest(message.q, message.qstate, message.qlog != 0, message.qvar1);
		sgbDeltaChanged = true;
	}

	return sizeof(message);
}

DWORD OnCheatExperience(const TCmd *pCmd, int pnum) // NOLINT(misc-unused-parameters)
{
#ifdef _DEBUG
	if (gbBufferMsgs == 1)
		SendPacket(pnum, pCmd, sizeof(*pCmd));
	else if (Players[pnum]._pLevel < MAXCHARLEVEL - 1) {
		Players[pnum]._pExperience = Players[pnum]._pNextExper;
		if (*sgOptions.Gameplay.experienceBar) {
			force_redraw = 255;
		}
		NextPlrLevel(pnum);
	}
#endif
	return sizeof(*pCmd);
}

DWORD OnCheatSpellLevel(const TCmd *pCmd, int pnum) // NOLINT(misc-unused-parameters)
{
#ifdef _DEBUG
	if (gbBufferMsgs == 1) {
		SendPacket(pnum, pCmd, sizeof(*pCmd));
	} else {
		auto &player = Players[pnum];
		player._pSplLvl[player._pRSpell]++;
	}
#endif
	return sizeof(*pCmd);
}

DWORD OnDebug(const TCmd *pCmd)
{
	return sizeof(*pCmd);
}

DWORD OnNova(const TCmd *pCmd, int pnum)
{
	const auto &message = *reinterpret_cast<const TCmdLoc *>(pCmd);
	const Point position { message.x, message.y };

	if (gbBufferMsgs != 1) {
		auto &player = Players[pnum];
		if (currlevel == player.plrlevel && pnum != MyPlayerId && InDungeonBounds(position)) {
			ClrPlrPath(player);
			player._pSpell = SPL_NOVA;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 3;
			player.destAction = ACTION_SPELL;
			player.destParam1 = position.x;
			player.destParam2 = position.y;
		}
	}

	return sizeof(message);
}

DWORD OnSetShield(const TCmd *pCmd, Player &player)
{
	if (gbBufferMsgs != 1)
		player.pManaShield = true;

	return sizeof(*pCmd);
}

DWORD OnRemoveShield(const TCmd *pCmd, Player &player)
{
	if (gbBufferMsgs != 1)
		player.pManaShield = false;

	return sizeof(*pCmd);
}

DWORD OnSetReflect(const TCmd *pCmd, Player &player)
{
	const auto &message = *reinterpret_cast<const TCmdParam1 *>(pCmd);

	if (gbBufferMsgs != 1)
		player.wReflections = message.wParam1;

	return sizeof(message);
}

DWORD OnNakrul(const TCmd *pCmd)
{
	if (gbBufferMsgs != 1) {
		OperateNakrulLever();
		IsUberRoomOpened = true;
		Quests[Q_NAKRUL]._qactive = QUEST_DONE;
		monster_some_crypt();
	}
	return sizeof(*pCmd);
}

DWORD OnOpenHive(const TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1) {
		AddMissile({ 0, 0 }, { 0, 0 }, Direction::South, MIS_HIVEEXP2, TARGET_MONSTERS, pnum, 0, 0);
		TownOpenHive();
		InitTownTriggers();
	}

	return sizeof(*pCmd);
}

DWORD OnOpenCrypt(const TCmd *pCmd)
{
	if (gbBufferMsgs != 1) {
		TownOpenGrave();
		InitTownTriggers();
		if (currlevel == 0)
			PlaySFX(IS_SARC);
	}
	return sizeof(*pCmd);
}

} // namespace

void msg_send_drop_pkt(int pnum, int reason)
{
	TFakeDropPlr cmd;

	cmd.dwReason = reason;
	cmd.bCmd = FAKE_CMD_DROPID;
	cmd.bPlr = pnum;
	SendPacket(pnum, &cmd, sizeof(cmd));
}

bool msg_wait_resync()
{
	bool success;

	GetNextPacket();
	sgbDeltaChunks = 0;
	sgnCurrMegaPlayer = -1;
	sgbRecvCmd = CMD_DLEVEL_END;
	gbBufferMsgs = 1;
	sgdwOwnerWait = SDL_GetTicks();
	success = UiProgressDialog(_("Waiting for game data..."), WaitForTurns);
	gbBufferMsgs = 0;
	if (!success) {
		FreePackets();
		return false;
	}

	if (gbGameDestroyed) {
		DrawDlg("%s", _("The game ended"));
		FreePackets();
		return false;
	}

	if (sgbDeltaChunks != MAX_CHUNKS) {
		DrawDlg("%s", _("Unable to get level data"));
		FreePackets();
		return false;
	}

	return true;
}

void run_delta_info()
{
	if (!gbIsMultiplayer)
		return;

	gbBufferMsgs = 2;
	PrePacket();
	gbBufferMsgs = 0;
	FreePackets();
}

void DeltaExportData(int pnum)
{
	if (sgbDeltaChanged) {
		for (int i = 0; i < NUMLEVELS; i++) {
			std::unique_ptr<byte[]> dst { new byte[sizeof(DLevel) + 1] };
			byte *dstEnd = &dst.get()[1];
			dstEnd = DeltaExportItem(dstEnd, sgLevels[i].item);
			dstEnd = DeltaExportObject(dstEnd, sgLevels[i].object);
			dstEnd = DeltaExportMonster(dstEnd, sgLevels[i].monster);
			int size = CompressData(dst.get(), dstEnd);
			dthread_send_delta(pnum, static_cast<_cmd_id>(i + CMD_DLEVEL_0), std::move(dst), size);
		}

		std::unique_ptr<byte[]> dst { new byte[sizeof(DJunk) + 1] };
		byte *dstEnd = &dst.get()[1];
		dstEnd = DeltaExportJunk(dstEnd);
		int size = CompressData(dst.get(), dstEnd);
		dthread_send_delta(pnum, CMD_DLEVEL_JUNK, std::move(dst), size);
	}

	std::unique_ptr<byte[]> src { new byte[1] { static_cast<byte>(0) } };
	dthread_send_delta(pnum, CMD_DLEVEL_END, std::move(src), 1);
}

void delta_init()
{
	sgbDeltaChanged = false;
	memset(&sgJunk, 0xFF, sizeof(sgJunk));
	memset(sgLevels, 0xFF, sizeof(sgLevels));
	memset(sgLocals, 0, sizeof(sgLocals));
	deltaload = false;
}

void delta_kill_monster(int mi, Point position, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;

	sgbDeltaChanged = true;
	DMonsterStr *pD = &sgLevels[bLevel].monster[mi];
	pD->_mx = position.x;
	pD->_my = position.y;
	pD->_mdir = Monsters[mi]._mdir;
	pD->_mhitpoints = 0;
}

void delta_monster_hp(int mi, int hp, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;

	sgbDeltaChanged = true;
	DMonsterStr *pD = &sgLevels[bLevel].monster[mi];
	if (pD->_mhitpoints > hp)
		pD->_mhitpoints = hp;
}

void delta_sync_monster(const TSyncMonster &monsterSync, uint8_t level)
{
	if (!gbIsMultiplayer)
		return;

	assert(level < NUMLEVELS);
	sgbDeltaChanged = true;

	DMonsterStr &monster = sgLevels[level].monster[monsterSync._mndx];
	if (monster._mhitpoints == 0)
		return;

	monster._mx = monsterSync._mx;
	monster._my = monsterSync._my;
	monster._mactive = UINT8_MAX;
	monster._menemy = monsterSync._menemy;
	monster._mhitpoints = monsterSync._mhitpoints;
	monster.mWhoHit = monsterSync.mWhoHit;
}

bool delta_portal_inited(int i)
{
	return sgJunk.portal[i].x == 0xFF;
}

bool delta_quest_inited(int i)
{
	return sgJunk.quests[i].qstate != QUEST_INVALID;
}

void DeltaAddItem(int ii)
{
	if (!gbIsMultiplayer)
		return;

	for (const TCmdPItem &item : sgLevels[currlevel].item) {
		if (item.bCmd != CMD_INVALID
		    && item.wIndx == Items[ii].IDidx
		    && item.wCI == Items[ii]._iCreateInfo
		    && item.dwSeed == Items[ii]._iSeed
		    && (item.bCmd == CMD_WALKXY || item.bCmd == CMD_STAND)) {
			return;
		}
	}

	for (TCmdPItem &item : sgLevels[currlevel].item) {
		if (item.bCmd != CMD_INVALID)
			continue;

		sgbDeltaChanged = true;
		item.bCmd = CMD_STAND;
		item.x = Items[ii].position.x;
		item.y = Items[ii].position.y;
		item.wIndx = Items[ii].IDidx;
		item.wCI = Items[ii]._iCreateInfo;
		item.dwSeed = Items[ii]._iSeed;
		item.bId = Items[ii]._iIdentified ? 1 : 0;
		item.bDur = Items[ii]._iDurability;
		item.bMDur = Items[ii]._iMaxDur;
		item.bCh = Items[ii]._iCharges;
		item.bMCh = Items[ii]._iMaxCharges;
		item.wValue = Items[ii]._ivalue;
		item.wToHit = Items[ii]._iPLToHit;
		item.wMaxDam = Items[ii]._iMaxDam;
		item.bMinStr = Items[ii]._iMinStr;
		item.bMinMag = Items[ii]._iMinMag;
		item.bMinDex = Items[ii]._iMinDex;
		item.bAC = Items[ii]._iAC;
		item.dwBuff = Items[ii].dwBuff;
		return;
	}
}

void DeltaSaveLevel()
{
	if (!gbIsMultiplayer)
		return;

	for (int i = 0; i < MAX_PLRS; i++) {
		if (i != MyPlayerId)
			ResetPlayerGFX(Players[i]);
	}
	Players[MyPlayerId]._pLvlVisited[currlevel] = true;
	DeltaLeaveSync(currlevel);
}

namespace {

Point GetItemPosition(Point position)
{
	if (CanPut(position))
		return position;

	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			int yy = position.y + j;
			for (int l = -k; l <= k; l++) {
				int xx = position.x + l;
				if (CanPut({ xx, yy }))
					return { xx, yy };
			}
		}
	}

	return position;
}

} // namespace

void DeltaLoadLevel()
{
	if (!gbIsMultiplayer)
		return;

	deltaload = true;
	if (currlevel != 0) {
		for (int i = 0; i < ActiveMonsterCount; i++) {
			if (sgLevels[currlevel].monster[i]._mx == 0xFF)
				continue;

			M_ClearSquares(i);
			int x = sgLevels[currlevel].monster[i]._mx;
			int y = sgLevels[currlevel].monster[i]._my;
			auto &monster = Monsters[i];
			monster.position.tile = { x, y };
			monster.position.old = { x, y };
			monster.position.future = { x, y };
			if (sgLevels[currlevel].monster[i]._mhitpoints != -1) {
				monster._mhitpoints = sgLevels[currlevel].monster[i]._mhitpoints;
				monster.mWhoHit = sgLevels[currlevel].monster[i].mWhoHit;
			}
			if (sgLevels[currlevel].monster[i]._mhitpoints == 0) {
				M_ClearSquares(i);
				if (monster._mAi != AI_DIABLO) {
					if (monster._uniqtype == 0) {
						assert(monster.MType != nullptr);
						AddCorpse(monster.position.tile, monster.MType->mdeadval, monster._mdir);
					} else {
						AddCorpse(monster.position.tile, monster._udeadval, monster._mdir);
					}
				}
				monster._mDelFlag = true;
				M_UpdateLeader(i);
			} else {
				decode_enemy(monster, sgLevels[currlevel].monster[i]._menemy);
				if (monster.position.tile != Point { 0, 0 } && monster.position.tile != GolemHoldingCell)
					dMonster[monster.position.tile.x][monster.position.tile.y] = i + 1;
				if (i < MAX_PLRS) {
					GolumAi(i);
					monster._mFlags |= (MFLAG_TARGETS_MONSTER | MFLAG_GOLEM);
				} else {
					M_StartStand(monster, monster._mdir);
				}
				monster._msquelch = sgLevels[currlevel].monster[i]._mactive;
			}
		}
		memcpy(AutomapView, &sgLocals[currlevel], sizeof(AutomapView));
	}

	for (int i = 0; i < MAXITEMS; i++) {
		if (sgLevels[currlevel].item[i].bCmd == CMD_INVALID)
			continue;

		if (sgLevels[currlevel].item[i].bCmd == CMD_WALKXY) {
			int activeItemIndex = FindGetItem(
			    sgLevels[currlevel].item[i].dwSeed,
			    sgLevels[currlevel].item[i].wIndx,
			    sgLevels[currlevel].item[i].wCI);
			if (activeItemIndex != -1) {
				const auto &position = Items[ActiveItems[activeItemIndex]].position;
				if (dItem[position.x][position.y] == ActiveItems[activeItemIndex] + 1)
					dItem[position.x][position.y] = 0;
				DeleteItem(activeItemIndex);
			}
		}
		if (sgLevels[currlevel].item[i].bCmd == CMD_ACK_PLRINFO) {
			int ii = AllocateItem();
			auto &item = Items[ii];

			if (sgLevels[currlevel].item[i].wIndx == IDI_EAR) {
				RecreateEar(
				    item,
				    sgLevels[currlevel].item[i].wCI,
				    sgLevels[currlevel].item[i].dwSeed,
				    sgLevels[currlevel].item[i].bId,
				    sgLevels[currlevel].item[i].bDur,
				    sgLevels[currlevel].item[i].bMDur,
				    sgLevels[currlevel].item[i].bCh,
				    sgLevels[currlevel].item[i].bMCh,
				    sgLevels[currlevel].item[i].wValue,
				    sgLevels[currlevel].item[i].dwBuff);
			} else {
				RecreateItem(
				    item,
				    sgLevels[currlevel].item[i].wIndx,
				    sgLevels[currlevel].item[i].wCI,
				    sgLevels[currlevel].item[i].dwSeed,
				    sgLevels[currlevel].item[i].wValue,
				    (sgLevels[currlevel].item[i].dwBuff & CF_HELLFIRE) != 0);
				if (sgLevels[currlevel].item[i].bId != 0)
					item._iIdentified = true;
				item._iDurability = sgLevels[currlevel].item[i].bDur;
				item._iMaxDur = sgLevels[currlevel].item[i].bMDur;
				item._iCharges = sgLevels[currlevel].item[i].bCh;
				item._iMaxCharges = sgLevels[currlevel].item[i].bMCh;
				item._iPLToHit = sgLevels[currlevel].item[i].wToHit;
				item._iMaxDam = sgLevels[currlevel].item[i].wMaxDam;
				item._iMinStr = sgLevels[currlevel].item[i].bMinStr;
				item._iMinMag = sgLevels[currlevel].item[i].bMinMag;
				item._iMinDex = sgLevels[currlevel].item[i].bMinDex;
				item._iAC = sgLevels[currlevel].item[i].bAC;
				item.dwBuff = sgLevels[currlevel].item[i].dwBuff;
			}
			int x = sgLevels[currlevel].item[i].x;
			int y = sgLevels[currlevel].item[i].y;
			item.position = GetItemPosition({ x, y });
			dItem[item.position.x][item.position.y] = ii + 1;
			RespawnItem(&Items[ii], false);
		}
	}

	if (currlevel != 0) {
		for (int i = 0; i < MAXOBJECTS; i++) {
			switch (sgLevels[currlevel].object[i].bCmd) {
			case CMD_OPENDOOR:
			case CMD_CLOSEDOOR:
			case CMD_OPERATEOBJ:
			case CMD_PLROPOBJ:
				SyncOpObject(-1, sgLevels[currlevel].object[i].bCmd, i);
				break;
			case CMD_BREAKOBJ:
				SyncBreakObj(-1, Objects[i]);
				break;
			default:
				break;
			}
		}

		for (int i = 0; i < ActiveObjectCount; i++) {
			if (Objects[ActiveObjects[i]].IsTrap()) {
				OperateTrap(Objects[ActiveObjects[i]]);
			}
		}
	}
	deltaload = false;
}

void NetSendCmd(bool bHiPri, _cmd_id bCmd)
{
	TCmd cmd;

	cmd.bCmd = bCmd;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdGolem(BYTE mx, BYTE my, Direction dir, BYTE menemy, int hp, BYTE cl)
{
	TCmdGolem cmd;

	cmd.bCmd = CMD_AWAKEGOLEM;
	cmd._mx = mx;
	cmd._my = my;
	cmd._mdir = dir;
	cmd._menemy = menemy;
	cmd._mhitpoints = hp;
	cmd._currlevel = cl;
	NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdLoc(int playerId, bool bHiPri, _cmd_id bCmd, Point position)
{
	TCmdLoc cmd;

	cmd.bCmd = bCmd;
	cmd.x = position.x;
	cmd.y = position.y;
	if (bHiPri)
		NetSendHiPri(playerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(playerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam1(bool bHiPri, _cmd_id bCmd, Point position, uint16_t wParam1)
{
	TCmdLocParam1 cmd;

	cmd.bCmd = bCmd;
	cmd.x = position.x;
	cmd.y = position.y;
	cmd.wParam1 = wParam1;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam2(bool bHiPri, _cmd_id bCmd, Point position, uint16_t wParam1, uint16_t wParam2)
{
	TCmdLocParam2 cmd;

	cmd.bCmd = bCmd;
	cmd.x = position.x;
	cmd.y = position.y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam3(bool bHiPri, _cmd_id bCmd, Point position, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3)
{
	TCmdLocParam3 cmd;

	cmd.bCmd = bCmd;
	cmd.x = position.x;
	cmd.y = position.y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdLocParam4(bool bHiPri, _cmd_id bCmd, Point position, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3, uint16_t wParam4)
{
	TCmdLocParam4 cmd;

	cmd.bCmd = bCmd;
	cmd.x = position.x;
	cmd.y = position.y;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	cmd.wParam4 = wParam4;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdParam1(bool bHiPri, _cmd_id bCmd, uint16_t wParam1)
{
	TCmdParam1 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdParam2(bool bHiPri, _cmd_id bCmd, uint16_t wParam1, uint16_t wParam2)
{
	TCmdParam2 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdParam3(bool bHiPri, _cmd_id bCmd, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3)
{
	TCmdParam3 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdParam4(bool bHiPri, _cmd_id bCmd, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3, uint16_t wParam4)
{
	TCmdParam4 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	cmd.wParam4 = wParam4;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdQuest(bool bHiPri, const Quest &quest)
{
	TCmdQuest cmd;
	cmd.bCmd = CMD_SYNCQUEST;
	cmd.q = quest._qidx,
	cmd.qstate = quest._qactive;
	cmd.qlog = quest._qlog ? 1 : 0;
	cmd.qvar1 = quest._qvar1;

	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdGItem(bool bHiPri, _cmd_id bCmd, BYTE mast, BYTE pnum, BYTE ii)
{
	TCmdGItem cmd;

	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;
	cmd.bLevel = currlevel;
	cmd.bCursitem = ii;
	cmd.dwTime = 0;
	cmd.x = Items[ii].position.x;
	cmd.y = Items[ii].position.y;
	cmd.wIndx = Items[ii].IDidx;

	if (Items[ii].IDidx == IDI_EAR) {
		cmd.wCI = Items[ii]._iName[8] | (Items[ii]._iName[7] << 8);
		cmd.dwSeed = Items[ii]._iName[12] | ((Items[ii]._iName[11] | ((Items[ii]._iName[10] | (Items[ii]._iName[9] << 8)) << 8)) << 8);
		cmd.bId = Items[ii]._iName[13];
		cmd.bDur = Items[ii]._iName[14];
		cmd.bMDur = Items[ii]._iName[15];
		cmd.bCh = Items[ii]._iName[16];
		cmd.bMCh = Items[ii]._iName[17];
		cmd.wValue = Items[ii]._ivalue | (Items[ii]._iName[18] << 8) | ((Items[ii]._iCurs - ICURS_EAR_SORCERER) << 6);
		cmd.dwBuff = Items[ii]._iName[22] | ((Items[ii]._iName[21] | ((Items[ii]._iName[20] | (Items[ii]._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = Items[ii]._iCreateInfo;
		cmd.dwSeed = Items[ii]._iSeed;
		cmd.bId = Items[ii]._iIdentified ? 1 : 0;
		cmd.bDur = Items[ii]._iDurability;
		cmd.bMDur = Items[ii]._iMaxDur;
		cmd.bCh = Items[ii]._iCharges;
		cmd.bMCh = Items[ii]._iMaxCharges;
		cmd.wValue = Items[ii]._ivalue;
		cmd.wToHit = Items[ii]._iPLToHit;
		cmd.wMaxDam = Items[ii]._iMaxDam;
		cmd.bMinStr = Items[ii]._iMinStr;
		cmd.bMinMag = Items[ii]._iMinMag;
		cmd.bMinDex = Items[ii]._iMinDex;
		cmd.bAC = Items[ii]._iAC;
		cmd.dwBuff = Items[ii].dwBuff;
	}

	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdPItem(bool bHiPri, _cmd_id bCmd, Point position)
{
	TCmdPItem cmd;

	cmd.bCmd = bCmd;
	cmd.x = position.x;
	cmd.y = position.y;
	auto &myPlayer = Players[MyPlayerId];
	cmd.wIndx = myPlayer.HoldItem.IDidx;

	if (myPlayer.HoldItem.IDidx == IDI_EAR) {
		cmd.wCI = myPlayer.HoldItem._iName[8] | (myPlayer.HoldItem._iName[7] << 8);
		cmd.dwSeed = myPlayer.HoldItem._iName[12] | ((myPlayer.HoldItem._iName[11] | ((myPlayer.HoldItem._iName[10] | (myPlayer.HoldItem._iName[9] << 8)) << 8)) << 8);
		cmd.bId = myPlayer.HoldItem._iName[13];
		cmd.bDur = myPlayer.HoldItem._iName[14];
		cmd.bMDur = myPlayer.HoldItem._iName[15];
		cmd.bCh = myPlayer.HoldItem._iName[16];
		cmd.bMCh = myPlayer.HoldItem._iName[17];
		cmd.wValue = myPlayer.HoldItem._ivalue | (myPlayer.HoldItem._iName[18] << 8) | ((myPlayer.HoldItem._iCurs - ICURS_EAR_SORCERER) << 6);
		cmd.dwBuff = myPlayer.HoldItem._iName[22] | ((myPlayer.HoldItem._iName[21] | ((myPlayer.HoldItem._iName[20] | (myPlayer.HoldItem._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = myPlayer.HoldItem._iCreateInfo;
		cmd.dwSeed = myPlayer.HoldItem._iSeed;
		cmd.bId = myPlayer.HoldItem._iIdentified ? 1 : 0;
		cmd.bDur = myPlayer.HoldItem._iDurability;
		cmd.bMDur = myPlayer.HoldItem._iMaxDur;
		cmd.bCh = myPlayer.HoldItem._iCharges;
		cmd.bMCh = myPlayer.HoldItem._iMaxCharges;
		cmd.wValue = myPlayer.HoldItem._ivalue;
		cmd.wToHit = myPlayer.HoldItem._iPLToHit;
		cmd.wMaxDam = myPlayer.HoldItem._iMaxDam;
		cmd.bMinStr = myPlayer.HoldItem._iMinStr;
		cmd.bMinMag = myPlayer.HoldItem._iMinMag;
		cmd.bMinDex = myPlayer.HoldItem._iMinDex;
		cmd.bAC = myPlayer.HoldItem._iAC;
		cmd.dwBuff = myPlayer.HoldItem.dwBuff;
	}

	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdChItem(bool bHiPri, BYTE bLoc)
{
	TCmdChItem cmd;

	auto &myPlayer = Players[MyPlayerId];

	cmd.bCmd = CMD_CHANGEPLRITEMS;
	cmd.bLoc = bLoc;
	cmd.wIndx = myPlayer.HoldItem.IDidx;
	cmd.wCI = myPlayer.HoldItem._iCreateInfo;
	cmd.dwSeed = myPlayer.HoldItem._iSeed;
	cmd.bId = myPlayer.HoldItem._iIdentified ? 1 : 0;
	cmd.dwBuff = myPlayer.HoldItem.dwBuff;

	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdDelItem(bool bHiPri, BYTE bLoc)
{
	TCmdDelItem cmd;

	cmd.bLoc = bLoc;
	cmd.bCmd = CMD_DELPLRITEMS;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdDItem(bool bHiPri, int ii)
{
	TCmdPItem cmd;

	cmd.bCmd = CMD_DROPITEM;
	cmd.x = Items[ii].position.x;
	cmd.y = Items[ii].position.y;
	cmd.wIndx = Items[ii].IDidx;

	if (Items[ii].IDidx == IDI_EAR) {
		cmd.wCI = Items[ii]._iName[8] | (Items[ii]._iName[7] << 8);
		cmd.dwSeed = Items[ii]._iName[12] | ((Items[ii]._iName[11] | ((Items[ii]._iName[10] | (Items[ii]._iName[9] << 8)) << 8)) << 8);
		cmd.bId = Items[ii]._iName[13];
		cmd.bDur = Items[ii]._iName[14];
		cmd.bMDur = Items[ii]._iName[15];
		cmd.bCh = Items[ii]._iName[16];
		cmd.bMCh = Items[ii]._iName[17];
		cmd.wValue = Items[ii]._ivalue | (Items[ii]._iName[18] << 8) | ((Items[ii]._iCurs - ICURS_EAR_SORCERER) << 6);
		cmd.dwBuff = Items[ii]._iName[22] | ((Items[ii]._iName[21] | ((Items[ii]._iName[20] | (Items[ii]._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = Items[ii]._iCreateInfo;
		cmd.dwSeed = Items[ii]._iSeed;
		cmd.bId = Items[ii]._iIdentified ? 1 : 0;
		cmd.bDur = Items[ii]._iDurability;
		cmd.bMDur = Items[ii]._iMaxDur;
		cmd.bCh = Items[ii]._iCharges;
		cmd.bMCh = Items[ii]._iMaxCharges;
		cmd.wValue = Items[ii]._ivalue;
		cmd.wToHit = Items[ii]._iPLToHit;
		cmd.wMaxDam = Items[ii]._iMaxDam;
		cmd.bMinStr = Items[ii]._iMinStr;
		cmd.bMinMag = Items[ii]._iMinMag;
		cmd.bMinDex = Items[ii]._iMinDex;
		cmd.bAC = Items[ii]._iAC;
		cmd.dwBuff = Items[ii].dwBuff;
	}

	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdDamage(bool bHiPri, uint8_t bPlr, uint32_t dwDam)
{
	TCmdDamage cmd;

	cmd.bCmd = CMD_PLRDAMAGE;
	cmd.bPlr = bPlr;
	cmd.dwDam = dwDam;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdMonDmg(bool bHiPri, uint16_t wMon, uint32_t dwDam)
{
	TCmdMonDamage cmd;

	cmd.bCmd = CMD_MONSTDAMAGE;
	cmd.wMon = wMon;
	cmd.dwDam = dwDam;
	if (bHiPri)
		NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdString(uint32_t pmask, const char *pszStr)
{
	TCmdString cmd;

	cmd.bCmd = CMD_STRING;
	strcpy(cmd.str, pszStr);
	multi_send_msg_packet(pmask, (byte *)&cmd, strlen(pszStr) + 2);
}

void delta_close_portal(int pnum)
{
	memset(&sgJunk.portal[pnum], 0xFF, sizeof(sgJunk.portal[pnum]));
	sgbDeltaChanged = true;
}

uint32_t ParseCmd(int pnum, const TCmd *pCmd)
{
	sbLastCmd = pCmd->bCmd;
	if (sgwPackPlrOffsetTbl[pnum] != 0 && sbLastCmd != CMD_ACK_PLRINFO && sbLastCmd != CMD_SEND_PLRINFO)
		return 0;

	auto &player = Players[pnum];

	switch (pCmd->bCmd) {
	case CMD_SYNCDATA:
		return OnSyncData(pCmd, pnum);
	case CMD_WALKXY:
		return OnWalk(pCmd, player);
	case CMD_ADDSTR:
		return OnAddStrength(pCmd, pnum);
	case CMD_ADDDEX:
		return OnAddDexterity(pCmd, pnum);
	case CMD_ADDMAG:
		return OnAddMagic(pCmd, pnum);
	case CMD_ADDVIT:
		return OnAddVitality(pCmd, pnum);
	case CMD_GOTOGETITEM:
		return OnGotoGetItem(pCmd, player);
	case CMD_REQUESTGITEM:
		return OnRequestGetItem(pCmd, player);
	case CMD_GETITEM:
		return OnGetItem(pCmd, pnum);
	case CMD_GOTOAGETITEM:
		return OnGotoAutoGetItem(pCmd, player);
	case CMD_REQUESTAGITEM:
		return OnRequestAutoGetItem(pCmd, player);
	case CMD_AGETITEM:
		return OnAutoGetItem(pCmd, pnum);
	case CMD_ITEMEXTRA:
		return OnItemExtra(pCmd, pnum);
	case CMD_PUTITEM:
		return OnPutItem(pCmd, pnum);
	case CMD_SYNCPUTITEM:
		return OnSyncPutItem(pCmd, pnum);
	case CMD_RESPAWNITEM:
		return OnRespawnItem(pCmd, pnum);
	case CMD_ATTACKXY:
		return OnAttackTile(pCmd, player);
	case CMD_SATTACKXY:
		return OnStandingAttackTile(pCmd, player);
	case CMD_RATTACKXY:
		return OnRangedAttackTile(pCmd, player);
	case CMD_SPELLXYD:
		return OnSpellWall(pCmd, player);
	case CMD_SPELLXY:
		return OnSpellTile(pCmd, player);
	case CMD_TSPELLXY:
		return OnTargetSpellTile(pCmd, player);
	case CMD_OPOBJXY:
		return OnOperateObjectTile(pCmd, player);
	case CMD_DISARMXY:
		return OnDisarm(pCmd, player);
	case CMD_OPOBJT:
		return OnOperateObjectTelekinesis(pCmd, player);
	case CMD_ATTACKID:
		return OnAttackMonster(pCmd, player);
	case CMD_ATTACKPID:
		return OnAttackPlayer(pCmd, player);
	case CMD_RATTACKID:
		return OnRangedAttackMonster(pCmd, player);
	case CMD_RATTACKPID:
		return OnRangedAttackPlayer(pCmd, player);
	case CMD_SPELLID:
		return OnSpellMonster(pCmd, player);
	case CMD_SPELLPID:
		return OnSpellPlayer(pCmd, player);
	case CMD_TSPELLID:
		return OnTargetSpellMonster(pCmd, player);
	case CMD_TSPELLPID:
		return OnTargetSpellPlayer(pCmd, player);
	case CMD_KNOCKBACK:
		return OnKnockback(pCmd, pnum);
	case CMD_RESURRECT:
		return OnResurrect(pCmd, pnum);
	case CMD_HEALOTHER:
		return OnHealOther(pCmd, pnum);
	case CMD_TALKXY:
		return OnTalkXY(pCmd, player);
	case CMD_DEBUG:
		return OnDebug(pCmd);
	case CMD_NEWLVL:
		return OnNewLevel(pCmd, pnum);
	case CMD_WARP:
		return OnWarp(pCmd, pnum);
	case CMD_MONSTDEATH:
		return OnMonstDeath(pCmd, pnum);
	case CMD_KILLGOLEM:
		return OnKillGolem(pCmd, pnum);
	case CMD_AWAKEGOLEM:
		return OnAwakeGolem(pCmd, pnum);
	case CMD_MONSTDAMAGE:
		return OnMonstDamage(pCmd, pnum);
	case CMD_PLRDEAD:
		return OnPlayerDeath(pCmd, pnum);
	case CMD_PLRDAMAGE:
		return OnPlayerDamage(pCmd, player);
	case CMD_OPENDOOR:
		return OnOpenDoor(pCmd, pnum);
	case CMD_CLOSEDOOR:
		return OnCloseDoor(pCmd, pnum);
	case CMD_OPERATEOBJ:
		return OnOperateObject(pCmd, pnum);
	case CMD_PLROPOBJ:
		return OnPlayerOperateObject(pCmd, pnum);
	case CMD_BREAKOBJ:
		return OnBreakObject(pCmd, pnum);
	case CMD_CHANGEPLRITEMS:
		return OnChangePlayerItems(pCmd, pnum);
	case CMD_DELPLRITEMS:
		return OnDeletePlayerItems(pCmd, pnum);
	case CMD_PLRLEVEL:
		return OnPlayerLevel(pCmd, pnum);
	case CMD_DROPITEM:
		return OnDropItem(pCmd, pnum);
	case CMD_ACK_PLRINFO:
	case CMD_SEND_PLRINFO:
		return OnSendPlayerInfo(pCmd, pnum);
	case CMD_PLAYER_JOINLEVEL:
		return OnPlayerJoinLevel(pCmd, pnum);
	case CMD_ACTIVATEPORTAL:
		return OnActivatePortal(pCmd, pnum);
	case CMD_DEACTIVATEPORTAL:
		return OnDeactivatePortal(pCmd, pnum);
	case CMD_RETOWN:
		return OnRestartTown(pCmd, pnum);
	case CMD_SETSTR:
		return OnSetStrength(pCmd, pnum);
	case CMD_SETMAG:
		return OnSetMagic(pCmd, pnum);
	case CMD_SETDEX:
		return OnSetDexterity(pCmd, pnum);
	case CMD_SETVIT:
		return OnSetVitality(pCmd, pnum);
	case CMD_STRING:
		return OnString(pCmd, pnum);
	case CMD_SYNCQUEST:
		return OnSyncQuest(pCmd, pnum);
	case CMD_CHEAT_EXPERIENCE:
		return OnCheatExperience(pCmd, pnum);
	case CMD_CHEAT_SPELL_LEVEL:
		return OnCheatSpellLevel(pCmd, pnum);
	case CMD_NOVA:
		return OnNova(pCmd, pnum);
	case CMD_SETSHIELD:
		return OnSetShield(pCmd, player);
	case CMD_REMSHIELD:
		return OnRemoveShield(pCmd, player);
	case CMD_SETREFLECT:
		return OnSetReflect(pCmd, player);
	case CMD_NAKRUL:
		return OnNakrul(pCmd);
	case CMD_OPENHIVE:
		return OnOpenHive(pCmd, pnum);
	case CMD_OPENCRYPT:
		return OnOpenCrypt(pCmd);
	default:
		break;
	}

	if (pCmd->bCmd < CMD_DLEVEL_0 || pCmd->bCmd > CMD_DLEVEL_END) {
		SNetDropPlayer(pnum, LEAVE_DROP);
		return 0;
	}

	return OnLevelData(pnum, pCmd);
}

} // namespace devilution
