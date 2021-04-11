/**
 * @file misdat.h
 *
 * Interface of data related to missiles.
 */
#pragma once

#include <stdint.h>

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

}
