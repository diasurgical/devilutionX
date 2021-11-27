/**
 * @file monster.cpp
 *
 * Implementation of monster functionality, AI, actions, spawning, loading, etc.
 */
#include "monster.h"

#include <algorithm>
#include <array>
#include <climits>

#include <fmt/format.h>

#include "control.h"
#include "cursor.h"
#include "dead.h"
#include "drlg_l1.h"
#include "drlg_l4.h"
#include "engine/cel_header.hpp"
#include "engine/load_file.hpp"
#include "engine/random.hpp"
#include "engine/render/cl2_render.hpp"
#include "init.h"
#include "lighting.h"
#include "minitext.h"
#include "missiles.h"
#include "movie.h"
#include "options.h"
#include "spelldat.h"
#include "storm/storm_net.hpp"
#include "themes.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"
#include "utils/utf8.hpp"

#ifdef _DEBUG
#include "debug.h"
#endif

namespace devilution {

CMonster LevelMonsterTypes[MAX_LVLMTYPES];
int LevelMonsterTypeCount;
Monster Monsters[MAXMONSTERS];
int ActiveMonsters[MAXMONSTERS];
int ActiveMonsterCount;
// BUGFIX: replace MonsterKillCounts[MAXMONSTERS] with MonsterKillCounts[NUM_MTYPES].
/** Tracks the total number of monsters killed per monster_id. */
int MonsterKillCounts[MAXMONSTERS];
bool sgbSaveSoundOn;

namespace {

#define NIGHTMARE_TO_HIT_BONUS 85
#define HELL_TO_HIT_BONUS 120

#define NIGHTMARE_AC_BONUS 50
#define HELL_AC_BONUS 80

/** Tracks which missile files are already loaded */
int totalmonsters;
int monstimgtot;
int uniquetrans;

// BUGFIX: MWVel velocity values are not rounded consistently. The correct
// formula for monster walk velocity is calculated as follows (for 16, 32 and 64
// pixel distances, respectively):
//
//    vel16 = (16 << monsterWalkShift) / nframes
//    vel32 = (32 << monsterWalkShift) / nframes
//    vel64 = (64 << monsterWalkShift) / nframes
//
// The correct monster walk velocity table is as follows:
//
//   int MWVel[24][3] = {
//      { 256, 512, 1024 },
//      { 128, 256, 512 },
//      { 85, 171, 341 },
//      { 64, 128, 256 },
//      { 51, 102, 205 },
//      { 43, 85, 171 },
//      { 37, 73, 146 },
//      { 32, 64, 128 },
//      { 28, 57, 114 },
//      { 26, 51, 102 },
//      { 23, 47, 93 },
//      { 21, 43, 85 },
//      { 20, 39, 79 },
//      { 18, 37, 73 },
//      { 17, 34, 68 },
//      { 16, 32, 64 },
//      { 15, 30, 60 },
//      { 14, 28, 57 },
//      { 13, 27, 54 },
//      { 13, 26, 51 },
//      { 12, 24, 49 },
//      { 12, 23, 47 },
//      { 11, 22, 45 },
//      { 11, 21, 43 }
//   };

/** Maps from monster walk animation frame num to monster velocity. */
int MWVel[24][3] = {
	{ 256, 512, 1024 },
	{ 128, 256, 512 },
	{ 85, 170, 341 },
	{ 64, 128, 256 },
	{ 51, 102, 204 },
	{ 42, 85, 170 },
	{ 36, 73, 146 },
	{ 32, 64, 128 },
	{ 28, 56, 113 },
	{ 26, 51, 102 },
	{ 23, 46, 93 },
	{ 21, 42, 85 },
	{ 19, 39, 78 },
	{ 18, 36, 73 },
	{ 17, 34, 68 },
	{ 16, 32, 64 },
	{ 15, 30, 60 },
	{ 14, 28, 57 },
	{ 13, 26, 54 },
	{ 12, 25, 51 },
	{ 12, 24, 48 },
	{ 11, 23, 46 },
	{ 11, 22, 44 },
	{ 10, 21, 42 }
};
/** Maps from monster action to monster animation letter. */
char animletter[7] = "nwahds";

void InitMonsterTRN(CMonster &monst)
{
	std::array<uint8_t, 256> colorTranslations;
	LoadFileInMem(monst.MData->TransFile, colorTranslations);

	std::replace(colorTranslations.begin(), colorTranslations.end(), 255, 0);

	int n = monst.MData->has_special ? 6 : 5;
	for (int i = 0; i < n; i++) {
		if (i == 1 && monst.mtype >= MT_COUNSLR && monst.mtype <= MT_ADVOCATE) {
			continue;
		}

		for (int j = 0; j < 8; j++) {
			Cl2ApplyTrans(
			    CelGetFrame(monst.Anims[i].CMem.get(), j),
			    colorTranslations,
			    monst.Anims[i].Frames);
		}
	}
}

void InitMonster(Monster &monster, Direction rd, int mtype, Point position)
{
	monster._mdir = rd;
	monster.position.tile = position;
	monster.position.future = position;
	monster.position.old = position;
	monster._mMTidx = mtype;
	monster._mmode = MonsterMode::Stand;
	monster.MType = &LevelMonsterTypes[mtype];
	monster.MData = monster.MType->MData;
	monster.mName = pgettext("monster", monster.MData->mName);
	monster.AnimInfo = {};
	monster.ChangeAnimationData(MonsterGraphic::Stand);
	monster.AnimInfo.TickCounterOfCurrentFrame = GenerateRnd(monster.AnimInfo.TicksPerFrame - 1);
	monster.AnimInfo.CurrentFrame = GenerateRnd(monster.AnimInfo.NumberOfFrames - 1) + 1;

	monster.mLevel = monster.MData->mLevel;
	monster._mmaxhp = (monster.MType->mMinHP + GenerateRnd(monster.MType->mMaxHP - monster.MType->mMinHP + 1)) << 6;
	if (monster.MType->mtype == MT_DIABLO && !gbIsHellfire) {
		monster._mmaxhp /= 2;
		monster.mLevel -= 15;
	}

	if (!gbIsMultiplayer)
		monster._mmaxhp = std::max(monster._mmaxhp / 2, 64);

	monster._mhitpoints = monster._mmaxhp;
	monster._mAi = monster.MData->mAi;
	monster._mint = monster.MData->mInt;
	monster._mgoal = MGOAL_NORMAL;
	monster._mgoalvar1 = 0;
	monster._mgoalvar2 = 0;
	monster._mgoalvar3 = 0;
	monster._pathcount = 0;
	monster._mDelFlag = false;
	monster._uniqtype = 0;
	monster._msquelch = 0;
	monster.mlid = NO_LIGHT; // BUGFIX monsters initial light id should be -1 (fixed)
	monster._mRndSeed = AdvanceRndSeed();
	monster._mAISeed = AdvanceRndSeed();
	monster.mWhoHit = 0;
	monster.mExp = monster.MData->mExp;
	monster.mHit = monster.MData->mHit;
	monster.mMinDamage = monster.MData->mMinDamage;
	monster.mMaxDamage = monster.MData->mMaxDamage;
	monster.mHit2 = monster.MData->mHit2;
	monster.mMinDamage2 = monster.MData->mMinDamage2;
	monster.mMaxDamage2 = monster.MData->mMaxDamage2;
	monster.mArmorClass = monster.MData->mArmorClass;
	monster.mMagicRes = monster.MData->mMagicRes;
	monster.leader = 0;
	monster.leaderRelation = LeaderRelation::None;
	monster._mFlags = monster.MData->mFlags;
	monster.mtalkmsg = TEXT_NONE;

	if (monster._mAi == AI_GARG) {
		monster.ChangeAnimationData(MonsterGraphic::Special);
		monster.AnimInfo.CurrentFrame = 1;
		monster._mFlags |= MFLAG_ALLOW_SPECIAL;
		monster._mmode = MonsterMode::SpecialMeleeAttack;
	}

	if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
		monster._mmaxhp = 3 * monster._mmaxhp;
		if (gbIsHellfire)
			monster._mmaxhp += (gbIsMultiplayer ? 100 : 50) << 6;
		else
			monster._mmaxhp += 64;
		monster._mhitpoints = monster._mmaxhp;
		monster.mLevel += 15;
		monster.mExp = 2 * (monster.mExp + 1000);
		monster.mHit += NIGHTMARE_TO_HIT_BONUS;
		monster.mMinDamage = 2 * (monster.mMinDamage + 2);
		monster.mMaxDamage = 2 * (monster.mMaxDamage + 2);
		monster.mHit2 += NIGHTMARE_TO_HIT_BONUS;
		monster.mMinDamage2 = 2 * (monster.mMinDamage2 + 2);
		monster.mMaxDamage2 = 2 * (monster.mMaxDamage2 + 2);
		monster.mArmorClass += NIGHTMARE_AC_BONUS;
	} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
		monster._mmaxhp = 4 * monster._mmaxhp;
		if (gbIsHellfire)
			monster._mmaxhp += (gbIsMultiplayer ? 200 : 100) << 6;
		else
			monster._mmaxhp += 192;
		monster._mhitpoints = monster._mmaxhp;
		monster.mLevel += 30;
		monster.mExp = 4 * (monster.mExp + 1000);
		monster.mHit += HELL_TO_HIT_BONUS;
		monster.mMinDamage = 4 * monster.mMinDamage + 6;
		monster.mMaxDamage = 4 * monster.mMaxDamage + 6;
		monster.mHit2 += HELL_TO_HIT_BONUS;
		monster.mMinDamage2 = 4 * monster.mMinDamage2 + 6;
		monster.mMaxDamage2 = 4 * monster.mMaxDamage2 + 6;
		monster.mArmorClass += HELL_AC_BONUS;
		monster.mMagicRes = monster.MData->mMagicRes2;
	}
}

bool CanPlaceMonster(int xp, int yp)
{
	if (!InDungeonBounds({ xp, yp })
	    || dMonster[xp][yp] != 0
	    || dPlayer[xp][yp] != 0) {
		return false;
	}

	if (IsTileVisible({ xp, yp })) {
		return false;
	}

	if (TileContainsSetPiece({ xp, yp })) {
		return false;
	}

	return !IsTileSolid({ xp, yp });
}

void PlaceMonster(int i, int mtype, int x, int y)
{
	if (LevelMonsterTypes[mtype].mtype == MT_NAKRUL) {
		for (int j = 0; j < ActiveMonsterCount; j++) {
			if (Monsters[j]._mMTidx == mtype) {
				return;
			}
			if (Monsters[j].MType->mtype == MT_NAKRUL) {
				return;
			}
		}
	}
	dMonster[x][y] = i + 1;

	auto rd = static_cast<Direction>(GenerateRnd(8));
	InitMonster(Monsters[i], rd, mtype, { x, y });
}

void PlaceGroup(int mtype, int num, UniqueMonsterPack uniqueMonsterPack, int leaderId)
{
	int placed = 0;

	auto &leader = Monsters[leaderId];

	for (int try1 = 0; try1 < 10; try1++) {
		while (placed != 0) {
			ActiveMonsterCount--;
			placed--;
			const auto &position = Monsters[ActiveMonsterCount].position.tile;
			dMonster[position.x][position.y] = 0;
		}

		int xp;
		int yp;
		if (uniqueMonsterPack != UniqueMonsterPack::None) {
			int offset = GenerateRnd(8);
			auto position = leader.position.tile + static_cast<Direction>(offset);
			xp = position.x;
			yp = position.y;
		} else {
			do {
				xp = GenerateRnd(80) + 16;
				yp = GenerateRnd(80) + 16;
			} while (!CanPlaceMonster(xp, yp));
		}
		int x1 = xp;
		int y1 = yp;

		if (num + ActiveMonsterCount > totalmonsters) {
			num = totalmonsters - ActiveMonsterCount;
		}

		int j = 0;
		for (int try2 = 0; j < num && try2 < 100; xp += Displacement(static_cast<Direction>(GenerateRnd(8))).deltaX, yp += Displacement(static_cast<Direction>(GenerateRnd(8))).deltaX) { /// BUGFIX: `yp += Point.y`
			if (!CanPlaceMonster(xp, yp)
			    || (dTransVal[xp][yp] != dTransVal[x1][y1])
			    || (uniqueMonsterPack == UniqueMonsterPack::Leashed && (abs(xp - x1) >= 4 || abs(yp - y1) >= 4))) {
				try2++;
				continue;
			}

			PlaceMonster(ActiveMonsterCount, mtype, xp, yp);
			if (uniqueMonsterPack != UniqueMonsterPack::None) {
				auto &minion = Monsters[ActiveMonsterCount];
				minion._mmaxhp *= 2;
				minion._mhitpoints = minion._mmaxhp;
				minion._mint = leader._mint;

				if (uniqueMonsterPack == UniqueMonsterPack::Leashed) {
					minion.leader = leaderId;
					minion.leaderRelation = LeaderRelation::Leashed;
					minion._mAi = leader._mAi;
				}

				if (minion._mAi != AI_GARG) {
					minion.ChangeAnimationData(MonsterGraphic::Stand);
					minion.AnimInfo.CurrentFrame = GenerateRnd(minion.AnimInfo.NumberOfFrames - 1) + 1;
					minion._mFlags &= ~MFLAG_ALLOW_SPECIAL;
					minion._mmode = MonsterMode::Stand;
				}
			}
			ActiveMonsterCount++;
			placed++;
			j++;
		}

		if (placed >= num) {
			break;
		}
	}

	if (uniqueMonsterPack == UniqueMonsterPack::Leashed) {
		leader.packsize = placed;
	}
}

void PlaceUniqueMonst(int uniqindex, int miniontype, int bosspacksize)
{
	auto &monster = Monsters[ActiveMonsterCount];
	const auto &uniqueMonsterData = UniqueMonstersData[uniqindex];

	if ((uniquetrans + 19) * 256 >= LIGHTSIZE) {
		return;
	}

	int uniqtype;
	for (uniqtype = 0; uniqtype < LevelMonsterTypeCount; uniqtype++) {
		if (LevelMonsterTypes[uniqtype].mtype == uniqueMonsterData.mtype) {
			break;
		}
	}

	int count = 0;
	int xp;
	int yp;
	while (true) {
		xp = GenerateRnd(80) + 16;
		yp = GenerateRnd(80) + 16;
		int count2 = 0;
		for (int x = xp - 3; x < xp + 3; x++) {
			for (int y = yp - 3; y < yp + 3; y++) {
				if (InDungeonBounds({ x, y }) && CanPlaceMonster(x, y)) {
					count2++;
				}
			}
		}

		if (count2 < 9) {
			count++;
			if (count < 1000) {
				continue;
			}
		}

		if (CanPlaceMonster(xp, yp)) {
			break;
		}
	}

	if (uniqindex == UMT_SNOTSPIL) {
		xp = 2 * setpc_x + 24;
		yp = 2 * setpc_y + 28;
	}
	if (uniqindex == UMT_WARLORD) {
		xp = 2 * setpc_x + 22;
		yp = 2 * setpc_y + 23;
	}
	if (uniqindex == UMT_ZHAR) {
		for (int i = 0; i < themeCount; i++) {
			if (i == zharlib) {
				xp = 2 * themeLoc[i].x + 20;
				yp = 2 * themeLoc[i].y + 20;
				break;
			}
		}
	}
	if (!gbIsMultiplayer) {
		if (uniqindex == UMT_LAZARUS) {
			xp = 32;
			yp = 46;
		}
		if (uniqindex == UMT_RED_VEX) {
			xp = 40;
			yp = 45;
		}
		if (uniqindex == UMT_BLACKJADE) {
			xp = 38;
			yp = 49;
		}
		if (uniqindex == UMT_SKELKING) {
			xp = 35;
			yp = 47;
		}
	} else {
		if (uniqindex == UMT_LAZARUS) {
			xp = 2 * setpc_x + 19;
			yp = 2 * setpc_y + 22;
		}
		if (uniqindex == UMT_RED_VEX) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 19;
		}
		if (uniqindex == UMT_BLACKJADE) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 25;
		}
	}
	if (uniqindex == UMT_BUTCHER) {
		bool done = false;
		for (yp = 0; yp < MAXDUNY && !done; yp++) {
			for (xp = 0; xp < MAXDUNX && !done; xp++) {
				done = dPiece[xp][yp] == 367;
			}
		}
	}

	if (uniqindex == UMT_NAKRUL) {
		if (UberRow == 0 || UberCol == 0) {
			UberDiabloMonsterIndex = -1;
			return;
		}
		xp = UberRow - 2;
		yp = UberCol;
		UberDiabloMonsterIndex = ActiveMonsterCount;
	}
	PlaceMonster(ActiveMonsterCount, uniqtype, xp, yp);
	monster._uniqtype = uniqindex + 1;

	if (uniqueMonsterData.mlevel != 0) {
		monster.mLevel = 2 * uniqueMonsterData.mlevel;
	} else {
		monster.mLevel = monster.MData->mLevel + 5;
	}

	monster.mExp *= 2;
	monster.mName = pgettext("monster", uniqueMonsterData.mName);
	monster._mmaxhp = uniqueMonsterData.mmaxhp << 6;

	if (!gbIsMultiplayer)
		monster._mmaxhp = std::max(monster._mmaxhp / 2, 64);

	monster._mhitpoints = monster._mmaxhp;
	monster._mAi = uniqueMonsterData.mAi;
	monster._mint = uniqueMonsterData.mint;
	monster.mMinDamage = uniqueMonsterData.mMinDamage;
	monster.mMaxDamage = uniqueMonsterData.mMaxDamage;
	monster.mMinDamage2 = uniqueMonsterData.mMinDamage;
	monster.mMaxDamage2 = uniqueMonsterData.mMaxDamage;
	monster.mMagicRes = uniqueMonsterData.mMagicRes;
	monster.mtalkmsg = uniqueMonsterData.mtalkmsg;
	if (uniqindex == UMT_HORKDMN)
		monster.mlid = NO_LIGHT; // BUGFIX monsters initial light id should be -1 (fixed)
	else
		monster.mlid = AddLight(monster.position.tile, 3);

	if (gbIsMultiplayer) {
		if (monster._mAi == AI_LAZHELP)
			monster.mtalkmsg = TEXT_NONE;
		if (monster._mAi == AI_LAZARUS && Quests[Q_BETRAYER]._qvar1 > 3) {
			monster._mgoal = MGOAL_NORMAL;
		} else if (monster.mtalkmsg != TEXT_NONE) {
			monster._mgoal = MGOAL_INQUIRING;
		}
	} else if (monster.mtalkmsg != TEXT_NONE) {
		monster._mgoal = MGOAL_INQUIRING;
	}

	if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
		monster._mmaxhp = 3 * monster._mmaxhp;
		if (gbIsHellfire)
			monster._mmaxhp += (gbIsMultiplayer ? 100 : 50) << 6;
		else
			monster._mmaxhp += 64;
		monster.mLevel += 15;
		monster._mhitpoints = monster._mmaxhp;
		monster.mExp = 2 * (monster.mExp + 1000);
		monster.mMinDamage = 2 * (monster.mMinDamage + 2);
		monster.mMaxDamage = 2 * (monster.mMaxDamage + 2);
		monster.mMinDamage2 = 2 * (monster.mMinDamage2 + 2);
		monster.mMaxDamage2 = 2 * (monster.mMaxDamage2 + 2);
	} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
		monster._mmaxhp = 4 * monster._mmaxhp;
		if (gbIsHellfire)
			monster._mmaxhp += (gbIsMultiplayer ? 200 : 100) << 6;
		else
			monster._mmaxhp += 192;
		monster.mLevel += 30;
		monster._mhitpoints = monster._mmaxhp;
		monster.mExp = 4 * (monster.mExp + 1000);
		monster.mMinDamage = 4 * monster.mMinDamage + 6;
		monster.mMaxDamage = 4 * monster.mMaxDamage + 6;
		monster.mMinDamage2 = 4 * monster.mMinDamage2 + 6;
		monster.mMaxDamage2 = 4 * monster.mMaxDamage2 + 6;
	}

	char filestr[64];
	sprintf(filestr, "Monsters\\Monsters\\%s.TRN", uniqueMonsterData.mTrnName);
	LoadFileInMem(filestr, &LightTables[256 * (uniquetrans + 19)], 256);

	monster._uniqtrans = uniquetrans++;

	if (uniqueMonsterData.customHitpoints != 0) {
		monster.mHit = uniqueMonsterData.customHitpoints;
		monster.mHit2 = uniqueMonsterData.customHitpoints;

		if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
			monster.mHit += NIGHTMARE_TO_HIT_BONUS;
			monster.mHit2 += NIGHTMARE_TO_HIT_BONUS;
		} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
			monster.mHit += HELL_TO_HIT_BONUS;
			monster.mHit2 += HELL_TO_HIT_BONUS;
		}
	}
	if (uniqueMonsterData.customArmorClass != 0) {
		monster.mArmorClass = uniqueMonsterData.customArmorClass;

		if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
			monster.mArmorClass += NIGHTMARE_AC_BONUS;
		} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
			monster.mArmorClass += HELL_AC_BONUS;
		}
	}

	ActiveMonsterCount++;

	if (uniqueMonsterData.monsterPack != UniqueMonsterPack::None) {
		PlaceGroup(miniontype, bosspacksize, uniqueMonsterData.monsterPack, ActiveMonsterCount - 1);
	}

	if (monster._mAi != AI_GARG) {
		monster.ChangeAnimationData(MonsterGraphic::Stand);
		monster.AnimInfo.CurrentFrame = GenerateRnd(monster.AnimInfo.NumberOfFrames - 1) + 1;
		monster._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		monster._mmode = MonsterMode::Stand;
	}
}

int AddMonsterType(_monster_id type, placeflag placeflag)
{
	bool done = false;
	int i;

	for (i = 0; i < LevelMonsterTypeCount && !done; i++) {
		done = LevelMonsterTypes[i].mtype == type;
	}

	i--;

	if (!done) {
		i = LevelMonsterTypeCount;
		LevelMonsterTypeCount++;
		LevelMonsterTypes[i].mtype = type;
		monstimgtot += MonstersData[type].mImage;
		InitMonsterGFX(i);
		InitMonsterSND(i);
	}

	LevelMonsterTypes[i].mPlaceFlags |= placeflag;
	return i;
}

