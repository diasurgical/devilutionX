/**
 * @file misdat.h
 *
 * Interface of data related to missiles.
 */
#pragma once

#include <cstdint>
#include <vector>

#include "effects.h"
#include "engine.h"
#include "engine/clx_sprite.hpp"
#include "utils/stdcompat/cstddef.hpp"
#include "utils/stdcompat/string_view.hpp"

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
	MFILE_NONE,
} missile_graphic_id;

/**
 * @brief Specifies what if and how movement distribution is applied
 */
enum class MissileMovementDistribution : uint8_t {
	/**
	 * @brief No movement distribution is calculated. Normally this means the missile doesn't move at all.
	 */
	Disabled,
	/**
	 * @brief The missile moves and if it hits a enemey it stops (for example firebolt)
	 */
	Blockable,
	/**
	 * @brief The missile moves and even it hits a enemy it keeps moving (for example flame wave)
	 */
	Unblockable,
};

struct Missile;
struct AddMissileParameter;

struct MissileData {
	void (*mAddProc)(Missile &, const AddMissileParameter &);
	void (*mProc)(Missile &);
	uint8_t mName;
	bool mDraw;
	uint8_t mType;
	missile_resistance mResist;
	uint8_t mFileNum;
	_sfx_id mlSFX;
	_sfx_id miSFX;
	MissileMovementDistribution MovementDistribution;
};

enum class MissileDataFlags : uint8_t {
	// clang-format off
	None         = 0,
	MonsterOwned = 1 << 0,
	NotAnimated  = 1 << 1,
	// clang-format on
};

struct MissileFileData {
	string_view name;
	uint8_t animName;
	uint8_t animFAmt;
	MissileDataFlags flags;
	std::array<uint8_t, 16> animDelay = {};
	std::array<uint8_t, 16> animLen = {};
	uint16_t animWidth;
	int16_t animWidth2;
	OptionalOwnedClxSpriteListOrSheet sprites;

	MissileFileData(string_view name, uint8_t animName, uint8_t animFAmt, MissileDataFlags flags,
	    std::initializer_list<uint8_t> animDelay, std::initializer_list<uint8_t> animLen,
	    uint16_t animWidth, int16_t animWidth2);

	void LoadGFX();

	void FreeGFX()
	{
		sprites = std::nullopt;
	}

	/**
	 * @brief Returns the sprite list for a given direction.
	 *
	 * @param direction One of the 16 directions. Valid range: [0, 15].
	 * @return OptionalClxSpriteList
	 */
	[[nodiscard]] OptionalClxSpriteList spritesForDirection(size_t direction) const
	{
		if (!sprites)
			return std::nullopt;
		return sprites->isSheet() ? sprites->sheet()[direction] : sprites->list();
	}
};

extern MissileData MissilesData[];
extern MissileFileData MissileSpriteData[];

void InitMissileGFX(bool loadHellfireGraphics = false);
void FreeMissileGFX();

} // namespace devilution
