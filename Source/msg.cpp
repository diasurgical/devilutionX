/**
 * @file msg.cpp
 *
 * Implementation of function for sending and reciving network messages.
 */
#include <climits>
#include <memory>

#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "automap.h"
#include "control.h"
#include "dead.h"
#include "drlg_l1.h"
#include "dthread.h"
#include "encrypt.h"
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

#define MAX_CHUNKS (NUMLEVELS + 4)

static DWORD sgdwOwnerWait;
static DWORD sgdwRecvOffset;
static int sgnCurrMegaPlayer;
static DLevel sgLevels[NUMLEVELS];
static BYTE sbLastCmd;
static TMegaPkt *sgpCurrPkt;
static byte sgRecvBuf[sizeof(DLevel) + 1];
static BYTE sgbRecvCmd;
static LocalLevel sgLocals[NUMLEVELS];
static DJunk sgJunk;
static TMegaPkt *sgpMegaPkt;
static bool sgbDeltaChanged;
static BYTE sgbDeltaChunks;
bool deltaload;
BYTE gbBufferMsgs;
int dwRecCount;

static void msg_get_next_packet()
{
	TMegaPkt *result;

	sgpCurrPkt = static_cast<TMegaPkt *>(std::malloc(sizeof(TMegaPkt)));
	sgpCurrPkt->pNext = nullptr;
	sgpCurrPkt->dwSpaceLeft = sizeof(result->data);

	result = (TMegaPkt *)&sgpMegaPkt;
	while (result->pNext != nullptr)
		result = result->pNext;

	result->pNext = sgpCurrPkt;
}

static void msg_free_packets()
{
	while (sgpMegaPkt != nullptr) {
		sgpCurrPkt = sgpMegaPkt->pNext;
		std::free(sgpMegaPkt);
		sgpMegaPkt = sgpCurrPkt;
	}
}

static void msg_pre_packet()
{
	uint8_t playerId = -1;
	for (TMegaPkt *pkt = sgpMegaPkt; pkt != nullptr; pkt = pkt->pNext) {
		byte *data = pkt->data;
		size_t spaceLeft = sizeof(pkt->data);
		while (spaceLeft != pkt->dwSpaceLeft) {
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

			int pktSize = ParseCmd(playerId, (TCmd *)data);
			data += pktSize;
			spaceLeft -= pktSize;
		}
	}
}

static void msg_send_packet(int pnum, const void *packet, DWORD dwSize)
{
	TFakeCmdPlr cmd;

	if (pnum != sgnCurrMegaPlayer) {
		sgnCurrMegaPlayer = pnum;
		cmd.bCmd = FAKE_CMD_SETID;
		cmd.bPlr = pnum;
		msg_send_packet(pnum, &cmd, sizeof(cmd));
	}
	if (sgpCurrPkt->dwSpaceLeft < dwSize)
		msg_get_next_packet();

	memcpy(sgpCurrPkt->data + sizeof(sgpCurrPkt->data) - sgpCurrPkt->dwSpaceLeft, packet, dwSize);
	sgpCurrPkt->dwSpaceLeft -= dwSize;
}

void msg_send_drop_pkt(int pnum, int reason)
{
	TFakeDropPlr cmd;

	cmd.dwReason = reason;
	cmd.bCmd = FAKE_CMD_DROPID;
	cmd.bPlr = pnum;
	msg_send_packet(pnum, &cmd, sizeof(cmd));
}

static int msg_wait_for_turns()
{
	bool received;
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
	if (nthread_has_500ms_passed())
		nthread_recv_turns(&received);

	if (gbGameDestroyed)
		return 100;
	if (gbDeltaSender >= MAX_PLRS) {
		sgbDeltaChunks = 0;
		sgbRecvCmd = CMD_DLEVEL_END;
		gbDeltaSender = myplr;
		nthread_set_turn_upper_bit();
	}
	if (sgbDeltaChunks == MAX_CHUNKS - 1) {
		sgbDeltaChunks = MAX_CHUNKS;
		return 99;
	}
	return 100 * sgbDeltaChunks / MAX_CHUNKS;
}

bool msg_wait_resync()
{
	bool success;

	msg_get_next_packet();
	sgbDeltaChunks = 0;
	sgnCurrMegaPlayer = -1;
	sgbRecvCmd = CMD_DLEVEL_END;
	gbBufferMsgs = 1;
	sgdwOwnerWait = SDL_GetTicks();
	success = UiProgressDialog(_("Waiting for game data..."), msg_wait_for_turns);
	gbBufferMsgs = 0;
	if (!success) {
		msg_free_packets();
		return false;
	}

	if (gbGameDestroyed) {
		DrawDlg("%s", _("The game ended"));
		msg_free_packets();
		return false;
	}

	if (sgbDeltaChunks != MAX_CHUNKS) {
		DrawDlg("%s", _("Unable to get level data"));
		msg_free_packets();
		return false;
	}

	return true;
}

void run_delta_info()
{
	if (!gbIsMultiplayer)
		return;

	gbBufferMsgs = 2;
	msg_pre_packet();
	gbBufferMsgs = 0;
	msg_free_packets();
}

static byte *DeltaExportItem(byte *dst, TCmdPItem *src)
{
	for (int i = 0; i < MAXITEMS; i++, src++) {
		if (src->bCmd == 0xFF)
			*dst++ = byte { 0xFF };
		else {
			memcpy(dst, src, sizeof(TCmdPItem));
			dst += sizeof(TCmdPItem);
		}
	}

	return dst;
}

static byte *DeltaImportItem(byte *src, TCmdPItem *dst)
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

static byte *DeltaExportObject(byte *dst, DObjectStr *src)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return dst + sizeof(DObjectStr) * MAXOBJECTS;
}

static byte *DeltaImportObject(byte *src, DObjectStr *dst)
{
	memcpy(dst, src, sizeof(DObjectStr) * MAXOBJECTS);
	return src + sizeof(DObjectStr) * MAXOBJECTS;
}

static byte *DeltaExportMonster(byte *dst, DMonsterStr *src)
{
	for (int i = 0; i < MAXMONSTERS; i++, src++) {
		if (src->_mx == 0xFF)
			*dst++ = byte { 0xFF };
		else {
			memcpy(dst, src, sizeof(DMonsterStr));
			dst += sizeof(DMonsterStr);
		}
	}

	return dst;
}

static byte *DeltaImportMonster(byte *src, DMonsterStr *dst)
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

static byte *DeltaExportJunk(byte *dst)
{
	int i, q;

	for (i = 0; i < MAXPORTAL; i++) {
		if (sgJunk.portal[i].x == 0xFF) {
			*dst++ = byte { 0xFF };
		} else {
			memcpy(dst, &sgJunk.portal[i], sizeof(DPortal));
			dst += sizeof(DPortal);
		}
	}

	for (i = 0, q = 0; i < MAXQUESTS; i++) {
		if (!questlist[i].isSinglePlayerOnly) {
			sgJunk.quests[q].qlog = quests[i]._qlog;
			sgJunk.quests[q].qstate = quests[i]._qactive;
			sgJunk.quests[q].qvar1 = quests[i]._qvar1;
			memcpy(dst, &sgJunk.quests[q], sizeof(MultiQuests));
			dst += sizeof(MultiQuests);
			q++;
		}
	}

	return dst;
}

