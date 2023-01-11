/**
 * @file itemdat.h
 *
 * Interface of all item data.
 */
#pragma once

#include <cstdint>

#include "objdat.h"
#include "spelldat.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

enum class ItemIndex : int16_t { // TODO defines all indexes in AllItemsList
	Gold,
	WarriorSword,
	WarriorShield,
	WarriorClub,
	RogueBow,
	SorcererStaff,
	Cleaver,
	FirstQuest = Cleaver,
	UndeadCrown,
	EmpyreanBand,
	MagicRock,
	OpticAmulet,
	RingOfTruth,
	TavernSign,
	HarlequinCrest,
	VeilOfSteel,
	GoldenElixir,
	AnvilOfFury,
	BlackMushroom,
	Brain,
	FungalTome,
	SpectralElixir,
	BloodStone,
	Map,
	LastQuest = Map,
	Ear,
	PotionOfHealing,
	PotionOfMana,
	ScrollOfIdentify,
	ScrollOfTownPortal,
	ArkainesValor,
	PotionOfFullHealing,
	PotionOfFullMana,
	GriswoldsEdge,
	BovinePlate,
	StaffOfLazarus,
	ScrollOfResurrect,
	Oil,
	MonkShortStaff,
	BardSword,
	BardDagger,
	RuneBomb,
	Theodore,
	AuricAmulet,
	TornNote1,
	TornNote2,
	TornNote3,
	ReconstructedNote,
	BrownSuit,
	GreySuit,
	Cap,
	SkullCap,
	Helm,
	FullHelm,
	Crown,
	GreatHelm,
	Cape,
	Rags,
	Cloak,
	Robe,
	QuiltedArmor,
	LeatherArmor,
	HardLeatherArmor,
	StuddedLeatherArmor,
	RingMail,
	ChainMail,
	ScaleMail,
	BreastPlate,
	SplintMail,
	PlateMail,
	FieldPlate,
	GothicPlate,
	FullPlateMail,
	Buckler,
	SmallShield,
	LargeShield,
	KiteShield,
	TowerShield,
	GothicShield,
	PotionOfHealing2,
	PotionOfFullHealing2,
	PotionOfMana2,
	PotionOfFullMana2,
	PotionOfRejuvenation,
	PotionOfFullRejuvenation,
	BlacksmithOil,
	OilOfAccuracy,
	OilOfSharpness,
	Oil2,
	ElixirOfStrength,
	ElixirOfMagic,
	ElixirOfDexterity,
	ElixirOfVitality,
	ScrollOfHealing,
	ScrollOfSearch,
	ScrollOfLightning,
	ScrollOfIdentify2,
	ScrollOfResurrect2,
	ScrollOfFireWall,
	ScrollOfInferno,
	ScrollOfTownPortal2,
	ScrollOfFlash,
	ScrollOfInfravision,
	ScrollOfPhasing,
	ScrollOfManaShield,
	ScrollOfFlameWave,
	ScrollOfFireball,
	ScrollOfStoneCurse,
	ScrollOfChainLightning,
	ScrollOfGuardian,
	NonItem,
	ScrollOfNova,
	ScrollOfGolem,
	ScrollOfNone,
	ScrollOfTeleport,
	ScrollOfApocalypse,
	Book1,
	Book2,
	Book3,
	Book4,
	Dagger,
	ShortSword,
	Falchion,
	Scimitar,
	Claymore,
	Blade,
	Sabre,
	LongSword,
	BroadSword,
	BastardSword,
	TwoHandedSword,
	GreatSword,
	SmallAxe,
	Axe,
	LargeAxe,
	BroadAxe,
	BattleAxe,
	GreatAxe,
	Mace,
	MorningStar,
	WarHammer,
	SpikedClub,
	Club,
	Flail,
	Maul,
	ShortBow,
	HuntersBow,
	LongBow,
	CompositeBow,
	ShortBattleBow,
	LongBattleBow,
	ShortWarBow,
	LongWarBow,
	ShortStaff,
	LongStaff,
	CompositeStaff,
	QuarterStaff,
	WarStaff,
	Ring1,
	Ring2,
	Ring3,
	Amulet1,
	Amulet2,
	RuneOfFire,
	RuneOfLight,
	RuneOfImmolation,
	RuneOfNova,
	RuneOfStone,
	SorcererStaffDiablo,

