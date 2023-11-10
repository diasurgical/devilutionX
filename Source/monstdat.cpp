/**
 * @file monstdat.cpp
 *
 * Implementation of all monster data.
 */
#include "monstdat.h"

#include <cstdint>
#include <unordered_map>

#include <expected.hpp>

#include "data/file.hpp"
#include "items.h"
#include "monster.h"
#include "textdat.h"
#include "utils/language.h"

namespace devilution {

namespace {

// Returns a `treasure` value for the given item.
constexpr uint16_t Uniq(_unique_items item)
{
	return static_cast<uint16_t>(T_UNIQ) + item;
}

std::vector<std::string> MonsterSpritePaths;

} // namespace

const char *MonsterData::spritePath() const
{
	return MonsterSpritePaths[static_cast<size_t>(spriteId)].c_str();
}

/** Contains the data related to each monster ID. */
std::vector<MonsterData> MonstersData;

/** Contains the data related to each unique monster ID. */
std::vector<UniqueMonsterData> UniqueMonstersData;

/**
 * Map between .DUN file value and monster type enum
 */
const _monster_id MonstConvTbl[] = {
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
	MT_SNEAK,
	MT_STALKER,
	MT_UNSEEN,
	MT_ILLWEAV,
	MT_NGOATMC,
	MT_BGOATMC,
	MT_RGOATMC,
	MT_GGOATMC,
	MT_FIEND,
	MT_GLOOM,
	MT_BLINK,
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
	MT_FAT,
	MT_MUDMAN,
	MT_TOAD,
	MT_FLAYED,
	MT_WYRM,
	MT_CAVSLUG,
	MT_DEVOUR,
	MT_DVLWYRM,
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
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_INCIN,
	MT_FLAMLRD,
	MT_DOOMFIRE,
	MT_HELLBURN,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_RSTORM,
	MT_STORM,
	MT_STORML,
	MT_MAEL,
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
	MT_GSNAKE,
	MT_BSNAKE,
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
	MT_INVALID,
	MT_DIABLO,
	MT_INVALID,
	MT_GOLEM,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID, // Monster from blood1.dun and blood2.dun
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID, // Snotspill from banner2.dun
	MT_INVALID,
	MT_INVALID,
	MT_BIGFALL,
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
	MT_LICH,
	MT_BICLOPS,
	MT_FLESTHNG,
	MT_REAPER,
	MT_NAKRUL,
	MT_CLEAVER,
	MT_INVILORD,
	MT_LRDSAYTR,
};

namespace {

tl::expected<_monster_id, std::string> ParseMonsterId(std::string_view value)
{
	if (value == "MT_NZOMBIE") return MT_NZOMBIE;
	if (value == "MT_BZOMBIE") return MT_BZOMBIE;
	if (value == "MT_GZOMBIE") return MT_GZOMBIE;
	if (value == "MT_YZOMBIE") return MT_YZOMBIE;
	if (value == "MT_RFALLSP") return MT_RFALLSP;
	if (value == "MT_DFALLSP") return MT_DFALLSP;
	if (value == "MT_YFALLSP") return MT_YFALLSP;
	if (value == "MT_BFALLSP") return MT_BFALLSP;
	if (value == "MT_WSKELAX") return MT_WSKELAX;
	if (value == "MT_TSKELAX") return MT_TSKELAX;
	if (value == "MT_RSKELAX") return MT_RSKELAX;
	if (value == "MT_XSKELAX") return MT_XSKELAX;
	if (value == "MT_RFALLSD") return MT_RFALLSD;
	if (value == "MT_DFALLSD") return MT_DFALLSD;
	if (value == "MT_YFALLSD") return MT_YFALLSD;
	if (value == "MT_BFALLSD") return MT_BFALLSD;
	if (value == "MT_NSCAV") return MT_NSCAV;
	if (value == "MT_BSCAV") return MT_BSCAV;
	if (value == "MT_WSCAV") return MT_WSCAV;
	if (value == "MT_YSCAV") return MT_YSCAV;
	if (value == "MT_WSKELBW") return MT_WSKELBW;
	if (value == "MT_TSKELBW") return MT_TSKELBW;
	if (value == "MT_RSKELBW") return MT_RSKELBW;
	if (value == "MT_XSKELBW") return MT_XSKELBW;
	if (value == "MT_WSKELSD") return MT_WSKELSD;
	if (value == "MT_TSKELSD") return MT_TSKELSD;
	if (value == "MT_RSKELSD") return MT_RSKELSD;
	if (value == "MT_XSKELSD") return MT_XSKELSD;
	if (value == "MT_SNEAK") return MT_SNEAK;
	if (value == "MT_STALKER") return MT_STALKER;
	if (value == "MT_UNSEEN") return MT_UNSEEN;
	if (value == "MT_ILLWEAV") return MT_ILLWEAV;
	if (value == "MT_NGOATMC") return MT_NGOATMC;
	if (value == "MT_BGOATMC") return MT_BGOATMC;
	if (value == "MT_RGOATMC") return MT_RGOATMC;
	if (value == "MT_GGOATMC") return MT_GGOATMC;
	if (value == "MT_FIEND") return MT_FIEND;
	if (value == "MT_GLOOM") return MT_GLOOM;
	if (value == "MT_BLINK") return MT_BLINK;
	if (value == "MT_FAMILIAR") return MT_FAMILIAR;
	if (value == "MT_NGOATBW") return MT_NGOATBW;
	if (value == "MT_BGOATBW") return MT_BGOATBW;
	if (value == "MT_RGOATBW") return MT_RGOATBW;
	if (value == "MT_GGOATBW") return MT_GGOATBW;
	if (value == "MT_NACID") return MT_NACID;
	if (value == "MT_RACID") return MT_RACID;
	if (value == "MT_BACID") return MT_BACID;
	if (value == "MT_XACID") return MT_XACID;
	if (value == "MT_SKING") return MT_SKING;
	if (value == "MT_FAT") return MT_FAT;
	if (value == "MT_MUDMAN") return MT_MUDMAN;
	if (value == "MT_TOAD") return MT_TOAD;
	if (value == "MT_FLAYED") return MT_FLAYED;
	if (value == "MT_WYRM") return MT_WYRM;
	if (value == "MT_CAVSLUG") return MT_CAVSLUG;
	if (value == "MT_DEVOUR") return MT_DEVOUR;
	if (value == "MT_DVLWYRM") return MT_DVLWYRM;
	if (value == "MT_NMAGMA") return MT_NMAGMA;
	if (value == "MT_YMAGMA") return MT_YMAGMA;
	if (value == "MT_BMAGMA") return MT_BMAGMA;
	if (value == "MT_WMAGMA") return MT_WMAGMA;
	if (value == "MT_HORNED") return MT_HORNED;
	if (value == "MT_MUDRUN") return MT_MUDRUN;
	if (value == "MT_FROSTC") return MT_FROSTC;
	if (value == "MT_OBLORD") return MT_OBLORD;
	if (value == "MT_BONEDMN") return MT_BONEDMN;
	if (value == "MT_REDDTH") return MT_REDDTH;
	if (value == "MT_LTCHDMN") return MT_LTCHDMN;
	if (value == "MT_UDEDBLRG") return MT_UDEDBLRG;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INCIN") return MT_INCIN;
	if (value == "MT_FLAMLRD") return MT_FLAMLRD;
	if (value == "MT_DOOMFIRE") return MT_DOOMFIRE;
	if (value == "MT_HELLBURN") return MT_HELLBURN;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_RSTORM") return MT_RSTORM;
	if (value == "MT_STORM") return MT_STORM;
	if (value == "MT_STORML") return MT_STORML;
	if (value == "MT_MAEL") return MT_MAEL;
	if (value == "MT_WINGED") return MT_WINGED;
	if (value == "MT_GARGOYLE") return MT_GARGOYLE;
	if (value == "MT_BLOODCLW") return MT_BLOODCLW;
	if (value == "MT_DEATHW") return MT_DEATHW;
	if (value == "MT_MEGA") return MT_MEGA;
	if (value == "MT_GUARD") return MT_GUARD;
	if (value == "MT_VTEXLRD") return MT_VTEXLRD;
	if (value == "MT_BALROG") return MT_BALROG;
	if (value == "MT_NSNAKE") return MT_NSNAKE;
	if (value == "MT_RSNAKE") return MT_RSNAKE;
	if (value == "MT_GSNAKE") return MT_GSNAKE;
	if (value == "MT_BSNAKE") return MT_BSNAKE;
	if (value == "MT_NBLACK") return MT_NBLACK;
	if (value == "MT_RTBLACK") return MT_RTBLACK;
	if (value == "MT_BTBLACK") return MT_BTBLACK;
	if (value == "MT_RBLACK") return MT_RBLACK;
	if (value == "MT_UNRAV") return MT_UNRAV;
	if (value == "MT_HOLOWONE") return MT_HOLOWONE;
	if (value == "MT_PAINMSTR") return MT_PAINMSTR;
	if (value == "MT_REALWEAV") return MT_REALWEAV;
	if (value == "MT_SUCCUBUS") return MT_SUCCUBUS;
	if (value == "MT_SNOWWICH") return MT_SNOWWICH;
	if (value == "MT_HLSPWN") return MT_HLSPWN;
	if (value == "MT_SOLBRNR") return MT_SOLBRNR;
	if (value == "MT_COUNSLR") return MT_COUNSLR;
	if (value == "MT_MAGISTR") return MT_MAGISTR;
	if (value == "MT_CABALIST") return MT_CABALIST;
	if (value == "MT_ADVOCATE") return MT_ADVOCATE;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_DIABLO") return MT_DIABLO;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_GOLEM") return MT_GOLEM;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_INVALID") return MT_INVALID;
	if (value == "MT_BIGFALL") return MT_BIGFALL;
	if (value == "MT_DARKMAGE") return MT_DARKMAGE;
	if (value == "MT_HELLBOAR") return MT_HELLBOAR;
	if (value == "MT_STINGER") return MT_STINGER;
	if (value == "MT_PSYCHORB") return MT_PSYCHORB;
	if (value == "MT_ARACHNON") return MT_ARACHNON;
	if (value == "MT_FELLTWIN") return MT_FELLTWIN;
	if (value == "MT_HORKSPWN") return MT_HORKSPWN;
	if (value == "MT_VENMTAIL") return MT_VENMTAIL;
	if (value == "MT_NECRMORB") return MT_NECRMORB;
	if (value == "MT_SPIDLORD") return MT_SPIDLORD;
	if (value == "MT_LASHWORM") return MT_LASHWORM;
	if (value == "MT_TORCHANT") return MT_TORCHANT;
	if (value == "MT_HORKDMN") return MT_HORKDMN;
	if (value == "MT_DEFILER") return MT_DEFILER;
	if (value == "MT_GRAVEDIG") return MT_GRAVEDIG;
	if (value == "MT_TOMBRAT") return MT_TOMBRAT;
	if (value == "MT_FIREBAT") return MT_FIREBAT;
	if (value == "MT_SKLWING") return MT_SKLWING;
	if (value == "MT_LICH") return MT_LICH;
	if (value == "MT_CRYPTDMN") return MT_CRYPTDMN;
	if (value == "MT_HELLBAT") return MT_HELLBAT;
	if (value == "MT_BONEDEMN") return MT_BONEDEMN;
	if (value == "MT_LICH") return MT_LICH;
	if (value == "MT_BICLOPS") return MT_BICLOPS;
	if (value == "MT_FLESTHNG") return MT_FLESTHNG;
	if (value == "MT_REAPER") return MT_REAPER;
	if (value == "MT_NAKRUL") return MT_NAKRUL;
	if (value == "MT_CLEAVER") return MT_CLEAVER;
	if (value == "MT_INVILORD") return MT_INVILORD;
	if (value == "MT_LRDSAYTR") return MT_LRDSAYTR;
	return tl::make_unexpected("Unknown enum value");
}

tl::expected<MonsterAvailability, std::string> ParseMonsterAvailability(std::string_view value)
{
	if (value == "Always") return MonsterAvailability::Always;
	if (value == "Never") return MonsterAvailability::Never;
	if (value == "Retail") return MonsterAvailability::Retail;
	return tl::make_unexpected("Expected one of: Always, Never, or Retail");
}

tl::expected<MonsterAIID, std::string> ParseAiId(std::string_view value)
{
	if (value == "Zombie") return MonsterAIID::Zombie;
	if (value == "Fat") return MonsterAIID::Fat;
	if (value == "SkeletonMelee") return MonsterAIID::SkeletonMelee;
	if (value == "SkeletonRanged") return MonsterAIID::SkeletonRanged;
	if (value == "Scavenger") return MonsterAIID::Scavenger;
	if (value == "Rhino") return MonsterAIID::Rhino;
	if (value == "GoatMelee") return MonsterAIID::GoatMelee;
	if (value == "GoatRanged") return MonsterAIID::GoatRanged;
	if (value == "Fallen") return MonsterAIID::Fallen;
	if (value == "Magma") return MonsterAIID::Magma;
	if (value == "SkeletonKing") return MonsterAIID::SkeletonKing;
	if (value == "Bat") return MonsterAIID::Bat;
	if (value == "Gargoyle") return MonsterAIID::Gargoyle;
	if (value == "Butcher") return MonsterAIID::Butcher;
	if (value == "Succubus") return MonsterAIID::Succubus;
	if (value == "Sneak") return MonsterAIID::Sneak;
	if (value == "Storm") return MonsterAIID::Storm;
	if (value == "FireMan") return MonsterAIID::FireMan;
	if (value == "Gharbad") return MonsterAIID::Gharbad;
	if (value == "Acid") return MonsterAIID::Acid;
	if (value == "AcidUnique") return MonsterAIID::AcidUnique;
	if (value == "Golem") return MonsterAIID::Golem;
	if (value == "Zhar") return MonsterAIID::Zhar;
	if (value == "Snotspill") return MonsterAIID::Snotspill;
	if (value == "Snake") return MonsterAIID::Snake;
	if (value == "Counselor") return MonsterAIID::Counselor;
	if (value == "Mega") return MonsterAIID::Mega;
	if (value == "Diablo") return MonsterAIID::Diablo;
	if (value == "Lazarus") return MonsterAIID::Lazarus;
	if (value == "LazarusSuccubus") return MonsterAIID::LazarusSuccubus;
	if (value == "Lachdanan") return MonsterAIID::Lachdanan;
	if (value == "Warlord") return MonsterAIID::Warlord;
	if (value == "FireBat") return MonsterAIID::FireBat;
	if (value == "Torchant") return MonsterAIID::Torchant;
	if (value == "HorkDemon") return MonsterAIID::HorkDemon;
	if (value == "Lich") return MonsterAIID::Lich;
	if (value == "ArchLich") return MonsterAIID::ArchLich;
	if (value == "Psychorb") return MonsterAIID::Psychorb;
	if (value == "Necromorb") return MonsterAIID::Necromorb;
	if (value == "BoneDemon") return MonsterAIID::BoneDemon;
	return tl::make_unexpected("Unknown enum value");
}

tl::expected<monster_flag, std::string> ParseMonsterFlag(std::string_view value)
{
	if (value == "HIDDEN") return MFLAG_HIDDEN;
	if (value == "LOCK_ANIMATION") return MFLAG_LOCK_ANIMATION;
	if (value == "ALLOW_SPECIAL") return MFLAG_ALLOW_SPECIAL;
	if (value == "TARGETS_MONSTER") return MFLAG_TARGETS_MONSTER;
	if (value == "GOLEM") return MFLAG_GOLEM;
	if (value == "QUEST_COMPLETE") return MFLAG_QUEST_COMPLETE;
	if (value == "KNOCKBACK") return MFLAG_KNOCKBACK;
	if (value == "SEARCH") return MFLAG_SEARCH;
	if (value == "CAN_OPEN_DOOR") return MFLAG_CAN_OPEN_DOOR;
	if (value == "NO_ENEMY") return MFLAG_NO_ENEMY;
	if (value == "BERSERK") return MFLAG_BERSERK;
	if (value == "NOLIFESTEAL") return MFLAG_NOLIFESTEAL;
	return tl::make_unexpected("Unknown enum value");
}

tl::expected<MonsterClass, std::string> ParseMonsterClass(std::string_view value)
{
	if (value == "Undead") return MonsterClass::Undead;
	if (value == "Demon") return MonsterClass::Demon;
	if (value == "Animal") return MonsterClass::Animal;
	return tl::make_unexpected("Unknown enum value");
}

tl::expected<monster_resistance, std::string> ParseMonsterResistance(std::string_view value)
{
	if (value == "RESIST_MAGIC") return RESIST_MAGIC;
	if (value == "RESIST_FIRE") return RESIST_FIRE;
	if (value == "RESIST_LIGHTNING") return RESIST_LIGHTNING;
	if (value == "IMMUNE_MAGIC") return IMMUNE_MAGIC;
	if (value == "IMMUNE_FIRE") return IMMUNE_FIRE;
	if (value == "IMMUNE_LIGHTNING") return IMMUNE_LIGHTNING;
	if (value == "IMMUNE_ACID") return IMMUNE_ACID;
	return tl::make_unexpected("Unknown enum value");
}

tl::expected<UniqueMonsterPack, std::string> ParseUniqueMonsterPack(std::string_view value)
{
	if (value == "None") return UniqueMonsterPack::None;
	if (value == "Independent") return UniqueMonsterPack::Independent;
	if (value == "Leashed") return UniqueMonsterPack::Leashed;
	return tl::make_unexpected("Unknown enum value");
}

void LoadMonstDat()
{
	const std::string_view filename = "txtdata\\monsters\\monstdat.tsv";
	tl::expected<DataFile, DataFile::Error> dataFileResult = DataFile::load(filename);
	if (!dataFileResult.has_value()) {
		DataFile::reportFatalError(dataFileResult.error(), filename);
	}

	DataFile &dataFile = dataFileResult.value();
	if (tl::expected<void, DataFile::Error> result = dataFile.skipHeader();
	    !result.has_value()) {
		DataFile::reportFatalError(result.error(), filename);
	}

	MonstersData.clear();
	MonstersData.reserve(dataFile.numRecords());
	std::unordered_map<std::string, size_t> spritePathToId;
	for (DataFileRecord record : dataFile) {
		FieldIterator fieldIt = record.begin();
		const FieldIterator endField = record.end();

		MonstersData.emplace_back();
		MonsterData &monster = MonstersData.back();

		const auto advance = [&]() {
			++fieldIt;
			if (fieldIt == endField) {
				DataFile::reportFatalError(DataFile::Error::NotEnoughColumns, filename);
			}
		};

		// Skip the first column (monster ID).

		// name
		advance();
		monster.name = (*fieldIt).value();

		// assetsSuffix
		advance();
		{
			std::string assetsSuffix { (*fieldIt).value() };
			const auto [it, inserted] = spritePathToId.emplace(assetsSuffix, spritePathToId.size());
			if (inserted)
				MonsterSpritePaths.push_back(it->first);
			monster.spriteId = it->second;
		}

		// soundSuffix
		advance();
		monster.soundSuffix = (*fieldIt).value();

		// trnFile
		advance();
		monster.trnFile = (*fieldIt).value();

		// availability
		advance();
		if (tl::expected<MonsterAvailability, std::string> result = ParseMonsterAvailability((*fieldIt).value()); result.has_value()) {
			monster.availability = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "availability", *fieldIt, result.error());
		}

		// width
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.width); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "width", *fieldIt);
		}

		// image
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.image); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "image", *fieldIt);
		}

		// hasSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseBool(monster.hasSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "hasSpecial", *fieldIt);
		}

		// hasSpecialSound
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseBool(monster.hasSpecialSound); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "hasSpecialSound", *fieldIt);
		}

		// frames[6]
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseIntArray(monster.frames); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "frames", *fieldIt);
		}

		// rate[6]
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseIntArray(monster.rate); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "rate", *fieldIt);
		}

		// minDunLvl
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.minDunLvl); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "minDunLvl", *fieldIt);
		}

		// maxDunLvl
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.maxDunLvl); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "maxDunLvl", *fieldIt);
		}

		// level
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.level); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "level", *fieldIt);
		}

		// hitPointsMinimum
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.hitPointsMinimum); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "hitPointsMinimum", *fieldIt);
		}

		// hitPointsMaximum
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.hitPointsMaximum); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "hitPointsMaximum", *fieldIt);
		}

		// ai
		advance();
		if (tl::expected<MonsterAIID, std::string> result = ParseAiId((*fieldIt).value()); result.has_value()) {
			monster.ai = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "ai", *fieldIt, result.error());
		}

		// abilityFlags
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseEnumList(monster.abilityFlags, ParseMonsterFlag); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "abilityFlags", *fieldIt);
		}

		// intelligence
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.intelligence); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "intelligence", *fieldIt);
		}

		// toHit
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.toHit); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "toHit", *fieldIt);
		}

		// animFrameNum
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.animFrameNum); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "animFrameNum", *fieldIt);
		}

		// minDamage
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.minDamage); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "minDamage", *fieldIt);
		}

		// maxDamage
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.maxDamage); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "maxDamage", *fieldIt);
		}

		// toHitSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.toHitSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "toHitSpecial", *fieldIt);
		}

		// animFrameNumSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.animFrameNumSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "animFrameNumSpecial", *fieldIt);
		}

		// minDamageSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.minDamageSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "minDamageSpecial", *fieldIt);
		}

		// maxDamageSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.maxDamageSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "maxDamageSpecial", *fieldIt);
		}

		// armorClass
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.armorClass); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "armorClass", *fieldIt);
		}

		// monsterClass
		advance();
		if (tl::expected<MonsterClass, std::string> result = ParseMonsterClass((*fieldIt).value()); result.has_value()) {
			monster.monsterClass = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "monsterClass", *fieldIt, result.error());
		}

		// resistance
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseEnumList(monster.resistance, ParseMonsterResistance); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "resistance", *fieldIt);
		}

		// resistanceHell
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseEnumList(monster.resistanceHell, ParseMonsterResistance); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "resistanceHell", *fieldIt);
		}

		// selectionType
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.selectionType); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "selectionType", *fieldIt);
		}

		// treasure
		// TODO: Replace this hack with proper parsing once items have been migrated to data files.
		advance();
		{
			const std::string_view value = (*fieldIt).value();
			if (value.empty()) {
				monster.treasure = 0;
			} else if (value == "None") {
				monster.treasure = T_NODROP;
			} else if (value == "Uniq(SKCROWN)") {
				monster.treasure = Uniq(UITEM_SKCROWN);
			} else if (value == "Uniq(CLEAVER)") {
				monster.treasure = Uniq(UITEM_CLEAVER);
			} else {
				DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "treasure", *fieldIt, "NOTE: Parser is incomplete");
			}
		}

		// exp
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.exp); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "exp", *fieldIt);
		}
	}

	MonstersData.shrink_to_fit();
}

