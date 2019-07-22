#include "diablo.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

// Tracks which missile files are already loaded
int MissileFileFlag;

int monstkills[MAXMONSTERS];
int monstactive[MAXMONSTERS];
int nummonsters;
BOOLEAN sgbSaveSoundOn;
MonsterStruct monster[MAXMONSTERS];
int totalmonsters;
CMonster Monsters[16];
// int END_Monsters_17;
int monstimgtot;
int uniquetrans;
int nummtypes;

const char plr2monst[9] = { 0, 5, 3, 7, 1, 4, 6, 0, 2 };
const BYTE counsmiss[4] = { MIS_FIREBOLT, MIS_CBOLT, MIS_LIGHTCTRL, MIS_FIREBALL };

/* data */

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
char animletter[7] = "nwahds";
int left[8] = { 7, 0, 1, 2, 3, 4, 5, 6 };
int right[8] = { 1, 2, 3, 4, 5, 6, 7, 0 };
int opposite[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
int offset_x[8] = { 1, 0, -1, -1, -1, 0, 1, 1 };
int offset_y[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

/* unused */
int rnd5[4] = { 5, 10, 15, 20 };
int rnd10[4] = { 10, 15, 20, 30 };
int rnd20[4] = { 20, 30, 40, 50 };
int rnd60[4] = { 60, 70, 80, 90 };

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
	&MAI_Warlord
};

void InitMonsterTRN(int monst, BOOL special)
{
	BYTE *f;
	int i, n, j;

	f = Monsters[monst].trans_file;
	for (i = 0; i < 256; i++) {
		if (*f == 255) {
			*f = 0;
		}
		f++;
	}

	n = special ? 6 : 5;
	for (i = 0; i < n; i++) {
		if (i != 1 || Monsters[monst].mtype < MT_COUNSLR || Monsters[monst].mtype > MT_ADVOCATE) {
			for (j = 0; j < 8; j++) {
				Cl2ApplyTrans(
				    Monsters[monst].Anims[i].Data[j],
				    Monsters[monst].trans_file,
				    Monsters[monst].Anims[i].Frames);
			}
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

int AddMonsterType(int type, int placeflag)
{
	BOOL done = FALSE;
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
	int typelist[MAXMONSTERS];
	int skeltypes[NUM_MTYPES];

	int minl; // min level
	int maxl; // max level
	char mamask;
	const int numskeltypes = 19;

	int nt; // number of types

	mamask = 3; // monster availability mask

	AddMonsterType(MT_GOLEM, 2);
	if (currlevel == 16) {
		AddMonsterType(MT_ADVOCATE, 1);
		AddMonsterType(MT_RBLACK, 1);
		AddMonsterType(MT_DIABLO, 2);
		return;
	}

	if (!setlevel) {
		if (QuestStatus(QTYPE_BUTCH))
			AddMonsterType(MT_CLEAVER, 2);
		if (QuestStatus(QTYPE_GARB))
			AddMonsterType(UniqMonst[UMT_GARBUD].mtype, 4);
		if (QuestStatus(QTYPE_ZHAR))
			AddMonsterType(UniqMonst[UMT_ZHAR].mtype, 4);
		if (QuestStatus(QTYPE_BOL))
			AddMonsterType(UniqMonst[UMT_SNOTSPIL].mtype, 4);
		if (QuestStatus(QTYPE_VEIL))
			AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, 4);
		if (QuestStatus(QTYPE_WARLRD))
			AddMonsterType(UniqMonst[UMT_WARLORD].mtype, 4);

		if (gbMaxPlayers != 1 && currlevel == quests[QTYPE_KING]._qlevel) {

			AddMonsterType(MT_SKING, 4);

			nt = 0;
			for (i = MT_WSKELAX; i <= MT_WSKELAX + numskeltypes; i++) {
				if (IsSkel(i)) {
					minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
					maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

					if (currlevel >= minl && currlevel <= maxl) {
						if (MonstAvailTbl[i] & mamask) {
							skeltypes[nt++] = i;
						}
					}
				}
			}
			AddMonsterType(skeltypes[random(88, nt)], 1);
		}

		nt = 0;
		for (i = 0; i < 111; i++) {
			minl = 15 * monsterdata[i].mMinDLvl / 30 + 1;
			maxl = 15 * monsterdata[i].mMaxDLvl / 30 + 1;

			if (currlevel >= minl && currlevel <= maxl) {
				if (MonstAvailTbl[i] & mamask) {
					typelist[nt++] = i;
				}
			}
		}

		if (monstdebug) {
			for (i = 0; i < debugmonsttypes; i++)
				AddMonsterType(DebugMonsters[i], 1);
		} else {

			while (nt > 0 && nummtypes < MAX_LVLMTYPES && monstimgtot < 4000) {
				for (i = 0; i < nt;) {
					if (monsterdata[typelist[i]].mImage > 4000 - monstimgtot) {
						typelist[i] = typelist[--nt];
						continue;
					}

					i++;
				}

				if (nt != 0) {
					i = random(88, nt);
					AddMonsterType(typelist[i], 1);
					typelist[i] = typelist[--nt];
				}
			}
		}

	} else {
		if (setlvlnum == SL_SKELKING) {
			AddMonsterType(MT_SKING, 4);
		}
	}
}

void InitMonsterGFX(int monst)
{
	int mtype, anim, i;
	char strBuff[256];
	BYTE *celBuf;

	mtype = Monsters[monst].mtype;

	for (anim = 0; anim < 6; anim++) {
		if ((animletter[anim] != 's' || monsterdata[mtype].has_special) && monsterdata[mtype].Frames[anim] > 0) {
			sprintf(strBuff, monsterdata[mtype].GraphicType, animletter[anim]);

			celBuf = LoadFileInMem(strBuff, NULL);
			Monsters[monst].Anims[anim].CMem = celBuf;

			if (Monsters[monst].mtype != MT_GOLEM || (animletter[anim] != 's' && animletter[anim] != 'd')) {

				for (i = 0; i < 8; i++) {
					Monsters[monst].Anims[anim].Data[i] = &celBuf[((int *)celBuf)[i]];
				}
			} else {
				for (i = 0; i < 8; i++) {
					Monsters[monst].Anims[anim].Data[i] = celBuf;
				}
			}
		}

		// TODO: either the AnimStruct members have wrong naming or the MonsterData ones it seems
		Monsters[monst].Anims[anim].Frames = monsterdata[mtype].Frames[anim];
		Monsters[monst].Anims[anim].Rate = monsterdata[mtype].Rate[anim];
	}

	Monsters[monst].width = monsterdata[mtype].width;
	Monsters[monst].width2 = (monsterdata[mtype].width - 64) >> 1;
	Monsters[monst].mMinHP = monsterdata[mtype].mMinHP;
	Monsters[monst].mMaxHP = monsterdata[mtype].mMaxHP;
	Monsters[monst].has_special = monsterdata[mtype].has_special;
	Monsters[monst].mAFNum = monsterdata[mtype].mAFNum;
	Monsters[monst].MData = &monsterdata[mtype];

	if (monsterdata[mtype].has_trans) {
		Monsters[monst].trans_file = LoadFileInMem(monsterdata[mtype].TransFile, NULL);
		InitMonsterTRN(monst, monsterdata[mtype].has_special);
		MemFreeDbg(Monsters[monst].trans_file);
	}

	if (mtype >= MT_NMAGMA && mtype <= MT_WMAGMA && !(MissileFileFlag & 1)) {
		MissileFileFlag |= 1;
		LoadMissileGFX(MFILE_MAGBALL);
	}
	if (mtype >= MT_STORM && mtype <= MT_MAEL && !(MissileFileFlag & 2)) {
		MissileFileFlag |= 2;
		LoadMissileGFX(MFILE_THINLGHT);
	}
	if (mtype == MT_SUCCUBUS) {
		if (MissileFileFlag & 4)
			return;

		MissileFileFlag |= 4;
		LoadMissileGFX(MFILE_FLARE);
		LoadMissileGFX(MFILE_FLAREEXP);
	}
	if (mtype == MT_SNOWWICH) {
		if (MissileFileFlag & 0x20)
			return;

		MissileFileFlag |= 0x20;
		LoadMissileGFX(MFILE_SCUBMISB);
		LoadMissileGFX(MFILE_SCBSEXPB);
	}
	if (mtype == MT_HLSPWN) {
		if (MissileFileFlag & 0x40)
			return;

		MissileFileFlag |= 0x40;
		LoadMissileGFX(MFILE_SCUBMISD);
		LoadMissileGFX(MFILE_SCBSEXPD);
	}
	if (mtype == MT_SOLBRNR) {
		if (MissileFileFlag & 0x80)
			return;

		MissileFileFlag |= 0x80;
		LoadMissileGFX(MFILE_SCUBMISC);
		LoadMissileGFX(MFILE_SCBSEXPC);
	}
	if (mtype >= MT_INCIN && mtype <= MT_HELLBURN && !(MissileFileFlag & 8)) {
		MissileFileFlag |= 8;
		LoadMissileGFX(MFILE_KRULL);
	}
	if (mtype >= MT_NACID && mtype <= MT_XACID && !(MissileFileFlag & 0x10)) {
		MissileFileFlag |= 0x10;
		LoadMissileGFX(MFILE_ACIDBF);
		LoadMissileGFX(MFILE_ACIDSPLA);
		LoadMissileGFX(MFILE_ACIDPUD);
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
	monster[i]._mVar4 = 0;
	monster[i]._mVar5 = 0;
	monster[i]._mVar6 = 0;
	monster[i]._mVar7 = 0;
	monster[i]._mVar8 = 0;
}

void InitMonster(int i, int rd, int mtype, int x, int y)
{
	CMonster *monst = &Monsters[mtype];

	monster[i]._mdir = rd;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	monster[i]._mmode = MM_STAND;
	monster[i]._mMTidx = mtype;
	monster[i].mName = monst->MData->mName;
	monster[i].MType = monst;
	monster[i].MData = monst->MData;
	monster[i]._mAnimData = monst->Anims[MA_STAND].Data[rd];
	monster[i]._mAnimDelay = monst->Anims[MA_STAND].Rate;
	monster[i]._mAnimCnt = random(88, monst->Anims[MA_STAND].Rate - 1);
	monster[i]._mAnimLen = monst->Anims[MA_STAND].Frames;
	monster[i]._mAnimFrame = random(88, monst->Anims[MA_STAND].Frames - 1) + 1;

	if (monst->mtype == MT_DIABLO) {
		monster[i]._mmaxhp = (random(88, 1) + 1666) << 6;
	} else {
		monster[i]._mmaxhp = (monst->mMinHP + random(88, monst->mMaxHP - monst->mMinHP + 1)) << 6;
	}

	if (gbMaxPlayers == 1) {
		monster[i]._mmaxhp >>= 1;
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
	monster[i]._uniqtype = 0;
	monster[i]._msquelch = 0;
	monster[i].field_18 = 0;
	monster[i]._mDelFlag = FALSE;
	monster[i]._mRndSeed = GetRndSeed();
	monster[i]._mAISeed = GetRndSeed();
	monster[i].mWhoHit = 0;
	monster[i].mLevel = monst->MData->mLevel;
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
	monster[i].mtalkmsg = 0;

	if (monster[i]._mAi == AI_GARG) {
		monster[i]._mAnimData = monst->Anims[MA_SPECIAL].Data[rd];
		monster[i]._mAnimFrame = 1;
		monster[i]._mFlags |= MFLAG_ALLOW_SPECIAL;
		monster[i]._mmode = MM_SATTACK;
	}

	if (gnDifficulty == DIFF_NIGHTMARE) {
		monster[i].mLevel += 15;
		monster[i].mHit += 85;
		monster[i].mHit2 += 85;
		monster[i]._mmaxhp = 3 * monster[i]._mmaxhp + 64;
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mExp = 2 * (monster[i].mExp + 1000);
		monster[i].mMinDamage = 2 * (monster[i].mMinDamage + 2);
		monster[i].mMaxDamage = 2 * (monster[i].mMaxDamage + 2);
		monster[i].mMinDamage2 = 2 * (monster[i].mMinDamage2 + 2);
		monster[i].mMaxDamage2 = 2 * (monster[i].mMaxDamage2 + 2);
		monster[i].mArmorClass += 50;
	}

	if (gnDifficulty == DIFF_HELL) {
		monster[i].mLevel += 30;
		monster[i]._mmaxhp = 4 * monster[i]._mmaxhp + 192;
		monster[i]._mhitpoints = monster[i]._mmaxhp;
		monster[i].mHit += 120;
		monster[i].mHit2 += 120;
		monster[i].mExp = 4 * (monster[i].mExp + 1000);
		monster[i].mMinDamage = 4 * monster[i].mMinDamage + 6;
		monster[i].mMaxDamage = 4 * monster[i].mMaxDamage + 6;
		monster[i].mMinDamage2 = 4 * monster[i].mMinDamage2 + 6;
		monster[i].mMaxDamage2 = 4 * monster[i].mMaxDamage2 + 6;
		monster[i].mArmorClass += 80;
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
		Monst->_mgoal = 0;
		Monst->_mmode = MM_STAND;
		Monst->_mVar1 = 0;
		Monst->_mVar2 = 0;
		Monst->_mx = 0;
		Monst->_my = 0;
		Monst->_mfutx = 0;
		Monst->_mfuty = 0;
		Monst->_moldx = 0;
		Monst->_moldy = 0;
		Monst->_mdir = random(89, 8);
		Monst->_mxvel = 0;
		Monst->_myvel = 0;
		Monst->_mAnimData = NULL;
		Monst->_mAnimDelay = 0;
		Monst->_mAnimCnt = 0;
		Monst->_mAnimLen = 0;
		Monst->_mAnimFrame = 0;
		Monst->_mFlags = 0;
		Monst->_mDelFlag = FALSE;
		Monst->_menemy = random(89, gbActivePlayers);
		Monst->_menemyx = plr[Monst->_menemy]._px;
		Monst->_menemyy = plr[Monst->_menemy]._py;
	}
}

BOOL MonstPlace(int xp, int yp)
{
	char f;

	if (xp < 0 || xp >= MAXDUNX
	    || yp < 0 || yp >= MAXDUNY
	    || dMonster[xp][yp]
	    || dPlayer[xp][yp]) {
		return FALSE;
	}

	f = dFlags[xp][yp];

	if (f & BFLAG_VISIBLE) {
		return FALSE;
	}

	if (f & BFLAG_POPULATED) {
		return FALSE;
	}

	return !SolidLoc(xp, yp);
}

void PlaceMonster(int i, int mtype, int x, int y)
{
	int rd;

	dMonster[x][y] = i + 1;

	rd = random(90, 8);
	InitMonster(i, rd, mtype, x, y);
}

void PlaceUniqueMonst(int uniqindex, int miniontype, int unpackfilesize)
{
	int xp, yp, x, y, i;
	int uniqtype;
	int count2;
	char filestr[64];
	BOOL zharflag, done;
	UniqMonstStruct *Uniq;
	MonsterStruct *Monst;
	int count;

	Monst = monster + nummonsters;
	count = 0;
	Uniq = UniqMonst + uniqindex;

	if ((uniquetrans + 19) << 8 >= LIGHTSIZE) {
		return;
	}

	for (uniqtype = 0; uniqtype < nummtypes; uniqtype++) {
		if (Monsters[uniqtype].mtype == UniqMonst[uniqindex].mtype) {
			break;
		}
	}

	while (1) {
		xp = random(91, 80) + 16;
		yp = random(91, 80) + 16;
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

	if (uniqindex == 3) {
		xp = 2 * setpc_x + 24;
		yp = 2 * setpc_y + 28;
	}
	if (uniqindex == 8) {
		xp = 2 * setpc_x + 22;
		yp = 2 * setpc_y + 23;
	}
	if (uniqindex == 2) {
		zharflag = TRUE;
		for (i = 0; i < themeCount; i++) {
			if (i == zharlib && zharflag == TRUE) {
				zharflag = FALSE;
				xp = 2 * themeLoc[i].x + 20;
				yp = 2 * themeLoc[i].y + 20;
			}
		}
	}
	if (gbMaxPlayers == 1) {
		if (uniqindex == 4) {
			xp = 32;
			yp = 46;
		}
		if (uniqindex == 5) {
			xp = 40;
			yp = 45;
		}
		if (uniqindex == 6) {
			xp = 38;
			yp = 49;
		}
		if (uniqindex == 1) {
			xp = 35;
			yp = 47;
		}
	} else {
		if (uniqindex == 4) {
			xp = 2 * setpc_x + 19;
			yp = 2 * setpc_y + 22;
		}
		if (uniqindex == 5) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 19;
		}
		if (uniqindex == 6) {
			xp = 2 * setpc_x + 21;
			yp = 2 * setpc_y + 25;
		}
	}
	if (uniqindex == 9) {
		done = FALSE;
		for (yp = 0; yp < MAXDUNY && !done; yp++) {
			for (xp = 0; xp < MAXDUNX && !done; xp++) {
				done = dPiece[xp][yp] == 367;
			}
		}
	}

	PlaceMonster(nummonsters, uniqtype, xp, yp);
	Monst->_uniqtype = uniqindex + 1;

	if (Uniq->mlevel) {
		Monst->mLevel = 2 * Uniq->mlevel;
	} else {
		Monst->mLevel += 5;
	}

	Monst->mExp *= 2;
	Monst->mName = Uniq->mName;
	Monst->_mmaxhp = Uniq->mmaxhp << 6;

	if (gbMaxPlayers == 1) {
		Monst->_mmaxhp = Monst->_mmaxhp >> 1;
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
	Monst->mlid = AddLight(Monst->_mx, Monst->_my, 3);

	if (gbMaxPlayers == 1) {
		if (Monst->mtalkmsg) {
			Monst->_mgoal = MGOAL_INQUIRING;
		}
	} else {
		if (Monst->_mAi == AI_LAZHELP) {
			Monst->mtalkmsg = 0;
		}

		if (Monst->_mAi != AI_LAZURUS || quests[QTYPE_VB]._qvar1 <= 3) {
			if (Monst->mtalkmsg) {
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		} else {
			Monst->_mgoal = MGOAL_NORMAL;
		}
	}

	if (gnDifficulty == DIFF_NIGHTMARE) {
		Monst->mLevel += 15;
		Monst->_mmaxhp = 3 * Monst->_mmaxhp + 64;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 2 * (Monst->mExp + 1000);
		Monst->mMinDamage = 2 * (Monst->mMinDamage + 2);
		Monst->mMaxDamage = 2 * (Monst->mMaxDamage + 2);
		Monst->mMinDamage2 = 2 * (Monst->mMinDamage2 + 2);
		Monst->mMaxDamage2 = 2 * (Monst->mMaxDamage2 + 2);
	}

	if (gnDifficulty == DIFF_HELL) {
		Monst->mLevel += 30;
		Monst->_mmaxhp = 4 * Monst->_mmaxhp + 192;
		Monst->_mhitpoints = Monst->_mmaxhp;
		Monst->mExp = 4 * (Monst->mExp + 1000);
		Monst->mMinDamage = 4 * Monst->mMinDamage + 6;
		Monst->mMaxDamage = 4 * Monst->mMaxDamage + 6;
		Monst->mMinDamage2 = 4 * Monst->mMinDamage2 + 6;
		Monst->mMaxDamage2 = 4 * Monst->mMaxDamage2 + 6;
	}

	sprintf(filestr, "Monsters\\Monsters\\%s.TRN", Uniq->mTrnName);
	LoadFileWithMem(filestr, &pLightTbl[256 * (uniquetrans + 19)]);

	Monst->_uniqtrans = uniquetrans++;

	if (Uniq->mUnqAttr & 4) {
		Monst->mHit = Uniq->mUnqVar1;
		Monst->mHit2 = Uniq->mUnqVar1;
	}
	if (Uniq->mUnqAttr & 8) {
		Monst->mArmorClass = Uniq->mUnqVar1;
	}

	nummonsters++;

	if (Uniq->mUnqAttr & 1) {
		PlaceGroup(miniontype, unpackfilesize, Uniq->mUnqAttr, nummonsters - 1);
	}

	if (Monst->_mAi != AI_GARG) {
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
		Monst->_mAnimFrame = random(88, Monst->_mAnimLen - 1) + 1;
		Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		Monst->_mmode = MM_STAND;
	}
}

void PlaceQuestMonsters()
{
	int skeltype;
	BYTE *setp;

	if (!setlevel) {
		if (QuestStatus(QTYPE_BUTCH)) {
			PlaceUniqueMonst(UMT_BUTCHER, 0, 0);
		}

		if (currlevel == quests[QTYPE_KING]._qlevel && gbMaxPlayers != 1) {
			skeltype = 0;

			for (skeltype = 0; skeltype < nummtypes; skeltype++) {
				if (IsSkel(Monsters[skeltype].mtype)) {
					break;
				}
			}

			PlaceUniqueMonst(UMT_SKELKING, skeltype, 30);
		}

		if (QuestStatus(QTYPE_BOL)) {
			setp = LoadFileInMem("Levels\\L1Data\\Banner1.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(QTYPE_BLOOD)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blood2.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(QTYPE_BLIND)) {
			setp = LoadFileInMem("Levels\\L2Data\\Blind2.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
		if (QuestStatus(QTYPE_ANVIL)) {
			setp = LoadFileInMem("Levels\\L3Data\\Anvil.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x + 2, 2 * setpc_y + 2);
			mem_free_dbg(setp);
		}
		if (QuestStatus(QTYPE_WARLRD)) {
			setp = LoadFileInMem("Levels\\L4Data\\Warlord.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
			AddMonsterType(UniqMonst[UMT_WARLORD].mtype, 1);
		}
		if (QuestStatus(QTYPE_VEIL)) {
			AddMonsterType(UniqMonst[UMT_LACHDAN].mtype, 1);
		}
		if (QuestStatus(QTYPE_ZHAR) && zharlib == -1) {
			quests[QTYPE_ZHAR]._qactive = 0;
		}

		if (currlevel == quests[QTYPE_VB]._qlevel && gbMaxPlayers != 1) {
			AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, 4);
			AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, 4);
			PlaceUniqueMonst(UMT_LAZURUS, 0, 0);
			PlaceUniqueMonst(UMT_RED_VEX, 0, 0);
			PlaceUniqueMonst(UMT_BLACKJADE, 0, 0);
			setp = LoadFileInMem("Levels\\L4Data\\Vile1.DUN", NULL);
			SetMapMonsters(setp, 2 * setpc_x, 2 * setpc_y);
			mem_free_dbg(setp);
		}
	} else if (setlvlnum == SL_SKELKING) {
		PlaceUniqueMonst(UMT_SKELKING, 0, 0);
	}
}

void PlaceGroup(int mtype, int num, int leaderf, int leader)
{
	int placed, try1, try2, j;
	int xp, yp, x1, y1;

	placed = 0;

	for (try1 = 0; try1 < 10; try1++) {
		while (placed) {
			nummonsters--;
			placed--;
			dMonster[monster[nummonsters]._mx][monster[nummonsters]._my] = 0;
		}

		x1 = 0;
		y1 = 0;
		xp = 0;
		yp = 0;
		if (leaderf & 1) {
			int offset = random(92, 8);
			xp = monster[leader]._mx + offset_x[offset];
			yp = monster[leader]._my + offset_y[offset];
			x1 = xp;
			y1 = yp;
		} else {
			do {
				xp = random(93, 80) + 16;
				x1 = xp;
				yp = random(93, 80) + 16;
				y1 = yp;
			} while (!MonstPlace(xp, yp));
		}

		if (num + nummonsters > totalmonsters) {
			num = totalmonsters - nummonsters;
		}

		j = 0;
		for (try2 = 0; j < num && try2 < 100; xp += offset_x[random(94, 8)], yp += offset_x[random(94, 8)]) {
			if (!MonstPlace(xp, yp)
			    || (dTransVal[xp][yp] != dTransVal[x1][y1])
			    || (leaderf & 2) && ((abs(xp - x1) >= 4) || (abs(yp - y1) >= 4))) {
				try2++;
				continue;
			}

			PlaceMonster(nummonsters, mtype, xp, yp);
			if (leaderf & 1) {
				monster[nummonsters]._mmaxhp *= 2;
				monster[nummonsters]._mhitpoints = monster[nummonsters]._mmaxhp;
				monster[nummonsters]._mint = monster[leader]._mint;

				if (leaderf & 2) {
					monster[nummonsters].leader = leader;
					monster[nummonsters].leaderflag = 1;
					monster[nummonsters]._mAi = monster[leader]._mAi;
				}

				if (monster[nummonsters]._mAi != AI_GARG) {
					monster[nummonsters]._mAnimData = monster[nummonsters].MType->Anims[MA_STAND].Data[monster[nummonsters]._mdir];
					monster[nummonsters]._mAnimFrame = random(88, monster[nummonsters]._mAnimLen - 1) + 1;
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

	if (leaderf & 2) {
		monster[leader].packsize = placed;
	}
}

void LoadDiabMonsts()
{
	BYTE *lpSetPiece;

	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab1.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad1x, 2 * diabquad1y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab2a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad2x, 2 * diabquad2y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab3a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad3x, 2 * diabquad3y);
	mem_free_dbg(lpSetPiece);
	lpSetPiece = LoadFileInMem("Levels\\L4Data\\diab4a.DUN", NULL);
	SetMapMonsters(lpSetPiece, 2 * diabquad4x, 2 * diabquad4y);
	mem_free_dbg(lpSetPiece);
}

void InitMonsters()
{
	int na, nt;
	int i, s, t;
	int numplacemonsters;
	int mtype;
	int numscattypes;
	int scattertypes[111];

	numscattypes = 0;
	if (gbMaxPlayers != 1)
		CheckDungeonClear();
	if (!setlevel) {
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		AddMonster(1, 0, 0, 0, FALSE);
		if (!setlevel && currlevel == 16)
			LoadDiabMonsts();
	}
	nt = numtrigs;
	if (currlevel == 15)
		nt = 1;
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoVision(s + trigs[i]._tx, t + trigs[i]._ty, 15, FALSE, FALSE);
		}
	}
	PlaceQuestMonsters();
	if (!setlevel) {
		PlaceUniques();
		na = 0;
		for (s = 16; s < 96; s++)
			for (t = 16; t < 96; t++)
				if (!SolidLoc(s, t))
					na++;
		numplacemonsters = na / 30;
		if (gbMaxPlayers != 1)
			numplacemonsters += numplacemonsters >> 1;
		if (nummonsters + numplacemonsters > 190)
			numplacemonsters = 190 - nummonsters;
		totalmonsters = nummonsters + numplacemonsters;
		for (i = 0; i < nummtypes; i++) {
			if (Monsters[i].mPlaceFlags & 1) {
				scattertypes[numscattypes] = i;
				numscattypes++;
			}
		}
		while (nummonsters < totalmonsters) {
			mtype = scattertypes[random(95, numscattypes)];
			if (currlevel == 1 || random(95, 2) == 0)
				na = 1;
			else if (currlevel == 2)
				na = random(95, 2) + 2;
			else
				na = random(95, 3) + 3;
			PlaceGroup(mtype, na, 0, 0);
		}
	}
	for (i = 0; i < nt; i++) {
		for (s = -2; s < 2; s++) {
			for (t = -2; t < 2; t++)
				DoUnVision(s + trigs[i]._tx, t + trigs[i]._ty, 15);
		}
	}
}

void PlaceUniques()
{
	int u, mt;
	BOOL done;

	for (u = 0; UniqMonst[u].mtype != -1; u++) {
		if (UniqMonst[u].mlevel != currlevel)
			continue;
		done = FALSE;
		for (mt = 0; mt < nummtypes; mt++) {
			if (done)
				break;
			done = (Monsters[mt].mtype == UniqMonst[u].mtype);
		}
		mt--;
		if (u == UMT_GARBUD && quests[QTYPE_GARB]._qactive == 0)
			done = FALSE;
		if (u == UMT_ZHAR && quests[QTYPE_ZHAR]._qactive == 0)
			done = FALSE;
		if (u == UMT_SNOTSPIL && quests[QTYPE_BOL]._qactive == 0)
			done = FALSE;
		if (u == UMT_LACHDAN && quests[QTYPE_VEIL]._qactive == 0)
			done = FALSE;
		if (u == UMT_WARLORD && quests[QTYPE_WARLRD]._qactive == 0)
			done = FALSE;
		if (done)
			PlaceUniqueMonst(u, mt, 8);
	}
}

void SetMapMonsters(BYTE *pMap, int startx, int starty)
{
	WORD rw, rh;
	WORD *lm;
	int i, j;
	int mtype;

	AddMonsterType(MT_GOLEM, 2);
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);
	AddMonster(1, 0, 0, 0, FALSE);
	if (setlevel && setlvlnum == SL_VILEBETRAYER) {
		AddMonsterType(UniqMonst[UMT_LAZURUS].mtype, 4);
		AddMonsterType(UniqMonst[UMT_RED_VEX].mtype, 4);
		AddMonsterType(UniqMonst[UMT_BLACKJADE].mtype, 4);
		PlaceUniqueMonst(UMT_LAZURUS, 0, 0);
		PlaceUniqueMonst(UMT_RED_VEX, 0, 0);
		PlaceUniqueMonst(UMT_BLACKJADE, 0, 0);
	}
	lm = (WORD *)pMap;
	rw = *lm;
	lm++;
	rh = *lm;
	lm += (rw * rh + 1);
	rw = rw << 1;
	rh = rh << 1;
	lm += rw * rh;

	for (j = 0; j < rh; j++) {
		for (i = 0; i < rw; i++) {
			if (*lm) {
				mtype = AddMonsterType(MonstConvTbl[(*lm) - 1], 2);
				PlaceMonster(nummonsters++, mtype, i + startx + 16, j + starty + 16);
			}
			lm++;
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

int AddMonster(int x, int y, int dir, int mtype, BOOL InMap)
{
	if (nummonsters < MAXMONSTERS) {
		int i = monstactive[nummonsters++];
		if (InMap)
			dMonster[x][y] = i + 1;
		InitMonster(i, dir, mtype, x, y);
		return i;
	}

	return -1;
}

void NewMonsterAnim(int i, AnimStruct *anim, int md)
{
	MonsterStruct *Monst = monster + i;
	Monst->_mAnimData = anim->Data[md];
	Monst->_mAnimLen = anim->Frames;
	Monst->_mAnimCnt = 0;
	Monst->_mAnimFrame = 1;
	Monst->_mAnimDelay = anim->Rate;
	Monst->_mFlags &= ~(MFLAG_LOCK_ANIMATION | MFLAG_ALLOW_SPECIAL);
	Monst->_mdir = md;
}

BOOL M_Ranged(int i)
{
	char ai = monster[i]._mAi;
	return ai == AI_SKELBOW || ai == AI_GOATBOW || ai == AI_SUCC || ai == AI_LAZHELP;
}

BOOL M_Talker(int i)
{
	char ai = monster[i]._mAi;
	return ai == AI_LAZURUS
	    || ai == AI_WARLORD
	    || ai == AI_GARBUD
	    || ai == AI_ZHAR
	    || ai == AI_SNOTSPIL
	    || ai == AI_LACHDAN
	    || ai == AI_LAZHELP;
}

void M_Enemy(int i)
{
	int j;
	int mi, pnum;
	int dist, best_dist;
	int _menemy;
	BOOL sameroom, bestsameroom;
	MonsterStruct *Monst;
	BYTE enemyx, enemyy;

	_menemy = -1;
	best_dist = -1;
	bestsameroom = 0;
	Monst = monster + i;
	if (!(Monst->_mFlags & MFLAG_GOLEM)) {
		for (pnum = 0; pnum < MAX_PLRS; pnum++) {
			if (!plr[pnum].plractive || currlevel != plr[pnum].plrlevel || plr[pnum]._pLvlChanging || (plr[pnum]._pHitPoints == 0 && gbMaxPlayers != 1))
				continue;
			if (dTransVal[Monst->_mx][Monst->_my] == dTransVal[plr[pnum].WorldX][plr[pnum].WorldY])
				sameroom = TRUE;
			else
				sameroom = FALSE;
			if (abs(Monst->_mx - plr[pnum].WorldX) > abs(Monst->_my - plr[pnum].WorldY))
				dist = Monst->_mx - plr[pnum].WorldX;
			else
				dist = Monst->_my - plr[pnum].WorldY;
			dist = abs(dist);
			if ((sameroom && !bestsameroom)
			    || ((sameroom || !bestsameroom) && dist < best_dist)
			    || (_menemy == -1)) {
				Monst->_mFlags &= ~MFLAG_TARGETS_MONSTER;
				_menemy = pnum;
				enemyx = plr[pnum]._px;
				enemyy = plr[pnum]._py;
				best_dist = dist;
				bestsameroom = sameroom;
			}
		}
	}
	for (j = 0; j < nummonsters; j++) {
		mi = monstactive[j];
		if (mi == i)
			continue;
		if (monster[mi]._mx == 1 && monster[mi]._my == 0)
			continue;
		if (M_Talker(mi) && monster[mi].mtalkmsg)
			continue;
		if (!(Monst->_mFlags & MFLAG_GOLEM)
		    && ((abs(monster[mi]._mx - Monst->_mx) >= 2 || abs(monster[mi]._my - Monst->_my) >= 2) && !M_Ranged(i)
		        || (!(Monst->_mFlags & MFLAG_GOLEM) && !(monster[mi]._mFlags & MFLAG_GOLEM)))) {
			continue;
		}
		sameroom = dTransVal[Monst->_mx][Monst->_my] == dTransVal[monster[mi]._mx][monster[mi]._my];
		if (abs(Monst->_mx - monster[mi]._mx) > abs(Monst->_my - monster[mi]._my))
			dist = Monst->_mx - monster[mi]._mx;
		else
			dist = Monst->_my - monster[mi]._my;
		dist = abs(dist);
		if ((sameroom && !bestsameroom)
		    || ((sameroom || !bestsameroom) && dist < best_dist)
		    || (_menemy == -1)) {
			Monst->_mFlags |= MFLAG_TARGETS_MONSTER;
			_menemy = mi;
			enemyx = monster[mi]._mfutx;
			enemyy = monster[mi]._mfuty;
			best_dist = dist;
			bestsameroom = sameroom;
		}
	}
	if (_menemy != -1) {
		Monst->_mFlags &= ~MFLAG_NO_ENEMY;
		Monst->_menemy = _menemy;
		Monst->_menemyx = enemyx;
		Monst->_menemyy = enemyy;
	} else {
		Monst->_mFlags |= MFLAG_NO_ENEMY;
	}
}

int M_GetDir(int i)
{
	return GetDirection(monster[i]._mx, monster[i]._my, monster[i]._menemyx, monster[i]._menemyy);
}

void M_CheckEFlag(int i)
{
	int f, j;
	int x, y;
	WORD *m;

	x = monster[i]._mx - 1;
	y = monster[i]._my + 1;
	f = 0;
	m = dpiece_defs_map_2[x][y].mt;
	if (m >= dpiece_defs_map_2[0][0].mt) {
		for (j = 2; j < 10; j++) {
			f |= m[j];
		}
	} else {
		monster[i]._meflag = FALSE;
		return;
	}

	if (f | dArch[x][y])
		monster[i]._meflag = TRUE;
	else {
		monster[i]._meflag = FALSE;
	}
}

void M_StartStand(int i, int md)
{
	ClearMVars(i);
	if (monster[i].MType->mtype == MT_GOLEM)
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], md);
	else
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_STAND], md);
	monster[i]._mVar1 = monster[i]._mmode;
	monster[i]._mVar2 = 0;
	monster[i]._mmode = MM_STAND;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
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

void M_StartSpStand(int i, int md)
{
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_SPSTAND;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void M_StartWalk(int i, int xvel, int yvel, int xadd, int yadd, int EndDir)
{
	int fx = xadd + monster[i]._mx;
	int fy = yadd + monster[i]._my;

	dMonster[fx][fy] = -(i + 1);
	monster[i]._mmode = MM_WALK;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar1 = xadd;
	monster[i]._mVar2 = yadd;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = 0;
	monster[i]._mVar7 = 0;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

void M_StartWalk2(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int EndDir)
{
	int fx = xadd + monster[i]._mx;
	int fy = yadd + monster[i]._my;

	dMonster[monster[i]._mx][monster[i]._my] = -(i + 1);
	monster[i]._mVar1 = monster[i]._mx;
	monster[i]._mVar2 = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mx = fx;
	monster[i]._my = fy;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	dMonster[fx][fy] = i + 1;
	if (monster[i]._uniqtype != 0)
		ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
	monster[i]._mxoff = xoff;
	monster[i]._myoff = yoff;
	monster[i]._mmode = MM_WALK2;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = 16 * xoff;
	monster[i]._mVar7 = 16 * yoff;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

void M_StartWalk3(int i, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, int EndDir)
{
	int fx = xadd + monster[i]._mx;
	int fy = yadd + monster[i]._my;
	int x = mapx + monster[i]._mx;
	int y = mapy + monster[i]._my;

	if (monster[i]._uniqtype != 0)
		ChangeLightXY(monster[i].mlid, x, y);

	dMonster[monster[i]._mx][monster[i]._my] = -(i + 1);
	dMonster[fx][fy] = -(i + 1);
	monster[i]._mVar4 = x;
	monster[i]._mVar5 = y;
	dFlags[x][y] |= BFLAG_MONSTLR;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mfutx = fx;
	monster[i]._mfuty = fy;
	monster[i]._mxoff = xoff;
	monster[i]._myoff = yoff;
	monster[i]._mmode = MM_WALK3;
	monster[i]._mxvel = xvel;
	monster[i]._myvel = yvel;
	monster[i]._mVar1 = fx;
	monster[i]._mVar2 = fy;
	monster[i]._mVar3 = EndDir;
	monster[i]._mdir = EndDir;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_WALK], EndDir);
	monster[i]._mVar6 = 16 * xoff;
	monster[i]._mVar7 = 16 * yoff;
	monster[i]._mVar8 = 0;
	M_CheckEFlag(i);
}

void M_StartAttack(int i)
{
	int md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_ATTACK], md);
	monster[i]._mmode = MM_ATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void M_StartRAttack(int i, int missile_type, int dam)
{
	int md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_ATTACK], md);
	monster[i]._mmode = MM_RATTACK;
	monster[i]._mVar1 = missile_type;
	monster[i]._mVar2 = dam;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void M_StartRSpAttack(int i, int missile_type, int dam)
{
	int md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_RSPATTACK;
	monster[i]._mVar1 = missile_type;
	monster[i]._mVar2 = 0;
	monster[i]._mVar3 = dam;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void M_StartSpAttack(int i)
{
	int md = M_GetDir(i);
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_SATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	monster[i]._mdir = md;
	M_CheckEFlag(i);
}

void M_StartEat(int i)
{
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], monster[i]._mdir);
	monster[i]._mmode = MM_SATTACK;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
}

void M_ClearSquares(int i)
{
	int x, y, mx, my, m1, m2;

	mx = monster[i]._moldx;
	my = monster[i]._moldy;
	m1 = -1 - i;
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
	int d = (monster[i]._mdir - 4) & 7;
	if (DirOK(i, d)) {
		M_ClearSquares(i);
		monster[i]._moldx += offset_x[d];
		monster[i]._moldy += offset_y[d];
		NewMonsterAnim(i, &monster[i].MType->Anims[MA_GOTHIT], monster[i]._mdir);
		monster[i]._mmode = MM_GOTHIT;
		monster[i]._mxoff = 0;
		monster[i]._myoff = 0;
		monster[i]._mx = monster[i]._moldx;
		monster[i]._my = monster[i]._moldy;
		monster[i]._mfutx = monster[i]._moldx;
		monster[i]._mfuty = monster[i]._moldy;
		M_CheckEFlag(i);
		M_ClearSquares(i);
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
	}
}

void M_StartHit(int i, int pnum, int dam)
{
	if (pnum >= 0)
		monster[i].mWhoHit |= 1 << pnum;
	if (pnum == myplr) {
		delta_monster_hp(i, monster[i]._mhitpoints, currlevel);
		NetSendCmdParam2(FALSE, CMD_MONSTDAMAGE, i, dam);
	}
	PlayEffect(i, 1);
	if (monster[i].MType->mtype >= MT_SNEAK && monster[i].MType->mtype <= MT_ILLWEAV || dam >> 6 >= monster[i].mLevel + 3) {
		if (pnum >= 0) {
			monster[i]._mFlags &= ~MFLAG_TARGETS_MONSTER;
			monster[i]._menemy = pnum;
			monster[i]._menemyx = plr[pnum]._px;
			monster[i]._menemyy = plr[pnum]._py;
			monster[i]._mdir = M_GetDir(i);
		}
		if (monster[i].MType->mtype == MT_BLINK) {
			M_Teleport(i);
		} else if (monster[i].MType->mtype >= MT_NSCAV && monster[i].MType->mtype <= MT_YSCAV) {
			monster[i]._mgoal = MGOAL_NORMAL;
		}
		if (monster[i]._mmode != MM_STONE) {
			NewMonsterAnim(i, &monster[i].MType->Anims[MA_GOTHIT], monster[i]._mdir);
			monster[i]._mmode = MM_GOTHIT;
			monster[i]._mxoff = 0;
			monster[i]._myoff = 0;
			monster[i]._mx = monster[i]._moldx;
			monster[i]._my = monster[i]._moldy;
			monster[i]._mfutx = monster[i]._moldx;
			monster[i]._mfuty = monster[i]._moldy;
			M_CheckEFlag(i);
			M_ClearSquares(i);
			dMonster[monster[i]._mx][monster[i]._my] = i + 1;
		}
	}
}

void M_DiabloDeath(int i, BOOL sendmsg)
{
	MonsterStruct *Monst, *pmonster;
	int dist;
	int j, k;
	int _moldx, _moldy;

	Monst = monster + i;
	PlaySFX(USFX_DIABLOD);
	quests[QTYPE_MOD]._qactive = 3;
	if (sendmsg)
		NetSendCmdQuest(TRUE, QTYPE_MOD);
	gbProcessPlayers = FALSE;
	sgbSaveSoundOn = gbSoundOn;
	for (j = 0; j < nummonsters; j++) {
		k = monstactive[j];
		if (k == i || monster[i]._msquelch == 0)
			continue;

		pmonster = monster + k;
		NewMonsterAnim(k, pmonster->MType->Anims + MA_DEATH, pmonster->_mdir);
		monster[k]._mxoff = 0;
		monster[k]._myoff = 0;
		monster[k]._mVar1 = 0;
		_moldx = monster[k]._moldx;
		_moldy = monster[k]._moldy;
		monster[k]._my = _moldy;
		monster[k]._mfuty = _moldy;
		monster[k]._mmode = MM_DEATH;
		monster[k]._mx = _moldx;
		monster[k]._mfutx = _moldx;
		M_CheckEFlag(k);
		M_ClearSquares(k);
		dMonster[pmonster->_mx][pmonster->_my] = k + 1;
	}
	AddLight(Monst->_mx, Monst->_my, 8);
	DoVision(Monst->_mx, Monst->_my, 8, FALSE, TRUE);
	if (abs(ViewX - Monst->_mx) > abs(ViewY - Monst->_my))
		dist = abs(ViewX - Monst->_mx);
	else
		dist = abs(ViewY - Monst->_my);
	if (dist > 20)
		dist = 20;
	j = ViewX << 16;
	k = ViewY << 16;
	Monst->_mVar3 = j;
	Monst->_mVar4 = k;
	Monst->_mVar5 = (int)((j - (Monst->_mx << 16)) / (double)dist);
	Monst->_mVar6 = (int)((k - (Monst->_my << 16)) / (double)dist);
}

void M2MStartHit(int mid, int i, int dam)
{
	if ((DWORD)mid >= MAXMONSTERS) {
		app_fatal("Invalid monster %d getting hit by monster", mid);
	}

	if (monster[mid].MType == NULL) {
		app_fatal("Monster %d \"%s\" getting hit by monster: MType NULL", mid, monster[mid].mName);
	}

	if (i >= 0)
		monster[i].mWhoHit |= 1 << i;

	delta_monster_hp(mid, monster[mid]._mhitpoints, currlevel);
	NetSendCmdParam2(FALSE, CMD_MONSTDAMAGE, mid, dam);
	PlayEffect(mid, 1);

	if (monster[mid].MType->mtype >= MT_SNEAK && monster[mid].MType->mtype <= MT_ILLWEAV || dam >> 6 >= monster[mid].mLevel + 3) {
		if (i >= 0)
			monster[mid]._mdir = (monster[i]._mdir - 4) & 7;

		if (monster[mid].MType->mtype == MT_BLINK) {
			M_Teleport(mid);
		} else if (monster[mid].MType->mtype >= MT_NSCAV && monster[mid].MType->mtype <= MT_YSCAV) {
			monster[mid]._mgoal = MGOAL_NORMAL;
		}

		if (monster[mid]._mmode != MM_STONE) {
			if (monster[mid].MType->mtype != MT_GOLEM) {
				NewMonsterAnim(mid, &monster[mid].MType->Anims[MA_GOTHIT], monster[mid]._mdir);
				monster[mid]._mmode = MM_GOTHIT;
			}

			monster[mid]._mxoff = 0;
			monster[mid]._myoff = 0;
			monster[mid]._mx = monster[mid]._moldx;
			monster[mid]._my = monster[mid]._moldy;
			monster[mid]._mfutx = monster[mid]._moldx;
			monster[mid]._mfuty = monster[mid]._moldy;
			M_CheckEFlag(mid);
			M_ClearSquares(mid);
			dMonster[monster[mid]._mx][monster[mid]._my] = mid + 1;
		}
	}
}

void MonstStartKill(int i, int pnum, BOOL sendmsg)
{
	int md;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("MonstStartKill: Invalid monster %d", i);
	}
	if (!monster[i].MType) {
		app_fatal("MonstStartKill: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	}

	if (pnum >= 0)
		monster[i].mWhoHit |= 1 << pnum;
	if (pnum < MAX_PLRS && i > MAX_PLRS)
		AddPlrMonstExper(monster[i].mLevel, monster[i].mExp, monster[i].mWhoHit);
	monstkills[monster[i].MType->mtype]++;
	monster[i]._mhitpoints = 0;
	SetRndSeed(monster[i]._mRndSeed);
	if (QuestStatus(QTYPE_GARB) && monster[i].mName == UniqMonst[UMT_GARBUD].mName) {
		CreateTypeItem(monster[i]._mx + 1, monster[i]._my + 1, TRUE, 4, FALSE, TRUE, FALSE);
	} else if (i > 3) {
		SpawnItem(i, monster[i]._mx, monster[i]._my, sendmsg);
	}
	if (monster[i].MType->mtype == MT_DIABLO)
		M_DiabloDeath(i, TRUE);
	else
		PlayEffect(i, 2);

	if (pnum >= 0)
		md = M_GetDir(i);
	else
		md = monster[i]._mdir;
	monster[i]._mdir = md;
	NewMonsterAnim(i, &monster[i].MType->Anims[MA_DEATH], md);
	monster[i]._mmode = MM_DEATH;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mVar1 = 0;
	monster[i]._mx = monster[i]._moldx;
	monster[i]._my = monster[i]._moldy;
	monster[i]._mfutx = monster[i]._moldx;
	monster[i]._mfuty = monster[i]._moldy;
	M_CheckEFlag(i);
	M_ClearSquares(i);
	dMonster[monster[i]._mx][monster[i]._my] = i + 1;
	CheckQuestKill(i, sendmsg);
	M_FallenFear(monster[i]._mx, monster[i]._my);
	if (monster[i].MType->mtype >= MT_NACID && monster[i].MType->mtype <= MT_XACID)
		AddMissile(monster[i]._mx, monster[i]._my, 0, 0, 0, MIS_ACIDPUD, 1, i, monster[i]._mint + 1, 0);
}

void M2MStartKill(int i, int mid)
{
	int md;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("M2MStartKill: Invalid monster (attacker) %d", i);
	}
	if ((DWORD)i >= MAXMONSTERS) { /// BUGFIX: should check `mid`
		app_fatal("M2MStartKill: Invalid monster (killed) %d", mid);
	}
	if (!monster[i].MType)
		app_fatal("M2MStartKill: Monster %d \"%s\" MType NULL", mid, monster[mid].mName);

	delta_kill_monster(mid, monster[mid]._mx, monster[mid]._my, currlevel);
	NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, monster[mid]._mx, monster[mid]._my, mid);

	monster[mid].mWhoHit |= 1 << i;
	if (i < MAX_PLRS)
		AddPlrMonstExper(monster[mid].mLevel, monster[mid].mExp, monster[mid].mWhoHit);

	monstkills[monster[mid].MType->mtype]++;
	monster[mid]._mhitpoints = 0;
	SetRndSeed(monster[mid]._mRndSeed);

	if (mid >= MAX_PLRS)
		SpawnItem(mid, monster[mid]._mx, monster[mid]._my, TRUE);

	if (monster[mid].MType->mtype == MT_DIABLO)
		M_DiabloDeath(mid, TRUE);
	else
		PlayEffect(i, 2);

	PlayEffect(mid, 2);

	md = (monster[i]._mdir - 4) & 7;
	if (monster[mid].MType->mtype == MT_GOLEM)
		md = 0;

	monster[mid]._mdir = md;
	NewMonsterAnim(mid, &monster[mid].MType->Anims[MA_DEATH], md);
	monster[mid]._mmode = MM_DEATH;
	monster[mid]._mxoff = 0;
	monster[mid]._myoff = 0;
	monster[mid]._mx = monster[mid]._moldx;
	monster[mid]._my = monster[mid]._moldy;
	monster[mid]._mfutx = monster[mid]._moldx;
	monster[mid]._mfuty = monster[mid]._moldy;
	M_CheckEFlag(mid);
	M_ClearSquares(mid);
	dMonster[monster[mid]._mx][monster[mid]._my] = mid + 1;
	CheckQuestKill(mid, TRUE);
	M_FallenFear(monster[mid]._mx, monster[mid]._my);
	if (monster[mid].MType->mtype >= MT_NACID && monster[mid].MType->mtype <= MT_XACID)
		AddMissile(monster[mid]._mx, monster[mid]._my, 0, 0, 0, MIS_ACIDPUD, 1, mid, monster[mid]._mint + 1, 0);
}

void M_StartKill(int i, int pnum)
{
	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("M_StartKill: Invalid monster %d", i);
	}

	if (myplr == pnum) {
		delta_kill_monster(i, monster[i]._mx, monster[i]._my, currlevel);
		if (i != pnum) {
			NetSendCmdLocParam1(FALSE, CMD_MONSTDEATH, monster[i]._mx, monster[i]._my, i);
		} else {
			NetSendCmdLocParam1(FALSE, CMD_KILLGOLEM, monster[i]._mx, monster[i]._my, currlevel);
		}
	}

	MonstStartKill(i, pnum, TRUE);
}

void M_SyncStartKill(int i, int x, int y, int pnum)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_SyncStartKill: Invalid monster %d", i);

	if (monster[i]._mhitpoints == 0 || monster[i]._mmode == MM_DEATH) {
		return;
	}

	if (dMonster[x][y] == 0) {
		M_ClearSquares(i);
		monster[i]._mx = x;
		monster[i]._my = y;
		monster[i]._moldx = x;
		monster[i]._moldy = y;
	}

	if (monster[i]._mmode == MM_STONE) {
		MonstStartKill(i, pnum, FALSE);
		monster[i]._mmode = MM_STONE;
	} else {
		MonstStartKill(i, pnum, FALSE);
	}
}

void M_StartFadein(int i, int md, BOOL backwards)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_StartFadein: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_StartFadein: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_FADEIN;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
	monster[i]._mdir = md;
	monster[i]._mFlags &= ~MFLAG_HIDDEN;
	if (backwards) {
		monster[i]._mFlags |= MFLAG_LOCK_ANIMATION;
		monster[i]._mAnimFrame = monster[i]._mAnimLen;
	}
}

void M_StartFadeout(int i, int md, BOOL backwards)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_StartFadeout: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_StartFadeout: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	NewMonsterAnim(i, &monster[i].MType->Anims[MA_SPECIAL], md);
	monster[i]._mmode = MM_FADEOUT;
	monster[i]._mxoff = 0;
	monster[i]._myoff = 0;
	monster[i]._mfutx = monster[i]._mx;
	monster[i]._mfuty = monster[i]._my;
	monster[i]._moldx = monster[i]._mx;
	monster[i]._moldy = monster[i]._my;
	M_CheckEFlag(i);
	monster[i]._mdir = md;
	if (backwards) {
		monster[i]._mFlags |= MFLAG_LOCK_ANIMATION;
		monster[i]._mAnimFrame = monster[i]._mAnimLen;
	}
}

void M_StartHeal(int i)
{
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_StartHeal: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_StartHeal: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	Monst = &monster[i];
	Monst->_mAnimData = Monst->MType->Anims[MA_SPECIAL].Data[Monst->_mdir];
	Monst->_mAnimFrame = Monst->MType->Anims[MA_SPECIAL].Frames;
	Monst->_mFlags |= MFLAG_LOCK_ANIMATION;
	Monst->_mmode = MM_HEAL;
	Monst->_mVar1 = Monst->_mmaxhp / (16 * (random(97, 5) + 4));
}

void M_ChangeLightOffset(int monst)
{
	int lx, ly, _mxoff, _myoff, sign;

	if ((DWORD)monst >= MAXMONSTERS)
		app_fatal("M_ChangeLightOffset: Invalid monster %d", monst);

	lx = monster[monst]._mxoff + 2 * monster[monst]._myoff;
	ly = 2 * monster[monst]._myoff - monster[monst]._mxoff;

	if (lx < 0) {
		sign = -1;
		lx = -lx;
	} else {
		sign = 1;
	}

	_mxoff = sign * (lx >> 3);
	if (ly < 0) {
		_myoff = -1;
		ly = -ly;
	} else {
		_myoff = 1;
	}

	ChangeLightOff(monster[monst].mlid, _mxoff, _myoff * (ly >> 3));
}

int M_DoStand(int i)
{
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoStand: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoStand: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	Monst = &monster[i];
	if (Monst->MType->mtype == MT_GOLEM)
		Monst->_mAnimData = Monst->MType->Anims[MA_WALK].Data[Monst->_mdir];
	else
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];

	if (Monst->_mAnimFrame == Monst->_mAnimLen)
		M_Enemy(i);

	Monst->_mVar2++;

	return FALSE;
}

BOOL M_DoWalk(int i)
{
	BOOL rv;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoWalk: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoWalk: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	rv = FALSE;
	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mx += monster[i]._mVar1;
		monster[i]._my += monster[i]._mVar2;
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
		if (monster[i]._uniqtype != 0)
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		M_StartStand(i, monster[i]._mdir);
		rv = TRUE;
	} else if (!monster[i]._mAnimCnt) {
		monster[i]._mVar8++;
		monster[i]._mVar6 += monster[i]._mxvel;
		monster[i]._mVar7 += monster[i]._myvel;
		monster[i]._mxoff = monster[i]._mVar6 >> 4;
		monster[i]._myoff = monster[i]._mVar7 >> 4;
	}

	if (monster[i]._uniqtype != 0)
		M_ChangeLightOffset(i);

	return rv;
}

BOOL M_DoWalk2(int i)
{
	BOOL rv;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoWalk2: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoWalk2: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mVar1][monster[i]._mVar2] = 0;
		if (monster[i]._uniqtype != 0)
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		M_StartStand(i, monster[i]._mdir);
		rv = TRUE;
	} else {
		if (!monster[i]._mAnimCnt) {
			monster[i]._mVar8++;
			monster[i]._mVar6 += monster[i]._mxvel;
			monster[i]._mVar7 += monster[i]._myvel;
			monster[i]._mxoff = monster[i]._mVar6 >> 4;
			monster[i]._myoff = monster[i]._mVar7 >> 4;
		}
		rv = FALSE;
	}
	if (monster[i]._uniqtype != 0)
		M_ChangeLightOffset(i);

	return rv;
}