static void DeltaImportJunk(byte *src)
{
	int i, q;

	for (i = 0; i < MAXPORTAL; i++) {
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

	for (i = 0, q = 0; i < MAXQUESTS; i++) {
		if (!questlist[i].isSinglePlayerOnly) {
			memcpy(&sgJunk.quests[q], src, sizeof(MultiQuests));
			src += sizeof(MultiQuests);
			quests[i]._qlog = sgJunk.quests[q].qlog;
			quests[i]._qactive = sgJunk.quests[q].qstate;
			quests[i]._qvar1 = sgJunk.quests[q].qvar1;
			q++;
		}
	}
}

static DWORD msg_comp_level(byte *buffer, byte *end)
{
	DWORD size = end - buffer - 1;
	DWORD pkSize = PkwareCompress(buffer + 1, size);

	*buffer = size != pkSize ? byte { 1 } : byte { 0 };

	return pkSize + 1;
}

void DeltaExportData(int pnum)
{
	if (sgbDeltaChanged) {
		int size;
		std::unique_ptr<byte[]> dst { new byte[sizeof(DLevel) + 1] };
		byte *dstEnd;
		for (int i = 0; i < NUMLEVELS; i++) {
			dstEnd = &dst[1];
			dstEnd = DeltaExportItem(dstEnd, sgLevels[i].item);
			dstEnd = DeltaExportObject(dstEnd, sgLevels[i].object);
			dstEnd = DeltaExportMonster(dstEnd, sgLevels[i].monster);
			size = msg_comp_level(dst.get(), dstEnd);
			dthread_send_delta(pnum, static_cast<_cmd_id>(i + CMD_DLEVEL_0), dst.get(), size);
		}
		dstEnd = &dst[1];
		dstEnd = DeltaExportJunk(dstEnd);
		size = msg_comp_level(dst.get(), dstEnd);
		dthread_send_delta(pnum, CMD_DLEVEL_JUNK, dst.get(), size);
	}
	byte src { 0 };
	dthread_send_delta(pnum, CMD_DLEVEL_END, &src, 1);
}

static void DeltaImportData(BYTE cmd, DWORD recv_offset)
{
	if (sgRecvBuf[0] != byte { 0 })
		PkwareDecompress(&sgRecvBuf[1], recv_offset, sizeof(sgRecvBuf) - 1);

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

static DWORD On_DLEVEL(int pnum, TCmd *pCmd)
{
	auto *p = (TCmdPlrInfoHdr *)pCmd;

	if (gbDeltaSender != pnum) {
		if (p->bCmd == CMD_DLEVEL_END) {
			gbDeltaSender = pnum;
			sgbRecvCmd = CMD_DLEVEL_END;
		} else if (p->bCmd == CMD_DLEVEL_0 && p->wOffset == 0) {
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
	pD->_mdir = monster[mi]._mdir;
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

void delta_sync_golem(TCmdGolem *pG, int pnum, BYTE bLevel)
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

void delta_leave_sync(BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;
	if (currlevel == 0)
		glSeedTbl[0] = AdvanceRndSeed();
	if (currlevel <= 0)
		return;

	for (int i = 0; i < nummonsters; i++) {
		int ma = monstactive[i];
		if (monster[ma]._mhitpoints == 0)
			continue;
		sgbDeltaChanged = true;
		DMonsterStr *pD = &sgLevels[bLevel].monster[ma];
		pD->_mx = monster[ma].position.tile.x;
		pD->_my = monster[ma].position.tile.y;
		pD->_mdir = monster[ma]._mdir;
		pD->_menemy = encode_enemy(ma);
		pD->_mhitpoints = monster[ma]._mhitpoints;
		pD->_mactive = monster[ma]._msquelch;
	}
	memcpy(&sgLocals[bLevel].automapsv, AutomapView, sizeof(AutomapView));
}

static void delta_sync_object(int oi, _cmd_id bCmd, BYTE bLevel)
{
	if (!gbIsMultiplayer)
		return;

	sgbDeltaChanged = true;
	sgLevels[bLevel].object[oi].bCmd = bCmd;
}

static bool delta_get_item(TCmdGItem *pI, BYTE bLevel)
{
	int i;

	if (!gbIsMultiplayer)
		return true;

	TCmdPItem *pD = sgLevels[bLevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
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
	for (i = 0; i < MAXITEMS; i++, pD++) {
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

static void delta_put_item(TCmdPItem *pI, int x, int y, BYTE bLevel)
{
	int i;

	if (!gbIsMultiplayer)
		return;

	TCmdPItem *pD = sgLevels[bLevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != CMD_WALKXY
		    && pD->bCmd != 0xFF
		    && pD->wIndx == pI->wIndx
		    && pD->wCI == pI->wCI
		    && pD->dwSeed == pI->dwSeed) {
			if (pD->bCmd == CMD_ACK_PLRINFO)
				return;
			app_fatal("%s", _("Trying to drop a floor item?"));
		}
	}

	pD = sgLevels[bLevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == 0xFF) {
			sgbDeltaChanged = true;
			memcpy(pD, pI, sizeof(TCmdPItem));
			pD->bCmd = CMD_ACK_PLRINFO;
			pD->x = x;
			pD->y = y;
			return;
		}
	}
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
	int i;

	if (!gbIsMultiplayer)
		return;

	TCmdPItem *pD = sgLevels[currlevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd != 0xFF
		    && pD->wIndx == items[ii].IDidx
		    && pD->wCI == items[ii]._iCreateInfo
		    && pD->dwSeed == items[ii]._iSeed
		    && (pD->bCmd == CMD_WALKXY || pD->bCmd == CMD_STAND)) {
			return;
		}
	}

	pD = sgLevels[currlevel].item;
	for (i = 0; i < MAXITEMS; i++, pD++) {
		if (pD->bCmd == 0xFF) {
			sgbDeltaChanged = true;
			pD->bCmd = CMD_STAND;
			pD->x = items[ii].position.x;
			pD->y = items[ii].position.y;
			pD->wIndx = items[ii].IDidx;
			pD->wCI = items[ii]._iCreateInfo;
			pD->dwSeed = items[ii]._iSeed;
			pD->bId = items[ii]._iIdentified;
			pD->bDur = items[ii]._iDurability;
			pD->bMDur = items[ii]._iMaxDur;
			pD->bCh = items[ii]._iCharges;
			pD->bMCh = items[ii]._iMaxCharges;
			pD->wValue = items[ii]._ivalue;
			pD->wToHit = items[ii]._iPLToHit;
			pD->wMaxDam = items[ii]._iMaxDam;
			pD->bMinStr = items[ii]._iMinStr;
			pD->bMinMag = items[ii]._iMinMag;
			pD->bMinDex = items[ii]._iMinDex;
			pD->bAC = items[ii]._iAC;
			pD->dwBuff = items[ii].dwBuff;
			return;
		}
	}
}

void DeltaSaveLevel()
{
	if (!gbIsMultiplayer)
		return;

	for (int i = 0; i < MAX_PLRS; i++) {
		if (i != myplr)
			ResetPlayerGFX(plr[i]);
	}
	plr[myplr]._pLvlVisited[currlevel] = true;
	delta_leave_sync(currlevel);
}

void DeltaLoadLevel()
{
	int ot;
	int i, j, k, l;
	int x, y, xx, yy;
	bool done;

	if (!gbIsMultiplayer)
		return;

	deltaload = true;
	if (currlevel != 0) {
		for (i = 0; i < nummonsters; i++) {
			if (sgLevels[currlevel].monster[i]._mx != 0xFF) {
				M_ClearSquares(i);
				x = sgLevels[currlevel].monster[i]._mx;
				y = sgLevels[currlevel].monster[i]._my;
				monster[i].position.tile = { x, y };
				monster[i].position.old = { x, y };
				monster[i].position.future = { x, y };
				if (sgLevels[currlevel].monster[i]._mhitpoints != -1)
					monster[i]._mhitpoints = sgLevels[currlevel].monster[i]._mhitpoints;
				if (sgLevels[currlevel].monster[i]._mhitpoints == 0) {
					M_ClearSquares(i);
					if (monster[i]._mAi != AI_DIABLO) {
						if (monster[i]._uniqtype == 0) {
							assert(monster[i].MType != nullptr);
							AddDead(monster[i].position.tile, monster[i].MType->mdeadval, monster[i]._mdir);
						} else {
							AddDead(monster[i].position.tile, monster[i]._udeadval, monster[i]._mdir);
						}
					}
					monster[i]._mDelFlag = true;
					M_UpdateLeader(i);
				} else {
					decode_enemy(i, sgLevels[currlevel].monster[i]._menemy);
					if ((monster[i].position.tile.x && monster[i].position.tile.x != 1) || monster[i].position.tile.y)
						dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = i + 1;
					if (i < MAX_PLRS) {
						MAI_Golum(i);
						monster[i]._mFlags |= (MFLAG_TARGETS_MONSTER | MFLAG_GOLEM);
					} else {
						M_StartStand(i, monster[i]._mdir);
					}
					monster[i]._msquelch = sgLevels[currlevel].monster[i]._mactive;
				}
			}
		}
		memcpy(AutomapView, &sgLocals[currlevel], sizeof(AutomapView));
	}

	for (i = 0; i < MAXITEMS; i++) {
		if (sgLevels[currlevel].item[i].bCmd != 0xFF) {
			if (sgLevels[currlevel].item[i].bCmd == CMD_WALKXY) {
				int ii = FindGetItem(
				    sgLevels[currlevel].item[i].wIndx,
				    sgLevels[currlevel].item[i].wCI,
				    sgLevels[currlevel].item[i].dwSeed);
				if (ii != -1) {
					if (dItem[items[ii].position.x][items[ii].position.y] == ii + 1)
						dItem[items[ii].position.x][items[ii].position.y] = 0;
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
					    static_cast<_item_indexes>(sgLevels[currlevel].item[i].wIndx),
					    sgLevels[currlevel].item[i].wCI,
					    sgLevels[currlevel].item[i].dwSeed,
					    sgLevels[currlevel].item[i].wValue,
					    (sgLevels[currlevel].item[i].dwBuff & CF_HELLFIRE) != 0);
					if (sgLevels[currlevel].item[i].bId)
						items[ii]._iIdentified = true;
					items[ii]._iDurability = sgLevels[currlevel].item[i].bDur;
					items[ii]._iMaxDur = sgLevels[currlevel].item[i].bMDur;
					items[ii]._iCharges = sgLevels[currlevel].item[i].bCh;
					items[ii]._iMaxCharges = sgLevels[currlevel].item[i].bMCh;
					items[ii]._iPLToHit = sgLevels[currlevel].item[i].wToHit;
					items[ii]._iMaxDam = sgLevels[currlevel].item[i].wMaxDam;
					items[ii]._iMinStr = sgLevels[currlevel].item[i].bMinStr;
					items[ii]._iMinMag = sgLevels[currlevel].item[i].bMinMag;
					items[ii]._iMinDex = sgLevels[currlevel].item[i].bMinDex;
					items[ii]._iAC = sgLevels[currlevel].item[i].bAC;
					items[ii].dwBuff = sgLevels[currlevel].item[i].dwBuff;
				}
				x = sgLevels[currlevel].item[i].x;
				y = sgLevels[currlevel].item[i].y;
				if (!CanPut({ x, y })) {
					done = false;
					for (k = 1; k < 50 && !done; k++) {
						for (j = -k; j <= k && !done; j++) {
							yy = y + j;
							for (l = -k; l <= k && !done; l++) {
								xx = x + l;
								if (CanPut({ xx, yy })) {
									done = true;
									x = xx;
									y = yy;
								}
							}
						}
					}
				}
				items[ii].position = { x, y };
				dItem[items[ii].position.x][items[ii].position.y] = ii + 1;
				RespawnItem(&items[ii], false);
			}
		}
	}

	if (currlevel != 0) {
		for (i = 0; i < MAXOBJECTS; i++) {
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

		for (i = 0; i < nobjects; i++) {
			ot = object[objectactive[i]]._otype;
			if (ot == OBJ_TRAPL || ot == OBJ_TRAPR)
				Obj_Trap(objectactive[i]);
		}
	}
	deltaload = false;
}

void NetSendCmd(bool bHiPri, _cmd_id bCmd)
{
	TCmd cmd;

	cmd.bCmd = bCmd;
	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
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
	NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
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
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
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
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
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
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdParam1(bool bHiPri, _cmd_id bCmd, uint16_t wParam1)
{
	TCmdParam1 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdParam2(bool bHiPri, _cmd_id bCmd, uint16_t wParam1, uint16_t wParam2)
{
	TCmdParam2 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdParam3(bool bHiPri, _cmd_id bCmd, uint16_t wParam1, uint16_t wParam2, uint16_t wParam3)
{
	TCmdParam3 cmd;

	cmd.bCmd = bCmd;
	cmd.wParam1 = wParam1;
	cmd.wParam2 = wParam2;
	cmd.wParam3 = wParam3;
	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdQuest(bool bHiPri, BYTE q)
{
	TCmdQuest cmd;

	cmd.q = q;
	cmd.bCmd = CMD_SYNCQUEST;
	cmd.qstate = quests[q]._qactive;
	cmd.qlog = quests[q]._qlog;
	cmd.qvar1 = quests[q]._qvar1;
	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
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
	cmd.x = items[ii].position.x;
	cmd.y = items[ii].position.y;
	cmd.wIndx = items[ii].IDidx;

	if (items[ii].IDidx == IDI_EAR) {
		cmd.wCI = items[ii]._iName[8] | (items[ii]._iName[7] << 8);
		cmd.dwSeed = items[ii]._iName[12] | ((items[ii]._iName[11] | ((items[ii]._iName[10] | (items[ii]._iName[9] << 8)) << 8)) << 8);
		cmd.bId = items[ii]._iName[13];
		cmd.bDur = items[ii]._iName[14];
		cmd.bMDur = items[ii]._iName[15];
		cmd.bCh = items[ii]._iName[16];
		cmd.bMCh = items[ii]._iName[17];
		cmd.wValue = items[ii]._ivalue | (items[ii]._iName[18] << 8) | ((items[ii]._iCurs - ICURS_EAR_SORCERER) << 6);
		cmd.dwBuff = items[ii]._iName[22] | ((items[ii]._iName[21] | ((items[ii]._iName[20] | (items[ii]._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = items[ii]._iCreateInfo;
		cmd.dwSeed = items[ii]._iSeed;
		cmd.bId = items[ii]._iIdentified;
		cmd.bDur = items[ii]._iDurability;
		cmd.bMDur = items[ii]._iMaxDur;
		cmd.bCh = items[ii]._iCharges;
		cmd.bMCh = items[ii]._iMaxCharges;
		cmd.wValue = items[ii]._ivalue;
		cmd.wToHit = items[ii]._iPLToHit;
		cmd.wMaxDam = items[ii]._iMaxDam;
		cmd.bMinStr = items[ii]._iMinStr;
		cmd.bMinMag = items[ii]._iMinMag;
		cmd.bMinDex = items[ii]._iMinDex;
		cmd.bAC = items[ii]._iAC;
		cmd.dwBuff = items[ii].dwBuff;
	}

	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
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
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
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
	NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdPItem(bool bHiPri, _cmd_id bCmd, Point position)
{
	TCmdPItem cmd;

	cmd.bCmd = bCmd;
	cmd.x = position.x;
	cmd.y = position.y;
	cmd.wIndx = plr[myplr].HoldItem.IDidx;

	if (plr[myplr].HoldItem.IDidx == IDI_EAR) {
		cmd.wCI = plr[myplr].HoldItem._iName[8] | (plr[myplr].HoldItem._iName[7] << 8);
		cmd.dwSeed = plr[myplr].HoldItem._iName[12] | ((plr[myplr].HoldItem._iName[11] | ((plr[myplr].HoldItem._iName[10] | (plr[myplr].HoldItem._iName[9] << 8)) << 8)) << 8);
		cmd.bId = plr[myplr].HoldItem._iName[13];
		cmd.bDur = plr[myplr].HoldItem._iName[14];
		cmd.bMDur = plr[myplr].HoldItem._iName[15];
		cmd.bCh = plr[myplr].HoldItem._iName[16];
		cmd.bMCh = plr[myplr].HoldItem._iName[17];
		cmd.wValue = plr[myplr].HoldItem._ivalue | (plr[myplr].HoldItem._iName[18] << 8) | ((plr[myplr].HoldItem._iCurs - ICURS_EAR_SORCERER) << 6);
		cmd.dwBuff = plr[myplr].HoldItem._iName[22] | ((plr[myplr].HoldItem._iName[21] | ((plr[myplr].HoldItem._iName[20] | (plr[myplr].HoldItem._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = plr[myplr].HoldItem._iCreateInfo;
		cmd.dwSeed = plr[myplr].HoldItem._iSeed;
		cmd.bId = plr[myplr].HoldItem._iIdentified;
		cmd.bDur = plr[myplr].HoldItem._iDurability;
		cmd.bMDur = plr[myplr].HoldItem._iMaxDur;
		cmd.bCh = plr[myplr].HoldItem._iCharges;
		cmd.bMCh = plr[myplr].HoldItem._iMaxCharges;
		cmd.wValue = plr[myplr].HoldItem._ivalue;
		cmd.wToHit = plr[myplr].HoldItem._iPLToHit;
		cmd.wMaxDam = plr[myplr].HoldItem._iMaxDam;
		cmd.bMinStr = plr[myplr].HoldItem._iMinStr;
		cmd.bMinMag = plr[myplr].HoldItem._iMinMag;
		cmd.bMinDex = plr[myplr].HoldItem._iMinDex;
		cmd.bAC = plr[myplr].HoldItem._iAC;
		cmd.dwBuff = plr[myplr].HoldItem.dwBuff;
	}

	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdChItem(bool bHiPri, BYTE bLoc)
{
	TCmdChItem cmd;

	cmd.bCmd = CMD_CHANGEPLRITEMS;
	cmd.bLoc = bLoc;
	cmd.wIndx = plr[myplr].HoldItem.IDidx;
	cmd.wCI = plr[myplr].HoldItem._iCreateInfo;
	cmd.dwSeed = plr[myplr].HoldItem._iSeed;
	cmd.bId = plr[myplr].HoldItem._iIdentified;
	cmd.dwBuff = plr[myplr].HoldItem.dwBuff;

	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdDelItem(bool bHiPri, BYTE bLoc)
{
	TCmdDelItem cmd;

	cmd.bLoc = bLoc;
	cmd.bCmd = CMD_DELPLRITEMS;
	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdDItem(bool bHiPri, int ii)
{
	TCmdPItem cmd;

	cmd.bCmd = CMD_DROPITEM;
	cmd.x = items[ii].position.x;
	cmd.y = items[ii].position.y;
	cmd.wIndx = items[ii].IDidx;

	if (items[ii].IDidx == IDI_EAR) {
		cmd.wCI = items[ii]._iName[8] | (items[ii]._iName[7] << 8);
		cmd.dwSeed = items[ii]._iName[12] | ((items[ii]._iName[11] | ((items[ii]._iName[10] | (items[ii]._iName[9] << 8)) << 8)) << 8);
		cmd.bId = items[ii]._iName[13];
		cmd.bDur = items[ii]._iName[14];
		cmd.bMDur = items[ii]._iName[15];
		cmd.bCh = items[ii]._iName[16];
		cmd.bMCh = items[ii]._iName[17];
		cmd.wValue = items[ii]._ivalue | (items[ii]._iName[18] << 8) | ((items[ii]._iCurs - ICURS_EAR_SORCERER) << 6);
		cmd.dwBuff = items[ii]._iName[22] | ((items[ii]._iName[21] | ((items[ii]._iName[20] | (items[ii]._iName[19] << 8)) << 8)) << 8);
	} else {
		cmd.wCI = items[ii]._iCreateInfo;
		cmd.dwSeed = items[ii]._iSeed;
		cmd.bId = items[ii]._iIdentified;
		cmd.bDur = items[ii]._iDurability;
		cmd.bMDur = items[ii]._iMaxDur;
		cmd.bCh = items[ii]._iCharges;
		cmd.bMCh = items[ii]._iMaxCharges;
		cmd.wValue = items[ii]._ivalue;
		cmd.wToHit = items[ii]._iPLToHit;
		cmd.wMaxDam = items[ii]._iMaxDam;
		cmd.bMinStr = items[ii]._iMinStr;
		cmd.bMinMag = items[ii]._iMinMag;
		cmd.bMinDex = items[ii]._iMinDex;
		cmd.bAC = items[ii]._iAC;
		cmd.dwBuff = items[ii].dwBuff;
	}

	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

static bool i_own_level(int nReqLevel)
{
	int i;

	for (i = 0; i < MAX_PLRS; i++) {
		if (!plr[i].plractive)
			continue;
		if (plr[i]._pLvlChanging)
			continue;
		if (plr[i].plrlevel != nReqLevel)
			continue;
		if (i == myplr && gbBufferMsgs != 0)
			continue;
		break;
	}
	return i == myplr;
}

void NetSendCmdDamage(bool bHiPri, uint8_t bPlr, DWORD dwDam)
{
	TCmdDamage cmd;

	cmd.bCmd = CMD_PLRDAMAGE;
	cmd.bPlr = bPlr;
	cmd.dwDam = dwDam;
	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
}

void NetSendCmdMonDmg(bool bHiPri, uint16_t wMon, DWORD dwDam)
{
	TCmdMonDamage cmd;

	cmd.bCmd = CMD_MONSTDAMAGE;
	cmd.wMon = wMon;
	cmd.dwDam = dwDam;
	if (bHiPri)
		NetSendHiPri(myplr, (byte *)&cmd, sizeof(cmd));
	else
		NetSendLoPri(myplr, (byte *)&cmd, sizeof(cmd));
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

static DWORD On_STRING2(int pnum, TCmd *pCmd)
{
	auto *p = (TCmdString *)pCmd;

	int len = strlen(p->str);
	if (!gbBufferMsgs)
		SendPlrMsg(pnum, p->str);

	return len + 2; // length of string + nul terminator + sizeof(p->bCmd)
}

static void delta_open_portal(int pnum, uint8_t x, uint8_t y, uint8_t bLevel, dungeon_type bLType, bool bSetLvl)
{
	sgbDeltaChanged = true;
	sgJunk.portal[pnum].x = x;
	sgJunk.portal[pnum].y = y;
	sgJunk.portal[pnum].level = bLevel;
	sgJunk.portal[pnum].ltype = bLType;
	sgJunk.portal[pnum].setlvl = bSetLvl;
}

void delta_close_portal(int pnum)
{
	memset(&sgJunk.portal[pnum], 0xFF, sizeof(sgJunk.portal[pnum]));
	sgbDeltaChanged = true;
}

static void check_update_plr(int pnum)
{
	if (gbIsMultiplayer && pnum == myplr)
		pfile_update(true);
}

static void msg_errorf(const char *pszFmt, ...)
{
	static DWORD msg_err_timer;
	DWORD ticks;
	char msg[256];
	va_list va;

	va_start(va, pszFmt);
	ticks = SDL_GetTicks();
	if (ticks - msg_err_timer >= 5000) {
		msg_err_timer = ticks;
		vsprintf(msg, pszFmt, va);
		ErrorPlrMsg(msg);
	}
	va_end(va);
}

static DWORD On_SYNCDATA(TCmd *pCmd, int pnum)
{
	return sync_update(pnum, (const byte *)pCmd);
}

static DWORD On_WALKXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		ClrPlrPath(plr[pnum]);
		MakePlrPath(pnum, { p->x, p->y }, true);
		plr[pnum].destAction = ACTION_NONE;
	}

	return sizeof(*p);
}

static DWORD On_ADDSTR(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		ModifyPlrStr(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_ADDMAG(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		ModifyPlrMag(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_ADDDEX(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		ModifyPlrDex(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_ADDVIT(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 256)
		ModifyPlrVit(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_SBSPELL(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1) {
		auto spell = static_cast<spell_id>(p->wParam1);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			plr[pnum]._pSpell = spell;
			plr[pnum]._pSplType = plr[pnum]._pSBkSplType;
			plr[pnum]._pSplFrom = 1;
			plr[pnum].destAction = ACTION_SPELL;
		} else
			msg_errorf(fmt::format(_("{:s} has cast an illegal spell."), plr[pnum]._pName).c_str());
	}

	return sizeof(*p);
}

static DWORD On_GOTOGETITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		MakePlrPath(pnum, { p->x, p->y }, false);
		plr[pnum].destAction = ACTION_PICKUPITEM;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_REQUESTGITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs != 1 && i_own_level(plr[pnum].plrlevel)) {
		if (GetItemRecord(p->dwSeed, p->wCI, p->wIndx)) {
			int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
			if (ii != -1) {
				NetSendCmdGItem2(false, CMD_GETITEM, myplr, p->bPnum, p);
				if (p->bPnum != myplr)
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				else
					InvGetItem(myplr, &items[ii], ii);
				SetItemRecord(p->dwSeed, p->wCI, p->wIndx);
			} else if (!NetSendCmdReq2(CMD_REQUESTGITEM, myplr, p->bPnum, p))
				NetSendCmdExtra(p);
		}
	}

	return sizeof(*p);
}

static DWORD On_GETITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
		if (delta_get_item(p, p->bLevel)) {
			if ((currlevel == p->bLevel || p->bPnum == myplr) && p->bMaster != myplr) {
				if (p->bPnum == myplr) {
					if (currlevel != p->bLevel) {
						ii = SyncPutItem(plr[myplr], plr[myplr].position.tile, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
						if (ii != -1)
							InvGetItem(myplr, &items[ii], ii);
					} else
						InvGetItem(myplr, &items[ii], ii);
				} else
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
			}
		} else
			NetSendCmdGItem2(true, CMD_GETITEM, p->bMaster, p->bPnum, p);
	}

	return sizeof(*p);
}

static DWORD On_GOTOAGETITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		MakePlrPath(pnum, { p->x, p->y }, false);
		plr[pnum].destAction = ACTION_PICKUPAITEM;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_REQUESTAGITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs != 1 && i_own_level(plr[pnum].plrlevel)) {
		if (GetItemRecord(p->dwSeed, p->wCI, p->wIndx)) {
			int ii = FindGetItem(p->wIndx, p->wCI, p->dwSeed);
			if (ii != -1) {
				NetSendCmdGItem2(false, CMD_AGETITEM, myplr, p->bPnum, p);
				if (p->bPnum != myplr)
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
				else
					AutoGetItem(myplr, &items[p->bCursitem], p->bCursitem);
				SetItemRecord(p->dwSeed, p->wCI, p->wIndx);
			} else if (!NetSendCmdReq2(CMD_REQUESTAGITEM, myplr, p->bPnum, p))
				NetSendCmdExtra(p);
		}
	}

	return sizeof(*p);
}

static DWORD On_AGETITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		FindGetItem(p->wIndx, p->wCI, p->dwSeed);
		if (delta_get_item(p, p->bLevel)) {
			if ((currlevel == p->bLevel || p->bPnum == myplr) && p->bMaster != myplr) {
				if (p->bPnum == myplr) {
					if (currlevel != p->bLevel) {
						int ii = SyncPutItem(plr[myplr], plr[myplr].position.tile, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
						if (ii != -1)
							AutoGetItem(myplr, &items[ii], ii);
					} else
						AutoGetItem(myplr, &items[p->bCursitem], p->bCursitem);
				} else
					SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
			}
		} else
			NetSendCmdGItem2(true, CMD_AGETITEM, p->bMaster, p->bPnum, p);
	}

	return sizeof(*p);
}

static DWORD On_ITEMEXTRA(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		delta_get_item(p, p->bLevel);
		if (currlevel == plr[pnum].plrlevel)
			SyncGetItem({ p->x, p->y }, p->wIndx, p->wCI, p->dwSeed);
	}

	return sizeof(*p);
}

static DWORD On_PUTITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (currlevel == plr[pnum].plrlevel) {
		int ii;
		if (pnum == myplr)
			ii = InvPutItem(plr[pnum], { p->x, p->y });
		else
			ii = SyncPutItem(plr[pnum], { p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
		if (ii != -1) {
			PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
			delta_put_item(p, items[ii].position.x, items[ii].position.y, plr[pnum].plrlevel);
			check_update_plr(pnum);
		}
		return sizeof(*p);
	} else {
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		delta_put_item(p, p->x, p->y, plr[pnum].plrlevel);
		check_update_plr(pnum);
	}

	return sizeof(*p);
}

static DWORD On_SYNCPUTITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (currlevel == plr[pnum].plrlevel) {
		int ii = SyncPutItem(plr[pnum], { p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
		if (ii != -1) {
			PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
			delta_put_item(p, items[ii].position.x, items[ii].position.y, plr[pnum].plrlevel);
			check_update_plr(pnum);
		}
		return sizeof(*p);
	} else {
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		delta_put_item(p, p->x, p->y, plr[pnum].plrlevel);
		check_update_plr(pnum);
	}

	return sizeof(*p);
}

static DWORD On_RESPAWNITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (currlevel == plr[pnum].plrlevel && pnum != myplr) {
			SyncPutItem(plr[pnum], { p->x, p->y }, p->wIndx, p->wCI, p->dwSeed, p->bId, p->bDur, p->bMDur, p->bCh, p->bMCh, p->wValue, p->dwBuff, p->wToHit, p->wMaxDam, p->bMinStr, p->bMinMag, p->bMinDex, p->bAC);
		}
		PutItemRecord(p->dwSeed, p->wCI, p->wIndx);
		delta_put_item(p, p->x, p->y, plr[pnum].plrlevel);
	}

	return sizeof(*p);
}

static DWORD On_ATTACKXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		MakePlrPath(pnum, { p->x, p->y }, false);
		plr[pnum].destAction = ACTION_ATTACK;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
	}

	return sizeof(*p);
}

static DWORD On_SATTACKXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		ClrPlrPath(plr[pnum]);
		plr[pnum].destAction = ACTION_ATTACK;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
	}

	return sizeof(*p);
}

static DWORD On_RATTACKXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		ClrPlrPath(plr[pnum]);
		plr[pnum].destAction = ACTION_RATTACK;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
	}

	return sizeof(*p);
}

static DWORD On_SPELLXYD(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam1);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(plr[pnum]);
			plr[pnum].destAction = ACTION_SPELLWALL;
			plr[pnum].destParam1 = p->x;
			plr[pnum].destParam2 = p->y;
			plr[pnum].destParam3 = static_cast<Direction>(p->wParam2);
			plr[pnum].destParam4 = p->wParam3;
			plr[pnum]._pSpell = spell;
			plr[pnum]._pSplType = plr[pnum]._pRSplType;
			plr[pnum]._pSplFrom = 0;
		} else
			msg_errorf(fmt::format(_("{:s} has cast an illegal spell."), plr[pnum]._pName).c_str());
	}

	return sizeof(*p);
}

static DWORD On_SPELLXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam2 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam1);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(plr[pnum]);
			plr[pnum].destAction = ACTION_SPELL;
			plr[pnum].destParam1 = p->x;
			plr[pnum].destParam2 = p->y;
			plr[pnum].destParam3 = static_cast<Direction>(p->wParam2);
			plr[pnum]._pSpell = spell;
			plr[pnum]._pSplType = plr[pnum]._pRSplType;
			plr[pnum]._pSplFrom = 0;
		} else
			msg_errorf(fmt::format(_("{:s} has cast an illegal spell."), plr[pnum]._pName).c_str());
	}

	return sizeof(*p);
}

static DWORD On_TSPELLXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam2 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam1);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(plr[pnum]);
			plr[pnum].destAction = ACTION_SPELL;
			plr[pnum].destParam1 = p->x;
			plr[pnum].destParam2 = p->y;
			plr[pnum].destParam3 = static_cast<Direction>(p->wParam2);
			plr[pnum]._pSpell = spell;
			plr[pnum]._pSplType = plr[pnum]._pTSplType;
			plr[pnum]._pSplFrom = 2;
		} else
			msg_errorf(fmt::format(_("{:s} has cast an illegal spell."), plr[pnum]._pName).c_str());
	}

	return sizeof(*p);
}

static DWORD On_OPOBJXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		if (object[p->wParam1]._oSolidFlag || object[p->wParam1]._oDoorFlag)
			MakePlrPath(pnum, { p->x, p->y }, false);
		else
			MakePlrPath(pnum, { p->x, p->y }, true);
		plr[pnum].destAction = ACTION_OPERATE;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_DISARMXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		if (object[p->wParam1]._oSolidFlag || object[p->wParam1]._oDoorFlag)
			MakePlrPath(pnum, { p->x, p->y }, false);
		else
			MakePlrPath(pnum, { p->x, p->y }, true);
		plr[pnum].destAction = ACTION_DISARM;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_OPOBJT(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		plr[pnum].destAction = ACTION_OPERATETK;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_ATTACKID(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		if (plr[pnum].position.tile.WalkingDistance(monster[p->wParam1].position.future) > 1)
			MakePlrPath(pnum, monster[p->wParam1].position.future, false);
		plr[pnum].destAction = ACTION_ATTACKMON;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_ATTACKPID(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		MakePlrPath(pnum, plr[p->wParam1].position.future, false);
		plr[pnum].destAction = ACTION_ATTACKPLR;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_RATTACKID(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		ClrPlrPath(plr[pnum]);
		plr[pnum].destAction = ACTION_RATTACKMON;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_RATTACKPID(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		ClrPlrPath(plr[pnum]);
		plr[pnum].destAction = ACTION_RATTACKPLR;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_SPELLID(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam2);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(plr[pnum]);
			plr[pnum].destAction = ACTION_SPELLMON;
			plr[pnum].destParam1 = p->wParam1;
			plr[pnum].destParam2 = p->wParam3;
			plr[pnum]._pSpell = spell;
			plr[pnum]._pSplType = plr[pnum]._pRSplType;
			plr[pnum]._pSplFrom = 0;
		} else
			msg_errorf(fmt::format(_("{:s} has cast an illegal spell."), plr[pnum]._pName).c_str());
	}

	return sizeof(*p);
}

static DWORD On_SPELLPID(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam2);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(plr[pnum]);
			plr[pnum].destAction = ACTION_SPELLPLR;
			plr[pnum].destParam1 = p->wParam1;
			plr[pnum].destParam2 = p->wParam3;
			plr[pnum]._pSpell = spell;
			plr[pnum]._pSplType = plr[pnum]._pRSplType;
			plr[pnum]._pSplFrom = 0;
		} else
			msg_errorf(fmt::format(_("{:s} has cast an illegal spell."), plr[pnum]._pName).c_str());
	}

	return sizeof(*p);
}

static DWORD On_TSPELLID(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam2);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(plr[pnum]);
			plr[pnum].destAction = ACTION_SPELLMON;
			plr[pnum].destParam1 = p->wParam1;
			plr[pnum].destParam2 = p->wParam3;
			plr[pnum]._pSpell = spell;
			plr[pnum]._pSplType = plr[pnum]._pTSplType;
			plr[pnum]._pSplFrom = 2;
		} else
			msg_errorf(fmt::format(_("{:s} has cast an illegal spell."), plr[pnum]._pName).c_str());
	}

	return sizeof(*p);
}