	Last = SorcererStaffDiablo,
	None = -1,
};

enum class ItemDropRate : uint8_t {
	Never,
	Regular,
	Double,
};

enum class ItemClass : uint8_t {
	None,
	Weapon,
	Armor,
	Misc,
	Gold,
	Quest,
};

enum class ItemEquipType : int8_t {
	None,
	OneHand,
	TwoHands,
	Armor,
	Helm,
	Ring,
	Amulet,
	Unequipable,
	Belt,
	Invalid = -1,
};

/// Item graphic IDs; frame_num-11 of objcurs.cel.
enum class ItemCursorGraphic : uint8_t {
	// clang-format off
	PotionOfFullMana,
	Scroll,
	Scroll2, // unused
	Scroll3, // unused
	GoldSmall,
	GoldMedium,
	GoldLarge,
	GoldRing, // unused
	Bleeder,
	Bramble,
	RingOfTruth,
	RingOfRegha,
	Ring,
	RingOfEngagement,
	ConstrictingRing,
	SpectralElixir,
	ThreeColorPot, // unused
	GoldenElixir,
	EmpyreanBand,
	EarSorcerer,
	EarWarrior,
	EarRogue,
	Sphere, // unused
	Cube, // unused
	Pyramid, // unused
	BloodStone,
	JSphere, // unused
	JCube, // unused
	JPyramid, // unused
	Vial, // unused
	Oil,
	ElixirOfVitality,
	PotionOfHealing,
	PotionOfFullRejuvenation,
	ElixirOfMagic,
	PotionOfFullHealing,
	ElixirOfDexterity,
	PotionOfRejuvenation,
	ElixirOfStrength,
	PotionOfMana,
	Brain,
	Claw, // unused
	Fang, // unused
	Bread, // unused
	OpticAmulet,
	Amulet,
	Amulet2, // unused
	Amulet3, // unused
	Amulet4, // unused
	Pouch1, // unused
	Wizardspike,
	Dagger,
	BigBottle, // unused
	BlackRazor,
	GonnagalsDirk,
	Dagger5, // unused
	Blade,
	BastardSword,
	ExecutionersBlade,
	Mace,
	LongSword,
	BroadSword,
	Falchion,
	MorningStar,
	ShortSword,
	Claymore,
	Club,
	Sabre,
	FalconsTalon,
	Club1, // unused
	SpikedClub,
	Club3, // unused
	Scimitar,
	MagSword, // unused
	SkulSword, // unused
	FullHelm,
	MagicRock,
	HelmOfSpirits,
	UndeadCrown,
	RoyalCirclet,
	FoolsCrest,
	HarlequinCrest,
	Helm,
	Buckler,
	FHelm2, // unused
	VeilOfSteel,
	BookGrey,
	BookRed,
	BookBlue,
	BlackMushroom,
	SkullCap,
	Cap,
	TornFleshOfSouls,
	ThinkingCap,
	Clothes, // unused
	Crown,
	Map,
	FungalTome,
	GreatHelm,
	OverLordsHelm,
	CompShld, // unused
	BattleAxe,
	HuntersBow,
	FieldPlate,
	Stonecleaver,
	SmallShield,
	Cleaver,
	StuddedLeatherArmor,
	Eaglehorn,
	ShortStaff,
	TwoHandedSword,
	ChainMail,
	SmallAxe,
	KiteShield,
	ScaleMail,
	SmlShld, // unused
	SplitSkullShield,
	DragonsBreach,
	ShortBow,
	LongBattleBow,
	LongWarBow,
	WarHammer,
	Maul,
	LongStaff,
	WarStaff,
	LongStaffUnused, // unused
	TavernSign,
	HardLeatherArmor,
	Rags,
	QuiltedArmor,
	BallNChn, // unused
	Flail,
	TowerShield,
	CompositeBow,
	GreatSword,
	LeatherArmor,
	SplintMail,
	Robe,
	Nightscape,
	RingArmor, // unused
	AnvilOfFury,
	BroadAxe,
	LargeAxe,
	GreatAxe,
	Axe,
	GreatAxeUnused, // unused
	HolyDefender,
	LargeShield,
	GothicShield,
	Cloak,
	Cape,
	FullPlateMail,
	GothicPlate,
	BreastPlate,
	RingMail,
	StaffOfLazarus,
	GemGrtAxe, // unused
	ArkainesValor,
	Needler,
	NajsLightPlate,
	Grizzly,
	Grandfather,
	Protector,
	MesserschmidtsReaver,
	Windforce,
	ShortWarBow,
	CompositeStaff,
	ShortBattleBow,
	XorinesRing, // Gold in D1
	ManaRing, // unused
	AmuletOfWarding,
	NecMagic, // unused
	NecHealth, // unused
	KariksRing,
	RingGround, // unused
	AmulProt, // unused
	MercurialRing,
	RingOfThunder,
	NecTruth, // unused
	GiantsKnuckle,
	AuricAmulet,
	RingOfMystics,
	RingCopper, // unused
	AcolytesAmulet,
	RingOfMagma,
	NecPurify, // unused
	GladiatorsRing,
	RuneBomb,
	Theodore,
	TornNote1,
	TornNote2,
	TornNote3,
	ReconstructedNote,
	RuneOfFire,
	RuneOfImmolation,
	RuneOfLight,
	RuneOfNova,
	RuneOfStone,
	GreySuit,
	BrownSuit,
	EaterOfSouls,
	SwordGlam, // unused
	SwordSerr, // unused
	ArmorOfGloom,
	BoneChainArmor,
	Thunderclap,
	Diamondedge,
	StafJester, // unused
	StafMana, // unused
	Flambeau,
	GnatSting,
	AxeAncient, // unused
	ClubCarnag, // unused
	MaceDark, // unused
	ClubDecay, // unused
	AxeDecay, // unused
	SwrdDecay, // unused
	MaceDecay, // unused
	StafDecay, // unused
	Blitzen,
	ClubOuch, // unused
	SwrdDevast, // unused
	AxeDevast, // unused
	MornDevast, // unused
	MaceDevast, // unused
	DemonPlateArmor,
	BovinePlate,
	// clang-format on
};

