/**
 * @file misdat.h
 *
 * Interface of data related to missiles.
 */
#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

#include "effects.h"
#include "engine.h"
#include "engine/clx_sprite.hpp"
#include "spelldat.h"
#include "utils/stdcompat/cstddef.hpp"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

enum mienemy_type : uint8_t {
	TARGET_MONSTERS,
	TARGET_PLAYERS,
	TARGET_BOTH,
};

enum class DamageType : uint8_t {
	Physical,
	Fire,
	Lightning,
	Magic,
	Acid,
};

enum class MissileGraphicID : uint8_t {
	Arrow,
	Fireball,
	Guardian,
	Lightning,
	FireWall,
	MagmaBallExplosion,
	TownPortal,
	FlashBottom,
	FlashTop,
	ManaShield,
	BloodHit,
	BoneHit,
	MetalHit,
	FireArrow,
	DoomSerpents,
	Golem,
	Spurt,
	ApocalypseBoom,
	StoneCurseShatter,
	BigExplosion,
	Inferno,
	ThinLightning,
	BloodStar,
	BloodStarExplosion,
	MagmaBall,
	Krull,
	ChargedBolt,
	HolyBolt,
	HolyBoltExplosion,
	LightningArrow,
	FireArrowExplosion,
	Acid,
	AcidSplat,
	AcidPuddle,
	Etherealize,
	Elemental,
	Resurrect,
	BoneSpirit,
	RedPortal,
	DiabloApocalypseBoom,
	BloodStarBlue,
	BloodStarBlueExplosion,
	BloodStarYellow,
	BloodStarYellowExplosion,
	BloodStarRed,
	BloodStarRedExplosion,
	HorkSpawn,
	Reflect,
	OrangeFlare,
	BlueFlare,
	RedFlare,
	YellowFlare,
	Rune,
	YellowFlareExplosion,
	BlueFlareExplosion,
	RedFlareExplosion,
	BlueFlare2,
	OrangeFlareExplosion,
	BlueFlareExplosion2,
	None,
};

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
	void (*mAddProc)(Missile &, AddMissileParameter &);
	void (*mProc)(Missile &);
	bool mDraw;
	uint8_t mType;
	DamageType damageType;
	MissileGraphicID mFileNum;
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
	uint8_t animFAmt;
	MissileDataFlags flags;
	std::array<uint8_t, 16> animDelay = {};
	std::array<uint8_t, 16> animLen = {};
	uint16_t animWidth;
	int16_t animWidth2;
	OptionalOwnedClxSpriteListOrSheet sprites;

	MissileFileData(string_view name, uint8_t animFAmt, MissileDataFlags flags,
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

inline MissileData &GetMissileData(MissileID missileId)
{
	return MissilesData[static_cast<std::underlying_type<MissileID>::type>(missileId)];
}

extern MissileFileData MissileSpriteData[];

inline MissileFileData &GetMissileSpriteData(MissileGraphicID graphicId)
{
	return MissileSpriteData[static_cast<std::underlying_type<MissileGraphicID>::type>(graphicId)];
}

void InitMissileGFX(bool loadHellfireGraphics = false);
void FreeMissileGFX();

} // namespace devilution
