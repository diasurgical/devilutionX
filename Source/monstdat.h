/**
 * @file monstdat.h
 *
 * Interface of all monster data.
 */
#pragma once

#include <SDL.h>
#include <stdint.h>
#include <string_view>

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

[[maybe_unused]] constexpr std::string_view toString(_mai_id value)
{
	switch(value) {
	case AI_ZOMBIE:
		return "Zombie";
	case AI_FAT:
		return "Fat";
	case AI_SKELSD:
		return "Skelsd";
	case AI_SKELBOW:
		return "Skelbow";
	case AI_SCAV:
		return "Scav";
	case AI_RHINO:
		return "Rhino";
	case AI_GOATMC:
		return "Goatmc";
	case AI_GOATBOW:
		return "Goatbow";
	case AI_FALLEN:
		return "Fallen";
	case AI_MAGMA:
		return "Magma";
	case AI_SKELKING:
		return "Skelking";
	case AI_BAT:
		return "Bat";
	case AI_GARG:
		return "Garg";
	case AI_CLEAVER:
		return "Cleaver";
	case AI_SUCC:
		return "Succ";
	case AI_SNEAK:
		return "Sneak";
	case AI_STORM:
		return "Storm";
	case AI_FIREMAN:
		return "Fireman";
	case AI_GARBUD:
		return "Garbud";
	case AI_ACID:
		return "Acid";
	case AI_ACIDUNIQ:
		return "Aciduniq";
	case AI_GOLUM:
		return "Golum";
	case AI_ZHAR:
		return "Zhar";
	case AI_SNOTSPIL:
		return "Snotspil";
	case AI_SNAKE:
		return "Snake";
	case AI_COUNSLR:
		return "Counslr";
	case AI_MEGA:
		return "Mega";
	case AI_DIABLO:
		return "Diablo";
	case AI_LAZURUS:
		return "Lazurus";
	case AI_LAZHELP:
		return "Lazhelp";
	case AI_LACHDAN:
		return "Lachdan";
	case AI_WARLORD:
		return "Warlord";
	case AI_FIREBAT:
		return "Firebat";
	case AI_TORCHANT:
		return "Torchant";
	case AI_HORKDMN:
		return "Horkdmn";
	case AI_LICH:
		return "Lich";
	case AI_ARCHLICH:
		return "Archlich";
	case AI_PSYCHORB:
		return "Psychorb";
	case AI_NECROMORB:
		return "Necromorb";
	case AI_BONEDEMON:
		return "Bonedemon";
	case AI_INVALID:
		return "Invalid";
	}
}

enum _mc_id : uint8_t {
	MC_UNDEAD,
	MC_DEMON,
	MC_ANIMAL,
};

[[maybe_unused]] constexpr std::string_view toString(_mc_id value)
{
	switch(value) {
	case MC_UNDEAD:
		return "Undead";
	case MC_DEMON:
		return "Demon";
	case MC_ANIMAL:
		return "Animal";
	}
}

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