static DWORD On_TSPELLPID(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam3 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		auto spell = static_cast<spell_id>(p->wParam2);
		if (currlevel != 0 || spelldata[spell].sTownSpell) {
			ClrPlrPath(plr[pnum]);
			plr[pnum].destAction = ACTION_SPELLPLR;
			plr[pnum].destParam1 = p->wParam1;
			plr[pnum].destParam2 = p->wParam3;
			plr[pnum]._pSpell = spell;
			plr[pnum]._pSplType = plr[pnum]._pTSplType;
			plr[pnum]._pSplFrom = 2;
		} else
			msg_errorf(fmt::format(_("{:s} has cast an illegal spell."), plr[pnum]._pName).c_str());
	}

	return sizeof(*p);
}

static DWORD On_KNOCKBACK(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		M_GetKnockback(p->wParam1);
		M_StartHit(p->wParam1, pnum, 0);
	}

	return sizeof(*p);
}

static DWORD On_RESURRECT(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		DoResurrect(pnum, p->wParam1);
		check_update_plr(pnum);
	}

	return sizeof(*p);
}

static DWORD On_HEALOTHER(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel)
		DoHealOther(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_TALKXY(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel) {
		MakePlrPath(pnum, { p->x, p->y }, false);
		plr[pnum].destAction = ACTION_TALK;
		plr[pnum].destParam1 = p->wParam1;
	}

	return sizeof(*p);
}

