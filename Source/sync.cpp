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

static void sync_one_monster()
{
	for (int i = 0; i < nummonsters; i++) {
		int m = monstactive[i];
		sgnMonsterPriority[m] = plr[myplr].position.tile.ManhattanDistance(monster[m].position.tile);
		if (monster[m]._msquelch == 0) {
			sgnMonsterPriority[m] += 0x1000;
		} else if (sgwLRU[m] != 0) {
			sgwLRU[m]--;
		}
	}
}

static void sync_monster_pos(TSyncMonster *p, int ndx)
{
	p->_mndx = ndx;
	p->_mx = monster[ndx].position.tile.x;
	p->_my = monster[ndx].position.tile.y;
	p->_menemy = encode_enemy(ndx);
	p->_mdelta = sgnMonsterPriority[ndx] > 255 ? 255 : sgnMonsterPriority[ndx];

	sgnMonsterPriority[ndx] = 0xFFFF;
	sgwLRU[ndx] = monster[ndx]._msquelch == 0 ? 0xFFFF : 0xFFFE;
}

static bool sync_monster_active(TSyncMonster *p)
{
	int i, m, ndx;
	uint32_t lru;

	ndx = -1;
	lru = 0xFFFFFFFF;

	for (i = 0; i < nummonsters; i++) {
		m = monstactive[i];
		if (sgnMonsterPriority[m] < lru && sgwLRU[m] < 0xFFFE) {
			lru = sgnMonsterPriority[m];
			ndx = monstactive[i];
		}
	}

	if (ndx == -1) {
		return false;
	}

	sync_monster_pos(p, ndx);
	return true;
}

static bool sync_monster_active2(TSyncMonster *p)
{
	int i, m, ndx;
	uint32_t lru;

	ndx = -1;
	lru = 0xFFFE;

	for (i = 0; i < nummonsters; i++) {
		if (sgnMonsters >= nummonsters) {
			sgnMonsters = 0;
		}
		m = monstactive[sgnMonsters];
		if (sgwLRU[m] < lru) {
			lru = sgwLRU[m];
			ndx = monstactive[sgnMonsters];
		}
		sgnMonsters++;
	}

	if (ndx == -1) {
		return false;
	}

	sync_monster_pos(p, ndx);
	return true;
}

static void SyncPlrInv(TSyncHeader *pHdr)
{
	int ii;
	ItemStruct *pItem;

	if (numitems > 0) {
		if (sgnSyncItem >= numitems) {
			sgnSyncItem = 0;
		}
		ii = itemactive[sgnSyncItem++];
		pHdr->bItemI = ii;
		pHdr->bItemX = items[ii].position.x;
		pHdr->bItemY = items[ii].position.y;
		pHdr->wItemIndx = items[ii].IDidx;
		if (items[ii].IDidx == IDI_EAR) {
			pHdr->wItemCI = (items[ii]._iName[7] << 8) | items[ii]._iName[8];
			pHdr->dwItemSeed = (items[ii]._iName[9] << 24) | (items[ii]._iName[10] << 16) | (items[ii]._iName[11] << 8) | items[ii]._iName[12];
			pHdr->bItemId = items[ii]._iName[13];
			pHdr->bItemDur = items[ii]._iName[14];
			pHdr->bItemMDur = items[ii]._iName[15];
			pHdr->bItemCh = items[ii]._iName[16];
			pHdr->bItemMCh = items[ii]._iName[17];
			pHdr->wItemVal = (items[ii]._iName[18] << 8) | ((items[ii]._iCurs - ICURS_EAR_SORCERER) << 6) | items[ii]._ivalue;
			pHdr->dwItemBuff = (items[ii]._iName[19] << 24) | (items[ii]._iName[20] << 16) | (items[ii]._iName[21] << 8) | items[ii]._iName[22];
		} else {
			pHdr->wItemCI = items[ii]._iCreateInfo;
			pHdr->dwItemSeed = items[ii]._iSeed;
			pHdr->bItemId = items[ii]._iIdentified;
			pHdr->bItemDur = items[ii]._iDurability;
			pHdr->bItemMDur = items[ii]._iMaxDur;
			pHdr->bItemCh = items[ii]._iCharges;
			pHdr->bItemMCh = items[ii]._iMaxCharges;
			if (items[ii].IDidx == IDI_GOLD) {
				pHdr->wItemVal = items[ii]._ivalue;
			}
		}
	} else {
		pHdr->bItemI = -1;
	}

	assert(sgnSyncPInv > -1 && sgnSyncPInv < NUM_INVLOC);
	pItem = &plr[myplr].InvBody[sgnSyncPInv];
	if (!pItem->isEmpty()) {
		pHdr->bPInvLoc = sgnSyncPInv;
		pHdr->wPInvIndx = pItem->IDidx;
		pHdr->wPInvCI = pItem->_iCreateInfo;
		pHdr->dwPInvSeed = pItem->_iSeed;
		pHdr->bPInvId = pItem->_iIdentified;
	} else {
		pHdr->bPInvLoc = -1;
	}

	sgnSyncPInv++;
	if (sgnSyncPInv >= NUM_INVLOC) {
		sgnSyncPInv = 0;
	}
}

} // namespace

