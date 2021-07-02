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
#include "storm/storm.h"
#include "themes.h"
#include "towners.h"
#include "trigs.h"
#include "utils/language.h"

#ifdef _DEBUG
#include "debug.h"
#endif

namespace devilution {

#define NIGHTMARE_TO_HIT_BONUS 85
#define HELL_TO_HIT_BONUS 120

#define NIGHTMARE_AC_BONUS 50
#define HELL_AC_BONUS 80

/** Tracks which missile files are already loaded */
int MissileFileFlag;

// BUGFIX: replace monstkills[MAXMONSTERS] with monstkills[NUM_MTYPES].
/** Tracks the total number of monsters killed per monster_id. */
int monstkills[MAXMONSTERS];
int monstactive[MAXMONSTERS];
int nummonsters;
bool sgbSaveSoundOn;
MonsterStruct monster[MAXMONSTERS];
int totalmonsters;
CMonster Monsters[MAX_LVLMTYPES];
int monstimgtot;
int uniquetrans;
int nummtypes;

/* data */

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
/** Maps from direction to a left turn from the direction. */
Direction left[8] = { DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW, DIR_N, DIR_NE, DIR_E };
/** Maps from direction to a right turn from the direction. */
Direction right[8] = { DIR_SW, DIR_W, DIR_NW, DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S };
/** Maps from direction to the opposite direction. */
Direction opposite[8] = { DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW };

/** Maps from monster AI ID to monster AI function. */
void (*AiProc[])(int i) = {
	&MAI_Zombie,
	&MAI_Fat,
	&MAI_SkelSd,
	&MAI_SkelBow,
	&MAI_Scav,
	&MAI_Rhino,
	&MAI_GoatMc,
	&MAI_GoatBow,
	&MAI_Fallen,
	&MAI_Magma,
	&MAI_SkelKing,
	&MAI_Bat,
	&MAI_Garg,
	&MAI_Cleaver,
	&MAI_Succ,
	&MAI_Sneak,
	&MAI_Storm,
	&MAI_Fireman,
	&MAI_Garbud,
	&MAI_Acid,
	&MAI_AcidUniq,
	&MAI_Golum,
	&MAI_Zhar,
	&MAI_SnotSpil,
	&MAI_Snake,
	&MAI_Counselor,
	&MAI_Mega,
	&MAI_Diablo,
	&MAI_Lazurus,
	&MAI_Lazhelp,
	&MAI_Lachdanan,
	&MAI_Warlord,
	&MAI_Firebat,
	&MAI_Torchant,
	&MAI_HorkDemon,
	&MAI_Lich,
	&MAI_ArchLich,
	&MAI_Psychorb,
	&MAI_Necromorb,
	&MAI_BoneDemon
};

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

void InitLevelMonsters()
{
	int i;

	nummtypes = 0;
	monstimgtot = 0;
	MissileFileFlag = 0;

	for (i = 0; i < MAX_LVLMTYPES; i++) {
		Monsters[i].mPlaceFlags = 0;
	}

	ClrAllMonsters();
	nummonsters = 0;
	totalmonsters = MAXMONSTERS;

	for (i = 0; i < MAXMONSTERS; i++) {
		monstactive[i] = i;
	}

	uniquetrans = 0;
}

int AddMonsterType(_monster_id type, placeflag placeflag)
{
	bool done = false;
	int i;

	for (i = 0; i < nummtypes && !done; i++) {
		done = Monsters[i].mtype == type;
	}

	i--;

	if (!done) {
		i = nummtypes;
		nummtypes++;
		Monsters[i].mtype = type;
		monstimgtot += monsterdata[type].mImage;
		InitMonsterGFX(i);
		InitMonsterSND(i);
	}

	Monsters[i].mPlaceFlags |= placeflag;
	return i;
}

void GetLevelMTypes()
{
	int i;

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
		if (QuestStatus(Q_BUTCHER))
			AddMonsterType(MT_CLEAVER, PLACE_SPECIAL);
		if (QuestStatus(Q_GARBUD))
			AddMonsterType(UniqMonst[UMT_GARBUD].mtype, PLACE_UNIQUE);
		if (QuestStatus(Q_ZHAR))
			AddMonsterType(UniqMonst[UMT_ZHAR].mtype, PLACE_UNIQUE);
		if (QuestStatus(Q_LTBANNER))
			AddMonsterType(UniqMonst[UMT_SNOTSPIL].mtype, PLACE_UNIQUE);
		if (QuestStatus(Q_VEIL))
			AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, PLACE_UNIQUE);
		if (QuestStatus(Q_WARLORD))
			AddMonsterType(UniqMonst[UMT_WARLORD].mtype, PLACE_UNIQUE);

		if (gbIsMultiplayer && currlevel == quests[Q_SKELKING]._qlevel) {

			AddMonsterType(MT_SKING, PLACE_UNIQUE);

			nt = 0;
			for (i = MT_WSKELAX; i <= MT_WSKELAX + numskeltypes; i++) {
				if (IsSkel(i)) {
					minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
					maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

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
		for (i = MT_NZOMBIE; i < NUM_MTYPES; i++) {
			minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
			maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

			if (currlevel >= minl && currlevel <= maxl) {
				if ((MonstAvailTbl[i] & mamask) != 0) {
					typelist[nt++] = (_monster_id)i;
				}
			}
		}

#ifdef _DEBUG
		if (monstdebug) {
			for (i = 0; i < debugmonsttypes; i++)
				AddMonsterType(DebugMonsters[i], PLACE_SCATTER);
		} else
#endif
		{

			while (nt > 0 && nummtypes < MAX_LVLMTYPES && monstimgtot < 4000) {
				for (i = 0; i < nt;) {
					if (monsterdata[typelist[i]].mImage > 4000 - monstimgtot) {
						typelist[i] = typelist[--nt];
						continue;
					}

					i++;
				}

				if (nt != 0) {
					i = GenerateRnd(nt);
					AddMonsterType(typelist[i], PLACE_SCATTER);
					typelist[i] = typelist[--nt];
				}
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
	int mtype, anim, i;
	char strBuff[256];

	mtype = Monsters[monst].mtype;
	int width = monsterdata[mtype].width;

	for (anim = 0; anim < 6; anim++) {
		int frames = monsterdata[mtype].Frames[anim];
		if (gbIsHellfire && mtype == MT_DIABLO && anim == 3)
			frames = 2;

		if ((animletter[anim] != 's' || monsterdata[mtype].has_special) && frames > 0) {
			sprintf(strBuff, monsterdata[mtype].GraphicType, animletter[anim]);

			byte *celBuf;
			{
				auto celData = LoadFileInMem(strBuff);
				celBuf = celData.get();
				Monsters[monst].Anims[anim].CMem = std::move(celData);
			}

			if (Monsters[monst].mtype != MT_GOLEM || (animletter[anim] != 's' && animletter[anim] != 'd')) {

				for (i = 0; i < 8; i++) {
					byte *pCelStart = CelGetFrame(celBuf, i);
					Monsters[monst].Anims[anim].CelSpritesForDirections[i].emplace(pCelStart, width);
				}
			} else {
				for (i = 0; i < 8; i++) {
					Monsters[monst].Anims[anim].CelSpritesForDirections[i].emplace(celBuf, width);
				}
			}
		}

		Monsters[monst].Anims[anim].Frames = frames;
		Monsters[monst].Anims[anim].Rate = monsterdata[mtype].Rate[anim];
	}

	Monsters[monst].mMinHP = monsterdata[mtype].mMinHP;
	Monsters[monst].mMaxHP = monsterdata[mtype].mMaxHP;
	if (!gbIsHellfire && mtype == MT_DIABLO) {
		Monsters[monst].mMinHP -= 2000;
		Monsters[monst].mMaxHP -= 2000;
	}
	Monsters[monst].has_special = monsterdata[mtype].has_special;
	Monsters[monst].mAFNum = monsterdata[mtype].mAFNum;
	Monsters[monst].MData = &monsterdata[mtype];

	if (monsterdata[mtype].has_trans) {
		InitMonsterTRN(Monsters[monst]);
	}

	if (mtype >= MT_NMAGMA && mtype <= MT_WMAGMA && (MissileFileFlag & 1) == 0) {
		MissileFileFlag |= 1;
		LoadMissileGFX(MFILE_MAGBALL);
	}
	if (mtype >= MT_STORM && mtype <= MT_MAEL && (MissileFileFlag & 2) == 0) {
		MissileFileFlag |= 2;
		LoadMissileGFX(MFILE_THINLGHT);
	}
	if (mtype == MT_SUCCUBUS && (MissileFileFlag & 4) == 0) {
		MissileFileFlag |= 4;
		LoadMissileGFX(MFILE_FLARE);
		LoadMissileGFX(MFILE_FLAREEXP);
	}
	if (mtype >= MT_INCIN && mtype <= MT_HELLBURN && (MissileFileFlag & 8) == 0) {
		MissileFileFlag |= 8;
		LoadMissileGFX(MFILE_KRULL);
	}
	if (mtype == MT_SNOWWICH && (MissileFileFlag & 0x20) == 0) {
		MissileFileFlag |= 0x20;
		LoadMissileGFX(MFILE_SCUBMISB);
		LoadMissileGFX(MFILE_SCBSEXPB);
	}
	if (mtype == MT_HLSPWN && (MissileFileFlag & 0x40) == 0) {
		MissileFileFlag |= 0x40;
		LoadMissileGFX(MFILE_SCUBMISD);
		LoadMissileGFX(MFILE_SCBSEXPD);
	}
	if (mtype == MT_SOLBRNR && (MissileFileFlag & 0x80) == 0) {
		MissileFileFlag |= 0x80;
		LoadMissileGFX(MFILE_SCUBMISC);
		LoadMissileGFX(MFILE_SCBSEXPC);
	}
	if (mtype >= MT_INCIN && mtype <= MT_HELLBURN && (MissileFileFlag & 8) == 0) {
		MissileFileFlag |= 8;
		LoadMissileGFX(MFILE_KRULL);
	}
	if (((mtype >= MT_NACID && mtype <= MT_XACID) || mtype == MT_SPIDLORD) && (MissileFileFlag & 0x10) == 0) {
		MissileFileFlag |= 0x10;
		LoadMissileGFX(MFILE_ACIDBF);
		LoadMissileGFX(MFILE_ACIDSPLA);
		LoadMissileGFX(MFILE_ACIDPUD);
	}
	if (mtype == MT_LICH && (MissileFileFlag & 0x100) == 0) {
		MissileFileFlag |= 0x100;
		LoadMissileGFX(MFILE_LICH);
		LoadMissileGFX(MFILE_EXORA1);
	}
	if (mtype == MT_ARCHLICH && (MissileFileFlag & 0x200) == 0) {
		MissileFileFlag |= 0x200;
		LoadMissileGFX(MFILE_ARCHLICH);
		LoadMissileGFX(MFILE_EXYEL2);
	}
	if ((mtype == MT_PSYCHORB || mtype == MT_BONEDEMN) && (MissileFileFlag & 0x400) == 0) {
		MissileFileFlag |= 0x400;
		LoadMissileGFX(MFILE_BONEDEMON);
	}
	if (mtype == MT_NECRMORB && (MissileFileFlag & 0x800) == 0) {
		MissileFileFlag |= 0x800;
		LoadMissileGFX(MFILE_NECROMORB);
		LoadMissileGFX(MFILE_EXRED3);
	}
	if (mtype == MT_PSYCHORB && (MissileFileFlag & 0x1000) == 0) {
		MissileFileFlag |= 0x1000;
		LoadMissileGFX(MFILE_EXBL2);
	}
	if (mtype == MT_BONEDEMN && (MissileFileFlag & 0x2000) == 0) {
		MissileFileFlag |= 0x2000;
		LoadMissileGFX(MFILE_EXBL3);
	}
	if (mtype == MT_DIABLO) {
		LoadMissileGFX(MFILE_FIREPLAR);
	}
}

void ClearMVars(int i)
{
	monster[i]._mVar1 = 0;
	monster[i]._mVar2 = 0;
	monster[i]._mVar3 = 0;
	monster[i].position.temp = { 0, 0 };
	monster[i].position.offset2 = { 0, 0 };
}

void InitMonster(int i, Direction rd, int mtype, Point position)
{
	CMonster *monst = &Monsters[mtype];

	monster[i]._mdir = rd;
	monster[i].position.tile = position;
	monster[i].position.future = position;
	monster[i].position.old = position;
	monster[i]._mMTidx = mtype;
	monster[i]._mmode = MM_STAND;
	monster[i].mName = _(monst->MData->mName);
	monster[i].MType = monst;
	monster[i].MData = monst->MData;
	monster[i].AnimInfo = {};
	monster[i].AnimInfo.pCelSprite = monst->Anims[MA_STAND].CelSpritesForDirections[rd] ? &*monst->Anims[MA_STAND].CelSpritesForDirections[rd] : nullptr;
	monster[i].AnimInfo.TicksPerFrame = monst->Anims[MA_STAND].Rate;
	monster[i].AnimInfo.TickCounterOfCurrentFrame = GenerateRnd(monster[i].AnimInfo.TicksPerFrame - 1);
	monster[i].AnimInfo.NumberOfFrames = monst->Anims[MA_STAND].Frames;
	monster[i].AnimInfo.CurrentFrame = GenerateRnd(monster[i].AnimInfo.NumberOfFrames - 1) + 1;

	monster[i].mLevel = monst->MData->mLevel;
	monster[i]._mmaxhp = (monst->mMinHP + GenerateRnd(monst->mMaxHP - monst->mMinHP + 1)) << 6;
	if (monst->mtype == MT_DIABLO && !gbIsHellfire) {
		monster[i]._mmaxhp /= 2;
		monster[i].mLevel -= 15;
	}

	if (!gbIsMultiplayer) {
		monster[i]._mmaxhp /= 2;
		if (monster[i]._mmaxhp < 64) {
			monster[i]._mmaxhp = 64;
		}
	}

	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i]._mAi = monst->MData->mAi;
	monster[i]._mint = monst->MData->mInt;
	monster[i]._mgoal = MGOAL_NORMAL;
	monster[i]._mgoalvar1 = 0;
	monster[i]._mgoalvar2 = 0;
	monster[i]._mgoalvar3 = 0;
	monster[i]._pathcount = 0;
	monster[i]._mDelFlag = false;
	monster[i]._uniqtype = 0;
	monster[i]._msquelch = 0;
	monster[i].mlid = NO_LIGHT; // BUGFIX monsters initial light id should be -1 (fixed)
	monster[i]._mRndSeed = AdvanceRndSeed();
	monster[i]._mAISeed = AdvanceRndSeed();
	monster[i].mWhoHit = 0;
	monster[i].mExp = monst->MData->mExp;
	monster[i].mHit = monst->MData->mHit;
	monster[i].mMinDamage = monst->MData->mMinDamage;
	monster[i].mMaxDamage = monst->MData->mMaxDamage;
	monster[i].mHit2 = monst->MData->mHit2;
	monster[i].mMinDamage2 = monst->MData->mMinDamage2;
	monster[i].mMaxDamage2 = monst->MData->mMaxDamage2;
	monster[i].mArmorClass = monst->MData->mArmorClass;
	monster[i].mMagicRes = monst->MData->mMagicRes;
	monster[i].leader = 0;
	monster[i].leaderflag = 0;
	monster[i]._mFlags = monst->MData->mFlags;
	monster[i].mtalkmsg = TEXT_NONE;

	if (monster[i]._mAi == AI_GARG) {
		monster[i].AnimInfo.pCelSprite = &*monst->Anims[MA_SPECIAL].CelSpritesForDirections[rd];
		monster[i].AnimInfo.CurrentFrame = 1;
		monster[i]._mFlags |= MFLAG_ALLOW_SPECIAL;
		monster[i]._mmode = MM_SATTACK;
	}

	if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
		monster[i]._mmaxhp = 3 * monster[i]._mmaxhp;
		if (gbIsHellfire)
			monster[i]._mmaxhp += (gbIsMultiplayer ? 100 : 50) << 6;
		else
			monster[i]._mmaxhp += 64;
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mLevel += 15;
		monster[i].mExp = 2 * (monster[i].mExp + 1000);
		monster[i].mHit += NIGHTMARE_TO_HIT_BONUS;
		monster[i].mMinDamage = 2 * (monster[i].mMinDamage + 2);
		monster[i].mMaxDamage = 2 * (monster[i].mMaxDamage + 2);
		monster[i].mHit2 += NIGHTMARE_TO_HIT_BONUS;
		monster[i].mMinDamage2 = 2 * (monster[i].mMinDamage2 + 2);
		monster[i].mMaxDamage2 = 2 * (monster[i].mMaxDamage2 + 2);
		monster[i].mArmorClass += NIGHTMARE_AC_BONUS;
	} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
		monster[i]._mmaxhp = 4 * monster[i]._mmaxhp;
		if (gbIsHellfire)
			monster[i]._mmaxhp += (gbIsMultiplayer ? 200 : 100) << 6;
		else
			monster[i]._mmaxhp += 192;
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mLevel += 30;
		monster[i].mExp = 4 * (monster[i].mExp + 1000);
		monster[i].mHit += HELL_TO_HIT_BONUS;
		monster[i].mMinDamage = 4 * monster[i].mMinDamage + 6;
		monster[i].mMaxDamage = 4 * monster[i].mMaxDamage + 6;
		monster[i].mHit2 += HELL_TO_HIT_BONUS;
		monster[i].mMinDamage2 = 4 * monster[i].mMinDamage2 + 6;
		monster[i].mMaxDamage2 = 4 * monster[i].mMaxDamage2 + 6;
		monster[i].mArmorClass += HELL_AC_BONUS;
		monster[i].mMagicRes = monst->MData->mMagicRes2;
	}
}

void ClrAllMonsters()
{
	int i;
	MonsterStruct *Monst;

	for (i = 0; i < MAXMONSTERS; i++) {
		Monst = &monster[i];
		ClearMVars(i);
		Monst->mName = "Invalid Monster";
		Monst->_mgoal = MGOAL_NONE;
		Monst->_mmode = MM_STAND;
		Monst->_mVar1 = 0;
		Monst->_mVar2 = 0;
		Monst->position.tile = { 0, 0 };
		Monst->position.future = { 0, 0 };
		Monst->position.old = { 0, 0 };
		Monst->_mdir = static_cast<Direction>(GenerateRnd(8));
		Monst->position.velocity = { 0, 0 };
		Monst->AnimInfo = {};
		Monst->_mFlags = 0;
		Monst->_mDelFlag = false;
		Monst->_menemy = GenerateRnd(gbActivePlayers);
		Monst->enemyPosition = plr[Monst->_menemy].position.future;
	}
}

bool MonstPlace(int xp, int yp)
{
	char f;

	if (xp < 0 || xp >= MAXDUNX
	    || yp < 0 || yp >= MAXDUNY
	    || dMonster[xp][yp] != 0
	    || dPlayer[xp][yp] != 0) {
		return false;
	}

	f = dFlags[xp][yp];

	if ((f & BFLAG_VISIBLE) != 0) {
		return false;
	}

	if ((f & BFLAG_POPULATED) != 0) {
		return false;
	}

	return !SolidLoc({ xp, yp });
}

void monster_some_crypt()
{
	MonsterStruct *mon;
	int hp;

	if (currlevel == 24 && UberDiabloMonsterIndex >= 0 && UberDiabloMonsterIndex < nummonsters) {
		mon = &monster[UberDiabloMonsterIndex];
		PlayEffect(UberDiabloMonsterIndex, 2);
		quests[Q_NAKRUL]._qlog = false;
		mon->mArmorClass -= 50;
		hp = mon->_mmaxhp / 2;
		mon->mMagicRes = 0;
		mon->_mhitpoints = hp;
		mon->_mmaxhp = hp;
	}
}

void PlaceMonster(int i, int mtype, int x, int y)
{
	if (Monsters[mtype].mtype == MT_NAKRUL) {
		for (int j = 0; j < nummonsters; j++) {
			if (monster[j]._mMTidx == mtype) {
				return;
			}
			if (monster[j].MType->mtype == MT_NAKRUL) {
				return;
			}
		}
	}
	dMonster[x][y] = i + 1;

	auto rd = static_cast<Direction>(GenerateRnd(8));
	InitMonster(i, rd, mtype, { x, y });
}

void PlaceUniqueMonst(int uniqindex, int miniontype, int bosspacksize)
{
	int xp, yp, x, y, i;
	int uniqtype;
	int count2;
	char filestr[64];
	bool zharflag, done;
	const UniqMonstStruct *Uniq;
	MonsterStruct *Monst;
	int count;

	Monst = &monster[nummonsters];
	count = 0;
	Uniq = &UniqMonst[uniqindex];

	if ((uniquetrans + 19) * 256 >= LIGHTSIZE) {
		return;
	}

	for (uniqtype = 0; uniqtype < nummtypes; uniqtype++) {
		if (Monsters[uniqtype].mtype == UniqMonst[uniqindex].mtype) {
			break;
		}
	}

	while (true) {
		xp = GenerateRnd(80) + 16;
		yp = GenerateRnd(80) + 16;
		count2 = 0;
		for (x = xp - 3; x < xp + 3; x++) {
			for (y = yp - 3; y < yp + 3; y++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && MonstPlace(x, y)) {
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

		if (MonstPlace(xp, yp)) {
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
		zharflag = true;
		for (i = 0; i < themeCount; i++) {
			if (i == zharlib && zharflag) {
				zharflag = false;
				xp = 2 * themeLoc[i].x + 20;
				yp = 2 * themeLoc[i].y + 20;
			}
		}
	}
	if (!gbIsMultiplayer) {
		if (uniqindex == UMT_LAZURUS) {
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
		if (uniqindex == UMT_LAZURUS) {
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
		done = false;
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
		UberDiabloMonsterIndex = nummonsters;
	}
	PlaceMonster(nummonsters, uniqtype, xp, yp);
	Monst->_uniqtype = uniqindex + 1;

	if (Uniq->mlevel != 0) {
		Monst->mLevel = 2 * Uniq->mlevel;
	} else {
		Monst->mLevel += 5;
	}

	Monst->mExp *= 2;
	Monst->mName = _(Uniq->mName);
	Monst->_mmaxhp = Uniq->mmaxhp << 6;

	if (!gbIsMultiplayer) {
		Monst->_mmaxhp = Monst->_mmaxhp / 2;
		if (Monst->_mmaxhp < 64) {
			Monst->_mmaxhp = 64;
		}
	}

	Monst->_mhitpoints = Monst->_mmaxhp;
	Monst->_mAi = Uniq->mAi;
	Monst->_mint = Uniq->mint;
	Monst->mMinDamage = Uniq->mMinDamage;
	Monst->mMaxDamage = Uniq->mMaxDamage;
	Monst->mMinDamage2 = Uniq->mMinDamage;
	Monst->mMaxDamage2 = Uniq->mMaxDamage;
	Monst->mMagicRes = Uniq->mMagicRes;
	Monst->mtalkmsg = Uniq->mtalkmsg;
	if (uniqindex == UMT_HORKDMN)
		Monst->mlid = NO_LIGHT; // BUGFIX monsters initial light id should be -1 (fixed)
	else
		Monst->mlid = AddLight(Monst->position.tile, 3);

	if (gbIsMultiplayer) {
		if (Monst->_mAi == AI_LAZHELP)
			Monst->mtalkmsg = TEXT_NONE;
		if (Monst->_mAi == AI_LAZURUS && quests[Q_BETRAYER]._qvar1 > 3) {
			Monst->_mgoal = MGOAL_NORMAL;
		} else if (Monst->mtalkmsg != TEXT_NONE) {
			Monst->_mgoal = MGOAL_INQUIRING;
		}
	} else if (Monst->mtalkmsg != TEXT_NONE) {
		Monst->_mgoal = MGOAL_INQUIRING;
	}

	if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
		Monst->_mmaxhp = 3 * Monst->_mmaxhp;
		if (gbIsHellfire)
			Monst->_mmaxhp += (gbIsMultiplayer ? 100 : 50) << 6;
		else
			Monst->_mmaxhp += 64;
		Monst->mLevel += 15;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 2 * (Monst->mExp + 1000);
		Monst->mMinDamage = 2 * (Monst->mMinDamage + 2);
		Monst->mMaxDamage = 2 * (Monst->mMaxDamage + 2);
		Monst->mMinDamage2 = 2 * (Monst->mMinDamage2 + 2);
		Monst->mMaxDamage2 = 2 * (Monst->mMaxDamage2 + 2);
	} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
		Monst->_mmaxhp = 4 * Monst->_mmaxhp;
		if (gbIsHellfire)
			Monst->_mmaxhp += (gbIsMultiplayer ? 200 : 100) << 6;
		else
			Monst->_mmaxhp += 192;
		Monst->mLevel += 30;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 4 * (Monst->mExp + 1000);
		Monst->mMinDamage = 4 * Monst->mMinDamage + 6;
		Monst->mMaxDamage = 4 * Monst->mMaxDamage + 6;
		Monst->mMinDamage2 = 4 * Monst->mMinDamage2 + 6;
		Monst->mMaxDamage2 = 4 * Monst->mMaxDamage2 + 6;
	}

	sprintf(filestr, "Monsters\\Monsters\\%s.TRN", Uniq->mTrnName);
	LoadFileInMem(filestr, &pLightTbl[256 * (uniquetrans + 19)], 256);

	Monst->_uniqtrans = uniquetrans++;

	if ((Uniq->mUnqAttr & 4) != 0) {
		Monst->mHit = Uniq->mUnqVar1;
		Monst->mHit2 = Uniq->mUnqVar1;

		if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
			Monst->mHit += NIGHTMARE_TO_HIT_BONUS;
			Monst->mHit2 += NIGHTMARE_TO_HIT_BONUS;
		} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
			Monst->mHit += HELL_TO_HIT_BONUS;
			Monst->mHit2 += HELL_TO_HIT_BONUS;
		}
	}
	if ((Uniq->mUnqAttr & 8) != 0) {
		Monst->mArmorClass = Uniq->mUnqVar1;

		if (sgGameInitInfo.nDifficulty == DIFF_NIGHTMARE) {
			Monst->mArmorClass += NIGHTMARE_AC_BONUS;
		} else if (sgGameInitInfo.nDifficulty == DIFF_HELL) {
			Monst->mArmorClass += HELL_AC_BONUS;
		}
	}

	nummonsters++;

	if ((Uniq->mUnqAttr & 1) != 0) {
		PlaceGroup(miniontype, bosspacksize, Uniq->mUnqAttr, nummonsters - 1);
	}

	if (Monst->_mAi != AI_GARG) {
		Monst->AnimInfo.pCelSprite = &*Monst->MType->Anims[MA_STAND].CelSpritesForDirections[Monst->_mdir];
		Monst->AnimInfo.CurrentFrame = GenerateRnd(Monst->AnimInfo.NumberOfFrames - 1) + 1;
		Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		Monst->_mmode = MM_STAND;
	}
}

static void PlaceUniques()
{
	int u, mt;
	bool done;

	for (u = 0; UniqMonst[u].mtype != -1; u++) {
		if (UniqMonst[u].mlevel != currlevel)
			continue;
		done = false;
		for (mt = 0; mt < nummtypes; mt++) {
			if (done)
				break;
			done = (Monsters[mt].mtype == UniqMonst[u].mtype);
		}
		mt--;
		if (u == UMT_GARBUD && quests[Q_GARBUD]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UMT_ZHAR && quests[Q_ZHAR]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UMT_SNOTSPIL && quests[Q_LTBANNER]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UMT_LACHDAN && quests[Q_VEIL]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (u == UMT_WARLORD && quests[Q_WARLORD]._qactive == QUEST_NOTAVAIL)
			done = false;
		if (done)
			PlaceUniqueMonst(u, mt, 8);
	}
}

void PlaceQuestMonsters()
{
	int skeltype;

	if (!setlevel) {
		if (QuestStatus(Q_BUTCHER)) {
			PlaceUniqueMonst(UMT_BUTCHER, 0, 0);
		}

		if (currlevel == quests[Q_SKELKING]._qlevel && gbIsMultiplayer) {
			skeltype = 0;

			for (skeltype = 0; skeltype < nummtypes; skeltype++) {
				if (IsSkel(Monsters[skeltype].mtype)) {
					break;
				}
			}

			PlaceUniqueMonst(UMT_SKELKING, skeltype, 30);
		}

		if (QuestStatus(Q_LTBANNER)) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L1Data\\Banner1.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
		}
		if (QuestStatus(Q_BLOOD)) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blood2.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
		}
		if (QuestStatus(Q_BLIND)) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L2Data\\Blind2.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
		}
		if (QuestStatus(Q_ANVIL)) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L3Data\\Anvil.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x + 2, setpc_y + 2 } * 2);
		}
		if (QuestStatus(Q_WARLORD)) {
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\Warlord.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
			AddMonsterType(UniqMonst[UMT_WARLORD].mtype, PLACE_SCATTER);
		}
		if (QuestStatus(Q_VEIL)) {
			AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, PLACE_SCATTER);
		}
		if (QuestStatus(Q_ZHAR) && zharlib == -1) {
			quests[Q_ZHAR]._qactive = QUEST_NOTAVAIL;
		}

		if (currlevel == quests[Q_BETRAYER]._qlevel && gbIsMultiplayer) {
			AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, PLACE_UNIQUE);
			AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, PLACE_UNIQUE);
			PlaceUniqueMonst(UMT_LAZURUS, 0, 0);
			PlaceUniqueMonst(UMT_RED_VEX, 0, 0);
			PlaceUniqueMonst(UMT_BLACKJADE, 0, 0);
			auto dunData = LoadFileInMem<uint16_t>("Levels\\L4Data\\Vile1.DUN");
			SetMapMonsters(dunData.get(), Point { setpc_x, setpc_y } * 2);
		}

		if (currlevel == 24) {
			UberDiabloMonsterIndex = -1;
			int i1;
			for (i1 = 0; i1 < nummtypes; i1++) {
				if (Monsters[i1].mtype == UniqMonst[UMT_NAKRUL].mtype)
					break;
			}

			if (i1 < nummtypes) {
				for (int i2 = 0; i2 < nummonsters; i2++) {
					if (monster[i2]._uniqtype != 0 || monster[i2]._mMTidx == i1) {
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

void PlaceGroup(int mtype, int num, int leaderf, int leader)
{
	int try1, try2, j;
	int xp, yp, x1, y1;

	int placed = 0;

	for (try1 = 0; try1 < 10; try1++) {
		while (placed != 0) {
			nummonsters--;
			placed--;
			dMonster[monster[nummonsters].position.tile.x][monster[nummonsters].position.tile.y] = 0;
		}

		if ((leaderf & 1) != 0) {
			int offset = GenerateRnd(8);
			auto position = monster[leader].position.tile + static_cast<Direction>(offset);
			x1 = xp = position.x;
			y1 = yp = position.y;
		} else {
			do {
				x1 = xp = GenerateRnd(80) + 16;
				y1 = yp = GenerateRnd(80) + 16;
			} while (!MonstPlace(xp, yp));
		}

		if (num + nummonsters > totalmonsters) {
			num = totalmonsters - nummonsters;
		}

		j = 0;
		for (try2 = 0; j < num && try2 < 100; xp += Displacement::fromDirection(static_cast<Direction>(GenerateRnd(8))).deltaX, yp += Displacement::fromDirection(static_cast<Direction>(GenerateRnd(8))).deltaX) { /// BUGFIX: `yp += Point.y`
			if (!MonstPlace(xp, yp)
			    || (dTransVal[xp][yp] != dTransVal[x1][y1])
			    || ((leaderf & 2) != 0 && (abs(xp - x1) >= 4 || abs(yp - y1) >= 4))) {
				try2++;
				continue;
			}

			PlaceMonster(nummonsters, mtype, xp, yp);
			if ((leaderf & 1) != 0) {
				monster[nummonsters]._mmaxhp *= 2;
				monster[nummonsters]._mhitpoints = monster[nummonsters]._mmaxhp;
				monster[nummonsters]._mint = monster[leader]._mint;

				if ((leaderf & 2) != 0) {
					monster[nummonsters].leader = leader;
					monster[nummonsters].leaderflag = 1;
					monster[nummonsters]._mAi = monster[leader]._mAi;
				}

				if (monster[nummonsters]._mAi != AI_GARG) {
					monster[nummonsters].AnimInfo.pCelSprite = &*monster[nummonsters].MType->Anims[MA_STAND].CelSpritesForDirections[monster[nummonsters]._mdir];
					monster[nummonsters].AnimInfo.CurrentFrame = GenerateRnd(monster[nummonsters].AnimInfo.NumberOfFrames - 1) + 1;
					monster[nummonsters]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
					monster[nummonsters]._mmode = MM_STAND;
				}
			}
			nummonsters++;
			placed++;
			j++;
		}

		if (placed >= num) {
			break;
		}
	}

	if ((leaderf & 2) != 0) {
		monster[leader].packsize = placed;
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

void InitMonsters()
{
	int na, nt;
	int i, s, t;
	int numplacemonsters;
	int mtype;
	int numscattypes;
	int scattertypes[NUM_MTYPES];

	numscattypes = 0;
#ifdef _DEBUG
	if (gbIsMultiplayer)
		CheckDungeonClear();
#endif
	if (!setlevel) {
		AddMonster({ 1, 0 }, DIR_S, 0, false);
		AddMonster({ 1, 0 }, DIR_S, 0, false);
		AddMonster({ 1, 0 }, DIR_S, 0, false);
		AddMonster({ 1, 0 }, DIR_S, 0, false);
	}

	if (!gbIsSpawn && !setlevel && currlevel == 16)
		LoadDiabMonsts();

	nt = numtrigs;
	if (currlevel == 15)
		nt = 1;
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoVision(trigs[i].position + Displacement { s, t }, 15, false, false);
		}
	}
	if (!gbIsSpawn)
		PlaceQuestMonsters();
	if (!setlevel) {
		if (!gbIsSpawn)
			PlaceUniques();
		na = 0;
		for (s = 16; s < 96; s++) {
			for (t = 16; t < 96; t++) {
				if (!SolidLoc({ s, t }))
					na++;
			}
		}
		numplacemonsters = na / 30;
		if (gbIsMultiplayer)
			numplacemonsters += numplacemonsters / 2;
		if (nummonsters + numplacemonsters > MAXMONSTERS - 10)
			numplacemonsters = MAXMONSTERS - 10 - nummonsters;
		totalmonsters = nummonsters + numplacemonsters;
		for (i = 0; i < nummtypes; i++) {
			if ((Monsters[i].mPlaceFlags & PLACE_SCATTER) != 0) {
				scattertypes[numscattypes] = i;
				numscattypes++;
			}
		}
		while (nummonsters < totalmonsters) {
			mtype = scattertypes[GenerateRnd(numscattypes)];
			if (currlevel == 1 || GenerateRnd(2) == 0)
				na = 1;
			else if (currlevel == 2 || (currlevel >= 21 && currlevel <= 24))
				na = GenerateRnd(2) + 2;
			else
				na = GenerateRnd(3) + 3;
			PlaceGroup(mtype, na, 0, 0);
		}
	}
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoUnVision(trigs[i].position + Displacement { s, t }, 15);
		}
	}
}

void SetMapMonsters(const uint16_t *dunData, Point startPosition)
{
	AddMonsterType(MT_GOLEM, PLACE_SPECIAL);
	AddMonster({ 1, 0 }, DIR_S, 0, false);
	AddMonster({ 1, 0 }, DIR_S, 0, false);
	AddMonster({ 1, 0 }, DIR_S, 0, false);
	AddMonster({ 1, 0 }, DIR_S, 0, false);
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, PLACE_UNIQUE);
		AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, PLACE_UNIQUE);
		AddMonsterType(UniqMonst[UMT_BLACKJADE].mtype, PLACE_UNIQUE);
		PlaceUniqueMonst(UMT_LAZURUS, 0, 0);
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
				PlaceMonster(nummonsters++, mtype, i + startPosition.x + 16, j + startPosition.y + 16);
			}
		}
	}
}

