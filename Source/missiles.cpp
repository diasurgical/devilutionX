/**
 * @file missiles.cpp
 *
 * Implementation of missile functionality.
 */
#include "missiles.h"

#include <climits>

#include "control.h"
#include "cursor.h"
#include "dead.h"
#include "engine/cel_header.hpp"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "init.h"
#include "inv.h"
#include "lighting.h"
#include "monster.h"
#include "spells.h"
#include "trigs.h"

namespace devilution {

int ActiveMissiles[MAXMISSILES];
int AvailableMissiles[MAXMISSILES];
MissileStruct Missiles[MAXMISSILES];
int ActiveMissileCount;
bool MissilePreFlag;

namespace {

ChainStruct chain[MAXMISSILES];
int numchains;

const int CrawlNum[19] = { 0, 3, 12, 45, 94, 159, 240, 337, 450, 579, 724, 885, 1062, 1255, 1464, 1689, 1930, 2187, 2460 };

int AddClassHealingBonus(int hp, HeroClass heroClass)
{
	switch (heroClass) {
	case HeroClass::Warrior:
	case HeroClass::Monk:
	case HeroClass::Barbarian:
		return hp * 2;
	case HeroClass::Rogue:
	case HeroClass::Bard:
		return hp + hp / 2;
	default:
		return hp;
	}
}

int ScaleSpellEffect(int base, int spellLevel)
{
	for (int i = 0; i < spellLevel; i++) {
		base += base / 8;
	}

	return base;
}

int GenerateRndSum(int range, int iterations)
{
	int value = 0;
	for (int i = 0; i < iterations; i++) {
		value += vanilla::GenerateRnd(range);
	}

	return value;
}

bool CheckBlock(Point from, Point to)
{
	while (from != to) {
		from += GetDirection(from, to);
		if (nSolidTable[dPiece[from.x][from.y]])
			return true;
	}

	return false;
}

inline bool InDungeonBounds(Point position)
{
	return position.x > 0 && position.x < MAXDUNX && position.y > 0 && position.y < MAXDUNY;
}

MonsterStruct *FindClosest(Point source, int rad)
{
	if (rad > 19)
		rad = 19;

	for (int i = 1; i < rad; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			int tx = source.x + CrawlTable[ck - 1];
			int ty = source.y + CrawlTable[ck];
			if (InDungeonBounds({ tx, ty })) {
				int mid = dMonster[tx][ty];
				if (mid > 0 && !CheckBlock(source, { tx, ty }))
					return &Monsters[mid - 1];
			}
		}
	}
	return nullptr;
}

constexpr Direction16 Direction16Flip(Direction16 x, Direction16 pivot)
{
	unsigned ret = (2 * pivot + 16 - x) % 16;

	return static_cast<Direction16>(ret);
}

void UpdateMissileVelocity(int i, Point source, Point destination, int v)
{
	Missiles[i].position.velocity = { 0, 0 };

	if (source == destination)
		return;

	double dxp = (destination.x + source.y - source.x - destination.y) * (1 << 21);
	double dyp = (destination.y + destination.x - source.x - source.y) * (1 << 21);
	double dr = sqrt(dxp * dxp + dyp * dyp);
	Missiles[i].position.velocity.deltaX = static_cast<int>((dxp * (v << 16)) / dr);
	Missiles[i].position.velocity.deltaY = static_cast<int>((dyp * (v << 15)) / dr);
}

/**
 * @brief Add the missile to the lookup tables
 * @param i Missiles index
 */
void PutMissile(int8_t i)
{
	Point position = Missiles[i].position.tile;

	if (!InDungeonBounds(position))
		Missiles[i]._miDelFlag = true;

	if (Missiles[i]._miDelFlag) {
		return;
	}

	dFlags[position.x][position.y] |= BFLAG_MISSILE;

	if (dMissile[position.x][position.y] == 0)
		dMissile[position.x][position.y] = i + 1;
	else
		dMissile[position.x][position.y] = -1;

	if (Missiles[i]._miPreFlag)
		MissilePreFlag = true;
}

void UpdateMissilePos(int i)
{
	int mx = Missiles[i].position.traveled.deltaX >> 16;
	int my = Missiles[i].position.traveled.deltaY >> 16;
	int dx = mx + 2 * my;
	int dy = 2 * my - mx;
	int lx = dx / 8;
	dx = dx / 64;
	int ly = dy / 8;
	dy = dy / 64;
	Missiles[i].position.tile = Missiles[i].position.start + Displacement { dx, dy };
	Missiles[i].position.offset.deltaX = mx + (dy * 32) - (dx * 32);
	Missiles[i].position.offset.deltaY = my - (dx * 16) - (dy * 16);
	ChangeLightOffset(Missiles[i]._mlid, { lx - (dx * 8), ly - (dy * 8) });
}

void MoveMissilePos(int i)
{
	int dx;
	int dy;

	switch (Missiles[i]._mimfnum) {
	case DIR_NW:
	case DIR_N:
	case DIR_NE:
		dx = 0;
		dy = 0;
		break;
	case DIR_E:
		dx = 1;
		dy = 0;
		break;
	case DIR_W:
		dx = 0;
		dy = 1;
		break;
	case DIR_S:
	case DIR_SW:
	case DIR_SE:
		dx = 1;
		dy = 1;
		break;
	}
	int x = Missiles[i].position.tile.x + dx;
	int y = Missiles[i].position.tile.y + dy;
	if (MonsterIsTileAvalible(Missiles[i]._misource, { x, y })) {
		Missiles[i].position.tile.x += dx;
		Missiles[i].position.tile.y += dy;
		Missiles[i].position.offset.deltaX += (dy * 32) - (dx * 32);
		Missiles[i].position.offset.deltaY -= (dy * 16) + (dx * 16);
	}
}

bool MonsterMHit(int pnum, int m, int mindam, int maxdam, int dist, int t, bool shift)
{
	auto &monster = Monsters[m];

	bool resist = false;
	if (monster.mtalkmsg != TEXT_NONE
	    || monster._mhitpoints >> 6 <= 0
	    || (t == MIS_HBOLT && monster.MType->mtype != MT_DIABLO && monster.MData->mMonstClass != MC_UNDEAD)) {
		return false;
	}
	if (monster.MType->mtype == MT_ILLWEAV && monster._mgoal == MGOAL_RETREAT)
		return false;
	if (monster._mmode == MM_CHARGE)
		return false;

	uint8_t mor = monster.mMagicRes;
	missile_resistance mir = MissileData[t].mResist;

	if (((mor & IMMUNE_MAGIC) != 0 && mir == MISR_MAGIC)
	    || ((mor & IMMUNE_FIRE) != 0 && mir == MISR_FIRE)
	    || ((mor & IMMUNE_LIGHTNING) != 0 && mir == MISR_LIGHTNING)
	    || ((mor & IMMUNE_ACID) != 0 && mir == MISR_ACID))
		return false;

	if (((mor & RESIST_MAGIC) != 0 && mir == MISR_MAGIC)
	    || ((mor & RESIST_FIRE) != 0 && mir == MISR_FIRE)
	    || ((mor & RESIST_LIGHTNING) != 0 && mir == MISR_LIGHTNING))
		resist = true;

	if (gbIsHellfire && t == MIS_HBOLT && (monster.MType->mtype == MT_DIABLO || monster.MType->mtype == MT_BONEDEMN))
		resist = true;

	int hit = vanilla::GenerateRnd(100);
	int hper = 0;
	if (pnum != -1) {
		const auto &player = Players[pnum];
		if (MissileData[t].mType == 0) {
			hper = player._pDexterity;
			hper += player._pIBonusToHit;
			hper += player._pLevel;
			hper -= monster.mArmorClass;
			hper -= (dist * dist) / 2;
			hper += player._pIEnAc;
			hper += 50;
			if (player._pClass == HeroClass::Rogue)
				hper += 20;
			if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Bard)
				hper += 10;
		} else {
			hper = player._pMagic - (monster.mLevel * 2) - dist + 50;
			if (player._pClass == HeroClass::Sorcerer)
				hper += 20;
			else if (player._pClass == HeroClass::Bard)
				hper += 10;
		}
	} else {
		hper = vanilla::GenerateRnd(75) - monster.mLevel * 2;
	}

	hper = clamp(hper, 5, 95);

	if (monster._mmode == MM_STONE)
		hit = 0;

	bool ret = false;
	if (CheckMonsterHit(monster, &ret))
		return ret;

#ifdef _DEBUG
	if (hit >= hper && !debug_mode_key_inverted_v && !debug_mode_dollar_sign)
		return false;
#else
	if (hit >= hper)
		return false;
#endif

	int dam;
	if (t == MIS_BONESPIRIT) {
		dam = monster._mhitpoints / 3 >> 6;
	} else {
		dam = mindam + vanilla::GenerateRnd(maxdam - mindam + 1);
	}

	if (MissileData[t].mType == 0) {
		const auto &player = Players[pnum];
		dam = player._pIBonusDamMod + dam * player._pIBonusDam / 100 + dam;
		if (player._pClass == HeroClass::Rogue)
			dam += player._pDamageMod;
		else
			dam += player._pDamageMod / 2;
	}

	if (!shift)
		dam <<= 6;
	if (resist)
		dam >>= 2;

	if (pnum == MyPlayerId)
		monster._mhitpoints -= dam;

	if ((gbIsHellfire && (Players[pnum]._pIFlags & ISPL_NOHEALMON) != 0) || (!gbIsHellfire && (Players[pnum]._pIFlags & ISPL_FIRE_ARROWS) != 0))
		monster._mFlags |= MFLAG_NOHEAL;

	if (monster._mhitpoints >> 6 <= 0) {
		if (monster._mmode == MM_STONE) {
			M_StartKill(m, pnum);
			monster.Petrify();
		} else {
			M_StartKill(m, pnum);
		}
	} else {
		if (resist) {
			PlayEffect(monster, 1);
		} else if (monster._mmode == MM_STONE) {
			if (m > MAX_PLRS - 1)
				M_StartHit(m, pnum, dam);
			monster.Petrify();
		} else {
			if (MissileData[t].mType == 0 && (Players[pnum]._pIFlags & ISPL_KNOCKBACK) != 0) {
				M_GetKnockback(m);
			}
			if (m > MAX_PLRS - 1)
				M_StartHit(m, pnum, dam);
		}
	}

	if (monster._msquelch == 0) {
		monster._msquelch = UINT8_MAX;
		monster.position.last = Players[pnum].position.tile;
	}

	return true;
}

bool Plr2PlrMHit(int pnum, int p, int mindam, int maxdam, int dist, int mtype, bool shift, bool *blocked)
{
	if (sgGameInitInfo.bFriendlyFire == 0 && gbFriendlyMode)
		return false;

	*blocked = false;

	auto &player = Players[pnum];
	auto &target = Players[p];

	if (target._pInvincible) {
		return false;
	}

	if (mtype == MIS_HBOLT) {
		return false;
	}

	if ((target._pSpellFlags & 1) != 0 && MissileData[mtype].mType == 0) {
		return false;
	}

	int8_t resper;
	switch (MissileData[mtype].mResist) {
	case MISR_FIRE:
		resper = target._pFireResist;
		break;
	case MISR_LIGHTNING:
		resper = target._pLghtResist;
		break;
	case MISR_MAGIC:
	case MISR_ACID:
		resper = target._pMagResist;
		break;
	default:
		resper = 0;
		break;
	}

	int hper = vanilla::GenerateRnd(100);

	int hit;
	if (MissileData[mtype].mType == 0) {
		hit = player._pIBonusToHit
		    + player._pLevel
		    - (dist * dist / 2)
		    - target._pDexterity / 5
		    - target._pIBonusAC
		    - target._pIAC
		    + player._pDexterity + 50;
		if (player._pClass == HeroClass::Rogue)
			hit += 20;
		if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Bard)
			hit += 10;
	} else {
		hit = player._pMagic
		    - (target._pLevel * 2)
		    - dist
		    + 50;
		if (player._pClass == HeroClass::Sorcerer)
			hit += 20;
		else if (player._pClass == HeroClass::Bard)
			hit += 10;
	}

	hit = clamp(hit, 5, 95);

	if (hper >= hit) {
		return false;
	}

	int blkper = 100;
	if (!shift && (target._pmode == PM_STAND || target._pmode == PM_ATTACK) && target._pBlockFlag) {
		blkper = vanilla::GenerateRnd(100);
	}

	int blk = target._pDexterity + target._pBaseToBlk + (target._pLevel * 2) - (player._pLevel * 2);
	blk = clamp(blk, 0, 100);

	int dam;
	if (mtype == MIS_BONESPIRIT) {
		dam = target._pHitPoints / 3;
	} else {
		dam = mindam + vanilla::GenerateRnd(maxdam - mindam + 1);
		if (MissileData[mtype].mType == 0)
			dam += player._pIBonusDamMod + player._pDamageMod + dam * player._pIBonusDam / 100;
		if (!shift)
			dam <<= 6;
	}
	if (MissileData[mtype].mType != 0)
		dam /= 2;
	if (resper > 0) {
		dam -= (dam * resper) / 100;
		if (pnum == MyPlayerId)
			NetSendCmdDamage(true, p, dam);
		player.Say(HeroSpeech::ArghClang);
		return true;
	}

	if (blkper < blk) {
		StartPlrBlock(p, GetDirection(target.position.tile, player.position.tile));
		*blocked = true;
	} else {
		if (pnum == MyPlayerId)
			NetSendCmdDamage(true, p, dam);
		StartPlrHit(p, dam, false);
	}

	return true;
}

void CheckMissileCol(int i, int mindam, int maxdam, bool shift, Point position, bool nodel)
{
	bool blocked;

	int mx = position.x;
	int my = position.y;

	if (i >= MAXMISSILES || i < 0)
		return;
	if (mx >= MAXDUNX || mx < 0)
		return;
	if (my >= MAXDUNY || my < 0)
		return;
	if (Missiles[i]._micaster != TARGET_BOTH && Missiles[i]._misource != -1) {
		if (Missiles[i]._micaster == TARGET_MONSTERS) {
			if (dMonster[mx][my] > 0) {
				if (MonsterMHit(
				        Missiles[i]._misource,
				        dMonster[mx][my] - 1,
				        mindam,
				        maxdam,
				        Missiles[i]._midist,
				        Missiles[i]._mitype,
				        shift)) {
					if (!nodel)
						Missiles[i]._mirange = 0;
					Missiles[i]._miHitFlag = true;
				}
			} else {
				if (dMonster[mx][my] < 0
				    && Monsters[-(dMonster[mx][my] + 1)]._mmode == MM_STONE
				    && MonsterMHit(
				        Missiles[i]._misource,
				        -(dMonster[mx][my] + 1),
				        mindam,
				        maxdam,
				        Missiles[i]._midist,
				        Missiles[i]._mitype,
				        shift)) {
					if (!nodel)
						Missiles[i]._mirange = 0;
					Missiles[i]._miHitFlag = true;
				}
			}
			if (dPlayer[mx][my] > 0
			    && dPlayer[mx][my] - 1 != Missiles[i]._misource
			    && Plr2PlrMHit(
			        Missiles[i]._misource,
			        dPlayer[mx][my] - 1,
			        mindam,
			        maxdam,
			        Missiles[i]._midist,
			        Missiles[i]._mitype,
			        shift,
			        &blocked)) {
				if (gbIsHellfire && blocked) {
					int dir = Missiles[i]._mimfnum + (vanilla::GenerateRnd(2) != 0 ? 1 : -1);
					int mAnimFAmt = MissileSpriteData[Missiles[i]._miAnimType].mAnimFAmt;
					if (dir < 0)
						dir = mAnimFAmt - 1;
					else if (dir > mAnimFAmt)
						dir = 0;

					SetMissDir(i, dir);
				} else if (!nodel) {
					Missiles[i]._mirange = 0;
				}
				Missiles[i]._miHitFlag = true;
			}
		} else {
			auto &monster = Monsters[Missiles[i]._misource];
			if ((monster._mFlags & MFLAG_TARGETS_MONSTER) != 0
			    && dMonster[mx][my] > 0
			    && (Monsters[dMonster[mx][my] - 1]._mFlags & MFLAG_GOLEM) != 0
			    && MonsterTrapHit(dMonster[mx][my] - 1, mindam, maxdam, Missiles[i]._midist, Missiles[i]._mitype, shift)) {
				if (!nodel)
					Missiles[i]._mirange = 0;
				Missiles[i]._miHitFlag = true;
			}
			if (dPlayer[mx][my] > 0
			    && PlayerMHit(
			        dPlayer[mx][my] - 1,
			        &monster,
			        Missiles[i]._midist,
			        mindam,
			        maxdam,
			        Missiles[i]._mitype,
			        shift,
			        0,
			        &blocked)) {
				if (gbIsHellfire && blocked) {
					int dir = Missiles[i]._mimfnum + (vanilla::GenerateRnd(2) != 0 ? 1 : -1);
					int mAnimFAmt = MissileSpriteData[Missiles[i]._miAnimType].mAnimFAmt;
					if (dir < 0)
						dir = mAnimFAmt - 1;
					else if (dir > mAnimFAmt)
						dir = 0;

					SetMissDir(i, dir);
				} else if (!nodel) {
					Missiles[i]._mirange = 0;
				}
				Missiles[i]._miHitFlag = true;
			}
		}
	} else {
		if (dMonster[mx][my] > 0) {
			if (Missiles[i]._micaster == TARGET_BOTH) {
				if (MonsterMHit(
				        Missiles[i]._misource,
				        dMonster[mx][my] - 1,
				        mindam,
				        maxdam,
				        Missiles[i]._midist,
				        Missiles[i]._mitype,
				        shift)) {
					if (!nodel)
						Missiles[i]._mirange = 0;
					Missiles[i]._miHitFlag = true;
				}
			} else if (MonsterTrapHit(dMonster[mx][my] - 1, mindam, maxdam, Missiles[i]._midist, Missiles[i]._mitype, shift)) {
				if (!nodel)
					Missiles[i]._mirange = 0;
				Missiles[i]._miHitFlag = true;
			}
		}
		if (dPlayer[mx][my] > 0) {
			if (PlayerMHit(
			        dPlayer[mx][my] - 1,
			        nullptr,
			        Missiles[i]._midist,
			        mindam,
			        maxdam,
			        Missiles[i]._mitype,
			        shift,
			        (Missiles[i]._miAnimType == MFILE_FIREWAL || Missiles[i]._miAnimType == MFILE_LGHNING) ? 1 : 0,
			        &blocked)) {
				if (gbIsHellfire && blocked) {
					int dir = Missiles[i]._mimfnum + (vanilla::GenerateRnd(2) != 0 ? 1 : -1);
					int mAnimFAmt = MissileSpriteData[Missiles[i]._miAnimType].mAnimFAmt;
					if (dir < 0)
						dir = mAnimFAmt - 1;
					else if (dir > mAnimFAmt)
						dir = 0;

					SetMissDir(i, dir);
				} else if (!nodel) {
					Missiles[i]._mirange = 0;
				}
				Missiles[i]._miHitFlag = true;
			}
		}
	}
	if (dObject[mx][my] != 0) {
		int oi = dObject[mx][my] > 0 ? dObject[mx][my] - 1 : -(dObject[mx][my] + 1);
		if (!Objects[oi]._oMissFlag) {
			if (Objects[oi]._oBreak == 1)
				BreakObject(-1, oi);
			if (!nodel)
				Missiles[i]._mirange = 0;
			Missiles[i]._miHitFlag = false;
		}
	}
	if (nMissileTable[dPiece[mx][my]]) {
		if (!nodel)
			Missiles[i]._mirange = 0;
		Missiles[i]._miHitFlag = false;
	}
	if (Missiles[i]._mirange == 0 && MissileData[Missiles[i]._mitype].miSFX != -1)
		PlaySfxLoc(MissileData[Missiles[i]._mitype].miSFX, Missiles[i].position.tile);
}

void SetMissAnim(int mi, int animtype)
{
	int dir = Missiles[mi]._mimfnum;

	if (animtype > MFILE_NONE) {
		animtype = MFILE_NONE;
	}

	Missiles[mi]._miAnimType = animtype;
	Missiles[mi]._miAnimFlags = MissileSpriteData[animtype].mFlags;
	Missiles[mi]._miAnimData = MissileSpriteData[animtype].mAnimData[dir];
	Missiles[mi]._miAnimDelay = MissileSpriteData[animtype].mAnimDelay[dir];
	Missiles[mi]._miAnimLen = MissileSpriteData[animtype].mAnimLen[dir];
	Missiles[mi]._miAnimWidth = MissileSpriteData[animtype].mAnimWidth[dir];
	Missiles[mi]._miAnimWidth2 = MissileSpriteData[animtype].mAnimWidth2[dir];
	Missiles[mi]._miAnimCnt = 0;
	Missiles[mi]._miAnimFrame = 1;
}

