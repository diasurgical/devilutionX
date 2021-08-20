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
#include "storm/storm.h"
#include "sync.h"
#include "town.h"
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
BYTE sgbRecvCmd;
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
	uint8_t playerId = -1;
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

			uint32_t pktSize = ParseCmd(playerId, (TCmd *)data);
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

byte *DeltaExportItem(byte *dst, TCmdPItem *src)
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

byte *DeltaImportItem(byte *src, TCmdPItem *dst)
{
	for (int i = 0; i < MAXITEMS; i++, dst++) {
		if (*src == byte { 0xFF }) {
			memset(dst, 0xFF, sizeof(TCmdPItem));
			src++;
		} else {
			memcpy(dst, src, sizeof(TCmdPItem));
			src += sizeof(TCmdPItem);
		}
	}

	return src;
}

byte *DeltaExportObject(byte *dst, DObjectStr *src)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return dst + sizeof(DObjectStr) * MAXOBJECTS;
}

byte *DeltaImportObject(byte *src, DObjectStr *dst)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return src + sizeof(DObjectStr) * MAXOBJECTS;
}

byte *DeltaExportMonster(byte *dst, DMonsterStr *src)
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

byte *DeltaImportMonster(byte *src, DMonsterStr *dst)
{
	for (int i = 0; i < MAXMONSTERS; i++, dst++) {
		if (*src == byte { 0xFF }) {
			memset(dst, 0xFF, sizeof(DMonsterStr));
			src++;
		} else {
			memcpy(dst, src, sizeof(DMonsterStr));
			src += sizeof(DMonsterStr);
		}
	}

	return src;
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
		if (!QuestData[quest._qidx].isSinglePlayerOnly) {
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

void DeltaImportJunk(byte *src)
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
		if (!QuestData[quest._qidx].isSinglePlayerOnly) {
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

void DeltaImportData(BYTE cmd, DWORD recvOffset)
{
	if (sgRecvBuf[0] != byte { 0 })
		PkwareDecompress(&sgRecvBuf[1], recvOffset, sizeof(sgRecvBuf) - 1);

	byte *src = &sgRecvBuf[1];
	if (cmd == CMD_DLEVEL_JUNK) {
		DeltaImportJunk(src);
	} else if (cmd >= CMD_DLEVEL_0 && cmd <= CMD_DLEVEL_24) {
		BYTE i = cmd - CMD_DLEVEL_0;
		src = DeltaImportItem(src, sgLevels[i].item);
		src = DeltaImportObject(src, sgLevels[i].object);
		DeltaImportMonster(src, sgLevels[i].monster);
	} else {
		app_fatal("Unkown network message type: %i", cmd);
	}

	sgbDeltaChunks++;
	sgbDeltaChanged = true;
}

DWORD OnLevelData(int pnum, TCmd *pCmd)
{
	auto *p = (TCmdPlrInfoHdr *)pCmd;

	if (gbDeltaSender != pnum) {
		if (p->bCmd == CMD_DLEVEL_END || (p->bCmd == CMD_DLEVEL_0 && p->wOffset == 0)) {
			gbDeltaSender = pnum;
			sgbRecvCmd = CMD_DLEVEL_END;
		} else {
			return p->wBytes + sizeof(*p);
		}
	}
	if (sgbRecvCmd == CMD_DLEVEL_END) {
		if (p->bCmd == CMD_DLEVEL_END) {
			sgbDeltaChunks = MAX_CHUNKS - 1;
			return p->wBytes + sizeof(*p);
		}
		if (p->bCmd == CMD_DLEVEL_0 && p->wOffset == 0) {
			sgdwRecvOffset = 0;
			sgbRecvCmd = p->bCmd;
		} else {
			return p->wBytes + sizeof(*p);
		}
	} else if (sgbRecvCmd != p->bCmd) {
		DeltaImportData(sgbRecvCmd, sgdwRecvOffset);
		if (p->bCmd == CMD_DLEVEL_END) {
			sgbDeltaChunks = MAX_CHUNKS - 1;
			sgbRecvCmd = CMD_DLEVEL_END;
			return p->wBytes + sizeof(*p);
		}
		sgdwRecvOffset = 0;
		sgbRecvCmd = p->bCmd;
	}

	assert(p->wOffset == sgdwRecvOffset);
	memcpy(&sgRecvBuf[p->wOffset], &p[1], p->wBytes);
	sgdwRecvOffset += p->wBytes;
	return p->wBytes + sizeof(*p);
}

void DeltaSyncGolem(TCmdGolem *pG, int pnum, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;

	sgbDeltaChanged = true;
	DMonsterStr *pD = &sgLevels[bLevel].monster[pnum];
	pD->_mx = pG->_mx;
	pD->_my = pG->_my;
	pD->_mactive = UINT8_MAX;
	pD->_menemy = pG->_menemy;
	pD->_mdir = pG->_mdir;
	pD->_mhitpoints = pG->_mhitpoints;
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
		DMonsterStr *pD = &sgLevels[bLevel].monster[ma];
		pD->_mx = monster.position.tile.x;
		pD->_my = monster.position.tile.y;
		pD->_mdir = monster._mdir;
		pD->_menemy = encode_enemy(monster);
		pD->_mhitpoints = monster._mhitpoints;
		pD->_mactive = monster._msquelch;
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

bool DeltaGetItem(TCmdGItem *pI, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return true;

	TCmdPItem *pD = sgLevels[bLevel].item;
	for (int i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == CMD_INVALID || pD->wIndx != pI->wIndx || pD->wCI != pI->wCI || pD->dwSeed != pI->dwSeed)
			continue;

		if (pD->bCmd == CMD_WALKXY) {
			return true;
		}
		if (pD->bCmd == CMD_STAND) {
			sgbDeltaChanged = true;
			pD->bCmd = CMD_WALKXY;
			return true;
		}
		if (pD->bCmd == CMD_ACK_PLRINFO) {
			sgbDeltaChanged = true;
			pD->bCmd = CMD_INVALID;
			return true;
		}

		app_fatal("delta:1");
	}

	if ((pI->wCI & CF_PREGEN) == 0)
		return false;

	pD = sgLevels[bLevel].item;
	for (int i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == CMD_INVALID) {
			sgbDeltaChanged = true;
			pD->bCmd = CMD_WALKXY;
			pD->x = pI->x;
			pD->y = pI->y;
			pD->wIndx = pI->wIndx;
			pD->wCI = pI->wCI;
			pD->dwSeed = pI->dwSeed;
			pD->bId = pI->bId;
			pD->bDur = pI->bDur;
			pD->bMDur = pI->bMDur;
			pD->bCh = pI->bCh;
			pD->bMCh = pI->bMCh;
			pD->wValue = pI->wValue;
			pD->dwBuff = pI->dwBuff;
			pD->wToHit = pI->wToHit;
			pD->wMaxDam = pI->wMaxDam;
			pD->bMinStr = pI->bMinStr;
			pD->bMinMag = pI->bMinMag;
			pD->bMinDex = pI->bMinDex;
			pD->bAC = pI->bAC;
			break;
		}
	}
	return true;
}

void DeltaPutItem(TCmdPItem *pI, int x, int y, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;

	TCmdPItem *pD = sgLevels[bLevel].item;
	for (int i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != CMD_WALKXY
		    && pD->bCmd != CMD_INVALID
		    && pD->wIndx == pI->wIndx
		    && pD->wCI == pI->wCI
		    && pD->dwSeed == pI->dwSeed) {
			if (pD->bCmd == CMD_ACK_PLRINFO)
				return;
			app_fatal("%s", _("Trying to drop a floor item?"));
		}
	}

	pD = sgLevels[bLevel].item;
	for (int i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == CMD_INVALID) {
			sgbDeltaChanged = true;
			memcpy(pD, pI, sizeof(TCmdPItem));
			pD->bCmd = CMD_ACK_PLRINFO;
			pD->x = x;
			pD->y = y;
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

void DeltaOpenPortal(int pnum, uint8_t x, uint8_t y, uint8_t bLevel, dungeon_type bLType, bool bSetLvl)
{
	sgbDeltaChanged = true;
	sgJunk.portal[pnum].x = x;
	sgJunk.portal[pnum].y = y;
	sgJunk.portal[pnum].level = bLevel;
	sgJunk.portal[pnum].ltype = bLType;
	sgJunk.portal[pnum].setlvl = bSetLvl ? 1 : 0;
}

void CheckUpdatePlayer(int pnum)
{
	if (gbIsMultiplayer && pnum == MyPlayerId)
		pfile_update(true);
}

void PlayerMessageFormat(const char *pszFmt, ...)
{
	static DWORD msgErrTimer;
	DWORD ticks;
	char msg[256];
	va_list va;

	va_start(va, pszFmt);
	ticks = SDL_GetTicks();
	if (ticks - msgErrTimer >= 5000) {
		msgErrTimer = ticks;
		vsprintf(msg, pszFmt, va);
		ErrorPlrMsg(msg);
	}
	va_end(va);
}

void NetSendCmdGItem2(bool usonly, _cmd_id bCmd, BYTE mast, BYTE pnum, TCmdGItem *p)
{
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
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

	multi_msg_add((byte *)&cmd.bCmd, sizeof(cmd));
}

bool NetSendCmdReq2(_cmd_id bCmd, BYTE mast, BYTE pnum, TCmdGItem *p)
{
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
	cmd.bCmd = bCmd;
	cmd.bPnum = pnum;
	cmd.bMaster = mast;

	int ticks = SDL_GetTicks();
	if (cmd.dwTime == 0)
		cmd.dwTime = ticks;
	else if (ticks - cmd.dwTime > 5000)
		return false;

	multi_msg_add((byte *)&cmd.bCmd, sizeof(cmd));

	return true;
}

void NetSendCmdExtra(TCmdGItem *p)
{
	TCmdGItem cmd;

	memcpy(&cmd, p, sizeof(cmd));
	cmd.dwTime = 0;
	cmd.bCmd = CMD_ITEMEXTRA;
	NetSendHiPri(MyPlayerId, (byte *)&cmd, sizeof(cmd));
}

DWORD OnSyncData(TCmd *pCmd, int pnum)
{
	return sync_update(pnum, (const byte *)pCmd);
}

DWORD OnWalk(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		ClrPlrPath(player);
		MakePlrPath(player, { p->x, p->y }, true);
		player.destAction = ACTION_NONE;
	}

	return sizeof(*p);
}

DWORD OnAddStrength(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		ModifyPlrStr(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnAddMagic(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		ModifyPlrMag(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnAddDexterity(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		ModifyPlrDex(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnAddVitality(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		ModifyPlrVit(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnGotoGetItem(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		MakePlrPath(player, { p->x, p->y }, false);
		player.destAction = ACTION_PICKUPITEM;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnRequestGetItem(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs != 1 && IOwnLevel(player.plrlevel)) {
		if (GetItemRecord(p->dwSeed, p->wCI, p->wIndx)) {
			int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
			if (ii != -1) {
				NetSendCmdGItem2(false, CMD_GETITEM, MyPlayerId, p->bPnum, p);
				if (p->bPnum != MyPlayerId)
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				else
					InvGetItem(MyPlayerId, &Items[ii], ii);
				SetItemRecord(p->dwSeed, p->wCI, p->wIndx);
			} else if (!NetSendCmdReq2(CMD_REQUESTGITEM, MyPlayerId, p->bPnum, p)) {
				NetSendCmdExtra(p);
			}
		}
	}

	return sizeof(*p);
}

DWORD OnGetItem(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
		if (DeltaGetItem(p, p->bLevel)) {
			if ((currlevel == p->bLevel || p->bPnum == MyPlayerId) && p->bMaster != MyPlayerId) {
				if (p->bPnum == MyPlayerId) {
					if (currlevel != p->bLevel) {
						auto &player = Players[MyPlayerId];
						ii = SyncPutItem(player, player.position.tile, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
						if (ii != -1)
							InvGetItem(MyPlayerId, &Items[ii], ii);
					} else {
						InvGetItem(MyPlayerId, &Items[ii], ii);
					}
				} else {
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				}
			}
		} else {
			NetSendCmdGItem2(true, CMD_GETITEM, p->bMaster, p->bPnum, p);
		}
	}

	return sizeof(*p);
}

DWORD OnGotoAutoGetItem(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		MakePlrPath(player, { p->x, p->y }, false);
		player.destAction = ACTION_PICKUPAITEM;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnRequestAutoGetItem(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs != 1 && IOwnLevel(player.plrlevel)) {
		if (GetItemRecord(p->dwSeed, p->wCI, p->wIndx)) {
			int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
			if (ii != -1) {
				NetSendCmdGItem2(false, CMD_AGETITEM, MyPlayerId, p->bPnum, p);
				if (p->bPnum != MyPlayerId)
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				else
					AutoGetItem(MyPlayerId, &Items[p->bCursitem], p->bCursitem);
				SetItemRecord(p->dwSeed, p->wCI, p->wIndx);
			} else if (!NetSendCmdReq2(CMD_REQUESTAGITEM, MyPlayerId, p->bPnum, p)) {
				NetSendCmdExtra(p);
			}
		}
	}

	return sizeof(*p);
}

DWORD OnAutoGetItem(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		FindGetItem(p->wIndx, p->wCI, p->dwSeed);
		if (DeltaGetItem(p, p->bLevel)) {
			if ((currlevel == p->bLevel || p->bPnum == MyPlayerId) && p->bMaster != MyPlayerId) {
				if (p->bPnum == MyPlayerId) {
					if (currlevel != p->bLevel) {
						auto &player = Players[MyPlayerId];
						int ii = SyncPutItem(player, player.position.tile, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
						if (ii != -1)
							AutoGetItem(MyPlayerId, &Items[ii], ii);
					} else {
						AutoGetItem(MyPlayerId, &Items[p->bCursitem], p->bCursitem);
					}
				} else {
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				}
			}
		} else {
			NetSendCmdGItem2(true, CMD_AGETITEM, p->bMaster, p->bPnum, p);
		}
	}

	return sizeof(*p);
}

DWORD OnItemExtra(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		DeltaGetItem(p, p->bLevel);
		if (currlevel == Players[pnum].plrlevel)
			SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
	}

	return sizeof(*p);
}

DWORD OnPutItem(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (currlevel == Players[pnum].plrlevel) {
		int ii;
		if (pnum == MyPlayerId)
			ii = InvPutItem(Players[pnum], { p->x, p->y });
		else
			ii = SyncPutItem(Players[pnum], { p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
		if (ii != -1) {
			PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
			DeltaPutItem(p, Items[ii].position.x, Items[ii].position.y, Players[pnum].plrlevel);
			CheckUpdatePlayer(pnum);
		}
		return sizeof(*p);
	} else {
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		DeltaPutItem(p, p->x, p->y, Players[pnum].plrlevel);
		CheckUpdatePlayer(pnum);
	}

	return sizeof(*p);
}

DWORD OnSyncPutItem(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (currlevel == Players[pnum].plrlevel) {
		int ii = SyncPutItem(Players[pnum], { p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
		if (ii != -1) {
			PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
			DeltaPutItem(p, Items[ii].position.x, Items[ii].position.y, Players[pnum].plrlevel);
			CheckUpdatePlayer(pnum);
		}
		return sizeof(*p);
	} else {
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		DeltaPutItem(p, p->x, p->y, Players[pnum].plrlevel);
		CheckUpdatePlayer(pnum);
	}

	return sizeof(*p);
}

DWORD OnRespawnItem(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		auto &player = Players[pnum];
		int playerLevel = player.plrlevel;
		if (currlevel == playerLevel && pnum != MyPlayerId) {
			SyncPutItem(player, { p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
		}
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		DeltaPutItem(p, p->x, p->y, playerLevel);
	}

	return sizeof(*p);
}

DWORD OnAttackTile(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		MakePlrPath(player, { p->x, p->y }, false);
		player.destAction = ACTION_ATTACK;
		player.destParam1 = p->x;
		player.destParam2 = p->y;
	}

	return sizeof(*p);
}

DWORD OnStandingAttackTile(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		ClrPlrPath(player);
		player.destAction = ACTION_ATTACK;
		player.destParam1 = p->x;
		player.destParam2 = p->y;
	}

	return sizeof(*p);
}

DWORD OnRangedAttackTile(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		ClrPlrPath(player);
		player.destAction = ACTION_RATTACK;
		player.destParam1 = p->x;
		player.destParam2 = p->y;
	}

	return sizeof(*p);
}

DWORD OnSpellWall(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLocParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam1);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(player);
			player.destAction = ACTION_SPELLWALL;
			player.destParam1 = p->x;
			player.destParam2 = p->y;
			player.destParam3 = static_cast<Direction>(p->wParam2);
			player.destParam4 = p->wParam3;
			player._pSpell = spell;
			player._pSplType = player._pRSplType;
			player._pSplFrom = 0;
		} else {
			PlayerMessageFormat(fmt::format(_("{:s} has cast an illegal spell."), player._pName).c_str());
		}
	}

	return sizeof(*p);
}

DWORD OnSpellTile(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLocParam2 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam1);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(player);
			player.destAction = ACTION_SPELL;
			player.destParam1 = p->x;
			player.destParam2 = p->y;
			player.destParam3 = static_cast<Direction>(p->wParam2);
			player._pSpell = spell;
			player._pSplType = player._pRSplType;
			player._pSplFrom = 0;
		} else {
			PlayerMessageFormat(fmt::format(_("{:s} has cast an illegal spell."), player._pName).c_str());
		}
	}

	return sizeof(*p);
}

DWORD OnTargetSpellTile(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLocParam2 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam1);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(player);
			player.destAction = ACTION_SPELL;
			player.destParam1 = p->x;
			player.destParam2 = p->y;
			player.destParam3 = static_cast<Direction>(p->wParam2);
			player._pSpell = spell;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 2;
		} else {
			PlayerMessageFormat(fmt::format(_("{:s} has cast an illegal spell."), player._pName).c_str());
		}
	}

	return sizeof(*p);
}

DWORD OnOperateObjectTile(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		MakePlrPath(player, { p->x, p->y }, !Objects[p->wParam1]._oSolidFlag && !Objects[p->wParam1]._oDoorFlag);
		player.destAction = ACTION_OPERATE;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnDisarm(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		MakePlrPath(player, { p->x, p->y }, !Objects[p->wParam1]._oSolidFlag && !Objects[p->wParam1]._oDoorFlag);
		player.destAction = ACTION_DISARM;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnOperateObjectTelekinesis(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		player.destAction = ACTION_OPERATETK;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnAttackMonster(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		Point position = Monsters[p->wParam1].position.future;
		if (player.position.tile.WalkingDistance(position) > 1)
			MakePlrPath(player, position, false);
		player.destAction = ACTION_ATTACKMON;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnAttackPlayer(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		MakePlrPath(player, Players[p->wParam1].position.future, false);
		player.destAction = ACTION_ATTACKPLR;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnRangedAttackMonster(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		ClrPlrPath(player);
		player.destAction = ACTION_RATTACKMON;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnRangedAttackPlayer(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		ClrPlrPath(player);
		player.destAction = ACTION_RATTACKPLR;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnSpellMonster(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam2);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(player);
			player.destAction = ACTION_SPELLMON;
			player.destParam1 = p->wParam1;
			player.destParam2 = p->wParam3;
			player._pSpell = spell;
			player._pSplType = player._pRSplType;
			player._pSplFrom = 0;
		} else {
			PlayerMessageFormat(fmt::format(_("{:s} has cast an illegal spell."), player._pName).c_str());
		}
	}

	return sizeof(*p);
}

DWORD OnSpellPlayer(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam2);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(player);
			player.destAction = ACTION_SPELLPLR;
			player.destParam1 = p->wParam1;
			player.destParam2 = p->wParam3;
			player._pSpell = spell;
			player._pSplType = player._pRSplType;
			player._pSplFrom = 0;
		} else {
			PlayerMessageFormat(fmt::format(_("{:s} has cast an illegal spell."), player._pName).c_str());
		}
	}

	return sizeof(*p);
}

DWORD OnTargetSpellMonster(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam2);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(player);
			player.destAction = ACTION_SPELLMON;
			player.destParam1 = p->wParam1;
			player.destParam2 = p->wParam3;
			player._pSpell = spell;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 2;
		} else {
			PlayerMessageFormat(fmt::format(_("{:s} has cast an illegal spell."), player._pName).c_str());
		}
	}

	return sizeof(*p);
}