void DeleteMonster(int i)
{
	int temp;

	nummonsters--;
	temp = monstactive[nummonsters];
	monstactive[nummonsters] = monstactive[i];
	monstactive[i] = temp;
}

int AddMonster(Point position, Direction dir, int mtype, bool InMap)
{
	if (nummonsters < MAXMONSTERS) {
		int i = monstactive[nummonsters++];
		if (InMap)
			dMonster[position.x][position.y] = i + 1;
		InitMonster(i, dir, mtype, position);
		return i;
	}

	return -1;
}

void AddDoppelganger(MonsterStruct &monster)
{
	if (monster.MType == nullptr) {
		return;
	}

	Point target = { 0, 0 };
	for (int d = 0; d < 8; d++) {
		const Point position = monster.position.tile + static_cast<Direction>(d);
		if (!SolidLoc(position)) {
			if (dPlayer[position.x][position.y] == 0 && dMonster[position.x][position.y] == 0) {
				if (dObject[position.x][position.y] == 0) {
					target = position;
					break;
				}
				int oi = dObject[position.x][position.y] > 0 ? dObject[position.x][position.y] - 1 : -(dObject[position.x][position.y] + 1);
				if (!object[oi]._oSolidFlag) {
					target = position;
					break;
				}
			}
		}
	}
	if (target != Point { 0, 0 }) {
		for (int j = 0; j < MAX_LVLMTYPES; j++) {
			if (Monsters[j].mtype == monster.MType->mtype) {
				AddMonster(target, monster._mdir, j, true);
				break;
			}
		}
	}
}

void NewMonsterAnim(int i, AnimStruct *anim, Direction md, AnimationDistributionFlags flags = AnimationDistributionFlags::None, int numSkippedFrames = 0, int distributeFramesBeforeFrame = 0)
{
	MonsterStruct *Monst = &monster[i];
	auto *pCelSprite = &*anim->CelSpritesForDirections[md];
	Monst->AnimInfo.SetNewAnimation(pCelSprite, anim->Frames, anim->Rate, flags, numSkippedFrames, distributeFramesBeforeFrame);
	Monst->_mFlags &= ~(MFLAG_LOCK_ANIMATION | MFLAG_ALLOW_SPECIAL);
	Monst->_mdir = md;
}

bool M_Ranged(int i)
{
	return IsAnyOf(monster[i]._mAi, AI_SKELBOW, AI_GOATBOW, AI_SUCC, AI_LAZHELP);
}

bool M_Talker(int i)
{
	return IsAnyOf(monster[i]._mAi, AI_LAZURUS, AI_WARLORD, AI_GARBUD, AI_ZHAR, AI_SNOTSPIL, AI_LACHDAN, AI_LAZHELP);
}

void M_Enemy(int i)
{
	int j;
	int mi, pnum;
	int dist, best_dist;
	int _menemy;
	bool sameroom, bestsameroom;
	MonsterStruct *Monst;
	BYTE enemyx, enemyy;

	_menemy = -1;
	best_dist = -1;
	bestsameroom = false;
	Monst = &monster[i];
	if ((Monst->_mFlags & MFLAG_BERSERK) != 0 || (Monst->_mFlags & MFLAG_GOLEM) == 0) {
		for (pnum = 0; pnum < MAX_PLRS; pnum++) {
			if (!plr[pnum].plractive || currlevel != plr[pnum].plrlevel || plr[pnum]._pLvlChanging
			    || (((plr[pnum]._pHitPoints >> 6) == 0) && gbIsMultiplayer))
				continue;
			sameroom = (dTransVal[Monst->position.tile.x][Monst->position.tile.y] == dTransVal[plr[pnum].position.tile.x][plr[pnum].position.tile.y]);
			dist = Monst->position.tile.WalkingDistance(plr[pnum].position.tile);
			if ((sameroom && !bestsameroom)
			    || ((sameroom || !bestsameroom) && dist < best_dist)
			    || (_menemy == -1)) {
				Monst->_mFlags &= ~MFLAG_TARGETS_MONSTER;
				_menemy = pnum;
				enemyx = plr[pnum].position.future.x;
				enemyy = plr[pnum].position.future.y;
				best_dist = dist;
				bestsameroom = sameroom;
			}
		}
	}
	for (j = 0; j < nummonsters; j++) {
		mi = monstactive[j];
		if (mi == i)
			continue;
		if (!((monster[mi]._mhitpoints >> 6) > 0))
			continue;
		if (monster[mi].position.tile.x == 1 && monster[mi].position.tile.y == 0)
			continue;
		if (M_Talker(mi) && monster[mi].mtalkmsg != TEXT_NONE)
			continue;
		if ((Monst->_mFlags & MFLAG_GOLEM) != 0 && (monster[mi]._mFlags & MFLAG_GOLEM) != 0) // prevent golems from fighting each other
			continue;

		dist = monster[mi].position.tile.WalkingDistance(Monst->position.tile);
		if (((Monst->_mFlags & MFLAG_GOLEM) == 0
		        && (Monst->_mFlags & MFLAG_BERSERK) == 0
		        && dist >= 2
		        && !M_Ranged(i))
		    || ((Monst->_mFlags & MFLAG_GOLEM) == 0
		        && (Monst->_mFlags & MFLAG_BERSERK) == 0
		        && (monster[mi]._mFlags & MFLAG_GOLEM) == 0)) {
			continue;
		}
		sameroom = dTransVal[Monst->position.tile.x][Monst->position.tile.y] == dTransVal[monster[mi].position.tile.x][monster[mi].position.tile.y];
		if ((sameroom && !bestsameroom)
		    || ((sameroom || !bestsameroom) && dist < best_dist)
		    || (_menemy == -1)) {
			Monst->_mFlags |= MFLAG_TARGETS_MONSTER;
			_menemy = mi;
			enemyx = monster[mi].position.future.x;
			enemyy = monster[mi].position.future.y;
			best_dist = dist;
			bestsameroom = sameroom;
		}
	}
	if (_menemy != -1) {
		Monst->_mFlags &= ~MFLAG_NO_ENEMY;
		Monst->_menemy = _menemy;
		Monst->enemyPosition = { enemyx, enemyy };
	} else {
		Monst->_mFlags |= MFLAG_NO_ENEMY;
	}
}

Direction M_GetDir(int i)
{
	return GetDirection(monster[i].position.tile, monster[i].enemyPosition);
}

void M_StartStand(int i, Direction md)
{
	ClearMVars(i);
	if (monster[i].MType->mtype == MT_GOLEM)
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], md);
	else
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_STAND], md);
	monster[i]._mVar1 = monster[i]._mmode;
	monster[i]._mVar2 = 0;
	monster[i]._mmode = MM_STAND;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
	monster[i]._mdir = md;
	M_Enemy(i);
}

void M_StartDelay(int i, int len)
{
	if (len <= 0) {
		return;
	}

	if (monster[i]._mAi != AI_LAZURUS) {
		monster[i]._mVar2 = len;
		monster[i]._mmode = MM_DELAY;
	}
}

void M_StartSpStand(int i, Direction md)
{
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_SPSTAND;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
	monster[i]._mdir = md;
}

void M_StartWalk(int i, int xvel, int yvel, int xadd, int yadd, Direction EndDir)
{
	int fx = xadd + monster[i].position.tile.x;
	int fy = yadd + monster[i].position.tile.y;

	dMonster[fx][fy] = -(i + 1);
	monster[i]._mmode = MM_WALK;
	monster[i].position.old = monster[i].position.tile;
	monster[i].position.future = { fx, fy };
	monster[i].position.velocity = { xvel, yvel };
	monster[i]._mVar1 = xadd;
	monster[i]._mVar2 = yadd;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir, AnimationDistributionFlags::ProcessAnimationPending, -1);
	monster[i].position.offset2 = { 0, 0 };
}

