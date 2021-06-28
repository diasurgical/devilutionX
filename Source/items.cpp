/**
 * @file items.cpp
 *
 * Implementation of item functionality.
 */
#include "items.h"

#include <algorithm>
#include <bitset>
#include <climits>
#include <cstdint>

#include <fmt/format.h>

#include "cursor.h"
#include "doom.h"
#include "dx.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/random.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "lighting.h"
#include "missiles.h"
#include "options.h"
#include "stores.h"
#include "utils/language.h"
#include "utils/math.h"
#include "utils/stdcompat/algorithm.hpp"

namespace devilution {
namespace {
std::optional<CelSprite> itemanims[ITEMTYPES];

} // namespace

enum anim_armor_id : uint8_t {
	// clang-format off
	ANIM_ID_LIGHT_ARMOR  = 0,
	ANIM_ID_MEDIUM_ARMOR = 1 << 4,
	ANIM_ID_HEAVY_ARMOR  = 1 << 5,
	// clang-format on
};

int itemactive[MAXITEMS];
bool uitemflag;
int itemavail[MAXITEMS];
ItemStruct curruitem;
ItemGetRecordStruct itemrecord[MAXITEMS];
/** Contains the items on ground in the current game. */
ItemStruct items[MAXITEMS + 1];
bool itemhold[3][3];
CornerStoneStruct CornerStone;
bool UniqueItemFlags[128];
int numitems;
int gnNumGetRecords;

/* data */

int OilLevels[] = { 1, 10, 1, 10, 4, 1, 5, 17, 1, 10 };
int OilValues[] = { 500, 2500, 500, 2500, 1500, 100, 2500, 15000, 500, 2500 };
item_misc_id OilMagic[] = {
	IMISC_OILACC,
	IMISC_OILMAST,
	IMISC_OILSHARP,
	IMISC_OILDEATH,
	IMISC_OILSKILL,
	IMISC_OILBSMTH,
	IMISC_OILFORT,
	IMISC_OILPERM,
	IMISC_OILHARD,
	IMISC_OILIMP,
};
char OilNames[10][25] = {
	N_("Oil of Accuracy"),
	N_("Oil of Mastery"),
	N_("Oil of Sharpness"),
	N_("Oil of Death"),
	N_("Oil of Skill"),
	N_("Blacksmith Oil"),
	N_("Oil of Fortitude"),
	N_("Oil of Permanence"),
	N_("Oil of Hardening"),
	N_("Oil of Imperviousness")
};
int MaxGold = GOLD_MAX_LIMIT;

/** Maps from item_cursor_graphic to in-memory item type. */
BYTE ItemCAnimTbl[] = {
	20, 16, 16, 16, 4, 4, 4, 12, 12, 12,
	12, 12, 12, 12, 12, 21, 21, 25, 12, 28,
	28, 28, 38, 38, 38, 32, 38, 38, 38, 24,
	24, 26, 2, 25, 22, 23, 24, 25, 27, 27,
	29, 0, 0, 0, 12, 12, 12, 12, 12, 0,
	8, 8, 0, 8, 8, 8, 8, 8, 8, 6,
	8, 8, 8, 6, 8, 8, 6, 8, 8, 6,
	6, 6, 8, 8, 8, 5, 9, 13, 13, 13,
	5, 5, 5, 15, 5, 5, 18, 18, 18, 30,
	5, 5, 14, 5, 14, 13, 16, 18, 5, 5,
	7, 1, 3, 17, 1, 15, 10, 14, 3, 11,
	8, 0, 1, 7, 0, 7, 15, 7, 3, 3,
	3, 6, 6, 11, 11, 11, 31, 14, 14, 14,
	6, 6, 7, 3, 8, 14, 0, 14, 14, 0,
	33, 1, 1, 1, 1, 1, 7, 7, 7, 14,
	14, 17, 17, 17, 0, 34, 1, 0, 3, 17,
	8, 8, 6, 1, 3, 3, 11, 3, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 35, 39, 36,
	36, 36, 37, 38, 38, 38, 38, 38, 41, 42,
	8, 8, 8, 17, 0, 6, 8, 11, 11, 3,
	3, 1, 6, 6, 6, 1, 8, 6, 11, 3,
	6, 8, 1, 6, 6, 17, 40, 0, 0
};
/** Map of item type .cel file names. */
const char *const ItemDropNames[] = {
	"Armor2",
	"Axe",
	"FBttle",
	"Bow",
	"GoldFlip",
	"Helmut",
	"Mace",
	"Shield",
	"SwrdFlip",
	"Rock",
	"Cleaver",
	"Staff",
	"Ring",
	"CrownF",
	"LArmor",
	"WShield",
	"Scroll",
	"FPlateAr",
	"FBook",
	"Food",
	"FBttleBB",
	"FBttleDY",
	"FBttleOR",
	"FBttleBR",
	"FBttleBL",
	"FBttleBY",
	"FBttleWH",
	"FBttleDB",
	"FEar",
	"FBrain",
	"FMush",
	"Innsign",
	"Bldstn",
	"Fanvil",
	"FLazStaf",
	"bombs1",
	"halfps1",
	"wholeps1",
	"runes1",
	"teddys1",
	"cows1",
	"donkys1",
	"mooses1",
};
/** Maps of item drop animation length. */
BYTE ItemAnimLs[] = {
	15,
	13,
	16,
	13,
	10,
	13,
	13,
	13,
	13,
	10,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	13,
	1,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	16,
	13,
	12,
	12,
	13,
	13,
	13,
	8,
	10,
	16,
	16,
	10,
	10,
	15,
	15,
	15,
};
/** Maps of drop sounds effect of dropping the item on ground. */
_sfx_id ItemDropSnds[] = {
	IS_FHARM,
	IS_FAXE,
	IS_FPOT,
	IS_FBOW,
	IS_GOLD,
	IS_FCAP,
	IS_FSWOR,
	IS_FSHLD,
	IS_FSWOR,
	IS_FROCK,
	IS_FAXE,
	IS_FSTAF,
	IS_FRING,
	IS_FCAP,
	IS_FLARM,
	IS_FSHLD,
	IS_FSCRL,
	IS_FHARM,
	IS_FBOOK,
	IS_FLARM,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FPOT,
	IS_FBODY,
	IS_FBODY,
	IS_FMUSH,
	IS_ISIGN,
	IS_FBLST,
	IS_FANVL,
	IS_FSTAF,
	IS_FROCK,
	IS_FSCRL,
	IS_FSCRL,
	IS_FROCK,
	IS_FMUSH,
	IS_FHARM,
	IS_FLARM,
	IS_FLARM,
};
/** Maps of drop sounds effect of placing the item in the inventory. */
_sfx_id ItemInvSnds[] = {
	IS_IHARM,
	IS_IAXE,
	IS_IPOT,
	IS_IBOW,
	IS_GOLD,
	IS_ICAP,
	IS_ISWORD,
	IS_ISHIEL,
	IS_ISWORD,
	IS_IROCK,
	IS_IAXE,
	IS_ISTAF,
	IS_IRING,
	IS_ICAP,
	IS_ILARM,
	IS_ISHIEL,
	IS_ISCROL,
	IS_IHARM,
	IS_IBOOK,
	IS_IHARM,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IPOT,
	IS_IBODY,
	IS_IBODY,
	IS_IMUSH,
	IS_ISIGN,
	IS_IBLST,
	IS_IANVL,
	IS_ISTAF,
	IS_IROCK,
	IS_ISCROL,
	IS_ISCROL,
	IS_IROCK,
	IS_IMUSH,
	IS_IHARM,
	IS_ILARM,
	IS_ILARM,
};
/** Maps from Griswold premium item number to a quality level delta as added to the base quality level. */
int premiumlvladd[] = {
	// clang-format off
	-1,
	-1,
	 0,
	 0,
	 1,
	 2,
	// clang-format on
};
/** Maps from Griswold premium item number to a quality level delta as added to the base quality level. */
int premiumLvlAddHellfire[] = {
	// clang-format off
	-1,
	-1,
	-1,
	 0,
	 0,
	 0,
	 0,
	 1,
	 1,
	 1,
	 1,
	 2,
	 2,
	 3,
	 3,
	// clang-format on
};

bool IsItemAvailable(int i)
{
	if (gbIsSpawn) {
		if (i >= 62 && i <= 71)
			return false; // Medium and heavy armors
		if (i == 105 || i == 107 || i == 108 || i == 110 || i == 111 || i == 113)
			return false; // Unavailable scrolls
	}

	if (gbIsHellfire)
		return true;

	return (
	           i != IDI_MAPOFDOOM                   // Cathedral Map
	           && i != IDI_LGTFORGE                 // Bovine Plate
	           && (i < IDI_OIL || i > IDI_GREYSUIT) // Hellfire exclusive items
	           && (i < 83 || i > 86)                // Oils
	           && i != 92                           // Scroll of Search
	           && (i < 161 || i > 165)              // Runes
	           && i != IDI_SORCERER                 // Short Staff of Mana
	           )
	    || (
	        // Bard items are technically Hellfire-exclusive
	        // but are just normal items with adjusted stats.
	        sgOptions.Gameplay.bTestBard && (i == IDI_BARDSWORD || i == IDI_BARDDAGGER));
}

BYTE GetOutlineColor(const ItemStruct &item, bool checkReq)
{
	if (checkReq && !item._iStatFlag)
		return ICOL_RED;
	if (item._itype == ITYPE_GOLD)
		return ICOL_YELLOW;
	if (item._iMagical == ITEM_QUALITY_MAGIC)
		return ICOL_BLUE;
	if (item._iMagical == ITEM_QUALITY_UNIQUE)
		return ICOL_YELLOW;

	return ICOL_WHITE;
}

bool IsUniqueAvailable(int i)
{
	return gbIsHellfire || i <= 89;
}

static bool IsPrefixValidForItemType(int i, int flgs)
{
	int PLIType = PL_Prefix[i].PLIType;

	if (!gbIsHellfire) {
		if (i > 82)
			return false;

		if (i >= 12 && i <= 20)
			PLIType &= ~PLT_STAFF;
	}

	return (flgs & PLIType) != 0;
}

static bool IsSuffixValidForItemType(int i, int flgs)
{
	int PLIType = PL_Suffix[i].PLIType;

	if (!gbIsHellfire) {
		if (i > 94)
			return false;

		if ((i >= 0 && i <= 1)
		    || (i >= 14 && i <= 15)
		    || (i >= 21 && i <= 22)
		    || (i >= 34 && i <= 36)
		    || (i >= 41 && i <= 44)
		    || (i >= 60 && i <= 63))
			PLIType &= ~PLT_STAFF;
	}

	return (flgs & PLIType) != 0;
}

int items_get_currlevel()
{
	int lvl;

	lvl = currlevel;
	if (currlevel >= 17 && currlevel <= 20)
		lvl = currlevel - 8;
	if (currlevel >= 21 && currlevel <= 24)
		lvl = currlevel - 7;

	return lvl;
}

void InitItemGFX()
{
	char arglist[64];

	int itemTypes = gbIsHellfire ? ITEMTYPES : 35;
	for (int i = 0; i < itemTypes; i++) {
		sprintf(arglist, "Items\\%s.CEL", ItemDropNames[i]);
		itemanims[i] = LoadCel(arglist, ItemAnimWidth);
	}
	memset(UniqueItemFlags, 0, sizeof(UniqueItemFlags));
}

bool ItemPlace(Point position)
{
	if (dMonster[position.x][position.y] != 0)
		return false;
	if (dPlayer[position.x][position.y] != 0)
		return false;
	if (dItem[position.x][position.y] != 0)
		return false;
	if (dObject[position.x][position.y] != 0)
		return false;
	if ((dFlags[position.x][position.y] & BFLAG_POPULATED) != 0)
		return false;
	if (nSolidTable[dPiece[position.x][position.y]])
		return false;

	return true;
}

Point GetRandomAvailableItemPosition()
{
	Point position = {};
	do {
		position = Point { 16, 16 } + Point { GenerateRnd(80), GenerateRnd(80) };
	} while (!ItemPlace(position));

	return position;
}

void AddInitItems()
{
	int j, rnd;

	int curlv = items_get_currlevel();
	rnd = GenerateRnd(3) + 3;
	for (j = 0; j < rnd; j++) {
		int ii = AllocateItem();

		Point position = GetRandomAvailableItemPosition();
		items[ii].position = position;

		dItem[position.x][position.y] = ii + 1;

		items[ii]._iSeed = AdvanceRndSeed();

		if (GenerateRnd(2) != 0)
			GetItemAttrs(ii, IDI_HEAL, curlv);
		else
			GetItemAttrs(ii, IDI_MANA, curlv);

		items[ii]._iCreateInfo = curlv | CF_PREGEN;
		SetupItem(ii);
		items[ii].AnimInfo.CurrentFrame = items[ii].AnimInfo.NumberOfFrames;
		items[ii]._iAnimFlag = false;
		items[ii]._iSelFlag = 1;
		DeltaAddItem(ii);
	}
}

static void items_42390F()
{
	int id;

	switch (currlevel) {
	case 22:
		id = IDI_NOTE2;
		break;
	case 23:
		id = IDI_NOTE3;
		break;
	default:
		id = IDI_NOTE1;
		break;
	}

	Point position = GetRandomAvailableItemPosition();
	SpawnQuestItem(id, position, 0, 1);
}

void InitItems()
{
	int i;

	memset(&items[0], 0, sizeof(*items));
	GetItemAttrs(0, IDI_GOLD, 1);
	golditem = items[0];
	golditem._iStatFlag = true;
	numitems = 0;

	for (i = 0; i < MAXITEMS; i++) {
		items[i]._itype = ITYPE_NONE;
		items[i].position = { 0, 0 };
		items[i]._iAnimFlag = false;
		items[i]._iSelFlag = 0;
		items[i]._iIdentified = false;
		items[i]._iPostDraw = false;
	}

	for (i = 0; i < MAXITEMS; i++) {
		itemavail[i] = i;
		itemactive[i] = 0;
	}

	if (!setlevel) {
		AdvanceRndSeed(); /* unused */
		if (QuestStatus(Q_ROCK))
			SpawnRock();
		if (QuestStatus(Q_ANVIL))
			SpawnQuestItem(IDI_ANVIL, { 2 * setpc_x + 27, 2 * setpc_y + 27 }, 0, 1);
		if (sgGameInitInfo.bCowQuest != 0 && currlevel == 20)
			SpawnQuestItem(IDI_BROWNSUIT, { 25, 25 }, 3, 1);
		if (sgGameInitInfo.bCowQuest != 0 && currlevel == 19)
			SpawnQuestItem(IDI_GREYSUIT, { 25, 25 }, 3, 1);
		if (currlevel > 0 && currlevel < 16)
			AddInitItems();
		if (currlevel >= 21 && currlevel <= 23)
			items_42390F();
	}

	uitemflag = false;
}

void CalcPlrItemVals(int playerId, bool Loadgfx)
{
	auto &player = plr[playerId];

	int mind = 0; // min damage
	int maxd = 0; // max damage
	int tac = 0;  // accuracy

	int g;
	int i;

	int bdam = 0;   // bonus damage
	int btohit = 0; // bonus chance to hit
	int bac = 0;    // bonus accuracy

	int iflgs = ISPL_NONE; // item_special_effect flags

	int pDamAcFlags = 0;

	int sadd = 0; // added strength
	int madd = 0; // added magic
	int dadd = 0; // added dexterity
	int vadd = 0; // added vitality

	uint64_t spl = 0; // bitarray for all enabled/active spells

	int fr = 0; // fire resistance
	int lr = 0; // lightning resistance
	int mr = 0; // magic resistance

	int dmod = 0; // bonus damage mod?
	int ghit = 0; // increased damage from enemies

	int lrad = 10; // light radius

	int ihp = 0;   // increased HP
	int imana = 0; // increased mana

	int spllvladd = 0; // increased spell level
	int enac = 0;      // enhanced accuracy

	int fmin = 0; // minimum fire damage
	int fmax = 0; // maximum fire damage
	int lmin = 0; // minimum lightning damage
	int lmax = 0; // maximum lightning damage

	for (i = 0; i < NUM_INVLOC; i++) {
		ItemStruct *itm = &player.InvBody[i];
		if (!itm->isEmpty() && itm->_iStatFlag) {

			mind += itm->_iMinDam;
			maxd += itm->_iMaxDam;
			tac += itm->_iAC;

			if (itm->_iSpell != SPL_NULL) {
				spl |= GetSpellBitmask(itm->_iSpell);
			}

			if (itm->_iMagical == ITEM_QUALITY_NORMAL || itm->_iIdentified) {
				bdam += itm->_iPLDam;
				btohit += itm->_iPLToHit;
				if (itm->_iPLAC != 0) {
					int tmpac = itm->_iAC;
					tmpac *= itm->_iPLAC;
					tmpac /= 100;
					if (tmpac == 0)
						tmpac = math::Sign(itm->_iPLAC);
					bac += tmpac;
				}
				iflgs |= itm->_iFlags;
				pDamAcFlags |= itm->_iDamAcFlags;
				sadd += itm->_iPLStr;
				madd += itm->_iPLMag;
				dadd += itm->_iPLDex;
				vadd += itm->_iPLVit;
				fr += itm->_iPLFR;
				lr += itm->_iPLLR;
				mr += itm->_iPLMR;
				dmod += itm->_iPLDamMod;
				ghit += itm->_iPLGetHit;
				lrad += itm->_iPLLight;
				ihp += itm->_iPLHP;
				imana += itm->_iPLMana;
				spllvladd += itm->_iSplLvlAdd;
				enac += itm->_iPLEnAc;
				fmin += itm->_iFMinDam;
				fmax += itm->_iFMaxDam;
				lmin += itm->_iLMinDam;
				lmax += itm->_iLMaxDam;
			}
		}
	}

	if (mind == 0 && maxd == 0) {
		mind = 1;
		maxd = 1;

		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			maxd = 3;
		}

		if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
			maxd = 3;
		}