void LoadUniqueMonstDat()
{
	const std::string_view filename = "txtdata\\monsters\\unique_monstdat.tsv";
	tl::expected<DataFile, DataFile::Error> dataFileResult = DataFile::load(filename);
	if (!dataFileResult.has_value()) {
		DataFile::reportFatalError(dataFileResult.error(), filename);
	}

	DataFile &dataFile = dataFileResult.value();
	if (tl::expected<void, DataFile::Error> result = dataFile.skipHeader();
	    !result.has_value()) {
		DataFile::reportFatalError(result.error(), filename);
	}

	UniqueMonstersData.clear();
	UniqueMonstersData.reserve(dataFile.numRecords());
	for (DataFileRecord record : dataFile) {
		FieldIterator fieldIt = record.begin();
		const FieldIterator endField = record.end();

		UniqueMonstersData.emplace_back();
		UniqueMonsterData &monster = UniqueMonstersData.back();

		const auto advance = [&]() {
			++fieldIt;
			if (fieldIt == endField) {
				DataFile::reportFatalError(DataFile::Error::NotEnoughColumns, filename);
			}
		};

		// type
		if (tl::expected<_monster_id, std::string> result = ParseMonsterId((*fieldIt).value()); result.has_value()) {
			monster.mtype = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "type", *fieldIt, result.error());
		}

		// name
		advance();
		monster.mName = (*fieldIt).value();

		// trn
		advance();
		monster.mTrnName = (*fieldIt).value();

		// level
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.mlevel); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "level", *fieldIt);
		}

		// maxHp
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.mmaxhp); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "maxHp", *fieldIt);
		}

		// ai
		advance();
		if (tl::expected<MonsterAIID, std::string> result = ParseAiId((*fieldIt).value()); result.has_value()) {
			monster.mAi = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "ai", *fieldIt, result.error());
		}

		// intelligence
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.mint); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "intelligence", *fieldIt);
		}

		// minDamage
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.mMinDamage); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "minDamage", *fieldIt);
		}

		// maxDamage
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.mMaxDamage); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "maxDamage", *fieldIt);
		}

		// resistance
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseEnumList(monster.mMagicRes, ParseMonsterResistance); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "resistance", *fieldIt);
		}

		// monsterPack
		advance();
		if (tl::expected<UniqueMonsterPack, std::string> result = ParseUniqueMonsterPack((*fieldIt).value()); result.has_value()) {
			monster.monsterPack = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "monsterPack", *fieldIt, result.error());
		}

		// customToHit
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.customToHit); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "customToHit", *fieldIt);
		}

		// customArmorClass
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.customArmorClass); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "customArmorClass", *fieldIt);
		}

		// talkMessage
		// TODO: Replace this hack with proper parsing once messages have been migrated to data files.
		advance();
		{
			const std::string_view value = (*fieldIt).value();
			if (value.empty()) {
				monster.mtalkmsg = TEXT_NONE;
			} else if (value == "TEXT_GARBUD1") {
				monster.mtalkmsg = TEXT_GARBUD1;
			} else if (value == "TEXT_ZHAR1") {
				monster.mtalkmsg = TEXT_ZHAR1;
			} else if (value == "TEXT_BANNER10") {
				monster.mtalkmsg = TEXT_BANNER10;
			} else if (value == "TEXT_VILE13") {
				monster.mtalkmsg = TEXT_VILE13;
			} else if (value == "TEXT_VEIL9") {
				monster.mtalkmsg = TEXT_VEIL9;
			} else if (value == "TEXT_WARLRD9") {
				monster.mtalkmsg = TEXT_WARLRD9;
			} else {
				DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "talkMessage", *fieldIt, "NOTE: Parser is incomplete");
			}
		}
	}

	UniqueMonstersData.shrink_to_fit();
}

} // namespace

void LoadMonsterData()
{
	LoadMonstDat();
	LoadUniqueMonstDat();
}

size_t GetNumMonsterSprites()
{
	return MonsterSpritePaths.size();
}

} // namespace devilution