enum class ItemType : int8_t {
	Misc,
	Sword,
	Axe,
	Bow,
	Mace,
	Shield,
	LightArmor,
	Helm,
	MediumArmor,
	HeavyArmor,
	Staff,
	Gold,
	Ring,
	Amulet,
	None = -1,
};

string_view ItemTypeToString(ItemType itemType);

enum class UniqueBaseItem : int8_t {
	None,
	ShortBow,
	LongBow,
	HuntersBow,
	CompoundBow,
	LongWarBow,
	LongBattleBow,
	Dagger,
	Falchion,
	Claymore,
	Broadsword,
	Sabre,
	Scimitar,
	Longsword,
	BastardSword,
	TwoHandedSword,
	GreatSword,
	Cleaver,
	LargeAxe,
	BroadAxe,
	SmallAxe,
	BattleAxe,
	GreatAxe,
	Mace,
	MorningStar,
	SpikedClub,
	Maul,
	WarHammer,
	Flail,
	LongStaff,
	ShortStaff,
	CompositeStaff,
	QuarterStaff,
	WarStaff,
	SkullCap,
	Helm,
	GreatHelm,
	Crown,
	Unused, // unused
	Rags,
	StuddedLeatherArmor,
	Cloak,
	Robe,
	ChainMail,
	LeatherArmor,
	BreastPlate,
	Cape,
	PlateMail,
	FullPlateMail,
	Buckler,
	SmallShield,
	LargeShield,
	KiteShield,
	GothicShield,
	Ring,
	Book, // unused
	Amulet,
	UndeadCrown,
	EmpyreanBand,
	OpticAmulet,
	RingOfTruth,
	HarlequinCrest,
	Map,
	Elixir,
	ArkainesValor,
	VeilOfSteel,
	GriswoldsEdge,
	Lightforge,
	StaffOfLazarus,
	BovinePlate,
	Invalid = -1,
};