void M_StartWalk2(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, Direction EndDir)
{
	int fx = xadd + monster[i].position.tile.x;
	int fy = yadd + monster[i].position.tile.y;

	dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = -(i + 1);
	monster[i]._mVar1 = monster[i].position.tile.x;
	monster[i]._mVar2 = monster[i].position.tile.y;
	monster[i].position.old = monster[i].position.tile;
	monster[i].position.tile = { fx, fy };
	monster[i].position.future = { fx, fy };
	dMonster[fx][fy] = i + 1;
	if (monster[i].mlid != NO_LIGHT)
		ChangeLightXY(monster[i].mlid, monster[i].position.tile);
	monster[i].position.offset = { xoff, yoff };
	monster[i]._mmode = MM_WALK2;
	monster[i].position.velocity = { xvel, yvel };
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir, AnimationDistributionFlags::ProcessAnimationPending, -1);
	monster[i].position.offset2 = { 16 * xoff, 16 * yoff };
}

void M_StartWalk3(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, Direction EndDir)
{
	int fx = xadd + monster[i].position.tile.x;
	int fy = yadd + monster[i].position.tile.y;
	int x = mapx + monster[i].position.tile.x;
	int y = mapy + monster[i].position.tile.y;

	if (monster[i].mlid != NO_LIGHT)
		ChangeLightXY(monster[i].mlid, { x, y });

	dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = -(i + 1);
	dMonster[fx][fy] = -(i + 1);
	monster[i].position.temp = { x, y };
	dFlags[x][y] |= BFLAG_MONSTLR;
	monster[i].position.old = monster[i].position.tile;
	monster[i].position.future = { fx, fy };
	monster[i].position.offset = { xoff, yoff };
	monster[i]._mmode = MM_WALK3;
	monster[i].position.velocity = { xvel, yvel };
	monster[i]._mVar1 = fx;
	monster[i]._mVar2 = fy;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir, AnimationDistributionFlags::ProcessAnimationPending, -1);
	monster[i].position.offset2 = { 16 * xoff, 16 * yoff };
}

void M_StartAttack(int i)
{
	Direction md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_ATTACK], md, AnimationDistributionFlags::ProcessAnimationPending);
	monster[i]._mmode = MM_ATTACK;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
	monster[i]._mdir = md;
}

void M_StartRAttack(int i, missile_id missile_type, int dam)
{
	Direction md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_ATTACK], md, AnimationDistributionFlags::ProcessAnimationPending);
	monster[i]._mmode = MM_RATTACK;
	monster[i]._mVar1 = missile_type;
	monster[i]._mVar2 = dam;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
	monster[i]._mdir = md;
}

void M_StartRSpAttack(int i, missile_id missile_type, int dam)
{
	Direction md = M_GetDir(i);
	int distributeFramesBeforeFrame = 0;
	if (monster[i]._mAi == AI_MEGA)
		distributeFramesBeforeFrame = monster[i].MData->mAFNum2;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md, AnimationDistributionFlags::ProcessAnimationPending, 0, distributeFramesBeforeFrame);
	monster[i]._mmode = MM_RSPATTACK;
	monster[i]._mVar1 = missile_type;
	monster[i]._mVar2 = 0;
	monster[i]._mVar3 = dam;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
	monster[i]._mdir = md;
}

void M_StartSpAttack(int i)
{
	Direction md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_SATTACK;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
	monster[i]._mdir = md;
}

void M_StartEat(int i)
{
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], monster[i]._mdir);
	monster[i]._mmode = MM_SATTACK;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
}

void M_ClearSquares(int i)
{
	int x, y, mx, my, m1, m2;

	mx = monster[i].position.old.x;
	my = monster[i].position.old.y;
	m1 = -(i + 1);
	m2 = i + 1;

	for (y = my - 1; y <= my + 1; y++) {
		if (y >= 0 && y < MAXDUNY) {
			for (x = mx - 1; x <= mx + 1; x++) {
				if (x >= 0 && x < MAXDUNX && (dMonster[x][y] == m1 || dMonster[x][y] == m2))
					dMonster[x][y] = 0;
			}
		}
	}

	if (mx + 1 < MAXDUNX)
		dFlags[mx + 1][my] &= ~BFLAG_MONSTLR;
	if (my + 1 < MAXDUNY)
		dFlags[mx][my + 1] &= ~BFLAG_MONSTLR;
}

void M_GetKnockback(int i)
{
	Direction d = opposite[monster[i]._mdir];
	if (DirOK(i, d)) {
		M_ClearSquares(i);
		monster[i].position.old += d;
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_GOTHIT], monster[i]._mdir, gGameLogicStep < GameLogicStep::ProcessMonsters ? AnimationDistributionFlags::ProcessAnimationPending : AnimationDistributionFlags::None);
		monster[i]._mmode = MM_GOTHIT;
		monster[i].position.offset = { 0, 0 };
		monster[i].position.tile = monster[i].position.old;
		monster[i].position.future = monster[i].position.tile;
		M_ClearSquares(i);
		dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = i + 1;
	}
}

void M_StartHit(int i, int pnum, int dam)
{
	if (pnum >= 0)
		monster[i].mWhoHit |= 1 << pnum;
	if (pnum == myplr) {
		delta_monster_hp(i, monster[i]._mhitpoints, currlevel);
		NetSendCmdMonDmg(false, i, dam);
	}
	PlayEffect(i, 1);
	if ((monster[i].MType->mtype >= MT_SNEAK && monster[i].MType->mtype <= MT_ILLWEAV) || dam >> 6 >= monster[i].mLevel + 3) {
		if (pnum >= 0) {
			monster[i]._menemy = pnum;
			monster[i].enemyPosition = plr[pnum].position.future;
			monster[i]._mFlags &= ~MFLAG_TARGETS_MONSTER;
			monster[i]._mdir = M_GetDir(i);
		}
		if (monster[i].MType->mtype == MT_BLINK) {
			M_Teleport(i);
		} else if ((monster[i].MType->mtype >= MT_NSCAV && monster[i].MType->mtype <= MT_YSCAV)
		    || monster[i].MType->mtype == MT_GRAVEDIG) {
			monster[i]._mgoal = MGOAL_NORMAL;
			monster[i]._mgoalvar1 = 0;
			monster[i]._mgoalvar2 = 0;
		}
		if (monster[i]._mmode != MM_STONE) {
			NewMonsterAnim(i, &monster[i].MType->Anims[MA_GOTHIT], monster[i]._mdir, gGameLogicStep < GameLogicStep::ProcessMonsters ? AnimationDistributionFlags::ProcessAnimationPending : AnimationDistributionFlags::None);
			monster[i]._mmode = MM_GOTHIT;
			monster[i].position.offset = { 0, 0 };
			monster[i].position.tile = monster[i].position.old;
			monster[i].position.future = monster[i].position.old;
			M_ClearSquares(i);
			dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = i + 1;
		}
	}
}

void M_DiabloDeath(int i, bool sendmsg)
{
	MonsterStruct *Monst;
	int dist;
	int j, k;

	Monst = &monster[i];
	PlaySFX(USFX_DIABLOD);
	quests[Q_DIABLO]._qactive = QUEST_DONE;
	if (sendmsg)
		NetSendCmdQuest(true, Q_DIABLO);
	sgbSaveSoundOn = gbSoundOn;
	gbProcessPlayers = false;
	for (j = 0; j < nummonsters; j++) {
		k = monstactive[j];
		if (k == i || Monst->_msquelch == 0)
			continue;

		NewMonsterAnim(k, &monster[k].MType->Anims[MA_DEATH], monster[k]._mdir);
		monster[k]._mmode = MM_DEATH;
		monster[k].position.offset = { 0, 0 };
		monster[k]._mVar1 = 0;
		monster[k].position.tile = monster[k].position.old;
		monster[k].position.future = monster[k].position.tile;
		M_ClearSquares(k);
		dMonster[monster[k].position.tile.x][monster[k].position.tile.y] = k + 1;
	}
	AddLight(Monst->position.tile, 8);
	DoVision(Monst->position.tile, 8, false, true);
	dist = Monst->position.tile.WalkingDistance({ ViewX, ViewY });
	if (dist > 20)
		dist = 20;
	Monst->_mVar3 = ViewX << 16;
	Monst->position.temp.x = ViewY << 16;
	Monst->position.temp.y = (int)((Monst->_mVar3 - (Monst->position.tile.x << 16)) / (double)dist);
	Monst->position.offset2.deltaX = (int)((Monst->position.temp.x - (Monst->position.tile.y << 16)) / (double)dist);
}

void SpawnLoot(int i, bool sendmsg)
{
	int nSFX;
	MonsterStruct *Monst;

	Monst = &monster[i];
	if (QuestStatus(Q_GARBUD) && Monst->_uniqtype - 1 == UMT_GARBUD) {
		CreateTypeItem(Monst->position.tile + Displacement { 1, 1 }, true, ITYPE_MACE, IMISC_NONE, true, false);
	} else if (Monst->_uniqtype - 1 == UMT_DEFILER) {
		if (effect_is_playing(USFX_DEFILER8))
			stream_stop();
		quests[Q_DEFILER]._qlog = false;
		SpawnMapOfDoom(Monst->position.tile);
	} else if (Monst->_uniqtype - 1 == UMT_HORKDMN) {
		if (sgGameInitInfo.bTheoQuest != 0) {
			SpawnTheodore(Monst->position.tile);
		} else {
			CreateAmulet(Monst->position.tile, 13, false, true);
		}
	} else if (Monst->MType->mtype == MT_HORKSPWN) {
	} else if (Monst->MType->mtype == MT_NAKRUL) {
		nSFX = IsUberRoomOpened ? USFX_NAKRUL4 : USFX_NAKRUL5;
		if (sgGameInitInfo.bCowQuest != 0)
			nSFX = USFX_NAKRUL6;
		if (effect_is_playing(nSFX))
			stream_stop();
		quests[Q_NAKRUL]._qlog = false;
		UberDiabloMonsterIndex = -2;
		CreateMagicWeapon(Monst->position.tile, ITYPE_SWORD, ICURS_GREAT_SWORD, false, true);
		CreateMagicWeapon(Monst->position.tile, ITYPE_STAFF, ICURS_WAR_STAFF, false, true);
		CreateMagicWeapon(Monst->position.tile, ITYPE_BOW, ICURS_LONG_WAR_BOW, false, true);
		CreateSpellBook(Monst->position.tile, SPL_APOCA, false, true);
	} else if (i > MAX_PLRS - 1) { // Golems should not spawn loot
		SpawnItem(i, Monst->position.tile, sendmsg);
	}
}

void M2MStartHit(int mid, int i, int dam)
{
	assurance((DWORD)mid < MAXMONSTERS, mid);
	assurance(monster[mid].MType != nullptr, mid);

	if (i >= 0 && i < MAX_PLRS)
		monster[mid].mWhoHit |= 1 << i;

	delta_monster_hp(mid, monster[mid]._mhitpoints, currlevel);
	NetSendCmdMonDmg(false, mid, dam);
	PlayEffect(mid, 1);

	if ((monster[mid].MType->mtype >= MT_SNEAK && monster[mid].MType->mtype <= MT_ILLWEAV) || dam >> 6 >= monster[mid].mLevel + 3) {
		if (i >= 0)
			monster[mid]._mdir = opposite[monster[i]._mdir];

		if (monster[mid].MType->mtype == MT_BLINK) {
			M_Teleport(mid);
		} else if ((monster[mid].MType->mtype >= MT_NSCAV && monster[mid].MType->mtype <= MT_YSCAV)
		    || monster[mid].MType->mtype == MT_GRAVEDIG) {
			monster[mid]._mgoal = MGOAL_NORMAL;
			monster[mid]._mgoalvar1 = 0;
			monster[mid]._mgoalvar2 = 0;
		}

		if (monster[mid]._mmode != MM_STONE) {
			if (monster[mid].MType->mtype != MT_GOLEM) {
				NewMonsterAnim(mid, &monster[mid].MType->Anims[MA_GOTHIT], monster[mid]._mdir, gGameLogicStep < GameLogicStep::ProcessMonsters ? AnimationDistributionFlags::ProcessAnimationPending : AnimationDistributionFlags::None);
				monster[mid]._mmode = MM_GOTHIT;
			}

			monster[mid].position.offset = { 0, 0 };
			monster[mid].position.tile = monster[mid].position.old;
			monster[mid].position.future = monster[mid].position.old;
			M_ClearSquares(mid);
			dMonster[monster[mid].position.tile.x][monster[mid].position.tile.y] = mid + 1;
		}
	}
}

void MonstStartKill(int i, int pnum, bool sendmsg)
{
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	assurance(Monst->MType != nullptr, i);

	if (pnum >= 0)
		Monst->mWhoHit |= 1 << pnum;
	if (pnum < MAX_PLRS && i >= MAX_PLRS) /// BUGFIX: i >= MAX_PLRS (fixed)
		AddPlrMonstExper(Monst->mLevel, Monst->mExp, Monst->mWhoHit);
	monstkills[Monst->MType->mtype]++;
	Monst->_mhitpoints = 0;
	SetRndSeed(Monst->_mRndSeed);
	SpawnLoot(i, sendmsg);
	if (Monst->MType->mtype == MT_DIABLO)
		M_DiabloDeath(i, true);
	else
		PlayEffect(i, 2);

	Direction md = pnum >= 0 ? M_GetDir(i) : Monst->_mdir;
	Monst->_mdir = md;
	NewMonsterAnim(i, &Monst->MType->Anims[MA_DEATH], md, gGameLogicStep < GameLogicStep::ProcessMonsters ? AnimationDistributionFlags::ProcessAnimationPending : AnimationDistributionFlags::None);
	Monst->_mmode = MM_DEATH;
	Monst->_mgoal = MGOAL_NONE;
	Monst->position.offset = { 0, 0 };
	Monst->_mVar1 = 0;
	Monst->position.tile = Monst->position.old;
	Monst->position.future = Monst->position.old;
	M_ClearSquares(i);
	dMonster[Monst->position.tile.x][Monst->position.tile.y] = i + 1;
	CheckQuestKill(i, sendmsg);
	M_FallenFear(Monst->position.tile);
	if ((Monst->MType->mtype >= MT_NACID && Monst->MType->mtype <= MT_XACID) || Monst->MType->mtype == MT_SPIDLORD)
		AddMissile(Monst->position.tile, { 0, 0 }, 0, MIS_ACIDPUD, TARGET_PLAYERS, i, Monst->_mint + 1, 0);
}

void M2MStartKill(int i, int mid)
{
	assurance((DWORD)i < MAXMONSTERS, i);
	assurance((DWORD)mid < MAXMONSTERS, mid);
	assurance(monster[mid].MType != nullptr, mid); /// BUGFIX: should check `mid` (fixed)

	delta_kill_monster(mid, monster[mid].position.tile, currlevel);
	NetSendCmdLocParam1(false, CMD_MONSTDEATH, monster[mid].position.tile, mid);

	if (i < MAX_PLRS) {
		monster[mid].mWhoHit |= 1 << i;
		if (mid >= MAX_PLRS)
			AddPlrMonstExper(monster[mid].mLevel, monster[mid].mExp, monster[mid].mWhoHit);
	}

	monstkills[monster[mid].MType->mtype]++;
	monster[mid]._mhitpoints = 0;
	SetRndSeed(monster[mid]._mRndSeed);

	SpawnLoot(mid, true);

	if (monster[mid].MType->mtype == MT_DIABLO)
		M_DiabloDeath(mid, true);
	else
		PlayEffect(mid, 2);

	Direction md = opposite[monster[i]._mdir];
	if (monster[mid].MType->mtype == MT_GOLEM)
		md = DIR_S;

	monster[mid]._mdir = md;
	NewMonsterAnim(mid, &monster[mid].MType->Anims[MA_DEATH], md, gGameLogicStep < GameLogicStep::ProcessMonsters ? AnimationDistributionFlags::ProcessAnimationPending : AnimationDistributionFlags::None);
	monster[mid]._mmode = MM_DEATH;
	monster[mid].position.offset = { 0, 0 };
	monster[mid].position.tile = monster[mid].position.old;
	monster[mid].position.future = monster[mid].position.old;
	M_ClearSquares(mid);
	dMonster[monster[mid].position.tile.x][monster[mid].position.tile.y] = mid + 1;
	CheckQuestKill(mid, true);
	M_FallenFear(monster[mid].position.tile);
	if (monster[mid].MType->mtype >= MT_NACID && monster[mid].MType->mtype <= MT_XACID)
		AddMissile(monster[mid].position.tile, { 0, 0 }, 0, MIS_ACIDPUD, TARGET_PLAYERS, mid, monster[mid]._mint + 1, 0);

	if (gbIsHellfire)
		M_StartStand(i, monster[i]._mdir);
}

void M_StartKill(int i, int pnum)
{
	assurance((DWORD)i < MAXMONSTERS, i);

	if (myplr == pnum) {
		delta_kill_monster(i, monster[i].position.tile, currlevel);
		if (i != pnum) {
			NetSendCmdLocParam1(false, CMD_MONSTDEATH, monster[i].position.tile, i);
		} else {
			NetSendCmdLocParam1(false, CMD_KILLGOLEM, monster[i].position.tile, currlevel);
		}
	}

	MonstStartKill(i, pnum, true);
}

void M_SyncStartKill(int i, Point position, int pnum)
{
	assurance((DWORD)i < MAXMONSTERS, i);

	if (monster[i]._mhitpoints > 0 || monster[i]._mmode == MM_DEATH) {
		return;
	}

	if (dMonster[position.x][position.y] == 0) {
		M_ClearSquares(i);
		monster[i].position.tile = position;
		monster[i].position.old = position;
	}

	if (monster[i]._mmode == MM_STONE) {
		MonstStartKill(i, pnum, false);
		monster[i].Petrify();
	} else {
		MonstStartKill(i, pnum, false);
	}
}

void M_StartFadein(int i, Direction md, bool backwards)
{
	assurance((DWORD)i < MAXMONSTERS, i);
	assurance(monster[i].MType != nullptr, i);

	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_FADEIN;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
	monster[i]._mdir = md;
	monster[i]._mFlags &= ~MFLAG_HIDDEN;
	if (backwards) {
		monster[i]._mFlags |= MFLAG_LOCK_ANIMATION;
		monster[i].AnimInfo.CurrentFrame = monster[i].AnimInfo.NumberOfFrames;
	}
}

void M_StartFadeout(int i, Direction md, bool backwards)
{
	assurance((DWORD)i < MAXMONSTERS, i);
	assurance(monster[i].MType != nullptr, i);
	assurance(monster[i].MType != nullptr, i);

	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_FADEOUT;
	monster[i].position.offset = { 0, 0 };
	monster[i].position.future = monster[i].position.tile;
	monster[i].position.old = monster[i].position.tile;
	monster[i]._mdir = md;
	if (backwards) {
		monster[i]._mFlags |= MFLAG_LOCK_ANIMATION;
		monster[i].AnimInfo.CurrentFrame = monster[i].AnimInfo.NumberOfFrames;
	}
}

void M_StartHeal(int i)
{
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);
	assurance(monster[i].MType != nullptr, i);

	Monst = &monster[i];
	Monst->AnimInfo.pCelSprite = &*Monst->MType->Anims[MA_SPECIAL].CelSpritesForDirections[Monst->_mdir];
	Monst->AnimInfo.CurrentFrame = Monst->MType->Anims[MA_SPECIAL].Frames;
	Monst->_mFlags |= MFLAG_LOCK_ANIMATION;
	Monst->_mmode = MM_HEAL;
	Monst->_mVar1 = Monst->_mmaxhp / (16 * (GenerateRnd(5) + 4));
}

void M_ChangeLightOffset(int monst)
{
	int lx, ly, _mxoff, _myoff, sign;

	assurance((DWORD)monst < MAXMONSTERS, monst);

	lx = monster[monst].position.offset.deltaX + 2 * monster[monst].position.offset.deltaY;
	ly = 2 * monster[monst].position.offset.deltaY - monster[monst].position.offset.deltaX;

	if (lx < 0) {
		sign = -1;
		lx = -lx;
	} else {
		sign = 1;
	}

	_mxoff = sign * (lx / 8);
	if (ly < 0) {
		_myoff = -1;
		ly = -ly;
	} else {
		_myoff = 1;
	}

	_myoff *= (ly / 8);
	if (monster[monst].mlid != NO_LIGHT)
		ChangeLightOff(monster[monst].mlid, { _mxoff, _myoff });
}

bool M_DoStand(int i)
{
	MonsterStruct *Monst;

	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);

	Monst = &monster[i];
	if (Monst->MType->mtype == MT_GOLEM)
		Monst->AnimInfo.pCelSprite = &*Monst->MType->Anims[MA_WALK].CelSpritesForDirections[Monst->_mdir];
	else
		Monst->AnimInfo.pCelSprite = &*Monst->MType->Anims[MA_STAND].CelSpritesForDirections[Monst->_mdir];

	if (Monst->AnimInfo.CurrentFrame == Monst->AnimInfo.NumberOfFrames)
		M_Enemy(i);

	Monst->_mVar2++;

	return false;
}

/**
 * @brief Continue movement towards new tile
 */
bool M_DoWalk(int i, int variant)
{
	bool returnValue;

	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);

	//Check if we reached new tile
	if (monster[i].AnimInfo.CurrentFrame == monster[i].MType->Anims[MA_WALK].Frames) {
		switch (variant) {
		case MM_WALK:
			dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = 0;
			monster[i].position.tile.x += monster[i]._mVar1;
			monster[i].position.tile.y += monster[i]._mVar2;
			dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = i + 1;
			break;
		case MM_WALK2:
			dMonster[monster[i]._mVar1][monster[i]._mVar2] = 0;
			break;
		case MM_WALK3:
			dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = 0;
			monster[i].position.tile = { monster[i]._mVar1, monster[i]._mVar2 };
			dFlags[monster[i].position.temp.x][monster[i].position.temp.y] &= ~BFLAG_MONSTLR;
			dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = i + 1;
			break;
		}
		if (monster[i].mlid != NO_LIGHT)
			ChangeLightXY(monster[i].mlid, monster[i].position.tile);
		M_StartStand(i, monster[i]._mdir);
		returnValue = true;
	} else { //We didn't reach new tile so update monster's "sub-tile" position
		if (monster[i].AnimInfo.TickCounterOfCurrentFrame == 0) {
			if (monster[i].AnimInfo.CurrentFrame == 0 && monster[i].MType->mtype == MT_FLESTHNG)
				PlayEffect(i, 3);
			monster[i].position.offset2 += monster[i].position.velocity;
			monster[i].position.offset.deltaX = monster[i].position.offset2.deltaX >> 4;
			monster[i].position.offset.deltaY = monster[i].position.offset2.deltaY >> 4;
		}
		returnValue = false;
	}

	if (monster[i].mlid != NO_LIGHT) // BUGFIX: change uniqtype check to mlid check like it is in all other places (fixed)
		M_ChangeLightOffset(i);

	return returnValue;
}