void ClearMVars(Monster &monster)
{
	monster._mVar1 = 0;
	monster._mVar2 = 0;
	monster._mVar3 = 0;
	monster.position.temp = { 0, 0 };
	monster.position.offset2 = { 0, 0 };
}

void ClrAllMonsters()
{
	for (auto &monster : Monsters) {
		ClearMVars(monster);
		monster.mName = "Invalid Monster";
		monster._mgoal = MGOAL_NONE;
		monster._mmode = MonsterMode::Stand;
		monster._mVar1 = 0;
		monster._mVar2 = 0;
		monster.position.tile = { 0, 0 };
		monster.position.future = { 0, 0 };
		monster.position.old = { 0, 0 };
		monster._mdir = static_cast<Direction>(GenerateRnd(8));
		monster.position.velocity = { 0, 0 };
		monster.AnimInfo = {};
		monster._mFlags = 0;
		monster._mDelFlag = false;
		monster._menemy = GenerateRnd(gbActivePlayers);
		monster.enemyPosition = Players[monster._menemy].position.future;
	}
}

void PlaceUniqueMonsters()
{
	for (int u = 0; UniqueMonstersData[u].mtype != -1; u++) {
		if (UniqueMonstersData[u].mlevel != currlevel)
			continue;
		bool done = false;
		int mt;
		for (mt = 0; mt < LevelMonsterTypeCount; mt++) {
			done = (LevelMonsterTypes[mt].mtype == UniqueMonstersData[u].mtype);
			if (done)
				break;
		}
		if (u == UMT_GARBUD && Quests[Q_GARBUD]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UMT_ZHAR && Quests[Q_ZHAR]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UMT_SNOTSPIL && Quests[Q_LTBANNER]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UMT_LACHDAN && Quests[Q_VEIL]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UMT_WARLORD && Quests[Q_WARLORD]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (done)
			PlaceUniqueMonst(u, mt, 8);
	}
}

void PlaceQuestMonsters()
{
	int skeltype;

	if (!setlevel) {
		if (Quests[Q_BUTCHER].IsAvailable()) {
			PlaceUniqueMonst(UMT_BUTCHER, 0, 0);
		}

		if (currlevel == Quests[Q_SKELKING]._qlevel && gbIsMultiplayer) {
			skeltype = 0;

			for (skeltype = 0; skeltype < LevelMonsterTypeCount; skeltype++) {
				if (IsSkel(LevelMonsterTypes[skeltype].mtype)) {
					break;
				}
			}

			PlaceUniqueMonst(UMT_SKELKING, skeltype, 30);
		}

		if (Quests[Q_LTBANNER].IsAvailable()) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L1Data\\Banner1.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
		}
		if (Quests[Q_BLOOD].IsAvailable()) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blood2.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
		}
		if (Quests[Q_BLIND].IsAvailable()) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blind2.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
		}
		if (Quests[Q_ANVIL].IsAvailable()) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L3Data\\Anvil.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x + 1, setpc_y + 1 } * 2);
		}
		if (Quests[Q_WARLORD].IsAvailable()) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\Warlord.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
			AddMonsterType(UniqueMonstersData[UMT_WARLORD].mtype, PLACE_SCATTER);
		}
		if (Quests[Q_VEIL].IsAvailable()) {
			AddMonsterType(UniqueMonstersData[UMT_LACHDAN].mtype, PLACE_SCATTER);
		}
		if (Quests[Q_ZHAR].IsAvailable() && zharlib == -1) {
			Quests[Q_ZHAR]._qactive = QUEST_NOTAVAIL;
		}

		if (currlevel == Quests[Q_BETRAYER]._qlevel && gbIsMultiplayer) {
			AddMonsterType(UniqueMonstersData[UMT_LAZARUS].mtype, PLACE_UNIQUE);
			AddMonsterType(UniqueMonstersData[UMT_RED_VEX].mtype, PLACE_UNIQUE);
			PlaceUniqueMonst(UMT_LAZARUS, 0, 0);
			PlaceUniqueMonst(UMT_RED_VEX, 0, 0);
			PlaceUniqueMonst(UMT_BLACKJADE, 0, 0);
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\Vile1.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
		}

		if (currlevel == 24) {
			UberDiabloMonsterIndex = -1;
			int i1;
			for (i1 = 0; i1 < LevelMonsterTypeCount; i1++) {
				if (LevelMonsterTypes[i1].mtype == UniqueMonstersData[UMT_NAKRUL].mtype)
					break;
			}

			if (i1 < LevelMonsterTypeCount) {
				for (int i2 = 0; i2 < ActiveMonsterCount; i2++) {
					auto &monster = Monsters[i2];
					if (monster._uniqtype != 0 || monster._mMTidx == i1) {
						UberDiabloMonsterIndex = i2;
						break;
					}
				}
			}
			if (UberDiabloMonsterIndex == -1)
				PlaceUniqueMonst(UMT_NAKRUL, 0, 0);
		}
	} else if (setlvlnum == SL_SKELKING) {
		PlaceUniqueMonst(UMT_SKELKING, 0, 0);
	}
}

void LoadDiabMonsts()
{
	{
		auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\diab1.DUN");
		SetMapMonsters(dunData.get(), Point { diabquad1x, diabquad1y } * 2);
	}
	{
		auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\diab2a.DUN");
		SetMapMonsters(dunData.get(), Point { diabquad2x, diabquad2y } * 2);
	}
	{
		auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\diab3a.DUN");
		SetMapMonsters(dunData.get(), Point { diabquad3x, diabquad3y } * 2);
	}
	{
		auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\diab4a.DUN");
		SetMapMonsters(dunData.get(), Point { diabquad4x, diabquad4y } * 2);
	}
}

void DeleteMonster(int i)
{
	const auto &monster = Monsters[ActiveMonsters[i]];
	if ((monster._mFlags & MFLAG_BERSERK) != 0) {
		AddUnLight(monster.mlid);
	}

	ActiveMonsterCount--;
	std::swap(ActiveMonsters[i], ActiveMonsters[ActiveMonsterCount]); // This ensures alive monsters are before ActiveMonsterCount in the array and any deleted monster after
}

void NewMonsterAnim(Monster &monster, MonsterGraphic graphic, Direction md, AnimationDistributionFlags flags = AnimationDistributionFlags::None, int numSkippedFrames = 0, int distributeFramesBeforeFrame = 0)
{
	const auto &animData = monster.MType->GetAnimData(graphic);
	const auto *pCelSprite = &*animData.CelSpritesForDirections[static_cast<size_t>(md)];
	monster.AnimInfo.SetNewAnimation(pCelSprite, animData.Frames, animData.Rate, flags, numSkippedFrames, distributeFramesBeforeFrame);
	monster._mFlags &= ~(MFLAG_LOCK_ANIMATION | MFLAG_ALLOW_SPECIAL);
	monster._mdir = md;
}

void StartMonsterGotHit(int monsterId)
{
	auto &monster = Monsters[monsterId];
	if (monster.MType->mtype != MT_GOLEM) {
		auto animationFlags = gGameLogicStep < GameLogicStep::ProcessMonsters ? AnimationDistributionFlags::ProcessAnimationPending : AnimationDistributionFlags::None;
		int numSkippedFrames = (gbIsHellfire && monster.MType->mtype == MT_DIABLO) ? 4 : 0;
		NewMonsterAnim(monster, MonsterGraphic::GotHit, monster._mdir, animationFlags, numSkippedFrames);
		monster._mmode = MonsterMode::HitRecovery;
	}
	monster.position.offset = { 0, 0 };
	monster.position.tile = monster.position.old;
	monster.position.future = monster.position.old;
	M_ClearSquares(monsterId);
	dMonster[monster.position.tile.x][monster.position.tile.y] = monsterId + 1;
}

bool IsRanged(Monster &monster)
{
	return IsAnyOf(monster._mAi, AI_SKELBOW, AI_GOATBOW, AI_SUCC, AI_LAZHELP);
}

void UpdateEnemy(Monster &monster)
{
	Point target;
	int menemy = -1;
	int bestDist = -1;
	bool bestsameroom = false;
	const auto &position = monster.position.tile;
	if ((monster._mFlags & MFLAG_BERSERK) != 0 || (monster._mFlags & MFLAG_GOLEM) == 0) {
		for (int pnum = 0; pnum < MAX_PLRS; pnum++) {
			auto &player = Players[pnum];
			if (!player.plractive || currlevel != player.plrlevel || player._pLvlChanging
			    || (((player._pHitPoints >> 6) == 0) && gbIsMultiplayer))
				continue;
			bool sameroom = (dTransVal[position.x][position.y] == dTransVal[player.position.tile.x][player.position.tile.y]);
			int dist = position.WalkingDistance(player.position.tile);
			if ((sameroom && !bestsameroom)
			    || ((sameroom || !bestsameroom) && dist < bestDist)
			    || (menemy == -1)) {
				monster._mFlags &= ~MFLAG_TARGETS_MONSTER;
				menemy = pnum;
				target = player.position.future;
				bestDist = dist;
				bestsameroom = sameroom;
			}
		}
	}
	for (int j = 0; j < ActiveMonsterCount; j++) {
		int mi = ActiveMonsters[j];
		auto &otherMonster = Monsters[mi];
		if (&otherMonster == &monster)
			continue;
		if ((otherMonster._mhitpoints >> 6) <= 0)
			continue;
		if (otherMonster.position.tile == GolemHoldingCell)
			continue;
		if (M_Talker(otherMonster) && otherMonster.mtalkmsg != TEXT_NONE)
			continue;
		bool isBerserked = (monster._mFlags & MFLAG_BERSERK) != 0 || (otherMonster._mFlags & MFLAG_BERSERK) != 0;
		if ((monster._mFlags & MFLAG_GOLEM) != 0 && (otherMonster._mFlags & MFLAG_GOLEM) != 0 && !isBerserked) // prevent golems from fighting each other
			continue;

		int dist = otherMonster.position.tile.WalkingDistance(position);
		if (((monster._mFlags & MFLAG_GOLEM) == 0
		        && (monster._mFlags & MFLAG_BERSERK) == 0
		        && dist >= 2
		        && !IsRanged(monster))
		    || ((monster._mFlags & MFLAG_GOLEM) == 0
		        && (monster._mFlags & MFLAG_BERSERK) == 0
		        && (otherMonster._mFlags & MFLAG_GOLEM) == 0)) {
			continue;
		}
		bool sameroom = dTransVal[position.x][position.y] == dTransVal[otherMonster.position.tile.x][otherMonster.position.tile.y];
		if ((sameroom && !bestsameroom)
		    || ((sameroom || !bestsameroom) && dist < bestDist)
		    || (menemy == -1)) {
			monster._mFlags |= MFLAG_TARGETS_MONSTER;
			menemy = mi;
			target = otherMonster.position.future;
			bestDist = dist;
			bestsameroom = sameroom;
		}
	}
	if (menemy != -1) {
		monster._mFlags &= ~MFLAG_NO_ENEMY;
		monster._menemy = menemy;
		monster.enemyPosition = target;
	} else {
		monster._mFlags |= MFLAG_NO_ENEMY;
	}
}

/**
 * @brief Make the AI wait a bit before thinking again
 * @param len
 */
void AiDelay(Monster &monster, int len)
{
	if (len <= 0) {
		return;
	}

	if (monster._mAi == AI_LAZARUS) {
		return;
	}

	monster._mVar2 = len;
	monster._mmode = MonsterMode::Delay;
}

/**
 * @brief Get the direction from the monster to its current enemy
 */
Direction GetMonsterDirection(Monster &monster)
{
	return GetDirection(monster.position.tile, monster.enemyPosition);
}

void StartSpecialStand(Monster &monster, Direction md)
{
	NewMonsterAnim(monster, MonsterGraphic::Special, md);
	monster._mmode = MonsterMode::SpecialStand;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
}

void StartWalk(int i, int xvel, int yvel, int xadd, int yadd, Direction endDir)
{
	auto &monster = Monsters[i];

	int fx = xadd + monster.position.tile.x;
	int fy = yadd + monster.position.tile.y;

	dMonster[fx][fy] = -(i + 1);
	monster._mmode = MonsterMode::MoveNorthwards;
	monster.position.old = monster.position.tile;
	monster.position.future = { fx, fy };
	monster.position.velocity = { xvel, yvel };
	monster._mVar1 = xadd;
	monster._mVar2 = yadd;
	monster._mVar3 = static_cast<int>(endDir);
	NewMonsterAnim(monster, MonsterGraphic::Walk, endDir, AnimationDistributionFlags::ProcessAnimationPending, -1);
	monster.position.offset2 = { 0, 0 };
}

void StartWalk2(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, Direction endDir)
{
	auto &monster = Monsters[i];

	int fx = xadd + monster.position.tile.x;
	int fy = yadd + monster.position.tile.y;

	dMonster[monster.position.tile.x][monster.position.tile.y] = -(i + 1);
	monster._mVar1 = monster.position.tile.x;
	monster._mVar2 = monster.position.tile.y;
	monster.position.old = monster.position.tile;
	monster.position.tile = { fx, fy };
	monster.position.future = { fx, fy };
	dMonster[fx][fy] = i + 1;
	if (monster.mlid != NO_LIGHT)
		ChangeLightXY(monster.mlid, monster.position.tile);
	monster.position.offset = { xoff, yoff };
	monster._mmode = MonsterMode::MoveSouthwards;
	monster.position.velocity = { xvel, yvel };
	monster._mVar3 = static_cast<int>(endDir);
	NewMonsterAnim(monster, MonsterGraphic::Walk, endDir, AnimationDistributionFlags::ProcessAnimationPending, -1);
	monster.position.offset2 = { 16 * xoff, 16 * yoff };
}

void StartWalk3(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, Direction endDir)
{
	auto &monster = Monsters[i];

	int fx = xadd + monster.position.tile.x;
	int fy = yadd + monster.position.tile.y;
	int x = mapx + monster.position.tile.x;
	int y = mapy + monster.position.tile.y;

	if (monster.mlid != NO_LIGHT)
		ChangeLightXY(monster.mlid, { x, y });

	dMonster[monster.position.tile.x][monster.position.tile.y] = -(i + 1);
	dMonster[fx][fy] = i + 1;
	monster.position.temp = { x, y };
	monster.position.old = monster.position.tile;
	monster.position.future = { fx, fy };
	monster.position.offset = { xoff, yoff };
	monster._mmode = MonsterMode::MoveSideways;
	monster.position.velocity = { xvel, yvel };
	monster._mVar1 = fx;
	monster._mVar2 = fy;
	monster._mVar3 = static_cast<int>(endDir);
	NewMonsterAnim(monster, MonsterGraphic::Walk, endDir, AnimationDistributionFlags::ProcessAnimationPending, -1);
	monster.position.offset2 = { 16 * xoff, 16 * yoff };
}

void StartAttack(Monster &monster)
{
	Direction md = GetMonsterDirection(monster);
	NewMonsterAnim(monster, MonsterGraphic::Attack, md, AnimationDistributionFlags::ProcessAnimationPending);
	monster._mmode = MonsterMode::MeleeAttack;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
}

void StartRangedAttack(Monster &monster, missile_id missileType, int dam)
{
	Direction md = GetMonsterDirection(monster);
	NewMonsterAnim(monster, MonsterGraphic::Attack, md, AnimationDistributionFlags::ProcessAnimationPending);
	monster._mmode = MonsterMode::RangedAttack;
	monster._mVar1 = missileType;
	monster._mVar2 = dam;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
}

void StartRangedSpecialAttack(Monster &monster, missile_id missileType, int dam)
{
	Direction md = GetMonsterDirection(monster);
	int distributeFramesBeforeFrame = 0;
	if (monster._mAi == AI_MEGA)
		distributeFramesBeforeFrame = monster.MData->mAFNum2;
	NewMonsterAnim(monster, MonsterGraphic::Special, md, AnimationDistributionFlags::ProcessAnimationPending, 0, distributeFramesBeforeFrame);
	monster._mmode = MonsterMode::SpecialRangedAttack;
	monster._mVar1 = missileType;
	monster._mVar2 = 0;
	monster._mVar3 = dam;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
}

void StartSpecialAttack(Monster &monster)
{
	Direction md = GetMonsterDirection(monster);
	NewMonsterAnim(monster, MonsterGraphic::Special, md);
	monster._mmode = MonsterMode::SpecialMeleeAttack;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
}

void StartEating(Monster &monster)
{
	NewMonsterAnim(monster, MonsterGraphic::Special, monster._mdir);
	monster._mmode = MonsterMode::SpecialMeleeAttack;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
}

void DiabloDeath(Monster &diablo, bool sendmsg)
{
	PlaySFX(USFX_DIABLOD);
	auto &quest = Quests[Q_DIABLO];
	quest._qactive = QUEST_DONE;
	if (sendmsg)
		NetSendCmdQuest(true, quest);
	sgbSaveSoundOn = gbSoundOn;
	gbProcessPlayers = false;
	for (int j = 0; j < ActiveMonsterCount; j++) {
		int k = ActiveMonsters[j];
		auto &monster = Monsters[k];
		if (monster.MType->mtype == MT_DIABLO || diablo._msquelch == 0)
			continue;

		NewMonsterAnim(monster, MonsterGraphic::Death, monster._mdir);
		monster._mmode = MonsterMode::Death;
		monster.position.offset = { 0, 0 };
		monster._mVar1 = 0;
		monster.position.tile = monster.position.old;
		monster.position.future = monster.position.tile;
		M_ClearSquares(k);
		dMonster[monster.position.tile.x][monster.position.tile.y] = k + 1;
	}
	AddLight(diablo.position.tile, 8);
	DoVision(diablo.position.tile, 8, MAP_EXP_NONE, true);
	int dist = diablo.position.tile.WalkingDistance(ViewPosition);
	if (dist > 20)
		dist = 20;
	diablo._mVar3 = ViewPosition.x << 16;
	diablo.position.temp.x = ViewPosition.y << 16;
	diablo.position.temp.y = (int)((diablo._mVar3 - (diablo.position.tile.x << 16)) / (double)dist);
	diablo.position.offset2.deltaX = (int)((diablo.position.temp.x - (diablo.position.tile.y << 16)) / (double)dist);
}

void SpawnLoot(Monster &monster, bool sendmsg)
{
	if (Quests[Q_GARBUD].IsAvailable() && monster._uniqtype - 1 == UMT_GARBUD) {
		CreateTypeItem(monster.position.tile + Displacement { 1, 1 }, true, ItemType::Mace, IMISC_NONE, true, false);
	} else if (monster._uniqtype - 1 == UMT_DEFILER) {
		if (effect_is_playing(USFX_DEFILER8))
			stream_stop();
		Quests[Q_DEFILER]._qlog = false;
		SpawnMapOfDoom(monster.position.tile);
	} else if (monster._uniqtype - 1 == UMT_HORKDMN) {
		if (sgGameInitInfo.bTheoQuest != 0) {
			SpawnTheodore(monster.position.tile);
		} else {
			CreateAmulet(monster.position.tile, 13, false, true);
		}
	} else if (monster.MType->mtype == MT_HORKSPWN) {
	} else if (monster.MType->mtype == MT_NAKRUL) {
		int nSFX = IsUberRoomOpened ? USFX_NAKRUL4 : USFX_NAKRUL5;
		if (sgGameInitInfo.bCowQuest != 0)
			nSFX = USFX_NAKRUL6;
		if (effect_is_playing(nSFX))
			stream_stop();
		Quests[Q_NAKRUL]._qlog = false;
		UberDiabloMonsterIndex = -2;
		CreateMagicWeapon(monster.position.tile, ItemType::Sword, ICURS_GREAT_SWORD, false, true);
		CreateMagicWeapon(monster.position.tile, ItemType::Staff, ICURS_WAR_STAFF, false, true);
		CreateMagicWeapon(monster.position.tile, ItemType::Bow, ICURS_LONG_WAR_BOW, false, true);
		CreateSpellBook(monster.position.tile, SPL_APOCA, false, true);
	} else if (monster.MType->mtype != MT_GOLEM) {
		SpawnItem(monster, monster.position.tile, sendmsg);
	}
}

void Teleport(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode == MonsterMode::Petrified)
		return;

	int mx = monster.enemyPosition.x;
	int my = monster.enemyPosition.y;
	int rx = 2 * GenerateRnd(2) - 1;
	int ry = 2 * GenerateRnd(2) - 1;

	bool done = false;

	int x;
	int y;
	for (int j = -1; j <= 1 && !done; j++) {
		for (int k = -1; k < 1 && !done; k++) {
			if (j != 0 || k != 0) {
				x = mx + rx * j;
				y = my + ry * k;
				if (InDungeonBounds({ x, y }) && x != monster.position.tile.x && y != monster.position.tile.y) {
					if (IsTileAvailable(monster, { x, y }))
						done = true;
				}
			}
		}
	}

	if (done) {
		M_ClearSquares(i);
		dMonster[monster.position.tile.x][monster.position.tile.y] = 0;
		dMonster[x][y] = i + 1;
		monster.position.old = { x, y };
		monster._mdir = GetMonsterDirection(monster);

		if (monster.mlid != NO_LIGHT) {
			ChangeLightXY(monster.mlid, { x, y });
		}
	}
}

void MonsterHitMonster(int mid, int i, int dam)
{
	assert(mid >= 0 && mid < MAXMONSTERS);
	auto &monster = Monsters[mid];
	assert(monster.MType != nullptr);

	if (i >= 0 && i < MAX_PLRS)
		monster.mWhoHit |= 1 << i;

	delta_monster_hp(mid, monster._mhitpoints, currlevel);
	NetSendCmdMonDmg(false, mid, dam);
	PlayEffect(monster, 1);

	if ((monster.MType->mtype >= MT_SNEAK && monster.MType->mtype <= MT_ILLWEAV) || dam >> 6 >= monster.mLevel + 3) {
		if (i >= 0)
			monster._mdir = Opposite(Monsters[i]._mdir);

		if (monster.MType->mtype == MT_BLINK) {
			Teleport(mid);
		} else if ((monster.MType->mtype >= MT_NSCAV && monster.MType->mtype <= MT_YSCAV)
		    || monster.MType->mtype == MT_GRAVEDIG) {
			monster._mgoal = MGOAL_NORMAL;
			monster._mgoalvar1 = 0;
			monster._mgoalvar2 = 0;
		}

		if (monster._mmode != MonsterMode::Petrified) {
			StartMonsterGotHit(mid);
		}
	}
}

