#include <gtest/gtest.h>
#include "all.h"

using namespace dvl;

static void ComparePackedItems(const PkItemStruct *item1, const PkItemStruct *item2)
{
	ASSERT_EQ(item1->iSeed, item2->iSeed);
	ASSERT_EQ(item1->iCreateInfo, item2->iCreateInfo);
	ASSERT_EQ(item1->idx, item2->idx);
	ASSERT_EQ(item1->bId, item2->bId);
	ASSERT_EQ(item1->bDur, item2->bDur);
	ASSERT_EQ(item1->bMDur, item2->bMDur);
	ASSERT_EQ(item1->bCh, item2->bCh);
	ASSERT_EQ(item1->bMCh, item2->bMCh);
	ASSERT_EQ(item1->wValue, item2->wValue);
	ASSERT_EQ(item1->dwBuff, item2->dwBuff);
}
typedef struct TestItemStruct {
	char _iIName[64];
	Sint32 _itype;
	Sint32 _iClass;
	Sint32 _iCurs;
	Sint32 _iIvalue;
	Sint32 _iMinDam;
	Sint32 _iMaxDam;
	Sint32 _iAC;
	Sint32 _iFlags;
	Sint32 _iMiscId;
	Sint32 _iSpell;
	Sint32 _iCharges;
	Sint32 _iMaxCharges;
	Sint32 _iDurability;
	Sint32 _iMaxDur;
	Sint32 _iPLDam;
	Sint32 _iPLToHit;
	Sint32 _iPLAC;
	Sint32 _iPLStr;
	Sint32 _iPLMag;
	Sint32 _iPLDex;
	Sint32 _iPLVit;
	Sint32 _iPLFR;
	Sint32 _iPLLR;
	Sint32 _iPLMR;
	Sint32 _iPLMana;
	Sint32 _iPLHP;
	Sint32 _iPLDamMod;
	Sint32 _iPLGetHit;
	Sint32 _iPLLight;
	Sint8 _iSplLvlAdd;
	Sint32 _iUid;
	Sint32 _iFMinDam;
	Sint32 _iFMaxDam;
	Sint32 _iLMinDam;
	Sint32 _iLMaxDam;
	Sint8 _iPrePower;
	Sint8 _iSufPower;
	Sint8 _iMinStr;
	Uint8 _iMinMag;
	Sint8 _iMinDex;
	Sint32 IDidx;
} TestItemStruct;

static void CompareItems(const ItemStruct *item1, const TestItemStruct *item2)
{
	ASSERT_STREQ(item1->_iIName, item2->_iIName);
	ASSERT_EQ(item1->_itype, item2->_itype);
	ASSERT_EQ(item1->_iClass, item2->_iClass);
	ASSERT_EQ(item1->_iCurs, item2->_iCurs);
	ASSERT_EQ(item1->_iIvalue, item2->_iIvalue);
	ASSERT_EQ(item1->_iMinDam, item2->_iMinDam);
	ASSERT_EQ(item1->_iMaxDam, item2->_iMaxDam);
	ASSERT_EQ(item1->_iAC, item2->_iAC);
	ASSERT_EQ(item1->_iFlags, item2->_iFlags);
	ASSERT_EQ(item1->_iMiscId, item2->_iMiscId);
	ASSERT_EQ(item1->_iSpell, item2->_iSpell);
	ASSERT_EQ(item1->_iCharges, item2->_iCharges);
	ASSERT_EQ(item1->_iMaxCharges, item2->_iMaxCharges);
	ASSERT_EQ(item1->_iDurability, item2->_iDurability);
	ASSERT_EQ(item1->_iMaxDur, item2->_iMaxDur);
	ASSERT_EQ(item1->_iPLDam, item2->_iPLDam);
	ASSERT_EQ(item1->_iPLToHit, item2->_iPLToHit);
	ASSERT_EQ(item1->_iPLAC, item2->_iPLAC);
	ASSERT_EQ(item1->_iPLStr, item2->_iPLStr);
	ASSERT_EQ(item1->_iPLMag, item2->_iPLMag);
	ASSERT_EQ(item1->_iPLDex, item2->_iPLDex);
	ASSERT_EQ(item1->_iPLVit, item2->_iPLVit);
	ASSERT_EQ(item1->_iPLFR, item2->_iPLFR);
	ASSERT_EQ(item1->_iPLLR, item2->_iPLLR);
	ASSERT_EQ(item1->_iPLMR, item2->_iPLMR);
	ASSERT_EQ(item1->_iPLMana, item2->_iPLMana);
	ASSERT_EQ(item1->_iPLHP, item2->_iPLHP);
	ASSERT_EQ(item1->_iPLDamMod, item2->_iPLDamMod);
	ASSERT_EQ(item1->_iPLGetHit, item2->_iPLGetHit);
	ASSERT_EQ(item1->_iPLLight, item2->_iPLLight);
	ASSERT_EQ(item1->_iSplLvlAdd, item2->_iSplLvlAdd);
	ASSERT_EQ(item1->_iUid, item2->_iUid);
	ASSERT_EQ(item1->_iFMinDam, item2->_iFMinDam);
	ASSERT_EQ(item1->_iFMaxDam, item2->_iFMaxDam);
	ASSERT_EQ(item1->_iLMinDam, item2->_iLMinDam);
	ASSERT_EQ(item1->_iLMaxDam, item2->_iLMaxDam);
	ASSERT_EQ(item1->_iPrePower, item2->_iPrePower);
	ASSERT_EQ(item1->_iSufPower, item2->_iSufPower);
	ASSERT_EQ(item1->_iMinStr, item2->_iMinStr);
	ASSERT_EQ(item1->_iMinMag, item2->_iMinMag);
	ASSERT_EQ(item1->_iMinDex, item2->_iMinDex);
	ASSERT_EQ(item1->IDidx, item2->IDidx);
}