void FreeMissileGFX(int mi)
{
	if ((MissileSpriteData[mi].mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
		if (MissileSpriteData[mi].mAnimData[0] != nullptr) {
			auto *p = (DWORD *)MissileSpriteData[mi].mAnimData[0];
			p -= MissileSpriteData[mi].mAnimFAmt;
			delete[] p;
			MissileSpriteData[mi].mAnimData[0] = nullptr;
		}
		return;
	}

	for (int i = 0; i < MissileSpriteData[mi].mAnimFAmt; i++) {
		if (MissileSpriteData[mi].mAnimData[i] != nullptr) {
			delete[] MissileSpriteData[mi].mAnimData[i];
			MissileSpriteData[mi].mAnimData[i] = nullptr;
		}
	}
}

bool MissilesFoundTarget(int mi, Point *position, int rad)
{
	rad = std::min(rad, 19);

	for (int i = 0; i < rad; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			int tx = position->x + CrawlTable[ck - 1];
			int ty = position->y + CrawlTable[ck];
			if (!InDungeonBounds({ tx, ty }))
				continue;

			int dp = dPiece[tx][ty];
			if (nSolidTable[dp] || dObject[tx][ty] != 0 || dMissile[tx][ty] != 0)
				continue;

			Missiles[mi].position.tile = { tx, ty };
			*position = { tx, ty };
			return true;
		}
	}
	return false;
}

bool CheckIfTrig(Point position)
{
	for (int i = 0; i < numtrigs; i++) {
		if (trigs[i].position.WalkingDistance(position) < 2)
			return true;
	}
	return false;
}

bool GuardianTryFireAt(int i, Point target)
{
	auto &missile = Missiles[i];
	Point position = missile.position.tile;

	if (!LineClearMissile(position, target))
		return false;
	int mi = dMonster[target.x][target.y] - 1;
	if (mi < MAX_PLRS)
		return false;
	if (Monsters[mi]._mhitpoints >> 6 <= 0)
		return false;

	Direction dir = GetDirection(position, target);
	missile._miVar3 = AvailableMissiles[0];
	AddMissile(position, target, dir, MIS_FIREBOLT, TARGET_MONSTERS, missile._misource, missile._midam, GetSpellLevel(missile._misource, SPL_FIREBOLT));
	SetMissDir(i, 2);
	missile._miVar2 = 3;

	return true;
}

void FireballUpdate(int i, Displacement offset, bool alwaysDelete)
{
	Missiles[i]._mirange--;

	int id = Missiles[i]._misource;
	Point p = (Missiles[i]._micaster == TARGET_MONSTERS) ? Players[id].position.tile : Monsters[id].position.tile;

	if (Missiles[i]._miAnimType == MFILE_BIGEXP) {
		if (Missiles[i]._mirange == 0) {
			Missiles[i]._miDelFlag = true;
			AddUnLight(Missiles[i]._mlid);
		}
	} else {
		int dam = Missiles[i]._midam;
		Missiles[i].position.traveled += offset;
		UpdateMissilePos(i);
		if (Missiles[i].position.tile != Missiles[i].position.start)
			CheckMissileCol(i, dam, dam, false, Missiles[i].position.tile, false);
		if (Missiles[i]._mirange == 0) {
			Point m = Missiles[i].position.tile;
			ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, Missiles[i]._miAnimFrame);

			constexpr Displacement Pattern[] = { { 0, 0 }, { 0, 1 }, { 0, -1 }, { 1, 0 }, { 1, -1 }, { 1, 1 }, { -1, 0 }, { -1, 1 }, { -1, -1 } };
			for (auto shift : Pattern) {
				if (!CheckBlock(p, m + shift))
					CheckMissileCol(i, dam, dam, false, m + shift, true);
			}

			if (!TransList[dTransVal[m.x][m.y]]
			    || (Missiles[i].position.velocity.deltaX < 0 && ((TransList[dTransVal[m.x][m.y + 1]] && nSolidTable[dPiece[m.x][m.y + 1]]) || (TransList[dTransVal[m.x][m.y - 1]] && nSolidTable[dPiece[m.x][m.y - 1]])))) {
				Missiles[i].position.tile.x++;
				Missiles[i].position.tile.y++;
				Missiles[i].position.offset.deltaY -= 32;
			}
			if (Missiles[i].position.velocity.deltaY > 0
			    && ((TransList[dTransVal[m.x + 1][m.y]] && nSolidTable[dPiece[m.x + 1][m.y]])
			        || (TransList[dTransVal[m.x - 1][m.y]] && nSolidTable[dPiece[m.x - 1][m.y]]))) {
				Missiles[i].position.offset.deltaY -= 32;
			}
			if (Missiles[i].position.velocity.deltaX > 0
			    && ((TransList[dTransVal[m.x][m.y + 1]] && nSolidTable[dPiece[m.x][m.y + 1]])
			        || (TransList[dTransVal[m.x][m.y - 1]] && nSolidTable[dPiece[m.x][m.y - 1]]))) {
				Missiles[i].position.offset.deltaX -= 32;
			}
			Missiles[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			Missiles[i]._mirange = Missiles[i]._miAnimLen - 1;
			Missiles[i].position.velocity = {};
		} else if (Missiles[i].position.tile.x != Missiles[i]._miVar1 || Missiles[i].position.tile.y != Missiles[i]._miVar2) {
			Missiles[i]._miVar1 = Missiles[i].position.tile.x;
			Missiles[i]._miVar2 = Missiles[i].position.tile.y;
			ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, 8);
		}
		if (alwaysDelete)
			Missiles[i]._miDelFlag = true;
	}

	PutMissile(i);
}

void MissileRing(int i, int type)
{
	Missiles[i]._miDelFlag = true;
	int8_t src = Missiles[i]._micaster;
	uint8_t lvl = src > 0 ? Players[src]._pLevel : currlevel;
	int dmg = 16 * (GenerateRndSum(10, 2) + lvl + 2) / 2;

	int k = CrawlNum[3];
	int ck = k + 2;
	for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
		int tx = Missiles[i]._miVar1 + CrawlTable[ck - 1];
		int ty = Missiles[i]._miVar2 + CrawlTable[ck];
		if (!InDungeonBounds({ tx, ty }))
			continue;
		int dp = dPiece[tx][ty];
		if (nSolidTable[dp])
			continue;
		if (dObject[tx][ty] != 0)
			continue;
		if (!LineClearMissile(Missiles[i].position.tile, { tx, ty }))
			continue;
		if (nMissileTable[dp] || Missiles[i].limitReached) {
			Missiles[i].limitReached = true;
			continue;
		}

		AddMissile({ tx, ty }, { tx, ty }, 0, type, TARGET_BOTH, src, dmg, Missiles[i]._mispllvl);
	}
}

bool GrowWall(int playerId, Point position, Point target, missile_id type, int spellLevel, int damage)
{
	int dp = dPiece[position.x][position.y];
	assert(dp <= MAXTILES && dp >= 0);

	if (nMissileTable[dp] || !InDungeonBounds(target)) {
		return false;
	}

	AddMissile(position, position, Players[playerId]._pdir, type, TARGET_BOTH, playerId, damage, spellLevel);
	return true;
}

} // namespace

void GetDamageAmt(int i, int *mind, int *maxd)
{
	assert(MyPlayerId >= 0 && MyPlayerId < MAX_PLRS);
	assert(i >= 0 && i < 64);

	auto &myPlayer = Players[MyPlayerId];

	int sl = myPlayer._pSplLvl[i] + myPlayer._pISplLvlAdd;

	switch (i) {
	case SPL_FIREBOLT:
		*mind = (myPlayer._pMagic / 8) + sl + 1;
		*maxd = *mind + 9;
		break;
	case SPL_HEAL:
	case SPL_HEALOTHER:
		/// BUGFIX: healing calculation is unused
		*mind = AddClassHealingBonus(myPlayer._pLevel + sl + 1, myPlayer._pClass) - 1;
		*maxd = AddClassHealingBonus((4 * myPlayer._pLevel) + (6 * sl) + 10, myPlayer._pClass) - 1;
		break;
	case SPL_LIGHTNING:
	case SPL_RUNELIGHT:
		*mind = 2;
		*maxd = 2 + myPlayer._pLevel;
		break;
	case SPL_FLASH:
		*mind = ScaleSpellEffect(myPlayer._pLevel, sl);
		*mind += *mind / 2;
		*maxd = *mind * 2;
		break;
	case SPL_IDENTIFY:
	case SPL_TOWN:
	case SPL_STONE:
	case SPL_INFRA:
	case SPL_RNDTELEPORT:
	case SPL_MANASHIELD:
	case SPL_DOOMSERP:
	case SPL_BLODRIT:
	case SPL_INVISIBIL:
	case SPL_BLODBOIL:
	case SPL_TELEPORT:
	case SPL_ETHEREALIZE:
	case SPL_REPAIR:
	case SPL_RECHARGE:
	case SPL_DISARM:
	case SPL_RESURRECT:
	case SPL_TELEKINESIS:
	case SPL_BONESPIRIT:
	case SPL_WARP:
	case SPL_REFLECT:
	case SPL_BERSERK:
	case SPL_SEARCH:
	case SPL_RUNESTONE:
		*mind = -1;
		*maxd = -1;
		break;
	case SPL_FIREWALL:
	case SPL_LIGHTWALL:
	case SPL_FIRERING:
		*mind = 2 * myPlayer._pLevel + 4;
		*maxd = *mind + 36;
		break;
	case SPL_FIREBALL:
	case SPL_RUNEFIRE: {
		int base = (2 * myPlayer._pLevel) + 4;
		*mind = ScaleSpellEffect(base, sl);
		*maxd = ScaleSpellEffect(base + 36, sl);
	} break;
	case SPL_GUARDIAN: {
		int base = (myPlayer._pLevel / 2) + 1;
		*mind = ScaleSpellEffect(base, sl);
		*maxd = ScaleSpellEffect(base + 9, sl);
	} break;
	case SPL_CHAIN:
		*mind = 4;
		*maxd = 4 + (2 * myPlayer._pLevel);
		break;
	case SPL_WAVE:
		*mind = 6 * (myPlayer._pLevel + 1);
		*maxd = *mind + 54;
		break;
	case SPL_NOVA:
	case SPL_IMMOLAT:
	case SPL_RUNEIMMOLAT:
	case SPL_RUNENOVA:
		*mind = ScaleSpellEffect((myPlayer._pLevel + 5) / 2, sl) * 5;
		*maxd = ScaleSpellEffect((myPlayer._pLevel + 30) / 2, sl) * 5;
		break;
	case SPL_FLAME:
		*mind = 3;
		*maxd = myPlayer._pLevel + 4;
		*maxd += *maxd / 2;
		break;
	case SPL_GOLEM:
		*mind = 11;
		*maxd = 17;
		break;
	case SPL_APOCA:
		*mind = myPlayer._pLevel;
		*maxd = *mind * 6;
		break;
	case SPL_ELEMENT:
		*mind = ScaleSpellEffect(2 * myPlayer._pLevel + 4, sl);
		/// BUGFIX: add here '*mind /= 2;'
		*maxd = ScaleSpellEffect(2 * myPlayer._pLevel + 40, sl);
		/// BUGFIX: add here '*maxd /= 2;'
		break;
	case SPL_CBOLT:
		*mind = 1;
		*maxd = *mind + (myPlayer._pMagic / 4);
		break;
	case SPL_HBOLT:
		*mind = myPlayer._pLevel + 9;
		*maxd = *mind + 9;
		break;
	case SPL_FLARE:
		*mind = (myPlayer._pMagic / 2) + 3 * sl - (myPlayer._pMagic / 8);
		*maxd = *mind;
		break;
	}
}

int GetSpellLevel(int playerId, spell_id sn)
{
	auto &player = Players[playerId];

	if (playerId != MyPlayerId)
		return 1; // BUGFIX spell level will be wrong in multiplayer

	return std::max(player._pISplLvlAdd + player._pSplLvl[sn], 0);
}

/**
 * @brief Returns the direction a vector from p1(x1, y1) to p2(x2, y2) is pointing to.
 *
 *      W  sW  SW   Sw  S
 *              ^
 *     nW       |       Se
 *              |
 *     NW ------+-----> SE
 *              |
 *     Nw       |       sE
 *              |
 *      N  Ne  NE   nE  E
 *
 * @param x1 the x coordinate of p1
 * @param y1 the y coordinate of p1
 * @param x2 the x coordinate of p2
 * @param y2 the y coordinate of p2
 * @return the direction of the p1->p2 vector
 */
Direction16 GetDirection16(Point p1, Point p2)
{
	Displacement offset = p2 - p1;
	Displacement absolute = abs(offset);

	bool flipY = offset.deltaX != absolute.deltaX;
	bool flipX = offset.deltaY != absolute.deltaY;

	bool flipMedian = false;
	if (absolute.deltaX > absolute.deltaY) {
		std::swap(absolute.deltaX, absolute.deltaY);
		flipMedian = true;
	}

	Direction16 ret = DIR16_S;
	if (3 * absolute.deltaX <= (absolute.deltaY * 2)) { // mx/my <= 2/3, approximation of tan(33.75)
		if (5 * absolute.deltaX < absolute.deltaY)      // mx/my < 0.2, approximation of tan(11.25)
			ret = DIR16_SW;
		else
			ret = DIR16_Sw;
	}

	Direction16 medianPivot = DIR16_S;
	if (flipY) {
		ret = Direction16Flip(ret, DIR16_SW);
		medianPivot = Direction16Flip(medianPivot, DIR16_SW);
	}
	if (flipX) {
		ret = Direction16Flip(ret, DIR16_SE);
		medianPivot = Direction16Flip(medianPivot, DIR16_SE);
	}
	if (flipMedian)
		ret = Direction16Flip(ret, medianPivot);
	return ret;
}

void DeleteMissile(int mi, int i)
{
	if (Missiles[mi]._mitype == MIS_MANASHIELD) {
		int src = Missiles[mi]._misource;
		if (src == MyPlayerId)
			NetSendCmd(true, CMD_REMSHIELD);
		Players[src].pManaShield = false;
	}

	AvailableMissiles[MAXMISSILES - ActiveMissileCount] = mi;
	ActiveMissileCount--;
	if (ActiveMissileCount > 0 && i != ActiveMissileCount)
		ActiveMissiles[i] = ActiveMissiles[ActiveMissileCount];
}

bool MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, bool shift)
{
	auto &monster = Monsters[m];

	bool resist = false;
	if (monster.mtalkmsg != TEXT_NONE) {
		return false;
	}
	if (monster._mhitpoints >> 6 <= 0) {
		return false;
	}
	if (monster.MType->mtype == MT_ILLWEAV && monster._mgoal == MGOAL_RETREAT)
		return false;
	if (monster._mmode == MM_CHARGE)
		return false;

	missile_resistance mir = MissileData[t].mResist;
	int mor = monster.mMagicRes;
	if (((mor & IMMUNE_MAGIC) != 0 && mir == MISR_MAGIC)
	    || ((mor & IMMUNE_FIRE) != 0 && mir == MISR_FIRE)
	    || ((mor & IMMUNE_LIGHTNING) != 0 && mir == MISR_LIGHTNING)) {
		return false;
	}

	if (((mor & RESIST_MAGIC) != 0 && mir == MISR_MAGIC)
	    || ((mor & RESIST_FIRE) != 0 && mir == MISR_FIRE)
	    || ((mor & RESIST_LIGHTNING) != 0 && mir == MISR_LIGHTNING)) {
		resist = true;
	}

	int hit = vanilla::GenerateRnd(100);
	int hper = 90 - (BYTE)monster.mArmorClass - dist;
	if (hper < 5)
		hper = 5;
	if (hper > 95)
		hper = 95;
	bool ret;
	if (CheckMonsterHit(monster, &ret)) {
		return ret;
	}
#ifdef _DEBUG
	if (hit < hper || debug_mode_dollar_sign || debug_mode_key_inverted_v || monster._mmode == MM_STONE) {
#else
	if (hit < hper || monster._mmode == MM_STONE) {
#endif
		int dam = mindam + vanilla::GenerateRnd(maxdam - mindam + 1);
		if (!shift)
			dam <<= 6;
		if (resist)
			monster._mhitpoints -= dam / 4;
		else
			monster._mhitpoints -= dam;
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
			monster._mhitpoints = 0;
#endif
		if (monster._mhitpoints >> 6 <= 0) {
			if (monster._mmode == MM_STONE) {
				M_StartKill(m, -1);
				monster.Petrify();
			} else {
				M_StartKill(m, -1);
			}
		} else {
			if (resist) {
				PlayEffect(monster, 1);
			} else if (monster._mmode == MM_STONE) {
				if (m > MAX_PLRS - 1)
					M_StartHit(m, -1, dam);
				monster.Petrify();
			} else {
				if (m > MAX_PLRS - 1)
					M_StartHit(m, -1, dam);
			}
		}
		return true;
	}
	return false;
}

bool PlayerMHit(int pnum, MonsterStruct *monster, int dist, int mind, int maxd, int mtype, bool shift, int earflag, bool *blocked)
{
	*blocked = false;

	auto &player = Players[pnum];

	if (player._pHitPoints >> 6 <= 0) {
		return false;
	}

	if (player._pInvincible) {
		return false;
	}

	if ((player._pSpellFlags & 1) != 0 && MissileData[mtype].mType == 0) {
		return false;
	}

	int hit = vanilla::GenerateRnd(100);
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		hit = 1000;
#endif
	int hper = 40;
	if (MissileData[mtype].mType == 0) {
		int tac = player._pIAC + player._pIBonusAC + player._pDexterity / 5;
		if (monster != nullptr) {
			hper = monster->mHit
			    + ((monster->mLevel - player._pLevel) * 2)
			    + 30
			    - (dist * 2) - tac;
		} else {
			hper = 100 - (tac / 2) - (dist * 2);
		}
	} else if (monster != nullptr) {
		hper += (monster->mLevel * 2) - (player._pLevel * 2) - (dist * 2);
	}

	if (hper < 10)
		hper = 10;
	if (currlevel == 14 && hper < 20) {
		hper = 20;
	}
	if (currlevel == 15 && hper < 25) {
		hper = 25;
	}
	if (currlevel == 16 && hper < 30) {
		hper = 30;
	}

	int blk = 100;
	if ((player._pmode == PM_STAND || player._pmode == PM_ATTACK) && player._pBlockFlag) {
		blk = vanilla::GenerateRnd(100);
	}

	if (shift)
		blk = 100;
	if (mtype == MIS_ACIDPUD)
		blk = 100;

	int blkper = player._pBaseToBlk + player._pDexterity;
	if (monster != nullptr)
		blkper -= (monster->mLevel - player._pLevel) * 2;

	if (blkper < 0)
		blkper = 0;
	if (blkper > 100)
		blkper = 100;

	int8_t resper;
	switch (MissileData[mtype].mResist) {
	case MISR_FIRE:
		resper = player._pFireResist;
		break;
	case MISR_LIGHTNING:
		resper = player._pLghtResist;
		break;
	case MISR_MAGIC:
	case MISR_ACID:
		resper = player._pMagResist;
		break;
	default:
		resper = 0;
		break;
	}

	if (hit >= hper) {
		return false;
	}

	int dam;
	if (mtype == MIS_BONESPIRIT) {
		dam = player._pHitPoints / 3;
	} else {
		if (!shift) {
			dam = (mind << 6) + vanilla::GenerateRnd((maxd - mind + 1) << 6);
			if (monster == nullptr)
				if ((player._pIFlags & ISPL_ABSHALFTRAP) != 0)
					dam /= 2;
			dam += (player._pIGetHit << 6);
		} else {
			dam = mind + vanilla::GenerateRnd(maxd - mind + 1);
			if (monster == nullptr)
				if ((player._pIFlags & ISPL_ABSHALFTRAP) != 0)
					dam /= 2;
			dam += player._pIGetHit;
		}

		dam = std::max(dam, 64);
	}

	if ((resper <= 0 || gbIsHellfire) && blk < blkper) {
		Direction dir = player._pdir;
		if (monster != nullptr) {
			dir = GetDirection(player.position.tile, monster->position.tile);
		}
		*blocked = true;
		StartPlrBlock(pnum, dir);
		return true;
	}

	if (resper > 0) {
		dam -= dam * resper / 100;
		if (pnum == MyPlayerId) {
			ApplyPlrDamage(pnum, 0, 0, dam, earflag);
		}

		if (player._pHitPoints >> 6 > 0) {
			player.Say(HeroSpeech::ArghClang);
		}
		return true;
	}

	if (pnum == MyPlayerId) {
		ApplyPlrDamage(pnum, 0, 0, dam, earflag);
	}

	if (player._pHitPoints >> 6 > 0) {
		StartPlrHit(pnum, dam, false);
	}

	return true;
}

void SetMissDir(int mi, int dir)
{
	Missiles[mi]._mimfnum = dir;
	SetMissAnim(mi, Missiles[mi]._miAnimType);
}

void LoadMissileGFX(BYTE mi)
{
	MisFileData *mfd = &MissileSpriteData[mi];
	if (mfd->mAnimData[0] != nullptr)
		return;

	char pszName[256];
	if ((mfd->mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		byte *file = LoadFileInMem(pszName).release();
		for (int i = 0; i < mfd->mAnimFAmt; i++)
			mfd->mAnimData[i] = CelGetFrame(file, i);
	} else if (mfd->mAnimFAmt == 1) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		mfd->mAnimData[0] = LoadFileInMem(pszName).release();
	} else {
		for (unsigned i = 0; i < mfd->mAnimFAmt; i++) {
			sprintf(pszName, "Missiles\\%s%u.CL2", mfd->mName, i + 1);
			mfd->mAnimData[i] = LoadFileInMem(pszName).release();
		}
	}
}

void InitMissileGFX()
{
	for (int mi = 0; MissileSpriteData[mi].mAnimFAmt != 0; mi++) {
		if (!gbIsHellfire && mi > MFILE_SCBSEXPD)
			break;
		if ((MissileSpriteData[mi].mFlags & MFLAG_HIDDEN) == 0)
			LoadMissileGFX(mi);
	}
}

void FreeMissiles()
{
	for (int mi = 0; MissileSpriteData[mi].mAnimFAmt != 0; mi++) {
		if ((MissileSpriteData[mi].mFlags & MFLAG_HIDDEN) == 0)
			FreeMissileGFX(mi);
	}
}

void FreeMissiles2()
{
	for (int mi = 0; MissileSpriteData[mi].mAnimFAmt != 0; mi++) {
		if ((MissileSpriteData[mi].mFlags & MFLAG_HIDDEN) != 0)
			FreeMissileGFX(mi);
	}
}

void InitMissiles()
{
	auto &myPlayer = Players[MyPlayerId];

	AutoMapShowItems = false;
	myPlayer._pSpellFlags &= ~0x1;
	if (myPlayer._pInfraFlag) {
		for (int i = 0; i < ActiveMissileCount; ++i) {
			int mi = ActiveMissiles[i];
			if (Missiles[mi]._mitype == MIS_INFRA) {
				int src = Missiles[mi]._misource;
				if (src == MyPlayerId)
					CalcPlrItemVals(MyPlayerId, true);
			}
		}
	}

	if ((myPlayer._pSpellFlags & 2) == 2 || (myPlayer._pSpellFlags & 4) == 4) {
		myPlayer._pSpellFlags &= ~0x2;
		myPlayer._pSpellFlags &= ~0x4;
		for (int i = 0; i < ActiveMissileCount; ++i) {
			int mi = ActiveMissiles[i];
			if (Missiles[mi]._mitype == MIS_BLODBOIL) {
				if (Missiles[mi]._misource == MyPlayerId) {
					int missingHP = myPlayer._pMaxHP - myPlayer._pHitPoints;
					CalcPlrItemVals(MyPlayerId, true);
					ApplyPlrDamage(MyPlayerId, 0, 1, missingHP + Missiles[mi]._miVar2);
				}
			}
		}
	}

	ActiveMissileCount = 0;
	for (int i = 0; i < MAXMISSILES; i++) {
		AvailableMissiles[i] = i;
		ActiveMissiles[i] = 0;
	}
	numchains = 0;
	for (auto &link : chain) {
		link.idx = -1;
		link._mitype = 0;
		link._mirange = 0;
	}
	for (int j = 0; j < MAXDUNY; j++) {
		for (int i = 0; i < MAXDUNX; i++) { // NOLINT(modernize-loop-convert)
			dFlags[i][j] &= ~BFLAG_MISSILE;
		}
	}
	myPlayer.wReflections = 0;
}

void AddHiveExplosion(int mi, Point /*src*/, Point /*dst*/, int midir, int8_t mienemy, int id, int dam)
{
	for (int x : { 80, 81 }) {
		for (int y : { 62, 63 }) {
			AddMissile({ x, y }, { 80, 62 }, midir, MIS_HIVEEXP, mienemy, id, dam, 0);
		}
	}
	Missiles[mi]._miDelFlag = true;
}

void AddFireRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNEFIRE);
		if (MissilesFoundTarget(mi, &dst, 10)) {
			Missiles[mi]._miVar1 = MIS_HIVEEXP;
			Missiles[mi]._miDelFlag = false;
			Missiles[mi]._mlid = AddLight(dst, 8);
		} else {
			Missiles[mi]._miDelFlag = true;
		}
	} else {
		Missiles[mi]._miDelFlag = true;
	}
}

void AddLightningRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNELIGHT);
		if (MissilesFoundTarget(mi, &dst, 10)) {
			Missiles[mi]._miVar1 = MIS_LIGHTBALL;
			Missiles[mi]._miDelFlag = false;
			Missiles[mi]._mlid = AddLight(dst, 8);
		} else {
			Missiles[mi]._miDelFlag = true;
		}
	} else {
		Missiles[mi]._miDelFlag = true;
	}
}

void AddGreatLightningRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNENOVA);
		if (MissilesFoundTarget(mi, &dst, 10)) {
			Missiles[mi]._miVar1 = MIS_NOVA;
			Missiles[mi]._miDelFlag = false;
			Missiles[mi]._mlid = AddLight(dst, 8);
		} else {
			Missiles[mi]._miDelFlag = true;
		}
	} else {
		Missiles[mi]._miDelFlag = true;
	}
}

void AddImmolationRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNEIMMOLAT);
		if (MissilesFoundTarget(mi, &dst, 10)) {
			Missiles[mi]._miVar1 = MIS_IMMOLATION;
			Missiles[mi]._miDelFlag = false;
			Missiles[mi]._mlid = AddLight(dst, 8);
		} else {
			Missiles[mi]._miDelFlag = true;
		}
	} else {
		Missiles[mi]._miDelFlag = true;
	}
}

void AddStoneRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNESTONE);
		if (MissilesFoundTarget(mi, &dst, 10)) {
			Missiles[mi]._miVar1 = MIS_STONE;
			Missiles[mi]._miDelFlag = false;
			Missiles[mi]._mlid = AddLight(dst, 8);
		} else {
			Missiles[mi]._miDelFlag = true;
		}
	} else {
		Missiles[mi]._miDelFlag = true;
	}
}

void AddReflection(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (id >= 0) {
		int lvl = 2;
		if (Missiles[mi]._mispllvl != 0)
			lvl = Missiles[mi]._mispllvl;

		Players[id].wReflections += lvl * Players[id]._pLevel;

		UseMana(id, SPL_REFLECT);
	}

	Missiles[mi]._mirange = 0;
	Missiles[mi]._miDelFlag = false;
}

void AddBerserk(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._mirange = 0;
	Missiles[mi]._miDelFlag = true;

	if (id < 0)
		return;

	Missiles[mi]._misource = id;
	for (int i = 0; i < 6; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			int tx = dst.x + CrawlTable[ck - 1];
			int ty = dst.y + CrawlTable[ck];
			if (!InDungeonBounds({ tx, ty }))
				continue;

			int dm = dMonster[tx][ty];
			dm = dm > 0 ? dm - 1 : -(dm + 1);
			if (dm < MAX_PLRS)
				continue;
			auto &monster = Monsters[dm];

			if (monster._uniqtype != 0 || monster._mAi == AI_DIABLO)
				continue;
			if (monster._mmode == MM_FADEIN || monster._mmode == MM_FADEOUT)
				continue;
			if ((monster.mMagicRes & IMMUNE_MAGIC) != 0)
				continue;
			if ((monster.mMagicRes & RESIST_MAGIC) != 0 && ((monster.mMagicRes & RESIST_MAGIC) != 1 || vanilla::GenerateRnd(2) != 0))
				continue;
			if (monster._mmode == MM_CHARGE)
				continue;

			i = 6;
			auto slvl = GetSpellLevel(id, SPL_BERSERK);
			monster._mFlags |= MFLAG_BERSERK | MFLAG_GOLEM;
			monster.mMinDamage = (vanilla::GenerateRnd(10) + 120) * monster.mMinDamage / 100 + slvl;
			monster.mMaxDamage = (vanilla::GenerateRnd(10) + 120) * monster.mMaxDamage / 100 + slvl;
			monster.mMinDamage2 = (vanilla::GenerateRnd(10) + 120) * monster.mMinDamage2 / 100 + slvl;
			monster.mMaxDamage2 = (vanilla::GenerateRnd(10) + 120) * monster.mMaxDamage2 / 100 + slvl;
			int r = (currlevel < 17 || currlevel > 20) ? 3 : 9;
			monster.mlid = AddLight(monster.position.tile, r);
			UseMana(id, SPL_BERSERK);
			break;
		}
	}
}

void AddHorkSpawn(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	UpdateMissileVelocity(mi, src, dst, 8);
	Missiles[mi]._mirange = 9;
	Missiles[mi]._miVar1 = midir;
	PutMissile(mi);
}

void AddJester(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int spell = MIS_FIREBOLT;
	switch (vanilla::GenerateRnd(10)) {
	case 0:
	case 1:
		spell = MIS_FIREBOLT;
		break;
	case 2:
		spell = MIS_FIREBALL;
		break;
	case 3:
		spell = MIS_FIREWALLC;
		break;
	case 4:
		spell = MIS_GUARDIAN;
		break;
	case 5:
		spell = MIS_CHAIN;
		break;
	case 6:
		spell = MIS_TOWN;
		UseMana(id, SPL_TOWN);
		break;
	case 7:
		spell = MIS_TELEPORT;
		break;
	case 8:
		spell = MIS_APOCA;
		break;
	case 9:
		spell = MIS_STONE;
		break;
	}
	AddMissile(src, dst, midir, spell, Missiles[mi]._micaster, Missiles[mi]._misource, 0, Missiles[mi]._mispllvl);
	Missiles[mi]._miDelFlag = true;
	Missiles[mi]._mirange = 0;
}

void AddStealPotions(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._misource = id;
	for (int i = 0; i < 3; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			int tx = src.x + CrawlTable[ck - 1];
			int ty = src.y + CrawlTable[ck];
			if (!InDungeonBounds({ tx, ty }))
				continue;
			int8_t pnum = dPlayer[tx][ty];
			if (pnum == 0)
				continue;
			auto &player = Players[pnum > 0 ? pnum - 1 : -(pnum + 1)];

			bool hasPlayedSFX = false;
			for (int si = 0; si < MAXBELTITEMS; si++) {
				int ii = -1;
				if (player.SpdList[si]._itype == ITYPE_MISC) {
					if (vanilla::GenerateRnd(2) == 0)
						continue;
					switch (player.SpdList[si]._iMiscId) {
					case IMISC_FULLHEAL:
						ii = ItemMiscIdIdx(IMISC_HEAL);
						break;
					case IMISC_HEAL:
					case IMISC_MANA:
						player.RemoveSpdBarItem(si);
						break;
					case IMISC_FULLMANA:
						ii = ItemMiscIdIdx(IMISC_MANA);
						break;
					case IMISC_REJUV:
						if (vanilla::GenerateRnd(2) != 0) {
							ii = ItemMiscIdIdx(IMISC_MANA);
						} else {
							ii = ItemMiscIdIdx(IMISC_HEAL);
						}
						break;
					case IMISC_FULLREJUV:
						switch (vanilla::GenerateRnd(3)) {
						case 0:
							ii = ItemMiscIdIdx(IMISC_FULLMANA);
							break;
						case 1:
							ii = ItemMiscIdIdx(IMISC_FULLHEAL);
							break;
						default:
							ii = ItemMiscIdIdx(IMISC_REJUV);
							break;
						}
						break;
					default:
						continue;
					}
				}
				if (ii != -1) {
					SetPlrHandItem(&player.HoldItem, ii);
					GetPlrHandSeed(&player.HoldItem);
					player.HoldItem._iStatFlag = true;
					player.SpdList[si] = player.HoldItem;
				}
				if (!hasPlayedSFX) {
					PlaySfxLoc(IS_POPPOP2, { tx, ty });
					hasPlayedSFX = true;
				}
			}
			force_redraw = 255;
		}
	}
	Missiles[mi]._mirange = 0;
	Missiles[mi]._miDelFlag = true;
}

void AddManaTrap(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._misource = id;
	for (int i = 0; i < 3; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			int tx = src.x + CrawlTable[ck - 1];
			int ty = src.y + CrawlTable[ck];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				int8_t pid = dPlayer[tx][ty];
				if (pid != 0) {
					auto &player = Players[(pid > 0) ? pid - 1 : -(pid + 1)];

					player._pMana = 0;
					player._pManaBase = player._pMana + player._pMaxManaBase - player._pMaxMana;
					CalcPlrInv(pid, false);
					drawmanaflag = true;
					PlaySfxLoc(TSFX_COW7, { tx, ty });
				}
			}
		}
	}
	Missiles[mi]._mirange = 0;
	Missiles[mi]._miDelFlag = true;
}

void AddSpecArrow(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	int av = 0;

	if (mienemy == TARGET_MONSTERS) {
		auto &player = Players[id];

		if (player._pClass == HeroClass::Rogue)
			av += (player._pLevel - 1) / 4;
		else if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Bard)
			av += (player._pLevel - 1) / 8;

		if ((player._pIFlags & ISPL_QUICKATTACK) != 0)
			av++;
		if ((player._pIFlags & ISPL_FASTATTACK) != 0)
			av += 2;
		if ((player._pIFlags & ISPL_FASTERATTACK) != 0)
			av += 4;
		if ((player._pIFlags & ISPL_FASTESTATTACK) != 0)
			av += 8;
	}

	Missiles[mi]._mirange = 1;
	Missiles[mi]._miVar1 = dst.x;
	Missiles[mi]._miVar2 = dst.y;
	Missiles[mi]._miVar3 = av;
}

void AddWarp(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	int minDistanceSq = std::numeric_limits<int>::max();
	if (id >= 0) {
		src = Players[id].position.tile;
	}
	Point tile = src;

	for (int i = 0; i < numtrigs && i < MAXTRIGGERS; i++) {
		TriggerStruct *trg = &trigs[i];
		if (trg->_tmsg == WM_DIABTWARPUP || trg->_tmsg == WM_DIABPREVLVL || trg->_tmsg == WM_DIABNEXTLVL || trg->_tmsg == WM_DIABRTNLVL) {
			Point candidate = trg->position;
			if ((leveltype == DTYPE_CATHEDRAL || leveltype == DTYPE_CATACOMBS) && (trg->_tmsg == WM_DIABNEXTLVL || trg->_tmsg == WM_DIABPREVLVL || trg->_tmsg == WM_DIABRTNLVL)) {
				candidate += Displacement { 0, 1 };
			} else {
				candidate += Displacement { 1, 0 };
			}
			Displacement off = src - candidate;
			int distanceSq = off.deltaY * off.deltaY + off.deltaX * off.deltaX;
			if (distanceSq < minDistanceSq) {
				minDistanceSq = distanceSq;
				tile = candidate;
			}
		}
	}
	Missiles[mi]._mirange = 2;
	Missiles[mi]._miVar1 = 0;
	Missiles[mi].position.tile = tile;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_WARP);
}

void AddLightningWall(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int dam)
{
	UpdateMissileVelocity(mi, src, dst, 16);
	Missiles[mi]._midam = dam;
	Missiles[mi]._miAnimFrame = vanilla::GenerateRnd(8) + 1;
	Missiles[mi]._mirange = 255 * (Missiles[mi]._mispllvl + 1);
	if (id < 0) {
		Missiles[mi]._miVar1 = src.x;
		Missiles[mi]._miVar2 = src.y;
	} else {
		Missiles[mi]._miVar1 = Players[id].position.tile.x;
		Missiles[mi]._miVar2 = Players[id].position.tile.y;
	}
}

void AddRuneExplosion(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	if (mienemy == TARGET_MONSTERS || mienemy == TARGET_BOTH) {
		int dmg = 2 * (Players[id]._pLevel + GenerateRndSum(10, 2)) + 4;
		dmg = ScaleSpellEffect(dmg, Missiles[mi]._mispllvl);

		Missiles[mi]._midam = dmg;

		constexpr Displacement Offsets[] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };
		for (Displacement offset : Offsets)
			CheckMissileCol(mi, dmg, dmg, false, Missiles[mi].position.tile + offset, true);
	}
	Missiles[mi]._mlid = AddLight(src, 8);
	SetMissDir(mi, 0);
	Missiles[mi]._miDelFlag = false;
	Missiles[mi]._mirange = Missiles[mi]._miAnimLen - 1;
}

void AddImmolation(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int sp = 16;
	if (mienemy == TARGET_MONSTERS) {
		sp += std::min(Missiles[mi]._mispllvl * 2, 34);

		int dmg = 2 * (Players[id]._pLevel + GenerateRndSum(10, 2)) + 4;
		Missiles[mi]._midam = ScaleSpellEffect(dmg, Missiles[mi]._mispllvl);

		UseMana(id, SPL_FIREBALL);
	}
	UpdateMissileVelocity(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._miVar4 = src.x;
	Missiles[mi]._miVar5 = src.y;
	Missiles[mi]._miVar6 = 2;
	Missiles[mi]._miVar7 = 2;
	Missiles[mi]._mlid = AddLight(src, 8);
}

void AddFireNova(int mi, Point src, Point dst, int midir, int8_t mienemy, int /*id*/, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int sp = 16;
	if (mienemy == TARGET_MONSTERS) {
		sp += std::min(Missiles[mi]._mispllvl, 34);
	}
	UpdateMissileVelocity(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._miVar4 = src.x;
	Missiles[mi]._miVar5 = src.y;
	Missiles[mi]._mlid = AddLight(src, 8);
}

void AddLightningArrow(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	UpdateMissileVelocity(mi, src, dst, 32);
	Missiles[mi]._miAnimFrame = vanilla::GenerateRnd(8) + 1;
	Missiles[mi]._mirange = 255;
	if (id < 0) {
		Missiles[mi]._miVar1 = src.x;
		Missiles[mi]._miVar2 = src.y;
	} else {
		Missiles[mi]._miVar1 = Players[id].position.tile.x;
		Missiles[mi]._miVar2 = Players[id].position.tile.y;
	}
	Missiles[mi]._midam <<= 6;
}

void AddMana(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &player = Players[id];

	int manaAmount = (vanilla::GenerateRnd(10) + 1) << 6;
	for (int i = 0; i < player._pLevel; i++) {
		manaAmount += (vanilla::GenerateRnd(4) + 1) << 6;
	}
	for (int i = 0; i < Missiles[mi]._mispllvl; i++) {
		manaAmount += (vanilla::GenerateRnd(6) + 1) << 6;
	}
	if (player._pClass == HeroClass::Sorcerer)
		manaAmount *= 2;
	if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Bard)
		manaAmount += manaAmount / 2;
	player._pMana += manaAmount;
	if (player._pMana > player._pMaxMana)
		player._pMana = player._pMaxMana;
	player._pManaBase += manaAmount;
	if (player._pManaBase > player._pMaxManaBase)
		player._pManaBase = player._pMaxManaBase;
	UseMana(id, SPL_MANA);
	Missiles[mi]._miDelFlag = true;
	drawmanaflag = true;
}

void AddMagi(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &player = Players[id];

	player._pMana = player._pMaxMana;
	player._pManaBase = player._pMaxManaBase;
	UseMana(id, SPL_MAGI);
	Missiles[mi]._miDelFlag = true;
	drawmanaflag = true;
}

void AddRing(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_FIRERING);
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._miDelFlag = false;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._miVar4 = 0;
	Missiles[mi]._miVar5 = 0;
	Missiles[mi]._miVar6 = 0;
	Missiles[mi]._miVar7 = 0;
	Missiles[mi].limitReached = false;
	Missiles[mi]._mirange = 7;
}