BOOL M_DoWalk3(int i)
{
	BOOL rv;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoWalk3: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoWalk3: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	if (monster[i]._mVar8 == monster[i].MType->Anims[MA_WALK].Frames) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mx = monster[i]._mVar1;
		monster[i]._my = monster[i]._mVar2;
		dFlags[monster[i]._mVar4][monster[i]._mVar5] &= ~BFLAG_MONSTLR;
		dMonster[monster[i]._mx][monster[i]._my] = i + 1;
		if (monster[i]._uniqtype)
			ChangeLightXY(monster[i].mlid, monster[i]._mx, monster[i]._my);
		M_StartStand(i, monster[i]._mdir);
		rv = TRUE;
	} else {
		if (!monster[i]._mAnimCnt) {
			monster[i]._mVar8++;
			monster[i]._mVar6 += monster[i]._mxvel;
			monster[i]._mVar7 += monster[i]._myvel;
			monster[i]._mxoff = monster[i]._mVar6 >> 4;
			monster[i]._myoff = monster[i]._mVar7 >> 4;
		}
		rv = FALSE;
	}
	if (monster[i]._uniqtype != 0)
		M_ChangeLightOffset(i);

	return rv;
}

void M_TryM2MHit(int i, int mid, int hper, int mind, int maxd)
{
	BOOL ret;

	if ((DWORD)mid >= MAXMONSTERS) {
		app_fatal("M_TryM2MHit: Invalid monster %d", mid);
	}
	if (monster[mid].MType == NULL)
		app_fatal("M_TryM2MHit: Monster %d \"%s\" MType NULL", mid, monster[mid].mName);
	if (monster[mid]._mhitpoints >> 6 > 0 && (monster[mid].MType->mtype != MT_ILLWEAV || monster[mid]._mgoal != MGOAL_RETREAT)) {
		int hit = random(4, 100);
		if (monster[mid]._mmode == MM_STONE)
			hit = 0;
		if (!CheckMonsterHit(mid, &ret) && hit < hper) {
			int dam = (mind + random(5, maxd - mind + 1)) << 6;
			monster[mid]._mhitpoints -= dam;
			if (monster[mid]._mhitpoints >> 6 <= 0) {
				if (monster[mid]._mmode == MM_STONE) {
					M2MStartKill(i, mid);
					monster[mid]._mmode = MM_STONE;
				} else {
					M2MStartKill(i, mid);
				}
			} else {
				if (monster[mid]._mmode == MM_STONE) {
					M2MStartHit(mid, i, dam);
					monster[mid]._mmode = MM_STONE;
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
	int dx, dy;
	int blk, blkper;
	int dam, mdam;
	int newx, newy;
	int j, misnum, ms_num, cur_ms_num, new_hp;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_TryH2HHit: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_TryH2HHit: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i]._mFlags & MFLAG_TARGETS_MONSTER) {
		M_TryM2MHit(i, pnum, Hit, MinDam, MaxDam);
		return;
	}
	if (plr[pnum]._pHitPoints >> 6 <= 0 || plr[pnum]._pInvincible || plr[pnum]._pSpellFlags & 1)
		return;
	dx = abs(monster[i]._mx - plr[pnum].WorldX);
	dy = abs(monster[i]._my - plr[pnum].WorldY);
	if (dx >= 2 || dy >= 2)
		return;

	hper = random(98, 100);
#ifdef _DEBUG
	if (debug_mode_dollar_sign || debug_mode_key_inverted_v)
		hper = 1000;
#endif
	hit = Hit
	    + 2 * (monster[i].mLevel - plr[pnum]._pLevel)
	    + 30
	    - plr[pnum]._pIBonusAC
	    - plr[pnum]._pIAC
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
		blkper = random(98, 100);
	} else {
		blkper = 100;
	}
	blk = plr[pnum]._pDexterity
	    + plr[pnum]._pBaseToBlk
	    - (monster[i].mLevel << 1)
	    + (plr[pnum]._pLevel << 1);
	if (blk < 0)
		blk = 0;
	if (blk > 100)
		blk = 100;
	if (hper >= hit)
		return;
	if (blkper < blk) {
		StartPlrBlock(pnum, GetDirection(plr[pnum].WorldX, plr[pnum].WorldY, monster[i]._mx, monster[i]._my));
		return;
	}
	if (monster[i].MType->mtype == MT_YZOMBIE && pnum == myplr) {
		ms_num = -1;
		cur_ms_num = -1;
		for (j = 0; j < nummissiles; j++) {
			misnum = missileactive[j];
			if (missile[misnum]._mitype != MIS_MANASHIELD)
				continue;
			if (missile[misnum]._misource == pnum)
				cur_ms_num = misnum;
			else
				ms_num = misnum;
		}
		if (plr[pnum]._pMaxHP > 64) {
			if (plr[pnum]._pMaxHPBase > 64) {
				new_hp = plr[pnum]._pMaxHP - 64;
				plr[pnum]._pMaxHP = new_hp;
				if (plr[pnum]._pHitPoints > new_hp) {
					plr[pnum]._pHitPoints = new_hp;
					if (cur_ms_num >= 0)
						missile[cur_ms_num]._miVar1 = new_hp;
				}
				new_hp = plr[pnum]._pMaxHPBase - 64;
				plr[pnum]._pMaxHPBase = new_hp;
				if (plr[pnum]._pHPBase > new_hp) {
					plr[pnum]._pHPBase = new_hp;
					if (cur_ms_num >= 0)
						missile[cur_ms_num]._miVar2 = new_hp;
				}
			}
		}
	}
	dam = (MinDam << 6) + random(99, (MaxDam - MinDam + 1) << 6);
	dam += (plr[pnum]._pIGetHit << 6);
	if (dam < 64)
		dam = 64;
	if (pnum == myplr) {
		plr[pnum]._pHitPoints -= dam;
		plr[pnum]._pHPBase -= dam;
	}
	if (plr[pnum]._pIFlags & ISPL_THORNS) {
		mdam = (random(99, 3) + 1) << 6;
		monster[i]._mhitpoints -= mdam;
		if (monster[i]._mhitpoints >> 6 <= 0)
			M_StartKill(i, pnum);
		else
			M_StartHit(i, pnum, mdam);
	}
	if (!(monster[i]._mFlags & MFLAG_NOLIFESTEAL) && monster[i].MType->mtype == MT_SKING && gbMaxPlayers != 1)
		monster[i]._mhitpoints += dam;
	if (plr[pnum]._pHitPoints > plr[pnum]._pMaxHP) {
		plr[pnum]._pHitPoints = plr[pnum]._pMaxHP;
		plr[pnum]._pHPBase = plr[pnum]._pMaxHPBase;
	}
	if (plr[pnum]._pHitPoints >> 6 <= 0) {
		SyncPlrKill(pnum, 0);
		return;
	}
	StartPlrHit(pnum, dam, 0);
	if (monster[i]._mFlags & MFLAG_KNOCKBACK) {
		if (plr[pnum]._pmode != PM_GOTHIT)
			StartPlrHit(pnum, 0, TRUE);
		newx = plr[pnum].WorldX + offset_x[monster[i]._mdir];
		newy = plr[pnum].WorldY + offset_y[monster[i]._mdir];
		if (PosOkPlayer(pnum, newx, newy)) {
			plr[pnum].WorldX = newx;
			plr[pnum].WorldY = newy;
			FixPlayerLocation(pnum, plr[pnum]._pdir);
			FixPlrWalkTags(pnum);
			dPlayer[newx][newy] = pnum + 1;
			SetPlayerOld(pnum);
		}
	}
}

BOOL M_DoAttack(int i)
{
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoAttack: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->MType == NULL)
		app_fatal("M_DoAttack: Monster %d \"%s\" MType NULL", i, Monst->mName);
	if (Monst->MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoAttack: Monster %d \"%s\" MData NULL", i, Monst->mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit, monster[i].mMinDamage, monster[i].mMaxDamage);
		if (monster[i]._mAi != AI_SNAKE)
			PlayEffect(i, 0);
	}
	if (monster[i].MType->mtype >= MT_NMAGMA && monster[i].MType->mtype <= MT_WMAGMA && monster[i]._mAnimFrame == 9) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit + 10, monster[i].mMinDamage - 2, monster[i].mMaxDamage - 2);
		PlayEffect(i, 0);
	}
	if (monster[i].MType->mtype >= MT_STORM && monster[i].MType->mtype <= MT_MAEL && monster[i]._mAnimFrame == 13) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit - 20, monster[i].mMinDamage + 4, monster[i].mMaxDamage + 4);
		PlayEffect(i, 0);
	}
	if (monster[i]._mAi == AI_SNAKE && monster[i]._mAnimFrame == 1)
		PlayEffect(i, 0);
	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL M_DoRAttack(int i)
{
	int multimissiles, mi;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoRAttack: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoRAttack: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i].MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoRAttack: Monster %d \"%s\" MData NULL", i, monster[i].mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum) {
		if (monster[i]._mVar1 != -1) {
			if (monster[i]._mVar1 != MIS_CBOLT)
				multimissiles = 1;
			else
				multimissiles = 3;
			for (mi = 0; mi < multimissiles; mi++) {
				AddMissile(
				    monster[i]._mx,
				    monster[i]._my,
				    monster[i]._menemyx,
				    monster[i]._menemyy,
				    monster[i]._mdir,
				    monster[i]._mVar1,
				    1,
				    i,
				    monster[i]._mVar2,
				    0);
			}
		}
		PlayEffect(i, 0);
	}

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