DWORD OnTargetSpellPlayer(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam2);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(player);
			player.destAction = ACTION_SPELLPLR;
			player.destParam1 = p->wParam1;
			player.destParam2 = p->wParam3;
			player._pSpell = spell;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 2;
		} else {
			PlayerMessageFormat(fmt::format(_("{:s} has cast an illegal spell."), player._pName).c_str());
		}
	}

	return sizeof(*p);
}

DWORD OnKnockback(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == Players[pnum].plrlevel) {
		M_GetKnockback(p->wParam1);
		M_StartHit(p->wParam1, pnum, 0);
	}

	return sizeof(*p);
}

DWORD OnResurrect(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		DoResurrect(pnum, p->wParam1);
		CheckUpdatePlayer(pnum);
	}

	return sizeof(*p);
}

DWORD OnHealOther(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == Players[pnum].plrlevel)
		DoHealOther(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnTalkXY(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == player.plrlevel) {
		MakePlrPath(player, { p->x, p->y }, false);
		player.destAction = ACTION_TALK;
		player.destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

DWORD OnNewLevel(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (pnum != MyPlayerId)
		StartNewLvl(pnum, (interface_mode)p->wParam1, p->wParam2);

	return sizeof(*p);
}

DWORD OnWarp(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		StartWarpLvl(pnum, p->wParam1);
	}

	return sizeof(*p);
}

DWORD OnMonstDeath(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (pnum != MyPlayerId && p->wParam1 < MAXMONSTERS) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			M_SyncStartKill(p->wParam1, { p->x, p->y }, pnum);
		delta_kill_monster(p->wParam1, { p->x, p->y }, playerLevel);
	}

	return sizeof(*p);
}