void StartMonsterDeath(int i, int pnum, bool sendmsg)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	assert(monster.MType != nullptr);

	if (pnum >= 0)
		monster.mWhoHit |= 1 << pnum;
	if (pnum < MAX_PLRS && i >= MAX_PLRS) /// BUGFIX: i >= MAX_PLRS (fixed)
		AddPlrMonstExper(monster.mLevel, monster.mExp, monster.mWhoHit);
	MonsterKillCounts[monster.MType->mtype]++;
	monster._mhitpoints = 0;
	SetRndSeed(monster._mRndSeed);
	SpawnLoot(monster, sendmsg);
	if (monster.MType->mtype == MT_DIABLO)
		DiabloDeath(monster, true);
	else
		PlayEffect(monster, 2);

	Direction md = pnum >= 0 ? GetMonsterDirection(monster) : monster._mdir;
	NewMonsterAnim(monster, MonsterGraphic::Death, md, gGameLogicStep < GameLogicStep::ProcessMonsters ? AnimationDistributionFlags::ProcessAnimationPending : AnimationDistributionFlags::None);
	monster._mmode = MonsterMode::Death;
	monster._mgoal = MGOAL_NONE;
	monster.position.offset = { 0, 0 };
	monster._mVar1 = 0;
	monster.position.tile = monster.position.old;
	monster.position.future = monster.position.old;
	M_ClearSquares(i);
	dMonster[monster.position.tile.x][monster.position.tile.y] = i + 1;
	CheckQuestKill(monster, sendmsg);
	M_FallenFear(monster.position.tile);
	if ((monster.MType->mtype >= MT_NACID && monster.MType->mtype <= MT_XACID) || monster.MType->mtype == MT_SPIDLORD)
		AddMissile(monster.position.tile, { 0, 0 }, Direction::South, MIS_ACIDPUD, TARGET_PLAYERS, i, monster._mint + 1, 0);
}

void StartDeathFromMonster(int i, int mid)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &killer = Monsters[i];
	assert(mid >= 0 && mid < MAXMONSTERS);
	auto &monster = Monsters[mid];
	assert(monster.MType != nullptr);

	delta_kill_monster(mid, monster.position.tile, currlevel);
	NetSendCmdLocParam1(false, CMD_MONSTDEATH, monster.position.tile, mid);

	if (i < MAX_PLRS) {
		monster.mWhoHit |= 1 << i;
		if (mid >= MAX_PLRS)
			AddPlrMonstExper(monster.mLevel, monster.mExp, monster.mWhoHit);
	}

	MonsterKillCounts[monster.MType->mtype]++;
	monster._mhitpoints = 0;
	SetRndSeed(monster._mRndSeed);

	SpawnLoot(monster, true);

	if (monster.MType->mtype == MT_DIABLO)
		DiabloDeath(monster, true);
	else
		PlayEffect(monster, 2);

	Direction md = Opposite(killer._mdir);
	if (monster.MType->mtype == MT_GOLEM)
		md = Direction::South;

	NewMonsterAnim(monster, MonsterGraphic::Death, md, gGameLogicStep < GameLogicStep::ProcessMonsters ? AnimationDistributionFlags::ProcessAnimationPending : AnimationDistributionFlags::None);
	monster._mmode = MonsterMode::Death;
	monster.position.offset = { 0, 0 };
	monster.position.tile = monster.position.old;
	monster.position.future = monster.position.old;
	M_ClearSquares(mid);
	dMonster[monster.position.tile.x][monster.position.tile.y] = mid + 1;
	CheckQuestKill(monster, true);
	M_FallenFear(monster.position.tile);
	if (monster.MType->mtype >= MT_NACID && monster.MType->mtype <= MT_XACID)
		AddMissile(monster.position.tile, { 0, 0 }, Direction::South, MIS_ACIDPUD, TARGET_PLAYERS, mid, monster._mint + 1, 0);

	if (gbIsHellfire)
		M_StartStand(killer, killer._mdir);
}

void StartFadein(Monster &monster, Direction md, bool backwards)
{
	NewMonsterAnim(monster, MonsterGraphic::Special, md);
	monster._mmode = MonsterMode::FadeIn;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
	monster._mFlags &= ~MFLAG_HIDDEN;
	if (backwards) {
		monster._mFlags |= MFLAG_LOCK_ANIMATION;
		monster.AnimInfo.CurrentFrame = monster.AnimInfo.NumberOfFrames;
	}
}

void StartFadeout(Monster &monster, Direction md, bool backwards)
{
	NewMonsterAnim(monster, MonsterGraphic::Special, md);
	monster._mmode = MonsterMode::FadeOut;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
	if (backwards) {
		monster._mFlags |= MFLAG_LOCK_ANIMATION;
		monster.AnimInfo.CurrentFrame = monster.AnimInfo.NumberOfFrames;
	}
}

void StartHeal(Monster &monster)
{
	monster.ChangeAnimationData(MonsterGraphic::Special);
	monster.AnimInfo.CurrentFrame = monster.MType->GetAnimData(MonsterGraphic::Special).Frames;
	monster._mFlags |= MFLAG_LOCK_ANIMATION;
	monster._mmode = MonsterMode::Heal;
	monster._mVar1 = monster._mmaxhp / (16 * (GenerateRnd(5) + 4));
}

void SyncLightPosition(Monster &monster)
{
	int lx = (monster.position.offset.deltaX + 2 * monster.position.offset.deltaY) / 8;
	int ly = (2 * monster.position.offset.deltaY - monster.position.offset.deltaX) / 8;

	if (monster.mlid != NO_LIGHT)
		ChangeLightOffset(monster.mlid, { lx, ly });
}

bool MonsterIdle(Monster &monster)
{
	if (monster.MType->mtype == MT_GOLEM)
		monster.ChangeAnimationData(MonsterGraphic::Walk);
	else
		monster.ChangeAnimationData(MonsterGraphic::Stand);

	if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames)
		UpdateEnemy(monster);

	monster._mVar2++;

	return false;
}

/**
 * @brief Continue movement towards new tile
 */
bool MonsterWalk(int i, MonsterMode variant)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	assert(monster.MType != nullptr);

	// Check if we reached new tile
	bool isAnimationEnd = monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames;
	if (isAnimationEnd) {
		switch (variant) {
		case MonsterMode::MoveNorthwards:
			dMonster[monster.position.tile.x][monster.position.tile.y] = 0;
			monster.position.tile.x += monster._mVar1;
			monster.position.tile.y += monster._mVar2;
			dMonster[monster.position.tile.x][monster.position.tile.y] = i + 1;
			break;
		case MonsterMode::MoveSouthwards:
			dMonster[monster._mVar1][monster._mVar2] = 0;
			break;
		case MonsterMode::MoveSideways:
			dMonster[monster.position.tile.x][monster.position.tile.y] = 0;
			monster.position.tile = { monster._mVar1, monster._mVar2 };
			// dMonster is set here for backwards comparability, without it the monster would be invisible if loaded from a vanilla save.
			dMonster[monster.position.tile.x][monster.position.tile.y] = i + 1;
			break;
		default:
			break;
		}
		if (monster.mlid != NO_LIGHT)
			ChangeLightXY(monster.mlid, monster.position.tile);
		M_StartStand(monster, monster._mdir);
	} else { // We didn't reach new tile so update monster's "sub-tile" position
		if (monster.AnimInfo.TickCounterOfCurrentFrame == 0) {
			if (monster.AnimInfo.CurrentFrame == 0 && monster.MType->mtype == MT_FLESTHNG)
				PlayEffect(monster, 3);
			monster.position.offset2 += monster.position.velocity;
			monster.position.offset.deltaX = monster.position.offset2.deltaX >> 4;
			monster.position.offset.deltaY = monster.position.offset2.deltaY >> 4;
		}
	}

	if (monster.mlid != NO_LIGHT) // BUGFIX: change uniqtype check to mlid check like it is in all other places (fixed)
		SyncLightPosition(monster);

	return isAnimationEnd;
}

void MonsterAttackMonster(int i, int mid, int hper, int mind, int maxd)
{
	assert(mid >= 0 && mid < MAXMONSTERS);
	auto &monster = Monsters[mid];
	assert(monster.MType != nullptr);

	if (monster._mhitpoints >> 6 > 0 && (monster.MType->mtype != MT_ILLWEAV || monster._mgoal != MGOAL_RETREAT)) {
		int hit = GenerateRnd(100);
		if (monster._mmode == MonsterMode::Petrified)
			hit = 0;
		bool unused;
		if (!CheckMonsterHit(monster, &unused) && hit < hper) {
			int dam = (mind + GenerateRnd(maxd - mind + 1)) << 6;
			monster._mhitpoints -= dam;
			if (monster._mhitpoints >> 6 <= 0) {
				if (monster._mmode == MonsterMode::Petrified) {
					StartDeathFromMonster(i, mid);
					monster.Petrify();
				} else {
					StartDeathFromMonster(i, mid);
				}
			} else {
				if (monster._mmode == MonsterMode::Petrified) {
					MonsterHitMonster(mid, i, dam);
					monster.Petrify();
				} else {
					MonsterHitMonster(mid, i, dam);
				}
			}
		}
	}
}

void CheckReflect(int mon, int pnum, int dam)
{
	auto &monster = Monsters[mon];
	auto &player = Players[pnum];

	player.wReflections--;
	if (player.wReflections <= 0)
		NetSendCmdParam1(true, CMD_SETREFLECT, 0);
	// reflects 20-30% damage
	int mdam = dam * (GenerateRnd(10) + 20L) / 100;
	monster._mhitpoints -= mdam;
	dam = std::max(dam - mdam, 0);
	if (monster._mhitpoints >> 6 <= 0)
		M_StartKill(mon, pnum);
	else
		M_StartHit(mon, pnum, mdam);
}

void MonsterAttackPlayer(int i, int pnum, int hit, int minDam, int maxDam)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	assert(monster.MType != nullptr);

	if ((monster._mFlags & MFLAG_TARGETS_MONSTER) != 0) {
		MonsterAttackMonster(i, pnum, hit, minDam, maxDam);
		return;
	}

	auto &player = Players[pnum];

	if (player._pHitPoints >> 6 <= 0 || player._pInvincible || (player._pSpellFlags & 1) != 0)
		return;
	if (monster.position.tile.WalkingDistance(player.position.tile) >= 2)
		return;

	int hper = GenerateRnd(100);
#ifdef _DEBUG
	if (DebugGodMode)
		hper = 1000;
#endif
	int ac = player.GetArmor();
	if ((player.pDamAcFlags & ISPLHF_ACDEMON) != 0 && monster.MData->mMonstClass == MonsterClass::Demon)
		ac += 40;
	if ((player.pDamAcFlags & ISPLHF_ACUNDEAD) != 0 && monster.MData->mMonstClass == MonsterClass::Undead)
		ac += 20;
	hit += 2 * (monster.mLevel - player._pLevel)
	    + 30
	    - ac;
	int minhit = 15;
	if (currlevel == 14)
		minhit = 20;
	if (currlevel == 15)
		minhit = 25;
	if (currlevel == 16)
		minhit = 30;
	hit = std::max(hit, minhit);
	int blkper = 100;
	if ((player._pmode == PM_STAND || player._pmode == PM_ATTACK) && player._pBlockFlag) {
		blkper = GenerateRnd(100);
	}
	int blk = player.GetBlockChance() - (monster.mLevel * 2);
	blk = clamp(blk, 0, 100);
	if (hper >= hit)
		return;
	if (blkper < blk) {
		Direction dir = GetDirection(player.position.tile, monster.position.tile);
		StartPlrBlock(pnum, dir);
		if (pnum == MyPlayerId && player.wReflections > 0) {
			int dam = GenerateRnd((maxDam - minDam + 1) << 6) + (minDam << 6);
			dam = std::max(dam + (player._pIGetHit << 6), 64);
			CheckReflect(i, pnum, dam);
		}
		return;
	}
	if (monster.MType->mtype == MT_YZOMBIE && pnum == MyPlayerId) {
		if (player._pMaxHP > 64) {
			if (player._pMaxHPBase > 64) {
				player._pMaxHP -= 64;
				if (player._pHitPoints > player._pMaxHP) {
					player._pHitPoints = player._pMaxHP;
				}
				player._pMaxHPBase -= 64;
				if (player._pHPBase > player._pMaxHPBase) {
					player._pHPBase = player._pMaxHPBase;
				}
			}
		}
	}
	int dam = (minDam << 6) + GenerateRnd((maxDam - minDam + 1) << 6);
	dam = std::max(dam + (player._pIGetHit << 6), 64);
	if (pnum == MyPlayerId) {
		if (player.wReflections > 0)
			CheckReflect(i, pnum, dam);
		ApplyPlrDamage(pnum, 0, 0, dam);
	}
	if ((player._pIFlags & ISPL_THORNS) != 0) {
		int mdam = (GenerateRnd(3) + 1) << 6;
		monster._mhitpoints -= mdam;
		if (monster._mhitpoints >> 6 <= 0)
			M_StartKill(i, pnum);
		else
			M_StartHit(i, pnum, mdam);
	}
	if ((monster._mFlags & MFLAG_NOLIFESTEAL) == 0 && monster.MType->mtype == MT_SKING && gbIsMultiplayer)
		monster._mhitpoints += dam;
	if (player._pHitPoints >> 6 <= 0) {
		if (gbIsHellfire)
			M_StartStand(monster, monster._mdir);
		return;
	}
	StartPlrHit(pnum, dam, false);
	if ((monster._mFlags & MFLAG_KNOCKBACK) != 0) {
		if (player._pmode != PM_GOTHIT)
			StartPlrHit(pnum, 0, true);

		Point newPosition = player.position.tile + monster._mdir;
		if (PosOkPlayer(player, newPosition)) {
			player.position.tile = newPosition;
			FixPlayerLocation(pnum, player._pdir);
			FixPlrWalkTags(pnum);
			dPlayer[newPosition.x][newPosition.y] = pnum + 1;
			SetPlayerOld(player);
		}
	}
}

bool MonsterAttack(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	assert(monster.MType != nullptr);
	assert(monster.MData != nullptr);

	if (monster.AnimInfo.CurrentFrame == monster.MData->mAFNum) {
		MonsterAttackPlayer(i, monster._menemy, monster.mHit, monster.mMinDamage, monster.mMaxDamage);
		if (monster._mAi != AI_SNAKE)
			PlayEffect(monster, 0);
	}
	if (monster.MType->mtype >= MT_NMAGMA && monster.MType->mtype <= MT_WMAGMA && monster.AnimInfo.CurrentFrame == 9) {
		MonsterAttackPlayer(i, monster._menemy, monster.mHit + 10, monster.mMinDamage - 2, monster.mMaxDamage - 2);
		PlayEffect(monster, 0);
	}
	if (monster.MType->mtype >= MT_STORM && monster.MType->mtype <= MT_MAEL && monster.AnimInfo.CurrentFrame == 13) {
		MonsterAttackPlayer(i, monster._menemy, monster.mHit - 20, monster.mMinDamage + 4, monster.mMaxDamage + 4);
		PlayEffect(monster, 0);
	}
	if (monster._mAi == AI_SNAKE && monster.AnimInfo.CurrentFrame == 1)
		PlayEffect(monster, 0);
	if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames) {
		M_StartStand(monster, monster._mdir);
		return true;
	}

	return false;
}

bool MonaterRangedAttack(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	assert(monster.MType != nullptr);
	assert(monster.MData != nullptr);

	if (monster.AnimInfo.CurrentFrame == monster.MData->mAFNum) {
		const auto &missileType = static_cast<missile_id>(monster._mVar1);
		if (missileType != MIS_NULL) {
			int multimissiles = 1;
			if (missileType == MIS_CBOLT)
				multimissiles = 3;
			for (int mi = 0; mi < multimissiles; mi++) {
				AddMissile(
				    monster.position.tile,
				    monster.enemyPosition,
				    monster._mdir,
				    missileType,
				    TARGET_PLAYERS,
				    i,
				    monster._mVar2,
				    0);
			}
		}
		PlayEffect(monster, 0);
	}

	if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames) {
		M_StartStand(monster, monster._mdir);
		return true;
	}

	return false;
}

bool MonsterRangedSpecialAttack(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	assert(monster.MType != nullptr);
	assert(monster.MData != nullptr);

	if (monster.AnimInfo.CurrentFrame == monster.MData->mAFNum2 && monster.AnimInfo.TickCounterOfCurrentFrame == 0) {
		AddMissile(
		    monster.position.tile,
		    monster.enemyPosition,
		    monster._mdir,
		    static_cast<missile_id>(monster._mVar1),
		    TARGET_PLAYERS,
		    i,
		    monster._mVar3,
		    0);
		PlayEffect(monster, 3);
	}

	if (monster._mAi == AI_MEGA && monster.AnimInfo.CurrentFrame == monster.MData->mAFNum2) {
		if (monster._mVar2++ == 0) {
			monster._mFlags |= MFLAG_ALLOW_SPECIAL;
		} else if (monster._mVar2 == 15) {
			monster._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
	}

	if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames) {
		M_StartStand(monster, monster._mdir);
		return true;
	}

	return false;
}

bool MonsterSpecialAttack(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	assert(monster.MType != nullptr);
	assert(monster.MData != nullptr);

	if (monster.AnimInfo.CurrentFrame == monster.MData->mAFNum2)
		MonsterAttackPlayer(i, monster._menemy, monster.mHit2, monster.mMinDamage2, monster.mMaxDamage2);

	if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames) {
		M_StartStand(monster, monster._mdir);
		return true;
	}

	return false;
}

bool MonsterFadein(Monster &monster)
{
	if (((monster._mFlags & MFLAG_LOCK_ANIMATION) == 0 || monster.AnimInfo.CurrentFrame != 1)
	    && ((monster._mFlags & MFLAG_LOCK_ANIMATION) != 0 || monster.AnimInfo.CurrentFrame != monster.AnimInfo.NumberOfFrames)) {
		return false;
	}

	M_StartStand(monster, monster._mdir);
	monster._mFlags &= ~MFLAG_LOCK_ANIMATION;

	return true;
}

bool MonsterFadeout(Monster &monster)
{
	if (((monster._mFlags & MFLAG_LOCK_ANIMATION) == 0 || monster.AnimInfo.CurrentFrame != 1)
	    && ((monster._mFlags & MFLAG_LOCK_ANIMATION) != 0 || monster.AnimInfo.CurrentFrame != monster.AnimInfo.NumberOfFrames)) {
		return false;
	}

	int mt = monster.MType->mtype;
	if (mt < MT_INCIN || mt > MT_HELLBURN) {
		monster._mFlags &= ~MFLAG_LOCK_ANIMATION;
		monster._mFlags |= MFLAG_HIDDEN;
	} else {
		monster._mFlags &= ~MFLAG_LOCK_ANIMATION;
	}

	M_StartStand(monster, monster._mdir);

	return true;
}

bool MonsterHeal(Monster &monster)
{
	if ((monster._mFlags & MFLAG_NOHEAL) != 0) {
		monster._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		monster._mmode = MonsterMode::SpecialMeleeAttack;
		return false;
	}

	if (monster.AnimInfo.CurrentFrame == 1) {
		monster._mFlags &= ~MFLAG_LOCK_ANIMATION;
		monster._mFlags |= MFLAG_ALLOW_SPECIAL;
		if (monster._mVar1 + monster._mhitpoints < monster._mmaxhp) {
			monster._mhitpoints = monster._mVar1 + monster._mhitpoints;
		} else {
			monster._mhitpoints = monster._mmaxhp;
			monster._mFlags &= ~MFLAG_ALLOW_SPECIAL;
			monster._mmode = MonsterMode::SpecialMeleeAttack;
		}
	}
	return false;
}