static DWORD On_NEWLVL(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr)
		StartNewLvl(pnum, (interface_mode)p->wParam1, p->wParam2);

	return sizeof(*p);
}

static DWORD On_WARP(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		StartWarpLvl(pnum, p->wParam1);
	}

	return sizeof(*p);
}

static DWORD On_MONSTDEATH(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr) {
		if (currlevel == plr[pnum].plrlevel)
			M_SyncStartKill(p->wParam1, { p->x, p->y }, pnum);
		delta_kill_monster(p->wParam1, { p->x, p->y }, plr[pnum].plrlevel);
	}

	return sizeof(*p);
}

static DWORD On_KILLGOLEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr) {
		if (currlevel == p->wParam1)
			M_SyncStartKill(pnum, { p->x, p->y }, pnum);
		delta_kill_monster(pnum, { p->x, p->y }, plr[pnum].plrlevel);
	}

	return sizeof(*p);
}

static DWORD On_AWAKEGOLEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdGolem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (currlevel != plr[pnum].plrlevel)
		delta_sync_golem(p, pnum, p->_currlevel);
	else if (pnum != myplr) {
		int i;
		// check if this player already has an active golem
		bool addGolem = true;
		for (i = 0; i < nummissiles; i++) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MIS_GOLEM && missile[mi]._misource == pnum) {
				addGolem = false;
				// BUGFIX: break, don't need to check the rest
			}
		}
		if (addGolem)
			AddMissile(plr[pnum].position.tile.x, plr[pnum].position.tile.y, p->_mx, p->_my, p->_mdir, MIS_GOLEM, TARGET_MONSTERS, pnum, 0, 1);
	}

	return sizeof(*p);
}