int M_DoRSpAttack(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoRSpAttack: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoRSpAttack: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i].MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoRSpAttack: Monster %d \"%s\" MData NULL", i, monster[i].mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2 && !monster[i]._mAnimCnt) {
		AddMissile(
		    monster[i]._mx,
		    monster[i]._my,
		    monster[i]._menemyx,
		    monster[i]._menemyy,
		    monster[i]._mdir,
		    monster[i]._mVar1,
		    1,
		    i,
		    monster[i]._mVar3,
		    0);
		PlayEffect(i, 3);
	}

	if (monster[i]._mAi == AI_MEGA && monster[i]._mAnimFrame == 3) {
		int hadV2 = monster[i]._mVar2;
		monster[i]._mVar2++;
		if (hadV2 == 0) {
			monster[i]._mFlags |= MFLAG_ALLOW_SPECIAL;
		} else if (monster[i]._mVar2 == 15) {
			monster[i]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
	}

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL M_DoSAttack(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoSAttack: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoSAttack: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i].MType == NULL) // BUGFIX: should check MData
		app_fatal("M_DoSAttack: Monster %d \"%s\" MData NULL", i, monster[i].mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2)
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit2, monster[i].mMinDamage2, monster[i].mMaxDamage2);

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL M_DoFadein(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoFadein: Invalid monster %d", i);

	if ((!(monster[i]._mFlags & MFLAG_LOCK_ANIMATION) || monster[i]._mAnimFrame != 1)
	    && (monster[i]._mFlags & MFLAG_LOCK_ANIMATION || monster[i]._mAnimFrame != monster[i]._mAnimLen)) {
		return FALSE;
	}

	M_StartStand(i, monster[i]._mdir);
	monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;

	return TRUE;
}

