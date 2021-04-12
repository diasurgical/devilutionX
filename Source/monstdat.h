/**
 * @file monstdat.h
 *
 * Interface of all monster data.
 */
#pragma once

#include <stdint.h>

namespace devilution {

enum _mai_id : int8_t {
	AI_ZOMBIE,
	AI_FAT,
	AI_SKELSD,
	AI_SKELBOW,
	AI_SCAV,
	AI_RHINO,
	AI_GOATMC,
	AI_GOATBOW,
	AI_FALLEN,
	AI_MAGMA,
	AI_SKELKING,
	AI_BAT,
	AI_GARG,
	AI_CLEAVER,
	AI_SUCC,
	AI_SNEAK,
	AI_STORM,
	AI_FIREMAN,
	AI_GARBUD,
	AI_ACID,
	AI_ACIDUNIQ,
	AI_GOLUM,
	AI_ZHAR,
	AI_SNOTSPIL,
	AI_SNAKE,
	AI_COUNSLR,
	AI_MEGA,
	AI_DIABLO,
	AI_LAZURUS,
	AI_LAZHELP,
	AI_LACHDAN,
	AI_WARLORD,
	AI_FIREBAT,
	AI_TORCHANT,
	AI_HORKDMN,
	AI_LICH,
	AI_ARCHLICH,
	AI_PSYCHORB,
	AI_NECROMORB,
	AI_BONEDEMON,
	AI_INVALID = -1,
};

enum _mc_id : uint8_t {
	MC_UNDEAD,
	MC_DEMON,
	MC_ANIMAL,
};

enum monster_resistance : uint8_t {
	// clang-format off
	RESIST_MAGIC     = 1 << 0,
	RESIST_FIRE      = 1 << 1,
	RESIST_LIGHTNING = 1 << 2,
	IMMUNE_MAGIC     = 1 << 3,
	IMMUNE_FIRE      = 1 << 4,
	IMMUNE_LIGHTNING = 1 << 5,
	IMMUNE_NULL_40   = 1 << 6,
	IMMUNE_ACID      = 1 << 7,
	// clang-format on
};

struct MonsterData {
	const char *mName;
	const char *GraphicType;
	const char *sndfile;
	const char *TransFile;
	Uint16 width;
	Uint16 mImage;
	bool has_special;
	bool snd_special;
	bool has_trans;
	Uint8 Frames[6];
	Uint8 Rate[6];
	Sint8 mMinDLvl;
	Sint8 mMaxDLvl;
	Sint8 mLevel;
	Uint16 mMinHP;
	Uint16 mMaxHP;
	_mai_id mAi;
	/** Usign monster_flag as bitflags */
	Uint16 mFlags;
	Uint8 mInt;
	Uint8 mHit;
	Uint8 mAFNum;
	Uint8 mMinDamage;
	Uint8 mMaxDamage;
	Uint8 mHit2;
	Uint8 mAFNum2;
	Uint8 mMinDamage2;
	Uint8 mMaxDamage2;
	Uint8 mArmorClass;
	_mc_id mMonstClass;
	/** Using monster_resistance as bitflags */
	Uint8 mMagicRes;
	/** Using monster_resistance as bitflags */
	Uint8 mMagicRes2;
	Sint8 mSelFlag;   // TODO Create enum
	Uint16 mTreasure; // TODO Create enum
	Uint16 mExp;
};

enum _monster_id : int16_t {
	MT_NZOMBIE,
	MT_BZOMBIE,
	MT_GZOMBIE,
	MT_YZOMBIE,
	MT_RFALLSP,
	MT_DFALLSP,
	MT_YFALLSP,
	MT_BFALLSP,
	MT_WSKELAX,
	MT_TSKELAX,
	MT_RSKELAX,
	MT_XSKELAX,
	MT_RFALLSD,
	MT_DFALLSD,
	MT_YFALLSD,
	MT_BFALLSD,
	MT_NSCAV,
	MT_BSCAV,
	MT_WSCAV,
	MT_YSCAV,
	MT_WSKELBW,
	MT_TSKELBW,
	MT_RSKELBW,
	MT_XSKELBW,
	MT_WSKELSD,
	MT_TSKELSD,
	MT_RSKELSD,
	MT_XSKELSD,
	MT_INVILORD,
	MT_SNEAK,
	MT_STALKER,
	MT_UNSEEN,
	MT_ILLWEAV,
	MT_LRDSAYTR,
	MT_NGOATMC,
	MT_BGOATMC,
	MT_RGOATMC,
	MT_GGOATMC,
	MT_FIEND,
	MT_BLINK,
	MT_GLOOM,
	MT_FAMILIAR,
	MT_NGOATBW,
	MT_BGOATBW,
	MT_RGOATBW,
	MT_GGOATBW,
	MT_NACID,
	MT_RACID,
	MT_BACID,
	MT_XACID,
	MT_SKING,
	MT_CLEAVER,
	MT_FAT,
	MT_MUDMAN,
	MT_TOAD,
	MT_FLAYED,
	MT_WYRM,
	MT_CAVSLUG,
	MT_DVLWYRM,
	MT_DEVOUR,
	MT_NMAGMA,
	MT_YMAGMA,
	MT_BMAGMA,
	MT_WMAGMA,
	MT_HORNED,
	MT_MUDRUN,
	MT_FROSTC,
	MT_OBLORD,
	MT_BONEDMN,
	MT_REDDTH,
	MT_LTCHDMN,
	MT_UDEDBLRG,
	MT_INCIN,
	MT_FLAMLRD,
	MT_DOOMFIRE,
	MT_HELLBURN,
	MT_STORM,
	MT_RSTORM,
	MT_STORML,
	MT_MAEL,
	MT_BIGFALL,
	MT_WINGED,
	MT_GARGOYLE,
	MT_BLOODCLW,
	MT_DEATHW,
	MT_MEGA,
	MT_GUARD,
	MT_VTEXLRD,
	MT_BALROG,
	MT_NSNAKE,
	MT_RSNAKE,
	MT_BSNAKE,
	MT_GSNAKE,
	MT_NBLACK,
	MT_RTBLACK,
	MT_BTBLACK,
	MT_RBLACK,
	MT_UNRAV,
	MT_HOLOWONE,
	MT_PAINMSTR,
	MT_REALWEAV,
	MT_SUCCUBUS,
	MT_SNOWWICH,
	MT_HLSPWN,
	MT_SOLBRNR,
	MT_COUNSLR,
	MT_MAGISTR,
	MT_CABALIST,
	MT_ADVOCATE,
	MT_GOLEM,
	MT_DIABLO,
	MT_DARKMAGE,
	MT_HELLBOAR,
	MT_STINGER,
	MT_PSYCHORB,
	MT_ARACHNON,
	MT_FELLTWIN,
	MT_HORKSPWN,
	MT_VENMTAIL,
	MT_NECRMORB,
	MT_SPIDLORD,
	MT_LASHWORM,
	MT_TORCHANT,
	MT_HORKDMN,
	MT_DEFILER,
	MT_GRAVEDIG,
	MT_TOMBRAT,
	MT_FIREBAT,
	MT_SKLWING,
	MT_LICH,
	MT_CRYPTDMN,
	MT_HELLBAT,
	MT_BONEDEMN,
	MT_ARCHLICH,
	MT_BICLOPS,
	MT_FLESTHNG,
	MT_REAPER,
	MT_NAKRUL,
	NUM_MTYPES,
	MT_INVALID = -1,
};

enum _monster_availability : uint8_t {
	MAT_NEVER,
	MAT_ALWAYS,
	MAT_RETAIL,
};

struct UniqMonstStruct {
	_monster_id mtype;
	const char *mName;
	const char *mTrnName;
	Uint8 mlevel;
	Uint16 mmaxhp;
	_mai_id mAi;
	Uint8 mint;
	Uint8 mMinDamage;
	Uint8 mMaxDamage;
	/** Using monster_resistance as bitflags */
	Uint16 mMagicRes;
	Uint16 mUnqAttr; // TODO create enum
	Uint8 mUnqVar1;
	Uint8 mUnqVar2;
	Sint32 mtalkmsg;
};

extern const MonsterData monsterdata[];
extern const _monster_id MonstConvTbl[];
extern const char MonstAvailTbl[];
extern const UniqMonstStruct UniqMonst[];

}