[[maybe_unused]] constexpr std::string_view toString(_monster_id value)
{
	switch(value) {
	case MT_NZOMBIE:
		return "Nzombie";
	case MT_BZOMBIE:
		return "Bzombie";
	case MT_GZOMBIE:
		return "Gzombie";
	case MT_YZOMBIE:
		return "Yzombie";
	case MT_RFALLSP:
		return "Rfallsp";
	case MT_DFALLSP:
		return "Dfallsp";
	case MT_YFALLSP:
		return "Yfallsp";
	case MT_BFALLSP:
		return "Bfallsp";
	case MT_WSKELAX:
		return "Wskelax";
	case MT_TSKELAX:
		return "Tskelax";
	case MT_RSKELAX:
		return "Rskelax";
	case MT_XSKELAX:
		return "Xskelax";
	case MT_RFALLSD:
		return "Rfallsd";
	case MT_DFALLSD:
		return "Dfallsd";
	case MT_YFALLSD:
		return "Yfallsd";
	case MT_BFALLSD:
		return "Bfallsd";
	case MT_NSCAV:
		return "Nscav";
	case MT_BSCAV:
		return "Bscav";
	case MT_WSCAV:
		return "Wscav";
	case MT_YSCAV:
		return "Yscav";
	case MT_WSKELBW:
		return "Wskelbw";
	case MT_TSKELBW:
		return "Tskelbw";
	case MT_RSKELBW:
		return "Rskelbw";
	case MT_XSKELBW:
		return "Xskelbw";
	case MT_WSKELSD:
		return "Wskelsd";
	case MT_TSKELSD:
		return "Tskelsd";
	case MT_RSKELSD:
		return "Rskelsd";
	case MT_XSKELSD:
		return "Xskelsd";
	case MT_INVILORD:
		return "Invilord";
	case MT_SNEAK:
		return "Sneak";
	case MT_STALKER:
		return "Stalker";
	case MT_UNSEEN:
		return "Unseen";
	case MT_ILLWEAV:
		return "Illweav";
	case MT_LRDSAYTR:
		return "Lrdsaytr";
	case MT_NGOATMC:
		return "Ngoatmc";
	case MT_BGOATMC:
		return "Bgoatmc";
	case MT_RGOATMC:
		return "Rgoatmc";
	case MT_GGOATMC:
		return "Ggoatmc";
	case MT_FIEND:
		return "Fiend";
	case MT_BLINK:
		return "Blink";
	case MT_GLOOM:
		return "Gloom";
	case MT_FAMILIAR:
		return "Familiar";
	case MT_NGOATBW:
		return "Ngoatbw";
	case MT_BGOATBW:
		return "Bgoatbw";
	case MT_RGOATBW:
		return "Rgoatbw";
	case MT_GGOATBW:
		return "Ggoatbw";
	case MT_NACID:
		return "Nacid";
	case MT_RACID:
		return "Racid";
	case MT_BACID:
		return "Bacid";
	case MT_XACID:
		return "Xacid";
	case MT_SKING:
		return "Sking";
	case MT_CLEAVER:
		return "Cleaver";
	case MT_FAT:
		return "Fat";
	case MT_MUDMAN:
		return "Mudman";
	case MT_TOAD:
		return "Toad";
	case MT_FLAYED:
		return "Flayed";
	case MT_WYRM:
		return "Wyrm";
	case MT_CAVSLUG:
		return "Cavslug";
	case MT_DVLWYRM:
		return "Dvlwyrm";
	case MT_DEVOUR:
		return "Devour";
	case MT_NMAGMA:
		return "Nmagma";
	case MT_YMAGMA:
		return "Ymagma";
	case MT_BMAGMA:
		return "Bmagma";
	case MT_WMAGMA:
		return "Wmagma";
	case MT_HORNED:
		return "Horned";
	case MT_MUDRUN:
		return "Mudrun";
	case MT_FROSTC:
		return "Frostc";
	case MT_OBLORD:
		return "Oblord";
	case MT_BONEDMN:
		return "Bonedmn";
	case MT_REDDTH:
		return "Reddth";
	case MT_LTCHDMN:
		return "Ltchdmn";
	case MT_UDEDBLRG:
		return "Udedblrg";
	case MT_INCIN:
		return "Incin";
	case MT_FLAMLRD:
		return "Flamlrd";
	case MT_DOOMFIRE:
		return "Doomfire";
	case MT_HELLBURN:
		return "Hellburn";
	case MT_STORM:
		return "Storm";
	case MT_RSTORM:
		return "Rstorm";
	case MT_STORML:
		return "Storml";
	case MT_MAEL:
		return "Mael";
	case MT_BIGFALL:
		return "Bigfall";
	case MT_WINGED:
		return "Winged";
	case MT_GARGOYLE:
		return "Gargoyle";
	case MT_BLOODCLW:
		return "Bloodclw";
	case MT_DEATHW:
		return "Deathw";
	case MT_MEGA:
		return "Mega";
	case MT_GUARD:
		return "Guard";
	case MT_VTEXLRD:
		return "Vtexlrd";
	case MT_BALROG:
		return "Balrog";
	case MT_NSNAKE:
		return "Nsnake";
	case MT_RSNAKE:
		return "Rsnake";
	case MT_BSNAKE:
		return "Bsnake";
	case MT_GSNAKE:
		return "Gsnake";
	case MT_NBLACK:
		return "Nblack";
	case MT_RTBLACK:
		return "Rtblack";
	case MT_BTBLACK:
		return "Btblack";
	case MT_RBLACK:
		return "Rblack";
	case MT_UNRAV:
		return "Unrav";
	case MT_HOLOWONE:
		return "Holowone";
	case MT_PAINMSTR:
		return "Painmstr";
	case MT_REALWEAV:
		return "Realweav";
	case MT_SUCCUBUS:
		return "Succubus";
	case MT_SNOWWICH:
		return "Snowwich";
	case MT_HLSPWN:
		return "Hlspwn";
	case MT_SOLBRNR:
		return "Solbrnr";
	case MT_COUNSLR:
		return "Counslr";
	case MT_MAGISTR:
		return "Magistr";
	case MT_CABALIST:
		return "Cabalist";
	case MT_ADVOCATE:
		return "Advocate";
	case MT_GOLEM:
		return "Golem";
	case MT_DIABLO:
		return "Diablo";
	case MT_DARKMAGE:
		return "Darkmage";
	case MT_HELLBOAR:
		return "Hellboar";
	case MT_STINGER:
		return "Stinger";
	case MT_PSYCHORB:
		return "Psychorb";
	case MT_ARACHNON:
		return "Arachnon";
	case MT_FELLTWIN:
		return "Felltwin";
	case MT_HORKSPWN:
		return "Horkspwn";
	case MT_VENMTAIL:
		return "Venmtail";
	case MT_NECRMORB:
		return "Necrmorb";
	case MT_SPIDLORD:
		return "Spidlord";
	case MT_LASHWORM:
		return "Lashworm";
	case MT_TORCHANT:
		return "Torchant";
	case MT_HORKDMN:
		return "Horkdmn";
	case MT_DEFILER:
		return "Defiler";
	case MT_GRAVEDIG:
		return "Gravedig";
	case MT_TOMBRAT:
		return "Tombrat";
	case MT_FIREBAT:
		return "Firebat";
	case MT_SKLWING:
		return "Sklwing";
	case MT_LICH:
		return "Lich";
	case MT_CRYPTDMN:
		return "Cryptdmn";
	case MT_HELLBAT:
		return "Hellbat";
	case MT_BONEDEMN:
		return "Bonedemn";
	case MT_ARCHLICH:
		return "Archlich";
	case MT_BICLOPS:
		return "Biclops";
	case MT_FLESTHNG:
		return "Flesthng";
	case MT_REAPER:
		return "Reaper";
	case MT_NAKRUL:
		return "Nakrul";
	case NUM_MTYPES:
		return "Num Monster Types";
	case MT_INVALID:
		return "Invalid";
	}
}

enum _monster_availability : uint8_t {
	MAT_NEVER,
	MAT_ALWAYS,
	MAT_RETAIL,
};

[[maybe_unused]] constexpr std::string_view toString(_monster_availability value)
{
	switch(value) {
	case MAT_NEVER:
		return "Never";
	case MAT_ALWAYS:
		return "Always";
	case MAT_RETAIL:
		return "Retail";
	}
}

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

} // namespace devilution