void AddSearch(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = false;
	Missiles[mi]._miVar1 = id;
	Missiles[mi]._miVar2 = 0;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._miVar4 = 0;
	Missiles[mi]._miVar5 = 0;
	Missiles[mi]._miVar6 = 0;
	Missiles[mi]._miVar7 = 0;
	Missiles[mi].limitReached = false;
	AutoMapShowItems = true;
	int lvl = 2;
	if (id > -1)
		lvl = Players[id]._pLevel * 2;
	Missiles[mi]._mirange = lvl + 10 * Missiles[mi]._mispllvl + 245;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_SEARCH);

	for (int i = 0; i < ActiveMissileCount; i++) {
		int mx = ActiveMissiles[i];
		if (mx != mi) {
			MissileStruct *mis = &Missiles[mx];
			if (mis->_miVar1 == id && mis->_mitype == 85) {
				int r1 = Missiles[mi]._mirange;
				int r2 = mis->_mirange;
				if (r2 < INT_MAX - r1)
					mis->_mirange = r1 + r2;
				Missiles[mi]._miDelFlag = true;
				break;
			}
		}
	}
}

void AddCboltArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int /*id*/, int /*dam*/)
{
	Missiles[mi]._mirnd = vanilla::GenerateRnd(15) + 1;
	if (mienemy != TARGET_MONSTERS) {
		Missiles[mi]._midam = 15;
	}

	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	Missiles[mi]._miAnimFrame = vanilla::GenerateRnd(8) + 1;
	Missiles[mi]._mlid = AddLight(src, 5);
	UpdateMissileVelocity(mi, src, dst, 8);
	Missiles[mi]._miVar1 = 5;
	Missiles[mi]._miVar2 = midir;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._mirange = 256;
}

void AddLArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int av = 32;
	if (mienemy == TARGET_MONSTERS) {
		auto &player = Players[id];
		if (player._pClass == HeroClass::Rogue)
			av += (player._pLevel) / 4;
		else if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Bard)
			av += (player._pLevel) / 8;

		if (gbIsHellfire) {
			if ((player._pIFlags & ISPL_QUICKATTACK) != 0)
				av++;
			if ((player._pIFlags & ISPL_FASTATTACK) != 0)
				av += 2;
			if ((player._pIFlags & ISPL_FASTERATTACK) != 0)
				av += 4;
			if ((player._pIFlags & ISPL_FASTESTATTACK) != 0)
				av += 8;
		} else {
			if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Bard)
				av -= 1;
		}
	}
	UpdateMissileVelocity(mi, src, dst, av);

	SetMissDir(mi, GetDirection16(src, dst));
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._mlid = AddLight(src, 5);
}

void AddArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int av = 32;
	if (mienemy == TARGET_MONSTERS) {
		auto &player = Players[id];

		if ((player._pIFlags & ISPL_RNDARROWVEL) != 0) {
			av = vanilla::GenerateRnd(32) + 16;
		}
		if (player._pClass == HeroClass::Rogue)
			av += (player._pLevel - 1) / 4;
		else if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Bard)
			av += (player._pLevel - 1) / 8;

		if (gbIsHellfire) {
			if ((player._pIFlags & ISPL_QUICKATTACK) != 0)
				av++;
			if ((player._pIFlags & ISPL_FASTATTACK) != 0)
				av += 2;
			if ((player._pIFlags & ISPL_FASTERATTACK) != 0)
				av += 4;
			if ((player._pIFlags & ISPL_FASTESTATTACK) != 0)
				av += 8;
		}
	}
	UpdateMissileVelocity(mi, src, dst, av);
	Missiles[mi]._miAnimFrame = GetDirection16(src, dst) + 1;
	Missiles[mi]._mirange = 256;
}

void UpdateVileMissPos(int mi, Point dst)
{
	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			int yy = j + dst.y;
			for (int i = -k; i <= k; i++) {
				int xx = i + dst.x;
				if (PosOkPlayer(MyPlayerId, { xx, yy })) {
					Missiles[mi].position.tile = { xx, yy };
					return;
				}
			}
		}
	}
	Missiles[mi].position.tile = dst;
}

void AddRndTeleport(int mi, Point src, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	int pn;
	int r1;
	int r2;

	int nTries = 0;
	do {
		nTries++;
		if (nTries > 500) {
			r1 = 0;
			r2 = 0;
			break; //BUGFIX: warps player to 0/0 in hellfire, change to return or use 1.09's version of the code
		}
		r1 = vanilla::GenerateRnd(3) + 4;
		r2 = vanilla::GenerateRnd(3) + 4;
		if (vanilla::GenerateRnd(2) == 1)
			r1 = -r1;
		if (vanilla::GenerateRnd(2) == 1)
			r2 = -r2;

		r1 += src.x;
		r2 += src.y;
		if (r1 < MAXDUNX && r1 >= 0 && r2 < MAXDUNY && r2 >= 0) { ///BUGFIX: < MAXDUNX / < MAXDUNY (fixed)
			pn = dPiece[r1][r2];
		}
	} while (nSolidTable[pn] || dObject[r1][r2] != 0 || dMonster[r1][r2] != 0);

	Missiles[mi]._mirange = 2;
	Missiles[mi]._miVar1 = 0;
	if (!setlevel || setlvlnum != SL_VILEBETRAYER) {
		Missiles[mi].position.tile = { r1, r2 };
		if (mienemy == TARGET_MONSTERS)
			UseMana(id, SPL_RNDTELEPORT);
	} else {
		int oi = dObject[dst.x][dst.y] - 1;
		// BUGFIX: should only run magic circle check if dObject[dx][dy] is non-zero.
		if (Objects[oi]._otype == OBJ_MCIRCLE1 || Objects[oi]._otype == OBJ_MCIRCLE2) {
			Missiles[mi].position.tile = dst;
			if (!PosOkPlayer(MyPlayerId, dst))
				UpdateVileMissPos(mi, dst);
		}
	}
}

void AddFirebolt(int mi, Point src, Point dst, int midir, int8_t micaster, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int sp = 26;
	if (micaster == 0) {
		sp = 16;
		if (id != -1) {
			sp += std::min(Missiles[mi]._mispllvl * 2, 47);
		}

		int i;
		for (i = 0; i < ActiveMissileCount; i++) {
			int mx = ActiveMissiles[i];
			if (Missiles[mx]._mitype == MIS_GUARDIAN && Missiles[mx]._misource == id && Missiles[mx]._miVar3 == mi)
				break;
		}
		if (i == ActiveMissileCount)
			UseMana(id, SPL_FIREBOLT);
	}
	UpdateMissileVelocity(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._mlid = AddLight(src, 8);
}

void AddMagmaball(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	UpdateMissileVelocity(mi, src, dst, 16);
	Missiles[mi].position.traveled.deltaX += 3 * Missiles[mi].position.velocity.deltaX;
	Missiles[mi].position.traveled.deltaY += 3 * Missiles[mi].position.velocity.deltaY;
	UpdateMissilePos(mi);
	if (!gbIsHellfire || (Missiles[mi].position.velocity.deltaX & 0xFFFF0000) != 0 || (Missiles[mi].position.velocity.deltaY & 0xFFFF0000) != 0)
		Missiles[mi]._mirange = 256;
	else
		Missiles[mi]._mirange = 1;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._mlid = AddLight(src, 8);
}

void AddKrull(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	UpdateMissileVelocity(mi, src, dst, 16);
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	PutMissile(mi);
}

void AddTeleport(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	for (int i = 0; i < 6; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			int tx = dst.x + CrawlTable[ck - 1];
			int ty = dst.y + CrawlTable[ck];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				if (!nSolidTable[dPiece[tx][ty]] && dMonster[tx][ty] == 0 && dObject[tx][ty] == 0 && dPlayer[tx][ty] == 0) {
					Missiles[mi].position.tile = { tx, ty };
					Missiles[mi].position.start = { tx, ty };
					Missiles[mi]._miDelFlag = false;
					i = 6;
					break;
				}
			}
		}
	}

	if (!Missiles[mi]._miDelFlag) {
		UseMana(id, SPL_TELEPORT);
		Missiles[mi]._mirange = 2;
	}
}

void AddLightball(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int dam)
{
	UpdateMissileVelocity(mi, src, dst, 16);
	Missiles[mi]._midam = dam;
	Missiles[mi]._miAnimFrame = vanilla::GenerateRnd(8) + 1;
	Missiles[mi]._mirange = 255;
	if (id < 0) {
		Missiles[mi]._miVar1 = src.x;
		Missiles[mi]._miVar2 = src.y;
	} else {
		Missiles[mi]._miVar1 = Players[id].position.tile.x;
		Missiles[mi]._miVar2 = Players[id].position.tile.y;
	}
}

void AddFirewall(int mi, Point src, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	Missiles[mi]._midam = GenerateRndSum(10, 2) + 2;
	Missiles[mi]._midam += id >= 0 ? Players[id]._pLevel : currlevel; // BUGFIX: missing parenthesis around ternary (fixed)
	Missiles[mi]._midam <<= 3;
	UpdateMissileVelocity(mi, src, dst, 16);
	int i = Missiles[mi]._mispllvl;
	Missiles[mi]._mirange = 10;
	if (i > 0)
		Missiles[mi]._mirange *= i + 1;
	if (mienemy == TARGET_PLAYERS || id < 0)
		Missiles[mi]._mirange += currlevel;
	else
		Missiles[mi]._mirange += (Players[id]._pISplDur * Missiles[mi]._mirange) / 128;
	Missiles[mi]._mirange *= 16;
	Missiles[mi]._miVar1 = Missiles[mi]._mirange - Missiles[mi]._miAnimLen;
	Missiles[mi]._miVar2 = 0;
}

void AddFireball(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int sp = 16;
	if (mienemy == TARGET_MONSTERS) {
		sp += std::min(Missiles[mi]._mispllvl * 2, 34);

		int dmg = 2 * (Players[id]._pLevel + GenerateRndSum(10, 2)) + 4;
		Missiles[mi]._midam = ScaleSpellEffect(dmg, Missiles[mi]._mispllvl);

		UseMana(id, SPL_FIREBALL);
	}
	UpdateMissileVelocity(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._miVar4 = src.x;
	Missiles[mi]._miVar5 = src.y;
	Missiles[mi]._mlid = AddLight(src, 8);
}

void AddLightctrl(int mi, Point src, Point dst, int /*midir*/, int8_t mienemy, int id, int dam)
{
	if (dam == 0 && mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_LIGHTNING);
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	UpdateMissileVelocity(mi, src, dst, 32);
	Missiles[mi]._miAnimFrame = vanilla::GenerateRnd(8) + 1;
	Missiles[mi]._mirange = 256;
}

void AddLightning(int mi, Point /*src*/, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	Missiles[mi].position.start = dst;
	if (midir >= 0) {
		Missiles[mi].position.offset = Missiles[midir].position.offset;
		Missiles[mi].position.traveled = Missiles[midir].position.traveled;
	}
	Missiles[mi]._miAnimFrame = vanilla::GenerateRnd(8) + 1;

	if (midir < 0 || mienemy == TARGET_PLAYERS || id == -1) {
		if (midir < 0 || id == -1)
			Missiles[mi]._mirange = 8;
		else
			Missiles[mi]._mirange = 10;
	} else {
		Missiles[mi]._mirange = (Missiles[mi]._mispllvl / 2) + 6;
	}
	Missiles[mi]._mlid = AddLight(Missiles[mi].position.tile, 4);
}

void AddMisexp(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	if (mienemy != 0 && id >= 0) {
		switch (Monsters[id].MType->mtype) {
		case MT_SUCCUBUS:
			SetMissAnim(mi, MFILE_FLAREEXP);
			break;
		case MT_SNOWWICH:
			SetMissAnim(mi, MFILE_SCBSEXPB);
			break;
		case MT_HLSPWN:
			SetMissAnim(mi, MFILE_SCBSEXPD);
			break;
		case MT_SOLBRNR:
			SetMissAnim(mi, MFILE_SCBSEXPC);
			break;
		default:
			break;
		}
	}

	Missiles[mi].position.tile = Missiles[dst.x].position.tile;
	Missiles[mi].position.start = Missiles[dst.x].position.start;
	Missiles[mi].position.offset = Missiles[dst.x].position.offset;
	Missiles[mi].position.traveled = Missiles[dst.x].position.traveled;
	Missiles[mi].position.velocity = { 0, 0 };
	Missiles[mi]._mirange = Missiles[mi]._miAnimLen;
	Missiles[mi]._miVar1 = 0;
}

void AddWeapexp(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	Missiles[mi].position.tile = src;
	Missiles[mi].position.start = src;
	Missiles[mi].position.velocity = { 0, 0 };
	Missiles[mi]._miVar1 = 0;
	Missiles[mi]._miVar2 = dst.x;
	Missiles[mi]._mimfnum = 0;
	if (dst.x == 1)
		SetMissAnim(mi, MFILE_MAGBLOS);
	else
		SetMissAnim(mi, MFILE_MINILTNG);
	Missiles[mi]._mirange = Missiles[mi]._miAnimLen - 1;
}

void AddTown(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int tx = dst.x;
	int ty = dst.y;
	if (currlevel != 0) {
		Missiles[mi]._miDelFlag = true;
		for (int i = 0; i < 6; i++) {
			int k = CrawlNum[i];
			int ck = k + 2;
			for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
				tx = dst.x + CrawlTable[ck - 1];
				ty = dst.y + CrawlTable[ck];
				if (InDungeonBounds({ tx, ty })) {
					int dp = dPiece[tx][ty];
					if (dMissile[tx][ty] == 0 && !nSolidTable[dp] && !nMissileTable[dp] && dObject[tx][ty] == 0 && dPlayer[tx][ty] == 0) {
						if (!CheckIfTrig({ tx, ty })) {
							Missiles[mi].position.tile = { tx, ty };
							Missiles[mi].position.start = { tx, ty };
							Missiles[mi]._miDelFlag = false;
							i = 6;
							break;
						}
					}
				}
			}
		}
	} else {
		Missiles[mi].position.tile = { tx, ty };
		Missiles[mi].position.start = { tx, ty };
		Missiles[mi]._miDelFlag = false;
	}
	Missiles[mi]._mirange = 100;
	Missiles[mi]._miVar1 = Missiles[mi]._mirange - Missiles[mi]._miAnimLen;
	Missiles[mi]._miVar2 = 0;
	for (int i = 0; i < ActiveMissileCount; i++) {
		int mx = ActiveMissiles[i];
		if (Missiles[mx]._mitype == MIS_TOWN && mx != mi && Missiles[mx]._misource == id)
			Missiles[mx]._mirange = 0;
	}
	PutMissile(mi);
	if (id == MyPlayerId && !Missiles[mi]._miDelFlag && currlevel != 0) {
		if (!setlevel) {
			NetSendCmdLocParam3(true, CMD_ACTIVATEPORTAL, { tx, ty }, currlevel, leveltype, 0);
		} else {
			NetSendCmdLocParam3(true, CMD_ACTIVATEPORTAL, { tx, ty }, setlvlnum, leveltype, 1);
		}
	}
}

void AddFlash(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	if (id != -1) {
		if (mienemy == TARGET_MONSTERS) {
			int dmg = GenerateRndSum(20, Players[id]._pLevel + 1) + Players[id]._pLevel + 1;
			Missiles[mi]._midam = ScaleSpellEffect(dmg, Missiles[mi]._mispllvl);
			Missiles[mi]._midam += Missiles[mi]._midam / 2;
			UseMana(id, SPL_FLASH);
		} else {
			Missiles[mi]._midam = Monsters[id].mLevel * 2;
		}
	} else {
		Missiles[mi]._midam = currlevel / 2;
	}
	Missiles[mi]._mirange = 19;
}

void AddFlash2(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	if (mienemy == TARGET_MONSTERS) {
		if (id != -1) {
			int dmg = GenerateRndSum(20, Players[id]._pLevel + 1) + Players[id]._pLevel + 1;
			Missiles[mi]._midam = ScaleSpellEffect(dmg, Missiles[mi]._mispllvl);
			Missiles[mi]._midam += Missiles[mi]._midam / 2;
		} else {
			Missiles[mi]._midam = currlevel / 2;
		}
	}
	Missiles[mi]._miPreFlag = true;
	Missiles[mi]._mirange = 19;
}

void AddManashield(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	Missiles[mi]._mirange = 48 * Players[id]._pLevel;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_MANASHIELD);
	if (id == MyPlayerId)
		NetSendCmd(true, CMD_SETSHIELD);
	Players[id].pManaShield = true;
}

void AddFiremove(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._midam = vanilla::GenerateRnd(10) + Players[id]._pLevel + 1;
	UpdateMissileVelocity(mi, src, dst, 16);
	Missiles[mi]._mirange = 255;
	Missiles[mi]._miVar1 = 0;
	Missiles[mi]._miVar2 = 0;
	Missiles[mi].position.tile.x++;
	Missiles[mi].position.tile.y++;
	Missiles[mi].position.offset.deltaY -= 32;
}

void AddGuardian(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int dmg = vanilla::GenerateRnd(10) + (Players[id]._pLevel / 2) + 1;
	Missiles[mi]._midam = ScaleSpellEffect(dmg, Missiles[mi]._mispllvl);

	Missiles[mi]._miDelFlag = true;
	for (int i = 0; i < 6; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			int tx = dst.x + CrawlTable[ck - 1];
			int ty = dst.y + CrawlTable[ck];
			k = dPiece[tx][ty];
			if (InDungeonBounds({ tx, ty })) {
				if (LineClearMissile(src, { tx, ty })) {
					if (dMonster[tx][ty] == 0 && !nSolidTable[k] && !nMissileTable[k] && dObject[tx][ty] == 0 && dMissile[tx][ty] == 0) {
						Missiles[mi].position.tile = { tx, ty };
						Missiles[mi].position.start = { tx, ty };
						Missiles[mi]._miDelFlag = false;
						UseMana(id, SPL_GUARDIAN);
						i = 6;
						break;
					}
				}
			}
		}
	}

	if (!Missiles[mi]._miDelFlag) {
		Missiles[mi]._misource = id;
		Missiles[mi]._mlid = AddLight(Missiles[mi].position.tile, 1);
		Missiles[mi]._mirange = Missiles[mi]._mispllvl + (Players[id]._pLevel / 2);
		Missiles[mi]._mirange += (Missiles[mi]._mirange * Players[id]._pISplDur) / 128;

		if (Missiles[mi]._mirange > 30)
			Missiles[mi]._mirange = 30;
		Missiles[mi]._mirange <<= 4;
		if (Missiles[mi]._mirange < 30)
			Missiles[mi]._mirange = 30;

		Missiles[mi]._miVar1 = Missiles[mi]._mirange - Missiles[mi]._miAnimLen;
		Missiles[mi]._miVar2 = 0;
		Missiles[mi]._miVar3 = 1;
	}
}

void AddChain(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miVar1 = dst.x;
	Missiles[mi]._miVar2 = dst.y;
	Missiles[mi]._mirange = 1;
	UseMana(id, SPL_CHAIN);
}

void AddBloodStar(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	SetMissDir(mi, dst.x);
	Missiles[mi]._midam = 0;
	Missiles[mi]._miLightFlag = true;
	Missiles[mi]._mirange = 250;
}

void AddBone(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	if (dst.x > 3)
		dst.x = 2;
	SetMissDir(mi, dst.x);
	Missiles[mi]._midam = 0;
	Missiles[mi]._miLightFlag = true;
	Missiles[mi]._mirange = 250;
}

void AddMetlHit(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	if (dst.x > 3)
		dst.x = 2;
	SetMissDir(mi, dst.x);
	Missiles[mi]._midam = 0;
	Missiles[mi]._miLightFlag = true;
	Missiles[mi]._mirange = Missiles[mi]._miAnimLen;
}

namespace {
void InitMissileAnimationFromMonster(MissileStruct &mis, int midir, const MonsterStruct &mon, MonsterGraphic graphic)
{
	const AnimStruct &anim = mon.MType->GetAnimData(graphic);
	mis._mimfnum = midir;
	mis._miAnimFlags = 0;
	const auto &celSprite = *anim.CelSpritesForDirections[midir];
	mis._miAnimData = celSprite.Data();
	mis._miAnimDelay = anim.Rate;
	mis._miAnimLen = anim.Frames;
	mis._miAnimWidth = celSprite.Width();
	mis._miAnimWidth2 = CalculateWidth2(celSprite.Width());
	mis._miAnimAdd = 1;
	mis._miVar1 = 0;
	mis._miVar2 = 0;
	mis._miLightFlag = true;
	mis._mirange = 256;
}
} // namespace