enum class ItemSpecialEffect : uint32_t {
	// clang-format off
	None                   = 0,
	Infravision            = 1 << 0, // unused
	RandomStealLife        = 1 << 1,
	RandomArrowVelocity    = 1 << 2,
	FireArrows             = 1 << 3,
	FireDamage             = 1 << 4,
	LightningDamage        = 1 << 5,
	DrainLife              = 1 << 6,
	PlayerNoMana           = 1 << 7, // unused
	PlayerNoHeal           = 1 << 8, // unused
	MultipleArrows         = 1 << 9,
	HalfTrapDamage2        = 1 << 10, // unused
	Knockback              = 1 << 11,
	MonsterNoHeal          = 1 << 12,
	StealMana3             = 1 << 13,
	StealMana5             = 1 << 14,
	StealLife3             = 1 << 15,
	StealLife5             = 1 << 16,
	QuickAttack            = 1 << 17,
	FastAttack             = 1 << 18,
	FasterAttack           = 1 << 19,
	FastestAttack          = 1 << 20,
	FastHitRecovery        = 1 << 21,
	FasterHitRecovery      = 1 << 22,
	FastestHitRecovery     = 1 << 23,
	FastBlock              = 1 << 24,
	LightningArrows        = 1 << 25,
	Thorns                 = 1 << 26,
	NoMana                 = 1 << 27,
	HalfTrapDamage         = 1 << 28,
	OneHanded              = 1 << 29, // unused
	TripleDemonDamage      = 1 << 30,
	ZeroResistance         = 1U << 31,
	// clang-format on
};
use_enum_as_flags(ItemSpecialEffect);

enum class ItemSpecialEffectHf : uint8_t {
	// clang-format off
	None               = 0,
	Devastation        = 1 << 0,
	Decay              = 1 << 1,
	Peril              = 1 << 2,
	Jesters            = 1 << 3,
	Doppelganger       = 1 << 4,
	ArmorClassVsDemons = 1 << 5,
	ArmorClassVsUndead = 1 << 6,
	// clang-format on
};
use_enum_as_flags(ItemSpecialEffectHf);

enum class ItemMiscID : int8_t {
	// clang-format off
	None,
	UseFirst,
	PotionOfFullHealing,
	PotionOfHealing,
	PotionOfSeriousHealing, // Unused
	PotionOfDeadlyHealing, // Unused
	PotionOfMana,
	PotionOfFullMana,
	PotionOfExperience, // Unused
	PotionOfExperienceCurse, // Unused
	ElixirOfStrength,
	ElixirOfMagic,
	ElixirOfDexterity,
	ElixirOfVitality,
	ElixirOfStrengthCurse,  // Unused
	ElixirOfMagicCurse,  // Unused
	ElixirOfDexterityCurse, // Unused
	ElixirOfVitalityCurse, // Unused
	PotionOfRejuvenation,
	PotionOfFullRejuvenation,
	UseLast,
	Scroll,
	ScrollTargeted,
	Staff,
	Book,
	Ring,
	Amulet,
	Unique,
	Meat, // Unused
	OilFirst,
	Oil, /* oils are beta or hellfire only */
	OilOfAccuracy,
	OilOfMastery,
	OilOfSharpness,
	OilOfDeath,
	OilOfSkill,
	OilBlacksmith,
	OilOfFortitude,
	OilOfPermanence,
	OilOfHardening,
	OilOfImperviousness,
	OilLast,
	Map,
	Ear,
	SpectralElixir,
	Bomb, // Unused
	RuneFirst,
	RuneOfFire,
	RuneOfLight,
	RuneOfNova,
	RuneOfImmolation,
	RuneOfStone,
	RuneLast,
	AuricAmulet,
	ReconstructedNote,
	Invalid = -1,
	// clang-format on
};

struct ItemData {
	enum ItemDropRate iRnd;
	enum ItemClass iClass;
	enum ItemEquipType iLoc;
	enum ItemCursorGraphic iCurs;
	enum ItemType itype;
	enum UniqueBaseItem iItemId;
	const char *iName;
	const char *iSName;
	uint8_t iMinMLvl;
	uint8_t iDurability;
	uint8_t iMinDam;
	uint8_t iMaxDam;
	uint8_t iMinAC;
	uint8_t iMaxAC;
	uint8_t iMinStr;
	uint8_t iMinMag;
	uint8_t iMinDex;
	ItemSpecialEffect iFlags; // ItemSpecialEffect as bit flags
	enum ItemMiscID iMiscId;
	enum SpellID iSpell;
	bool iUsable;
	uint16_t iValue;
};