bool MonsterTalk(Monster &monster)
{
	M_StartStand(monster, monster._mdir);
	monster._mgoal = MGOAL_TALKING;
	if (effect_is_playing(Speeches[monster.mtalkmsg].sfxnr))
		return false;
	InitQTextMsg(monster.mtalkmsg);
	if (monster._uniqtype - 1 == UMT_GARBUD) {
		if (monster.mtalkmsg == TEXT_GARBUD1) {
			Quests[Q_GARBUD]._qactive = QUEST_ACTIVE;
			Quests[Q_GARBUD]._qlog = true; // BUGFIX: (?) for other quests qactive and qlog go together, maybe this should actually go into the if above (fixed)
		}
		if (monster.mtalkmsg == TEXT_GARBUD2 && (monster._mFlags & MFLAG_QUEST_COMPLETE) == 0) {
			SpawnItem(monster, monster.position.tile + Displacement { 1, 1 }, true);
			monster._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (monster._uniqtype - 1 == UMT_ZHAR
	    && monster.mtalkmsg == TEXT_ZHAR1
	    && (monster._mFlags & MFLAG_QUEST_COMPLETE) == 0) {
		Quests[Q_ZHAR]._qactive = QUEST_ACTIVE;
		Quests[Q_ZHAR]._qlog = true;
		CreateTypeItem(monster.position.tile + Displacement { 1, 1 }, false, ItemType::Misc, IMISC_BOOK, true, false);
		monster._mFlags |= MFLAG_QUEST_COMPLETE;
	}
	if (monster._uniqtype - 1 == UMT_SNOTSPIL) {
		if (monster.mtalkmsg == TEXT_BANNER10 && (monster._mFlags & MFLAG_QUEST_COMPLETE) == 0) {
			ObjChangeMap(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 2, (setpc_h / 2) + setpc_y - 2);
			auto tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 4, setpc_y + (setpc_h / 2));
			TransVal = tren;
			Quests[Q_LTBANNER]._qvar1 = 2;
			if (Quests[Q_LTBANNER]._qactive == QUEST_INIT)
				Quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
			monster._mFlags |= MFLAG_QUEST_COMPLETE;
		}
		if (Quests[Q_LTBANNER]._qvar1 < 2) {
			app_fatal("SS Talk = %i, Flags = %i", monster.mtalkmsg, monster._mFlags);
		}
	}
	if (monster._uniqtype - 1 == UMT_LACHDAN) {
		if (monster.mtalkmsg == TEXT_VEIL9) {
			Quests[Q_VEIL]._qactive = QUEST_ACTIVE;
			Quests[Q_VEIL]._qlog = true;
		}
		if (monster.mtalkmsg == TEXT_VEIL11 && (monster._mFlags & MFLAG_QUEST_COMPLETE) == 0) {
			SpawnUnique(UITEM_STEELVEIL, monster.position.tile + Direction::South);
			monster._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (monster._uniqtype - 1 == UMT_WARLORD)
		Quests[Q_WARLORD]._qvar1 = 2;
	if (monster._uniqtype - 1 == UMT_LAZARUS && gbIsMultiplayer) {
		Quests[Q_BETRAYER]._qvar1 = 6;
		monster._mgoal = MGOAL_NORMAL;
		monster._msquelch = UINT8_MAX;
		monster.mtalkmsg = TEXT_NONE;
	}
	return false;
}

bool MonsterGotHit(Monster &monster)
{
	if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames) {
		M_StartStand(monster, monster._mdir);

		return true;
	}

	return false;
}

bool MonsterDeath(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	assert(monster.MType != nullptr);

	monster._mVar1++;
	if (monster.MType->mtype == MT_DIABLO) {
		if (monster.position.tile.x < ViewPosition.x) {
			ViewPosition.x--;
		} else if (monster.position.tile.x > ViewPosition.x) {
			ViewPosition.x++;
		}

		if (monster.position.tile.y < ViewPosition.y) {
			ViewPosition.y--;
		} else if (monster.position.tile.y > ViewPosition.y) {
			ViewPosition.y++;
		}

		if (monster._mVar1 == 140)
			PrepDoEnding();
	} else if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames) {
		if (monster._uniqtype == 0)
			AddCorpse(monster.position.tile, monster.MType->mdeadval, monster._mdir);
		else
			AddCorpse(monster.position.tile, monster._udeadval, monster._mdir);

		dMonster[monster.position.tile.x][monster.position.tile.y] = 0;
		monster._mDelFlag = true;

		M_UpdateLeader(i);
	}
	return false;
}

bool MonsterSpecialStand(Monster &monster)
{
	if (monster.AnimInfo.CurrentFrame == monster.MData->mAFNum2)
		PlayEffect(monster, 3);

	if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames) {
		M_StartStand(monster, monster._mdir);
		return true;
	}

	return false;
}

bool MonsterDelay(Monster &monster)
{
	monster.ChangeAnimationData(MonsterGraphic::Stand, GetMonsterDirection(monster));
	if (monster._mAi == AI_LAZARUS) {
		if (monster._mVar2 > 8 || monster._mVar2 < 0)
			monster._mVar2 = 8;
	}

	if (monster._mVar2-- == 0) {
		int oFrame = monster.AnimInfo.CurrentFrame;
		M_StartStand(monster, monster._mdir);
		monster.AnimInfo.CurrentFrame = oFrame;
		return true;
	}

	return false;
}

bool MonsterPetrified(Monster &monster)
{
	if (monster._mhitpoints <= 0) {
		dMonster[monster.position.tile.x][monster.position.tile.y] = 0;
		monster._mDelFlag = true;
	}

	return false;
}

int AddSkeleton(Point position, Direction dir, bool inMap)
{
	int j = 0;
	for (int i = 0; i < LevelMonsterTypeCount; i++) {
		if (IsSkel(LevelMonsterTypes[i].mtype))
			j++;
	}

	if (j == 0) {
		return -1;
	}

	int skeltypes = GenerateRnd(j);
	int m = 0;
	for (int i = 0; m < LevelMonsterTypeCount && i <= skeltypes; m++) {
		if (IsSkel(LevelMonsterTypes[m].mtype))
			i++;
	}
	return AddMonster(position, dir, m - 1, inMap);
}

void SpawnSkeleton(Point position, Direction dir)
{
	int skel = AddSkeleton(position, dir, true);
	if (skel != -1)
		StartSpecialStand(Monsters[skel], dir);
}

bool IsLineNotSolid(Point startPoint, Point endPoint)
{
	return LineClear(IsTileNotSolid, startPoint, endPoint);
}

void FollowTheLeader(Monster &monster)
{
	if (monster.leader == 0)
		return;

	if (monster.leaderRelation != LeaderRelation::Leashed)
		return;

	auto &leader = Monsters[monster.leader];
	if (monster._msquelch >= leader._msquelch)
		return;

	monster.position.last = leader.position.tile;
	monster._msquelch = leader._msquelch - 1;
}

void GroupUnity(Monster &monster)
{
	if (monster.leaderRelation == LeaderRelation::None)
		return;

	// Someone with a leaderRelation should have a leader ...
	assert(monster.leader >= 0);
	// And no unique monster would be a minion of someone else!
	assert(monster._uniqtype == 0);

	auto &leader = Monsters[monster.leader];
	if (IsLineNotSolid(monster.position.tile, leader.position.future)) {
		if (monster.leaderRelation == LeaderRelation::Separated
		    && monster.position.tile.WalkingDistance(leader.position.future) < 4) {
			// Reunite the separated monster with the pack
			leader.packsize++;
			monster.leaderRelation = LeaderRelation::Leashed;
		}
	} else if (monster.leaderRelation == LeaderRelation::Leashed) {
		leader.packsize--;
		monster.leaderRelation = LeaderRelation::Separated;
	}

	if (monster.leaderRelation == LeaderRelation::Leashed) {
		if (monster._msquelch > leader._msquelch) {
			leader.position.last = monster.position.tile;
			leader._msquelch = monster._msquelch - 1;
		}
		if (leader._mAi == AI_GARG && (leader._mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
			leader._mFlags &= ~MFLAG_ALLOW_SPECIAL;
			leader._mmode = MonsterMode::SpecialMeleeAttack;
		}
	}
}

bool RandomWalk(int i, Direction md)
{
	Direction mdtemp = md;
	bool ok = DirOK(i, md);
	if (GenerateRnd(2) != 0)
		ok = ok || (md = Left(mdtemp), DirOK(i, md)) || (md = Right(mdtemp), DirOK(i, md));
	else
		ok = ok || (md = Right(mdtemp), DirOK(i, md)) || (md = Left(mdtemp), DirOK(i, md));
	if (GenerateRnd(2) != 0) {
		ok = ok
		    || (md = Right(Right(mdtemp)), DirOK(i, md))
		    || (md = Left(Left(mdtemp)), DirOK(i, md));
	} else {
		ok = ok
		    || (md = Left(Left(mdtemp)), DirOK(i, md))
		    || (md = Right(Right(mdtemp)), DirOK(i, md));
	}
	if (ok)
		M_WalkDir(i, md);
	return ok;
}

bool RandomWalk2(int i, Direction md)
{
	Direction mdtemp = md;
	bool ok = DirOK(i, md);    // Can we continue in the same direction
	if (GenerateRnd(2) != 0) { // Randomly go left or right
		ok = ok || (mdtemp = Left(md), DirOK(i, Left(md))) || (mdtemp = Right(md), DirOK(i, Right(md)));
	} else {
		ok = ok || (mdtemp = Right(md), DirOK(i, Right(md))) || (mdtemp = Left(md), DirOK(i, Left(md)));
	}

	if (ok)
		M_WalkDir(i, mdtemp);

	return ok;
}

/**
 * @brief Check if a tile is affected by a spell we are vunerable to
 */
bool IsTileSafe(const Monster &monster, Point position)
{
	if (!TileContainsMissile(position)) {
		return true;
	}

	bool fearsFire = (monster.mMagicRes & IMMUNE_FIRE) == 0 || monster.MType->mtype == MT_DIABLO;
	bool fearsLightning = (monster.mMagicRes & IMMUNE_LIGHTNING) == 0 || monster.MType->mtype == MT_DIABLO;

	for (int j = 0; j < ActiveMissileCount; j++) {
		uint8_t mi = ActiveMissiles[j];
		auto &missile = Missiles[mi];
		if (missile.position.tile == position) {
			if (fearsFire && missile._mitype == MIS_FIREWALL) {
				return false;
			}
			if (fearsLightning && missile._mitype == MIS_LIGHTWALL) {
				return false;
			}
		}
	}

	return true;
}

/**
 * @brief Check that the given tile is not currently blocked
 */
bool IsTileAvailable(Point position)
{
	if (dPlayer[position.x][position.y] != 0 || dMonster[position.x][position.y] != 0)
		return false;

	if (!IsTileWalkable(position))
		return false;

	return true;
}

/**
 * @brief If a monster can access the given tile (possibly by opening a door)
 */
bool IsTileAccessible(const Monster &monster, Point position)
{
	if (dPlayer[position.x][position.y] != 0 || dMonster[position.x][position.y] != 0)
		return false;

	if (!IsTileWalkable(position, (monster._mFlags & MFLAG_CAN_OPEN_DOOR) != 0))
		return false;

	return IsTileSafe(monster, position);
}

bool AiPlanWalk(int i)
{
	int8_t path[MAX_PATH_LENGTH];

	/** Maps from walking path step to facing direction. */
	const Direction plr2monst[9] = { Direction::South, Direction::NorthEast, Direction::NorthWest, Direction::SouthEast, Direction::SouthWest, Direction::North, Direction::East, Direction::South, Direction::West };

	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (FindPath([&monster](Point position) { return IsTileAccessible(monster, position); }, monster.position.tile, monster.enemyPosition, path) == 0) {
		return false;
	}

	RandomWalk(i, plr2monst[path[0]]);
	return true;
}

bool DumbWalk(int i, Direction md)
{
	bool ok = DirOK(i, md);
	if (ok)
		M_WalkDir(i, md);

	return ok;
}

Direction Turn(Direction direction, bool turnLeft)
{
	return turnLeft ? Left(direction) : Right(direction);
}

bool RoundWalk(int i, Direction direction, int *dir)
{
	Direction turn45deg = Turn(direction, *dir != 0);
	Direction turn90deg = Turn(turn45deg, *dir != 0);

	if (DirOK(i, turn90deg)) {
		// Turn 90 degrees
		M_WalkDir(i, turn90deg);
		return true;
	}

	if (DirOK(i, turn45deg)) {
		// Only do a small turn
		M_WalkDir(i, turn45deg);
		return true;
	}

	if (DirOK(i, direction)) {
		// Continue straight
		M_WalkDir(i, direction);
		return true;
	}

	// Try 90 degrees in the opposite than desired direction
	*dir = (*dir == 0) ? 1 : 0;
	return RandomWalk(i, Opposite(turn90deg));
}

bool AiPlanPath(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster.MType->mtype != MT_GOLEM) {
		if (monster._msquelch == 0)
			return false;
		if (monster._mmode != MonsterMode::Stand)
			return false;
		if (monster._mgoal != MGOAL_NORMAL && monster._mgoal != MGOAL_MOVE && monster._mgoal != MGOAL_ATTACK2)
			return false;
		if (monster.position.tile.x == 1 && monster.position.tile.y == 0)
			return false;
	}

	bool clear = LineClear(
	    [&monster](Point position) { return IsTileAvailable(monster, position); },
	    monster.position.tile,
	    monster.enemyPosition);
	if (!clear || (monster._pathcount >= 5 && monster._pathcount < 8)) {
		if ((monster._mFlags & MFLAG_CAN_OPEN_DOOR) != 0)
			MonstCheckDoors(monster);
		monster._pathcount++;
		if (monster._pathcount < 5)
			return false;
		if (AiPlanWalk(i))
			return true;
	}

	if (monster.MType->mtype != MT_GOLEM)
		monster._pathcount = 0;

	return false;
}

void AiAvoidance(int i, bool special)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	int mx = monster.position.tile.x - fx;
	int my = monster.position.tile.y - fy;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	if (monster._msquelch < UINT8_MAX)
		MonstCheckDoors(monster);
	int v = GenerateRnd(100);
	if ((abs(mx) >= 2 || abs(my) >= 2) && monster._msquelch == UINT8_MAX && dTransVal[monster.position.tile.x][monster.position.tile.y] == dTransVal[fx][fy]) {
		if (monster._mgoal == MGOAL_MOVE || ((abs(mx) >= 4 || abs(my) >= 4) && GenerateRnd(4) == 0)) {
			if (monster._mgoal != MGOAL_MOVE) {
				monster._mgoalvar1 = 0;
				monster._mgoalvar2 = GenerateRnd(2);
			}
			monster._mgoal = MGOAL_MOVE;
			int dist = std::max(abs(mx), abs(my));
			if ((monster._mgoalvar1++ >= 2 * dist && DirOK(i, md)) || dTransVal[monster.position.tile.x][monster.position.tile.y] != dTransVal[fx][fy]) {
				monster._mgoal = MGOAL_NORMAL;
			} else if (!RoundWalk(i, md, &monster._mgoalvar2)) {
				AiDelay(monster, GenerateRnd(10) + 10);
			}
		}
	} else {
		monster._mgoal = MGOAL_NORMAL;
	}
	if (monster._mgoal == MGOAL_NORMAL) {
		if (abs(mx) >= 2 || abs(my) >= 2) {
			if ((monster._mVar2 > 20 && v < 2 * monster._mint + 28)
			    || (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways)
			        && monster._mVar2 == 0
			        && v < 2 * monster._mint + 78)) {
				RandomWalk(i, md);
			}
		} else if (v < 2 * monster._mint + 23) {
			monster._mdir = md;
			if (special && monster._mhitpoints < (monster._mmaxhp / 2) && GenerateRnd(2) != 0)
				StartSpecialAttack(monster);
			else
				StartAttack(monster);
		}
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void AiRanged(int i, missile_id missileType, bool special)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}

	if (monster._msquelch == UINT8_MAX || (monster._mFlags & MFLAG_TARGETS_MONSTER) != 0) {
		int fx = monster.enemyPosition.x;
		int fy = monster.enemyPosition.y;
		int mx = monster.position.tile.x - fx;
		int my = monster.position.tile.y - fy;
		Direction md = GetMonsterDirection(monster);
		if (monster._msquelch < UINT8_MAX)
			MonstCheckDoors(monster);
		monster._mdir = md;
		if (static_cast<MonsterMode>(monster._mVar1) == MonsterMode::RangedAttack) {
			AiDelay(monster, GenerateRnd(20));
		} else if (abs(mx) < 4 && abs(my) < 4) {
			if (GenerateRnd(100) < 10 * (monster._mint + 7))
				RandomWalk(i, Opposite(md));
		}
		if (monster._mmode == MonsterMode::Stand) {
			if (LineClearMissile(monster.position.tile, { fx, fy })) {
				if (special)
					StartRangedSpecialAttack(monster, missileType, 4);
				else
					StartRangedAttack(monster, missileType, 4);
			} else {
				monster.CheckStandAnimationIsLoaded(md);
			}
		}
		return;
	}

	if (monster._msquelch != 0) {
		int fx = monster.position.last.x;
		int fy = monster.position.last.y;
		Direction md = GetDirection(monster.position.tile, { fx, fy });
		RandomWalk(i, md);
	}
}

void AiRangedAvoidance(int i, missile_id missileType, bool checkdoors, int dam, int lessmissiles)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	int mx = monster.position.tile.x - fx;
	int my = monster.position.tile.y - fy;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	if (checkdoors && monster._msquelch < UINT8_MAX)
		MonstCheckDoors(monster);
	int v = GenerateRnd(10000);
	int dist = std::max(abs(mx), abs(my));
	if (dist >= 2 && monster._msquelch == UINT8_MAX && dTransVal[monster.position.tile.x][monster.position.tile.y] == dTransVal[fx][fy]) {
		if (monster._mgoal == MGOAL_MOVE || (dist >= 3 && GenerateRnd(4 << lessmissiles) == 0)) {
			if (monster._mgoal != MGOAL_MOVE) {
				monster._mgoalvar1 = 0;
				monster._mgoalvar2 = GenerateRnd(2);
			}
			monster._mgoal = MGOAL_MOVE;
			if (monster._mgoalvar1++ >= 2 * dist && DirOK(i, md)) {
				monster._mgoal = MGOAL_NORMAL;
			} else if (v < (500 * (monster._mint + 1) >> lessmissiles)
			    && (LineClearMissile(monster.position.tile, { fx, fy }))) {
				StartRangedSpecialAttack(monster, missileType, dam);
			} else {
				RoundWalk(i, md, &monster._mgoalvar2);
			}
		}
	} else {
		monster._mgoal = MGOAL_NORMAL;
	}
	if (monster._mgoal == MGOAL_NORMAL) {
		if (((dist >= 3 && v < ((500 * (monster._mint + 2)) >> lessmissiles))
		        || v < ((500 * (monster._mint + 1)) >> lessmissiles))
		    && LineClearMissile(monster.position.tile, { fx, fy })) {
			StartRangedSpecialAttack(monster, missileType, dam);
		} else if (dist >= 2) {
			v = GenerateRnd(100);
			if (v < 1000 * (monster._mint + 5)
			    || (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways) && monster._mVar2 == 0 && v < 1000 * (monster._mint + 8))) {
				RandomWalk(i, md);
			}
		} else if (v < 1000 * (monster._mint + 6)) {
			monster._mdir = md;
			StartAttack(monster);
		}
	}
	if (monster._mmode == MonsterMode::Stand) {
		AiDelay(monster, GenerateRnd(10) + 5);
	}
}

void ZombieAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}

	if (!IsTileVisible(monster.position.tile)) {
		return;
	}

	if (GenerateRnd(100) < 2 * monster._mint + 10) {
		int dist = monster.enemyPosition.WalkingDistance(monster.position.tile);
		if (dist >= 2) {
			if (dist >= 2 * monster._mint + 4) {
				Direction md = monster._mdir;
				if (GenerateRnd(100) < 2 * monster._mint + 20) {
					md = static_cast<Direction>(GenerateRnd(8));
				}
				DumbWalk(i, md);
			} else {
				RandomWalk(i, GetMonsterDirection(monster));
			}
		} else {
			StartAttack(monster);
		}
	}

	monster.CheckStandAnimationIsLoaded(monster._mdir);
}

void OverlordAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int mx = monster.position.tile.x - monster.enemyPosition.x;
	int my = monster.position.tile.y - monster.enemyPosition.y;
	Direction md = GetMonsterDirection(monster);
	monster._mdir = md;
	int v = GenerateRnd(100);
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if ((monster._mVar2 > 20 && v < 4 * monster._mint + 20)
		    || (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways)
		        && monster._mVar2 == 0
		        && v < 4 * monster._mint + 70)) {
			RandomWalk(i, md);
		}
	} else if (v < 4 * monster._mint + 15) {
		StartAttack(monster);
	} else if (v < 4 * monster._mint + 20) {
		StartSpecialAttack(monster);
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void SkeletonAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int x = monster.position.tile.x - monster.enemyPosition.x;
	int y = monster.position.tile.y - monster.enemyPosition.y;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	monster._mdir = md;
	if (abs(x) >= 2 || abs(y) >= 2) {
		if (static_cast<MonsterMode>(monster._mVar1) == MonsterMode::Delay || (GenerateRnd(100) >= 35 - 4 * monster._mint)) {
			RandomWalk(i, md);
		} else {
			AiDelay(monster, 15 - 2 * monster._mint + GenerateRnd(10));
		}
	} else {
		if (static_cast<MonsterMode>(monster._mVar1) == MonsterMode::Delay || (GenerateRnd(100) < 2 * monster._mint + 20)) {
			StartAttack(monster);
		} else {
			AiDelay(monster, 2 * (5 - monster._mint) + GenerateRnd(10));
		}
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void SkeletonBowAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int mx = monster.position.tile.x - monster.enemyPosition.x;
	int my = monster.position.tile.y - monster.enemyPosition.y;

	Direction md = GetMonsterDirection(monster);
	monster._mdir = md;
	int v = GenerateRnd(100);

	bool walking = false;

	if (abs(mx) < 4 && abs(my) < 4) {
		if ((monster._mVar2 > 20 && v < 2 * monster._mint + 13)
		    || (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways)
		        && monster._mVar2 == 0
		        && v < 2 * monster._mint + 63)) {
			walking = DumbWalk(i, Opposite(md));
		}
	}

	if (!walking) {
		if (GenerateRnd(100) < 2 * monster._mint + 3) {
			if (LineClearMissile(monster.position.tile, monster.enemyPosition))
				StartRangedAttack(monster, MIS_ARROW, 4);
		}
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void ScavengerAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand)
		return;
	if (monster._mhitpoints < (monster._mmaxhp / 2) && monster._mgoal != MGOAL_HEALING) {
		if (monster.leaderRelation != LeaderRelation::None) {
			if (monster.leaderRelation == LeaderRelation::Leashed)
				Monsters[monster.leader].packsize--;
			monster.leaderRelation = LeaderRelation::None;
		}
		monster._mgoal = MGOAL_HEALING;
		monster._mgoalvar3 = 10;
	}
	if (monster._mgoal == MGOAL_HEALING && monster._mgoalvar3 != 0) {
		monster._mgoalvar3--;
		if (dCorpse[monster.position.tile.x][monster.position.tile.y] != 0) {
			StartEating(monster);
			if ((monster._mFlags & MFLAG_NOHEAL) == 0) {
				if (gbIsHellfire) {
					int mMaxHP = monster._mmaxhp; // BUGFIX use _mmaxhp or we loose health when difficulty isn't normal (fixed)
					monster._mhitpoints += mMaxHP / 8;
					if (monster._mhitpoints > monster._mmaxhp)
						monster._mhitpoints = monster._mmaxhp;
					if (monster._mgoalvar3 <= 0 || monster._mhitpoints == monster._mmaxhp)
						dCorpse[monster.position.tile.x][monster.position.tile.y] = 0;
				} else {
					monster._mhitpoints += 64;
				}
			}
			int targetHealth = monster._mmaxhp;
			if (!gbIsHellfire)
				targetHealth = (monster._mmaxhp / 2) + (monster._mmaxhp / 4);
			if (monster._mhitpoints >= targetHealth) {
				monster._mgoal = MGOAL_NORMAL;
				monster._mgoalvar1 = 0;
				monster._mgoalvar2 = 0;
			}
		} else {
			if (monster._mgoalvar1 == 0) {
				bool done = false;
				int x;
				int y;
				if (GenerateRnd(2) != 0) {
					for (y = -4; y <= 4 && !done; y++) {
						for (x = -4; x <= 4 && !done; x++) {
							// BUGFIX: incorrect check of offset against limits of the dungeon (fixed)
							if (!InDungeonBounds(monster.position.tile + Displacement { x, y }))
								continue;
							done = dCorpse[monster.position.tile.x + x][monster.position.tile.y + y] != 0
							    && IsLineNotSolid(
							        monster.position.tile,
							        monster.position.tile + Displacement { x, y });
						}
					}
					x--;
					y--;
				} else {
					for (y = 4; y >= -4 && !done; y--) {
						for (x = 4; x >= -4 && !done; x--) {
							// BUGFIX: incorrect check of offset against limits of the dungeon (fixed)
							if (!InDungeonBounds(monster.position.tile + Displacement { x, y }))
								continue;
							done = dCorpse[monster.position.tile.x + x][monster.position.tile.y + y] != 0
							    && IsLineNotSolid(
							        monster.position.tile,
							        monster.position.tile + Displacement { x, y });
						}
					}
					x++;
					y++;
				}
				if (done) {
					monster._mgoalvar1 = x + monster.position.tile.x + 1;
					monster._mgoalvar2 = y + monster.position.tile.y + 1;
				}
			}
			if (monster._mgoalvar1 != 0) {
				int x = monster._mgoalvar1 - 1;
				int y = monster._mgoalvar2 - 1;
				monster._mdir = GetDirection(monster.position.tile, { x, y });
				RandomWalk(i, monster._mdir);
			}
		}
	}

	if (monster._mmode == MonsterMode::Stand)
		SkeletonAi(i);
}

void RhinoAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	int mx = monster.position.tile.x - fx;
	int my = monster.position.tile.y - fy;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	if (monster._msquelch < UINT8_MAX)
		MonstCheckDoors(monster);
	int v = GenerateRnd(100);
	int dist = std::max(abs(mx), abs(my));
	if (dist >= 2) {
		if (monster._mgoal == MGOAL_MOVE || (dist >= 5 && GenerateRnd(4) != 0)) {
			if (monster._mgoal != MGOAL_MOVE) {
				monster._mgoalvar1 = 0;
				monster._mgoalvar2 = GenerateRnd(2);
			}
			monster._mgoal = MGOAL_MOVE;
			if (monster._mgoalvar1++ >= 2 * dist || dTransVal[monster.position.tile.x][monster.position.tile.y] != dTransVal[fx][fy]) {
				monster._mgoal = MGOAL_NORMAL;
			} else if (!RoundWalk(i, md, &monster._mgoalvar2)) {
				AiDelay(monster, GenerateRnd(10) + 10);
			}
		}
	} else {
		monster._mgoal = MGOAL_NORMAL;
	}
	if (monster._mgoal == MGOAL_NORMAL) {
		if (dist >= 5
		    && v < 2 * monster._mint + 43
		    && LineClear([&monster](Point position) { return IsTileAvailable(monster, position); }, monster.position.tile, { fx, fy })) {
			if (AddMissile(monster.position.tile, { fx, fy }, md, MIS_RHINO, TARGET_PLAYERS, i, 0, 0) != -1) {
				if (monster.MData->snd_special)
					PlayEffect(monster, 3);
				dMonster[monster.position.tile.x][monster.position.tile.y] = -(i + 1);
				monster._mmode = MonsterMode::Charge;
			}
		} else {
			if (dist >= 2) {
				v = GenerateRnd(100);
				if (v >= 2 * monster._mint + 33
				    && (IsNoneOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways)
				        || monster._mVar2 != 0
				        || v >= 2 * monster._mint + 83)) {
					AiDelay(monster, GenerateRnd(10) + 10);
				} else {
					RandomWalk(i, md);
				}
			} else if (v < 2 * monster._mint + 28) {
				monster._mdir = md;
				StartAttack(monster);
			}
		}
	}

	monster.CheckStandAnimationIsLoaded(monster._mdir);
}

void GoatAi(int i)
{
	AiAvoidance(i, true);
}

void GoatBowAi(int i)
{
	AiRanged(i, MIS_ARROW, false);
}

void FallenAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mgoal == MGOAL_ATTACK2) {
		if (monster._mgoalvar1 != 0)
			monster._mgoalvar1--;
		else
			monster._mgoal = MGOAL_NORMAL;
	}
	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	if (monster._mgoal == MGOAL_RETREAT) {
		if (monster._mgoalvar1-- == 0) {
			monster._mgoal = MGOAL_NORMAL;
			M_StartStand(monster, Opposite(static_cast<Direction>(monster._mgoalvar2)));
		}
	}

	if (monster.AnimInfo.CurrentFrame == monster.AnimInfo.NumberOfFrames) {
		if (GenerateRnd(4) != 0) {
			return;
		}
		if ((monster._mFlags & MFLAG_NOHEAL) == 0) {
			StartSpecialStand(monster, monster._mdir);
			if (monster._mmaxhp - (2 * monster._mint + 2) >= monster._mhitpoints)
				monster._mhitpoints += 2 * monster._mint + 2;
			else
				monster._mhitpoints = monster._mmaxhp;
		}
		int rad = 2 * monster._mint + 4;
		for (int y = -rad; y <= rad; y++) {
			for (int x = -rad; x <= rad; x++) {
				int xpos = monster.position.tile.x + x;
				int ypos = monster.position.tile.y + y;
				if (InDungeonBounds({ x, y })) {
					int m = dMonster[xpos][ypos];
					if (m <= 0)
						continue;

					auto &otherMonster = Monsters[m - 1];
					if (otherMonster._mAi != AI_FALLEN)
						continue;

					otherMonster._mgoal = MGOAL_ATTACK2;
					otherMonster._mgoalvar1 = 30 * monster._mint + 105;
				}
			}
		}
	} else if (monster._mgoal == MGOAL_RETREAT) {
		RandomWalk(i, monster._mdir);
	} else if (monster._mgoal == MGOAL_ATTACK2) {
		int xpos = monster.position.tile.x - monster.enemyPosition.x;
		int ypos = monster.position.tile.y - monster.enemyPosition.y;
		if (abs(xpos) < 2 && abs(ypos) < 2)
			StartAttack(monster);
		else
			RandomWalk(i, GetMonsterDirection(monster));
	} else
		SkeletonAi(i);
}

void MagmaAi(int i)
{
	AiRangedAvoidance(i, MIS_MAGMABALL, true, 4, 0);
}

void LeoricAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	int mx = monster.position.tile.x - fx;
	int my = monster.position.tile.y - fy;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	if (monster._msquelch < UINT8_MAX)
		MonstCheckDoors(monster);
	int v = GenerateRnd(100);
	int dist = std::max(abs(mx), abs(my));
	if (dist >= 2 && monster._msquelch == UINT8_MAX && dTransVal[monster.position.tile.x][monster.position.tile.y] == dTransVal[fx][fy]) {
		if (monster._mgoal == MGOAL_MOVE || ((abs(mx) >= 3 || abs(my) >= 3) && GenerateRnd(4) == 0)) {
			if (monster._mgoal != MGOAL_MOVE) {
				monster._mgoalvar1 = 0;
				monster._mgoalvar2 = GenerateRnd(2);
			}
			monster._mgoal = MGOAL_MOVE;
			if ((monster._mgoalvar1++ >= 2 * dist && DirOK(i, md)) || dTransVal[monster.position.tile.x][monster.position.tile.y] != dTransVal[fx][fy]) {
				monster._mgoal = MGOAL_NORMAL;
			} else if (!RoundWalk(i, md, &monster._mgoalvar2)) {
				AiDelay(monster, GenerateRnd(10) + 10);
			}
		}
	} else {
		monster._mgoal = MGOAL_NORMAL;
	}
	if (monster._mgoal == MGOAL_NORMAL) {
		if (!gbIsMultiplayer
		    && ((dist >= 3 && v < 4 * monster._mint + 35) || v < 6)
		    && LineClearMissile(monster.position.tile, { fx, fy })) {
			Point newPosition = monster.position.tile + md;
			if (IsTileAvailable(monster, newPosition) && ActiveMonsterCount < MAXMONSTERS) {
				SpawnSkeleton(newPosition, md);
				StartSpecialStand(monster, md);
			}
		} else {
			if (dist >= 2) {
				v = GenerateRnd(100);
				if (v >= monster._mint + 25
				    && (IsNoneOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways) || monster._mVar2 != 0 || (v >= monster._mint + 75))) {
					AiDelay(monster, GenerateRnd(10) + 10);
				} else {
					RandomWalk(i, md);
				}
			} else if (v < monster._mint + 20) {
				monster._mdir = md;
				StartAttack(monster);
			}
		}
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void BatAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int xd = monster.position.tile.x - monster.enemyPosition.x;
	int yd = monster.position.tile.y - monster.enemyPosition.y;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	monster._mdir = md;
	int v = GenerateRnd(100);
	if (monster._mgoal == MGOAL_RETREAT) {
		if (monster._mgoalvar1 == 0) {
			RandomWalk(i, Opposite(md));
			monster._mgoalvar1++;
		} else {
			if (GenerateRnd(2) != 0)
				RandomWalk(i, Left(md));
			else
				RandomWalk(i, Right(md));
			monster._mgoal = MGOAL_NORMAL;
		}
		return;
	}

	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	if (monster.MType->mtype == MT_GLOOM
	    && (abs(xd) >= 5 || abs(yd) >= 5)
	    && v < 4 * monster._mint + 33
	    && LineClear([&monster](Point position) { return IsTileAvailable(monster, position); }, monster.position.tile, { fx, fy })) {
		if (AddMissile(monster.position.tile, { fx, fy }, md, MIS_RHINO, TARGET_PLAYERS, i, 0, 0) != -1) {
			dMonster[monster.position.tile.x][monster.position.tile.y] = -(i + 1);
			monster._mmode = MonsterMode::Charge;
		}
	} else if (abs(xd) >= 2 || abs(yd) >= 2) {
		if ((monster._mVar2 > 20 && v < monster._mint + 13)
		    || (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways)
		        && monster._mVar2 == 0
		        && v < monster._mint + 63)) {
			RandomWalk(i, md);
		}
	} else if (v < 4 * monster._mint + 8) {
		StartAttack(monster);
		monster._mgoal = MGOAL_RETREAT;
		monster._mgoalvar1 = 0;
		if (monster.MType->mtype == MT_FAMILIAR) {
			AddMissile(monster.enemyPosition, { monster.enemyPosition.x + 1, 0 }, Direction::South, MIS_LIGHTNING, TARGET_PLAYERS, i, GenerateRnd(10) + 1, 0);
		}
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void GargoyleAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	int dx = monster.position.tile.x - monster.position.last.x;
	int dy = monster.position.tile.y - monster.position.last.y;
	Direction md = GetMonsterDirection(monster);
	if (monster._msquelch != 0 && (monster._mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
		UpdateEnemy(monster);
		int mx = monster.position.tile.x - monster.enemyPosition.x;
		int my = monster.position.tile.y - monster.enemyPosition.y;
		if (abs(mx) < monster._mint + 2 && abs(my) < monster._mint + 2) {
			monster._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
		return;
	}

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	if (monster._mhitpoints < (monster._mmaxhp / 2))
		if ((monster._mFlags & MFLAG_NOHEAL) == 0)
			monster._mgoal = MGOAL_RETREAT;
	if (monster._mgoal == MGOAL_RETREAT) {
		if (abs(dx) >= monster._mint + 2 || abs(dy) >= monster._mint + 2) {
			monster._mgoal = MGOAL_NORMAL;
			StartHeal(monster);
		} else if (!RandomWalk(i, Opposite(md))) {
			monster._mgoal = MGOAL_NORMAL;
		}
	}
	AiAvoidance(i, false);
}

void ButcherAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int mx = monster.position.tile.x;
	int my = monster.position.tile.y;
	int x = mx - monster.enemyPosition.x;
	int y = my - monster.enemyPosition.y;

	Direction md = GetDirection({ mx, my }, monster.position.last);
	monster._mdir = md;

	if (abs(x) >= 2 || abs(y) >= 2)
		RandomWalk(i, md);
	else
		StartAttack(monster);

	monster.CheckStandAnimationIsLoaded(md);
}

void SuccubusAi(int i)
{
	AiRanged(i, MIS_FLARE, false);
}

void SneakAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}
	int mx = monster.position.tile.x;
	int my = monster.position.tile.y;
	if (dLight[mx][my] == LightsMax) {
		return;
	}
	mx -= monster.enemyPosition.x;
	my -= monster.enemyPosition.y;

	int dist = 5 - monster._mint;
	if (static_cast<MonsterMode>(monster._mVar1) == MonsterMode::HitRecovery) {
		monster._mgoal = MGOAL_RETREAT;
		monster._mgoalvar1 = 0;
	} else if (abs(mx) >= dist + 3 || abs(my) >= dist + 3 || monster._mgoalvar1 > 8) {
		monster._mgoal = MGOAL_NORMAL;
		monster._mgoalvar1 = 0;
	}
	Direction md = GetMonsterDirection(monster);
	if (monster._mgoal == MGOAL_RETREAT && (monster._mFlags & MFLAG_NO_ENEMY) == 0) {
		if ((monster._mFlags & MFLAG_TARGETS_MONSTER) != 0)
			md = GetDirection(monster.position.tile, Monsters[monster._menemy].position.tile);
		else
			md = GetDirection(monster.position.tile, Players[monster._menemy].position.last);
		md = Opposite(md);
		if (monster.MType->mtype == MT_UNSEEN) {
			if (GenerateRnd(2) != 0)
				md = Left(md);
			else
				md = Right(md);
		}
	}
	monster._mdir = md;
	int v = GenerateRnd(100);
	if (abs(mx) < dist && abs(my) < dist && (monster._mFlags & MFLAG_HIDDEN) != 0) {
		StartFadein(monster, md, false);
	} else {
		if ((abs(mx) >= dist + 1 || abs(my) >= dist + 1) && (monster._mFlags & MFLAG_HIDDEN) == 0) {
			StartFadeout(monster, md, true);
		} else {
			if (monster._mgoal == MGOAL_RETREAT
			    || ((abs(mx) >= 2 || abs(my) >= 2) && ((monster._mVar2 > 20 && v < 4 * monster._mint + 14) || (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways) && monster._mVar2 == 0 && v < 4 * monster._mint + 64)))) {
				monster._mgoalvar1++;
				RandomWalk(i, md);
			}
		}
	}
	if (monster._mmode == MonsterMode::Stand) {
		if (abs(mx) >= 2 || abs(my) >= 2 || v >= 4 * monster._mint + 10)
			monster.ChangeAnimationData(MonsterGraphic::Stand);
		else
			StartAttack(monster);
	}
}

void StormAi(int i)
{
	AiRangedAvoidance(i, MIS_LIGHTCTRL2, true, 4, 0);
}

void GharbadAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}

	Direction md = GetMonsterDirection(monster);

	if (monster.mtalkmsg >= TEXT_GARBUD1
	    && monster.mtalkmsg <= TEXT_GARBUD3
	    && !IsTileVisible(monster.position.tile)
	    && monster._mgoal == MGOAL_TALKING) {
		monster._mgoal = MGOAL_INQUIRING;
		switch (monster.mtalkmsg) {
		case TEXT_GARBUD1:
			monster.mtalkmsg = TEXT_GARBUD2;
			break;
		case TEXT_GARBUD2:
			monster.mtalkmsg = TEXT_GARBUD3;
			break;
		case TEXT_GARBUD3:
			monster.mtalkmsg = TEXT_GARBUD4;
			break;
		default:
			break;
		}
	}

	if (IsTileVisible(monster.position.tile)) {
		if (monster.mtalkmsg == TEXT_GARBUD4) {
			if (!effect_is_playing(USFX_GARBUD4) && monster._mgoal == MGOAL_TALKING) {
				monster._mgoal = MGOAL_NORMAL;
				monster._msquelch = UINT8_MAX;
				monster.mtalkmsg = TEXT_NONE;
			}
		}
	}

	if (monster._mgoal == MGOAL_NORMAL || monster._mgoal == MGOAL_MOVE)
		AiAvoidance(i, true);

	monster.CheckStandAnimationIsLoaded(md);
}

void AcidAvoidanceAi(int i)
{
	AiRangedAvoidance(i, MIS_ACID, false, 4, 1);
}

void AcidAi(int i)
{
	AiRanged(i, MIS_ACID, true);
}

void SnotSpilAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}

	Direction md = GetMonsterDirection(monster);

	if (monster.mtalkmsg == TEXT_BANNER10 && !IsTileVisible(monster.position.tile) && monster._mgoal == MGOAL_TALKING) {
		monster.mtalkmsg = TEXT_BANNER11;
		monster._mgoal = MGOAL_INQUIRING;
	}

	if (monster.mtalkmsg == TEXT_BANNER11 && Quests[Q_LTBANNER]._qvar1 == 3) {
		monster.mtalkmsg = TEXT_NONE;
		monster._mgoal = MGOAL_NORMAL;
	}

	if (IsTileVisible(monster.position.tile)) {
		if (monster.mtalkmsg == TEXT_BANNER12) {
			if (!effect_is_playing(USFX_SNOT3) && monster._mgoal == MGOAL_TALKING) {
				ObjChangeMap(setpc_x, setpc_y, setpc_x + setpc_w + 1, setpc_y + setpc_h + 1);
				Quests[Q_LTBANNER]._qvar1 = 3;
				RedoPlayerVision();
				monster._msquelch = UINT8_MAX;
				monster.mtalkmsg = TEXT_NONE;
				monster._mgoal = MGOAL_NORMAL;
			}
		}
		if (Quests[Q_LTBANNER]._qvar1 == 3) {
			if (monster._mgoal == MGOAL_NORMAL || monster._mgoal == MGOAL_ATTACK2)
				FallenAi(i);
		}
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void SnakeAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	char pattern[6] = { 1, 1, 0, -1, -1, 0 };
	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0)
		return;
	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	int mx = monster.position.tile.x - fx;
	int my = monster.position.tile.y - fy;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	monster._mdir = md;
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (abs(mx) < 3 && abs(my) < 3 && LineClear([&monster](Point position) { return IsTileAvailable(monster, position); }, monster.position.tile, { fx, fy }) && static_cast<MonsterMode>(monster._mVar1) != MonsterMode::Charge) {
			if (AddMissile(monster.position.tile, { fx, fy }, md, MIS_RHINO, TARGET_PLAYERS, i, 0, 0) != -1) {
				PlayEffect(monster, 0);
				dMonster[monster.position.tile.x][monster.position.tile.y] = -(i + 1);
				monster._mmode = MonsterMode::Charge;
			}
		} else if (static_cast<MonsterMode>(monster._mVar1) == MonsterMode::Delay || GenerateRnd(100) >= 35 - 2 * monster._mint) {
			if (pattern[monster._mgoalvar1] == -1)
				md = Left(md);
			else if (pattern[monster._mgoalvar1] == 1)
				md = Right(md);

			monster._mgoalvar1++;
			if (monster._mgoalvar1 > 5)
				monster._mgoalvar1 = 0;

			Direction targetDirection = static_cast<Direction>(monster._mgoalvar2);
			if (md != targetDirection) {
				int drift = static_cast<int>(md) - monster._mgoalvar2;
				if (drift < 0)
					drift += 8;

				if (drift < 4)
					md = Right(targetDirection);
				else if (drift > 4)
					md = Left(targetDirection);
				monster._mgoalvar2 = static_cast<int>(md);
			}

			if (!DumbWalk(i, md))
				RandomWalk2(i, monster._mdir);
		} else {
			AiDelay(monster, 15 - monster._mint + GenerateRnd(10));
		}
	} else {
		if (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::Delay, MonsterMode::Charge)
		    || (GenerateRnd(100) < monster._mint + 20)) {
			StartAttack(monster);
		} else
			AiDelay(monster, 10 - monster._mint + GenerateRnd(10));
	}

	monster.CheckStandAnimationIsLoaded(monster._mdir);
}

void CounselorAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}
	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	int mx = monster.position.tile.x - fx;
	int my = monster.position.tile.y - fy;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	if (monster._msquelch < UINT8_MAX)
		MonstCheckDoors(monster);
	int v = GenerateRnd(100);
	if (monster._mgoal == MGOAL_RETREAT) {
		if (monster._mgoalvar1++ <= 3)
			RandomWalk(i, Opposite(md));
		else {
			monster._mgoal = MGOAL_NORMAL;
			StartFadein(monster, md, true);
		}
	} else if (monster._mgoal == MGOAL_MOVE) {
		int dist = std::max(abs(mx), abs(my));
		if (dist >= 2 && monster._msquelch == UINT8_MAX && dTransVal[monster.position.tile.x][monster.position.tile.y] == dTransVal[fx][fy]) {
			if (monster._mgoalvar1++ < 2 * dist || !DirOK(i, md)) {
				RoundWalk(i, md, &monster._mgoalvar2);
			} else {
				monster._mgoal = MGOAL_NORMAL;
				StartFadein(monster, md, true);
			}
		} else {
			monster._mgoal = MGOAL_NORMAL;
			StartFadein(monster, md, true);
		}
	} else if (monster._mgoal == MGOAL_NORMAL) {
		if (abs(mx) >= 2 || abs(my) >= 2) {
			if (v < 5 * (monster._mint + 10) && LineClearMissile(monster.position.tile, { fx, fy })) {
				constexpr missile_id MissileTypes[4] = { MIS_FIREBOLT, MIS_CBOLT, MIS_LIGHTCTRL, MIS_FIREBALL };
				StartRangedAttack(monster, MissileTypes[monster._mint], monster.mMinDamage + GenerateRnd(monster.mMaxDamage - monster.mMinDamage + 1));
			} else if (GenerateRnd(100) < 30) {
				monster._mgoal = MGOAL_MOVE;
				monster._mgoalvar1 = 0;
				StartFadeout(monster, md, false);
			} else
				AiDelay(monster, GenerateRnd(10) + 2 * (5 - monster._mint));
		} else {
			monster._mdir = md;
			if (monster._mhitpoints < (monster._mmaxhp / 2)) {
				monster._mgoal = MGOAL_RETREAT;
				monster._mgoalvar1 = 0;
				StartFadeout(monster, md, false);
			} else if (static_cast<MonsterMode>(monster._mVar1) == MonsterMode::Delay
			    || GenerateRnd(100) < 2 * monster._mint + 20) {
				StartRangedAttack(monster, MIS_NULL, 0);
				AddMissile(monster.position.tile, { 0, 0 }, monster._mdir, MIS_FLASH, TARGET_PLAYERS, i, 4, 0);
				AddMissile(monster.position.tile, { 0, 0 }, monster._mdir, MIS_FLASH2, TARGET_PLAYERS, i, 4, 0);
			} else
				AiDelay(monster, GenerateRnd(10) + 2 * (5 - monster._mint));
		}
	}
	if (monster._mmode == MonsterMode::Stand) {
		AiDelay(monster, GenerateRnd(10) + 5);
	}
}

void ZharAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}

	Direction md = GetMonsterDirection(monster);
	if (monster.mtalkmsg == TEXT_ZHAR1 && !IsTileVisible(monster.position.tile) && monster._mgoal == MGOAL_TALKING) {
		monster.mtalkmsg = TEXT_ZHAR2;
		monster._mgoal = MGOAL_INQUIRING;
	}

	if (IsTileVisible(monster.position.tile)) {
		if (monster.mtalkmsg == TEXT_ZHAR2) {
			if (!effect_is_playing(USFX_ZHAR2) && monster._mgoal == MGOAL_TALKING) {
				monster._msquelch = UINT8_MAX;
				monster.mtalkmsg = TEXT_NONE;
				monster._mgoal = MGOAL_NORMAL;
			}
		}
	}

	if (monster._mgoal == MGOAL_NORMAL || monster._mgoal == MGOAL_RETREAT || monster._mgoal == MGOAL_MOVE)
		CounselorAi(i);

	monster.CheckStandAnimationIsLoaded(md);
}

void MegaAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	int mx = monster.position.tile.x - monster.enemyPosition.x;
	int my = monster.position.tile.y - monster.enemyPosition.y;
	if (abs(mx) >= 5 || abs(my) >= 5) {
		SkeletonAi(i);
		return;
	}

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	mx = monster.position.tile.x - fx;
	my = monster.position.tile.y - fy;
	Direction md = GetDirection(monster.position.tile, monster.position.last);
	if (monster._msquelch < UINT8_MAX)
		MonstCheckDoors(monster);
	int v = GenerateRnd(100);
	int dist = std::max(abs(mx), abs(my));
	if (dist >= 2 && monster._msquelch == UINT8_MAX && dTransVal[monster.position.tile.x][monster.position.tile.y] == dTransVal[fx][fy]) {
		if (monster._mgoal == MGOAL_MOVE || dist >= 3) {
			if (monster._mgoal != MGOAL_MOVE) {
				monster._mgoalvar1 = 0;
				monster._mgoalvar2 = GenerateRnd(2);
			}
			monster._mgoal = MGOAL_MOVE;
			monster._mgoalvar3 = 4;
			if (monster._mgoalvar1++ < 2 * dist || !DirOK(i, md)) {
				if (v < 5 * (monster._mint + 16))
					RoundWalk(i, md, &monster._mgoalvar2);
			} else
				monster._mgoal = MGOAL_NORMAL;
		}
	} else {
		monster._mgoal = MGOAL_NORMAL;
	}
	if (monster._mgoal == MGOAL_NORMAL) {
		if (((dist >= 3 && v < 5 * (monster._mint + 2)) || v < 5 * (monster._mint + 1) || monster._mgoalvar3 == 4) && LineClearMissile(monster.position.tile, { fx, fy })) {
			StartRangedSpecialAttack(monster, MIS_FLAMEC, 0);
		} else if (dist >= 2) {
			v = GenerateRnd(100);
			if (v < 2 * (5 * monster._mint + 25)
			    || (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways)
			        && monster._mVar2 == 0
			        && v < 2 * (5 * monster._mint + 40))) {
				RandomWalk(i, md);
			}
		} else {
			if (GenerateRnd(100) < 10 * (monster._mint + 4)) {
				monster._mdir = md;
				if (GenerateRnd(2) != 0)
					StartAttack(monster);
				else
					StartRangedSpecialAttack(monster, MIS_FLAMEC, 0);
			}
		}
		monster._mgoalvar3 = 1;
	}
	if (monster._mmode == MonsterMode::Stand) {
		AiDelay(monster, GenerateRnd(10) + 5);
	}
}

void DiabloAi(int i)
{
	AiRangedAvoidance(i, MIS_DIABAPOCA, false, 40, 0);
}

void LazarusAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}

	Direction md = GetMonsterDirection(monster);
	if (IsTileVisible(monster.position.tile)) {
		if (!gbIsMultiplayer) {
			auto &myPlayer = Players[MyPlayerId];
			if (monster.mtalkmsg == TEXT_VILE13 && monster._mgoal == MGOAL_INQUIRING && myPlayer.position.tile == Point { 35, 46 }) {
				PlayInGameMovie("gendata\\fprst3.smk");
				monster._mmode = MonsterMode::Talk;
				Quests[Q_BETRAYER]._qvar1 = 5;
			}

			if (monster.mtalkmsg == TEXT_VILE13 && !effect_is_playing(USFX_LAZ1) && monster._mgoal == MGOAL_TALKING) {
				ObjChangeMapResync(1, 18, 20, 24);
				RedoPlayerVision();
				Quests[Q_BETRAYER]._qvar1 = 6;
				monster._mgoal = MGOAL_NORMAL;
				monster._msquelch = UINT8_MAX;
				monster.mtalkmsg = TEXT_NONE;
			}
		}

		if (gbIsMultiplayer && monster.mtalkmsg == TEXT_VILE13 && monster._mgoal == MGOAL_INQUIRING && Quests[Q_BETRAYER]._qvar1 <= 3) {
			monster._mmode = MonsterMode::Talk;
		}
	}

	if (monster._mgoal == MGOAL_NORMAL || monster._mgoal == MGOAL_RETREAT || monster._mgoal == MGOAL_MOVE) {
		if (!gbIsMultiplayer && Quests[Q_BETRAYER]._qvar1 == 4 && monster.mtalkmsg == TEXT_NONE) { // Fix save games affected by teleport bug
			ObjChangeMapResync(1, 18, 20, 24);
			RedoPlayerVision();
			Quests[Q_BETRAYER]._qvar1 = 6;
		}
		monster.mtalkmsg = TEXT_NONE;
		CounselorAi(i);
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void LazarusMinionAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand)
		return;

	Direction md = GetMonsterDirection(monster);

	if (IsTileVisible(monster.position.tile)) {
		if (!gbIsMultiplayer) {
			if (Quests[Q_BETRAYER]._qvar1 <= 5) {
				monster._mgoal = MGOAL_INQUIRING;
			} else {
				monster._mgoal = MGOAL_NORMAL;
				monster.mtalkmsg = TEXT_NONE;
			}
		} else
			monster._mgoal = MGOAL_NORMAL;
	}
	if (monster._mgoal == MGOAL_NORMAL)
		SuccubusAi(i);

	monster.CheckStandAnimationIsLoaded(md);
}

void LachdananAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}

	Direction md = GetMonsterDirection(monster);

	if (monster.mtalkmsg == TEXT_VEIL9 && !IsTileVisible(monster.position.tile) && monster._mgoal == MGOAL_TALKING) {
		monster.mtalkmsg = TEXT_VEIL10;
		monster._mgoal = MGOAL_INQUIRING;
	}

	if (IsTileVisible(monster.position.tile)) {
		if (monster.mtalkmsg == TEXT_VEIL11) {
			if (!effect_is_playing(USFX_LACH3) && monster._mgoal == MGOAL_TALKING) {
				monster.mtalkmsg = TEXT_NONE;
				Quests[Q_VEIL]._qactive = QUEST_DONE;
				M_StartKill(i, -1);
			}
		}
	}

	monster.CheckStandAnimationIsLoaded(md);
}

void WarlordAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand) {
		return;
	}

	Direction md = GetMonsterDirection(monster);
	if (IsTileVisible(monster.position.tile)) {
		if (monster.mtalkmsg == TEXT_WARLRD9 && monster._mgoal == MGOAL_INQUIRING)
			monster._mmode = MonsterMode::Talk;
		if (monster.mtalkmsg == TEXT_WARLRD9 && !effect_is_playing(USFX_WARLRD1) && monster._mgoal == MGOAL_TALKING) {
			monster._msquelch = UINT8_MAX;
			monster.mtalkmsg = TEXT_NONE;
			monster._mgoal = MGOAL_NORMAL;
		}
	}

	if (monster._mgoal == MGOAL_NORMAL)
		SkeletonAi(i);

	monster.CheckStandAnimationIsLoaded(md);
}

void FirebatAi(int i)
{
	AiRanged(i, MIS_FIREBOLT, false);
}

void TorchantAi(int i)
{
	AiRanged(i, MIS_FIREBALL, false);
}

void HorkDemonAi(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mmode != MonsterMode::Stand || monster._msquelch == 0) {
		return;
	}

	int fx = monster.enemyPosition.x;
	int fy = monster.enemyPosition.y;
	int mx = monster.position.tile.x - fx;
	int my = monster.position.tile.y - fy;
	Direction md = GetDirection(monster.position.tile, monster.position.last);

	if (monster._msquelch < 255) {
		MonstCheckDoors(monster);
	}

	int v = GenerateRnd(100);

	if (abs(mx) < 2 && abs(my) < 2) {
		monster._mgoal = MGOAL_NORMAL;
	} else if (monster._mgoal == 4 || ((abs(mx) >= 5 || abs(my) >= 5) && GenerateRnd(4) != 0)) {
		if (monster._mgoal != 4) {
			monster._mgoalvar1 = 0;
			monster._mgoalvar2 = GenerateRnd(2);
		}
		monster._mgoal = MGOAL_MOVE;
		int dist = std::max(abs(mx), abs(my));
		if (monster._mgoalvar1++ >= 2 * dist || dTransVal[monster.position.tile.x][monster.position.tile.y] != dTransVal[fx][fy]) {
			monster._mgoal = MGOAL_NORMAL;
		} else if (!RoundWalk(i, md, &monster._mgoalvar2)) {
			AiDelay(monster, GenerateRnd(10) + 10);
		}
	}

	if (monster._mgoal == 1) {
		if ((abs(mx) >= 3 || abs(my) >= 3) && v < 2 * monster._mint + 43) {
			Point position = monster.position.tile + monster._mdir;
			if (IsTileAvailable(monster, position) && ActiveMonsterCount < MAXMONSTERS) {
				StartRangedSpecialAttack(monster, MIS_HORKDMN, 0);
			}
		} else if (abs(mx) < 2 && abs(my) < 2) {
			if (v < 2 * monster._mint + 28) {
				monster._mdir = md;
				StartAttack(monster);
			}
		} else {
			v = GenerateRnd(100);
			if (v < 2 * monster._mint + 33
			    || (IsAnyOf(static_cast<MonsterMode>(monster._mVar1), MonsterMode::MoveNorthwards, MonsterMode::MoveSouthwards, MonsterMode::MoveSideways) && monster._mVar2 == 0 && v < 2 * monster._mint + 83)) {
				RandomWalk(i, md);
			} else {
				AiDelay(monster, GenerateRnd(10) + 10);
			}
		}
	}

	monster.CheckStandAnimationIsLoaded(monster._mdir);
}

void LichAi(int i)
{
	AiRanged(i, MIS_LICH, false);
}

void ArchLichAi(int i)
{
	AiRanged(i, MIS_ARCHLICH, false);
}

void PsychorbAi(int i)
{
	AiRanged(i, MIS_PSYCHORB, false);
}

void NecromorbAi(int i)
{
	AiRanged(i, MIS_NECROMORB, false);
}

void BoneDemonAi(int i)
{
	AiRangedAvoidance(i, MIS_BONEDEMON, true, 4, 0);
}

const char *GetMonsterTypeText(const MonsterData &monsterData)
{
	switch (monsterData.mMonstClass) {
	case MonsterClass::Animal:
		return _("Animal");
	case MonsterClass::Demon:
		return _("Demon");
	case MonsterClass::Undead:
		return _("Undead");
	}

	app_fatal("Unknown mMonstClass %i", static_cast<int>(monsterData.mMonstClass));
}

void ActivateSpawn(int i, Point position, Direction dir)
{
	auto &monster = Monsters[i];
	dMonster[position.x][position.y] = i + 1;
	monster.position.tile = position;
	monster.position.future = position;
	monster.position.old = position;
	StartSpecialStand(monster, dir);
}

/** Maps from monster AI ID to monster AI function. */
void (*AiProc[])(int i) = {
	/*AI_ZOMBIE   */ &ZombieAi,
	/*AI_FAT      */ &OverlordAi,
	/*AI_SKELSD   */ &SkeletonAi,
	/*AI_SKELBOW  */ &SkeletonBowAi,
	/*AI_SCAV     */ &ScavengerAi,
	/*AI_RHINO    */ &RhinoAi,
	/*AI_GOATMC   */ &GoatAi,
	/*AI_GOATBOW  */ &GoatBowAi,
	/*AI_FALLEN   */ &FallenAi,
	/*AI_MAGMA    */ &MagmaAi,
	/*AI_SKELKING */ &LeoricAi,
	/*AI_BAT      */ &BatAi,
	/*AI_GARG     */ &GargoyleAi,
	/*AI_CLEAVER  */ &ButcherAi,
	/*AI_SUCC     */ &SuccubusAi,
	/*AI_SNEAK    */ &SneakAi,
	/*AI_STORM    */ &StormAi,
	/*AI_FIREMAN  */ nullptr,
	/*AI_GARBUD   */ &GharbadAi,
	/*AI_ACID     */ &AcidAvoidanceAi,
	/*AI_ACIDUNIQ */ &AcidAi,
	/*AI_GOLUM    */ &GolumAi,
	/*AI_ZHAR     */ &ZharAi,
	/*AI_SNOTSPIL */ &SnotSpilAi,
	/*AI_SNAKE    */ &SnakeAi,
	/*AI_COUNSLR  */ &CounselorAi,
	/*AI_MEGA     */ &MegaAi,
	/*AI_DIABLO   */ &DiabloAi,
	/*AI_LAZARUS  */ &LazarusAi,
	/*AI_LAZHELP  */ &LazarusMinionAi,
	/*AI_LACHDAN  */ &LachdananAi,
	/*AI_WARLORD  */ &WarlordAi,
	/*AI_FIREBAT  */ &FirebatAi,
	/*AI_TORCHANT */ &TorchantAi,
	/*AI_HORKDMN  */ &HorkDemonAi,
	/*AI_LICH     */ &LichAi,
	/*AI_ARCHLICH */ &ArchLichAi,
	/*AI_PSYCHORB */ &PsychorbAi,
	/*AI_NECROMORB*/ &NecromorbAi,
	/*AI_BONEDEMON*/ &BoneDemonAi
};

} // namespace

void InitLevelMonsters()
{
	LevelMonsterTypeCount = 0;
	monstimgtot = 0;

	for (auto &levelMonsterType : LevelMonsterTypes) {
		levelMonsterType.mPlaceFlags = 0;
	}

	ClrAllMonsters();
	ActiveMonsterCount = 0;
	totalmonsters = MAXMONSTERS;

	for (int i = 0; i < MAXMONSTERS; i++) {
		ActiveMonsters[i] = i;
	}

	uniquetrans = 0;
}

void GetLevelMTypes()
{
	// this array is merged with skeltypes down below.
	_monster_id typelist[MAXMONSTERS];
	_monster_id skeltypes[NUM_MTYPES];

	int minl; // min level
	int maxl; // max level
	char mamask;
	const int numskeltypes = 19;

	int nt; // number of types

	if (gbIsSpawn)
		mamask = 1; // monster availability mask
	else
		mamask = 3; // monster availability mask

	AddMonsterType(MT_GOLEM, PLACE_SPECIAL);
	if (currlevel == 16) {
		AddMonsterType(MT_ADVOCATE, PLACE_SCATTER);
		AddMonsterType(MT_RBLACK, PLACE_SCATTER);
		AddMonsterType(MT_DIABLO, PLACE_SPECIAL);
		return;
	}

	if (currlevel == 18)
		AddMonsterType(MT_HORKSPWN, PLACE_SCATTER);
	if (currlevel == 19) {
		AddMonsterType(MT_HORKSPWN, PLACE_SCATTER);
		AddMonsterType(MT_HORKDMN, PLACE_UNIQUE);
	}
	if (currlevel == 20)
		AddMonsterType(MT_DEFILER, PLACE_UNIQUE);
	if (currlevel == 24) {
		AddMonsterType(MT_ARCHLICH, PLACE_SCATTER);
		AddMonsterType(MT_NAKRUL, PLACE_SPECIAL);
	}

	if (!setlevel) {
		if (Quests[Q_BUTCHER].IsAvailable())
			AddMonsterType(MT_CLEAVER, PLACE_SPECIAL);
		if (Quests[Q_GARBUD].IsAvailable())
			AddMonsterType(UniqueMonstersData[UMT_GARBUD].mtype, PLACE_UNIQUE);
		if (Quests[Q_ZHAR].IsAvailable())
			AddMonsterType(UniqueMonstersData[UMT_ZHAR].mtype, PLACE_UNIQUE);
		if (Quests[Q_LTBANNER].IsAvailable())
			AddMonsterType(UniqueMonstersData[UMT_SNOTSPIL].mtype, PLACE_UNIQUE);
		if (Quests[Q_VEIL].IsAvailable())
			AddMonsterType(UniqueMonstersData[UMT_LACHDAN].mtype, PLACE_UNIQUE);
		if (Quests[Q_WARLORD].IsAvailable())
			AddMonsterType(UniqueMonstersData[UMT_WARLORD].mtype, PLACE_UNIQUE);

		if (gbIsMultiplayer && currlevel == Quests[Q_SKELKING]._qlevel) {

			AddMonsterType(MT_SKING, PLACE_UNIQUE);

			nt = 0;
			for (int i = MT_WSKELAX; i <= MT_WSKELAX + numskeltypes; i++) {
				if (IsSkel(i)) {
					minl = 15 * MonstersData[i].mMinDLvl / 30 + 1;
					maxl = 15 * MonstersData[i].mMaxDLvl / 30 + 1;

					if (currlevel >= minl && currlevel <= maxl) {
						if ((MonstAvailTbl[i] & mamask) != 0) {
							skeltypes[nt++] = (_monster_id)i;
						}
					}
				}
			}
			AddMonsterType(skeltypes[GenerateRnd(nt)], PLACE_SCATTER);
		}

		nt = 0;
		for (int i = MT_NZOMBIE; i < NUM_MTYPES; i++) {
			minl = 15 * MonstersData[i].mMinDLvl / 30 + 1;
			maxl = 15 * MonstersData[i].mMaxDLvl / 30 + 1;

			if (currlevel >= minl && currlevel <= maxl) {
				if ((MonstAvailTbl[i] & mamask) != 0) {
					typelist[nt++] = (_monster_id)i;
				}
			}
		}

		while (nt > 0 && LevelMonsterTypeCount < MAX_LVLMTYPES && monstimgtot < 4000) {
			for (int i = 0; i < nt;) {
				if (MonstersData[typelist[i]].mImage > 4000 - monstimgtot) {
					typelist[i] = typelist[--nt];
					continue;
				}

				i++;
			}

			if (nt != 0) {
				int i = GenerateRnd(nt);
				AddMonsterType(typelist[i], PLACE_SCATTER);
				typelist[i] = typelist[--nt];
			}
		}

	} else {
		if (setlvlnum == SL_SKELKING) {
			AddMonsterType(MT_SKING, PLACE_UNIQUE);
		}
	}
}