BOOL M_DoFadeout(int i)
{
	int mt;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoFadeout: Invalid monster %d", i);

	if ((!(monster[i]._mFlags & MFLAG_LOCK_ANIMATION) || monster[i]._mAnimFrame != 1)
	    && (monster[i]._mFlags & MFLAG_LOCK_ANIMATION || monster[i]._mAnimFrame != monster[i]._mAnimLen)) {
		return FALSE;
	}

	mt = monster[i].MType->mtype;
	if (mt < MT_INCIN || mt > MT_HELLBURN) {
		monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;
		monster[i]._mFlags |= MFLAG_HIDDEN;
	} else {
		monster[i]._mFlags &= ~MFLAG_LOCK_ANIMATION;
	}

	M_StartStand(i, monster[i]._mdir);

	return TRUE;
}

int M_DoHeal(int i)
{
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoHeal: Invalid monster %d", i);
	Monst = monster + i;
	if (monster[i]._mFlags & MFLAG_NOHEAL) {
		Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		Monst->_mmode = MM_SATTACK;
		return FALSE;
	}

	if (Monst->_mAnimFrame == 1) {
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
	return FALSE;
}

int M_DoTalk(int i)
{
	MonsterStruct *Monst;
	int tren;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoTalk: Invalid monster %d", i);

	Monst = monster + i;
	M_StartStand(i, Monst->_mdir);
	monster[i]._mgoal = MGOAL_TALKING;
	if (effect_is_playing(alltext[Monst->mtalkmsg].sfxnr))
		return FALSE;
	InitQTextMsg(Monst->mtalkmsg);
	if (Monst->mName == UniqMonst[UMT_GARBUD].mName) {
		if (Monst->mtalkmsg == QUEST_GARBUD1)
			quests[QTYPE_GARB]._qactive = 2;
		quests[QTYPE_GARB]._qlog = TRUE;
		if (Monst->mtalkmsg == QUEST_GARBUD2 && !(Monst->_mFlags & MFLAG_QUEST_COMPLETE)) {
			SpawnItem(i, Monst->_mx + 1, Monst->_my + 1, TRUE);
			Monst->_mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (Monst->mName == UniqMonst[UMT_ZHAR].mName
	    && Monst->mtalkmsg == QUEST_ZHAR1
	    && !(Monst->_mFlags & MFLAG_QUEST_COMPLETE)) {
		quests[QTYPE_ZHAR]._qactive = 2;
		quests[QTYPE_ZHAR]._qlog = TRUE;
		CreateTypeItem(Monst->_mx + 1, Monst->_my + 1, FALSE, 0, 24, TRUE, 0);
		Monst->_mFlags |= MFLAG_QUEST_COMPLETE;
	}
	if (Monst->mName == UniqMonst[UMT_SNOTSPIL].mName) {
		if (Monst->mtalkmsg == QUEST_BANNER10 && !(Monst->_mFlags & MFLAG_QUEST_COMPLETE)) {
			ObjChangeMap(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 2, (setpc_h >> 1) + setpc_y - 2);
			tren = TransVal;
			TransVal = 9;
			DRLG_MRectTrans(setpc_x, setpc_y, (setpc_w >> 1) + setpc_x + 4, setpc_y + (setpc_h >> 1));
			TransVal = tren;
			quests[QTYPE_BOL]._qvar1 = 2;
			if (quests[QTYPE_BOL]._qactive == 1)
				quests[QTYPE_BOL]._qactive = 2;
			Monst->_mFlags |= MFLAG_QUEST_COMPLETE;
		}
		if (quests[QTYPE_BOL]._qvar1 < 2) {
			sprintf(tempstr, "SS Talk = %i, Flags = %i", Monst->mtalkmsg, Monst->_mFlags);
			app_fatal(tempstr);
		}
	}
	if (Monst->mName == UniqMonst[UMT_LACHDAN].mName) {
		if (Monst->mtalkmsg == QUEST_VEIL9) {
			quests[QTYPE_VEIL]._qactive = 2;
			quests[QTYPE_VEIL]._qlog = TRUE;
		}
		if (Monst->mtalkmsg == QUEST_VEIL11 && !(Monst->_mFlags & MFLAG_QUEST_COMPLETE)) {
			SpawnUnique(UITEM_STEELVEIL, Monst->_mx + 1, Monst->_my + 1);
			Monst->_mFlags |= MFLAG_QUEST_COMPLETE;
		}
	}
	if (Monst->mName == UniqMonst[UMT_WARLORD].mName)
		quests[QTYPE_WARLRD]._qvar1 = 2;
	if (Monst->mName == UniqMonst[UMT_LAZURUS].mName && gbMaxPlayers != 1) {
		Monst->_msquelch = UCHAR_MAX;
		Monst->mtalkmsg = 0;
		quests[QTYPE_VB]._qvar1 = 6;
		Monst->_mgoal = MGOAL_NORMAL;
	}
	return FALSE;
}

void M_Teleport(int i)
{
	BOOL tren;
	MonsterStruct *Monst;
	int k, j, x, y, _mx, _my, rx, ry;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_Teleport: Invalid monster %d", i);

	tren = FALSE;

	Monst = &monster[i];
	if (Monst->_mmode != MM_STONE) {
		_mx = Monst->_menemyx;
		_my = Monst->_menemyy;
		rx = 2 * random(100, 2) - 1;
		ry = 2 * random(100, 2) - 1;

		for (j = -1; j <= 1 && !tren; j++) {
			for (k = -1; k < 1 && !tren; k++) {
				if (j != 0 || k != 0) {
					x = _mx + rx * j;
					y = _my + ry * k;
					if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX && x != Monst->_mx && y != Monst->_my) {
						if (PosOkMonst(i, x, y))
							tren = TRUE;
					}
				}
			}
		}
	}

	if (tren) {
		M_ClearSquares(i);
		dMonster[Monst->_mx][Monst->_my] = 0;
		dMonster[x][y] = i + 1;
		Monst->_moldx = x;
		Monst->_moldy = y;
		Monst->_mdir = M_GetDir(i);
		M_CheckEFlag(i);
	}
}

BOOL M_DoGotHit(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoGotHit: Invalid monster %d", i);

	if (monster[i].MType == NULL)
		app_fatal("M_DoGotHit: Monster %d \"%s\" MType NULL", i, monster[i].mName);
	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);

		return TRUE;
	}

	return FALSE;
}

void M_UpdateLeader(int i)
{
	int ma, j;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_UpdateLeader: Invalid monster %d", i);

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
	BOOL bMusicOn;
	int musicVolume;

	if (gbMaxPlayers > 1) {
		SNetLeaveGame(0x40000004);
	}

	music_stop();

	if (gbMaxPlayers > 1) {
		Sleep(1000);
	}

	if (plr[myplr]._pClass == PC_WARRIOR) {
		play_movie("gendata\\DiabVic2.smk", 0);
	} else if (plr[myplr]._pClass == PC_SORCERER) {
		play_movie("gendata\\DiabVic1.smk", 0);
	} else {
		play_movie("gendata\\DiabVic3.smk", 0);
	}
	play_movie("gendata\\Diabend.smk", 0);

	bMusicOn = gbMusicOn;
	gbMusicOn = TRUE;

	musicVolume = sound_get_or_set_music_volume(1);
	sound_get_or_set_music_volume(0);

	music_start(2);
	loop_movie = TRUE;
	play_movie("gendata\\loopdend.smk", 1);
	loop_movie = FALSE;
	music_stop();

	sound_get_or_set_music_volume(musicVolume);
	gbMusicOn = bMusicOn;
}

void PrepDoEnding()
{
	int newKillLevel, i;
	DWORD *killLevel;

	gbSoundOn = sgbSaveSoundOn;
	gbRunGame = FALSE;
	deathflag = FALSE;
	cineflag = TRUE;

	killLevel = &plr[myplr].pDiabloKillLevel;
	newKillLevel = gnDifficulty + 1;
	if (*killLevel > newKillLevel)
		newKillLevel = *killLevel;
	plr[myplr].pDiabloKillLevel = newKillLevel;

	for (i = 0; i < MAX_PLRS; i++) {
		plr[i]._pmode = PM_QUIT;
		plr[i]._pInvincible = TRUE;
		if (gbMaxPlayers > 1) {
			if (plr[i]._pHitPoints >> 6 == 0)
				plr[i]._pHitPoints = 64;
			if (plr[i]._pMana >> 6 == 0)
				plr[i]._pMana = 64;
		}
	}
}

BOOL M_DoDeath(int i)
{
	int var1;
	int x, y;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoDeath: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoDeath: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	monster[i]._mVar1++;
	var1 = monster[i]._mVar1;
	if (monster[i].MType->mtype == MT_DIABLO) {
		x = monster[i]._mx - ViewX;
		if (x < 0)
			x = -1;
		else
			x = x > 0;
		ViewX += x;

		y = monster[i]._my - ViewY;
		if (y < 0) {
			y = -1;
		} else {
			y = y > 0;
		}
		ViewY += y;

		if (var1 == 140)
			PrepDoEnding();
	} else if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		if (monster[i]._uniqtype == 0)
			AddDead(monster[i]._mx, monster[i]._my, monster[i].MType->mdeadval, (direction)monster[i]._mdir);
		else
			AddDead(monster[i]._mx, monster[i]._my, monster[i]._udeadval, (direction)monster[i]._mdir);

		monster[i]._mDelFlag = TRUE;
		dMonster[monster[i]._mx][monster[i]._my] = 0;

		M_UpdateLeader(i);
	}
	return FALSE;
}

BOOL M_DoSpStand(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoSpStand: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoSpStand: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	if (monster[i]._mAnimFrame == monster[i].MData->mAFNum2)
		PlayEffect(i, 3);

	if (monster[i]._mAnimFrame == monster[i]._mAnimLen) {
		M_StartStand(i, monster[i]._mdir);
		return TRUE;
	}

	return FALSE;
}

BOOL M_DoDelay(int i)
{
	int mVar2;
	int oFrame;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoDelay: Invalid monster %d", i);
	if (monster[i].MType == NULL)
		app_fatal("M_DoDelay: Monster %d \"%s\" MType NULL", i, monster[i].mName);

	monster[i]._mAnimData = monster[i].MType->Anims[MA_STAND].Data[M_GetDir(i)];
	if (monster[i]._mAi == AI_LAZURUS) {
		if (monster[i]._mVar2 > 8 || monster[i]._mVar2 < 0)
			monster[i]._mVar2 = 8;
	}

	mVar2 = monster[i]._mVar2;
	monster[i]._mVar2--;

	if (!mVar2) {
		oFrame = monster[i]._mAnimFrame;
		M_StartStand(i, monster[i]._mdir);
		monster[i]._mAnimFrame = oFrame;
		return TRUE;
	}

	return FALSE;
}

BOOL M_DoStone(int i)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_DoStone: Invalid monster %d", i);

	if (!monster[i]._mhitpoints) {
		dMonster[monster[i]._mx][monster[i]._my] = 0;
		monster[i]._mDelFlag = TRUE;
	}

	return FALSE;
}

void M_WalkDir(int i, int md)
{
	int mwi;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_WalkDir: Invalid monster %d", i);

	mwi = monster[i].MType->Anims[MA_WALK].Frames - 1;
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
	}
}