void AddRhino(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &monster = Monsters[id];

	MonsterGraphic graphic = MonsterGraphic::Special;
	if (monster.MType->mtype < MT_HORNED || monster.MType->mtype > MT_OBLORD) {
		if (monster.MType->mtype < MT_NSNAKE || monster.MType->mtype > MT_GSNAKE) {
			graphic = MonsterGraphic::Walk;
		} else {
			graphic = MonsterGraphic::Attack;
		}
	}
	UpdateMissileVelocity(mi, src, dst, 18);
	InitMissileAnimationFromMonster(Missiles[mi], midir, monster, graphic);
	if (monster.MType->mtype >= MT_NSNAKE && monster.MType->mtype <= MT_GSNAKE)
		Missiles[mi]._miAnimFrame = 7;
	if (monster._uniqtype != 0) {
		Missiles[mi]._miUniqTrans = monster._uniqtrans + 1;
		Missiles[mi]._mlid = monster.mlid;
	}
	PutMissile(mi);
}

void AddFireman(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &monster = Monsters[id];

	UpdateMissileVelocity(mi, src, dst, 16);
	InitMissileAnimationFromMonster(Missiles[mi], midir, monster, MonsterGraphic::Walk);
	if (monster._uniqtype != 0)
		Missiles[mi]._miUniqTrans = monster._uniqtrans + 1;
	dMonster[monster.position.tile.x][monster.position.tile.y] = 0;
	PutMissile(mi);
}

void AddFlare(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	UpdateMissileVelocity(mi, src, dst, 16);
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._mlid = AddLight(src, 8);
	if (mienemy == TARGET_MONSTERS) {
		UseMana(id, SPL_FLARE);
		ApplyPlrDamage(id, 5);
	} else if (id > 0) {
		auto &monster = Monsters[id];
		if (monster.MType->mtype == MT_SUCCUBUS)
			SetMissAnim(mi, MFILE_FLARE);
		if (monster.MType->mtype == MT_SNOWWICH)
			SetMissAnim(mi, MFILE_SCUBMISB);
		if (monster.MType->mtype == MT_HLSPWN)
			SetMissAnim(mi, MFILE_SCUBMISD);
		if (monster.MType->mtype == MT_SOLBRNR)
			SetMissAnim(mi, MFILE_SCUBMISC);
	}

	if (MissileSpriteData[Missiles[mi]._miAnimType].mAnimFAmt == 16) {
		SetMissDir(mi, GetDirection16(src, dst));
	}
}

void AddAcid(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	UpdateMissileVelocity(mi, src, dst, 16);
	SetMissDir(mi, GetDirection16(src, dst));
	if ((!gbIsHellfire && (Missiles[mi].position.velocity.deltaX & 0xFFFF0000) != 0) || (Missiles[mi].position.velocity.deltaY & 0xFFFF0000) != 0)
		Missiles[mi]._mirange = 5 * (Monsters[id]._mint + 4);
	else
		Missiles[mi]._mirange = 1;
	Missiles[mi]._mlid = NO_LIGHT;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	PutMissile(mi);
}

void AddFireWallA(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int dam)
{
	Missiles[mi]._midam = dam;
	Missiles[mi].position.velocity = { 0, 0 };
	Missiles[mi]._mirange = 50;
	Missiles[mi]._miVar1 = Missiles[mi]._mirange - Missiles[mi]._miAnimLen;
	Missiles[mi]._miVar2 = 0;
}

void AddAcidpud(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	Missiles[mi].position.velocity = { 0, 0 };
	Missiles[mi].position.offset = { 0, 0 };
	Missiles[mi]._miLightFlag = true;
	int monst = Missiles[mi]._misource;
	Missiles[mi]._mirange = vanilla::GenerateRnd(15) + 40 * (Monsters[monst]._mint + 1);
	Missiles[mi]._miPreFlag = true;
}

void AddStone(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int tx;
	int ty;

	bool faded = false;
	Missiles[mi]._misource = id;
	for (int i = 0; i < 6; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			tx = dst.x + CrawlTable[ck - 1];
			ty = dst.y + CrawlTable[ck];
			if (InDungeonBounds({ tx, ty })) {
				int mid = dMonster[tx][ty];
				if (mid == 0)
					continue;
				mid = mid > 0 ? mid - 1 : -(mid + 1);
				auto &monster = Monsters[mid];
				if (mid > MAX_PLRS - 1 && monster._mAi != AI_DIABLO && monster.MType->mtype != MT_NAKRUL) {
					if (monster._mmode != MM_FADEIN && monster._mmode != MM_FADEOUT && monster._mmode != MM_CHARGE) {
						faded = true;
						i = 6;
						Missiles[mi]._miVar1 = monster._mmode;
						Missiles[mi]._miVar2 = mid;
						monster.Petrify();
						break;
					}
				}
			}
		}
	}

	if (!faded) {
		Missiles[mi]._miDelFlag = true;
		return;
	}

	Missiles[mi].position.tile = { tx, ty };
	Missiles[mi].position.start = Missiles[mi].position.tile;
	Missiles[mi]._mirange = Missiles[mi]._mispllvl + 6;
	Missiles[mi]._mirange += (Missiles[mi]._mirange * Players[id]._pISplDur) / 128;

	if (Missiles[mi]._mirange > 15)
		Missiles[mi]._mirange = 15;
	Missiles[mi]._mirange <<= 4;
	UseMana(id, SPL_STONE);
}

void AddGolem(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = false;
	for (int i = 0; i < ActiveMissileCount; i++) {
		int mx = ActiveMissiles[i];
		if (Missiles[mx]._mitype == MIS_GOLEM) {
			if (mx != mi && Missiles[mx]._misource == id) {
				Missiles[mi]._miDelFlag = true;
				return;
			}
		}
	}
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._miVar4 = dst.x;
	Missiles[mi]._miVar5 = dst.y;
	if ((Monsters[id].position.tile.x != 1 || Monsters[id].position.tile.y != 0) && id == MyPlayerId)
		M_StartKill(id, id);
	UseMana(id, SPL_GOLEM);
}

void AddEtherealize(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	auto &player = Players[id];

	int range = 8 * player._pLevel;
	Missiles[mi]._mirange = ScaleSpellEffect(range, Missiles[mi]._mispllvl);

	Missiles[mi]._mirange += Missiles[mi]._mirange * player._pISplDur / 128;
	Missiles[mi]._miVar1 = player._pHitPoints;
	Missiles[mi]._miVar2 = player._pHPBase;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_ETHEREALIZE);
}

void AddDummy(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
}

void AddBlodbur(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int dam)
{
	Missiles[mi]._midam = dam;
	Missiles[mi].position.tile = src;
	Missiles[mi].position.start = src;
	Missiles[mi]._misource = id;
	if (dam == 1)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, 1);
	Missiles[mi]._miLightFlag = true;
	Missiles[mi]._mirange = Missiles[mi]._miAnimLen;
}

void AddBoom(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int dam)
{
	Missiles[mi].position.tile = dst;
	Missiles[mi].position.start = dst;
	Missiles[mi].position.velocity = { 0, 0 };
	Missiles[mi]._midam = dam;
	Missiles[mi]._mirange = Missiles[mi]._miAnimLen;
	Missiles[mi]._miVar1 = 0;
}

void AddHeal(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &player = Players[id];

	int hp = (vanilla::GenerateRnd(10) + 1) << 6;
	for (int i = 0; i < player._pLevel; i++) {
		hp += (vanilla::GenerateRnd(4) + 1) << 6;
	}
	for (int i = 0; i < Missiles[mi]._mispllvl; i++) {
		hp += (vanilla::GenerateRnd(6) + 1) << 6;
	}

	if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Barbarian || player._pClass == HeroClass::Monk) {
		hp *= 2;
	} else if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Bard) {
		hp += hp / 2;
	}

	player._pHitPoints = std::min(player._pHitPoints + hp, player._pMaxHP);
	player._pHPBase = std::min(player._pHPBase + hp, player._pMaxHPBase);

	UseMana(id, SPL_HEAL);
	Missiles[mi]._miDelFlag = true;
	drawhpflag = true;
}

void AddHealOther(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	UseMana(id, SPL_HEALOTHER);
	if (id == MyPlayerId) {
		NewCursor(CURSOR_HEALOTHER);
		if (sgbControllerActive)
			TryIconCurs();
	}
}

void AddElement(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}

	int dmg = 2 * (Players[id]._pLevel + GenerateRndSum(10, 2)) + 4;
	Missiles[mi]._midam = ScaleSpellEffect(dmg, Missiles[mi]._mispllvl) / 2;

	UpdateMissileVelocity(mi, src, dst, 16);
	SetMissDir(mi, GetDirection(src, dst));
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._miVar4 = dst.x;
	Missiles[mi]._miVar5 = dst.y;
	Missiles[mi]._mlid = AddLight(src, 8);
	UseMana(id, SPL_ELEMENT);
}

extern void FocusOnInventory();

void AddIdentify(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	UseMana(id, SPL_IDENTIFY);
	if (id == MyPlayerId) {
		if (sbookflag)
			sbookflag = false;
		if (!invflag) {
			invflag = true;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		NewCursor(CURSOR_IDENTIFY);
	}
}

void AddFirewallC(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	for (int i = 0; i < 6; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			int tx = dst.x + CrawlTable[ck - 1];
			int ty = dst.y + CrawlTable[ck];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				k = dPiece[tx][ty];
				if (LineClearMissile(src, { tx, ty })) {
					if (src != Point { tx, ty } && !nSolidTable[k] && dObject[tx][ty] == 0) {
						Missiles[mi]._miVar1 = tx;
						Missiles[mi]._miVar2 = ty;
						Missiles[mi]._miVar5 = tx;
						Missiles[mi]._miVar6 = ty;
						Missiles[mi]._miDelFlag = false;
						i = 6;
						break;
					}
				}
			}
		}
	}

	if (!Missiles[mi]._miDelFlag) {
		Missiles[mi]._miVar7 = 0;
		Missiles[mi].limitReached = false;
		Missiles[mi]._miVar3 = left[left[midir]];
		Missiles[mi]._miVar4 = right[right[midir]];
		Missiles[mi]._mirange = 7;
		UseMana(id, SPL_FIREWALL);
	}
}

void AddInfra(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	Missiles[mi]._mirange = ScaleSpellEffect(1584, Missiles[mi]._mispllvl);
	Missiles[mi]._mirange += Missiles[mi]._mirange * Players[id]._pISplDur / 128;

	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_INFRA);
}

void AddWave(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miVar1 = dst.x;
	Missiles[mi]._miVar2 = dst.y;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._miVar4 = 0;
	Missiles[mi]._mirange = 1;
	Missiles[mi]._miAnimFrame = 4;
	UseMana(id, SPL_WAVE);
}

void AddNova(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	Missiles[mi]._miVar1 = dst.x;
	Missiles[mi]._miVar2 = dst.y;

	if (id != -1) {
		int dmg = GenerateRndSum(6, 5) + Players[id]._pLevel + 5;
		Missiles[mi]._midam = ScaleSpellEffect(dmg / 2, Missiles[mi]._mispllvl);

		if (mienemy == TARGET_MONSTERS)
			UseMana(id, SPL_NOVA);
	} else {
		Missiles[mi]._midam = (currlevel / 2) + GenerateRndSum(3, 3);
	}

	Missiles[mi]._mirange = 1;
}

void AddBlodboil(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &player = Players[id];

	if ((player._pSpellFlags & 6) != 0 || player._pHitPoints <= player._pLevel << 6) {
		Missiles[mi]._miDelFlag = true;
		return;
	}

	UseMana(id, SPL_BLODBOIL);
	Missiles[mi]._miVar1 = id;
	int tmp = 3 * player._pLevel;
	tmp <<= 7;
	player._pSpellFlags |= 2;
	Missiles[mi]._miVar2 = tmp;
	int lvl = player._pLevel * 2;
	Missiles[mi]._mirange = lvl + 10 * Missiles[mi]._mispllvl + 245;
	CalcPlrItemVals(id, true);
	force_redraw = 255;
	player.Say(HeroSpeech::Aaaaargh);
}

void AddRepair(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	UseMana(id, SPL_REPAIR);
	if (id == MyPlayerId) {
		if (sbookflag)
			sbookflag = false;
		if (!invflag) {
			invflag = true;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		NewCursor(CURSOR_REPAIR);
	}
}

void AddRecharge(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	UseMana(id, SPL_RECHARGE);
	if (id == MyPlayerId) {
		if (sbookflag)
			sbookflag = false;
		if (!invflag) {
			invflag = true;
			if (sgbControllerActive)
				FocusOnInventory();
		}
		NewCursor(CURSOR_RECHARGE);
	}
}

void AddDisarm(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	UseMana(id, SPL_DISARM);
	if (id == MyPlayerId) {
		NewCursor(CURSOR_DISARM);
		if (sgbControllerActive) {
			if (pcursobj != -1)
				NetSendCmdLocParam1(true, CMD_DISARMXY, { cursmx, cursmy }, pcursobj);
			else
				NewCursor(CURSOR_HAND);
		}
	}
}

void AddApoca(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miVar1 = 8;
	Missiles[mi]._miVar2 = src.y - Missiles[mi]._miVar1;
	Missiles[mi]._miVar3 = Missiles[mi]._miVar1 + src.y;
	Missiles[mi]._miVar4 = src.x - Missiles[mi]._miVar1;
	Missiles[mi]._miVar5 = Missiles[mi]._miVar1 + src.x;
	Missiles[mi]._miVar6 = Missiles[mi]._miVar4;
	if (Missiles[mi]._miVar2 <= 0)
		Missiles[mi]._miVar2 = 1;
	if (Missiles[mi]._miVar3 >= MAXDUNY)
		Missiles[mi]._miVar3 = MAXDUNY - 1;
	if (Missiles[mi]._miVar4 <= 0)
		Missiles[mi]._miVar4 = 1;
	if (Missiles[mi]._miVar5 >= MAXDUNX)
		Missiles[mi]._miVar5 = MAXDUNX - 1;
	Missiles[mi]._midam = GenerateRndSum(6, Players[id]._pLevel) + Players[id]._pLevel;
	Missiles[mi]._mirange = 255;
	Missiles[mi]._miDelFlag = false;
	UseMana(id, SPL_APOCA);
}

void AddFlame(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam)
{
	Missiles[mi]._miVar2 = 5 * dam;
	Missiles[mi].position.start = dst;
	Missiles[mi].position.offset = Missiles[midir].position.offset;
	Missiles[mi].position.traveled = Missiles[midir].position.traveled;
	Missiles[mi]._mirange = Missiles[mi]._miVar2 + 20;
	Missiles[mi]._mlid = AddLight(src, 1);
	if (mienemy == TARGET_MONSTERS) {
		int i = vanilla::GenerateRnd(Players[id]._pLevel) + vanilla::GenerateRnd(2);
		Missiles[mi]._midam = 8 * i + 16 + ((8 * i + 16) / 2);
	} else {
		auto &monster = Monsters[id];
		Missiles[mi]._midam = monster.mMinDamage + vanilla::GenerateRnd(monster.mMaxDamage - monster.mMinDamage + 1);
	}
}

void AddFlamec(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	UpdateMissileVelocity(mi, src, dst, 32);
	if (mienemy == TARGET_MONSTERS) {
		UseMana(id, SPL_FLAME);
	}
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._mirange = 256;
}

void AddCbolt(int mi, Point src, Point dst, int midir, int8_t micaster, int id, int /*dam*/)
{
	assert((DWORD)mi < MAXMISSILES);

	Missiles[mi]._mirnd = vanilla::GenerateRnd(15) + 1;
	Missiles[mi]._midam = (micaster == 0) ? (vanilla::GenerateRnd(Players[id]._pMagic / 4) + 1) : 15;

	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	Missiles[mi]._miAnimFrame = vanilla::GenerateRnd(8) + 1;
	Missiles[mi]._mlid = AddLight(src, 5);

	UpdateMissileVelocity(mi, src, dst, 8);
	Missiles[mi]._miVar1 = 5;
	Missiles[mi]._miVar2 = midir;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._mirange = 256;
}

void AddHbolt(int mi, Point src, Point dst, int midir, int8_t /*micaster*/, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}

	int sp = 16;
	if (id != -1) {
		sp += std::min(Missiles[mi]._mispllvl * 2, 47);
	}

	UpdateMissileVelocity(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._mlid = AddLight(src, 8);
	Missiles[mi]._midam = vanilla::GenerateRnd(10) + Players[id]._pLevel + 9;
	UseMana(id, SPL_HBOLT);
}

void AddResurrect(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	UseMana(id, SPL_RESURRECT);
	if (id == MyPlayerId) {
		NewCursor(CURSOR_RESURRECT);
		if (sgbControllerActive)
			TryIconCurs();
	}
	Missiles[mi]._miDelFlag = true;
}

void AddResurrectBeam(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	Missiles[mi].position.tile = dst;
	Missiles[mi].position.start = Missiles[mi].position.tile;
	Missiles[mi].position.velocity = { 0, 0 };
	Missiles[mi]._mirange = MissileSpriteData[MFILE_RESSUR1].mAnimLen[0];
}

void AddTelekinesis(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	Missiles[mi]._miDelFlag = true;
	UseMana(id, SPL_TELEKINESIS);
	if (id == MyPlayerId)
		NewCursor(CURSOR_TELEKINESIS);
}

void AddBoneSpirit(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	Missiles[mi]._midam = 0;
	UpdateMissileVelocity(mi, src, dst, 16);
	SetMissDir(mi, GetDirection(src, dst));
	Missiles[mi]._mirange = 256;
	Missiles[mi]._miVar1 = src.x;
	Missiles[mi]._miVar2 = src.y;
	Missiles[mi]._miVar3 = 0;
	Missiles[mi]._miVar4 = dst.x;
	Missiles[mi]._miVar5 = dst.y;
	Missiles[mi]._mlid = AddLight(src, 8);
	if (mienemy == TARGET_MONSTERS) {
		UseMana(id, SPL_BONESPIRIT);
		ApplyPlrDamage(id, 6);
	}
}

void AddRportal(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	Missiles[mi].position.tile = src;
	Missiles[mi].position.start = src;
	Missiles[mi]._mirange = 100;
	Missiles[mi]._miVar1 = 100 - Missiles[mi]._miAnimLen;
	Missiles[mi]._miVar2 = 0;
	PutMissile(mi);
}

void AddDiabApoca(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int dam)
{
	int players = gbIsMultiplayer ? MAX_PLRS : 1;
	for (int pnum = 0; pnum < players; pnum++) {
		auto &player = Players[pnum];
		if (!player.plractive)
			continue;
		if (!LineClearMissile(src, player.position.future))
			continue;

		AddMissile({ 0, 0 }, player.position.future, 0, MIS_BOOM2, mienemy, id, dam, 0);
	}
	Missiles[mi]._miDelFlag = true;
}

int AddMissile(Point src, Point dst, int midir, int mitype, int8_t micaster, int id, int midam, int spllvl)
{
	if (ActiveMissileCount >= MAXMISSILES - 1)
		return -1;

	if (mitype == MIS_MANASHIELD && Players[id].pManaShield) {
		if (currlevel != Players[id].plrlevel)
			return -1;

		for (int i = 0; i < ActiveMissileCount; i++) {
			int mi = ActiveMissiles[i];
			if (Missiles[mi]._mitype == MIS_MANASHIELD && Missiles[mi]._misource == id)
				return -1;
		}
	}

	int mi = AvailableMissiles[0];

	AvailableMissiles[0] = AvailableMissiles[MAXMISSILES - ActiveMissileCount - 1];
	ActiveMissiles[ActiveMissileCount] = mi;
	ActiveMissileCount++;

	memset(&Missiles[mi], 0, sizeof(*Missiles));

	Missiles[mi]._mitype = mitype;
	Missiles[mi]._micaster = micaster;
	Missiles[mi]._misource = id;
	Missiles[mi]._miAnimType = MissileData[mitype].mFileNum;
	Missiles[mi]._miDrawFlag = MissileData[mitype].mDraw;
	Missiles[mi]._mispllvl = spllvl;
	Missiles[mi]._mimfnum = midir;

	if (Missiles[mi]._miAnimType == MFILE_NONE || MissileSpriteData[Missiles[mi]._miAnimType].mAnimFAmt < 8)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, midir);

	Missiles[mi].position.tile = src;
	Missiles[mi].position.offset = { 0, 0 };
	Missiles[mi].position.start = src;
	Missiles[mi].position.traveled = { 0, 0 };
	Missiles[mi]._miDelFlag = false;
	Missiles[mi]._miAnimAdd = 1;
	Missiles[mi]._miLightFlag = false;
	Missiles[mi]._miPreFlag = false;
	Missiles[mi]._miUniqTrans = 0;
	Missiles[mi]._midam = midam;
	Missiles[mi]._miHitFlag = false;
	Missiles[mi]._midist = 0;
	Missiles[mi]._mlid = NO_LIGHT;
	Missiles[mi]._mirnd = 0;

	if (MissileData[mitype].mlSFX != -1) {
		PlaySfxLoc(MissileData[mitype].mlSFX, Missiles[mi].position.start);
	}

	MissileData[mitype].mAddProc(mi, src, dst, midir, micaster, id, midam);

	return mi;
}

