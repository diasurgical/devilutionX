/**
 * @file sync.cpp
 *
 * Implementation of functionality for syncing game state with other players.
 */
#include <climits>

#include "gendung.h"
#include "monster.h"
#include "player.h"

namespace devilution {

namespace {

uint16_t sgnMonsterPriority[MAXMONSTERS];
int sgnMonsters;
uint16_t sgwLRU[MAXMONSTERS];
int sgnSyncItem;
int sgnSyncPInv;

void SyncOneMonster()
{
	for (int i = 0; i < ActiveMonsterCount; i++) {
		int m = ActiveMonsters[i];
		auto &monster = Monsters[m];
		sgnMonsterPriority[m] = Players[MyPlayerId].position.tile.ManhattanDistance(monster.position.tile);
		if (monster._msquelch == 0) {
			sgnMonsterPriority[m] += 0x1000;
		} else if (sgwLRU[m] != 0) {
			sgwLRU[m]--;
		}
	}
}

void SyncMonsterPos(TSyncMonster *p, int ndx)
{
	auto &monster = Monsters[ndx];
	p->_mndx = ndx;
	p->_mx = monster.position.tile.x;
	p->_my = monster.position.tile.y;
	p->_menemy = encode_enemy(monster);
	p->_mdelta = sgnMonsterPriority[ndx] > 255 ? 255 : sgnMonsterPriority[ndx];

	sgnMonsterPriority[ndx] = 0xFFFF;
	sgwLRU[ndx] = monster._msquelch == 0 ? 0xFFFF : 0xFFFE;
}

bool SyncMonsterActive(TSyncMonster *p)
{
	int ndx = -1;
	uint32_t lru = 0xFFFFFFFF;

	for (int i = 0; i < ActiveMonsterCount; i++) {
		int m = ActiveMonsters[i];
		if (sgnMonsterPriority[m] < lru && sgwLRU[m] < 0xFFFE) {
			lru = sgnMonsterPriority[m];
			ndx = ActiveMonsters[i];
		}
	}

	if (ndx == -1) {
		return false;
	}

	SyncMonsterPos(p, ndx);
	return true;
}

bool SyncMonsterActive2(TSyncMonster *p)
{
	int ndx = -1;
	uint32_t lru = 0xFFFE;

	for (int i = 0; i < ActiveMonsterCount; i++) {
		if (sgnMonsters >= ActiveMonsterCount) {
			sgnMonsters = 0;
		}
		int m = ActiveMonsters[sgnMonsters];
		if (sgwLRU[m] < lru) {
			lru = sgwLRU[m];
			ndx = ActiveMonsters[sgnMonsters];
		}
		sgnMonsters++;
	}

	if (ndx == -1) {
		return false;
	}

	SyncMonsterPos(p, ndx);
	return true;
}

void SyncPlrInv(TSyncHeader *pHdr)
{
	pHdr->bItemI = -1;
	if (ActiveItemCount > 0) {
		if (sgnSyncItem >= ActiveItemCount) {
			sgnSyncItem = 0;
		}
		pHdr->bItemI = ActiveItems[sgnSyncItem];
		sgnSyncItem++;
		auto &item = Items[pHdr->bItemI];
		pHdr->bItemX = item.position.x;
		pHdr->bItemY = item.position.y;
		pHdr->wItemIndx = item.IDidx;
		if (item.IDidx == IDI_EAR) {
			pHdr->wItemCI = (item._iName[7] << 8) | item._iName[8];
			pHdr->dwItemSeed = (item._iName[9] << 24) | (item._iName[10] << 16) | (item._iName[11] << 8) | item._iName[12];
			pHdr->bItemId = item._iName[13];
			pHdr->bItemDur = item._iName[14];
			pHdr->bItemMDur = item._iName[15];
			pHdr->bItemCh = item._iName[16];
			pHdr->bItemMCh = item._iName[17];
			pHdr->wItemVal = (item._iName[18] << 8) | ((item._iCurs - ICURS_EAR_SORCERER) << 6) | item._ivalue;
			pHdr->dwItemBuff = (item._iName[19] << 24) | (item._iName[20] << 16) | (item._iName[21] << 8) | item._iName[22];
		} else {
			pHdr->wItemCI = item._iCreateInfo;
			pHdr->dwItemSeed = item._iSeed;
			pHdr->bItemId = item._iIdentified ? 1 : 0;
			pHdr->bItemDur = item._iDurability;
			pHdr->bItemMDur = item._iMaxDur;
			pHdr->bItemCh = item._iCharges;
			pHdr->bItemMCh = item._iMaxCharges;
			if (item.IDidx == IDI_GOLD) {
				pHdr->wItemVal = item._ivalue;
			}
		}
	}

	pHdr->bPInvLoc = -1;
	assert(sgnSyncPInv > -1 && sgnSyncPInv < NUM_INVLOC);
	const auto &item = Players[MyPlayerId].InvBody[sgnSyncPInv];
	if (!item.isEmpty()) {
		pHdr->bPInvLoc = sgnSyncPInv;
		pHdr->wPInvIndx = item.IDidx;
		pHdr->wPInvCI = item._iCreateInfo;
		pHdr->dwPInvSeed = item._iSeed;
		pHdr->bPInvId = item._iIdentified ? 1 : 0;
	}

	sgnSyncPInv++;
	if (sgnSyncPInv >= NUM_INVLOC) {
		sgnSyncPInv = 0;
	}
}

void SyncMonster(int pnum, const TSyncMonster *p)
{
	int ndx = p->_mndx;

	auto &monster = Monsters[ndx];

	if (monster._mhitpoints <= 0) {
		return;
	}

	uint32_t delta = Players[MyPlayerId].position.tile.ManhattanDistance(monster.position.tile);
	if (delta > 255) {
		delta = 255;
	}

	if (delta < p->_mdelta || (delta == p->_mdelta && pnum > MyPlayerId)) {
		return;
	}
	if (monster.position.future.x == p->_mx && monster.position.future.y == p->_my) {
		return;
	}
	if (monster._mmode == MM_CHARGE || monster._mmode == MM_STONE) {
		return;
	}

	if (monster.position.tile.WalkingDistance({ p->_mx, p->_my }) <= 2) {
		if (monster._mmode < MM_WALK || monster._mmode > MM_WALK3) {
			Direction md = GetDirection(monster.position.tile, { p->_mx, p->_my });
			if (DirOK(ndx, md)) {
				M_ClearSquares(ndx);
				dMonster[monster.position.tile.x][monster.position.tile.y] = ndx + 1;
				M_WalkDir(ndx, md);
				monster._msquelch = UINT8_MAX;
			}
		}
	} else if (dMonster[p->_mx][p->_my] == 0) {
		M_ClearSquares(ndx);
		dMonster[p->_mx][p->_my] = ndx + 1;
		monster.position.tile = { p->_mx, p->_my };
		decode_enemy(monster, p->_menemy);
		Direction md = GetDirection({ p->_mx, p->_my }, monster.enemyPosition);
		M_StartStand(monster, md);
		monster._msquelch = UINT8_MAX;
	}

	decode_enemy(monster, p->_menemy);
}

} // namespace

uint32_t sync_all_monsters(const byte *pbBuf, uint32_t dwMaxLen)
{
	if (ActiveMonsterCount < 1) {
		return dwMaxLen;
	}
	if (dwMaxLen < sizeof(TSyncHeader) + sizeof(TSyncMonster)) {
		return dwMaxLen;
	}

	auto *pHdr = (TSyncHeader *)pbBuf;
	pbBuf += sizeof(TSyncHeader);
	dwMaxLen -= sizeof(TSyncHeader);

	pHdr->bCmd = CMD_SYNCDATA;
	pHdr->bLevel = currlevel;
	pHdr->wLen = 0;
	SyncPlrInv(pHdr);
	assert(dwMaxLen <= 0xffff);
	SyncOneMonster();

	for (int i = 0; i < ActiveMonsterCount && dwMaxLen >= sizeof(TSyncMonster); i++) {
		bool sync = false;
		if (i < 2) {
			sync = SyncMonsterActive2((TSyncMonster *)pbBuf);
		}
		if (!sync) {
			sync = SyncMonsterActive((TSyncMonster *)pbBuf);
		}
		if (!sync) {
			break;
		}
		pbBuf += sizeof(TSyncMonster);
		pHdr->wLen += sizeof(TSyncMonster);
		dwMaxLen -= sizeof(TSyncMonster);
	}

	return dwMaxLen;
}

uint32_t sync_update(int pnum, const byte *pbBuf)
{
	uint16_t wLen;

	auto *pHdr = (TSyncHeader *)pbBuf;
	pbBuf += sizeof(*pHdr);

	if (pHdr->bCmd != CMD_SYNCDATA) {
		app_fatal("bad sync command");
	}

	assert(gbBufferMsgs != 2);

	if (gbBufferMsgs == 1) {
		return pHdr->wLen + sizeof(*pHdr);
	}
	if (pnum == MyPlayerId) {
		return pHdr->wLen + sizeof(*pHdr);
	}

	for (wLen = pHdr->wLen; wLen >= sizeof(TSyncMonster); wLen -= sizeof(TSyncMonster)) {
		if (currlevel == pHdr->bLevel) {
			SyncMonster(pnum, (TSyncMonster *)pbBuf);
		}
		delta_sync_monster((TSyncMonster *)pbBuf, pHdr->bLevel);
		pbBuf += sizeof(TSyncMonster);
	}

	assert(wLen == 0);

	return pHdr->wLen + sizeof(*pHdr);
}

void sync_init()
{
	sgnMonsters = 16 * MyPlayerId;
	memset(sgwLRU, 255, sizeof(sgwLRU));
}

} // namespace devilution
