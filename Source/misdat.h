/**
 * @file misdat.h
 *
 * Interface of data related to missiles.
 */
#pragma once

#include <stdint.h>
#include <string_view>

#include "effects.h"

namespace devilution {

enum mienemy_type : uint8_t {
	TARGET_MONSTERS,
	TARGET_PLAYERS,
	TARGET_BOTH,
};

[[maybe_unused]] constexpr std::string_view toString(mienemy_type value)
{
	switch(value) {
	case TARGET_MONSTERS:
		return "Monsters";
	case TARGET_PLAYERS:
		return "Players";
	case TARGET_BOTH:
		return "Both";
	}
}

enum missile_resistance : uint8_t {
	MISR_NONE,
	MISR_FIRE,
	MISR_LIGHTNING,
	MISR_MAGIC,
	MISR_ACID,
};

[[maybe_unused]] constexpr std::string_view toString(missile_resistance value)
{
	switch(value) {
	case MISR_NONE:
		return "None";
	case MISR_FIRE:
		return "Fire";
	case MISR_LIGHTNING:
		return "Lightning";
	case MISR_MAGIC:
		return "Magic";
	case MISR_ACID:
		return "Acid";
	}
}

typedef enum missile_graphic_id : uint8_t {
	MFILE_ARROWS,
	MFILE_FIREBA,
	MFILE_GUARD,
	MFILE_LGHNING,
	MFILE_FIREWAL,
	MFILE_MAGBLOS,
	MFILE_PORTAL,
	MFILE_BLUEXFR,
	MFILE_BLUEXBK,
	MFILE_MANASHLD,
	MFILE_BLOOD,
	MFILE_BONE,
	MFILE_METLHIT,
	MFILE_FARROW,
	MFILE_DOOM,
	MFILE_0F,
	MFILE_BLODBUR,
	MFILE_NEWEXP,
	MFILE_SHATTER1,
	MFILE_BIGEXP,
	MFILE_INFERNO,
	MFILE_THINLGHT,
	MFILE_FLARE,
	MFILE_FLAREEXP,
	MFILE_MAGBALL,
	MFILE_KRULL,
	MFILE_MINILTNG,
	MFILE_HOLY,
	MFILE_HOLYEXPL,
	MFILE_LARROW,
	MFILE_FIRARWEX,
	MFILE_ACIDBF,
	MFILE_ACIDSPLA,
	MFILE_ACIDPUD,
	MFILE_ETHRSHLD,
	MFILE_FIRERUN,
	MFILE_RESSUR1,
	MFILE_SKLBALL,
	MFILE_RPORTAL,
	MFILE_FIREPLAR,
	MFILE_SCUBMISB,
	MFILE_SCBSEXPB,
	MFILE_SCUBMISC,
	MFILE_SCBSEXPC,
	MFILE_SCUBMISD,
	MFILE_SCBSEXPD,
	MFILE_SPAWNS,
	MFILE_REFLECT,
	MFILE_LICH,
	MFILE_MSBLA,
	MFILE_NECROMORB,
	MFILE_ARCHLICH,
	MFILE_RUNE,
	MFILE_EXYEL2,
	MFILE_EXBL2,
	MFILE_EXRED3,
	MFILE_BONEDEMON,
	MFILE_EXORA1,
	MFILE_EXBL3,
	MFILE_NONE, // BUGFIX: should be `MFILE_NONE = MFILE_SCBSEXPD+1`, i.e. MFILE_NULL, since there would otherwise be an out-of-bounds in SetMissAnim when accessing misfiledata for any of the missiles that have MFILE_NONE as mFileNum in missiledata. (fixed)
} missile_graphic_id;

[[maybe_unused]] constexpr std::string_view toString(missile_graphic_id value)
{
	switch(value) {
	case MFILE_ARROWS:
		return "Arrows";
	case MFILE_FIREBA:
		return "Fireba";
	case MFILE_GUARD:
		return "Guard";
	case MFILE_LGHNING:
		return "Lghning";
	case MFILE_FIREWAL:
		return "Firewal";
	case MFILE_MAGBLOS:
		return "Magblos";
	case MFILE_PORTAL:
		return "Portal";
	case MFILE_BLUEXFR:
		return "Bluexfr";
	case MFILE_BLUEXBK:
		return "Bluexbk";
	case MFILE_MANASHLD:
		return "Manashld";
	case MFILE_BLOOD:
		return "Blood";
	case MFILE_BONE:
		return "Bone";
	case MFILE_METLHIT:
		return "Metlhit";
	case MFILE_FARROW:
		return "Farrow";
	case MFILE_DOOM:
		return "Doom";
	case MFILE_0F:
		return "0f";
	case MFILE_BLODBUR:
		return "Blodbur";
	case MFILE_NEWEXP:
		return "Newexp";
	case MFILE_SHATTER1:
		return "Shatter1";
	case MFILE_BIGEXP:
		return "Bigexp";
	case MFILE_INFERNO:
		return "Inferno";
	case MFILE_THINLGHT:
		return "Thinlght";
	case MFILE_FLARE:
		return "Flare";
	case MFILE_FLAREEXP:
		return "Flareexp";
	case MFILE_MAGBALL:
		return "Magball";
	case MFILE_KRULL:
		return "Krull";
	case MFILE_MINILTNG:
		return "Miniltng";
	case MFILE_HOLY:
		return "Holy";
	case MFILE_HOLYEXPL:
		return "Holyexpl";
	case MFILE_LARROW:
		return "Larrow";
	case MFILE_FIRARWEX:
		return "Firarwex";
	case MFILE_ACIDBF:
		return "Acidbf";
	case MFILE_ACIDSPLA:
		return "Acidspla";
	case MFILE_ACIDPUD:
		return "Acidpud";
	case MFILE_ETHRSHLD:
		return "Ethrshld";
	case MFILE_FIRERUN:
		return "Firerun";
	case MFILE_RESSUR1:
		return "Ressur1";
	case MFILE_SKLBALL:
		return "Sklball";
	case MFILE_RPORTAL:
		return "Rportal";
	case MFILE_FIREPLAR:
		return "Fireplar";
	case MFILE_SCUBMISB:
		return "Scubmisb";
	case MFILE_SCBSEXPB:
		return "Scbsexpb";
	case MFILE_SCUBMISC:
		return "Scubmisc";
	case MFILE_SCBSEXPC:
		return "Scbsexpc";
	case MFILE_SCUBMISD:
		return "Scubmisd";
	case MFILE_SCBSEXPD:
		return "Scbsexpd";
	case MFILE_SPAWNS:
		return "Spawns";
	case MFILE_REFLECT:
		return "Reflect";
	case MFILE_LICH:
		return "Lich";
	case MFILE_MSBLA:
		return "Msbla";
	case MFILE_NECROMORB:
		return "Necromorb";
	case MFILE_ARCHLICH:
		return "Archlich";
	case MFILE_RUNE:
		return "Rune";
	case MFILE_EXYEL2:
		return "Exyel2";
	case MFILE_EXBL2:
		return "Exbl2";
	case MFILE_EXRED3:
		return "Exred3";
	case MFILE_BONEDEMON:
		return "Bonedemon";
	case MFILE_EXORA1:
		return "Exora1";
	case MFILE_EXBL3:
		return "Exbl3";
	case MFILE_NONE:
		return "None";
	}
}

typedef struct MissileData {
	void (*mAddProc)(Sint32, Sint32, Sint32, Sint32, Sint32, Sint32, Sint8, Sint32, Sint32);
	void (*mProc)(Sint32);
	Uint8 mName;
	bool mDraw;
	Uint8 mType;
	missile_resistance mResist;
	Uint8 mFileNum;
	_sfx_id mlSFX;
	_sfx_id miSFX;
} MissileData;

typedef struct MisFileData {
	const char *mName;
	Uint8 mAnimName;
	Uint8 mAnimFAmt;
	Sint32 mFlags;
	Uint8 *mAnimData[16];
	Uint8 mAnimDelay[16];
	Uint8 mAnimLen[16];
	Sint16 mAnimWidth[16];
	Sint16 mAnimWidth2[16];
} MisFileData;

extern MissileData missiledata[];
extern MisFileData misfiledata[];

} // namespace devilution