void MI_Dummy(int i)
{
}

void MI_Golem(int mi)
{
	int src = Missiles[mi]._misource;
	if (Monsters[src].position.tile.x == 1 && Monsters[src].position.tile.y == 0) {
		for (int i = 0; i < 6; i++) {
			int k = CrawlNum[i];
			int ck = k + 2;
			for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
				const int8_t *ct = &CrawlTable[ck];
				int tx = Missiles[mi]._miVar4 + *(ct - 1);
				int ty = Missiles[mi]._miVar5 + *ct;
				if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
					int dp = dPiece[tx][ty];
					if (LineClearMissile({ Missiles[mi]._miVar1, Missiles[mi]._miVar2 }, { tx, ty })) {
						if (dMonster[tx][ty] == 0 && !nSolidTable[dp] && dObject[tx][ty] == 0) {
							i = 6;
							SpawnGolum(src, { tx, ty }, mi);
							break;
						}
					}
				}
			}
		}
	}
	Missiles[mi]._miDelFlag = true;
}

void MI_LArrow(int i)
{
	Missiles[i]._mirange--;
	int p = Missiles[i]._misource;
	if (Missiles[i]._miAnimType == MFILE_MINILTNG || Missiles[i]._miAnimType == MFILE_MAGBLOS) {
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, Missiles[i]._miAnimFrame + 5);
		missile_resistance rst = MissileData[Missiles[i]._mitype].mResist;
		if (Missiles[i]._mitype == MIS_LARROW) {
			int mind;
			int maxd;
			if (p != -1) {
				mind = Players[p]._pILMinDam;
				maxd = Players[p]._pILMaxDam;
			} else {
				mind = vanilla::GenerateRnd(10) + 1 + currlevel;
				maxd = vanilla::GenerateRnd(10) + 1 + currlevel * 2;
			}
			MissileData[MIS_LARROW].mResist = MISR_LIGHTNING;
			CheckMissileCol(i, mind, maxd, false, Missiles[i].position.tile, true);
		}
		if (Missiles[i]._mitype == MIS_FARROW) {
			int mind;
			int maxd;
			if (p != -1) {
				mind = Players[p]._pIFMinDam;
				maxd = Players[p]._pIFMaxDam;
			} else {
				mind = vanilla::GenerateRnd(10) + 1 + currlevel;
				maxd = vanilla::GenerateRnd(10) + 1 + currlevel * 2;
			}
			MissileData[MIS_FARROW].mResist = MISR_FIRE;
			CheckMissileCol(i, mind, maxd, false, Missiles[i].position.tile, true);
		}
		MissileData[Missiles[i]._mitype].mResist = rst;
	} else {
		Missiles[i]._midist++;
		Missiles[i].position.traveled += Missiles[i].position.velocity;
		UpdateMissilePos(i);

		int mind;
		int maxd;
		if (p != -1) {
			if (Missiles[i]._micaster == TARGET_MONSTERS) {
				mind = Players[p]._pIMinDam;
				maxd = Players[p]._pIMaxDam;
			} else {
				mind = Monsters[p].mMinDamage;
				maxd = Monsters[p].mMaxDamage;
			}
		} else {
			mind = vanilla::GenerateRnd(10) + 1 + currlevel;
			maxd = vanilla::GenerateRnd(10) + 1 + currlevel * 2;
		}

		if (Missiles[i].position.tile != Missiles[i].position.start) {
			missile_resistance rst = MissileData[Missiles[i]._mitype].mResist;
			MissileData[Missiles[i]._mitype].mResist = MISR_NONE;
			CheckMissileCol(i, mind, maxd, false, Missiles[i].position.tile, false);
			MissileData[Missiles[i]._mitype].mResist = rst;
		}
		if (Missiles[i]._mirange == 0) {
			Missiles[i]._mimfnum = 0;
			Missiles[i].position.traveled -= Missiles[i].position.velocity;
			UpdateMissilePos(i);
			if (Missiles[i]._mitype == MIS_LARROW)
				SetMissAnim(i, MFILE_MINILTNG);
			else
				SetMissAnim(i, MFILE_MAGBLOS);
			Missiles[i]._mirange = Missiles[i]._miAnimLen - 1;
			Missiles[i].position.StopMissile();
		} else {
			if (Missiles[i].position.tile.x != Missiles[i]._miVar1 || Missiles[i].position.tile.y != Missiles[i]._miVar2) {
				Missiles[i]._miVar1 = Missiles[i].position.tile.x;
				Missiles[i]._miVar2 = Missiles[i].position.tile.y;
				ChangeLight(Missiles[i]._mlid, { Missiles[i]._miVar1, Missiles[i]._miVar2 }, 5);
			}
		}
	}
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	PutMissile(i);
}

void MI_Arrow(int i)
{
	Missiles[i]._mirange--;
	Missiles[i]._midist++;
	Missiles[i].position.traveled += Missiles[i].position.velocity;
	UpdateMissilePos(i);
	int p = Missiles[i]._misource;

	int mind;
	int maxd;
	if (p != -1) {
		if (Missiles[i]._micaster == TARGET_MONSTERS) {
			mind = Players[p]._pIMinDam;
			maxd = Players[p]._pIMaxDam;
		} else {
			mind = Monsters[p].mMinDamage;
			maxd = Monsters[p].mMaxDamage;
		}
	} else {
		mind = currlevel;
		maxd = 2 * currlevel;
	}
	if (Missiles[i].position.tile != Missiles[i].position.start)
		CheckMissileCol(i, mind, maxd, false, Missiles[i].position.tile, false);
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Firebolt(int i)
{
	int d;

	Missiles[i]._mirange--;
	if (Missiles[i]._mitype != MIS_BONESPIRIT || Missiles[i]._mimfnum != 8) {
		int omx = Missiles[i].position.traveled.deltaX;
		int omy = Missiles[i].position.traveled.deltaY;
		Missiles[i].position.traveled += Missiles[i].position.velocity;
		UpdateMissilePos(i);
		int p = Missiles[i]._misource;
		if (p != -1) {
			if (Missiles[i]._micaster == TARGET_MONSTERS) {
				switch (Missiles[i]._mitype) {
				case MIS_FIREBOLT:
					d = vanilla::GenerateRnd(10) + (Players[p]._pMagic / 8) + Missiles[i]._mispllvl + 1;
					break;
				case MIS_FLARE:
					d = 3 * Missiles[i]._mispllvl - (Players[p]._pMagic / 8) + (Players[p]._pMagic / 2);
					break;
				case MIS_BONESPIRIT:
					d = 0;
					break;
				}
			} else {
				auto &monster = Monsters[p];
				d = monster.mMinDamage + vanilla::GenerateRnd(monster.mMaxDamage - monster.mMinDamage + 1);
			}
		} else {
			d = currlevel + vanilla::GenerateRnd(2 * currlevel);
		}
		if (Missiles[i].position.tile != Missiles[i].position.start) {
			CheckMissileCol(i, d, d, false, Missiles[i].position.tile, false);
		}
		if (Missiles[i]._mirange == 0) {
			Missiles[i]._miDelFlag = true;
			Missiles[i].position.traveled = { omx, omy };
			UpdateMissilePos(i);
			Missiles[i].position.StopMissile();
			switch (Missiles[i]._mitype) {
			case MIS_FIREBOLT:
			case MIS_MAGMABALL:
				AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_MISEXP, Missiles[i]._micaster, Missiles[i]._misource, 0, 0);
				break;
			case MIS_FLARE:
				AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_MISEXP2, Missiles[i]._micaster, Missiles[i]._misource, 0, 0);
				break;
			case MIS_ACID:
				AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_MISEXP3, Missiles[i]._micaster, Missiles[i]._misource, 0, 0);
				break;
			case MIS_BONESPIRIT:
				SetMissDir(i, DIR_OMNI);
				Missiles[i]._mirange = 7;
				Missiles[i]._miDelFlag = false;
				PutMissile(i);
				return;
			case MIS_LICH:
				AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_EXORA1, Missiles[i]._micaster, Missiles[i]._misource, 0, 0);
				break;
			case MIS_PSYCHORB:
				AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_EXBL2, Missiles[i]._micaster, Missiles[i]._misource, 0, 0);
				break;
			case MIS_NECROMORB:
				AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_EXRED3, Missiles[i]._micaster, Missiles[i]._misource, 0, 0);
				break;
			case MIS_ARCHLICH:
				AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_EXYEL2, Missiles[i]._micaster, Missiles[i]._misource, 0, 0);
				break;
			case MIS_BONEDEMON:
				AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_EXBL3, Missiles[i]._micaster, Missiles[i]._misource, 0, 0);
				break;
			}
			if (Missiles[i]._mlid != NO_LIGHT)
				AddUnLight(Missiles[i]._mlid);
			PutMissile(i);
		} else {
			if (Missiles[i].position.tile.x != Missiles[i]._miVar1 || Missiles[i].position.tile.y != Missiles[i]._miVar2) {
				Missiles[i]._miVar1 = Missiles[i].position.tile.x;
				Missiles[i]._miVar2 = Missiles[i].position.tile.y;
				if (Missiles[i]._mlid != NO_LIGHT)
					ChangeLight(Missiles[i]._mlid, { Missiles[i]._miVar1, Missiles[i]._miVar2 }, 8);
			}
			PutMissile(i);
		}
	} else if (Missiles[i]._mirange == 0) {
		if (Missiles[i]._mlid != NO_LIGHT)
			AddUnLight(Missiles[i]._mlid);
		Missiles[i]._miDelFlag = true;
		PlaySfxLoc(LS_BSIMPCT, Missiles[i].position.tile);
		PutMissile(i);
	} else
		PutMissile(i);
}

void MI_Lightball(int i)
{
	int tx = Missiles[i]._miVar1;
	int ty = Missiles[i]._miVar2;
	Missiles[i]._mirange--;
	Missiles[i].position.traveled += Missiles[i].position.velocity;
	UpdateMissilePos(i);
	int j = Missiles[i]._mirange;
	CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, false, Missiles[i].position.tile, false);
	if (Missiles[i]._miHitFlag)
		Missiles[i]._mirange = j;
	int8_t obj = dObject[tx][ty];
	if (obj != 0 && tx == Missiles[i].position.tile.x && ty == Missiles[i].position.tile.y) {
		int oi = (obj > 0) ? (obj - 1) : -(obj + 1);
		if (Objects[oi]._otype == OBJ_SHRINEL || Objects[oi]._otype == OBJ_SHRINER)
			Missiles[i]._mirange = j;
	}
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Krull(int i)
{
	Missiles[i]._mirange--;
	Missiles[i].position.traveled += Missiles[i].position.velocity;
	UpdateMissilePos(i);
	CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, false, Missiles[i].position.tile, false);
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Acidpud(int i)
{
	Missiles[i]._mirange--;
	int range = Missiles[i]._mirange;
	CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, true, Missiles[i].position.tile, false);
	Missiles[i]._mirange = range;
	if (range == 0) {
		if (Missiles[i]._mimfnum != 0) {
			Missiles[i]._miDelFlag = true;
		} else {
			SetMissDir(i, 1);
			Missiles[i]._mirange = Missiles[i]._miAnimLen;
		}
	}
	PutMissile(i);
}

void MI_Firewall(int i)
{
	constexpr int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == Missiles[i]._miVar1) {
		SetMissDir(i, 1);
		Missiles[i]._miAnimFrame = vanilla::GenerateRnd(11) + 1;
	}
	if (Missiles[i]._mirange == Missiles[i]._miAnimLen - 1) {
		SetMissDir(i, 0);
		Missiles[i]._miAnimFrame = 13;
		Missiles[i]._miAnimAdd = -1;
	}
	CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, true, Missiles[i].position.tile, true);
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	if (Missiles[i]._mimfnum != 0 && Missiles[i]._mirange != 0 && Missiles[i]._miAnimAdd != -1 && Missiles[i]._miVar2 < 12) {
		if (Missiles[i]._miVar2 == 0)
			Missiles[i]._mlid = AddLight(Missiles[i].position.tile, ExpLight[0]);
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, ExpLight[Missiles[i]._miVar2]);
		Missiles[i]._miVar2++;
	}
	PutMissile(i);
}

void MI_Fireball(int i)
{
	FireballUpdate(i, Missiles[i].position.velocity, false);
}

void MI_HorkSpawn(int mi)
{
	Missiles[mi]._mirange--;
	CheckMissileCol(mi, 0, 0, false, Missiles[mi].position.tile, false);
	if (Missiles[mi]._mirange <= 0) {
		Missiles[mi]._miDelFlag = true;
		for (int i = 0; i < 2; i++) {
			int k = CrawlNum[i];
			int ck = k + 2;
			for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
				int tx = Missiles[mi].position.tile.x + CrawlTable[ck - 1];
				int ty = Missiles[mi].position.tile.y + CrawlTable[ck];
				if (InDungeonBounds({ tx, ty })) {
					int dp = dPiece[tx][ty];
					if (!nSolidTable[dp] && dMonster[tx][ty] == 0 && dPlayer[tx][ty] == 0 && dObject[tx][ty] == 0) {
						i = 6;
						auto md = static_cast<Direction>(Missiles[mi]._miVar1);
						int monsterId = AddMonster({ tx, ty }, md, 1, true);
						if (monsterId != -1)
							M_StartStand(Monsters[monsterId], md);
						break;
					}
				}
			}
		}
	} else {
		Missiles[mi]._midist++;
		Missiles[mi].position.traveled += Missiles[mi].position.velocity;
		UpdateMissilePos(mi);
	}
	PutMissile(mi);
}

void MI_Rune(int i)
{
	int mx = Missiles[i].position.tile.x;
	int my = Missiles[i].position.tile.y;
	int mid = dMonster[mx][my];
	int8_t pid = dPlayer[mx][my];
	if (mid != 0 || pid != 0) {
		Direction dir;
		if (mid != 0) {
			mid = (mid > 0) ? (mid - 1) : -(mid + 1);
			dir = GetDirection(Missiles[i].position.tile, Monsters[mid].position.tile);
		} else {
			pid = (pid > 0) ? (pid - 1) : -(pid + 1);
			dir = GetDirection(Missiles[i].position.tile, Players[pid].position.tile);
		}
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
		AddMissile({ mx, my }, { mx, my }, dir, Missiles[i]._miVar1, TARGET_BOTH, Missiles[i]._misource, Missiles[i]._midam, Missiles[i]._mispllvl);
	}
	PutMissile(i);
}

void MI_LightningWall(int i)
{
	Missiles[i]._mirange--;
	int range = Missiles[i]._mirange;
	CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, true, Missiles[i].position.tile, false);
	if (Missiles[i]._miHitFlag)
		Missiles[i]._mirange = range;
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_HiveExplode(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange <= 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	PutMissile(i);
}

void MI_Immolation(int i)
{
	if (Missiles[i]._miVar7 < 0) {
		int v = 2 * Missiles[i]._miVar6;
		Missiles[i]._miVar6 = v;
		Missiles[i]._miVar7 = v;
		Missiles[i]._mimfnum = left[Missiles[i]._mimfnum];
	} else {
		Missiles[i]._miVar7--;
	}

	Displacement offset = Missiles[i].position.velocity;

	switch (Missiles[i]._mimfnum) {
	case DIR_S:
	case DIR_N:
		offset.deltaY = 0;
		break;
	case DIR_W:
	case DIR_E:
		offset.deltaX = 0;
		break;
	default:
		break;
	}

	FireballUpdate(i, offset, true);
}

void MI_LightningArrow(int i)
{
	Missiles[i]._mirange--;
	Missiles[i].position.traveled += Missiles[i].position.velocity;
	UpdateMissilePos(i);

	int mx = Missiles[i].position.tile.x;
	int my = Missiles[i].position.tile.y;
	assert((DWORD)mx < MAXDUNX);
	assert((DWORD)my < MAXDUNY);
	int pn = dPiece[mx][my];
	assert((DWORD)pn <= MAXTILES);

	if (Missiles[i]._misource == -1) {
		if ((mx != Missiles[i].position.start.x || my != Missiles[i].position.start.y) && nMissileTable[pn]) {
			Missiles[i]._mirange = 0;
		}
	} else if (nMissileTable[pn]) {
		Missiles[i]._mirange = 0;
	}

	if (!nMissileTable[pn]) {
		if ((mx != Missiles[i]._miVar1 || my != Missiles[i]._miVar2) && mx > 0 && my > 0 && mx < MAXDUNX && my < MAXDUNY) {
			if (Missiles[i]._misource != -1) {
				if (Missiles[i]._micaster == TARGET_PLAYERS
				    && Monsters[Missiles[i]._misource].MType->mtype >= MT_STORM
				    && Monsters[Missiles[i]._misource].MType->mtype <= MT_MAEL) {
					AddMissile(
					    Missiles[i].position.tile,
					    Missiles[i].position.start,
					    i,
					    MIS_LIGHTNING2,
					    Missiles[i]._micaster,
					    Missiles[i]._misource,
					    Missiles[i]._midam,
					    Missiles[i]._mispllvl);
				} else {
					AddMissile(
					    Missiles[i].position.tile,
					    Missiles[i].position.start,
					    i,
					    MIS_LIGHTNING,
					    Missiles[i]._micaster,
					    Missiles[i]._misource,
					    Missiles[i]._midam,
					    Missiles[i]._mispllvl);
				}
			} else {
				AddMissile(
				    Missiles[i].position.tile,
				    Missiles[i].position.start,
				    i,
				    MIS_LIGHTNING,
				    Missiles[i]._micaster,
				    Missiles[i]._misource,
				    Missiles[i]._midam,
				    Missiles[i]._mispllvl);
			}
			Missiles[i]._miVar1 = Missiles[i].position.tile.x;
			Missiles[i]._miVar2 = Missiles[i].position.tile.y;
		}
	}

	if (Missiles[i]._mirange == 0 || mx <= 0 || my <= 0 || mx >= MAXDUNX || my > MAXDUNY) { // BUGFIX my >= MAXDUNY
		Missiles[i]._miDelFlag = true;
	}
}

void MI_Reflect(int i)
{
	int src = Missiles[i]._misource;
	if (src != MyPlayerId && currlevel != Players[src].plrlevel)
		Missiles[i]._miDelFlag = true;
	if (Players[src].wReflections <= 0) {
		Missiles[i]._miDelFlag = true;
		NetSendCmd(true, CMD_REFLECT);
	}
	PutMissile(i);
}

void MI_FireRing(int i)
{
	MissileRing(i, MIS_FIREWALL);
}

void MI_LightningRing(int i)
{
	MissileRing(i, MIS_LIGHTWALL);
}

void MI_Search(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange != 0)
		return;

	Missiles[i]._miDelFlag = true;
	PlaySfxLoc(IS_CAST7, Players[Missiles[i]._miVar1].position.tile);
	AutoMapShowItems = false;
}

void MI_LightningWallC(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		return;
	}

	int id = Missiles[i]._misource;
	int lvl = (id > -1) ? Players[id]._pLevel : 0;
	int dmg = 16 * (GenerateRndSum(10, 2) + lvl + 2);

	{
		Point position = { Missiles[i]._miVar1, Missiles[i]._miVar2 };
		Point target = position + static_cast<Direction>(Missiles[i]._miVar3);

		if (!Missiles[i].limitReached && GrowWall(id, position, target, MIS_LIGHTWALL, Missiles[i]._mispllvl, dmg)) {
			Missiles[i]._miVar1 = target.x;
			Missiles[i]._miVar2 = target.y;
		} else {
			Missiles[i].limitReached = true;
		}
	}

	{
		Point position = { Missiles[i]._miVar5, Missiles[i]._miVar6 };
		Point target = position + static_cast<Direction>(Missiles[i]._miVar4);

		if (Missiles[i]._miVar7 == 0 && GrowWall(id, position, target, MIS_LIGHTWALL, Missiles[i]._mispllvl, dmg)) {
			Missiles[i]._miVar5 = target.x;
			Missiles[i]._miVar6 = target.y;
		} else {
			Missiles[i]._miVar7 = 1;
		}
	}
}

