/**
 * @file sync.cpp
 *
 * Implementation of functionality for syncing game state with other players.
 */
#include <climits>

#include "levels/gendung.h"
#include "monster.h"
#include "player.h"

namespace devilution {

namespace {

uint16_t sgnMonsterPriority[MaxMonsters];
size_t sgnMonsters;
uint16_t sgwLRU[MaxMonsters];
int sgnSyncItem;
int sgnSyncPInv;

void SyncOneMonster()
{
	for (size_t i = 0; i < ActiveMonsterCount; i++) {
		int m = ActiveMonsters[i];
		auto &monster = Monsters[m];
		sgnMonsterPriority[m] = MyPlayer->position.tile.ManhattanDistance(monster.position.tile);
		if (monster.activeForTicks == 0) {
			sgnMonsterPriority[m] += 0x1000;
		} else if (sgwLRU[m] != 0) {
			sgwLRU[m]--;
		}
	}
}

void SyncMonsterPos(TSyncMonster &monsterSync, int ndx)
{
	auto &monster = Monsters[ndx];
	monsterSync._mndx = ndx;
	monsterSync._mx = monster.position.tile.x;
	monsterSync._my = monster.position.tile.y;
	monsterSync._menemy = encode_enemy(monster);
	monsterSync._mdelta = sgnMonsterPriority[ndx] > 255 ? 255 : sgnMonsterPriority[ndx];
	monsterSync.mWhoHit = monster.whoHit;
	monsterSync._mhitpoints = monster.hitPoints;

	sgnMonsterPriority[ndx] = 0xFFFF;
	sgwLRU[ndx] = monster.activeForTicks == 0 ? 0xFFFF : 0xFFFE;
}

bool SyncMonsterActive(TSyncMonster &monsterSync)
{
	int ndx = -1;
	uint32_t lru = 0xFFFFFFFF;

	for (size_t i = 0; i < ActiveMonsterCount; i++) {
		int m = ActiveMonsters[i];
		if (sgnMonsterPriority[m] < lru && sgwLRU[m] < 0xFFFE) {
			lru = sgnMonsterPriority[m];
			ndx = ActiveMonsters[i];
		}
	}

	if (ndx == -1) {
		return false;
	}

	SyncMonsterPos(monsterSync, ndx);
	return true;
}

bool SyncMonsterActive2(TSyncMonster &monsterSync)
{
	int ndx = -1;
	uint32_t lru = 0xFFFE;

	for (size_t i = 0; i < ActiveMonsterCount; i++) {
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

	SyncMonsterPos(monsterSync, ndx);
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
			pHdr->wItemCI = (item._iIName[0] << 8) | item._iIName[1];
			pHdr->dwItemSeed = (item._iIName[2] << 24) | (item._iIName[3] << 16) | (item._iIName[4] << 8) | item._iIName[5];
			pHdr->bItemId = item._iIName[6];
			pHdr->bItemDur = item._iIName[7];
			pHdr->bItemMDur = item._iIName[8];
			pHdr->bItemCh = item._iIName[9];
			pHdr->bItemMCh = item._iIName[10];
			pHdr->wItemVal = (item._iIName[11] << 8) | ((item._iCurs - ICURS_EAR_SORCERER) << 6) | item._ivalue;
			pHdr->dwItemBuff = (item._iIName[12] << 24) | (item._iIName[13] << 16) | (item._iIName[14] << 8) | item._iIName[15];
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
	const auto &item = MyPlayer->InvBody[sgnSyncPInv];
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

void SyncMonster(bool isOwner, const TSyncMonster &monsterSync)
{
	const int monsterId = monsterSync._mndx;
	Monster &monster = Monsters[monsterId];
	if (monster.hitPoints <= 0 || monster.mode == MonsterMode::Death) {
		return;
	}

	const Point position { monsterSync._mx, monsterSync._my };
	const int enemyId = monsterSync._menemy;

	if (monster.activeForTicks != 0) {
		uint32_t delta = MyPlayer->position.tile.ManhattanDistance(monster.position.tile);
		if (delta > 255) {
			delta = 255;
		}

		if (delta < monsterSync._mdelta || (delta == monsterSync._mdelta && isOwner)) {
			return;
		}
		if (monster.position.future == position) {
			return;
		}
	}
	if (IsAnyOf(monster.mode, MonsterMode::Charge, MonsterMode::Petrified)) {
		return;
	}

	if (monster.position.tile.WalkingDistance(position) <= 2) {
		if (!monster.isWalking()) {
			Direction md = GetDirection(monster.position.tile, position);
			if (DirOK(monster, md)) {
				M_ClearSquares(monster);
				dMonster[monster.position.tile.x][monster.position.tile.y] = monsterId + 1;
				Walk(monster, md);
				monster.activeForTicks = UINT8_MAX;
			}
		}
	} else if (dMonster[position.x][position.y] == 0) {
		M_ClearSquares(monster);
		dMonster[position.x][position.y] = monsterId + 1;
		monster.position.tile = position;
		decode_enemy(monster, enemyId);
		Direction md = GetDirection(position, monster.enemyPosition);
		M_StartStand(monster, md);
		monster.activeForTicks = UINT8_MAX;
	}

	decode_enemy(monster, enemyId);
	monster.whoHit |= monsterSync.mWhoHit;
}

bool IsEnemyIdValid(const Monster &monster, int enemyId)
{
	if (enemyId < 0) {
		return false;
	}

	if (enemyId < MAX_PLRS) {
		return Players[enemyId].plractive;
	}

	enemyId -= MAX_PLRS;
	if (static_cast<size_t>(enemyId) >= MaxMonsters) {
		return false;
	}

	const Monster &enemy = Monsters[enemyId];

	if (&enemy == &monster) {
		return false;
	}

	if (enemy.hitPoints <= 0) {
		return false;
	}

	return true;
}

bool IsTSyncMonsterValidate(const TSyncMonster &monsterSync)
{
	const size_t monsterId = monsterSync._mndx;

	if (monsterId >= MaxMonsters)
		return false;

	if (!InDungeonBounds({ monsterSync._mx, monsterSync._my }))
		return false;

	if (!IsEnemyIdValid(Monsters[monsterId], monsterSync._menemy))
		return false;

	return true;
}

} // namespace

uint32_t sync_all_monsters(byte *pbBuf, uint32_t dwMaxLen)
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
	pHdr->bLevel = GetLevelForMultiplayer(*MyPlayer);
	pHdr->wLen = 0;
	SyncPlrInv(pHdr);
	assert(dwMaxLen <= 0xffff);
	SyncOneMonster();

	for (size_t i = 0; i < ActiveMonsterCount && dwMaxLen >= sizeof(TSyncMonster); i++) {
		auto &monsterSync = *reinterpret_cast<TSyncMonster *>(pbBuf);
		bool sync = false;
		if (i < 2) {
			sync = SyncMonsterActive2(monsterSync);
		}
		if (!sync) {
			sync = SyncMonsterActive(monsterSync);
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

uint32_t OnSyncData(const TCmd *pCmd, size_t pnum)
{
	const auto &header = *reinterpret_cast<const TSyncHeader *>(pCmd);

	assert(gbBufferMsgs != 2);

	if (gbBufferMsgs == 1) {
		return header.wLen + sizeof(header);
	}
	if (pnum == MyPlayerId) {
		return header.wLen + sizeof(header);
	}

	assert(header.wLen % sizeof(TSyncMonster) == 0);
	int monsterCount = header.wLen / sizeof(TSyncMonster);

	uint8_t level = header.bLevel;

	if (IsValidLevelForMultiplayer(level)) {
		const auto *monsterSyncs = reinterpret_cast<const TSyncMonster *>(pCmd + sizeof(header));

		for (int i = 0; i < monsterCount; i++) {
			if (!IsTSyncMonsterValidate(monsterSyncs[i]))
				continue;

			if (GetLevelForMultiplayer(*MyPlayer) == level) {
				SyncMonster(pnum > MyPlayerId, monsterSyncs[i]);
			}

			delta_sync_monster(monsterSyncs[i], level);
		}
	}

	return header.wLen + sizeof(header);
}

void sync_init()
{
	sgnMonsters = 16 * MyPlayerId;
	memset(sgwLRU, 255, sizeof(sgwLRU));
}

} // namespace devilution
