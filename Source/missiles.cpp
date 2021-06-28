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
#include "spells.h"
#include "trigs.h"

namespace devilution {

int missileactive[MAXMISSILES];
int missileavail[MAXMISSILES];
MissileStruct missile[MAXMISSILES];
int nummissiles;
ChainStruct chain[MAXMISSILES];
bool MissilePreFlag;
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
		value += GenerateRnd(range);
	}

	return value;
}

void GetDamageAmt(int i, int *mind, int *maxd)
{
	assert(myplr >= 0 && myplr < MAX_PLRS);
	assert(i >= 0 && i < 64);

	auto &myPlayer = plr[myplr];

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

static bool CheckBlock(Point from, Point to)
{
	while (from != to) {
		from += GetDirection(from, to);
		if (nSolidTable[dPiece[from.x][from.y]])
			return true;
	}

	return false;
}

static inline bool InDungeonBounds(Point position)
{
	return position.x > 0 && position.x < MAXDUNX && position.y > 0 && position.y < MAXDUNY;
}

static int FindClosest(Point source, int rad)
{
	if (rad > 19)
		rad = 19;

	for (int i = 1; i < rad; i++) {
		int cr = CrawlNum[i] + 2;
		for (int j = (BYTE)CrawlTable[CrawlNum[i]]; j > 0; j--) {
			int tx = source.x + CrawlTable[cr - 1];
			int ty = source.y + CrawlTable[cr];
			if (InDungeonBounds({ tx, ty })) {
				int mid = dMonster[tx][ty];
				if (mid > 0 && !CheckBlock(source, { tx, ty }))
					return mid - 1;
			}
			cr += 2;
		}
	}
	return -1;
}

int GetSpellLevel(int playerId, spell_id sn)
{
	auto &player = plr[playerId];

	if (playerId != myplr)
		return 1; // BUGFIX spell level will be wrong in multiplayer

	return std::max(player._pISplLvlAdd + player._pSplLvl[sn], 0);
}

constexpr Direction16 Direction16Flip(Direction16 x, Direction16 pivot)
{
	unsigned ret = (2 * pivot + 16 - x) % 16;

	return static_cast<Direction16>(ret);
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
	Point offset = p2 - p1;
	Point absolute = abs(offset);

	bool flipY = offset.x != absolute.x;
	bool flipX = offset.y != absolute.y;

	bool flipMedian = false;
	if (absolute.x > absolute.y) {
		std::swap(absolute.x, absolute.y);
		flipMedian = true;
	}

	Direction16 ret = DIR16_S;
	if (3 * absolute.x <= (absolute.y * 2)) { // mx/my <= 2/3, approximation of tan(33.75)
		if (5 * absolute.x < absolute.y)      // mx/my < 0.2, approximation of tan(11.25)
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
	if (missile[mi]._mitype == MIS_MANASHIELD) {
		int src = missile[mi]._misource;
		if (src == myplr)
			NetSendCmd(true, CMD_REMSHIELD);
		plr[src].pManaShield = false;
	}

	missileavail[MAXMISSILES - nummissiles] = mi;
	nummissiles--;
	if (nummissiles > 0 && i != nummissiles)
		missileactive[i] = missileactive[nummissiles];
}

static void GetMissileVel(int i, Point source, Point destination, int v)
{
	missile[i].position.velocity = { 0, 0 };

	if (source == destination)
		return;

	double dxp = (destination.x + source.y - source.x - destination.y) * (1 << 21);
	double dyp = (destination.y + destination.x - source.x - source.y) * (1 << 21);
	double dr = sqrt(dxp * dxp + dyp * dyp);
	missile[i].position.velocity.x = (dxp * (v << 16)) / dr;
	missile[i].position.velocity.y = (dyp * (v << 15)) / dr;
}

static void PutMissile(int i)
{
	int x = missile[i].position.tile.x;
	int y = missile[i].position.tile.y;
	if (!InDungeonBounds({ x, y }))
		missile[i]._miDelFlag = true;
	if (!missile[i]._miDelFlag) {
		dFlags[x][y] |= BFLAG_MISSILE;
		if (dMissile[x][y] == 0)
			dMissile[x][y] = i + 1;
		else
			dMissile[x][y] = -1;
		if (missile[i]._miPreFlag)
			MissilePreFlag = true;
	}
}

static void GetMissilePos(int i)
{
	int lx, ly;

	int mx = missile[i].position.traveled.x >> 16;
	int my = missile[i].position.traveled.y >> 16;
	int dx = mx + 2 * my;
	int dy = 2 * my - mx;
	if (dx < 0) {
		lx = -(-dx / 8);
		dx = -(-dx / 64);
	} else {
		lx = dx / 8;
		dx = dx / 64;
	}
	if (dy < 0) {
		ly = -(-dy / 8);
		dy = -(-dy / 64);
	} else {
		ly = dy / 8;
		dy = dy / 64;
	}
	missile[i].position.tile = Point { dx, dy } + missile[i].position.start;
	missile[i].position.offset.x = mx + (dy * 32) - (dx * 32);
	missile[i].position.offset.y = my - (dx * 16) - (dy * 16);
	ChangeLightOff(missile[i]._mlid, { lx - (dx * 8), ly - (dy * 8) });
}

void MoveMissilePos(int i)
{
	int dx, dy, x, y;

	switch (missile[i]._mimfnum) {
	case DIR_S:
		dx = 1;
		dy = 1;
		break;
	case DIR_SW:
		dx = 1;
		dy = 1;
		break;
	case DIR_W:
		dx = 0;
		dy = 1;
		break;
	case DIR_NW:
		dx = 0;
		dy = 0;
		break;
	case DIR_N:
		dx = 0;
		dy = 0;
		break;
	case DIR_NE:
		dx = 0;
		dy = 0;
		break;
	case DIR_E:
		dx = 1;
		dy = 0;
		break;
	case DIR_SE:
		dx = 1;
		dy = 1;
		break;
	}
	x = missile[i].position.tile.x + dx;
	y = missile[i].position.tile.y + dy;
	if (PosOkMonst(missile[i]._misource, { x, y })) {
		missile[i].position.tile.x += dx;
		missile[i].position.tile.y += dy;
		missile[i].position.offset.x += (dy * 32) - (dx * 32);
		missile[i].position.offset.y -= (dy * 16) + (dx * 16);
	}
}

bool MonsterTrapHit(int m, int mindam, int maxdam, int dist, int t, bool shift)
{
	int hit, hper, dam, mor;
	bool resist, ret;

	resist = false;
	if (monster[m].mtalkmsg != TEXT_NONE) {
		return false;
	}
	if (monster[m]._mhitpoints >> 6 <= 0) {
		return false;
	}
	if (monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MGOAL_RETREAT)
		return false;
	if (monster[m]._mmode == MM_CHARGE)
		return false;

	missile_resistance mir = missiledata[t].mResist;
	mor = monster[m].mMagicRes;
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

	hit = GenerateRnd(100);
	hper = 90 - (BYTE)monster[m].mArmorClass - dist;
	if (hper < 5)
		hper = 5;
	if (hper > 95)
		hper = 95;
	if (CheckMonsterHit(m, &ret)) {
		return ret;
	}
#ifdef _DEBUG
	if (hit < hper || debug_mode_dollar_sign || debug_mode_key_inverted_v || monster[m]._mmode == MM_STONE) {
#else
	if (hit < hper || monster[m]._mmode == MM_STONE) {
#endif
		dam = mindam + GenerateRnd(maxdam - mindam + 1);
		if (!shift)
			dam <<= 6;
		if (resist)
			monster[m]._mhitpoints -= dam / 4;
		else
			monster[m]._mhitpoints -= dam;
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
			monster[m]._mhitpoints = 0;
#endif
		if (monster[m]._mhitpoints >> 6 <= 0) {
			if (monster[m]._mmode == MM_STONE) {
				M_StartKill(m, -1);
				monster[m].Petrify();
			} else {
				M_StartKill(m, -1);
			}
		} else {
			if (resist) {
				PlayEffect(m, 1);
			} else if (monster[m]._mmode == MM_STONE) {
				if (m > MAX_PLRS - 1)
					M_StartHit(m, -1, dam);
				monster[m].Petrify();
			} else {
				if (m > MAX_PLRS - 1)
					M_StartHit(m, -1, dam);
			}
		}
		return true;
	}
	return false;
}

bool MonsterMHit(int pnum, int m, int mindam, int maxdam, int dist, int t, bool shift)
{
	int dam;

	bool resist = false;
	if (monster[m].mtalkmsg != TEXT_NONE
	    || monster[m]._mhitpoints >> 6 <= 0
	    || (t == MIS_HBOLT && monster[m].MType->mtype != MT_DIABLO && monster[m].MData->mMonstClass != MC_UNDEAD)) {
		return false;
	}
	if (monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MGOAL_RETREAT)
		return false;
	if (monster[m]._mmode == MM_CHARGE)
		return false;

	uint8_t mor = monster[m].mMagicRes;
	missile_resistance mir = missiledata[t].mResist;

	if (((mor & IMMUNE_MAGIC) != 0 && mir == MISR_MAGIC)
	    || ((mor & IMMUNE_FIRE) != 0 && mir == MISR_FIRE)
	    || ((mor & IMMUNE_LIGHTNING) != 0 && mir == MISR_LIGHTNING)
	    || ((mor & IMMUNE_ACID) != 0 && mir == MISR_ACID))
		return false;

	if (((mor & RESIST_MAGIC) != 0 && mir == MISR_MAGIC)
	    || ((mor & RESIST_FIRE) != 0 && mir == MISR_FIRE)
	    || ((mor & RESIST_LIGHTNING) != 0 && mir == MISR_LIGHTNING))
		resist = true;

	if (gbIsHellfire && t == MIS_HBOLT && (monster[m].MType->mtype == MT_DIABLO || monster[m].MType->mtype == MT_BONEDEMN))
		resist = true;

	int hit = GenerateRnd(100);
	int hper = 0;
	if (pnum != -1) {
		const auto &player = plr[pnum];
		if (missiledata[t].mType == 0) {
			hper = player._pDexterity;
			hper += player._pIBonusToHit;
			hper += player._pLevel;
			hper -= monster[m].mArmorClass;
			hper -= (dist * dist) / 2;
			hper += player._pIEnAc;
			hper += 50;
			if (player._pClass == HeroClass::Rogue)
				hper += 20;
			if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Bard)
				hper += 10;
		} else {
			hper = player._pMagic - (monster[m].mLevel * 2) - dist + 50;
			if (player._pClass == HeroClass::Sorcerer)
				hper += 20;
			else if (player._pClass == HeroClass::Bard)
				hper += 10;
		}
	} else {
		hper = GenerateRnd(75) - monster[m].mLevel * 2;
	}

	hper = clamp(hper, 5, 95);

	if (monster[m]._mmode == MM_STONE)
		hit = 0;

	bool ret = false;
	if (CheckMonsterHit(m, &ret))
		return ret;

#ifdef _DEBUG
	if (hit >= hper && !debug_mode_key_inverted_v && !debug_mode_dollar_sign)
		return false;
#else
	if (hit >= hper)
		return false;
#endif

	if (t == MIS_BONESPIRIT) {
		dam = monster[m]._mhitpoints / 3 >> 6;
	} else {
		dam = mindam + GenerateRnd(maxdam - mindam + 1);
	}

	if (missiledata[t].mType == 0) {
		const auto &player = plr[pnum];
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

	if (pnum == myplr)
		monster[m]._mhitpoints -= dam;

	if ((gbIsHellfire && (plr[pnum]._pIFlags & ISPL_NOHEALMON) != 0) || (!gbIsHellfire && (plr[pnum]._pIFlags & ISPL_FIRE_ARROWS) != 0))
		monster[m]._mFlags |= MFLAG_NOHEAL;

	if (monster[m]._mhitpoints >> 6 <= 0) {
		if (monster[m]._mmode == MM_STONE) {
			M_StartKill(m, pnum);
			monster[m].Petrify();
		} else {
			M_StartKill(m, pnum);
		}
	} else {
		if (resist) {
			PlayEffect(m, 1);
		} else if (monster[m]._mmode == MM_STONE) {
			if (m > MAX_PLRS - 1)
				M_StartHit(m, pnum, dam);
			monster[m].Petrify();
		} else {
			if (missiledata[t].mType == 0 && (plr[pnum]._pIFlags & ISPL_KNOCKBACK) != 0) {
				M_GetKnockback(m);
			}
			if (m > MAX_PLRS - 1)
				M_StartHit(m, pnum, dam);
		}
	}

	if (monster[m]._msquelch == 0) {
		monster[m]._msquelch = UINT8_MAX;
		monster[m].position.last = plr[pnum].position.tile;
	}

	return true;
}

bool PlayerMHit(int pnum, int m, int dist, int mind, int maxd, int mtype, bool shift, int earflag, bool *blocked)
{
	int hper, dam, resper;
	*blocked = false;

	auto &player = plr[pnum];

	if (player._pHitPoints >> 6 <= 0) {
		return false;
	}

	if (player._pInvincible) {
		return false;
	}

	if ((player._pSpellFlags & 1) != 0 && missiledata[mtype].mType == 0) {
		return false;
	}

	int hit = GenerateRnd(100);
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		hit = 1000;
#endif
	if (missiledata[mtype].mType == 0) {
		int tac = player._pIAC + player._pIBonusAC + player._pDexterity / 5;
		if (m != -1) {
			hper = monster[m].mHit
			    + ((monster[m].mLevel - player._pLevel) * 2)
			    + 30
			    - (dist * 2) - tac;
		} else {
			hper = 100 - (tac / 2) - (dist * 2);
		}
	} else {
		hper = 40;
		if (m != -1) {
			hper += (monster[m].mLevel * 2) - (player._pLevel * 2) - (dist * 2);
		}
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
		blk = GenerateRnd(100);
	}

	if (shift)
		blk = 100;
	if (mtype == MIS_ACIDPUD)
		blk = 100;

	int blkper = player._pBaseToBlk + player._pDexterity;
	if (m != -1)
		blkper -= (monster[m].mLevel - player._pLevel) * 2;

	if (blkper < 0)
		blkper = 0;
	if (blkper > 100)
		blkper = 100;

	switch (missiledata[mtype].mResist) {
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

	if (mtype == MIS_BONESPIRIT) {
		dam = player._pHitPoints / 3;
	} else {
		if (!shift) {
			dam = (mind << 6) + GenerateRnd((maxd - mind + 1) << 6);
			if (m == -1)
				if ((player._pIFlags & ISPL_ABSHALFTRAP) != 0)
					dam /= 2;
			dam += (player._pIGetHit << 6);
		} else {
			dam = mind + GenerateRnd(maxd - mind + 1);
			if (m == -1)
				if ((player._pIFlags & ISPL_ABSHALFTRAP) != 0)
					dam /= 2;
			dam += player._pIGetHit;
		}

		dam = std::max(dam, 64);
	}

	if ((resper <= 0 || gbIsHellfire) && blk < blkper) {
		Direction dir = player._pdir;
		if (m != -1) {
			dir = GetDirection(player.position.tile, monster[m].position.tile);
		}
		*blocked = true;
		StartPlrBlock(pnum, dir);
		return true;
	}

	if (resper > 0) {
		dam = dam - dam * resper / 100;
		if (pnum == myplr) {
			ApplyPlrDamage(pnum, 0, 0, dam, earflag);
		}

		if (player._pHitPoints >> 6 > 0) {
			player.Say(HeroSpeech::ArghClang);
		}
		return true;
	}

	if (pnum == myplr) {
		ApplyPlrDamage(pnum, 0, 0, dam, earflag);
	}

	if (player._pHitPoints >> 6 > 0) {
		StartPlrHit(pnum, dam, false);
	}

	return true;
}

bool Plr2PlrMHit(int pnum, int p, int mindam, int maxdam, int dist, int mtype, bool shift, bool *blocked)
{
	int dam, hit, resper;

	if (sgGameInitInfo.bFriendlyFire == 0 && gbFriendlyMode)
		return false;

	*blocked = false;

	auto &player = plr[pnum];
	auto &target = plr[p];

	if (target._pInvincible) {
		return false;
	}

	if (mtype == MIS_HBOLT) {
		return false;
	}

	if ((target._pSpellFlags & 1) != 0 && missiledata[mtype].mType == 0) {
		return false;
	}

	switch (missiledata[mtype].mResist) {
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

	int hper = GenerateRnd(100);

	if (missiledata[mtype].mType == 0) {
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
		blkper = GenerateRnd(100);
	}

	int blk = target._pDexterity + target._pBaseToBlk + (target._pLevel * 2) - (player._pLevel * 2);
	blk = clamp(blk, 0, 100);

	if (mtype == MIS_BONESPIRIT) {
		dam = target._pHitPoints / 3;
	} else {
		dam = mindam + GenerateRnd(maxdam - mindam + 1);
		if (missiledata[mtype].mType == 0)
			dam += player._pIBonusDamMod + player._pDamageMod + dam * player._pIBonusDam / 100;
		if (!shift)
			dam <<= 6;
	}
	if (missiledata[mtype].mType != 0)
		dam /= 2;
	if (resper > 0) {
		dam -= (dam * resper) / 100;
		if (pnum == myplr)
			NetSendCmdDamage(true, p, dam);
		player.Say(HeroSpeech::ArghClang);
		return true;
	}

	if (blkper < blk) {
		StartPlrBlock(p, GetDirection(target.position.tile, player.position.tile));
		*blocked = true;
	} else {
		if (pnum == myplr)
			NetSendCmdDamage(true, p, dam);
		StartPlrHit(p, dam, false);
	}

	return true;
}

void CheckMissileCol(int i, int mindam, int maxdam, bool shift, Point position, bool nodel)
{
	int oi;
	bool blocked;
	int dir, mAnimFAmt;

	int mx = position.x;
	int my = position.y;

	if (i >= MAXMISSILES || i < 0)
		return;
	if (mx >= MAXDUNX || mx < 0)
		return;
	if (my >= MAXDUNY || my < 0)
		return;
	if (missile[i]._micaster != TARGET_BOTH && missile[i]._misource != -1) {
		if (missile[i]._micaster == TARGET_MONSTERS) {
			if (dMonster[mx][my] > 0) {
				if (MonsterMHit(
				        missile[i]._misource,
				        dMonster[mx][my] - 1,
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = true;
				}
			} else {
				if (dMonster[mx][my] < 0
				    && monster[-(dMonster[mx][my] + 1)]._mmode == MM_STONE
				    && MonsterMHit(
				        missile[i]._misource,
				        -(dMonster[mx][my] + 1),
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = true;
				}
			}
			if (dPlayer[mx][my] > 0
			    && dPlayer[mx][my] - 1 != missile[i]._misource
			    && Plr2PlrMHit(
			        missile[i]._misource,
			        dPlayer[mx][my] - 1,
			        mindam,
			        maxdam,
			        missile[i]._midist,
			        missile[i]._mitype,
			        shift,
			        &blocked)) {
				if (gbIsHellfire && blocked) {
					dir = missile[i]._mimfnum + (GenerateRnd(2) != 0 ? 1 : -1);
					mAnimFAmt = misfiledata[missile[i]._miAnimType].mAnimFAmt;
					if (dir < 0)
						dir = mAnimFAmt - 1;
					else if (dir > mAnimFAmt)
						dir = 0;

					SetMissDir(i, dir);
				} else if (!nodel) {
					missile[i]._mirange = 0;
				}
				missile[i]._miHitFlag = true;
			}
		} else {
			if ((monster[missile[i]._misource]._mFlags & MFLAG_TARGETS_MONSTER) != 0
			    && dMonster[mx][my] > 0
			    && (monster[dMonster[mx][my] - 1]._mFlags & MFLAG_GOLEM) != 0
			    && MonsterTrapHit(dMonster[mx][my] - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = true;
			}
			if (dPlayer[mx][my] > 0
			    && PlayerMHit(
			        dPlayer[mx][my] - 1,
			        missile[i]._misource,
			        missile[i]._midist,
			        mindam,
			        maxdam,
			        missile[i]._mitype,
			        shift,
			        0,
			        &blocked)) {
				if (gbIsHellfire && blocked) {
					dir = missile[i]._mimfnum + (GenerateRnd(2) != 0 ? 1 : -1);
					mAnimFAmt = misfiledata[missile[i]._miAnimType].mAnimFAmt;
					if (dir < 0)
						dir = mAnimFAmt - 1;
					else if (dir > mAnimFAmt)
						dir = 0;

					SetMissDir(i, dir);
				} else if (!nodel) {
					missile[i]._mirange = 0;
				}
				missile[i]._miHitFlag = true;
			}
		}
	} else {
		if (dMonster[mx][my] > 0) {
			if (missile[i]._micaster == TARGET_BOTH) {
				if (MonsterMHit(
				        missile[i]._misource,
				        dMonster[mx][my] - 1,
				        mindam,
				        maxdam,
				        missile[i]._midist,
				        missile[i]._mitype,
				        shift)) {
					if (!nodel)
						missile[i]._mirange = 0;
					missile[i]._miHitFlag = true;
				}
			} else if (MonsterTrapHit(dMonster[mx][my] - 1, mindam, maxdam, missile[i]._midist, missile[i]._mitype, shift)) {
				if (!nodel)
					missile[i]._mirange = 0;
				missile[i]._miHitFlag = true;
			}
		}
		if (dPlayer[mx][my] > 0) {
			if (PlayerMHit(
			        dPlayer[mx][my] - 1,
			        -1,
			        missile[i]._midist,
			        mindam,
			        maxdam,
			        missile[i]._mitype,
			        shift,
			        (missile[i]._miAnimType == MFILE_FIREWAL || missile[i]._miAnimType == MFILE_LGHNING) ? 1 : 0,
			        &blocked)) {
				if (gbIsHellfire && blocked) {
					dir = missile[i]._mimfnum + (GenerateRnd(2) != 0 ? 1 : -1);
					mAnimFAmt = misfiledata[missile[i]._miAnimType].mAnimFAmt;
					if (dir < 0)
						dir = mAnimFAmt - 1;
					else if (dir > mAnimFAmt)
						dir = 0;

					SetMissDir(i, dir);
				} else if (!nodel) {
					missile[i]._mirange = 0;
				}
				missile[i]._miHitFlag = true;
			}
		}
	}
	if (dObject[mx][my] != 0) {
		oi = dObject[mx][my] > 0 ? dObject[mx][my] - 1 : -(dObject[mx][my] + 1);
		if (!object[oi]._oMissFlag) {
			if (object[oi]._oBreak == 1)
				BreakObject(-1, oi);
			if (!nodel)
				missile[i]._mirange = 0;
			missile[i]._miHitFlag = false;
		}
	}
	if (nMissileTable[dPiece[mx][my]]) {
		if (!nodel)
			missile[i]._mirange = 0;
		missile[i]._miHitFlag = false;
	}
	if (missile[i]._mirange == 0 && missiledata[missile[i]._mitype].miSFX != -1)
		PlaySfxLoc(missiledata[missile[i]._mitype].miSFX, missile[i].position.tile);
}

void SetMissAnim(int mi, int animtype)
{
	int dir = missile[mi]._mimfnum;

	if (animtype > MFILE_NONE) {
		animtype = MFILE_NONE;
	}

	missile[mi]._miAnimType = animtype;
	missile[mi]._miAnimFlags = misfiledata[animtype].mFlags;
	missile[mi]._miAnimData = misfiledata[animtype].mAnimData[dir];
	missile[mi]._miAnimDelay = misfiledata[animtype].mAnimDelay[dir];
	missile[mi]._miAnimLen = misfiledata[animtype].mAnimLen[dir];
	missile[mi]._miAnimWidth = misfiledata[animtype].mAnimWidth[dir];
	missile[mi]._miAnimWidth2 = misfiledata[animtype].mAnimWidth2[dir];
	missile[mi]._miAnimCnt = 0;
	missile[mi]._miAnimFrame = 1;
}

void SetMissDir(int mi, int dir)
{
	missile[mi]._mimfnum = dir;
	SetMissAnim(mi, missile[mi]._miAnimType);
}

void LoadMissileGFX(BYTE mi)
{
	MisFileData *mfd = &misfiledata[mi];
	if (mfd->mAnimData[0] != nullptr)
		return;

	char pszName[256];
	if ((mfd->mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
		sprintf(pszName, "Missiles\\%s.CL2", mfd->mName);
		byte *file = LoadFileInMem(pszName).release();
		for (unsigned i = 0; i < mfd->mAnimFAmt; i++)
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
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt != 0; mi++) {
		if (!gbIsHellfire && mi > MFILE_SCBSEXPD)
			break;
		if ((misfiledata[mi].mFlags & MFLAG_HIDDEN) == 0)
			LoadMissileGFX(mi);
	}
}

void FreeMissileGFX(int mi)
{
	int i;
	DWORD *p;

	if ((misfiledata[mi].mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
		if (misfiledata[mi].mAnimData[0] != nullptr) {
			p = (DWORD *)misfiledata[mi].mAnimData[0];
			p -= misfiledata[mi].mAnimFAmt;
			delete[] p;
			misfiledata[mi].mAnimData[0] = nullptr;
		}
		return;
	}

	for (i = 0; i < misfiledata[mi].mAnimFAmt; i++) {
		if (misfiledata[mi].mAnimData[i] != nullptr) {
			delete[] misfiledata[mi].mAnimData[i];
			misfiledata[mi].mAnimData[i] = nullptr;
		}
	}
}

void FreeMissiles()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt != 0; mi++) {
		if ((misfiledata[mi].mFlags & MFLAG_HIDDEN) == 0)
			FreeMissileGFX(mi);
	}
}

void FreeMissiles2()
{
	int mi;

	for (mi = 0; misfiledata[mi].mAnimFAmt != 0; mi++) {
		if ((misfiledata[mi].mFlags & MFLAG_HIDDEN) != 0)
			FreeMissileGFX(mi);
	}
}

void InitMissiles()
{
	auto &myPlayer = plr[myplr];

	AutoMapShowItems = false;
	myPlayer._pSpellFlags &= ~0x1;
	if (myPlayer._pInfraFlag) {
		for (int i = 0; i < nummissiles; ++i) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MIS_INFRA) {
				int src = missile[mi]._misource;
				if (src == myplr)
					CalcPlrItemVals(myplr, true);
			}
		}
	}

	if ((myPlayer._pSpellFlags & 2) == 2 || (myPlayer._pSpellFlags & 4) == 4) {
		myPlayer._pSpellFlags &= ~0x2;
		myPlayer._pSpellFlags &= ~0x4;
		for (int i = 0; i < nummissiles; ++i) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MIS_BLODBOIL) {
				if (missile[mi]._misource == myplr) {
					int missingHP = myPlayer._pMaxHP - myPlayer._pHitPoints;
					CalcPlrItemVals(myplr, true);
					ApplyPlrDamage(myplr, 0, 1, missingHP + missile[mi]._miVar2);
				}
			}
		}
	}

	nummissiles = 0;
	for (int i = 0; i < MAXMISSILES; i++) {
		missileavail[i] = i;
		missileactive[i] = 0;
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
	missile[mi]._miDelFlag = true;
}

static bool missiles_found_target(int mi, Point *position, int rad)
{
	rad = std::min(rad, 19);

	for (int j = 0; j < rad; j++) {
		int k = CrawlNum[j] + 2;
		for (int i = CrawlTable[CrawlNum[j]]; i > 0; i--, k += 2) {
			int tx = position->x + CrawlTable[k - 1];
			int ty = position->y + CrawlTable[k];
			if (!InDungeonBounds({ tx, ty }))
				continue;

			int dp = dPiece[tx][ty];
			if (nSolidTable[dp] || dObject[tx][ty] != 0 || dMissile[tx][ty] != 0)
				continue;

			missile[mi].position.tile = { tx, ty };
			*position = { tx, ty };
			return true;
		}
	}
	return false;
}

void AddFireRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNEFIRE);
		if (missiles_found_target(mi, &dst, 10)) {
			missile[mi]._miVar1 = MIS_HIVEEXP;
			missile[mi]._miDelFlag = false;
			missile[mi]._mlid = AddLight(dst, 8);
		} else {
			missile[mi]._miDelFlag = true;
		}
	} else {
		missile[mi]._miDelFlag = true;
	}
}

void AddLightningRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNELIGHT);
		if (missiles_found_target(mi, &dst, 10)) {
			missile[mi]._miVar1 = MIS_LIGHTBALL;
			missile[mi]._miDelFlag = false;
			missile[mi]._mlid = AddLight(dst, 8);
		} else {
			missile[mi]._miDelFlag = true;
		}
	} else {
		missile[mi]._miDelFlag = true;
	}
}

void AddGreatLightningRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNENOVA);
		if (missiles_found_target(mi, &dst, 10)) {
			missile[mi]._miVar1 = MIS_NOVA;
			missile[mi]._miDelFlag = false;
			missile[mi]._mlid = AddLight(dst, 8);
		} else {
			missile[mi]._miDelFlag = true;
		}
	} else {
		missile[mi]._miDelFlag = true;
	}
}

void AddImmolationRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNEIMMOLAT);
		if (missiles_found_target(mi, &dst, 10)) {
			missile[mi]._miVar1 = MIS_IMMOLATION;
			missile[mi]._miDelFlag = false;
			missile[mi]._mlid = AddLight(dst, 8);
		} else {
			missile[mi]._miDelFlag = true;
		}
	} else {
		missile[mi]._miDelFlag = true;
	}
}

void AddStoneRune(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (LineClearMissile(src, dst)) {
		if (id >= 0)
			UseMana(id, SPL_RUNESTONE);
		if (missiles_found_target(mi, &dst, 10)) {
			missile[mi]._miVar1 = MIS_STONE;
			missile[mi]._miDelFlag = false;
			missile[mi]._mlid = AddLight(dst, 8);
		} else {
			missile[mi]._miDelFlag = true;
		}
	} else {
		missile[mi]._miDelFlag = true;
	}
}

void AddReflection(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (id >= 0) {
		int lvl = 2;
		if (missile[mi]._mispllvl != 0)
			lvl = missile[mi]._mispllvl;

		plr[id].wReflections += lvl * plr[id]._pLevel;

		UseMana(id, SPL_REFLECT);
	}

	missile[mi]._mirange = 0;
	missile[mi]._miDelFlag = false;
}

void AddBerserk(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._mirange = 0;
	missile[mi]._miDelFlag = true;

	if (id < 0)
		return;

	missile[mi]._misource = id;
	for (int j = 0; j < 6; j++) {
		int k = CrawlNum[j] + 2;
		for (int i = CrawlTable[CrawlNum[j]]; i > 0; i--, k += 2) {
			int tx = dst.x + CrawlTable[k - 1];
			int ty = dst.y + CrawlTable[k];
			if (!InDungeonBounds({ tx, ty }))
				continue;

			int dm = dMonster[tx][ty];
			dm = dm > 0 ? dm - 1 : -(dm + 1);
			if (dm <= 3)
				continue;

			if (monster[dm]._uniqtype != 0 || monster[dm]._mAi == AI_DIABLO)
				continue;
			if (monster[dm]._mmode == MM_FADEIN || monster[dm]._mmode == MM_FADEOUT)
				continue;
			if ((monster[dm].mMagicRes & IMMUNE_MAGIC) != 0)
				continue;
			if ((monster[dm].mMagicRes & RESIST_MAGIC) != 0 && ((monster[dm].mMagicRes & RESIST_MAGIC) != 1 || GenerateRnd(2) != 0))
				continue;
			if (monster[dm]._mmode == MM_CHARGE)
				continue;

			j = 6;
			auto slvl = static_cast<double>(GetSpellLevel(id, SPL_BERSERK));
			monster[dm]._mFlags |= MFLAG_BERSERK | MFLAG_GOLEM;
			monster[dm].mMinDamage = ((double)(GenerateRnd(10) + 20) / 100 + 1) * (double)monster[dm].mMinDamage + slvl;
			monster[dm].mMaxDamage = ((double)(GenerateRnd(10) + 20) / 100 + 1) * (double)monster[dm].mMaxDamage + slvl;
			monster[dm].mMinDamage2 = ((double)(GenerateRnd(10) + 20) / 100 + 1) * (double)monster[dm].mMinDamage2 + slvl;
			monster[dm].mMaxDamage2 = ((double)(GenerateRnd(10) + 20) / 100 + 1) * (double)monster[dm].mMaxDamage2 + slvl;
			int r = (currlevel < 17 || currlevel > 20) ? 3 : 9;
			monster[dm].mlid = AddLight(monster[dm].position.tile, r);
			UseMana(id, SPL_BERSERK);
			break;
		}
	}
}

void AddHorkSpawn(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	GetMissileVel(mi, src, dst, 8);
	missile[mi]._mirange = 9;
	missile[mi]._miVar1 = midir;
	PutMissile(mi);
}