DWORD OnKillGolem(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (pnum != MyPlayerId) {
		if (currlevel == p->wParam1)
			M_SyncStartKill(pnum, { p->x, p->y }, pnum);
		delta_kill_monster(pnum, { p->x, p->y }, p->wParam1);
	}

	return sizeof(*p);
}

DWORD OnAwakeGolem(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGolem *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (currlevel != Players[pnum].plrlevel)
		DeltaSyncGolem(p, pnum, p->_currlevel);
	else if (pnum != MyPlayerId) {
		// check if this player already has an active golem
		bool addGolem = true;
		for (int i = 0; i < ActiveMissileCount; i++) {
			int mi = ActiveMissiles[i];
			auto &missile = Missiles[mi];
			if (missile._mitype == MIS_GOLEM && missile._misource == pnum) {
				addGolem = false;
				// CODEFIX: break, don't need to check the rest
			}
		}
		if (addGolem)
			AddMissile(Players[pnum].position.tile, { p->_mx, p->_my }, p->_mdir, MIS_GOLEM, TARGET_MONSTERS, pnum, 0, 1);
	}

	return sizeof(*p);
}

DWORD OnMonstDamage(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdMonDamage *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p)); // BUGFIX: change to sizeof(*p) or it still uses TCmdParam2 size for hellfire (fixed)
	else if (pnum != MyPlayerId) {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel) {
			auto &monster = Monsters[p->wMon];
			monster.mWhoHit |= 1 << pnum;
			if (monster._mhitpoints > 0) {
				monster._mhitpoints -= p->dwDam;
				if ((monster._mhitpoints >> 6) < 1)
					monster._mhitpoints = 1 << 6;
				delta_monster_hp(p->wMon, monster._mhitpoints, playerLevel);
			}
		}
	}

	return sizeof(*p);
}