static DWORD On_MONSTDAMAGE(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdMonDamage *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p)); // BUGFIX: change to sizeof(*p) or it still uses TCmdParam2 size for hellfire (fixed)
	else if (pnum != myplr) {
		if (currlevel == plr[pnum].plrlevel) {
			monster[p->wMon].mWhoHit |= 1 << pnum;
			if (monster[p->wMon]._mhitpoints > 0) {
				monster[p->wMon]._mhitpoints -= p->dwDam;
				if ((monster[p->wMon]._mhitpoints >> 6) < 1)
					monster[p->wMon]._mhitpoints = 1 << 6;
				delta_monster_hp(p->wMon, monster[p->wMon]._mhitpoints, plr[pnum].plrlevel);
			}
		}
	}

	return sizeof(*p);
}

static DWORD On_PLRDEAD(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr)
		StartPlayerKill(pnum, p->wParam1);
	else
		check_update_plr(pnum);

	return sizeof(*p);
}

static DWORD On_PLRDAMAGE(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdDamage *)pCmd;

	if (p->bPlr == myplr && currlevel != 0 && gbBufferMsgs != 1) {
		if (currlevel == plr[pnum].plrlevel && p->dwDam <= 192000 && plr[myplr]._pHitPoints >> 6 > 0) {
			ApplyPlrDamage(myplr, 0, 0, p->dwDam, 1);
		}
	}

	return sizeof(*p);
}