const PkItemStruct PackedDiabloItems[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
	{ 2082213289,       0x119,  53,   3,   60,    60,   0,    0,      0,      0 }, // Amber Helm of harmony
	{  338833725,       0x118, 154,   3,    0,     0,   0,    0,      0,      0 }, // Cobalt Amulet of giants
	{  545145866,       0x11A, 120,   3,   38,    40,   0,    0,      0,      0 }, // Brutal Sword of gore
	{ 1504248345,       0x35A,  70,   5,  255,   255,   0,    0,      0,      0 }, // Demonspike Coat
	{ 1884424756,       0x146, 151,   3,    0,     0,   0,    0,      0,      0 }, // Steel Ring of the jaguar
	{ 1712759905,        0xDC, 151,   3,    0,     0,   0,    0,      0,      0 }, // Ring of the heavens
	{  981777658,       0x11E, 153,   2,    0,     0,   0,    0,      0,      0 }, // Ring of sorcery
	{  844854815,       0x11A,  75,   3,  255,   255,   0,    0,      0,      0 }, // Shield of the ages
	{ 1151513535,       0x11E,  73,   2,   12,    32,   0,    0,      0,      0 }, // Sapphire Shield
	{  640243885,           6,  27,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Town Portal
	{  741806938,           9,  25,   0,    0,     0,   0,    0,      0,      0 }, // IDI_MANA
	{ 1456608333,         257,  79,   0,    0,     0,   0,    0,      0,      0 }, // Mana
	{  554676945,          16,  30,   0,    0,     0,   0,    0,      0,      0 }, // Full mana
	{  355389938,           0,  24,   0,    0,     0,   0,    0,      0,      0 }, // Healing
	{  868435486,          16,  29,   0,    0,     0,   0,    0,      0,      0 }, // Full healing
	{ 1372832903,           0,   4,   0,   30,    30,   0,    0,      0,      0 }, // IDI_ROGUE
	{  896239556,        2068,  53,   3,   56,    60,   0,    0,      0,      0 }, // Jade Helm of the wolf
	{ 1286225254,         269, 151,   3,    0,     0,   0,    0,      0,      0 }, // Steel Ring of accuracy
	{  548642293,         266,  21,   0,    0,     0,   0,    0,      0,      0 }, // Blood Stone
	{  307669016,         270, 151,   3,    0,     0,   0,    0,      0,      0 }, // Ring of power
	{  204766888,         332, 154,   3,    0,     0,   0,    0,      0,      0 }, // Gold Amulet of accuracy
	{ 1642744332,         273, 122,   3,   25,    60,   0,    0,      0,      0 }, // Sword of the bat
	{ 1031508271,        1036,  72,   0,   14,    24,   0,    0,      0,      0 }, // Small Shield
	{ 1384620228,         338,  65,   3,   44,    80,   0,    0,      0,      0 }, // Plate of giant
	{  681296530,         266,  87,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Healing
	{  109984285,       16390,  81,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Rejuvenation - made by Pepin
	{ 1857753366,         260,  81,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Rejuvenation
	{  965103261,         273,  82,   0,    0,     0,   0,    0,      0,      0 }, // Potion of Full Rejuvenation
	{  430621075,         217, 141,   3,   45,    45,   0,    0,      0,      0 }, // Savage Bow of perfection
	{ 1272669062,         258, 115,   0,   10,    20,   0,    0,      0,      0 }, // Falchion
	{ 1133884051,         278, 120,   2,   18,    40,   0,    0,      0,      0 }, // Sword of vim
	{ 1743897351,         259, 146,   2,   10,    25,  60,   60,      0,      0 }, // Frog's Staff of Holy Bolt
	{  429107209,           0,   5,   0,   25,    25,   9,   40,      0,      0 }, // IDI_SORCERER
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
	{  557339094,        8208, 150,   3,   75,    75,   0,    0,      0,      0 }, // Staff of haste
	{ 1684844665,        8208, 150,   3,   75,    75,  56,   56,      0,      0 }, // White Staff of Lightning
	{ 1297052552,        2074, 137,   3,   50,    50,   0,    0,      0,      0 }, // Lightning Maul
	{  981895960,        2073, 130,   3,   75,    75,   0,    0,      0,      0 }, // Ivory Axe of blood
	{  935416728,        2070,  52,   3,   18,    40,   0,    0,      0,      0 }, // Jade Crown of vim
	// clang-format on
};

const TestItemStruct DiabloItems[] = {
	// clang-format off
	//_iIName,                       _itype, _iClass, _iCurs, _iIvalue, _iMinDam, _iMaxDam, _iAC,   _iFlags, _iMiscId, _iSpell, _iCharges, _iMaxCharges, _iDurability, _iMaxDur, _iPLDam, _iPLToHit, _iPLAC, _iPLStr, _iPLMag, _iPLDex, _iPLVit, _iPLFR, _iPLLR, _iPLMR, _iPLMana, _iPLHP, _iPLDamMod, _iPLGetHit, _iPLLight, _iSplLvlAdd, _iUid, _iFMinDam, _iFMaxDam, _iLMinDam, _iLMaxDam, _iPrePower, _iSufPower, _iMinStr, _iMinMag, _iMinDex, IDidx );
	{ "Amber Helm of harmony",            7,       2,     98,    21100,        0,        0,   11,   8388608,        0,       0,         0,            0,           60,       60,       0,         0,      0,       0,       0,       0,       0,     18,     18,     18,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         11,         59,       50,        0,        0,    53 },
	{ "Cobalt Amulet of giants",         13,       3,     45,    26840,        0,        0,    0,         0,       26,       0,         0,            0,            0,        0,       0,         0,      0,      19,       0,       0,       0,      0,     46,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          9,         19,        0,        0,        0,   159 },
	{ "Brutal Sword of gore",             1,       1,     60,    13119,        2,       10,    0,         0,        0,       0,         0,            0,           38,       40,      91,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,         12,          0,         0,           0,     0,         0,         0,         0,         0,          2,         61,       30,        0,       30,   125 },
	{ "Demonspike Coat",                  9,       2,    151,   251175,        0,        0,  100,         0,        0,       0,         0,            0,          255,      255,       0,         0,      0,      10,       0,       0,       0,     50,      0,      0,        0,      0,          0,         -6,         0,           0,    78,         0,         0,         0,         0,         -1,         -1,       90,        0,        0,    70 },
	{ "Steel Ring of the jaguar",        12,       3,     12,    10600,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,        15,      0,       0,       0,       0,       0,      0,      0,      0,        0,   1024,          0,          0,         0,           0,     0,         0,         0,         0,         0,          0,         31,        0,        0,        0,   156 },
	{ "Ring of the heavens",             12,       3,     12,    37552,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,         0,      0,      14,      14,      14,      14,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         27,        0,        0,        0,   156 },
	{ "Ring of sorcery",                 12,       3,     12,    10200,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,         0,      0,       0,      16,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         21,        0,        0,        0,   158 },
	{ "Shield of the ages",               5,       2,    132,     4850,        0,        0,   18,         0,        0,       0,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         37,       60,        0,        0,    75 },
	{ "Sapphire Shield",                  5,       2,    147,    21000,        0,        0,    7,         0,        0,       0,         0,            0,           12,       32,       0,         0,      0,       0,       0,       0,       0,      0,     60,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          9,         -1,       40,        0,        0,    73 },
	{ "Scroll of Town Portal",            0,       3,      1,      200,        0,        0,    0,         0,       21,       7,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    27 },
	{ "Potion of Mana",                   0,       3,     39,       50,        0,        0,    0,         0,        6,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    25 },
	{ "Potion of Mana",                   0,       3,     39,       50,        0,        0,    0,         0,        6,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    79 },
	{ "Potion of Full Mana",              0,       3,      0,      150,        0,        0,    0,         0,        7,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    30 },
	{ "Potion of Healing",                0,       3,     32,       50,        0,        0,    0,         0,        3,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    24 },
	{ "Potion of Full Healing",           0,       3,     35,      150,        0,        0,    0,         0,        2,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    29 },
	{ "Short Bow",                        3,       1,    118,      100,        1,        4,    0,         0,        0,       0,         0,            0,           30,       30,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,     4 },
	{ "Jade Helm of the wolf",            7,       2,     98,    22310,        0,        0,   14,         0,        0,       0,         0,            0,           56,       60,       0,         0,      0,       0,       0,       0,       0,     27,     27,     27,        0,   2112,          0,          0,         0,           0,     0,         0,         0,         0,         0,         11,         31,       50,        0,        0,    53 },
	{ "Steel Ring of accuracy",          12,       3,     12,    13400,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,        14,      0,       0,       0,      15,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          0,         23,        0,        0,        0,   156 },
	{ "Blood Stone",                      0,       5,     25,        0,        0,        0,    0,         0,        0,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    21 },
	{ "Ring of power",                   12,       3,     12,     6400,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,         0,      0,      12,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         19,        0,        0,        0,   156 },
	{ "Gold Amulet of accuracy",         13,       3,     45,    20896,        0,        0,    0,         0,       26,       0,         0,            0,            0,        0,       0,        25,      0,       0,       0,      14,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          0,         23,        0,        0,        0,   159 },
	{ "Sword of the bat",                 1,       1,     57,    10500,        6,       15,    0,      8192,        0,       0,         0,            0,           25,       60,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         55,       50,        0,        0,   127 },
	{ "Small Shield",                     5,       2,    105,       90,        0,        0,    3,         0,        0,       0,         0,            0,           14,       24,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,       25,        0,        0,    72 },
	{ "Plate of giants",                  9,       2,    153,    23250,        0,        0,   20,         0,        0,       0,         0,            0,           44,       80,       0,         0,      0,      17,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         19,       40,        0,        0,    65 },
	{ "Scroll of Healing",                0,       3,      1,       50,        0,        0,    0,         0,       21,       2,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    91 },
	{ "Potion of Rejuvenation",           0,       3,     37,      120,        0,        0,    0,         0,       18,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    81 },
	{ "Potion of Rejuvenation",           0,       3,     37,      120,        0,        0,    0,         0,       18,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    81 },
	{ "Potion of Full Rejuvenation",      0,       3,     33,      600,        0,        0,    0,         0,       19,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    82 },
	{ "Savage Bow of perfection",         3,       1,    133,    23438,        3,        6,    0,         0,        0,       0,         0,            0,           45,       45,     117,         0,      0,       0,       0,      22,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          2,         23,       25,        0,       40,   146 },
	{ "Falchion",                         1,       1,     62,      250,        4,        8,    0,         0,        0,       0,         0,            0,           10,       20,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,       30,        0,        0,   120 },
	{ "Sword of vim",                     1,       1,     60,     4400,        2,       10,    0,         0,        0,       0,         0,            0,           18,       40,       0,         0,      0,       0,       0,       0,      15,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         25,       30,        0,       30,   125 },
	{ "Frog's Staff of Holy Bolt",       10,       1,    109,        1,        2,        4,    0,         0,       23,      31,        60,           60,           10,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,     -384,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         34,         -1,        0,       20,        0,   151 },
	{ "Short Staff of Charged Bolt",     10,       1,    109,      520,        2,        4,    0,         0,       23,      30,         9,           40,           25,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,       20,        0,   166 },
	{ "Staff of Charged Bolt",           10,       1,    109,        1,        2,        4,    0,         0,       23,      30,        50,           50,           18,       25,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,       25,        0,   151 },
	{ "Cap of the mind",                  7,       2,     91,     1845,        0,        0,    2,         0,        0,       0,         0,            0,           12,       15,       0,         0,      0,       0,       9,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         21,        0,        0,        0,    48 },
	{ "Armor of protection",              6,       2,    129,     1200,        0,        0,    7,         0,        0,       0,         0,            0,           30,       30,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,         -2,         0,           0,     0,         0,         0,         0,         0,         -1,         30,        0,        0,        0,    58 },
	{ "Empyrean Band",                   12,       3,     18,     8000,        0,        0,    0, 270532608,       27,       0,         0,            0,            0,        0,       0,         0,      0,       2,       2,       2,       2,      0,      0,      0,        0,      0,          0,          0,         2,           0,     2,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,     8 },
	{ "Optic Amulet",                    13,       3,     44,     9750,        0,        0,    0,         0,       27,       0,         0,            0,            0,        0,       0,         0,      0,       0,       5,       0,       0,      0,     20,      0,        0,      0,          0,         -1,         2,           0,     3,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    10 },
	{ "Ring of Truth",                   12,       3,     10,     9100,        0,        0,    0,         0,       27,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,     10,     10,     10,        0,    640,          0,         -1,         0,           0,     4,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    11 },
	{ "Harlequin Crest",                  7,       2,     81,     4000,        0,        0,   -3,         0,       27,       0,         0,            0,           15,       15,       0,         0,      0,       2,       2,       2,       2,      0,      0,      0,      448,    448,          0,         -1,         0,           0,     5,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    13 },
	{ "Veil of Steel",                    7,       2,     85,    63800,        0,        0,   18,         0,       27,       0,         0,            0,           60,       60,       0,         0,     60,      15,       0,       0,      15,     50,     50,     50,    -1920,      0,          0,          0,        -2,           0,     6,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    14 },
	{ "Arkaine's Valor",                  8,       2,    157,    42000,        0,        0,   25,   8388608,       27,       0,         0,            0,           39,       40,       0,         0,      0,       0,       0,       0,      10,      0,      0,      0,        0,      0,          0,         -3,         0,           0,     7,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    28 },
	{ "Griswold's Edge",                  1,       1,     61,    42000,        4,       12,    0,    264208,       27,       0,         0,            0,           42,       44,       0,        25,      0,       0,       0,       0,       0,      0,      0,      0,     1280,  -1280,          0,          0,         0,           0,     8,         1,        10,         0,         0,         -1,         -1,       40,        0,        0,    31 },
	{ "Staff of haste",                  10,       1,    124,    40000,        8,       16,    0,   1048576,       23,       0,         0,            0,           75,       75,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         58,       30,        0,        0,   155 },
	{ "White Staff of Lightning",        10,       1,    124,     7160,        8,       16,    0,         0,       23,       3,        56,           56,           75,       75,       0,         0,      0,       0,       0,       0,       0,      0,      0,     13,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         10,         -1,       30,       20,        0,   155 },
	{ "Lightning Maul",                   4,       1,    122,    11800,        6,       20,    0,        32,        0,       0,         0,            0,           50,       50,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         2,        20,         17,         -1,       55,        0,        0,   142 },
	{ "Ivory Axe of blood",               2,       1,    143,    31194,       12,       30,    0,     65536,        0,       0,         0,            0,           75,       75,       0,         0,      0,       0,       0,       0,       0,      0,      0,     37,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         10,         56,       80,        0,        0,   135 },
	{ "Jade Crown of vim",                7,       2,     95,    19200,        0,        0,   10,         0,        0,       0,         0,            0,           18,       40,       0,         0,      0,       0,       0,       0,      14,     30,     30,     30,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         11,         25,        0,        0,        0,    52 },

	// clang-format on
};

TEST(pack, UnPackItem_diablo)
{
	ItemStruct id;
	PkItemStruct is;

	gbIsHellfire = false;
	gbIsMultiplayer = false;

	for (size_t i = 0; i < sizeof(PackedDiabloItems) / sizeof(*PackedDiabloItems); i++) {
		UnPackItem(&PackedDiabloItems[i], &id, false);
		CompareItems(&id, &DiabloItems[i]);

		PackItem(&is, &id);
		ComparePackedItems(&is, &PackedDiabloItems[i]);
	}
}

TEST(pack, UnPackItem_diablo_unique_bug)
{
	PkItemStruct pkItemBug = { 6, 911, 14, 5, 60, 60, 0, 0, 0, 0 }; // Veil of Steel - with morph bug
	PkItemStruct pkItem = { 6, 655, 14, 5, 60, 60, 0, 0, 0, 0 };    // Veil of Steel - fixed

	gbIsHellfire = false;
	gbIsMultiplayer = false;

	ItemStruct id;
	UnPackItem(&pkItemBug, &id, false);
	ASSERT_STREQ(id._iIName, "Veil of Steel");
	ASSERT_EQ(id._itype, ITYPE_HELM);
	ASSERT_EQ(id._iClass, ICLASS_ARMOR);
	ASSERT_EQ(id._iCurs, 85);
	ASSERT_EQ(id._iIvalue, 63800);
	ASSERT_EQ(id._iAC, 18);
	ASSERT_EQ(id._iMiscId, IMISC_UNIQUE);
	ASSERT_EQ(id._iPLAC, 60);
	ASSERT_EQ(id._iPLStr, 15);
	ASSERT_EQ(id._iPLVit, 15);
	ASSERT_EQ(id._iPLFR, 50);
	ASSERT_EQ(id._iPLLR, 50);
	ASSERT_EQ(id._iPLMR, 50);
	ASSERT_EQ(id._iPLMana, -1920);
	ASSERT_EQ(id._iPLLight, -2);
	ASSERT_EQ(id._iUid, 6);
	ASSERT_EQ(id.IDidx, IDI_STEELVEIL);

	PkItemStruct is;
	PackItem(&is, &id);
	ComparePackedItems(&is, &pkItem);
}

const PkItemStruct PackedDiabloMPItems[] = {
	// clang-format off
	//     iSeed, iCreateInfo, idx, bId, bDur, bMDur, bCh, bMCh, wValue, dwBuff
    {  309674341,         193, 109,   0,    0,     0,   0,    0,      0,      0 }, // Book of Firebolt
	{ 1291471654,           6,  34,   0,    0,     0,   0,    0,      0,      0 }, // Scroll of Resurrect
	// clang-format on
};

const TestItemStruct DiabloMPItems[] = {
	// clang-format off
	//_iIName,            _itype, _iClass, _iCurs, _iIvalue, _iMinDam, _iMaxDam, _iAC,   _iFlags, _iMiscId, _iSpell, _iCharges, _iMaxCharges, _iDurability, _iMaxDur, _iPLDam, _iPLToHit, _iPLAC, _iPLStr, _iPLMag, _iPLDex, _iPLVit, _iPLFR, _iPLLR, _iPLMR, _iPLMana, _iPLHP, _iPLDamMod, _iPLGetHit, _iPLLight, _iSplLvlAdd, _iUid, _iFMinDam, _iFMaxDam, _iLMinDam, _iLMaxDam, _iPrePower, _iSufPower, _iMinStr, _iMinMag, _iMinDex, IDidx );
	{ "Book of Firebolt",      0,       3,     87,     1000,        0,        0,    0,         0,       24,       1,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,       15,        0,   114 },
	{ "Scroll of Resurrect",   0,       3,      1,      250,        0,        0,    0,         0,       22,      32,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    34 },
	// clang-format on
};

TEST(pack, UnPackItem_diablo_multiplayer)
{
	ItemStruct id;
	PkItemStruct is;

	gbIsHellfire = false;
	gbIsMultiplayer = true;

	for (size_t i = 0; i < sizeof(PackedDiabloMPItems) / sizeof(*PackedDiabloMPItems); i++) {
		UnPackItem(&PackedDiabloMPItems[i], &id, false);
		CompareItems(&id, &DiabloMPItems[i]);

		PackItem(&is, &id);
		ComparePackedItems(&is, &PackedDiabloMPItems[i]);
	}
}

const PkItemStruct PackedHellfireItems[] = {
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
	{  303108965,         280, 148,   3,   18,    50,   0,    0,      0,      0 }, // Bow of shock
	{  575830996,         257, 143,   3,   30,    30,   0,    0,      0,      0 }, // Bow of magic
	{ 1488880650,         194, 152,   3,   35,    35,  22,   33,      0,      0 }, // Red Staff of Healing
	{ 1864450901,         263,  71,   0,    6,    16,   0,    0,      0,      0 }, // Buckler
	{   28387651,         263,  49,   0,   15,    20,   0,    0,      0,      0 }, // Skull Cap
	{ 1298183212,         257,  55,   0,    6,     6,   0,    0,      0,      0 }, // Rags
	{ 1113945523,         260,  58,   0,   30,    30,   0,    0,      0,      0 }, // Quilted Armor
	{  765757608,         260,  58,   2,   12,    30,   0,    0,      0,      0 }, // Armor of light
	{  188812770,         346,  67,   3,   75,    75,   0,    0,      0,      0 }, // Saintly Plate of the stars
	{  283577043,        2070,  67,   3,   63,    75,   0,    0,      0,      0 }, // Plate of the stars
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
	{  342570085,        4114,  74,   3,  255,   255,   0,    0,      0,      0 }, // Shield of the ages
	{ 1514523617,        2066, 139,   3,   20,    20,   0,    0,      0,      0 }, // Heavy Club of puncturing
	{  701987341,        8208, 114,   1,    0,     0,   0,    0,      0,      0 }, // Book of Lightning
	{  568338383,         196, 124,   3,   23,    45,   0,    0,      0,      0 }, // Jester's Sabre
	{ 1308277119,        2056,  72,   3,   24,    24,   0,    0,      0,      0 }, // Shield of blocking
	{          0,         512,   6,   5,   10,    10,   0,    0,      0,      0 }, // The Butcher's Cleaver
	{ 1621745295,        2057, 121,   3,   28,    28,   0,    0,      0,      0 }, // Scimitar of peril
	{  492619876,        2054, 132,   3,   12,    12,   0,    0,      0,      0 }, // Crystalline Axe
	{ 1859493982,        2053,  56,   3,   18,    18,   0,    0,      0,      0 }, // Red Cloak
	{ 1593032051,        2050, 136,   3,   32,    32,   0,    0,      0,      0 }, // Mace of decay
	// clang-format on
};

const TestItemStruct HellfireItems[] = {
	// clang-format off
	//_iIName,                       _itype, _iClass, _iCurs, _iIvalue, _iMinDam, _iMaxDam, _iAC,   _iFlags, _iMiscId, _iSpell, _iCharges, _iMaxCharges, _iDurability, _iMaxDur, _iPLDam, _iPLToHit, _iPLAC, _iPLStr, _iPLMag, _iPLDex, _iPLVit, _iPLFR, _iPLLR, _iPLMR, _iPLMana, _iPLHP, _iPLDamMod, _iPLGetHit, _iPLLight, _iSplLvlAdd, _iUid, _iFMinDam, _iFMaxDam, _iLMinDam, _iLMaxDam, _iPrePower, _iSufPower, _iMinStr, _iMinMag, _iMinDex, IDidx );
	{ "Ring of stability",               12,       3,     12,     8000,        0,        0,    0,   4194304,       25,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         59,        0,        0,        0,   156 },
	{ "Ring of precision",               12,       3,     12,    10200,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,      16,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         23,        0,        0,        0,   157 },
	{ "Obsidian Ring of wizardry",       12,       3,     12,    56928,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,         0,      0,       0,      27,       0,       0,     37,     37,     37,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         11,         21,        0,        0,        0,   157 },
	{ "Ring of precision",               12,       3,     12,    10200,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,      16,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         23,        0,        0,        0,   158 },
	{ "Amulet of titans",                13,       3,     45,    20896,        0,        0,    0,         0,       26,       0,         0,            0,            0,        0,       0,         0,      0,      28,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         19,        0,        0,        0,   160 },
	{ "Gold Amulet",                     13,       3,     45,    13692,        0,        0,    0,         0,       26,       0,         0,            0,            0,        0,       0,        29,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          0,         -1,        0,        0,        0,   160 },
	{ "Messerschmidt's Reaver",           2,       1,    163,    58000,       12,       30,    0,        16,        0,       0,         0,            0,           75,       75,     200,         0,      0,       5,       5,       5,       5,      0,      0,      0,        0,  -3200,         15,          0,         0,           0,    44,         2,        12,         0,         0,         -1,         -1,       80,        0,        0,   135 },
	{ "Vicious Maul of structure",        4,       1,    122,    10489,        6,       20,    0,         0,        0,       0,         0,            0,          127,      128,      72,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          2,         35,       55,        0,        0,   142 },
	{ "Short Sword",                      1,       1,     64,      120,        2,        6,    0,         0,        0,       0,         0,            0,           15,       24,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,       18,        0,        0,   119 },
	{ "Bow of shock",                     3,       1,    119,     8000,        1,       10,    0,  33554432,        0,       0,         0,            0,           18,       50,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         1,         6,         -1,         43,       30,        0,       60,   148 },
	{ "Bow of magic",                     3,       1,    118,      400,        1,        4,    0,         0,        0,       0,         0,            0,           30,       30,       0,         0,      0,       0,       1,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         21,        0,        0,        0,   143 },
	{ "Red Staff of Healing",            10,       1,    123,     1360,        4,        8,    0,         0,       23,       2,        22,           33,           35,       35,       0,         0,      0,       0,       0,       0,       0,     10,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          8,         -1,        0,       17,        0,   152 },
	{ "Buckler",                          5,       2,     83,       30,        0,        0,    5,         0,        0,       0,         0,            0,            6,       16,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    71 },
	{ "Skull Cap",                        7,       2,     90,       25,        0,        0,    3,         0,        0,       0,         0,            0,           15,       20,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    49 },
	{ "Rags",                             6,       2,    128,        5,        0,        0,    4,         0,        0,       0,         0,            0,            6,        6,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    55 },
	{ "Quilted Armor",                    6,       2,    129,      200,        0,        0,    7,         0,        0,       0,         0,            0,           30,       30,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    58 },
	{ "Armor of light",                   6,       2,    129,     1150,        0,        0,   10,         0,        0,       0,         0,            0,           12,       30,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         2,           0,     0,         0,         0,         0,         0,         -1,         38,        0,        0,        0,    58 },
	{ "Saintly Plate of the stars",       9,       2,    103,   140729,        0,        0,   46,         0,        0,       0,         0,            0,           75,       75,       0,         0,    121,      10,      10,      10,      10,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          6,         27,       60,        0,        0,    67 },
	{ "Plate of the stars",               9,       2,    103,    77800,        0,        0,   49,         0,        0,       0,         0,            0,           63,       75,       0,         0,      0,       8,       8,       8,       8,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         27,       60,        0,        0,    67 },
	{ "Potion of Healing",                0,       3,     32,       50,        0,        0,    0,         0,        3,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    24 },
	{ "Potion of Full Healing",           0,       3,     35,      150,        0,        0,    0,         0,        2,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    29 },
	{ "Potion of Mana",                   0,       3,     39,       50,        0,        0,    0,         0,        6,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    25 },
	{ "Scroll of Golem",                  0,       3,      1,     1100,        0,        0,    0,         0,       22,      21,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,       51,        0,   110 },
	{ "Scroll of Search",                 0,       3,      1,       50,        0,        0,    0,         0,       21,      46,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    92 },
	{ "Scroll of Identify",               0,       3,      1,      100,        0,        0,    0,         0,       21,       5,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    94 },
	{ "Scroll of Town Portal",            0,       3,      1,      200,        0,        0,    0,         0,       21,       7,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    98 },
	{ "Scroll of Healing",                0,       3,      1,       50,        0,        0,    0,         0,       21,       2,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    91 },
	{ "Rune of Fire",                     0,       3,    193,      100,        0,        0,    0,         0,       47,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,   161 },
	{ "Gold",                            11,       4,      5,        0,        0,        0,    0,         0,        0,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,     0 },
	{ "The Undead Crown",                 7,       2,     77,    16650,        0,        0,    8,         2,       27,       0,         0,            0,           45,       50,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     1,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,     7 },
	{ "Empyrean Band",                   12,       3,     18,     8000,        0,        0,    0, 270532608,       27,       0,         0,            0,            0,        0,       0,         0,      0,       2,       2,       2,       2,      0,      0,      0,        0,      0,          0,          0,         2,           0,     2,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,     8 },
	{ "Ring of Truth",                   12,       3,     10,     9100,        0,        0,    0,         0,       27,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,     10,     10,     10,        0,    640,          0,         -1,         0,           0,     4,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    11 },
	{ "Griswold's Edge",                  1,       1,     61,    42000,        4,       12,    0,    264208,       27,       0,         0,            0,           50,       50,       0,        25,      0,       0,       0,       0,       0,      0,      0,      0,     1280,  -1280,          0,          0,         0,           0,     8,         1,        10,         0,         0,         -1,         -1,       40,        0,        0,    31 },
	{ "Bovine Plate",                     9,       2,    226,      400,        0,        0,  150,         0,       27,       0,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,     30,     30,     30,    -3200,      0,          0,          0,         5,          -2,     9,         0,         0,         0,         0,         -1,         -1,       50,        0,        0,    32 },
	{ "Book of Healing",                  0,       3,     86,     1000,        0,        0,    0,         0,       24,       2,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,       17,        0,   114 },
	{ "Book of Charged Bolt",             0,       3,     88,     1000,        0,        0,    0,         0,       24,      30,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,       25,        0,   114 },
	{ "Book of Firebolt",                 0,       3,     87,     1000,        0,        0,    0,         0,       24,       1,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,       15,        0,   114 },
	{ "Blacksmith Oil",                   0,       3,     30,      100,        0,        0,    0,         0,       36,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    83 },
	{ "Oil of Accuracy",                  0,       3,     30,      500,        0,        0,    0,         0,       31,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    84 },
	{ "Oil of Sharpness",                 0,       3,     30,      500,        0,        0,    0,         0,       33,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    85 },
	{ "Oil of Permanence",                0,       3,     30,    15000,        0,        0,    0,         0,       38,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,    86 },
	{ "Doppelganger's Axe",               2,       1,    144,     6640,        4,       12,    0,         0,        0,       0,         0,            0,           23,       32,      86,        26,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         89,         -1,       22,        0,        0,   131 },
	{ "Flail of vampires",                4,       1,    131,    16500,        2,       12,    0,     16384,        0,       0,         0,            0,           36,       36,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         55,       30,        0,        0,   141 },
	{ "Gladiator's Ring",                12,       3,    186,    10000,        0,        0,    0,         0,       25,       0,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,   109,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,   157 },
	{ "Warrior's Staff of the moon",     10,       1,    124,    42332,        8,       16,    0,         0,       23,       0,         0,            0,           75,       75,      54,        15,      0,       5,       5,       5,       5,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          4,         27,       30,        0,        0,   155 },
	{ "Shield of the ages",               5,       2,    113,     2600,        0,        0,   10,         0,        0,       0,         0,            0,          255,      255,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         37,       50,        0,        0,    74 },
	{ "Heavy Club of puncturing",         4,       1,     70,     5239,        3,        6,    0,         0,        0,       0,         0,            0,           20,       20,      52,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          2,         57,       18,        0,        0,   139 },
	{ "Book of Lightning",                0,       3,     88,     3000,        0,        0,    0,         0,       24,       3,         0,            0,            0,        0,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,       20,        0,   114 },
	{ "Jester's Sabre",                   1,       1,     67,     1710,        1,        8,    0,         0,        0,       0,         0,            0,           23,       45,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         87,         -1,       17,        0,        0,   124 },
	{ "Shield of blocking",               5,       2,    105,     4360,        0,        0,    6,  16777216,        0,       0,         0,            0,           24,       24,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         60,       25,        0,        0,    72 },
	{ "The Butcher's Cleaver",            2,       1,    106,     3650,        4,       24,    0,         0,       27,       0,         0,            0,           10,       10,       0,         0,      0,      10,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         -1,        0,        0,        0,     6 },
	{ "Scimitar of peril",                1,       1,     72,      700,        3,        7,    0,         0,        0,       0,         0,            0,           28,       28,       0,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         86,       23,        0,       23,   121 },
	{ "Crystalline Axe",                  2,       1,    142,     5250,        6,       16,    0,         0,        0,       0,         0,            0,           12,       12,     280,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         88,         -1,       30,        0,        0,   132 },
	{ "Red Cloak",                        6,       2,    149,      580,        0,        0,    3,         0,        0,       0,         0,            0,           18,       18,       0,         0,      0,       0,       0,       0,       0,     10,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,          8,         -1,        0,        0,        0,    56 },
	{ "Mace of decay",                    4,       1,     59,      600,        1,        8,    0,         0,        0,       0,         0,            0,           32,       32,     232,         0,      0,       0,       0,       0,       0,      0,      0,      0,        0,      0,          0,          0,         0,           0,     0,         0,         0,         0,         0,         -1,         85,       16,        0,        0,   136 },
	// clang-format on
};

TEST(pack, UnPackItem_hellfire)
{
	ItemStruct id;
	PkItemStruct is;

	gbIsHellfire = true;
	gbIsMultiplayer = false;

	for (size_t i = 0; i < sizeof(PackedHellfireItems) / sizeof(*PackedHellfireItems); i++) {
		UnPackItem(&PackedHellfireItems[i], &id, true);
		CompareItems(&id, &HellfireItems[i]);

		PackItem(&is, &id);
		is.dwBuff &= ~CF_HELLFIRE;
		ComparePackedItems(&is, &PackedHellfireItems[i]);
	}
}

TEST(pack, UnPackItem_diablo_strip_hellfire_items)
{
	PkItemStruct is = { 1478792102, 259, 92, 0, 0, 0, 0, 0, 0, 0 }; // Scroll of Search
	ItemStruct id;

	gbIsHellfire = false;
	gbIsMultiplayer = false;

	UnPackItem(&is, &id, true);

	ASSERT_EQ(id._itype, ITYPE_NONE);
}

TEST(pack, UnPackItem_empty)
{
	PkItemStruct is = { 0, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0 };
	ItemStruct id;

	UnPackItem(&is, &id, false);

	ASSERT_EQ(id._itype, ITYPE_NONE);
}

TEST(pack, PackItem_empty)
{
	PkItemStruct is;
	ItemStruct id;

	id._itype = ITYPE_NONE;

	PackItem(&is, &id);

	ASSERT_EQ(is.idx, 0xFFFF);
}

static void compareGold(const PkItemStruct *is, int iCurs)
{
	ItemStruct id;
	UnPackItem(is, &id, false);
	ASSERT_EQ(id._iCurs, iCurs);
	ASSERT_EQ(id.IDidx, IDI_GOLD);
	ASSERT_EQ(id._ivalue, is->wValue);
	ASSERT_EQ(id._itype, ITYPE_GOLD);
	ASSERT_EQ(id._iClass, ICLASS_GOLD);

	PkItemStruct is2;
	PackItem(&is2, &id);
	ComparePackedItems(is, &is2);
}

TEST(pack, UnPackItem_gold_small)
{
	PkItemStruct is = { 0, 0, IDI_GOLD, 0, 0, 0, 0, 0, 1000, 0 };
	compareGold(&is, ICURS_GOLD_SMALL);
}

TEST(pack, UnPackItem_gold_medium)
{
	PkItemStruct is = { 0, 0, IDI_GOLD, 0, 0, 0, 0, 0, 1001, 0 };
	compareGold(&is, ICURS_GOLD_MEDIUM);
}

TEST(pack, UnPackItem_gold_large)
{
	PkItemStruct is = { 0, 0, IDI_GOLD, 0, 0, 0, 0, 0, 2500, 0 };
	compareGold(&is, ICURS_GOLD_LARGE);
}

TEST(pack, UnPackItem_ear)
{
	PkItemStruct is = { 1633955154, 17509, 23, 111, 103, 117, 101, 68, 19843, 0 };
	ItemStruct id;

	UnPackItem(&is, &id, false);
	ASSERT_STREQ(id._iName, "Ear of Dead-RogueDM");
	ASSERT_EQ(id._ivalue, 3);

	PkItemStruct is2;
	PackItem(&is2, &id);
	ComparePackedItems(&is, &is2);
}