		if (player._pClass == HeroClass::Monk) {
			mind = std::max(mind, player._pLevel / 2);
			maxd = std::max(maxd, (int)player._pLevel);
		}
	}

	if ((player._pSpellFlags & 2) == 2) {
		sadd += 2 * player._pLevel;
		dadd += player._pLevel + player._pLevel / 2;
		vadd += 2 * player._pLevel;
	}
	if ((player._pSpellFlags & 4) == 4) {
		sadd -= 2 * player._pLevel;
		dadd -= player._pLevel + player._pLevel / 2;
		vadd -= 2 * player._pLevel;
	}

	player._pIMinDam = mind;
	player._pIMaxDam = maxd;
	player._pIAC = tac;
	player._pIBonusDam = bdam;
	player._pIBonusToHit = btohit;
	player._pIBonusAC = bac;
	player._pIFlags = iflgs;
	player.pDamAcFlags = pDamAcFlags;
	player._pIBonusDamMod = dmod;
	player._pIGetHit = ghit;

	lrad = clamp(lrad, 2, 15);

	if (player._pLightRad != lrad && playerId == myplr) {
		ChangeLightRadius(player._plid, lrad);
		ChangeVisionRadius(player._pvid, lrad);
		player._pLightRad = lrad;
	}

	player._pStrength = std::max(0, sadd + player._pBaseStr);
	player._pMagic = std::max(0, madd + player._pBaseMag);
	player._pDexterity = std::max(0, dadd + player._pBaseDex);
	player._pVitality = std::max(0, vadd + player._pBaseVit);

	if (player._pClass == HeroClass::Rogue) {
		player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 200;
	} else if (player._pClass == HeroClass::Monk) {
		if (player.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_STAFF) {
			if (player.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_STAFF && (!player.InvBody[INVLOC_HAND_LEFT].isEmpty() || !player.InvBody[INVLOC_HAND_RIGHT].isEmpty())) {
				player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 300;
			} else {
				player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 150;
			}
		} else {
			player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 150;
		}
	} else if (player._pClass == HeroClass::Bard) {
		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD)
			player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 150;
		else if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_BOW) {
			player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 250;
		} else {
			player._pDamageMod = player._pLevel * player._pStrength / 100;
		}
	} else if (player._pClass == HeroClass::Barbarian) {

		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_AXE || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_AXE) {
			player._pDamageMod = player._pLevel * player._pStrength / 75;
		} else if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_MACE) {
			player._pDamageMod = player._pLevel * player._pStrength / 75;
		} else if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_BOW || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_BOW) {
			player._pDamageMod = player._pLevel * player._pStrength / 300;
		} else {
			player._pDamageMod = player._pLevel * player._pStrength / 100;
		}

		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
			if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD)
				player._pIAC -= player.InvBody[INVLOC_HAND_LEFT]._iAC / 2;
			else if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD)
				player._pIAC -= player.InvBody[INVLOC_HAND_RIGHT]._iAC / 2;
		} else if (player.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_STAFF && player.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_STAFF && player.InvBody[INVLOC_HAND_LEFT]._itype != ITYPE_BOW && player.InvBody[INVLOC_HAND_RIGHT]._itype != ITYPE_BOW) {
			player._pDamageMod += player._pLevel * player._pVitality / 100;
		}
		player._pIAC += player._pLevel / 4;
	} else {
		player._pDamageMod = player._pLevel * player._pStrength / 100;
	}

	player._pISpells = spl;

	EnsureValidReadiedSpell(player);

	player._pISplLvlAdd = spllvladd;
	player._pIEnAc = enac;

	if (player._pClass == HeroClass::Barbarian) {
		mr += player._pLevel;
		fr += player._pLevel;
		lr += player._pLevel;
	}

	if ((player._pSpellFlags & 4) == 4) {
		mr -= player._pLevel;
		fr -= player._pLevel;
		lr -= player._pLevel;
	}

	if ((iflgs & ISPL_ALLRESZERO) != 0) {
		// reset resistances to zero if the respective special effect is active
		mr = 0;
		fr = 0;
		lr = 0;
	}

	player._pMagResist = clamp(mr, 0, MAXRESIST);
	player._pFireResist = clamp(fr, 0, MAXRESIST);
	player._pLghtResist = clamp(lr, 0, MAXRESIST);

	if (player._pClass == HeroClass::Warrior) {
		vadd *= 2;
	} else if (player._pClass == HeroClass::Barbarian) {
		vadd += vadd;
		vadd += (vadd / 4);
	} else if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk || player._pClass == HeroClass::Bard) {
		vadd += vadd / 2;
	}
	ihp += (vadd << 6); // BUGFIX: blood boil can cause negative shifts here (see line 757)

	if (player._pClass == HeroClass::Sorcerer) {
		madd *= 2;
	}
	if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk) {
		madd += madd / 2;
	} else if (player._pClass == HeroClass::Bard) {
		madd += (madd / 4) + (madd / 2);
	}
	imana += (madd << 6);

	player._pMaxHP = ihp + player._pMaxHPBase;
	player._pHitPoints = std::min(ihp + player._pHPBase, player._pMaxHP);

	if (playerId == myplr && (player._pHitPoints >> 6) <= 0) {
		SetPlayerHitPoints(playerId, 0);
	}

	player._pMaxMana = imana + player._pMaxManaBase;
	player._pMana = std::min(imana + player._pManaBase, player._pMaxMana);

	player._pIFMinDam = fmin;
	player._pIFMaxDam = fmax;
	player._pILMinDam = lmin;
	player._pILMaxDam = lmax;

	player._pInfraFlag = (iflgs & ISPL_INFRAVISION) != 0;

	player._pBlockFlag = false;
	if (player._pClass == HeroClass::Monk) {
		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			player._pBlockFlag = true;
			player._pIFlags |= ISPL_FASTBLOCK;
		}
		if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_STAFF && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
			player._pBlockFlag = true;
			player._pIFlags |= ISPL_FASTBLOCK;
		}
		if (player.InvBody[INVLOC_HAND_LEFT].isEmpty() && player.InvBody[INVLOC_HAND_RIGHT].isEmpty())
			player._pBlockFlag = true;
		if (player.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && player.InvBody[INVLOC_HAND_LEFT]._iLoc != ILOC_TWOHAND && player.InvBody[INVLOC_HAND_RIGHT].isEmpty())
			player._pBlockFlag = true;
		if (player.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && player.InvBody[INVLOC_HAND_RIGHT]._iLoc != ILOC_TWOHAND && player.InvBody[INVLOC_HAND_LEFT].isEmpty())
			player._pBlockFlag = true;
	}
	player._pwtype = WT_MELEE;

	g = 0;

	if (!player.InvBody[INVLOC_HAND_LEFT].isEmpty()
	    && player.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON
	    && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		g = player.InvBody[INVLOC_HAND_LEFT]._itype;
	}

	if (!player.InvBody[INVLOC_HAND_RIGHT].isEmpty()
	    && player.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON
	    && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		g = player.InvBody[INVLOC_HAND_RIGHT]._itype;
	}

	switch (g) {
	case ITYPE_SWORD:
		g = ANIM_ID_SWORD;
		break;
	case ITYPE_AXE:
		g = ANIM_ID_AXE;
		break;
	case ITYPE_BOW:
		player._pwtype = WT_RANGED;
		g = ANIM_ID_BOW;
		break;
	case ITYPE_MACE:
		g = ANIM_ID_MACE;
		break;
	case ITYPE_STAFF:
		g = ANIM_ID_STAFF;
		break;
	}

	if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		player._pBlockFlag = true;
		g++;
	}
	if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		player._pBlockFlag = true;
		g++;
	}

	if (player.InvBody[INVLOC_CHEST]._itype == ITYPE_HARMOR && player.InvBody[INVLOC_CHEST]._iStatFlag) {
		if (player._pClass == HeroClass::Monk && player.InvBody[INVLOC_CHEST]._iMagical == ITEM_QUALITY_UNIQUE)
			player._pIAC += player._pLevel / 2;
		g += ANIM_ID_HEAVY_ARMOR;
	} else if (player.InvBody[INVLOC_CHEST]._itype == ITYPE_MARMOR && player.InvBody[INVLOC_CHEST]._iStatFlag) {
		if (player._pClass == HeroClass::Monk) {
			if (player.InvBody[INVLOC_CHEST]._iMagical == ITEM_QUALITY_UNIQUE)
				player._pIAC += player._pLevel * 2;
			else
				player._pIAC += player._pLevel / 2;
		}
		g += ANIM_ID_MEDIUM_ARMOR;
	} else if (player._pClass == HeroClass::Monk) {
		player._pIAC += player._pLevel * 2;
	}

	if (player._pgfxnum != g && Loadgfx) {
		player._pgfxnum = g;
		ResetPlayerGFX(player);
		SetPlrAnims(player);
		if (player._pmode == PM_STAND) {
			LoadPlrGFX(player, player_graphic::Stand);
			player.AnimInfo.ChangeAnimationData(&*player.AnimationData[static_cast<size_t>(player_graphic::Stand)].CelSpritesForDirections[player._pdir], player._pNFrames, 3);
		} else {
			LoadPlrGFX(player, player_graphic::Walk);
			player.AnimInfo.ChangeAnimationData(&*player.AnimationData[static_cast<size_t>(player_graphic::Walk)].CelSpritesForDirections[player._pdir], player._pWFrames, 0);
		}
	} else {
		player._pgfxnum = g;
	}

	if (player.InvBody[INVLOC_AMULET].isEmpty() || player.InvBody[INVLOC_AMULET].IDidx != IDI_AURIC) {
		int half = MaxGold;
		MaxGold = GOLD_MAX_LIMIT;

		if (half != MaxGold)
			StripTopGold(playerId);
	} else {
		MaxGold = GOLD_MAX_LIMIT * 2;
	}

	drawmanaflag = true;
	drawhpflag = true;
}

void CalcSelfItems(PlayerStruct &player)
{
	int i;
	ItemStruct *pi;
	bool sf, changeflag;
	int sa, ma, da;

	sa = 0;
	ma = 0;
	da = 0;
	pi = player.InvBody;
	for (i = 0; i < NUM_INVLOC; i++, pi++) {
		if (!pi->isEmpty()) {
			pi->_iStatFlag = true;
			if (pi->_iIdentified) {
				sa += pi->_iPLStr;
				ma += pi->_iPLMag;
				da += pi->_iPLDex;
			}
		}
	}
	do {
		changeflag = false;
		pi = player.InvBody;
		for (i = 0; i < NUM_INVLOC; i++, pi++) {
			if (!pi->isEmpty() && pi->_iStatFlag) {
				sf = true;
				if (sa + player._pBaseStr < pi->_iMinStr)
					sf = false;
				if (ma + player._pBaseMag < pi->_iMinMag)
					sf = false;
				if (da + player._pBaseDex < pi->_iMinDex)
					sf = false;
				if (!sf) {
					changeflag = true;
					pi->_iStatFlag = false;
					if (pi->_iIdentified) {
						sa -= pi->_iPLStr;
						ma -= pi->_iPLMag;
						da -= pi->_iPLDex;
					}
				}
			}
		}
	} while (changeflag);
}

static bool ItemMinStats(const PlayerStruct &player, ItemStruct *x)
{
	if (player._pMagic < x->_iMinMag)
		return false;

	if (player._pStrength < x->_iMinStr)
		return false;

	if (player._pDexterity < x->_iMinDex)
		return false;

	return true;
}

void CalcPlrItemMin(PlayerStruct &player)
{
	for (int i = 0; i < player._pNumInv; i++) {
		auto &item = player.InvList[i];
		item._iStatFlag = ItemMinStats(player, &item);
	}

	for (auto &item : player.SpdList) {
		if (!item.isEmpty()) {
			item._iStatFlag = ItemMinStats(player, &item);
		}
	}
}

void CalcPlrBookVals(PlayerStruct &player)
{
	if (currlevel == 0) {
		for (int i = 1; !witchitem[i].isEmpty(); i++) {
			WitchBookLevel(i);
			witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
		}
	}

	for (int i = 0; i < player._pNumInv; i++) {
		if (player.InvList[i]._itype == ITYPE_MISC && player.InvList[i]._iMiscId == IMISC_BOOK) {
			player.InvList[i]._iMinMag = spelldata[player.InvList[i]._iSpell].sMinInt;
			int slvl = player._pSplLvl[player.InvList[i]._iSpell];

			while (slvl != 0) {
				player.InvList[i]._iMinMag += 20 * player.InvList[i]._iMinMag / 100;
				slvl--;
				if (player.InvList[i]._iMinMag + 20 * player.InvList[i]._iMinMag / 100 > 255) {
					player.InvList[i]._iMinMag = 255;
					slvl = 0;
				}
			}
			player.InvList[i]._iStatFlag = ItemMinStats(player, &player.InvList[i]);
		}
	}
}

void CalcPlrInv(int playerId, bool Loadgfx)
{
	auto &player = plr[playerId];

	CalcPlrItemMin(player);
	CalcSelfItems(player);
	CalcPlrItemVals(playerId, Loadgfx);
	CalcPlrItemMin(player);
	if (playerId == myplr) {
		CalcPlrBookVals(player);
		player.CalcScrolls();
		CalcPlrStaff(player);
		if (playerId == myplr && currlevel == 0)
			RecalcStoreStats();
	}
}

void SetPlrHandItem(ItemStruct *h, int idata)
{
	ItemDataStruct *pAllItem;

	pAllItem = &AllItemsList[idata];

	// zero-initialize struct
	memset(h, 0, sizeof(*h));

	h->_itype = pAllItem->itype;
	h->_iCurs = pAllItem->iCurs;
	strcpy(h->_iName, _(pAllItem->iName));
	strcpy(h->_iIName, _(pAllItem->iName));
	h->_iLoc = pAllItem->iLoc;
	h->_iClass = pAllItem->iClass;
	h->_iMinDam = pAllItem->iMinDam;
	h->_iMaxDam = pAllItem->iMaxDam;
	h->_iAC = pAllItem->iMinAC;
	h->_iMiscId = pAllItem->iMiscId;
	h->_iSpell = pAllItem->iSpell;

	if (pAllItem->iMiscId == IMISC_STAFF) {
		h->_iCharges = gbIsHellfire ? 18 : 40;
	}

	h->_iMaxCharges = h->_iCharges;
	h->_iDurability = pAllItem->iDurability;
	h->_iMaxDur = pAllItem->iDurability;
	h->_iMinStr = pAllItem->iMinStr;
	h->_iMinMag = pAllItem->iMinMag;
	h->_iMinDex = pAllItem->iMinDex;
	h->_ivalue = pAllItem->iValue;
	h->_iIvalue = pAllItem->iValue;
	h->_iPrePower = IPL_INVALID;
	h->_iSufPower = IPL_INVALID;
	h->_iMagical = ITEM_QUALITY_NORMAL;
	h->IDidx = static_cast<_item_indexes>(idata);
	if (gbIsHellfire)
		h->dwBuff |= CF_HELLFIRE;
}

void GetPlrHandSeed(ItemStruct *h)
{
	h->_iSeed = AdvanceRndSeed();
}

/**
 * @brief Set a new unique seed value on the given item
 * @param pnum Player id
 * @param h Item to update
 */
void GetGoldSeed(int pnum, ItemStruct *h)
{
	int i, ii, s;
	bool doneflag;

	do {
		doneflag = true;
		s = AdvanceRndSeed();
		for (i = 0; i < numitems; i++) {
			ii = itemactive[i];
			if (items[ii]._iSeed == s)
				doneflag = false;
		}
		if (pnum == myplr) {
			for (i = 0; i < plr[pnum]._pNumInv; i++) {
				if (plr[pnum].InvList[i]._iSeed == s)
					doneflag = false;
			}
		}
	} while (!doneflag);

	h->_iSeed = s;
}

void SetPlrHandSeed(ItemStruct *h, int iseed)
{
	h->_iSeed = iseed;
}

int GetGoldCursor(int value)
{
	if (value >= GOLD_MEDIUM_LIMIT)
		return ICURS_GOLD_LARGE;

	if (value <= GOLD_SMALL_LIMIT)
		return ICURS_GOLD_SMALL;

	return ICURS_GOLD_MEDIUM;
}

/**
 * @brief Update the gold cursor on the given gold item
 * @param h The item to update
 */
void SetPlrHandGoldCurs(ItemStruct *h)
{
	h->_iCurs = GetGoldCursor(h->_ivalue);
}

void CreatePlrItems(int playerId)
{
	auto &player = plr[playerId];

	for (auto &item : player.InvBody) {
		item._itype = ITYPE_NONE;
	}

	// converting this to a for loop creates a `rep stosd` instruction,
	// so this probably actually was a memset
	memset(&player.InvGrid, 0, sizeof(player.InvGrid));

	for (auto &item : player.InvList) {
		item._itype = ITYPE_NONE;
	}

	player._pNumInv = 0;

	for (auto &item : player.SpdList) {
		item._itype = ITYPE_NONE;
	}

	switch (player._pClass) {
	case HeroClass::Warrior:
		SetPlrHandItem(&player.InvBody[INVLOC_HAND_LEFT], IDI_WARRIOR);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&player.InvBody[INVLOC_HAND_RIGHT], IDI_WARRSHLD);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_RIGHT]);

#ifdef _DEBUG
		if (!debug_mode_key_w)
#endif
		{
			SetPlrHandItem(&player.HoldItem, IDI_WARRCLUB);
			GetPlrHandSeed(&player.HoldItem);
			AutoPlaceItemInInventory(player, player.HoldItem, true);
		}

		SetPlrHandItem(&player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(&player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	case HeroClass::Rogue:
		SetPlrHandItem(&player.InvBody[INVLOC_HAND_LEFT], IDI_ROGUE);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(&player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	case HeroClass::Sorcerer:
		SetPlrHandItem(&player.InvBody[INVLOC_HAND_LEFT], gbIsHellfire ? IDI_SORCERER : 166);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&player.SpdList[0], gbIsHellfire ? IDI_HEAL : IDI_MANA);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(&player.SpdList[1], gbIsHellfire ? IDI_HEAL : IDI_MANA);
		GetPlrHandSeed(&player.SpdList[1]);
		break;

	case HeroClass::Monk:
		SetPlrHandItem(&player.InvBody[INVLOC_HAND_LEFT], IDI_SHORTSTAFF);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);
		SetPlrHandItem(&player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(&player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	case HeroClass::Bard:
		SetPlrHandItem(&player.InvBody[INVLOC_HAND_LEFT], IDI_BARDSWORD);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&player.InvBody[INVLOC_HAND_RIGHT], IDI_BARDDAGGER);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_RIGHT]);
		SetPlrHandItem(&player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(&player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	case HeroClass::Barbarian:
		SetPlrHandItem(&player.InvBody[INVLOC_HAND_LEFT], 139); // TODO: add more enums to items
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(&player.InvBody[INVLOC_HAND_RIGHT], IDI_WARRSHLD);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_RIGHT]);
		SetPlrHandItem(&player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(&player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	}

	SetPlrHandItem(&player.HoldItem, IDI_GOLD);
	GetPlrHandSeed(&player.HoldItem);

#ifdef _DEBUG
	if (!debug_mode_key_w) {
#endif
		player.HoldItem._ivalue = 100;
		player.HoldItem._iCurs = ICURS_GOLD_SMALL;
		player._pGold = player.HoldItem._ivalue;
		player.InvList[player._pNumInv++] = player.HoldItem;
		player.InvGrid[30] = player._pNumInv;
#ifdef _DEBUG
	} else {
		player.HoldItem._ivalue = GOLD_MAX_LIMIT;
		player.HoldItem._iCurs = ICURS_GOLD_LARGE;
		player._pGold = player.HoldItem._ivalue * 40;
		for (auto &cell : player.InvGrid) {
			GetPlrHandSeed(&player.HoldItem);
			player.InvList[player._pNumInv++] = player.HoldItem;
			cell = player._pNumInv;
		}
	}
#endif

	CalcPlrItemVals(playerId, false);
}

bool ItemSpaceOk(Point position)
{
	int oi;

	// BUGFIX: Check `i + 1 >= MAXDUNX` and `j + 1 >= MAXDUNY` (applied)
	if (position.x < 0 || position.x + 1 >= MAXDUNX || position.y < 0 || position.y + 1 >= MAXDUNY)
		return false;

	if (dMonster[position.x][position.y] != 0)
		return false;

	if (dPlayer[position.x][position.y] != 0)
		return false;

	if (dItem[position.x][position.y] != 0)
		return false;

	if (dObject[position.x][position.y] != 0) {
		oi = dObject[position.x][position.y] > 0 ? dObject[position.x][position.y] - 1 : -(dObject[position.x][position.y] + 1);
		if (object[oi]._oSolidFlag)
			return false;
	}

	if (dObject[position.x + 1][position.y + 1] > 0 && object[dObject[position.x + 1][position.y + 1] - 1]._oSelFlag != 0)
		return false;

	if (dObject[position.x + 1][position.y + 1] < 0 && object[-(dObject[position.x + 1][position.y + 1] + 1)]._oSelFlag != 0)
		return false;

	if (dObject[position.x + 1][position.y] > 0
	    && dObject[position.x][position.y + 1] > 0
	    && object[dObject[position.x + 1][position.y] - 1]._oSelFlag != 0
	    && object[dObject[position.x][position.y + 1] - 1]._oSelFlag != 0) {
		return false;
	}

	return !nSolidTable[dPiece[position.x][position.y]];
}

static bool GetItemSpace(Point position, int8_t inum)
{
	int rs;
	int xx, yy;
	bool savail;

	yy = 0;
	for (int j = position.y - 1; j <= position.y + 1; j++) {
		xx = 0;
		for (int i = position.x - 1; i <= position.x + 1; i++) {
			itemhold[xx][yy] = ItemSpaceOk({ i, j });
			xx++;
		}
		yy++;
	}

	savail = false;
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 3; i++) { // NOLINT(modernize-loop-convert)
			if (itemhold[i][j])
				savail = true;
		}
	}

	rs = GenerateRnd(15) + 1;

	if (!savail)
		return false;

	xx = 0;
	yy = 0;
	while (rs > 0) {
		if (itemhold[xx][yy])
			rs--;
		if (rs <= 0)
			continue;
		xx++;
		if (xx != 3)
			continue;
		xx = 0;
		yy++;
		if (yy == 3)
			yy = 0;
	}

	xx += position.x - 1;
	yy += position.y - 1;
	items[inum].position = { xx, yy };
	dItem[xx][yy] = inum + 1;

	return true;
}

int AllocateItem()
{
	int inum = itemavail[0];
	itemavail[0] = itemavail[MAXITEMS - numitems - 1];
	itemactive[numitems] = inum;
	numitems++;

	memset(&items[inum], 0, sizeof(*items));

	return inum;
}

static void GetSuperItemSpace(Point position, int8_t inum)
{
	Point positionToCheck = position;
	if (GetItemSpace(positionToCheck, inum))
		return;
	for (int k = 2; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			for (int i = -k; i <= k; i++) {
				Point offset = { i, j };
				positionToCheck = position + offset;
				if (!ItemSpaceOk(positionToCheck))
					continue;
				items[inum].position = positionToCheck;
				dItem[positionToCheck.x][positionToCheck.y] = inum + 1;
				return;
			}
		}
	}
}

Point GetSuperItemLoc(Point position)
{
	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			for (int i = -k; i <= k; i++) {
				Point offset = { i, j };
				Point positionToCheck = position + offset;
				if (ItemSpaceOk(positionToCheck)) {
					return positionToCheck;
				}
			}
		}
	}

	return { 0, 0 }; // TODO handle no space for dropping items
}

void CalcItemValue(int i)
{
	int v = items[i]._iVMult1 + items[i]._iVMult2;
	if (v > 0) {
		v *= items[i]._ivalue;
	}
	if (v < 0) {
		v = items[i]._ivalue / v;
	}
	v = items[i]._iVAdd1 + items[i]._iVAdd2 + v;
	items[i]._iIvalue = std::max(v, 1);
}