DWORD OnPlayerDeath(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (pnum != MyPlayerId)
		StartPlayerKill(pnum, p->wParam1);
	else
		CheckUpdatePlayer(pnum);

	return sizeof(*p);
}

DWORD OnPlayerDamage(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdDamage *)pCmd;

	if (p->bPlr == MyPlayerId && currlevel != 0 && gbBufferMsgs != 1) {
		if (currlevel == player.plrlevel && p->dwDam <= 192000 && Players[MyPlayerId]._pHitPoints >> 6 > 0) {
			ApplyPlrDamage(MyPlayerId, 0, 0, p->dwDam, 1);
		}
	}

	return sizeof(*p);
}

DWORD OnOpenDoor(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncOpObject(pnum, CMD_OPENDOOR, p->wParam1);
		DeltaSyncObject(p->wParam1, CMD_OPENDOOR, playerLevel);
	}

	return sizeof(*p);
}

DWORD OnCloseDoor(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncOpObject(pnum, CMD_CLOSEDOOR, p->wParam1);
		DeltaSyncObject(p->wParam1, CMD_CLOSEDOOR, playerLevel);
	}

	return sizeof(*p);
}

DWORD OnOperateObject(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncOpObject(pnum, CMD_OPERATEOBJ, p->wParam1);
		DeltaSyncObject(p->wParam1, CMD_OPERATEOBJ, playerLevel);
	}

	return sizeof(*p);
}