enum class ItemEffectType : int8_t {
	// clang-format off
	ToHit,
	ToHitCurse,
	DamagePercent,
	DamagePercentCurse,
	ToHitDamagePercent,
	ToHitDamagePercentCurse,
	ArmorClassPercent,
	ArmorClassPercentCurse,
	FireResistance,
	LightningResistance,
	MagicResistance,
	AllResistances,
	SpellCost, // unused
	SpellDuration, // unused
	SpellLevelAdd,
	Charges,
	FireDamage,
	LightningDamage,
	Chaos, // unused
	Strength,
	StrengthCurse,
	Magic,
	MagicCurse,
	Dexterity,
	DexterityCurse,
	Vitality,
	VitalityCurse,
	AllAttributes,
	AllAttributesCurse,
	GetHitCurse,
	GetHit,
	Life,
	LifeCurse,
	Mana,
	ManaCurse,
	Durability,
	DurabilityCurse,
	Indestructible,
	LightRadius,
	LightRadiusCurse,
	Invisibility, // unused
	MultipleArrows, /* only used in hellfire */
	FireArrows,
	LightningArrows,
	Graphic,
	Thorns,
	NoMana,
	PlayerNoHeal, // unused
	Fear, // unused
	Rabid, // unused
	Fireball, /* only used in hellfire */
	SeeInvisible, // unused
	HalfTrapDamage,
	Knockback,
	MonsterNoHeal, // unused
	StealMana,
	StealLife,
	TargetArmorClass,
	FastAttack,
	FastHitRecovery,
	FastBlock,
	DamageModifier,
	RandomArrowVelocity,
	SetDamage,
	SetDurability,
	NoMinimumStrength,
	Spell,
	FastSwing, // unused
	OneHanded,
	TripleDemonDamage,
	ZeroResistances,
	Hyperspace, // unused
	DrainLife,
	RandomLifeSteal,
	Infravision, // unused
	SetArmorClass,
	Lightning,
	ChargedBolt,
	LevelFireResistance, // unused
	SetArmorClassCurse,
	LastDiablo = SetArmorClassCurse,
	FireResistanceCurse,
	LightningResistanceCurse,
	MagicResistanceCurse,
	AllResistancesCurse, // unused
	Devastation,
	Decay,
	Peril,
	Jesters,
	Crystalline,
	Doppelganger,
	ArmorClassVsDemons,
	ArmorClassVsUndead,
	ManaToLife,
	LifeToMana,
	X, // unused
	Invalid = -1,
	// clang-format on
};

enum class GoodOrEvil : uint8_t {
	Any,
	Evil,
	Good,
};

enum class AffixItemType : uint8_t {
	// clang-format off
	None      = 0,
	Misc      = 1 << 0,
	Bow       = 1 << 1,
	Staff     = 1 << 2,
	Weapon    = 1 << 3,
	Shield    = 1 << 4,
	Armor     = 1 << 5,
	// clang-format on
};
use_enum_as_flags(AffixItemType);

struct ItemPower {
	ItemEffectType type = ItemEffectType::Invalid;
	int param1 = 0;
	int param2 = 0;
};

struct PLStruct {
	const char *PLName;
	ItemPower power;
	int8_t PLMinLvl;
	AffixItemType PLIType; // AffixItemType as bit flags
	enum GoodOrEvil PLGOE;
	bool PLDouble;
	bool PLOk;
	int minVal;
	int maxVal;
	int multVal;
};

struct UniqueItem {
	const char *UIName;
	enum UniqueBaseItem UIItemId;
	int8_t UIMinLvl;
	uint8_t UINumPL;
	int UIValue;
	ItemPower powers[6];
};

extern ItemData AllItemsList[];
extern const PLStruct ItemPrefixes[];
extern const PLStruct ItemSuffixes[];
extern const UniqueItem UniqueItems[];

} // namespace devilution