void GetBookSpell(int i, int lvl)
{
	int rv;

	if (lvl == 0)
		lvl = 1;

	int maxSpells = gbIsHellfire ? MAX_SPELLS : 37;

	rv = GenerateRnd(maxSpells) + 1;

	if (gbIsSpawn && lvl > 5)
		lvl = 5;

	int s = SPL_FIREBOLT;
	enum spell_id bs = SPL_FIREBOLT;
	while (rv > 0) {
		int sLevel = GetSpellBookLevel(static_cast<spell_id>(s));
		if (sLevel != -1 && lvl >= sLevel) {
			rv--;
			bs = static_cast<spell_id>(s);
		}
		s++;
		if (!gbIsMultiplayer) {
			if (s == SPL_RESURRECT)
				s = SPL_TELEKINESIS;
		}
		if (!gbIsMultiplayer) {
			if (s == SPL_HEALOTHER)
				s = SPL_FLARE;
		}
		if (s == maxSpells)
			s = 1;
	}
	strcat(items[i]._iName, _(spelldata[bs].sNameText));
	strcat(items[i]._iIName, _(spelldata[bs].sNameText));
	items[i]._iSpell = bs;
	items[i]._iMinMag = spelldata[bs].sMinInt;
	items[i]._ivalue += spelldata[bs].sBookCost;
	items[i]._iIvalue += spelldata[bs].sBookCost;
	if (spelldata[bs].sType == STYPE_FIRE)
		items[i]._iCurs = ICURS_BOOK_RED;
	else if (spelldata[bs].sType == STYPE_LIGHTNING)
		items[i]._iCurs = ICURS_BOOK_BLUE;
	else if (spelldata[bs].sType == STYPE_MAGIC)
		items[i]._iCurs = ICURS_BOOK_GREY;
}

static bool control_WriteStringToBuffer(const char *str)
{
	return GetLineWidth(str, GameFontSmall, 0) < 125;
}

void GetStaffPower(int i, int lvl, int bs, bool onlygood)
{
	int l[256];
	char istr[128];
	int nl, j, preidx;
	int tmp;

	tmp = GenerateRnd(10);
	preidx = -1;
	if (tmp == 0 || onlygood) {
		nl = 0;
		for (j = 0; PL_Prefix[j].PLPower != IPL_INVALID; j++) {
			if (!IsPrefixValidForItemType(j, PLT_STAFF) || PL_Prefix[j].PLMinLvl > lvl)
				continue;
			if (onlygood && !PL_Prefix[j].PLOk)
				continue;
			l[nl] = j;
			nl++;
			if (PL_Prefix[j].PLDouble) {
				l[nl] = j;
				nl++;
			}
		}
		if (nl != 0) {
			preidx = l[GenerateRnd(nl)];
			sprintf(istr, "%s %s", _(PL_Prefix[preidx].PLName), items[i]._iIName);
			strcpy(items[i]._iIName, istr);
			items[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    i,
			    PL_Prefix[preidx].PLPower,
			    PL_Prefix[preidx].PLParam1,
			    PL_Prefix[preidx].PLParam2,
			    PL_Prefix[preidx].PLMinVal,
			    PL_Prefix[preidx].PLMaxVal,
			    PL_Prefix[preidx].PLMultVal);
			items[i]._iPrePower = PL_Prefix[preidx].PLPower;
		}
	}
	if (!control_WriteStringToBuffer(items[i]._iIName)) {
		strcpy(items[i]._iIName, _(AllItemsList[items[i].IDidx].iSName));
		if (preidx != -1) {
			sprintf(istr, "%s %s", _(PL_Prefix[preidx].PLName), items[i]._iIName);
			strcpy(items[i]._iIName, istr);
		}
		strcpy(istr, fmt::format(_(/* TRANSLATORS: Constructs item names. Format: <Prefix> <Item> of <Suffix>. Example: King's Long Sword of the Whale */ "{:s} of {:s}"), items[i]._iIName, _(spelldata[bs].sNameText)).c_str());
		strcpy(items[i]._iIName, istr);
		if (items[i]._iMagical == ITEM_QUALITY_NORMAL)
			strcpy(items[i]._iName, items[i]._iIName);
	}
	CalcItemValue(i);
}

void GetStaffSpell(int i, int lvl, bool onlygood)
{
	int l, rv, minc, maxc, v;
	char istr[68];

	if (!gbIsHellfire && GenerateRnd(4) == 0) {
		GetItemPower(i, lvl / 2, lvl, PLT_STAFF, onlygood);
	} else {
		int maxSpells = gbIsHellfire ? MAX_SPELLS : 37;
		l = lvl / 2;
		if (l == 0)
			l = 1;
		rv = GenerateRnd(maxSpells) + 1;

		if (gbIsSpawn && lvl > 10)
			lvl = 10;

		int s = SPL_FIREBOLT;
		enum spell_id bs = SPL_NULL;
		while (rv > 0) {
			int sLevel = GetSpellStaffLevel(static_cast<spell_id>(s));
			if (sLevel != -1 && l >= sLevel) {
				rv--;
				bs = static_cast<spell_id>(s);
			}
			s++;
			if (!gbIsMultiplayer && s == SPL_RESURRECT)
				s = SPL_TELEKINESIS;
			if (!gbIsMultiplayer && s == SPL_HEALOTHER)
				s = SPL_FLARE;
			if (s == maxSpells)
				s = SPL_FIREBOLT;
		}
		if (!control_WriteStringToBuffer(istr))
			strcpy(istr, fmt::format(_("{:s} of {:s}"), items[i]._iName, _(spelldata[bs].sNameText)).c_str());
		strcpy(istr, fmt::format(_("Staff of {:s}"), _(spelldata[bs].sNameText)).c_str());
		strcpy(items[i]._iName, istr);
		strcpy(items[i]._iIName, istr);

		minc = spelldata[bs].sStaffMin;
		maxc = spelldata[bs].sStaffMax - minc + 1;
		items[i]._iSpell = bs;
		items[i]._iCharges = minc + GenerateRnd(maxc);
		items[i]._iMaxCharges = items[i]._iCharges;

		items[i]._iMinMag = spelldata[bs].sMinInt;
		v = items[i]._iCharges * spelldata[bs].sStaffCost / 5;
		items[i]._ivalue += v;
		items[i]._iIvalue += v;
		GetStaffPower(i, lvl, bs, onlygood);
	}
}

void GetOilType(int i, int max_lvl)
{
	int cnt, t, j, r;
	char rnd[32];

	if (!gbIsMultiplayer) {
		if (max_lvl == 0)
			max_lvl = 1;
		cnt = 0;

		for (j = 0; j < (int)(sizeof(OilLevels) / sizeof(OilLevels[0])); j++) {
			if (OilLevels[j] <= max_lvl) {
				rnd[cnt] = j;
				cnt++;
			}
		}
		r = GenerateRnd(cnt);
		t = rnd[r];
	} else {
		r = GenerateRnd(2);
		t = (r != 0 ? 6 : 5);
	}
	strcpy(items[i]._iName, _(OilNames[t]));
	strcpy(items[i]._iIName, _(OilNames[t]));
	items[i]._iMiscId = OilMagic[t];
	items[i]._ivalue = OilValues[t];
	items[i]._iIvalue = OilValues[t];
}

void GetItemAttrs(int i, int idata, int lvl)
{
	items[i]._itype = AllItemsList[idata].itype;
	items[i]._iCurs = AllItemsList[idata].iCurs;
	strcpy(items[i]._iName, _(AllItemsList[idata].iName));
	strcpy(items[i]._iIName, _(AllItemsList[idata].iName));
	items[i]._iLoc = AllItemsList[idata].iLoc;
	items[i]._iClass = AllItemsList[idata].iClass;
	items[i]._iMinDam = AllItemsList[idata].iMinDam;
	items[i]._iMaxDam = AllItemsList[idata].iMaxDam;
	items[i]._iAC = AllItemsList[idata].iMinAC + GenerateRnd(AllItemsList[idata].iMaxAC - AllItemsList[idata].iMinAC + 1);
	items[i]._iFlags = AllItemsList[idata].iFlags;
	items[i]._iMiscId = AllItemsList[idata].iMiscId;
	items[i]._iSpell = AllItemsList[idata].iSpell;
	items[i]._iMagical = ITEM_QUALITY_NORMAL;
	items[i]._ivalue = AllItemsList[idata].iValue;
	items[i]._iIvalue = AllItemsList[idata].iValue;
	items[i]._iDurability = AllItemsList[idata].iDurability;
	items[i]._iMaxDur = AllItemsList[idata].iDurability;
	items[i]._iMinStr = AllItemsList[idata].iMinStr;
	items[i]._iMinMag = AllItemsList[idata].iMinMag;
	items[i]._iMinDex = AllItemsList[idata].iMinDex;
	items[i].IDidx = static_cast<_item_indexes>(idata);
	if (gbIsHellfire)
		items[i].dwBuff |= CF_HELLFIRE;
	items[i]._iPrePower = IPL_INVALID;
	items[i]._iSufPower = IPL_INVALID;

	if (items[i]._iMiscId == IMISC_BOOK)
		GetBookSpell(i, lvl);

	if (gbIsHellfire && items[i]._iMiscId == IMISC_OILOF)
		GetOilType(i, lvl);

	if (items[i]._itype != ITYPE_GOLD)
		return;

	int rndv;
	int itemlevel = items_get_currlevel();
	switch (sgGameInitInfo.nDifficulty) {
	case DIFF_NORMAL:
		rndv = 5 * itemlevel + GenerateRnd(10 * itemlevel);
		break;
	case DIFF_NIGHTMARE:
		rndv = 5 * (itemlevel + 16) + GenerateRnd(10 * (itemlevel + 16));
		break;
	case DIFF_HELL:
		rndv = 5 * (itemlevel + 32) + GenerateRnd(10 * (itemlevel + 32));
		break;
	}
	if (leveltype == DTYPE_HELL)
		rndv += rndv / 8;

	items[i]._ivalue = std::min(rndv, GOLD_MAX_LIMIT);
	SetPlrHandGoldCurs(&items[i]);
}

int RndPL(int param1, int param2)
{
	return param1 + GenerateRnd(param2 - param1 + 1);
}

int PLVal(int pv, int p1, int p2, int minv, int maxv)
{
	if (p1 == p2)
		return minv;
	if (minv == maxv)
		return minv;
	return minv + (maxv - minv) * (100 * (pv - p1) / (p2 - p1)) / 100;
}

void SaveItemPower(int i, item_effect_type power, int param1, int param2, int minval, int maxval, int multval)
{
	int r, r2;

	r = RndPL(param1, param2);
	switch (power) {
	case IPL_TOHIT:
		items[i]._iPLToHit += r;
		break;
	case IPL_TOHIT_CURSE:
		items[i]._iPLToHit -= r;
		break;
	case IPL_DAMP:
		items[i]._iPLDam += r;
		break;
	case IPL_DAMP_CURSE:
		items[i]._iPLDam -= r;
		break;
	case IPL_DOPPELGANGER:
		items[i]._iDamAcFlags |= 16;
		[[fallthrough]];
	case IPL_TOHIT_DAMP:
		r = RndPL(param1, param2);
		items[i]._iPLDam += r;
		if (param1 == 20)
			r2 = RndPL(1, 5);
		if (param1 == 36)
			r2 = RndPL(6, 10);
		if (param1 == 51)
			r2 = RndPL(11, 15);
		if (param1 == 66)
			r2 = RndPL(16, 20);
		if (param1 == 81)
			r2 = RndPL(21, 30);
		if (param1 == 96)
			r2 = RndPL(31, 40);
		if (param1 == 111)
			r2 = RndPL(41, 50);
		if (param1 == 126)
			r2 = RndPL(51, 75);
		if (param1 == 151)
			r2 = RndPL(76, 100);
		items[i]._iPLToHit += r2;
		break;
	case IPL_TOHIT_DAMP_CURSE:
		items[i]._iPLDam -= r;
		if (param1 == 25)
			r2 = RndPL(1, 5);
		if (param1 == 50)
			r2 = RndPL(6, 10);
		items[i]._iPLToHit -= r2;
		break;
	case IPL_ACP:
		items[i]._iPLAC += r;
		break;
	case IPL_ACP_CURSE:
		items[i]._iPLAC -= r;
		break;
	case IPL_SETAC:
		items[i]._iAC = r;
		break;
	case IPL_AC_CURSE:
		items[i]._iAC -= r;
		break;
	case IPL_FIRERES:
		items[i]._iPLFR += r;
		break;
	case IPL_LIGHTRES:
		items[i]._iPLLR += r;
		break;
	case IPL_MAGICRES:
		items[i]._iPLMR += r;
		break;
	case IPL_ALLRES:
		items[i]._iPLFR = std::max(items[i]._iPLFR + r, 0);
		items[i]._iPLLR = std::max(items[i]._iPLLR + r, 0);
		items[i]._iPLMR = std::max(items[i]._iPLMR + r, 0);
		break;
	case IPL_SPLLVLADD:
		items[i]._iSplLvlAdd = r;
		break;
	case IPL_CHARGES:
		items[i]._iCharges *= param1;
		items[i]._iMaxCharges = items[i]._iCharges;
		break;
	case IPL_SPELL:
		items[i]._iSpell = static_cast<spell_id>(param1);
		items[i]._iCharges = param2;
		items[i]._iMaxCharges = param2;
		break;
	case IPL_FIREDAM:
		items[i]._iFlags |= ISPL_FIREDAM;
		items[i]._iFlags &= ~ISPL_LIGHTDAM;
		items[i]._iFMinDam = param1;
		items[i]._iFMaxDam = param2;
		items[i]._iLMinDam = 0;
		items[i]._iLMaxDam = 0;
		break;
	case IPL_LIGHTDAM:
		items[i]._iFlags |= ISPL_LIGHTDAM;
		items[i]._iFlags &= ~ISPL_FIREDAM;
		items[i]._iLMinDam = param1;
		items[i]._iLMaxDam = param2;
		items[i]._iFMinDam = 0;
		items[i]._iFMaxDam = 0;
		break;
	case IPL_STR:
		items[i]._iPLStr += r;
		break;
	case IPL_STR_CURSE:
		items[i]._iPLStr -= r;
		break;
	case IPL_MAG:
		items[i]._iPLMag += r;
		break;
	case IPL_MAG_CURSE:
		items[i]._iPLMag -= r;
		break;
	case IPL_DEX:
		items[i]._iPLDex += r;
		break;
	case IPL_DEX_CURSE:
		items[i]._iPLDex -= r;
		break;
	case IPL_VIT:
		items[i]._iPLVit += r;
		break;
	case IPL_VIT_CURSE:
		items[i]._iPLVit -= r;
		break;
	case IPL_ATTRIBS:
		items[i]._iPLStr += r;
		items[i]._iPLMag += r;
		items[i]._iPLDex += r;
		items[i]._iPLVit += r;
		break;
	case IPL_ATTRIBS_CURSE:
		items[i]._iPLStr -= r;
		items[i]._iPLMag -= r;
		items[i]._iPLDex -= r;
		items[i]._iPLVit -= r;
		break;
	case IPL_GETHIT_CURSE:
		items[i]._iPLGetHit += r;
		break;
	case IPL_GETHIT:
		items[i]._iPLGetHit -= r;
		break;
	case IPL_LIFE:
		items[i]._iPLHP += r << 6;
		break;
	case IPL_LIFE_CURSE:
		items[i]._iPLHP -= r << 6;
		break;
	case IPL_MANA:
		items[i]._iPLMana += r << 6;
		drawmanaflag = true;
		break;
	case IPL_MANA_CURSE:
		items[i]._iPLMana -= r << 6;
		drawmanaflag = true;
		break;
	case IPL_DUR:
		r2 = r * items[i]._iMaxDur / 100;
		items[i]._iMaxDur += r2;
		items[i]._iDurability += r2;
		break;
	case IPL_CRYSTALLINE:
		items[i]._iPLDam += 140 + r * 2;
		[[fallthrough]];
	case IPL_DUR_CURSE:
		items[i]._iMaxDur -= r * items[i]._iMaxDur / 100;
		items[i]._iMaxDur = std::max<uint8_t>(items[i]._iMaxDur, 1);

		items[i]._iDurability = items[i]._iMaxDur;
		break;
	case IPL_INDESTRUCTIBLE:
		items[i]._iDurability = DUR_INDESTRUCTIBLE;
		items[i]._iMaxDur = DUR_INDESTRUCTIBLE;
		break;
	case IPL_LIGHT:
		items[i]._iPLLight += param1;
		break;
	case IPL_LIGHT_CURSE:
		items[i]._iPLLight -= param1;
		break;
	case IPL_MULT_ARROWS:
		items[i]._iFlags |= ISPL_MULT_ARROWS;
		break;
	case IPL_FIRE_ARROWS:
		items[i]._iFlags |= ISPL_FIRE_ARROWS;
		items[i]._iFlags &= ~ISPL_LIGHT_ARROWS;
		items[i]._iFMinDam = param1;
		items[i]._iFMaxDam = param2;
		items[i]._iLMinDam = 0;
		items[i]._iLMaxDam = 0;
		break;
	case IPL_LIGHT_ARROWS:
		items[i]._iFlags |= ISPL_LIGHT_ARROWS;
		items[i]._iFlags &= ~ISPL_FIRE_ARROWS;
		items[i]._iLMinDam = param1;
		items[i]._iLMaxDam = param2;
		items[i]._iFMinDam = 0;
		items[i]._iFMaxDam = 0;
		break;
	case IPL_FIREBALL:
		items[i]._iFlags |= (ISPL_LIGHT_ARROWS | ISPL_FIRE_ARROWS);
		items[i]._iFMinDam = param1;
		items[i]._iFMaxDam = param2;
		items[i]._iLMinDam = 0;
		items[i]._iLMaxDam = 0;
		break;
	case IPL_THORNS:
		items[i]._iFlags |= ISPL_THORNS;
		break;
	case IPL_NOMANA:
		items[i]._iFlags |= ISPL_NOMANA;
		drawmanaflag = true;
		break;
	case IPL_NOHEALPLR:
		items[i]._iFlags |= ISPL_NOHEALPLR;
		break;
	case IPL_ABSHALFTRAP:
		items[i]._iFlags |= ISPL_ABSHALFTRAP;
		break;
	case IPL_KNOCKBACK:
		items[i]._iFlags |= ISPL_KNOCKBACK;
		break;
	case IPL_3XDAMVDEM:
		items[i]._iFlags |= ISPL_3XDAMVDEM;
		break;
	case IPL_ALLRESZERO:
		items[i]._iFlags |= ISPL_ALLRESZERO;
		break;
	case IPL_NOHEALMON:
		items[i]._iFlags |= ISPL_NOHEALMON;
		break;
	case IPL_STEALMANA:
		if (param1 == 3)
			items[i]._iFlags |= ISPL_STEALMANA_3;
		if (param1 == 5)
			items[i]._iFlags |= ISPL_STEALMANA_5;
		drawmanaflag = true;
		break;
	case IPL_STEALLIFE:
		if (param1 == 3)
			items[i]._iFlags |= ISPL_STEALLIFE_3;
		if (param1 == 5)
			items[i]._iFlags |= ISPL_STEALLIFE_5;
		drawhpflag = true;
		break;
	case IPL_TARGAC:
		if (gbIsHellfire)
			items[i]._iPLEnAc = param1;
		else
			items[i]._iPLEnAc += r;
		break;
	case IPL_FASTATTACK:
		if (param1 == 1)
			items[i]._iFlags |= ISPL_QUICKATTACK;
		if (param1 == 2)
			items[i]._iFlags |= ISPL_FASTATTACK;
		if (param1 == 3)
			items[i]._iFlags |= ISPL_FASTERATTACK;
		if (param1 == 4)
			items[i]._iFlags |= ISPL_FASTESTATTACK;
		break;
	case IPL_FASTRECOVER:
		if (param1 == 1)
			items[i]._iFlags |= ISPL_FASTRECOVER;
		if (param1 == 2)
			items[i]._iFlags |= ISPL_FASTERRECOVER;
		if (param1 == 3)
			items[i]._iFlags |= ISPL_FASTESTRECOVER;
		break;
	case IPL_FASTBLOCK:
		items[i]._iFlags |= ISPL_FASTBLOCK;
		break;
	case IPL_DAMMOD:
		items[i]._iPLDamMod += r;
		break;
	case IPL_RNDARROWVEL:
		items[i]._iFlags |= ISPL_RNDARROWVEL;
		break;
	case IPL_SETDAM:
		items[i]._iMinDam = param1;
		items[i]._iMaxDam = param2;
		break;
	case IPL_SETDUR:
		items[i]._iDurability = param1;
		items[i]._iMaxDur = param1;
		break;
	case IPL_FASTSWING:
		items[i]._iFlags |= ISPL_FASTERATTACK;
		break;
	case IPL_ONEHAND:
		items[i]._iLoc = ILOC_ONEHAND;
		break;
	case IPL_DRAINLIFE:
		items[i]._iFlags |= ISPL_DRAINLIFE;
		break;
	case IPL_RNDSTEALLIFE:
		items[i]._iFlags |= ISPL_RNDSTEALLIFE;
		break;
	case IPL_INFRAVISION:
		items[i]._iFlags |= ISPL_INFRAVISION;
		break;
	case IPL_NOMINSTR:
		items[i]._iMinStr = 0;
		break;
	case IPL_INVCURS:
		items[i]._iCurs = param1;
		break;
	case IPL_ADDACLIFE:
		items[i]._iFlags |= (ISPL_LIGHT_ARROWS | ISPL_FIRE_ARROWS);
		items[i]._iFMinDam = param1;
		items[i]._iFMaxDam = param2;
		items[i]._iLMinDam = 1;
		items[i]._iLMaxDam = 0;
		break;
	case IPL_ADDMANAAC:
		items[i]._iFlags |= (ISPL_LIGHTDAM | ISPL_FIREDAM);
		items[i]._iFMinDam = param1;
		items[i]._iFMaxDam = param2;
		items[i]._iLMinDam = 2;
		items[i]._iLMaxDam = 0;
		break;
	case IPL_FIRERESCLVL:
		items[i]._iPLFR = 30 - plr[myplr]._pLevel;
		items[i]._iPLFR = std::max<int16_t>(items[i]._iPLFR, 0);

		break;
	case IPL_FIRERES_CURSE:
		items[i]._iPLFR -= r;
		break;
	case IPL_LIGHTRES_CURSE:
		items[i]._iPLLR -= r;
		break;
	case IPL_MAGICRES_CURSE:
		items[i]._iPLMR -= r;
		break;
	case IPL_ALLRES_CURSE:
		items[i]._iPLFR -= r;
		items[i]._iPLLR -= r;
		items[i]._iPLMR -= r;
		break;
	case IPL_DEVASTATION:
		items[i]._iDamAcFlags |= 0x01;
		break;
	case IPL_DECAY:
		items[i]._iDamAcFlags |= 0x02;
		items[i]._iPLDam += r;
		break;
	case IPL_PERIL:
		items[i]._iDamAcFlags |= 0x04;
		break;
	case IPL_JESTERS:
		items[i]._iDamAcFlags |= 0x08;
		break;
	case IPL_ACDEMON:
		items[i]._iDamAcFlags |= 0x20;
		break;
	case IPL_ACUNDEAD:
		items[i]._iDamAcFlags |= 0x40;
		break;
	case IPL_MANATOLIFE:
		r2 = ((plr[myplr]._pMaxManaBase >> 6) * 50 / 100);
		items[i]._iPLMana -= (r2 << 6);
		items[i]._iPLHP += (r2 << 6);
		break;
	case IPL_LIFETOMANA:
		r2 = ((plr[myplr]._pMaxHPBase >> 6) * 40 / 100);
		items[i]._iPLHP -= (r2 << 6);
		items[i]._iPLMana += (r2 << 6);
		break;
	default:
		break;
	}
	if (items[i]._iVAdd1 != 0 || items[i]._iVMult1 != 0) {
		items[i]._iVAdd2 = PLVal(r, param1, param2, minval, maxval);
		items[i]._iVMult2 = multval;
	} else {
		items[i]._iVAdd1 = PLVal(r, param1, param2, minval, maxval);
		items[i]._iVMult1 = multval;
	}
}