void M_TryM2MHit(int i, int mid, int hper, int mind, int maxd)
{
	bool ret;

	assurance((DWORD)mid < MAXMONSTERS, mid);
	assurance(monster[mid].MType != nullptr, mid);
	if (monster[mid]._mhitpoints >> 6 > 0 && (monster[mid].MType->mtype != MT_ILLWEAV || monster[mid]._mgoal != MGOAL_RETREAT)) {
		int hit = GenerateRnd(100);
		if (monster[mid]._mmode == MM_STONE)
			hit = 0;
		if (!CheckMonsterHit(mid, &ret) && hit < hper) {
			int dam = (mind + GenerateRnd(maxd - mind + 1)) << 6;
			monster[mid]._mhitpoints -= dam;
			if (monster[mid]._mhitpoints >> 6 <= 0) {
				if (monster[mid]._mmode == MM_STONE) {
					M2MStartKill(i, mid);
					monster[mid].Petrify();
				} else {
					M2MStartKill(i, mid);
				}
			} else {
				if (monster[mid]._mmode == MM_STONE) {
					M2MStartHit(mid, i, dam);
					monster[mid].Petrify();
				} else {
					M2MStartHit(mid, i, dam);
				}
			}
		}
	}
}

void M_TryH2HHit(int i, int pnum, int Hit, int MinDam, int MaxDam)
{
	int hit, hper;
	int blk, blkper;
	int dam, mdam;
	int j, misnum, cur_ms_num, ac;

	assurance((DWORD)i < MAXMONSTERS, i);
	assurance(monster[i].MType != nullptr, i);
	if ((monster[i]._mFlags & MFLAG_TARGETS_MONSTER) != 0) {
		M_TryM2MHit(i, pnum, Hit, MinDam, MaxDam);
		return;
	}
	if (plr[pnum]._pHitPoints >> 6 <= 0 || plr[pnum]._pInvincible || (plr[pnum]._pSpellFlags & 1) != 0)
		return;
	if (monster[i].position.tile.WalkingDistance(plr[pnum].position.tile) >= 2)
		return;

	hper = GenerateRnd(100);
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		hper = 1000;
#endif
	ac = plr[pnum]._pIBonusAC + plr[pnum]._pIAC;
	if ((plr[pnum].pDamAcFlags & 0x20) != 0 && monster[i].MData->mMonstClass == MC_DEMON)
		ac += 40;
	if ((plr[pnum].pDamAcFlags & 0x40) != 0 && monster[i].MData->mMonstClass == MC_UNDEAD)
		ac += 20;
	hit = Hit
	    + 2 * (monster[i].mLevel - plr[pnum]._pLevel)
	    + 30
	    - ac
	    - plr[pnum]._pDexterity / 5;
	if (hit < 15)
		hit = 15;
	if (currlevel == 14 && hit < 20)
		hit = 20;
	if (currlevel == 15 && hit < 25)
		hit = 25;
	if (currlevel == 16 && hit < 30)
		hit = 30;
	if ((plr[pnum]._pmode == PM_STAND || plr[pnum]._pmode == PM_ATTACK) && plr[pnum]._pBlockFlag) {
		blkper = GenerateRnd(100);
	} else {
		blkper = 100;
	}
	blk = plr[pnum]._pDexterity
	    + plr[pnum]._pBaseToBlk
	    - (monster[i].mLevel * 2)
	    + (plr[pnum]._pLevel * 2);
	if (blk < 0)
		blk = 0;
	if (blk > 100)
		blk = 100;
	if (hper >= hit)
		return;
	if (blkper < blk) {
		Direction dir = GetDirection(plr[pnum].position.tile, monster[i].position.tile);
		StartPlrBlock(pnum, dir);
		if (pnum == myplr && plr[pnum].wReflections > 0) {
			plr[pnum].wReflections--;
			dam = GenerateRnd((MaxDam - MinDam + 1) << 6) + (MinDam << 6);
			dam += plr[pnum]._pIGetHit << 6;
			if (dam < 64)
				dam = 64;
			mdam = dam * (0.01 * (GenerateRnd(10) + 20));
			monster[i]._mhitpoints -= mdam;
			dam -= mdam;
			if (dam < 0)
				dam = 0;
			if (monster[i]._mhitpoints >> 6 <= 0)
				M_StartKill(i, pnum);
			else
				M_StartHit(i, pnum, mdam);
		}
		return;
	}
	if (monster[i].MType->mtype == MT_YZOMBIE && pnum == myplr) {
		cur_ms_num = -1;
		for (j = 0; j < nummissiles; j++) {
			misnum = missileactive[j];
			if (missile[misnum]._mitype != MIS_MANASHIELD)
				continue;
			if (missile[misnum]._misource == pnum)
				cur_ms_num = misnum;
		}
		if (plr[pnum]._pMaxHP > 64) {
			if (plr[pnum]._pMaxHPBase > 64) {
				plr[pnum]._pMaxHP -= 64;
				if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
					plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
					if (cur_ms_num >= 0)
						missile[cur_ms_num]._miVar1 = plr[pnum]._pHitPoints;
				}
				plr[pnum]._pMaxHPBase -= 64;
				if (plr[pnum]._pHPBase > plr[pnum]._pMaxHPBase) {
					plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
					if (cur_ms_num >= 0)
						missile[cur_ms_num]._miVar2 = plr[pnum]._pHPBase;
				}
			}
		}
	}
	dam = (MinDam << 6) + GenerateRnd((MaxDam - MinDam + 1) << 6);
	dam += (plr[pnum]._pIGetHit << 6);
	if (dam < 64)
		dam = 64;
	if (pnum == myplr) {
		if (plr[pnum].wReflections > 0) {
			plr[pnum].wReflections--;
			mdam = dam * (0.01 * (GenerateRnd(10) + 20));
			monster[i]._mhitpoints -= mdam;
			dam -= mdam;
			if (dam < 0)
				dam = 0;
			if (monster[i]._mhitpoints >> 6 <= 0)
				M_StartKill(i, pnum);
			else
				M_StartHit(i, pnum, mdam);
		}
		ApplyPlrDamage(pnum, 0, 0, dam);
	}
	if ((plr[pnum]._pIFlags & ISPL_THORNS) != 0) {
		mdam = (GenerateRnd(3) + 1) << 6;
		monster[i]._mhitpoints -= mdam;
		if (monster[i]._mhitpoints >> 6 <= 0)
			M_StartKill(i, pnum);
		else
			M_StartHit(i, pnum, mdam);
	}
	if ((monster[i]._mFlags & MFLAG_NOLIFESTEAL) == 0 && monster[i].MType->mtype == MT_SKING && gbIsMultiplayer)
		monster[i]._mhitpoints += dam;
	if (plr[pnum]._pHitPoints >> 6 <= 0) {
		if (gbIsHellfire)
			M_StartStand(i, monster[i]._mdir);
		return;
	}
	StartPlrHit(pnum, dam, false);
	if ((monster[i]._mFlags & MFLAG_KNOCKBACK) != 0) {
		if (plr[pnum]._pmode != PM_GOTHIT)
			StartPlrHit(pnum, 0, true);

		Point newPosition = plr[pnum].position.tile + monster[i]._mdir;
		if (PosOkPlayer(pnum, newPosition)) {
			plr[pnum].position.tile = newPosition;
			FixPlayerLocation(pnum, plr[pnum]._pdir);
			FixPlrWalkTags(pnum);
			dPlayer[newPosition.x][newPosition.y] = pnum + 1;
			SetPlayerOld(plr[pnum]);
		}
	}
}

bool M_DoAttack(int i)
{
	MonsterStruct *Monst;

	commitment((DWORD)i < MAXMONSTERS, i);
	Monst = &monster[i];
	commitment(Monst->MType != nullptr, i);
	commitment(Monst->MData != nullptr, i); // BUGFIX: should check MData (fixed)

	if (monster[i].AnimInfo.CurrentFrame == monster[i].MData->mAFNum) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit, monster[i].mMinDamage, monster[i].mMaxDamage);
		if (monster[i]._mAi != AI_SNAKE)
			PlayEffect(i, 0);
	}
	if (monster[i].MType->mtype >= MT_NMAGMA && monster[i].MType->mtype <= MT_WMAGMA && monster[i].AnimInfo.CurrentFrame == 9) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit + 10, monster[i].mMinDamage - 2, monster[i].mMaxDamage - 2);
		PlayEffect(i, 0);
	}
	if (monster[i].MType->mtype >= MT_STORM && monster[i].MType->mtype <= MT_MAEL && monster[i].AnimInfo.CurrentFrame == 13) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit - 20, monster[i].mMinDamage + 4, monster[i].mMaxDamage + 4);
		PlayEffect(i, 0);
	}
	if (monster[i]._mAi == AI_SNAKE && monster[i].AnimInfo.CurrentFrame == 1)
		PlayEffect(i, 0);
	if (monster[i].AnimInfo.CurrentFrame == monster[i].AnimInfo.NumberOfFrames) {
		M_StartStand(i, monster[i]._mdir);
		return true;
	}

	return false;
}

bool M_DoRAttack(int i)
{
	int multimissiles, mi;

	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);
	commitment(monster[i].MData != nullptr, i);

	if (monster[i].AnimInfo.CurrentFrame == monster[i].MData->mAFNum) {
		if (monster[i]._mVar1 != -1) {
			if (monster[i]._mVar1 == MIS_CBOLT)
				multimissiles = 3;
			else
				multimissiles = 1;
			for (mi = 0; mi < multimissiles; mi++) {
				Point sourcePosition = monster[i].position.tile;
				if (gbIsHellfire) {
					sourcePosition += monster[i]._mdir;
				}

				AddMissile(
				    sourcePosition,
				    monster[i].enemyPosition,
				    monster[i]._mdir,
				    monster[i]._mVar1,
				    TARGET_PLAYERS,
				    i,
				    monster[i]._mVar2,
				    0);
			}
		}
		PlayEffect(i, 0);
	}

	if (monster[i].AnimInfo.CurrentFrame == monster[i].AnimInfo.NumberOfFrames) {
		M_StartStand(i, monster[i]._mdir);
		return true;
	}

	return false;
}

bool M_DoRSpAttack(int i)
{
	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);
	commitment(monster[i].MData != nullptr, i); // BUGFIX: should check MData (fixed)

	if (monster[i].AnimInfo.CurrentFrame == monster[i].MData->mAFNum2 && monster[i].AnimInfo.TickCounterOfCurrentFrame == 0) {
		Point sourcePosition = monster[i].position.tile;
		if (gbIsHellfire) {
			sourcePosition += monster[i]._mdir;
		}

		AddMissile(
		    sourcePosition,
		    monster[i].enemyPosition,
		    monster[i]._mdir,
		    monster[i]._mVar1,
		    TARGET_PLAYERS,
		    i,
		    monster[i]._mVar3,
		    0);
		PlayEffect(i, 3);
	}

	if (monster[i]._mAi == AI_MEGA && monster[i].AnimInfo.CurrentFrame == monster[i].MData->mAFNum2) {
		if (monster[i]._mVar2++ == 0) {
			monster[i]._mFlags |= MFLAG_ALLOW_SPECIAL;
		} else if (monster[i]._mVar2 == 15) {
			monster[i]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
	}

	if (monster[i].AnimInfo.CurrentFrame == monster[i].AnimInfo.NumberOfFrames) {
		M_StartStand(i, monster[i]._mdir);
		return true;
	}

	return false;
}

bool M_DoSAttack(int i)
{
	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);
	commitment(monster[i].MData != nullptr, i);

	if (monster[i].AnimInfo.CurrentFrame == monster[i].MData->mAFNum2)
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit2, monster[i].mMinDamage2, monster[i].mMaxDamage2);

	if (monster[i].AnimInfo.CurrentFrame == monster[i].AnimInfo.NumberOfFrames) {
		M_StartStand(i, monster[i]._mdir);
		return true;
	}

	return false;
}

bool M_DoFadein(int i)
{
	commitment((DWORD)i < MAXMONSTERS, i);

	if (((monster[i]._mFlags & MFLAG_LOCK_ANIMATION) == 0 || monster[i].AnimInfo.CurrentFrame != 1)
	    && ((monster[i]._mFlags & MFLAG_LOCK_ANIMATION) != 0 || monster[i].AnimInfo.CurrentFrame != monster[i].AnimInfo.NumberOfFrames)) {
		return false;
	}

	M_StartStand(i, monster[i]._mdir);
	monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;

	return true;
}

bool M_DoFadeout(int i)
{
	int mt;

	commitment((DWORD)i < MAXMONSTERS, i);

	if (((monster[i]._mFlags & MFLAG_LOCK_ANIMATION) == 0 || monster[i].AnimInfo.CurrentFrame != 1)
	    && ((monster[i]._mFlags & MFLAG_LOCK_ANIMATION) != 0 || monster[i].AnimInfo.CurrentFrame != monster[i].AnimInfo.NumberOfFrames)) {
		return false;
	}

	mt = monster[i].MType->mtype;
	if (mt < MT_INCIN || mt > MT_HELLBURN) {
		monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;
		monster[i]._mFlags |= MFLAG_HIDDEN;
	} else {
		monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;
	}

	M_StartStand(i, monster[i]._mdir);

	return true;
}

bool M_DoHeal(int i)
{
	MonsterStruct *Monst;

	commitment((DWORD)i < MAXMONSTERS, i);
	Monst = &monster[i];
	if ((monster[i]._mFlags & MFLAG_NOHEAL) != 0) {
		Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		Monst->_mmode = MM_SATTACK;
		return false;
	}

	if (Monst->AnimInfo.CurrentFrame == 1) {
		Monst->_mFlags &= ~MFLAG_LOCK_ANIMATION;
		Monst->_mFlags |= MFLAG_ALLOW_SPECIAL;
		if (Monst->_mVar1 + Monst->_mhitpoints < Monst->_mmaxhp) {
			Monst->_mhitpoints = Monst->_mVar1 + Monst->_mhitpoints;
		} else {
			Monst->_mhitpoints = Monst->_mmaxhp;
			Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
			Monst->_mmode = MM_SATTACK;
		}
	}
	return false;
}