void AddJester(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int spell;

	spell = MIS_FIREBOLT;
	switch (GenerateRnd(10)) {
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
	AddMissile(src, dst, midir, spell, missile[mi]._micaster, missile[mi]._misource, 0, missile[mi]._mispllvl);
	missile[mi]._miDelFlag = true;
	missile[mi]._mirange = 0;
}

void AddStealPotions(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._misource = id;
	for (int i = 0; i < 3; i++) {
		int k = CrawlNum[i];
		int l = k + 2;
		for (int j = CrawlTable[k]; j > 0; j--, l += 2) {
			int tx = src.x + CrawlTable[l - 1];
			int ty = src.y + CrawlTable[l];
			if (!InDungeonBounds({ tx, ty }))
				continue;
			int pnum = dPlayer[tx][ty];
			if (pnum == 0)
				continue;
			auto &player = plr[pnum > 0 ? pnum - 1 : -(pnum + 1)];

			bool hasPlayedSFX = false;
			for (int si = 0; si < MAXBELTITEMS; si++) {
				int ii = -1;
				if (player.SpdList[si]._itype == ITYPE_MISC) {
					if (GenerateRnd(2) == 0)
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
						if (GenerateRnd(2) != 0) {
							ii = ItemMiscIdIdx(IMISC_MANA);
						} else {
							ii = ItemMiscIdIdx(IMISC_HEAL);
						}
						break;
					case IMISC_FULLREJUV:
						switch (GenerateRnd(3)) {
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
	missile[mi]._mirange = 0;
	missile[mi]._miDelFlag = true;
}

void AddManaTrap(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._misource = id;
	for (int i = 0; i < 3; i++) {
		int k = CrawlNum[i];
		int pn = k + 2;
		for (int j = CrawlTable[k]; j > 0; j--) {
			int tx = src.x + CrawlTable[pn - 1];
			int ty = src.y + CrawlTable[pn];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				int pid = dPlayer[tx][ty];
				if (pid != 0) {
					auto &player = plr[(pid > 0) ? pid - 1 : -(pid + 1)];

					player._pMana = 0;
					player._pManaBase = player._pMana + player._pMaxManaBase - player._pMaxMana;
					CalcPlrInv(pid, false);
					drawmanaflag = true;
					PlaySfxLoc(TSFX_COW7, { tx, ty });
				}
			}
			pn += 2;
		}
	}
	missile[mi]._mirange = 0;
	missile[mi]._miDelFlag = true;
}

void AddSpecArrow(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	int av = 0;

	if (mienemy == TARGET_MONSTERS) {
		auto &player = plr[id];

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

	missile[mi]._mirange = 1;
	missile[mi]._miVar1 = dst.x;
	missile[mi]._miVar2 = dst.y;
	missile[mi]._miVar3 = av;
}

void AddWarp(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	int minDistanceSq = std::numeric_limits<int>::max();
	if (id >= 0) {
		src = plr[id].position.tile;
	}
	Point tile = src;

	for (int i = 0; i < numtrigs && i < MAXTRIGGERS; i++) {
		TriggerStruct *trg = &trigs[i];
		if (trg->_tmsg == 1032 || trg->_tmsg == 1027 || trg->_tmsg == 1026 || trg->_tmsg == 1028) {
			Point candidate = trg->position;
			if ((leveltype == 1 || leveltype == 2) && (trg->_tmsg == 1026 || trg->_tmsg == 1027 || trg->_tmsg == 1028)) {
				candidate += Point { 0, 1 };
			} else {
				candidate += Point { 1, 0 };
			}
			Point off = src - candidate;
			int distanceSq = off.y * off.y + off.x * off.x;
			if (distanceSq < minDistanceSq) {
				minDistanceSq = distanceSq;
				tile = candidate;
			}
		}
	}
	missile[mi]._mirange = 2;
	missile[mi]._miVar1 = 0;
	missile[mi].position.tile = tile;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_WARP);
}

void AddLightningWall(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int dam)
{
	GetMissileVel(mi, src, dst, 16);
	missile[mi]._midam = dam;
	missile[mi]._miAnimFrame = GenerateRnd(8) + 1;
	missile[mi]._mirange = 255 * (missile[mi]._mispllvl + 1);
	if (id < 0) {
		missile[mi]._miVar1 = src.x;
		missile[mi]._miVar2 = src.y;
	} else {
		missile[mi]._miVar1 = plr[id].position.tile.x;
		missile[mi]._miVar2 = plr[id].position.tile.y;
	}
}

void AddRuneExplosion(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	if (mienemy == TARGET_MONSTERS || mienemy == TARGET_BOTH) {
		int dmg = 2 * (plr[id]._pLevel + GenerateRndSum(10, 2)) + 4;
		dmg = ScaleSpellEffect(dmg, missile[mi]._mispllvl);

		missile[mi]._midam = dmg;

		constexpr Point offsets[] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };
		for (Point offset : offsets)
			CheckMissileCol(mi, dmg, dmg, false, missile[mi].position.tile + offset, true);
	}
	missile[mi]._mlid = AddLight(src, 8);
	SetMissDir(mi, 0);
	missile[mi]._miDelFlag = false;
	missile[mi]._mirange = missile[mi]._miAnimLen - 1;
}

void AddImmolation(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int sp = 16;
	if (mienemy == TARGET_MONSTERS) {
		sp += std::min(missile[mi]._mispllvl * 2, 34);

		int dmg = 2 * (plr[id]._pLevel + GenerateRndSum(10, 2)) + 4;
		missile[mi]._midam = ScaleSpellEffect(dmg, missile[mi]._mispllvl);

		UseMana(id, SPL_FIREBALL);
	}
	GetMissileVel(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = src.x;
	missile[mi]._miVar5 = src.y;
	missile[mi]._miVar6 = 2;
	missile[mi]._miVar7 = 2;
	missile[mi]._mlid = AddLight(src, 8);
}

void AddFireNova(int mi, Point src, Point dst, int midir, int8_t mienemy, int /*id*/, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int sp = 16;
	if (mienemy == TARGET_MONSTERS) {
		sp += std::min(missile[mi]._mispllvl, 34);
	}
	GetMissileVel(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = src.x;
	missile[mi]._miVar5 = src.y;
	missile[mi]._mlid = AddLight(src, 8);
}

void AddLightningArrow(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	GetMissileVel(mi, src, dst, 32);
	missile[mi]._miAnimFrame = GenerateRnd(8) + 1;
	missile[mi]._mirange = 255;
	if (id < 0) {
		missile[mi]._miVar1 = src.x;
		missile[mi]._miVar2 = src.y;
	} else {
		missile[mi]._miVar1 = plr[id].position.tile.x;
		missile[mi]._miVar2 = plr[id].position.tile.y;
	}
	missile[mi]._midam <<= 6;
}

void AddMana(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &player = plr[id];

	int ManaAmount = (GenerateRnd(10) + 1) << 6;
	for (int i = 0; i < player._pLevel; i++) {
		ManaAmount += (GenerateRnd(4) + 1) << 6;
	}
	for (int i = 0; i < missile[mi]._mispllvl; i++) {
		ManaAmount += (GenerateRnd(6) + 1) << 6;
	}
	if (player._pClass == HeroClass::Sorcerer)
		ManaAmount *= 2;
	if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Bard)
		ManaAmount += ManaAmount / 2;
	player._pMana += ManaAmount;
	if (player._pMana > player._pMaxMana)
		player._pMana = player._pMaxMana;
	player._pManaBase += ManaAmount;
	if (player._pManaBase > player._pMaxManaBase)
		player._pManaBase = player._pMaxManaBase;
	UseMana(id, SPL_MANA);
	missile[mi]._miDelFlag = true;
	drawmanaflag = true;
}

void AddMagi(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &player = plr[id];

	player._pMana = player._pMaxMana;
	player._pManaBase = player._pMaxManaBase;
	UseMana(id, SPL_MAGI);
	missile[mi]._miDelFlag = true;
	drawmanaflag = true;
}

void AddRing(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	missile[mi]._miDelFlag = true;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_FIRERING);
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._miDelFlag = false;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._miVar5 = 0;
	missile[mi]._miVar6 = 0;
	missile[mi]._miVar7 = 0;
	missile[mi].limitReached = false;
	missile[mi]._mirange = 7;
}

void AddSearch(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	int i, mx, r1, r2;
	MissileStruct *mis;

	missile[mi]._miDelFlag = false;
	missile[mi]._miVar1 = id;
	missile[mi]._miVar2 = 0;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._miVar5 = 0;
	missile[mi]._miVar6 = 0;
	missile[mi]._miVar7 = 0;
	missile[mi].limitReached = false;
	AutoMapShowItems = true;
	int lvl = 2;
	if (id > -1)
		lvl = plr[id]._pLevel * 2;
	missile[mi]._mirange = lvl + 10 * missile[mi]._mispllvl + 245;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_SEARCH);

	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (mx != mi) {
			mis = &missile[mx];
			if (mis->_miVar1 == id && mis->_mitype == 85) {
				r1 = missile[mi]._mirange;
				r2 = mis->_mirange;
				if (r2 < INT_MAX - r1)
					mis->_mirange = r1 + r2;
				missile[mi]._miDelFlag = true;
				break;
			}
		}
	}
}

void AddCboltArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int /*id*/, int /*dam*/)
{
	missile[mi]._mirnd = GenerateRnd(15) + 1;
	if (mienemy != TARGET_MONSTERS) {
		missile[mi]._midam = 15;
	}

	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	missile[mi]._miAnimFrame = GenerateRnd(8) + 1;
	missile[mi]._mlid = AddLight(src, 5);
	GetMissileVel(mi, src, dst, 8);
	missile[mi]._miVar1 = 5;
	missile[mi]._miVar2 = midir;
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
}

void AddLArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int av = 32;
	if (mienemy == TARGET_MONSTERS) {
		auto &player = plr[id];
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
	GetMissileVel(mi, src, dst, av);

	SetMissDir(mi, GetDirection16(src, dst));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._mlid = AddLight(src, 5);
}

void AddArrow(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int av = 32;
	if (mienemy == TARGET_MONSTERS) {
		auto &player = plr[id];

		if ((player._pIFlags & ISPL_RNDARROWVEL) != 0) {
			av = GenerateRnd(32) + 16;
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
	GetMissileVel(mi, src, dst, av);
	missile[mi]._miAnimFrame = GetDirection16(src, dst) + 1;
	missile[mi]._mirange = 256;
}

void GetVileMissPos(int mi, Point dst)
{
	int xx, yy, k, j, i;

	for (k = 1; k < 50; k++) {
		for (j = -k; j <= k; j++) {
			yy = j + dst.y;
			for (i = -k; i <= k; i++) {
				xx = i + dst.x;
				if (PosOkPlayer(myplr, { xx, yy })) {
					missile[mi].position.tile = { xx, yy };
					return;
				}
			}
		}
	}
	missile[mi].position.tile = dst;
}

void AddRndTeleport(int mi, Point src, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	int pn, r1, r2, nTries;

	nTries = 0;
	do {
		nTries++;
		if (nTries > 500) {
			r1 = 0;
			r2 = 0;
			break; //BUGFIX: warps player to 0/0 in hellfire, change to return or use 1.09's version of the code
		}
		r1 = GenerateRnd(3) + 4;
		r2 = GenerateRnd(3) + 4;
		if (GenerateRnd(2) == 1)
			r1 = -r1;
		if (GenerateRnd(2) == 1)
			r2 = -r2;

		r1 += src.x;
		r2 += src.y;
		if (r1 < MAXDUNX && r1 >= 0 && r2 < MAXDUNY && r2 >= 0) { ///BUGFIX: < MAXDUNX / < MAXDUNY (fixed)
			pn = dPiece[r1][r2];
		}
	} while (nSolidTable[pn] || dObject[r1][r2] != 0 || dMonster[r1][r2] != 0);

	missile[mi]._mirange = 2;
	missile[mi]._miVar1 = 0;
	if (!setlevel || setlvlnum != SL_VILEBETRAYER) {
		missile[mi].position.tile = { r1, r2 };
		if (mienemy == TARGET_MONSTERS)
			UseMana(id, SPL_RNDTELEPORT);
	} else {
		pn = dObject[dst.x][dst.y] - 1;
		// BUGFIX: should only run magic circle check if dObject[dx][dy] is non-zero.
		if (object[pn]._otype == OBJ_MCIRCLE1 || object[pn]._otype == OBJ_MCIRCLE2) {
			missile[mi].position.tile = dst;
			if (!PosOkPlayer(myplr, dst))
				GetVileMissPos(mi, dst);
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
			sp += std::min(missile[mi]._mispllvl * 2, 47);
		}

		int i;
		for (i = 0; i < nummissiles; i++) {
			int mx = missileactive[i];
			if (missile[mx]._mitype == MIS_GUARDIAN && missile[mx]._misource == id && missile[mx]._miVar3 == mi)
				break;
		}
		if (i == nummissiles)
			UseMana(id, SPL_FIREBOLT);
	}
	GetMissileVel(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._mlid = AddLight(src, 8);
}

void AddMagmaball(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	GetMissileVel(mi, src, dst, 16);
	missile[mi].position.traveled.x += 3 * missile[mi].position.velocity.x;
	missile[mi].position.traveled.y += 3 * missile[mi].position.velocity.y;
	GetMissilePos(mi);
	if (!gbIsHellfire || (missile[mi].position.velocity.x & 0xFFFF0000) != 0 || (missile[mi].position.velocity.y & 0xFFFF0000) != 0)
		missile[mi]._mirange = 256;
	else
		missile[mi]._mirange = 1;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._mlid = AddLight(src, 8);
}

void AddKrull(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	GetMissileVel(mi, src, dst, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	PutMissile(mi);
}

void AddTeleport(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int i, pn, k, j, tx, ty;

	missile[mi]._miDelFlag = true;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			tx = dst.x + CrawlTable[pn - 1];
			ty = dst.y + CrawlTable[pn];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				if (!nSolidTable[dPiece[tx][ty]] && dMonster[tx][ty] == 0 && dObject[tx][ty] == 0 && dPlayer[tx][ty] == 0) {
					missile[mi].position.tile = { tx, ty };
					missile[mi].position.start = { tx, ty };
					missile[mi]._miDelFlag = false;
					i = 6;
					break;
				}
			}
			pn += 2;
		}
	}

	if (!missile[mi]._miDelFlag) {
		UseMana(id, SPL_TELEPORT);
		missile[mi]._mirange = 2;
	}
}

void AddLightball(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int dam)
{
	GetMissileVel(mi, src, dst, 16);
	missile[mi]._midam = dam;
	missile[mi]._miAnimFrame = GenerateRnd(8) + 1;
	missile[mi]._mirange = 255;
	if (id < 0) {
		missile[mi]._miVar1 = src.x;
		missile[mi]._miVar2 = src.y;
	} else {
		missile[mi]._miVar1 = plr[id].position.tile.x;
		missile[mi]._miVar2 = plr[id].position.tile.y;
	}
}

void AddFirewall(int mi, Point src, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	missile[mi]._midam = GenerateRndSum(10, 2) + 2;
	missile[mi]._midam += id >= 0 ? plr[id]._pLevel : currlevel; // BUGFIX: missing parenthesis around ternary (fixed)
	missile[mi]._midam <<= 3;
	GetMissileVel(mi, src, dst, 16);
	int i = missile[mi]._mispllvl;
	missile[mi]._mirange = 10;
	if (i > 0)
		missile[mi]._mirange *= i + 1;
	if (mienemy == TARGET_PLAYERS || id < 0)
		missile[mi]._mirange += currlevel;
	else
		missile[mi]._mirange += (plr[id]._pISplDur * missile[mi]._mirange) / 128;
	missile[mi]._mirange *= 16;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void AddFireball(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	int sp = 16;
	if (mienemy == TARGET_MONSTERS) {
		sp += std::min(missile[mi]._mispllvl * 2, 34);

		int dmg = 2 * (plr[id]._pLevel + GenerateRndSum(10, 2)) + 4;
		missile[mi]._midam = ScaleSpellEffect(dmg, missile[mi]._mispllvl);

		UseMana(id, SPL_FIREBALL);
	}
	GetMissileVel(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = src.x;
	missile[mi]._miVar5 = src.y;
	missile[mi]._mlid = AddLight(src, 8);
}

void AddLightctrl(int mi, Point src, Point dst, int /*midir*/, int8_t mienemy, int id, int dam)
{
	if (dam == 0 && mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_LIGHTNING);
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	GetMissileVel(mi, src, dst, 32);
	missile[mi]._miAnimFrame = GenerateRnd(8) + 1;
	missile[mi]._mirange = 256;
}

void AddLightning(int mi, Point /*src*/, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	missile[mi].position.start = dst;
	if (midir >= 0) {
		missile[mi].position.offset = missile[midir].position.offset;
		missile[mi].position.traveled = missile[midir].position.traveled;
	}
	missile[mi]._miAnimFrame = GenerateRnd(8) + 1;

	if (midir < 0 || mienemy == TARGET_PLAYERS || id == -1) {
		if (midir < 0 || id == -1)
			missile[mi]._mirange = 8;
		else
			missile[mi]._mirange = 10;
	} else {
		missile[mi]._mirange = (missile[mi]._mispllvl / 2) + 6;
	}
	missile[mi]._mlid = AddLight(missile[mi].position.tile, 4);
}

void AddMisexp(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	if (mienemy != 0 && id > 0) {
		switch (monster[id].MType->mtype) {
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

	missile[mi].position.tile = missile[dst.x].position.tile;
	missile[mi].position.start = missile[dst.x].position.start;
	missile[mi].position.offset = missile[dst.x].position.offset;
	missile[mi].position.traveled = missile[dst.x].position.traveled;
	missile[mi].position.velocity = { 0, 0 };
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void AddWeapexp(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	missile[mi].position.tile = src;
	missile[mi].position.start = src;
	missile[mi].position.velocity = { 0, 0 };
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = dst.x;
	missile[mi]._mimfnum = 0;
	if (dst.x == 1)
		SetMissAnim(mi, MFILE_MAGBLOS);
	else
		SetMissAnim(mi, MFILE_MINILTNG);
	missile[mi]._mirange = missile[mi]._miAnimLen - 1;
}

bool CheckIfTrig(Point position)
{
	for (int i = 0; i < numtrigs; i++) {
		if (trigs[i].position.WalkingDistance(position) < 2)
			return true;
	}
	return false;
}

void AddTown(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int i, j, k, mx, tx, ty, dp;

	if (currlevel != 0) {
		missile[mi]._miDelFlag = true;
		for (j = 0; j < 6; j++) {
			k = CrawlNum[j] + 2;
			for (i = (BYTE)CrawlTable[CrawlNum[j]]; i > 0; i--) {
				tx = dst.x + CrawlTable[k - 1];
				ty = dst.y + CrawlTable[k];
				if (InDungeonBounds({ tx, ty })) {
					dp = dPiece[tx][ty];
					if (dMissile[tx][ty] == 0 && !nSolidTable[dp] && !nMissileTable[dp] && dObject[tx][ty] == 0 && dPlayer[tx][ty] == 0) {
						if (!CheckIfTrig({ tx, ty })) {
							missile[mi].position.tile = { tx, ty };
							missile[mi].position.start = { tx, ty };
							missile[mi]._miDelFlag = false;
							j = 6;
							break;
						}
					}
				}
				k += 2;
			}
		}
	} else {
		tx = dst.x;
		ty = dst.y;
		missile[mi].position.tile = { tx, ty };
		missile[mi].position.start = { tx, ty };
		missile[mi]._miDelFlag = false;
	}
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_TOWN && mx != mi && missile[mx]._misource == id)
			missile[mx]._mirange = 0;
	}
	PutMissile(mi);
	if (id == myplr && !missile[mi]._miDelFlag && currlevel != 0) {
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
			int dmg = GenerateRndSum(20, plr[id]._pLevel + 1) + plr[id]._pLevel + 1;
			missile[mi]._midam = ScaleSpellEffect(dmg, missile[mi]._mispllvl);
			missile[mi]._midam += missile[mi]._midam / 2;
			UseMana(id, SPL_FLASH);
		} else {
			missile[mi]._midam = monster[id].mLevel * 2;
		}
	} else {
		missile[mi]._midam = currlevel / 2;
	}
	missile[mi]._mirange = 19;
}

void AddFlash2(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	if (mienemy == TARGET_MONSTERS) {
		if (id != -1) {
			int dmg = GenerateRndSum(20, plr[id]._pLevel + 1) + plr[id]._pLevel + 1;
			missile[mi]._midam = ScaleSpellEffect(dmg, missile[mi]._mispllvl);
			missile[mi]._midam += missile[mi]._midam / 2;
		} else {
			missile[mi]._midam = currlevel / 2;
		}
	}
	missile[mi]._miPreFlag = true;
	missile[mi]._mirange = 19;
}

void AddManashield(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	missile[mi]._mirange = 48 * plr[id]._pLevel;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_MANASHIELD);
	if (id == myplr)
		NetSendCmd(true, CMD_SETSHIELD);
	plr[id].pManaShield = true;
}

void AddFiremove(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._midam = GenerateRnd(10) + plr[id]._pLevel + 1;
	GetMissileVel(mi, src, dst, 16);
	missile[mi]._mirange = 255;
	missile[mi]._miVar1 = 0;
	missile[mi]._miVar2 = 0;
	missile[mi].position.tile.x++;
	missile[mi].position.tile.y++;
	missile[mi].position.offset.y -= 32;
}

void AddGuardian(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int pn, k, j, tx, ty;

	int dmg = GenerateRnd(10) + (plr[id]._pLevel / 2) + 1;
	missile[mi]._midam = ScaleSpellEffect(dmg, missile[mi]._mispllvl);

	missile[mi]._miDelFlag = true;
	for (int i = 0; i < 6; i++) {
		pn = CrawlNum[i];
		k = pn + 2;
		for (j = (BYTE)CrawlTable[pn]; j > 0; j--) {
			tx = dst.x + CrawlTable[k - 1];
			ty = dst.y + CrawlTable[k];
			pn = dPiece[tx][ty];
			if (InDungeonBounds({ tx, ty })) {
				if (LineClearMissile(src, { tx, ty })) {
					if (dMonster[tx][ty] == 0 && !nSolidTable[pn] && !nMissileTable[pn] && dObject[tx][ty] == 0 && dMissile[tx][ty] == 0) {
						missile[mi].position.tile = { tx, ty };
						missile[mi].position.start = { tx, ty };
						missile[mi]._miDelFlag = false;
						UseMana(id, SPL_GUARDIAN);
						i = 6;
						break;
					}
				}
			}
			k += 2;
		}
	}

	if (!missile[mi]._miDelFlag) {
		missile[mi]._misource = id;
		missile[mi]._mlid = AddLight(missile[mi].position.tile, 1);
		missile[mi]._mirange = missile[mi]._mispllvl + (plr[id]._pLevel / 2);
		missile[mi]._mirange += (missile[mi]._mirange * plr[id]._pISplDur) / 128;

		if (missile[mi]._mirange > 30)
			missile[mi]._mirange = 30;
		missile[mi]._mirange <<= 4;
		if (missile[mi]._mirange < 30)
			missile[mi]._mirange = 30;

		missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
		missile[mi]._miVar2 = 0;
		missile[mi]._miVar3 = 1;
	}
}

void AddChain(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._miVar1 = dst.x;
	missile[mi]._miVar2 = dst.y;
	missile[mi]._mirange = 1;
	UseMana(id, SPL_CHAIN);
}

void AddBloodStar(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	SetMissDir(mi, dst.x);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = true;
	missile[mi]._mirange = 250;
}

void AddBone(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	if (dst.x > 3)
		dst.x = 2;
	SetMissDir(mi, dst.x);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = true;
	missile[mi]._mirange = 250;
}

void AddMetlHit(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	if (dst.x > 3)
		dst.x = 2;
	SetMissDir(mi, dst.x);
	missile[mi]._midam = 0;
	missile[mi]._miLightFlag = true;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

namespace {
void InitMissileAnimationFromMonster(MissileStruct &mis, int midir, const MonsterStruct &mon, int graphic)
{
	const AnimStruct &anim = mon.MType->Anims[graphic];
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
	int graphic = MA_SPECIAL;
	if (monster[id].MType->mtype < MT_HORNED || monster[id].MType->mtype > MT_OBLORD) {
		if (monster[id].MType->mtype < MT_NSNAKE || monster[id].MType->mtype > MT_GSNAKE) {
			graphic = MA_WALK;
		} else {
			graphic = MA_ATTACK;
		}
	}
	GetMissileVel(mi, src, dst, 18);
	InitMissileAnimationFromMonster(missile[mi], midir, monster[id], graphic);
	if (monster[id].MType->mtype >= MT_NSNAKE && monster[id].MType->mtype <= MT_GSNAKE)
		missile[mi]._miAnimFrame = 7;
	if (monster[id]._uniqtype != 0) {
		missile[mi]._miUniqTrans = monster[id]._uniqtrans + 1;
		missile[mi]._mlid = monster[id].mlid;
	}
	PutMissile(mi);
}

void AddFireman(int mi, Point src, Point dst, int midir, int8_t /*mienemy*/, int id, int /*dam*/)
{
	GetMissileVel(mi, src, dst, 16);
	auto &mon = monster[id];
	InitMissileAnimationFromMonster(missile[mi], midir, mon, MA_WALK);
	if (mon._uniqtype != 0)
		missile[mi]._miUniqTrans = mon._uniqtrans + 1;
	dMonster[mon.position.tile.x][mon.position.tile.y] = 0;
	PutMissile(mi);
}

void AddFlare(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	GetMissileVel(mi, src, dst, 16);
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._mlid = AddLight(src, 8);
	if (mienemy == TARGET_MONSTERS) {
		UseMana(id, SPL_FLARE);
		ApplyPlrDamage(id, 5);
	} else {
		if (id > 0) {
			if (monster[id].MType->mtype == MT_SUCCUBUS)
				SetMissAnim(mi, MFILE_FLARE);
			if (monster[id].MType->mtype == MT_SNOWWICH)
				SetMissAnim(mi, MFILE_SCUBMISB);
			if (monster[id].MType->mtype == MT_HLSPWN)
				SetMissAnim(mi, MFILE_SCUBMISD);
			if (monster[id].MType->mtype == MT_SOLBRNR)
				SetMissAnim(mi, MFILE_SCUBMISC);
		}
	}

	if (misfiledata[missile[mi]._miAnimType].mAnimFAmt == 16) {
		SetMissDir(mi, GetDirection16(src, dst));
	}
}

void AddAcid(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	GetMissileVel(mi, src, dst, 16);
	SetMissDir(mi, GetDirection16(src, dst));
	if ((!gbIsHellfire && (missile[mi].position.velocity.x & 0xFFFF0000) != 0) || (missile[mi].position.velocity.y & 0xFFFF0000) != 0)
		missile[mi]._mirange = 5 * (monster[id]._mint + 4);
	else
		missile[mi]._mirange = 1;
	missile[mi]._mlid = NO_LIGHT;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	PutMissile(mi);
}

void AddFireWallA(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int dam)
{
	missile[mi]._midam = dam;
	missile[mi].position.velocity = { 0, 0 };
	missile[mi]._mirange = 50;
	missile[mi]._miVar1 = missile[mi]._mirange - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
}

void AddAcidpud(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	int monst;

	missile[mi].position.velocity = { 0, 0 };
	missile[mi].position.offset = { 0, 0 };
	missile[mi]._miLightFlag = true;
	monst = missile[mi]._misource;
	missile[mi]._mirange = GenerateRnd(15) + 40 * (monster[monst]._mint + 1);
	missile[mi]._miPreFlag = true;
}

void AddStone(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int i, j, k, l, tx, ty, mid;

	missile[mi]._misource = id;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		l = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			tx = dst.x + CrawlTable[l - 1];
			ty = dst.y + CrawlTable[l];
			if (InDungeonBounds({ tx, ty })) {
				mid = dMonster[tx][ty];
				mid = mid > 0 ? mid - 1 : -(mid + 1);
				if (mid > MAX_PLRS - 1 && monster[mid]._mAi != AI_DIABLO && monster[mid].MType->mtype != MT_NAKRUL) {
					if (monster[mid]._mmode != MM_FADEIN && monster[mid]._mmode != MM_FADEOUT && monster[mid]._mmode != MM_CHARGE) {
						j = -99;
						i = 6;
						missile[mi]._miVar1 = monster[mid]._mmode;
						missile[mi]._miVar2 = mid;
						monster[mid].Petrify();
						break;
					}
				}
			}
			l += 2;
		}
	}

	if (j != -99) {
		missile[mi]._miDelFlag = true;
	} else {
		missile[mi].position.tile = { tx, ty };
		missile[mi].position.start = missile[mi].position.tile;
		missile[mi]._mirange = missile[mi]._mispllvl + 6;
		missile[mi]._mirange += (missile[mi]._mirange * plr[id]._pISplDur) / 128;

		if (missile[mi]._mirange > 15)
			missile[mi]._mirange = 15;
		missile[mi]._mirange <<= 4;
		UseMana(id, SPL_STONE);
	}
}

void AddGolem(int mi, Point src, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	int i;
	int mx;

	missile[mi]._miDelFlag = false;
	for (i = 0; i < nummissiles; i++) {
		mx = missileactive[i];
		if (missile[mx]._mitype == MIS_GOLEM) {
			if (mx != mi && missile[mx]._misource == id) {
				missile[mi]._miDelFlag = true;
				return;
			}
		}
	}
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._miVar4 = dst.x;
	missile[mi]._miVar5 = dst.y;
	if ((monster[id].position.tile.x != 1 || monster[id].position.tile.y != 0) && id == myplr)
		M_StartKill(id, id);
	UseMana(id, SPL_GOLEM);
}

void AddEtherealize(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	auto &player = plr[id];

	int range = 8 * player._pLevel;
	missile[mi]._mirange = ScaleSpellEffect(range, missile[mi]._mispllvl);

	missile[mi]._mirange += missile[mi]._mirange * player._pISplDur / 128;
	missile[mi]._miVar1 = player._pHitPoints;
	missile[mi]._miVar2 = player._pHPBase;
	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_ETHEREALIZE);
}

void AddDummy(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	missile[mi]._miDelFlag = true;
}

void AddBlodbur(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int dam)
{
	missile[mi]._midam = dam;
	missile[mi].position.tile = src;
	missile[mi].position.start = src;
	missile[mi]._misource = id;
	if (dam == 1)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, 1);
	missile[mi]._miLightFlag = true;
	missile[mi]._mirange = missile[mi]._miAnimLen;
}

void AddBoom(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int dam)
{
	missile[mi].position.tile = dst;
	missile[mi].position.start = dst;
	missile[mi].position.velocity = { 0, 0 };
	missile[mi]._midam = dam;
	missile[mi]._mirange = missile[mi]._miAnimLen;
	missile[mi]._miVar1 = 0;
}

void AddHeal(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &player = plr[id];

	int hp = (GenerateRnd(10) + 1) << 6;
	for (int i = 0; i < player._pLevel; i++) {
		hp += (GenerateRnd(4) + 1) << 6;
	}
	for (int i = 0; i < missile[mi]._mispllvl; i++) {
		hp += (GenerateRnd(6) + 1) << 6;
	}

	if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Barbarian || player._pClass == HeroClass::Monk) {
		hp *= 2;
	} else if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Bard) {
		hp += hp / 2;
	}

	player._pHitPoints = std::min(player._pHitPoints + hp, player._pMaxHP);
	player._pHPBase = std::min(player._pHPBase + hp, player._pMaxHPBase);

	UseMana(id, SPL_HEAL);
	missile[mi]._miDelFlag = true;
	drawhpflag = true;
}