static void SaveItemSuffix(int i, int sufidx)
{
	int param1 = PL_Suffix[sufidx].PLParam1;
	int param2 = PL_Suffix[sufidx].PLParam2;

	if (!gbIsHellfire) {
		if (sufidx >= 84 && sufidx <= 86) {
			param1 = 2 << param1;
			param2 = 6 << param2;
		}
	}

	SaveItemPower(
	    i,
	    PL_Suffix[sufidx].PLPower,
	    param1,
	    param2,
	    PL_Suffix[sufidx].PLMinVal,
	    PL_Suffix[sufidx].PLMaxVal,
	    PL_Suffix[sufidx].PLMultVal);
}

void GetItemPower(int i, int minlvl, int maxlvl, affix_item_type flgs, bool onlygood)
{
	int l[256];
	char istr[128];
	goodorevil goe;

	int pre = GenerateRnd(4);
	int post = GenerateRnd(3);
	if (pre != 0 && post == 0) {
		if (GenerateRnd(2) != 0)
			post = 1;
		else
			pre = 0;
	}
	int preidx = -1;
	int sufidx = -1;
	goe = GOE_ANY;
	if (!onlygood && GenerateRnd(3) != 0)
		onlygood = true;
	if (pre == 0) {
		int nt = 0;
		for (int j = 0; PL_Prefix[j].PLPower != IPL_INVALID; j++) {
			if (!IsPrefixValidForItemType(j, flgs))
				continue;
			if (PL_Prefix[j].PLMinLvl < minlvl || PL_Prefix[j].PLMinLvl > maxlvl)
				continue;
			if (onlygood && !PL_Prefix[j].PLOk)
				continue;
			if (flgs == PLT_STAFF && PL_Prefix[j].PLPower == IPL_CHARGES)
				continue;
			l[nt] = j;
			nt++;
			if (PL_Prefix[j].PLDouble) {
				l[nt] = j;
				nt++;
			}
		}
		if (nt != 0) {
			preidx = l[GenerateRnd(nt)];
			sprintf(istr, "%s %s", _(PL_Prefix[preidx].PLName), items[i]._iIName);
			strcpy(items[i]._iIName, istr);
			items[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemPower(
			    i,
			    PL_Prefix[preidx].PLPower,
			    PL_Prefix[preidx].PLParam1,
			    PL_Prefix[preidx].PLParam2,
			    PL_Prefix[preidx].PLMinVal,
			    PL_Prefix[preidx].PLMaxVal,
			    PL_Prefix[preidx].PLMultVal);
			items[i]._iPrePower = PL_Prefix[preidx].PLPower;
			goe = PL_Prefix[preidx].PLGOE;
		}
	}
	if (post != 0) {
		int nl = 0;
		for (int j = 0; PL_Suffix[j].PLPower != IPL_INVALID; j++) {
			if (IsSuffixValidForItemType(j, flgs)
			    && PL_Suffix[j].PLMinLvl >= minlvl && PL_Suffix[j].PLMinLvl <= maxlvl
			    && !((goe == GOE_GOOD && PL_Suffix[j].PLGOE == GOE_EVIL) || (goe == GOE_EVIL && PL_Suffix[j].PLGOE == GOE_GOOD))
			    && (!onlygood || PL_Suffix[j].PLOk)) {
				l[nl] = j;
				nl++;
			}
		}
		if (nl != 0) {
			sufidx = l[GenerateRnd(nl)];
			strcpy(istr, fmt::format(_("{:s} of {:s}"), items[i]._iIName, _(PL_Suffix[sufidx].PLName)).c_str());
			strcpy(items[i]._iIName, istr);
			items[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemSuffix(i, sufidx);
			items[i]._iSufPower = PL_Suffix[sufidx].PLPower;
		}
	}
	if (!control_WriteStringToBuffer(items[i]._iIName)) {
		int aii = items[i].IDidx;
		if (AllItemsList[aii].iSName != nullptr)
			strcpy(items[i]._iIName, _(AllItemsList[aii].iSName));
		else
			items[i]._iName[0] = 0;

		if (preidx != -1) {
			sprintf(istr, "%s %s", _(PL_Prefix[preidx].PLName), items[i]._iIName);
			strcpy(items[i]._iIName, istr);
		}
		if (sufidx != -1) {
			strcpy(istr, fmt::format(_("{:s} of {:s}"), items[i]._iIName, _(PL_Suffix[sufidx].PLName)).c_str());
			strcpy(items[i]._iIName, istr);
		}
	}
	if (preidx != -1 || sufidx != -1)
		CalcItemValue(i);
}

void GetItemBonus(int i, int minlvl, int maxlvl, bool onlygood, bool allowspells)
{
	if (minlvl > 25)
		minlvl = 25;

	switch (items[i]._itype) {
	case ITYPE_SWORD:
	case ITYPE_AXE:
	case ITYPE_MACE:
		GetItemPower(i, minlvl, maxlvl, PLT_WEAP, onlygood);
		break;
	case ITYPE_BOW:
		GetItemPower(i, minlvl, maxlvl, PLT_BOW, onlygood);
		break;
	case ITYPE_SHIELD:
		GetItemPower(i, minlvl, maxlvl, PLT_SHLD, onlygood);
		break;
	case ITYPE_LARMOR:
	case ITYPE_HELM:
	case ITYPE_MARMOR:
	case ITYPE_HARMOR:
		GetItemPower(i, minlvl, maxlvl, PLT_ARMO, onlygood);
		break;
	case ITYPE_STAFF:
		if (allowspells)
			GetStaffSpell(i, maxlvl, onlygood);
		else
			GetItemPower(i, minlvl, maxlvl, PLT_STAFF, onlygood);
		break;
	case ITYPE_RING:
	case ITYPE_AMULET:
		GetItemPower(i, minlvl, maxlvl, PLT_MISC, onlygood);
		break;
	case ITYPE_NONE:
	case ITYPE_MISC:
	case ITYPE_GOLD:
		break;
	}
}

void SetupItem(int i)
{
	items[i].SetNewAnimation(plr[myplr].pLvlLoad == 0);
	items[i]._iIdentified = false;
}

int RndItem(int m)
{
	int i, ri, r;
	int ril[512];

	if ((monster[m].MData->mTreasure & 0x8000) != 0)
		return -((monster[m].MData->mTreasure & 0xFFF) + 1);

	if ((monster[m].MData->mTreasure & 0x4000) != 0)
		return 0;

	if (GenerateRnd(100) > 40)
		return 0;

	if (GenerateRnd(100) > 25)
		return IDI_GOLD + 1;

	ri = 0;
	for (i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (!IsItemAvailable(i))
			continue;

		if (AllItemsList[i].iRnd == IDROP_DOUBLE && monster[m].mLevel >= AllItemsList[i].iMinMLvl
		    && ri < 512) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iRnd != IDROP_NEVER && monster[m].mLevel >= AllItemsList[i].iMinMLvl
		    && ri < 512) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iSpell == SPL_RESURRECT && !gbIsMultiplayer)
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && !gbIsMultiplayer)
			ri--;
	}

	r = GenerateRnd(ri);
	return ril[r] + 1;
}

int RndUItem(int m)
{
	int ril[512];
	bool okflag;

	if (m != -1 && (monster[m].MData->mTreasure & 0x8000) != 0 && !gbIsMultiplayer)
		return -((monster[m].MData->mTreasure & 0xFFF) + 1);

	int curlv = items_get_currlevel();
	int ri = 0;
	for (int i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (!IsItemAvailable(i))
			continue;

		okflag = true;
		if (AllItemsList[i].iRnd == IDROP_NEVER)
			okflag = false;
		if (m != -1) {
			if (monster[m].mLevel < AllItemsList[i].iMinMLvl)
				okflag = false;
		} else {
			if (2 * curlv < AllItemsList[i].iMinMLvl)
				okflag = false;
		}
		if (AllItemsList[i].itype == ITYPE_MISC)
			okflag = false;
		if (AllItemsList[i].itype == ITYPE_GOLD)
			okflag = false;
		if (AllItemsList[i].iMiscId == IMISC_BOOK)
			okflag = true;
		if (AllItemsList[i].iSpell == SPL_RESURRECT && !gbIsMultiplayer)
			okflag = false;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && !gbIsMultiplayer)
			okflag = false;
		if (okflag && ri < 512) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[GenerateRnd(ri)];
}

int RndAllItems()
{
	int ril[512];

	if (GenerateRnd(100) > 25)
		return 0;

	int curlv = items_get_currlevel();
	int ri = 0;
	for (int i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (!IsItemAvailable(i))
			continue;

		if (AllItemsList[i].iRnd != IDROP_NEVER && 2 * curlv >= AllItemsList[i].iMinMLvl && ri < 512) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iSpell == SPL_RESURRECT && !gbIsMultiplayer)
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && !gbIsMultiplayer)
			ri--;
	}

	return ril[GenerateRnd(ri)];
}

int RndTypeItems(int itype, int imid, int lvl)
{
	int ril[512];

	int ri = 0;
	for (int i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (!IsItemAvailable(i))
			continue;

		bool okflag = true;
		if (AllItemsList[i].iRnd == IDROP_NEVER)
			okflag = false;
		if (lvl * 2 < AllItemsList[i].iMinMLvl)
			okflag = false;
		if (AllItemsList[i].itype != itype)
			okflag = false;
		if (imid != -1 && AllItemsList[i].iMiscId != imid)
			okflag = false;
		if (okflag && ri < 512) {
			ril[ri] = i;
			ri++;
		}
	}

	return ril[GenerateRnd(ri)];
}

_unique_items CheckUnique(int i, int lvl, int uper, bool recreate)
{
	std::bitset<128> uok = {};

	if (GenerateRnd(100) > uper)
		return UITEM_INVALID;

	int numu = 0;
	for (int j = 0; UniqueItemList[j].UIItemId != UITYPE_INVALID; j++) {
		if (!IsUniqueAvailable(j))
			break;
		if (UniqueItemList[j].UIItemId == AllItemsList[items[i].IDidx].iItemId
		    && lvl >= UniqueItemList[j].UIMinLvl
		    && (recreate || !UniqueItemFlags[j] || gbIsMultiplayer)) {
			uok[j] = true;
			numu++;
		}
	}

	if (numu == 0)
		return UITEM_INVALID;

	GenerateRnd(10); /// BUGFIX: unused, last unique in array always gets chosen
	uint8_t idata = 0;
	while (numu > 0) {
		if (uok[idata])
			numu--;
		if (numu > 0)
			idata = (idata + 1) % 128;
	}

	return (_unique_items)idata;
}

void GetUniqueItem(int i, _unique_items uid)
{
	UniqueItemFlags[uid] = true;
	SaveItemPower(i, UniqueItemList[uid].UIPower1, UniqueItemList[uid].UIParam1, UniqueItemList[uid].UIParam2, 0, 0, 1);

	if (UniqueItemList[uid].UINumPL > 1)
		SaveItemPower(i, UniqueItemList[uid].UIPower2, UniqueItemList[uid].UIParam3, UniqueItemList[uid].UIParam4, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 2)
		SaveItemPower(i, UniqueItemList[uid].UIPower3, UniqueItemList[uid].UIParam5, UniqueItemList[uid].UIParam6, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 3)
		SaveItemPower(i, UniqueItemList[uid].UIPower4, UniqueItemList[uid].UIParam7, UniqueItemList[uid].UIParam8, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 4)
		SaveItemPower(i, UniqueItemList[uid].UIPower5, UniqueItemList[uid].UIParam9, UniqueItemList[uid].UIParam10, 0, 0, 1);
	if (UniqueItemList[uid].UINumPL > 5)
		SaveItemPower(i, UniqueItemList[uid].UIPower6, UniqueItemList[uid].UIParam11, UniqueItemList[uid].UIParam12, 0, 0, 1);

	strcpy(items[i]._iIName, _(UniqueItemList[uid].UIName));
	items[i]._iIvalue = UniqueItemList[uid].UIValue;

	if (items[i]._iMiscId == IMISC_UNIQUE)
		items[i]._iSeed = uid;

	items[i]._iUid = uid;
	items[i]._iMagical = ITEM_QUALITY_UNIQUE;
	items[i]._iCreateInfo |= CF_UNIQUE;
}

void SpawnUnique(_unique_items uid, Point position)
{
	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();
	GetSuperItemSpace(position, ii);
	int curlv = items_get_currlevel();

	int idx = 0;
	while (AllItemsList[idx].iItemId != UniqueItemList[uid].UIItemId)
		idx++;

	GetItemAttrs(ii, idx, curlv);
	GetUniqueItem(ii, uid);
	SetupItem(ii);
}

void ItemRndDur(int ii)
{
	if (items[ii]._iDurability > 0 && items[ii]._iDurability != DUR_INDESTRUCTIBLE)
		items[ii]._iDurability = GenerateRnd(items[ii]._iMaxDur / 2) + (items[ii]._iMaxDur / 4) + 1;
}

void SetupAllItems(int ii, int idx, int iseed, int lvl, int uper, bool onlygood, bool recreate, bool pregen)
{
	int iblvl;

	items[ii]._iSeed = iseed;
	SetRndSeed(iseed);
	GetItemAttrs(ii, idx, lvl / 2);
	items[ii]._iCreateInfo = lvl;

	if (pregen)
		items[ii]._iCreateInfo |= CF_PREGEN;
	if (onlygood)
		items[ii]._iCreateInfo |= CF_ONLYGOOD;

	if (uper == 15)
		items[ii]._iCreateInfo |= CF_UPER15;
	else if (uper == 1)
		items[ii]._iCreateInfo |= CF_UPER1;

	if (items[ii]._iMiscId != IMISC_UNIQUE) {
		iblvl = -1;
		if (GenerateRnd(100) <= 10 || GenerateRnd(100) <= lvl) {
			iblvl = lvl;
		}
		if (iblvl == -1 && items[ii]._iMiscId == IMISC_STAFF) {
			iblvl = lvl;
		}
		if (iblvl == -1 && items[ii]._iMiscId == IMISC_RING) {
			iblvl = lvl;
		}
		if (iblvl == -1 && items[ii]._iMiscId == IMISC_AMULET) {
			iblvl = lvl;
		}
		if (onlygood)
			iblvl = lvl;
		if (uper == 15)
			iblvl = lvl + 4;
		if (iblvl != -1) {
			_unique_items uid = CheckUnique(ii, iblvl, uper, recreate);
			if (uid == UITEM_INVALID) {
				GetItemBonus(ii, iblvl / 2, iblvl, onlygood, true);
			} else {
				GetUniqueItem(ii, uid);
			}
		}
		if (items[ii]._iMagical != ITEM_QUALITY_UNIQUE)
			ItemRndDur(ii);
	} else {
		if (items[ii]._iLoc != ILOC_UNEQUIPABLE) {
			GetUniqueItem(ii, (_unique_items)iseed); // uid is stored in iseed for uniques
		}
	}
	SetupItem(ii);
}

void SpawnItem(int m, Point position, bool sendmsg)
{
	int idx;
	bool onlygood = true;

	if (monster[m]._uniqtype != 0 || ((monster[m].MData->mTreasure & 0x8000) != 0 && gbIsMultiplayer)) {
		idx = RndUItem(m);
		if (idx < 0) {
			SpawnUnique((_unique_items) - (idx + 1), position);
			return;
		}
		onlygood = true;
	} else if (quests[Q_MUSHROOM]._qactive != QUEST_ACTIVE || quests[Q_MUSHROOM]._qvar1 != QS_MUSHGIVEN) {
		idx = RndItem(m);
		if (idx == 0)
			return;
		if (idx > 0) {
			idx--;
			onlygood = false;
		} else {
			SpawnUnique((_unique_items) - (idx + 1), position);
			return;
		}
	} else {
		idx = IDI_BRAIN;
		quests[Q_MUSHROOM]._qvar1 = QS_BRAINSPAWNED;
	}

	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();
	GetSuperItemSpace(position, ii);
	int uper = monster[m]._uniqtype != 0 ? 15 : 1;

	int mLevel = monster[m].MData->mLevel;
	if (!gbIsHellfire && monster[m].MType->mtype == MT_DIABLO)
		mLevel -= 15;

	SetupAllItems(ii, idx, AdvanceRndSeed(), mLevel, uper, onlygood, false, false);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
}

static void SetupBaseItem(Point position, int idx, bool onlygood, bool sendmsg, bool delta)
{
	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();
	GetSuperItemSpace(position, ii);
	int curlv = items_get_currlevel();

	SetupAllItems(ii, idx, AdvanceRndSeed(), 2 * curlv, 1, onlygood, false, delta);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
	if (delta)
		DeltaAddItem(ii);
}

void CreateRndItem(Point position, bool onlygood, bool sendmsg, bool delta)
{
	int idx = onlygood ? RndUItem(-1) : RndAllItems();

	SetupBaseItem(position, idx, onlygood, sendmsg, delta);
}

void SetupAllUseful(int ii, int iseed, int lvl)
{
	int idx;

	items[ii]._iSeed = iseed;
	SetRndSeed(iseed);

	if (gbIsHellfire) {
		idx = GenerateRnd(7);
		switch (idx) {
		case 0:
			idx = IDI_PORTAL;
			if ((lvl <= 1))
				idx = IDI_HEAL;
			break;
		case 1:
		case 2:
			idx = IDI_HEAL;
			break;
		case 3:
			idx = IDI_PORTAL;
			if ((lvl <= 1))
				idx = IDI_MANA;
			break;
		case 4:
		case 5:
			idx = IDI_MANA;
			break;
		default:
			idx = IDI_OIL;
			break;
		}
	} else {
		if (GenerateRnd(2) != 0)
			idx = IDI_HEAL;
		else
			idx = IDI_MANA;

		if (lvl > 1 && GenerateRnd(3) == 0)
			idx = IDI_PORTAL;
	}

	GetItemAttrs(ii, idx, lvl);
	items[ii]._iCreateInfo = lvl | CF_USEFUL;
	SetupItem(ii);
}

void CreateRndUseful(Point position, bool sendmsg)
{
	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();
	GetSuperItemSpace(position, ii);
	int curlv = items_get_currlevel();

	SetupAllUseful(ii, AdvanceRndSeed(), curlv);
	if (sendmsg)
		NetSendCmdDItem(false, ii);
}