DWORD OnPlayerOperateObject(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncOpObject(p->wParam1, CMD_PLROPOBJ, p->wParam2);
		DeltaSyncObject(p->wParam2, CMD_PLROPOBJ, playerLevel);
	}

	return sizeof(*p);
}

DWORD OnBreakObject(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		int playerLevel = Players[pnum].plrlevel;
		if (currlevel == playerLevel)
			SyncBreakObj(p->wParam1, p->wParam2);
		DeltaSyncObject(p->wParam2, CMD_BREAKOBJ, playerLevel);
	}

	return sizeof(*p);
}

DWORD OnChangePlayerItems(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdChItem *)pCmd;
	auto bodyLocation = static_cast<inv_body_loc>(p->bLoc);

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (pnum != MyPlayerId)
		CheckInvSwap(pnum, p->bLoc, p->wIndx, p->wCI, p->dwSeed, p->bId != 0, p->dwBuff);

	Players[pnum].ReadySpellFromEquipment(bodyLocation);

	return sizeof(*p);
}

DWORD OnDeletePlayerItems(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdDelItem *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (pnum != MyPlayerId)
		inv_update_rem_item(pnum, p->bLoc);

	return sizeof(*p);
}

DWORD OnPlayerLevel(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= MAXCHARLEVEL && pnum != MyPlayerId)
		Players[pnum]._pLevel = p->wParam1;

	return sizeof(*p);
}