void AddHealOther(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SPL_HEALOTHER);
	if (id == myplr) {
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

	int dmg = 2 * (plr[id]._pLevel + GenerateRndSum(10, 2)) + 4;
	missile[mi]._midam = ScaleSpellEffect(dmg, missile[mi]._mispllvl) / 2;

	GetMissileVel(mi, src, dst, 16);
	SetMissDir(mi, GetDirection(src, dst));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = dst.x;
	missile[mi]._miVar5 = dst.y;
	missile[mi]._mlid = AddLight(src, 8);
	UseMana(id, SPL_ELEMENT);
}

extern void FocusOnInventory();

void AddIdentify(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SPL_IDENTIFY);
	if (id == myplr) {
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
	int i, j, k, tx, ty, pn;

	missile[mi]._miDelFlag = true;
	for (i = 0; i < 6; i++) {
		k = CrawlNum[i];
		pn = k + 2;
		for (j = (BYTE)CrawlTable[k]; j > 0; j--) {
			tx = dst.x + CrawlTable[pn - 1];
			ty = dst.y + CrawlTable[pn];
			if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
				k = dPiece[tx][ty];
				if (LineClearMissile(src, { tx, ty })) {
					if (src != Point { tx, ty } && !nSolidTable[k] && dObject[tx][ty] == 0) {
						missile[mi]._miVar1 = tx;
						missile[mi]._miVar2 = ty;
						missile[mi]._miVar5 = tx;
						missile[mi]._miVar6 = ty;
						missile[mi]._miDelFlag = false;
						i = 6;
						break;
					}
				}
			}
			pn += 2;
		}
	}

	if (!missile[mi]._miDelFlag) {
		missile[mi]._miVar7 = 0;
		missile[mi].limitReached = false;
		missile[mi]._miVar3 = left[left[midir]];
		missile[mi]._miVar4 = right[right[midir]];
		missile[mi]._mirange = 7;
		UseMana(id, SPL_FIREWALL);
	}
}

void AddInfra(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	missile[mi]._mirange = ScaleSpellEffect(1584, missile[mi]._mispllvl);
	missile[mi]._mirange += missile[mi]._mirange * plr[id]._pISplDur / 128;

	if (mienemy == TARGET_MONSTERS)
		UseMana(id, SPL_INFRA);
}

void AddWave(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._miVar1 = dst.x;
	missile[mi]._miVar2 = dst.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = 0;
	missile[mi]._mirange = 1;
	missile[mi]._miAnimFrame = 4;
	UseMana(id, SPL_WAVE);
}

void AddNova(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t mienemy, int id, int /*dam*/)
{
	missile[mi]._miVar1 = dst.x;
	missile[mi]._miVar2 = dst.y;

	if (id != -1) {
		int dmg = GenerateRndSum(6, 5) + plr[id]._pLevel + 5;
		missile[mi]._midam = ScaleSpellEffect(dmg / 2, missile[mi]._mispllvl);

		if (mienemy == TARGET_MONSTERS)
			UseMana(id, SPL_NOVA);
	} else {
		missile[mi]._midam = ((DWORD)currlevel / 2) + GenerateRndSum(3, 3);
	}

	missile[mi]._mirange = 1;
}

void AddBlodboil(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	auto &player = plr[id];

	if ((player._pSpellFlags & 6) != 0 || player._pHitPoints <= player._pLevel << 6) {
		missile[mi]._miDelFlag = true;
		return;
	}

	UseMana(id, SPL_BLODBOIL);
	missile[mi]._miVar1 = id;
	int tmp = 3 * player._pLevel;
	tmp <<= 7;
	player._pSpellFlags |= 2;
	missile[mi]._miVar2 = tmp;
	int lvl = player._pLevel * 2;
	missile[mi]._mirange = lvl + 10 * missile[mi]._mispllvl + 245;
	CalcPlrItemVals(id, true);
	force_redraw = 255;
	player.Say(HeroSpeech::Aaaaargh);
}