void CreateTypeItem(Point position, bool onlygood, int itype, int imisc, bool sendmsg, bool delta)
{
	int idx;

	int curlv = items_get_currlevel();
	if (itype != ITYPE_GOLD)
		idx = RndTypeItems(itype, imisc, curlv);
	else
		idx = IDI_GOLD;

	SetupBaseItem(position, idx, onlygood, sendmsg, delta);
}

void RecreateItem(int ii, int idx, uint16_t icreateinfo, int iseed, int ivalue, bool isHellfire)
{
	bool _gbIsHellfire = gbIsHellfire;
	gbIsHellfire = isHellfire;

	if (idx == IDI_GOLD) {
		SetPlrHandItem(&items[ii], IDI_GOLD);
		items[ii]._iSeed = iseed;
		items[ii]._iCreateInfo = icreateinfo;
		items[ii]._ivalue = ivalue;
		SetPlrHandGoldCurs(&items[ii]);
		gbIsHellfire = _gbIsHellfire;
		return;
	}

	if (icreateinfo == 0) {
		SetPlrHandItem(&items[ii], idx);
		SetPlrHandSeed(&items[ii], iseed);
		gbIsHellfire = _gbIsHellfire;
		return;
	}

	if ((icreateinfo & CF_UNIQUE) == 0) {
		if ((icreateinfo & CF_TOWN) != 0) {
			RecreateTownItem(ii, idx, icreateinfo, iseed);
			gbIsHellfire = _gbIsHellfire;
			return;
		}

		if ((icreateinfo & CF_USEFUL) == CF_USEFUL) {
			SetupAllUseful(ii, iseed, icreateinfo & CF_LEVEL);
			gbIsHellfire = _gbIsHellfire;
			return;
		}
	}

	int level = icreateinfo & CF_LEVEL;

	int uper = 0;
	if ((icreateinfo & CF_UPER1) != 0)
		uper = 1;
	if ((icreateinfo & CF_UPER15) != 0)
		uper = 15;

	bool onlygood = (icreateinfo & CF_ONLYGOOD) != 0;
	bool recreate = (icreateinfo & CF_UNIQUE) != 0;
	bool pregen = (icreateinfo & CF_PREGEN) != 0;

	SetupAllItems(ii, idx, iseed, level, uper, onlygood, recreate, pregen);
	gbIsHellfire = _gbIsHellfire;
}

void RecreateEar(int ii, uint16_t ic, int iseed, int Id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff)
{
	SetPlrHandItem(&items[ii], IDI_EAR);
	tempstr[0] = (ic >> 8) & 0x7F;
	tempstr[1] = ic & 0x7F;
	tempstr[2] = (iseed >> 24) & 0x7F;
	tempstr[3] = (iseed >> 16) & 0x7F;
	tempstr[4] = (iseed >> 8) & 0x7F;
	tempstr[5] = iseed & 0x7F;
	tempstr[6] = Id & 0x7F;
	tempstr[7] = dur & 0x7F;
	tempstr[8] = mdur & 0x7F;
	tempstr[9] = ch & 0x7F;
	tempstr[10] = mch & 0x7F;
	tempstr[11] = (ivalue >> 8) & 0x7F;
	tempstr[12] = (ibuff >> 24) & 0x7F;
	tempstr[13] = (ibuff >> 16) & 0x7F;
	tempstr[14] = (ibuff >> 8) & 0x7F;
	tempstr[15] = ibuff & 0x7F;
	tempstr[16] = '\0';
	strcpy(items[ii]._iName, fmt::format(_(/* TRANSLATORS: {:s} will be a Character Name */ "Ear of {:s}"), tempstr).c_str());
	items[ii]._iCurs = ((ivalue >> 6) & 3) + ICURS_EAR_SORCERER;
	items[ii]._ivalue = ivalue & 0x3F;
	items[ii]._iCreateInfo = ic;
	items[ii]._iSeed = iseed;
}

void items_427A72()
{
	if (!CornerStone.activated)
		return;
	if (!CornerStone.item.isEmpty()) {
		PkItemStruct id;
		PackItem(&id, &CornerStone.item);
		BYTE *buffer = (BYTE *)&id;
		for (size_t i = 0; i < sizeof(PkItemStruct); i++) {
			sprintf(&sgOptions.Hellfire.szItem[i * 2], "%02X", buffer[i]);
		}
	} else {
		sgOptions.Hellfire.szItem[0] = '\0';
	}
}

int char2int(char input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	return 0;
}

void hex2bin(const char *src, int bytes, char *target)
{
	for (int i = 0; i < bytes; i++, src += 2) {
		target[i] = (char2int(*src) << 4) | char2int(src[1]);
	}
}

void items_427ABA(Point position)
{
	PkItemStruct PkSItem;

	if (CornerStone.activated || position.x == 0 || position.y == 0) {
		return;
	}

	CornerStone.item._itype = ITYPE_NONE;
	CornerStone.activated = true;
	if (dItem[position.x][position.y] != 0) {
		int ii = dItem[position.x][position.y] - 1;
		for (int i = 0; i < numitems; i++) {
			if (itemactive[i] == ii) {
				DeleteItem(ii, i);
				break;
			}
		}
		dItem[position.x][position.y] = 0;
	}

	if (strlen(sgOptions.Hellfire.szItem) < sizeof(PkItemStruct) * 2)
		return;

	hex2bin(sgOptions.Hellfire.szItem, sizeof(PkItemStruct), (char *)&PkSItem);

	int ii = AllocateItem();

	dItem[position.x][position.y] = ii + 1;

	UnPackItem(&PkSItem, &items[ii], (PkSItem.dwBuff & CF_HELLFIRE) != 0);
	items[ii].position = position;
	RespawnItem(&items[ii], false);
	CornerStone.item = items[ii];
}

void SpawnQuestItem(int itemid, Point position, int randarea, int selflag)
{
	if (randarea > 0) {
		int tries = 0;
		while (true) {
			tries++;
			if (tries > 1000 && randarea > 1)
				randarea--;

			position.x = GenerateRnd(MAXDUNX);
			position.y = GenerateRnd(MAXDUNY);

			bool failed = false;
			for (int i = 0; i < randarea && !failed; i++) {
				for (int j = 0; j < randarea && !failed; j++) {
					failed = !ItemSpaceOk(position + Point { i, j });
				}
			}
			if (!failed)
				break;
		}
	}

	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();

	items[ii].position = position;

	dItem[position.x][position.y] = ii + 1;

	int curlv = items_get_currlevel();
	GetItemAttrs(ii, itemid, curlv);

	SetupItem(ii);
	items[ii]._iSeed = AdvanceRndSeed();
	items[ii]._iPostDraw = true;
	if (selflag != 0) {
		items[ii]._iSelFlag = selflag;
		items[ii].AnimInfo.CurrentFrame = items[ii].AnimInfo.NumberOfFrames;
		items[ii]._iAnimFlag = false;
	}
}

void SpawnRock()
{
	if (numitems >= MAXITEMS)
		return;

	int oi;
	bool ostand = false;
	for (int i = 0; i < nobjects && !ostand; i++) {
		oi = objectactive[i];
		ostand = object[oi]._otype == OBJ_STAND;
	}

	if (!ostand)
		return;

	int ii = AllocateItem();

	items[ii].position = object[oi].position;
	dItem[object[oi].position.x][object[oi].position.y] = ii + 1;
	int curlv = items_get_currlevel();
	GetItemAttrs(ii, IDI_ROCK, curlv);
	SetupItem(ii);
	items[ii]._iSelFlag = 2;
	items[ii]._iPostDraw = true;
	items[ii].AnimInfo.CurrentFrame = 11;
}

void SpawnRewardItem(int itemid, Point position)
{
	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();

	items[ii].position = position;
	dItem[position.x][position.y] = ii + 1;
	int curlv = items_get_currlevel();
	GetItemAttrs(ii, itemid, curlv);
	items[ii].SetNewAnimation(true);
	items[ii]._iSelFlag = 2;
	items[ii]._iPostDraw = true;
	items[ii]._iIdentified = true;
}

void SpawnMapOfDoom(Point position)
{
	SpawnRewardItem(IDI_MAPOFDOOM, position);
}

void SpawnRuneBomb(Point position)
{
	SpawnRewardItem(IDI_RUNEBOMB, position);
}

void SpawnTheodore(Point position)
{
	SpawnRewardItem(IDI_THEODORE, position);
}

void RespawnItem(ItemStruct *item, bool FlipFlag)
{
	int it = ItemCAnimTbl[item->_iCurs];
	item->SetNewAnimation(FlipFlag);
	item->_iRequest = false;

	if (item->_iCurs == ICURS_MAGIC_ROCK) {
		item->_iSelFlag = 1;
		PlaySfxLoc(ItemDropSnds[it], item->position);
	}
	if (item->_iCurs == ICURS_TAVERN_SIGN)
		item->_iSelFlag = 1;
	if (item->_iCurs == ICURS_ANVIL_OF_FURY)
		item->_iSelFlag = 1;
}

void DeleteItem(int ii, int i)
{
	itemavail[MAXITEMS - numitems] = ii;
	numitems--;
	if (numitems > 0 && i != numitems)
		itemactive[i] = itemactive[numitems];
}

void ItemDoppel()
{
	if (!gbIsMultiplayer)
		return;

	static int idoppely = 16;

	for (int idoppelx = 16; idoppelx < 96; idoppelx++) {
		if (dItem[idoppelx][idoppely] != 0) {
			ItemStruct *i = &items[dItem[idoppelx][idoppely] - 1];
			if (i->position.x != idoppelx || i->position.y != idoppely)
				dItem[idoppelx][idoppely] = 0;
		}
	}

	idoppely++;
	if (idoppely == 96)
		idoppely = 16;
}

void ProcessItems()
{
	for (int i = 0; i < numitems; i++) {
		int ii = itemactive[i];
		if (!items[ii]._iAnimFlag)
			continue;
		items[ii].AnimInfo.ProcessAnimation();
		if (items[ii]._iCurs == ICURS_MAGIC_ROCK) {
			if (items[ii]._iSelFlag == 1 && items[ii].AnimInfo.CurrentFrame == 11)
				items[ii].AnimInfo.CurrentFrame = 1;
			if (items[ii]._iSelFlag == 2 && items[ii].AnimInfo.CurrentFrame == 21)
				items[ii].AnimInfo.CurrentFrame = 11;
		} else {
			if (items[ii].AnimInfo.CurrentFrame == items[ii].AnimInfo.NumberOfFrames / 2)
				PlaySfxLoc(ItemDropSnds[ItemCAnimTbl[items[ii]._iCurs]], items[ii].position);

			if (items[ii].AnimInfo.CurrentFrame >= items[ii].AnimInfo.NumberOfFrames) {
				items[ii].AnimInfo.CurrentFrame = items[ii].AnimInfo.NumberOfFrames;
				items[ii]._iAnimFlag = false;
				items[ii]._iSelFlag = 1;
			}
		}
	}
	ItemDoppel();
}

void FreeItemGFX()
{
	for (auto &itemanim : itemanims) {
		itemanim = std::nullopt;
	}
}

void GetItemFrm(int i)
{
	items[i].AnimInfo.pCelSprite = &*itemanims[ItemCAnimTbl[items[i]._iCurs]];
}

void GetItemStr(int i)
{
	if (items[i]._itype != ITYPE_GOLD) {
		if (items[i]._iIdentified)
			strcpy(infostr, items[i]._iIName);
		else
			strcpy(infostr, items[i]._iName);

		infoclr = items[i].getTextColor();
	} else {
		int nGold = items[i]._ivalue;
		strcpy(infostr, fmt::format(ngettext("{:d} gold piece", "{:d} gold pieces", nGold), nGold).c_str());
	}
}

void CheckIdentify(int pnum, int cii)
{
	ItemStruct *pi;

	if (cii >= NUM_INVLOC)
		pi = &plr[pnum].InvList[cii - NUM_INVLOC];
	else
		pi = &plr[pnum].InvBody[cii];

	pi->_iIdentified = true;
	CalcPlrInv(pnum, true);

	if (pnum == myplr)
		NewCursor(CURSOR_HAND);
}

static void RepairItem(ItemStruct *i, int lvl)
{
	if (i->_iDurability == i->_iMaxDur) {
		return;
	}

	if (i->_iMaxDur <= 0) {
		i->_itype = ITYPE_NONE;
		return;
	}

	int rep = 0;
	do {
		rep += lvl + GenerateRnd(lvl);
		i->_iMaxDur -= std::max(i->_iMaxDur / (lvl + 9), 1);
		if (i->_iMaxDur == 0) {
			i->_itype = ITYPE_NONE;
			return;
		}
	} while (rep + i->_iDurability < i->_iMaxDur);

	i->_iDurability = std::min<int>(i->_iDurability + rep, i->_iMaxDur);
}

void DoRepair(int pnum, int cii)
{
	ItemStruct *pi;

	auto &player = plr[pnum];

	PlaySfxLoc(IS_REPAIR, player.position.tile);

	if (cii >= NUM_INVLOC) {
		pi = &player.InvList[cii - NUM_INVLOC];
	} else {
		pi = &player.InvBody[cii];
	}

	RepairItem(pi, player._pLevel);
	CalcPlrInv(pnum, true);

	if (pnum == myplr)
		NewCursor(CURSOR_HAND);
}

static void RechargeItem(ItemStruct *i, int r)
{
	if (i->_iCharges == i->_iMaxCharges)
		return;

	do {
		i->_iMaxCharges--;
		if (i->_iMaxCharges == 0) {
			return;
		}
		i->_iCharges += r;
	} while (i->_iCharges < i->_iMaxCharges);

	i->_iCharges = std::min(i->_iCharges, i->_iMaxCharges);
}

void DoRecharge(int pnum, int cii)
{
	ItemStruct *pi;

	auto &player = plr[pnum];
	if (cii >= NUM_INVLOC) {
		pi = &player.InvList[cii - NUM_INVLOC];
	} else {
		pi = &player.InvBody[cii];
	}
	if (pi->_itype == ITYPE_STAFF && pi->_iSpell != SPL_NULL) {
		int r = GetSpellBookLevel(pi->_iSpell);
		r = GenerateRnd(player._pLevel / r) + 1;
		RechargeItem(pi, r);
		CalcPlrInv(pnum, true);
	}

	if (pnum == myplr)
		NewCursor(CURSOR_HAND);
}

static bool OilItem(ItemStruct *x, PlayerStruct &player)
{
	int r;

	if (x->_iClass == ICLASS_MISC) {
		return false;
	}
	if (x->_iClass == ICLASS_GOLD) {
		return false;
	}
	if (x->_iClass == ICLASS_QUEST) {
		return false;
	}

	switch (player._pOilType) {
	case IMISC_OILACC:
	case IMISC_OILMAST:
	case IMISC_OILSHARP:
		if (x->_iClass == ICLASS_ARMOR) {
			return false;
		}
		break;
	case IMISC_OILDEATH:
		if (x->_iClass == ICLASS_ARMOR) {
			return false;
		}
		if (x->_itype == ITYPE_BOW) {
			return false;
		}
		break;
	case IMISC_OILHARD:
	case IMISC_OILIMP:
		if (x->_iClass == ICLASS_WEAPON) {
			return false;
		}
		break;
	default:
		break;
	}

	switch (player._pOilType) {
	case IMISC_OILACC:
		if (x->_iPLToHit < 50) {
			x->_iPLToHit += GenerateRnd(2) + 1;
		}
		break;
	case IMISC_OILMAST:
		if (x->_iPLToHit < 100) {
			x->_iPLToHit += GenerateRnd(3) + 3;
		}
		break;
	case IMISC_OILSHARP:
		if (x->_iMaxDam - x->_iMinDam < 30) {
			x->_iMaxDam = x->_iMaxDam + 1;
		}
		break;
	case IMISC_OILDEATH:
		if (x->_iMaxDam - x->_iMinDam < 30) {
			x->_iMinDam = x->_iMinDam + 1;
			x->_iMaxDam = x->_iMaxDam + 2;
		}
		break;
	case IMISC_OILSKILL:
		r = GenerateRnd(6) + 5;
		x->_iMinStr = std::max(0, x->_iMinStr - r);
		x->_iMinMag = std::max(0, x->_iMinMag - r);
		x->_iMinDex = std::max(0, x->_iMinDex - r);
		break;
	case IMISC_OILBSMTH:
		if (x->_iMaxDur == 255)
			return true;
		if (x->_iDurability < x->_iMaxDur) {
			x->_iDurability = (x->_iMaxDur + 4) / 5 + x->_iDurability;
			x->_iDurability = std::min<int>(x->_iDurability, x->_iMaxDur);
		} else {
			if (x->_iMaxDur >= 100) {
				return true;
			}
			x->_iMaxDur++;
			x->_iDurability = x->_iMaxDur;
		}
		break;
	case IMISC_OILFORT:
		if (x->_iMaxDur != 255 && x->_iMaxDur < 200) {
			r = GenerateRnd(41) + 10;
			x->_iMaxDur += r;
			x->_iDurability += r;
		}
		break;
	case IMISC_OILPERM:
		x->_iDurability = 255;
		x->_iMaxDur = 255;
		break;
	case IMISC_OILHARD:
		if (x->_iAC < 60) {
			x->_iAC += GenerateRnd(2) + 1;
		}
		break;
	case IMISC_OILIMP:
		if (x->_iAC < 120) {
			x->_iAC += GenerateRnd(3) + 3;
		}
		break;
	default:
		return false;
	}
	return true;
}

void DoOil(int pnum, int cii)
{
	if (cii < NUM_INVLOC && cii != INVLOC_HEAD && (cii <= INVLOC_AMULET || cii > INVLOC_CHEST))
		return;
	auto &player = plr[pnum];
	if (!OilItem(&player.InvBody[cii], player))
		return;
	CalcPlrInv(pnum, true);
	if (pnum == myplr) {
		NewCursor(CURSOR_HAND);
	}
}