DWORD OnDropItem(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else
		DeltaPutItem(p, p->x, p->y, Players[pnum].plrlevel);

	return sizeof(*p);
}

DWORD OnSendPlayerInfo(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPlrInfoHdr *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, p->wBytes + sizeof(*p));
	else
		recv_plrinfo(pnum, p, p->bCmd == CMD_ACK_PLRINFO);

	return p->wBytes + sizeof(*p);
}

DWORD OnPlayerJoinLevel(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
		return sizeof(*p);
	}

	auto &player = Players[pnum];

	player._pLvlChanging = false;
	if (player._pName[0] != 0 && !player.plractive) {
		ResetPlayerGFX(player);
		player.plractive = true;
		gbActivePlayers++;
		EventPlrMsg(fmt::format(_("Player '{:s}' (level {:d}) just joined the game"), player._pName, player._pLevel).c_str());
	}

	if (player.plractive && MyPlayerId != pnum) {
		player.position.tile = { p->x, p->y };
		player.plrlevel = p->wParam1;
		ResetPlayerGFX(player);
		if (currlevel == player.plrlevel) {
			SyncInitPlr(pnum);
			if ((player._pHitPoints >> 6) > 0) {
				StartStand(pnum, DIR_S);
			} else {
				player._pgfxnum = 0;
				player._pmode = PM_DEATH;
				NewPlrAnim(player, player_graphic::Death, DIR_S, player._pDFrames, 1);
				player.AnimInfo.CurrentFrame = player.AnimInfo.NumberOfFrames - 1;
				dFlags[player.position.tile.x][player.position.tile.y] |= BFLAG_DEAD_PLAYER;
			}

			player._pvid = AddVision(player.position.tile, player._pLightRad, pnum == MyPlayerId);
			player._plid = NO_LIGHT;
		}
	}

	return sizeof(*p);
}

DWORD OnActivatePortal(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam3 *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		ActivatePortal(pnum, p->x, p->y, p->wParam1, static_cast<dungeon_type>(p->wParam2), p->wParam3 != 0);
		if (pnum != MyPlayerId) {
			if (currlevel == 0)
				AddInTownPortal(pnum);
			else if (currlevel == Players[pnum].plrlevel) {
				bool addPortal = true;
				for (int i = 0; i < ActiveMissileCount; i++) {
					int mi = ActiveMissiles[i];
					auto &missile = Missiles[mi];
					if (missile._mitype == MIS_TOWN && missile._misource == pnum) {
						addPortal = false;
						// CODEFIX: break
					}
				}
				if (addPortal)
					AddWarpMissile(pnum, p->x, p->y);
			} else {
				RemovePortalMissile(pnum);
			}
		}
		DeltaOpenPortal(pnum, p->x, p->y, p->wParam1, static_cast<dungeon_type>(p->wParam2), p->wParam3 != 0);
	}

	return sizeof(*p);
}

DWORD OnDeactivatePortal(TCmd *pCmd, int pnum)
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

DWORD OnRestartTown(TCmd *pCmd, int pnum)
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

