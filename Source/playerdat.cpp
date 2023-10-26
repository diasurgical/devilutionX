/**
 * @file playerdat.cpp
 *
 * Implementation of all player data.
 */

#include "playerdat.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <charconv>
#include <cstdint>
#include <vector>

#include <expected.hpp>
#include <fmt/format.h>

#include "data/file.hpp"
#include "items.h"
#include "player.h"
#include "textdat.h"
#include "utils/language.h"
#include "utils/static_vector.hpp"
#include "utils/str_cat.hpp"

namespace devilution {

namespace {

class ExperienceData {
	/** Specifies the experience point limit of each level. */
	std::vector<uint32_t> levelThresholds;

public:
	uint8_t getMaxLevel() const
	{
		return static_cast<uint8_t>(std::min<size_t>(levelThresholds.size(), std::numeric_limits<uint8_t>::max()));
	}

	DVL_REINITIALIZES void clear()
	{
		levelThresholds.clear();
	}

	[[nodiscard]] uint32_t getThresholdForLevel(unsigned level) const
	{
		if (level > 0)
			return levelThresholds[std::min<unsigned>(level - 1, getMaxLevel())];

		return 0;
	}

	void setThresholdForLevel(unsigned level, uint32_t experience)
	{
		if (level > 0) {
			if (level > levelThresholds.size()) {
				// To avoid ValidatePlayer() resetting players to 0 experience we need to use the maximum possible value here
				// As long as the file has no gaps it'll get initialised properly.
				levelThresholds.resize(level, std::numeric_limits<uint32_t>::max());
			}

			levelThresholds[static_cast<size_t>(level - 1)] = experience;
		}
	}
} ExperienceData;

enum class ExperienceColumn {
	Level,
	Experience,
	LAST = Experience
};

tl::expected<ExperienceColumn, ColumnDefinition::Error> mapExperienceColumnFromName(std::string_view name)
{
	if (name == "Level") {
		return ExperienceColumn::Level;
	}
	if (name == "Experience") {
		return ExperienceColumn::Experience;
	}
	return tl::unexpected { ColumnDefinition::Error::UnknownColumn };
}

void ReloadExperienceData()
{
	constexpr std::string_view filename = "txtdata\\Experience.tsv";
	auto dataFileResult = DataFile::load(filename);
	if (!dataFileResult.has_value()) {
		DataFile::reportFatalError(dataFileResult.error(), filename);
	}
	DataFile &dataFile = dataFileResult.value();

	constexpr unsigned ExpectedColumnCount = enum_size<ExperienceColumn>::value;

	std::array<ColumnDefinition, ExpectedColumnCount> columns;
	auto parseHeaderResult = dataFile.parseHeader<ExperienceColumn>(columns.data(), columns.data() + columns.size(), mapExperienceColumnFromName);

	if (!parseHeaderResult.has_value()) {
		DataFile::reportFatalError(parseHeaderResult.error(), filename);
	}

	ExperienceData.clear();
	for (DataFileRecord record : dataFile) {
		uint8_t level = 0;
		uint32_t experience = 0;
		bool skipRecord = false;

		FieldIterator fieldIt = record.begin();
		FieldIterator endField = record.end();
		for (auto &column : columns) {
			fieldIt += column.skipLength;

			if (fieldIt == endField) {
				DataFile::reportFatalError(DataFile::Error::NotEnoughColumns, filename);
			}

			DataFileField field = *fieldIt;

			switch (static_cast<ExperienceColumn>(column)) {
			case ExperienceColumn::Level: {
				auto parseIntResult = field.parseInt(level);

				if (!parseIntResult.has_value()) {
					if (*field == "MaxLevel") {
						skipRecord = true;
					} else {
						DataFile::reportFatalFieldError(parseIntResult.error(), filename, "Level", field);
					}
				}
			} break;

			case ExperienceColumn::Experience: {
				auto parseIntResult = field.parseInt(experience);

				if (!parseIntResult.has_value()) {
					DataFile::reportFatalFieldError(parseIntResult.error(), filename, "Experience", field);
				}
			} break;

			default:
				break;
			}

			if (skipRecord)
				break;

			++fieldIt;
		}

		if (!skipRecord)
			ExperienceData.setThresholdForLevel(level, experience);
	}
}

void LoadClassData(std::string_view classPath, ClassAttributes &attributes, PlayerCombatData &combat)
{
	const std::string filename = StrCat("txtdata\\classes\\", classPath, "\\attributes.tsv");
	tl::expected<DataFile, DataFile::Error> dataFileResult = DataFile::load(filename);
	if (!dataFileResult.has_value()) {
		DataFile::reportFatalError(dataFileResult.error(), filename);
	}
	DataFile &dataFile = dataFileResult.value();

	if (tl::expected<void, DataFile::Error> result = dataFile.skipHeader();
	    !result.has_value()) {
		DataFile::reportFatalError(result.error(), filename);
	}

	auto recordIt = dataFile.begin();
	const auto recordEnd = dataFile.end();

	const auto getValueField = [&](std::string_view expectedKey) {
		if (recordIt == recordEnd) {
			app_fatal(fmt::format("Missing field {} in {}", expectedKey, filename));
		}
		DataFileRecord record = *recordIt;
		FieldIterator fieldIt = record.begin();
		const FieldIterator endField = record.end();

		const std::string_view key = (*fieldIt).value();
		if (key != expectedKey) {
			app_fatal(fmt::format("Unexpected field in {}: got {}, expected {}", filename, key, expectedKey));
		}

		++fieldIt;
		if (fieldIt == endField) {
			DataFile::reportFatalError(DataFile::Error::NotEnoughColumns, filename);
		}
		return *fieldIt;
	};

	const auto valueReader = [&](auto &&readFn) {
		return [&](std::string_view expectedKey, auto &outValue) {
			DataFileField valueField = getValueField(expectedKey);
			if (const tl::expected<void, devilution::DataFileField::Error> result = readFn(valueField, outValue);
			    !result.has_value()) {
				DataFile::reportFatalFieldError(result.error(), filename, "Value", valueField);
			}
			++recordIt;
		};
	};

	const auto readInt = valueReader([](DataFileField &valueField, auto &outValue) {
		return valueField.parseInt(outValue);
	});
	const auto readDecimal = valueReader([](DataFileField &valueField, auto &outValue) {
		return valueField.parseFixed6(outValue);
	});

	readInt("baseStr", attributes.baseStr);
	readInt("baseMag", attributes.baseMag);
	readInt("baseDex", attributes.baseDex);
	readInt("baseVit", attributes.baseVit);
	readInt("maxStr", attributes.maxStr);
	readInt("maxMag", attributes.maxMag);
	readInt("maxDex", attributes.maxDex);
	readInt("maxVit", attributes.maxVit);
	readInt("blockBonus", combat.baseToBlock);
	readDecimal("adjLife", attributes.adjLife);
	readDecimal("adjMana", attributes.adjMana);
	readDecimal("lvlLife", attributes.lvlLife);
	readDecimal("lvlMana", attributes.lvlMana);
	readDecimal("chrLife", attributes.chrLife);
	readDecimal("chrMana", attributes.chrMana);
	readDecimal("itmLife", attributes.itmLife);
	readDecimal("itmMana", attributes.itmMana);
	readInt("baseMagicToHit", combat.baseMagicToHit);
	readInt("baseMeleeToHit", combat.baseMeleeToHit);
	readInt("baseRangedToHit", combat.baseRangedToHit);
}

std::vector<ClassAttributes> ClassAttributesPerClass;

std::vector<PlayerCombatData> PlayersCombatData;

void LoadClassesAttributes()
{
	const std::array classPaths { "warrior", "rogue", "sorcerer", "monk", "bard", "barbarian" };
	ClassAttributesPerClass.clear();
	ClassAttributesPerClass.reserve(classPaths.size());
	PlayersCombatData.clear();
	PlayersCombatData.reserve(classPaths.size());
	for (std::string_view path : classPaths) {
		LoadClassData(path, ClassAttributesPerClass.emplace_back(), PlayersCombatData.emplace_back());
	}
}

/** Contains the data related to each player class. */
const PlayerData PlayersData[] = {
	// clang-format off
// HeroClass                 className
// TRANSLATORS: Player Block start
/* HeroClass::Warrior */   { N_("Warrior"),   },
/* HeroClass::Rogue */     { N_("Rogue"),     },
/* HeroClass::Sorcerer */  { N_("Sorcerer"),  },
/* HeroClass::Monk */      { N_("Monk"),      },
/* HeroClass::Bard */      { N_("Bard"),      },
// TRANSLATORS: Player Block end
/* HeroClass::Barbarian */ { N_("Barbarian"), },
	// clang-format on
};

const std::array<PlayerStartingLoadoutData, enum_size<HeroClass>::value> PlayersStartingLoadoutData { {
	// clang-format off
// HeroClass                 skill,                  spell,             spellLevel,     items[0].diablo,       items[0].hellfire, items[1].diablo,  items[1].hellfire, items[2].diablo, items[2].hellfire, items[3].diablo, items[3].hellfire, items[4].diablo, items[4].hellfire, gold,
/* HeroClass::Warrior   */ { SpellID::ItemRepair,    SpellID::Null,              0, { { { IDI_WARRIOR,         IDI_WARRIOR,    }, { IDI_WARRSHLD,   IDI_WARRSHLD,   }, { IDI_WARRCLUB,  IDI_WARRCLUB,   }, { IDI_HEAL,    IDI_HEAL,  }, { IDI_HEAL,      IDI_HEAL, }, }, },  100, },
/* HeroClass::Rogue     */ { SpellID::TrapDisarm,    SpellID::Null,              0, { { { IDI_ROGUE,           IDI_ROGUE,      }, { IDI_HEAL,       IDI_HEAL,       }, { IDI_HEAL,      IDI_HEAL,       }, { IDI_NONE,    IDI_NONE,  }, { IDI_NONE,      IDI_NONE, }, }, },  100, },
/* HeroClass::Sorcerer  */ { SpellID::StaffRecharge, SpellID::Firebolt,          2, { { { IDI_SORCERER_DIABLO, IDI_SORCERER,   }, { IDI_MANA,       IDI_HEAL,       }, { IDI_MANA,      IDI_HEAL,       }, { IDI_NONE,    IDI_NONE,  }, { IDI_NONE,      IDI_NONE, }, }, },  100, },
/* HeroClass::Monk      */ { SpellID::Search,        SpellID::Null,              0, { { { IDI_SHORTSTAFF,      IDI_SHORTSTAFF, }, { IDI_HEAL,       IDI_HEAL,       }, { IDI_HEAL,      IDI_HEAL,       }, { IDI_NONE,    IDI_NONE,  }, { IDI_NONE,      IDI_NONE, }, }, },  100, },
/* HeroClass::Bard      */ { SpellID::Identify,      SpellID::Null,              0, { { { IDI_BARDSWORD,       IDI_BARDSWORD,  }, { IDI_BARDDAGGER, IDI_BARDDAGGER, }, { IDI_HEAL,      IDI_HEAL,       }, { IDI_HEAL,    IDI_HEAL,  }, { IDI_NONE,      IDI_NONE, }, }, },  100, },
/* HeroClass::Barbarian */ { SpellID::Rage,          SpellID::Null,              0, { { { IDI_BARBARIAN,       IDI_BARBARIAN,  }, { IDI_WARRSHLD,   IDI_WARRSHLD,   }, { IDI_HEAL,      IDI_HEAL,       }, { IDI_HEAL,    IDI_HEAL,  }, { IDI_NONE,      IDI_NONE, }, }, },  100, }
	// clang-format on
} };

} // namespace

const ClassAttributes &GetClassAttributes(HeroClass playerClass)
{
	return ClassAttributesPerClass[static_cast<size_t>(playerClass)];
}

void LoadPlayerDataFiles()
{
	ReloadExperienceData();
	LoadClassesAttributes();
}

uint32_t GetNextExperienceThresholdForLevel(unsigned level)
{
	return ExperienceData.getThresholdForLevel(level);
}

uint8_t GetMaximumCharacterLevel()
{
	return ExperienceData.getMaxLevel();
}

const PlayerData &GetPlayerDataForClass(HeroClass playerClass)
{
	return PlayersData[static_cast<size_t>(playerClass)];
}

const _sfx_id herosounds[enum_size<HeroClass>::value][enum_size<HeroSpeech>::value] = {
	// clang-format off
	{ PS_WARR1,  PS_WARR2,  PS_WARR3,  PS_WARR4,  PS_WARR5,  PS_WARR6,  PS_WARR7,  PS_WARR8,  PS_WARR9,  PS_WARR10,  PS_WARR11,  PS_WARR12,  PS_WARR13,  PS_WARR14,  PS_WARR15,  PS_WARR16,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR24,  SFX_NONE, SFX_NONE, PS_WARR27,  SFX_NONE, PS_WARR29,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR34,  PS_WARR35,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR43,  SFX_NONE, SFX_NONE, PS_WARR46,  SFX_NONE, SFX_NONE, PS_WARR49,  PS_WARR50,  PS_WARR51,  PS_WARR52,  PS_WARR53,  PS_WARR54,  PS_WARR55,  PS_WARR56,  SFX_NONE, PS_WARR58,  PS_WARR59,  PS_WARR60,  PS_WARR61,  PS_WARR62,  PS_WARR63,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR68,  PS_WARR69,  PS_WARR70,  PS_WARR71,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR79,  PS_WARR80,  PS_WARR81,  PS_WARR82,  PS_WARR83,  PS_WARR84,  PS_WARR85,  PS_WARR86,  PS_WARR87,  PS_WARR88,  PS_WARR89,  PS_WARR90,  PS_WARR91,  PS_WARR92,  PS_WARR93,  PS_WARR94,  PS_WARR95,  PS_WARR96B, PS_WARR97,  PS_WARR98,  PS_WARR99,  PS_WARR100,  PS_WARR101,  PS_WARR102,  PS_DEAD    },
	{ PS_ROGUE1, PS_ROGUE2, PS_ROGUE3, PS_ROGUE4, PS_ROGUE5, PS_ROGUE6, PS_ROGUE7, PS_ROGUE8, PS_ROGUE9, PS_ROGUE10, PS_ROGUE11, PS_ROGUE12, PS_ROGUE13, PS_ROGUE14, PS_ROGUE15, PS_ROGUE16, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE24, SFX_NONE, SFX_NONE, PS_ROGUE27, SFX_NONE, PS_ROGUE29, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE34, PS_ROGUE35, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE43, SFX_NONE, SFX_NONE, PS_ROGUE46, SFX_NONE, SFX_NONE, PS_ROGUE49, PS_ROGUE50, PS_ROGUE51, PS_ROGUE52, PS_ROGUE53, PS_ROGUE54, PS_ROGUE55, PS_ROGUE56, SFX_NONE, PS_ROGUE58, PS_ROGUE59, PS_ROGUE60, PS_ROGUE61, PS_ROGUE62, PS_ROGUE63, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE68, PS_ROGUE69, PS_ROGUE70, PS_ROGUE71, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE79, PS_ROGUE80, PS_ROGUE81, PS_ROGUE82, PS_ROGUE83, PS_ROGUE84, PS_ROGUE85, PS_ROGUE86, PS_ROGUE87, PS_ROGUE88, PS_ROGUE89, PS_ROGUE90, PS_ROGUE91, PS_ROGUE92, PS_ROGUE93, PS_ROGUE94, PS_ROGUE95, PS_ROGUE96, PS_ROGUE97, PS_ROGUE98, PS_ROGUE99, PS_ROGUE100, PS_ROGUE101, PS_ROGUE102, PS_ROGUE71 },
	{ PS_MAGE1,  PS_MAGE2,  PS_MAGE3,  PS_MAGE4,  PS_MAGE5,  PS_MAGE6,  PS_MAGE7,  PS_MAGE8,  PS_MAGE9,  PS_MAGE10,  PS_MAGE11,  PS_MAGE12,  PS_MAGE13,  PS_MAGE14,  PS_MAGE15,  PS_MAGE16,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MAGE24,  SFX_NONE, SFX_NONE, PS_MAGE27,  SFX_NONE, PS_MAGE29,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MAGE34,  PS_MAGE35,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MAGE43,  SFX_NONE, SFX_NONE, PS_MAGE46,  SFX_NONE, SFX_NONE, PS_MAGE49,  PS_MAGE50,  PS_MAGE51,  PS_MAGE52,  PS_MAGE53,  PS_MAGE54,  PS_MAGE55,  PS_MAGE56,  SFX_NONE, PS_MAGE58,  PS_MAGE59,  PS_MAGE60,  PS_MAGE61,  PS_MAGE62,  PS_MAGE63,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MAGE68,  PS_MAGE69,  PS_MAGE70,  PS_MAGE71,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MAGE79,  PS_MAGE80,  PS_MAGE81,  PS_MAGE82,  PS_MAGE83,  PS_MAGE84,  PS_MAGE85,  PS_MAGE86,  PS_MAGE87,  PS_MAGE88,  PS_MAGE89,  PS_MAGE90,  PS_MAGE91,  PS_MAGE92,  PS_MAGE93,  PS_MAGE94,  PS_MAGE95,  PS_MAGE96,  PS_MAGE97,  PS_MAGE98,  PS_MAGE99,  PS_MAGE100,  PS_MAGE101,  PS_MAGE102,  PS_MAGE71  },
	{ PS_MONK1,  SFX_NONE,  SFX_NONE,  SFX_NONE,  SFX_NONE,  SFX_NONE,  SFX_NONE,  PS_MONK8,  PS_MONK9,  PS_MONK10,  PS_MONK11,  PS_MONK12,  PS_MONK13,  PS_MONK14,  PS_MONK15,  PS_MONK16,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MONK24,  SFX_NONE, SFX_NONE, PS_MONK27,  SFX_NONE, PS_MONK29,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MONK34,  PS_MONK35,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MONK43,  SFX_NONE, SFX_NONE, PS_MONK46,  SFX_NONE, SFX_NONE, PS_MONK49,  PS_MONK50,  SFX_NONE,   PS_MONK52,  SFX_NONE,   PS_MONK54,  PS_MONK55,  PS_MONK56,  SFX_NONE, SFX_NONE,   SFX_NONE,   SFX_NONE,   PS_MONK61,  PS_MONK62,  SFX_NONE,   SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MONK68,  PS_MONK69,  PS_MONK70,  PS_MONK71,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_MONK79,  PS_MONK80,  SFX_NONE,   PS_MONK82,  PS_MONK83,  SFX_NONE,   SFX_NONE,   SFX_NONE,   PS_MONK87,  PS_MONK88,  PS_MONK89,  SFX_NONE,   PS_MONK91,  PS_MONK92,  SFX_NONE,   PS_MONK94,  PS_MONK95,  PS_MONK96,  PS_MONK97,  PS_MONK98,  PS_MONK99,  SFX_NONE,    SFX_NONE,    SFX_NONE,    PS_MONK71  },
	{ PS_ROGUE1, PS_ROGUE2, PS_ROGUE3, PS_ROGUE4, PS_ROGUE5, PS_ROGUE6, PS_ROGUE7, PS_ROGUE8, PS_ROGUE9, PS_ROGUE10, PS_ROGUE11, PS_ROGUE12, PS_ROGUE13, PS_ROGUE14, PS_ROGUE15, PS_ROGUE16, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE24, SFX_NONE, SFX_NONE, PS_ROGUE27, SFX_NONE, PS_ROGUE29, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE34, PS_ROGUE35, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE43, SFX_NONE, SFX_NONE, PS_ROGUE46, SFX_NONE, SFX_NONE, PS_ROGUE49, PS_ROGUE50, PS_ROGUE51, PS_ROGUE52, PS_ROGUE53, PS_ROGUE54, PS_ROGUE55, PS_ROGUE56, SFX_NONE, PS_ROGUE58, PS_ROGUE59, PS_ROGUE60, PS_ROGUE61, PS_ROGUE62, PS_ROGUE63, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE68, PS_ROGUE69, PS_ROGUE70, PS_ROGUE71, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_ROGUE79, PS_ROGUE80, PS_ROGUE81, PS_ROGUE82, PS_ROGUE83, PS_ROGUE84, PS_ROGUE85, PS_ROGUE86, PS_ROGUE87, PS_ROGUE88, PS_ROGUE89, PS_ROGUE90, PS_ROGUE91, PS_ROGUE92, PS_ROGUE93, PS_ROGUE94, PS_ROGUE95, PS_ROGUE96, PS_ROGUE97, PS_ROGUE98, PS_ROGUE99, PS_ROGUE100, PS_ROGUE101, PS_ROGUE102, PS_ROGUE71 },
	{ PS_WARR1,  PS_WARR2,  PS_WARR3,  PS_WARR4,  PS_WARR5,  PS_WARR6,  PS_WARR7,  PS_WARR8,  PS_WARR9,  PS_WARR10,  PS_WARR11,  PS_WARR12,  PS_WARR13,  PS_WARR14,  PS_WARR15,  PS_WARR16,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR24,  SFX_NONE, SFX_NONE, PS_WARR27,  SFX_NONE, PS_WARR29,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR34,  PS_WARR35,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR43,  SFX_NONE, SFX_NONE, PS_WARR46,  SFX_NONE, SFX_NONE, PS_WARR49,  PS_WARR50,  PS_WARR51,  PS_WARR52,  PS_WARR53,  PS_WARR54,  PS_WARR55,  PS_WARR56,  SFX_NONE, PS_WARR58,  PS_WARR59,  PS_WARR60,  PS_WARR61,  PS_WARR62,  PS_WARR63,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR68,  PS_WARR69,  PS_WARR70,  PS_WARR71,  SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, SFX_NONE, PS_WARR79,  PS_WARR80,  PS_WARR81,  PS_WARR82,  PS_WARR83,  PS_WARR84,  PS_WARR85,  PS_WARR86,  PS_WARR87,  PS_WARR88,  PS_WARR89,  PS_WARR90,  PS_WARR91,  PS_WARR92,  PS_WARR93,  PS_WARR94,  PS_WARR95,  PS_WARR96B, PS_WARR97,  PS_WARR98,  PS_WARR99,  PS_WARR100,  PS_WARR101,  PS_WARR102,  PS_WARR71  },
	// clang-format on
};

const PlayerCombatData &GetPlayerCombatDataForClass(HeroClass clazz)
{
	return PlayersCombatData[static_cast<size_t>(clazz)];
}

const PlayerStartingLoadoutData &GetPlayerStartingLoadoutForClass(HeroClass clazz)
{
	return PlayersStartingLoadoutData[static_cast<size_t>(clazz)];
}

/** Contains the data related to each player class. */
const PlayerSpriteData PlayersSpriteData[] = {
	// clang-format off
// HeroClass                 classPath,  stand,   walk,   attack,   bow, swHit,   block,   lightning,   fire,   magic,   death

/* HeroClass::Warrior */   { "warrior",     96,     96,      128,    96,    96,      96,          96,     96,      96,     128 },
/* HeroClass::Rogue */     { "rogue",       96,     96,      128,   128,    96,      96,          96,     96,      96,     128 },
/* HeroClass::Sorcerer */  { "sorceror",    96,     96,      128,   128,    96,      96,         128,    128,     128,     128 },
/* HeroClass::Monk */      { "monk",       112,    112,      130,   130,    98,      98,         114,    114,     114,     160 },
/* HeroClass::Bard */      { "rogue",       96,     96,      128,   128,    96,      96,          96,     96,      96,     128 },
/* HeroClass::Barbarian */ { "warrior",     96,     96,      128,    96,    96,      96,          96,     96,      96,     128 },
	// clang-format on
};

const PlayerAnimData PlayersAnimData[] = {
	// clang-format off
// HeroClass                unarmedFrames, unarmedActionFrame, unarmedShieldFrames, unarmedShieldActionFrame, swordFrames, swordActionFrame, swordShieldFrames, swordShieldActionFrame, bowFrames, bowActionFrame, axeFrames, axeActionFrame, maceFrames, maceActionFrame, maceShieldFrames, maceShieldActionFrame, staffFrames, staffActionFrame, idleFrames,  walkingFrames, blockingFrames, deathFrames, castingFrames, recoveryFrames, townIdleFrames, townWalkingFrames, castingActionFrame
/* HeroClass::Warrior */   {           16,                  9,                  16,                        9,          16,                9,                16,                      9,        16,             11,        20,             10,         16,               9,               16,                     9,          16,               11,         10,              8,              2,          20,            20,              6,             20,                 8,                 14 },
/* HeroClass::Rogue */     {           18,                 10,                  18,                       10,          18,               10,                18,                     10,        12,              7,        22,             13,         18,              10,               18,                    10,          16,               11,          8,              8,              4,          20,            16,              7,             20,                 8,                 12 },
/* HeroClass::Sorcerer */  {           20,                 12,                  16,                        9,          16,               12,                16,                     12,        20,             16,        24,             16,         16,              12,               16,                    12,          16,               12,          8,              8,              6,          20,            12,              8,             20,                 8,                  8 },
/* HeroClass::Monk */      {           12,                  7,                  12,                        7,          16,               12,                16,                     12,        20,             14,        23,             14,         16,              12,               16,                    12,          13,                8,          8,              8,              3,          20,            18,              6,             20,                 8,                 13 },
/* HeroClass::Bard */      {           18,                 10,                  18,                       10,          18,               10,                18,                     10,        12,             11,        22,             13,         18,              10,               18,                    10,          16,               11,          8,              8,              4,          20,            16,              7,             20,                 8,                 12 },
/* HeroClass::Barbarian */ {           16,                  9,                  16,                        9,          16,                9,                16,                      9,        16,             11,        20,              8,         16,               8,               16,                     8,          16,               11,         10,              8,              2,          20,            20,              6,             20,                 8,                 14 },
	// clang-format on
};

} // namespace devilution