static DWORD On_OPENDOOR(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (currlevel == plr[pnum].plrlevel)
			SyncOpObject(pnum, CMD_OPENDOOR, p->wParam1);
		delta_sync_object(p->wParam1, CMD_OPENDOOR, plr[pnum].plrlevel);
	}

	return sizeof(*p);
}

static DWORD On_CLOSEDOOR(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (currlevel == plr[pnum].plrlevel)
			SyncOpObject(pnum, CMD_CLOSEDOOR, p->wParam1);
		delta_sync_object(p->wParam1, CMD_CLOSEDOOR, plr[pnum].plrlevel);
	}

	return sizeof(*p);
}

static DWORD On_OPERATEOBJ(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (currlevel == plr[pnum].plrlevel)
			SyncOpObject(pnum, CMD_OPERATEOBJ, p->wParam1);
		delta_sync_object(p->wParam1, CMD_OPERATEOBJ, plr[pnum].plrlevel);
	}

	return sizeof(*p);
}

static DWORD On_PLROPOBJ(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (currlevel == plr[pnum].plrlevel)
			SyncOpObject(p->wParam1, CMD_PLROPOBJ, p->wParam2);
		delta_sync_object(p->wParam2, CMD_PLROPOBJ, plr[pnum].plrlevel);
	}

	return sizeof(*p);
}

static DWORD On_BREAKOBJ(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam2 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (currlevel == plr[pnum].plrlevel)
			SyncBreakObj(p->wParam1, p->wParam2);
		delta_sync_object(p->wParam2, CMD_BREAKOBJ, plr[pnum].plrlevel);
	}

	return sizeof(*p);
}

static DWORD On_CHANGEPLRITEMS(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdChItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr)
		CheckInvSwap(pnum, p->bLoc, p->wIndx, p->wCI, p->dwSeed, p->bId, p->dwBuff);

	return sizeof(*p);
}