DWORD OnSetStrength(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != MyPlayerId)
		SetPlrStr(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnSetDexterity(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != MyPlayerId)
		SetPlrDex(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnSetMagic(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != MyPlayerId)
		SetPlrMag(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnSetVitality(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		SendPacket(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != MyPlayerId)
		SetPlrVit(pnum, p->wParam1);

	return sizeof(*p);
}

DWORD OnString(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdString *)pCmd;

	int len = strlen(p->str);
	if (gbBufferMsgs == 0)
		SendPlrMsg(pnum, p->str);

	return len + 2; // length of string + nul terminator + sizeof(p->bCmd)
}

DWORD OnSyncQuest(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdQuest *)pCmd;

	if (gbBufferMsgs == 1) {
		SendPacket(pnum, p, sizeof(*p));
	} else {
		if (pnum != MyPlayerId)
			SetMultiQuest(p->q, p->qstate, p->qlog != 0, p->qvar1);
		sgbDeltaChanged = true;
	}

	return sizeof(*p);
}

DWORD OnCheatExperience(TCmd *pCmd, int pnum) // NOLINT(misc-unused-parameters)
{
#ifdef _DEBUG
	if (gbBufferMsgs == 1)
		SendPacket(pnum, pCmd, sizeof(*pCmd));
	else if (Players[pnum]._pLevel < MAXCHARLEVEL - 1) {
		Players[pnum]._pExperience = Players[pnum]._pNextExper;
		if (sgOptions.Gameplay.bExperienceBar) {
			force_redraw = 255;
		}
		NextPlrLevel(pnum);
	}
#endif
	return sizeof(*pCmd);
}

DWORD OnCheatSpellLevel(TCmd *pCmd, int pnum) // NOLINT(misc-unused-parameters)
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

DWORD OnDebug(TCmd *pCmd)
{
	return sizeof(*pCmd);
}

DWORD OnNova(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1) {
		auto &player = Players[pnum];
		if (currlevel == player.plrlevel && pnum != MyPlayerId) {
			ClrPlrPath(player);
			player._pSpell = SPL_NOVA;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 3;
			player.destAction = ACTION_SPELL;
			player.destParam1 = p->x;
			player.destParam2 = p->y;
		}
	}

	return sizeof(*p);
}

DWORD OnSetShield(TCmd *pCmd, PlayerStruct &player)
{
	if (gbBufferMsgs != 1)
		player.pManaShield = true;

	return sizeof(*pCmd);
}

DWORD OnRemoveShield(TCmd *pCmd, PlayerStruct &player)
{
	if (gbBufferMsgs != 1)
		player.pManaShield = false;

	return sizeof(*pCmd);
}

DWORD OnEndShield(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1 && pnum != MyPlayerId && currlevel == Players[pnum].plrlevel) {
		for (int i = 0; i < ActiveMissileCount; i++) {
			int mi = ActiveMissiles[i];
			auto &missile = Missiles[mi];
			if (missile._mitype == MIS_MANASHIELD && missile._misource == pnum) {
				ClearMissileSpot(missile.position.tile);
				DeleteMissile(mi, i);
			}
		}
	}

	return sizeof(*pCmd);
}

DWORD OnSetReflect(TCmd *pCmd, PlayerStruct &player)
{
	auto *p = (TCmdParam1 *)pCmd;
	if (gbBufferMsgs != 1)
		player.wReflections = p->wParam1;

	return sizeof(*p);
}

DWORD OnRemoveReflect(TCmd *pCmd, PlayerStruct &player)
{
	if (gbBufferMsgs != 1)
		player.wReflections = 0;

	return sizeof(*pCmd);
}

DWORD OnEndReflect(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1 && pnum != MyPlayerId && currlevel == Players[pnum].plrlevel) {
		for (int i = 0; i < ActiveMissileCount; i++) {
			int mi = ActiveMissiles[i];
			auto &missile = Missiles[mi];
			if (missile._mitype == MIS_REFLECT && missile._misource == pnum) {
				ClearMissileSpot(missile.position.tile);
				DeleteMissile(mi, i);
			}
		}
	}

	return sizeof(*pCmd);
}

DWORD OnNakrul(TCmd *pCmd)
{
	if (gbBufferMsgs != 1) {
		OperateNakrulLever();
		IsUberRoomOpened = true;
		Quests[Q_NAKRUL]._qactive = QUEST_DONE;
		monster_some_crypt();
	}
	return sizeof(*pCmd);
}

DWORD OnOpenHive(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam2 *)pCmd;
	if (gbBufferMsgs != 1) {
		AddMissile({ p->x, p->y }, { p->wParam1, p->wParam2 }, 0, MIS_HIVEEXP2, TARGET_MONSTERS, pnum, 0, 0);
		TownOpenHive();
		InitTownTriggers();
	}
	return sizeof(*p);
}

DWORD OnOpenCrypt(TCmd *pCmd)
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

