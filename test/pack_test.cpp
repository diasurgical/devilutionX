#include <cstdint>

#include <gtest/gtest.h>

#include "pack.h"
#include "utils/paths.h"

namespace devilution {
namespace {

void SwapLE(ItemPack &pack)
{
	pack.iSeed = SDL_SwapLE32(pack.iSeed);
	pack.iCreateInfo = SDL_SwapLE16(pack.iCreateInfo);
	pack.idx = SDL_SwapLE16(pack.idx);
	pack.wValue = SDL_SwapLE16(pack.wValue);
	pack.dwBuff = SDL_SwapLE32(pack.dwBuff);
}

ItemPack SwappedLE(const ItemPack &pack)
{
	ItemPack swapped = pack;
	SwapLE(swapped);
	return swapped;
}

void ComparePackedItems(const ItemPack &item1LE, const ItemPack &item2LE)
{
	// Packs are little-endian.
	// Swap to native endianness on big-endian systems before comparison for better error messages.
	const ItemPack item1 = SwappedLE(item1LE);
	const ItemPack item2 = SwappedLE(item2LE);

	// `ItemPack` is packed, so we copy the unaligned values out before comparing them.
	// This avoids the following UBSAN error such as this one:
	// runtime error: load of misaligned address for type 'const unsigned int', which requires 4 byte alignment
	{
		const auto item1_iSeed = item1.iSeed;
		const auto item2_iSeed = item2.iSeed;
		EXPECT_EQ(item1_iSeed, item2_iSeed);
	}
	{
		const auto item1_iCreateInfo = item1.iCreateInfo;
		const auto item2_iCreateInfo = item2.iCreateInfo;
		EXPECT_EQ(item1_iCreateInfo, item2_iCreateInfo);
	}
	{
		const auto item1_idx = item1.idx;
		const auto item2_idx = item2.idx;
		EXPECT_EQ(item1_idx, item2_idx);
	}
	EXPECT_EQ(item1.bId, item2.bId);
	EXPECT_EQ(item1.bDur, item2.bDur);
	EXPECT_EQ(item1.bMDur, item2.bMDur);
	EXPECT_EQ(item1.bCh, item2.bCh);
	EXPECT_EQ(item1.bMCh, item2.bMCh);
	{
		const auto item1_wValue = item1.wValue;
		const auto item2_wValue = item2.wValue;
		EXPECT_EQ(item1_wValue, item2_wValue);
	}
	{
		const auto item1_dwBuff = item1.dwBuff;
		const auto item2_dwBuff = item2.dwBuff;
		EXPECT_EQ(item1_dwBuff, item2_dwBuff);
	}
}
typedef struct TestItemStruct {
	char _iIName[64];
	ItemType _itype;
	ItemClass _iClass;
	ItemCursorGraphic _iCurs;
	int _iIvalue;
	int _iMinDam;
	int _iMaxDam;
	int _iAC;
	ItemSpecialEffect _iFlags;
	ItemMiscID _iMiscId;
	SpellID _iSpell;
	int _iCharges;
	int _iMaxCharges;
	int _iDurability;
	int _iMaxDur;
	int _iPLDam;
	int _iPLToHit;
	int _iPLAC;
	int _iPLStr;
	int _iPLMag;
	int _iPLDex;
	int _iPLVit;
	int _iPLFR;
	int _iPLLR;
	int _iPLMR;
	int _iPLMana;
	int _iPLHP;
	int _iPLDamMod;
	int _iPLGetHit;
	int _iPLLight;
	int8_t _iSplLvlAdd;
	int _iUid;
	int _iFMinDam;
	int _iFMaxDam;
	int _iLMinDam;
	int _iLMaxDam;
	ItemEffectType _iPrePower;
	ItemEffectType _iSufPower;
	int8_t _iMinStr;
	uint8_t _iMinMag;
	int8_t _iMinDex;
	ItemIndex IDidx;
} TestItemStruct;

static void CompareItems(const Item &item1, const TestItemStruct &item2)
{
	ASSERT_STREQ(item1._iIName, item2._iIName);
	EXPECT_EQ(item1._itype, item2._itype);
	EXPECT_EQ(item1._iClass, item2._iClass);
	EXPECT_EQ(item1._iCurs, item2._iCurs);
	EXPECT_EQ(item1._iIvalue, item2._iIvalue);
	EXPECT_EQ(item1._iMinDam, item2._iMinDam);
	EXPECT_EQ(item1._iMaxDam, item2._iMaxDam);
	EXPECT_EQ(item1._iAC, item2._iAC);
	EXPECT_EQ(item1._iFlags, item2._iFlags);
	EXPECT_EQ(item1._iMiscId, item2._iMiscId);
	EXPECT_EQ(item1._iSpell, item2._iSpell);
	EXPECT_EQ(item1._iCharges, item2._iCharges);
	EXPECT_EQ(item1._iMaxCharges, item2._iMaxCharges);
	EXPECT_EQ(item1._iDurability, item2._iDurability);
	EXPECT_EQ(item1._iMaxDur, item2._iMaxDur);
	EXPECT_EQ(item1._iPLDam, item2._iPLDam);
	EXPECT_EQ(item1._iPLToHit, item2._iPLToHit);
	EXPECT_EQ(item1._iPLAC, item2._iPLAC);
	EXPECT_EQ(item1._iPLStr, item2._iPLStr);
	EXPECT_EQ(item1._iPLMag, item2._iPLMag);
	EXPECT_EQ(item1._iPLDex, item2._iPLDex);
	EXPECT_EQ(item1._iPLVit, item2._iPLVit);
	EXPECT_EQ(item1._iPLFR, item2._iPLFR);
	EXPECT_EQ(item1._iPLLR, item2._iPLLR);
	EXPECT_EQ(item1._iPLMR, item2._iPLMR);
	EXPECT_EQ(item1._iPLMana, item2._iPLMana);
	EXPECT_EQ(item1._iPLHP, item2._iPLHP);
	EXPECT_EQ(item1._iPLDamMod, item2._iPLDamMod);
	EXPECT_EQ(item1._iPLGetHit, item2._iPLGetHit);
	EXPECT_EQ(item1._iPLLight, item2._iPLLight);
	EXPECT_EQ(item1._iSplLvlAdd, item2._iSplLvlAdd);
	EXPECT_EQ(item1._iUid, item2._iUid);
	EXPECT_EQ(item1._iFMinDam, item2._iFMinDam);
	EXPECT_EQ(item1._iFMaxDam, item2._iFMaxDam);
	EXPECT_EQ(item1._iLMinDam, item2._iLMinDam);
	EXPECT_EQ(item1._iLMaxDam, item2._iLMaxDam);
	EXPECT_EQ(item1._iPrePower, item2._iPrePower);
	EXPECT_EQ(item1._iSufPower, item2._iSufPower);
	EXPECT_EQ(item1._iMinStr, item2._iMinStr);
	EXPECT_EQ(item1._iMinMag, item2._iMinMag);
	EXPECT_EQ(item1._iMinDex, item2._iMinDex);
	EXPECT_EQ(item1.IDidx, item2.IDidx);
}

const ItemPack PackedDiabloItems[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
	{ 2082213289,       0x119,  53,   3,   60,    60,   0,    0,      0,      0 }, // Amber Helm of harmony
	{  338833725,       0x118, 154,   3,    0,     0,   0,    0,      0,      0 }, // Cobalt Amulet of giants
	{  545145866,       0x11A, 120,   3,   38,    40,   0,    0,      0,      0 }, // Brutal Sword of gore
	{ 1504248345,       0x35A,  70,   5,  255,   255,   0,    0,      0,      0 }, // Demonspike Coat
	{ 1884424756,       0x146, 151,   3,    0,     0,   0,    0,      0,      0 }, // Steel Ring of the jaguar
	{ 1712759905,        0xDC, 151,   3,    0,     0,   0,    0,      0,      0 }, // Ring of the heavens
	{  981777658,       0x11E, 153,   2,    0,     0,   0,    0,      0,      0 }, // Ring of sorcery
	{  844854815,       0x11A,  75,   3,  255,   255,   0,    0,      0,      0 }, // Tower Shield of the ages
	{ 1151513535,       0x11E,  73,   2,   12,    32,   0,    0,      0,      0 }, // Sapphire Large Shield
	{  640243885,           6,  27,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Town Portal
	{  741806938,           9,  25,   0,    0,     0,   0,    0,      0,      0 }, // ItemIndex::PotionOfMana
	{ 1456608333,         257,  79,   0,    0,     0,   0,    0,      0,      0 }, // Mana
	{  554676945,          16,  30,   0,    0,     0,   0,    0,      0,      0 }, // Full mana
	{  355389938,           0,  24,   0,    0,     0,   0,    0,      0,      0 }, // Healing
	{  868435486,          16,  29,   0,    0,     0,   0,    0,      0,      0 }, // Full healing
	{ 1372832903,           0,   4,   0,   30,    30,   0,    0,      0,      0 }, // ItemIndex::RogueBow
	{  896239556,        2068,  53,   3,   56,    60,   0,    0,      0,      0 }, // Jade Helm of the wolf
	{ 1286225254,         269, 151,   3,    0,     0,   0,    0,      0,      0 }, // Steel Ring of accuracy
	{  548642293,         266,  21,   0,    0,     0,   0,    0,      0,      0 }, // Blood Stone
	{  307669016,         270, 151,   3,    0,     0,   0,    0,      0,      0 }, // Ring of power
	{  204766888,         332, 154,   3,    0,     0,   0,    0,      0,      0 }, // Gold Amulet of accuracy
	{ 1642744332,         273, 122,   3,   25,    60,   0,    0,      0,      0 }, // Bastard Sword of the bat
	{ 1031508271,        1036,  72,   0,   14,    24,   0,    0,      0,      0 }, // Small Shield
	{ 1384620228,         338,  65,   3,   44,    80,   0,    0,      0,      0 }, // Breast Plate of giants
	{  681296530,         266,  87,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Healing
	{  109984285,       16390,  81,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Rejuvenation - made by Pepin
	{ 1857753366,         260,  81,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Rejuvenation
	{  965103261,         273,  82,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Full Rejuvenation
	{  430621075,         217, 141,   3,   45,    45,   0,    0,      0,      0 }, // Savage Bow of perfection
	{ 1272669062,         258, 115,   0,   10,    20,   0,    0,      0,      0 }, // Falchion
	{ 1133884051,         278, 120,   2,   18,    40,   0,    0,      0,      0 }, // Long Sword of vim
	{ 1743897351,         259, 146,   2,   10,    25,  60,   60,      0,      0 }, // Frog's Staff of Holy Bolt
	{  429107209,           0,   5,   0,   25,    25,   9,   40,      0,      0 }, // ItemIndex::SorcererStaff
	{  466015738,         257, 146,   0,   18,    25,  50,   50,      0,      0 }, // Staff of Charged Bolt
	{  686949358,         193,  48,   3,   12,    15,   0,    0,      0,      0 }, // Cap of the mind armor
	{  888855755,         195,  58,   3,   30,    30,   0,    0,      0,      0 }, // Armor of protection
	{          2,         776,   8,   5,    0,     0,   0,    0,      0,      0 }, // Empyrean Band,
	{          3,         776,  10,   5,    0,     0,   0,    0,      0,      0 }, // Optic Amulet
	{          4,         772,  11,   5,    0,     0,   0,    0,      0,      0 }, // Ring of Truth
	{          5,         776,  13,   5,   15,    15,   0,    0,      0,      0 }, // Harlequin Crest
	{          6,         527,  14,   5,   60,    60,   0,    0,      0,      0 }, // Veil of Steel
	{          7,         781,  28,   5,   39,    40,   0,    0,      0,      0 }, // Arkaine's Valor
	{          8,         787,  31,   5,   42,    44,   0,    0,      0,      0 }, // Griswold's Edge
	{  557339094,        8208, 150,   3,   75,    75,   0,    0,      0,      0 }, // War Staff of haste
	{ 1684844665,        8208, 150,   3,   75,    75,  56,   56,      0,      0 }, // White Staff of Lightning
	{ 1297052552,        2074, 137,   3,   50,    50,   0,    0,      0,      0 }, // Lightning Maul
	{  981895960,        2073, 130,   3,   75,    75,   0,    0,      0,      0 }, // Ivory Great Axe of blood
	{  935416728,        2070,  52,   3,   18,    40,   0,    0,      0,      0 }, // Jade Crown of vim
	{ 1140525626,         257, 138,   3,   16,    30,   0,    0,      0,      0 }, // Short Bow of atrophy
	{ 1187758333,         258, 113,   3,   11,    16,   0,    0,      0,      0 }, // Brass Dagger of weakness
	{ 1283803700,         267, 138,   3,   16,    30,   0,    0,      0,      0 }, // Clumsy Short Bow
	{ 1317748726,         261, 114,   3,   17,    24,   0,    0,      0,      0 }, // Tin Sword of the fool
	{ 1331764286,         261, 135,   3,    6,    20,   0,    0,      0,      0 }, // Club of paralysis
	{ 1375639122,         269, 146,   3,   18,    25,  46,   46,      0,      0 }, // Dull Staff of Lightning
	{  145523894,         277, 115,   3,    6,    20,   0,    0,      0,      0 }, // Falchion of speed
	{ 1527777846,         259, 115,   3,   14,    20,   0,    0,      0,      0 }, // Bent Falchion
	{ 1655088363,         288, 146,   3,   13,    25,  98,   98,      0,      0 }, // Plentiful Staff of Firebolt
	{ 1679472538,         263, 113,   3,    6,    16,   0,    0,      0,      0 }, // Dagger of illness
	{ 1812092773,         264,  54,   3,    4,    12,   0,    0,      0,      0 }, // Cape of corruption
	{ 1965885799,         278, 119,   3,   33,    45,   0,    0,      0,      0 }, // Sabre of trouble
	{ 1970135469,         258,  48,   3,    5,    15,   0,    0,      0,      0 }, // Cap of tears
	{ 1979635474,         261, 135,   3,   14,    20,   0,    0,      0,      0 }, // Tin Club
	{ 2008721689,         258,  54,   3,    9,    12,   0,    0,      0,      0 }, // Rusted Cape of dyslexia
	{ 2082373839,         262,  48,   3,   10,    15,   0,    0,      0,      0 }, // Cap of pain
	{  278391972,         263, 119,   3,    1,     1,   0,    0,      0,      0 }, // Clumsy Sabre of fragility
	{  283130709,         260,  48,   3,    5,    15,   0,    0,      0,      0 }, // Vulnerable Cap of health
	{  308974695,         264, 113,   3,    5,    16,   0,    0,      0,      0 }, // Useless Dagger
	{  588501657,         300, 146,   3,   17,    25,  36,   36,      0,      0 }, // Bountiful Staff of Fire Wall
	{  640482348,         259, 131,   3,   22,    32,   0,    0,      0,      0 }, // Mace of frailty
	{  715324531,         258, 138,   3,   20,    30,   0,    0,      0,      0 }, // Weak Short Bow
	{  794222370,         259, 146,   3,   12,    25,   0,    0,      0,      0 }, // Short Staff of readiness
	{   49304273,           6,  24,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Healing
	{ 1015622844,           6,  29,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Full Healing
	{  376595272,           6,  25,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Mana
	{ 1354859033,           6,  30,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Full Mana
	{ 2088923858,           6,  27,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Town Portal
	{  998169832,         769, 138,   5,   30,    30,   0,    0,      0,      0 }, // The Rift Bow
	{  583181782,         734, 124,   5,  100,   100,   0,    0,      0,      0 }, // The Grandfather
	{ 1488997713,         257,  76,   3,   17,    60,   0,    0,      0,      0 }, // Gothic Shield of thorns
	{ 2140415407,         723,  67,   5,   75,    75,   0,    0,      0,      0 }, // Naj's Light Plate
	{  601465140,         777, 151,   5,    0,     0,   0,    0,      0,      0 }, // Constricting Ring
	{ 1154984294,         728,  53,   5,   60,    60,   0,    0,      0,      0 }, // Gotterdamerung
	{  951404319,         220, 122,   3,   32,    60,   0,    0,      0,      0 }, // King's Bastard Sword
	{ 1452181783,        4134, 135,   3,   60,    60,   0,    0,      0,      0 }, // Champion's Club of gore
	{  846169922,        2077, 136,   3,   60,    60,   0,    0,      0,      0 }, // Master's Flail
	{   18974604,        2078, 121,   3,   60,    60,   0,    0,      0,      0 }, // Knight's Broad Sword
	{ 1535190647,         284, 115,   3,   13,    20,   0,    0,      0,      0 }, // Lord's Falchion of precision
	{  300540323,         286, 122,   3,   60,    60,   0,    0,      0,      0 }, // Soldier's Sword of vigor
	{ 1813930929,         266, 140,   3,   35,    35,   0,    0,      0,      0 }, // Fine Long Bow of the pit
	{  733908617,         257, 120,   3,   24,    40,   0,    0,      0,      0 }, // Sharp Sword of atrophy
	{ 1158010141,         319, 145,   3,   60,    60,   0,    0,      0,      0 }, // Emerald Bow of burning
	// clang-format on
};

constexpr ItemSpecialEffect EmpyreanBandSpecialEffect = ItemSpecialEffect::FastHitRecovery | ItemSpecialEffect::HalfTrapDamage;
constexpr ItemSpecialEffect GrisworldEdgeSpecialEffect = ItemSpecialEffect::FireDamage | ItemSpecialEffect::Knockback | ItemSpecialEffect::FastAttack;

const TestItemStruct DiabloItems[] = {
	// clang-format off
	//_iIName,                        _itype,                      _iClass,             _iCurs,                                  _iIvalue, _iMinDam, _iMaxDam, _iAC,                                    _iFlags,       _iMiscId,                           _iSpell,       _iCharges, _iMaxCharges, _iDurability, _iMaxDur, _iPLDam, _iPLToHit, _iPLAC, _iPLStr, _iPLMag, _iPLDex, _iPLVit, _iPLFR, _iPLLR, _iPLMR, _iPLMana, _iPLHP, _iPLDamMod, _iPLGetHit, _iPLLight, _iSplLvlAdd, _iUid, _iFMinDam, _iFMaxDam, _iLMinDam, _iLMaxDam,         _iPrePower,                              _iSufPower,                   _iMinStr, _iMinMag, _iMinDex,   IDidx                             );
	{ "Amber Helm of harmony",        ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::GreatHelm,               21100,        0,        0,   11,      ItemSpecialEffect::FastestHitRecovery,       ItemMiscID::None,                   SpellID::Null,         0,            0,           60,       60,       0,         0,      0,       0,       0,       0,       0,     18,     18,     18,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::AllResistances,          ItemEffectType::FastHitRecovery,    50,        0,        0,   ItemIndex::GreatHelm              },
	{ "Cobalt Amulet of giants",      ItemType::Amulet,            ItemClass::Misc,     ItemCursorGraphic::Amulet,                  26840,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Amulet,                 SpellID::Null,         0,            0,            0,        0,       0,         0,      0,      19,       0,       0,       0,      0,     46,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::LightningResistance,     ItemEffectType::Strength,            0,        0,        0,   ItemIndex::Amulet1                },
	{ "Brutal Long Sword of gore",    ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::LongSword,               13119,        2,       10,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           38,       40,      91,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,         12,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercent,           ItemEffectType::DamageModifier,     30,        0,       30,   ItemIndex::LongSword              },
	{ "Demonspike Coat",              ItemType::HeavyArmor,        ItemClass::Armor,    ItemCursorGraphic::FullPlateMail,          251175,        0,        0,  100,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,          255,      255,       0,         0,      0,      10,       0,       0,       0,     50,      0,      0,        0,      0,          0,         -6,         0,           0,    78,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,            90,        0,        0,   ItemIndex::FullPlateMail          },
	{ "Steel Ring of the jaguar",     ItemType::Ring,              ItemClass::Misc,     ItemCursorGraphic::Ring,                    10600,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,                   SpellID::Null,         0,            0,            0,        0,       0,        15,      0,       0,       0,       0,       0,      0,      0,      0,        0,   1024,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHit,                   ItemEffectType::Life,                0,        0,        0,   ItemIndex::Ring1                  },
	{ "Ring of the heavens",          ItemType::Ring,              ItemClass::Misc,     ItemCursorGraphic::Ring,                    37552,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,                   SpellID::Null,         0,            0,            0,        0,       0,         0,      0,      14,      14,      14,      14,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::AllAttributes,       0,        0,        0,   ItemIndex::Ring1                  },
	{ "Ring of sorcery",              ItemType::Ring,              ItemClass::Misc,     ItemCursorGraphic::Ring,                    10200,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,                   SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,      16,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Magic,               0,        0,        0,   ItemIndex::Ring3                  },
	{ "Tower Shield of the ages",     ItemType::Shield,            ItemClass::Armor,    ItemCursorGraphic::TowerShield,              4850,        0,        0,   18,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Indestructible,     60,        0,        0,   ItemIndex::TowerShield            },
	{ "Sapphire Large Shield",        ItemType::Shield,            ItemClass::Armor,    ItemCursorGraphic::LargeShield,             21000,        0,        0,    7,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           12,       32,       0,         0,      0,       0,       0,       0,       0,      0,     60,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::LightningResistance,     ItemEffectType::Invalid,            40,        0,        0,   ItemIndex::LargeShield            },
	{ "Scroll of Town Portal",        ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::Scroll,                    200,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Scroll,                 SpellID::TownPortal,   0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::ScrollOfTownPortal       },
	{ "Potion of Mana",               ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfMana,                 50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfMana,             SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfMana             },
	{ "Potion of Mana",               ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfMana,                 50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfMana,             SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfMana2            },
	{ "Potion of Full Mana",          ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfFullMana,            150,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfFullMana,         SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfFullMana         },
	{ "Potion of Healing",            ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfHealing,              50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfHealing,          SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfHealing          },
	{ "Potion of Full Healing",       ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfFullHealing,         150,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfFullHealing,      SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfFullHealing      },
	{ "Short Bow",                    ItemType::Bow,               ItemClass::Weapon,   ItemCursorGraphic::ShortBow,                  100,        1,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           30,       30,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::RogueBow               },
	{ "Jade Great Helm of the wolf",  ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::GreatHelm,               22310,        0,        0,   14,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           56,       60,       0,         0,      0,       0,       0,       0,       0,     27,     27,     27,        0,   2112,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::AllResistances,          ItemEffectType::Life,               50,        0,        0,   ItemIndex::GreatHelm              },
	{ "Steel Ring of accuracy",       ItemType::Ring,              ItemClass::Misc,     ItemCursorGraphic::Ring,                    13400,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,                   SpellID::Null,         0,            0,            0,        0,       0,        14,      0,       0,       0,      15,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHit,                   ItemEffectType::Dexterity,           0,        0,        0,   ItemIndex::Ring1                  },
	{ "Blood Stone",                  ItemType::Misc,              ItemClass::Quest,    ItemCursorGraphic::BloodStone,                  0,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::BloodStone             },
	{ "Ring of power",                ItemType::Ring,              ItemClass::Misc,     ItemCursorGraphic::Ring,                     6400,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,                   SpellID::Null,         0,            0,            0,        0,       0,         0,      0,      12,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Strength,            0,        0,        0,   ItemIndex::Ring1                  },
	{ "Gold Amulet of accuracy",      ItemType::Amulet,            ItemClass::Misc,     ItemCursorGraphic::Amulet,                  20896,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Amulet,                 SpellID::Null,         0,            0,            0,        0,       0,        25,      0,       0,       0,      14,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHit,                   ItemEffectType::Dexterity,           0,        0,        0,   ItemIndex::Amulet1                },
	{ "Bastard Sword of the bat",     ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::BastardSword,            10500,        6,       15,    0,              ItemSpecialEffect::StealMana3,       ItemMiscID::None,                   SpellID::Null,         0,            0,           25,       60,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::StealMana,          50,        0,        0,   ItemIndex::BastardSword           },
	{ "Small Shield",                 ItemType::Shield,            ItemClass::Armor,    ItemCursorGraphic::SmallShield,                90,        0,        0,    3,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           14,       24,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,            25,        0,        0,   ItemIndex::SmallShield            },
	{ "Breast Plate of giants",       ItemType::HeavyArmor,        ItemClass::Armor,    ItemCursorGraphic::BreastPlate,             23250,        0,        0,   20,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           44,       80,       0,         0,      0,      17,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Strength,           40,        0,        0,   ItemIndex::BreastPlate            },
	{ "Scroll of Healing",            ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::Scroll,                     50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Scroll,                 SpellID::Healing,      0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::ScrollOfHealing          },
	{ "Potion of Rejuvenation",       ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfRejuvenation,        120,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfRejuvenation,     SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfRejuvenation     },
	{ "Potion of Rejuvenation",       ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfRejuvenation,        120,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfRejuvenation,     SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfRejuvenation     },
	{ "Potion of Full Rejuvenation",  ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfFullRejuvenation,    600,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfFullRejuvenation, SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfFullRejuvenation },
	{ "Savage Bow of perfection",     ItemType::Bow,               ItemClass::Weapon,   ItemCursorGraphic::CompositeBow,            23438,        3,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           45,       45,     117,         0,      0,       0,       0,      22,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercent,           ItemEffectType::Dexterity,          25,        0,       40,   ItemIndex::CompositeBow           },
	{ "Falchion",                     ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Falchion,                  250,        4,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           10,       20,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,            30,        0,        0,   ItemIndex::Falchion               },
	{ "Long Sword of vim",            ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::LongSword,                4400,        2,       10,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           18,       40,       0,         0,      0,       0,       0,       0,      15,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Vitality,           30,        0,       30,   ItemIndex::LongSword              },
	{ "Frog's Staff of Holy Bolt",    ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::ShortStaff,                  1,        2,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,                  SpellID::HolyBolt,    60,           60,           10,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,     -384,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ManaCurse,               ItemEffectType::Invalid,             0,       20,        0,   ItemIndex::ShortStaff             },
	{ "Short Staff of Charged Bolt",  ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::ShortStaff,                520,        2,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,                  SpellID::ChargedBolt,  9,           40,           25,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,       20,        0,   ItemIndex::SorcererStaffDiablo    },
	{ "Short Staff of Charged Bolt",  ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::ShortStaff,                  1,        2,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,                  SpellID::ChargedBolt, 50,           50,           18,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,       25,        0,   ItemIndex::ShortStaff             },
	{ "Cap of the mind",              ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::Cap,                      1845,        0,        0,    2,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           12,       15,       0,         0,      0,       0,       9,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Magic,               0,        0,        0,   ItemIndex::Cap                    },
	{ "Quilted Armor of protection",  ItemType::LightArmor,        ItemClass::Armor,    ItemCursorGraphic::QuiltedArmor,             1200,        0,        0,    7,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           30,       30,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,         -2,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::GetHit,              0,        0,        0,   ItemIndex::QuiltedArmor           },
	{ "Empyrean Band",                ItemType::Ring,              ItemClass::Misc,     ItemCursorGraphic::EmpyreanBand,             8000,        0,        0,    0,                  EmpyreanBandSpecialEffect,       ItemMiscID::Unique,                 SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       2,       2,       2,       2,      0,      0,      0,        0,      0,          0,          0,         2,           0,     2,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::EmpyreanBand           },
	{ "Optic Amulet",                 ItemType::Amulet,            ItemClass::Misc,     ItemCursorGraphic::OpticAmulet,              9750,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Unique,                 SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       5,       0,       0,      0,     20,      0,        0,      0,          0,         -1,         2,           0,     3,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::OpticAmulet            },
	{ "Ring of Truth",                ItemType::Ring,              ItemClass::Misc,     ItemCursorGraphic::RingOfTruth,                9100,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Unique,                 SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,     10,     10,     10,        0,    640,          0,         -1,         0,           0,     4,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::RingOfTruth              },
	{ "Harlequin Crest",              ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::HarlequinCrest,           4000,        0,        0,   -3,                    ItemSpecialEffect::None,       ItemMiscID::Unique,                 SpellID::Null,         0,            0,           15,       15,       0,         0,      0,       2,       2,       2,       2,      0,      0,      0,      448,    448,          0,         -1,         0,           0,     5,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::HarlequinCrest         },
	{ "Veil of Steel",                ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::VeilOfSteel,               63800,        0,        0,   18,                    ItemSpecialEffect::None,       ItemMiscID::Unique,                 SpellID::Null,         0,            0,           60,       60,       0,         0,     60,      15,       0,       0,      15,     50,     50,     50,    -1920,      0,          0,          0,        -2,           0,     6,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::VeilOfSteel              },
	{ "Arkaine's Valor",              ItemType::MediumArmor,       ItemClass::Armor,    ItemCursorGraphic::ArkainesValor,           42000,        0,        0,   25,      ItemSpecialEffect::FastestHitRecovery,       ItemMiscID::Unique,                 SpellID::Null,         0,            0,           39,       40,       0,         0,      0,       0,       0,       0,      10,      0,      0,      0,        0,      0,          0,         -3,         0,           0,     7,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::ArkainesValor          },
	{ "Griswold's Edge",              ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::BroadSword,              42000,        4,       12,    0,                 GrisworldEdgeSpecialEffect,       ItemMiscID::Unique,                 SpellID::Null,         0,            0,           42,       44,       0,        25,      0,       0,       0,       0,       0,      0,      0,      0,     1280,  -1280,          0,          0,         0,           0,     8,         1,        10,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,            40,        0,        0,   ItemIndex::GriswoldsEdge          },
	{ "War Staff of haste",           ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::WarStaff,                40000,        8,       16,    0,           ItemSpecialEffect::FastestAttack,       ItemMiscID::Staff,                  SpellID::Null,         0,            0,           75,       75,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::FastAttack,         30,        0,        0,   ItemIndex::WarStaff               },
	{ "White Staff of Lightning",     ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::WarStaff,                 7160,        8,       16,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,                  SpellID::Lightning,   56,           56,           75,       75,       0,         0,      0,       0,       0,       0,       0,      0,      0,     13,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::MagicResistance,         ItemEffectType::Invalid,            30,       20,        0,   ItemIndex::WarStaff               },
	{ "Lightning Maul",               ItemType::Mace,              ItemClass::Weapon,   ItemCursorGraphic::Maul,                    11800,        6,       20,    0,         ItemSpecialEffect::LightningDamage,       ItemMiscID::None,                   SpellID::Null,         0,            0,           50,       50,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         2,        20,         ItemEffectType::LightningDamage,         ItemEffectType::Invalid,            55,        0,        0,   ItemIndex::Maul                   },
	{ "Ivory Great Axe of blood",     ItemType::Axe,               ItemClass::Weapon,   ItemCursorGraphic::GreatAxe,                31194,       12,       30,    0,              ItemSpecialEffect::StealLife5,       ItemMiscID::None,                   SpellID::Null,         0,            0,           75,       75,       0,         0,      0,       0,       0,       0,       0,      0,      0,     37,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::MagicResistance,         ItemEffectType::StealLife,          80,        0,        0,   ItemIndex::GreatAxe               },
	{ "Jade Crown of vim",            ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::Crown,                   19200,        0,        0,   10,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           18,       40,       0,         0,      0,       0,       0,       0,      14,     30,     30,     30,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::AllResistances,          ItemEffectType::Vitality,            0,        0,        0,   ItemIndex::Crown                  },
	{ "Short Bow of atrophy",         ItemType::Bow,               ItemClass::Weapon,   ItemCursorGraphic::ShortBow,                    1,        1,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           16,       30,       0,         0,      0,       0,       0,      -5,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::DexterityCurse,      0,        0,        0,   ItemIndex::ShortBow               },
	{ "Brass Dagger of weakness",     ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Dagger,                      1,        1,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           11,       16,       0,        -1,      0,      -5,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitCurse,              ItemEffectType::StrengthCurse,       0,        0,        0,   ItemIndex::Dagger                 },
	{ "Clumsy Short Bow",             ItemType::Bow,               ItemClass::Weapon,   ItemCursorGraphic::ShortBow,                    1,        1,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           16,       30,     -67,        -8,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercentCurse, ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::ShortBow               },
	{ "Tin Short Sword of the fool",  ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::ShortSword,                  1,        2,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           17,       24,       0,        -7,      0,       0,      -9,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitCurse,              ItemEffectType::MagicCurse,         18,        0,        0,   ItemIndex::ShortSword             },
	{ "Club of paralysis",            ItemType::Mace,              ItemClass::Weapon,   ItemCursorGraphic::Club,                        1,        1,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,            6,       20,       0,         0,      0,       0,       0,      -9,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::DexterityCurse,      0,        0,        0,   ItemIndex::Club                   },
	{ "Dull Staff of Lightning",      ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::ShortStaff,                  1,        2,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,                  SpellID::Lightning,   46,           46,           18,       25,     -28,        -1,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercentCurse, ItemEffectType::Invalid,             0,       20,        0,   ItemIndex::ShortStaff             },
	{ "Falchion of speed",            ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Falchion,                10000,        4,        8,    0,            ItemSpecialEffect::FasterAttack,       ItemMiscID::None,                   SpellID::Null,         0,            0,            6,       20,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::FastAttack,         30,        0,        0,   ItemIndex::Falchion               },
	{ "Bent Falchion",                ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Falchion,                    1,        4,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           14,       20,     -68,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercentCurse,      ItemEffectType::Invalid,            30,        0,        0,   ItemIndex::Falchion               },
	{ "Plentiful Staff of Firebolt",  ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::ShortStaff,               3040,        2,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,                  SpellID::Firebolt,    98,           98,           13,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Charges,                 ItemEffectType::Invalid,             0,       15,        0,   ItemIndex::ShortStaff             },
	{ "Dagger of illness",            ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Dagger,                      1,        1,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,            6,       16,       0,         0,      0,       0,       0,       0,      -8,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::VitalityCurse,       0,        0,        0,   ItemIndex::Dagger                 },
	{ "Cape of corruption",           ItemType::LightArmor,        ItemClass::Armor,    ItemCursorGraphic::Cape,                        1,        0,        0,    3,                  ItemSpecialEffect::NoMana,       ItemMiscID::None,                   SpellID::Null,         0,            0,            4,       12,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::NoMana,              0,        0,        0,   ItemIndex::Cape                   },
	{ "Sabre of trouble",             ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Sabre,                       1,        1,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           33,       45,       0,         0,      0,      -6,      -6,      -6,      -6,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::AllAttributesCurse, 17,        0,        0,   ItemIndex::Sabre                  },
	{ "Cap of tears",                 ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::Cap,                         1,        0,        0,    2,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,            5,       15,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          1,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::GetHitCurse,         0,        0,        0,   ItemIndex::Cap                    },
	{ "Tin Club",                     ItemType::Mace,              ItemClass::Weapon,   ItemCursorGraphic::Club,                        1,        1,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           14,       20,       0,        -8,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitCurse,              ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::Club                   },
	{ "Rusted Cape of dyslexia",      ItemType::LightArmor,        ItemClass::Armor,    ItemCursorGraphic::Cape,                        1,        0,        0,    2,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,            9,       12,       0,         0,    -34,       0,      -4,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ArmorClassPercentCurse,  ItemEffectType::MagicCurse,          0,        0,        0,   ItemIndex::Cape                   },
	{ "Cap of pain",                  ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::Cap,                         1,        0,        0,    2,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           10,       15,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          2,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::GetHitCurse,         0,        0,        0,   ItemIndex::Cap                    },
	{ "Clumsy Sabre of fragility",    ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Sabre,                       1,        1,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,            1,        1,     -75,       -10,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercentCurse, ItemEffectType::DurabilityCurse,    17,        0,        0,   ItemIndex::Sabre                  },
	{ "Vulnerable Cap of health",     ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::Cap,                       185,        0,        0,    1,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,            5,       15,       0,         0,    -63,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,         -1,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ArmorClassPercentCurse,  ItemEffectType::GetHit,              0,        0,        0,   ItemIndex::Cap                    },
	{ "Useless Dagger",               ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Dagger,                      1,        1,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,            5,       16,    -100,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercentCurse,      ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::Dagger                 },
	{ "Bountiful Staff of Fire Wall", ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::ShortStaff,               5970,        2,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,                  SpellID::FireWall,    36,           36,           17,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Charges,                 ItemEffectType::Invalid,             0,       27,        0,   ItemIndex::ShortStaff             },
	{ "Mace of frailty",              ItemType::Mace,              ItemClass::Weapon,   ItemCursorGraphic::Mace,                        1,        1,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           22,       32,       0,         0,      0,      -7,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::StrengthCurse,      16,        0,        0,   ItemIndex::Mace                   },
	{ "Weak Short Bow",               ItemType::Bow,               ItemClass::Weapon,   ItemCursorGraphic::ShortBow,                    1,        1,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           20,       30,     -44,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercentCurse,      ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::ShortBow               },
	{ "Short Staff of readiness",     ItemType::Staff,             ItemClass::Weapon,   ItemCursorGraphic::ShortStaff,               2060,        2,        4,    0,             ItemSpecialEffect::QuickAttack,       ItemMiscID::Staff,                  SpellID::Null,         0,            0,           12,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::FastAttack,          0,        0,        0,   ItemIndex::ShortStaff             },
	{ "Potion of Healing",            ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfHealing,              50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfHealing,          SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfHealing          },
	{ "Potion of Full Healing",       ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfFullHealing,         150,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfFullHealing,      SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfFullHealing      },
	{ "Potion of Mana",               ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfMana,                 50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfMana,             SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfMana             },
	{ "Potion of Full Mana",          ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::PotionOfFullMana,            150,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfFullMana,         SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PotionOfFullMana         },
	{ "Scroll of Town Portal",        ItemType::Misc,              ItemClass::Misc,     ItemCursorGraphic::Scroll,                    200,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Scroll,                 SpellID::TownPortal,   0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::ScrollOfTownPortal       },
	{ "The Rift Bow",                 ItemType::Bow,               ItemClass::Weapon,   ItemCursorGraphic::ShortBow,                 1800,        1,        4,    0,     ItemSpecialEffect::RandomArrowVelocity,       ItemMiscID::None,                   SpellID::Null,         0,            0,           30,       30,       0,         0,      0,       0,       0,      -3,       0,      0,      0,      0,        0,      0,          2,          0,         0,           0,    10,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::ShortBow               },
	{ "The Grandfather",              ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Grandfather,            119800,       10,       20,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,          100,      100,      70,        20,      0,       5,       5,       5,       5,      0,      0,      0,        0,   1280,          0,          0,         0,           0,    35,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,            75,        0,        0,   ItemIndex::GreatSword             },
	{ "Gothic Shield of thorns",      ItemType::Shield,            ItemClass::Armor,    ItemCursorGraphic::GothicShield,             5100,        0,        0,   18,                  ItemSpecialEffect::Thorns,       ItemMiscID::None,                   SpellID::Null,         0,            0,           17,       60,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Thorns,             80,        0,        0,   ItemIndex::GothicShield           },
	{ "Naj's Light Plate",            ItemType::HeavyArmor,        ItemClass::Armor,    ItemCursorGraphic::NajsLightPlate,          78700,        0,        0,   50,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           75,       75,       0,         0,      0,       0,       5,       0,       0,     20,     20,     20,     1280,      0,          0,          0,         0,           1,    77,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::PlateMail              },
	{ "Constricting Ring",            ItemType::Ring,              ItemClass::Misc,     ItemCursorGraphic::ConstrictingRing,        62000,        0,        0,    0,               ItemSpecialEffect::DrainLife,       ItemMiscID::Ring,                   SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,     75,     75,     75,        0,      0,          0,          0,         0,           0,    88,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,             0,        0,        0,   ItemIndex::Ring1                  },
	{ "Gotterdamerung",               ItemType::Helm,              ItemClass::Armor,    ItemCursorGraphic::VeilOfSteel,               54900,        0,        0,   60,          ItemSpecialEffect::ZeroResistance,       ItemMiscID::None,                   SpellID::Null,         0,            0,           60,       60,       0,         0,      0,      20,      20,      20,      20,      0,      0,      0,        0,      0,          0,         -4,        -4,           0,    67,         0,         0,         0,         0,         ItemEffectType::Invalid,                 ItemEffectType::Invalid,            50,        0,        0,   ItemIndex::GreatHelm              },
	{ "King's Bastard Sword",         ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::BastardSword,            69730,        6,       15,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           32,       60,     168,        95,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent,      ItemEffectType::Invalid,            50,        0,        0,   ItemIndex::BastardSword           },
	{ "Champion's Club of gore",      ItemType::Mace,              ItemClass::Weapon,   ItemCursorGraphic::Club,                    25576,        1,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           60,       60,     141,        75,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          9,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent,      ItemEffectType::DamageModifier,      0,        0,        0,   ItemIndex::Club                   },
	{ "Master's Flail",               ItemType::Mace,              ItemClass::Weapon,   ItemCursorGraphic::Flail,                   27340,        2,       12,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           60,       60,     123,        44,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent,      ItemEffectType::Invalid,            30,        0,        0,   ItemIndex::Flail                  },
	{ "Knight's Broad Sword",         ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::BroadSword,              27597,        4,       12,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           60,       60,     108,        37,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent,      ItemEffectType::Invalid,            40,        0,        0,   ItemIndex::BroadSword             },
	{ "Lord's Falchion of precision", ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::Falchion,                17025,        4,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           13,       20,      88,        23,      0,       0,       0,      20,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent,      ItemEffectType::Dexterity,          30,        0,        0,   ItemIndex::Falchion               },
	{ "Soldier's Sword of vigor",     ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::BastardSword,            31600,        6,       15,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           60,       60,      66,        19,      0,       0,       0,       0,      20,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent,      ItemEffectType::Vitality,           50,        0,        0,   ItemIndex::BastardSword           },
	{ "Fine Long Bow of the pit",     ItemType::Bow,               ItemClass::Weapon,   ItemCursorGraphic::HuntersBow,               2152,        1,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           35,       35,      49,        10,      0,      -2,      -2,      -2,      -2,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent,      ItemEffectType::AllAttributesCurse, 25,        0,       30,   ItemIndex::LongBow                },
	{ "Sharp Sword of atrophy",       ItemType::Sword,             ItemClass::Weapon,   ItemCursorGraphic::LongSword,                1958,        2,       10,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,                   SpellID::Null,         0,            0,           24,       40,      34,         4,      0,       0,       0,      -1,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent,      ItemEffectType::DexterityCurse,     30,        0,       30,   ItemIndex::LongSword              },
	{ "Emerald Bow of burning",       ItemType::Bow,               ItemClass::Weapon,   ItemCursorGraphic::LongWarBow,             107000,        1,       14,    0,              ItemSpecialEffect::FireArrows,       ItemMiscID::None,                   SpellID::Null,         0,            0,           60,       60,       0,         0,      0,       0,       0,       0,       0,     50,     50,     50,        0,      0,          0,          0,         0,           0,     0,         1,        16,         0,         0,         ItemEffectType::AllResistances,          ItemEffectType::FireArrows,         45,        0,       80,   ItemIndex::LongWarBow             },
	// clang-format on
};

class PackTest : public ::testing::Test {
public:
	void SetUp() override
	{
		Players.resize(1);
		MyPlayer = &Players[0];
	}
};

TEST_F(PackTest, UnPackItem_diablo)
{
	Item id;
	ItemPack is;

	gbIsHellfire = false;
	gbIsMultiplayer = false;
	gbIsSpawn = false;

	MyPlayer->_pMaxManaBase = 125 << 6;
	MyPlayer->_pMaxHPBase = 125 << 6;

	for (size_t i = 0; i < sizeof(PackedDiabloItems) / sizeof(*PackedDiabloItems); i++) {
		const ItemPack packed = SwappedLE(PackedDiabloItems[i]);
		UnPackItem(packed, *MyPlayer, id, false);
		CompareItems(id, DiabloItems[i]);

		PackItem(is, id, gbIsHellfire);
		ComparePackedItems(is, packed);
	}
}

TEST_F(PackTest, UnPackItem_diablo_unique_bug)
{
	const auto pkItemBug = SwappedLE(ItemPack { 6, 911, 14, 5, 60, 60, 0, 0, 0, 0 }); // Veil of Steel - with morph bug
	const auto pkItem = SwappedLE(ItemPack { 6, 655, 14, 5, 60, 60, 0, 0, 0, 0 });    // Veil of Steel - fixed

	gbIsHellfire = false;
	gbIsMultiplayer = false;
	gbIsSpawn = false;

	Item id;
	UnPackItem(pkItemBug, *MyPlayer, id, false);
	ASSERT_STREQ(id._iIName, "Veil of Steel");
	ASSERT_EQ(id._itype, ItemType::Helm);
	ASSERT_EQ(id._iClass, ItemClass::Armor);
	ASSERT_EQ(id._iCurs, ItemCursorGraphic::VeilOfSteel);
	ASSERT_EQ(id._iIvalue, 63800);
	ASSERT_EQ(id._iAC, 18);
	ASSERT_EQ(id._iMiscId, ItemMiscID::Unique);
	ASSERT_EQ(id._iPLAC, 60);
	ASSERT_EQ(id._iPLStr, 15);
	ASSERT_EQ(id._iPLVit, 15);
	ASSERT_EQ(id._iPLFR, 50);
	ASSERT_EQ(id._iPLLR, 50);
	ASSERT_EQ(id._iPLMR, 50);
	ASSERT_EQ(id._iPLMana, -1920);
	ASSERT_EQ(id._iPLLight, -2);
	ASSERT_EQ(id._iUid, 6);
	ASSERT_EQ(id.IDidx, ItemIndex::VeilOfSteel);

	ItemPack is;
	PackItem(is, id, gbIsHellfire);
	ComparePackedItems(is, pkItem);
}

const ItemPack PackedSpawnItems[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
	{ 2060036013,         257, 131,   0,   11,    25,  50,   50,      0,      0 }, // Short Staff of Firebolt
	{   81574809,         258,  94,   0,    0,     0,   0,    0,      0,      0 }, // Book of Holy Bolt
	// clang-format on
};

const TestItemStruct SpawnItems[] = {
	// clang-format off
	//_iIName,                   _itype,          _iClass,           _iCurs,                        _iIvalue, _iMinDam, _iMaxDam, _iAC,                 _iFlags,          _iMiscId,           _iSpell, _iCharges, _iMaxCharges, _iDurability, _iMaxDur, _iPLDam, _iPLToHit, _iPLAC, _iPLStr, _iPLMag, _iPLDex, _iPLVit, _iPLFR, _iPLLR, _iPLMR, _iPLMana, _iPLHP, _iPLDamMod, _iPLGetHit, _iPLLight, _iSplLvlAdd, _iUid, _iFMinDam, _iFMaxDam, _iLMinDam, _iLMaxDam,                      _iPrePower,              _iSufPower, _iMinStr, _iMinMag, _iMinDex,   IDidx                 );
	{ "Short Staff of Firebolt", ItemType::Staff, ItemClass::Weapon, ItemCursorGraphic::ShortStaff,        1,        2,        4,    0, ItemSpecialEffect::None, ItemMiscID::Staff, SpellID::Firebolt,        50,           50,           11,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid, ItemEffectType::Invalid,        0,       15,        0,   ItemIndex::ShortStaff },
	{ "Book of Holy Bolt",       ItemType::Misc,  ItemClass::Misc,   ItemCursorGraphic::BookGrey,       1000,        0,        0,    0, ItemSpecialEffect::None, ItemMiscID::Book,  SpellID::HolyBolt,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid, ItemEffectType::Invalid,        0,       20,        0,   ItemIndex::Book1      },
	// clang-format on
};

TEST_F(PackTest, UnPackItem_spawn)
{
	Item id;
	ItemPack is;

	gbIsHellfire = false;
	gbIsMultiplayer = false;
	gbIsSpawn = true;

	MyPlayer->_pMaxManaBase = 125 << 6;
	MyPlayer->_pMaxHPBase = 125 << 6;

	for (size_t i = 0; i < sizeof(PackedSpawnItems) / sizeof(*PackedSpawnItems); i++) {
		const ItemPack packed = SwappedLE(PackedSpawnItems[i]);
		UnPackItem(packed, *MyPlayer, id, false);
		CompareItems(id, SpawnItems[i]);

		PackItem(is, id, gbIsHellfire);
		ComparePackedItems(is, packed);
	}
}

const ItemPack PackedDiabloMPItems[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
	{  309674341,         193, 109,   0,    0,     0,   0,    0,      0,      0 }, // Book of Firebolt
	{ 1291471654,           6,  34,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Resurrect
	{ 1580941742,         385,  24,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Healing
	{  467997257,         388,  27,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Town Portal
	{  796933756,         385,  25,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Mana
	// clang-format on
};

const TestItemStruct DiabloMPItems[] = {
	// clang-format off
	//_iIName,                 _itype,         _iClass,         _iCurs,                           _iIvalue, _iMinDam, _iMaxDam, _iAC, _iFlags,                 _iMiscId,                   _iSpell,           _iCharges, _iMaxCharges, _iDurability, _iMaxDur, _iPLDam, _iPLToHit, _iPLAC, _iPLStr, _iPLMag, _iPLDex, _iPLVit, _iPLFR, _iPLLR, _iPLMR, _iPLMana, _iPLHP, _iPLDamMod, _iPLGetHit, _iPLLight, _iSplLvlAdd, _iUid, _iFMinDam, _iFMaxDam, _iLMinDam, _iLMaxDam,         _iPrePower,              _iSufPower,              _iMinStr, _iMinMag, _iMinDex,   IDidx                       );
	{ "Book of Firebolt",      ItemType::Misc, ItemClass::Misc, ItemCursorGraphic::BookRed,           1000,        0,        0,    0, ItemSpecialEffect::None, ItemMiscID::Book,           SpellID::Firebolt,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid, ItemEffectType::Invalid,        0,       15,        0,   ItemIndex::Book1            },
	{ "Scroll of Resurrect",   ItemType::Misc, ItemClass::Misc, ItemCursorGraphic::Scroll,             250,        0,        0,    0, ItemSpecialEffect::None, ItemMiscID::ScrollTargeted, SpellID::Resurrect,        0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid, ItemEffectType::Invalid,        0,        0,        0,   ItemIndex::ScrollOfResurrect  },
	{ "Potion of Healing",     ItemType::Misc, ItemClass::Misc, ItemCursorGraphic::PotionOfHealing,       50,        0,        0,    0, ItemSpecialEffect::None, ItemMiscID::PotionOfHealing,  SpellID::Null,             0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid, ItemEffectType::Invalid,        0,        0,        0,   ItemIndex::PotionOfHealing    },
	{ "Scroll of Town Portal", ItemType::Misc, ItemClass::Misc, ItemCursorGraphic::Scroll,             200,        0,        0,    0, ItemSpecialEffect::None, ItemMiscID::Scroll,         SpellID::TownPortal,       0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid, ItemEffectType::Invalid,        0,        0,        0,   ItemIndex::ScrollOfTownPortal },
	{ "Potion of Mana",        ItemType::Misc, ItemClass::Misc, ItemCursorGraphic::PotionOfMana,          50,        0,        0,    0, ItemSpecialEffect::None, ItemMiscID::PotionOfMana,     SpellID::Null,             0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid, ItemEffectType::Invalid,        0,        0,        0,   ItemIndex::PotionOfMana       },

	// clang-format on
};

TEST_F(PackTest, UnPackItem_diablo_multiplayer)
{
	Item id;
	ItemPack is;

	gbIsHellfire = false;
	gbIsMultiplayer = true;
	gbIsSpawn = false;

	MyPlayer->_pMaxManaBase = 125 << 6;
	MyPlayer->_pMaxHPBase = 125 << 6;

	for (size_t i = 0; i < sizeof(PackedDiabloMPItems) / sizeof(*PackedDiabloMPItems); i++) {
		const ItemPack packed = SwappedLE(PackedDiabloMPItems[i]);
		UnPackItem(packed, *MyPlayer, id, false);
		CompareItems(id, DiabloMPItems[i]);

		PackItem(is, id, gbIsHellfire);
		ComparePackedItems(is, packed);
	}
}

const ItemPack PackedHellfireItems[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
	{ 1717442367,         266, 156,   3,    0,     0,   0,    0,      0,      0 }, // Ring of stability
	{ 1268518156,         338, 157,   3,    0,     0,   0,    0,      0,      0 }, // Ring of precision
	{  132733863,         283, 157,   3,    0,     0,   0,    0,      0,      0 }, // Obsidian Ring of wizardry
	{  511953594,         283, 158,   3,    0,     0,   0,    0,      0,      0 }, // Ring of precision
	{ 1183326923,         346, 160,   3,    0,     0,   0,    0,      0,      0 }, // Amulet of titans
	{ 1863009736,         280, 160,   3,    0,     0,   0,    0,      0,      0 }, // Gold Amulet
	{ 1872860650,         734, 135,   5,   75,    75,   0,    0,      0,      0 }, // Messerschmidt's Reaver
	{ 1584694222,         281, 142,   3,  127,   128,   0,    0,      0,      0 }, // Vicious Maul of structure
	{  669112929,         280, 119,   0,   15,    24,   0,    0,      0,      0 }, // Short Sword
	{  303108965,         280, 148,   3,   18,    50,   0,    0,      0,      0 }, // Long Battle Bow of shock
	{  575830996,         257, 143,   3,   30,    30,   0,    0,      0,      0 }, // Short Bow of magic
	{ 1488880650,         194, 152,   3,   35,    35,  22,   33,      0,      0 }, // Red Long Staff of Healing
	{ 1864450901,         263,  71,   0,    6,    16,   0,    0,      0,      0 }, // Buckler
	{   28387651,         263,  49,   0,   15,    20,   0,    0,      0,      0 }, // Skull Cap
	{ 1298183212,         257,  55,   0,    6,     6,   0,    0,      0,      0 }, // Rags
	{ 1113945523,         260,  58,   0,   30,    30,   0,    0,      0,      0 }, // Quilted Armor
	{  765757608,         260,  58,   2,   12,    30,   0,    0,      0,      0 }, // Quilted Armor of light
	{  188812770,         346,  67,   3,   75,    75,   0,    0,      0,      0 }, // Saintly Plate of the stars
	{  283577043,        2070,  67,   3,   63,    75,   0,    0,      0,      0 }, // Plate Mail of the stars
	{  123272767,          16,  24,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Healing
	{  433688373,          16,  29,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Full Healing
	{ 1213385484,       32770,  25,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Mana
	{ 1405075219,         280, 110,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Golem
	{ 1478792102,         259,  92,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Search
	{ 1569255955,         262,  94,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Identify
	{ 1291205782,         261,  98,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Town Portal
	{  811925807,         260,  91,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Healing
	{ 1275007287,         257, 161,   0,    0,     0,   0,    0,      0,      0 }, // Rune of Fire
	{  561216242,         278,   0,   0,    0,     0,   0,    0,   1663,      0 }, // Gold
	{          1,         515,   7,   5,   45,    50,   0,    0,      0,      0 }, // The Undead Crown
	{          2,         774,   8,   5,    0,     0,   0,    0,      0,      0 }, // Empyrean Band
	{          4,         769,  11,   5,    0,     0,   0,    0,      0,      0 }, // Ring of Truth
	{          8,         512,  31,   5,   50,    50,   0,    0,      0,      0 }, // Griswold's Edge
	{          9,         850,  32,   5,  255,   255,   0,    0,      0,      0 }, // Bovine Plate
	{  410929431,         258, 114,   0,    0,     0,   0,    0,      0,      0 }, // Book of Healing
	{  876535546,         258, 114,   0,    0,     0,   0,    0,      0,      0 }, // Book of Charged Bolt
	{ 1009350361,         258, 114,   0,    0,     0,   0,    0,      0,      0 }, // Book of Firebolt
	{   41417651,         258,  83,   0,    0,     0,   0,    0,      0,      0 }, // Blacksmith Oil
	{  132200437,         258,  84,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Accuracy
	{  385651490,         257,  85,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Sharpness
	{ 1154514759,         290,  86,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Permanence
	{ 2020998927,        2066, 131,   3,   23,    32,   0,    0,      0,      0 }, // Doppelganger's Axe
	{  581541889,        2067, 141,   3,   36,    36,   0,    0,      0,      0 }, // Flail of vampires
	{ 1069448901,         844, 157,   5,    0,     0,   0,    0,      0,      0 }, // Gladiator's Ring
	{ 1670063399,        2068, 155,   3,   75,    75,   0,    0,      0,      0 }, // Warrior's Staff of the moon
	{  342570085,        4114,  74,   3,  255,   255,   0,    0,      0,      0 }, // Kite Shield of the ages
	{ 1514523617,        2066, 139,   3,   20,    20,   0,    0,      0,      0 }, // Heavy Club of puncturing
	{  701987341,        8208, 114,   1,    0,     0,   0,    0,      0,      0 }, // Book of Lightning
	{  568338383,         196, 124,   3,   23,    45,   0,    0,      0,      0 }, // Jester's Sabre
	{ 1308277119,        2056,  72,   3,   24,    24,   0,    0,      0,      0 }, // Small Shield of blocking
	{          0,         512,   6,   5,   10,    10,   0,    0,      0,      0 }, // The Butcher's Cleaver
	{ 1621745295,        2057, 121,   3,   28,    28,   0,    0,      0,      0 }, // Scimitar of peril
	{  492619876,        2054, 132,   3,   12,    12,   0,    0,      0,      0 }, // Crystalline Large Axe
	{ 1859493982,        2053,  56,   3,   18,    18,   0,    0,      0,      0 }, // Red Cloak
	{ 1593032051,        2050, 136,   3,   32,    32,   0,    0,      0,      0 }, // Mace of decay
	{          4,         512,  11,   5,    0,     0,   0,    0,      0,      0 }, // Ring of Truth
	{ 1500728519,         260,  61,   3,   18,    45,   0,    0,      0,      0 }, // Red Armor of paralysis
	{  954183925,         261, 144,   3,   26,    40,   0,    0,      0,      0 }, // Bent Hunter's Bow
	{  438248055,         711, 136,   5,   32,    32,   0,    0,      0,      0 }, // Civerb's Cudgel
	{ 1133027011,         328, 139,   3,    8,    20,   0,    0,      0,      0 }, // Deadly Spiked Club
	{  224835143,         732, 144,   5,  255,   255,   0,    0,      0,      0 }, // Gnat Sting
	{ 1498080548,         796, 138,   5,  255,   255,   0,    0,      0,      0 }, // Thunderclap
	{ 1218409601,         792, 155,   5,   75,    75,  50,   50,      0,      0 }, // Rod of Onan
	{ 1228950066,         711, 146,   5,  255,   255,   0,    0,      0,      0 }, // Flambeau
	{  863852923,         709, 156,   5,    0,     0,   0,    0,      0,      0 }, // Ring of Thunder
	{   89183927,         716, 159,   5,    0,     0,   0,    0,      0,      0 }, // Acolyte's Amulet
	{  858625694,         796, 151,   5,   25,    25,  86,   86,      0,      0 }, // The Protector
	{  127653047,         734,  63,   5,   55,    55,   0,    0,      0,      0 }, // Bone Chain Armor
	{ 1282740811,         290,  86,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Permanence
	{ 1403842263,         858,  70,   5,   90,    90,   0,    0,      0,      0 }, // Demon Plate Armor
	{ 1543909415,         284,  86,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Fortitude
	{ 1572202402,         769, 157,   5,    0,     0,   0,    0,      0,      0 }, // Ring of Regha
	{ 1572202657,         257, 156,   5,    0,     0,   0,    0,      0,      0 }, // Bronze Ring of dexterity
	{ 1642077210,         264,  84,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Accuracy
	{ 2049461998,         388,  35,   0,    0,     0,   0,    0,      0,      0 }, // Blacksmith Oil
	{ 2054447852,        2050, 151,   3,   25,    25,   0,    0,      0,      0 }, // Spider's Staff of devastation
	{  257276810,         284,  86,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Hardening
	{   29449848,         267,  86,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Skill
	{  296008111,         282,  86,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Mastery
	{  521895255,         724, 146,   5,  255,   255,   0,    0,      0,      0 }, // Blitzen
	{  580426378,         279,  86,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Death
	{  626919077,         264,  85,   0,    0,     0,   0,    0,      0,      0 }, // Oil of Sharpness
	{  979073763,         267, 119,   3,   13,    13,   0,    0,      0,      0 }, // Crystalline Sword of the leech
	{ 1294354855,        8208, 153,   3,   45,    45,  14,   14,      0,      0 }, // Plentiful Staff of Mana Shield
	{  695065155,        2078, 155,   3,   75,    75,   0,    0,      0,      0 }, // King's War Staff
	{ 1100844414,         386,  25,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Mana
	{ 1944120644,         386,  27,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Town Portal
	{  525564945,         385,  25,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Mana
	// clang-format on
};

constexpr ItemSpecialEffect GnatStingSpecialEffect = ItemSpecialEffect::MultipleArrows | ItemSpecialEffect::QuickAttack;
constexpr ItemSpecialEffect ThunderclapSpecialEffect = ItemSpecialEffect::FireDamage | ItemSpecialEffect::LightningDamage;
constexpr ItemSpecialEffect ExplosiveArrows = ItemSpecialEffect::FireArrows | ItemSpecialEffect::LightningArrows;

const TestItemStruct HellfireItems[] = {
	// clang-format off
	//_iIName,                          _itype,                _iClass,           _iCurs,                                  _iIvalue, _iMinDam, _iMaxDam, _iAC,                                    _iFlags,       _iMiscId,                      _iSpell,       _iCharges, _iMaxCharges, _iDurability, _iMaxDur, _iPLDam, _iPLToHit, _iPLAC, _iPLStr, _iPLMag, _iPLDex, _iPLVit, _iPLFR, _iPLLR, _iPLMR, _iPLMana, _iPLHP, _iPLDamMod, _iPLGetHit, _iPLLight, _iSplLvlAdd, _iUid, _iFMinDam, _iFMaxDam, _iLMinDam, _iLMaxDam,         _iPrePower,                         _iSufPower,                  _iMinStr, _iMinMag, _iMinDex,   IDidx                          );
	{ "Ring of stability",              ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::Ring,                     8000,        0,        0,    0,       ItemSpecialEffect::FasterHitRecovery,       ItemMiscID::Ring,              SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::FastHitRecovery,    0,        0,        0,   ItemIndex::Ring1               },
	{ "Ring of precision",              ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::Ring,                    10200,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,              SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,      16,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Dexterity,          0,        0,        0,   ItemIndex::Ring2               },
	{ "Obsidian Ring of wizardry",      ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::Ring,                    56928,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,              SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,      27,       0,       0,     37,     37,     37,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::AllResistances,     ItemEffectType::Magic,              0,        0,        0,   ItemIndex::Ring2               },
	{ "Ring of precision",              ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::Ring,                    10200,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,              SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,      16,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Dexterity,          0,        0,        0,   ItemIndex::Ring3               },
	{ "Amulet of titans",               ItemType::Amulet,      ItemClass::Misc,   ItemCursorGraphic::Amulet,                  20896,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Amulet,            SpellID::Null,         0,            0,            0,        0,       0,         0,      0,      28,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Strength,           0,        0,        0,   ItemIndex::Amulet2             },
	{ "Gold Amulet",                    ItemType::Amulet,      ItemClass::Misc,   ItemCursorGraphic::Amulet,                  13692,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Amulet,            SpellID::Null,         0,            0,            0,        0,       0,        29,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHit,              ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Amulet2             },
	{ "Messerschmidt's Reaver",         ItemType::Axe,         ItemClass::Weapon, ItemCursorGraphic::MesserschmidtsReaver,    58000,       12,       30,    0,              ItemSpecialEffect::FireDamage,       ItemMiscID::None,              SpellID::Null,         0,            0,           75,       75,     200,         0,      0,       5,       5,       5,       5,      0,      0,      0,        0,  -3200,         15,          0,         0,           0,    44,         2,        12,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           80,        0,        0,   ItemIndex::GreatAxe            },
	{ "Vicious Maul of structure",      ItemType::Mace,        ItemClass::Weapon, ItemCursorGraphic::Maul,                    10489,        6,       20,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,          127,      128,      72,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercent,      ItemEffectType::Durability,        55,        0,        0,   ItemIndex::Maul                },
	{ "Short Sword",                    ItemType::Sword,       ItemClass::Weapon, ItemCursorGraphic::ShortSword,                120,        2,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           15,       24,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           18,        0,        0,   ItemIndex::ShortSword          },
	{ "Long Battle Bow of shock",       ItemType::Bow,         ItemClass::Weapon, ItemCursorGraphic::LongBattleBow,            8000,        1,       10,    0,         ItemSpecialEffect::LightningArrows,       ItemMiscID::None,              SpellID::Null,         0,            0,           18,       50,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         1,         6,         ItemEffectType::Invalid,            ItemEffectType::LightningArrows,   30,        0,       60,   ItemIndex::LongBattleBow       },
	{ "Short Bow of magic",             ItemType::Bow,         ItemClass::Weapon, ItemCursorGraphic::ShortBow,                  400,        1,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           30,       30,       0,         0,      0,       0,       1,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Magic,              0,        0,        0,   ItemIndex::ShortBow            },
	{ "Red Long Staff of Healing",      ItemType::Staff,       ItemClass::Weapon, ItemCursorGraphic::LongStaff,                1360,        4,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,             SpellID::Healing,     22,           33,           35,       35,       0,         0,      0,       0,       0,       0,       0,     10,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::FireResistance,     ItemEffectType::Invalid,            0,       17,        0,   ItemIndex::LongStaff           },
	{ "Buckler",                        ItemType::Shield,      ItemClass::Armor,  ItemCursorGraphic::Buckler,                    30,        0,        0,    5,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,            6,       16,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Buckler             },
	{ "Skull Cap",                      ItemType::Helm,        ItemClass::Armor,  ItemCursorGraphic::SkullCap,                   25,        0,        0,    3,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           15,       20,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::SkullCap            },
	{ "Rags",                           ItemType::LightArmor,  ItemClass::Armor,  ItemCursorGraphic::Rags,                        5,        0,        0,    4,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,            6,        6,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Rags                },
	{ "Quilted Armor",                  ItemType::LightArmor,  ItemClass::Armor,  ItemCursorGraphic::QuiltedArmor,              200,        0,        0,    7,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           30,       30,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::QuiltedArmor        },
	{ "Quilted Armor of light",         ItemType::LightArmor,  ItemClass::Armor,  ItemCursorGraphic::QuiltedArmor,             1150,        0,        0,   10,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           12,       30,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         2,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::LightRadius,        0,        0,        0,   ItemIndex::QuiltedArmor        },
	{ "Saintly Plate of the stars",     ItemType::HeavyArmor,  ItemClass::Armor,  ItemCursorGraphic::FieldPlate,             140729,        0,        0,   46,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           75,       75,       0,         0,    121,      10,      10,      10,      10,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ArmorClassPercent,  ItemEffectType::AllAttributes,     60,        0,        0,   ItemIndex::PlateMail           },
	{ "Plate Mail of the stars",        ItemType::HeavyArmor,  ItemClass::Armor,  ItemCursorGraphic::FieldPlate,              77800,        0,        0,   49,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           63,       75,       0,         0,      0,       8,       8,       8,       8,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::AllAttributes,     60,        0,        0,   ItemIndex::PlateMail           },
	{ "Potion of Healing",              ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::PotionOfHealing,              50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfHealing,     SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::PotionOfHealing       },
	{ "Potion of Full Healing",         ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::PotionOfFullHealing,         150,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfFullHealing, SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::PotionOfFullHealing   },
	{ "Potion of Mana",                 ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::PotionOfMana,                 50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfMana,        SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::PotionOfMana          },
	{ "Scroll of Golem",                ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Scroll,                   1100,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::ScrollTargeted,    SpellID::Golem,        0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,       51,        0,   ItemIndex::ScrollOfGolem         },
	{ "Scroll of Search",               ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Scroll,                     50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Scroll,            SpellID::Search,       0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::ScrollOfSearch        },
	{ "Scroll of Identify",             ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Scroll,                    100,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Scroll,            SpellID::Identify,     0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::ScrollOfIdentify2     },
	{ "Scroll of Town Portal",          ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Scroll,                    200,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Scroll,            SpellID::TownPortal,   0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::ScrollOfTownPortal2   },
	{ "Scroll of Healing",              ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Scroll,                     50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Scroll,            SpellID::Healing,      0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::ScrollOfHealing       },
	{ "Rune of Fire",                   ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::RuneOfFire,                  100,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::RuneOfFire,          SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::RuneOfFire            },
	{ "Gold",                           ItemType::Gold,        ItemClass::Gold,   ItemCursorGraphic::GoldMedium,                  0,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Gold                },
	{ "The Undead Crown",               ItemType::Helm,        ItemClass::Armor,  ItemCursorGraphic::HelmOfSpirits,             16650,        0,        0,    8,         ItemSpecialEffect::RandomStealLife,       ItemMiscID::Unique,            SpellID::Null,         0,            0,           45,       50,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     1,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::UndeadCrown         },
	{ "Empyrean Band",                  ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::EmpyreanBand,             8000,        0,        0,    0,                  EmpyreanBandSpecialEffect,       ItemMiscID::Unique,            SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       2,       2,       2,       2,      0,      0,      0,        0,      0,          0,          0,         2,           0,     2,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::EmpyreanBand        },
	{ "Ring of Truth",                  ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::RingOfTruth,                9100,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Unique,            SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,     10,     10,     10,        0,    640,          0,         -1,         0,           0,     4,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::RingOfTruth           },
	{ "Griswold's Edge",                ItemType::Sword,       ItemClass::Weapon, ItemCursorGraphic::BroadSword,              42000,        4,       12,    0,                 GrisworldEdgeSpecialEffect,       ItemMiscID::Unique,            SpellID::Null,         0,            0,           50,       50,       0,        25,      0,       0,       0,       0,       0,      0,      0,      0,     1280,  -1280,          0,          0,         0,           0,     8,         1,        10,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           40,        0,        0,   ItemIndex::GriswoldsEdge       },
	{ "Bovine Plate",                   ItemType::HeavyArmor,  ItemClass::Armor,  ItemCursorGraphic::BovinePlate,               400,        0,        0,  150,                    ItemSpecialEffect::None,       ItemMiscID::Unique,            SpellID::Null,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,     30,     30,     30,    -3200,      0,          0,          0,         5,          -2,     9,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           50,        0,        0,   ItemIndex::BovinePlate         },
	{ "Book of Healing",                ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::BookGrey,                 1000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Book,              SpellID::Healing,      0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,       17,        0,   ItemIndex::Book1               },
	{ "Book of Charged Bolt",           ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::BookBlue,                 1000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Book,              SpellID::ChargedBolt,  0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,       25,        0,   ItemIndex::Book1               },
	{ "Book of Firebolt",               ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::BookRed,                  1000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Book,              SpellID::Firebolt,     0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,       15,        0,   ItemIndex::Book1               },
	{ "Blacksmith Oil",                 ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                       100,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilBlacksmith,     SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::BlacksmithOil       },
	{ "Oil of Accuracy",                ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                       500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfAccuracy,       SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::OilOfAccuracy         },
	{ "Oil of Sharpness",               ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                       500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfSharpness,      SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::OilOfSharpness        },
	{ "Oil of Permanence",              ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                     15000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfPermanence,     SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Oil2                },
	{ "Doppelganger's Axe",             ItemType::Axe,         ItemClass::Weapon, ItemCursorGraphic::Axe,                      6640,        4,       12,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           23,       32,      86,        26,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Doppelganger,       ItemEffectType::Invalid,           22,        0,        0,   ItemIndex::Axe                 },
	{ "Flail of vampires",              ItemType::Mace,        ItemClass::Weapon, ItemCursorGraphic::Flail,                   16500,        2,       12,    0,              ItemSpecialEffect::StealMana5,       ItemMiscID::None,              SpellID::Null,         0,            0,           36,       36,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::StealMana,         30,        0,        0,   ItemIndex::Flail               },
	{ "Gladiator's Ring",               ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::GladiatorsRing,          10000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,              SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,     3200,  -3200,          0,          0,         0,           0,   109,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Ring2               },
	{ "Warrior's Staff of the moon",    ItemType::Staff,       ItemClass::Weapon, ItemCursorGraphic::WarStaff,                42332,        8,       16,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,             SpellID::Null,         0,            0,           75,       75,      54,        15,      0,       5,       5,       5,       5,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent, ItemEffectType::AllAttributes,     30,        0,        0,   ItemIndex::WarStaff            },
	{ "Kite Shield of the ages",        ItemType::Shield,      ItemClass::Armor,  ItemCursorGraphic::KiteShield,               2600,        0,        0,   10,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Indestructible,    50,        0,        0,   ItemIndex::KiteShield          },
	{ "Heavy Club of puncturing",       ItemType::Mace,        ItemClass::Weapon, ItemCursorGraphic::SpikedClub,               5239,        3,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           20,       20,      52,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercent,      ItemEffectType::TargetArmorClass,  18,        0,        0,   ItemIndex::SpikedClub          },
	{ "Book of Lightning",              ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::BookBlue,                 3000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Book,              SpellID::Lightning,    0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,       20,        0,   ItemIndex::Book1               },
	{ "Jester's Sabre",                 ItemType::Sword,       ItemClass::Weapon, ItemCursorGraphic::Sabre,                    1710,        1,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           23,       45,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Jesters,            ItemEffectType::Invalid,           17,        0,        0,   ItemIndex::Sabre               },
	{ "Small Shield of blocking",       ItemType::Shield,      ItemClass::Armor,  ItemCursorGraphic::SmallShield,              4360,        0,        0,    6,               ItemSpecialEffect::FastBlock,       ItemMiscID::None,              SpellID::Null,         0,            0,           24,       24,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::FastBlock,         25,        0,        0,   ItemIndex::SmallShield         },
	{ "The Butcher's Cleaver",          ItemType::Axe,         ItemClass::Weapon, ItemCursorGraphic::Cleaver,                  3650,        4,       24,    0,                    ItemSpecialEffect::None,       ItemMiscID::Unique,            SpellID::Null,         0,            0,           10,       10,       0,         0,      0,      10,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Cleaver             },
	{ "Scimitar of peril",              ItemType::Sword,       ItemClass::Weapon, ItemCursorGraphic::Scimitar,                  700,        3,        7,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           28,       28,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Peril,             23,        0,       23,   ItemIndex::Scimitar            },
	{ "Crystalline Large Axe",          ItemType::Axe,         ItemClass::Weapon, ItemCursorGraphic::LargeAxe,                 5250,        6,       16,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           12,       12,     280,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Crystalline,        ItemEffectType::Invalid,           30,        0,        0,   ItemIndex::LargeAxe            },
	{ "Red Cloak",                      ItemType::LightArmor,  ItemClass::Armor,  ItemCursorGraphic::Cloak,                     580,        0,        0,    3,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           18,       18,       0,         0,      0,       0,       0,       0,       0,     10,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::FireResistance,     ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Cloak               },
	{ "Mace of decay",                  ItemType::Mace,        ItemClass::Weapon, ItemCursorGraphic::Mace,                      600,        1,        8,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           32,       32,     232,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Decay,             16,        0,        0,   ItemIndex::Mace                },
	{ "Ring of Truth",                  ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::RingOfTruth,                9100,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Unique,            SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,     10,     10,     10,        0,    640,          0,         -1,         0,           0,     4,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::RingOfTruth           },
	{ "Red Armor of paralysis",         ItemType::LightArmor,  ItemClass::Armor,  ItemCursorGraphic::StuddedLeatherArmor,       800,        0,        0,   17,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           18,       45,       0,         0,      0,       0,       0,      -8,       0,     20,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::FireResistance,     ItemEffectType::DexterityCurse,    20,        0,        0,   ItemIndex::StuddedLeatherArmor },
	{ "Bent Hunter's Bow",              ItemType::Bow,         ItemClass::Weapon, ItemCursorGraphic::HuntersBow,                  1,        2,        5,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           26,       40,     -69,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercentCurse, ItemEffectType::Invalid,           20,        0,       35,   ItemIndex::HuntersBow          },
	{ "Civerb's Cudgel",                ItemType::Mace,        ItemClass::Weapon, ItemCursorGraphic::Mace,                     2000,        1,        8,    0,       ItemSpecialEffect::TripleDemonDamage,       ItemMiscID::None,              SpellID::Null,         0,            0,           32,       32,       0,         0,      0,       0,      -2,      -5,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,    47,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           16,        0,        0,   ItemIndex::Mace                },
	{ "Deadly Spiked Club",             ItemType::Mace,        ItemClass::Weapon, ItemCursorGraphic::SpikedClub,               1556,        3,        6,    0,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,            8,       20,      47,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::DamagePercent,      ItemEffectType::Invalid,           18,        0,        0,   ItemIndex::SpikedClub          },
	{ "Gnat Sting",                     ItemType::Bow,         ItemClass::Weapon, ItemCursorGraphic::GnatSting,               30000,        1,        2,    0,                     GnatStingSpecialEffect,       ItemMiscID::None,              SpellID::Null,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,    98,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           20,        0,       35,   ItemIndex::HuntersBow          },
	{ "Thunderclap",                    ItemType::Mace,        ItemClass::Weapon, ItemCursorGraphic::Thunderclap,             30000,        5,        9,    0,                   ThunderclapSpecialEffect,       ItemMiscID::None,              SpellID::Null,         0,            0,          255,      255,       0,         0,      0,      20,       0,       0,       0,      0,     30,      0,        0,      0,          0,          0,         2,           0,   102,         3,         6,         2,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           40,        0,        0,   ItemIndex::WarHammer           },
	{ "Rod of Onan",                    ItemType::Staff,       ItemClass::Weapon, ItemCursorGraphic::WarStaff,                44167,        8,       16,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,             SpellID::Golem,       50,           50,           75,       75,     100,         0,      0,       5,       5,       5,       5,      0,      0,      0,        0,      0,          0,          0,         0,           0,    62,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           30,        0,        0,   ItemIndex::WarStaff            },
	{ "Flambeau",                       ItemType::Bow,         ItemClass::Weapon, ItemCursorGraphic::Flambeau,                30000,        0,        0,    0,                            ExplosiveArrows,       ItemMiscID::None,              SpellID::Null,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,    99,        15,        20,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           25,        0,       40,   ItemIndex::CompositeBow        },
	{ "Ring of Thunder",                ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::RingOfThunder,              8000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,              SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,    -30,     60,    -30,        0,      0,          0,          0,         0,           0,    96,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Ring1               },
	{ "Acolyte's Amulet",               ItemType::Amulet,      ItemClass::Misc,   ItemCursorGraphic::AcolytesAmulet,          10000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Amulet,            SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,    -3968,   3968,          0,          0,         0,           0,   108,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Amulet1             },
	{ "The Protector",                  ItemType::Staff,       ItemClass::Weapon, ItemCursorGraphic::Protector,               17240,        2,        4,   40,                  ItemSpecialEffect::Thorns,       ItemMiscID::Staff,             SpellID::Healing,     86,           86,           25,       25,       0,         0,      0,       0,       0,       0,       5,      0,      0,      0,        0,      0,          0,         -5,         0,           0,    59,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::ShortStaff          },
	{ "Bone Chain Armor",               ItemType::MediumArmor, ItemClass::Armor,  ItemCursorGraphic::BoneChainArmor,          36000,        0,        0,   40,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           55,       55,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,   106,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           30,        0,        0,   ItemIndex::ChainMail           },
	{ "Oil of Permanence",              ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                     15000,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfPermanence,     SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Oil2                },
	{ "Demon Plate Armor",              ItemType::HeavyArmor,  ItemClass::Armor,  ItemCursorGraphic::DemonPlateArmor,         80000,        0,        0,   80,                    ItemSpecialEffect::None,       ItemMiscID::None,              SpellID::Null,         0,            0,           90,       90,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,   107,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           90,        0,        0,   ItemIndex::FullPlateMail       },
	{ "Oil of Fortitude",               ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                      2500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfFortitude,      SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Oil2                },
	{ "Ring of Regha",                  ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::RingOfRegha,                4175,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,              SpellID::Null,         0,            0,            0,        0,       0,         0,      0,      -3,      10,      -3,       0,      0,      0,     10,        0,      0,          0,          0,         1,           0,    86,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Ring2               },
	{ "Bronze Ring of dexterity",       ItemType::Ring,        ItemClass::Misc,   ItemCursorGraphic::Ring,                     5200,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Ring,              SpellID::Null,         0,            0,            0,        0,       0,         4,      0,       0,       0,       4,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHit,              ItemEffectType::Dexterity,          0,        0,        0,   ItemIndex::Ring1               },
	{ "Oil of Accuracy",                ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                       500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfAccuracy,       SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::OilOfAccuracy         },
	{ "Blacksmith Oil",                 ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                       100,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilBlacksmith,     SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Oil                 },
	{ "Spider's Staff of devastation",  ItemType::Staff,       ItemClass::Weapon, ItemCursorGraphic::ShortStaff,               2050,        2,        4,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,             SpellID::Null,         0,            0,           25,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,      768,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Mana,               ItemEffectType::Devastation,        0,        0,        0,   ItemIndex::ShortStaff          },
	{ "Oil of Hardening",               ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                       500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfHardening,      SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Oil2                },
	{ "Oil of Skill",                   ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                      1500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfSkill,          SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Oil2                },
	{ "Oil of Mastery",                 ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                      2500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfMastery,        SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Oil2                },
	{ "Blitzen",                        ItemType::Bow,         ItemClass::Weapon, ItemCursorGraphic::Blitzen,                 30000,        0,        0,    0,                            ExplosiveArrows,       ItemMiscID::None,              SpellID::Null,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,   101,        10,        15,         1,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,           25,        0,       40,   ItemIndex::CompositeBow        },
	{ "Oil of Death",                   ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                      2500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfDeath,          SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::Oil2                },
	{ "Oil of Sharpness",               ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Oil,                       500,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::OilOfSharpness,      SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::OilOfSharpness        },
	{ "Crystalline Sword of the leech", ItemType::Sword,       ItemClass::Weapon, ItemCursorGraphic::ShortSword,              10020,        2,        6,    0,              ItemSpecialEffect::StealLife3,       ItemMiscID::None,              SpellID::Null,         0,            0,           13,       13,     232,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Crystalline,         ItemEffectType::StealLife,        18,        0,        0,   ItemIndex::ShortSword          },
	{ "Plentiful Staff of Mana Shield", ItemType::Staff,       ItemClass::Weapon, ItemCursorGraphic::CompositeStaff,           6360,        5,       10,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,             SpellID::ManaShield,  14,           14,           45,       45,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Charges,            ItemEffectType::Invalid,            0,       25,        0,   ItemIndex::CompositeStaff      },
	{ "King's War Staff",               ItemType::Staff,       ItemClass::Weapon, ItemCursorGraphic::WarStaff,                92000,        8,       16,    0,                    ItemSpecialEffect::None,       ItemMiscID::Staff,             SpellID::Null,         0,            0,           75,       75,     175,        76,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::ToHitDamagePercent, ItemEffectType::Invalid,           30,        0,        0,   ItemIndex::WarStaff            },
	{ "Potion of Mana",                 ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::PotionOfMana,                 50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfMana,        SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::PotionOfMana          },
	{ "Scroll of Town Portal",          ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::Scroll,                    200,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::Scroll,            SpellID::TownPortal,   0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::ScrollOfTownPortal    },
	{ "Potion of Mana",                 ItemType::Misc,        ItemClass::Misc,   ItemCursorGraphic::PotionOfMana,                 50,        0,        0,    0,                    ItemSpecialEffect::None,       ItemMiscID::PotionOfMana,        SpellID::Null,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         ItemEffectType::Invalid,            ItemEffectType::Invalid,            0,        0,        0,   ItemIndex::PotionOfMana          },
	// clang-format on
};

TEST_F(PackTest, UnPackItem_hellfire)
{
	Item id;
	ItemPack is;

	gbIsHellfire = true;
	gbIsMultiplayer = false;
	gbIsSpawn = false;

	MyPlayer->_pMaxManaBase = 125 << 6;
	MyPlayer->_pMaxHPBase = 125 << 6;

	for (size_t i = 0; i < sizeof(PackedHellfireItems) / sizeof(*PackedHellfireItems); i++) {
		const ItemPack packed = SwappedLE(PackedHellfireItems[i]);
		UnPackItem(packed, *MyPlayer, id, true);
		CompareItems(id, HellfireItems[i]);

		PackItem(is, id, gbIsHellfire);
		is.dwBuff &= ~CF_HELLFIRE;
		ComparePackedItems(is, packed);
	}
}

TEST_F(PackTest, UnPackItem_diablo_strip_hellfire_items)
{
	const auto is = SwappedLE(ItemPack { 1478792102, 259, 92, 0, 0, 0, 0, 0, 0, 0 }); // Scroll of Search
	Item id;

	gbIsHellfire = false;
	gbIsMultiplayer = false;
	gbIsSpawn = false;

	UnPackItem(is, *MyPlayer, id, true);

	ASSERT_EQ(id._itype, ItemType::None);
}

TEST_F(PackTest, UnPackItem_empty)
{
	const auto is = SwappedLE(ItemPack { 0, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0 });
	Item id;

	UnPackItem(is, *MyPlayer, id, false);

	ASSERT_EQ(id._itype, ItemType::None);
}

TEST_F(PackTest, PackItem_empty)
{
	ItemPack is;
	Item id = {};

	id._itype = ItemType::None;

	PackItem(is, id, gbIsHellfire);

	// Copy the value out before comparing to avoid loading a misaligned address.
	const auto idx = is.idx;
	ASSERT_EQ(SDL_SwapLE16(idx), 0xFFFF);
}

static void compareGold(const ItemPack &is, ItemCursorGraphic iCurs)
{
	Item id;
	UnPackItem(is, *MyPlayer, id, false);
	ASSERT_EQ(id._iCurs, iCurs);
	ASSERT_EQ(id.IDidx, ItemIndex::Gold);
	// Copy the value out before comparing to avoid loading a misaligned address.
	const auto wvalue = SDL_SwapLE16(is.wValue);
	ASSERT_EQ(id._ivalue, wvalue);
	ASSERT_EQ(id._itype, ItemType::Gold);
	ASSERT_EQ(id._iClass, ItemClass::Gold);

	ItemPack is2;
	PackItem(is2, id, gbIsHellfire);
	ComparePackedItems(is, is2);
}

TEST_F(PackTest, UnPackItem_gold_small)
{
	const auto is = SwappedLE(ItemPack { 0, 0, static_cast<int16_t>(ItemIndex::Gold), 0, 0, 0, 0, 0, 1000, 0 });
	compareGold(is, ItemCursorGraphic::GoldSmall);
}

TEST_F(PackTest, UnPackItem_gold_medium)
{
	const auto is = SwappedLE(ItemPack { 0, 0, static_cast<int16_t>(ItemIndex::Gold), 0, 0, 0, 0, 0, 1001, 0 });
	compareGold(is, ItemCursorGraphic::GoldMedium);
}

TEST_F(PackTest, UnPackItem_gold_large)
{
	const auto is = SwappedLE(ItemPack { 0, 0, static_cast<int16_t>(ItemIndex::Gold), 0, 0, 0, 0, 0, 2500, 0 });
	compareGold(is, ItemCursorGraphic::GoldLarge);
}

TEST_F(PackTest, UnPackItem_ear)
{
	const auto is = SwappedLE(ItemPack { 1633955154, 17509, 23, 111, 103, 117, 101, 68, 19843, 0 });
	Item id;

	UnPackItem(is, *MyPlayer, id, false);
	ASSERT_STREQ(id._iName, "Ear of Dead-RogueDM");
	ASSERT_EQ(id._ivalue, 3);

	ItemPack is2;
	PackItem(is2, id, gbIsHellfire);
	ComparePackedItems(is, is2);
}

} // namespace
} // namespace devilution