bool M_DoTalk(int i)
{
	MonsterStruct *Monst;
	int tren;

	commitment((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	M_StartStand(i, monster[i]._mdir);
	Monst->_mgoal = MGOAL_TALKING; // CODEFIX: apply Monst instead of monster[i] in the rest of the function
	if (effect_is_playing(alltext[monster[i].mtalkmsg].sfxnr))
		return false;
	InitQTextMsg(monster[i].mtalkmsg);
	if (monster[i]._uniqtype - 1 == UMT_GARBUD) {
		if (monster[i].mtalkmsg == TEXT_GARBUD1) {
			quests[Q_GARBUD]._qactive = QUEST_ACTIVE;
			quests[Q_GARBUD]._qlog = true; // BUGFIX: (?) for other quests qactive and qlog go together, maybe this should actually go into the if above (fixed)
		}
		if (monster[i].mtalkmsg == TEXT_GARBUD2 && (monster[i]._mFlags & MFLAG_QUEST_COMPLETE) == 0) {
			SpawnItem(i, monster[i].position.tile + Displacement { 1, 1 }, true);
			monster[i]._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (monster[i]._uniqtype - 1 == UMT_ZHAR
	    && monster[i].mtalkmsg == TEXT_ZHAR1
	    && (monster[i]._mFlags & MFLAG_QUEST_COMPLETE) == 0) {
		quests[Q_ZHAR]._qactive = QUEST_ACTIVE;
		quests[Q_ZHAR]._qlog = true;
		CreateTypeItem(monster[i].position.tile + Displacement { 1, 1 }, false, ITYPE_MISC, IMISC_BOOK, true, false);
		monster[i]._mFlags |= MFLAG_QUEST_COMPLETE;
	}
	if (monster[i]._uniqtype - 1 == UMT_SNOTSPIL) {
		if (monster[i].mtalkmsg == TEXT_BANNER10 && (monster[i]._mFlags & MFLAG_QUEST_COMPLETE) == 0) {
			ObjChangeMap(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 2, (setpc_h / 2) + setpc_y - 2);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w / 2) + setpc_x + 4, setpc_y + (setpc_h / 2));
			TransVal = tren;
			quests[Q_LTBANNER]._qvar1 = 2;
			if (quests[Q_LTBANNER]._qactive == QUEST_INIT)
				quests[Q_LTBANNER]._qactive = QUEST_ACTIVE;
			monster[i]._mFlags |= MFLAG_QUEST_COMPLETE;
		}
		if (quests[Q_LTBANNER]._qvar1 < 2) {
			app_fatal("SS Talk = %i, Flags = %i", monster[i].mtalkmsg, monster[i]._mFlags);
		}
	}
	if (monster[i]._uniqtype - 1 == UMT_LACHDAN) {
		if (monster[i].mtalkmsg == TEXT_VEIL9) {
			quests[Q_VEIL]._qactive = QUEST_ACTIVE;
			quests[Q_VEIL]._qlog = true;
		}
		if (monster[i].mtalkmsg == TEXT_VEIL11 && (monster[i]._mFlags & MFLAG_QUEST_COMPLETE) == 0) {
			SpawnUnique(UITEM_STEELVEIL, monster[i].position.tile + DIR_S);
			monster[i]._mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (monster[i]._uniqtype - 1 == UMT_WARLORD)
		quests[Q_WARLORD]._qvar1 = 2;
	if (monster[i]._uniqtype - 1 == UMT_LAZURUS && gbIsMultiplayer) {
		quests[Q_BETRAYER]._qvar1 = 6;
		monster[i]._mgoal = MGOAL_NORMAL;
		monster[i]._msquelch = UINT8_MAX;
		monster[i].mtalkmsg = TEXT_NONE;
	}
	return false;
}

void M_Teleport(int i)
{
	bool done;
	MonsterStruct *Monst;
	int k, j, x, y, _mx, _my, rx, ry;

	assurance((DWORD)i < MAXMONSTERS, i);

	done = false;

	Monst = &monster[i];
	if (Monst->_mmode == MM_STONE)
		return;

	_mx = Monst->enemyPosition.x;
	_my = Monst->enemyPosition.y;
	rx = 2 * GenerateRnd(2) - 1;
	ry = 2 * GenerateRnd(2) - 1;

	for (j = -1; j <= 1 && !done; j++) {
		for (k = -1; k < 1 && !done; k++) {
			if (j != 0 || k != 0) {
				x = _mx + rx * j;
				y = _my + ry * k;
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && x != Monst->position.tile.x && y != Monst->position.tile.y) {
					if (PosOkMonst(i, { x, y }))
						done = true;
				}
			}
		}
	}

	if (done) {
		M_ClearSquares(i);
		dMonster[Monst->position.tile.x][Monst->position.tile.y] = 0;
		dMonster[x][y] = i + 1;
		Monst->position.old = { x, y };
		Monst->_mdir = M_GetDir(i);
	}
}

bool M_DoGotHit(int i)
{
	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);

	if (monster[i].AnimInfo.CurrentFrame == monster[i].AnimInfo.NumberOfFrames) {
		M_StartStand(i, monster[i]._mdir);

		return true;
	}

	return false;
}

void M_UpdateLeader(int i)
{
	int ma, j;

	assurance((DWORD)i < MAXMONSTERS, i);

	for (j = 0; j < nummonsters; j++) {
		ma = monstactive[j];
		if (monster[ma].leaderflag == 1 && monster[ma].leader == i)
			monster[ma].leaderflag = 0;
	}

	if (monster[i].leaderflag == 1) {
		monster[monster[i].leader].packsize--;
	}
}

void DoEnding()
{
	bool bMusicOn;
	int musicVolume;

	if (gbIsMultiplayer) {
		SNetLeaveGame(LEAVE_ENDING);
	}

	music_stop();

	if (gbIsMultiplayer) {
		SDL_Delay(1000);
	}

	if (gbIsSpawn)
		return;

	if (plr[myplr]._pClass == HeroClass::Warrior || plr[myplr]._pClass == HeroClass::Barbarian) {
		play_movie("gendata\\DiabVic2.smk", false);
	} else if (plr[myplr]._pClass == HeroClass::Sorcerer) {
		play_movie("gendata\\DiabVic1.smk", false);
	} else if (plr[myplr]._pClass == HeroClass::Monk) {
		play_movie("gendata\\DiabVic1.smk", false);
	} else {
		play_movie("gendata\\DiabVic3.smk", false);
	}
	play_movie("gendata\\Diabend.smk", false);

	bMusicOn = gbMusicOn;
	gbMusicOn = true;

	musicVolume = sound_get_or_set_music_volume(1);
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
	deathflag = false;
	cineflag = true;

	plr[myplr].pDiabloKillLevel = std::max(plr[myplr].pDiabloKillLevel, static_cast<uint8_t>(sgGameInitInfo.nDifficulty + 1));

	for (auto &player : plr) {
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

bool M_DoDeath(int i)
{
	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);

	monster[i]._mVar1++;
	if (monster[i].MType->mtype == MT_DIABLO) {
		if (monster[i].position.tile.x < ViewX) {
			ViewX--;
		} else if (monster[i].position.tile.x > ViewX) {
			ViewX++;
		}

		if (monster[i].position.tile.y < ViewY) {
			ViewY--;
		} else if (monster[i].position.tile.y > ViewY) {
			ViewY++;
		}

		if (monster[i]._mVar1 == 140)
			PrepDoEnding();
	} else if (monster[i].AnimInfo.CurrentFrame == monster[i].AnimInfo.NumberOfFrames) {
		if (monster[i]._uniqtype == 0)
			AddDead(monster[i].position.tile, monster[i].MType->mdeadval, monster[i]._mdir);
		else
			AddDead(monster[i].position.tile, monster[i]._udeadval, monster[i]._mdir);

		dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = 0;
		monster[i]._mDelFlag = true;

		M_UpdateLeader(i);
	}
	return false;
}

bool M_DoSpStand(int i)
{
	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);

	if (monster[i].AnimInfo.CurrentFrame == monster[i].MData->mAFNum2)
		PlayEffect(i, 3);

	if (monster[i].AnimInfo.CurrentFrame == monster[i].AnimInfo.NumberOfFrames) {
		M_StartStand(i, monster[i]._mdir);
		return true;
	}

	return false;
}

bool M_DoDelay(int i)
{
	int oFrame;

	commitment((DWORD)i < MAXMONSTERS, i);
	commitment(monster[i].MType != nullptr, i);

	monster[i].AnimInfo.pCelSprite = &*monster[i].MType->Anims[MA_STAND].CelSpritesForDirections[M_GetDir(i)];
	if (monster[i]._mAi == AI_LAZURUS) {
		if (monster[i]._mVar2 > 8 || monster[i]._mVar2 < 0)
			monster[i]._mVar2 = 8;
	}

	if (monster[i]._mVar2-- == 0) {
		oFrame = monster[i].AnimInfo.CurrentFrame;
		M_StartStand(i, monster[i]._mdir);
		monster[i].AnimInfo.CurrentFrame = oFrame;
		return true;
	}

	return false;
}

bool M_DoStone(int i)
{
	commitment((DWORD)i < MAXMONSTERS, i);

	if (monster[i]._mhitpoints <= 0) {
		dMonster[monster[i].position.tile.x][monster[i].position.tile.y] = 0;
		monster[i]._mDelFlag = true;
	}

	return false;
}

void M_WalkDir(int i, Direction md)
{
	assurance((DWORD)i < MAXMONSTERS, i);

	int mwi = monster[i].MType->Anims[MA_WALK].Frames - 1;
	switch (md) {
	case DIR_N:
		M_StartWalk(i, 0, -MWVel[mwi][1], -1, -1, DIR_N);
		break;
	case DIR_NE:
		M_StartWalk(i, MWVel[mwi][1], -MWVel[mwi][0], 0, -1, DIR_NE);
		break;
	case DIR_E:
		M_StartWalk3(i, MWVel[mwi][2], 0, -32, -16, 1, -1, 1, 0, DIR_E);
		break;
	case DIR_SE:
		M_StartWalk2(i, MWVel[mwi][1], MWVel[mwi][0], -32, -16, 1, 0, DIR_SE);
		break;
	case DIR_S:
		M_StartWalk2(i, 0, MWVel[mwi][1], 0, -32, 1, 1, DIR_S);
		break;
	case DIR_SW:
		M_StartWalk2(i, -MWVel[mwi][1], MWVel[mwi][0], 32, -16, 0, 1, DIR_SW);
		break;
	case DIR_W:
		M_StartWalk3(i, -MWVel[mwi][2], 0, 32, -16, -1, 1, 0, 1, DIR_W);
		break;
	case DIR_NW:
		M_StartWalk(i, -MWVel[mwi][1], -MWVel[mwi][0], -1, 0, DIR_NW);
		break;
	case DIR_OMNI:
		break;
	}
}

void GroupUnity(int i)
{
	int leader, m, j;
	bool clear;

	assurance((DWORD)i < MAXMONSTERS, i);

	if (monster[i].leaderflag != 0) {
		leader = monster[i].leader;
		clear = LineClearSolid(monster[i].position.tile, monster[leader].position.future);
		if (clear || monster[i].leaderflag != 1) {
			if (clear
			    && monster[i].leaderflag == 2
			    && monster[i].position.tile.WalkingDistance(monster[leader].position.future) < 4) {
				monster[leader].packsize++;
				monster[i].leaderflag = 1;
			}
		} else {
			monster[leader].packsize--;
			monster[i].leaderflag = 2;
		}
	}

	if (monster[i].leaderflag == 1) {
		if (monster[i]._msquelch > monster[leader]._msquelch) {
			monster[leader].position.last = monster[i].position.tile;
			monster[leader]._msquelch = monster[i]._msquelch - 1;
		}
		if (monster[leader]._mAi == AI_GARG) {
			if ((monster[leader]._mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
				monster[leader]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
				monster[leader]._mmode = MM_SATTACK;
			}
		}
	} else if (monster[i]._uniqtype != 0) {
		if ((UniqMonst[monster[i]._uniqtype - 1].mUnqAttr & 2) != 0) {
			for (j = 0; j < nummonsters; j++) {
				m = monstactive[j];
				if (monster[m].leaderflag == 1 && monster[m].leader == i) {
					if (monster[i]._msquelch > monster[m]._msquelch) {
						monster[m].position.last = monster[i].position.tile;
						monster[m]._msquelch = monster[i]._msquelch - 1;
					}
					if (monster[m]._mAi == AI_GARG) {
						if ((monster[m]._mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
							monster[m]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
							monster[m]._mmode = MM_SATTACK;
						}
					}
				}
			}
		}
	}
}

bool M_CallWalk(int i, Direction md)
{
	Direction mdtemp = md;
	bool ok = DirOK(i, md);
	if (GenerateRnd(2) != 0)
		ok = ok || (md = left[mdtemp], DirOK(i, md)) || (md = right[mdtemp], DirOK(i, md));
	else
		ok = ok || (md = right[mdtemp], DirOK(i, md)) || (md = left[mdtemp], DirOK(i, md));
	if (GenerateRnd(2) != 0) {
		ok = ok
		    || (md = right[right[mdtemp]], DirOK(i, md))
		    || (md = left[left[mdtemp]], DirOK(i, md));
	} else {
		ok = ok
		    || (md = left[left[mdtemp]], DirOK(i, md))
		    || (md = right[right[mdtemp]], DirOK(i, md));
	}
	if (ok)
		M_WalkDir(i, md);
	return ok;
}

bool M_PathWalk(int i)
{
	int8_t path[MAX_PATH_LENGTH];
	bool (*Check)(int, Point);

	/** Maps from walking path step to facing direction. */
	const Direction plr2monst[9] = { DIR_S, DIR_NE, DIR_NW, DIR_SE, DIR_SW, DIR_N, DIR_E, DIR_S, DIR_W };

	commitment((DWORD)i < MAXMONSTERS, i);

	Check = PosOkMonst3;
	if ((monster[i]._mFlags & MFLAG_CAN_OPEN_DOOR) == 0)
		Check = PosOkMonst;

	if (FindPath(Check, i, monster[i].position.tile.x, monster[i].position.tile.y, monster[i].enemyPosition.x, monster[i].enemyPosition.y, path) == 0) {
		return false;
	}

	M_CallWalk(i, plr2monst[path[0]]);
	return true;
}

bool M_CallWalk2(int i, Direction md)
{
	Direction mdtemp = md;
	bool ok = DirOK(i, md);    // Can we continue in the same direction
	if (GenerateRnd(2) != 0) { // Randomly go left or right
		ok = ok || (mdtemp = left[md], DirOK(i, left[md])) || (mdtemp = right[md], DirOK(i, right[md]));
	} else {
		ok = ok || (mdtemp = right[md], DirOK(i, right[md])) || (mdtemp = left[md], DirOK(i, left[md]));
	}

	if (ok)
		M_WalkDir(i, mdtemp);

	return ok;
}

bool M_DumbWalk(int i, Direction md)
{
	bool ok = DirOK(i, md);
	if (ok)
		M_WalkDir(i, md);

	return ok;
}

static Direction turn(Direction direction, bool turnLeft)
{
	return turnLeft ? left[direction] : right[direction];
}

bool M_RoundWalk(int i, Direction direction, int *dir)
{
	Direction turn45deg = turn(direction, *dir != 0);
	Direction turn90deg = turn(turn45deg, *dir != 0);

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
	return M_CallWalk(i, opposite[turn90deg]);
}

void MAI_Zombie(int i)
{
	MonsterStruct *Monst;
	int mx, my;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND) {
		return;
	}

	mx = Monst->position.tile.x;
	my = Monst->position.tile.y;
	if ((dFlags[mx][my] & BFLAG_VISIBLE) == 0) {
		return;
	}

	if (GenerateRnd(100) < 2 * Monst->_mint + 10) {
		int dist = Monst->enemyPosition.WalkingDistance({ mx, my });
		if (dist >= 2) {
			if (dist >= 2 * Monst->_mint + 4) {
				Direction md = Monst->_mdir;
				if (GenerateRnd(100) < 2 * Monst->_mint + 20) {
					md = static_cast<Direction>(GenerateRnd(8));
				}
				M_DumbWalk(i, md);
			} else {
				M_CallWalk(i, M_GetDir(i));
			}
		} else {
			M_StartAttack(i);
		}
	}

	Monst->CheckStandAnimationIsLoaded(Monst->_mdir);
}

void MAI_SkelSd(int i)
{
	assurance((DWORD)i < MAXMONSTERS, i);

	MonsterStruct *Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	int x = Monst->position.tile.x - Monst->enemyPosition.x;
	int y = Monst->position.tile.y - Monst->enemyPosition.y;
	Direction md = GetDirection(Monst->position.tile, Monst->position.last);
	Monst->_mdir = md;
	if (abs(x) >= 2 || abs(y) >= 2) {
		if (Monst->_mVar1 == MM_DELAY || (GenerateRnd(100) >= 35 - 4 * Monst->_mint)) {
			M_CallWalk(i, md);
		} else {
			M_StartDelay(i, 15 - 2 * Monst->_mint + GenerateRnd(10));
		}
	} else {
		if (Monst->_mVar1 == MM_DELAY || (GenerateRnd(100) < 2 * Monst->_mint + 20)) {
			M_StartAttack(i);
		} else {
			M_StartDelay(i, 2 * (5 - Monst->_mint) + GenerateRnd(10));
		}
	}

	Monst->CheckStandAnimationIsLoaded(md);
}

bool MAI_Path(int i)
{
	MonsterStruct *Monst;
	bool clear;

	commitment((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->MType->mtype != MT_GOLEM) {
		if (Monst->_msquelch == 0)
			return false;
		if (Monst->_mmode != MM_STAND)
			return false;
		if (Monst->_mgoal != MGOAL_NORMAL && Monst->_mgoal != MGOAL_MOVE && Monst->_mgoal != MGOAL_ATTACK2)
			return false;
		if (Monst->position.tile.x == 1 && Monst->position.tile.y == 0)
			return false;
	}

	clear = LineClear(
	    PosOkMonst2,
	    i,
	    Monst->position.tile,
	    Monst->enemyPosition);
	if (!clear || (Monst->_pathcount >= 5 && Monst->_pathcount < 8)) {
		if ((Monst->_mFlags & MFLAG_CAN_OPEN_DOOR) != 0)
			MonstCheckDoors(i);
		Monst->_pathcount++;
		if (Monst->_pathcount < 5)
			return false;
		if (M_PathWalk(i))
			return true;
	}

	if (Monst->MType->mtype != MT_GOLEM)
		Monst->_pathcount = 0;

	return false;
}

void MAI_Snake(int i)
{
	MonsterStruct *Monst;
	int fx, fy, mx, my;
	int pnum;

	assurance((DWORD)i < MAXMONSTERS, i);
	char pattern[6] = { 1, 1, 0, -1, -1, 0 };
	Monst = &monster[i];
	pnum = Monst->_menemy;
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0)
		return;
	fx = Monst->enemyPosition.x;
	fy = Monst->enemyPosition.y;
	mx = Monst->position.tile.x - fx;
	my = Monst->position.tile.y - fy;
	Direction md = GetDirection(Monst->position.tile, Monst->position.last);
	Monst->_mdir = md;
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (abs(mx) < 3 && abs(my) < 3 && LineClear(PosOkMonst, i, Monst->position.tile, { fx, fy }) && Monst->_mVar1 != MM_CHARGE) {
			if (AddMissile(Monst->position.tile, { fx, fy }, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
				PlayEffect(i, 0);
				dMonster[Monst->position.tile.x][Monst->position.tile.y] = -(i + 1);
				Monst->_mmode = MM_CHARGE;
			}
		} else if (Monst->_mVar1 == MM_DELAY || GenerateRnd(100) >= 35 - 2 * Monst->_mint) {
			if (pattern[Monst->_mgoalvar1] == -1)
				md = left[md];
			else if (pattern[Monst->_mgoalvar1] == 1)
				md = right[md];

			Monst->_mgoalvar1++;
			if (Monst->_mgoalvar1 > 5)
				Monst->_mgoalvar1 = 0;

			if (md != Monst->_mgoalvar2) {
				int drift = md - Monst->_mgoalvar2;
				if (drift < 0)
					drift += 8;

				if (drift < 4)
					md = right[Monst->_mgoalvar2];
				else if (drift > 4)
					md = left[Monst->_mgoalvar2];
				Monst->_mgoalvar2 = md;
			}

			if (!M_DumbWalk(i, md))
				M_CallWalk2(i, Monst->_mdir);
		} else {
			M_StartDelay(i, 15 - Monst->_mint + GenerateRnd(10));
		}
	} else {
		if (Monst->_mVar1 == MM_DELAY
		    || Monst->_mVar1 == MM_CHARGE
		    || (GenerateRnd(100) < Monst->_mint + 20)) {
			M_StartAttack(i);
		} else
			M_StartDelay(i, 10 - Monst->_mint + GenerateRnd(10));
	}

	Monst->CheckStandAnimationIsLoaded(Monst->_mdir);
}

void MAI_Bat(int i)
{
	MonsterStruct *Monst;
	int v, pnum;
	int fx, fy, xd, yd;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	pnum = Monst->_menemy;
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	xd = Monst->position.tile.x - Monst->enemyPosition.x;
	yd = Monst->position.tile.y - Monst->enemyPosition.y;
	Direction md = GetDirection(Monst->position.tile, Monst->position.last);
	Monst->_mdir = md;
	v = GenerateRnd(100);
	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (Monst->_mgoalvar1 == 0) {
			M_CallWalk(i, opposite[md]);
			Monst->_mgoalvar1++;
		} else {
			if (GenerateRnd(2) != 0)
				M_CallWalk(i, left[md]);
			else
				M_CallWalk(i, right[md]);
			Monst->_mgoal = MGOAL_NORMAL;
		}
		return;
	}

	fx = Monst->enemyPosition.x;
	fy = Monst->enemyPosition.y;
	if (Monst->MType->mtype == MT_GLOOM
	    && (abs(xd) >= 5 || abs(yd) >= 5)
	    && v < 4 * Monst->_mint + 33
	    && LineClear(PosOkMonst, i, Monst->position.tile, { fx, fy })) {
		if (AddMissile(Monst->position.tile, { fx, fy }, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
			dMonster[Monst->position.tile.x][Monst->position.tile.y] = -(i + 1);
			Monst->_mmode = MM_CHARGE;
		}
	} else if (abs(xd) >= 2 || abs(yd) >= 2) {
		if ((Monst->_mVar2 > 20 && v < Monst->_mint + 13)
		    || ((Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < Monst->_mint + 63)) {
			M_CallWalk(i, md);
		}
	} else if (v < 4 * Monst->_mint + 8) {
		M_StartAttack(i);
		Monst->_mgoal = MGOAL_RETREAT;
		Monst->_mgoalvar1 = 0;
		if (Monst->MType->mtype == MT_FAMILIAR) {
			AddMissile(Monst->enemyPosition, { Monst->enemyPosition.x + 1, 0 }, -1, MIS_LIGHTNING, TARGET_PLAYERS, i, GenerateRnd(10) + 1, 0);
		}
	}

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_SkelBow(int i)
{
	MonsterStruct *Monst;
	int mx, my, v;
	bool walking;

	walking = false;
	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	mx = Monst->position.tile.x - Monst->enemyPosition.x;
	my = Monst->position.tile.y - Monst->enemyPosition.y;

	Direction md = M_GetDir(i);
	Monst->_mdir = md;
	v = GenerateRnd(100);

	if (abs(mx) < 4 && abs(my) < 4) {
		if ((Monst->_mVar2 > 20 && v < 2 * Monst->_mint + 13)
		    || ((Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < 2 * Monst->_mint + 63)) {
			walking = M_DumbWalk(i, opposite[md]);
		}
	}

	if (!walking) {
		if (GenerateRnd(100) < 2 * Monst->_mint + 3) {
			if (LineClearMissile(Monst->position.tile, Monst->enemyPosition))
				M_StartRAttack(i, MIS_ARROW, 4);
		}
	}

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_Fat(int i)
{
	MonsterStruct *Monst;
	int mx, my, v;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	mx = Monst->position.tile.x - Monst->enemyPosition.x;
	my = Monst->position.tile.y - Monst->enemyPosition.y;
	Direction md = M_GetDir(i);
	Monst->_mdir = md;
	v = GenerateRnd(100);
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if ((Monst->_mVar2 > 20 && v < 4 * Monst->_mint + 20)
		    || ((Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < 4 * Monst->_mint + 70)) {
			M_CallWalk(i, md);
		}
	} else if (v < 4 * Monst->_mint + 15) {
		M_StartAttack(i);
	} else if (v < 4 * Monst->_mint + 20) {
		M_StartSpAttack(i);
	}

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_Sneak(int i)
{
	MonsterStruct *Monst;
	int mx, my;
	int dist, v;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->_mmode == MM_STAND) {
		mx = Monst->position.tile.x;
		my = Monst->position.tile.y;
		if (dLight[mx][my] != lightmax) {
			mx -= Monst->enemyPosition.x;
			my -= Monst->enemyPosition.y;

			Direction md = M_GetDir(i);
			dist = 5 - Monst->_mint;
			if (Monst->_mVar1 == MM_GOTHIT) {
				Monst->_mgoal = MGOAL_RETREAT;
				Monst->_mgoalvar1 = 0;
			} else {
				if (abs(mx) >= dist + 3 || abs(my) >= dist + 3 || Monst->_mgoalvar1 > 8) {
					Monst->_mgoal = MGOAL_NORMAL;
					Monst->_mgoalvar1 = 0;
				}
			}
			if (Monst->_mgoal == MGOAL_RETREAT && (Monst->_mFlags & MFLAG_NO_ENEMY) == 0) {
				if ((Monst->_mFlags & MFLAG_TARGETS_MONSTER) != 0)
					md = GetDirection(Monst->position.tile, monster[Monst->_menemy].position.tile);
				else
					md = GetDirection(Monst->position.tile, plr[Monst->_menemy].position.last);
				md = opposite[md];
				if (Monst->MType->mtype == MT_UNSEEN) {
					if (GenerateRnd(2) != 0)
						md = left[md];
					else
						md = right[md];
				}
			}
			Monst->_mdir = md;
			v = GenerateRnd(100);
			if (abs(mx) < dist && abs(my) < dist && (Monst->_mFlags & MFLAG_HIDDEN) != 0) {
				M_StartFadein(i, md, false);
			} else {
				if ((abs(mx) >= dist + 1 || abs(my) >= dist + 1) && (Monst->_mFlags & MFLAG_HIDDEN) == 0) {
					M_StartFadeout(i, md, true);
				} else {
					if (Monst->_mgoal == MGOAL_RETREAT
					    || ((abs(mx) >= 2 || abs(my) >= 2) && ((Monst->_mVar2 > 20 && v < 4 * Monst->_mint + 14) || ((Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3) && Monst->_mVar2 == 0 && v < 4 * Monst->_mint + 64)))) {
						Monst->_mgoalvar1++;
						M_CallWalk(i, md);
					}
				}
			}
			if (Monst->_mmode == MM_STAND) {
				if (abs(mx) >= 2 || abs(my) >= 2 || v >= 4 * Monst->_mint + 10)
					Monst->AnimInfo.pCelSprite = &*Monst->MType->Anims[MA_STAND].CelSpritesForDirections[md];
				else
					M_StartAttack(i);
			}
		}
	}
}

void MAI_Fireman(int i)
{
	int xd, yd;
	int pnum;
	int fx, fy;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND || Monst->_msquelch == 0)
		return;

	pnum = monster[i]._menemy;
	fx = monster[i].enemyPosition.x;
	fy = monster[i].enemyPosition.y;
	xd = monster[i].position.tile.x - fx;
	yd = monster[i].position.tile.y - fy;

	Direction md = M_GetDir(i);
	if (Monst->_mgoal == MGOAL_NORMAL) {
		if (LineClearMissile(Monst->position.tile, { fx, fy })
		    && AddMissile(Monst->position.tile, { fx, fy }, md, MIS_FIREMAN, pnum, i, 0, 0) != -1) {
			Monst->_mmode = MM_CHARGE;
			Monst->_mgoal = MGOAL_ATTACK2;
			Monst->_mgoalvar1 = 0;
		}
	} else if (Monst->_mgoal == MGOAL_ATTACK2) {
		if (Monst->_mgoalvar1 == 3) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartFadeout(i, md, true);
		} else if (LineClearMissile(Monst->position.tile, { fx, fy })) {
			M_StartRAttack(i, MIS_KRULL, 4);
			Monst->_mgoalvar1++;
		} else {
			M_StartDelay(i, GenerateRnd(10) + 5);
			Monst->_mgoalvar1++;
		}
	} else if (Monst->_mgoal == MGOAL_RETREAT) {
		M_StartFadein(i, md, false);
		Monst->_mgoal = MGOAL_ATTACK2;
	}
	Monst->_mdir = md;
	GenerateRnd(100);
	if (Monst->_mmode != MM_STAND)
		return;

	if (abs(xd) < 2 && abs(yd) < 2 && Monst->_mgoal == MGOAL_NORMAL) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit, monster[i].mMinDamage, monster[i].mMaxDamage);
		Monst->_mgoal = MGOAL_RETREAT;
		if (!M_CallWalk(i, opposite[md])) {
			M_StartFadein(i, md, false);
			Monst->_mgoal = MGOAL_ATTACK2;
		}
	} else if (!M_CallWalk(i, md) && (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT)) {
		M_StartFadein(i, md, false);
		Monst->_mgoal = MGOAL_ATTACK2;
	}
}

void MAI_Fallen(int i)
{
	int x, y, xpos, ypos;
	int m, rad;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->_mgoal == MGOAL_ATTACK2) {
		if (Monst->_mgoalvar1 != 0)
			Monst->_mgoalvar1--;
		else
			Monst->_mgoal = MGOAL_NORMAL;
	}
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (Monst->_mgoalvar1-- == 0) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartStand(i, opposite[Monst->_mdir]);
		}
	}

	if (Monst->AnimInfo.CurrentFrame == Monst->AnimInfo.NumberOfFrames) {
		if (GenerateRnd(4) != 0) {
			return;
		}
		if ((monster[i]._mFlags & MFLAG_NOHEAL) == 0) { // CODEFIX: - change to Monst-> in devilutionx
			M_StartSpStand(i, Monst->_mdir);
			if (Monst->_mmaxhp - (2 * Monst->_mint + 2) >= Monst->_mhitpoints)
				Monst->_mhitpoints += 2 * Monst->_mint + 2;
			else
				Monst->_mhitpoints = Monst->_mmaxhp;
		}
		rad = 2 * Monst->_mint + 4;
		for (y = -rad; y <= rad; y++) {
			for (x = -rad; x <= rad; x++) {
				xpos = Monst->position.tile.x + x;
				ypos = Monst->position.tile.y + y;
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
					m = dMonster[xpos][ypos];
					if (m > 0) {
						m--;
						if (monster[m]._mAi == AI_FALLEN) {
							monster[m]._mgoal = MGOAL_ATTACK2;
							monster[m]._mgoalvar1 = 30 * Monst->_mint + 105;
						}
					}
				}
			}
		}
	} else if (Monst->_mgoal == MGOAL_RETREAT) {
		M_CallWalk(i, Monst->_mdir);
	} else if (Monst->_mgoal == MGOAL_ATTACK2) {
		xpos = Monst->position.tile.x - Monst->enemyPosition.x;
		ypos = Monst->position.tile.y - Monst->enemyPosition.y;
		if (abs(xpos) < 2 && abs(ypos) < 2)
			M_StartAttack(i);
		else
			M_CallWalk(i, M_GetDir(i));
	} else
		MAI_SkelSd(i);
}