static DWORD On_DELPLRITEMS(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdDelItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (pnum != myplr)
		inv_update_rem_item(pnum, p->bLoc);

	return sizeof(*p);
}

static DWORD On_PLRLEVEL(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= MAXCHARLEVEL && pnum != myplr)
		plr[pnum]._pLevel = p->wParam1;

	return sizeof(*p);
}

static DWORD On_DROPITEM(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPItem *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else
		delta_put_item(p, p->x, p->y, plr[pnum].plrlevel);

	return sizeof(*p);
}

static DWORD On_SEND_PLRINFO(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdPlrInfoHdr *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, p->wBytes + sizeof(*p));
	else
		recv_plrinfo(pnum, p, p->bCmd == CMD_ACK_PLRINFO);

	return p->wBytes + sizeof(*p);
}

static DWORD On_ACK_PLRINFO(TCmd *pCmd, int pnum)
{
	return On_SEND_PLRINFO(pCmd, pnum);
}

static DWORD On_PLAYER_JOINLEVEL(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam1 *)pCmd;
	auto &player = plr[pnum];

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		player._pLvlChanging = false;
		if (player._pName[0] != 0 && !player.plractive) {
			ResetPlayerGFX(player);
			player.plractive = true;
			gbActivePlayers++;
			EventPlrMsg(fmt::format(_("Player '{:s}' (level {:d}) just joined the game"), player._pName, player._pLevel).c_str());
		}

		if (player.plractive && myplr != pnum) {
			player.position.tile = { p->x, p->y };
			player.plrlevel = p->wParam1;
			ResetPlayerGFX(player);
			if (currlevel == player.plrlevel) {
				SyncInitPlr(pnum);
				if ((player._pHitPoints >> 6) > 0)
					StartStand(pnum, DIR_S);
				else {
					player._pgfxnum = 0;
					player._pmode = PM_DEATH;
					NewPlrAnim(player, player_graphic::Death, DIR_S, player._pDFrames, 1);
					player.AnimInfo.CurrentFrame = player.AnimInfo.NumberOfFrames - 1;
					dFlags[player.position.tile.x][player.position.tile.y] |= BFLAG_DEAD_PLAYER;
				}

				player._pvid = AddVision(player.position.tile, player._pLightRad, pnum == myplr);
				player._plid = NO_LIGHT;
			}
		}
	}

	return sizeof(*p);
}

static DWORD On_ACTIVATEPORTAL(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam3 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		ActivatePortal(pnum, p->x, p->y, p->wParam1, (dungeon_type)p->wParam2, p->wParam3);
		if (pnum != myplr) {
			if (currlevel == 0)
				AddInTownPortal(pnum);
			else if (currlevel == plr[pnum].plrlevel) {
				bool addPortal = true;
				for (int i = 0; i < nummissiles; i++) {
					int mi = missileactive[i];
					if (missile[mi]._mitype == MIS_TOWN && missile[mi]._misource == pnum) {
						addPortal = false;
						// CODEFIX: break
					}
				}
				if (addPortal)
					AddWarpMissile(pnum, p->x, p->y);
			} else
				RemovePortalMissile(pnum);
		}
		delta_open_portal(pnum, p->x, p->y, p->wParam1, (dungeon_type)p->wParam2, p->wParam3);
	}

	return sizeof(*p);
}

static DWORD On_DEACTIVATEPORTAL(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, pCmd, sizeof(*pCmd));
	else {
		if (PortalOnLevel(pnum))
			RemovePortalMissile(pnum);
		DeactivatePortal(pnum);
		delta_close_portal(pnum);
	}

	return sizeof(*pCmd);
}

static DWORD On_RETOWN(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, pCmd, sizeof(*pCmd));
	else {
		if (pnum == myplr) {
			deathflag = false;
			gamemenu_off();
		}
		RestartTownLvl(pnum);
	}

	return sizeof(*pCmd);
}

static DWORD On_SETSTR(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != myplr)
		SetPlrStr(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_SETDEX(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != myplr)
		SetPlrDex(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_SETMAG(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != myplr)
		SetPlrMag(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_SETVIT(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdParam1 *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else if (p->wParam1 <= 750 && pnum != myplr)
		SetPlrVit(pnum, p->wParam1);

	return sizeof(*p);
}

static DWORD On_STRING(TCmd *pCmd, int pnum)
{
	return On_STRING2(pnum, pCmd);
}

static DWORD On_SYNCQUEST(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdQuest *)pCmd;

	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, p, sizeof(*p));
	else {
		if (pnum != myplr)
			SetMultiQuest(p->q, p->qstate, p->qlog, p->qvar1);
		sgbDeltaChanged = true;
	}

	return sizeof(*p);
}

static DWORD On_ENDSHIELD(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1 && pnum != myplr && currlevel == plr[pnum].plrlevel) {
		for (int i = 0; i < nummissiles; i++) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == pnum) {
				ClearMissileSpot(mi);
				DeleteMissile(mi, i);
			}
		}
	}

	return sizeof(*pCmd);
}

static DWORD On_CHEAT_EXPERIENCE(TCmd *pCmd, int pnum)
{
#ifdef _DEBUG
	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, pCmd, sizeof(*pCmd));
	else if (plr[pnum]._pLevel < MAXCHARLEVEL - 1) {
		plr[pnum]._pExperience = plr[pnum]._pNextExper;
		if (sgOptions.Gameplay.bExperienceBar) {
			force_redraw = 255;
		}
		NextPlrLevel(pnum);
	}
#endif
	return sizeof(*pCmd);
}

static DWORD On_CHEAT_SPELL_LEVEL(TCmd *pCmd, int pnum)
{
#ifdef _DEBUG
	if (gbBufferMsgs == 1)
		msg_send_packet(pnum, pCmd, sizeof(*pCmd));
	else
		plr[pnum]._pSplLvl[plr[pnum]._pRSpell]++;
#endif
	return sizeof(*pCmd);
}

static DWORD On_DEBUG(TCmd *pCmd)
{
	return sizeof(*pCmd);
}

static DWORD On_NOVA(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLoc *)pCmd;

	if (gbBufferMsgs != 1 && currlevel == plr[pnum].plrlevel && pnum != myplr) {
		ClrPlrPath(plr[pnum]);
		plr[pnum]._pSpell = SPL_NOVA;
		plr[pnum]._pSplType = RSPLTYPE_INVALID;
		plr[pnum]._pSplFrom = 3;
		plr[pnum].destAction = ACTION_SPELL;
		plr[pnum].destParam1 = p->x;
		plr[pnum].destParam2 = p->y;
	}

	return sizeof(*p);
}

static DWORD On_SETSHIELD(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1)
		plr[pnum].pManaShield = true;

	return sizeof(*pCmd);
}

static DWORD On_REMSHIELD(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1)
		plr[pnum].pManaShield = false;

	return sizeof(*pCmd);
}

static DWORD On_REFLECT(TCmd *pCmd, int pnum)
{
	if (gbBufferMsgs != 1 && pnum != myplr && currlevel == plr[pnum].plrlevel) {
		for (int i = 0; i < nummissiles; i++) {
			int mx = missileactive[i];
			if (missile[mx]._mitype == MIS_REFLECT && missile[mx]._misource == pnum) {
				ClearMissileSpot(mx);
				DeleteMissile(mx, i);
			}
		}
	}

	return sizeof(*pCmd);
}

static DWORD On_NAKRUL(TCmd *pCmd)
{
	if (gbBufferMsgs != 1) {
		operate_lv24_lever();
		IsUberRoomOpened = true;
		quests[Q_NAKRUL]._qactive = QUEST_DONE;
		monster_some_crypt();
	}
	return sizeof(*pCmd);
}

static DWORD On_OPENHIVE(TCmd *pCmd, int pnum)
{
	auto *p = (TCmdLocParam2 *)pCmd;
	if (gbBufferMsgs != 1) {
		AddMissile(p->x, p->y, p->wParam1, p->wParam2, 0, MIS_HIVEEXP2, TARGET_MONSTERS, pnum, 0, 0);
		TownOpenHive();
	}
	return sizeof(*p);
}