void MI_FireNova(int i)
{
	int sx1 = 0;
	int sy1 = 0;
	int id = Missiles[i]._misource;
	int dam = Missiles[i]._midam;
	Point src = Missiles[i].position.tile;
	Direction dir = DIR_S;
	mienemy_type en = TARGET_PLAYERS;
	if (id != -1) {
		dir = Players[id]._pdir;
		en = TARGET_MONSTERS;
	}
	for (const auto &k : VisionCrawlTable) {
		if (sx1 != k[6] || sy1 != k[7]) {
			Displacement offsets[] = { { k[6], k[7] }, { -k[6], -k[7] }, { -k[6], +k[7] }, { +k[6], -k[7] } };
			for (Displacement offset : offsets)
				AddMissile(src, src + offset, dir, MIS_FIRENOVA, en, id, dam, Missiles[i]._mispllvl);
			sx1 = k[6];
			sy1 = k[7];
		}
	}
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
}

void MI_SpecArrow(int i)
{
	int id = Missiles[i]._misource;
	int dam = Missiles[i]._midam;
	Point src = Missiles[i].position.tile;
	Point dst = { Missiles[i]._miVar1, Missiles[i]._miVar2 };
	int spllvl = Missiles[i]._miVar3;
	int mitype = 0;
	Direction dir = DIR_S;
	mienemy_type micaster = TARGET_PLAYERS;
	if (id != -1) {
		dir = Players[id]._pdir;
		micaster = TARGET_MONSTERS;

		switch (Players[id]._pILMinDam) {
		case 0:
			mitype = MIS_FIRENOVA;
			break;
		case 1:
			mitype = MIS_LIGHTARROW;
			break;
		case 2:
			mitype = MIS_CBOLTARROW;
			break;
		case 3:
			mitype = MIS_HBOLTARROW;
			break;
		}
	}
	AddMissile(src, dst, dir, mitype, micaster, id, dam, spllvl);
	if (mitype == MIS_CBOLTARROW) {
		AddMissile(src, dst, dir, mitype, micaster, id, dam, spllvl);
		AddMissile(src, dst, dir, mitype, micaster, id, dam, spllvl);
	}
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
}

void MI_Lightctrl(int i)
{
	assert((DWORD)i < MAXMISSILES);
	Missiles[i]._mirange--;

	int dam;
	int id = Missiles[i]._misource;
	if (id != -1) {
		if (Missiles[i]._micaster == TARGET_MONSTERS) {
			dam = (vanilla::GenerateRnd(2) + vanilla::GenerateRnd(Players[id]._pLevel) + 2) << 6;
		} else {
			auto &monster = Monsters[id];
			dam = 2 * (monster.mMinDamage + vanilla::GenerateRnd(monster.mMaxDamage - monster.mMinDamage + 1));
		}
	} else {
		dam = vanilla::GenerateRnd(currlevel) + 2 * currlevel;
	}

	Missiles[i].position.traveled += Missiles[i].position.velocity;
	UpdateMissilePos(i);

	int mx = Missiles[i].position.tile.x;
	int my = Missiles[i].position.tile.y;
	assert((DWORD)mx < MAXDUNX);
	assert((DWORD)my < MAXDUNY);
	int pn = dPiece[mx][my];
	assert((DWORD)pn <= MAXTILES);

	if (id != -1 || Point { mx, my } != Missiles[i].position.start) {
		if (nMissileTable[pn]) {
			Missiles[i]._mirange = 0;
		}
	}
	if (!nMissileTable[pn]
	    && Point { mx, my } != Point { Missiles[i]._miVar1, Missiles[i]._miVar2 }
	    && InDungeonBounds({ mx, my })) {
		if (id != -1) {
			if (Missiles[i]._micaster == TARGET_PLAYERS
			    && Monsters[id].MType->mtype >= MT_STORM
			    && Monsters[id].MType->mtype <= MT_MAEL) {
				AddMissile(
				    Missiles[i].position.tile,
				    Missiles[i].position.start,
				    i,
				    MIS_LIGHTNING2,
				    Missiles[i]._micaster,
				    id,
				    dam,
				    Missiles[i]._mispllvl);
			} else {
				AddMissile(
				    Missiles[i].position.tile,
				    Missiles[i].position.start,
				    i,
				    MIS_LIGHTNING,
				    Missiles[i]._micaster,
				    id,
				    dam,
				    Missiles[i]._mispllvl);
			}
		} else {
			AddMissile(
			    Missiles[i].position.tile,
			    Missiles[i].position.start,
			    i,
			    MIS_LIGHTNING,
			    Missiles[i]._micaster,
			    id,
			    dam,
			    Missiles[i]._mispllvl);
		}
		Missiles[i]._miVar1 = Missiles[i].position.tile.x;
		Missiles[i]._miVar2 = Missiles[i].position.tile.y;
	}
	assert(mx != 0 && my != 0);
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
	}
}

void MI_Lightning(int i)
{
	Missiles[i]._mirange--;
	int j = Missiles[i]._mirange;
	if (Missiles[i].position.tile != Missiles[i].position.start)
		CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, true, Missiles[i].position.tile, false);
	if (Missiles[i]._miHitFlag)
		Missiles[i]._mirange = j;
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	PutMissile(i);
}

void MI_Town(int i)
{
	int expLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };

	if (Missiles[i]._mirange > 1)
		Missiles[i]._mirange--;
	if (Missiles[i]._mirange == Missiles[i]._miVar1)
		SetMissDir(i, 1);
	if (currlevel != 0 && Missiles[i]._mimfnum != 1 && Missiles[i]._mirange != 0) {
		if (Missiles[i]._miVar2 == 0)
			Missiles[i]._mlid = AddLight(Missiles[i].position.tile, 1);
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, expLight[Missiles[i]._miVar2]);
		Missiles[i]._miVar2++;
	}

	for (int p = 0; p < MAX_PLRS; p++) {
		auto &player = Players[p];
		if (player.plractive && currlevel == player.plrlevel && !player._pLvlChanging && player._pmode == PM_STAND && player.position.tile == Missiles[i].position.tile) {
			ClrPlrPath(player);
			if (p == MyPlayerId) {
				NetSendCmdParam1(true, CMD_WARP, Missiles[i]._misource);
				player._pmode = PM_NEWLVL;
			}
		}
	}

	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	PutMissile(i);
}

void MI_Flash(int i)
{
	if (Missiles[i]._micaster == TARGET_MONSTERS) {
		if (Missiles[i]._misource != -1)
			Players[Missiles[i]._misource]._pInvincible = true;
	}
	Missiles[i]._mirange--;

	constexpr Displacement Offsets[] = { { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };
	for (Displacement offset : Offsets)
		CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, true, Missiles[i].position.tile + offset, true);

	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		if (Missiles[i]._micaster == TARGET_MONSTERS) {
			if (Missiles[i]._misource != -1)
				Players[Missiles[i]._misource]._pInvincible = false;
		}
	}
	PutMissile(i);
}

void MI_Flash2(int i)
{
	if (Missiles[i]._micaster == TARGET_MONSTERS) {
		if (Missiles[i]._misource != -1)
			Players[Missiles[i]._misource]._pInvincible = true;
	}
	Missiles[i]._mirange--;

	constexpr Displacement Offsets[] = { { -1, -1 }, { 0, -1 }, { 1, -1 } };
	for (Displacement offset : Offsets)
		CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, true, Missiles[i].position.tile + offset, true);

	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		if (Missiles[i]._micaster == TARGET_MONSTERS) {
			if (Missiles[i]._misource != -1)
				Players[Missiles[i]._misource]._pInvincible = false;
		}
	}
	PutMissile(i);
}

void MI_Manashield(int i)
{
	int id = Missiles[i]._misource;
	if (id != MyPlayerId) {
		if (currlevel != Players[id].plrlevel)
			Missiles[i]._miDelFlag = true;
	} else {
		if (Players[id]._pMana <= 0 || !Players[id].plractive)
			Missiles[i]._mirange = 0;

		if (Missiles[i]._mirange == 0) {
			Missiles[i]._miDelFlag = true;
			NetSendCmd(true, CMD_ENDSHIELD);
		}
	}
	PutMissile(i);
}

void MI_Etherealize(int i)
{
	Missiles[i]._mirange--;

	auto &player = Players[Missiles[i]._misource];

	Missiles[i].position.tile = player.position.tile;
	Missiles[i].position.traveled.deltaX = player.position.offset.deltaX << 16;
	Missiles[i].position.traveled.deltaY = player.position.offset.deltaY << 16;
	if (player._pmode == PM_WALK3) {
		Missiles[i].position.start = player.position.future;
	} else {
		Missiles[i].position.start = player.position.tile;
	}
	UpdateMissilePos(i);
	if (player._pmode == PM_WALK3) {
		if (player._pdir == DIR_W)
			Missiles[i].position.tile.x++;
		else
			Missiles[i].position.tile.y++;
	}
	player._pSpellFlags |= 1;
	if (Missiles[i]._mirange == 0 || player._pHitPoints <= 0) {
		Missiles[i]._miDelFlag = true;
		player._pSpellFlags &= ~0x1;
	}
	PutMissile(i);
}

void MI_Firemove(int i)
{
	constexpr int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	Missiles[i].position.tile.x--;
	Missiles[i].position.tile.y--;
	Missiles[i].position.offset.deltaY += 32;
	Missiles[i]._miVar1++;
	if (Missiles[i]._miVar1 == Missiles[i]._miAnimLen) {
		SetMissDir(i, 1);
		Missiles[i]._miAnimFrame = vanilla::GenerateRnd(11) + 1;
	}
	Missiles[i].position.traveled += Missiles[i].position.velocity;
	UpdateMissilePos(i);
	int j = Missiles[i]._mirange;
	CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, false, Missiles[i].position.tile, false);
	if (Missiles[i]._miHitFlag)
		Missiles[i]._mirange = j;
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	if (Missiles[i]._mimfnum != 0 || Missiles[i]._mirange == 0) {
		if (Missiles[i].position.tile.x != Missiles[i]._miVar3 || Missiles[i].position.tile.y != Missiles[i]._miVar4) {
			Missiles[i]._miVar3 = Missiles[i].position.tile.x;
			Missiles[i]._miVar4 = Missiles[i].position.tile.y;
			ChangeLight(Missiles[i]._mlid, { Missiles[i]._miVar3, Missiles[i]._miVar4 }, 8);
		}
	} else {
		if (Missiles[i]._miVar2 == 0)
			Missiles[i]._mlid = AddLight(Missiles[i].position.tile, ExpLight[0]);
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, ExpLight[Missiles[i]._miVar2]);
		Missiles[i]._miVar2++;
	}
	Missiles[i].position.tile.x++;
	Missiles[i].position.tile.y++;
	Missiles[i].position.offset.deltaY -= 32;
	PutMissile(i);
}

void MI_Guardian(int i)
{
	assert((DWORD)i < MAXMISSILES);

	Missiles[i]._mirange--;

	if (Missiles[i]._miVar2 > 0) {
		Missiles[i]._miVar2--;
	}
	if (Missiles[i]._mirange == Missiles[i]._miVar1 || (Missiles[i]._mimfnum == MFILE_GUARD && Missiles[i]._miVar2 == 0)) {
		SetMissDir(i, 1);
	}

	Point position = Missiles[i].position.tile;

	if ((Missiles[i]._mirange % 16) == 0) {
		Displacement previous = { 0, 0 };

		bool found = false;
		for (int j = 0; j < 23 && !found; j++) {
			for (int k = 10; k >= 0 && !found; k -= 2) {
				const Displacement offset { VisionCrawlTable[j][k], VisionCrawlTable[j][k + 1] };
				if (offset == Displacement { 0, 0 }) {
					break;
				}
				if (previous == offset) {
					continue;
				}
				found = GuardianTryFireAt(i, { position.x + offset.deltaX, position.y + offset.deltaY })
				    || GuardianTryFireAt(i, { position.x - offset.deltaX, position.y - offset.deltaY })
				    || GuardianTryFireAt(i, { position.x + offset.deltaX, position.y - offset.deltaY })
				    || GuardianTryFireAt(i, { position.x - offset.deltaX, position.y + offset.deltaY });
				if (!found) {
					previous = offset;
				}
			}
		}
	}

	if (Missiles[i]._mirange == 14) {
		SetMissDir(i, 0);
		Missiles[i]._miAnimFrame = 15;
		Missiles[i]._miAnimAdd = -1;
	}

	Missiles[i]._miVar3 += Missiles[i]._miAnimAdd;

	if (Missiles[i]._miVar3 > 15) {
		Missiles[i]._miVar3 = 15;
	} else if (Missiles[i]._miVar3 > 0) {
		ChangeLight(Missiles[i]._mlid, position, Missiles[i]._miVar3);
	}

	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}

	PutMissile(i);
}

void MI_Chain(int mi)
{
	int id = Missiles[mi]._misource;
	Point position = Missiles[mi].position.tile;
	Direction dir = GetDirection(position, { Missiles[mi]._miVar1, Missiles[mi]._miVar2 });
	AddMissile(position, { Missiles[mi]._miVar1, Missiles[mi]._miVar2 }, dir, MIS_LIGHTCTRL, TARGET_MONSTERS, id, 1, Missiles[mi]._mispllvl);
	int rad = Missiles[mi]._mispllvl + 3;
	if (rad > 19)
		rad = 19;
	for (int i = 1; i < rad; i++) {
		int k = CrawlNum[i];
		int ck = k + 2;
		for (auto j = static_cast<uint8_t>(CrawlTable[k]); j > 0; j--, ck += 2) {
			Point target = position + Displacement { CrawlTable[ck - 1], CrawlTable[ck] };
			if (InDungeonBounds(target) && dMonster[target.x][target.y] > 0) {
				dir = GetDirection(position, target);
				AddMissile(position, target, dir, MIS_LIGHTCTRL, TARGET_MONSTERS, id, 1, Missiles[mi]._mispllvl);
			}
		}
	}
	Missiles[mi]._mirange--;
	if (Missiles[mi]._mirange == 0)
		Missiles[mi]._miDelFlag = true;
}

void MI_Blood(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
	if (Missiles[i]._miAnimFrame == Missiles[i]._miAnimLen)
		Missiles[i]._miPreFlag = true;
	PutMissile(i);
}

void MI_Weapexp(int i)
{
	constexpr int ExpLight[10] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2 };

	Missiles[i]._mirange--;
	int id = Missiles[i]._misource;
	int mind;
	int maxd;
	if (Missiles[i]._miVar2 == 1) {
		mind = Players[id]._pIFMinDam;
		maxd = Players[id]._pIFMaxDam;
		MissileData[Missiles[i]._mitype].mResist = MISR_FIRE;
	} else {
		mind = Players[id]._pILMinDam;
		maxd = Players[id]._pILMaxDam;
		MissileData[Missiles[i]._mitype].mResist = MISR_LIGHTNING;
	}
	CheckMissileCol(i, mind, maxd, false, Missiles[i].position.tile, false);
	if (Missiles[i]._miVar1 == 0) {
		Missiles[i]._mlid = AddLight(Missiles[i].position.tile, 9);
	} else {
		if (Missiles[i]._mirange != 0)
			ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, ExpLight[Missiles[i]._miVar1]);
	}
	Missiles[i]._miVar1++;
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	} else {
		PutMissile(i);
	}
}

void MI_Misexp(int i)
{
	constexpr int ExpLight[] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2, 1, 0, 0, 0, 0 };

	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	} else {
		if (Missiles[i]._miVar1 == 0)
			Missiles[i]._mlid = AddLight(Missiles[i].position.tile, 9);
		else
			ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, ExpLight[Missiles[i]._miVar1]);
		Missiles[i]._miVar1++;
		PutMissile(i);
	}
}

void MI_Acidsplat(int i)
{
	if (Missiles[i]._mirange == Missiles[i]._miAnimLen) {
		Missiles[i].position.tile.x++;
		Missiles[i].position.tile.y++;
		Missiles[i].position.offset.deltaY -= 32;
	}
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		int monst = Missiles[i]._misource;
		int dam = (Monsters[monst].MData->mLevel >= 2 ? 2 : 1);
		AddMissile(Missiles[i].position.tile, { i, 0 }, Missiles[i]._mimfnum, MIS_ACIDPUD, TARGET_PLAYERS, monst, dam, Missiles[i]._mispllvl);
	} else {
		PutMissile(i);
	}
}

void MI_Teleport(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange <= 0) {
		Missiles[i]._miDelFlag = true;
		return;
	}

	int id = Missiles[i]._misource;
	auto &player = Players[id];

	dPlayer[player.position.tile.x][player.position.tile.y] = 0;
	PlrClrTrans(player.position.tile);
	player.position.tile = { Missiles[i].position.tile.x, Missiles[i].position.tile.y };
	player.position.future = player.position.tile;
	player.position.old = player.position.tile;
	PlrDoTrans(player.position.tile);
	Missiles[i]._miVar1 = 1;
	dPlayer[player.position.tile.x][player.position.tile.y] = id + 1;
	if (leveltype != DTYPE_TOWN) {
		ChangeLightXY(player._plid, player.position.tile);
		ChangeVisionXY(player._pvid, player.position.tile);
	}
	if (id == MyPlayerId) {
		ViewX = player.position.tile.x - ScrollInfo.tile.x;
		ViewY = player.position.tile.y - ScrollInfo.tile.y;
	}
}

void MI_Stone(int i)
{
	Missiles[i]._mirange--;
	auto &monster = Monsters[Missiles[i]._miVar2];
	if (monster._mhitpoints == 0 && Missiles[i]._miAnimType != MFILE_SHATTER1) {
		Missiles[i]._mimfnum = 0;
		Missiles[i]._miDrawFlag = true;
		SetMissAnim(i, MFILE_SHATTER1);
		Missiles[i]._mirange = 11;
	}
	if (monster._mmode != MM_STONE) {
		Missiles[i]._miDelFlag = true;
		return;
	}

	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		if (monster._mhitpoints > 0) {
			monster._mmode = (MON_MODE)Missiles[i]._miVar1;
			monster.AnimInfo.IsPetrified = false;
		} else {
			AddDead(monster.position.tile, stonendx, monster._mdir);
		}
	}
	if (Missiles[i]._miAnimType == MFILE_SHATTER1)
		PutMissile(i);
}

void MI_Boom(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._miVar1 == 0)
		CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, false, Missiles[i].position.tile, true);
	if (Missiles[i]._miHitFlag)
		Missiles[i]._miVar1 = 1;
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Rhino(int i)
{
	int monst = Missiles[i]._misource;
	auto &monster = Monsters[monst];
	if (monster._mmode != MM_CHARGE) {
		Missiles[i]._miDelFlag = true;
		return;
	}
	UpdateMissilePos(i);
	Point prevPos = Missiles[i].position.tile;
	Point newPosSnake;
	dMonster[prevPos.x][prevPos.y] = 0;
	if (monster._mAi == AI_SNAKE) {
		Missiles[i].position.traveled += Missiles[i].position.velocity * 2;
		UpdateMissilePos(i);
		newPosSnake = Missiles[i].position.tile;
		Missiles[i].position.traveled -= Missiles[i].position.velocity;
	} else {
		Missiles[i].position.traveled += Missiles[i].position.velocity;
	}
	UpdateMissilePos(i);
	Point newPos = Missiles[i].position.tile;
	if (!MonsterIsTileAvalible(monst, newPos) || (monster._mAi == AI_SNAKE && !MonsterIsTileAvalible(monst, newPosSnake))) {
		MissToMonst(i, prevPos);
		Missiles[i]._miDelFlag = true;
		return;
	}
	monster.position.future = newPos;
	monster.position.old = newPos;
	monster.position.tile = newPos;
	dMonster[newPos.x][newPos.y] = -(monst + 1);
	if (monster._uniqtype != 0)
		ChangeLightXY(Missiles[i]._mlid, newPos);
	MoveMissilePos(i);
	PutMissile(i);
}

