/**
 * @file sync.cpp
 *
 * Implementation of functionality for syncing game state with other players.
 */
#include <cstdint>

#include <limits>

#include "levels/gendung.h"
#include "lighting.h"
#include "monster.h"
#include "player.h"
#include "utils/is_of.hpp"

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
		const unsigned m = ActiveMonsters[i];
		Monster &monster = Monsters[m];
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
	Monster &monster = Monsters[ndx];
	monsterSync._mndx = ndx;
	monsterSync._mx = monster.position.tile.x;
	monsterSync._my = monster.position.tile.y;
	monsterSync._menemy = encode_enemy(monster);
	monsterSync._mdelta = sgnMonsterPriority[ndx] > 255 ? 255 : sgnMonsterPriority[ndx];
	monsterSync.mWhoHit = monster.whoHit;
	monsterSync._mhitpoints = SDL_SwapLE32(monster.hitPoints);

	sgnMonsterPriority[ndx] = 0xFFFF;
	sgwLRU[ndx] = monster.activeForTicks == 0 ? 0xFFFF : 0xFFFE;
}

bool SyncMonsterActive(TSyncMonster &monsterSync)
{
	unsigned ndx = std::numeric_limits<unsigned>::max();
	uint32_t lru = 0xFFFFFFFF;

	for (size_t i = 0; i < ActiveMonsterCount; i++) {
		const unsigned m = ActiveMonsters[i];
		if (sgnMonsterPriority[m] < lru && sgwLRU[m] < 0xFFFE) {
			lru = sgnMonsterPriority[m];
			ndx = ActiveMonsters[i];
		}
	}

	if (ndx == std::numeric_limits<unsigned>::max()) {
		return false;
	}

	SyncMonsterPos(monsterSync, ndx);
	return true;
}

bool SyncMonsterActive2(TSyncMonster &monsterSync)
{
	unsigned ndx = std::numeric_limits<unsigned>::max();
	uint32_t lru = 0xFFFE;

	for (size_t i = 0; i < ActiveMonsterCount; i++) {
		if (sgnMonsters >= ActiveMonsterCount) {
			sgnMonsters = 0;
		}
		const unsigned m = ActiveMonsters[sgnMonsters];
		if (sgwLRU[m] < lru) {
			lru = sgwLRU[m];
			ndx = ActiveMonsters[sgnMonsters];
		}
		sgnMonsters++;
	}

	if (ndx == std::numeric_limits<unsigned>::max()) {
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
		pHdr->wItemIndx = SDL_SwapLE16(item.IDidx);
		if (item.IDidx == IDI_EAR) {
			pHdr->wItemCI = SDL_SwapLE16((item._iIName[0] << 8) | item._iIName[1]);
			pHdr->dwItemSeed = SDL_SwapLE32((item._iIName[2] << 24) | (item._iIName[3] << 16) | (item._iIName[4] << 8) | item._iIName[5]);
			pHdr->bItemId = item._iIName[6];
			pHdr->bItemDur = item._iIName[7];
			pHdr->bItemMDur = item._iIName[8];
			pHdr->bItemCh = item._iIName[9];
			pHdr->bItemMCh = item._iIName[10];
			pHdr->wItemVal = SDL_SwapLE16((item._iIName[11] << 8) | ((item._iCurs - ICURS_EAR_SORCERER) << 6) | item._ivalue);
			pHdr->dwItemBuff = SDL_SwapLE32((item._iIName[12] << 24) | (item._iIName[13] << 16) | (item._iIName[14] << 8) | item._iIName[15]);
		} else {
			pHdr->wItemCI = SDL_SwapLE16(item._iCreateInfo);
			pHdr->dwItemSeed = SDL_SwapLE32(item._iSeed);
			pHdr->bItemId = item._iIdentified ? 1 : 0;
			pHdr->bItemDur = item._iDurability;
			pHdr->bItemMDur = item._iMaxDur;
			pHdr->bItemCh = item._iCharges;
			pHdr->bItemMCh = item._iMaxCharges;
			if (item.IDidx == IDI_GOLD) {
				pHdr->wItemVal = SDL_SwapLE16(item._ivalue);
			}
		}
	}

	pHdr->bPInvLoc = -1;
	assert(sgnSyncPInv > -1 && sgnSyncPInv < NUM_INVLOC);
	const auto &item = MyPlayer->InvBody[sgnSyncPInv];
	if (!item.isEmpty()) {
		pHdr->bPInvLoc = sgnSyncPInv;
		pHdr->wPInvIndx = SDL_SwapLE16(item.IDidx);
		pHdr->wPInvCI = SDL_SwapLE16(item._iCreateInfo);
		pHdr->dwPInvSeed = SDL_SwapLE32(item._iSeed);
		pHdr->bPInvId = item._iIdentified ? 1 : 0;
	}

	sgnSyncPInv++;
	if (sgnSyncPInv >= NUM_INVLOC) {
		sgnSyncPInv = 0;
	}
}