void AddRepair(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SPL_REPAIR);
	if (id == myplr) {
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
	missile[mi]._miDelFlag = true;
	UseMana(id, SPL_RECHARGE);
	if (id == myplr) {
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
	missile[mi]._miDelFlag = true;
	UseMana(id, SPL_DISARM);
	if (id == myplr) {
		NewCursor(CURSOR_DISARM);
		if (sgbControllerActive) {
			if (pcursobj != -1)
				NetSendCmdLocParam1(true, CMD_DISARMXY, cursPosition, pcursobj);
			else
				NewCursor(CURSOR_HAND);
		}
	}
}

void AddApoca(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._miVar1 = 8;
	missile[mi]._miVar2 = src.y - missile[mi]._miVar1;
	missile[mi]._miVar3 = missile[mi]._miVar1 + src.y;
	missile[mi]._miVar4 = src.x - missile[mi]._miVar1;
	missile[mi]._miVar5 = missile[mi]._miVar1 + src.x;
	missile[mi]._miVar6 = missile[mi]._miVar4;
	if (missile[mi]._miVar2 <= 0)
		missile[mi]._miVar2 = 1;
	if (missile[mi]._miVar3 >= MAXDUNY)
		missile[mi]._miVar3 = MAXDUNY - 1;
	if (missile[mi]._miVar4 <= 0)
		missile[mi]._miVar4 = 1;
	if (missile[mi]._miVar5 >= MAXDUNX)
		missile[mi]._miVar5 = MAXDUNX - 1;
	missile[mi]._midam = GenerateRndSum(6, plr[id]._pLevel) + plr[id]._pLevel;
	missile[mi]._mirange = 255;
	missile[mi]._miDelFlag = false;
	UseMana(id, SPL_APOCA);
}

void AddFlame(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int dam)
{
	int i;

	missile[mi]._miVar2 = 0;
	for (i = dam; i > 0; i--) {
		missile[mi]._miVar2 += 5;
	}
	missile[mi].position.start = dst;
	missile[mi].position.offset = missile[midir].position.offset;
	missile[mi].position.traveled = missile[midir].position.traveled;
	missile[mi]._mirange = missile[mi]._miVar2 + 20;
	missile[mi]._mlid = AddLight(src, 1);
	if (mienemy == TARGET_MONSTERS) {
		i = GenerateRnd(plr[id]._pLevel) + GenerateRnd(2);
		missile[mi]._midam = 8 * i + 16 + ((8 * i + 16) / 2);
	} else {
		missile[mi]._midam = monster[id].mMinDamage + GenerateRnd(monster[id].mMaxDamage - monster[id].mMinDamage + 1);
	}
}

void AddFlamec(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	GetMissileVel(mi, src, dst, 32);
	if (mienemy == TARGET_MONSTERS) {
		UseMana(id, SPL_FLAME);
	}
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
}

void AddCbolt(int mi, Point src, Point dst, int midir, int8_t micaster, int id, int /*dam*/)
{
	assert((DWORD)mi < MAXMISSILES);

	missile[mi]._mirnd = GenerateRnd(15) + 1;
	missile[mi]._midam = (micaster == 0) ? (GenerateRnd(plr[id]._pMagic / 4) + 1) : 15;

	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	missile[mi]._miAnimFrame = GenerateRnd(8) + 1;
	missile[mi]._mlid = AddLight(src, 5);

	GetMissileVel(mi, src, dst, 8);
	missile[mi]._miVar1 = 5;
	missile[mi]._miVar2 = midir;
	missile[mi]._miVar3 = 0;
	missile[mi]._mirange = 256;
}

void AddHbolt(int mi, Point src, Point dst, int midir, int8_t /*micaster*/, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}

	int sp = 16;
	if (id != -1) {
		sp += std::min(missile[mi]._mispllvl * 2, 47);
	}

	GetMissileVel(mi, src, dst, sp);
	SetMissDir(mi, GetDirection16(src, dst));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._mlid = AddLight(src, 8);
	missile[mi]._midam = GenerateRnd(10) + plr[id]._pLevel + 9;
	UseMana(id, SPL_HBOLT);
}

void AddResurrect(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	UseMana(id, SPL_RESURRECT);
	if (id == myplr) {
		NewCursor(CURSOR_RESURRECT);
		if (sgbControllerActive)
			TryIconCurs();
	}
	missile[mi]._miDelFlag = true;
}

void AddResurrectBeam(int mi, Point /*src*/, Point dst, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	missile[mi].position.tile = dst;
	missile[mi].position.start = missile[mi].position.tile;
	missile[mi].position.velocity = { 0, 0 };
	missile[mi]._mirange = misfiledata[MFILE_RESSUR1].mAnimLen[0];
}

void AddTelekinesis(int mi, Point /*src*/, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int id, int /*dam*/)
{
	missile[mi]._miDelFlag = true;
	UseMana(id, SPL_TELEKINESIS);
	if (id == myplr)
		NewCursor(CURSOR_TELEKINESIS);
}

void AddBoneSpirit(int mi, Point src, Point dst, int midir, int8_t mienemy, int id, int /*dam*/)
{
	if (src == dst) {
		dst += static_cast<Direction>(midir);
	}
	missile[mi]._midam = 0;
	GetMissileVel(mi, src, dst, 16);
	SetMissDir(mi, GetDirection(src, dst));
	missile[mi]._mirange = 256;
	missile[mi]._miVar1 = src.x;
	missile[mi]._miVar2 = src.y;
	missile[mi]._miVar3 = 0;
	missile[mi]._miVar4 = dst.x;
	missile[mi]._miVar5 = dst.y;
	missile[mi]._mlid = AddLight(src, 8);
	if (mienemy == TARGET_MONSTERS) {
		UseMana(id, SPL_BONESPIRIT);
		ApplyPlrDamage(id, 6);
	}
}

void AddRportal(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t /*mienemy*/, int /*id*/, int /*dam*/)
{
	missile[mi].position.tile = src;
	missile[mi].position.start = src;
	missile[mi]._mirange = 100;
	missile[mi]._miVar1 = 100 - missile[mi]._miAnimLen;
	missile[mi]._miVar2 = 0;
	PutMissile(mi);
}

void AddDiabApoca(int mi, Point src, Point /*dst*/, int /*midir*/, int8_t mienemy, int id, int dam)
{
	int players = gbIsMultiplayer ? MAX_PLRS : 1;
	for (int pnum = 0; pnum < players; pnum++) {
		auto &player = plr[pnum];
		if (!player.plractive)
			continue;
		if (!LineClearMissile(src, player.position.future))
			continue;

		AddMissile({ 0, 0 }, player.position.future, 0, MIS_BOOM2, mienemy, id, dam, 0);
	}
	missile[mi]._miDelFlag = true;
}

int AddMissile(Point src, Point dst, int midir, int mitype, int8_t micaster, int id, int midam, int spllvl)
{
	if (nummissiles >= MAXMISSILES - 1)
		return -1;

	if (mitype == MIS_MANASHIELD && plr[id].pManaShield) {
		if (currlevel != plr[id].plrlevel)
			return -1;

		for (int i = 0; i < nummissiles; i++) {
			int mi = missileactive[i];
			if (missile[mi]._mitype == MIS_MANASHIELD && missile[mi]._misource == id)
				return -1;
		}
	}

	int mi = missileavail[0];

	missileavail[0] = missileavail[MAXMISSILES - nummissiles - 1];
	missileactive[nummissiles] = mi;
	nummissiles++;

	memset(&missile[mi], 0, sizeof(*missile));

	missile[mi]._mitype = mitype;
	missile[mi]._micaster = micaster;
	missile[mi]._misource = id;
	missile[mi]._miAnimType = missiledata[mitype].mFileNum;
	missile[mi]._miDrawFlag = missiledata[mitype].mDraw;
	missile[mi]._mispllvl = spllvl;
	missile[mi]._mimfnum = midir;

	if (missile[mi]._miAnimType == MFILE_NONE || misfiledata[missile[mi]._miAnimType].mAnimFAmt < 8)
		SetMissDir(mi, 0);
	else
		SetMissDir(mi, midir);

	missile[mi].position.tile = src;
	missile[mi].position.offset = { 0, 0 };
	missile[mi].position.start = src;
	missile[mi].position.traveled = { 0, 0 };
	missile[mi]._miDelFlag = false;
	missile[mi]._miAnimAdd = 1;
	missile[mi]._miLightFlag = false;
	missile[mi]._miPreFlag = false;
	missile[mi]._miUniqTrans = 0;
	missile[mi]._midam = midam;
	missile[mi]._miHitFlag = false;
	missile[mi]._midist = 0;
	missile[mi]._mlid = NO_LIGHT;
	missile[mi]._mirnd = 0;

	if (missiledata[mitype].mlSFX != -1) {
		PlaySfxLoc(missiledata[mitype].mlSFX, missile[mi].position.start);
	}

	missiledata[mitype].mAddProc(mi, src, dst, midir, micaster, id, midam);

	return mi;
}

static int Sentfire(int i, Point src)
{
	int ex = 0;
	if (LineClearMissile(missile[i].position.tile, src)) {
		if (dMonster[src.x][src.y] > 0 && monster[dMonster[src.x][src.y] - 1]._mhitpoints >> 6 > 0 && dMonster[src.x][src.y] - 1 > MAX_PLRS - 1) {
			Direction dir = GetDirection(missile[i].position.tile, src);
			missile[i]._miVar3 = missileavail[0];
			AddMissile(missile[i].position.tile, src, dir, MIS_FIREBOLT, TARGET_MONSTERS, missile[i]._misource, missile[i]._midam, GetSpellLevel(missile[i]._misource, SPL_FIREBOLT));
			ex = -1;
		}
	}
	if (ex == -1) {
		SetMissDir(i, 2);
		missile[i]._miVar2 = 3;
	}

	return ex;
}

void MI_Dummy(int i)
{
}

void MI_Golem(int i)
{
	int tx, ty, dp, l, m, src, k, tid;
	const char *ct;

	src = missile[i]._misource;
	if (monster[src].position.tile.x == 1 && monster[src].position.tile.y == 0) {
		for (l = 0; l < 6; l++) {
			k = CrawlNum[l];
			tid = k + 2;
			for (m = (BYTE)CrawlTable[k]; m > 0; m--) {
				ct = &CrawlTable[tid];
				tx = missile[i]._miVar4 + *(ct - 1);
				ty = missile[i]._miVar5 + *ct;
				if (0 < tx && tx < MAXDUNX && 0 < ty && ty < MAXDUNY) {
					dp = dPiece[tx][ty];
					if (LineClearMissile({ missile[i]._miVar1, missile[i]._miVar2 }, { tx, ty })) {
						if (dMonster[tx][ty] == 0 && !nSolidTable[dp] && dObject[tx][ty] == 0) {
							l = 6;
							SpawnGolum(src, { tx, ty }, i);
							break;
						}
					}
				}
				tid += 2;
			}
		}
	}
	missile[i]._miDelFlag = true;
}

void MI_LArrow(int i)
{
	int p, mind, maxd;

	missile[i]._mirange--;
	p = missile[i]._misource;
	if (missile[i]._miAnimType == MFILE_MINILTNG || missile[i]._miAnimType == MFILE_MAGBLOS) {
		ChangeLight(missile[i]._mlid, missile[i].position.tile, missile[i]._miAnimFrame + 5);
		missile_resistance rst = missiledata[missile[i]._mitype].mResist;
		if (missile[i]._mitype == MIS_LARROW) {
			if (p != -1) {
				mind = plr[p]._pILMinDam;
				maxd = plr[p]._pILMaxDam;
			} else {
				mind = GenerateRnd(10) + 1 + currlevel;
				maxd = GenerateRnd(10) + 1 + currlevel * 2;
			}
			missiledata[MIS_LARROW].mResist = MISR_LIGHTNING;
			CheckMissileCol(i, mind, maxd, false, missile[i].position.tile, true);
		}
		if (missile[i]._mitype == MIS_FARROW) {
			if (p != -1) {
				mind = plr[p]._pIFMinDam;
				maxd = plr[p]._pIFMaxDam;
			} else {
				mind = GenerateRnd(10) + 1 + currlevel;
				maxd = GenerateRnd(10) + 1 + currlevel * 2;
			}
			missiledata[MIS_FARROW].mResist = MISR_FIRE;
			CheckMissileCol(i, mind, maxd, false, missile[i].position.tile, true);
		}
		missiledata[missile[i]._mitype].mResist = rst;
	} else {
		missile[i]._midist++;
		missile[i].position.traveled += missile[i].position.velocity;
		GetMissilePos(i);

		if (p != -1) {
			if (missile[i]._micaster == TARGET_MONSTERS) {
				mind = plr[p]._pIMinDam;
				maxd = plr[p]._pIMaxDam;
			} else {
				mind = monster[p].mMinDamage;
				maxd = monster[p].mMaxDamage;
			}
		} else {
			mind = GenerateRnd(10) + 1 + currlevel;
			maxd = GenerateRnd(10) + 1 + currlevel * 2;
		}

		if (missile[i].position.tile != missile[i].position.start) {
			missile_resistance rst = missiledata[missile[i]._mitype].mResist;
			missiledata[missile[i]._mitype].mResist = MISR_NONE;
			CheckMissileCol(i, mind, maxd, false, missile[i].position.tile, false);
			missiledata[missile[i]._mitype].mResist = rst;
		}
		if (missile[i]._mirange == 0) {
			missile[i]._mimfnum = 0;
			missile[i].position.traveled.x -= missile[i].position.velocity.x;
			missile[i].position.traveled.y -= missile[i].position.velocity.y;
			GetMissilePos(i);
			if (missile[i]._mitype == MIS_LARROW)
				SetMissAnim(i, MFILE_MINILTNG);
			else
				SetMissAnim(i, MFILE_MAGBLOS);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if (missile[i].position.tile.x != missile[i]._miVar1 || missile[i].position.tile.y != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i].position.tile.x;
				missile[i]._miVar2 = missile[i].position.tile.y;
				ChangeLight(missile[i]._mlid, { missile[i]._miVar1, missile[i]._miVar2 }, 5);
			}
		}
	}
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Arrow(int i)
{
	int p, mind, maxd;

	missile[i]._mirange--;
	missile[i]._midist++;
	missile[i].position.traveled += missile[i].position.velocity;
	GetMissilePos(i);
	p = missile[i]._misource;
	if (p != -1) {
		if (missile[i]._micaster == TARGET_MONSTERS) {
			mind = plr[p]._pIMinDam;
			maxd = plr[p]._pIMaxDam;
		} else {
			mind = monster[p].mMinDamage;
			maxd = monster[p].mMaxDamage;
		}
	} else {
		mind = currlevel;
		maxd = 2 * currlevel;
	}
	if (missile[i].position.tile != missile[i].position.start)
		CheckMissileCol(i, mind, maxd, false, missile[i].position.tile, false);
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Firebolt(int i)
{
	int omx, omy;
	int d, p;

	missile[i]._mirange--;
	if (missile[i]._mitype != MIS_BONESPIRIT || missile[i]._mimfnum != 8) {
		omx = missile[i].position.traveled.x;
		omy = missile[i].position.traveled.y;
		missile[i].position.traveled += missile[i].position.velocity;
		GetMissilePos(i);
		p = missile[i]._misource;
		if (p != -1) {
			if (missile[i]._micaster == TARGET_MONSTERS) {
				switch (missile[i]._mitype) {
				case MIS_FIREBOLT:
					d = GenerateRnd(10) + (plr[p]._pMagic / 8) + missile[i]._mispllvl + 1;
					break;
				case MIS_FLARE:
					d = 3 * missile[i]._mispllvl - (plr[p]._pMagic / 8) + (plr[p]._pMagic / 2);
					break;
				case MIS_BONESPIRIT:
					d = 0;
					break;
				}
			} else {
				d = monster[p].mMinDamage + GenerateRnd(monster[p].mMaxDamage - monster[p].mMinDamage + 1);
			}
		} else {
			d = currlevel + GenerateRnd(2 * currlevel);
		}
		if (missile[i].position.tile != missile[i].position.start) {
			CheckMissileCol(i, d, d, false, missile[i].position.tile, false);
		}
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = true;
			missile[i].position.traveled = { omx, omy };
			GetMissilePos(i);
			switch (missile[i]._mitype) {
			case MIS_FIREBOLT:
			case MIS_MAGMABALL:
				AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_MISEXP, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_FLARE:
				AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_MISEXP2, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_ACID:
				AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_MISEXP3, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_BONESPIRIT:
				SetMissDir(i, DIR_OMNI);
				missile[i]._mirange = 7;
				missile[i]._miDelFlag = false;
				PutMissile(i);
				return;
			case MIS_LICH:
				AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_EXORA1, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_PSYCHORB:
				AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_EXBL2, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_NECROMORB:
				AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_EXRED3, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_ARCHLICH:
				AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_EXYEL2, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			case MIS_BONEDEMON:
				AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_EXBL3, missile[i]._micaster, missile[i]._misource, 0, 0);
				break;
			}
			if (missile[i]._mlid != NO_LIGHT)
				AddUnLight(missile[i]._mlid);
			PutMissile(i);
		} else {
			if (missile[i].position.tile.x != missile[i]._miVar1 || missile[i].position.tile.y != missile[i]._miVar2) {
				missile[i]._miVar1 = missile[i].position.tile.x;
				missile[i]._miVar2 = missile[i].position.tile.y;
				if (missile[i]._mlid != NO_LIGHT)
					ChangeLight(missile[i]._mlid, { missile[i]._miVar1, missile[i]._miVar2 }, 8);
			}
			PutMissile(i);
		}
	} else if (missile[i]._mirange == 0) {
		if (missile[i]._mlid != NO_LIGHT)
			AddUnLight(missile[i]._mlid);
		missile[i]._miDelFlag = true;
		PlaySfxLoc(LS_BSIMPCT, missile[i].position.tile);
		PutMissile(i);
	} else
		PutMissile(i);
}

void MI_Lightball(int i)
{
	int tx, ty, j, oi;
	char obj;

	tx = missile[i]._miVar1;
	ty = missile[i]._miVar2;
	missile[i]._mirange--;
	missile[i].position.traveled += missile[i].position.velocity;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i].position.tile, false);
	if (missile[i]._miHitFlag)
		missile[i]._mirange = j;
	obj = dObject[tx][ty];
	if (obj != 0 && tx == missile[i].position.tile.x && ty == missile[i].position.tile.y) {
		if (obj > 0) {
			oi = obj - 1;
		} else {
			oi = -(obj + 1);
		}
		if (object[oi]._otype == OBJ_SHRINEL || object[oi]._otype == OBJ_SHRINER)
			missile[i]._mirange = j;
	}
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Krull(int i)
{
	missile[i]._mirange--;
	missile[i].position.traveled += missile[i].position.velocity;
	GetMissilePos(i);
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i].position.tile, false);
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Acidpud(int i)
{
	int range;

	missile[i]._mirange--;
	range = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i].position.tile, false);
	missile[i]._mirange = range;
	if (range == 0) {
		if (missile[i]._mimfnum != 0) {
			missile[i]._miDelFlag = true;
		} else {
			SetMissDir(i, 1);
			missile[i]._mirange = missile[i]._miAnimLen;
		}
	}
	PutMissile(i);
}