void MI_Fireman(int i)
{
	UpdateMissilePos(i);
	Point a = Missiles[i].position.tile;
	Missiles[i].position.traveled += Missiles[i].position.velocity;
	UpdateMissilePos(i);
	auto &monster = Monsters[Missiles[i]._misource];
	Point b = Missiles[i].position.tile;
	int enemy = monster._menemy;
	Point c = (monster._mFlags & MFLAG_TARGETS_MONSTER) == 0 ? Players[enemy].position.tile : Monsters[enemy].position.tile;

	int j = 0;
	if (b != a && (((Missiles[i]._miVar1 & 1) != 0 && a.WalkingDistance(c) >= 4) || Missiles[i]._miVar2 > 1) && MonsterIsTileAvalible(Missiles[i]._misource, a)) {
		MissToMonst(i, a);
		Missiles[i]._miDelFlag = true;
	} else if ((monster._mFlags & MFLAG_TARGETS_MONSTER) == 0) {
		j = dPlayer[b.x][b.y];
	} else {
		j = dMonster[b.x][b.y];
	}
	if (!PosOkMissile(0, b) || (j > 0 && (Missiles[i]._miVar1 & 1) == 0)) {
		Missiles[i].position.velocity *= -1;
		Missiles[i]._mimfnum = opposite[Missiles[i]._mimfnum];
		Missiles[i]._miAnimData = monster.MType->GetAnimData(MonsterGraphic::Walk).CelSpritesForDirections[Missiles[i]._mimfnum]->Data();
		Missiles[i]._miVar2++;
		if (j > 0)
			Missiles[i]._miVar1 |= 1;
	}
	MoveMissilePos(i);
	PutMissile(i);
}

void MI_FirewallC(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		return;
	}

	int id = Missiles[i]._misource;

	{
		Point position = { Missiles[i]._miVar1, Missiles[i]._miVar2 };
		Point target = position + static_cast<Direction>(Missiles[i]._miVar3);

		if (!Missiles[i].limitReached && GrowWall(id, position, target, MIS_FIREWALL, Missiles[i]._mispllvl, 0)) {
			Missiles[i]._miVar1 = target.x;
			Missiles[i]._miVar2 = target.y;
		} else {
			Missiles[i].limitReached = true;
		}
	}

	{
		Point position = { Missiles[i]._miVar5, Missiles[i]._miVar6 };
		Point target = position + static_cast<Direction>(Missiles[i]._miVar4);

		if (Missiles[i]._miVar7 == 0 && GrowWall(id, position, target, MIS_FIREWALL, Missiles[i]._mispllvl, 0)) {
			Missiles[i]._miVar5 = target.x;
			Missiles[i]._miVar6 = target.y;
		} else {
			Missiles[i]._miVar7 = 1;
		}
	}
}

void MI_Infra(int i)
{
	Missiles[i]._mirange--;
	Players[Missiles[i]._misource]._pInfraFlag = true;
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		CalcPlrItemVals(Missiles[i]._misource, true);
	}
}

void MI_Apoca(int i)
{
	int id = Missiles[i]._misource;
	bool exit = false;
	int j;
	int k;
	for (j = Missiles[i]._miVar2; j < Missiles[i]._miVar3 && !exit; j++) {
		for (k = Missiles[i]._miVar4; k < Missiles[i]._miVar5 && !exit; k++) {
			if (dMonster[k][j] < MAX_PLRS)
				continue;
			if (nSolidTable[dPiece[k][j]])
				continue;
			if (gbIsHellfire && !LineClearMissile(Missiles[i].position.tile, { k, j }))
				continue;
			AddMissile({ k, j }, { k, j }, Players[id]._pdir, MIS_BOOM, TARGET_MONSTERS, id, Missiles[i]._midam, 0);
			exit = true;
		}
		if (!exit) {
			Missiles[i]._miVar4 = Missiles[i]._miVar6;
		}
	}

	if (exit) {
		Missiles[i]._miVar2 = j - 1;
		Missiles[i]._miVar4 = k;
	} else {
		Missiles[i]._miDelFlag = true;
	}
}

void MI_Wave(int i)
{
	bool f1 = false;
	bool f2 = false;
	assert((DWORD)i < MAXMISSILES);

	int id = Missiles[i]._misource;
	Point src = Missiles[i].position.tile;
	Direction sd = GetDirection(src, { Missiles[i]._miVar1, Missiles[i]._miVar2 });
	Direction dira = left[left[sd]];
	Direction dirb = right[right[sd]];
	Point na = src + sd;
	int pn = dPiece[na.x][na.y];
	assert((DWORD)pn <= MAXTILES);
	if (!nMissileTable[pn]) {
		Direction pdir = Players[id]._pdir;
		AddMissile(na, na + sd, pdir, MIS_FIREMOVE, TARGET_MONSTERS, id, 0, Missiles[i]._mispllvl);
		na += dira;
		Point nb = src + sd + dirb;
		for (int j = 0; j < (Missiles[i]._mispllvl / 2) + 2; j++) {
			pn = dPiece[na.x][na.y]; // BUGFIX: dPiece is accessed before check against dungeon size and 0
			assert((DWORD)pn <= MAXTILES);
			if (nMissileTable[pn] || f1 || !InDungeonBounds(na)) {
				f1 = true;
			} else {
				AddMissile(na, na + sd, pdir, MIS_FIREMOVE, TARGET_MONSTERS, id, 0, Missiles[i]._mispllvl);
				na += dira;
			}
			pn = dPiece[nb.x][nb.y]; // BUGFIX: dPiece is accessed before check against dungeon size and 0
			assert((DWORD)pn <= MAXTILES);
			if (nMissileTable[pn] || f2 || !InDungeonBounds(nb)) {
				f2 = true;
			} else {
				AddMissile(nb, nb + sd, pdir, MIS_FIREMOVE, TARGET_MONSTERS, id, 0, Missiles[i]._mispllvl);
				nb += dirb;
			}
		}
	}

	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
}

void MI_Nova(int i)
{
	int sx1 = 0;
	int sy1 = 0;
	int id = Missiles[i]._misource;
	int dam = Missiles[i]._midam;
	Point src = Missiles[i].position.tile;
	Direction dir = DIR_S;
	mienemy_type en = TARGET_PLAYERS;
	if (id != -1) {
		dir = Players[id]._pdir;
		en = TARGET_MONSTERS;
	}
	for (const auto &k : VisionCrawlTable) {
		if (sx1 != k[6] || sy1 != k[7]) {
			AddMissile(src, src + Displacement { k[6], k[7] }, dir, MIS_LIGHTBALL, en, id, dam, Missiles[i]._mispllvl);
			AddMissile(src, src + Displacement { -k[6], -k[7] }, dir, MIS_LIGHTBALL, en, id, dam, Missiles[i]._mispllvl);
			AddMissile(src, src + Displacement { -k[6], k[7] }, dir, MIS_LIGHTBALL, en, id, dam, Missiles[i]._mispllvl);
			AddMissile(src, src + Displacement { k[6], -k[7] }, dir, MIS_LIGHTBALL, en, id, dam, Missiles[i]._mispllvl);
			sx1 = k[6];
			sy1 = k[7];
		}
	}
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
}

void MI_Blodboil(int i)
{
	Missiles[i]._mirange--;

	if (Missiles[i]._mirange != 0) {
		return;
	}

	int id = Missiles[i]._miVar1;
	auto &player = Players[id];

	int hpdif = player._pMaxHP - player._pHitPoints;

	if ((player._pSpellFlags & 2) != 0) {
		player._pSpellFlags &= ~0x2;
		player._pSpellFlags |= 4;
		int lvl = player._pLevel * 2;
		Missiles[i]._mirange = lvl + 10 * Missiles[i]._mispllvl + 245;
	} else {
		player._pSpellFlags &= ~0x4;
		Missiles[i]._miDelFlag = true;
		hpdif += Missiles[i]._miVar2;
	}

	CalcPlrItemVals(id, true);
	ApplyPlrDamage(id, 0, 1, hpdif);
	force_redraw = 255;
	player.Say(HeroSpeech::HeavyBreathing);
}

void MI_Flame(int i)
{
	Missiles[i]._mirange--;
	Missiles[i]._miVar2--;
	int k = Missiles[i]._mirange;
	CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, true, Missiles[i].position.tile, false);
	if (Missiles[i]._mirange == 0 && Missiles[i]._miHitFlag)
		Missiles[i]._mirange = k;
	if (Missiles[i]._miVar2 == 0)
		Missiles[i]._miAnimFrame = 20;
	if (Missiles[i]._miVar2 <= 0) {
		k = Missiles[i]._miAnimFrame;
		if (k > 11)
			k = 24 - k;
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, k);
	}
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	if (Missiles[i]._miVar2 <= 0)
		PutMissile(i);
}

void MI_Flamec(int i)
{
	Missiles[i]._mirange--;
	int src = Missiles[i]._misource;
	Missiles[i].position.traveled += Missiles[i].position.velocity;
	UpdateMissilePos(i);
	if (Missiles[i].position.tile.x != Missiles[i]._miVar1 || Missiles[i].position.tile.y != Missiles[i]._miVar2) {
		int id = dPiece[Missiles[i].position.tile.x][Missiles[i].position.tile.y];
		if (!nMissileTable[id]) {
			AddMissile(
			    Missiles[i].position.tile,
			    Missiles[i].position.start,
			    i,
			    MIS_FLAME,
			    Missiles[i]._micaster,
			    src,
			    Missiles[i]._miVar3,
			    Missiles[i]._mispllvl);
		} else {
			Missiles[i]._mirange = 0;
		}
		Missiles[i]._miVar1 = Missiles[i].position.tile.x;
		Missiles[i]._miVar2 = Missiles[i].position.tile.y;
		Missiles[i]._miVar3++;
	}
	if (Missiles[i]._mirange == 0 || Missiles[i]._miVar3 == 3)
		Missiles[i]._miDelFlag = true;
}

void MI_Cbolt(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._miAnimType != MFILE_LGHNING) {
		if (Missiles[i]._miVar3 == 0) {
			constexpr int BPath[16] = { -1, 0, 1, -1, 0, 1, -1, -1, 0, 0, 1, 1, 0, 1, -1, 0 };

			auto md = static_cast<Direction>(Missiles[i]._miVar2);
			switch (BPath[Missiles[i]._mirnd]) {
			case -1:
				md = left[md];
				break;
			case 1:
				md = right[md];
				break;
			}

			Missiles[i]._mirnd = (Missiles[i]._mirnd + 1) & 0xF;
			UpdateMissileVelocity(i, Missiles[i].position.tile, Missiles[i].position.tile + md, 8);
			Missiles[i]._miVar3 = 16;
		} else {
			Missiles[i]._miVar3--;
		}
		Missiles[i].position.traveled += Missiles[i].position.velocity;
		UpdateMissilePos(i);
		CheckMissileCol(i, Missiles[i]._midam, Missiles[i]._midam, false, Missiles[i].position.tile, false);
		if (Missiles[i]._miHitFlag) {
			Missiles[i]._miVar1 = 8;
			Missiles[i]._mimfnum = 0;
			Missiles[i].position.offset = { 0, 0 };
			Missiles[i].position.velocity = {};
			SetMissAnim(i, MFILE_LGHNING);
			Missiles[i]._mirange = Missiles[i]._miAnimLen;
			UpdateMissilePos(i);
		}
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, Missiles[i]._miVar1);
	}
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	PutMissile(i);
}

void MI_Hbolt(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._miAnimType != MFILE_HOLYEXPL) {
		Missiles[i].position.traveled += Missiles[i].position.velocity;
		UpdateMissilePos(i);
		int dam = Missiles[i]._midam;
		if (Missiles[i].position.tile != Missiles[i].position.start) {
			CheckMissileCol(i, dam, dam, false, Missiles[i].position.tile, false);
		}
		if (Missiles[i]._mirange == 0) {
			Missiles[i].position.traveled -= Missiles[i].position.velocity;
			UpdateMissilePos(i);
			Missiles[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_HOLYEXPL);
			Missiles[i]._mirange = Missiles[i]._miAnimLen - 1;
			Missiles[i].position.StopMissile();
		} else {
			if (Missiles[i].position.tile != Point { Missiles[i]._miVar1, Missiles[i]._miVar2 }) {
				Missiles[i]._miVar1 = Missiles[i].position.tile.x;
				Missiles[i]._miVar2 = Missiles[i].position.tile.y;
				ChangeLight(Missiles[i]._mlid, { Missiles[i]._miVar1, Missiles[i]._miVar2 }, 8);
			}
		}
	} else {
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, Missiles[i]._miAnimFrame + 7);
		if (Missiles[i]._mirange == 0) {
			Missiles[i]._miDelFlag = true;
			AddUnLight(Missiles[i]._mlid);
		}
	}
	PutMissile(i);
}

void MI_Element(int i)
{
	Missiles[i]._mirange--;
	int dam = Missiles[i]._midam;
	int id = Missiles[i]._misource;
	if (Missiles[i]._miAnimType == MFILE_BIGEXP) {
		Point c = Missiles[i].position.tile;
		Point p = Players[id].position.tile;
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, Missiles[i]._miAnimFrame);
		if (!CheckBlock(p, c))
			CheckMissileCol(i, dam, dam, true, c, true);

		constexpr Displacement Offsets[] = { { 0, 1 }, { 0, -1 }, { 1, 0 }, { 1, -1 }, { 1, 1 }, { -1, 0 }, { -1, 1 }, { -1, -1 } };
		for (Displacement offset : Offsets) {
			if (!CheckBlock(p, c + offset))
				CheckMissileCol(i, dam, dam, true, c + offset, true);
		}

		if (Missiles[i]._mirange == 0) {
			Missiles[i]._miDelFlag = true;
			AddUnLight(Missiles[i]._mlid);
		}
	} else {
		Missiles[i].position.traveled += Missiles[i].position.velocity;
		UpdateMissilePos(i);
		Point c = Missiles[i].position.tile;
		CheckMissileCol(i, dam, dam, false, c, false);
		if (Missiles[i]._miVar3 == 0 && c == Point { Missiles[i]._miVar4, Missiles[i]._miVar5 })
			Missiles[i]._miVar3 = 1;
		if (Missiles[i]._miVar3 == 1) {
			Missiles[i]._miVar3 = 2;
			Missiles[i]._mirange = 255;
			auto *monster = FindClosest(c, 19);
			if (monster != nullptr) {
				Direction sd = GetDirection(c, monster->position.tile);
				SetMissDir(i, sd);
				UpdateMissileVelocity(i, c, monster->position.tile, 16);
			} else {
				Direction sd = Players[id]._pdir;
				SetMissDir(i, sd);
				UpdateMissileVelocity(i, c, c + sd, 16);
			}
		}
		if (c != Point { Missiles[i]._miVar1, Missiles[i]._miVar2 }) {
			Missiles[i]._miVar1 = c.x;
			Missiles[i]._miVar2 = c.y;
			ChangeLight(Missiles[i]._mlid, c, 8);
		}
		if (Missiles[i]._mirange == 0) {
			Missiles[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			Missiles[i]._mirange = Missiles[i]._miAnimLen - 1;
			Missiles[i].position.StopMissile();
		}
	}
	PutMissile(i);
}

void MI_Bonespirit(int i)
{
	Missiles[i]._mirange--;
	int dam = Missiles[i]._midam;
	int id = Missiles[i]._misource;
	if (Missiles[i]._mimfnum == DIR_OMNI) {
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, Missiles[i]._miAnimFrame);
		if (Missiles[i]._mirange == 0) {
			Missiles[i]._miDelFlag = true;
			AddUnLight(Missiles[i]._mlid);
		}
		PutMissile(i);
	} else {
		Missiles[i].position.traveled += Missiles[i].position.velocity;
		UpdateMissilePos(i);
		Point c = Missiles[i].position.tile;
		CheckMissileCol(i, dam, dam, false, c, false);
		if (Missiles[i]._miVar3 == 0 && c == Point { Missiles[i]._miVar4, Missiles[i]._miVar5 })
			Missiles[i]._miVar3 = 1;
		if (Missiles[i]._miVar3 == 1) {
			Missiles[i]._miVar3 = 2;
			Missiles[i]._mirange = 255;
			auto *monster = FindClosest(c, 19);
			if (monster != nullptr) {
				Missiles[i]._midam = monster->_mhitpoints >> 7;
				SetMissDir(i, GetDirection(c, monster->position.tile));
				UpdateMissileVelocity(i, c, monster->position.tile, 16);
			} else {
				Direction sd = Players[id]._pdir;
				SetMissDir(i, sd);
				UpdateMissileVelocity(i, c, c + sd, 16);
			}
		}
		if (c != Point { Missiles[i]._miVar1, Missiles[i]._miVar2 }) {
			Missiles[i]._miVar1 = c.x;
			Missiles[i]._miVar2 = c.y;
			ChangeLight(Missiles[i]._mlid, c, 8);
		}
		if (Missiles[i]._mirange == 0) {
			SetMissDir(i, DIR_OMNI);
			Missiles[i].position.velocity = {};
			Missiles[i]._mirange = 7;
		}
		PutMissile(i);
	}
}

void MI_ResurrectBeam(int i)
{
	Missiles[i]._mirange--;
	if (Missiles[i]._mirange == 0)
		Missiles[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Rportal(int i)
{
	int expLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };

	if (Missiles[i]._mirange > 1)
		Missiles[i]._mirange--;
	if (Missiles[i]._mirange == Missiles[i]._miVar1)
		SetMissDir(i, 1);

	if (currlevel != 0 && Missiles[i]._mimfnum != 1 && Missiles[i]._mirange != 0) {
		if (Missiles[i]._miVar2 == 0)
			Missiles[i]._mlid = AddLight(Missiles[i].position.tile, 1);
		ChangeLight(Missiles[i]._mlid, Missiles[i].position.tile, expLight[Missiles[i]._miVar2]);
		Missiles[i]._miVar2++;
	}
	if (Missiles[i]._mirange == 0) {
		Missiles[i]._miDelFlag = true;
		AddUnLight(Missiles[i]._mlid);
	}
	PutMissile(i);
}

static void DeleteMissiles()
{
	for (int i = 0; i < ActiveMissileCount;) {
		if (Missiles[ActiveMissiles[i]]._miDelFlag) {
			DeleteMissile(ActiveMissiles[i], i);
		} else {
			i++;
		}
	}
}

void ProcessMissiles()
{
	for (int i = 0; i < ActiveMissileCount; i++) {
		auto &missile = Missiles[ActiveMissiles[i]];
		const auto &position = missile.position.tile;
		dFlags[position.x][position.y] &= ~BFLAG_MISSILE;
		dMissile[position.x][position.y] = 0;
		if (!InDungeonBounds(position))
			missile._miDelFlag = true;
	}

	DeleteMissiles();

	MissilePreFlag = false;

	for (int i = 0; i < ActiveMissileCount; i++) {
		auto &missile = Missiles[ActiveMissiles[i]];
		MissileData[missile._mitype].mProc(ActiveMissiles[i]);
		if ((missile._miAnimFlags & MFLAG_LOCK_ANIMATION) != 0)
			continue;

		missile._miAnimCnt++;
		if (missile._miAnimCnt < missile._miAnimDelay)
			continue;

		missile._miAnimCnt = 0;
		missile._miAnimFrame += missile._miAnimAdd;
		if (missile._miAnimFrame > missile._miAnimLen)
			missile._miAnimFrame = 1;
		else if (missile._miAnimFrame < 1)
			missile._miAnimFrame = missile._miAnimLen;
	}

	DeleteMissiles();
}

void missiles_process_charge()
{
	for (int i = 0; i < ActiveMissileCount; i++) {
		int mi = ActiveMissiles[i];
		MissileStruct *mis = &Missiles[mi];

		mis->_miAnimData = MissileSpriteData[mis->_miAnimType].mAnimData[mis->_mimfnum];
		if (mis->_mitype != MIS_RHINO)
			continue;

		CMonster *mon = Monsters[mis->_misource].MType;

		MonsterGraphic graphic;
		if (mon->mtype >= MT_HORNED && mon->mtype <= MT_OBLORD) {
			graphic = MonsterGraphic::Special;
		} else if (mon->mtype >= MT_NSNAKE && mon->mtype <= MT_GSNAKE) {
			graphic = MonsterGraphic::Attack;
		} else {
			graphic = MonsterGraphic::Walk;
		}
		Missiles[mi]._miAnimData = mon->GetAnimData(graphic).CelSpritesForDirections[mis->_mimfnum]->Data();
	}
}

void ClearMissileSpot(int mi)
{
	dFlags[Missiles[mi].position.tile.x][Missiles[mi].position.tile.y] &= ~BFLAG_MISSILE;
	dMissile[Missiles[mi].position.tile.x][Missiles[mi].position.tile.y] = 0;
}

} // namespace devilution
