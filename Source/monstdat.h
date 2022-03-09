/**
 * @file monstdat.h
 *
 * Interface of all monster data.
 */
#pragma once

#include <cstdint>

#include "textdat.h"

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
	AI_LAZARUS,
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

enum class MonsterClass : uint8_t {
	Undead,
	Demon,
	Animal,
};

struct MonsterResists {
	// NOTE: unless we remove backwards compatability, or maybe use up some extra memory locations,
	// magic, fire, and lightning resistances should be less than 128, otherwise they will not save properly.
	uint8_t Magic = 0;
	uint8_t Fire = 0;
	uint8_t Lightning = 0;
	uint8_t Acid = 0;

	uint8_t getMagicResist()
	{
		return this->Magic;
	}
	uint8_t getFireResist()
	{
		return this->Fire;
	}
	uint8_t getLightningResist()
	{
		return this->Lightning;
	}
	uint8_t getAcidResist()
	{
		return this->Acid;
	}

	/**
	 * @brief Returns if monster has any resistances (or immunities) to Magic, Fire, or Lightning.
	 * @return True if any of the monster's non-acid resistances are greater than 0.
	 */
	bool hasResistancesOrImmunities() const
	{
		return this->Magic > 0 || this->Fire > 0 || this->Lightning > 0;
	}
	/**
	 * @brief Returns if monster has any resistances to Magic, Fire, or Lightning. Must have at least one greater than 0 but less than 100.
	 * @return True if any of the monster's non-acid resistances are greater than 0 but less than 100.
	 */
	bool hasResistances() const
	{
		return (this->Magic > 0 && this->Magic < 100) || (this->Fire > 0 && this->Fire < 100) || (this->Lightning > 0 && this->Lightning < 100);
	}

	/**
	 * @brief Returns if monster has any immunities to Magic, Fire, or Lightning.
	 * @return True if monster has non-acid immunities.
	 */
	bool hasImmunities() const
	{
		return this->Magic >= 100 || this->Fire >= 100 || this->Lightning >= 100;
	}

	/**
	 * @brief Returns if monster has resistance (not immunity) to Magic.
	 * @return True if monster has resistance greater than 0 but less than 100 to Magic.
	 */
	bool isMagicResistant() const
	{
		return this->Magic > 0 && this->Magic < 100;
	}

	/**
	 * @brief Returns if monster has immunity to Magic.
	 * @return True if monster has resistance greateer than or equal to 100 to Magic.
	 */
	bool isMagicImmune() const
	{
		return this->Magic >= 100;
	}

	/**
	 * @brief Returns if monster has resistance (not immunity) to Fire.
	 * @return True if monster has resistance greater than 0 but less than 100 to Fire.
	 */
	bool isFireResistant() const
	{
		return this->Fire > 0 && this->Fire < 100;
	}

	/**
	 * @brief Returns if monster has immunity to Fire.
	 * @return True if monster has resistance greateer than or equal to 100 to Fire.
	 */
	bool isFireImmune() const
	{
		return this->Fire >= 100;
	}

	/**
	 * @brief Returns if monster has resistance (not immunity) to Lightning.
	 * @return True if monster has resistance greater than 0 but less than 100 to Lightning.
	 */
	bool isLightningResistant() const
	{
		return this->Lightning > 0 && this->Lightning < 100;
	}

	/**
	 * @brief Returns if monster has immunity to Lightning.
	 * @return True if monster has resistance greateer than or equal to 100 to Lightning.
	 */
	bool isLightningImmune() const
	{
		return this->Lightning >= 100;
	}

	/**
	 * @brief Returns if monster has resistance (not immunity) to Acid.
	 * @return True if monster has resistance greater than 0 but less than 100 to Acid.
	 */
	bool isAcidResistant() const
	{
		return this->Acid > 0 && this->Acid < 100;
	}

	/**
	 * @brief Returns if monster has immunity to Acid.
	 * @return True if monster has resistance greateer than or equal to 100 to Acid.
	 */
	bool isAcidImmune() const
	{
		return this->Acid >= 100;
	}