void MI_Firewall(int i)
{
	constexpr int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = GenerateRnd(11) + 1;
	}
	if (missile[i]._mirange == missile[i]._miAnimLen - 1) {
		SetMissDir(i, 0);
		missile[i]._miAnimFrame = 13;
		missile[i]._miAnimAdd = -1;
	}
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i].position.tile, true);
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._mimfnum != 0 && missile[i]._mirange != 0 && missile[i]._miAnimAdd != -1 && missile[i]._miVar2 < 12) {
		if (missile[i]._miVar2 == 0)
			missile[i]._mlid = AddLight(missile[i].position.tile, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i].position.tile, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	PutMissile(i);
}

static void FireballUpdate(int i, Point offset, bool alwaysDelete)
{
	missile[i]._mirange--;

	int id = missile[i]._misource;
	Point p = (missile[i]._micaster == TARGET_MONSTERS) ? plr[id].position.tile : monster[id].position.tile;

	if (missile[i]._miAnimType == MFILE_BIGEXP) {
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = true;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		int dam = missile[i]._midam;
		missile[i].position.traveled += offset;
		GetMissilePos(i);
		if (missile[i].position.tile != missile[i].position.start)
			CheckMissileCol(i, dam, dam, false, missile[i].position.tile, false);
		if (missile[i]._mirange == 0) {
			Point m = missile[i].position.tile;
			ChangeLight(missile[i]._mlid, missile[i].position.tile, missile[i]._miAnimFrame);

			constexpr Point Offsets[] = { { 0, 0 }, { 0, 1 }, { 0, -1 }, { 1, 0 }, { 1, -1 }, { 1, 1 }, { -1, 0 }, { -1, 1 }, { -1, -1 } };
			for (Point offset : Offsets) {
				if (!CheckBlock(p, m + offset))
					CheckMissileCol(i, dam, dam, false, m + offset, true);
			}

			if (!TransList[dTransVal[m.x][m.y]]
			    || (missile[i].position.velocity.x < 0 && ((TransList[dTransVal[m.x][m.y + 1]] && nSolidTable[dPiece[m.x][m.y + 1]]) || (TransList[dTransVal[m.x][m.y - 1]] && nSolidTable[dPiece[m.x][m.y - 1]])))) {
				missile[i].position.tile.x++;
				missile[i].position.tile.y++;
				missile[i].position.offset.y -= 32;
			}
			if (missile[i].position.velocity.y > 0
			    && ((TransList[dTransVal[m.x + 1][m.y]] && nSolidTable[dPiece[m.x + 1][m.y]])
			        || (TransList[dTransVal[m.x - 1][m.y]] && nSolidTable[dPiece[m.x - 1][m.y]]))) {
				missile[i].position.offset.y -= 32;
			}
			if (missile[i].position.velocity.x > 0
			    && ((TransList[dTransVal[m.x][m.y + 1]] && nSolidTable[dPiece[m.x][m.y + 1]])
			        || (TransList[dTransVal[m.x][m.y - 1]] && nSolidTable[dPiece[m.x][m.y - 1]]))) {
				missile[i].position.offset.x -= 32;
			}
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else if (missile[i].position.tile.x != missile[i]._miVar1 || missile[i].position.tile.y != missile[i]._miVar2) {
			missile[i]._miVar1 = missile[i].position.tile.x;
			missile[i]._miVar2 = missile[i].position.tile.y;
			ChangeLight(missile[i]._mlid, missile[i].position.tile, 8);
		}
		if (alwaysDelete)
			missile[i]._miDelFlag = true;
	}

	PutMissile(i);
}

void MI_Fireball(int i)
{
	FireballUpdate(i, missile[i].position.velocity, false);
}

void MI_HorkSpawn(int i)
{
	int t, j, k, tx, ty, dp;

	missile[i]._mirange--;
	CheckMissileCol(i, 0, 0, false, missile[i].position.tile, false);
	if (missile[i]._mirange <= 0) {
		missile[i]._miDelFlag = true;
		for (j = 0; j < 2; j++) {
			k = CrawlNum[j] + 2;
			for (t = CrawlTable[CrawlNum[j]]; t > 0; t--, k += 2) {
				tx = missile[i].position.tile.x + CrawlTable[k - 1];
				ty = missile[i].position.tile.y + CrawlTable[k];
				if (InDungeonBounds({ tx, ty })) {
					dp = dPiece[tx][ty];
					if (!nSolidTable[dp] && dMonster[tx][ty] == 0 && dPlayer[tx][ty] == 0 && dObject[tx][ty] == 0) {
						j = 6;
						auto md = static_cast<Direction>(missile[i]._miVar1);
						int mon = AddMonster({ tx, ty }, md, 1, true);
						M_StartStand(mon, md);
						break;
					}
				}
			}
		}
	} else {
		missile[i]._midist++;
		missile[i].position.traveled += missile[i].position.velocity;
		GetMissilePos(i);
	}
	PutMissile(i);
}

void MI_Rune(int i)
{
	int mid, pid, mx, my;
	Direction dir;

	mx = missile[i].position.tile.x;
	my = missile[i].position.tile.y;
	mid = dMonster[mx][my];
	pid = dPlayer[mx][my];
	if (mid != 0 || pid != 0) {
		if (mid != 0) {
			if (mid > 0)
				mid = mid - 1;
			else
				mid = -(mid + 1);
			dir = GetDirection(missile[i].position.tile, monster[mid].position.tile);
		} else {
			if (pid > 0)
				pid = pid - 1;
			else
				pid = -(pid + 1);
			dir = GetDirection(missile[i].position.tile, plr[pid].position.tile);
		}
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
		AddMissile({ mx, my }, { mx, my }, dir, missile[i]._miVar1, TARGET_BOTH, missile[i]._misource, missile[i]._midam, missile[i]._mispllvl);
	}
	PutMissile(i);
}

void MI_LightningWall(int i)
{
	int range;

	missile[i]._mirange--;
	range = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i].position.tile, false);
	if (missile[i]._miHitFlag)
		missile[i]._mirange = range;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_HiveExplode(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange <= 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Immolation(int i)
{
	if (missile[i]._miVar7 < 0) {
		int v = 2 * missile[i]._miVar6;
		missile[i]._miVar6 = v;
		missile[i]._miVar7 = v;
		missile[i]._mimfnum = left[missile[i]._mimfnum];
	} else {
		missile[i]._miVar7--;
	}

	Point offset = missile[i].position.velocity;

	switch (missile[i]._mimfnum) {
	case DIR_S:
	case DIR_N:
		offset.y = 0;
		break;
	case DIR_W:
	case DIR_E:
		offset.x = 0;
		break;
	default:
		break;
	}

	FireballUpdate(i, offset, true);
}

void MI_LightningArrow(int i)
{
	int pn, mx, my;

	missile[i]._mirange--;
	missile[i].position.traveled += missile[i].position.velocity;
	GetMissilePos(i);

	mx = missile[i].position.tile.x;
	my = missile[i].position.tile.y;
	assert((DWORD)mx < MAXDUNX);
	assert((DWORD)my < MAXDUNY);
	pn = dPiece[mx][my];
	assert((DWORD)pn <= MAXTILES);

	if (missile[i]._misource == -1) {
		if ((mx != missile[i].position.start.x || my != missile[i].position.start.y) && nMissileTable[pn]) {
			missile[i]._mirange = 0;
		}
	} else if (nMissileTable[pn]) {
		missile[i]._mirange = 0;
	}

	if (!nMissileTable[pn]) {
		if ((mx != missile[i]._miVar1 || my != missile[i]._miVar2) && mx > 0 && my > 0 && mx < MAXDUNX && my < MAXDUNY) {
			if (missile[i]._misource != -1) {
				if (missile[i]._micaster == TARGET_PLAYERS
				    && monster[missile[i]._misource].MType->mtype >= MT_STORM
				    && monster[missile[i]._misource].MType->mtype <= MT_MAEL) {
					AddMissile(
					    missile[i].position.tile,
					    missile[i].position.start,
					    i,
					    MIS_LIGHTNING2,
					    missile[i]._micaster,
					    missile[i]._misource,
					    missile[i]._midam,
					    missile[i]._mispllvl);
				} else {
					AddMissile(
					    missile[i].position.tile,
					    missile[i].position.start,
					    i,
					    MIS_LIGHTNING,
					    missile[i]._micaster,
					    missile[i]._misource,
					    missile[i]._midam,
					    missile[i]._mispllvl);
				}
			} else {
				AddMissile(
				    missile[i].position.tile,
				    missile[i].position.start,
				    i,
				    MIS_LIGHTNING,
				    missile[i]._micaster,
				    missile[i]._misource,
				    missile[i]._midam,
				    missile[i]._mispllvl);
			}
			missile[i]._miVar1 = missile[i].position.tile.x;
			missile[i]._miVar2 = missile[i].position.tile.y;
		}
	}

	if (missile[i]._mirange == 0 || mx <= 0 || my <= 0 || mx >= MAXDUNX || my > MAXDUNY) { // BUGFIX my >= MAXDUNY
		missile[i]._miDelFlag = true;
	}
}

void MI_FlashFront(int i)
{
	int src = missile[i]._misource;
	if (missile[i]._micaster == TARGET_MONSTERS && src != -1) {
		missile[i].position.tile = plr[src].position.tile;
		missile[i].position.traveled.x = plr[src].position.offset.x << 16;
		missile[i].position.traveled.y = plr[src].position.offset.y << 16;
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		if (missile[i]._micaster == TARGET_MONSTERS) {
			src = missile[i]._misource;
			if (src != -1)
				plr[src]._pBaseToBlk -= 50;
		}
	}
	PutMissile(i);
}

void MI_FlashBack(int i)
{
	if (missile[i]._micaster == TARGET_MONSTERS) {
		if (missile[i]._misource != -1) {
			missile[i].position.tile = plr[missile[i]._misource].position.future;
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Reflect(int i)
{
	int src = missile[i]._misource;
	if (src != myplr && currlevel != plr[src].plrlevel)
		missile[i]._miDelFlag = true;
	if (plr[src].wReflections <= 0) {
		missile[i]._miDelFlag = true;
		NetSendCmd(true, CMD_REFLECT);
	}
	PutMissile(i);
}

static void MI_Ring(int i, int type)
{
	int b = CrawlNum[3];
	missile[i]._miDelFlag = true;
	int src = missile[i]._micaster;
	int k = CrawlNum[3] + 1;
	uint8_t lvl = src > 0 ? plr[src]._pLevel : currlevel;
	int dmg = 16 * (GenerateRndSum(10, 2) + lvl + 2) / 2;

	for (int j = CrawlTable[b]; j > 0; j--, k += 2) {
		int tx = missile[i]._miVar1 + CrawlTable[k - 1];
		int ty = missile[i]._miVar2 + CrawlTable[k];
		if (!InDungeonBounds({ tx, ty }))
			continue;
		int dp = dPiece[tx][ty];
		if (nSolidTable[dp])
			continue;
		if (dObject[tx][ty] != 0)
			continue;
		if (!LineClearMissile(missile[i].position.tile, { tx, ty }))
			continue;
		if (nMissileTable[dp] || missile[i].limitReached) {
			missile[i].limitReached = true;
			continue;
		}

		AddMissile({ tx, ty }, { tx, ty }, 0, type, TARGET_BOTH, src, dmg, missile[i]._mispllvl);
	}
}

void MI_FireRing(int i)
{
	MI_Ring(i, MIS_FIREWALL);
}

void MI_LightningRing(int i)
{
	MI_Ring(i, MIS_LIGHTWALL);
}

void MI_Search(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange != 0)
		return;

	missile[i]._miDelFlag = true;
	PlaySfxLoc(IS_CAST7, plr[missile[i]._miVar1].position.tile);
	AutoMapShowItems = false;
}

bool GrowWall(int playerId, Point position, Point target, missile_id type, int spellLevel, int damage)
{
	int dp = dPiece[position.x][position.y];
	assert(dp <= MAXTILES && dp >= 0);

	if (nMissileTable[dp] || !InDungeonBounds(target)) {
		return false;
	}

	AddMissile(position, position, plr[playerId]._pdir, type, TARGET_BOTH, playerId, damage, spellLevel);
	return true;
}

void MI_LightningWallC(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		return;
	}

	int id = missile[i]._misource;
	int lvl = (id > -1) ? plr[id]._pLevel : 0;
	int dmg = 16 * (GenerateRndSum(10, 2) + lvl + 2);

	{
		Point position = { missile[i]._miVar1, missile[i]._miVar2 };
		Point target = position + static_cast<Direction>(missile[i]._miVar3);

		if (!missile[i].limitReached && GrowWall(id, position, target, MIS_LIGHTWALL, missile[i]._mispllvl, dmg)) {
			missile[i]._miVar1 = target.x;
			missile[i]._miVar2 = target.y;
		} else {
			missile[i].limitReached = true;
		}
	}

	{
		Point position = { missile[i]._miVar5, missile[i]._miVar6 };
		Point target = position + static_cast<Direction>(missile[i]._miVar4);

		if (missile[i]._miVar7 == 0 && GrowWall(id, position, target, MIS_LIGHTWALL, missile[i]._mispllvl, dmg)) {
			missile[i]._miVar5 = target.x;
			missile[i]._miVar6 = target.y;
		} else {
			missile[i]._miVar7 = 1;
		}
	}
}

void MI_FireNova(int i)
{
	int sx1 = 0;
	int sy1 = 0;
	int id = missile[i]._misource;
	int dam = missile[i]._midam;
	Point src = missile[i].position.tile;
	Direction dir = DIR_S;
	mienemy_type en = TARGET_PLAYERS;
	if (id != -1) {
		dir = plr[id]._pdir;
		en = TARGET_MONSTERS;
	}
	for (const auto &k : vCrawlTable) {
		if (sx1 != k[6] || sy1 != k[7]) {
			Point offsets[] = { { k[6], k[7] }, { -k[6], -k[7] }, { -k[6], +k[7] }, { +k[6], -k[7] } };
			for (Point offset : offsets)
				AddMissile(src, src + offset, dir, MIS_FIRENOVA, en, id, dam, missile[i]._mispllvl);
			sx1 = k[6];
			sy1 = k[7];
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
}

void MI_SpecArrow(int i)
{
	int id = missile[i]._misource;
	int dam = missile[i]._midam;
	Point src = missile[i].position.tile;
	Point dst = { missile[i]._miVar1, missile[i]._miVar2 };
	int spllvl = missile[i]._miVar3;
	int mitype = 0;
	Direction dir = DIR_S;
	mienemy_type micaster = TARGET_PLAYERS;
	if (id != -1) {
		dir = plr[id]._pdir;
		micaster = TARGET_MONSTERS;

		switch (plr[id]._pILMinDam) {
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
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
}

void MI_Lightctrl(int i)
{
	int dam;

	assert((DWORD)i < MAXMISSILES);
	missile[i]._mirange--;

	int id = missile[i]._misource;
	if (id != -1) {
		if (missile[i]._micaster == TARGET_MONSTERS) {
			dam = (GenerateRnd(2) + GenerateRnd(plr[id]._pLevel) + 2) << 6;
		} else {
			dam = 2 * (monster[id].mMinDamage + GenerateRnd(monster[id].mMaxDamage - monster[id].mMinDamage + 1));
		}
	} else {
		dam = GenerateRnd(currlevel) + 2 * currlevel;
	}

	missile[i].position.traveled += missile[i].position.velocity;
	GetMissilePos(i);

	int mx = missile[i].position.tile.x;
	int my = missile[i].position.tile.y;
	assert((DWORD)mx < MAXDUNX);
	assert((DWORD)my < MAXDUNY);
	int pn = dPiece[mx][my];
	assert((DWORD)pn <= MAXTILES);

	if (id != -1 || Point { mx, my } != missile[i].position.start) {
		if (nMissileTable[pn]) {
			missile[i]._mirange = 0;
		}
	}
	if (!nMissileTable[pn]
	    && Point { mx, my } != Point { missile[i]._miVar1, missile[i]._miVar2 }
	    && InDungeonBounds({ mx, my })) {
		if (id != -1) {
			if (missile[i]._micaster == TARGET_PLAYERS
			    && monster[id].MType->mtype >= MT_STORM
			    && monster[id].MType->mtype <= MT_MAEL) {
				AddMissile(
				    missile[i].position.tile,
				    missile[i].position.start,
				    i,
				    MIS_LIGHTNING2,
				    missile[i]._micaster,
				    id,
				    dam,
				    missile[i]._mispllvl);
			} else {
				AddMissile(
				    missile[i].position.tile,
				    missile[i].position.start,
				    i,
				    MIS_LIGHTNING,
				    missile[i]._micaster,
				    id,
				    dam,
				    missile[i]._mispllvl);
			}
		} else {
			AddMissile(
			    missile[i].position.tile,
			    missile[i].position.start,
			    i,
			    MIS_LIGHTNING,
			    missile[i]._micaster,
			    id,
			    dam,
			    missile[i]._mispllvl);
		}
		missile[i]._miVar1 = missile[i].position.tile.x;
		missile[i]._miVar2 = missile[i].position.tile.y;
	}
	assert(mx != 0 && my != 0);
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
	}
}

void MI_Lightning(int i)
{
	int j;

	missile[i]._mirange--;
	j = missile[i]._mirange;
	if (missile[i].position.tile != missile[i].position.start)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i].position.tile, false);
	if (missile[i]._miHitFlag)
		missile[i]._mirange = j;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Town(int i)
{
	int expLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, 1);
	if (currlevel != 0 && missile[i]._mimfnum != 1 && missile[i]._mirange != 0) {
		if (missile[i]._miVar2 == 0)
			missile[i]._mlid = AddLight(missile[i].position.tile, 1);
		ChangeLight(missile[i]._mlid, missile[i].position.tile, expLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}

	for (int p = 0; p < MAX_PLRS; p++) {
		auto &player = plr[p];
		if (player.plractive && currlevel == player.plrlevel && !player._pLvlChanging && player._pmode == PM_STAND && player.position.tile == missile[i].position.tile) {
			ClrPlrPath(player);
			if (p == myplr) {
				NetSendCmdParam1(true, CMD_WARP, missile[i]._misource);
				player._pmode = PM_NEWLVL;
			}
		}
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Flash(int i)
{
	if (missile[i]._micaster == TARGET_MONSTERS) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource]._pInvincible = true;
	}
	missile[i]._mirange--;

	constexpr Point Offsets[] = { { -1, 0 }, { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };
	for (Point offset : Offsets)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i].position.tile + offset, true);

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		if (missile[i]._micaster == TARGET_MONSTERS) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource]._pInvincible = false;
		}
	}
	PutMissile(i);
}