static DWORD On_OPENCRYPT(TCmd *pCmd)
{
	if (gbBufferMsgs != 1) {
		TownOpenGrave();
		InitTownTriggers();
		if (currlevel == 0)
			PlaySFX(IS_SARC);
	}
	return sizeof(*pCmd);
}

DWORD ParseCmd(int pnum, TCmd *pCmd)
{
	sbLastCmd = pCmd->bCmd;
	if (sgwPackPlrOffsetTbl[pnum] != 0 && sbLastCmd != CMD_ACK_PLRINFO && sbLastCmd != CMD_SEND_PLRINFO)
		return 0;

	switch (pCmd->bCmd) {
	case CMD_SYNCDATA:
		return On_SYNCDATA(pCmd, pnum);
	case CMD_WALKXY:
		return On_WALKXY(pCmd, pnum);
	case CMD_ADDSTR:
		return On_ADDSTR(pCmd, pnum);
	case CMD_ADDDEX:
		return On_ADDDEX(pCmd, pnum);
	case CMD_ADDMAG:
		return On_ADDMAG(pCmd, pnum);
	case CMD_ADDVIT:
		return On_ADDVIT(pCmd, pnum);
	case CMD_SBSPELL:
		return On_SBSPELL(pCmd, pnum);
	case CMD_GOTOGETITEM:
		return On_GOTOGETITEM(pCmd, pnum);
	case CMD_REQUESTGITEM:
		return On_REQUESTGITEM(pCmd, pnum);
	case CMD_GETITEM:
		return On_GETITEM(pCmd, pnum);
	case CMD_GOTOAGETITEM:
		return On_GOTOAGETITEM(pCmd, pnum);
	case CMD_REQUESTAGITEM:
		return On_REQUESTAGITEM(pCmd, pnum);
	case CMD_AGETITEM:
		return On_AGETITEM(pCmd, pnum);
	case CMD_ITEMEXTRA:
		return On_ITEMEXTRA(pCmd, pnum);
	case CMD_PUTITEM:
		return On_PUTITEM(pCmd, pnum);
	case CMD_SYNCPUTITEM:
		return On_SYNCPUTITEM(pCmd, pnum);
	case CMD_RESPAWNITEM:
		return On_RESPAWNITEM(pCmd, pnum);
	case CMD_ATTACKXY:
		return On_ATTACKXY(pCmd, pnum);
	case CMD_SATTACKXY:
		return On_SATTACKXY(pCmd, pnum);
	case CMD_RATTACKXY:
		return On_RATTACKXY(pCmd, pnum);
	case CMD_SPELLXYD:
		return On_SPELLXYD(pCmd, pnum);
	case CMD_SPELLXY:
		return On_SPELLXY(pCmd, pnum);
	case CMD_TSPELLXY:
		return On_TSPELLXY(pCmd, pnum);
	case CMD_OPOBJXY:
		return On_OPOBJXY(pCmd, pnum);
	case CMD_DISARMXY:
		return On_DISARMXY(pCmd, pnum);
	case CMD_OPOBJT:
		return On_OPOBJT(pCmd, pnum);
	case CMD_ATTACKID:
		return On_ATTACKID(pCmd, pnum);
	case CMD_ATTACKPID:
		return On_ATTACKPID(pCmd, pnum);
	case CMD_RATTACKID:
		return On_RATTACKID(pCmd, pnum);
	case CMD_RATTACKPID:
		return On_RATTACKPID(pCmd, pnum);
	case CMD_SPELLID:
		return On_SPELLID(pCmd, pnum);
	case CMD_SPELLPID:
		return On_SPELLPID(pCmd, pnum);
	case CMD_TSPELLID:
		return On_TSPELLID(pCmd, pnum);
	case CMD_TSPELLPID:
		return On_TSPELLPID(pCmd, pnum);
	case CMD_KNOCKBACK:
		return On_KNOCKBACK(pCmd, pnum);
	case CMD_RESURRECT:
		return On_RESURRECT(pCmd, pnum);
	case CMD_HEALOTHER:
		return On_HEALOTHER(pCmd, pnum);
	case CMD_TALKXY:
		return On_TALKXY(pCmd, pnum);
	case CMD_DEBUG:
		return On_DEBUG(pCmd);
	case CMD_NEWLVL:
		return On_NEWLVL(pCmd, pnum);
	case CMD_WARP:
		return On_WARP(pCmd, pnum);
	case CMD_MONSTDEATH:
		return On_MONSTDEATH(pCmd, pnum);
	case CMD_KILLGOLEM:
		return On_KILLGOLEM(pCmd, pnum);
	case CMD_AWAKEGOLEM:
		return On_AWAKEGOLEM(pCmd, pnum);
	case CMD_MONSTDAMAGE:
		return On_MONSTDAMAGE(pCmd, pnum);
	case CMD_PLRDEAD:
		return On_PLRDEAD(pCmd, pnum);
	case CMD_PLRDAMAGE:
		return On_PLRDAMAGE(pCmd, pnum);
	case CMD_OPENDOOR:
		return On_OPENDOOR(pCmd, pnum);
	case CMD_CLOSEDOOR:
		return On_CLOSEDOOR(pCmd, pnum);
	case CMD_OPERATEOBJ:
		return On_OPERATEOBJ(pCmd, pnum);
	case CMD_PLROPOBJ:
		return On_PLROPOBJ(pCmd, pnum);
	case CMD_BREAKOBJ:
		return On_BREAKOBJ(pCmd, pnum);
	case CMD_CHANGEPLRITEMS:
		return On_CHANGEPLRITEMS(pCmd, pnum);
	case CMD_DELPLRITEMS:
		return On_DELPLRITEMS(pCmd, pnum);
	case CMD_PLRLEVEL:
		return On_PLRLEVEL(pCmd, pnum);
	case CMD_DROPITEM:
		return On_DROPITEM(pCmd, pnum);
	case CMD_ACK_PLRINFO:
		return On_ACK_PLRINFO(pCmd, pnum);
	case CMD_SEND_PLRINFO:
		return On_SEND_PLRINFO(pCmd, pnum);
	case CMD_PLAYER_JOINLEVEL:
		return On_PLAYER_JOINLEVEL(pCmd, pnum);
	case CMD_ACTIVATEPORTAL:
		return On_ACTIVATEPORTAL(pCmd, pnum);
	case CMD_DEACTIVATEPORTAL:
		return On_DEACTIVATEPORTAL(pCmd, pnum);
	case CMD_RETOWN:
		return On_RETOWN(pCmd, pnum);
	case CMD_SETSTR:
		return On_SETSTR(pCmd, pnum);
	case CMD_SETMAG:
		return On_SETMAG(pCmd, pnum);
	case CMD_SETDEX:
		return On_SETDEX(pCmd, pnum);
	case CMD_SETVIT:
		return On_SETVIT(pCmd, pnum);
	case CMD_STRING:
		return On_STRING(pCmd, pnum);
	case CMD_SYNCQUEST:
		return On_SYNCQUEST(pCmd, pnum);
	case CMD_ENDSHIELD:
		return On_ENDSHIELD(pCmd, pnum);
	case CMD_CHEAT_EXPERIENCE:
		return On_CHEAT_EXPERIENCE(pCmd, pnum);
	case CMD_CHEAT_SPELL_LEVEL:
		return On_CHEAT_SPELL_LEVEL(pCmd, pnum);
	case CMD_NOVA:
		return On_NOVA(pCmd, pnum);
	case CMD_SETSHIELD:
		return On_SETSHIELD(pCmd, pnum);
	case CMD_REMSHIELD:
		return On_REMSHIELD(pCmd, pnum);
	case CMD_REFLECT:
		return On_REFLECT(pCmd, pnum);
	case CMD_NAKRUL:
		return On_NAKRUL(pCmd);
	case CMD_OPENHIVE:
		return On_OPENHIVE(pCmd, pnum);
	case CMD_OPENCRYPT:
		return On_OPENCRYPT(pCmd);
	default:
		break;
	}

	if (pCmd->bCmd < CMD_DLEVEL_0 || pCmd->bCmd > CMD_DLEVEL_END) {
		SNetDropPlayer(pnum, LEAVE_DROP);
		return 0;
	}

	return On_DLEVEL(pnum, pCmd);
}

} // namespace devilution