	/**
	 * @brief Sets all monster resistances to 0.
	 */
	void setToZero()
	{
		this->Magic = 0;
		this->Fire = 0;
		this->Lightning = 0;
		this->Acid = 0;
	}

	/**
	 * @brief Converts the monster resists to save info.
	 * @return 32 bit int for saving purposes.
	 */
	uint32_t getSaveData()
	{
		/**
		 * Save Structure (low to high):
		 * 8 bits - original Diablo save data.
		 * 3 bits - acid data.
		 * 7 biits - lightning data.
		 * 7 bits - fire data.
		 * 7 bits - magic data.
		 */
		uint32_t saveData = 0;
		// set lower 8 to original resistance values, for compatability
		if (this->isMagicResistant())
			saveData |= 1 << 0;
		else if (this->isMagicImmune())
			saveData |= 1 << 3;
		if (this->isFireResistant())
			saveData |= 1 << 1;
		else if (this->isFireImmune())
			saveData |= 1 << 4;
		if (this->isLightningResistant())
			saveData |= 1 << 2;
		else if (this->isLightningImmune())
			saveData |= 1 << 5;
		if (this->isAcidImmune() || this->isAcidResistant())
			saveData |= 1 << 7;

		uint32_t temp = 0;
		uint8_t mask = 0b01111111;
		if (this->isMagicResistant() || this->isMagicImmune()) {
			if (this->Magic < 128)
				temp |= (this->Magic & mask);
			else
				temp |= 0b01111111;
		}
		temp <<= 7;
		if (this->isFireResistant() || this->isFireImmune()) {
			if (this->Fire < 128)
				temp |= (this->Fire * mask);
			else
				temp |= 0b01111111;
		}
		temp <<= 7;
		if (this->isLightningResistant() || this->isLightningImmune()) {
			if (this->Lightning < 128)
				temp |= (this->Lightning & mask);
			else
				temp |= 0b01111111;
		}
		temp <<= 3;
		int8_t acidTemp = 0;
		if (this->isAcidResistant() || this->isAcidImmune()) {
			if (this->isAcidImmune())
				acidTemp = 0b111;
			else {
				acidTemp = this->Acid;
				if (acidTemp > 0 && acidTemp < 16)
					acidTemp = 0b001;
				else if (acidTemp >= 16 && acidTemp < 32)
					acidTemp = 0b010;
				else if (acidTemp >= 32 && acidTemp < 48)
					acidTemp = 0b011;
				else if (acidTemp >= 48 && acidTemp < 64)
					acidTemp = 0b100;
				else if (acidTemp >= 64 && acidTemp < 80)
					acidTemp = 0b101;
				else /* >80 and < 100 */
					acidTemp = 0b110;
			}
			temp |= acidTemp;
		}
		temp <<= 8;
		saveData |= temp;

		return saveData;
	}

	/**
	 * @brief Unpacks save data based on expanded save data from getSaveData()
	 * @param 32 bit int from save file saving purposes
	 */
	void unpackSaveData(uint32_t saveData)
	{
		uint32_t temp = saveData;
		uint8_t originalSave = temp & 0b11111111;
		temp >>= 8;
		uint8_t _acid = temp & 0b111;
		temp >>= 3;
		uint8_t _lightning = temp & 0b1111111;
		temp >>= 7;
		uint8_t _fire = temp & 0b1111111;
		temp >>= 7;
		uint8_t _magic = temp & 0b1111111;

		constexpr uint8_t ResistMagic = 1 << 0;
		constexpr uint8_t ResistFire = 1 << 1;
		constexpr uint8_t ResistLightning = 1 << 2;
		constexpr uint8_t ImmuneMagic = 1 << 3;
		constexpr uint8_t ImmuneFire = 1 << 4;
		constexpr uint8_t ImmuneLightning = 1 << 5;
		constexpr uint8_t ImmuneAcid = 1 << 7;

		// check if loaded data contained info that matches original save, if not, use original save values

		bool hasResistMagic = (originalSave & ResistMagic) != 0;
		bool hasImmuneMagic = (originalSave & ImmuneMagic) != 0;
		bool hasResistFire = (originalSave & ResistFire) != 0;
		bool hasImmuneFire = (originalSave & ImmuneFire) != 0;
		bool hasResistLightning = (originalSave & ResistLightning) != 0;
		bool hasImmuneLightning = (originalSave & ImmuneLightning) != 0;
		bool hasImmuneAcid = (originalSave & ImmuneAcid) != 0;

		if (hasResistMagic) {
			if (_magic > 0 && _magic < 100)
				this->Magic = _magic;
			else
				this->Magic = 75;
		} else if (hasImmuneMagic) {
			if (_magic >= 100)
				this->Magic = _magic;
			else
				this->Magic = 100;
		}

		if (hasResistFire) {
			if (_fire > 0 && _fire < 100)
				this->Fire = _fire;
			else
				this->Fire = 75;
		} else if (hasImmuneFire) {
			if (_fire >= 100)
				this->Fire = _fire;
			else
				this->Fire = 100;
		}

		if (hasResistLightning) {
			if (_lightning > 0 && _lightning < 100)
				this->Lightning = _lightning;
			else
				this->Lightning = 75;
		} else if (hasImmuneLightning) {
			if (_lightning >= 100)
				this->Lightning = _lightning;
			else
				this->Lightning = 100;
		}

		if (hasImmuneAcid) {
			if (_acid > 0 && _acid < 0b111)
				this->Acid = _acid * 16;
			else
				this->Acid = 100;
		}
	}
};

