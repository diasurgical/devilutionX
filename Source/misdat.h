/**
 * @file misdat.h
 *
 * Interface of data related to missiles.
 */
#pragma once

#include <cstdint>

#include "engine.h"
#include "effects.h"
#include "utils/stdcompat/cstddef.hpp"

namespace devilution {

enum mienemy_type : uint8_t {
	TARGET_MONSTERS,
	TARGET_PLAYERS,
	TARGET_BOTH,
};

enum missile_resistance : uint8_t {
	MISR_NONE,
	MISR_FIRE,
	MISR_LIGHTNING,
	MISR_MAGIC,
	MISR_ACID,
};

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

typedef struct MissileData {
	void (*mAddProc)(int, Point, Point, int, int8_t, int, int);
	void (*mProc)(int);
	uint8_t mName;
	bool mDraw;
	uint8_t mType;
	missile_resistance mResist;
	uint8_t mFileNum;
	_sfx_id mlSFX;
	_sfx_id miSFX;
} MissileData;

typedef struct MisFileData {
	const char *mName;
	uint8_t mAnimName;
	uint8_t mAnimFAmt;
	uint32_t mFlags;
	byte *mAnimData[16];
	uint8_t mAnimDelay[16];
	uint8_t mAnimLen[16];
	int16_t mAnimWidth[16];
	int16_t mAnimWidth2[16];
} MisFileData;

extern MissileData missiledata[];
extern MisFileData misfiledata[];

} // namespace devilution