void PrintItemOil(char IDidx)
{
	switch (IDidx) {
	case IMISC_OILACC:
		strcpy(tempstr, _("increases a weapon's"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("chance to hit"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILMAST:
		strcpy(tempstr, _("greatly increases a"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("weapon's chance to hit"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILSHARP:
		strcpy(tempstr, _("increases a weapon's"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("damage potential"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILDEATH:
		strcpy(tempstr, _("greatly increases a weapon's"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("damage potential - not bows"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILSKILL:
		strcpy(tempstr, _("reduces attributes needed"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("to use armor or weapons"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILBSMTH:
		/*xgettext:no-c-format*/ strcpy(tempstr, _("restores 20% of an"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("item's durability"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILFORT:
		strcpy(tempstr, _("increases an item's"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("current and max durability"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILPERM:
		strcpy(tempstr, _("makes an item indestructible"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILHARD:
		strcpy(tempstr, _("increases the armor class"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("of armor and shields"));
		AddPanelString(tempstr);
		break;
	case IMISC_OILIMP:
		strcpy(tempstr, _("greatly increases the armor"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("class of armor and shields"));
		AddPanelString(tempstr);
		break;
	case IMISC_RUNEF:
		strcpy(tempstr, _("sets fire trap"));
		AddPanelString(tempstr);
		break;
	case IMISC_RUNEL:
		strcpy(tempstr, _("sets lightning trap"));
		AddPanelString(tempstr);
		break;
	case IMISC_GR_RUNEL:
		strcpy(tempstr, _("sets lightning trap"));
		AddPanelString(tempstr);
		break;
	case IMISC_GR_RUNEF:
		strcpy(tempstr, _("sets fire trap"));
		AddPanelString(tempstr);
		break;
	case IMISC_RUNES:
		strcpy(tempstr, _("sets petrification trap"));
		AddPanelString(tempstr);
		break;
	case IMISC_FULLHEAL:
		strcpy(tempstr, _("restore all life"));
		AddPanelString(tempstr);
		break;
	case IMISC_HEAL:
		strcpy(tempstr, _("restore some life"));
		AddPanelString(tempstr);
		break;
	case IMISC_OLDHEAL:
		strcpy(tempstr, _("recover life"));
		AddPanelString(tempstr);
		break;
	case IMISC_DEADHEAL:
		strcpy(tempstr, _("deadly heal"));
		AddPanelString(tempstr);
		break;
	case IMISC_MANA:
		strcpy(tempstr, _("restore some mana"));
		AddPanelString(tempstr);
		break;
	case IMISC_FULLMANA:
		strcpy(tempstr, _("restore all mana"));
		AddPanelString(tempstr);
		break;
	case IMISC_ELIXSTR:
		strcpy(tempstr, _("increase strength"));
		AddPanelString(tempstr);
		break;
	case IMISC_ELIXMAG:
		strcpy(tempstr, _("increase magic"));
		AddPanelString(tempstr);
		break;
	case IMISC_ELIXDEX:
		strcpy(tempstr, _("increase dexterity"));
		AddPanelString(tempstr);
		break;
	case IMISC_ELIXVIT:
		strcpy(tempstr, _("increase vitality"));
		AddPanelString(tempstr);
		break;
	case IMISC_ELIXWEAK:
		strcpy(tempstr, _("decrease strength"));
		AddPanelString(tempstr);
		break;
	case IMISC_ELIXDIS:
		strcpy(tempstr, _("decrease strength"));
		AddPanelString(tempstr);
		break;
	case IMISC_ELIXCLUM:
		strcpy(tempstr, _("decrease dexterity"));
		AddPanelString(tempstr);
		break;
	case IMISC_ELIXSICK:
		strcpy(tempstr, _("decrease vitality"));
		AddPanelString(tempstr);
		break;
	case IMISC_REJUV:
		strcpy(tempstr, _("restore some life and mana"));
		AddPanelString(tempstr);
		break;
	case IMISC_FULLREJUV:
		strcpy(tempstr, _("restore all life and mana"));
		AddPanelString(tempstr);
		break;
	}
}

void PrintItemPower(char plidx, ItemStruct *x)
{
	switch (plidx) {
	case IPL_TOHIT:
	case IPL_TOHIT_CURSE:
		strcpy(tempstr, fmt::format(_("chance to hit: {:+d}%"), x->_iPLToHit).c_str());
		break;
	case IPL_DAMP:
	case IPL_DAMP_CURSE:
		strcpy(tempstr, fmt::format(_("{:+d}% damage"), x->_iPLDam).c_str());
		break;
	case IPL_TOHIT_DAMP:
	case IPL_TOHIT_DAMP_CURSE:
		strcpy(tempstr, fmt::format(_("to hit: {:+d}%, {:+d}% damage"), x->_iPLToHit, x->_iPLDam).c_str());
		break;
	case IPL_ACP:
	case IPL_ACP_CURSE:
		strcpy(tempstr, fmt::format(_("{:+d}% armor"), x->_iPLAC).c_str());
		break;
	case IPL_SETAC:
		strcpy(tempstr, fmt::format(_("armor class: {:d}"), x->_iAC).c_str());
		break;
	case IPL_AC_CURSE:
		strcpy(tempstr, fmt::format(_("armor class: {:d}"), x->_iAC).c_str());
		break;
	case IPL_FIRERES:
	case IPL_FIRERES_CURSE:
		if (x->_iPLFR < 75)
			strcpy(tempstr, fmt::format(_("Resist Fire: {:+d}%"), x->_iPLFR).c_str());
		else
			/*xgettext:no-c-format*/ strcpy(tempstr, _("Resist Fire: 75% MAX"));
		break;
	case IPL_LIGHTRES:
	case IPL_LIGHTRES_CURSE:
		if (x->_iPLLR < 75)
			strcpy(tempstr, fmt::format(_("Resist Lightning: {:+d}%"), x->_iPLLR).c_str());
		else
			/*xgettext:no-c-format*/ strcpy(tempstr, _("Resist Lightning: 75% MAX"));
		break;
	case IPL_MAGICRES:
	case IPL_MAGICRES_CURSE:
		if (x->_iPLMR < 75)
			strcpy(tempstr, fmt::format(_("Resist Magic: {:+d}%"), x->_iPLMR).c_str());
		else
			/*xgettext:no-c-format*/ strcpy(tempstr, _("Resist Magic: 75% MAX"));
		break;
	case IPL_ALLRES:
	case IPL_ALLRES_CURSE:
		if (x->_iPLFR < 75)
			strcpy(tempstr, fmt::format(_("Resist All: {:+d}%"), x->_iPLFR).c_str());
		if (x->_iPLFR >= 75)
			/*xgettext:no-c-format*/ strcpy(tempstr, _("Resist All: 75% MAX"));
		break;
	case IPL_SPLLVLADD:
		if (x->_iSplLvlAdd > 0)
			strcpy(tempstr, fmt::format(ngettext("spells are increased {:d} level", "spells are increased {:d} levels", x->_iSplLvlAdd), x->_iSplLvlAdd).c_str());
		else if (x->_iSplLvlAdd < 0)
			strcpy(tempstr, fmt::format(ngettext("spells are decreased {:d} level", "spells are decreased {:d} levels", -x->_iSplLvlAdd), -x->_iSplLvlAdd).c_str());
		else if (x->_iSplLvlAdd == 0)
			strcpy(tempstr, _("spell levels unchanged (?)"));
		break;
	case IPL_CHARGES:
		strcpy(tempstr, _("Extra charges"));
		break;
	case IPL_SPELL:
		strcpy(tempstr, fmt::format(ngettext("{:d} {:s} charge", "{:d} {:s} charges", x->_iMaxCharges), x->_iMaxCharges, _(spelldata[x->_iSpell].sNameText)).c_str());
		break;
	case IPL_FIREDAM:
		if (x->_iFMinDam == x->_iFMaxDam)
			strcpy(tempstr, fmt::format(_("Fire hit damage: {:d}"), x->_iFMinDam).c_str());
		else
			strcpy(tempstr, fmt::format(_("Fire hit damage: {:d}-{:d}"), x->_iFMinDam, x->_iFMaxDam).c_str());
		break;
	case IPL_LIGHTDAM:
		if (x->_iLMinDam == x->_iLMaxDam)
			strcpy(tempstr, fmt::format(_("Lightning hit damage: {:d}"), x->_iLMinDam).c_str());
		else
			strcpy(tempstr, fmt::format(_("Lightning hit damage: {:d}-{:d}"), x->_iLMinDam, x->_iLMaxDam).c_str());
		break;
	case IPL_STR:
	case IPL_STR_CURSE:
		strcpy(tempstr, fmt::format(_("{:+d} to strength"), x->_iPLStr).c_str());
		break;
	case IPL_MAG:
	case IPL_MAG_CURSE:
		strcpy(tempstr, fmt::format(_("{:+d} to magic"), x->_iPLMag).c_str());
		break;
	case IPL_DEX:
	case IPL_DEX_CURSE:
		strcpy(tempstr, fmt::format(_("{:+d} to dexterity"), x->_iPLDex).c_str());
		break;
	case IPL_VIT:
	case IPL_VIT_CURSE:
		strcpy(tempstr, fmt::format(_("{:+d} to vitality"), x->_iPLVit).c_str());
		break;
	case IPL_ATTRIBS:
	case IPL_ATTRIBS_CURSE:
		strcpy(tempstr, fmt::format(_("{:+d} to all attributes"), x->_iPLStr).c_str());
		break;
	case IPL_GETHIT_CURSE:
	case IPL_GETHIT:
		strcpy(tempstr, fmt::format(_("{:+d} damage from enemies"), x->_iPLGetHit).c_str());
		break;
	case IPL_LIFE:
	case IPL_LIFE_CURSE:
		strcpy(tempstr, fmt::format(_("Hit Points: {:+d}"), x->_iPLHP >> 6).c_str());
		break;
	case IPL_MANA:
	case IPL_MANA_CURSE:
		strcpy(tempstr, fmt::format(_("Mana: {:+d}"), x->_iPLMana >> 6).c_str());
		break;
	case IPL_DUR:
		strcpy(tempstr, _("high durability"));
		break;
	case IPL_DUR_CURSE:
		strcpy(tempstr, _("decreased durability"));
		break;
	case IPL_INDESTRUCTIBLE:
		strcpy(tempstr, _("indestructible"));
		break;
	case IPL_LIGHT:
		/*xgettext:no-c-format*/ strcpy(tempstr, fmt::format(_("+{:d}% light radius"), 10 * x->_iPLLight).c_str());
		break;
	case IPL_LIGHT_CURSE:
		/*xgettext:no-c-format*/ strcpy(tempstr, fmt::format(_("-{:d}% light radius"), -10 * x->_iPLLight).c_str());
		break;
	case IPL_MULT_ARROWS:
		strcpy(tempstr, _("multiple arrows per shot"));
		break;
	case IPL_FIRE_ARROWS:
		if (x->_iFMinDam == x->_iFMaxDam)
			strcpy(tempstr, fmt::format(_("fire arrows damage: {:d}"), x->_iFMinDam).c_str());
		else
			strcpy(tempstr, fmt::format(_("fire arrows damage: {:d}-{:d}"), x->_iFMinDam, x->_iFMaxDam).c_str());
		break;
	case IPL_LIGHT_ARROWS:
		if (x->_iLMinDam == x->_iLMaxDam)
			strcpy(tempstr, fmt::format(_("lightning arrows damage {:d}"), x->_iLMinDam).c_str());
		else
			strcpy(tempstr, fmt::format(_("lightning arrows damage {:d}-{:d}"), x->_iLMinDam, x->_iLMaxDam).c_str());
		break;
	case IPL_FIREBALL:
		if (x->_iFMinDam == x->_iFMaxDam)
			strcpy(tempstr, fmt::format(_("fireball damage: {:d}"), x->_iFMinDam).c_str());
		else
			strcpy(tempstr, fmt::format(_("fireball damage: {:d}-{:d}"), x->_iFMinDam, x->_iFMaxDam).c_str());
		break;
	case IPL_THORNS:
		strcpy(tempstr, _("attacker takes 1-3 damage"));
		break;
	case IPL_NOMANA:
		strcpy(tempstr, _("user loses all mana"));
		break;
	case IPL_NOHEALPLR:
		strcpy(tempstr, _("you can't heal"));
		break;
	case IPL_ABSHALFTRAP:
		strcpy(tempstr, _("absorbs half of trap damage"));
		break;
	case IPL_KNOCKBACK:
		strcpy(tempstr, _("knocks target back"));
		break;
	case IPL_3XDAMVDEM:
		/*xgettext:no-c-format*/ strcpy(tempstr, _("+200% damage vs. demons"));
		break;
	case IPL_ALLRESZERO:
		strcpy(tempstr, _("All Resistance equals 0"));
		break;
	case IPL_NOHEALMON:
		strcpy(tempstr, _("hit monster doesn't heal"));
		break;
	case IPL_STEALMANA:
		if ((x->_iFlags & ISPL_STEALMANA_3) != 0)
			/*xgettext:no-c-format*/ strcpy(tempstr, _("hit steals 3% mana"));
		if ((x->_iFlags & ISPL_STEALMANA_5) != 0)
			/*xgettext:no-c-format*/ strcpy(tempstr, _("hit steals 5% mana"));
		break;
	case IPL_STEALLIFE:
		if ((x->_iFlags & ISPL_STEALLIFE_3) != 0)
			/*xgettext:no-c-format*/ strcpy(tempstr, _("hit steals 3% life"));
		if ((x->_iFlags & ISPL_STEALLIFE_5) != 0)
			/*xgettext:no-c-format*/ strcpy(tempstr, _("hit steals 5% life"));
		break;
	case IPL_TARGAC:
		strcpy(tempstr, _("penetrates target's armor"));
		break;
	case IPL_FASTATTACK:
		if ((x->_iFlags & ISPL_QUICKATTACK) != 0)
			strcpy(tempstr, _("quick attack"));
		if ((x->_iFlags & ISPL_FASTATTACK) != 0)
			strcpy(tempstr, _("fast attack"));
		if ((x->_iFlags & ISPL_FASTERATTACK) != 0)
			strcpy(tempstr, _("faster attack"));
		if ((x->_iFlags & ISPL_FASTESTATTACK) != 0)
			strcpy(tempstr, _("fastest attack"));
		break;
	case IPL_FASTRECOVER:
		if ((x->_iFlags & ISPL_FASTRECOVER) != 0)
			strcpy(tempstr, _("fast hit recovery"));
		if ((x->_iFlags & ISPL_FASTERRECOVER) != 0)
			strcpy(tempstr, _("faster hit recovery"));
		if ((x->_iFlags & ISPL_FASTESTRECOVER) != 0)
			strcpy(tempstr, _("fastest hit recovery"));
		break;
	case IPL_FASTBLOCK:
		strcpy(tempstr, _("fast block"));
		break;
	case IPL_DAMMOD:
		strcpy(tempstr, fmt::format(ngettext("adds {:d} point to damage", "adds {:d} points to damage", x->_iPLDamMod), x->_iPLDamMod).c_str());
		break;
	case IPL_RNDARROWVEL:
		strcpy(tempstr, _("fires random speed arrows"));
		break;
	case IPL_SETDAM:
		strcpy(tempstr, _("unusual item damage"));
		break;
	case IPL_SETDUR:
		strcpy(tempstr, _("altered durability"));
		break;
	case IPL_FASTSWING:
		strcpy(tempstr, _("Faster attack swing"));
		break;
	case IPL_ONEHAND:
		strcpy(tempstr, _("one handed sword"));
		break;
	case IPL_DRAINLIFE:
		strcpy(tempstr, _("constantly lose hit points"));
		break;
	case IPL_RNDSTEALLIFE:
		strcpy(tempstr, _("life stealing"));
		break;
	case IPL_NOMINSTR:
		strcpy(tempstr, _("no strength requirement"));
		break;
	case IPL_INFRAVISION:
		strcpy(tempstr, _("see with infravision"));
		break;
	case IPL_INVCURS:
		strcpy(tempstr, " ");
		break;
	case IPL_ADDACLIFE:
		if (x->_iFMinDam == x->_iFMaxDam)
			strcpy(tempstr, fmt::format(_("lightning damage: {:d}"), x->_iFMinDam).c_str());
		else
			strcpy(tempstr, fmt::format(_("lightning damage: {:d}-{:d}"), x->_iFMinDam, x->_iFMaxDam).c_str());
		break;
	case IPL_ADDMANAAC:
		strcpy(tempstr, _("charged bolts on hits"));
		break;
	case IPL_FIRERESCLVL:
		if (x->_iPLFR <= 0)
			strcpy(tempstr, " ");
		else if (x->_iPLFR >= 1)
			strcpy(tempstr, fmt::format(_("Resist Fire: {:+d}%"), x->_iPLFR).c_str());
		break;
	case IPL_DEVASTATION:
		strcpy(tempstr, _("occasional triple damage"));
		break;
	case IPL_DECAY:
		strcpy(tempstr, fmt::format(_("decaying {:+d}% damage"), x->_iPLDam).c_str());
		break;
	case IPL_PERIL:
		strcpy(tempstr, _("2x dmg to monst, 1x to you"));
		break;
	case IPL_JESTERS:
		/*xgettext:no-c-format*/ strcpy(tempstr, _("Random 0 - 500% damage"));
		break;
	case IPL_CRYSTALLINE:
		strcpy(tempstr, fmt::format(_("low dur, {:+d}% damage"), x->_iPLDam).c_str());
		break;
	case IPL_DOPPELGANGER:
		strcpy(tempstr, fmt::format(_("to hit: {:+d}%, {:+d}% damage"), x->_iPLToHit, x->_iPLDam).c_str());
		break;
	case IPL_ACDEMON:
		strcpy(tempstr, _("extra AC vs demons"));
		break;
	case IPL_ACUNDEAD:
		strcpy(tempstr, _("extra AC vs undead"));
		break;
	case IPL_MANATOLIFE:
		/*xgettext:no-c-format*/ strcpy(tempstr, _("50% Mana moved to Health"));
		break;
	case IPL_LIFETOMANA:
		/*xgettext:no-c-format*/ strcpy(tempstr, _("40% Health moved to Mana"));
		break;
	default:
		strcpy(tempstr, _("Another ability (NW)"));
		break;
	}
}

static void DrawUTextBack(const CelOutputBuffer &out)
{
	CelDrawTo(out, { RIGHT_PANEL_X - SPANEL_WIDTH + 24, 327 }, *pSTextBoxCels, 1);
	DrawHalfTransparentRectTo(out, RIGHT_PANEL_X - SPANEL_WIDTH + 27, 28, 265, 297);
}

static void DrawULine(const CelOutputBuffer &out, int y)
{
	BYTE *src = out.at(26 + RIGHT_PANEL - SPANEL_WIDTH, 25);
	BYTE *dst = out.at(26 + RIGHT_PANEL_X - SPANEL_WIDTH, y * 12 + 38);

	for (int i = 0; i < 3; i++, src += out.pitch(), dst += out.pitch())
		memcpy(dst, src, 267); // BUGFIX: should be 267 (fixed)
}

void DrawUniqueInfo(const CelOutputBuffer &out)
{
	if ((chrflag || questlog) && gnScreenWidth < SPANEL_WIDTH * 3) {
		return;
	}

	DrawUTextBack(GlobalBackBuffer());

	Rectangle rect { 32 + RIGHT_PANEL - SPANEL_WIDTH, 44 + 2 * 12, 257, 0 };
	const UItemStruct &uitem = UniqueItemList[curruitem._iUid];
	DrawString(out, _(uitem.UIName), rect, UIS_CENTER);

	DrawULine(out, 5);

	rect.position.y += (12 - uitem.UINumPL) * 12;
	PrintItemPower(uitem.UIPower1, &curruitem);
	DrawString(out, tempstr, rect, UIS_SILVER | UIS_CENTER);
	if (uitem.UINumPL > 1) {
		rect.position.y += 2 * 12;
		PrintItemPower(uitem.UIPower2, &curruitem);
		DrawString(out, tempstr, rect, UIS_SILVER | UIS_CENTER);
	}
	if (uitem.UINumPL > 2) {
		rect.position.y += 2 * 12;
		PrintItemPower(uitem.UIPower3, &curruitem);
		DrawString(out, tempstr, rect, UIS_SILVER | UIS_CENTER);
	}
	if (uitem.UINumPL > 3) {
		rect.position.y += 2 * 12;
		PrintItemPower(uitem.UIPower4, &curruitem);
		DrawString(out, tempstr, rect, UIS_SILVER | UIS_CENTER);
	}
	if (uitem.UINumPL > 4) {
		rect.position.y += 2 * 12;
		PrintItemPower(uitem.UIPower5, &curruitem);
		DrawString(out, tempstr, rect, UIS_SILVER | UIS_CENTER);
	}
	if (uitem.UINumPL > 5) {
		rect.position.y += 2 * 12;
		PrintItemPower(uitem.UIPower6, &curruitem);
		DrawString(out, tempstr, rect, UIS_SILVER | UIS_CENTER);
	}
}

void PrintItemMisc(ItemStruct *x)
{
	if (x->_iMiscId == IMISC_SCROLL) {
		strcpy(tempstr, _("Right-click to read"));
		AddPanelString(tempstr);
	}
	if (x->_iMiscId == IMISC_SCROLLT) {
		strcpy(tempstr, _("Right-click to read, then"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("left-click to target"));
		AddPanelString(tempstr);
	}
	if (x->_iMiscId >= IMISC_USEFIRST && x->_iMiscId <= IMISC_USELAST) {
		PrintItemOil(x->_iMiscId);
		strcpy(tempstr, _("Right-click to use"));
		AddPanelString(tempstr);
	}
	if (x->_iMiscId > IMISC_OILFIRST && x->_iMiscId < IMISC_OILLAST) {
		PrintItemOil(x->_iMiscId);
		strcpy(tempstr, _("Right click to use"));
		AddPanelString(tempstr);
	}
	if (x->_iMiscId > IMISC_RUNEFIRST && x->_iMiscId < IMISC_RUNELAST) {
		PrintItemOil(x->_iMiscId);
		strcpy(tempstr, _("Right click to use"));
		AddPanelString(tempstr);
	}
	if (x->_iMiscId == IMISC_BOOK) {
		strcpy(tempstr, _("Right-click to read"));
		AddPanelString(tempstr);
	}
	if (x->_iMiscId == IMISC_NOTE) {
		strcpy(tempstr, _("Right click to read"));
		AddPanelString(tempstr);
	}
	if (x->_iMiscId == IMISC_MAPOFDOOM) {
		strcpy(tempstr, _("Right-click to view"));
		AddPanelString(tempstr);
	}
	if (x->_iMiscId == IMISC_EAR) {
		strcpy(tempstr, fmt::format(_("Level: {:d}"), x->_ivalue).c_str());
		AddPanelString(tempstr);
	}
	if (x->_iMiscId == IMISC_AURIC) {
		strcpy(tempstr, _("Doubles gold capacity"));
		AddPanelString(tempstr);
	}
}

static void PrintItemInfo(ItemStruct *x)
{
	PrintItemMisc(x);
	uint8_t str = x->_iMinStr;
	uint8_t dex = x->_iMinDex;
	uint8_t mag = x->_iMinMag;
	if (str != 0 || mag != 0 || dex != 0) {
		strcpy(tempstr, _("Required:"));
		if (str != 0)
			strcpy(tempstr + strlen(tempstr), fmt::format(_(" {:d} Str"), str).c_str());
		if (mag != 0)
			strcpy(tempstr + strlen(tempstr), fmt::format(_(" {:d} Mag"), mag).c_str());
		if (dex != 0)
			strcpy(tempstr + strlen(tempstr), fmt::format(_(" {:d} Dex"), dex).c_str());
		AddPanelString(tempstr);
	}
	pinfoflag = true;
}

void PrintItemDetails(ItemStruct *x)
{
	if (x->_iClass == ICLASS_WEAPON) {
		if (x->_iMinDam == x->_iMaxDam) {
			if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
				strcpy(tempstr, fmt::format(_("damage: {:d}  Indestructible"), x->_iMinDam).c_str());
			else
				strcpy(tempstr, fmt::format(_(/* TRANSLATORS: Dur: is durability */ "damage: {:d}  Dur: {:d}/{:d}"), x->_iMinDam, x->_iDurability, x->_iMaxDur).c_str());
		} else {
			if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
				strcpy(tempstr, fmt::format(_("damage: {:d}-{:d}  Indestructible"), x->_iMinDam, x->_iMaxDam).c_str());
			else
				strcpy(tempstr, fmt::format(_(/* TRANSLATORS: Dur: is durability */ "damage: {:d}-{:d}  Dur: {:d}/{:d}"), x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur).c_str());
		}
		AddPanelString(tempstr);
	}
	if (x->_iClass == ICLASS_ARMOR) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			strcpy(tempstr, fmt::format(_("armor: {:d}  Indestructible"), x->_iAC).c_str());
		else
			strcpy(tempstr, fmt::format(_(/* TRANSLATORS: Dur: is durability */ "armor: {:d}  Dur: {:d}/{:d}"), x->_iAC, x->_iDurability, x->_iMaxDur).c_str());
		AddPanelString(tempstr);
	}
	if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges != 0) {
		if (x->_iMinDam == x->_iMaxDam)
			strcpy(tempstr, fmt::format(_(/* TRANSLATORS: dam: is damage Dur: is durability */ "dam: {:d}  Dur: {:d}/{:d}"), x->_iMinDam, x->_iDurability, x->_iMaxDur).c_str());
		else
			strcpy(tempstr, fmt::format(_(/* TRANSLATORS: dam: is damage Dur: is durability */ "dam: {:d}-{:d}  Dur: {:d}/{:d}"), x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur).c_str());
		strcpy(tempstr, fmt::format(_("Charges: {:d}/{:d}"), x->_iCharges, x->_iMaxCharges).c_str());
		AddPanelString(tempstr);
	}
	if (x->_iPrePower != -1) {
		PrintItemPower(x->_iPrePower, x);
		AddPanelString(tempstr);
	}
	if (x->_iSufPower != -1) {
		PrintItemPower(x->_iSufPower, x);
		AddPanelString(tempstr);
	}
	if (x->_iMagical == ITEM_QUALITY_UNIQUE) {
		AddPanelString(_("unique item"));
		uitemflag = true;
		curruitem = *x;
	}
	PrintItemInfo(x);
}

void PrintItemDur(ItemStruct *x)
{
	if (x->_iClass == ICLASS_WEAPON) {
		if (x->_iMinDam == x->_iMaxDam) {
			if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
				strcpy(tempstr, fmt::format(_("damage: {:d}  Indestructible"), x->_iMinDam).c_str());
			else
				strcpy(tempstr, fmt::format(_("damage: {:d}  Dur: {:d}/{:d}"), x->_iMinDam, x->_iDurability, x->_iMaxDur).c_str());
		} else {
			if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
				strcpy(tempstr, fmt::format(_("damage: {:d}-{:d}  Indestructible"), x->_iMinDam, x->_iMaxDam).c_str());
			else
				strcpy(tempstr, fmt::format(_("damage: {:d}-{:d}  Dur: {:d}/{:d}"), x->_iMinDam, x->_iMaxDam, x->_iDurability, x->_iMaxDur).c_str());
		}
		AddPanelString(tempstr);
		if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges > 0) {
			strcpy(tempstr, fmt::format(_("Charges: {:d}/{:d}"), x->_iCharges, x->_iMaxCharges).c_str());
			AddPanelString(tempstr);
		}
		if (x->_iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString(_("Not Identified"));
	}
	if (x->_iClass == ICLASS_ARMOR) {
		if (x->_iMaxDur == DUR_INDESTRUCTIBLE)
			strcpy(tempstr, fmt::format(_("armor: {:d}  Indestructible"), x->_iAC).c_str());
		else
			strcpy(tempstr, fmt::format(_("armor: {:d}  Dur: {:d}/{:d}"), x->_iAC, x->_iDurability, x->_iMaxDur).c_str());
		AddPanelString(tempstr);
		if (x->_iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString(_("Not Identified"));
		if (x->_iMiscId == IMISC_STAFF && x->_iMaxCharges > 0) {
			strcpy(tempstr, fmt::format(_("Charges: {:d}/{:d}"), x->_iCharges, x->_iMaxCharges).c_str());
			AddPanelString(tempstr);
		}
	}
	if (x->_itype == ITYPE_RING || x->_itype == ITYPE_AMULET)
		AddPanelString(_("Not Identified"));
	PrintItemInfo(x);
}

void UseItem(int p, item_misc_id Mid, spell_id spl)
{
	int l, j;

	auto &player = plr[p];

	switch (Mid) {
	case IMISC_HEAL:
	case IMISC_FOOD:
		j = player._pMaxHP >> 8;
		l = ((j / 2) + GenerateRnd(j)) << 6;
		if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Barbarian)
			l *= 2;
		if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk || player._pClass == HeroClass::Bard)
			l += l / 2;
		player._pHitPoints = std::min(player._pHitPoints + l, player._pMaxHP);
		player._pHPBase = std::min(player._pHPBase + l, player._pMaxHPBase);
		drawhpflag = true;
		break;
	case IMISC_FULLHEAL:
		player._pHitPoints = player._pMaxHP;
		player._pHPBase = player._pMaxHPBase;
		drawhpflag = true;
		break;
	case IMISC_MANA:
		j = player._pMaxMana >> 8;
		l = ((j / 2) + GenerateRnd(j)) << 6;
		if (player._pClass == HeroClass::Sorcerer)
			l *= 2;
		if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk || player._pClass == HeroClass::Bard)
			l += l / 2;
		if ((player._pIFlags & ISPL_NOMANA) == 0) {
			player._pMana = std::min(player._pMana + l, player._pMaxMana);
			player._pManaBase = std::min(player._pManaBase + l, player._pMaxManaBase);
			drawmanaflag = true;
		}
		break;
	case IMISC_FULLMANA:
		if ((player._pIFlags & ISPL_NOMANA) == 0) {
			player._pMana = player._pMaxMana;
			player._pManaBase = player._pMaxManaBase;
			drawmanaflag = true;
		}
		break;
	case IMISC_ELIXSTR:
		ModifyPlrStr(p, 1);
		break;
	case IMISC_ELIXMAG:
		ModifyPlrMag(p, 1);
		if (gbIsHellfire) {
			player._pMana = player._pMaxMana;
			player._pManaBase = player._pMaxManaBase;
			drawmanaflag = true;
		}
		break;
	case IMISC_ELIXDEX:
		ModifyPlrDex(p, 1);
		break;
	case IMISC_ELIXVIT:
		ModifyPlrVit(p, 1);
		if (gbIsHellfire) {
			player._pHitPoints = player._pMaxHP;
			player._pHPBase = player._pMaxHPBase;
			drawhpflag = true;
		}
		break;
	case IMISC_REJUV:
		j = player._pMaxHP >> 8;
		l = ((j / 2) + GenerateRnd(j)) << 6;
		if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Barbarian)
			l *= 2;
		if (player._pClass == HeroClass::Rogue)
			l += l / 2;
		player._pHitPoints = std::min(player._pHitPoints + l, player._pMaxHP);
		player._pHPBase = std::min(player._pHPBase + l, player._pMaxHPBase);
		drawhpflag = true;
		j = player._pMaxMana >> 8;
		l = ((j / 2) + GenerateRnd(j)) << 6;
		if (player._pClass == HeroClass::Sorcerer)
			l *= 2;
		if (player._pClass == HeroClass::Rogue)
			l += l / 2;
		if ((player._pIFlags & ISPL_NOMANA) == 0) {
			player._pMana = std::min(player._pMana + l, player._pMaxMana);
			player._pManaBase = std::min(player._pManaBase + l, player._pMaxManaBase);
			drawmanaflag = true;
		}
		break;
	case IMISC_FULLREJUV:
		player._pHitPoints = player._pMaxHP;
		player._pHPBase = player._pMaxHPBase;
		drawhpflag = true;
		if ((player._pIFlags & ISPL_NOMANA) == 0) {
			player._pMana = player._pMaxMana;
			player._pManaBase = player._pMaxManaBase;
			drawmanaflag = true;
		}
		break;
	case IMISC_SCROLL:
		if (spelldata[spl].sTargeted) {
			player._pTSpell = spl;
			player._pTSplType = RSPLTYPE_INVALID;
			if (p == myplr)
				NewCursor(CURSOR_TELEPORT);
		} else {
			ClrPlrPath(player);
			player._pSpell = spl;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 3;
			player.destAction = ACTION_SPELL;
			player.destParam1 = cursPosition.x;
			player.destParam2 = cursPosition.y;
			if (p == myplr && spl == SPL_NOVA)
				NetSendCmdLoc(myplr, true, CMD_NOVA, cursPosition);
		}
		break;
	case IMISC_SCROLLT:
		if (spelldata[spl].sTargeted) {
			player._pTSpell = spl;
			player._pTSplType = RSPLTYPE_INVALID;
			if (p == myplr)
				NewCursor(CURSOR_TELEPORT);
		} else {
			ClrPlrPath(player);
			player._pSpell = spl;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 3;
			player.destAction = ACTION_SPELL;
			player.destParam1 = cursPosition.x;
			player.destParam2 = cursPosition.y;
		}
		break;
	case IMISC_BOOK:
		player._pMemSpells |= GetSpellBitmask(spl);
		if (player._pSplLvl[spl] < MAX_SPELL_LEVEL)
			player._pSplLvl[spl]++;
		if ((player._pIFlags & ISPL_NOMANA) == 0) {
			player._pMana += spelldata[spl].sManaCost << 6;
			player._pMana = std::min(player._pMana, player._pMaxMana);
			player._pManaBase += spelldata[spl].sManaCost << 6;
			player._pManaBase = std::min(player._pManaBase, player._pMaxManaBase);
		}
		if (p == myplr)
			CalcPlrBookVals(player);
		drawmanaflag = true;
		break;
	case IMISC_MAPOFDOOM:
		doom_init();
		break;
	case IMISC_OILACC:
	case IMISC_OILMAST:
	case IMISC_OILSHARP:
	case IMISC_OILDEATH:
	case IMISC_OILSKILL:
	case IMISC_OILBSMTH:
	case IMISC_OILFORT:
	case IMISC_OILPERM:
	case IMISC_OILHARD:
	case IMISC_OILIMP:
		player._pOilType = Mid;
		if (p != myplr) {
			return;
		}
		if (sbookflag) {
			sbookflag = false;
		}
		if (!invflag) {
			invflag = true;
		}
		NewCursor(CURSOR_OIL);
		break;
	case IMISC_SPECELIX:
		ModifyPlrStr(p, 3);
		ModifyPlrMag(p, 3);
		ModifyPlrDex(p, 3);
		ModifyPlrVit(p, 3);
		break;
	case IMISC_RUNEF:
		player._pTSpell = SPL_RUNEFIRE;
		player._pTSplType = RSPLTYPE_INVALID;
		if (p == myplr)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_RUNEL:
		player._pTSpell = SPL_RUNELIGHT;
		player._pTSplType = RSPLTYPE_INVALID;
		if (p == myplr)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_GR_RUNEL:
		player._pTSpell = SPL_RUNENOVA;
		player._pTSplType = RSPLTYPE_INVALID;
		if (p == myplr)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_GR_RUNEF:
		player._pTSpell = SPL_RUNEIMMOLAT;
		player._pTSplType = RSPLTYPE_INVALID;
		if (p == myplr)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_RUNES:
		player._pTSpell = SPL_RUNESTONE;
		player._pTSplType = RSPLTYPE_INVALID;
		if (p == myplr)
			NewCursor(CURSOR_TELEPORT);
		break;
	default:
		break;
	}
}

bool StoreStatOk(ItemStruct *h)
{
	const auto &myPlayer = plr[myplr];

	if (myPlayer._pStrength < h->_iMinStr)
		return false;
	if (myPlayer._pMagic < h->_iMinMag)
		return false;
	if (myPlayer._pDexterity < h->_iMinDex)
		return false;

	return true;
}

bool SmithItemOk(int i)
{
	if (AllItemsList[i].itype == ITYPE_MISC)
		return false;
	if (AllItemsList[i].itype == ITYPE_GOLD)
		return false;
	if (AllItemsList[i].itype == ITYPE_STAFF && (!gbIsHellfire || AllItemsList[i].iSpell != SPL_NULL))
		return false;
	if (AllItemsList[i].itype == ITYPE_RING)
		return false;
	if (AllItemsList[i].itype == ITYPE_AMULET)
		return false;

	return true;
}

template <bool (*Ok)(int), bool ConsiderDropRate = false>
int RndVendorItem(int minlvl, int maxlvl)
{
	int ril[512];

	int ri = 0;
	for (int i = 1; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (!IsItemAvailable(i))
			continue;
		if (AllItemsList[i].iRnd == IDROP_NEVER)
			continue;
		if (!Ok(i))
			continue;
		if (AllItemsList[i].iMinMLvl < minlvl || AllItemsList[i].iMinMLvl > maxlvl)
			continue;

		ril[ri] = i;
		ri++;
		if (ri == 512)
			break;

		if (!ConsiderDropRate || AllItemsList[i].iRnd != IDROP_DOUBLE)
			continue;

		ril[ri] = i;
		ri++;
		if (ri == 512)
			break;
	}

	return ril[GenerateRnd(ri)] + 1;
}

int RndSmithItem(int lvl)
{
	return RndVendorItem<SmithItemOk, true>(0, lvl);
}

void SortVendor(ItemStruct *items)
{
	int count = 1;
	while (!items[count].isEmpty())
		count++;

	auto cmp = [](const ItemStruct &a, const ItemStruct &b) {
		return a.IDidx < b.IDidx;
	};

	std::sort(items, items + count, cmp);
}

void SpawnSmith(int lvl)
{
	constexpr int PinnedItemCount = 0;

	int maxValue, maxItems;

	ItemStruct holditem;
	holditem = items[0];

	if (gbIsHellfire) {
		maxValue = 200000;
		maxItems = 25;
	} else {
		maxValue = 140000;
		maxItems = 20;
	}

	int iCnt = GenerateRnd(maxItems - 10) + 10;
	for (int i = 0; i < iCnt; i++) {
		do {
			memset(&items[0], 0, sizeof(*items));
			items[0]._iSeed = AdvanceRndSeed();
			int idata = RndSmithItem(lvl) - 1;
			GetItemAttrs(0, idata, lvl);
		} while (items[0]._iIvalue > maxValue);
		smithitem[i] = items[0];
		smithitem[i]._iCreateInfo = lvl | CF_SMITH;
		smithitem[i]._iIdentified = true;
		smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
	}
	for (int i = iCnt; i < SMITH_ITEMS; i++)
		smithitem[i]._itype = ITYPE_NONE;

	SortVendor(smithitem + PinnedItemCount);
	items[0] = holditem;
}

bool PremiumItemOk(int i)
{
	if (AllItemsList[i].itype == ITYPE_MISC)
		return false;
	if (AllItemsList[i].itype == ITYPE_GOLD)
		return false;
	if (!gbIsHellfire && AllItemsList[i].itype == ITYPE_STAFF)
		return false;

	if (gbIsMultiplayer) {
		if (AllItemsList[i].iMiscId == IMISC_OILOF)
			return false;
		if (AllItemsList[i].itype == ITYPE_RING)
			return false;
		if (AllItemsList[i].itype == ITYPE_AMULET)
			return false;
	}

	return true;
}

int RndPremiumItem(int minlvl, int maxlvl)
{
	return RndVendorItem<PremiumItemOk>(minlvl, maxlvl);
}

static void SpawnOnePremium(int i, int plvl, int myplr)
{
	int ivalue = 0;
	bool keepgoing = false;
	ItemStruct holditem = items[0];

	auto &myPlayer = plr[myplr];

	int strength = std::max(myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength), myPlayer._pStrength);
	int dexterity = std::max(myPlayer.GetMaximumAttributeValue(CharacterAttribute::Dexterity), myPlayer._pDexterity);
	int magic = std::max(myPlayer.GetMaximumAttributeValue(CharacterAttribute::Magic), myPlayer._pMagic);
	strength += strength / 5;
	dexterity += dexterity / 5;
	magic += magic / 5;

	plvl = clamp(plvl, 1, 30);

	int count = 0;

	do {
		keepgoing = false;
		memset(&items[0], 0, sizeof(*items));
		items[0]._iSeed = AdvanceRndSeed();
		int itype = RndPremiumItem(plvl / 4, plvl) - 1;
		GetItemAttrs(0, itype, plvl);
		GetItemBonus(0, plvl / 2, plvl, true, !gbIsHellfire);

		if (!gbIsHellfire) {
			if (items[0]._iIvalue > 140000) {
				keepgoing = true; // prevent breaking the do/while loop too early by failing hellfire's condition in while
				continue;
			}
			break;
		}

		switch (items[0]._itype) {
		case ITYPE_LARMOR:
		case ITYPE_MARMOR:
		case ITYPE_HARMOR: {
			const auto *const mostValuablePlayerArmor = myPlayer.GetMostValuableItem(
			    [](const ItemStruct &item) {
				    return item._itype == ITYPE_LARMOR
				        || item._itype == ITYPE_MARMOR
				        || item._itype == ITYPE_HARMOR;
			    });

			ivalue = mostValuablePlayerArmor == nullptr ? 0 : mostValuablePlayerArmor->_iIvalue;
			break;
		}
		case ITYPE_SHIELD:
		case ITYPE_AXE:
		case ITYPE_BOW:
		case ITYPE_MACE:
		case ITYPE_SWORD:
		case ITYPE_HELM:
		case ITYPE_STAFF:
		case ITYPE_RING:
		case ITYPE_AMULET: {
			const auto *const mostValuablePlayerItem = myPlayer.GetMostValuableItem(
			    [](const ItemStruct &item) { return item._itype == items[0]._itype; });

			ivalue = mostValuablePlayerItem == nullptr ? 0 : mostValuablePlayerItem->_iIvalue;
			break;
		}
		default:
			ivalue = 0;
			break;
		}
		ivalue *= 0.8;

		count++;
	} while (keepgoing
	    || ((
	            items[0]._iIvalue > 200000
	            || items[0]._iMinStr > strength
	            || items[0]._iMinMag > magic
	            || items[0]._iMinDex > dexterity
	            || items[0]._iIvalue < ivalue)
	        && count < 150));
	premiumitems[i] = items[0];
	premiumitems[i]._iCreateInfo = plvl | CF_SMITHPREMIUM;
	premiumitems[i]._iIdentified = true;
	premiumitems[i]._iStatFlag = StoreStatOk(&premiumitems[i]);
	items[0] = holditem;
}

void SpawnPremium(int pnum)
{
	int lvl = plr[pnum]._pLevel;
	int maxItems = gbIsHellfire ? SMITH_PREMIUM_ITEMS : 6;
	if (numpremium < maxItems) {
		for (int i = 0; i < maxItems; i++) {
			if (premiumitems[i].isEmpty()) {
				int plvl = premiumlevel + (gbIsHellfire ? premiumLvlAddHellfire[i] : premiumlvladd[i]);
				SpawnOnePremium(i, plvl, pnum);
			}
		}
		numpremium = maxItems;
	}
	while (premiumlevel < lvl) {
		premiumlevel++;
		if (gbIsHellfire) {
			// Discard first 3 items and shift next 10
			std::move(&premiumitems[3], &premiumitems[12] + 1, &premiumitems[0]);
			SpawnOnePremium(10, premiumlevel + premiumLvlAddHellfire[10], pnum);
			premiumitems[11] = premiumitems[13];
			SpawnOnePremium(12, premiumlevel + premiumLvlAddHellfire[12], pnum);
			premiumitems[13] = premiumitems[14];
			SpawnOnePremium(14, premiumlevel + premiumLvlAddHellfire[14], pnum);
		} else {
			// Discard first 2 items and shift next 3
			std::move(&premiumitems[2], &premiumitems[4] + 1, &premiumitems[0]);
			SpawnOnePremium(3, premiumlevel + premiumlvladd[3], pnum);
			premiumitems[4] = premiumitems[5];
			SpawnOnePremium(5, premiumlevel + premiumlvladd[5], pnum);
		}
	}
}

bool WitchItemOk(int i)
{
	if (AllItemsList[i].itype != ITYPE_MISC && AllItemsList[i].itype != ITYPE_STAFF)
		return false;
	if (AllItemsList[i].iMiscId == IMISC_MANA)
		return false;
	if (AllItemsList[i].iMiscId == IMISC_FULLMANA)
		return false;
	if (AllItemsList[i].iSpell == SPL_TOWN)
		return false;
	if (AllItemsList[i].iMiscId == IMISC_FULLHEAL)
		return false;
	if (AllItemsList[i].iMiscId == IMISC_HEAL)
		return false;
	if (AllItemsList[i].iMiscId > IMISC_OILFIRST && AllItemsList[i].iMiscId < IMISC_OILLAST)
		return false;
	if (AllItemsList[i].iSpell == SPL_RESURRECT && !gbIsMultiplayer)
		return false;
	if (AllItemsList[i].iSpell == SPL_HEALOTHER && !gbIsMultiplayer)
		return false;

	return true;
}

int RndWitchItem(int lvl)
{
	return RndVendorItem<WitchItemOk>(0, lvl);
}

void WitchBookLevel(int ii)
{
	if (witchitem[ii]._iMiscId != IMISC_BOOK)
		return;
	witchitem[ii]._iMinMag = spelldata[witchitem[ii]._iSpell].sMinInt;
	int slvl = plr[myplr]._pSplLvl[witchitem[ii]._iSpell];
	while (slvl > 0) {
		witchitem[ii]._iMinMag += 20 * witchitem[ii]._iMinMag / 100;
		slvl--;
		if (witchitem[ii]._iMinMag + 20 * witchitem[ii]._iMinMag / 100 > 255) {
			witchitem[ii]._iMinMag = 255;
			slvl = 0;
		}
	}
}

void SpawnWitch(int lvl)
{
	constexpr int PinnedItemCount = 3;

	int iCnt;
	int idata, maxlvl, maxValue;

	int j = PinnedItemCount;

	memset(&items[0], 0, sizeof(*items));
	GetItemAttrs(0, IDI_MANA, 1);
	witchitem[0] = items[0];
	witchitem[0]._iCreateInfo = lvl;
	witchitem[0]._iStatFlag = true;
	memset(&items[0], 0, sizeof(*items));
	GetItemAttrs(0, IDI_FULLMANA, 1);
	witchitem[1] = items[0];
	witchitem[1]._iCreateInfo = lvl;
	witchitem[1]._iStatFlag = true;
	memset(&items[0], 0, sizeof(*items));
	GetItemAttrs(0, IDI_PORTAL, 1);
	witchitem[2] = items[0];
	witchitem[2]._iCreateInfo = lvl;
	witchitem[2]._iStatFlag = true;

	if (gbIsHellfire) {
		iCnt = GenerateRnd(WITCH_ITEMS - 10) + 10;
		maxValue = 200000;

		int books = GenerateRnd(4);
		for (int i = 114, bCnt = 0; i <= 117 && bCnt < books; ++i) {
			if (!WitchItemOk(i))
				continue;
			if (lvl < AllItemsList[i].iMinMLvl)
				continue;

			memset(&items[0], 0, sizeof(*items));
			items[0]._iSeed = AdvanceRndSeed();
			GenerateRnd(1);

			GetItemAttrs(0, i, lvl);
			witchitem[j] = items[0];
			witchitem[j]._iCreateInfo = lvl | CF_WITCH;
			witchitem[j]._iIdentified = true;
			WitchBookLevel(j);
			witchitem[j]._iStatFlag = StoreStatOk(&witchitem[j]);
			j++;
			bCnt++;
		}
	} else {
		iCnt = GenerateRnd(WITCH_ITEMS - 12) + 10;
		maxValue = 140000;
	}

	for (int i = j; i < iCnt; i++) {
		do {
			memset(&items[0], 0, sizeof(*items));
			items[0]._iSeed = AdvanceRndSeed();
			idata = RndWitchItem(lvl) - 1;
			GetItemAttrs(0, idata, lvl);
			maxlvl = -1;
			if (GenerateRnd(100) <= 5)
				maxlvl = 2 * lvl;
			if (maxlvl == -1 && items[0]._iMiscId == IMISC_STAFF)
				maxlvl = 2 * lvl;
			if (maxlvl != -1)
				GetItemBonus(0, maxlvl / 2, maxlvl, true, true);
		} while (items[0]._iIvalue > maxValue);
		witchitem[i] = items[0];
		witchitem[i]._iCreateInfo = lvl | CF_WITCH;
		witchitem[i]._iIdentified = true;
		WitchBookLevel(i);
		witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
	}

	for (int i = iCnt; i < WITCH_ITEMS; i++)
		witchitem[i]._itype = ITYPE_NONE;

	SortVendor(witchitem + PinnedItemCount);
}

int RndBoyItem(int lvl)
{
	return RndVendorItem<PremiumItemOk>(0, lvl);
}

void SpawnBoy(int lvl)
{
	int ivalue = 0;
	bool keepgoing = false;
	int count = 0;

	auto &myPlayer = plr[myplr];

	HeroClass pc = myPlayer._pClass;
	int strength = std::max(myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength), myPlayer._pStrength);
	int dexterity = std::max(myPlayer.GetMaximumAttributeValue(CharacterAttribute::Dexterity), myPlayer._pDexterity);
	int magic = std::max(myPlayer.GetMaximumAttributeValue(CharacterAttribute::Magic), myPlayer._pMagic);
	strength += strength / 5;
	dexterity += dexterity / 5;
	magic += magic / 5;

	if (boylevel >= (lvl / 2) && !boyitem.isEmpty())
		return;
	do {
		keepgoing = false;
		memset(&items[0], 0, sizeof(*items));
		items[0]._iSeed = AdvanceRndSeed();
		int itype = RndBoyItem(lvl) - 1;
		GetItemAttrs(0, itype, lvl);
		GetItemBonus(0, lvl, 2 * lvl, true, true);

		if (!gbIsHellfire) {
			if (items[0]._iIvalue > 90000) {
				keepgoing = true; // prevent breaking the do/while loop too early by failing hellfire's condition in while
				continue;
			}
			break;
		}

		ivalue = 0;

		int itemType = items[0]._itype;

		switch (itemType) {
		case ITYPE_LARMOR:
		case ITYPE_MARMOR:
		case ITYPE_HARMOR: {
			const auto *const mostValuablePlayerArmor = myPlayer.GetMostValuableItem(
			    [](const ItemStruct &item) {
				    return item._itype == ITYPE_LARMOR
				        || item._itype == ITYPE_MARMOR
				        || item._itype == ITYPE_HARMOR;
			    });

			ivalue = mostValuablePlayerArmor == nullptr ? 0 : mostValuablePlayerArmor->_iIvalue;
			break;
		}
		case ITYPE_SHIELD:
		case ITYPE_AXE:
		case ITYPE_BOW:
		case ITYPE_MACE:
		case ITYPE_SWORD:
		case ITYPE_HELM:
		case ITYPE_STAFF:
		case ITYPE_RING:
		case ITYPE_AMULET: {
			const auto *const mostValuablePlayerItem = myPlayer.GetMostValuableItem(
			    [itemType](const ItemStruct &item) { return item._itype == itemType; });

			ivalue = mostValuablePlayerItem == nullptr ? 0 : mostValuablePlayerItem->_iIvalue;
			break;
		}
		}
		ivalue *= 0.8;

		count++;

		if (count < 200) {
			switch (pc) {
			case HeroClass::Warrior:
				if (itemType == ITYPE_BOW || itemType == ITYPE_STAFF)
					ivalue = INT_MAX;
				break;
			case HeroClass::Rogue:
				if (itemType == ITYPE_SWORD || itemType == ITYPE_STAFF || itemType == ITYPE_AXE || itemType == ITYPE_MACE || itemType == ITYPE_SHIELD)
					ivalue = INT_MAX;
				break;
			case HeroClass::Sorcerer:
				if (itemType == ITYPE_STAFF || itemType == ITYPE_AXE || itemType == ITYPE_BOW || itemType == ITYPE_MACE)
					ivalue = INT_MAX;
				break;
			case HeroClass::Monk:
				if (itemType == ITYPE_BOW || itemType == ITYPE_MARMOR || itemType == ITYPE_SHIELD || itemType == ITYPE_MACE)
					ivalue = INT_MAX;
				break;
			case HeroClass::Bard:
				if (itemType == ITYPE_AXE || itemType == ITYPE_MACE || itemType == ITYPE_STAFF)
					ivalue = INT_MAX;
				break;
			case HeroClass::Barbarian:
				if (itemType == ITYPE_BOW || itemType == ITYPE_STAFF)
					ivalue = INT_MAX;
				break;
			}
		}
	} while (keepgoing
	    || ((
	            items[0]._iIvalue > 200000
	            || items[0]._iMinStr > strength
	            || items[0]._iMinMag > magic
	            || items[0]._iMinDex > dexterity
	            || items[0]._iIvalue < ivalue)
	        && count < 250));
	boyitem = items[0];
	boyitem._iCreateInfo = lvl | CF_BOY;
	boyitem._iIdentified = true;
	boyitem._iStatFlag = StoreStatOk(&boyitem);
	boylevel = lvl / 2;
}

bool HealerItemOk(int i)
{
	if (AllItemsList[i].itype != ITYPE_MISC)
		return false;

	if (AllItemsList[i].iMiscId == IMISC_SCROLL)
		return AllItemsList[i].iSpell == SPL_HEAL;
	if (AllItemsList[i].iMiscId == IMISC_SCROLLT)
		return AllItemsList[i].iSpell == SPL_HEALOTHER && gbIsMultiplayer;

	if (!gbIsMultiplayer) {
		auto &myPlayer = plr[myplr];

		if (AllItemsList[i].iMiscId == IMISC_ELIXSTR)
			return !gbIsHellfire || myPlayer._pBaseStr < myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength);
		if (AllItemsList[i].iMiscId == IMISC_ELIXMAG)
			return !gbIsHellfire || myPlayer._pBaseMag < myPlayer.GetMaximumAttributeValue(CharacterAttribute::Magic);
		if (AllItemsList[i].iMiscId == IMISC_ELIXDEX)
			return !gbIsHellfire || myPlayer._pBaseDex < myPlayer.GetMaximumAttributeValue(CharacterAttribute::Dexterity);
		if (AllItemsList[i].iMiscId == IMISC_ELIXVIT)
			return !gbIsHellfire || myPlayer._pBaseVit < myPlayer.GetMaximumAttributeValue(CharacterAttribute::Vitality);
	}

	if (AllItemsList[i].iMiscId == IMISC_REJUV)
		return true;
	if (AllItemsList[i].iMiscId == IMISC_FULLREJUV)
		return true;

	return false;
}

int RndHealerItem(int lvl)
{
	return RndVendorItem<HealerItemOk>(0, lvl);
}

void SpawnHealer(int lvl)
{
	constexpr int PinnedItemCount = 2;

	int srnd;

	memset(&items[0], 0, sizeof(*items));
	GetItemAttrs(0, IDI_HEAL, 1);
	healitem[0] = items[0];
	healitem[0]._iCreateInfo = lvl;
	healitem[0]._iStatFlag = true;

	memset(&items[0], 0, sizeof(*items));
	GetItemAttrs(0, IDI_FULLHEAL, 1);
	healitem[1] = items[0];
	healitem[1]._iCreateInfo = lvl;
	healitem[1]._iStatFlag = true;

	if (gbIsMultiplayer) {
		memset(&items[0], 0, sizeof(*items));
		GetItemAttrs(0, IDI_RESURRECT, 1);
		healitem[2] = items[0];
		healitem[2]._iCreateInfo = lvl;
		healitem[2]._iStatFlag = true;

		srnd = 3;
	} else {
		srnd = 2;
	}
	int nsi = GenerateRnd(gbIsHellfire ? 10 : 8) + 10;
	for (int i = srnd; i < nsi; i++) {
		memset(&items[0], 0, sizeof(*items));
		items[0]._iSeed = AdvanceRndSeed();
		int itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(0, itype, lvl);
		healitem[i] = items[0];
		healitem[i]._iCreateInfo = lvl | CF_HEALER;
		healitem[i]._iIdentified = true;
		healitem[i]._iStatFlag = StoreStatOk(&healitem[i]);
	}
	for (int i = nsi; i < 20; i++) {
		healitem[i]._itype = ITYPE_NONE;
	}
	SortVendor(healitem + PinnedItemCount);
}

void SpawnStoreGold()
{
	memset(&items[0], 0, sizeof(*items));
	GetItemAttrs(0, IDI_GOLD, 1);
	golditem = items[0];
	golditem._iStatFlag = true;
}

void RecreateSmithItem(int ii, int lvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndSmithItem(lvl) - 1;
	GetItemAttrs(ii, itype, lvl);

	items[ii]._iSeed = iseed;
	items[ii]._iCreateInfo = lvl | CF_SMITH;
	items[ii]._iIdentified = true;
}

void RecreatePremiumItem(int ii, int plvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndPremiumItem(plvl / 4, plvl) - 1;
	GetItemAttrs(ii, itype, plvl);
	GetItemBonus(ii, plvl / 2, plvl, true, !gbIsHellfire);

	items[ii]._iSeed = iseed;
	items[ii]._iCreateInfo = plvl | CF_SMITHPREMIUM;
	items[ii]._iIdentified = true;
}

void RecreateBoyItem(int ii, int lvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndBoyItem(lvl) - 1;
	GetItemAttrs(ii, itype, lvl);
	GetItemBonus(ii, lvl, 2 * lvl, true, true);

	items[ii]._iSeed = iseed;
	items[ii]._iCreateInfo = lvl | CF_BOY;
	items[ii]._iIdentified = true;
}

void RecreateWitchItem(int ii, int idx, int lvl, int iseed)
{
	if (idx == IDI_MANA || idx == IDI_FULLMANA || idx == IDI_PORTAL) {
		GetItemAttrs(ii, idx, lvl);
	} else if (gbIsHellfire && idx >= 114 && idx <= 117) {
		SetRndSeed(iseed);
		GenerateRnd(1);
		GetItemAttrs(ii, idx, lvl);
	} else {
		SetRndSeed(iseed);
		int itype = RndWitchItem(lvl) - 1;
		GetItemAttrs(ii, itype, lvl);
		int iblvl = -1;
		if (GenerateRnd(100) <= 5)
			iblvl = 2 * lvl;
		if (iblvl == -1 && items[ii]._iMiscId == IMISC_STAFF)
			iblvl = 2 * lvl;
		if (iblvl != -1)
			GetItemBonus(ii, iblvl / 2, iblvl, true, true);
	}

	items[ii]._iSeed = iseed;
	items[ii]._iCreateInfo = lvl | CF_WITCH;
	items[ii]._iIdentified = true;
}

void RecreateHealerItem(int ii, int idx, int lvl, int iseed)
{
	if (idx == IDI_HEAL || idx == IDI_FULLHEAL || idx == IDI_RESURRECT) {
		GetItemAttrs(ii, idx, lvl);
	} else {
		SetRndSeed(iseed);
		int itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(ii, itype, lvl);
	}

	items[ii]._iSeed = iseed;
	items[ii]._iCreateInfo = lvl | CF_HEALER;
	items[ii]._iIdentified = true;
}

void RecreateTownItem(int ii, int idx, uint16_t icreateinfo, int iseed)
{
	if ((icreateinfo & CF_SMITH) != 0)
		RecreateSmithItem(ii, icreateinfo & CF_LEVEL, iseed);
	else if ((icreateinfo & CF_SMITHPREMIUM) != 0)
		RecreatePremiumItem(ii, icreateinfo & CF_LEVEL, iseed);
	else if ((icreateinfo & CF_BOY) != 0)
		RecreateBoyItem(ii, icreateinfo & CF_LEVEL, iseed);
	else if ((icreateinfo & CF_WITCH) != 0)
		RecreateWitchItem(ii, idx, icreateinfo & CF_LEVEL, iseed);
	else if ((icreateinfo & CF_HEALER) != 0)
		RecreateHealerItem(ii, idx, icreateinfo & CF_LEVEL, iseed);
}

void RecalcStoreStats()
{
	for (auto &item : smithitem) {
		if (!item.isEmpty()) {
			item._iStatFlag = StoreStatOk(&item);
		}
	}
	for (auto &item : premiumitems) {
		if (!item.isEmpty()) {
			item._iStatFlag = StoreStatOk(&item);
		}
	}
	for (int i = 0; i < 20; i++) {
		if (!witchitem[i].isEmpty()) {
			witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
		}
	}
	for (auto &item : healitem) {
		if (!item.isEmpty()) {
			item._iStatFlag = StoreStatOk(&item);
		}
	}
	boyitem._iStatFlag = StoreStatOk(&boyitem);
}

int ItemNoFlippy()
{
	int r = itemactive[numitems - 1];
	items[r].AnimInfo.CurrentFrame = items[r].AnimInfo.NumberOfFrames;
	items[r]._iAnimFlag = false;
	items[r]._iSelFlag = 1;

	return r;
}

void CreateSpellBook(Point position, spell_id ispell, bool sendmsg, bool delta)
{
	int lvl = currlevel;

	if (gbIsHellfire) {
		lvl = GetSpellBookLevel(ispell) + 1;
		if (lvl < 1) {
			return;
		}
	}

	int idx = RndTypeItems(ITYPE_MISC, IMISC_BOOK, lvl);
	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();

	while (true) {
		memset(&items[ii], 0, sizeof(*items));
		SetupAllItems(ii, idx, AdvanceRndSeed(), 2 * lvl, 1, true, false, delta);
		if (items[ii]._iMiscId == IMISC_BOOK && items[ii]._iSpell == ispell)
			break;
	}
	GetSuperItemSpace(position, ii);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
	if (delta)
		DeltaAddItem(ii);
}

static void CreateMagicItem(Point position, int lvl, int imisc, int imid, int icurs, bool sendmsg, bool delta)
{
	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();
	int idx = RndTypeItems(imisc, imid, lvl);

	while (true) {
		memset(&items[ii], 0, sizeof(*items));
		SetupAllItems(ii, idx, AdvanceRndSeed(), 2 * lvl, 1, true, false, delta);
		if (items[ii]._iCurs == icurs)
			break;

		idx = RndTypeItems(imisc, imid, lvl);
	}
	GetSuperItemSpace(position, ii);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
	if (delta)
		DeltaAddItem(ii);
}

void CreateMagicArmor(Point position, int imisc, int icurs, bool sendmsg, bool delta)
{
	int lvl = items_get_currlevel();
	CreateMagicItem(position, lvl, imisc, IMISC_NONE, icurs, sendmsg, delta);
}

void CreateAmulet(Point position, int lvl, bool sendmsg, bool delta)
{
	CreateMagicItem(position, lvl, ITYPE_AMULET, IMISC_AMULET, ICURS_AMULET, sendmsg, delta);
}

void CreateMagicWeapon(Point position, int imisc, int icurs, bool sendmsg, bool delta)
{
	int imid = IMISC_NONE;
	if (imisc == ITYPE_STAFF)
		imid = IMISC_STAFF;

	int curlv = items_get_currlevel();

	CreateMagicItem(position, curlv, imisc, imid, icurs, sendmsg, delta);
}

static void NextItemRecord(int i)
{
	gnNumGetRecords--;

	if (gnNumGetRecords == 0) {
		return;
	}

	itemrecord[i].dwTimestamp = itemrecord[gnNumGetRecords].dwTimestamp;
	itemrecord[i].nSeed = itemrecord[gnNumGetRecords].nSeed;
	itemrecord[i].wCI = itemrecord[gnNumGetRecords].wCI;
	itemrecord[i].nIndex = itemrecord[gnNumGetRecords].nIndex;
}

bool GetItemRecord(int nSeed, uint16_t wCI, int nIndex)
{
	uint32_t ticks = SDL_GetTicks();

	for (int i = 0; i < gnNumGetRecords; i++) {
		if (ticks - itemrecord[i].dwTimestamp > 6000) {
			NextItemRecord(i);
			i--;
		} else if (nSeed == itemrecord[i].nSeed && wCI == itemrecord[i].wCI && nIndex == itemrecord[i].nIndex) {
			return false;
		}
	}

	return true;
}

void SetItemRecord(int nSeed, uint16_t wCI, int nIndex)
{
	uint32_t ticks = SDL_GetTicks();

	if (gnNumGetRecords == MAXITEMS) {
		return;
	}

	itemrecord[gnNumGetRecords].dwTimestamp = ticks;
	itemrecord[gnNumGetRecords].nSeed = nSeed;
	itemrecord[gnNumGetRecords].wCI = wCI;
	itemrecord[gnNumGetRecords].nIndex = nIndex;
	gnNumGetRecords++;
}

void PutItemRecord(int nSeed, uint16_t wCI, int nIndex)
{
	uint32_t ticks = SDL_GetTicks();

	for (int i = 0; i < gnNumGetRecords; i++) {
		if (ticks - itemrecord[i].dwTimestamp > 6000) {
			NextItemRecord(i);
			i--;
		} else if (nSeed == itemrecord[i].nSeed && wCI == itemrecord[i].wCI && nIndex == itemrecord[i].nIndex) {
			NextItemRecord(i);
			break;
		}
	}
}

void ItemStruct::SetNewAnimation(bool showAnimation)
{
	int it = ItemCAnimTbl[_iCurs];
	int numberOfFrames = ItemAnimLs[it];
	auto *pCelSprite = itemanims[it] ? &*itemanims[it] : nullptr;
	if (_iCurs != ICURS_MAGIC_ROCK)
		AnimInfo.SetNewAnimation(pCelSprite, numberOfFrames, 1, AnimationDistributionFlags::ProcessAnimationPending, 0, numberOfFrames);
	else
		AnimInfo.SetNewAnimation(pCelSprite, numberOfFrames, 1);
	_iPostDraw = false;
	_iRequest = false;
	if (showAnimation) {
		_iAnimFlag = true;
		_iSelFlag = 0;
	} else {
		AnimInfo.CurrentFrame = AnimInfo.NumberOfFrames;
		_iAnimFlag = false;
		_iSelFlag = 1;
	}
}

} // namespace devilution