void SyncMonster(bool isOwner, const TSyncMonster &monsterSync)
{
	Monster &monster = Monsters[monsterSync._mndx];
	if (monster.hitPoints <= 0 || monster.mode == MonsterMode::Death) {
		return;
	}

	const Point position { monsterSync._mx, monsterSync._my };
	const uint8_t enemyId = monsterSync._menemy;

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
				monster.occupyTile(monster.position.tile, false);
				Walk(monster, md);
				monster.activeForTicks = std::numeric_limits<uint8_t>::max();
			}
		}
	} else if (dMonster[position.x][position.y] == 0) {
		M_ClearSquares(monster);
		monster.occupyTile(position, false);
		monster.position.tile = position;
		if (monster.lightId != NO_LIGHT)
			ChangeLightXY(monster.lightId, position);
		decode_enemy(monster, enemyId);
		Direction md = GetDirection(position, monster.enemyPosition);
		M_StartStand(monster, md);
		monster.activeForTicks = std::numeric_limits<uint8_t>::max();
	}

	decode_enemy(monster, enemyId);
	monster.whoHit |= monsterSync.mWhoHit;
}

bool IsEnemyIdValid(const Monster &monster, uint8_t enemyId)
{
	if (enemyId > MaxMonsters) {
		enemyId -= MaxMonsters;
		if (enemyId >= Players.size())
			return false;
		return Players[enemyId].plractive;
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

size_t sync_all_monsters(std::byte *pbBuf, size_t dwMaxLen)
{
	if (ActiveMonsterCount < 1) {
		return dwMaxLen;
	}
	if (dwMaxLen < sizeof(TSyncHeader) + sizeof(TSyncMonster)) {
		return dwMaxLen;
	}
	if (MyPlayer->_pLvlChanging) {
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
	pHdr->wLen = SDL_SwapLE16(pHdr->wLen);

	return dwMaxLen;
}

uint32_t OnSyncData(const TCmd *pCmd, const Player &player)
{
	const auto &header = *reinterpret_cast<const TSyncHeader *>(pCmd);
	const uint16_t wLen = SDL_SwapLE16(header.wLen);

	assert(gbBufferMsgs != 2);

	if (gbBufferMsgs == 1) {
		return wLen + sizeof(header);
	}
	if (&player == MyPlayer) {
		return wLen + sizeof(header);
	}

	assert(header.wLen % sizeof(TSyncMonster) == 0);
	int monsterCount = wLen / sizeof(TSyncMonster);

	uint8_t level = header.bLevel;
	bool syncLocalLevel = !MyPlayer->_pLvlChanging && GetLevelForMultiplayer(*MyPlayer) == level;

	if (IsValidLevelForMultiplayer(level)) {
		const auto *monsterSyncs = reinterpret_cast<const TSyncMonster *>(pCmd + sizeof(header));
		bool isOwner = player.getId() > MyPlayerId;

		for (int i = 0; i < monsterCount; i++) {
			if (!IsTSyncMonsterValidate(monsterSyncs[i]))
				continue;

			if (syncLocalLevel) {
				SyncMonster(isOwner, monsterSyncs[i]);
			}

			delta_sync_monster(monsterSyncs[i], level);
		}
	}

	return wLen + sizeof(header);
}

void sync_init()
{
	sgnMonsters = 16 * MyPlayerId;
	memset(sgwLRU, 255, sizeof(sgwLRU));
}

} // namespace devilution