void GroupUnity(int i)
{
	int leader, m, j;
	BOOL clear;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("GroupUnity: Invalid monster %d", i);

	if (monster[i].leaderflag) {
		leader = monster[i].leader;
		clear = LineClearF(CheckNoSolid, monster[i]._mx, monster[i]._my, monster[leader]._mfutx, monster[leader]._mfuty);
		if (clear || monster[i].leaderflag != 1) {
			if (clear
			    && monster[i].leaderflag == 2
			    && abs(monster[i]._mx - monster[leader]._mfutx) < 4
			    && abs(monster[i]._my - monster[leader]._mfuty) < 4) {
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
			monster[leader]._lastx = monster[i]._mx;
			monster[leader]._lasty = monster[i]._my;
			monster[leader]._msquelch = monster[i]._msquelch - 1;
		}
		if (monster[leader]._mAi == AI_GARG) {
			if (monster[leader]._mFlags & MFLAG_ALLOW_SPECIAL) {
				monster[leader]._mmode = MM_SATTACK;
				monster[leader]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
			}
		}
		return;
	}
	if (monster[i]._uniqtype != 0) {
		if (UniqMonst[monster[i]._uniqtype - 1].mUnqAttr & 2) {
			for (j = 0; j < nummonsters; j++) {
				m = monstactive[j];
				if (monster[m].leaderflag == 1 && monster[m].leader == i) {
					if (monster[i]._msquelch > monster[m]._msquelch) {
						monster[m]._lastx = monster[i]._mx;
						monster[m]._lasty = monster[i]._my;
						monster[m]._msquelch = monster[i]._msquelch - 1;
					}
					if (monster[m]._mAi == AI_GARG) {
						if (monster[m]._mFlags & MFLAG_ALLOW_SPECIAL) {
							monster[m]._mmode = MM_SATTACK;
							monster[m]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
						}
					}
				}
			}
		}
	}
}

BOOL M_CallWalk(int i, int md)
{
	int mdtemp;
	BOOL ok;

	mdtemp = md;
	ok = DirOK(i, md);
	if (random(101, 2))
		ok = ok || (md = left[mdtemp], DirOK(i, md)) || (md = right[mdtemp], DirOK(i, md));
	else
		ok = ok || (md = right[mdtemp], DirOK(i, md)) || (md = left[mdtemp], DirOK(i, md));
	if (random(102, 2))
		ok = ok
		    || (md = right[right[mdtemp]], DirOK(i, md))
		    || (md = left[left[mdtemp]], DirOK(i, md));
	else
		ok = ok
		    || (md = left[left[mdtemp]], DirOK(i, md))
		    || (md = right[right[mdtemp]], DirOK(i, md));
	if (ok)
		M_WalkDir(i, md);
	return ok;
}

BOOL M_PathWalk(int i)
{
	char path[25];
	BOOL(*Check)
	(int, int, int);

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("M_PathWalk: Invalid monster %d", i);

	Check = PosOkMonst3;
	if (!(monster[i]._mFlags & MFLAG_CAN_OPEN_DOOR))
		Check = PosOkMonst;

	if (FindPath(Check, i, monster[i]._mx, monster[i]._my, monster[i]._menemyx, monster[i]._menemyy, path)) {
		M_CallWalk(i, plr2monst[path[0]]); /* plr2monst is local */
		return TRUE;
	}

	return FALSE;
}

BOOL M_CallWalk2(int i, int md)
{
	BOOL ok;
	int mdtemp;

	mdtemp = md;
	ok = DirOK(i, md);    // Can we continue in the same direction
	if (random(101, 2)) { // Randomly go left or right
		ok = ok || (mdtemp = left[md], DirOK(i, left[md])) || (mdtemp = right[md], DirOK(i, right[md]));
	} else {
		ok = ok || (mdtemp = right[md], DirOK(i, right[md])) || (mdtemp = left[md], DirOK(i, left[md]));
	}

	if (ok)
		M_WalkDir(i, mdtemp);

	return ok;
}

BOOL M_DumbWalk(int i, int md)
{
	BOOL ok;
	ok = DirOK(i, md);
	if (ok)
		M_WalkDir(i, md);

	return ok;
}

BOOL M_RoundWalk(int i, int md, int *dir)
{
	int mdtemp;
	BOOL ok;
	if (*dir)
		md = left[left[md]];
	else
		md = right[right[md]];

	ok = DirOK(i, md);
	mdtemp = md;
	if (!ok) {
		if (*dir) {
			md = right[mdtemp];
			ok = DirOK(i, md) || (md = right[right[mdtemp]], DirOK(i, md));
		} else {
			md = left[mdtemp];
			ok = (DirOK(i, md) || (md = left[left[mdtemp]], DirOK(i, md)));
		}
	}
	if (ok) {
		M_WalkDir(i, md);
	} else {
		*dir = !*dir;
		ok = M_CallWalk(i, opposite[mdtemp]);
	}
	return ok;
}

void MAI_Zombie(int i)
{
	MonsterStruct *Monst;
	int mx, my;
	int md, v;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("MAI_Zombie: Invalid monster %d", i);
	}

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	if (!(dFlags[mx][my] & BFLAG_VISIBLE)) {
		return;
	}

	mx = mx - Monst->_menemyx;
	my = my - Monst->_menemyy;
	md = Monst->_mdir;
	v = random(103, 100);
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (v < 2 * Monst->_mint + 10) {
			if (abs(mx) >= 2 * Monst->_mint + 4 || abs(my) >= 2 * Monst->_mint + 4) {
				if (random(104, 100) < 2 * Monst->_mint + 20) {
					md = random(104, 8);
				}
				M_DumbWalk(i, md);
			} else {
				md = M_GetDir(i);
				M_CallWalk(i, md);
			}
		}
	} else if (v < 2 * Monst->_mint + 10) {
		M_StartAttack(i);
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
}

void MAI_SkelSd(int i)
{
	MonsterStruct *Monst;
	int mx, my, x, y, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_SkelSd: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	x = mx - Monst->_menemyx;
	y = my - Monst->_menemyy;
	md = GetDirection(mx, my, Monst->_lastx, Monst->_lasty);
	Monst->_mdir = md;
	if (abs(x) >= 2 || abs(y) >= 2) {
		if (Monst->_mVar1 == MM_DELAY || (random(106, 100) >= 35 - 4 * Monst->_mint)) {
			M_CallWalk(i, md);
		} else {
			M_StartDelay(i, 15 - 2 * Monst->_mint + random(106, 10));
		}
	} else {
		if (Monst->_mVar1 == MM_DELAY || (random(105, 100) < 2 * Monst->_mint + 20)) {
			M_StartAttack(i);
		} else {
			M_StartDelay(i, 2 * (5 - Monst->_mint) + random(105, 10));
		}
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

BOOL MAI_Path(int i)
{
	MonsterStruct *Monst;
	BOOL clear;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Path: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->MType->mtype != MT_GOLEM) {
		if (Monst->_msquelch == 0)
			return FALSE;
		if (Monst->_mmode != MM_STAND)
			return FALSE;
		if (Monst->_mgoal != MGOAL_NORMAL && Monst->_mgoal != MGOAL_MOVE && Monst->_mgoal != MGOAL_SHOOT)
			return FALSE;
		if (Monst->_mx == 1 && Monst->_my == 0)
			return FALSE;
	}

	clear = LineClearF1(
	    PosOkMonst2,
	    i,
	    Monst->_mx,
	    Monst->_my,
	    Monst->_menemyx,
	    Monst->_menemyy);
	if (!clear || Monst->_pathcount >= 5 && Monst->_pathcount < 8) {
		if (Monst->_mFlags & MFLAG_CAN_OPEN_DOOR)
			MonstCheckDoors(i);
		Monst->_pathcount++;
		if (Monst->_pathcount < 5)
			return FALSE;
		if (M_PathWalk(i))
			return TRUE;
	}

	if (Monst->MType->mtype != MT_GOLEM)
		Monst->_pathcount = 0;

	return FALSE;
}

void MAI_Snake(int i)
{
	MonsterStruct *Monst;
	int fx, fy, mx, my, md;
	int pnum;
	int tmp;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Snake: Invalid monster %d", i);
	char pattern[6] = { 1, 1, 0, -1, -1, 0 };
	Monst = monster + i;
	pnum = Monst->_menemy;
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0)
		return;
	fx = Monst->_menemyx;
	fy = Monst->_menemyy;
	mx = Monst->_mx - fx;
	my = Monst->_my - fy;
	md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
	Monst->_mdir = md;
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (abs(mx) < 3 && abs(my) < 3 && LineClearF1(PosOkMonst, i, Monst->_mx, Monst->_my, fx, fy) && Monst->_mVar1 != MM_CHARGE) {
			if (AddMissile(Monst->_mx, Monst->_my, fx, fy, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
				PlayEffect(i, 0);
				Monst->_mmode = MM_CHARGE;
				dMonster[Monst->_mx][Monst->_my] = -1 - i;
			}
		} else if (Monst->_mVar1 == MM_DELAY || random(106, 100) >= 35 - 2 * Monst->_mint) {
			if (md + pattern[Monst->_mgoalvar1] < 0) {
				tmp = md + pattern[Monst->_mgoalvar1] + 8;
			} else {
				tmp = md + pattern[Monst->_mgoalvar1] - 8;
				if (md + pattern[Monst->_mgoalvar1] < 8)
					tmp = md + pattern[Monst->_mgoalvar1];
			}
			Monst->_mgoalvar1++;
			if (Monst->_mgoalvar1 > 5)
				Monst->_mgoalvar1 = 0;
			if (tmp - Monst->_mgoalvar2 < 0) {
				md = tmp - Monst->_mgoalvar2 + 8;
			} else if (tmp - Monst->_mgoalvar2 >= 8) {
				md = tmp - Monst->_mgoalvar2 - 8;
			} else
				md = tmp - Monst->_mgoalvar2;
			if (md > 0) {
				if (md < 4) {
					if (Monst->_mgoalvar2 + 1 < 0) {
						md = Monst->_mgoalvar2 + 9;
					} else if (Monst->_mgoalvar2 + 1 >= 8) {
						md = Monst->_mgoalvar2 - 7;
					} else
						md = Monst->_mgoalvar2 + 1;
					Monst->_mgoalvar2 = md;
				} else if (md == 4) {
					Monst->_mgoalvar2 = tmp;
				} else {
					if (Monst->_mgoalvar2 - 1 < 0) {
						md = Monst->_mgoalvar2 + 7;
					} else if (Monst->_mgoalvar2 - 1 >= 8) {
						md = Monst->_mgoalvar2 - 9;
					} else
						md = Monst->_mgoalvar2 - 1;
					Monst->_mgoalvar2 = md;
				}
			}
			if (!M_DumbWalk(i, Monst->_mgoalvar2))
				M_CallWalk2(i, Monst->_mdir);
		} else {
			M_StartDelay(i, 15 - Monst->_mint + random(106, 10));
		}
	} else {
		if (Monst->_mVar1 == MM_DELAY
		    || Monst->_mVar1 == MM_CHARGE
		    || (random(105, 100) < Monst->_mint + 20)) {
			M_StartAttack(i);
		} else
			M_StartDelay(i, 10 - Monst->_mint + random(105, 10));
	}
	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
}

void MAI_Bat(int i)
{
	MonsterStruct *Monst;
	int md, v, pnum;
	int fx, fy, xd, yd;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Bat: Invalid monster %d", i);

	Monst = &monster[i];
	pnum = Monst->_menemy;
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	xd = Monst->_mx - Monst->_menemyx;
	yd = Monst->_my - Monst->_menemyy;
	md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
	Monst->_mdir = md;
	v = random(107, 100);
	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (!Monst->_mgoalvar1) {
			M_CallWalk(i, opposite[md]);
			Monst->_mgoalvar1++;
		} else {
			if (random(108, 2))
				M_CallWalk(i, left[md]);
			else
				M_CallWalk(i, right[md]);
			Monst->_mgoal = MGOAL_NORMAL;
		}
		return;
	}

	fx = Monst->_menemyx;
	fy = Monst->_menemyy;
	if (Monst->MType->mtype == MT_GLOOM
	    && (abs(xd) >= 5 || abs(yd) >= 5)
	    && v < 4 * Monst->_mint + 33
	    && LineClearF1(PosOkMonst, i, Monst->_mx, Monst->_my, fx, fy)) {
		if (AddMissile(Monst->_mx, Monst->_my, fx, fy, md, MIS_RHINO, pnum, i, 0, 0) != -1) {
			dMonster[Monst->_mx][Monst->_my] = -(i + 1);
			Monst->_mmode = MM_CHARGE;
		}
	} else if (abs(xd) >= 2 || abs(yd) >= 2) {
		if (Monst->_mVar2 > 20 && v < Monst->_mint + 13
		    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < Monst->_mint + 63) {
			M_CallWalk(i, md);
		}
	} else if (v < 4 * Monst->_mint + 8) {
		M_StartAttack(i);
		Monst->_mgoal = MGOAL_RETREAT;
		Monst->_mgoalvar1 = 0;
		if (Monst->MType->mtype == MT_FAMILIAR) {
			AddMissile(Monst->_menemyx, Monst->_menemyy, Monst->_menemyx + 1, 0, -1, MIS_LIGHTNING, 1, i, random(109, 10) + 1, 0);
		}
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_SkelBow(int i)
{
	MonsterStruct *Monst;
	int mx, my, md, v;
	BOOL walking;

	walking = FALSE;
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_SkelBow: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	mx = Monst->_mx - Monst->_menemyx;
	my = Monst->_my - Monst->_menemyy;

	md = M_GetDir(i);
	Monst->_mdir = md;
	v = random(110, 100);

	if (abs(mx) < 4 && abs(my) < 4) {
		if (Monst->_mVar2 > 20 && v < 2 * Monst->_mint + 13
		    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < 2 * Monst->_mint + 63) {
			walking = M_DumbWalk(i, opposite[md]);
		}
	}

	mx = Monst->_menemyx;
	my = Monst->_menemyy;
	if (!walking) {
		if (random(110, 100) < 2 * Monst->_mint + 3) {
			if (LineClear(Monst->_mx, Monst->_my, mx, my))
				M_StartRAttack(i, MIS_ARROW, 4);
		}
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_Fat(int i)
{
	MonsterStruct *Monst;
	int mx, my, md, v;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Fat: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	mx = Monst->_mx - Monst->_menemyx;
	my = Monst->_my - Monst->_menemyy;
	md = M_GetDir(i);
	Monst->_mdir = md;
	v = random(111, 100);
	if (abs(mx) >= 2 || abs(my) >= 2) {
		if (Monst->_mVar2 > 20 && v < 4 * Monst->_mint + 20
		    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
		        && Monst->_mVar2 == 0
		        && v < 4 * Monst->_mint + 70) {
			M_CallWalk(i, md);
		}
	} else if (v < 4 * Monst->_mint + 15) {
		M_StartAttack(i);
	} else if (v < 4 * Monst->_mint + 20) {
		M_StartSpAttack(i);
	}

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_Sneak(int i)
{
	MonsterStruct *Monst;
	int mx, my, md;
	int dist, v;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("MAI_Sneak: Invalid monster %d", i);
	}

	Monst = monster + i;
	if (Monst->_mmode == MM_STAND) {
		mx = Monst->_mx;
		my = Monst->_my;
		if (dLight[mx][my] != lightmax) {
			mx -= Monst->_menemyx;
			my -= Monst->_menemyy;

			md = M_GetDir(i);
			dist = 5 - Monst->_mint;
			if (Monst->_mVar1 == MM_GOTHIT) {
				Monst->_mgoalvar1 = 0;
				Monst->_mgoal = MGOAL_RETREAT;
			} else {
				if (abs(mx) >= dist + 3 || abs(my) >= dist + 3 || Monst->_mgoalvar1 > 8) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoal = MGOAL_NORMAL;
				}
			}
			if (Monst->_mgoal == MGOAL_RETREAT) {
				if (Monst->_mFlags & MFLAG_TARGETS_MONSTER)
					md = GetDirection(Monst->_mx, Monst->_my, plr[Monst->_menemy]._pownerx, plr[Monst->_menemy]._pownery);
				md = opposite[md];
				if (Monst->MType->mtype == MT_UNSEEN) {
					if (random(112, 2))
						md = left[md];
					else
						md = right[md];
				}
			}
			Monst->_mdir = md;
			v = random(112, 100);
			if (abs(mx) < dist && abs(my) < dist && Monst->_mFlags & MFLAG_HIDDEN) {
				M_StartFadein(i, md, FALSE);
			} else {
				if ((abs(mx) >= dist + 1 || abs(my) >= dist + 1) && !(Monst->_mFlags & MFLAG_HIDDEN)) {
					M_StartFadeout(i, md, TRUE);
				} else {
					if (Monst->_mgoal == MGOAL_RETREAT
					    || (abs(mx) >= 2 || abs(my) >= 2) && (Monst->_mVar2 > 20 && v < 4 * Monst->_mint + 14 || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3) && Monst->_mVar2 == 0 && v < 4 * Monst->_mint + 64)) {
						Monst->_mgoalvar1++;
						M_CallWalk(i, md);
					}
				}
			}
			if (Monst->_mmode == MM_STAND) {
				if (abs(mx) >= 2 || abs(my) >= 2 || v >= 4 * Monst->_mint + 10)
					Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
				else
					M_StartAttack(i);
			}
		}
	}
}

void MAI_Fireman(int i)
{
	int xd, yd;
	int md, pnum;
	int fx, fy;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Fireman: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND || Monst->_msquelch == 0)
		return;

	pnum = monster[i]._menemy;
	fx = monster[i]._menemyx;
	fy = monster[i]._menemyy;
	xd = monster[i]._mx - fx;
	yd = monster[i]._my - fy;

	md = M_GetDir(i);
	if (Monst->_mgoal == MGOAL_NORMAL) {
		if (LineClear(Monst->_mx, Monst->_my, fx, fy)
		    && AddMissile(Monst->_mx, Monst->_my, fx, fy, md, MIS_FIREMAN, pnum, i, 0, 0) != -1) {
			Monst->_mmode = MM_CHARGE;
			Monst->_mgoal = MGOAL_SHOOT;
			Monst->_mgoalvar1 = 0;
		}
	} else if (Monst->_mgoal == MGOAL_SHOOT) {
		if (Monst->_mgoalvar1 == 3) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartFadeout(i, md, TRUE);
		} else if (LineClear(Monst->_mx, Monst->_my, fx, fy)) {
			M_StartRAttack(i, MIS_KRULL, 4);
			Monst->_mgoalvar1++;
		} else {
			M_StartDelay(i, random(112, 10) + 5);
			Monst->_mgoalvar1++;
		}
	} else if (Monst->_mgoal == MGOAL_RETREAT) {
		M_StartFadein(i, md, FALSE);
		Monst->_mgoal = MGOAL_SHOOT;
	}
	Monst->_mdir = md;
	random(112, 100);
	if (Monst->_mmode != MM_STAND)
		return;

	if (abs(xd) < 2 && abs(yd) < 2 && Monst->_mgoal == MGOAL_NORMAL) {
		M_TryH2HHit(i, monster[i]._menemy, monster[i].mHit, monster[i].mMinDamage, monster[i].mMaxDamage);
		Monst->_mgoal = MGOAL_RETREAT;
		if (!M_CallWalk(i, opposite[md])) {
			M_StartFadein(i, md, FALSE);
			Monst->_mgoal = MGOAL_SHOOT;
		}
	} else if (!M_CallWalk(i, md) && (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT)) {
		M_StartFadein(i, md, FALSE);
		Monst->_mgoal = MGOAL_SHOOT;
	}
}

void MAI_Fallen(int i)
{
	int x, y, xpos, ypos;
	int m, rad, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS) {
		app_fatal("MAI_Fallen: Invalid monster %d", i);
	}
	if (monster[i]._mgoal == MGOAL_SHOOT) {
		if (monster[i]._mgoalvar1)
			monster[i]._mgoalvar1--;
		else
			monster[i]._mgoal = MGOAL_NORMAL;
	}

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND || monster[i]._msquelch == 0) {
		return;
	}

	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (!Monst->_mgoalvar1--) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartStand(i, opposite[Monst->_mdir]);
		}
	}

	if (Monst->_mAnimFrame == Monst->_mAnimLen) {
		if (random(113, 4)) {
			return;
		}
		if (!(Monst->_mFlags & MFLAG_NOHEAL)) {
			M_StartSpStand(i, Monst->_mdir);
			rad = 2 * Monst->_mint + 2;
			if (Monst->_mmaxhp - rad >= Monst->_mhitpoints)
				Monst->_mhitpoints = rad + Monst->_mhitpoints;
			else
				Monst->_mhitpoints = Monst->_mmaxhp;
		}
		rad = 2 * Monst->_mint + 4;
		for (y = -rad; y <= rad; y++) {
			for (x = -rad; x <= rad; x++) {
				if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
					m = dMonster[x + Monst->_mx][y + Monst->_my];
					if (m > 0) {
						m--;
						if (monster[m]._mAi == AI_FALLEN) {
							monster[m]._mgoal = MGOAL_SHOOT;
							monster[m]._mgoalvar1 = 30 * Monst->_mint + 105;
						}
					}
				}
			}
		}
	} else if (Monst->_mgoal == MGOAL_RETREAT) {
		md = Monst->_mdir;
		M_CallWalk(i, md);
	} else if (Monst->_mgoal == MGOAL_SHOOT) {
		xpos = Monst->_mx - Monst->_menemyx;
		ypos = Monst->_my - Monst->_menemyy;
		if (abs(xpos) < 2 && abs(ypos) < 2) {
			M_StartAttack(i);
		} else {
			md = M_GetDir(i);
			M_CallWalk(i, md);
		}
	} else {
		MAI_SkelSd(i);
	}
}