uint32_t sync_all_monsters(const byte *pbBuf, uint32_t dwMaxLen)
{
	TSyncHeader *pHdr;
	int i;
	bool sync;

	if (nummonsters < 1) {
		return dwMaxLen;
	}
	if (dwMaxLen < sizeof(*pHdr) + sizeof(TSyncMonster)) {
		return dwMaxLen;
	}

	pHdr = (TSyncHeader *)pbBuf;
	pbBuf += sizeof(*pHdr);
	dwMaxLen -= sizeof(*pHdr);

	pHdr->bCmd = CMD_SYNCDATA;
	pHdr->bLevel = currlevel;
	pHdr->wLen = 0;
	SyncPlrInv(pHdr);
	assert(dwMaxLen <= 0xffff);
	sync_one_monster();

	for (i = 0; i < nummonsters && dwMaxLen >= sizeof(TSyncMonster); i++) {
		sync = false;
		if (i < 2) {
			sync = sync_monster_active2((TSyncMonster *)pbBuf);
		}
		if (!sync) {
			sync = sync_monster_active((TSyncMonster *)pbBuf);
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

static void sync_monster(int pnum, const TSyncMonster *p)
{
	int ndx, mdx, mdy;
	uint32_t delta;

	ndx = p->_mndx;

	if (monster[ndx]._mhitpoints <= 0) {
		return;
	}

	delta = abs(plr[myplr].position.tile.x - monster[ndx].position.tile.x) + abs(plr[myplr].position.tile.y - monster[ndx].position.tile.y);
	if (delta > 255) {
		delta = 255;
	}

	if (delta < p->_mdelta || (delta == p->_mdelta && pnum > myplr)) {
		return;
	}
	if (monster[ndx].position.future.x == p->_mx && monster[ndx].position.future.y == p->_my) {
		return;
	}
	if (monster[ndx]._mmode == MM_CHARGE || monster[ndx]._mmode == MM_STONE) {
		return;
	}

	mdx = abs(monster[ndx].position.tile.x - p->_mx);
	mdy = abs(monster[ndx].position.tile.y - p->_my);
	if (mdx <= 2 && mdy <= 2) {
		if (monster[ndx]._mmode < MM_WALK || monster[ndx]._mmode > MM_WALK3) {
			Direction md = GetDirection(monster[ndx].position.tile, { p->_mx, p->_my });
			if (DirOK(ndx, md)) {
				M_ClearSquares(ndx);
				dMonster[monster[ndx].position.tile.x][monster[ndx].position.tile.y] = ndx + 1;
				M_WalkDir(ndx, md);
				monster[ndx]._msquelch = UINT8_MAX;
			}
		}
	} else if (dMonster[p->_mx][p->_my] == 0) {
		M_ClearSquares(ndx);
		dMonster[p->_mx][p->_my] = ndx + 1;
		monster[ndx].position.tile = { p->_mx, p->_my };
		decode_enemy(ndx, p->_menemy);
		Direction md = GetDirection({ p->_mx, p->_my }, monster[ndx].enemyPosition);
		M_StartStand(ndx, md);
		monster[ndx]._msquelch = UINT8_MAX;
	}

	decode_enemy(ndx, p->_menemy);
}

uint32_t sync_update(int pnum, const byte *pbBuf)
{
	uint16_t wLen;

	TSyncHeader *pHdr = (TSyncHeader *)pbBuf;
	pbBuf += sizeof(*pHdr);

	if (pHdr->bCmd != CMD_SYNCDATA) {
		app_fatal("bad sync command");
	}

	assert(gbBufferMsgs != 2);

	if (gbBufferMsgs == 1) {
		return pHdr->wLen + sizeof(*pHdr);
	}
	if (pnum == myplr) {
		return pHdr->wLen + sizeof(*pHdr);
	}

	for (wLen = pHdr->wLen; wLen >= sizeof(TSyncMonster); wLen -= sizeof(TSyncMonster)) {
		if (currlevel == pHdr->bLevel) {
			sync_monster(pnum, (TSyncMonster *)pbBuf);
		}
		delta_sync_monster((TSyncMonster *)pbBuf, pHdr->bLevel);
		pbBuf += sizeof(TSyncMonster);
	}

	assert(wLen == 0);

	return pHdr->wLen + sizeof(*pHdr);
}

void sync_init()
{
	sgnMonsters = 16 * myplr;
	memset(sgwLRU, 255, sizeof(sgwLRU));
}

} // namespace devilution