enum monster_treasure : uint16_t {
	// clang-format off
	T_MASK    = 0xFFF,
	T_NODROP = 0x4000, // monster doesn't drop any loot
	T_UNIQ    = 0x8000, // use combined with unique item's ID - for example butcher's cleaver = T_UNIQ+UITEM_CLEAVE
	// clang-format on
};

struct MonsterData {
	const char *mName;
	const char *GraphicType;
	const char *sndfile;
	const char *TransFile;
	uint16_t width;
	uint16_t mImage;
	bool has_special;
	bool snd_special;
	bool has_trans;
	uint8_t Frames[6];
	uint8_t Rate[6];
	int8_t mMinDLvl;
	int8_t mMaxDLvl;
	int8_t mLevel;
	uint16_t mMinHP;
	uint16_t mMaxHP;
	_mai_id mAi;
	/** Usign monster_flag as bitflags */
	uint16_t mFlags;
	uint8_t mInt;
	uint8_t mHit;
	uint8_t mAFNum;
	uint8_t mMinDamage;
	uint8_t mMaxDamage;
	uint8_t mHit2;
	uint8_t mAFNum2;
	uint8_t mMinDamage2;
	uint8_t mMaxDamage2;
	uint8_t mArmorClass;
	MonsterClass mMonstClass;
	MonsterResists mNormalResist;
	MonsterResists mNightmareResist;
	MonsterResists mHellResist;
	int8_t mSelFlag; // TODO Create enum
	/** Using monster_treasure */
	uint16_t mTreasure;
	uint16_t mExp;
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

/**
 * @brief Defines if and how a group of monsters should be spawned with the unique monster
 */
enum class UniqueMonsterPack {
	/**
	 * @brief Don't spawn a group of monsters with the unique monster
	 */
	None,
	/**
	 * @brief Spawn a group of monsters that are independent from the unique monster
	 */
	Independent,
	/**
	 * @brief Spawn a group of monsters that are leashed to the unique monster
	 */
	Leashed,
};

struct UniqueMonsterData {
	_monster_id mtype;
	const char *mName;
	const char *mTrnName;
	uint8_t mlevel;
	uint16_t mmaxhp;
	_mai_id mAi;
	uint8_t mint;
	uint8_t mMinDamage;
	uint8_t mMaxDamage;
	MonsterResists mMagicRes;
	/**
	 * @brief Defines if and how a group of monsters should be spawned with the unique monster
	 */
	UniqueMonsterPack monsterPack;
	uint8_t customToHit;
	uint8_t customArmorClass;
	_speech_id mtalkmsg;
};

extern const MonsterData MonstersData[];
extern const _monster_id MonstConvTbl[];
extern const char MonstAvailTbl[];
extern const UniqueMonsterData UniqueMonstersData[];

} // namespace devilution