void delta_sync_monster(const TSyncMonster *pSync, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;

	assert(pSync != nullptr);
	assert(bLevel < NUMLEVELS);
	sgbDeltaChanged = true;

	DMonsterStr *pD = &sgLevels[bLevel].monster[pSync->_mndx];
	if (pD->_mhitpoints == 0)
		return;

	pD->_mx = pSync->_mx;
	pD->_my = pSync->_my;
	pD->_mactive = UINT8_MAX;
	pD->_menemy = pSync->_menemy;
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

	TCmdPItem *pD = sgLevels[currlevel].item;
	for (int i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != CMD_INVALID
		    && pD->wIndx == Items[ii].IDidx
		    && pD->wCI == Items[ii]._iCreateInfo
		    && pD->dwSeed == Items[ii]._iSeed
		    && (pD->bCmd == CMD_WALKXY || pD->bCmd == CMD_STAND)) {
			return;
		}
	}

	pD = sgLevels[currlevel].item;
	for (int i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != CMD_INVALID)
			continue;

		sgbDeltaChanged = true;
		pD->bCmd = CMD_STAND;
		pD->x = Items[ii].position.x;
		pD->y = Items[ii].position.y;
		pD->wIndx = Items[ii].IDidx;
		pD->wCI = Items[ii]._iCreateInfo;
		pD->dwSeed = Items[ii]._iSeed;
		pD->bId = Items[ii]._iIdentified ? 1 : 0;
		pD->bDur = Items[ii]._iDurability;
		pD->bMDur = Items[ii]._iMaxDur;
		pD->bCh = Items[ii]._iCharges;
		pD->bMCh = Items[ii]._iMaxCharges;
		pD->wValue = Items[ii]._ivalue;
		pD->wToHit = Items[ii]._iPLToHit;
		pD->wMaxDam = Items[ii]._iMaxDam;
		pD->bMinStr = Items[ii]._iMinStr;
		pD->bMinMag = Items[ii]._iMinMag;
		pD->bMinDex = Items[ii]._iMinDex;
		pD->bAC = Items[ii]._iAC;
		pD->dwBuff = Items[ii].dwBuff;
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

} //namespace

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
			if (sgLevels[currlevel].monster[i]._mhitpoints != -1)
				monster._mhitpoints = sgLevels[currlevel].monster[i]._mhitpoints;
			if (sgLevels[currlevel].monster[i]._mhitpoints == 0) {
				M_ClearSquares(i);
				if (monster._mAi != AI_DIABLO) {
					if (monster._uniqtype == 0) {
						assert(monster.MType != nullptr);
						AddDead(monster.position.tile, monster.MType->mdeadval, monster._mdir);
					} else {
						AddDead(monster.position.tile, monster._udeadval, monster._mdir);
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
			int ii = FindGetItem(
			    sgLevels[currlevel].item[i].wIndx,
			    sgLevels[currlevel].item[i].wCI,
			    sgLevels[currlevel].item[i].dwSeed);
			if (ii != -1) {
				if (dItem[Items[ii].position.x][Items[ii].position.y] == ii + 1)
					dItem[Items[ii].position.x][Items[ii].position.y] = 0;
				DeleteItem(ii, i);
			}
		}
		if (sgLevels[currlevel].item[i].bCmd == CMD_ACK_PLRINFO) {
			int ii = AllocateItem();

			if (sgLevels[currlevel].item[i].wIndx == IDI_EAR) {
				RecreateEar(
				    ii,
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
				    ii,
				    sgLevels[currlevel].item[i].wIndx,
				    sgLevels[currlevel].item[i].wCI,
				    sgLevels[currlevel].item[i].dwSeed,
				    sgLevels[currlevel].item[i].wValue,
				    (sgLevels[currlevel].item[i].dwBuff & CF_HELLFIRE) != 0);
				if (sgLevels[currlevel].item[i].bId != 0)
					Items[ii]._iIdentified = true;
				Items[ii]._iDurability = sgLevels[currlevel].item[i].bDur;
				Items[ii]._iMaxDur = sgLevels[currlevel].item[i].bMDur;
				Items[ii]._iCharges = sgLevels[currlevel].item[i].bCh;
				Items[ii]._iMaxCharges = sgLevels[currlevel].item[i].bMCh;
				Items[ii]._iPLToHit = sgLevels[currlevel].item[i].wToHit;
				Items[ii]._iMaxDam = sgLevels[currlevel].item[i].wMaxDam;
				Items[ii]._iMinStr = sgLevels[currlevel].item[i].bMinStr;
				Items[ii]._iMinMag = sgLevels[currlevel].item[i].bMinMag;
				Items[ii]._iMinDex = sgLevels[currlevel].item[i].bMinDex;
				Items[ii]._iAC = sgLevels[currlevel].item[i].bAC;
				Items[ii].dwBuff = sgLevels[currlevel].item[i].dwBuff;
			}
			int x = sgLevels[currlevel].item[i].x;
			int y = sgLevels[currlevel].item[i].y;
			Items[ii].position = GetItemPosition({ x, y });
			dItem[Items[ii].position.x][Items[ii].position.y] = ii + 1;
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
				SyncBreakObj(-1, i);
				break;
			default:
				break;
			}
		}

		for (int i = 0; i < ActiveObjectCount; i++) {
			int ot = Objects[ActiveObjects[i]]._otype;
			if (ot == OBJ_TRAPL || ot == OBJ_TRAPR)
				Obj_Trap(ActiveObjects[i]);
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

void NetSendCmdQuest(bool bHiPri, const QuestStruct &quest)
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
	int dwStrLen;
	TCmdString cmd;

	dwStrLen = strlen(pszStr);
	cmd.bCmd = CMD_STRING;
	strcpy(cmd.str, pszStr);
	multi_send_msg_packet(pmask, (byte *)&cmd.bCmd, dwStrLen + 2);
}

void delta_close_portal(int pnum)
{
	memset(&sgJunk.portal[pnum], 0xFF, sizeof(sgJunk.portal[pnum]));
	sgbDeltaChanged = true;
}

DWORD ParseCmd(int pnum, TCmd *pCmd)
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
	case CMD_ENDSHIELD:
		return OnEndShield(pCmd, pnum);
	case CMD_SETREFLECT:
		return OnSetReflect(pCmd, player);
	case CMD_REMREFLECT:
		return OnRemoveReflect(pCmd, player);
	case CMD_ENDREFLECT:
		return OnEndReflect(pCmd, pnum);
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