void MAI_Cleaver(int i)
{
	MonsterStruct *Monst;
	int x, y, mx, my, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Cleaver: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	x = mx - Monst->_menemyx;
	y = my - Monst->_menemyy;

	md = GetDirection(mx, my, Monst->_lastx, Monst->_lasty);
	Monst->_mdir = md;

	if (abs(x) >= 2 || abs(y) >= 2)
		M_CallWalk(i, md);
	else
		M_StartAttack(i);

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_Round(int i, BOOL special)
{
	MonsterStruct *Monst;
	int fx, fy;
	int mx, my, md;
	int dist, v;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Round: Invalid monster %d", i);
	Monst = monster + i;
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fy = Monst->_menemyy;
		fx = Monst->_menemyx;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		if (Monst->_msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random(114, 100);
		if ((abs(mx) >= 2 || abs(my) >= 2) && Monst->_msquelch == UCHAR_MAX && dTransVal[Monst->_mx][Monst->_my] == dTransVal[fx][fy]) {
			if (Monst->_mgoal == MGOAL_MOVE || (abs(mx) >= 4 || abs(my) >= 4) && random(115, 4) == 0) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = random(116, 2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				if (abs(mx) > abs(my))
					dist = abs(mx);
				else
					dist = abs(my);
				if (Monst->_mgoalvar1++ >= 2 * dist && DirOK(i, md) || dTransVal[Monst->_mx][Monst->_my] != dTransVal[fx][fy]) {
					Monst->_mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(i, md, &Monst->_mgoalvar2)) {
					M_StartDelay(i, random(125, 10) + 10);
				}
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (abs(mx) >= 2 || abs(my) >= 2) {
				if (Monst->_mVar2 > 20 && v < 2 * Monst->_mint + 28
				    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
				        && Monst->_mVar2 == 0
				        && v < 2 * Monst->_mint + 78) {
					M_CallWalk(i, md);
				}
			} else if (v < 2 * Monst->_mint + 23) {
				Monst->_mdir = md;
				if (special && Monst->_mhitpoints < (Monst->_mmaxhp >> 1) && random(117, 2) != 0)
					M_StartSpAttack(i);
				else
					M_StartAttack(i);
			}
		}
		if (Monst->_mmode == MM_STAND)
			Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
	}
}

void MAI_GoatMc(int i)
{
	MAI_Round(i, TRUE);
}

void MAI_Ranged(int i, int missile_type, BOOL special)
{
	int md;
	int fx, fy, mx, my;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Ranged: Invalid monster %d", i);

	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	Monst = monster + i;
	if (Monst->_msquelch == UCHAR_MAX || Monst->_mFlags & MFLAG_TARGETS_MONSTER) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = M_GetDir(i);
		if (Monst->_msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		Monst->_mdir = md;
		if (Monst->_mVar1 == MM_RATTACK) {
			M_StartDelay(i, random(118, 20));
		} else if (abs(mx) < 4 && abs(my) < 4) {
			if (random(119, 100) < 10 * (Monst->_mint + 7))
				M_CallWalk(i, opposite[md]);
		}
		if (Monst->_mmode == MM_STAND) {
			if (LineClear(Monst->_mx, Monst->_my, fx, fy)) {
				if (special)
					M_StartRSpAttack(i, missile_type, 4);
				else
					M_StartRAttack(i, missile_type, 4);
			} else {
				Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
			}
		}
	} else if (Monst->_msquelch != 0) {
		fx = Monst->_lastx;
		fy = Monst->_lasty;
		md = GetDirection(Monst->_mx, Monst->_my, fx, fy);
		M_CallWalk(i, md);
	}
}

void MAI_GoatBow(int i)
{
	MAI_Ranged(i, MIS_ARROW, FALSE);
}

void MAI_Succ(int i)
{
	MAI_Ranged(i, MIS_FLARE, FALSE);
}

void MAI_AcidUniq(int i)
{
	MAI_Ranged(i, MIS_ACID, TRUE);
}

void MAI_Scav(int i)
{
	BOOL done;
	int x, y;
	int _mx, _my;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Scav: Invalid monster %d", i);
	Monst = monster + i;
	_mx = Monst->_mx;
	_my = Monst->_my;
	done = FALSE;
	if (monster[i]._mmode != MM_STAND)
		return;
	if (Monst->_mhitpoints < (Monst->_mmaxhp >> 1) && Monst->_mgoal != MGOAL_HEALING) {
		if (Monst->leaderflag) {
			monster[Monst->leader].packsize--;
			Monst->leaderflag = 0;
		}
		Monst->_mgoal = MGOAL_HEALING;
		Monst->_mgoalvar3 = 10;
	}
	if (Monst->_mgoal == MGOAL_HEALING && Monst->_mgoalvar3 != 0) {
		Monst->_mgoalvar3--;
		if (dDead[Monst->_mx][Monst->_my]) {
			M_StartEat(i);
			if (!(Monst->_mFlags & MFLAG_NOHEAL))
				Monst->_mhitpoints += 64;
			if (Monst->_mhitpoints >= (Monst->_mmaxhp >> 1) + (Monst->_mmaxhp >> 2)) {
				Monst->_mgoal = MGOAL_NORMAL;
				Monst->_mgoalvar1 = 0;
				Monst->_mgoalvar2 = 0;
			}
		} else {
			if (Monst->_mgoalvar1 == 0) {
				if (random(120, 2) != 0) {
					for (y = -4; y <= 4 && !done; y++) {
						for (x = -4; x <= 4 && !done; x++) {
							// BUGFIX: incorrect check of offset against limits of the dungeon
							if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
								continue;
							done = dDead[Monst->_mx + x][Monst->_my + y] != 0
							    && LineClearF(
							        CheckNoSolid,
							        Monst->_mx,
							        Monst->_my,
							        Monst->_mx + x,
							        Monst->_my + y);
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
							done = dDead[Monst->_mx + x][Monst->_my + y] != 0
							    && LineClearF(
							        CheckNoSolid,
							        Monst->_mx,
							        Monst->_my,
							        Monst->_mx + x,
							        Monst->_my + y);
						}
					}
					x++;
					y++;
				}
				if (done) {
					Monst->_mgoalvar1 = x + Monst->_mx + 1;
					Monst->_mgoalvar2 = y + Monst->_my + 1;
				}
			}
			if (Monst->_mgoalvar1) {
				x = Monst->_mgoalvar1 - 1;
				y = Monst->_mgoalvar2 - 1;
				Monst->_mdir = GetDirection(Monst->_mx, Monst->_my, x, y);
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
	int mx, my, dx, dy, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Garg: Invalid monster %d", i);

	Monst = &monster[i];
	dx = Monst->_mx - Monst->_lastx;
	dy = Monst->_my - Monst->_lasty;
	md = M_GetDir(i);
	if (Monst->_msquelch != 0 && Monst->_mFlags & MFLAG_ALLOW_SPECIAL) {
		M_Enemy(i);
		mx = Monst->_mx - Monst->_menemyx;
		my = Monst->_my - Monst->_menemyy;
		if (abs(mx) < Monst->_mint + 2 && abs(my) < Monst->_mint + 2) {
			Monst->_mFlags &= ~MFLAG_ALLOW_SPECIAL;
		}
		return;
	}

	if (Monst->_mmode != MM_STAND || Monst->_msquelch == 0) {
		return;
	}

	if (Monst->_mhitpoints<Monst->_mmaxhp>> 1 && !(Monst->_mFlags & MFLAG_NOHEAL))
		Monst->_mgoal = MGOAL_RETREAT;
	if (Monst->_mgoal == MGOAL_RETREAT) {
		if (abs(dx) >= Monst->_mint + 2 || abs(dy) >= Monst->_mint + 2) {
			Monst->_mgoal = MGOAL_NORMAL;
			M_StartHeal(i);
		} else if (!M_CallWalk(i, opposite[md])) {
			Monst->_mgoal = MGOAL_NORMAL;
		}
	}
	MAI_Round(i, FALSE);
}

void MAI_RoundRanged(int i, int missile_type, BOOL checkdoors, int dam, int lessmissiles)
{
	MonsterStruct *Monst;
	int mx, my;
	int fx, fy;
	int md, dist, v;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_RoundRanged: Invalid monster %d", i);
	Monst = monster + i;
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		if (checkdoors && Monst->_msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random(121, 10000);
		if ((abs(mx) >= 2 || abs(my) >= 2) && Monst->_msquelch == UCHAR_MAX && dTransVal[Monst->_mx][Monst->_my] == dTransVal[fx][fy]) {
			if (Monst->_mgoal == MGOAL_MOVE || ((abs(mx) >= 3 || abs(my) >= 3) && random(122, 4 << lessmissiles) == 0)) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = random(123, 2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				if (abs(mx) > abs(my)) {
					dist = abs(mx);
				} else {
					dist = abs(my);
				}
				if (Monst->_mgoalvar1++ >= 2 * dist && DirOK(i, md)) {
					Monst->_mgoal = MGOAL_NORMAL;
				} else if (v<500 * (Monst->_mint + 1)>> lessmissiles
				    && (LineClear(Monst->_mx, Monst->_my, fx, fy))) {
					M_StartRSpAttack(i, missile_type, dam);
				} else {
					M_RoundWalk(i, md, &Monst->_mgoalvar2);
				}
			}
		} else {
			Monst->_mgoal = MGOAL_NORMAL;
		}
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (((abs(mx) >= 3 || abs(my) >= 3) && v < ((500 * (Monst->_mint + 2)) >> lessmissiles)
			        || v < ((500 * (Monst->_mint + 1)) >> lessmissiles))
			    && LineClear(Monst->_mx, Monst->_my, fx, fy)) {
				M_StartRSpAttack(i, missile_type, dam);
			} else if (abs(mx) >= 2 || abs(my) >= 2) {
				v = random(124, 100);
				if (v < 1000 * (Monst->_mint + 5)
				    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3) && Monst->_mVar2 == 0 && v < 1000 * (Monst->_mint + 8)) {
					M_CallWalk(i, md);
				}
			} else if (v < 1000 * (Monst->_mint + 6)) {
				Monst->_mdir = md;
				M_StartAttack(i);
			}
		}
		if (Monst->_mmode == MM_STAND) {
			M_StartDelay(i, random(125, 10) + 5);
		}
	}
}

void MAI_Magma(int i)
{
	MAI_RoundRanged(i, MIS_MAGMABALL, TRUE, 4, 0);
}

void MAI_Storm(int i)
{
	MAI_RoundRanged(i, MIS_LIGHTCTRL2, TRUE, 4, 0);
}

void MAI_Acid(int i)
{
	MAI_RoundRanged(i, MIS_ACID, FALSE, 4, 1);
}

void MAI_Diablo(int i)
{
	MAI_RoundRanged(i, MIS_DIABAPOCA, FALSE, 40, 0);
}

void MAI_RR2(int i, int mistype, int dam)
{
	MonsterStruct *Monst;
	int mx, my, fx, fy;
	int dist, v, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_RR2: Invalid monster %d", i);

	Monst = monster + i;
	mx = Monst->_mx - Monst->_menemyx;
	my = Monst->_my - Monst->_menemyy;
	if (abs(mx) >= 5 || abs(my) >= 5) {
		MAI_SkelSd(i);
		return;
	}

	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		if (Monst->_msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random(121, 100);
		if ((abs(mx) >= 2 || abs(my) >= 2) && Monst->_msquelch == UCHAR_MAX && dTransVal[Monst->_mx][Monst->_my] == dTransVal[fx][fy]) {
			if (Monst->_mgoal == MGOAL_MOVE || (abs(mx) >= 3 || abs(my) >= 3)) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = random(123, 2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				Monst->_mgoalvar3 = 4;
				if (abs(mx) > abs(my)) {
					dist = abs(mx);
				} else {
					dist = abs(my);
				}
				if (Monst->_mgoalvar1++ < 2 * dist || !DirOK(i, md)) {
					if (v < 5 * (Monst->_mint + 16))
						M_RoundWalk(i, md, &Monst->_mgoalvar2);
				} else
					Monst->_mgoal = MGOAL_NORMAL;
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (((abs(mx) >= 3 || abs(my) >= 3) && v < 5 * (Monst->_mint + 2) || v < 5 * (Monst->_mint + 1) || Monst->_mgoalvar3 == 4) && LineClear(Monst->_mx, Monst->_my, fx, fy)) {
				M_StartRSpAttack(i, mistype, dam);
			} else if (abs(mx) >= 2 || abs(my) >= 2) {
				v = random(124, 100);
				if (v < 2 * (5 * Monst->_mint + 25)
				    || (Monst->_mVar1 == MM_WALK || Monst->_mVar1 == MM_WALK2 || Monst->_mVar1 == MM_WALK3)
				        && Monst->_mVar2 == 0
				        && v < 2 * (5 * Monst->_mint + 40)) {
					M_CallWalk(i, md);
				}
			} else {
				if (random(124, 100) < 10 * (Monst->_mint + 4)) {
					Monst->_mdir = md;
					if (random(124, 2) != 0)
						M_StartAttack(i);
					else
						M_StartRSpAttack(i, mistype, dam);
				}
			}
			Monst->_mgoalvar3 = 1;
		}
		if (Monst->_mmode == MM_STAND) {
			M_StartDelay(i, random(125, 10) + 5);
		}
	}
}

void MAI_Mega(int i)
{
	MAI_RR2(i, MIS_FLAMEC, 0);
}

void MAI_Golum(int i)
{
	int mx, my, _mex, _mey;
	int md, j, k, _menemy;
	MonsterStruct *Monst;
	BOOL have_enemy, ok;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Golum: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mx == 1 && Monst->_my == 0) {
		return;
	}

	if (Monst->_mmode == MM_DEATH
	    || Monst->_mmode == MM_SPSTAND
	    || (Monst->_mmode >= MM_WALK && Monst->_mmode <= MM_WALK3)) {
		return;
	}

	if (!(Monst->_mFlags & MFLAG_TARGETS_MONSTER))
		M_Enemy(i);

	have_enemy = !(monster[i]._mFlags & MFLAG_NO_ENEMY);

	if (Monst->_mmode == MM_ATTACK) {
		return;
	}

	_menemy = monster[i]._menemy;

	mx = monster[i]._mx;
	my = monster[i]._my;
	_mex = mx - monster[_menemy]._mfutx;
	_mey = my - monster[_menemy]._mfuty;
	md = GetDirection(mx, my, monster[_menemy]._mx, monster[_menemy]._my);
	monster[i]._mdir = md;
	if (abs(_mex) >= 2 || abs(_mey) >= 2) {
		if (have_enemy && MAI_Path(i))
			return;
	} else if (have_enemy) {
		_menemy = monster[i]._menemy;
		monster[i]._menemyx = monster[_menemy]._mx;
		monster[i]._menemyy = monster[_menemy]._my;
		if (monster[_menemy]._msquelch == 0) {
			monster[_menemy]._msquelch = UCHAR_MAX;
			monster[monster[i]._menemy]._lastx = monster[i]._mx;
			monster[monster[i]._menemy]._lasty = monster[i]._my;
			for (j = 0; j < 5; j++) {
				for (k = 0; k < 5; k++) {
					_menemy = dMonster[monster[i]._mx + k - 2][monster[i]._my + j - 2];
					if (_menemy > 0)
						monster[_menemy]._msquelch = UCHAR_MAX;
				}
			}
		}
		M_StartAttack(i);
		return;
	}

	monster[i]._pathcount++;
	if (monster[i]._pathcount > 8)
		monster[i]._pathcount = 5;

	ok = M_CallWalk(i, plr[i]._pdir);
	if (!ok) {
		md = (md - 1) & 7;
		for (j = 0; j < 8 && !ok; j++) {
			md = (md + 1) & 7;
			ok = DirOK(i, md);
		}
		if (!ok) {
			return;
		}
		M_WalkDir(i, md);
	}
}

void MAI_SkelKing(int i)
{
	MonsterStruct *Monst;
	int mx, my, fx, fy, nx, ny;
	int dist, v, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_SkelKing: Invalid monster %d", i);
	Monst = monster + i;
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		if (Monst->_msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random(126, 100);
		if ((abs(mx) >= 2 || abs(my) >= 2) && Monst->_msquelch == UCHAR_MAX && dTransVal[Monst->_mx][Monst->_my] == dTransVal[fx][fy]) {
			if (Monst->_mgoal == MGOAL_MOVE || (abs(mx) >= 3 || abs(my) >= 3) && random(127, 4) == 0) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = random(128, 2);
				}
				Monst->_mgoal = MGOAL_MOVE;
				if (abs(mx) > abs(my)) {
					dist = abs(mx);
				} else {
					dist = abs(my);
				}
				if (Monst->_mgoalvar1++ >= 2 * dist && DirOK(i, md) || dTransVal[Monst->_mx][Monst->_my] != dTransVal[fx][fy]) {
					Monst->_mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(i, md, &Monst->_mgoalvar2)) {
					M_StartDelay(i, random(125, 10) + 10);
				}
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if (gbMaxPlayers == 1
			    && ((abs(mx) >= 3 || abs(my) >= 3) && v < 4 * Monst->_mint + 35 || v < 6)
			    && LineClear(Monst->_mx, Monst->_my, fx, fy)) {
				nx = Monst->_mx + offset_x[md];
				ny = Monst->_my + offset_y[md];
				if (PosOkMonst(i, nx, ny) && nummonsters < MAXMONSTERS) {
					M_SpawnSkel(nx, ny, md);
					M_StartSpStand(i, md);
				}
			} else {
				if (abs(mx) >= 2 || abs(my) >= 2) {
					v = random(129, 100);
					if (v >= Monst->_mint + 25
					    && (Monst->_mVar1 != MM_WALK && Monst->_mVar1 != MM_WALK2 && Monst->_mVar1 != MM_WALK3 || Monst->_mVar2 != 0 || (v >= Monst->_mint + 75))) {
						M_StartDelay(i, random(130, 10) + 10);
					} else {
						M_CallWalk(i, md);
					}
				} else if (v < Monst->_mint + 20) {
					Monst->_mdir = md;
					M_StartAttack(i);
				}
			}
		}
		if (Monst->_mmode == MM_STAND)
			Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
	}
}

void MAI_Rhino(int i)
{
	MonsterStruct *Monst;
	int mx, my, fx, fy;
	int v, dist, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Rhino: Invalid monster %d", i);
	Monst = monster + i;
	if (Monst->_mmode == MM_STAND && Monst->_msquelch != 0) {
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		if (Monst->_msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random(131, 100);
		if (abs(mx) >= 2 || abs(my) >= 2) {
			if (Monst->_mgoal == MGOAL_MOVE || (abs(mx) >= 5 || abs(my) >= 5) && random(132, 4) != 0) {
				if (Monst->_mgoal != MGOAL_MOVE) {
					Monst->_mgoalvar1 = 0;
					Monst->_mgoalvar2 = random(133, 2);
				}
				Monst->_mgoal = 4;
				if (abs(mx) > abs(my)) {
					dist = abs(mx);
				} else {
					dist = abs(my);
				}
				if (Monst->_mgoalvar1++ >= 2 * dist || dTransVal[Monst->_mx][Monst->_my] != dTransVal[fx][fy]) {
					Monst->_mgoal = MGOAL_NORMAL;
				} else if (!M_RoundWalk(i, md, &Monst->_mgoalvar2)) {
					M_StartDelay(i, random(125, 10) + 10);
				}
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
		if (Monst->_mgoal == MGOAL_NORMAL) {
			if ((abs(mx) >= 5 || abs(my) >= 5)
			    && v < 2 * Monst->_mint + 43
			    && LineClearF1(PosOkMonst, i, Monst->_mx, Monst->_my, fx, fy)) {
				if (AddMissile(Monst->_mx, Monst->_my, fx, fy, md, MIS_RHINO, Monst->_menemy, i, 0, 0) != -1) {
					if (Monst->MData->snd_special)
						PlayEffect(i, 3);
					Monst->_mmode = MM_CHARGE;
					dMonster[Monst->_mx][Monst->_my] = -1 - i;
				}
			} else {
				if (abs(mx) >= 2 || abs(my) >= 2) {
					v = random(134, 100);
					if (v >= 2 * Monst->_mint + 33
					    && (Monst->_mVar1 != MM_WALK && Monst->_mVar1 != MM_WALK2 && Monst->_mVar1 != MM_WALK3
					        || Monst->_mVar2
					        || v >= 2 * Monst->_mint + 83)) {
						M_StartDelay(i, random(135, 10) + 10);
					} else {
						M_CallWalk(i, md);
					}
				} else if (v < 2 * Monst->_mint + 28) {
					Monst->_mdir = md;
					M_StartAttack(i);
				}
			}
		}
		if (Monst->_mmode == MM_STAND)
			Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
	}
}

void MAI_Counselor(int i)
{
	int mx, my, fx, fy;
	int dist, md, v;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Counselor: Invalid monster %d", i);
	if (monster[i]._mmode == MM_STAND && monster[i]._msquelch != 0) {
		Monst = monster + i;
		fx = Monst->_menemyx;
		fy = Monst->_menemyy;
		mx = Monst->_mx - fx;
		my = Monst->_my - fy;
		md = GetDirection(Monst->_mx, Monst->_my, Monst->_lastx, Monst->_lasty);
		if (Monst->_msquelch < UCHAR_MAX)
			MonstCheckDoors(i);
		v = random(121, 100);
		if (Monst->_mgoal == MGOAL_RETREAT) {
			if (Monst->_mgoalvar1++ <= 3)
				M_CallWalk(i, opposite[md]);
			else {
				Monst->_mgoal = MGOAL_NORMAL;
				M_StartFadein(i, md, TRUE);
			}
		} else if (Monst->_mgoal == MGOAL_MOVE) {
			if (abs(mx) > abs(my))
				dist = abs(mx);
			else
				dist = abs(my);
			if ((abs(mx) >= 2 || abs(my) >= 2) && Monst->_msquelch == UCHAR_MAX && dTransVal[Monst->_mx][Monst->_my] == dTransVal[fx][fy]) {
				if (Monst->_mgoalvar1++ < 2 * dist || !DirOK(i, md)) {
					M_RoundWalk(i, md, &Monst->_mgoalvar2);
				} else {
					Monst->_mgoal = MGOAL_NORMAL;
					M_StartFadein(i, md, TRUE);
				}
			} else {
				Monst->_mgoal = MGOAL_NORMAL;
				M_StartFadein(i, md, TRUE);
			}
		} else if (Monst->_mgoal == MGOAL_NORMAL) {
			if (abs(mx) >= 2 || abs(my) >= 2) {
				if (v < 5 * (Monst->_mint + 10) && LineClear(Monst->_mx, Monst->_my, fx, fy)) {
					M_StartRAttack(i, counsmiss[Monst->_mint], Monst->mMinDamage + random(77, Monst->mMaxDamage - Monst->mMinDamage + 1));
				} else if (random(124, 100) < 30) {
					Monst->_mgoal = MGOAL_MOVE;
					Monst->_mgoalvar1 = 0;
					M_StartFadeout(i, md, FALSE);
				} else
					M_StartDelay(i, random(105, 10) + 2 * (5 - Monst->_mint));
			} else {
				Monst->_mdir = md;
				if (Monst->_mhitpoints < (Monst->_mmaxhp >> 1)) {
					Monst->_mgoal = MGOAL_RETREAT;
					Monst->_mgoalvar1 = 0;
					M_StartFadeout(i, md, FALSE);
				} else if (Monst->_mVar1 == MM_DELAY
				    || random(105, 100) < 2 * Monst->_mint + 20) {
					M_StartRAttack(i, -1, 0);
					AddMissile(Monst->_mx, Monst->_my, 0, 0, Monst->_mdir, MIS_FLASH, 1, i, 4, 0);
					AddMissile(Monst->_mx, Monst->_my, 0, 0, Monst->_mdir, MIS_FLASH2, 1, i, 4, 0);
				} else
					M_StartDelay(i, random(105, 10) + 2 * (5 - Monst->_mint));
			}
		}
		if (Monst->_mmode == MM_STAND) {
			M_StartDelay(i, random(125, 10) + 5);
		}
	}
}

void MAI_Garbud(int i)
{
	int _mx, _my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Garbud: Invalid monster %d", i);

	Monst = &monster[i];
	if (Monst->_mmode != MM_STAND) {
		return;
	}

	_mx = Monst->_mx;
	_my = Monst->_my;
	md = M_GetDir(i);

	if (Monst->mtalkmsg < QUEST_GARBUD4
	    && Monst->mtalkmsg > QUEST_DOOM10
	    && !(dFlags[_mx][_my] & BFLAG_VISIBLE)
	    && Monst->_mgoal == MGOAL_TALKING) {
		Monst->_mgoal = MGOAL_INQUIRING;
		Monst->mtalkmsg++;
	}

	if (dFlags[_mx][_my] & BFLAG_VISIBLE) {
		if (Monst->mtalkmsg == QUEST_GARBUD4) {
			if (!effect_is_playing(USFX_GARBUD4) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->_mgoal = MGOAL_NORMAL;
				Monst->_msquelch = UCHAR_MAX;
				Monst->mtalkmsg = 0;
			}
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_MOVE)
		MAI_Round(i, TRUE);

	monster[i]._mdir = md;

	if (Monst->_mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_Zhar(int i)
{
	int mx, my, _mx, _my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Zhar: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	my = Monst->_my;
	mx = Monst->_mx;
	md = M_GetDir(i);
	if (Monst->mtalkmsg == QUEST_ZHAR1 && !(dFlags[mx][my] & BFLAG_VISIBLE) && Monst->_mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = QUEST_ZHAR2;
		Monst->_mgoal = MGOAL_INQUIRING;
	}

	if (dFlags[mx][my] & BFLAG_VISIBLE) {
		_mx = Monst->_mx - Monst->_menemyx;
		_my = Monst->_my - Monst->_menemyy;
		if (abs(_mx) > abs(_my))
			abs(_mx);
		else
			abs(_my);
		if (Monst->mtalkmsg == QUEST_ZHAR2) {
			if (!effect_is_playing(USFX_ZHAR2) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->_msquelch = UCHAR_MAX;
				Monst->mtalkmsg = 0;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT || Monst->_mgoal == MGOAL_MOVE)
		MAI_Counselor(i);

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_SnotSpil(int i)
{
	int mx, my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_SnotSpil: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	md = M_GetDir(i);

	if (Monst->mtalkmsg == QUEST_BANNER10 && !(dFlags[mx][my] & BFLAG_VISIBLE) && Monst->_mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = QUEST_BANNER11;
		Monst->_mgoal = MGOAL_INQUIRING;
	}

	if (Monst->mtalkmsg == QUEST_BANNER11 && quests[QTYPE_BOL]._qvar1 == 3) {
		Monst->mtalkmsg = 0;
		Monst->_mgoal = MGOAL_NORMAL;
	}

	if (dFlags[mx][my] & BFLAG_VISIBLE) {
		if (Monst->mtalkmsg == QUEST_BANNER12) {
			if (!effect_is_playing(USFX_SNOT3) && Monst->_mgoal == MGOAL_TALKING) {
				ObjChangeMap(setpc_x, setpc_y, setpc_x + setpc_w + 1, setpc_y + setpc_h + 1);
				quests[QTYPE_BOL]._qvar1 = 3;
				RedoPlayerVision();
				Monst->_msquelch = UCHAR_MAX;
				Monst->mtalkmsg = 0;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		}
		if (quests[QTYPE_BOL]._qvar1 == 3) {
			if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_SHOOT)
				MAI_Fallen(i);
		}
	}

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_Lazurus(int i)
{
	int mx, my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Lazurus: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	md = M_GetDir(i);
	if (dFlags[mx][my] & BFLAG_VISIBLE) {
		if (gbMaxPlayers == 1) {
			if (Monst->mtalkmsg == QUEST_VILE13 && Monst->_mgoal == MGOAL_INQUIRING && plr[myplr].WorldX == QUEST_VILE13 && plr[myplr].WorldY == 46) {
				PlayInGameMovie("gendata\\fprst3.smk");
				Monst->_mmode = MM_TALK;
				quests[QTYPE_VB]._qvar1 = 5;
			}

			if (Monst->mtalkmsg == QUEST_VILE13 && !effect_is_playing(USFX_LAZ1) && Monst->_mgoal == MGOAL_TALKING) {
				ObjChangeMapResync(1, 18, 20, 24);
				RedoPlayerVision();
				Monst->_msquelch = UCHAR_MAX;
				Monst->mtalkmsg = 0;
				quests[QTYPE_VB]._qvar1 = 6;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		}

		if (gbMaxPlayers != 1 && Monst->mtalkmsg == QUEST_VILE13 && Monst->_mgoal == MGOAL_INQUIRING && quests[QTYPE_VB]._qvar1 <= 3) {
			Monst->_mmode = MM_TALK;
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL || Monst->_mgoal == MGOAL_RETREAT || Monst->_mgoal == MGOAL_MOVE) {
		Monst->mtalkmsg = 0;
		MAI_Counselor(i);
	}

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND || monster[i]._mmode == MM_TALK)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_Lazhelp(int i)
{
	int _mx, _my;
	volatile int md; // BUGFIX: very questionable volatile
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Lazhelp: Invalid monster %d", i);
	if (monster[i]._mmode != MM_STAND)
		return;

	Monst = monster + i;
	_mx = Monst->_mx;
	_my = Monst->_my;
	md = M_GetDir(i);

	if (dFlags[_mx][_my] & BFLAG_VISIBLE) {
		if (gbMaxPlayers == 1) {
			if (quests[QTYPE_VB]._qvar1 <= 5) {
				Monst->_mgoal = MGOAL_INQUIRING;
			} else {
				Monst->mtalkmsg = 0;
				Monst->_mgoal = MGOAL_NORMAL;
			}
		} else
			Monst->_mgoal = MGOAL_NORMAL;
	}
	if (Monst->_mgoal == MGOAL_NORMAL)
		MAI_Succ(i);
	Monst->_mdir = md;
	if (monster[i]._mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_Lachdanan(int i)
{
	int _mx, _my, md;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Lachdanan: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	_mx = Monst->_mx;
	_my = Monst->_my;
	md = M_GetDir(i);
	if (Monst->mtalkmsg == QUEST_VEIL9 && !(dFlags[_mx][_my] & BFLAG_VISIBLE) && monster[i]._mgoal == MGOAL_TALKING) {
		Monst->mtalkmsg = QUEST_VEIL10;
		monster[i]._mgoal = MGOAL_INQUIRING;
	}

	if (dFlags[_mx][_my] & BFLAG_VISIBLE) {
		if (Monst->mtalkmsg == QUEST_VEIL11) {
			if (!effect_is_playing(USFX_LACH3) && Monst->_mgoal == MGOAL_TALKING) {
				Monst->mtalkmsg = 0;
				quests[QTYPE_VEIL]._qactive = 3;
				M_StartKill(i, -1);
			}
		}
	}

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[md];
}

void MAI_Warlord(int i)
{
	MonsterStruct *Monst;
	int mx, my, md;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("MAI_Warlord: Invalid monster %d", i);

	Monst = &monster[i];
	if (monster[i]._mmode != MM_STAND) {
		return;
	}

	mx = Monst->_mx;
	my = Monst->_my;
	md = M_GetDir(i);
	if (dFlags[mx][my] & BFLAG_VISIBLE) {
		if (Monst->mtalkmsg == QUEST_WARLRD9 && Monst->_mgoal == MGOAL_INQUIRING)
			Monst->_mmode = MM_TALK;
		if (Monst->mtalkmsg == QUEST_WARLRD9 && !effect_is_playing(USFX_WARLRD1) && Monst->_mgoal == MGOAL_TALKING) {
			Monst->_msquelch = UCHAR_MAX;
			Monst->mtalkmsg = 0;
			Monst->_mgoal = MGOAL_NORMAL;
		}
	}

	if (Monst->_mgoal == MGOAL_NORMAL)
		MAI_SkelSd(i);

	Monst->_mdir = md;

	if (monster[i]._mmode == MM_STAND || monster[i]._mmode == MM_TALK)
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[Monst->_mdir];
}

void DeleteMonsterList()
{
	int i;
	for (i = 0; i < MAX_PLRS; i++) {
		if (monster[i]._mDelFlag) {
			monster[i]._mx = 1;
			monster[i]._my = 0;
			monster[i]._mfutx = 0;
			monster[i]._mfuty = 0;
			monster[i]._moldx = 0;
			monster[i]._moldy = 0;
			monster[i]._mDelFlag = FALSE;
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
	BOOL raflag;
	MonsterStruct *Monst;

	DeleteMonsterList();

	/// ASSERT: assert((DWORD)nummonsters <= MAXMONSTERS);
	for (i = 0; i < nummonsters; i++) {
		mi = monstactive[i];
		Monst = &monster[mi];
		raflag = FALSE;
		if (gbMaxPlayers > 1) {
			SetRndSeed(Monst->_mAISeed);
			Monst->_mAISeed = GetRndSeed();
		}
		if (!(monster[mi]._mFlags & MFLAG_NOHEAL) && Monst->_mhitpoints < Monst->_mmaxhp && Monst->_mhitpoints >> 6 > 0) {
			if (Monst->mLevel <= 1) {
				Monst->_mhitpoints += Monst->mLevel;
			} else {
				Monst->_mhitpoints += Monst->mLevel >> 1;
			}
		}
		mx = Monst->_mx;
		my = Monst->_my;
		if (dFlags[mx][my] & BFLAG_VISIBLE && Monst->_msquelch == 0 && Monst->MType->mtype == MT_CLEAVER) {
			PlaySFX(USFX_CLEAVER);
		}
		if (Monst->_mFlags & MFLAG_TARGETS_MONSTER) {
			_menemy = Monst->_menemy;
			if ((DWORD)_menemy >= MAXMONSTERS) {
				app_fatal("Illegal enemy monster %d for monster \"%s\"", _menemy, Monst->mName);
			}
			Monst->_lastx = monster[Monst->_menemy]._mfutx;
			Monst->_menemyx = Monst->_lastx;
			Monst->_lasty = monster[Monst->_menemy]._mfuty;
			Monst->_menemyy = Monst->_lasty;
		} else {
			_menemy = Monst->_menemy;
			if ((DWORD)_menemy >= MAX_PLRS) {
				app_fatal("Illegal enemy player %d for monster \"%s\"", _menemy, Monst->mName);
			}
			Monst->_menemyx = plr[Monst->_menemy]._px;
			Monst->_menemyy = plr[Monst->_menemy]._py;
			if (dFlags[mx][my] & BFLAG_VISIBLE) {
				Monst->_msquelch = 255;
				Monst->_lastx = plr[Monst->_menemy]._px;
				Monst->_lasty = plr[Monst->_menemy]._py;
			} else if (Monst->_msquelch != 0 && Monst->_mAi != MT_DIABLO) { /// BUGFIX: change '_mAi' to 'MType->mtype'
				Monst->_msquelch--;
			}
		}
		do {
			if (!(Monst->_mFlags & MFLAG_SEARCH)) {
				AiProc[Monst->_mAi](mi);
			} else if (!MAI_Path(mi)) {
				AiProc[Monst->_mAi](mi);
			}
			switch (Monst->_mmode) {
			case MM_STAND:
				raflag = M_DoStand(mi);
				break;
			case MM_WALK:
				raflag = M_DoWalk(mi);
				break;
			case MM_WALK2:
				raflag = M_DoWalk2(mi);
				break;
			case MM_WALK3:
				raflag = M_DoWalk3(mi);
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
				raflag = FALSE;
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
			Monst->_mAnimCnt++;
			if (!(Monst->_mFlags & MFLAG_ALLOW_SPECIAL) && Monst->_mAnimCnt >= Monst->_mAnimDelay) {
				Monst->_mAnimCnt = 0;
				if (Monst->_mFlags & MFLAG_LOCK_ANIMATION) {
					Monst->_mAnimFrame--;
					if (Monst->_mAnimFrame == 0) {
						Monst->_mAnimFrame = Monst->_mAnimLen;
					}
				} else {
					Monst->_mAnimFrame++;
					if (Monst->_mAnimFrame > Monst->_mAnimLen) {
						Monst->_mAnimFrame = 1;
					}
				}
			}
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
				MemFreeDbg(Monsters[i].Anims[j].CMem);
			}
		}
	}

	FreeMissiles2();
}

BOOL DirOK(int i, int mdir)
{
	int fx, fy;
	int x, y;
	int mcount, mi;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("DirOK: Invalid monster %d", i);
	fx = monster[i]._mx + offset_x[mdir];
	fy = monster[i]._my + offset_y[mdir];
	if (fy < 0 || fy >= MAXDUNY || fx < 0 || fx >= MAXDUNX || !PosOkMonst(i, fx, fy))
		return FALSE;
	if (mdir == DIR_E) {
		if (SolidLoc(fx, fy + 1) || dFlags[fx][fy + 1] & BFLAG_MONSTLR)
			return FALSE;
	}
	if (mdir == DIR_W) {
		if (SolidLoc(fx + 1, fy) || dFlags[fx + 1][fy] & BFLAG_MONSTLR)
			return FALSE;
	}
	if (mdir == DIR_N) {
		if (SolidLoc(fx + 1, fy) || SolidLoc(fx, fy + 1))
			return FALSE;
	}
	if (mdir == DIR_S)
		if (SolidLoc(fx - 1, fy) || SolidLoc(fx, fy - 1))
			return FALSE;
	if (monster[i].leaderflag == 1) {
		if (abs(fx - monster[monster[i].leader]._mfutx) >= 4
		    || abs(fy - monster[monster[i].leader]._mfuty) >= 4) {
			return FALSE;
		}
		return TRUE;
	}
	if (monster[i]._uniqtype == 0 || !(UniqMonst[monster[i]._uniqtype - 1].mUnqAttr & 2))
		return TRUE;
	mcount = 0;
	for (x = fx - 3; x <= fx + 3; x++) {
		for (y = fy - 3; y <= fy + 3; y++) {
			if (y < 0 || y >= MAXDUNY || x < 0 || x >= MAXDUNX)
				continue;
			mi = dMonster[x][y];
			if (mi < 0)
				mi = -mi;
			if (mi != 0)
				mi--;
			if (monster[mi].leaderflag == 1
			    && monster[mi].leader == i
			    && monster[mi]._mfutx == x
			    && monster[mi]._mfuty == y) {
				mcount++;
			}
		}
	}
	return mcount == monster[i].packsize;
}

BOOL PosOkMissile(int x, int y)
{
	return !nMissileTable[dPiece[x][y]] && !(dFlags[x][y] & BFLAG_MONSTLR);
}

BOOL CheckNoSolid(int x, int y)
{
	return nSolidTable[dPiece[x][y]] == 0;
}

BOOL LineClearF(BOOL (*Clear)(int, int), int x1, int y1, int x2, int y2)
{
	int xorg, yorg;
	int dx, dy;
	int d;
	int xincD, yincD, dincD, dincH;
	int tmp;

	xorg = x1;
	yorg = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	if (abs(dx) > abs(dy)) {
		if (dx < 0) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = -dx;
			dy = -dy;
		}
		if (dy > 0) {
			d = 2 * dy - dx;
			dincH = 2 * (dy - dx);
			dincD = 2 * dy;
			yincD = 1;
		} else {
			d = 2 * dy + dx;
			dincH = 2 * (dx + dy);
			dincD = 2 * dy;
			yincD = -1;
		}
		while (x1 != x2 || y1 != y2) {
			if ((d <= 0) ^ (yincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				y1 += yincD;
			}
			x1++;
			if ((x1 != xorg || y1 != yorg) && !Clear(x1, y1))
				break;
		}
	} else {
		if (dy < 0) {
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			dy = -dy;
			dx = -dx;
		}
		if (dx > 0) {
			d = 2 * dx - dy;
			dincH = 2 * (dx - dy);
			dincD = 2 * dx;
			xincD = 1;
		} else {
			d = 2 * dx + dy;
			dincH = 2 * (dy + dx);
			dincD = 2 * dx;
			xincD = -1;
		}
		while (y1 != y2 || x1 != x2) {
			if ((d <= 0) ^ (xincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				x1 += xincD;
			}
			y1++;
			if ((y1 != yorg || x1 != xorg) && !Clear(x1, y1))
				break;
		}
	}
	return x1 == x2 && y1 == y2;
}

BOOL LineClear(int x1, int y1, int x2, int y2)
{
	return LineClearF(PosOkMissile, x1, y1, x2, y2);
}

BOOL LineClearF1(BOOL (*Clear)(int, int, int), int monst, int x1, int y1, int x2, int y2)
{
	int xorg, yorg;
	int dx, dy;
	int d;
	int xincD, yincD, dincD, dincH;
	int tmp;

	xorg = x1;
	yorg = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	if (abs(dx) > abs(dy)) {
		if (dx < 0) {
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			dx = -dx;
			dy = -dy;
		}
		if (dy > 0) {
			d = 2 * dy - dx;
			dincH = 2 * (dy - dx);
			dincD = 2 * dy;
			yincD = 1;
		} else {
			d = 2 * dy + dx;
			dincH = 2 * (dx + dy);
			dincD = 2 * dy;
			yincD = -1;
		}
		while (x1 != x2 || y1 != y2) {
			if ((d <= 0) ^ (yincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				y1 += yincD;
			}
			x1++;
			if ((x1 != xorg || y1 != yorg) && !Clear(monst, x1, y1))
				break;
		}
	} else {
		if (dy < 0) {
			tmp = y1;
			y1 = y2;
			y2 = tmp;
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			dy = -dy;
			dx = -dx;
		}
		if (dx > 0) {
			d = 2 * dx - dy;
			dincH = 2 * (dx - dy);
			dincD = 2 * dx;
			xincD = 1;
		} else {
			d = 2 * dx + dy;
			dincH = 2 * (dy + dx);
			dincD = 2 * dx;
			xincD = -1;
		}
		while (y1 != y2 || x1 != x2) {
			if ((d <= 0) ^ (xincD < 0)) {
				d += dincD;
			} else {
				d += dincH;
				x1 += xincD;
			}
			y1++;
			if ((y1 != yorg || x1 != xorg) && !Clear(monst, x1, y1))
				break;
		}
	}
	return x1 == x2 && y1 == y2;
}

void SyncMonsterAnim(int i)
{
	MonsterData *MData;
	int _mdir;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("SyncMonsterAnim: Invalid monster %d", i);
	Monst = monster + i;
	Monst->MType = Monsters + Monst->_mMTidx;
	MData = Monsters[Monst->_mMTidx].MData;
	Monst->MData = MData;
	if (Monst->_uniqtype != 0)
		Monst->mName = UniqMonst[Monst->_uniqtype - 1].mName;
	else
		Monst->mName = MData->mName;
	_mdir = monster[i]._mdir;

	switch (Monst->_mmode) {
	case MM_WALK:
	case MM_WALK2:
	case MM_WALK3:
		Monst->_mAnimData = Monst->MType->Anims[MA_WALK].Data[_mdir];
		return;
	case MM_ATTACK:
	case MM_RATTACK:
		Monst->_mAnimData = Monst->MType->Anims[MA_ATTACK].Data[_mdir];
		return;
	case MM_GOTHIT:
		Monst->_mAnimData = Monst->MType->Anims[MA_GOTHIT].Data[_mdir];
		return;
	case MM_DEATH:
		Monst->_mAnimData = Monst->MType->Anims[MA_DEATH].Data[_mdir];
		return;
	case MM_SATTACK:
	case MM_FADEIN:
	case MM_FADEOUT:
		Monst->_mAnimData = Monst->MType->Anims[MA_SPECIAL].Data[_mdir];
		return;
	case MM_SPSTAND:
	case MM_RSPATTACK:
		Monst->_mAnimData = Monst->MType->Anims[MA_SPECIAL].Data[_mdir];
		return;
	case MM_HEAL:
		Monst->_mAnimData = Monst->MType->Anims[MA_SPECIAL].Data[_mdir];
		return;
	case MM_STAND:
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[_mdir];
		return;
	case MM_DELAY:
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[_mdir];
		return;
	case MM_TALK:
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[_mdir];
		return;
	case MM_CHARGE:
		Monst->_mAnimData = Monst->MType->Anims[MA_ATTACK].Data[_mdir];
		Monst->_mAnimFrame = 1;
		Monst->_mAnimLen = Monst->MType->Anims[MA_ATTACK].Frames;
		break;
	default:
		Monst->_mAnimData = Monst->MType->Anims[MA_STAND].Data[_mdir];
		Monst->_mAnimFrame = 1;
		Monst->_mAnimLen = Monst->MType->Anims[MA_STAND].Frames;
		break;
	}
}

void M_FallenFear(int x, int y)
{
	int i, mi, rundist, aitype;

	for (i = 0; i < nummonsters; i++) {
		rundist = 0;
		mi = monstactive[i];

		switch (monster[mi].MType->mtype) {
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
		}
		aitype = monster[mi]._mAi;
		if (aitype == AI_FALLEN
		    && rundist
		    && abs(x - monster[mi]._mx) < 5
		    && abs(y - monster[mi]._my) < 5
		    && monster[mi]._mhitpoints >> 6 > 0) {
			monster[mi]._mgoal = MGOAL_RETREAT;
			monster[mi]._mgoalvar1 = rundist;
			monster[mi]._mdir = GetDirection(x, y, monster[i]._mx, monster[i]._my);
		}
	}
}

void PrintMonstHistory(int mt)
{
	int minHP, maxHP, res;

	sprintf(tempstr, "Total kills : %i", monstkills[mt]);
	AddPanelString(tempstr, TRUE);
	if (monstkills[mt] >= 30) {
		minHP = monsterdata[mt].mMinHP;
		maxHP = monsterdata[mt].mMaxHP;
		if (gbMaxPlayers == 1) {
			minHP = monsterdata[mt].mMinHP >> 1;
			maxHP = monsterdata[mt].mMaxHP >> 1;
		}
		if (minHP < 1)
			minHP = 1;
		if (maxHP < 1)
			maxHP = 1;
		if (gnDifficulty == DIFF_NIGHTMARE) {
			minHP = 3 * minHP + 1;
			maxHP = 3 * maxHP + 1;
		}
		if (gnDifficulty == DIFF_HELL) {
			minHP = 4 * minHP + 3;
			maxHP = 4 * maxHP + 3;
		}
		sprintf(tempstr, "Hit Points : %i-%i", minHP, maxHP);
		AddPanelString(tempstr, TRUE);
	}
	if (monstkills[mt] >= 15) {
		if (gnDifficulty != DIFF_HELL)
			res = monsterdata[mt].mMagicRes;
		else
			res = monsterdata[mt].mMagicRes2;
		res = res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING);
		if (!res) {
			strcpy(tempstr, "No magic resistance");
			AddPanelString(tempstr, TRUE);
		} else {
			if (res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING)) {
				strcpy(tempstr, "Resists : ");
				if (res & RESIST_MAGIC)
					strcat(tempstr, "Magic ");
				if (res & RESIST_FIRE)
					strcat(tempstr, "Fire ");
				if (res & RESIST_LIGHTNING)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, TRUE);
			}
			if (res & (IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING)) {
				strcpy(tempstr, "Immune : ");
				if (res & IMUNE_MAGIC)
					strcat(tempstr, "Magic ");
				if (res & IMUNE_FIRE)
					strcat(tempstr, "Fire ");
				if (res & IMUNE_LIGHTNING)
					strcat(tempstr, "Lightning ");
				tempstr[strlen(tempstr) - 1] = '\0';
				AddPanelString(tempstr, TRUE);
			}
		}
	}
	pinfoflag = 1;
}

void PrintUniqueHistory()
{
	int res;

	res = monster[pcursmonst].mMagicRes & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING | IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING);
	if (!res) {
		strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, TRUE);
		strcpy(tempstr, "No Immunities");
	} else {
		if (res & (RESIST_MAGIC | RESIST_FIRE | RESIST_LIGHTNING))
			strcpy(tempstr, "Some Magic Resistances");
		else
			strcpy(tempstr, "No resistances");
		AddPanelString(tempstr, TRUE);
		if (res & (IMUNE_MAGIC | IMUNE_FIRE | IMUNE_LIGHTNING)) {
			strcpy(tempstr, "Some Magic Immunities");
		} else {
			strcpy(tempstr, "No Immunities");
		}
	}
	AddPanelString(tempstr, TRUE);
	pinfoflag = TRUE;
}

void MissToMonst(int i, int x, int y)
{
	int oldx, oldy;
	int newx, newy;
	int m, pnum;
	MissileStruct *Miss;
	MonsterStruct *Monst;

	if ((DWORD)i >= MAXMISSILES)
		app_fatal("MissToMonst: Invalid missile %d", i);

	Miss = &missile[i];
	m = Miss->_misource;

	if ((DWORD)m >= MAXMONSTERS)
		app_fatal("MissToMonst: Invalid monster %d", m);

	Monst = &monster[m];
	oldx = Miss->_mix;
	oldy = Miss->_miy;
	dMonster[x][y] = m + 1;
	Monst->_mdir = Miss->_mimfnum;
	Monst->_mx = x;
	Monst->_my = y;
	M_StartStand(m, Monst->_mdir);
	if (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN) {
		if (!(Monst->_mFlags & MFLAG_TARGETS_MONSTER))
			M_StartHit(m, -1, 0);
		else
			M2MStartHit(m, -1, 0);
	} else {
		M_StartFadein(m, Monst->_mdir, FALSE);
	}

	if (!(Monst->_mFlags & MFLAG_TARGETS_MONSTER)) {
		pnum = dPlayer[oldx][oldy] - 1;
		if (dPlayer[oldx][oldy] > 0) {
			if (Monst->MType->mtype != MT_GLOOM && (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN)) {
				M_TryH2HHit(m, dPlayer[oldx][oldy] - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (pnum == dPlayer[oldx][oldy] - 1 && (Monst->MType->mtype < MT_NSNAKE || Monst->MType->mtype > MT_GSNAKE)) {
					if (plr[pnum]._pmode != 7 && plr[pnum]._pmode != 8)
						StartPlrHit(pnum, 0, 1);
					newx = oldx + offset_x[Monst->_mdir];
					newy = oldy + offset_y[Monst->_mdir];
					if (PosOkPlayer(pnum, newx, newy)) {
						plr[pnum].WorldX = newx;
						plr[pnum].WorldY = newy;
						FixPlayerLocation(pnum, plr[pnum]._pdir);
						FixPlrWalkTags(pnum);
						dPlayer[newx][newy] = pnum + 1;
						SetPlayerOld(pnum);
					}
				}
			}
		}
	} else {
		if (dMonster[oldx][oldy] > 0) {
			if (Monst->MType->mtype != MT_GLOOM && (Monst->MType->mtype < MT_INCIN || Monst->MType->mtype > MT_HELLBURN)) {
				M_TryM2MHit(m, dMonster[oldx][oldy] - 1, 500, Monst->mMinDamage2, Monst->mMaxDamage2);
				if (Monst->MType->mtype < MT_NSNAKE || Monst->MType->mtype > MT_GSNAKE) {
					newx = oldx + offset_x[Monst->_mdir];
					newy = oldy + offset_y[Monst->_mdir];
					if (PosOkMonst(dMonster[oldx][oldy] - 1, newx, newy)) {
						m = dMonster[oldx][oldy];
						dMonster[newx][newy] = m;
						dMonster[oldx][oldy] = 0;
						m--;
						monster[m]._mx = newx;
						monster[m]._mfutx = newx;
						monster[m]._my = newy;
						monster[m]._mfuty = newy;
					}
				}
			}
		}
	}
}

BOOL PosOkMonst(int i, int x, int y)
{
	int oi, mi, j;
	BOOL ret, fire;

	fire = FALSE;
	ret = !SolidLoc(x, y) && !dPlayer[x][y] && !dMonster[x][y];
	if (ret && dObject[x][y]) {
		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
		if (object[oi]._oSolidFlag)
			ret = FALSE;
	}

	if (ret && dMissile[x][y] && i >= 0) {
		mi = dMissile[x][y];
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (done)
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MIS_FIREWALL)
						fire = TRUE;
				}
			}
		}
		if (fire && (!(monster[i].mMagicRes & IMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO))
			ret = FALSE;
	}

	return ret;
}

BOOL PosOkMonst2(int i, int x, int y)
{
	int oi, mi, j;
	BOOL ret, fire;

	fire = FALSE;
	ret = !SolidLoc(x, y);
	if (ret && dObject[x][y]) {
		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
		if (object[oi]._oSolidFlag)
			ret = FALSE;
	}

	if (ret && dMissile[x][y] && i >= 0) {
		mi = dMissile[x][y];
		if (mi > 0) {
			if (missile[mi - 1]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1' (done)
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					if (missile[missileactive[j]]._mitype == MIS_FIREWALL)
						fire = TRUE;
				}
			}
		}
		if (fire && (!(monster[i].mMagicRes & IMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO))
			ret = FALSE;
	}

	return ret;
}

BOOL PosOkMonst3(int i, int x, int y)
{
	int j, oi, objtype, mi;
	BOOL ret, fire, isdoor;

	fire = FALSE;
	ret = TRUE;
	isdoor = FALSE;

	if (ret && dObject[x][y] != 0) {
		oi = dObject[x][y] > 0 ? dObject[x][y] - 1 : -(dObject[x][y] + 1);
		objtype = object[oi]._otype;
		isdoor = objtype == OBJ_L1LDOOR || objtype == OBJ_L1RDOOR
		    || objtype == OBJ_L2LDOOR || objtype == OBJ_L2RDOOR
		    || objtype == OBJ_L3LDOOR || objtype == OBJ_L3RDOOR;
		if (object[oi]._oSolidFlag && !isdoor) {
			ret = FALSE;
		}
	}
	if (ret) {
		ret = (!SolidLoc(x, y) || isdoor) && dPlayer[x][y] == 0 && dMonster[x][y] == 0;
	}
	if (ret && dMissile[x][y] != 0 && i >= 0) {
		mi = dMissile[x][y];
		if (mi > 0) {
			if (missile[mi]._mitype == MIS_FIREWALL) { // BUGFIX: Change 'mi' to 'mi - 1'
				fire = TRUE;
			} else {
				for (j = 0; j < nummissiles; j++) {
					mi = missileactive[j];
					if (missile[mi]._mitype == MIS_FIREWALL) {
						fire = TRUE;
					}
				}
			}
		}
		if (fire && (!(monster[i].mMagicRes & IMUNE_FIRE) || monster[i].MType->mtype == MT_DIABLO)) {
			ret = FALSE;
		}
	}

	return ret;
}

BOOL IsSkel(int mt)
{
	return mt >= MT_WSKELAX && mt <= MT_XSKELAX
	    || mt >= MT_WSKELBW && mt <= MT_XSKELBW
	    || mt >= MT_WSKELSD && mt <= MT_XSKELSD;
}

BOOL IsGoat(int mt)
{
	return mt >= MT_NGOATMC && mt <= MT_GGOATMC
	    || mt >= MT_NGOATBW && mt <= MT_GGOATBW;
}

int M_SpawnSkel(int x, int y, int dir)
{
	int i, j, skeltypes, skel;

	j = 0;
	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(Monsters[i].mtype))
			j++;
	}

	if (j) {
		skeltypes = random(136, j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(Monsters[i].mtype))
				j++;
		}
		skel = AddMonster(x, y, dir, i - 1, 1);
		if (skel != -1)
			M_StartSpStand(skel, dir);

		return skel;
	}

	return -1;
}

void ActivateSpawn(int i, int x, int y, int dir)
{
	dMonster[x][y] = i + 1;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	M_StartSpStand(i, dir);
}

BOOL SpawnSkeleton(int ii, int x, int y)
{
	int dx, dy, xx, yy, dir, j, k, rs;
	BOOL savail;
	int monstok[3][3];

	if (ii == -1)
		return FALSE;

	if (PosOkMonst(-1, x, y)) {
		dir = GetDirection(x, y, x, y);
		ActivateSpawn(ii, x, y, dir);
		return TRUE;
	}

	savail = FALSE;
	yy = 0;
	for (j = y - 1; j <= y + 1; j++) {
		xx = 0;
		for (k = x - 1; k <= x + 1; k++) {
			monstok[xx][yy] = PosOkMonst(-1, k, j);
			savail |= monstok[xx][yy];
			xx++;
		}
		yy++;
	}
	if (!savail) {
		return FALSE;
	}

	rs = random(137, 15) + 1;
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

	dx = x - 1 + xx;
	dy = y - 1 + yy;
	dir = GetDirection(dx, dy, x, y);
	ActivateSpawn(ii, dx, dy, dir);

	return TRUE;
}

int PreSpawnSkeleton()
{
	int i, j, skeltypes, skel;

	j = 0;

	if (nummtypes <= 0)
		return -1;

	for (i = 0; i < nummtypes; i++) {
		if (IsSkel(Monsters[i].mtype))
			j++;
	}

	if (j) {
		skeltypes = random(136, j);
		j = 0;
		for (i = 0; i < nummtypes && j <= skeltypes; i++) {
			if (IsSkel(Monsters[i].mtype))
				j++;
		}
		skel = AddMonster(0, 0, 0, i - 1, 0);
		if (skel != -1)
			M_StartStand(skel, 0);

		return skel;
	}

	return -1;
}

void TalktoMonster(int i)
{
	MonsterStruct *Monst;
	int pnum, itm;

	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("TalktoMonster: Invalid monster %d", i);

	Monst = &monster[i];
	pnum = Monst->_menemy;
	Monst->_mmode = MM_TALK;
	if (Monst->_mAi == AI_SNOTSPIL || Monst->_mAi == AI_LACHDAN) {
		if (QuestStatus(QTYPE_BOL) && quests[QTYPE_BOL]._qvar1 == 2 && PlrHasItem(pnum, IDI_BANNER, &itm)) {
			RemoveInvItem(pnum, itm);
			quests[QTYPE_BOL]._qactive = 3;
			Monst->mtalkmsg = QUEST_BANNER12;
			Monst->_mgoal = MGOAL_INQUIRING;
		}
		if (QuestStatus(QTYPE_VEIL) && Monst->mtalkmsg >= QUEST_VEIL9) {
			if (PlrHasItem(pnum, IDI_GLDNELIX, &itm)) {
				RemoveInvItem(pnum, itm);
				Monst->mtalkmsg = QUEST_VEIL11;
				Monst->_mgoal = MGOAL_INQUIRING;
			}
		}
	}
}

void SpawnGolum(int i, int x, int y, int mi)
{
	if ((DWORD)i >= MAXMONSTERS)
		app_fatal("SpawnGolum: Invalid monster %d", i);

	dMonster[x][y] = i + 1;
	monster[i]._mx = x;
	monster[i]._my = y;
	monster[i]._mfutx = x;
	monster[i]._mfuty = y;
	monster[i]._moldx = x;
	monster[i]._moldy = y;
	monster[i]._pathcount = 0;
	monster[i]._mFlags |= MFLAG_GOLEM;
	monster[i].mArmorClass = 25;
	monster[i]._mmaxhp = 2 * (320 * missile[mi]._mispllvl + plr[i]._pMaxMana / 3);
	monster[i]._mhitpoints = monster[i]._mmaxhp;
	monster[i].mHit = 5 * (missile[mi]._mispllvl + 8) + 2 * plr[i]._pLevel;
	monster[i].mMinDamage = 2 * (missile[mi]._mispllvl + 4);
	monster[i].mMaxDamage = 2 * (missile[mi]._mispllvl + 8);
	M_StartSpStand(i, 0);
	M_Enemy(i);
	if (i == myplr) {
		NetSendCmdGolem(
		    monster[i]._mx,
		    monster[i]._my,
		    monster[i]._mdir,
		    monster[i]._menemy,
		    monster[i]._mhitpoints,
		    currlevel);
	}
}

BOOL CanTalkToMonst(int m)
{
	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("CanTalkToMonst: Invalid monster %d", m);
	}

	if (monster[m]._mgoal == MGOAL_INQUIRING) {
		return TRUE;
	}

	return monster[m]._mgoal == MGOAL_TALKING;
}

BOOL CheckMonsterHit(int m, BOOL *ret)
{
	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("CheckMonsterHit: Invalid monster %d", m);
	}

	if (monster[m]._mAi == AI_GARG && monster[m]._mFlags & MFLAG_ALLOW_SPECIAL) {
		monster[m]._mmode = MM_SATTACK;
		monster[m]._mFlags &= ~MFLAG_ALLOW_SPECIAL;
		*ret = TRUE;
		return TRUE;
	}

	if (monster[m].MType->mtype < MT_COUNSLR || monster[m].MType->mtype > MT_ADVOCATE || monster[m]._mgoal == MGOAL_NORMAL) {
		return FALSE;
	} else {
		*ret = FALSE;
	}

	return TRUE;
}

int encode_enemy(int m)
{
	int enemy;

	enemy = monster[m]._menemy;
	if (monster[m]._mFlags & MFLAG_TARGETS_MONSTER)
		enemy += MAX_PLRS;

	return enemy;
}

void decode_enemy(int m, int enemy)
{
	if (enemy < MAX_PLRS) {
		monster[m]._mFlags &= ~MFLAG_TARGETS_MONSTER;
		monster[m]._menemy = enemy;
		monster[m]._menemyx = plr[enemy]._px;
		monster[m]._menemyy = plr[enemy]._py;
	} else {
		monster[m]._mFlags |= MFLAG_TARGETS_MONSTER;
		enemy -= MAX_PLRS;
		monster[m]._menemy = enemy;
		monster[m]._menemyx = monster[enemy]._mfutx;
		monster[m]._menemyy = monster[enemy]._mfuty;
	}
}

DEVILUTION_END_NAMESPACE