void MAI_Cleaver(int i)
{
	MonsterStruct *Monst;
	int x, y, mx, my;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	mx = Monst->position.tile.x;
	my = Monst->position.tile.y;
	x = mx - Monst->enemyPosition.x;
	y = my - Monst->enemyPosition.y;

	Direction md = GetDirection({ mx, my }, Monst->position.last);
	Monst->_mdir = md;

	if (abs(x) >= 2 || abs(y) >= 2)
		M_CallWalk(i, md);
	else
		M_StartAttack(i);

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_Round(int i, bool special)
{
	MonsterStruct *Monst;
	int fx, fy;
	int mx, my;
	int dist, v;

	assurance((DWORD)i < MAXMONSTERS, i);
	Monst = &monster[i];
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fy = Monst->enemyPosition.y;
		fx = Monst->enemyPosition.x;
		mx = Monst->position.tile.x - fx;
		my = Monst->position.tile.y - fy;
		Direction md = GetDirection(Monst->position.tile, Monst->position.last);
		if (Monst->_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		v = GenerateRnd(100);
		if ((abs(mx) >= 2 || abs(my) >= 2) && Monst->_msquelch == UINT8_MAX && dTransVal[Monst->position.tile.x][Monst->position.tile.y] == dTransVal[fx][fy]) {
			if (Monst->_mgoal == MGOAL_MOVE || ((abs(mx) >= 4 || abs(my) >= 4) && GenerateRnd(4) == 0)) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = GenerateRnd(2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				if (abs(mx) > abs(my))
					dist = abs(mx);
				else
					dist = abs(my);
				if ((Monst->_mgoalvar1++ >= 2 * dist && DirOK(i, md)) || dTransVal[Monst->position.tile.x][Monst->position.tile.y] != dTransVal[fx][fy]) {
					Monst->_mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(i, md, &Monst->_mgoalvar2)) {
					M_StartDelay(i, GenerateRnd(10) + 10);
				}
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (abs(mx) >= 2 || abs(my) >= 2) {
				if ((Monst->_mVar2 > 20 && v < 2 * Monst->_mint + 28)
				    || ((Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
				        && Monst->_mVar2 == 0
				        && v < 2 * Monst->_mint + 78)) {
					M_CallWalk(i, md);
				}
			} else if (v < 2 * Monst->_mint + 23) {
				Monst->_mdir = md;
				if (special && Monst->_mhitpoints < (Monst->_mmaxhp / 2) && GenerateRnd(2) != 0)
					M_StartSpAttack(i);
				else
					M_StartAttack(i);
			}
		}

		Monst->CheckStandAnimationIsLoaded(md);
	}
}

void MAI_GoatMc(int i)
{
	MAI_Round(i, true);
}

void MAI_Ranged(int i, missile_id missile_type, bool special)
{
	int fx, fy, mx, my;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	Monst = &monster[i];
	if (Monst->_msquelch == UINT8_MAX || (Monst->_mFlags & MFLAG_TARGETS_MONSTER) != 0) {
		fx = Monst->enemyPosition.x;
		fy = Monst->enemyPosition.y;
		mx = Monst->position.tile.x - fx;
		my = Monst->position.tile.y - fy;
		Direction md = M_GetDir(i);
		if (Monst->_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		Monst->_mdir = md;
		if (Monst->_mVar1 == MM_RATTACK) {
			M_StartDelay(i, GenerateRnd(20));
		} else if (abs(mx) < 4 && abs(my) < 4) {
			if (GenerateRnd(100) < 10 * (Monst->_mint + 7))
				M_CallWalk(i, opposite[md]);
		}
		if (Monst->_mmode == MM_STAND) {
			if (LineClearMissile(Monst->position.tile, { fx, fy })) {
				if (special)
					M_StartRSpAttack(i, missile_type, 4);
				else
					M_StartRAttack(i, missile_type, 4);
			} else {
				Monst->AnimInfo.pCelSprite = &*Monst->MType->Anims[MA_STAND].CelSpritesForDirections[md];
			}
		}
	} else if (Monst->_msquelch != 0) {
		fx = Monst->position.last.x;
		fy = Monst->position.last.y;
		Direction md = GetDirection(Monst->position.tile, { fx, fy });
		M_CallWalk(i, md);
	}
}

void MAI_GoatBow(int i)
{
	MAI_Ranged(i, MIS_ARROW, false);
}

void MAI_Succ(int i)
{
	MAI_Ranged(i, MIS_FLARE, false);
}

void MAI_Lich(int i)
{
	MAI_Ranged(i, MIS_LICH, false);
}

void MAI_ArchLich(int i)
{
	MAI_Ranged(i, MIS_ARCHLICH, false);
}

void MAI_Psychorb(int i)
{
	MAI_Ranged(i, MIS_PSYCHORB, false);
}

void MAI_Necromorb(int i)
{
	MAI_Ranged(i, MIS_NECROMORB, false);
}

void MAI_AcidUniq(int i)
{
	MAI_Ranged(i, MIS_ACID, true);
}

void MAI_Firebat(int i)
{
	MAI_Ranged(i, MIS_FIREBOLT, false);
}

void MAI_Torchant(int i)
{
	MAI_Ranged(i, MIS_FIREBALL, false);
}

void MAI_Scav(int i)
{
	bool done;
	int x, y;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);
	Monst = &monster[i];
	done = false;
	if (monster[i]._mmode != MM_STAND)
		return;
	if (Monst->_mhitpoints < (Monst->_mmaxhp / 2) && Monst->_mgoal != MGOAL_HEALING) {
		if (Monst->leaderflag != 0) {
			monster[Monst->leader].packsize--;
			Monst->leaderflag = 0;
		}
		Monst->_mgoal = MGOAL_HEALING;
		Monst->_mgoalvar3 = 10;
	}
	if (Monst->_mgoal == MGOAL_HEALING && Monst->_mgoalvar3 != 0) {
		Monst->_mgoalvar3--;
		if (dDead[Monst->position.tile.x][Monst->position.tile.y] != 0) {
			M_StartEat(i);
			if ((Monst->_mFlags & MFLAG_NOHEAL) == 0) {
				if (gbIsHellfire) {
					int mMaxHP = Monst->_mmaxhp; // BUGFIX use _mmaxhp or we loose health when difficulty isn't normal (fixed)
					Monst->_mhitpoints += mMaxHP / 8;
					if (Monst->_mhitpoints > Monst->_mmaxhp)
						Monst->_mhitpoints = Monst->_mmaxhp;
					if (Monst->_mgoalvar3 <= 0 || Monst->_mhitpoints == Monst->_mmaxhp)
						dDead[Monst->position.tile.x][Monst->position.tile.y] = 0;
				} else {
					Monst->_mhitpoints += 64;
				}
			}
			int targetHealth = Monst->_mmaxhp;
			if (!gbIsHellfire)
				targetHealth = (Monst->_mmaxhp / 2) + (Monst->_mmaxhp / 4);
			if (Monst->_mhitpoints >= targetHealth) {
				Monst->_mgoal = MGOAL_NORMAL;
				Monst->_mgoalvar1 = 0;
				Monst->_mgoalvar2 = 0;
			}
		} else {
			if (Monst->_mgoalvar1 == 0) {
				if (GenerateRnd(2) != 0) {
					for (y = -4; y <= 4 && !done; y++) {
						for (x = -4; x <= 4 && !done; x++) {
							// BUGFIX: incorrect check of offset against limits of the dungeon
							if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
								continue;
							done = dDead[Monst->position.tile.x + x][Monst->position.tile.y + y] != 0
							    && LineClearSolid(
							        Monst->position.tile,
							        Monst->position.tile + Displacement { x, y });
						}
					}
					x--;
					y--;
				} else {
					for (y = 4; y >= -4 && !done; y--) {
						for (x = 4; x >= -4 && !done; x--) {
							// BUGFIX: incorrect check of offset against limits of the dungeon
							if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
								continue;
							done = dDead[Monst->position.tile.x + x][Monst->position.tile.y + y] != 0
							    && LineClearSolid(
							        Monst->position.tile,
							        Monst->position.tile + Displacement { x, y });
						}
					}
					x++;
					y++;
				}
				if (done) {
					Monst->_mgoalvar1 = x + Monst->position.tile.x + 1;
					Monst->_mgoalvar2 = y + Monst->position.tile.y + 1;
				}
			}
			if (Monst->_mgoalvar1 != 0) {
				x = Monst->_mgoalvar1 - 1;
				y = Monst->_mgoalvar2 - 1;
				Monst->_mdir = GetDirection(Monst->position.tile, { x, y });
				M_CallWalk(i, Monst->_mdir);
			}
		}
	}

	if (Monst->_mmode == MM_STAND)
		MAI_SkelSd(i);
}

void MAI_Garg(int i)
{
	MonsterStruct *Monst;
	int mx, my, dx, dy;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	dx = Monst->position.tile.x - Monst->position.last.x;
	dy = Monst->position.tile.y - Monst->position.last.y;
	Direction md = M_GetDir(i);
	if (Monst->_msquelch != 0 && (Monst->_mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
		M_Enemy(i);
		mx = Monst->position.tile.x - Monst->enemyPosition.x;
		my = Monst->position.tile.y - Monst->enemyPosition.y;
		if (abs(mx) < Monst->_mint + 2 && abs(my) < Monst->_mint + 2) {
			Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
		return;
	}

	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	if (Monst->_mhitpoints < (Monst->_mmaxhp / 2))
		if ((Monst->_mFlags & MFLAG_NOHEAL) == 0)
			Monst->_mgoal = MGOAL_RETREAT;
	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (abs(dx) >= Monst->_mint + 2 || abs(dy) >= Monst->_mint + 2) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartHeal(i);
		} else if (!M_CallWalk(i, opposite[md])) {
			Monst->_mgoal = MGOAL_NORMAL;
		}
	}
	MAI_Round(i, false);
}

void MAI_RoundRanged(int i, missile_id missile_type, bool checkdoors, int dam, int lessmissiles)
{
	MonsterStruct *Monst;
	int mx, my;
	int fx, fy;
	int dist, v;

	assurance((DWORD)i < MAXMONSTERS, i);
	Monst = &monster[i];
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->enemyPosition.x;
		fy = Monst->enemyPosition.y;
		mx = Monst->position.tile.x - fx;
		my = Monst->position.tile.y - fy;
		Direction md = GetDirection(Monst->position.tile, Monst->position.last);
		if (checkdoors && Monst->_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		v = GenerateRnd(10000);
		dist = std::max(abs(mx), abs(my));
		if (dist >= 2 && Monst->_msquelch == UINT8_MAX && dTransVal[Monst->position.tile.x][Monst->position.tile.y] == dTransVal[fx][fy]) {
			if (Monst->_mgoal == MGOAL_MOVE || (dist >= 3 && GenerateRnd(4 << lessmissiles) == 0)) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = GenerateRnd(2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				if (Monst->_mgoalvar1++ >= 2 * dist && DirOK(i, md)) {
					Monst->_mgoal = MGOAL_NORMAL;
				} else if (v < (500 * (Monst->_mint + 1) >> lessmissiles)
				    && (LineClearMissile(Monst->position.tile, { fx, fy }))) {
					M_StartRSpAttack(i, missile_type, dam);
				} else {
					M_RoundWalk(i, md, &Monst->_mgoalvar2);
				}
			}
		} else {
			Monst->_mgoal = MGOAL_NORMAL;
		}
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (((dist >= 3 && v < ((500 * (Monst->_mint + 2)) >> lessmissiles))
			        || v < ((500 * (Monst->_mint + 1)) >> lessmissiles))
			    && LineClearMissile(Monst->position.tile, { fx, fy })) {
				M_StartRSpAttack(i, missile_type, dam);
			} else if (dist >= 2) {
				v = GenerateRnd(100);
				if (v < 1000 * (Monst->_mint + 5)
				    || ((Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3) && Monst->_mVar2 == 0 && v < 1000 * (Monst->_mint + 8))) {
					M_CallWalk(i, md);
				}
			} else if (v < 1000 * (Monst->_mint + 6)) {
				Monst->_mdir = md;
				M_StartAttack(i);
			}
		}
		if (Monst->_mmode == MM_STAND) {
			M_StartDelay(i, GenerateRnd(10) + 5);
		}
	}
}

void MAI_Magma(int i)
{
	MAI_RoundRanged(i, MIS_MAGMABALL, true, 4, 0);
}

void MAI_Storm(int i)
{
	MAI_RoundRanged(i, MIS_LIGHTCTRL2, true, 4, 0);
}

void MAI_BoneDemon(int i)
{
	MAI_RoundRanged(i, MIS_BONEDEMON, true, 4, 0);
}

void MAI_Acid(int i)
{
	MAI_RoundRanged(i, MIS_ACID, false, 4, 1);
}

void MAI_Diablo(int i)
{
	MAI_RoundRanged(i, MIS_DIABAPOCA, false, 40, 0);
}

void MAI_RR2(int i, missile_id mistype, int dam)
{
	MonsterStruct *Monst;
	int mx, my, fx, fy;
	int dist, v;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	mx = Monst->position.tile.x - Monst->enemyPosition.x;
	my = Monst->position.tile.y - Monst->enemyPosition.y;
	if (abs(mx) >= 5 || abs(my) >= 5) {
		MAI_SkelSd(i);
		return;
	}

	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->enemyPosition.x;
		fy = Monst->enemyPosition.y;
		mx = Monst->position.tile.x - fx;
		my = Monst->position.tile.y - fy;
		Direction md = GetDirection(Monst->position.tile, Monst->position.last);
		if (Monst->_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		v = GenerateRnd(100);
		dist = std::max(abs(mx), abs(my));
		if (dist >= 2 && Monst->_msquelch == UINT8_MAX && dTransVal[Monst->position.tile.x][Monst->position.tile.y] == dTransVal[fx][fy]) {
			if (Monst->_mgoal == MGOAL_MOVE || dist >= 3) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = GenerateRnd(2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				Monst->_mgoalvar3 = 4;
				if (Monst->_mgoalvar1++ < 2 * dist || !DirOK(i, md)) {
					if (v < 5 * (Monst->_mint + 16))
						M_RoundWalk(i, md, &Monst->_mgoalvar2);
				} else
					Monst->_mgoal = MGOAL_NORMAL;
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (((dist >= 3 && v < 5 * (Monst->_mint + 2)) || v < 5 * (Monst->_mint + 1) || Monst->_mgoalvar3 == 4) && LineClearMissile(Monst->position.tile, { fx, fy })) {
				M_StartRSpAttack(i, mistype, dam);
			} else if (dist >= 2) {
				v = GenerateRnd(100);
				if (v < 2 * (5 * Monst->_mint + 25)
				    || ((Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
				        && Monst->_mVar2 == 0
				        && v < 2 * (5 * Monst->_mint + 40))) {
					M_CallWalk(i, md);
				}
			} else {
				if (GenerateRnd(100) < 10 * (Monst->_mint + 4)) {
					Monst->_mdir = md;
					if (GenerateRnd(2) != 0)
						M_StartAttack(i);
					else
						M_StartRSpAttack(i, mistype, dam);
				}
			}
			Monst->_mgoalvar3 = 1;
		}
		if (Monst->_mmode == MM_STAND) {
			M_StartDelay(i, GenerateRnd(10) + 5);
		}
	}
}

void MAI_Mega(int i)
{
	MAI_RR2(i, MIS_FLAMEC, 0);
}

void MAI_Golum(int i)
{
	assurance((DWORD)i < MAXMONSTERS, i);

	MonsterStruct *Monst = &monster[i];
	if (Monst->position.tile.x == 1 && Monst->position.tile.y == 0) {
		return;
	}

	if (Monst->_mmode == MM_DEATH
	    || Monst->_mmode == MM_SPSTAND
	    || (Monst->_mmode >= MM_WALK && Monst->_mmode <= MM_WALK3)) {
		return;
	}

	if ((Monst->_mFlags & MFLAG_TARGETS_MONSTER) == 0)
		M_Enemy(i);

	bool have_enemy = (monster[i]._mFlags & MFLAG_NO_ENEMY) == 0;

	if (Monst->_mmode == MM_ATTACK) {
		return;
	}

	int menemy = monster[i]._menemy;

	int mex = monster[i].position.tile.x - monster[menemy].position.future.x;
	int mey = monster[i].position.tile.y - monster[menemy].position.future.y;
	Direction md = GetDirection(monster[i].position.tile, monster[menemy].position.tile);
	monster[i]._mdir = md;
	if (abs(mex) < 2 && abs(mey) < 2 && have_enemy) {
		menemy = monster[i]._menemy;
		monster[i].enemyPosition = monster[menemy].position.tile;
		if (monster[menemy]._msquelch == 0) {
			monster[menemy]._msquelch = UINT8_MAX;
			monster[monster[i]._menemy].position.last = monster[i].position.tile;
			for (int j = 0; j < 5; j++) {
				for (int k = 0; k < 5; k++) {
					menemy = dMonster[monster[i].position.tile.x + k - 2][monster[i].position.tile.y + j - 2];
					if (menemy > 0)
						monster[menemy - 1]._msquelch = UINT8_MAX; // BUGFIX: should be `monster[_menemy-1]`, not monster[_menemy]. (fixed)
				}
			}
		}
		M_StartAttack(i);
		return;
	}

	if (have_enemy && MAI_Path(i))
		return;

	monster[i]._pathcount++;
	if (monster[i]._pathcount > 8)
		monster[i]._pathcount = 5;

	bool ok = M_CallWalk(i, plr[i]._pdir);
	if (ok)
		return;

	md = left[md];
	for (int j = 0; j < 8 && !ok; j++) {
		md = right[md];
		ok = DirOK(i, md);
	}
	if (ok)
		M_WalkDir(i, md);
}

void MAI_SkelKing(int i)
{
	MonsterStruct *Monst;
	int mx, my, fx, fy;
	int dist, v;

	assurance((DWORD)i < MAXMONSTERS, i);
	Monst = &monster[i];
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->enemyPosition.x;
		fy = Monst->enemyPosition.y;
		mx = Monst->position.tile.x - fx;
		my = Monst->position.tile.y - fy;
		Direction md = GetDirection(Monst->position.tile, Monst->position.last);
		if (Monst->_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		v = GenerateRnd(100);
		dist = std::max(abs(mx), abs(my));
		if (dist >= 2 && Monst->_msquelch == UINT8_MAX && dTransVal[Monst->position.tile.x][Monst->position.tile.y] == dTransVal[fx][fy]) {
			if (Monst->_mgoal == MGOAL_MOVE || ((abs(mx) >= 3 || abs(my) >= 3) && GenerateRnd(4) == 0)) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = GenerateRnd(2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				if ((Monst->_mgoalvar1++ >= 2 * dist && DirOK(i, md)) || dTransVal[Monst->position.tile.x][Monst->position.tile.y] != dTransVal[fx][fy]) {
					Monst->_mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(i, md, &Monst->_mgoalvar2)) {
					M_StartDelay(i, GenerateRnd(10) + 10);
				}
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (!gbIsMultiplayer
			    && ((dist >= 3 && v < 4 * Monst->_mint + 35) || v < 6)
			    && LineClearMissile(Monst->position.tile, { fx, fy })) {
				Point newPosition = Monst->position.tile + md;
				if (PosOkMonst(i, newPosition) && nummonsters < MAXMONSTERS) {
					M_SpawnSkel(newPosition, md);
					M_StartSpStand(i, md);
				}
			} else {
				if (dist >= 2) {
					v = GenerateRnd(100);
					if (v >= Monst->_mint + 25
					    && ((Monst->_mVar1 != MM_WALK && Monst->_mVar1 != MM_WALK2 && Monst->_mVar1 != MM_WALK3) || Monst->_mVar2 != 0 || (v >= Monst->_mint + 75))) {
						M_StartDelay(i, GenerateRnd(10) + 10);
					} else {
						M_CallWalk(i, md);
					}
				} else if (v < Monst->_mint + 20) {
					Monst->_mdir = md;
					M_StartAttack(i);
				}
			}
		}

		Monst->CheckStandAnimationIsLoaded(md);
	}
}

void MAI_Rhino(int i)
{
	MonsterStruct *Monst;
	int mx, my, fx, fy;
	int v, dist;

	assurance((DWORD)i < MAXMONSTERS, i);
	Monst = &monster[i];
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->enemyPosition.x;
		fy = Monst->enemyPosition.y;
		mx = Monst->position.tile.x - fx;
		my = Monst->position.tile.y - fy;
		Direction md = GetDirection(Monst->position.tile, Monst->position.last);
		if (Monst->_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		v = GenerateRnd(100);
		dist = std::max(abs(mx), abs(my));
		if (dist >= 2) {
			if (Monst->_mgoal == MGOAL_MOVE || (dist >= 5 && GenerateRnd(4) != 0)) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = GenerateRnd(2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				if (Monst->_mgoalvar1++ >= 2 * dist || dTransVal[Monst->position.tile.x][Monst->position.tile.y] != dTransVal[fx][fy]) {
					Monst->_mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(i, md, &Monst->_mgoalvar2)) {
					M_StartDelay(i, GenerateRnd(10) + 10);
				}
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (dist >= 5
			    && v < 2 * Monst->_mint + 43
			    && LineClear(PosOkMonst, i, Monst->position.tile, { fx, fy })) {
				if (AddMissile(Monst->position.tile, { fx, fy }, md, MIS_RHINO, Monst->_menemy, i, 0, 0) != -1) {
					if (Monst->MData->snd_special)
						PlayEffect(i, 3);
					dMonster[Monst->position.tile.x][Monst->position.tile.y] = -(i + 1);
					Monst->_mmode = MM_CHARGE;
				}
			} else {
				if (dist >= 2) {
					v = GenerateRnd(100);
					if (v >= 2 * Monst->_mint + 33
					    && ((Monst->_mVar1 != MM_WALK && Monst->_mVar1 != MM_WALK2 && Monst->_mVar1 != MM_WALK3)
					        || Monst->_mVar2 != 0
					        || v >= 2 * Monst->_mint + 83)) {
						M_StartDelay(i, GenerateRnd(10) + 10);
					} else {
						M_CallWalk(i, md);
					}
				} else if (v < 2 * Monst->_mint + 28) {
					Monst->_mdir = md;
					M_StartAttack(i);
				}
			}
		}

		Monst->CheckStandAnimationIsLoaded(Monst->_mdir);
	}
}

void MAI_HorkDemon(int i)
{
	MonsterStruct *Monst;
	int fx, fy, mx, my, v, dist;

	if ((DWORD)i >= MAXMONSTERS) {
		return;
	}

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	fx = Monst->enemyPosition.x;
	fy = Monst->enemyPosition.y;
	mx = Monst->position.tile.x - fx;
	my = Monst->position.tile.y - fy;
	Direction md = GetDirection(Monst->position.tile, Monst->position.last);

	if (Monst->_msquelch < 255) {
		MonstCheckDoors(i);
	}

	v = GenerateRnd(100);

	if (abs(mx) < 2 && abs(my) < 2) {
		Monst->_mgoal = MGOAL_NORMAL;
	} else if (Monst->_mgoal == 4 || ((abs(mx) >= 5 || abs(my) >= 5) && GenerateRnd(4) != 0)) {
		if (Monst->_mgoal != 4) {
			Monst->_mgoalvar1 = 0;
			Monst->_mgoalvar2 = GenerateRnd(2);
		}
		Monst->_mgoal = MGOAL_MOVE;
		if (abs(mx) > abs(my)) {
			dist = abs(mx);
		} else {
			dist = abs(my);
		}
		if (Monst->_mgoalvar1++ >= 2 * dist || dTransVal[Monst->position.tile.x][Monst->position.tile.y] != dTransVal[fx][fy]) {
			Monst->_mgoal = MGOAL_NORMAL;
		} else if (!M_RoundWalk(i, md, &Monst->_mgoalvar2)) {
			M_StartDelay(i, GenerateRnd(10) + 10);
		}
	}

	if (Monst->_mgoal == 1) {
		if ((abs(mx) >= 3 || abs(my) >= 3) && v < 2 * Monst->_mint + 43) {
			Point position = Monst->position.tile + Monst->_mdir;
			if (PosOkMonst(i, position) && nummonsters < MAXMONSTERS) {
				M_StartRSpAttack(i, MIS_HORKDMN, 0);
			}
		} else if (abs(mx) < 2 && abs(my) < 2) {
			if (v < 2 * Monst->_mint + 28) {
				Monst->_mdir = md;
				M_StartAttack(i);
			}
		} else {
			v = GenerateRnd(100);
			if (v < 2 * Monst->_mint + 33
			    || ((Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3) && Monst->_mVar2 == 0 && v < 2 * Monst->_mint + 83)) {
				M_CallWalk(i, md);
			} else {
				M_StartDelay(i, GenerateRnd(10) + 10);
			}
		}
	}

	Monst->CheckStandAnimationIsLoaded(Monst->_mdir);
}

void MAI_Counselor(int i)
{
	int mx, my, fx, fy;
	int dist, v;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->enemyPosition.x;
		fy = Monst->enemyPosition.y;
		mx = Monst->position.tile.x - fx;
		my = Monst->position.tile.y - fy;
		Direction md = GetDirection(Monst->position.tile, Monst->position.last);
		if (Monst->_msquelch < UINT8_MAX)
			MonstCheckDoors(i);
		v = GenerateRnd(100);
		if (Monst->_mgoal == MGOAL_RETREAT) {
			if (Monst->_mgoalvar1++ <= 3)
				M_CallWalk(i, opposite[md]);
			else {
				Monst->_mgoal = MGOAL_NORMAL;
				M_StartFadein(i, md, true);
			}
		} else if (Monst->_mgoal == MGOAL_MOVE) {
			dist = std::max(abs(mx), abs(my));
			if (dist >= 2 && Monst->_msquelch == UINT8_MAX && dTransVal[Monst->position.tile.x][Monst->position.tile.y] == dTransVal[fx][fy]) {
				if (Monst->_mgoalvar1++ < 2 * dist || !DirOK(i, md)) {
					M_RoundWalk(i, md, &Monst->_mgoalvar2);
				} else {
					Monst->_mgoal = MGOAL_NORMAL;
					M_StartFadein(i, md, true);
				}
			} else {
				Monst->_mgoal = MGOAL_NORMAL;
				M_StartFadein(i, md, true);
			}
		} else if (Monst->_mgoal == MGOAL_NORMAL) {
			if (abs(mx) >= 2 || abs(my) >= 2) {
				if (v < 5 * (Monst->_mint + 10) && LineClearMissile(Monst->position.tile, { fx, fy })) {
					constexpr missile_id counsmiss[4] = { MIS_FIREBOLT, MIS_CBOLT, MIS_LIGHTCTRL, MIS_FIREBALL };
					M_StartRAttack(i, counsmiss[Monst->_mint], Monst->mMinDamage + GenerateRnd(Monst->mMaxDamage - Monst->mMinDamage + 1));
				} else if (GenerateRnd(100) < 30) {
					Monst->_mgoal = MGOAL_MOVE;
					Monst->_mgoalvar1 = 0;
					M_StartFadeout(i, md, false);
				} else
					M_StartDelay(i, GenerateRnd(10) + 2 * (5 - Monst->_mint));
			} else {
				Monst->_mdir = md;
				if (Monst->_mhitpoints < (Monst->_mmaxhp / 2)) {
					Monst->_mgoal = MGOAL_RETREAT;
					Monst->_mgoalvar1 = 0;
					M_StartFadeout(i, md, false);
				} else if (Monst->_mVar1 == MM_DELAY
				    || GenerateRnd(100) < 2 * Monst->_mint + 20) {
					M_StartRAttack(i, MIS_NULL, 0);
					AddMissile(Monst->position.tile, { 0, 0 }, Monst->_mdir, MIS_FLASH, TARGET_PLAYERS, i, 4, 0);
					AddMissile(Monst->position.tile, { 0, 0 }, Monst->_mdir, MIS_FLASH2, TARGET_PLAYERS, i, 4, 0);
				} else
					M_StartDelay(i, GenerateRnd(10) + 2 * (5 - Monst->_mint));
			}
		}
		if (Monst->_mmode == MM_STAND) {
			M_StartDelay(i, GenerateRnd(10) + 5);
		}
	}
}

void MAI_Garbud(int i)
{
	int _mx, _my;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND) {
		return;
	}

	_mx = Monst->position.tile.x;
	_my = Monst->position.tile.y;
	Direction md = M_GetDir(i);

	if (Monst->mtalkmsg >= TEXT_GARBUD1
	    && Monst->mtalkmsg <= TEXT_GARBUD3
	    && (dFlags[_mx][_my] & BFLAG_VISIBLE) == 0
	    && Monst->_mgoal == MGOAL_TALKING) {
		Monst->_mgoal = MGOAL_INQUIRING;
		switch (Monst->mtalkmsg) {
		case TEXT_GARBUD1:
			Monst->mtalkmsg = TEXT_GARBUD2;
			break;
		case TEXT_GARBUD2:
			Monst->mtalkmsg = TEXT_GARBUD3;
			break;
		case TEXT_GARBUD3:
			Monst->mtalkmsg = TEXT_GARBUD4;
			break;
		default:
			break;
		}
	}

	if ((dFlags[_mx][_my] & BFLAG_VISIBLE) != 0) {
		if (Monst->mtalkmsg == TEXT_GARBUD4) {
			if (!effect_is_playing(USFX_GARBUD4) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->_mgoal = MGOAL_NORMAL;
				Monst->_msquelch = UINT8_MAX;
				Monst->mtalkmsg = TEXT_NONE;
			}
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_MOVE)
		MAI_Round(i, true);

	monster[i]._mdir = md;

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_Zhar(int i)
{
	int mx, my;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->position.tile.x;
	my = Monst->position.tile.y;
	Direction md = M_GetDir(i);
	if (Monst->mtalkmsg == TEXT_ZHAR1 && (dFlags[mx][my] & BFLAG_VISIBLE) == 0 && Monst->_mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = TEXT_ZHAR2;
		Monst->_mgoal = MGOAL_INQUIRING;
	}

	if ((dFlags[mx][my] & BFLAG_VISIBLE) != 0) {
		if (Monst->mtalkmsg == TEXT_ZHAR2) {
			if (!effect_is_playing(USFX_ZHAR2) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->_msquelch = UINT8_MAX;
				Monst->mtalkmsg = TEXT_NONE;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT || Monst->_mgoal == MGOAL_MOVE)
		MAI_Counselor(i);

	Monst->_mdir = md;

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_SnotSpil(int i)
{
	int mx, my;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->position.tile.x;
	my = Monst->position.tile.y;
	Direction md = M_GetDir(i);

	if (Monst->mtalkmsg == TEXT_BANNER10 && (dFlags[mx][my] & BFLAG_VISIBLE) == 0 && Monst->_mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = TEXT_BANNER11;
		Monst->_mgoal = MGOAL_INQUIRING;
	}

	if (Monst->mtalkmsg == TEXT_BANNER11 && quests[Q_LTBANNER]._qvar1 == 3) {
		Monst->mtalkmsg = TEXT_NONE;
		Monst->_mgoal = MGOAL_NORMAL;
	}

	if ((dFlags[mx][my] & BFLAG_VISIBLE) != 0) {
		if (Monst->mtalkmsg == TEXT_BANNER12) {
			if (!effect_is_playing(USFX_SNOT3) && Monst->_mgoal == MGOAL_TALKING) {
				ObjChangeMap(setpc_x, setpc_y, setpc_x + setpc_w + 1, setpc_y + setpc_h + 1);
				quests[Q_LTBANNER]._qvar1 = 3;
				RedoPlayerVision();
				Monst->_msquelch = UINT8_MAX;
				Monst->mtalkmsg = TEXT_NONE;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		}
		if (quests[Q_LTBANNER]._qvar1 == 3) {
			if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_ATTACK2)
				MAI_Fallen(i);
		}
	}

	Monst->_mdir = md;

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_Lazurus(int i)
{
	int mx, my;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->position.tile.x;
	my = Monst->position.tile.y;
	Direction md = M_GetDir(i);
	if ((dFlags[mx][my] & BFLAG_VISIBLE) != 0) {
		if (!gbIsMultiplayer) {
			if (Monst->mtalkmsg == TEXT_VILE13 && Monst->_mgoal == MGOAL_INQUIRING && plr[myplr].position.tile.x == 35 && plr[myplr].position.tile.y == 46) {
				PlayInGameMovie("gendata\\fprst3.smk");
				Monst->_mmode = MM_TALK;
				quests[Q_BETRAYER]._qvar1 = 5;
			}

			if (Monst->mtalkmsg == TEXT_VILE13 && !effect_is_playing(USFX_LAZ1) && Monst->_mgoal == MGOAL_TALKING) {
				ObjChangeMapResync(1, 18, 20, 24);
				RedoPlayerVision();
				quests[Q_BETRAYER]._qvar1 = 6;
				Monst->_mgoal = MGOAL_NORMAL;
				Monst->_msquelch = UINT8_MAX;
				Monst->mtalkmsg = TEXT_NONE;
			}
		}

		if (gbIsMultiplayer && Monst->mtalkmsg == TEXT_VILE13 && Monst->_mgoal == MGOAL_INQUIRING && quests[Q_BETRAYER]._qvar1 <= 3) {
			Monst->_mmode = MM_TALK;
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT || Monst->_mgoal == MGOAL_MOVE) {
		if (!gbIsMultiplayer && quests[Q_BETRAYER]._qvar1 == 4 && Monst->mtalkmsg == TEXT_NONE) { // Fix save games affected by teleport bug
			ObjChangeMapResync(1, 18, 20, 24);
			RedoPlayerVision();
			quests[Q_BETRAYER]._qvar1 = 6;
		}
		Monst->mtalkmsg = TEXT_NONE;
		MAI_Counselor(i);
	}

	Monst->_mdir = md;

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_Lazhelp(int i)
{
	int _mx, _my;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);
	if (monster[i]._mmode != MM_STAND)
		return;

	Monst = &monster[i];
	_mx = Monst->position.tile.x;
	_my = Monst->position.tile.y;
	Direction md = M_GetDir(i);

	if ((dFlags[_mx][_my] & BFLAG_VISIBLE) != 0) {
		if (!gbIsMultiplayer) {
			if (quests[Q_BETRAYER]._qvar1 <= 5) {
				Monst->_mgoal = MGOAL_INQUIRING;
			} else {
				Monst->_mgoal = MGOAL_NORMAL;
				Monst->mtalkmsg = TEXT_NONE;
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
	}
	if (Monst->_mgoal == MGOAL_NORMAL)
		MAI_Succ(i);
	Monst->_mdir = md;

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_Lachdanan(int i)
{
	int _mx, _my;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	_mx = Monst->position.tile.x;
	_my = Monst->position.tile.y;
	Direction md = M_GetDir(i);

	if (Monst->mtalkmsg == TEXT_VEIL9 && (dFlags[_mx][_my] & BFLAG_VISIBLE) == 0 && monster[i]._mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = TEXT_VEIL10;
		monster[i]._mgoal = MGOAL_INQUIRING;
	}

	if ((dFlags[_mx][_my] & BFLAG_VISIBLE) != 0) {
		if (Monst->mtalkmsg == TEXT_VEIL11) {
			if (!effect_is_playing(USFX_LACH3) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->mtalkmsg = TEXT_NONE;
				quests[Q_VEIL]._qactive = QUEST_DONE;
				M_StartKill(i, -1);
			}
		}
	}

	Monst->_mdir = md;

	Monst->CheckStandAnimationIsLoaded(md);
}

void MAI_Warlord(int i)
{
	MonsterStruct *Monst;
	int mx, my;

	assurance((DWORD)i < MAXMONSTERS, i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->position.tile.x;
	my = Monst->position.tile.y;
	Direction md = M_GetDir(i);
	if ((dFlags[mx][my] & BFLAG_VISIBLE) != 0) {
		if (Monst->mtalkmsg == TEXT_WARLRD9 && Monst->_mgoal == MGOAL_INQUIRING)
			Monst->_mmode = MM_TALK;
		if (Monst->mtalkmsg == TEXT_WARLRD9 && !effect_is_playing(USFX_WARLRD1) && Monst->_mgoal == MGOAL_TALKING) {
			Monst->_msquelch = UINT8_MAX;
			Monst->mtalkmsg = TEXT_NONE;
			Monst->_mgoal = MGOAL_NORMAL;
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL)
		MAI_SkelSd(i);

	Monst->_mdir = md;

	Monst->CheckStandAnimationIsLoaded(Monst->_mdir);
}

void DeleteMonsterList()
{
	int i;
	for (i = 0; i < MAX_PLRS; i++) {
		if (monster[i]._mDelFlag) {
			monster[i].position.tile = { 1, 0 };
			monster[i].position.future = { 0, 0 };
			monster[i].position.old = { 0, 0 };
			monster[i]._mDelFlag = false;
		}
	}

	i = MAX_PLRS;
	while (i < nummonsters) {
		if (monster[monstactive[i]]._mDelFlag) {
			DeleteMonster(i);
			i = 0; // TODO: check if this should be MAX_PLRS.
		} else {
			i++;
		}
	}
}

void ProcessMonsters()
{
	int i, mi, mx, my, _menemy;
	bool raflag;
	MonsterStruct *Monst;

	DeleteMonsterList();

	assert((DWORD)nummonsters <= MAXMONSTERS);
	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		Monst = &monster[mi];
		raflag = false;
		if (gbIsMultiplayer) {
			SetRndSeed(Monst->_mAISeed);
			Monst->_mAISeed = AdvanceRndSeed();
		}
		if ((monster[mi]._mFlags & MFLAG_NOHEAL) == 0 && Monst->_mhitpoints < Monst->_mmaxhp && Monst->_mhitpoints >> 6 > 0) {
			if (Monst->mLevel > 1) {
				Monst->_mhitpoints += Monst->mLevel / 2;
			} else {
				Monst->_mhitpoints += Monst->mLevel;
			}
		}
		mx = Monst->position.tile.x;
		my = Monst->position.tile.y;

		if ((dFlags[mx][my] & BFLAG_VISIBLE) != 0 && Monst->_msquelch == 0) {
			if (Monst->MType->mtype == MT_CLEAVER) {
				PlaySFX(USFX_CLEAVER);
			}
			if (Monst->MType->mtype == MT_NAKRUL) {
				if (sgGameInitInfo.bCowQuest != 0) {
					PlaySFX(USFX_NAKRUL6);
				} else {
					if (IsUberRoomOpened)
						PlaySFX(USFX_NAKRUL4);
					else
						PlaySFX(USFX_NAKRUL5);
				}
			}
			if (Monst->MType->mtype == MT_DEFILER)
				PlaySFX(USFX_DEFILER8);
			M_Enemy(mi);
		}

		if ((Monst->_mFlags & MFLAG_TARGETS_MONSTER) != 0) {
			_menemy = Monst->_menemy;
			assurance((DWORD)_menemy < MAXMONSTERS, _menemy);
			Monst->position.last = monster[Monst->_menemy].position.future;
			Monst->enemyPosition = Monst->position.last;
		} else {
			_menemy = Monst->_menemy;
			assurance((DWORD)_menemy < MAX_PLRS, _menemy);
			Monst->enemyPosition = plr[Monst->_menemy].position.future;
			if ((dFlags[mx][my] & BFLAG_VISIBLE) != 0) {
				Monst->_msquelch = UINT8_MAX;
				Monst->position.last = plr[Monst->_menemy].position.future;
			} else if (Monst->_msquelch != 0 && Monst->MType->mtype != MT_DIABLO) { /// BUGFIX: change '_mAi' to 'MType->mtype'
				Monst->_msquelch--;
			}
		}
		do {
			if ((Monst->_mFlags & MFLAG_SEARCH) == 0) {
				AiProc[Monst->_mAi](mi);
			} else if (!MAI_Path(mi)) {
				AiProc[Monst->_mAi](mi);
			}
			switch (Monst->_mmode) {
			case MM_STAND:
				raflag = M_DoStand(mi);
				break;
			case MM_WALK:
			case MM_WALK2:
			case MM_WALK3:
				raflag = M_DoWalk(mi, Monst->_mmode);
				break;
			case MM_ATTACK:
				raflag = M_DoAttack(mi);
				break;
			case MM_GOTHIT:
				raflag = M_DoGotHit(mi);
				break;
			case MM_DEATH:
				raflag = M_DoDeath(mi);
				break;
			case MM_SATTACK:
				raflag = M_DoSAttack(mi);
				break;
			case MM_FADEIN:
				raflag = M_DoFadein(mi);
				break;
			case MM_FADEOUT:
				raflag = M_DoFadeout(mi);
				break;
			case MM_RATTACK:
				raflag = M_DoRAttack(mi);
				break;
			case MM_SPSTAND:
				raflag = M_DoSpStand(mi);
				break;
			case MM_RSPATTACK:
				raflag = M_DoRSpAttack(mi);
				break;
			case MM_DELAY:
				raflag = M_DoDelay(mi);
				break;
			case MM_CHARGE:
				raflag = false;
				break;
			case MM_STONE:
				raflag = M_DoStone(mi);
				break;
			case MM_HEAL:
				raflag = M_DoHeal(mi);
				break;
			case MM_TALK:
				raflag = M_DoTalk(mi);
				break;
			}
			if (raflag) {
				GroupUnity(mi);
			}
		} while (raflag);
		if (Monst->_mmode != MM_STONE) {
			Monst->AnimInfo.ProcessAnimation((Monst->_mFlags & MFLAG_LOCK_ANIMATION) != 0, (Monst->_mFlags & MFLAG_ALLOW_SPECIAL) != 0);
		}
	}

	DeleteMonsterList();
}

void FreeMonsters()
{
	int mtype;
	int i, j;

	for (i = 0; i < nummtypes; i++) {
		mtype = Monsters[i].mtype;
		for (j = 0; j < 6; j++) {
			if (animletter[j] != 's' || monsterdata[mtype].has_special) {
				Monsters[i].Anims[j].CMem = nullptr;
			}
		}
	}

	FreeMissiles2();
}

bool DirOK(int i, Direction mdir)
{
	commitment((DWORD)i < MAXMONSTERS, i);
	Point position = monster[i].position.tile;
	Point futurePosition = position + mdir;
	if (futurePosition.y < 0 || futurePosition.y >= MAXDUNY || futurePosition.x < 0 || futurePosition.x >= MAXDUNX || !PosOkMonst(i, futurePosition))
		return false;
	if (mdir == DIR_E) {
		if (SolidLoc(position + DIR_SE) || (dFlags[position.x + 1][position.y] & BFLAG_MONSTLR) != 0)
			return false;
	} else if (mdir == DIR_W) {
		if (SolidLoc(position + DIR_SW) || (dFlags[position.x][position.y + 1] & BFLAG_MONSTLR) != 0)
			return false;
	} else if (mdir == DIR_N) {
		if (SolidLoc(position + DIR_NE) || SolidLoc(position + DIR_NW))
			return false;
	} else if (mdir == DIR_S)
		if (SolidLoc(position + DIR_SW) || SolidLoc(position + DIR_SE))
			return false;
	if (monster[i].leaderflag == 1) {
		return futurePosition.WalkingDistance(monster[monster[i].leader].position.future) < 4;
	}
	if (monster[i]._uniqtype == 0 || (UniqMonst[monster[i]._uniqtype - 1].mUnqAttr & 2) == 0)
		return true;
	int mcount = 0;
	for (int x = futurePosition.x - 3; x <= futurePosition.x + 3; x++) {
		for (int y = futurePosition.y - 3; y <= futurePosition.y + 3; y++) {
			if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
				continue;
			int mi = dMonster[x][y];
			if (mi < 0)
				mi = -mi;
			if (mi != 0)
				mi--;
			// BUGFIX: should only run pack member check if mi was non-zero prior to executing the body of the above if-statement.
			if (monster[mi].leaderflag == 1
			    && monster[mi].leader == i
			    && monster[mi].position.future == Point { x, y }) {
				mcount++;
			}
		}
	}
	return mcount == monster[i].packsize;
}

bool PosOkMissile(int /*entity*/, Point position)
{
	return !nMissileTable[dPiece[position.x][position.y]] && (dFlags[position.x][position.y] & BFLAG_MONSTLR) == 0;
}

bool CheckNoSolid(int /*entity*/, Point position)
{
	return !nSolidTable[dPiece[position.x][position.y]];
}

bool LineClearSolid(Point startPoint, Point endPoint)
{
	return LineClear(CheckNoSolid, 0, startPoint, endPoint);
}

bool LineClearMissile(Point startPoint, Point endPoint)
{
	return LineClear(PosOkMissile, 0, startPoint, endPoint);
}

bool LineClear(bool (*Clear)(int, Point), int entity, Point startPoint, Point endPoint)
{
	int d;
	int xincD, yincD, dincD, dincH;
	bool done = false;

	Point position = startPoint;

	int dx = endPoint.x - position.x;
	int dy = endPoint.y - position.y;
	if (abs(dx) > abs(dy)) {
		if (dx < 0) {
			std::swap(position, endPoint);
			dx = -dx;
			dy = -dy;
		}
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
		while (!done && position != endPoint) {
			if ((d <= 0) ^ (yincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				position.y += yincD;
			}
			position.x++;
			done = position != startPoint && !Clear(entity, position);
		}
	} else {
		if (dy < 0) {
			std::swap(position, endPoint);
			dy = -dy;
			dx = -dx;
		}
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
		while (!done && position != endPoint) {
			if ((d <= 0) ^ (xincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				position.x += xincD;
			}
			position.y++;
			done = position != startPoint && !Clear(entity, position);
		}
	}
	return position == endPoint;
}

void SyncMonsterAnim(int i)
{
	int _mdir;

	assurance((DWORD)i < MAXMONSTERS || i < 0, i);
	monster[i].MType = &Monsters[monster[i]._mMTidx];
	monster[i].MData = Monsters[monster[i]._mMTidx].MData;
	if (monster[i]._uniqtype != 0)
		monster[i].mName = _(UniqMonst[monster[i]._uniqtype - 1].mName);
	else
		monster[i].mName = _(monster[i].MData->mName);
	_mdir = monster[i]._mdir;

	int graphic = MA_STAND;

	switch (monster[i]._mmode) {
	case MM_STAND:
	case MM_DELAY:
	case MM_TALK:
		break;
	case MM_WALK:
	case MM_WALK2:
	case MM_WALK3:
		graphic = MA_WALK;
		break;
	case MM_ATTACK:
	case MM_RATTACK:
		graphic = MA_ATTACK;
		break;
	case MM_GOTHIT:
		graphic = MA_GOTHIT;
		break;
	case MM_DEATH:
		graphic = MA_DEATH;
		break;
	case MM_SATTACK:
	case MM_FADEIN:
	case MM_FADEOUT:
	case MM_SPSTAND:
	case MM_RSPATTACK:
	case MM_HEAL:
		graphic = MA_SPECIAL;
		break;
	case MM_CHARGE:
		graphic = MA_ATTACK;
		monster[i].AnimInfo.CurrentFrame = 1;
		monster[i].AnimInfo.NumberOfFrames = monster[i].MType->Anims[MA_ATTACK].Frames;
		break;
	default:
		monster[i].AnimInfo.CurrentFrame = 1;
		monster[i].AnimInfo.NumberOfFrames = monster[i].MType->Anims[MA_STAND].Frames;
		break;
	}

	if (monster[i].MType->Anims[graphic].CelSpritesForDirections[_mdir])
		monster[i].AnimInfo.pCelSprite = &*monster[i].MType->Anims[graphic].CelSpritesForDirections[_mdir];
	else
		monster[i].AnimInfo.pCelSprite = nullptr;
}

void M_FallenFear(Point position)
{
	MonsterStruct *m;
	int i, rundist;

	for (i = 0; i < nummonsters; i++) {
		m = &monster[monstactive[i]];

		switch (m->MType->mtype) {
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
		if (m->_mAi == AI_FALLEN
		    && position.WalkingDistance(m->position.tile) < 5
		    && m->_mhitpoints >> 6 > 0) {
			m->_mgoal = MGOAL_RETREAT;
			m->_mgoalvar1 = rundist;
			m->_mdir = GetDirection(position, m->position.tile);
		}
	}
}

const char *GetMonsterTypeText(const MonsterData &monsterData)
{
	switch (monsterData.mMonstClass) {
	case MC_ANIMAL:
		return _("Animal");
	case MC_DEMON:
		return _("Demon");
	case MC_UNDEAD:
		return _("Undead");
	}

	app_fatal("Unknown mMonstClass %i", monsterData.mMonstClass);
}

void PrintMonstHistory(int mt)
{
	int minHP, maxHP, res;

	if (sgOptions.Gameplay.bShowMonsterType) {
		strcpy(tempstr, fmt::format(_("Type: {:s}  Kills: {:d}"), GetMonsterTypeText(monsterdata[mt]), monstkills[mt]).c_str());
	} else {
		strcpy(tempstr, fmt::format(_("Total kills: {:d}"), monstkills[mt]).c_str());
	}

	AddPanelString(tempstr);
	if (monstkills[mt] >= 30) {
		minHP = monsterdata[mt].mMinHP;
		maxHP = monsterdata[mt].mMaxHP;
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
	if (monstkills[mt] >= 15) {
		if (sgGameInitInfo.nDifficulty != DIFF_HELL)
			res = monsterdata[mt].mMagicRes;
		else
			res = monsterdata[mt].mMagicRes2;
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
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr);
			}
			if ((res & (IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING)) != 0) {
				strcpy(tempstr, _("Immune: "));
				if ((res & IMMUNE_MAGIC) != 0)
					strcat(tempstr, _("Magic "));
				if ((res & IMMUNE_FIRE) != 0)
					strcat(tempstr, _("Fire "));
				if ((res & IMMUNE_LIGHTNING) != 0)
					strcat(tempstr, _("Lightning "));
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr);
			}
		}
	}
	pinfoflag = true;
}

void PrintUniqueHistory()
{
	int res;

	if (sgOptions.Gameplay.bShowMonsterType) {
		strcpy(tempstr, fmt::format(_("Type: {:s}"), GetMonsterTypeText(*monster[pcursmonst].MData)).c_str());
		AddPanelString(tempstr);
	}

	res = monster[pcursmonst].mMagicRes & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING);
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
	pinfoflag = true;
}

void MissToMonst(int i, Point position)
{
	int m, pnum;
	MissileStruct *Miss;
	MonsterStruct *Monst;

	assurance((DWORD)i < MAXMISSILES, i);

	Miss = &missile[i];
	m = Miss->_misource;

	assurance((DWORD)m < MAXMONSTERS, m);

	Monst = &monster[m];
	Point oldPosition = Miss->position.tile;
	dMonster[position.x][position.y] = m + 1;
	Monst->_mdir = static_cast<Direction>(Miss->_mimfnum);
	Monst->position.tile = position;
	M_StartStand(m, Monst->_mdir);
	if (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN) {
		if ((Monst->_mFlags & MFLAG_TARGETS_MONSTER) == 0)
			M_StartHit(m, -1, 0);
		else
			M2MStartHit(m, -1, 0);
	} else {
		M_StartFadein(m, Monst->_mdir, false);
	}

	Point newPosition = {};
	if ((Monst->_mFlags & MFLAG_TARGETS_MONSTER) == 0) {
		pnum = dPlayer[oldPosition.x][oldPosition.y] - 1;
		if (dPlayer[oldPosition.x][oldPosition.y] > 0) {
			if (Monst->MType->mtype != MT_GLOOM && (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN)) {
				M_TryH2HHit(m, dPlayer[oldPosition.x][oldPosition.y] - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (pnum == dPlayer[oldPosition.x][oldPosition.y] - 1 && (Monst->MType->mtype < MT_NSNAKE || Monst->MType->mtype > MT_GSNAKE)) {
					if (plr[pnum]._pmode != PM_GOTHIT && plr[pnum]._pmode != PM_DEATH)
						StartPlrHit(pnum, 0, true);
					newPosition = oldPosition + Monst->_mdir;
					if (PosOkPlayer(pnum, newPosition)) {
						plr[pnum].position.tile = newPosition;
						FixPlayerLocation(pnum, plr[pnum]._pdir);
						FixPlrWalkTags(pnum);
						dPlayer[newPosition.x][newPosition.y] = pnum + 1;
						SetPlayerOld(plr[pnum]);
					}
				}
			}
		}
	} else {
		if (dMonster[oldPosition.x][oldPosition.y] > 0) {
			if (Monst->MType->mtype != MT_GLOOM && (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN)) {
				M_TryM2MHit(m, dMonster[oldPosition.x][oldPosition.y] - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (Monst->MType->mtype < MT_NSNAKE || Monst->MType->mtype > MT_GSNAKE) {
					newPosition = oldPosition + Monst->_mdir;
					if (PosOkMonst(dMonster[oldPosition.x][oldPosition.y] - 1, newPosition)) {
						m = dMonster[oldPosition.x][oldPosition.y];
						dMonster[newPosition.x][newPosition.y] = m;
						dMonster[oldPosition.x][oldPosition.y] = 0;
						m--;
						monster[m].position.tile = newPosition;
						monster[m].position.future = newPosition;
					}
				}
			}
		}
	}
}

bool PosOkMonst(int i, Point position)
{
	int oi;
	bool ret;

	ret = !SolidLoc(position) && dPlayer[position.x][position.y] == 0 && dMonster[position.x][position.y] == 0;
	if (ret && dObject[position.x][position.y] != 0) {
		oi = dObject[position.x][position.y] > 0 ? dObject[position.x][position.y] - 1 : -(dObject[position.x][position.y] + 1);
		if (object[oi]._oSolidFlag)
			ret = false;
	}
	if (ret)
		ret = monster_posok(i, position);

	return ret;
}

bool monster_posok(int i, Point position)
{
	int mi, j;
	bool ret, fire, lightning;

	ret = true;
	mi = dMissile[position.x][position.y];
	if (mi != 0 && i >= 0) {
		fire = false;
		lightning = false;
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				fire = true;
			} else if (missile[mi - 1]._mitype == MIS_LIGHTWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (fixed)
				lightning = true;
			}
		} else {
			for (j = 0; j < nummissiles; j++) {
				mi = missileactive[j];
				if (missile[mi].position.tile == position) {
					if (missile[mi]._mitype == MIS_FIREWALL) {
						fire = true;
						break;
					}
					if (missile[mi]._mitype == MIS_LIGHTWALL) {
						lightning = true;
						break;
					}
				}
			}
		}
		if (fire && ((monster[i].mMagicRes & IMMUNE_FIRE) == 0 || monster[i].MType->mtype == MT_DIABLO))
			ret = false;
		if (lightning && ((monster[i].mMagicRes & IMMUNE_LIGHTNING) == 0 || monster[i].MType->mtype == MT_DIABLO))
			ret = false;
	}

	return ret;
}

bool PosOkMonst2(int i, Point position)
{
	int oi;
	bool ret;

	ret = !SolidLoc(position);
	if (ret && dObject[position.x][position.y] != 0) {
		oi = dObject[position.x][position.y] > 0 ? dObject[position.x][position.y] - 1 : -(dObject[position.x][position.y] + 1);
		if (object[oi]._oSolidFlag)
			ret = false;
	}
	if (ret)
		ret = monster_posok(i, position);

	return ret;
}

bool PosOkMonst3(int i, Point position)
{
	int oi, objtype;
	bool ret, isdoor;

	ret = true;
	isdoor = false;

	if (ret && dObject[position.x][position.y] != 0) {
		oi = dObject[position.x][position.y] > 0 ? dObject[position.x][position.y] - 1 : -(dObject[position.x][position.y] + 1);
		objtype = object[oi]._otype;
		isdoor = IsAnyOf(objtype, OBJ_L1LDOOR, OBJ_L1RDOOR, OBJ_L2LDOOR, OBJ_L2RDOOR, OBJ_L3LDOOR, OBJ_L3RDOOR);
		if (object[oi]._oSolidFlag && !isdoor) {
			ret = false;
		}
	}
	if (ret) {
		ret = (!SolidLoc(position) || isdoor) && dPlayer[position.x][position.y] == 0 && dMonster[position.x][position.y] == 0;
	}
	if (ret)
		ret = monster_posok(i, position);

	return ret;
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

int M_SpawnSkel(Point position, Direction dir)
{
	int i, j, skeltypes, skel;

	j = 0;
	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(Monsters[i].mtype))
			j++;
	}

	if (j != 0) {
		skeltypes = GenerateRnd(j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(Monsters[i].mtype))
				j++;
		}
		skel = AddMonster(position, dir, i - 1, true);
		if (skel != -1)
			M_StartSpStand(skel, dir);

		return skel;
	}

	return -1;
}

void ActivateSpawn(int i, int x, int y, Direction dir)
{
	dMonster[x][y] = i + 1;
	monster[i].position.tile = { x, y };
	monster[i].position.future = { x, y };
	monster[i].position.old = { x, y };
	M_StartSpStand(i, dir);
}

bool SpawnSkeleton(int ii, Point position)
{
	int dx, dy, xx, yy, j, k, rs;
	bool monstok[3][3];

	if (ii == -1)
		return false;

	if (PosOkMonst(-1, position)) {
		Direction dir = GetDirection(position, position); // TODO useless calculation
		ActivateSpawn(ii, position.x, position.y, dir);
		return true;
	}

	bool savail = false;
	yy = 0;
	for (j = position.y - 1; j <= position.y + 1; j++) {
		xx = 0;
		for (k = position.x - 1; k <= position.x + 1; k++) {
			monstok[xx][yy] = PosOkMonst(-1, { k, j });
			savail = savail || monstok[xx][yy];
			xx++;
		}
		yy++;
	}
	if (!savail) {
		return false;
	}

	rs = GenerateRnd(15) + 1;
	xx = 0;
	yy = 0;
	while (rs > 0) {
		if (monstok[xx][yy])
			rs--;
		if (rs > 0) {
			xx++;
			if (xx == 3) {
				xx = 0;
				yy++;
				if (yy == 3)
					yy = 0;
			}
		}
	}

	dx = position.x - 1 + xx;
	dy = position.y - 1 + yy;
	Direction dir = GetDirection({ dx, dy }, position);
	ActivateSpawn(ii, dx, dy, dir);

	return true;
}

int PreSpawnSkeleton()
{
	int i, j, skeltypes, skel;

	j = 0;

	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(Monsters[i].mtype))
			j++;
	}

	if (j != 0) {
		skeltypes = GenerateRnd(j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(Monsters[i].mtype))
				j++;
		}
		skel = AddMonster({ 0, 0 }, DIR_S, i - 1, false);
		if (skel != -1)
			M_StartStand(skel, DIR_S);

		return skel;
	}

	return -1;
}

void TalktoMonster(int i)
{
	assurance((DWORD)i < MAXMONSTERS, i);

	MonsterStruct *Monst = &monster[i];
	int pnum = Monst->_menemy;
	Monst->_mmode = MM_TALK;
	if (Monst->_mAi == AI_SNOTSPIL || Monst->_mAi == AI_LACHDAN) {
		if (QuestStatus(Q_LTBANNER) && quests[Q_LTBANNER]._qvar1 == 2) {
			if (plr[pnum].TryRemoveInvItemById(IDI_BANNER)) {
				quests[Q_LTBANNER]._qactive = QUEST_DONE;
				Monst->mtalkmsg = TEXT_BANNER12;
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		}
		if (QuestStatus(Q_VEIL) && Monst->mtalkmsg >= TEXT_VEIL9) {
			if (plr[pnum].TryRemoveInvItemById(IDI_GLDNELIX)) {
				Monst->mtalkmsg = TEXT_VEIL11;
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		}
	}
}

void SpawnGolum(int i, Point position, int mi)
{
	assurance((DWORD)i < MAXMONSTERS, i);

	dMonster[position.x][position.y] = i + 1;
	monster[i].position.tile = position;
	monster[i].position.future = position;
	monster[i].position.old = position;
	monster[i]._pathcount = 0;
	monster[i]._mmaxhp = 2 * (320 * missile[mi]._mispllvl + plr[i]._pMaxMana / 3);
	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i].mArmorClass = 25;
	monster[i].mHit = 5 * (missile[mi]._mispllvl + 8) + 2 * plr[i]._pLevel;
	monster[i].mMinDamage = 2 * (missile[mi]._mispllvl + 4);
	monster[i].mMaxDamage = 2 * (missile[mi]._mispllvl + 8);
	monster[i]._mFlags |= MFLAG_GOLEM;
	M_StartSpStand(i, DIR_S);
	M_Enemy(i);
	if (i == myplr) {
		NetSendCmdGolem(
		    monster[i].position.tile.x,
		    monster[i].position.tile.y,
		    monster[i]._mdir,
		    monster[i]._menemy,
		    monster[i]._mhitpoints,
		    currlevel);
	}
}

bool CanTalkToMonst(int m)
{
	commitment((DWORD)m < MAXMONSTERS, m);

	if (monster[m]._mgoal == MGOAL_INQUIRING) {
		return true;
	}

	return monster[m]._mgoal == MGOAL_TALKING;
}

bool CheckMonsterHit(int m, bool *ret)
{
	commitment((DWORD)m < MAXMONSTERS, m);

	if (monster[m]._mAi == AI_GARG && (monster[m]._mFlags & MFLAG_ALLOW_SPECIAL) != 0) {
		monster[m]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		monster[m]._mmode = MM_SATTACK;
		*ret = true;
		return true;
	}

	if (monster[m].MType->mtype >= MT_COUNSLR && monster[m].MType->mtype <= MT_ADVOCATE) {
		if (monster[m]._mgoal != MGOAL_NORMAL) {
			*ret = false;
			return true;
		}
	}

	return false;
}

int encode_enemy(int m)
{
	if ((monster[m]._mFlags & MFLAG_TARGETS_MONSTER) != 0)
		return monster[m]._menemy + MAX_PLRS;

	return monster[m]._menemy;
}

void decode_enemy(int m, int enemy)
{
	if (enemy < MAX_PLRS) {
		monster[m]._mFlags &= ~MFLAG_TARGETS_MONSTER;
		monster[m]._menemy = enemy;
		monster[m].enemyPosition = plr[enemy].position.future;
	} else {
		monster[m]._mFlags |= MFLAG_TARGETS_MONSTER;
		enemy -= MAX_PLRS;
		monster[m]._menemy = enemy;
		monster[m].enemyPosition = monster[enemy].position.future;
	}
}

void MonsterStruct::CheckStandAnimationIsLoaded(int mdir)
{
	if (_mmode == MM_STAND || _mmode == MM_TALK)
		AnimInfo.pCelSprite = &*MType->Anims[MA_STAND].CelSpritesForDirections[mdir];
}

void MonsterStruct::Petrify()
{
	_mmode = MM_STONE;
	AnimInfo.IsPetrified = true;
}

bool MonsterStruct::IsWalking() const
{
	switch (_mmode) {
	case MM_WALK:
	case MM_WALK2:
	case MM_WALK3:
		return true;
	default:
		return false;
	}
}

} // namespace devilution