void MI_Flash2(int i)
{
	if (missile[i]._micaster == TARGET_MONSTERS) {
		if (missile[i]._misource != -1)
			plr[missile[i]._misource]._pInvincible = true;
	}
	missile[i]._mirange--;

	constexpr Point Offsets[] = { { -1, -1 }, { 0, -1 }, { 1, -1 } };
	for (Point offset : Offsets)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i].position.tile + offset, true);

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		if (missile[i]._micaster == TARGET_MONSTERS) {
			if (missile[i]._misource != -1)
				plr[missile[i]._misource]._pInvincible = false;
		}
	}
	PutMissile(i);
}

void MI_Manashield(int i)
{
	int id = missile[i]._misource;
	if (id != myplr) {
		if (currlevel != plr[id].plrlevel)
			missile[i]._miDelFlag = true;
	} else {
		if (plr[id]._pMana <= 0 || !plr[id].plractive)
			missile[i]._mirange = 0;

		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = true;
			NetSendCmd(true, CMD_ENDSHIELD);
		}
	}
	PutMissile(i);
}

void MI_Etherealize(int i)
{
	missile[i]._mirange--;

	auto &player = plr[missile[i]._misource];

	missile[i].position.tile = player.position.tile;
	missile[i].position.traveled.x = player.position.offset.x << 16;
	missile[i].position.traveled.y = player.position.offset.y << 16;
	if (player._pmode == PM_WALK3) {
		missile[i].position.start = player.position.future;
	} else {
		missile[i].position.start = player.position.tile;
	}
	GetMissilePos(i);
	if (player._pmode == PM_WALK3) {
		if (player._pdir == DIR_W)
			missile[i].position.tile.x++;
		else
			missile[i].position.tile.y++;
	}
	player._pSpellFlags |= 1;
	if (missile[i]._mirange == 0 || player._pHitPoints <= 0) {
		missile[i]._miDelFlag = true;
		player._pSpellFlags &= ~0x1;
	}
	PutMissile(i);
}

void MI_Firemove(int i)
{
	int j;
	constexpr int ExpLight[14] = { 2, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 12 };

	missile[i].position.tile.x--;
	missile[i].position.tile.y--;
	missile[i].position.offset.y += 32;
	missile[i]._miVar1++;
	if (missile[i]._miVar1 == missile[i]._miAnimLen) {
		SetMissDir(i, 1);
		missile[i]._miAnimFrame = GenerateRnd(11) + 1;
	}
	missile[i].position.traveled += missile[i].position.velocity;
	GetMissilePos(i);
	j = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i].position.tile, false);
	if (missile[i]._miHitFlag)
		missile[i]._mirange = j;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._mimfnum != 0 || missile[i]._mirange == 0) {
		if (missile[i].position.tile.x != missile[i]._miVar3 || missile[i].position.tile.y != missile[i]._miVar4) {
			missile[i]._miVar3 = missile[i].position.tile.x;
			missile[i]._miVar4 = missile[i].position.tile.y;
			ChangeLight(missile[i]._mlid, { missile[i]._miVar3, missile[i]._miVar4 }, 8);
		}
	} else {
		if (missile[i]._miVar2 == 0)
			missile[i]._mlid = AddLight(missile[i].position.tile, ExpLight[0]);
		ChangeLight(missile[i]._mlid, missile[i].position.tile, ExpLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	missile[i].position.tile.x++;
	missile[i].position.tile.y++;
	missile[i].position.offset.y -= 32;
	PutMissile(i);
}

void MI_Guardian(int i)
{
	int j, k, sx, sy, sx1, sy1, ex;

	assert((DWORD)i < MAXMISSILES);

	sx1 = 0;
	sy1 = 0;
	missile[i]._mirange--;

	if (missile[i]._miVar2 > 0) {
		missile[i]._miVar2--;
	}
	if (missile[i]._mirange == missile[i]._miVar1 || (missile[i]._mimfnum == MFILE_GUARD && missile[i]._miVar2 == 0)) {
		SetMissDir(i, 1);
	}

	if ((missile[i]._mirange % 16) == 0) {
		ex = 0;
		for (j = 0; j < 23 && ex != -1; j++) {
			for (k = 10; k >= 0 && ex != -1 && (vCrawlTable[j][k] != 0 || vCrawlTable[j][k + 1] != 0); k -= 2) {
				if (sx1 == vCrawlTable[j][k] && sy1 == vCrawlTable[j][k + 1]) {
					continue;
				}
				sx = missile[i].position.tile.x + vCrawlTable[j][k];
				sy = missile[i].position.tile.y + vCrawlTable[j][k + 1];
				ex = Sentfire(i, { sx, sy });
				if (ex == -1) {
					break;
				}
				sx = missile[i].position.tile.x - vCrawlTable[j][k];
				sy = missile[i].position.tile.y - vCrawlTable[j][k + 1];
				ex = Sentfire(i, { sx, sy });
				if (ex == -1) {
					break;
				}
				sx = missile[i].position.tile.x + vCrawlTable[j][k];
				sy = missile[i].position.tile.y - vCrawlTable[j][k + 1];
				ex = Sentfire(i, { sx, sy });
				if (ex == -1) {
					break;
				}
				sx = missile[i].position.tile.x - vCrawlTable[j][k];
				sy = missile[i].position.tile.y + vCrawlTable[j][k + 1];
				ex = Sentfire(i, { sx, sy });
				if (ex == -1) {
					break;
				}
				sx1 = vCrawlTable[j][k];
				sy1 = vCrawlTable[j][k + 1];
			}
		}
	}

	if (missile[i]._mirange == 14) {
		SetMissDir(i, 0);
		missile[i]._miAnimFrame = 15;
		missile[i]._miAnimAdd = -1;
	}

	missile[i]._miVar3 += missile[i]._miAnimAdd;

	if (missile[i]._miVar3 > 15) {
		missile[i]._miVar3 = 15;
	} else if (missile[i]._miVar3 > 0) {
		ChangeLight(missile[i]._mlid, missile[i].position.tile, missile[i]._miVar3);
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}

	PutMissile(i);
}

void MI_Chain(int i)
{
	int sx, sy, id, l, n, m, k, rad, tx, ty;

	id = missile[i]._misource;
	sx = missile[i].position.tile.x;
	sy = missile[i].position.tile.y;
	Direction dir = GetDirection({ sx, sy }, { missile[i]._miVar1, missile[i]._miVar2 });
	AddMissile({ sx, sy }, { missile[i]._miVar1, missile[i]._miVar2 }, dir, MIS_LIGHTCTRL, TARGET_MONSTERS, id, 1, missile[i]._mispllvl);
	rad = missile[i]._mispllvl + 3;
	if (rad > 19)
		rad = 19;
	for (m = 1; m < rad; m++) {
		k = CrawlNum[m];
		l = k + 2;
		for (n = (BYTE)CrawlTable[k]; n > 0; n--) {
			tx = sx + CrawlTable[l - 1];
			ty = sy + CrawlTable[l];
			if (InDungeonBounds({ tx, ty }) && dMonster[tx][ty] > 0) {
				dir = GetDirection({ sx, sy }, { tx, ty });
				AddMissile({ sx, sy }, { tx, ty }, dir, MIS_LIGHTCTRL, TARGET_MONSTERS, id, 1, missile[i]._mispllvl);
			}
			l += 2;
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
}

void MI_Blood(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	if (missile[i]._miAnimFrame == missile[i]._miAnimLen)
		missile[i]._miPreFlag = true;
	PutMissile(i);
}

void MI_Weapexp(int i)
{
	int id, mind, maxd;
	int ExpLight[10] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2 };

	missile[i]._mirange--;
	id = missile[i]._misource;
	if (missile[i]._miVar2 == 1) {
		mind = plr[id]._pIFMinDam;
		maxd = plr[id]._pIFMaxDam;
		missiledata[missile[i]._mitype].mResist = MISR_FIRE;
	} else {
		mind = plr[id]._pILMinDam;
		maxd = plr[id]._pILMaxDam;
		missiledata[missile[i]._mitype].mResist = MISR_LIGHTNING;
	}
	CheckMissileCol(i, mind, maxd, false, missile[i].position.tile, false);
	if (missile[i]._miVar1 == 0) {
		missile[i]._mlid = AddLight(missile[i].position.tile, 9);
	} else {
		if (missile[i]._mirange != 0)
			ChangeLight(missile[i]._mlid, missile[i].position.tile, ExpLight[missile[i]._miVar1]);
	}
	missile[i]._miVar1++;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	} else {
		PutMissile(i);
	}
}

void MI_Misexp(int i)
{
	constexpr int ExpLight[] = { 9, 10, 11, 12, 11, 10, 8, 6, 4, 2, 1, 0, 0, 0, 0 };

	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	} else {
		if (missile[i]._miVar1 == 0)
			missile[i]._mlid = AddLight(missile[i].position.tile, 9);
		else
			ChangeLight(missile[i]._mlid, missile[i].position.tile, ExpLight[missile[i]._miVar1]);
		missile[i]._miVar1++;
		PutMissile(i);
	}
}

void MI_Acidsplat(int i)
{
	int monst, dam;

	if (missile[i]._mirange == missile[i]._miAnimLen) {
		missile[i].position.tile.x++;
		missile[i].position.tile.y++;
		missile[i].position.offset.y -= 32;
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		monst = missile[i]._misource;
		dam = (monster[monst].MData->mLevel >= 2 ? 2 : 1);
		AddMissile(missile[i].position.tile, { i, 0 }, missile[i]._mimfnum, MIS_ACIDPUD, TARGET_PLAYERS, monst, dam, missile[i]._mispllvl);
	} else {
		PutMissile(i);
	}
}

void MI_Teleport(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange <= 0) {
		missile[i]._miDelFlag = true;
		return;
	}

	int id = missile[i]._misource;
	auto &player = plr[id];

	dPlayer[player.position.tile.x][player.position.tile.y] = 0;
	PlrClrTrans(player.position.tile);
	player.position.tile = { missile[i].position.tile.x, missile[i].position.tile.y };
	player.position.future = player.position.tile;
	player.position.old = player.position.tile;
	PlrDoTrans(player.position.tile);
	missile[i]._miVar1 = 1;
	dPlayer[player.position.tile.x][player.position.tile.y] = id + 1;
	if (leveltype != DTYPE_TOWN) {
		ChangeLightXY(player._plid, player.position.tile);
		ChangeVisionXY(player._pvid, player.position.tile);
	}
	if (id == myplr) {
		ViewX = player.position.tile.x - ScrollInfo.tile.x;
		ViewY = player.position.tile.y - ScrollInfo.tile.y;
	}
}

void MI_Stone(int i)
{
	int m;

	missile[i]._mirange--;
	m = missile[i]._miVar2;
	if (monster[m]._mhitpoints == 0 && missile[i]._miAnimType != MFILE_SHATTER1) {
		missile[i]._mimfnum = 0;
		missile[i]._miDrawFlag = true;
		SetMissAnim(i, MFILE_SHATTER1);
		missile[i]._mirange = 11;
	}
	if (monster[m]._mmode != MM_STONE) {
		missile[i]._miDelFlag = true;
		return;
	}

	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		if (monster[m]._mhitpoints > 0) {
			monster[m]._mmode = (MON_MODE)missile[i]._miVar1;
			monster[m].AnimInfo.IsPetrified = false;
		} else {
			AddDead(monster[m].position.tile, stonendx, monster[m]._mdir);
		}
	}
	if (missile[i]._miAnimType == MFILE_SHATTER1)
		PutMissile(i);
}

void MI_Boom(int i)
{
	missile[i]._mirange--;
	if (missile[i]._miVar1 == 0)
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i].position.tile, true);
	if (missile[i]._miHitFlag)
		missile[i]._miVar1 = 1;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Rhino(int i)
{
	int mix, miy, mix2, miy2, omx, omy, monst;

	monst = missile[i]._misource;
	if (monster[monst]._mmode != MM_CHARGE) {
		missile[i]._miDelFlag = true;
		return;
	}
	GetMissilePos(i);
	mix = missile[i].position.tile.x;
	miy = missile[i].position.tile.y;
	dMonster[mix][miy] = 0;
	if (monster[monst]._mAi == AI_SNAKE) {
		missile[i].position.traveled += missile[i].position.velocity * 2;
		GetMissilePos(i);
		mix2 = missile[i].position.tile.x;
		miy2 = missile[i].position.tile.y;
		missile[i].position.traveled -= missile[i].position.velocity;
	} else {
		missile[i].position.traveled += missile[i].position.velocity;
	}
	GetMissilePos(i);
	omx = missile[i].position.tile.x;
	omy = missile[i].position.tile.y;
	if (!PosOkMonst(monst, missile[i].position.tile) || (monster[monst]._mAi == AI_SNAKE && !PosOkMonst(monst, { mix2, miy2 }))) {
		MissToMonst(i, { mix, miy });
		missile[i]._miDelFlag = true;
		return;
	}
	monster[monst].position.future = { omx, omy };
	monster[monst].position.old = { omx, omy };
	monster[monst].position.tile = { omx, omy };
	dMonster[omx][omy] = -(monst + 1);
	if (monster[monst]._uniqtype != 0)
		ChangeLightXY(missile[i]._mlid, { omx, omy });
	MoveMissilePos(i);
	PutMissile(i);
}

void MI_Fireman(int i)
{
	int j;

	GetMissilePos(i);
	Point a = missile[i].position.tile;
	missile[i].position.traveled += missile[i].position.velocity;
	GetMissilePos(i);
	int src = missile[i]._misource;
	Point b = missile[i].position.tile;
	int enemy = monster[src]._menemy;
	Point c = (monster[src]._mFlags & MFLAG_TARGETS_MONSTER) == 0 ? plr[enemy].position.tile : monster[enemy].position.tile;
	if (b != a && (((missile[i]._miVar1 & 1) != 0 && a.WalkingDistance(c) >= 4) || missile[i]._miVar2 > 1) && PosOkMonst(missile[i]._misource, a)) {
		MissToMonst(i, a);
		missile[i]._miDelFlag = true;
	} else if ((monster[src]._mFlags & MFLAG_TARGETS_MONSTER) == 0) {
		j = dPlayer[b.x][b.y];
	} else {
		j = dMonster[b.x][b.y];
	}
	if (!PosOkMissile(0, b) || (j > 0 && (missile[i]._miVar1 & 1) == 0)) {
		missile[i].position.velocity *= -1;
		missile[i]._mimfnum = opposite[missile[i]._mimfnum];
		missile[i]._miAnimData = monster[src].MType->Anims[MA_WALK].CelSpritesForDirections[missile[i]._mimfnum]->Data();
		missile[i]._miVar2++;
		if (j > 0)
			missile[i]._miVar1 |= 1;
	}
	MoveMissilePos(i);
	PutMissile(i);
}

void MI_FirewallC(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		return;
	}

	int id = missile[i]._misource;

	{
		Point position = { missile[i]._miVar1, missile[i]._miVar2 };
		Point target = position + static_cast<Direction>(missile[i]._miVar3);

		if (!missile[i].limitReached && GrowWall(id, position, target, MIS_FIREWALL, missile[i]._mispllvl, 0)) {
			missile[i]._miVar1 = target.x;
			missile[i]._miVar2 = target.y;
		} else {
			missile[i].limitReached = true;
		}
	}

	{
		Point position = { missile[i]._miVar5, missile[i]._miVar6 };
		Point target = position + static_cast<Direction>(missile[i]._miVar4);

		if (missile[i]._miVar7 == 0 && GrowWall(id, position, target, MIS_FIREWALL, missile[i]._mispllvl, 0)) {
			missile[i]._miVar5 = target.x;
			missile[i]._miVar6 = target.y;
		} else {
			missile[i]._miVar7 = 1;
		}
	}
}