void InitMonsterGFX(int monst)
{
	int mtype = LevelMonsterTypes[monst].mtype;
	int width = MonstersData[mtype].width;

	for (int anim = 0; anim < 6; anim++) {
		int frames = MonstersData[mtype].Frames[anim];

		if ((animletter[anim] != 's' || MonstersData[mtype].has_special) && frames > 0) {
			char strBuff[256];
			sprintf(strBuff, MonstersData[mtype].GraphicType, animletter[anim]);

			byte *celBuf;
			{
				auto celData = LoadFileInMem(strBuff);
				celBuf = celData.get();
				LevelMonsterTypes[monst].Anims[anim].CMem = std::move(celData);
			}

			if (LevelMonsterTypes[monst].mtype != MT_GOLEM || (animletter[anim] != 's' && animletter[anim] != 'd')) {
				for (int i = 0; i < 8; i++) {
					byte *pCelStart = CelGetFrame(celBuf, i);
					LevelMonsterTypes[monst].Anims[anim].CelSpritesForDirections[i].emplace(pCelStart, width);
				}
			} else {
				for (int i = 0; i < 8; i++) {
					LevelMonsterTypes[monst].Anims[anim].CelSpritesForDirections[i].emplace(celBuf, width);
				}
			}
		}

		LevelMonsterTypes[monst].Anims[anim].Frames = frames;
		LevelMonsterTypes[monst].Anims[anim].Rate = MonstersData[mtype].Rate[anim];
	}

	LevelMonsterTypes[monst].mMinHP = MonstersData[mtype].mMinHP;
	LevelMonsterTypes[monst].mMaxHP = MonstersData[mtype].mMaxHP;
	if (!gbIsHellfire && mtype == MT_DIABLO) {
		LevelMonsterTypes[monst].mMinHP -= 2000;
		LevelMonsterTypes[monst].mMaxHP -= 2000;
	}
	LevelMonsterTypes[monst].mAFNum = MonstersData[mtype].mAFNum;
	LevelMonsterTypes[monst].MData = &MonstersData[mtype];

	if (MonstersData[mtype].has_trans) {
		InitMonsterTRN(LevelMonsterTypes[monst]);
	}

	if (mtype >= MT_NMAGMA && mtype <= MT_WMAGMA)
		MissileSpriteData[MFILE_MAGBALL].LoadGFX();
	if (mtype >= MT_STORM && mtype <= MT_MAEL)
		MissileSpriteData[MFILE_THINLGHT].LoadGFX();
	if (mtype == MT_SNOWWICH) {
		MissileSpriteData[MFILE_SCUBMISB].LoadGFX();
		MissileSpriteData[MFILE_SCBSEXPB].LoadGFX();
	}
	if (mtype == MT_HLSPWN) {
		MissileSpriteData[MFILE_SCUBMISD].LoadGFX();
		MissileSpriteData[MFILE_SCBSEXPD].LoadGFX();
	}
	if (mtype == MT_SOLBRNR) {
		MissileSpriteData[MFILE_SCUBMISC].LoadGFX();
		MissileSpriteData[MFILE_SCBSEXPC].LoadGFX();
	}
	if ((mtype >= MT_NACID && mtype <= MT_XACID) || mtype == MT_SPIDLORD) {
		MissileSpriteData[MFILE_ACIDBF].LoadGFX();
		MissileSpriteData[MFILE_ACIDSPLA].LoadGFX();
		MissileSpriteData[MFILE_ACIDPUD].LoadGFX();
	}
	if (mtype == MT_LICH) {
		MissileSpriteData[MFILE_LICH].LoadGFX();
		MissileSpriteData[MFILE_EXORA1].LoadGFX();
	}
	if (mtype == MT_ARCHLICH) {
		MissileSpriteData[MFILE_ARCHLICH].LoadGFX();
		MissileSpriteData[MFILE_EXYEL2].LoadGFX();
	}
	if (mtype == MT_PSYCHORB || mtype == MT_BONEDEMN)
		MissileSpriteData[MFILE_BONEDEMON].LoadGFX();
	if (mtype == MT_NECRMORB) {
		MissileSpriteData[MFILE_NECROMORB].LoadGFX();
		MissileSpriteData[MFILE_EXRED3].LoadGFX();
	}
	if (mtype == MT_PSYCHORB)
		MissileSpriteData[MFILE_EXBL2].LoadGFX();
	if (mtype == MT_BONEDEMN)
		MissileSpriteData[MFILE_EXBL3].LoadGFX();
	if (mtype == MT_DIABLO)
		MissileSpriteData[MFILE_FIREPLAR].LoadGFX();
}

void monster_some_crypt()
{
	if (currlevel != 24 || UberDiabloMonsterIndex < 0 || UberDiabloMonsterIndex >= ActiveMonsterCount)
		return;

	auto &monster = Monsters[UberDiabloMonsterIndex];
	PlayEffect(monster, 2);
	Quests[Q_NAKRUL]._qlog = false;
	monster.mArmorClass -= 50;
	int hp = monster._mmaxhp / 2;
	monster.mMagicRes = 0;
	monster._mhitpoints = hp;
	monster._mmaxhp = hp;
}

void InitMonsters()
{
	if (!setlevel) {
		for (int i = 0; i < MAX_PLRS; i++)
			AddMonster(GolemHoldingCell, Direction::South, 0, false);
	}

	if (!gbIsSpawn && !setlevel && currlevel == 16)
		LoadDiabMonsts();

	int nt = numtrigs;
	if (currlevel == 15)
		nt = 1;
	for (int i = 0; i < nt; i++) {
		for (int s = -2; s < 2; s++) {
			for (int t = -2; t < 2; t++)
				DoVision(trigs[i].position + Displacement { s, t }, 15, MAP_EXP_NONE, false);
		}
	}
	if (!gbIsSpawn)
		PlaceQuestMonsters();
	if (!setlevel) {
		if (!gbIsSpawn)
			PlaceUniqueMonsters();
		int na = 0;
		for (int s = 16; s < 96; s++) {
			for (int t = 16; t < 96; t++) {
				if (!IsTileSolid({ s, t }))
					na++;
			}
		}
		int numplacemonsters = na / 30;
		if (gbIsMultiplayer)
			numplacemonsters += numplacemonsters / 2;
		if (ActiveMonsterCount + numplacemonsters > MAXMONSTERS - 10)
			numplacemonsters = MAXMONSTERS - 10 - ActiveMonsterCount;
		totalmonsters = ActiveMonsterCount + numplacemonsters;
		int numscattypes = 0;
		int scattertypes[NUM_MTYPES];
		for (int i = 0; i < LevelMonsterTypeCount; i++) {
			if ((LevelMonsterTypes[i].mPlaceFlags & PLACE_SCATTER) != 0) {
				scattertypes[numscattypes] = i;
				numscattypes++;
			}
		}
		while (ActiveMonsterCount < totalmonsters) {
			int mtype = scattertypes[GenerateRnd(numscattypes)];
			if (currlevel == 1 || GenerateRnd(2) == 0)
				na = 1;
			else if (currlevel == 2 || (currlevel >= 21 && currlevel <= 24))
				na = GenerateRnd(2) + 2;
			else
				na = GenerateRnd(3) + 3;
			PlaceGroup(mtype, na, UniqueMonsterPack::None, 0);
		}
	}
	for (int i = 0; i < nt; i++) {
		for (int s = -2; s < 2; s++) {
			for (int t = -2; t < 2; t++)
				DoUnVision(trigs[i].position + Displacement { s, t }, 15);
		}
	}
}

void SetMapMonsters(const uint16_t *dunData, Point startPosition)
{
	AddMonsterType(MT_GOLEM, PLACE_SPECIAL);
	if (setlevel)
		for (int i = 0; i < MAX_PLRS; i++)
			AddMonster(GolemHoldingCell, Direction::South, 0, false);

	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		AddMonsterType(UniqueMonstersData[UMT_LAZARUS].mtype, PLACE_UNIQUE);
		AddMonsterType(UniqueMonstersData[UMT_RED_VEX].mtype, PLACE_UNIQUE);
		AddMonsterType(UniqueMonstersData[UMT_BLACKJADE].mtype, PLACE_UNIQUE);
		PlaceUniqueMonst(UMT_LAZARUS, 0, 0);
		PlaceUniqueMonst(UMT_RED_VEX, 0, 0);
		PlaceUniqueMonst(UMT_BLACKJADE, 0, 0);
	}

	int width = SDL_SwapLE16(dunData[0]);
	int height = SDL_SwapLE16(dunData[1]);

	int layer2Offset = 2 + width * height;

	// The rest of the layers are at dPiece scale
	width *= 2;
	height *= 2;

	const uint16_t *monsterLayer = &dunData[layer2Offset + width * height];

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			uint8_t monsterId = SDL_SwapLE16(monsterLayer[j * width + i]);
			if (monsterId != 0) {
				int mtype = AddMonsterType(MonstConvTbl[monsterId - 1], PLACE_SPECIAL);
				PlaceMonster(ActiveMonsterCount++, mtype, i + startPosition.x + 16, j + startPosition.y + 16);
			}
		}
	}
}

int AddMonster(Point position, Direction dir, int mtype, bool inMap)
{
	if (ActiveMonsterCount < MAXMONSTERS) {
		int i = ActiveMonsters[ActiveMonsterCount++];
		if (inMap)
			dMonster[position.x][position.y] = i + 1;
		InitMonster(Monsters[i], dir, mtype, position);
		return i;
	}

	return -1;
}

void AddDoppelganger(Monster &monster)
{
	if (monster.MType == nullptr) {
		return;
	}

	Point target = { 0, 0 };
	for (int d = 0; d < 8; d++) {
		const Point position = monster.position.tile + static_cast<Direction>(d);
		if (!IsTileAvailable(position))
			continue;
		target = position;
	}
	if (target != Point { 0, 0 }) {
		for (int j = 0; j < MAX_LVLMTYPES; j++) {
			if (LevelMonsterTypes[j].mtype == monster.MType->mtype) {
				AddMonster(target, monster._mdir, j, true);
				break;
			}
		}
	}
}

bool M_Talker(const Monster &monster)
{
	return IsAnyOf(monster._mAi, AI_LAZARUS, AI_WARLORD, AI_GARBUD, AI_ZHAR, AI_SNOTSPIL, AI_LACHDAN, AI_LAZHELP);
}

void M_StartStand(Monster &monster, Direction md)
{
	ClearMVars(monster);
	if (monster.MType->mtype == MT_GOLEM)
		NewMonsterAnim(monster, MonsterGraphic::Walk, md);
	else
		NewMonsterAnim(monster, MonsterGraphic::Stand, md);
	monster._mVar1 = static_cast<int>(monster._mmode);
	monster._mVar2 = 0;
	monster._mmode = MonsterMode::Stand;
	monster.position.offset = { 0, 0 };
	monster.position.future = monster.position.tile;
	monster.position.old = monster.position.tile;
	UpdateEnemy(monster);
}

void M_ClearSquares(int i)
{
	auto &monster = Monsters[i];

	int mx = monster.position.old.x;
	int my = monster.position.old.y;
	int m1 = -(i + 1);
	int m2 = i + 1;

	for (int y = my - 1; y <= my + 1; y++) {
		for (int x = mx - 1; x <= mx + 1; x++) {
			if (InDungeonBounds({ x, y }) && (dMonster[x][y] == m1 || dMonster[x][y] == m2))
				dMonster[x][y] = 0;
		}
	}
}

void M_GetKnockback(int i)
{
	auto &monster = Monsters[i];

	Direction d = Opposite(monster._mdir);
	if (!DirOK(i, d)) {
		return;
	}

	M_ClearSquares(i);
	monster.position.old += d;
	StartMonsterGotHit(i);
}

void M_StartHit(int i, int pnum, int dam)
{
	auto &monster = Monsters[i];

	if (pnum >= 0)
		monster.mWhoHit |= 1 << pnum;
	if (pnum == MyPlayerId) {
		delta_monster_hp(i, monster._mhitpoints, currlevel);
		NetSendCmdMonDmg(false, i, dam);
	}
	PlayEffect(monster, 1);
	if ((monster.MType->mtype >= MT_SNEAK && monster.MType->mtype <= MT_ILLWEAV) || dam >> 6 >= monster.mLevel + 3) {
		if (pnum >= 0) {
			monster._menemy = pnum;
			monster.enemyPosition = Players[pnum].position.future;
			monster._mFlags &= ~MFLAG_TARGETS_MONSTER;
			monster._mdir = GetMonsterDirection(monster);
		}
		if (monster.MType->mtype == MT_BLINK) {
			Teleport(i);
		} else if ((monster.MType->mtype >= MT_NSCAV && monster.MType->mtype <= MT_YSCAV)
		    || monster.MType->mtype == MT_GRAVEDIG) {
			monster._mgoal = MGOAL_NORMAL;
			monster._mgoalvar1 = 0;
			monster._mgoalvar2 = 0;
		}
		if (monster._mmode != MonsterMode::Petrified) {
			StartMonsterGotHit(i);
		}
	}
}

void M_StartKill(int i, int pnum)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (MyPlayerId == pnum) {
		delta_kill_monster(i, monster.position.tile, currlevel);
		if (i != pnum) {
			NetSendCmdLocParam1(false, CMD_MONSTDEATH, monster.position.tile, i);
		} else {
			NetSendCmdLocParam1(false, CMD_KILLGOLEM, monster.position.tile, currlevel);
		}
	}

	StartMonsterDeath(i, pnum, true);
}

void M_SyncStartKill(int i, Point position, int pnum)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	if (monster._mhitpoints == 0 || monster._mmode == MonsterMode::Death) {
		return;
	}

	if (dMonster[position.x][position.y] == 0) {
		M_ClearSquares(i);
		monster.position.tile = position;
		monster.position.old = position;
	}

	if (monster._mmode == MonsterMode::Petrified) {
		StartMonsterDeath(i, pnum, false);
		monster.Petrify();
	} else {
		StartMonsterDeath(i, pnum, false);
	}
}

void M_UpdateLeader(int i)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];

	for (int j = 0; j < ActiveMonsterCount; j++) {
		auto &minion = Monsters[ActiveMonsters[j]];
		if (minion.leaderRelation == LeaderRelation::Leashed && minion.leader == i)
			minion.leaderRelation = LeaderRelation::None;
	}

	if (monster.leaderRelation == LeaderRelation::Leashed) {
		Monsters[monster.leader].packsize--;
	}
}

void DoEnding()
{
	if (gbIsMultiplayer) {
		SNetLeaveGame(LEAVE_ENDING);
	}

	music_stop();

	if (gbIsMultiplayer) {
		SDL_Delay(1000);
	}

	if (gbIsSpawn)
		return;

	switch (Players[MyPlayerId]._pClass) {
	case HeroClass::Sorcerer:
	case HeroClass::Monk:
		play_movie("gendata\\DiabVic1.smk", false);
		break;
	case HeroClass::Warrior:
	case HeroClass::Barbarian:
		play_movie("gendata\\DiabVic2.smk", false);
		break;
	default:
		play_movie("gendata\\DiabVic3.smk", false);
		break;
	}
	play_movie("gendata\\Diabend.smk", false);

	bool bMusicOn = gbMusicOn;
	gbMusicOn = true;

	int musicVolume = sound_get_or_set_music_volume(1);
	sound_get_or_set_music_volume(0);

	music_start(TMUSIC_L2);
	loop_movie = true;
	play_movie("gendata\\loopdend.smk", true);
	loop_movie = false;
	music_stop();

	sound_get_or_set_music_volume(musicVolume);
	gbMusicOn = bMusicOn;
}

void PrepDoEnding()
{
	gbSoundOn = sgbSaveSoundOn;
	gbRunGame = false;
	MyPlayerIsDead = false;
	cineflag = true;

	auto &myPlayer = Players[MyPlayerId];

	myPlayer.pDiabloKillLevel = std::max(myPlayer.pDiabloKillLevel, static_cast<uint8_t>(sgGameInitInfo.nDifficulty + 1));

	for (auto &player : Players) {
		player._pmode = PM_QUIT;
		player._pInvincible = true;
		if (gbIsMultiplayer) {
			if (player._pHitPoints >> 6 == 0)
				player._pHitPoints = 64;
			if (player._pMana >> 6 == 0)
				player._pMana = 64;
		}
	}
}

void M_WalkDir(int i, Direction md)
{
	assert(i >= 0 && i < MAXMONSTERS);

	int mwi = Monsters[i].MType->GetAnimData(MonsterGraphic::Walk).Frames - 1;
	switch (md) {
	case Direction::North:
		StartWalk(i, 0, -MWVel[mwi][1], -1, -1, Direction::North);
		break;
	case Direction::NorthEast:
		StartWalk(i, MWVel[mwi][1], -MWVel[mwi][0], 0, -1, Direction::NorthEast);
		break;
	case Direction::East:
		StartWalk3(i, MWVel[mwi][2], 0, -32, -16, 1, -1, 1, 0, Direction::East);
		break;
	case Direction::SouthEast:
		StartWalk2(i, MWVel[mwi][1], MWVel[mwi][0], -32, -16, 1, 0, Direction::SouthEast);
		break;
	case Direction::South:
		StartWalk2(i, 0, MWVel[mwi][1], 0, -32, 1, 1, Direction::South);
		break;
	case Direction::SouthWest:
		StartWalk2(i, -MWVel[mwi][1], MWVel[mwi][0], 32, -16, 0, 1, Direction::SouthWest);
		break;
	case Direction::West:
		StartWalk3(i, -MWVel[mwi][2], 0, 32, -16, -1, 1, 0, 1, Direction::West);
		break;
	case Direction::NorthWest:
		StartWalk(i, -MWVel[mwi][1], -MWVel[mwi][0], -1, 0, Direction::NorthWest);
		break;
	}
}

void GolumAi(int i)
{
	assert(i >= 0 && i < MAX_PLRS);
	auto &golem = Monsters[i];

	if (golem.position.tile.x == 1 && golem.position.tile.y == 0) {
		return;
	}

	if (IsAnyOf(golem._mmode, MonsterMode::Death, MonsterMode::SpecialStand) || golem.IsWalking()) {
		return;
	}

	if ((golem._mFlags & MFLAG_TARGETS_MONSTER) == 0)
		UpdateEnemy(golem);

	if (golem._mmode == MonsterMode::MeleeAttack) {
		return;
	}

	if ((golem._mFlags & MFLAG_NO_ENEMY) == 0) {
		auto &enemy = Monsters[golem._menemy];
		int mex = golem.position.tile.x - enemy.position.future.x;
		int mey = golem.position.tile.y - enemy.position.future.y;
		golem._mdir = GetDirection(golem.position.tile, enemy.position.tile);
		if (abs(mex) < 2 && abs(mey) < 2) {
			golem.enemyPosition = enemy.position.tile;
			if (enemy._msquelch == 0) {
				enemy._msquelch = UINT8_MAX;
				enemy.position.last = golem.position.tile;
				for (int j = 0; j < 5; j++) {
					for (int k = 0; k < 5; k++) {
						int enemyId = dMonster[golem.position.tile.x + k - 2][golem.position.tile.y + j - 2]; // BUGFIX: Check if indexes are between 0 and 112
						if (enemyId > 0)
							Monsters[enemyId - 1]._msquelch = UINT8_MAX; // BUGFIX: should be `Monsters[_menemy-1]`, not Monsters[_menemy]. (fixed)
					}
				}
			}
			StartAttack(golem);
			return;
		}
		if (AiPlanPath(i))
			return;
	}

	golem._pathcount++;
	if (golem._pathcount > 8)
		golem._pathcount = 5;

	if (RandomWalk(i, Players[i]._pdir))
		return;

	Direction md = Left(golem._mdir);
	bool ok = false;
	for (int j = 0; j < 8 && !ok; j++) {
		md = Right(md);
		ok = DirOK(i, md);
	}
	if (ok)
		M_WalkDir(i, md);
}

void DeleteMonsterList()
{
	for (int i = 0; i < MAX_PLRS; i++) {
		auto &golem = Monsters[i];
		if (!golem._mDelFlag)
			continue;

		golem.position.tile = GolemHoldingCell;
		golem.position.future = { 0, 0 };
		golem.position.old = { 0, 0 };
		golem._mDelFlag = false;
	}

	for (int i = MAX_PLRS; i < ActiveMonsterCount;) {
		if (Monsters[ActiveMonsters[i]]._mDelFlag) {
			if (pcursmonst == ActiveMonsters[i]) // Unselect monster if player highlighted it
				pcursmonst = -1;
			DeleteMonster(i);
		} else {
			i++;
		}
	}
}

void ProcessMonsters()
{
	DeleteMonsterList();

	assert(ActiveMonsterCount >= 0 && ActiveMonsterCount <= MAXMONSTERS);
	for (int i = 0; i < ActiveMonsterCount; i++) {
		int mi = ActiveMonsters[i];
		auto &monster = Monsters[mi];
		FollowTheLeader(monster);
		bool raflag = false;
		if (gbIsMultiplayer) {
			SetRndSeed(monster._mAISeed);
			monster._mAISeed = AdvanceRndSeed();
		}
		if ((monster._mFlags & MFLAG_NOHEAL) == 0 && monster._mhitpoints < monster._mmaxhp && monster._mhitpoints >> 6 > 0) {
			if (monster.mLevel > 1) {
				monster._mhitpoints += monster.mLevel / 2;
			} else {
				monster._mhitpoints += monster.mLevel;
			}
		}

		if (IsTileVisible(monster.position.tile) && monster._msquelch == 0) {
			if (monster.MType->mtype == MT_CLEAVER) {
				PlaySFX(USFX_CLEAVER);
			}
			if (monster.MType->mtype == MT_NAKRUL) {
				if (sgGameInitInfo.bCowQuest != 0) {
					PlaySFX(USFX_NAKRUL6);
				} else {
					if (IsUberRoomOpened)
						PlaySFX(USFX_NAKRUL4);
					else
						PlaySFX(USFX_NAKRUL5);
				}
			}
			if (monster.MType->mtype == MT_DEFILER)
				PlaySFX(USFX_DEFILER8);
			UpdateEnemy(monster);
		}

		if ((monster._mFlags & MFLAG_TARGETS_MONSTER) != 0) {
			assert(monster._menemy >= 0 && monster._menemy < MAXMONSTERS);
			monster.position.last = Monsters[monster._menemy].position.future;
			monster.enemyPosition = monster.position.last;
		} else {
			assert(monster._menemy >= 0 && monster._menemy < MAX_PLRS);
			auto &player = Players[monster._menemy];
			monster.enemyPosition = player.position.future;
			if (IsTileVisible(monster.position.tile)) {
				monster._msquelch = UINT8_MAX;
				monster.position.last = player.position.future;
			} else if (monster._msquelch != 0 && monster.MType->mtype != MT_DIABLO) { /// BUGFIX: change '_mAi' to 'MType->mtype'
				monster._msquelch--;
			}
		}
		do {
			if ((monster._mFlags & MFLAG_SEARCH) == 0 || !AiPlanPath(mi)) {
				AiProc[monster._mAi](mi);
			}
			switch (monster._mmode) {
			case MonsterMode::Stand:
				raflag = MonsterIdle(monster);
				break;
			case MonsterMode::MoveNorthwards:
			case MonsterMode::MoveSouthwards:
			case MonsterMode::MoveSideways:
				raflag = MonsterWalk(mi, monster._mmode);
				break;
			case MonsterMode::MeleeAttack:
				raflag = MonsterAttack(mi);
				break;
			case MonsterMode::HitRecovery:
				raflag = MonsterGotHit(monster);
				break;
			case MonsterMode::Death:
				raflag = MonsterDeath(mi);
				break;
			case MonsterMode::SpecialMeleeAttack:
				raflag = MonsterSpecialAttack(mi);
				break;
			case MonsterMode::FadeIn:
				raflag = MonsterFadein(monster);
				break;
			case MonsterMode::FadeOut:
				raflag = MonsterFadeout(monster);
				break;
			case MonsterMode::RangedAttack:
				raflag = MonaterRangedAttack(mi);
				break;
			case MonsterMode::SpecialStand:
				raflag = MonsterSpecialStand(monster);
				break;
			case MonsterMode::SpecialRangedAttack:
				raflag = MonsterRangedSpecialAttack(mi);
				break;
			case MonsterMode::Delay:
				raflag = MonsterDelay(monster);
				break;
			case MonsterMode::Charge:
				raflag = false;
				break;
			case MonsterMode::Petrified:
				raflag = MonsterPetrified(monster);
				break;
			case MonsterMode::Heal:
				raflag = MonsterHeal(monster);
				break;
			case MonsterMode::Talk:
				raflag = MonsterTalk(monster);
				break;
			}
			if (raflag) {
				GroupUnity(monster);
			}
		} while (raflag);
		if (monster._mmode != MonsterMode::Petrified) {
			monster.AnimInfo.ProcessAnimation((monster._mFlags & MFLAG_LOCK_ANIMATION) != 0, (monster._mFlags & MFLAG_ALLOW_SPECIAL) != 0);
		}
	}

	DeleteMonsterList();
}

