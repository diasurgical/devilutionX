/**
 * @file monstdat.h
 *
 * Interface of all monster data.
 */
#ifndef __MONSTDAT_H__
#define __MONSTDAT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _mai_id {
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
} _mai_id;

typedef enum _mc_id {
	MC_UNDEAD,
	MC_DEMON,
	MC_ANIMAL,
} _mc_id;

typedef enum monster_resistance {
	// clang-format off
	RESIST_MAGIC     = 0x01,
	RESIST_FIRE      = 0x02,
	RESIST_LIGHTNING = 0x04,
	IMMUNE_MAGIC     = 0x08,
	IMMUNE_FIRE      = 0x10,
	IMMUNE_LIGHTNING = 0x20,
	IMMUNE_NULL_40   = 0x40,
	IMMUNE_ACID      = 0x80,
	// clang-format on
} monster_resistance;

typedef struct MonsterData {
	Sint32 width;
	Sint32 mImage;
	const char *GraphicType;
	bool has_special;
	const char *sndfile;
	bool snd_special;
	bool has_trans;
	const char *TransFile;
	Sint32 Frames[6];
	Sint32 Rate[6];
	const char *mName;
	Sint8 mMinDLvl;
	Sint8 mMaxDLvl;
	Sint8 mLevel;
	Sint32 mMinHP;
	Sint32 mMaxHP;
	_mai_id mAi;
	/** Usign monster_flag as bitflags */
	Sint32 mFlags;
	Uint8 mInt;
	Uint16 mHit;
	Uint8 mAFNum;
	Uint8 mMinDamage;
	Uint8 mMaxDamage;
	Uint16 mHit2;
	Uint8 mAFNum2;
	Uint8 mMinDamage2;
	Uint8 mMaxDamage2;
	Uint8 mArmorClass;
	_mc_id mMonstClass;
	/** Using monster_resistance as bitflags */
	Uint16 mMagicRes;
	/** Using monster_resistance as bitflags */
	Uint16 mMagicRes2;
	Uint16 mTreasure; // TODO Create enum
	Sint8 mSelFlag;   // TODO Create enum
	Uint16 mExp;
} MonsterData;

typedef enum _monster_id {
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
} _monster_id;

typedef enum _monster_availability {
	MAT_NEVER,
	MAT_ALWAYS,
	MAT_RETAIL,
} _monster_availability;

typedef struct UniqMonstStruct {
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
} UniqMonstStruct;

extern const MonsterData monsterdata[];
extern const _monster_id MonstConvTbl[];
extern const char MonstAvailTbl[];
extern const UniqMonstStruct UniqMonst[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MONSTDAT_H__ */