void MI_Infra(int i)
{
	missile[i]._mirange--;
	plr[missile[i]._misource]._pInfraFlag = true;
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		CalcPlrItemVals(missile[i]._misource, true);
	}
}

void MI_Apoca(int i)
{
	int j, k;

	int id = missile[i]._misource;
	bool exit = false;
	for (j = missile[i]._miVar2; j < missile[i]._miVar3 && !exit; j++) {
		for (k = missile[i]._miVar4; k < missile[i]._miVar5 && !exit; k++) {
			if (dMonster[k][j] < MAX_PLRS)
				continue;
			if (nSolidTable[dPiece[k][j]])
				continue;
			if (gbIsHellfire && !LineClearMissile(missile[i].position.tile, { k, j }))
				continue;
			AddMissile({ k, j }, { k, j }, plr[id]._pdir, MIS_BOOM, TARGET_MONSTERS, id, missile[i]._midam, 0);
			exit = true;
		}
		if (!exit) {
			missile[i]._miVar4 = missile[i]._miVar6;
		}
	}

	if (exit) {
		missile[i]._miVar2 = j - 1;
		missile[i]._miVar4 = k;
	} else {
		missile[i]._miDelFlag = true;
	}
}

void MI_Wave(int i)
{
	bool f1 = false;
	bool f2 = false;
	assert((DWORD)i < MAXMISSILES);

	int id = missile[i]._misource;
	Point src = missile[i].position.tile;
	Direction sd = GetDirection(src, { missile[i]._miVar1, missile[i]._miVar2 });
	Direction dira = left[left[sd]];
	Direction dirb = right[right[sd]];
	Point na = src + sd;
	int pn = dPiece[na.x][na.y];
	assert((DWORD)pn <= MAXTILES);
	if (!nMissileTable[pn]) {
		Direction pdir = plr[id]._pdir;
		AddMissile(na, na + sd, pdir, MIS_FIREMOVE, TARGET_MONSTERS, id, 0, missile[i]._mispllvl);
		na += dira;
		Point nb = src + sd + dirb;
		for (int j = 0; j < (missile[i]._mispllvl / 2) + 2; j++) {
			pn = dPiece[na.x][na.y]; // BUGFIX: dPiece is accessed before check against dungeon size and 0
			assert((DWORD)pn <= MAXTILES);
			if (nMissileTable[pn] || f1 || !InDungeonBounds(na)) {
				f1 = true;
			} else {
				AddMissile(na, na + sd, pdir, MIS_FIREMOVE, TARGET_MONSTERS, id, 0, missile[i]._mispllvl);
				na += dira;
			}
			pn = dPiece[nb.x][nb.y]; // BUGFIX: dPiece is accessed before check against dungeon size and 0
			assert((DWORD)pn <= MAXTILES);
			if (nMissileTable[pn] || f2 || !InDungeonBounds(nb)) {
				f2 = true;
			} else {
				AddMissile(nb, nb + sd, pdir, MIS_FIREMOVE, TARGET_MONSTERS, id, 0, missile[i]._mispllvl);
				nb += dirb;
			}
		}
	}

	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
}

void MI_Nova(int i)
{
	int sx1 = 0;
	int sy1 = 0;
	int id = missile[i]._misource;
	int dam = missile[i]._midam;
	Point src = missile[i].position.tile;
	Direction dir = DIR_S;
	mienemy_type en = TARGET_PLAYERS;
	if (id != -1) {
		dir = plr[id]._pdir;
		en = TARGET_MONSTERS;
	}
	for (const auto &k : vCrawlTable) {
		if (sx1 != k[6] || sy1 != k[7]) {
			AddMissile(src, src + Point { k[6], k[7] }, dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(src, src + Point { -k[6], -k[7] }, dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(src, src + Point { -k[6], k[7] }, dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			AddMissile(src, src + Point { k[6], -k[7] }, dir, MIS_LIGHTBALL, en, id, dam, missile[i]._mispllvl);
			sx1 = k[6];
			sy1 = k[7];
		}
	}
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
}

void MI_Blodboil(int i)
{
	missile[i]._mirange--;

	if (missile[i]._mirange != 0) {
		return;
	}

	int id = missile[i]._miVar1;
	auto &player = plr[id];

	int hpdif = player._pMaxHP - player._pHitPoints;

	if ((player._pSpellFlags & 2) != 0) {
		player._pSpellFlags &= ~0x2;
		player._pSpellFlags |= 4;
		int lvl = player._pLevel * 2;
		missile[i]._mirange = lvl + 10 * missile[i]._mispllvl + 245;
	} else {
		player._pSpellFlags &= ~0x4;
		missile[i]._miDelFlag = true;
		hpdif += missile[i]._miVar2;
	}

	CalcPlrItemVals(id, true);
	ApplyPlrDamage(id, 0, 1, hpdif);
	force_redraw = 255;
	player.Say(HeroSpeech::HeavyBreathing);
}

void MI_Flame(int i)
{
	int k;

	missile[i]._mirange--;
	missile[i]._miVar2--;
	k = missile[i]._mirange;
	CheckMissileCol(i, missile[i]._midam, missile[i]._midam, true, missile[i].position.tile, false);
	if (missile[i]._mirange == 0 && missile[i]._miHitFlag)
		missile[i]._mirange = k;
	if (missile[i]._miVar2 == 0)
		missile[i]._miAnimFrame = 20;
	if (missile[i]._miVar2 <= 0) {
		k = missile[i]._miAnimFrame;
		if (k > 11)
			k = 24 - k;
		ChangeLight(missile[i]._mlid, missile[i].position.tile, k);
	}
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	if (missile[i]._miVar2 <= 0)
		PutMissile(i);
}

void MI_Flamec(int i)
{
	int id, src;

	missile[i]._mirange--;
	src = missile[i]._misource;
	missile[i].position.traveled += missile[i].position.velocity;
	GetMissilePos(i);
	if (missile[i].position.tile.x != missile[i]._miVar1 || missile[i].position.tile.y != missile[i]._miVar2) {
		id = dPiece[missile[i].position.tile.x][missile[i].position.tile.y];
		if (!nMissileTable[id]) {
			AddMissile(
			    missile[i].position.tile,
			    missile[i].position.start,
			    i,
			    MIS_FLAME,
			    missile[i]._micaster,
			    src,
			    missile[i]._miVar3,
			    missile[i]._mispllvl);
		} else {
			missile[i]._mirange = 0;
		}
		missile[i]._miVar1 = missile[i].position.tile.x;
		missile[i]._miVar2 = missile[i].position.tile.y;
		missile[i]._miVar3++;
	}
	if (missile[i]._mirange == 0 || missile[i]._miVar3 == 3)
		missile[i]._miDelFlag = true;
}

void MI_Cbolt(int i)
{
	missile[i]._mirange--;
	if (missile[i]._miAnimType != MFILE_LGHNING) {
		if (missile[i]._miVar3 == 0) {
			constexpr int bpath[16] = { -1, 0, 1, -1, 0, 1, -1, -1, 0, 0, 1, 1, 0, 1, -1, 0 };

			auto md = static_cast<Direction>(missile[i]._miVar2);
			switch (bpath[missile[i]._mirnd]) {
			case -1:
				md = left[md];
				break;
			case 1:
				md = right[md];
				break;
			}

			missile[i]._mirnd = (missile[i]._mirnd + 1) & 0xF;
			GetMissileVel(i, missile[i].position.tile, missile[i].position.tile + md, 8);
			missile[i]._miVar3 = 16;
		} else {
			missile[i]._miVar3--;
		}
		missile[i].position.traveled += missile[i].position.velocity;
		GetMissilePos(i);
		CheckMissileCol(i, missile[i]._midam, missile[i]._midam, false, missile[i].position.tile, false);
		if (missile[i]._miHitFlag) {
			missile[i]._miVar1 = 8;
			missile[i]._mimfnum = 0;
			missile[i].position.offset = { 0, 0 };
			SetMissAnim(i, MFILE_LGHNING);
			missile[i]._mirange = missile[i]._miAnimLen;
			GetMissilePos(i);
		}
		ChangeLight(missile[i]._mlid, missile[i].position.tile, missile[i]._miVar1);
	}
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void MI_Hbolt(int i)
{
	missile[i]._mirange--;
	if (missile[i]._miAnimType != MFILE_HOLYEXPL) {
		missile[i].position.traveled += missile[i].position.velocity;
		GetMissilePos(i);
		int dam = missile[i]._midam;
		if (missile[i].position.tile != missile[i].position.start) {
			CheckMissileCol(i, dam, dam, false, missile[i].position.tile, false);
		}
		if (missile[i]._mirange == 0) {
			missile[i].position.traveled -= missile[i].position.velocity;
			GetMissilePos(i);
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_HOLYEXPL);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		} else {
			if (missile[i].position.tile != Point { missile[i]._miVar1, missile[i]._miVar2 }) {
				missile[i]._miVar1 = missile[i].position.tile.x;
				missile[i]._miVar2 = missile[i].position.tile.y;
				ChangeLight(missile[i]._mlid, { missile[i]._miVar1, missile[i]._miVar2 }, 8);
			}
		}
	} else {
		ChangeLight(missile[i]._mlid, missile[i].position.tile, missile[i]._miAnimFrame + 7);
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = true;
			AddUnLight(missile[i]._mlid);
		}
	}
	PutMissile(i);
}

void MI_Element(int i)
{
	int mid, dam, id;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (missile[i]._miAnimType == MFILE_BIGEXP) {
		Point c = missile[i].position.tile;
		Point p = plr[id].position.tile;
		ChangeLight(missile[i]._mlid, missile[i].position.tile, missile[i]._miAnimFrame);
		if (!CheckBlock(p, c))
			CheckMissileCol(i, dam, dam, true, c, true);

		constexpr Point Offsets[] = { { 0, 1 }, { 0, -1 }, { 1, 0 }, { 1, -1 }, { 1, 1 }, { -1, 0 }, { -1, 1 }, { -1, -1 } };
		for (Point offset : Offsets) {
			if (!CheckBlock(p, c + offset))
				CheckMissileCol(i, dam, dam, true, c + offset, true);
		}

		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = true;
			AddUnLight(missile[i]._mlid);
		}
	} else {
		missile[i].position.traveled += missile[i].position.velocity;
		GetMissilePos(i);
		Point c = missile[i].position.tile;
		CheckMissileCol(i, dam, dam, false, c, false);
		if (missile[i]._miVar3 == 0 && c == Point { missile[i]._miVar4, missile[i]._miVar5 })
			missile[i]._miVar3 = 1;
		if (missile[i]._miVar3 == 1) {
			missile[i]._miVar3 = 2;
			missile[i]._mirange = 255;
			mid = FindClosest(c, 19);
			if (mid > 0) {
				Direction sd = GetDirection(c, monster[mid].position.tile);
				SetMissDir(i, sd);
				GetMissileVel(i, c, monster[mid].position.tile, 16);
			} else {
				Direction sd = plr[id]._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, c, c + sd, 16);
			}
		}
		if (c != Point { missile[i]._miVar1, missile[i]._miVar2 }) {
			missile[i]._miVar1 = c.x;
			missile[i]._miVar2 = c.y;
			ChangeLight(missile[i]._mlid, c, 8);
		}
		if (missile[i]._mirange == 0) {
			missile[i]._mimfnum = 0;
			SetMissAnim(i, MFILE_BIGEXP);
			missile[i]._mirange = missile[i]._miAnimLen - 1;
		}
	}
	PutMissile(i);
}

void MI_Bonespirit(int i)
{
	int id, mid, dam;

	missile[i]._mirange--;
	dam = missile[i]._midam;
	id = missile[i]._misource;
	if (missile[i]._mimfnum == DIR_OMNI) {
		ChangeLight(missile[i]._mlid, missile[i].position.tile, missile[i]._miAnimFrame);
		if (missile[i]._mirange == 0) {
			missile[i]._miDelFlag = true;
			AddUnLight(missile[i]._mlid);
		}
		PutMissile(i);
	} else {
		missile[i].position.traveled += missile[i].position.velocity;
		GetMissilePos(i);
		Point c = missile[i].position.tile;
		CheckMissileCol(i, dam, dam, false, c, false);
		if (missile[i]._miVar3 == 0 && c == Point { missile[i]._miVar4, missile[i]._miVar5 })
			missile[i]._miVar3 = 1;
		if (missile[i]._miVar3 == 1) {
			missile[i]._miVar3 = 2;
			missile[i]._mirange = 255;
			mid = FindClosest(c, 19);
			if (mid > 0) {
				missile[i]._midam = monster[mid]._mhitpoints >> 7;
				SetMissDir(i, GetDirection(c, monster[mid].position.tile));
				GetMissileVel(i, c, monster[mid].position.tile, 16);
			} else {
				Direction sd = plr[id]._pdir;
				SetMissDir(i, sd);
				GetMissileVel(i, c, c + sd, 16);
			}
		}
		if (c != Point { missile[i]._miVar1, missile[i]._miVar2 }) {
			missile[i]._miVar1 = c.x;
			missile[i]._miVar2 = c.y;
			ChangeLight(missile[i]._mlid, c, 8);
		}
		if (missile[i]._mirange == 0) {
			SetMissDir(i, DIR_OMNI);
			missile[i]._mirange = 7;
		}
		PutMissile(i);
	}
}

void MI_ResurrectBeam(int i)
{
	missile[i]._mirange--;
	if (missile[i]._mirange == 0)
		missile[i]._miDelFlag = true;
	PutMissile(i);
}

void MI_Rportal(int i)
{
	int expLight[17] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 15 };

	if (missile[i]._mirange > 1)
		missile[i]._mirange--;
	if (missile[i]._mirange == missile[i]._miVar1)
		SetMissDir(i, 1);

	if (currlevel != 0 && missile[i]._mimfnum != 1 && missile[i]._mirange != 0) {
		if (missile[i]._miVar2 == 0)
			missile[i]._mlid = AddLight(missile[i].position.tile, 1);
		ChangeLight(missile[i]._mlid, missile[i].position.tile, expLight[missile[i]._miVar2]);
		missile[i]._miVar2++;
	}
	if (missile[i]._mirange == 0) {
		missile[i]._miDelFlag = true;
		AddUnLight(missile[i]._mlid);
	}
	PutMissile(i);
}

void ProcessMissiles()
{
	int i, mi;

	for (i = 0; i < nummissiles; i++) {
		dFlags[missile[missileactive[i]].position.tile.x][missile[missileactive[i]].position.tile.y] &= ~BFLAG_MISSILE;
		dMissile[missile[missileactive[i]].position.tile.x][missile[missileactive[i]].position.tile.y] = 0;
		if (missile[missileactive[i]].position.tile.x < 0 || missile[missileactive[i]].position.tile.x >= MAXDUNX - 1 || missile[missileactive[i]].position.tile.y < 0 || missile[missileactive[i]].position.tile.y >= MAXDUNY - 1)
			missile[missileactive[i]]._miDelFlag = true;
	}

	i = 0;
	while (i < nummissiles) {
		if (missile[missileactive[i]]._miDelFlag) {
			DeleteMissile(missileactive[i], i);
			i = 0;
		} else {
			i++;
		}
	}

	MissilePreFlag = false;

	for (i = 0; i < nummissiles; i++) {
		mi = missileactive[i];
		missiledata[missile[mi]._mitype].mProc(missileactive[i]);
		if ((missile[mi]._miAnimFlags & MFLAG_LOCK_ANIMATION) == 0) {
			missile[mi]._miAnimCnt++;
			if (missile[mi]._miAnimCnt >= missile[mi]._miAnimDelay) {
				missile[mi]._miAnimCnt = 0;
				missile[mi]._miAnimFrame += missile[mi]._miAnimAdd;
				if (missile[mi]._miAnimFrame > missile[mi]._miAnimLen)
					missile[mi]._miAnimFrame = 1;
				if (missile[mi]._miAnimFrame < 1)
					missile[mi]._miAnimFrame = missile[mi]._miAnimLen;
			}
		}
	}

	i = 0;
	while (i < nummissiles) {
		if (missile[missileactive[i]]._miDelFlag) {
			DeleteMissile(missileactive[i], i);
			i = 0;
		} else {
			i++;
		}
	}
}

void missiles_process_charge()
{
	AnimStruct *anim;

	for (int i = 0; i < nummissiles; i++) {
		int mi = missileactive[i];
		MissileStruct *mis = &missile[mi];

		mis->_miAnimData = misfiledata[mis->_miAnimType].mAnimData[mis->_mimfnum];
		if (mis->_mitype != MIS_RHINO)
			continue;

		CMonster *mon = monster[mis->_misource].MType;

		if (mon->mtype >= MT_HORNED && mon->mtype <= MT_OBLORD) {
			anim = &mon->Anims[MA_SPECIAL];
		} else {
			if (mon->mtype >= MT_NSNAKE && mon->mtype <= MT_GSNAKE)
				anim = &mon->Anims[MA_ATTACK];
			else
				anim = &mon->Anims[MA_WALK];
		}
		missile[mi]._miAnimData = anim->CelSpritesForDirections[mis->_mimfnum]->Data();
	}
}

void ClearMissileSpot(int mi)
{
	dFlags[missile[mi].position.tile.x][missile[mi].position.tile.y] &= ~BFLAG_MISSILE;
	dMissile[missile[mi].position.tile.x][missile[mi].position.tile.y] = 0;
}

} // namespace devilution