void FreeMonsters()
{
	for (int i = 0; i < LevelMonsterTypeCount; i++) {
		int mtype = LevelMonsterTypes[i].mtype;
		for (int j = 0; j < 6; j++) {
			if (animletter[j] != 's' || MonstersData[mtype].has_special) {
				LevelMonsterTypes[i].Anims[j].CMem = nullptr;
			}
		}
	}
}

bool DirOK(int i, Direction mdir)
{
	assert(i >= 0 && i < MAXMONSTERS);
	auto &monster = Monsters[i];
	Point position = monster.position.tile;
	Point futurePosition = position + mdir;
	if (!InDungeonBounds(futurePosition) || !IsTileAvailable(monster, futurePosition))
		return false;
	if (mdir == Direction::East) {
		if (IsTileSolid(position + Direction::SouthEast))
			return false;
	} else if (mdir == Direction::West) {
		if (IsTileSolid(position + Direction::SouthWest))
			return false;
	} else if (mdir == Direction::North) {
		if (IsTileSolid(position + Direction::NorthEast) || IsTileSolid(position + Direction::NorthWest))
			return false;
	} else if (mdir == Direction::South)
		if (IsTileSolid(position + Direction::SouthWest) || IsTileSolid(position + Direction::SouthEast))
			return false;
	if (monster.leaderRelation == LeaderRelation::Leashed) {
		return futurePosition.WalkingDistance(Monsters[monster.leader].position.future) < 4;
	}
	if (monster._uniqtype == 0 || UniqueMonstersData[monster._uniqtype - 1].monsterPack != UniqueMonsterPack::Leashed)
		return true;
	int mcount = 0;
	for (int x = futurePosition.x - 3; x <= futurePosition.x + 3; x++) {
		for (int y = futurePosition.y - 3; y <= futurePosition.y + 3; y++) {
			if (!InDungeonBounds({ x, y }))
				continue;
			int mi = dMonster[x][y];
			if (mi == 0)
				continue;

			auto &minion = Monsters[(mi < 0) ? -(mi + 1) : (mi - 1)];
			if (minion.leaderRelation == LeaderRelation::Leashed
			    && minion.leader == i
			    && minion.position.future == Point { x, y }) {
				mcount++;
			}
		}
	}
	return mcount == monster.packsize;
}

bool PosOkMissile(Point position)
{
	return !nMissileTable[dPiece[position.x][position.y]];
}

bool LineClearMissile(Point startPoint, Point endPoint)
{
	return LineClear(PosOkMissile, startPoint, endPoint);
}

bool LineClear(const std::function<bool(Point)> &clear, Point startPoint, Point endPoint)
{
	Point position = startPoint;

	int dx = endPoint.x - position.x;
	int dy = endPoint.y - position.y;
	if (abs(dx) > abs(dy)) {
		if (dx < 0) {
			std::swap(position, endPoint);
			dx = -dx;
			dy = -dy;
		}
		int d;
		int yincD;
		int dincD;
		int dincH;
		if (dy > 0) {
			d = 2 * dy - dx;
			dincD = 2 * dy;
			dincH = 2 * (dy - dx);
			yincD = 1;
		} else {
			d = 2 * dy + dx;
			dincD = 2 * dy;
			dincH = 2 * (dx + dy);
			yincD = -1;
		}
		bool done = false;
		while (!done && position != endPoint) {
			if ((d <= 0) ^ (yincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				position.y += yincD;
			}
			position.x++;
			done = position != startPoint && !clear(position);
		}
	} else {
		if (dy < 0) {
			std::swap(position, endPoint);
			dy = -dy;
			dx = -dx;
		}
		int d;
		int xincD;
		int dincD;
		int dincH;
		if (dx > 0) {
			d = 2 * dx - dy;
			dincD = 2 * dx;
			dincH = 2 * (dx - dy);
			xincD = 1;
		} else {
			d = 2 * dx + dy;
			dincD = 2 * dx;
			dincH = 2 * (dy + dx);
			xincD = -1;
		}
		bool done = false;
		while (!done && position != endPoint) {
			if ((d <= 0) ^ (xincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				position.x += xincD;
			}
			position.y++;
			done = position != startPoint && !clear(position);
		}
	}
	return position == endPoint;
}

void SyncMonsterAnim(Monster &monster)
{
	monster.MType = &LevelMonsterTypes[monster._mMTidx];
#ifdef _DEBUG
	// fix for saves with debug monsters having type originally not on the level
	if (LevelMonsterTypes[monster._mMTidx].MData == nullptr) {
		InitMonsterGFX(monster._mMTidx);
		LevelMonsterTypes[monster._mMTidx].mdeadval = 1;
	}
#endif
	monster.MData = LevelMonsterTypes[monster._mMTidx].MData;
	if (monster._uniqtype != 0)
		monster.mName = pgettext("monster", UniqueMonstersData[monster._uniqtype - 1].mName);
	else
		monster.mName = pgettext("monster", monster.MData->mName);

	MonsterGraphic graphic = MonsterGraphic::Stand;

	switch (monster._mmode) {
	case MonsterMode::Stand:
	case MonsterMode::Delay:
	case MonsterMode::Talk:
		break;
	case MonsterMode::MoveNorthwards:
	case MonsterMode::MoveSouthwards:
	case MonsterMode::MoveSideways:
		graphic = MonsterGraphic::Walk;
		break;
	case MonsterMode::MeleeAttack:
	case MonsterMode::RangedAttack:
		graphic = MonsterGraphic::Attack;
		break;
	case MonsterMode::HitRecovery:
		graphic = MonsterGraphic::GotHit;
		break;
	case MonsterMode::Death:
		graphic = MonsterGraphic::Death;
		break;
	case MonsterMode::SpecialMeleeAttack:
	case MonsterMode::FadeIn:
	case MonsterMode::FadeOut:
	case MonsterMode::SpecialStand:
	case MonsterMode::SpecialRangedAttack:
	case MonsterMode::Heal:
		graphic = MonsterGraphic::Special;
		break;
	case MonsterMode::Charge:
		graphic = MonsterGraphic::Attack;
		monster.AnimInfo.CurrentFrame = 1;
		break;
	default:
		monster.AnimInfo.CurrentFrame = 1;
		break;
	}

	monster.ChangeAnimationData(graphic);
}

void M_FallenFear(Point position)
{
	for (int i = 0; i < ActiveMonsterCount; i++) {
		auto &monster = Monsters[ActiveMonsters[i]];

		if (monster._mAi != AI_FALLEN)
			continue;
		if (position.WalkingDistance(monster.position.tile) >= 5)
			continue;
		if (monster._mhitpoints >> 6 <= 0)
			continue;

		int rundist;
		switch (monster.MType->mtype) {
		case MT_RFALLSP:
		case MT_RFALLSD:
			rundist = 7;
			break;
		case MT_DFALLSP:
		case MT_DFALLSD:
			rundist = 5;
			break;
		case MT_YFALLSP:
		case MT_YFALLSD:
			rundist = 3;
			break;
		case MT_BFALLSP:
		case MT_BFALLSD:
			rundist = 2;
			break;
		default:
			continue;
		}

		monster._mgoal = MGOAL_RETREAT;
		monster._mgoalvar1 = rundist;
		monster._mgoalvar2 = static_cast<int>(GetDirection(position, monster.position.tile));
	}
}

void PrintMonstHistory(int mt)
{
	if (*sgOptions.Gameplay.showMonsterType) {
		strcpy(tempstr, fmt::format(_("Type: {:s}  Kills: {:d}"), GetMonsterTypeText(MonstersData[mt]), MonsterKillCounts[mt]).c_str());
	} else {
		strcpy(tempstr, fmt::format(_("Total kills: {:d}"), MonsterKillCounts[mt]).c_str());
	}

	AddPanelString(tempstr);
	if (MonsterKillCounts[mt] >= 30) {
		int minHP = MonstersData[mt].mMinHP;
		int maxHP = MonstersData[mt].mMaxHP;
		if (!gbIsHellfire && mt == MT_DIABLO) {
			minHP -= 2000;
			maxHP -= 2000;
		}
		if (!gbIsMultiplayer) {
			minHP /= 2;
			maxHP /= 2;
		}
		if (minHP < 1)
			minHP = 1;
		if (maxHP < 1)
			maxHP = 1;

		int hpBonusNightmare = 1;
		int hpBonusHell = 3;
		if (gbIsHellfire) {
			hpBonusNightmare = (!gbIsMultiplayer ? 50 : 100);
			hpBonusHell = (!gbIsMultiplayer ? 100 : 200);
		}
		if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
			minHP = 3 * minHP + hpBonusNightmare;
			maxHP = 3 * maxHP + hpBonusNightmare;
		} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
			minHP = 4 * minHP + hpBonusHell;
			maxHP = 4 * maxHP + hpBonusHell;
		}
		strcpy(tempstr, fmt::format(_("Hit Points: {:d}-{:d}"), minHP, maxHP).c_str());
		AddPanelString(tempstr);
	}
	if (MonsterKillCounts[mt] >= 15) {
		int res = (sgGameInitInfo.nDifficulty != DIFF_HELL) ? MonstersData[mt].mMagicRes : MonstersData[mt].mMagicRes2;
		if ((res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING)) == 0) {
			strcpy(tempstr, _("No magic resistance"));
			AddPanelString(tempstr);
		} else {
			if ((res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING)) != 0) {
				strcpy(tempstr, _("Resists: "));
				if ((res & RESIST_MAGIC) != 0)
					strcat(tempstr, _("Magic "));
				if ((res & RESIST_FIRE) != 0)
					strcat(tempstr, _("Fire "));
				if ((res & RESIST_LIGHTNING) != 0)
					strcat(tempstr, _("Lightning "));
				string_view str { tempstr };
				str.remove_suffix(str.size() - FindLastUtf8Symbols(str));
				AddPanelString(str);
			}
			if ((res & (IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING)) != 0) {
				strcpy(tempstr, _("Immune: "));
				if ((res & IMMUNE_MAGIC) != 0)
					strcat(tempstr, _("Magic "));
				if ((res & IMMUNE_FIRE) != 0)
					strcat(tempstr, _("Fire "));
				if ((res & IMMUNE_LIGHTNING) != 0)
					strcat(tempstr, _("Lightning "));
				string_view str { tempstr };
				str.remove_suffix(str.size() - FindLastUtf8Symbols(str));
				AddPanelString(str);
			}
		}
	}
}

void PrintUniqueHistory()
{
	auto &monster = Monsters[pcursmonst];
	if (*sgOptions.Gameplay.showMonsterType) {
		strcpy(tempstr, fmt::format(_("Type: {:s}"), GetMonsterTypeText(*monster.MData)).c_str());
		AddPanelString(tempstr);
	}

	int res = monster.mMagicRes & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING);
	if (res == 0) {
		strcpy(tempstr, _("No resistances"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("No Immunities"));
	} else {
		if ((res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING)) != 0)
			strcpy(tempstr, _("Some Magic Resistances"));
		else
			strcpy(tempstr, _("No resistances"));
		AddPanelString(tempstr);
		if ((res & (IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING)) != 0) {
			strcpy(tempstr, _("Some Magic Immunities"));
		} else {
			strcpy(tempstr, _("No Immunities"));
		}
	}
	AddPanelString(tempstr);
}

void PlayEffect(Monster &monster, int mode)
{
	if (Players[MyPlayerId].pLvlLoad != 0) {
		return;
	}

	int sndIdx = GenerateRnd(2);
	if (!gbSndInited || !gbSoundOn || gbBufferMsgs != 0) {
		return;
	}

	int mi = monster._mMTidx;
	TSnd *snd = LevelMonsterTypes[mi].Snds[mode][sndIdx].get();
	if (snd == nullptr || snd->isPlaying()) {
		return;
	}

	int lVolume = 0;
	int lPan = 0;
	if (!CalculateSoundPosition(monster.position.tile, &lVolume, &lPan))
		return;

	snd_play_snd(snd, lVolume, lPan);
}

void MissToMonst(Missile &missile, Point position)
{
	int m = missile._misource;

	assert(m >= 0 && m < MAXMONSTERS);
	auto &monster = Monsters[m];

	Point oldPosition = missile.position.tile;
	dMonster[position.x][position.y] = m + 1;
	monster._mdir = static_cast<Direction>(missile._mimfnum);
	monster.position.tile = position;
	M_StartStand(monster, monster._mdir);
	if (monster.MType->mtype < MT_INCIN || monster.MType->mtype > MT_HELLBURN) {
		if ((monster._mFlags & MFLAG_TARGETS_MONSTER) == 0)
			M_StartHit(m, -1, 0);
		else
			MonsterHitMonster(m, -1, 0);
	} else {
		StartFadein(monster, monster._mdir, false);
	}

	if ((monster._mFlags & MFLAG_TARGETS_MONSTER) == 0) {
		int pnum = dPlayer[oldPosition.x][oldPosition.y] - 1;
		if (dPlayer[oldPosition.x][oldPosition.y] > 0) {
			if (monster.MType->mtype != MT_GLOOM && (monster.MType->mtype < MT_INCIN || monster.MType->mtype > MT_HELLBURN)) {
				MonsterAttackPlayer(m, dPlayer[oldPosition.x][oldPosition.y] - 1, 500, monster.mMinDamage2, monster.mMaxDamage2);
				if (pnum == dPlayer[oldPosition.x][oldPosition.y] - 1 && (monster.MType->mtype < MT_NSNAKE || monster.MType->mtype > MT_GSNAKE)) {
					auto &player = Players[pnum];
					if (player._pmode != PM_GOTHIT && player._pmode != PM_DEATH)
						StartPlrHit(pnum, 0, true);
					Point newPosition = oldPosition + monster._mdir;
					if (PosOkPlayer(player, newPosition)) {
						player.position.tile = newPosition;
						FixPlayerLocation(pnum, player._pdir);
						FixPlrWalkTags(pnum);
						dPlayer[newPosition.x][newPosition.y] = pnum + 1;
						SetPlayerOld(player);
					}
				}
			}
		}
		return;
	}

	if (dMonster[oldPosition.x][oldPosition.y] > 0) {
		if (monster.MType->mtype != MT_GLOOM && (monster.MType->mtype < MT_INCIN || monster.MType->mtype > MT_HELLBURN)) {
			MonsterAttackMonster(m, dMonster[oldPosition.x][oldPosition.y] - 1, 500, monster.mMinDamage2, monster.mMaxDamage2);
			if (monster.MType->mtype < MT_NSNAKE || monster.MType->mtype > MT_GSNAKE) {
				Point newPosition = oldPosition + monster._mdir;
				if (IsTileAvailable(Monsters[dMonster[oldPosition.x][oldPosition.y] - 1], newPosition)) {
					m = dMonster[oldPosition.x][oldPosition.y];
					dMonster[newPosition.x][newPosition.y] = m;
					dMonster[oldPosition.x][oldPosition.y] = 0;
					m--;
					monster.position.tile = newPosition;
					monster.position.future = newPosition;
				}
			}
		}
	}
}

bool IsTileAvailable(const Monster &monster, Point position)
{
	if (!IsTileAvailable(position))
		return false;

	return IsTileSafe(monster, position);
}

bool IsSkel(int mt)
{
	return (mt >= MT_WSKELAX && mt <= MT_XSKELAX)
	    || (mt >= MT_WSKELBW && mt <= MT_XSKELBW)
	    || (mt >= MT_WSKELSD && mt <= MT_XSKELSD);
}

bool IsGoat(int mt)
{
	return (mt >= MT_NGOATMC && mt <= MT_GGOATMC)
	    || (mt >= MT_NGOATBW && mt <= MT_GGOATBW);
}

bool SpawnSkeleton(int ii, Point position)
{
	if (ii == -1)
		return false;

	if (IsTileAvailable(position)) {
		Direction dir = GetDirection(position, position); // TODO useless calculation
		ActivateSpawn(ii, position, dir);
		return true;
	}

	bool monstok[3][3];

	bool savail = false;
	int yy = 0;
	for (int j = position.y - 1; j <= position.y + 1; j++) {
		int xx = 0;
		for (int k = position.x - 1; k <= position.x + 1; k++) {
			monstok[xx][yy] = IsTileAvailable({ k, j });
			savail = savail || monstok[xx][yy];
			xx++;
		}
		yy++;
	}
	if (!savail) {
		return false;
	}

	int rs = GenerateRnd(15) + 1;
	int x2 = 0;
	int y2 = 0;
	while (rs > 0) {
		if (monstok[x2][y2])
			rs--;
		if (rs > 0) {
			x2++;
			if (x2 == 3) {
				x2 = 0;
				y2++;
				if (y2 == 3)
					y2 = 0;
			}
		}
	}

	Point spawn = position + Displacement { x2 - 1, y2 - 1 };
	Direction dir = GetDirection(spawn, position);
	ActivateSpawn(ii, spawn, dir);

	return true;
}

int PreSpawnSkeleton()
{
	int skel = AddSkeleton({ 0, 0 }, Direction::South, false);
	if (skel != -1)
		M_StartStand(Monsters[skel], Direction::South);

	return skel;
}

void TalktoMonster(Monster &monster)
{
	auto &player = Players[monster._menemy];
	monster._mmode = MonsterMode::Talk;
	if (monster._mAi != AI_SNOTSPIL && monster._mAi != AI_LACHDAN) {
		return;
	}

	if (Quests[Q_LTBANNER].IsAvailable() && Quests[Q_LTBANNER]._qvar1 == 2) {
		if (player.TryRemoveInvItemById(IDI_BANNER)) {
			Quests[Q_LTBANNER]._qactive = QUEST_DONE;
			monster.mtalkmsg = TEXT_BANNER12;
			monster._mgoal = MGOAL_INQUIRING;
		}
	}
	if (Quests[Q_VEIL].IsAvailable() && monster.mtalkmsg >= TEXT_VEIL9) {
		if (player.TryRemoveInvItemById(IDI_GLDNELIX)) {
			monster.mtalkmsg = TEXT_VEIL11;
			monster._mgoal = MGOAL_INQUIRING;
		}
	}
}

void SpawnGolem(int i, Point position, Missile &missile)
{
	assert(i >= 0 && i < MAX_PLRS);
	auto &player = Players[i];
	auto &golem = Monsters[i];

	dMonster[position.x][position.y] = i + 1;
	golem.position.tile = position;
	golem.position.future = position;
	golem.position.old = position;
	golem._pathcount = 0;
	golem._mmaxhp = 2 * (320 * missile._mispllvl + player._pMaxMana / 3);
	golem._mhitpoints = golem._mmaxhp;
	golem.mArmorClass = 25;
	golem.mHit = 5 * (missile._mispllvl + 8) + 2 * player._pLevel;
	golem.mMinDamage = 2 * (missile._mispllvl + 4);
	golem.mMaxDamage = 2 * (missile._mispllvl + 8);
	golem._mFlags |= MFLAG_GOLEM;
	StartSpecialStand(golem, Direction::South);
	UpdateEnemy(golem);
	if (i == MyPlayerId) {
		NetSendCmdGolem(
		    golem.position.tile.x,
		    golem.position.tile.y,
		    golem._mdir,
		    golem._menemy,
		    golem._mhitpoints,
		    currlevel);
	}
}

bool CanTalkToMonst(const Monster &monster)
{
	return IsAnyOf(monster._mgoal, MGOAL_INQUIRING, MGOAL_TALKING);
}

bool CheckMonsterHit(Monster &monster, bool *ret)
{
	if (monster._mAi == AI_GARG && (monster._mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
		monster._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		monster._mmode = MonsterMode::SpecialMeleeAttack;
		*ret = true;
		return true;
	}

	if (monster.MType->mtype >= MT_COUNSLR && monster.MType->mtype <= MT_ADVOCATE) {
		if (monster._mgoal != MGOAL_NORMAL) {
			*ret = false;
			return true;
		}
	}

	return false;
}

int encode_enemy(Monster &monster)
{
	if ((monster._mFlags & MFLAG_TARGETS_MONSTER) != 0)
		return monster._menemy + MAX_PLRS;

	return monster._menemy;
}

void decode_enemy(Monster &monster, int enemyId)
{
	if (enemyId < MAX_PLRS) {
		monster._mFlags &= ~MFLAG_TARGETS_MONSTER;
		monster._menemy = enemyId;
		monster.enemyPosition = Players[enemyId].position.future;
	} else {
		monster._mFlags |= MFLAG_TARGETS_MONSTER;
		enemyId -= MAX_PLRS;
		monster._menemy = enemyId;
		monster.enemyPosition = Monsters[enemyId].position.future;
	}
}

void Monster::CheckStandAnimationIsLoaded(Direction mdir)
{
	if (IsAnyOf(_mmode, MonsterMode::Stand, MonsterMode::Talk)) {
		_mdir = mdir;
		ChangeAnimationData(MonsterGraphic::Stand);
	}
}

void Monster::Petrify()
{
	_mmode = MonsterMode::Petrified;
	AnimInfo.IsPetrified = true;
}

bool Monster::IsWalking() const
{
	switch (_mmode) {
	case MonsterMode::MoveNorthwards:
	case MonsterMode::MoveSouthwards:
	case MonsterMode::MoveSideways:
		return true;
	default:
		return false;
	}
}

} // namespace devilution
