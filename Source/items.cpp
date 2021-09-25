/**
 * @file items.cpp
 *
 * Implementation of item functionality.
 */
#include "items.h"

#include <algorithm>
#include <bitset>
#ifdef _DEBUG
#include <random>
#endif
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
#include "inv_iterators.hpp"
#include "lighting.h"
#include "missiles.h"
#include "options.h"
#include "player.h"
#include "stores.h"
#include "utils/language.h"
#include "utils/math.h"
#include "utils/stdcompat/algorithm.hpp"

namespace devilution {

/** Contains the items on ground in the current game. */
Item Items[MAXITEMS + 1];
int ActiveItems[MAXITEMS];
int ActiveItemCount;
int AvailableItems[MAXITEMS];
bool ShowUniqueItemInfoBox;
CornerStoneStruct CornerStone;
bool UniqueItemFlags[128];
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

namespace {

std::optional<CelSprite> itemanims[ITEMTYPES];

enum class PlayerArmorGraphic : uint8_t {
	// clang-format off
	Light  = 0,
	Medium = 1 << 4,
	Heavy  = 1 << 5,
	// clang-format on
};

Item curruitem;

/** Holds item get records, tracking items being recently looted. This is in an effort to prevent items being picked up more than once. */
ItemGetRecordStruct itemrecord[MAXITEMS];

bool itemhold[3][3];

/** Specifies the number of active item get records. */
int gnNumGetRecords;

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

bool IsPrefixValidForItemType(int i, int flgs)
{
	int itemTypes = ItemPrefixes[i].PLIType;

	if (!gbIsHellfire) {
		if (i > 82)
			return false;

		if (i >= 12 && i <= 20)
			itemTypes &= ~PLT_STAFF;
	}

	return (flgs & itemTypes) != 0;
}

bool IsSuffixValidForItemType(int i, int flgs)
{
	int itemTypes = ItemSuffixes[i].PLIType;

	if (!gbIsHellfire) {
		if (i > 94)
			return false;

		if ((i >= 0 && i <= 1)
		    || (i >= 14 && i <= 15)
		    || (i >= 21 && i <= 22)
		    || (i >= 34 && i <= 36)
		    || (i >= 41 && i <= 44)
		    || (i >= 60 && i <= 63))
			itemTypes &= ~PLT_STAFF;
	}

	return (flgs & itemTypes) != 0;
}

int ItemsGetCurrlevel()
{
	int lvl = currlevel;
	if (currlevel >= 17 && currlevel <= 20)
		lvl = currlevel - 8;
	if (currlevel >= 21 && currlevel <= 24)
		lvl = currlevel - 7;

	return lvl;
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
		position = Point { GenerateRnd(80), GenerateRnd(80) } + Displacement { 16, 16 };
	} while (!ItemPlace(position));

	return position;
}

void AddInitItems()
{
	int curlv = ItemsGetCurrlevel();
	int rnd = GenerateRnd(3) + 3;
	for (int j = 0; j < rnd; j++) {
		int ii = AllocateItem();
		auto &item = Items[ii];

		Point position = GetRandomAvailableItemPosition();
		item.position = position;

		dItem[position.x][position.y] = ii + 1;

		item._iSeed = AdvanceRndSeed();
		SetRndSeed(item._iSeed);

		if (GenerateRnd(2) != 0)
			GetItemAttrs(item, IDI_HEAL, curlv);
		else
			GetItemAttrs(item, IDI_MANA, curlv);

		item._iCreateInfo = curlv | CF_PREGEN;
		SetupItem(item);
		item.AnimInfo.CurrentFrame = item.AnimInfo.NumberOfFrames;
		item._iAnimFlag = false;
		item._iSelFlag = 1;
		DeltaAddItem(ii);
	}
}

void SpawnNote()
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

void CalcSelfItems(Player &player)
{
	int sa = 0;
	int ma = 0;
	int da = 0;
	for (int i = 0; i < NUM_INVLOC; i++) {
		auto &equipment = player.InvBody[i];
		if (!equipment.isEmpty()) {
			equipment._iStatFlag = true;
			if (equipment._iIdentified) {
				sa += equipment._iPLStr;
				ma += equipment._iPLMag;
				da += equipment._iPLDex;
			}
		}
	}

	bool changeflag;
	do {
		changeflag = false;
		auto *pi = player.InvBody;
		for (int i = 0; i < NUM_INVLOC; i++, pi++) {
			if (!pi->isEmpty() && pi->_iStatFlag) {
				bool sf = true;
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

bool ItemMinStats(const Player &player, Item &x)
{
	if (player._pMagic < x._iMinMag)
		return false;

	if (player._pStrength < x._iMinStr)
		return false;

	if (player._pDexterity < x._iMinDex)
		return false;

	return true;
}

void CalcPlrItemMin(Player &player)
{
	for (Item &item : InventoryAndBeltPlayerItemsRange { player }) {
		item._iStatFlag = ItemMinStats(player, item);
	}
}

void WitchBookLevel(int ii)
{
	if (witchitem[ii]._iMiscId != IMISC_BOOK)
		return;
	witchitem[ii]._iMinMag = spelldata[witchitem[ii]._iSpell].sMinInt;
	int8_t spellLevel = Players[MyPlayerId]._pSplLvl[witchitem[ii]._iSpell];
	while (spellLevel > 0) {
		witchitem[ii]._iMinMag += 20 * witchitem[ii]._iMinMag / 100;
		spellLevel--;
		if (witchitem[ii]._iMinMag + 20 * witchitem[ii]._iMinMag / 100 > 255) {
			witchitem[ii]._iMinMag = 255;
			spellLevel = 0;
		}
	}
}

bool StoreStatOk(Item &item)
{
	const auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pStrength < item._iMinStr)
		return false;
	if (myPlayer._pMagic < item._iMinMag)
		return false;
	if (myPlayer._pDexterity < item._iMinDex)
		return false;

	return true;
}

void CalcPlrBookVals(Player &player)
{
	if (currlevel == 0) {
		for (int i = 1; !witchitem[i].isEmpty(); i++) {
			WitchBookLevel(i);
			witchitem[i]._iStatFlag = StoreStatOk(witchitem[i]);
		}
	}

	for (int i = 0; i < player._pNumInv; i++) {
		if (player.InvList[i]._itype == ItemType::Misc && player.InvList[i]._iMiscId == IMISC_BOOK) {
			player.InvList[i]._iMinMag = spelldata[player.InvList[i]._iSpell].sMinInt;
			int8_t spellLevel = player._pSplLvl[player.InvList[i]._iSpell];

			while (spellLevel != 0) {
				player.InvList[i]._iMinMag += 20 * player.InvList[i]._iMinMag / 100;
				spellLevel--;
				if (player.InvList[i]._iMinMag + 20 * player.InvList[i]._iMinMag / 100 > 255) {
					player.InvList[i]._iMinMag = 255;
					spellLevel = 0;
				}
			}
			player.InvList[i]._iStatFlag = ItemMinStats(player, player.InvList[i]);
		}
	}
}

void SetPlrHandSeed(Item &item, int iseed)
{
	item._iSeed = iseed;
}

bool GetItemSpace(Point position, int8_t inum)
{
	int xx = 0;
	int yy = 0;
	for (int j = position.y - 1; j <= position.y + 1; j++) {
		xx = 0;
		for (int i = position.x - 1; i <= position.x + 1; i++) {
			itemhold[xx][yy] = ItemSpaceOk({ i, j });
			xx++;
		}
		yy++;
	}

	bool savail = false;
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 3; i++) { // NOLINT(modernize-loop-convert)
			if (itemhold[i][j])
				savail = true;
		}
	}

	int rs = GenerateRnd(15) + 1;

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
	Items[inum].position = { xx, yy };
	dItem[xx][yy] = inum + 1;

	return true;
}

void GetSuperItemSpace(Point position, int8_t inum)
{
	Point positionToCheck = position;
	if (GetItemSpace(positionToCheck, inum))
		return;
	for (int k = 2; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			for (int i = -k; i <= k; i++) {
				Displacement offset = { i, j };
				positionToCheck = position + offset;
				if (!ItemSpaceOk(positionToCheck))
					continue;
				Items[inum].position = positionToCheck;
				dItem[positionToCheck.x][positionToCheck.y] = inum + 1;
				return;
			}
		}
	}
}

void CalcItemValue(Item &item)
{
	int v = item._iVMult1 + item._iVMult2;
	if (v > 0) {
		v *= item._ivalue;
	}
	if (v < 0) {
		v = item._ivalue / v;
	}
	v = item._iVAdd1 + item._iVAdd2 + v;
	item._iIvalue = std::max(v, 1);
}

void GetBookSpell(Item &item, int lvl)
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
	strcat(item._iName, pgettext("spell", spelldata[bs].sNameText));
	strcat(item._iIName, pgettext("spell", spelldata[bs].sNameText));
	item._iSpell = bs;
	item._iMinMag = spelldata[bs].sMinInt;
	item._ivalue += spelldata[bs].sBookCost;
	item._iIvalue += spelldata[bs].sBookCost;
	if (spelldata[bs].sType == STYPE_FIRE)
		item._iCurs = ICURS_BOOK_RED;
	else if (spelldata[bs].sType == STYPE_LIGHTNING)
		item._iCurs = ICURS_BOOK_BLUE;
	else if (spelldata[bs].sType == STYPE_MAGIC)
		item._iCurs = ICURS_BOOK_GREY;
}

int RndPL(int param1, int param2)
{
	return param1 + GenerateRnd(param2 - param1 + 1);
}

int CalculateToHitBonus(int level)
{
	switch (level) {
	case -50:
		return -RndPL(6, 10);
	case -25:
		return -RndPL(1, 5);
	case 20:
		return RndPL(1, 5);
	case 36:
		return RndPL(6, 10);
	case 51:
		return RndPL(11, 15);
	case 66:
		return RndPL(16, 20);
	case 81:
		return RndPL(21, 30);
	case 96:
		return RndPL(31, 40);
	case 111:
		return RndPL(41, 50);
	case 126:
		return RndPL(51, 75);
	case 151:
		return RndPL(76, 100);
	default:
		app_fatal("Unknown to hit bonus");
	}
}

int SaveItemPower(Item &item, const ItemPower &power)
{
	int r = RndPL(power.param1, power.param2);

	switch (power.type) {
	case IPL_TOHIT:
		item._iPLToHit += r;
		break;
	case IPL_TOHIT_CURSE:
		item._iPLToHit -= r;
		break;
	case IPL_DAMP:
		item._iPLDam += r;
		break;
	case IPL_DAMP_CURSE:
		item._iPLDam -= r;
		break;
	case IPL_DOPPELGANGER:
		item._iDamAcFlags |= ISPLHF_DOPPELGANGER;
		[[fallthrough]];
	case IPL_TOHIT_DAMP:
		r = RndPL(power.param1, power.param2);
		item._iPLDam += r;
		item._iPLToHit += CalculateToHitBonus(power.param1);
		break;
	case IPL_TOHIT_DAMP_CURSE:
		item._iPLDam -= r;
		item._iPLToHit += CalculateToHitBonus(-power.param1);
		break;
	case IPL_ACP:
		item._iPLAC += r;
		break;
	case IPL_ACP_CURSE:
		item._iPLAC -= r;
		break;
	case IPL_SETAC:
		item._iAC = r;
		break;
	case IPL_AC_CURSE:
		item._iAC -= r;
		break;
	case IPL_FIRERES:
		item._iPLFR += r;
		break;
	case IPL_LIGHTRES:
		item._iPLLR += r;
		break;
	case IPL_MAGICRES:
		item._iPLMR += r;
		break;
	case IPL_ALLRES:
		item._iPLFR = std::max(item._iPLFR + r, 0);
		item._iPLLR = std::max(item._iPLLR + r, 0);
		item._iPLMR = std::max(item._iPLMR + r, 0);
		break;
	case IPL_SPLLVLADD:
		item._iSplLvlAdd = r;
		break;
	case IPL_CHARGES:
		item._iCharges *= power.param1;
		item._iMaxCharges = item._iCharges;
		break;
	case IPL_SPELL:
		item._iSpell = static_cast<spell_id>(power.param1);
		item._iCharges = power.param2;
		item._iMaxCharges = power.param2;
		break;
	case IPL_FIREDAM:
		item._iFlags |= ISPL_FIREDAM;
		item._iFlags &= ~ISPL_LIGHTDAM;
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 0;
		item._iLMaxDam = 0;
		break;
	case IPL_LIGHTDAM:
		item._iFlags |= ISPL_LIGHTDAM;
		item._iFlags &= ~ISPL_FIREDAM;
		item._iLMinDam = power.param1;
		item._iLMaxDam = power.param2;
		item._iFMinDam = 0;
		item._iFMaxDam = 0;
		break;
	case IPL_STR:
		item._iPLStr += r;
		break;
	case IPL_STR_CURSE:
		item._iPLStr -= r;
		break;
	case IPL_MAG:
		item._iPLMag += r;
		break;
	case IPL_MAG_CURSE:
		item._iPLMag -= r;
		break;
	case IPL_DEX:
		item._iPLDex += r;
		break;
	case IPL_DEX_CURSE:
		item._iPLDex -= r;
		break;
	case IPL_VIT:
		item._iPLVit += r;
		break;
	case IPL_VIT_CURSE:
		item._iPLVit -= r;
		break;
	case IPL_ATTRIBS:
		item._iPLStr += r;
		item._iPLMag += r;
		item._iPLDex += r;
		item._iPLVit += r;
		break;
	case IPL_ATTRIBS_CURSE:
		item._iPLStr -= r;
		item._iPLMag -= r;
		item._iPLDex -= r;
		item._iPLVit -= r;
		break;
	case IPL_GETHIT_CURSE:
		item._iPLGetHit += r;
		break;
	case IPL_GETHIT:
		item._iPLGetHit -= r;
		break;
	case IPL_LIFE:
		item._iPLHP += r << 6;
		break;
	case IPL_LIFE_CURSE:
		item._iPLHP -= r << 6;
		break;
	case IPL_MANA:
		item._iPLMana += r << 6;
		drawmanaflag = true;
		break;
	case IPL_MANA_CURSE:
		item._iPLMana -= r << 6;
		drawmanaflag = true;
		break;
	case IPL_DUR: {
		int bonus = r * item._iMaxDur / 100;
		item._iMaxDur += bonus;
		item._iDurability += bonus;
	} break;
	case IPL_CRYSTALLINE:
		item._iPLDam += 140 + r * 2;
		[[fallthrough]];
	case IPL_DUR_CURSE:
		item._iMaxDur -= r * item._iMaxDur / 100;
		item._iMaxDur = std::max<uint8_t>(item._iMaxDur, 1);
		item._iDurability = item._iMaxDur;
		break;
	case IPL_INDESTRUCTIBLE:
		item._iDurability = DUR_INDESTRUCTIBLE;
		item._iMaxDur = DUR_INDESTRUCTIBLE;
		break;
	case IPL_LIGHT:
		item._iPLLight += power.param1;
		break;
	case IPL_LIGHT_CURSE:
		item._iPLLight -= power.param1;
		break;
	case IPL_MULT_ARROWS:
		item._iFlags |= ISPL_MULT_ARROWS;
		break;
	case IPL_FIRE_ARROWS:
		item._iFlags |= ISPL_FIRE_ARROWS;
		item._iFlags &= ~ISPL_LIGHT_ARROWS;
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 0;
		item._iLMaxDam = 0;
		break;
	case IPL_LIGHT_ARROWS:
		item._iFlags |= ISPL_LIGHT_ARROWS;
		item._iFlags &= ~ISPL_FIRE_ARROWS;
		item._iLMinDam = power.param1;
		item._iLMaxDam = power.param2;
		item._iFMinDam = 0;
		item._iFMaxDam = 0;
		break;
	case IPL_FIREBALL:
		item._iFlags |= (ISPL_LIGHT_ARROWS | ISPL_FIRE_ARROWS);
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 0;
		item._iLMaxDam = 0;
		break;
	case IPL_THORNS:
		item._iFlags |= ISPL_THORNS;
		break;
	case IPL_NOMANA:
		item._iFlags |= ISPL_NOMANA;
		drawmanaflag = true;
		break;
	case IPL_NOHEALPLR:
		item._iFlags |= ISPL_NOHEALPLR;
		break;
	case IPL_ABSHALFTRAP:
		item._iFlags |= ISPL_ABSHALFTRAP;
		break;
	case IPL_KNOCKBACK:
		item._iFlags |= ISPL_KNOCKBACK;
		break;
	case IPL_3XDAMVDEM:
		item._iFlags |= ISPL_3XDAMVDEM;
		break;
	case IPL_ALLRESZERO:
		item._iFlags |= ISPL_ALLRESZERO;
		break;
	case IPL_NOHEALMON:
		item._iFlags |= ISPL_NOHEALMON;
		break;
	case IPL_STEALMANA:
		if (power.param1 == 3)
			item._iFlags |= ISPL_STEALMANA_3;
		if (power.param1 == 5)
			item._iFlags |= ISPL_STEALMANA_5;
		drawmanaflag = true;
		break;
	case IPL_STEALLIFE:
		if (power.param1 == 3)
			item._iFlags |= ISPL_STEALLIFE_3;
		if (power.param1 == 5)
			item._iFlags |= ISPL_STEALLIFE_5;
		drawhpflag = true;
		break;
	case IPL_TARGAC:
		if (gbIsHellfire)
			item._iPLEnAc = power.param1;
		else
			item._iPLEnAc += r;
		break;
	case IPL_FASTATTACK:
		if (power.param1 == 1)
			item._iFlags |= ISPL_QUICKATTACK;
		if (power.param1 == 2)
			item._iFlags |= ISPL_FASTATTACK;
		if (power.param1 == 3)
			item._iFlags |= ISPL_FASTERATTACK;
		if (power.param1 == 4)
			item._iFlags |= ISPL_FASTESTATTACK;
		break;
	case IPL_FASTRECOVER:
		if (power.param1 == 1)
			item._iFlags |= ISPL_FASTRECOVER;
		if (power.param1 == 2)
			item._iFlags |= ISPL_FASTERRECOVER;
		if (power.param1 == 3)
			item._iFlags |= ISPL_FASTESTRECOVER;
		break;
	case IPL_FASTBLOCK:
		item._iFlags |= ISPL_FASTBLOCK;
		break;
	case IPL_DAMMOD:
		item._iPLDamMod += r;
		break;
	case IPL_RNDARROWVEL:
		item._iFlags |= ISPL_RNDARROWVEL;
		break;
	case IPL_SETDAM:
		item._iMinDam = power.param1;
		item._iMaxDam = power.param2;
		break;
	case IPL_SETDUR:
		item._iDurability = power.param1;
		item._iMaxDur = power.param1;
		break;
	case IPL_FASTSWING:
		item._iFlags |= ISPL_FASTERATTACK;
		break;
	case IPL_ONEHAND:
		item._iLoc = ILOC_ONEHAND;
		break;
	case IPL_DRAINLIFE:
		item._iFlags |= ISPL_DRAINLIFE;
		break;
	case IPL_RNDSTEALLIFE:
		item._iFlags |= ISPL_RNDSTEALLIFE;
		break;
	case IPL_INFRAVISION:
		item._iFlags |= ISPL_INFRAVISION;
		break;
	case IPL_NOMINSTR:
		item._iMinStr = 0;
		break;
	case IPL_INVCURS:
		item._iCurs = power.param1;
		break;
	case IPL_ADDACLIFE:
		item._iFlags |= (ISPL_LIGHT_ARROWS | ISPL_FIRE_ARROWS);
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 1;
		item._iLMaxDam = 0;
		break;
	case IPL_ADDMANAAC:
		item._iFlags |= (ISPL_LIGHTDAM | ISPL_FIREDAM);
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 2;
		item._iLMaxDam = 0;
		break;
	case IPL_FIRERESCLVL:
		item._iPLFR = 30 - Players[MyPlayerId]._pLevel;
		item._iPLFR = std::max<int16_t>(item._iPLFR, 0);
		break;
	case IPL_FIRERES_CURSE:
		item._iPLFR -= r;
		break;
	case IPL_LIGHTRES_CURSE:
		item._iPLLR -= r;
		break;
	case IPL_MAGICRES_CURSE:
		item._iPLMR -= r;
		break;
	case IPL_ALLRES_CURSE:
		item._iPLFR -= r;
		item._iPLLR -= r;
		item._iPLMR -= r;
		break;
	case IPL_DEVASTATION:
		item._iDamAcFlags |= ISPLHF_DEVASTATION;
		break;
	case IPL_DECAY:
		item._iDamAcFlags |= ISPLHF_DECAY;
		item._iPLDam += r;
		break;
	case IPL_PERIL:
		item._iDamAcFlags |= ISPLHF_PERIL;
		break;
	case IPL_JESTERS:
		item._iDamAcFlags |= ISPLHF_JESTERS;
		break;
	case IPL_ACDEMON:
		item._iDamAcFlags |= ISPLHF_ACDEMON;
		break;
	case IPL_ACUNDEAD:
		item._iDamAcFlags |= ISPLHF_ACUNDEAD;
		break;
	case IPL_MANATOLIFE: {
		int portion = ((Players[MyPlayerId]._pMaxManaBase >> 6) * 50 / 100) << 6;
		item._iPLMana -= portion;
		item._iPLHP += portion;
	} break;
	case IPL_LIFETOMANA: {
		int portion = ((Players[MyPlayerId]._pMaxHPBase >> 6) * 40 / 100) << 6;
		item._iPLHP -= portion;
		item._iPLMana += portion;
	} break;
	default:
		break;
	}

	return r;
}

bool StringInPanel(const char *str)
{
	return GetLineWidth(str, GameFont12, 2) < 254;
}

int PLVal(int pv, int p1, int p2, int minv, int maxv)
{
	if (p1 == p2)
		return minv;
	if (minv == maxv)
		return minv;
	return minv + (maxv - minv) * (100 * (pv - p1) / (p2 - p1)) / 100;
}

void SaveItemAffix(Item &item, const PLStruct &affix)
{
	auto power = affix.power;

	if (!gbIsHellfire) {
		if (power.type == IPL_TARGAC) {
			power.param1 = 1 << power.param1;
			power.param2 = 3 << power.param2;
		}
	}

	int value = SaveItemPower(item, power);

	value = PLVal(value, power.param1, power.param2, affix.minVal, affix.maxVal);
	if (item._iVAdd1 != 0 || item._iVMult1 != 0) {
		item._iVAdd2 = value;
		item._iVMult2 = affix.multVal;
	} else {
		item._iVAdd1 = value;
		item._iVMult1 = affix.multVal;
	}
}

void GetStaffPower(Item &item, int lvl, int bs, bool onlygood)
{
	int preidx = -1;
	if (GenerateRnd(10) == 0 || onlygood) {
		int nl = 0;
		int l[256];
		for (int j = 0; ItemPrefixes[j].power.type != IPL_INVALID; j++) {
			if (!IsPrefixValidForItemType(j, PLT_STAFF) || ItemPrefixes[j].PLMinLvl > lvl)
				continue;
			if (onlygood && !ItemPrefixes[j].PLOk)
				continue;
			l[nl] = j;
			nl++;
			if (ItemPrefixes[j].PLDouble) {
				l[nl] = j;
				nl++;
			}
		}
		if (nl != 0) {
			preidx = l[GenerateRnd(nl)];
			char istr[128];
			sprintf(istr, "%s %s", _(ItemPrefixes[preidx].PLName), item._iIName);
			strcpy(item._iIName, istr);
			item._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemAffix(item, ItemPrefixes[preidx]);
			item._iPrePower = ItemPrefixes[preidx].power.type;
		}
	}
	if (!StringInPanel(item._iIName)) {
		strcpy(item._iIName, _(AllItemsList[item.IDidx].iSName));
		char istr[128];
		if (preidx != -1) {
			sprintf(istr, "%s %s", _(ItemPrefixes[preidx].PLName), item._iIName);
			strcpy(item._iIName, istr);
		}
		strcpy(istr, fmt::format(_(/* TRANSLATORS: Constructs item names. Format: <Prefix> <Item> of <Suffix>. Example: King's Long Sword of the Whale */ "{:s} of {:s}"), item._iIName, pgettext("spell", spelldata[bs].sNameText)).c_str());
		strcpy(item._iIName, istr);
		if (item._iMagical == ITEM_QUALITY_NORMAL)
			strcpy(item._iName, item._iIName);
	}
	CalcItemValue(item);
}

void GetItemPower(Item &item, int minlvl, int maxlvl, affix_item_type flgs, bool onlygood)
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
		for (int j = 0; ItemPrefixes[j].power.type != IPL_INVALID; j++) {
			if (!IsPrefixValidForItemType(j, flgs))
				continue;
			if (ItemPrefixes[j].PLMinLvl < minlvl || ItemPrefixes[j].PLMinLvl > maxlvl)
				continue;
			if (onlygood && !ItemPrefixes[j].PLOk)
				continue;
			if (flgs == PLT_STAFF && ItemPrefixes[j].power.type == IPL_CHARGES)
				continue;
			l[nt] = j;
			nt++;
			if (ItemPrefixes[j].PLDouble) {
				l[nt] = j;
				nt++;
			}
		}
		if (nt != 0) {
			preidx = l[GenerateRnd(nt)];
			sprintf(istr, "%s %s", _(ItemPrefixes[preidx].PLName), item._iIName);
			strcpy(item._iIName, istr);
			item._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemAffix(item, ItemPrefixes[preidx]);
			item._iPrePower = ItemPrefixes[preidx].power.type;
			goe = ItemPrefixes[preidx].PLGOE;
		}
	}
	if (post != 0) {
		int nl = 0;
		for (int j = 0; ItemSuffixes[j].power.type != IPL_INVALID; j++) {
			if (IsSuffixValidForItemType(j, flgs)
			    && ItemSuffixes[j].PLMinLvl >= minlvl && ItemSuffixes[j].PLMinLvl <= maxlvl
			    && !((goe == GOE_GOOD && ItemSuffixes[j].PLGOE == GOE_EVIL) || (goe == GOE_EVIL && ItemSuffixes[j].PLGOE == GOE_GOOD))
			    && (!onlygood || ItemSuffixes[j].PLOk)) {
				l[nl] = j;
				nl++;
			}
		}
		if (nl != 0) {
			sufidx = l[GenerateRnd(nl)];
			strcpy(istr, fmt::format(_("{:s} of {:s}"), item._iIName, _(ItemSuffixes[sufidx].PLName)).c_str());
			strcpy(item._iIName, istr);
			item._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemAffix(item, ItemSuffixes[sufidx]);
			item._iSufPower = ItemSuffixes[sufidx].power.type;
		}
	}
	if (!StringInPanel(item._iIName)) {
		int aii = item.IDidx;
		if (AllItemsList[aii].iSName != nullptr)
			strcpy(item._iIName, _(AllItemsList[aii].iSName));
		else
			item._iName[0] = 0;

		if (preidx != -1) {
			sprintf(istr, "%s %s", _(ItemPrefixes[preidx].PLName), item._iIName);
			strcpy(item._iIName, istr);
		}
		if (sufidx != -1) {
			strcpy(istr, fmt::format(_("{:s} of {:s}"), item._iIName, _(ItemSuffixes[sufidx].PLName)).c_str());
			strcpy(item._iIName, istr);
		}
	}
	if (preidx != -1 || sufidx != -1)
		CalcItemValue(item);
}

void GetStaffSpell(Item &item, int lvl, bool onlygood)
{
	if (!gbIsHellfire && GenerateRnd(4) == 0) {
		GetItemPower(item, lvl / 2, lvl, PLT_STAFF, onlygood);
		return;
	}

	int maxSpells = gbIsHellfire ? MAX_SPELLS : 37;
	int l = lvl / 2;
	if (l == 0)
		l = 1;
	int rv = GenerateRnd(maxSpells) + 1;

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

	char istr[68];
	if (!StringInPanel(istr))
		strcpy(istr, fmt::format(_("{:s} of {:s}"), item._iName, pgettext("spell", spelldata[bs].sNameText)).c_str());
	strcpy(istr, fmt::format(_("Staff of {:s}"), pgettext("spell", spelldata[bs].sNameText)).c_str());
	strcpy(item._iName, istr);
	strcpy(item._iIName, istr);

	int minc = spelldata[bs].sStaffMin;
	int maxc = spelldata[bs].sStaffMax - minc + 1;
	item._iSpell = bs;
	item._iCharges = minc + GenerateRnd(maxc);
	item._iMaxCharges = item._iCharges;

	item._iMinMag = spelldata[bs].sMinInt;
	int v = item._iCharges * spelldata[bs].sStaffCost / 5;
	item._ivalue += v;
	item._iIvalue += v;
	GetStaffPower(item, lvl, bs, onlygood);
}

void GetOilType(Item &item, int maxLvl)
{
	int cnt = 2;
	int8_t rnd[32] = { 5, 6 };

	if (!gbIsMultiplayer) {
		if (maxLvl == 0)
			maxLvl = 1;

		cnt = 0;
		for (size_t j = 0; j < sizeof(OilLevels) / sizeof(OilLevels[0]); j++) {
			if (OilLevels[j] <= maxLvl) {
				rnd[cnt] = j;
				cnt++;
			}
		}
	}

	int8_t t = rnd[GenerateRnd(cnt)];

	strcpy(item._iName, _(OilNames[t]));
	strcpy(item._iIName, _(OilNames[t]));
	item._iMiscId = OilMagic[t];
	item._ivalue = OilValues[t];
	item._iIvalue = OilValues[t];
}

void GetItemBonus(Item &item, int minlvl, int maxlvl, bool onlygood, bool allowspells)
{
	if (minlvl > 25)
		minlvl = 25;

	switch (item._itype) {
	case ItemType::Sword:
	case ItemType::Axe:
	case ItemType::Mace:
		GetItemPower(item, minlvl, maxlvl, PLT_WEAP, onlygood);
		break;
	case ItemType::Bow:
		GetItemPower(item, minlvl, maxlvl, PLT_BOW, onlygood);
		break;
	case ItemType::Shield:
		GetItemPower(item, minlvl, maxlvl, PLT_SHLD, onlygood);
		break;
	case ItemType::LightArmor:
	case ItemType::Helm:
	case ItemType::MediumArmor:
	case ItemType::HeavyArmor:
		GetItemPower(item, minlvl, maxlvl, PLT_ARMO, onlygood);
		break;
	case ItemType::Staff:
		if (allowspells)
			GetStaffSpell(item, maxlvl, onlygood);
		else
			GetItemPower(item, minlvl, maxlvl, PLT_STAFF, onlygood);
		break;
	case ItemType::Ring:
	case ItemType::Amulet:
		GetItemPower(item, minlvl, maxlvl, PLT_MISC, onlygood);
		break;
	case ItemType::None:
	case ItemType::Misc:
	case ItemType::Gold:
		break;
	}
}

int RndUItem(Monster *monster)
{
	if (monster != nullptr && (monster->MData->mTreasure & T_UNIQ) != 0 && !gbIsMultiplayer)
		return -((monster->MData->mTreasure & T_MASK) + 1);

	int ril[512];

	int curlv = ItemsGetCurrlevel();
	int ri = 0;
	for (int i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (!IsItemAvailable(i))
			continue;

		bool okflag = true;
		if (AllItemsList[i].iRnd == IDROP_NEVER)
			okflag = false;
		if (monster != nullptr) {
			if (monster->mLevel < AllItemsList[i].iMinMLvl)
				okflag = false;
		} else {
			if (2 * curlv < AllItemsList[i].iMinMLvl)
				okflag = false;
		}
		if (AllItemsList[i].itype == ItemType::Misc)
			okflag = false;
		if (AllItemsList[i].itype == ItemType::Gold)
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
	if (GenerateRnd(100) > 25)
		return 0;

	int ril[512];

	int curlv = ItemsGetCurrlevel();
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

int RndTypeItems(ItemType itemType, int imid, int lvl)
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
		if (AllItemsList[i].itype != itemType)
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

_unique_items CheckUnique(Item &item, int lvl, int uper, bool recreate)
{
	std::bitset<128> uok = {};

	if (GenerateRnd(100) > uper)
		return UITEM_INVALID;

	int numu = 0;
	for (int j = 0; UniqueItems[j].UIItemId != UITYPE_INVALID; j++) {
		if (!IsUniqueAvailable(j))
			break;
		if (UniqueItems[j].UIItemId == AllItemsList[item.IDidx].iItemId
		    && lvl >= UniqueItems[j].UIMinLvl
		    && (recreate || !UniqueItemFlags[j] || gbIsMultiplayer)) {
			uok[j] = true;
			numu++;
		}
	}

	if (numu == 0)
		return UITEM_INVALID;

	AdvanceRndSeed();
	uint8_t itemData = 0;
	while (numu > 0) {
		if (uok[itemData])
			numu--;
		if (numu > 0)
			itemData = (itemData + 1) % 128;
	}

	return (_unique_items)itemData;
}

void GetUniqueItem(Item &item, _unique_items uid)
{
	UniqueItemFlags[uid] = true;

	for (const auto &power : UniqueItems[uid].powers) {
		if (power.type == IPL_INVALID)
			break;
		SaveItemPower(item, power);
	}

	strcpy(item._iIName, _(UniqueItems[uid].UIName));
	item._iIvalue = UniqueItems[uid].UIValue;

	if (item._iMiscId == IMISC_UNIQUE)
		item._iSeed = uid;

	item._iUid = uid;
	item._iMagical = ITEM_QUALITY_UNIQUE;
	item._iCreateInfo |= CF_UNIQUE;
}

void ItemRndDur(Item &item)
{
	if (item._iDurability > 0 && item._iDurability != DUR_INDESTRUCTIBLE)
		item._iDurability = GenerateRnd(item._iMaxDur / 2) + (item._iMaxDur / 4) + 1;
}

void SetupAllItems(Item &item, int idx, int iseed, int lvl, int uper, bool onlygood, bool recreate, bool pregen)
{
	item._iSeed = iseed;
	SetRndSeed(iseed);
	GetItemAttrs(item, idx, lvl / 2);
	item._iCreateInfo = lvl;

	if (pregen)
		item._iCreateInfo |= CF_PREGEN;
	if (onlygood)
		item._iCreateInfo |= CF_ONLYGOOD;

	if (uper == 15)
		item._iCreateInfo |= CF_UPER15;
	else if (uper == 1)
		item._iCreateInfo |= CF_UPER1;

	if (item._iMiscId != IMISC_UNIQUE) {
		int iblvl = -1;
		if (GenerateRnd(100) <= 10 || GenerateRnd(100) <= lvl) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item._iMiscId == IMISC_STAFF) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item._iMiscId == IMISC_RING) {
			iblvl = lvl;
		}
		if (iblvl == -1 && item._iMiscId == IMISC_AMULET) {
			iblvl = lvl;
		}
		if (onlygood)
			iblvl = lvl;
		if (uper == 15)
			iblvl = lvl + 4;
		if (iblvl != -1) {
			_unique_items uid = CheckUnique(item, iblvl, uper, recreate);
			if (uid == UITEM_INVALID) {
				GetItemBonus(item, iblvl / 2, iblvl, onlygood, true);
			} else {
				GetUniqueItem(item, uid);
			}
		}
		if (item._iMagical != ITEM_QUALITY_UNIQUE)
			ItemRndDur(item);
	} else {
		if (item._iLoc != ILOC_UNEQUIPABLE) {
			GetUniqueItem(item, (_unique_items)iseed); // uid is stored in iseed for uniques
		}
	}
	SetupItem(item);
}

void SetupBaseItem(Point position, int idx, bool onlygood, bool sendmsg, bool delta)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];
	GetSuperItemSpace(position, ii);
	int curlv = ItemsGetCurrlevel();

	SetupAllItems(item, idx, AdvanceRndSeed(), 2 * curlv, 1, onlygood, false, delta);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
	if (delta)
		DeltaAddItem(ii);
}

void SetupAllUseful(Item &item, int iseed, int lvl)
{
	int idx;

	item._iSeed = iseed;
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

	GetItemAttrs(item, idx, lvl);
	item._iCreateInfo = lvl | CF_USEFUL;
	SetupItem(item);
}

uint8_t Char2int(uint8_t input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	return 0;
}

void Hex2bin(const char *src, int bytes, uint8_t *target)
{
	for (int i = 0; i < bytes; i++, src += 2) {
		target[i] = (Char2int(src[0]) << 4) | Char2int(src[1]);
	}
}

void SpawnRock()
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int oi;
	bool ostand = false;
	for (int i = 0; i < ActiveObjectCount && !ostand; i++) {
		oi = ActiveObjects[i];
		ostand = Objects[oi]._otype == OBJ_STAND;
	}

	if (!ostand)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];

	item.position = Objects[oi].position;
	dItem[Objects[oi].position.x][Objects[oi].position.y] = ii + 1;
	int curlv = ItemsGetCurrlevel();
	GetItemAttrs(item, IDI_ROCK, curlv);
	SetupItem(item);
	item._iSelFlag = 2;
	item._iPostDraw = true;
	item.AnimInfo.CurrentFrame = 11;
}

void ItemDoppel()
{
	if (!gbIsMultiplayer)
		return;

	static int idoppely = 16;

	for (int idoppelx = 16; idoppelx < 96; idoppelx++) {
		if (dItem[idoppelx][idoppely] != 0) {
			Item *i = &Items[dItem[idoppelx][idoppely] - 1];
			if (i->position.x != idoppelx || i->position.y != idoppely)
				dItem[idoppelx][idoppely] = 0;
		}
	}

	idoppely++;
	if (idoppely == 96)
		idoppely = 16;
}

void RepairItem(Item &item, int lvl)
{
	if (item._iDurability == item._iMaxDur) {
		return;
	}

	if (item._iMaxDur <= 0) {
		item._itype = ItemType::None;
		return;
	}

	int rep = 0;
	do {
		rep += lvl + GenerateRnd(lvl);
		item._iMaxDur -= std::max(item._iMaxDur / (lvl + 9), 1);
		if (item._iMaxDur == 0) {
			item._itype = ItemType::None;
			return;
		}
	} while (rep + item._iDurability < item._iMaxDur);

	item._iDurability = std::min<int>(item._iDurability + rep, item._iMaxDur);
}

void RechargeItem(Item &item, int r)
{
	if (item._iCharges == item._iMaxCharges)
		return;

	do {
		item._iMaxCharges--;
		if (item._iMaxCharges == 0) {
			return;
		}
		item._iCharges += r;
	} while (item._iCharges < item._iMaxCharges);

	item._iCharges = std::min(item._iCharges, item._iMaxCharges);
}

bool ApplyOilToItem(Item &item, Player &player)
{
	int r;

	if (item._iClass == ICLASS_MISC) {
		return false;
	}
	if (item._iClass == ICLASS_GOLD) {
		return false;
	}
	if (item._iClass == ICLASS_QUEST) {
		return false;
	}

	switch (player._pOilType) {
	case IMISC_OILACC:
	case IMISC_OILMAST:
	case IMISC_OILSHARP:
		if (item._iClass == ICLASS_ARMOR) {
			return false;
		}
		break;
	case IMISC_OILDEATH:
		if (item._iClass == ICLASS_ARMOR) {
			return false;
		}
		if (item._itype == ItemType::Bow) {
			return false;
		}
		break;
	case IMISC_OILHARD:
	case IMISC_OILIMP:
		if (item._iClass == ICLASS_WEAPON) {
			return false;
		}
		break;
	default:
		break;
	}

	switch (player._pOilType) {
	case IMISC_OILACC:
		if (item._iPLToHit < 50) {
			item._iPLToHit += GenerateRnd(2) + 1;
		}
		break;
	case IMISC_OILMAST:
		if (item._iPLToHit < 100) {
			item._iPLToHit += GenerateRnd(3) + 3;
		}
		break;
	case IMISC_OILSHARP:
		if (item._iMaxDam - item._iMinDam < 30) {
			item._iMaxDam = item._iMaxDam + 1;
		}
		break;
	case IMISC_OILDEATH:
		if (item._iMaxDam - item._iMinDam < 30) {
			item._iMinDam = item._iMinDam + 1;
			item._iMaxDam = item._iMaxDam + 2;
		}
		break;
	case IMISC_OILSKILL:
		r = GenerateRnd(6) + 5;
		item._iMinStr = std::max(0, item._iMinStr - r);
		item._iMinMag = std::max(0, item._iMinMag - r);
		item._iMinDex = std::max(0, item._iMinDex - r);
		break;
	case IMISC_OILBSMTH:
		if (item._iMaxDur == 255)
			return true;
		if (item._iDurability < item._iMaxDur) {
			item._iDurability = (item._iMaxDur + 4) / 5 + item._iDurability;
			item._iDurability = std::min<int>(item._iDurability, item._iMaxDur);
		} else {
			if (item._iMaxDur >= 100) {
				return true;
			}
			item._iMaxDur++;
			item._iDurability = item._iMaxDur;
		}
		break;
	case IMISC_OILFORT:
		if (item._iMaxDur != DUR_INDESTRUCTIBLE && item._iMaxDur < 200) {
			r = GenerateRnd(41) + 10;
			item._iMaxDur += r;
			item._iDurability += r;
		}
		break;
	case IMISC_OILPERM:
		item._iDurability = 255;
		item._iMaxDur = 255;
		break;
	case IMISC_OILHARD:
		if (item._iAC < 60) {
			item._iAC += GenerateRnd(2) + 1;
		}
		break;
	case IMISC_OILIMP:
		if (item._iAC < 120) {
			item._iAC += GenerateRnd(3) + 3;
		}
		break;
	default:
		return false;
	}
	return true;
}

void PrintItemOil(char iDidx)
{
	switch (iDidx) {
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

void DrawUniqueInfoWindow(const Surface &out)
{
	CelDrawTo(out, GetPanelPosition(UiPanels::Inventory, { 24 - SPANEL_WIDTH, 327 }), *pSTextBoxCels, 1);
	DrawHalfTransparentRectTo(out, RightPanel.position.x - SPANEL_WIDTH + 27, RightPanel.position.y + 28, 265, 297);
}

void PrintItemMisc(Item &item)
{
	if (item._iMiscId == IMISC_SCROLL) {
		strcpy(tempstr, _("Right-click to read"));
		AddPanelString(tempstr);
	}
	if (item._iMiscId == IMISC_SCROLLT) {
		strcpy(tempstr, _("Right-click to read, then"));
		AddPanelString(tempstr);
		strcpy(tempstr, _("left-click to target"));
		AddPanelString(tempstr);
	}
	if (item._iMiscId >= IMISC_USEFIRST && item._iMiscId <= IMISC_USELAST) {
		PrintItemOil(item._iMiscId);
		strcpy(tempstr, _("Right-click to use"));
		AddPanelString(tempstr);
	}
	if (item._iMiscId > IMISC_OILFIRST && item._iMiscId < IMISC_OILLAST) {
		PrintItemOil(item._iMiscId);
		strcpy(tempstr, _("Right click to use"));
		AddPanelString(tempstr);
	}
	if (item._iMiscId > IMISC_RUNEFIRST && item._iMiscId < IMISC_RUNELAST) {
		PrintItemOil(item._iMiscId);
		strcpy(tempstr, _("Right click to use"));
		AddPanelString(tempstr);
	}
	if (item._iMiscId == IMISC_BOOK) {
		strcpy(tempstr, _("Right-click to read"));
		AddPanelString(tempstr);
	}
	if (item._iMiscId == IMISC_NOTE) {
		strcpy(tempstr, _("Right click to read"));
		AddPanelString(tempstr);
	}
	if (item._iMiscId == IMISC_MAPOFDOOM) {
		strcpy(tempstr, _("Right-click to view"));
		AddPanelString(tempstr);
	}
	if (item._iMiscId == IMISC_EAR) {
		strcpy(tempstr, fmt::format(_("Level: {:d}"), item._ivalue).c_str());
		AddPanelString(tempstr);
	}
	if (item._iMiscId == IMISC_AURIC) {
		strcpy(tempstr, _("Doubles gold capacity"));
		AddPanelString(tempstr);
	}
}

void PrintItemInfo(Item &item)
{
	PrintItemMisc(item);
	uint8_t str = item._iMinStr;
	uint8_t dex = item._iMinDex;
	uint8_t mag = item._iMinMag;
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
}

bool SmithItemOk(int i)
{
	if (AllItemsList[i].itype == ItemType::Misc)
		return false;
	if (AllItemsList[i].itype == ItemType::Gold)
		return false;
	if (AllItemsList[i].itype == ItemType::Staff && (!gbIsHellfire || AllItemsList[i].iSpell != SPL_NULL))
		return false;
	if (AllItemsList[i].itype == ItemType::Ring)
		return false;
	if (AllItemsList[i].itype == ItemType::Amulet)
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

void SortVendor(Item *itemList)
{
	int count = 1;
	while (!itemList[count].isEmpty())
		count++;

	auto cmp = [](const Item &a, const Item &b) {
		return a.IDidx < b.IDidx;
	};

	std::sort(itemList, itemList + count, cmp);
}

bool PremiumItemOk(int i)
{
	if (AllItemsList[i].itype == ItemType::Misc)
		return false;
	if (AllItemsList[i].itype == ItemType::Gold)
		return false;
	if (!gbIsHellfire && AllItemsList[i].itype == ItemType::Staff)
		return false;

	if (gbIsMultiplayer) {
		if (AllItemsList[i].iMiscId == IMISC_OILOF)
			return false;
		if (AllItemsList[i].itype == ItemType::Ring)
			return false;
		if (AllItemsList[i].itype == ItemType::Amulet)
			return false;
	}

	return true;
}

int RndPremiumItem(int minlvl, int maxlvl)
{
	return RndVendorItem<PremiumItemOk>(minlvl, maxlvl);
}

void SpawnOnePremium(int i, int plvl, int playerId)
{
	int itemValue = 0;
	bool keepGoing = false;
	Item tempItem = Items[0];

	auto &player = Players[playerId];

	int strength = std::max(player.GetMaximumAttributeValue(CharacterAttribute::Strength), player._pStrength);
	int dexterity = std::max(player.GetMaximumAttributeValue(CharacterAttribute::Dexterity), player._pDexterity);
	int magic = std::max(player.GetMaximumAttributeValue(CharacterAttribute::Magic), player._pMagic);
	strength += strength / 5;
	dexterity += dexterity / 5;
	magic += magic / 5;

	plvl = clamp(plvl, 1, 30);

	int count = 0;

	do {
		keepGoing = false;
		memset(&Items[0], 0, sizeof(*Items));
		Items[0]._iSeed = AdvanceRndSeed();
		SetRndSeed(Items[0]._iSeed);
		int itemType = RndPremiumItem(plvl / 4, plvl) - 1;
		GetItemAttrs(Items[0], itemType, plvl);
		GetItemBonus(Items[0], plvl / 2, plvl, true, !gbIsHellfire);

		if (!gbIsHellfire) {
			if (Items[0]._iIvalue > 140000) {
				keepGoing = true; // prevent breaking the do/while loop too early by failing hellfire's condition in while
				continue;
			}
			break;
		}

		switch (Items[0]._itype) {
		case ItemType::LightArmor:
		case ItemType::MediumArmor:
		case ItemType::HeavyArmor: {
			const auto *const mostValuablePlayerArmor = player.GetMostValuableItem(
			    [](const Item &item) {
				    return IsAnyOf(item._itype, ItemType::LightArmor, ItemType::MediumArmor, ItemType::HeavyArmor);
			    });

			itemValue = mostValuablePlayerArmor == nullptr ? 0 : mostValuablePlayerArmor->_iIvalue;
			break;
		}
		case ItemType::Shield:
		case ItemType::Axe:
		case ItemType::Bow:
		case ItemType::Mace:
		case ItemType::Sword:
		case ItemType::Helm:
		case ItemType::Staff:
		case ItemType::Ring:
		case ItemType::Amulet: {
			const auto *const mostValuablePlayerItem = player.GetMostValuableItem(
			    [](const Item &item) { return item._itype == Items[0]._itype; });

			itemValue = mostValuablePlayerItem == nullptr ? 0 : mostValuablePlayerItem->_iIvalue;
			break;
		}
		default:
			itemValue = 0;
			break;
		}
		itemValue = itemValue * 4 / 5; // avoids forced int > float > int conversion

		count++;
	} while (keepGoing
	    || ((
	            Items[0]._iIvalue > 200000
	            || Items[0]._iMinStr > strength
	            || Items[0]._iMinMag > magic
	            || Items[0]._iMinDex > dexterity
	            || Items[0]._iIvalue < itemValue)
	        && count < 150));
	premiumitems[i] = Items[0];
	premiumitems[i]._iCreateInfo = plvl | CF_SMITHPREMIUM;
	premiumitems[i]._iIdentified = true;
	premiumitems[i]._iStatFlag = StoreStatOk(premiumitems[i]);
	Items[0] = tempItem;
}

bool WitchItemOk(int i)
{
	if (IsNoneOf(AllItemsList[i].itype, ItemType::Misc, ItemType::Staff))
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

int RndBoyItem(int lvl)
{
	return RndVendorItem<PremiumItemOk>(0, lvl);
}

bool HealerItemOk(int i)
{
	if (AllItemsList[i].itype != ItemType::Misc)
		return false;

	if (AllItemsList[i].iMiscId == IMISC_SCROLL)
		return AllItemsList[i].iSpell == SPL_HEAL;
	if (AllItemsList[i].iMiscId == IMISC_SCROLLT)
		return AllItemsList[i].iSpell == SPL_HEALOTHER && gbIsMultiplayer;

	if (!gbIsMultiplayer) {
		auto &myPlayer = Players[MyPlayerId];

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

void RecreateSmithItem(Item &item, int lvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndSmithItem(lvl) - 1;
	GetItemAttrs(item, itype, lvl);

	item._iSeed = iseed;
	item._iCreateInfo = lvl | CF_SMITH;
	item._iIdentified = true;
}

void RecreatePremiumItem(Item &item, int plvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndPremiumItem(plvl / 4, plvl) - 1;
	GetItemAttrs(item, itype, plvl);
	GetItemBonus(item, plvl / 2, plvl, true, !gbIsHellfire);

	item._iSeed = iseed;
	item._iCreateInfo = plvl | CF_SMITHPREMIUM;
	item._iIdentified = true;
}

void RecreateBoyItem(Item &item, int lvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndBoyItem(lvl) - 1;
	GetItemAttrs(item, itype, lvl);
	GetItemBonus(item, lvl, 2 * lvl, true, true);

	item._iSeed = iseed;
	item._iCreateInfo = lvl | CF_BOY;
	item._iIdentified = true;
}

void RecreateWitchItem(Item &item, int idx, int lvl, int iseed)
{
	if (IsAnyOf(idx, IDI_MANA, IDI_FULLMANA, IDI_PORTAL)) {
		GetItemAttrs(item, idx, lvl);
	} else if (gbIsHellfire && idx >= 114 && idx <= 117) {
		SetRndSeed(iseed);
		AdvanceRndSeed();
		GetItemAttrs(item, idx, lvl);
	} else {
		SetRndSeed(iseed);
		int itype = RndWitchItem(lvl) - 1;
		GetItemAttrs(item, itype, lvl);
		int iblvl = -1;
		if (GenerateRnd(100) <= 5)
			iblvl = 2 * lvl;
		if (iblvl == -1 && item._iMiscId == IMISC_STAFF)
			iblvl = 2 * lvl;
		if (iblvl != -1)
			GetItemBonus(item, iblvl / 2, iblvl, true, true);
	}

	item._iSeed = iseed;
	item._iCreateInfo = lvl | CF_WITCH;
	item._iIdentified = true;
}

void RecreateHealerItem(Item &item, int idx, int lvl, int iseed)
{
	if (IsAnyOf(idx, IDI_HEAL, IDI_FULLHEAL, IDI_RESURRECT)) {
		GetItemAttrs(item, idx, lvl);
	} else {
		SetRndSeed(iseed);
		int itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(item, itype, lvl);
	}

	item._iSeed = iseed;
	item._iCreateInfo = lvl | CF_HEALER;
	item._iIdentified = true;
}

void RecreateTownItem(Item &item, int idx, uint16_t icreateinfo, int iseed)
{
	if ((icreateinfo & CF_SMITH) != 0)
		RecreateSmithItem(item, icreateinfo & CF_LEVEL, iseed);
	else if ((icreateinfo & CF_SMITHPREMIUM) != 0)
		RecreatePremiumItem(item, icreateinfo & CF_LEVEL, iseed);
	else if ((icreateinfo & CF_BOY) != 0)
		RecreateBoyItem(item, icreateinfo & CF_LEVEL, iseed);
	else if ((icreateinfo & CF_WITCH) != 0)
		RecreateWitchItem(item, idx, icreateinfo & CF_LEVEL, iseed);
	else if ((icreateinfo & CF_HEALER) != 0)
		RecreateHealerItem(item, idx, icreateinfo & CF_LEVEL, iseed);
}

void RecalcStoreStats()
{
	for (auto &item : smithitem) {
		if (!item.isEmpty()) {
			item._iStatFlag = StoreStatOk(item);
		}
	}
	for (auto &item : premiumitems) {
		if (!item.isEmpty()) {
			item._iStatFlag = StoreStatOk(item);
		}
	}
	for (int i = 0; i < 20; i++) {
		if (!witchitem[i].isEmpty()) {
			witchitem[i]._iStatFlag = StoreStatOk(witchitem[i]);
		}
	}
	for (auto &item : healitem) {
		if (!item.isEmpty()) {
			item._iStatFlag = StoreStatOk(item);
		}
	}
	boyitem._iStatFlag = StoreStatOk(boyitem);
}

void CreateMagicItem(Point position, int lvl, ItemType itemType, int imid, int icurs, bool sendmsg, bool delta)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];
	int idx = RndTypeItems(itemType, imid, lvl);

	while (true) {
		memset(&item, 0, sizeof(item));
		SetupAllItems(item, idx, AdvanceRndSeed(), 2 * lvl, 1, true, false, delta);
		if (item._iCurs == icurs)
			break;

		idx = RndTypeItems(itemType, imid, lvl);
	}
	GetSuperItemSpace(position, ii);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
	if (delta)
		DeltaAddItem(ii);
}

void NextItemRecord(int i)
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

} // namespace

bool IsItemAvailable(int i)
{
	if (i < 0 || i > IDI_LAST)
		return false;

	if (gbIsSpawn) {
		if (i >= 62 && i <= 71)
			return false; // Medium and heavy armors
		if (IsAnyOf(i, 105, 107, 108, 110, 111, 113))
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
	        sgOptions.Gameplay.bTestBard && IsAnyOf(i, IDI_BARDSWORD, IDI_BARDDAGGER));
}

BYTE GetOutlineColor(const Item &item, bool checkReq)
{
	if (checkReq && !item._iStatFlag)
		return ICOL_RED;
	if (item._itype == ItemType::Gold)
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

void InitItems()
{
	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(Items[0], IDI_GOLD, 1);
	golditem = Items[0];
	golditem._iStatFlag = true;
	ActiveItemCount = 0;

	for (int i = 0; i < MAXITEMS; i++) {
		auto &item = Items[i];
		item._itype = ItemType::None;
		item.position = { 0, 0 };
		item._iAnimFlag = false;
		item._iSelFlag = 0;
		item._iIdentified = false;
		item._iPostDraw = false;
	}

	for (int i = 0; i < MAXITEMS; i++) {
		AvailableItems[i] = i;
		ActiveItems[i] = 0;
	}

	if (!setlevel) {
		AdvanceRndSeed(); /* unused */
		if (Quests[Q_ROCK].IsAvailable())
			SpawnRock();
		if (Quests[Q_ANVIL].IsAvailable())
			SpawnQuestItem(IDI_ANVIL, { 2 * setpc_x + 27, 2 * setpc_y + 27 }, 0, 1);
		if (sgGameInitInfo.bCowQuest != 0 && currlevel == 20)
			SpawnQuestItem(IDI_BROWNSUIT, { 25, 25 }, 3, 1);
		if (sgGameInitInfo.bCowQuest != 0 && currlevel == 19)
			SpawnQuestItem(IDI_GREYSUIT, { 25, 25 }, 3, 1);
		if (currlevel > 0 && currlevel < 16)
			AddInitItems();
		if (currlevel >= 21 && currlevel <= 23)
			SpawnNote();
	}

	ShowUniqueItemInfoBox = false;

	// BUGFIX: item get records not reset when resetting items (fixed).
	initItemGetRecords();
}

void CalcPlrItemVals(Player &player, bool loadgfx)
{
	int mind = 0; // min damage
	int maxd = 0; // max damage
	int tac = 0;  // accuracy

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

	for (auto &item : player.InvBody) {
		if (!item.isEmpty() && item._iStatFlag) {

			mind += item._iMinDam;
			maxd += item._iMaxDam;
			tac += item._iAC;

			if (item._iSpell != SPL_NULL) {
				spl |= GetSpellBitmask(item._iSpell);
			}

			if (item._iMagical == ITEM_QUALITY_NORMAL || item._iIdentified) {
				bdam += item._iPLDam;
				btohit += item._iPLToHit;
				if (item._iPLAC != 0) {
					int tmpac = item._iAC;
					tmpac *= item._iPLAC;
					tmpac /= 100;
					if (tmpac == 0)
						tmpac = math::Sign(item._iPLAC);
					bac += tmpac;
				}
				iflgs |= item._iFlags;
				pDamAcFlags |= item._iDamAcFlags;
				sadd += item._iPLStr;
				madd += item._iPLMag;
				dadd += item._iPLDex;
				vadd += item._iPLVit;
				fr += item._iPLFR;
				lr += item._iPLLR;
				mr += item._iPLMR;
				dmod += item._iPLDamMod;
				ghit += item._iPLGetHit;
				lrad += item._iPLLight;
				ihp += item._iPLHP;
				imana += item._iPLMana;
				spllvladd += item._iSplLvlAdd;
				enac += item._iPLEnAc;
				fmin += item._iFMinDam;
				fmax += item._iFMaxDam;
				lmin += item._iLMinDam;
				lmax += item._iLMaxDam;
			}
		}
	}

	if (mind == 0 && maxd == 0) {
		mind = 1;
		maxd = 1;

		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Shield && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			maxd = 3;
		}

		if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Shield && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
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

	if (player._pLightRad != lrad && &player == &Players[MyPlayerId]) {
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
		if (player.InvBody[INVLOC_HAND_LEFT]._itype != ItemType::Staff) {
			if (player.InvBody[INVLOC_HAND_RIGHT]._itype != ItemType::Staff && (!player.InvBody[INVLOC_HAND_LEFT].isEmpty() || !player.InvBody[INVLOC_HAND_RIGHT].isEmpty())) {
				player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 300;
			} else {
				player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 150;
			}
		} else {
			player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 150;
		}
	} else if (player._pClass == HeroClass::Bard) {
		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Sword || player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Sword)
			player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 150;
		else if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow || player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Bow) {
			player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 250;
		} else {
			player._pDamageMod = player._pLevel * player._pStrength / 100;
		}
	} else if (player._pClass == HeroClass::Barbarian) {

		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Axe || player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Axe) {
			player._pDamageMod = player._pLevel * player._pStrength / 75;
		} else if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Mace || player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Mace) {
			player._pDamageMod = player._pLevel * player._pStrength / 75;
		} else if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Bow || player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Bow) {
			player._pDamageMod = player._pLevel * player._pStrength / 300;
		} else {
			player._pDamageMod = player._pLevel * player._pStrength / 100;
		}

		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Shield || player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Shield) {
			if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Shield)
				player._pIAC -= player.InvBody[INVLOC_HAND_LEFT]._iAC / 2;
			else if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Shield)
				player._pIAC -= player.InvBody[INVLOC_HAND_RIGHT]._iAC / 2;
		} else if (IsNoneOf(player.InvBody[INVLOC_HAND_LEFT]._itype, ItemType::Staff, ItemType::Bow) && IsNoneOf(player.InvBody[INVLOC_HAND_RIGHT]._itype, ItemType::Staff, ItemType::Bow)) {
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
	} else if (IsAnyOf(player._pClass, HeroClass::Rogue, HeroClass::Monk, HeroClass::Bard)) {
		vadd += vadd / 2;
	}
	ihp += (vadd << 6); // BUGFIX: blood boil can cause negative shifts here (see line 757)

	if (player._pClass == HeroClass::Sorcerer) {
		madd *= 2;
	}
	if (IsAnyOf(player._pClass, HeroClass::Rogue, HeroClass::Monk)) {
		madd += madd / 2;
	} else if (player._pClass == HeroClass::Bard) {
		madd += (madd / 4) + (madd / 2);
	}
	imana += (madd << 6);

	player._pMaxHP = ihp + player._pMaxHPBase;
	player._pHitPoints = std::min(ihp + player._pHPBase, player._pMaxHP);

	if (&player == &Players[MyPlayerId] && (player._pHitPoints >> 6) <= 0) {
		SetPlayerHitPoints(player, 0);
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
		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Staff && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			player._pBlockFlag = true;
			player._pIFlags |= ISPL_FASTBLOCK;
		}
		if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Staff && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
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

	ItemType weaponItemType = ItemType::None;
	bool holdsShield = false;
	if (!player.InvBody[INVLOC_HAND_LEFT].isEmpty()
	    && player.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON
	    && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		weaponItemType = player.InvBody[INVLOC_HAND_LEFT]._itype;
	}

	if (!player.InvBody[INVLOC_HAND_RIGHT].isEmpty()
	    && player.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON
	    && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		weaponItemType = player.InvBody[INVLOC_HAND_RIGHT]._itype;
	}

	if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Shield && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		player._pBlockFlag = true;
		holdsShield = true;
	}
	if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Shield && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		player._pBlockFlag = true;
		holdsShield = true;
	}

	PlayerWeaponGraphic animWeaponId = holdsShield ? PlayerWeaponGraphic::UnarmedShield : PlayerWeaponGraphic::Unarmed;
	switch (weaponItemType) {
	case ItemType::Sword:
		animWeaponId = holdsShield ? PlayerWeaponGraphic::SwordShield : PlayerWeaponGraphic::Sword;
		break;
	case ItemType::Axe:
		animWeaponId = PlayerWeaponGraphic::Axe;
		break;
	case ItemType::Bow:
		animWeaponId = PlayerWeaponGraphic::Bow;
		break;
	case ItemType::Mace:
		animWeaponId = holdsShield ? PlayerWeaponGraphic::MaceShield : PlayerWeaponGraphic::Mace;
		break;
	case ItemType::Staff:
		animWeaponId = PlayerWeaponGraphic::Staff;
		break;
	default:
		break;
	}

	PlayerArmorGraphic animArmorId = PlayerArmorGraphic::Light;
	if (player.InvBody[INVLOC_CHEST]._itype == ItemType::HeavyArmor && player.InvBody[INVLOC_CHEST]._iStatFlag) {
		if (player._pClass == HeroClass::Monk && player.InvBody[INVLOC_CHEST]._iMagical == ITEM_QUALITY_UNIQUE)
			player._pIAC += player._pLevel / 2;
		animArmorId = PlayerArmorGraphic::Heavy;
	} else if (player.InvBody[INVLOC_CHEST]._itype == ItemType::MediumArmor && player.InvBody[INVLOC_CHEST]._iStatFlag) {
		if (player._pClass == HeroClass::Monk) {
			if (player.InvBody[INVLOC_CHEST]._iMagical == ITEM_QUALITY_UNIQUE)
				player._pIAC += player._pLevel * 2;
			else
				player._pIAC += player._pLevel / 2;
		}
		animArmorId = PlayerArmorGraphic::Medium;
	} else if (player._pClass == HeroClass::Monk) {
		player._pIAC += player._pLevel * 2;
	}

	int gfxNum = static_cast<int>(animWeaponId) | static_cast<int>(animArmorId);
	if (player._pgfxnum != gfxNum && loadgfx) {
		player._pgfxnum = gfxNum;
		ResetPlayerGFX(player);
		SetPlrAnims(player);
		if (player._pmode == PM_STAND) {
			LoadPlrGFX(player, player_graphic::Stand);
			player.AnimInfo.ChangeAnimationData(&*player.AnimationData[static_cast<size_t>(player_graphic::Stand)].GetCelSpritesForDirection(player._pdir), player._pNFrames, 4);
		} else {
			LoadPlrGFX(player, player_graphic::Walk);
			player.AnimInfo.ChangeAnimationData(&*player.AnimationData[static_cast<size_t>(player_graphic::Walk)].GetCelSpritesForDirection(player._pdir), player._pWFrames, 1);
		}
	} else {
		player._pgfxnum = gfxNum;
	}

	if (player.InvBody[INVLOC_AMULET].isEmpty() || player.InvBody[INVLOC_AMULET].IDidx != IDI_AURIC) {
		int half = MaxGold;
		MaxGold = GOLD_MAX_LIMIT;

		if (half != MaxGold)
			StripTopGold(player);
	} else {
		MaxGold = GOLD_MAX_LIMIT * 2;
	}

	drawmanaflag = true;
	drawhpflag = true;
}

void CalcPlrInv(Player &player, bool loadgfx)
{
	CalcPlrItemMin(player);
	CalcSelfItems(player);
	CalcPlrItemVals(player, loadgfx);
	CalcPlrItemMin(player);
	if (&player == &Players[MyPlayerId]) {
		CalcPlrBookVals(player);
		player.CalcScrolls();
		CalcPlrStaff(player);
		if (currlevel == 0)
			RecalcStoreStats();
	}
}

void SetPlrHandItem(Item &item, int itemData)
{
	auto &pAllItem = AllItemsList[itemData];

	// zero-initialize struct
	memset(&item, 0, sizeof(item));

	item._itype = pAllItem.itype;
	item._iCurs = pAllItem.iCurs;
	strcpy(item._iName, _(pAllItem.iName));
	strcpy(item._iIName, _(pAllItem.iName));
	item._iLoc = pAllItem.iLoc;
	item._iClass = pAllItem.iClass;
	item._iMinDam = pAllItem.iMinDam;
	item._iMaxDam = pAllItem.iMaxDam;
	item._iAC = pAllItem.iMinAC;
	item._iMiscId = pAllItem.iMiscId;
	item._iSpell = pAllItem.iSpell;

	if (pAllItem.iMiscId == IMISC_STAFF) {
		item._iCharges = gbIsHellfire ? 18 : 40;
	}

	item._iMaxCharges = item._iCharges;
	item._iDurability = pAllItem.iDurability;
	item._iMaxDur = pAllItem.iDurability;
	item._iMinStr = pAllItem.iMinStr;
	item._iMinMag = pAllItem.iMinMag;
	item._iMinDex = pAllItem.iMinDex;
	item._ivalue = pAllItem.iValue;
	item._iIvalue = pAllItem.iValue;
	item._iPrePower = IPL_INVALID;
	item._iSufPower = IPL_INVALID;
	item._iMagical = ITEM_QUALITY_NORMAL;
	item.IDidx = static_cast<_item_indexes>(itemData);
	if (gbIsHellfire)
		item.dwBuff |= CF_HELLFIRE;
}

void GetPlrHandSeed(Item *h)
{
	h->_iSeed = AdvanceRndSeed();
}

void SetGoldSeed(Player &player, Item &gold)
{
	int s = 0;

	bool doneflag;
	do {
		doneflag = true;
		s = AdvanceRndSeed();
		for (int i = 0; i < ActiveItemCount; i++) {
			int ii = ActiveItems[i];
			auto &item = Items[ii];
			if (item._iSeed == s)
				doneflag = false;
		}
		if (&player == &Players[MyPlayerId]) {
			for (int i = 0; i < player._pNumInv; i++) {
				if (player.InvList[i]._iSeed == s)
					doneflag = false;
			}
		}
	} while (!doneflag);

	gold._iSeed = s;
}

int GetGoldCursor(int value)
{
	if (value >= GOLD_MEDIUM_LIMIT)
		return ICURS_GOLD_LARGE;

	if (value <= GOLD_SMALL_LIMIT)
		return ICURS_GOLD_SMALL;

	return ICURS_GOLD_MEDIUM;
}

void SetPlrHandGoldCurs(Item &gold)
{
	gold._iCurs = GetGoldCursor(gold._ivalue);
}

void CreatePlrItems(int playerId)
{
	auto &player = Players[playerId];

	for (auto &item : player.InvBody) {
		item._itype = ItemType::None;
	}

	// converting this to a for loop creates a `rep stosd` instruction,
	// so this probably actually was a memset
	memset(&player.InvGrid, 0, sizeof(player.InvGrid));

	for (auto &item : player.InvList) {
		item._itype = ItemType::None;
	}

	player._pNumInv = 0;

	for (auto &item : player.SpdList) {
		item._itype = ItemType::None;
	}

	switch (player._pClass) {
	case HeroClass::Warrior:
		SetPlrHandItem(player.InvBody[INVLOC_HAND_LEFT], IDI_WARRIOR);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(player.InvBody[INVLOC_HAND_RIGHT], IDI_WARRSHLD);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_RIGHT]);

		SetPlrHandItem(player.HoldItem, IDI_WARRCLUB);
		GetPlrHandSeed(&player.HoldItem);
		AutoPlaceItemInInventory(player, player.HoldItem, true);

		SetPlrHandItem(player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	case HeroClass::Rogue:
		SetPlrHandItem(player.InvBody[INVLOC_HAND_LEFT], IDI_ROGUE);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	case HeroClass::Sorcerer:
		SetPlrHandItem(player.InvBody[INVLOC_HAND_LEFT], gbIsHellfire ? IDI_SORCERER : 166);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(player.SpdList[0], gbIsHellfire ? IDI_HEAL : IDI_MANA);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(player.SpdList[1], gbIsHellfire ? IDI_HEAL : IDI_MANA);
		GetPlrHandSeed(&player.SpdList[1]);
		break;

	case HeroClass::Monk:
		SetPlrHandItem(player.InvBody[INVLOC_HAND_LEFT], IDI_SHORTSTAFF);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);
		SetPlrHandItem(player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	case HeroClass::Bard:
		SetPlrHandItem(player.InvBody[INVLOC_HAND_LEFT], IDI_BARDSWORD);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(player.InvBody[INVLOC_HAND_RIGHT], IDI_BARDDAGGER);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_RIGHT]);
		SetPlrHandItem(player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	case HeroClass::Barbarian:
		SetPlrHandItem(player.InvBody[INVLOC_HAND_LEFT], 139); // TODO: add more enums to items
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_LEFT]);

		SetPlrHandItem(player.InvBody[INVLOC_HAND_RIGHT], IDI_WARRSHLD);
		GetPlrHandSeed(&player.InvBody[INVLOC_HAND_RIGHT]);
		SetPlrHandItem(player.SpdList[0], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[0]);

		SetPlrHandItem(player.SpdList[1], IDI_HEAL);
		GetPlrHandSeed(&player.SpdList[1]);
		break;
	}

	SetPlrHandItem(player.HoldItem, IDI_GOLD);
	GetPlrHandSeed(&player.HoldItem);

	player.HoldItem._ivalue = 100;
	player.HoldItem._iCurs = ICURS_GOLD_SMALL;
	player._pGold = player.HoldItem._ivalue;
	player.InvList[player._pNumInv++] = player.HoldItem;
	player.InvGrid[30] = player._pNumInv;

	CalcPlrItemVals(player, false);
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
		if (Objects[oi]._oSolidFlag)
			return false;
	}

	if (dObject[position.x + 1][position.y + 1] > 0 && Objects[dObject[position.x + 1][position.y + 1] - 1]._oSelFlag != 0)
		return false;

	if (dObject[position.x + 1][position.y + 1] < 0 && Objects[-(dObject[position.x + 1][position.y + 1] + 1)]._oSelFlag != 0)
		return false;

	if (dObject[position.x + 1][position.y] > 0
	    && dObject[position.x][position.y + 1] > 0
	    && Objects[dObject[position.x + 1][position.y] - 1]._oSelFlag != 0
	    && Objects[dObject[position.x][position.y + 1] - 1]._oSelFlag != 0) {
		return false;
	}

	return IsTileNotSolid(position);
}

int AllocateItem()
{
	int inum = AvailableItems[0];
	AvailableItems[0] = AvailableItems[MAXITEMS - ActiveItemCount - 1];
	ActiveItems[ActiveItemCount] = inum;
	ActiveItemCount++;

	memset(&Items[inum], 0, sizeof(*Items));

	return inum;
}

Point GetSuperItemLoc(Point position)
{
	for (int k = 1; k < 50; k++) {
		for (int j = -k; j <= k; j++) {
			for (int i = -k; i <= k; i++) {
				Displacement offset = { i, j };
				Point positionToCheck = position + offset;
				if (ItemSpaceOk(positionToCheck)) {
					return positionToCheck;
				}
			}
		}
	}

	return { 0, 0 }; // TODO handle no space for dropping items
}

void GetItemAttrs(Item &item, int itemData, int lvl)
{
	item._itype = AllItemsList[itemData].itype;
	item._iCurs = AllItemsList[itemData].iCurs;
	strcpy(item._iName, _(AllItemsList[itemData].iName));
	strcpy(item._iIName, _(AllItemsList[itemData].iName));
	item._iLoc = AllItemsList[itemData].iLoc;
	item._iClass = AllItemsList[itemData].iClass;
	item._iMinDam = AllItemsList[itemData].iMinDam;
	item._iMaxDam = AllItemsList[itemData].iMaxDam;
	item._iAC = AllItemsList[itemData].iMinAC + GenerateRnd(AllItemsList[itemData].iMaxAC - AllItemsList[itemData].iMinAC + 1);
	item._iFlags = AllItemsList[itemData].iFlags;
	item._iMiscId = AllItemsList[itemData].iMiscId;
	item._iSpell = AllItemsList[itemData].iSpell;
	item._iMagical = ITEM_QUALITY_NORMAL;
	item._ivalue = AllItemsList[itemData].iValue;
	item._iIvalue = AllItemsList[itemData].iValue;
	item._iDurability = AllItemsList[itemData].iDurability;
	item._iMaxDur = AllItemsList[itemData].iDurability;
	item._iMinStr = AllItemsList[itemData].iMinStr;
	item._iMinMag = AllItemsList[itemData].iMinMag;
	item._iMinDex = AllItemsList[itemData].iMinDex;
	item.IDidx = static_cast<_item_indexes>(itemData);
	if (gbIsHellfire)
		item.dwBuff |= CF_HELLFIRE;
	item._iPrePower = IPL_INVALID;
	item._iSufPower = IPL_INVALID;

	if (item._iMiscId == IMISC_BOOK)
		GetBookSpell(item, lvl);

	if (gbIsHellfire && item._iMiscId == IMISC_OILOF)
		GetOilType(item, lvl);

	if (item._itype != ItemType::Gold)
		return;

	int rndv;
	int itemlevel = ItemsGetCurrlevel();
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

	item._ivalue = std::min(rndv, GOLD_MAX_LIMIT);
	SetPlrHandGoldCurs(item);
}

void SetupItem(Item &item)
{
	item.SetNewAnimation(Players[MyPlayerId].pLvlLoad == 0);
	item._iIdentified = false;
}

int RndItem(const Monster &monster)
{
	if ((monster.MData->mTreasure & T_UNIQ) != 0)
		return -((monster.MData->mTreasure & T_MASK) + 1);

	if ((monster.MData->mTreasure & T_NODROP) != 0)
		return 0;

	if (GenerateRnd(100) > 40)
		return 0;

	if (GenerateRnd(100) > 25)
		return IDI_GOLD + 1;

	int ril[512];

	int ri = 0;
	for (int i = 0; AllItemsList[i].iLoc != ILOC_INVALID; i++) {
		if (!IsItemAvailable(i))
			continue;

		if (AllItemsList[i].iRnd == IDROP_DOUBLE && monster.mLevel >= AllItemsList[i].iMinMLvl
		    && ri < 512) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iRnd != IDROP_NEVER && monster.mLevel >= AllItemsList[i].iMinMLvl
		    && ri < 512) {
			ril[ri] = i;
			ri++;
		}
		if (AllItemsList[i].iSpell == SPL_RESURRECT && !gbIsMultiplayer)
			ri--;
		if (AllItemsList[i].iSpell == SPL_HEALOTHER && !gbIsMultiplayer)
			ri--;
	}

	int r = GenerateRnd(ri);
	return ril[r] + 1;
}

void SpawnUnique(_unique_items uid, Point position)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];
	GetSuperItemSpace(position, ii);
	int curlv = ItemsGetCurrlevel();

	int idx = 0;
	while (AllItemsList[idx].iItemId != UniqueItems[uid].UIItemId)
		idx++;

	GetItemAttrs(item, idx, curlv);
	GetUniqueItem(item, uid);
	SetupItem(item);
}

void SpawnItem(Monster &monster, Point position, bool sendmsg)
{
	int idx;
	bool onlygood = true;

	if (monster._uniqtype != 0 || ((monster.MData->mTreasure & T_UNIQ) != 0 && gbIsMultiplayer)) {
		idx = RndUItem(&monster);
		if (idx < 0) {
			SpawnUnique((_unique_items) - (idx + 1), position);
			return;
		}
		onlygood = true;
	} else if (Quests[Q_MUSHROOM]._qactive != QUEST_ACTIVE || Quests[Q_MUSHROOM]._qvar1 != QS_MUSHGIVEN) {
		idx = RndItem(monster);
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
		Quests[Q_MUSHROOM]._qvar1 = QS_BRAINSPAWNED;
	}

	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];
	GetSuperItemSpace(position, ii);
	int uper = monster._uniqtype != 0 ? 15 : 1;

	int8_t mLevel = monster.mLevel;
	if (!gbIsHellfire && monster.MType->mtype == MT_DIABLO)
		mLevel -= 15;
	if (mLevel > CF_LEVEL)
		mLevel = CF_LEVEL;

	SetupAllItems(item, idx, AdvanceRndSeed(), mLevel, uper, onlygood, false, false);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
}

void CreateRndItem(Point position, bool onlygood, bool sendmsg, bool delta)
{
	int idx = onlygood ? RndUItem(nullptr) : RndAllItems();

	SetupBaseItem(position, idx, onlygood, sendmsg, delta);
}

void CreateRndUseful(Point position, bool sendmsg)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];
	GetSuperItemSpace(position, ii);
	int curlv = ItemsGetCurrlevel();

	SetupAllUseful(item, AdvanceRndSeed(), curlv);
	if (sendmsg)
		NetSendCmdDItem(false, ii);
}

void CreateTypeItem(Point position, bool onlygood, ItemType itemType, int imisc, bool sendmsg, bool delta)
{
	int idx;

	int curlv = ItemsGetCurrlevel();
	if (itemType != ItemType::Gold)
		idx = RndTypeItems(itemType, imisc, curlv);
	else
		idx = IDI_GOLD;

	SetupBaseItem(position, idx, onlygood, sendmsg, delta);
}

void RecreateItem(Item &item, int idx, uint16_t icreateinfo, int iseed, int ivalue, bool isHellfire)
{
	bool tmpIsHellfire = gbIsHellfire;
	gbIsHellfire = isHellfire;

	if (idx == IDI_GOLD) {
		SetPlrHandItem(item, IDI_GOLD);
		item._iSeed = iseed;
		item._iCreateInfo = icreateinfo;
		item._ivalue = ivalue;
		SetPlrHandGoldCurs(item);
		gbIsHellfire = tmpIsHellfire;
		return;
	}

	if (icreateinfo == 0) {
		SetPlrHandItem(item, idx);
		SetPlrHandSeed(item, iseed);
		gbIsHellfire = tmpIsHellfire;
		return;
	}

	if ((icreateinfo & CF_UNIQUE) == 0) {
		if ((icreateinfo & CF_TOWN) != 0) {
			RecreateTownItem(item, idx, icreateinfo, iseed);
			gbIsHellfire = tmpIsHellfire;
			return;
		}

		if ((icreateinfo & CF_USEFUL) == CF_USEFUL) {
			SetupAllUseful(item, iseed, icreateinfo & CF_LEVEL);
			gbIsHellfire = tmpIsHellfire;
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

	SetupAllItems(item, idx, iseed, level, uper, onlygood, recreate, pregen);
	gbIsHellfire = tmpIsHellfire;
}

void RecreateEar(Item &item, uint16_t ic, int iseed, int id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff)
{
	SetPlrHandItem(item, IDI_EAR);
	tempstr[0] = static_cast<char>((ic >> 8) & 0x7F);
	tempstr[1] = static_cast<char>(ic & 0x7F);
	tempstr[2] = static_cast<char>((iseed >> 24) & 0x7F);
	tempstr[3] = static_cast<char>((iseed >> 16) & 0x7F);
	tempstr[4] = static_cast<char>((iseed >> 8) & 0x7F);
	tempstr[5] = static_cast<char>(iseed & 0x7F);
	tempstr[6] = static_cast<char>(id & 0x7F);
	tempstr[7] = static_cast<char>(dur & 0x7F);
	tempstr[8] = static_cast<char>(mdur & 0x7F);
	tempstr[9] = static_cast<char>(ch & 0x7F);
	tempstr[10] = static_cast<char>(mch & 0x7F);
	tempstr[11] = static_cast<char>((ivalue >> 8) & 0x7F);
	tempstr[12] = static_cast<char>((ibuff >> 24) & 0x7F);
	tempstr[13] = static_cast<char>((ibuff >> 16) & 0x7F);
	tempstr[14] = static_cast<char>((ibuff >> 8) & 0x7F);
	tempstr[15] = static_cast<char>(ibuff & 0x7F);
	tempstr[16] = '\0';
	strcpy(item._iName, fmt::format(_(/* TRANSLATORS: {:s} will be a Character Name */ "Ear of {:s}"), tempstr).c_str());
	item._iCurs = ((ivalue >> 6) & 3) + ICURS_EAR_SORCERER;
	item._ivalue = ivalue & 0x3F;
	item._iCreateInfo = ic;
	item._iSeed = iseed;
}

void CornerstoneSave()
{
	if (!CornerStone.activated)
		return;
	if (!CornerStone.item.isEmpty()) {
		ItemPack id;
		PackItem(&id, &CornerStone.item);
		const auto *buffer = reinterpret_cast<char *>(&id);
		for (size_t i = 0; i < sizeof(ItemPack); i++) {
			sprintf(&sgOptions.Hellfire.szItem[i * 2], "%02X", buffer[i]);
		}
	} else {
		sgOptions.Hellfire.szItem[0] = '\0';
	}
}

void CornerstoneLoad(Point position)
{
	ItemPack pkSItem;

	if (CornerStone.activated || position.x == 0 || position.y == 0) {
		return;
	}

	CornerStone.item._itype = ItemType::None;
	CornerStone.activated = true;
	if (dItem[position.x][position.y] != 0) {
		int ii = dItem[position.x][position.y] - 1;
		for (int i = 0; i < ActiveItemCount; i++) {
			if (ActiveItems[i] == ii) {
				DeleteItem(ii, i);
				break;
			}
		}
		dItem[position.x][position.y] = 0;
	}

	if (strlen(sgOptions.Hellfire.szItem) < sizeof(ItemPack) * 2)
		return;

	Hex2bin(sgOptions.Hellfire.szItem, sizeof(ItemPack), reinterpret_cast<uint8_t *>(&pkSItem));

	int ii = AllocateItem();
	auto &item = Items[ii];

	dItem[position.x][position.y] = ii + 1;

	UnPackItem(&pkSItem, &item, (pkSItem.dwBuff & CF_HELLFIRE) != 0);
	item.position = position;
	RespawnItem(&item, false);
	CornerStone.item = item;
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
					failed = !ItemSpaceOk(position + Displacement { i, j });
				}
			}
			if (!failed)
				break;
		}
	}

	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];

	item.position = position;

	dItem[position.x][position.y] = ii + 1;

	int curlv = ItemsGetCurrlevel();
	GetItemAttrs(item, itemid, curlv);

	SetupItem(item);
	item._iSeed = AdvanceRndSeed();
	SetRndSeed(item._iSeed);
	item._iPostDraw = true;
	if (selflag != 0) {
		item._iSelFlag = selflag;
		item.AnimInfo.CurrentFrame = item.AnimInfo.NumberOfFrames;
		item._iAnimFlag = false;
	}
}

void SpawnRewardItem(int itemid, Point position)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];

	item.position = position;
	dItem[position.x][position.y] = ii + 1;
	int curlv = ItemsGetCurrlevel();
	GetItemAttrs(item, itemid, curlv);
	item.SetNewAnimation(true);
	item._iSelFlag = 2;
	item._iPostDraw = true;
	item._iIdentified = true;
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

void RespawnItem(Item *item, bool flipFlag)
{
	int it = ItemCAnimTbl[item->_iCurs];
	item->SetNewAnimation(flipFlag);
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
	AvailableItems[MAXITEMS - ActiveItemCount] = ii;
	ActiveItemCount--;
	if (ActiveItemCount > 0 && i != ActiveItemCount)
		ActiveItems[i] = ActiveItems[ActiveItemCount];
	if (pcursitem == ii) // Unselect item if player has it highlighted
		pcursitem = -1;
}

void ProcessItems()
{
	for (int i = 0; i < ActiveItemCount; i++) {
		int ii = ActiveItems[i];
		auto &item = Items[ii];
		if (!item._iAnimFlag)
			continue;
		item.AnimInfo.ProcessAnimation();
		if (item._iCurs == ICURS_MAGIC_ROCK) {
			if (item._iSelFlag == 1 && item.AnimInfo.CurrentFrame == 11)
				item.AnimInfo.CurrentFrame = 1;
			if (item._iSelFlag == 2 && item.AnimInfo.CurrentFrame == 21)
				item.AnimInfo.CurrentFrame = 11;
		} else {
			if (item.AnimInfo.CurrentFrame == item.AnimInfo.NumberOfFrames / 2)
				PlaySfxLoc(ItemDropSnds[ItemCAnimTbl[item._iCurs]], item.position);

			if (item.AnimInfo.CurrentFrame >= item.AnimInfo.NumberOfFrames) {
				item.AnimInfo.CurrentFrame = item.AnimInfo.NumberOfFrames;
				item._iAnimFlag = false;
				item._iSelFlag = 1;
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

void GetItemFrm(Item &item)
{
	item.AnimInfo.pCelSprite = &*itemanims[ItemCAnimTbl[item._iCurs]];
}

void GetItemStr(Item &item)
{
	if (item._itype != ItemType::Gold) {
		if (item._iIdentified)
			strcpy(infostr, item._iIName);
		else
			strcpy(infostr, item._iName);

		InfoColor = item.getTextColor();
	} else {
		int nGold = item._ivalue;
		strcpy(infostr, fmt::format(ngettext("{:d} gold piece", "{:d} gold pieces", nGold), nGold).c_str());
	}
}

void CheckIdentify(Player &player, int cii)
{
	Item *pi;

	if (cii >= NUM_INVLOC)
		pi = &player.InvList[cii - NUM_INVLOC];
	else
		pi = &player.InvBody[cii];

	pi->_iIdentified = true;
	CalcPlrInv(player, true);

	if (&player == &Players[MyPlayerId])
		NewCursor(CURSOR_HAND);
}

void DoRepair(Player &player, int cii)
{
	Item *pi;

	PlaySfxLoc(IS_REPAIR, player.position.tile);

	if (cii >= NUM_INVLOC) {
		pi = &player.InvList[cii - NUM_INVLOC];
	} else {
		pi = &player.InvBody[cii];
	}

	RepairItem(*pi, player._pLevel);
	CalcPlrInv(player, true);

	if (&player == &Players[MyPlayerId])
		NewCursor(CURSOR_HAND);
}

void DoRecharge(Player &player, int cii)
{
	Item *pi;

	if (cii >= NUM_INVLOC) {
		pi = &player.InvList[cii - NUM_INVLOC];
	} else {
		pi = &player.InvBody[cii];
	}
	if (pi->_itype == ItemType::Staff && pi->_iSpell != SPL_NULL) {
		int r = GetSpellBookLevel(pi->_iSpell);
		r = GenerateRnd(player._pLevel / r) + 1;
		RechargeItem(*pi, r);
		CalcPlrInv(player, true);
	}

	if (&player == &Players[MyPlayerId])
		NewCursor(CURSOR_HAND);
}

void DoOil(Player &player, int cii)
{
	Item *pi;
	if (cii >= NUM_INVLOC) {
		pi = &player.InvList[cii - NUM_INVLOC];
	} else {
		pi = &player.InvBody[cii];
	}
	if (!ApplyOilToItem(*pi, player))
		return;
	CalcPlrInv(player, true);
	if (&player == &Players[MyPlayerId])
		NewCursor(CURSOR_HAND);
}

void PrintItemPower(char plidx, Item *x)
{
	switch (plidx) {
	case IPL_TOHIT:
	case IPL_TOHIT_CURSE:
		strcpy(tempstr, fmt::format(_("chance to hit: {:+d}%"), x->_iPLToHit).c_str());
		break;
	case IPL_DAMP:
	case IPL_DAMP_CURSE:
		/*xgettext:no-c-format*/ strcpy(tempstr, fmt::format(_("{:+d}% damage"), x->_iPLDam).c_str());
		break;
	case IPL_TOHIT_DAMP:
	case IPL_TOHIT_DAMP_CURSE:
		strcpy(tempstr, fmt::format(_("to hit: {:+d}%, {:+d}% damage"), x->_iPLToHit, x->_iPLDam).c_str());
		break;
	case IPL_ACP:
	case IPL_ACP_CURSE:
		/*xgettext:no-c-format*/ strcpy(tempstr, fmt::format(_("{:+d}% armor"), x->_iPLAC).c_str());
		break;
	case IPL_SETAC:
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
		strcpy(tempstr, fmt::format(ngettext("{:d} {:s} charge", "{:d} {:s} charges", x->_iMaxCharges), x->_iMaxCharges, pgettext("spell", spelldata[x->_iSpell].sNameText)).c_str());
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
		/*xgettext:no-c-format*/ strcpy(tempstr, fmt::format(_("decaying {:+d}% damage"), x->_iPLDam).c_str());
		break;
	case IPL_PERIL:
		strcpy(tempstr, _("2x dmg to monst, 1x to you"));
		break;
	case IPL_JESTERS:
		/*xgettext:no-c-format*/ strcpy(tempstr, _("Random 0 - 500% damage"));
		break;
	case IPL_CRYSTALLINE:
		/*xgettext:no-c-format*/ strcpy(tempstr, fmt::format(_("low dur, {:+d}% damage"), x->_iPLDam).c_str());
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

void DrawUniqueInfo(const Surface &out)
{
	const Point position { RightPanel.position.x - SPANEL_WIDTH, RightPanel.position.y };
	if ((chrflag || QuestLogIsOpen) && LeftPanel.Contains(position)) {
		return;
	}

	DrawUniqueInfoWindow(out);

	Rectangle rect { position + Displacement { 32, 56 }, { 257, 0 } };
	const UniqueItem &uitem = UniqueItems[curruitem._iUid];
	DrawString(out, _(uitem.UIName), rect, UiFlags::AlignCenter);

	const Rectangle dividerLineRect { position + Displacement { 26, 25 }, { 267, 3 } };
	out.BlitFrom(out, MakeSdlRect(dividerLineRect), dividerLineRect.position + Displacement { 0, 5 * 12 + 13 });

	rect.position.y += (10 - uitem.UINumPL) * 12;
	assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));
	for (const auto &power : uitem.powers) {
		if (power.type == IPL_INVALID)
			break;
		rect.position.y += 2 * 12;
		PrintItemPower(power.type, &curruitem);
		DrawString(out, tempstr, rect, UiFlags::ColorWhite | UiFlags::AlignCenter);
	}
}

void PrintItemDetails(Item *x)
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
		ShowUniqueItemInfoBox = true;
		curruitem = *x;
	}
	PrintItemInfo(*x);
}

void PrintItemDur(Item *x)
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
	if (IsAnyOf(x->_itype, ItemType::Ring, ItemType::Amulet))
		AddPanelString(_("Not Identified"));
	PrintItemInfo(*x);
}

void UseItem(int p, item_misc_id mid, spell_id spl)
{
	auto &player = Players[p];

	switch (mid) {
	case IMISC_HEAL:
	case IMISC_FOOD: {
		int j = player._pMaxHP >> 8;
		int l = ((j / 2) + GenerateRnd(j)) << 6;
		if (IsAnyOf(player._pClass, HeroClass::Warrior, HeroClass::Barbarian))
			l *= 2;
		if (IsAnyOf(player._pClass, HeroClass::Rogue, HeroClass::Monk, HeroClass::Bard))
			l += l / 2;
		player._pHitPoints = std::min(player._pHitPoints + l, player._pMaxHP);
		player._pHPBase = std::min(player._pHPBase + l, player._pMaxHPBase);
		drawhpflag = true;
	} break;
	case IMISC_FULLHEAL:
		player._pHitPoints = player._pMaxHP;
		player._pHPBase = player._pMaxHPBase;
		drawhpflag = true;
		break;
	case IMISC_MANA: {
		int j = player._pMaxMana >> 8;
		int l = ((j / 2) + GenerateRnd(j)) << 6;
		if (player._pClass == HeroClass::Sorcerer)
			l *= 2;
		if (IsAnyOf(player._pClass, HeroClass::Rogue, HeroClass::Monk, HeroClass::Bard))
			l += l / 2;
		if ((player._pIFlags & ISPL_NOMANA) == 0) {
			player._pMana = std::min(player._pMana + l, player._pMaxMana);
			player._pManaBase = std::min(player._pManaBase + l, player._pMaxManaBase);
			drawmanaflag = true;
		}
	} break;
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
	case IMISC_REJUV: {
		int j = player._pMaxHP >> 8;
		int l = ((j / 2) + GenerateRnd(j)) << 6;
		if (IsAnyOf(player._pClass, HeroClass::Warrior, HeroClass::Barbarian))
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
	} break;
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
			if (p == MyPlayerId)
				NewCursor(CURSOR_TELEPORT);
		} else {
			ClrPlrPath(player);
			player._pSpell = spl;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 3;
			player.destAction = ACTION_SPELL;
			player.destParam1 = cursPosition.x;
			player.destParam2 = cursPosition.y;
			if (p == MyPlayerId && spl == SPL_NOVA)
				NetSendCmdLoc(MyPlayerId, true, CMD_NOVA, cursPosition);
		}
		break;
	case IMISC_SCROLLT:
		if (spelldata[spl].sTargeted) {
			player._pTSpell = spl;
			if (p == MyPlayerId)
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
		if (p == MyPlayerId)
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
		player._pOilType = mid;
		if (p != MyPlayerId) {
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
		if (p == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_RUNEL:
		player._pTSpell = SPL_RUNELIGHT;
		if (p == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_GR_RUNEL:
		player._pTSpell = SPL_RUNENOVA;
		if (p == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_GR_RUNEF:
		player._pTSpell = SPL_RUNEIMMOLAT;
		if (p == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_RUNES:
		player._pTSpell = SPL_RUNESTONE;
		if (p == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	default:
		break;
	}
}

void SpawnSmith(int lvl)
{
	constexpr int PinnedItemCount = 0;

	Item holditem;
	holditem = Items[0];

	int maxValue = 140000;
	int maxItems = 20;
	if (gbIsHellfire) {
		maxValue = 200000;
		maxItems = 25;
	}

	int iCnt = GenerateRnd(maxItems - 10) + 10;
	for (int i = 0; i < iCnt; i++) {
		do {
			memset(&Items[0], 0, sizeof(*Items));
			Items[0]._iSeed = AdvanceRndSeed();
			SetRndSeed(Items[0]._iSeed);
			int itemData = RndSmithItem(lvl) - 1;
			GetItemAttrs(Items[0], itemData, lvl);
		} while (Items[0]._iIvalue > maxValue);
		smithitem[i] = Items[0];
		smithitem[i]._iCreateInfo = lvl | CF_SMITH;
		smithitem[i]._iIdentified = true;
		smithitem[i]._iStatFlag = StoreStatOk(smithitem[i]);
	}
	for (int i = iCnt; i < SMITH_ITEMS; i++)
		smithitem[i]._itype = ItemType::None;

	SortVendor(smithitem + PinnedItemCount);
	Items[0] = holditem;
}

void SpawnPremium(int pnum)
{
	int8_t lvl = Players[pnum]._pLevel;
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

void SpawnWitch(int lvl)
{
	constexpr int PinnedItemCount = 3;

	int j = PinnedItemCount;

	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(Items[0], IDI_MANA, 1);
	witchitem[0] = Items[0];
	witchitem[0]._iCreateInfo = lvl;
	witchitem[0]._iStatFlag = true;
	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(Items[0], IDI_FULLMANA, 1);
	witchitem[1] = Items[0];
	witchitem[1]._iCreateInfo = lvl;
	witchitem[1]._iStatFlag = true;
	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(Items[0], IDI_PORTAL, 1);
	witchitem[2] = Items[0];
	witchitem[2]._iCreateInfo = lvl;
	witchitem[2]._iStatFlag = true;

	int maxValue = 140000;
	int reservedItems = 12;
	if (gbIsHellfire) {
		maxValue = 200000;
		reservedItems = 10;

		int books = GenerateRnd(4);
		for (int i = 114, bCnt = 0; i <= 117 && bCnt < books; ++i) {
			if (!WitchItemOk(i))
				continue;
			if (lvl < AllItemsList[i].iMinMLvl)
				continue;

			memset(&Items[0], 0, sizeof(*Items));
			Items[0]._iSeed = AdvanceRndSeed();
			SetRndSeed(Items[0]._iSeed);
			AdvanceRndSeed();

			GetItemAttrs(Items[0], i, lvl);
			witchitem[j] = Items[0];
			witchitem[j]._iCreateInfo = lvl | CF_WITCH;
			witchitem[j]._iIdentified = true;
			WitchBookLevel(j);
			witchitem[j]._iStatFlag = StoreStatOk(witchitem[j]);
			j++;
			bCnt++;
		}
	}
	int iCnt = GenerateRnd(WITCH_ITEMS - reservedItems) + 10;

	for (int i = j; i < iCnt; i++) {
		do {
			memset(&Items[0], 0, sizeof(*Items));
			Items[0]._iSeed = AdvanceRndSeed();
			SetRndSeed(Items[0]._iSeed);
			int itemData = RndWitchItem(lvl) - 1;
			GetItemAttrs(Items[0], itemData, lvl);
			int maxlvl = -1;
			if (GenerateRnd(100) <= 5)
				maxlvl = 2 * lvl;
			if (maxlvl == -1 && Items[0]._iMiscId == IMISC_STAFF)
				maxlvl = 2 * lvl;
			if (maxlvl != -1)
				GetItemBonus(Items[0], maxlvl / 2, maxlvl, true, true);
		} while (Items[0]._iIvalue > maxValue);
		witchitem[i] = Items[0];
		witchitem[i]._iCreateInfo = lvl | CF_WITCH;
		witchitem[i]._iIdentified = true;
		WitchBookLevel(i);
		witchitem[i]._iStatFlag = StoreStatOk(witchitem[i]);
	}

	for (int i = iCnt; i < WITCH_ITEMS; i++)
		witchitem[i]._itype = ItemType::None;

	SortVendor(witchitem + PinnedItemCount);
}

void SpawnBoy(int lvl)
{
	int ivalue = 0;
	bool keepgoing = false;
	int count = 0;

	auto &myPlayer = Players[MyPlayerId];

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
		memset(&Items[0], 0, sizeof(*Items));
		Items[0]._iSeed = AdvanceRndSeed();
		SetRndSeed(Items[0]._iSeed);
		int itype = RndBoyItem(lvl) - 1;
		GetItemAttrs(Items[0], itype, lvl);
		GetItemBonus(Items[0], lvl, 2 * lvl, true, true);

		if (!gbIsHellfire) {
			if (Items[0]._iIvalue > 90000) {
				keepgoing = true; // prevent breaking the do/while loop too early by failing hellfire's condition in while
				continue;
			}
			break;
		}

		ivalue = 0;

		ItemType itemType = Items[0]._itype;

		switch (itemType) {
		case ItemType::LightArmor:
		case ItemType::MediumArmor:
		case ItemType::HeavyArmor: {
			const auto *const mostValuablePlayerArmor = myPlayer.GetMostValuableItem(
			    [](const Item &item) {
				    return IsAnyOf(item._itype, ItemType::LightArmor, ItemType::MediumArmor, ItemType::HeavyArmor);
			    });

			ivalue = mostValuablePlayerArmor == nullptr ? 0 : mostValuablePlayerArmor->_iIvalue;
			break;
		}
		case ItemType::Shield:
		case ItemType::Axe:
		case ItemType::Bow:
		case ItemType::Mace:
		case ItemType::Sword:
		case ItemType::Helm:
		case ItemType::Staff:
		case ItemType::Ring:
		case ItemType::Amulet: {
			const auto *const mostValuablePlayerItem = myPlayer.GetMostValuableItem(
			    [itemType](const Item &item) { return item._itype == itemType; });

			ivalue = mostValuablePlayerItem == nullptr ? 0 : mostValuablePlayerItem->_iIvalue;
			break;
		}
		default:
			app_fatal("Invalid item spawn");
		}
		ivalue = ivalue * 4 / 5; // avoids forced int > float > int conversion

		count++;

		if (count < 200) {
			switch (pc) {
			case HeroClass::Warrior:
				if (IsAnyOf(itemType, ItemType::Bow, ItemType::Staff))
					ivalue = INT_MAX;
				break;
			case HeroClass::Rogue:
				if (IsAnyOf(itemType, ItemType::Sword, ItemType::Staff, ItemType::Axe, ItemType::Mace, ItemType::Shield))
					ivalue = INT_MAX;
				break;
			case HeroClass::Sorcerer:
				if (IsAnyOf(itemType, ItemType::Staff, ItemType::Axe, ItemType::Bow, ItemType::Mace))
					ivalue = INT_MAX;
				break;
			case HeroClass::Monk:
				if (IsAnyOf(itemType, ItemType::Bow, ItemType::MediumArmor, ItemType::Shield, ItemType::Mace))
					ivalue = INT_MAX;
				break;
			case HeroClass::Bard:
				if (IsAnyOf(itemType, ItemType::Axe, ItemType::Mace, ItemType::Staff))
					ivalue = INT_MAX;
				break;
			case HeroClass::Barbarian:
				if (IsAnyOf(itemType, ItemType::Bow, ItemType::Staff))
					ivalue = INT_MAX;
				break;
			}
		}
	} while (keepgoing
	    || ((
	            Items[0]._iIvalue > 200000
	            || Items[0]._iMinStr > strength
	            || Items[0]._iMinMag > magic
	            || Items[0]._iMinDex > dexterity
	            || Items[0]._iIvalue < ivalue)
	        && count < 250));
	boyitem = Items[0];
	boyitem._iCreateInfo = lvl | CF_BOY;
	boyitem._iIdentified = true;
	boyitem._iStatFlag = StoreStatOk(boyitem);
	boylevel = lvl / 2;
}

void SpawnHealer(int lvl)
{
	constexpr int PinnedItemCount = 2;

	int srnd;

	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(Items[0], IDI_HEAL, 1);
	healitem[0] = Items[0];
	healitem[0]._iCreateInfo = lvl;
	healitem[0]._iStatFlag = true;

	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(Items[0], IDI_FULLHEAL, 1);
	healitem[1] = Items[0];
	healitem[1]._iCreateInfo = lvl;
	healitem[1]._iStatFlag = true;

	if (gbIsMultiplayer) {
		memset(&Items[0], 0, sizeof(*Items));
		GetItemAttrs(Items[0], IDI_RESURRECT, 1);
		healitem[2] = Items[0];
		healitem[2]._iCreateInfo = lvl;
		healitem[2]._iStatFlag = true;

		srnd = 3;
	} else {
		srnd = 2;
	}
	int nsi = GenerateRnd(gbIsHellfire ? 10 : 8) + 10;
	for (int i = srnd; i < nsi; i++) {
		memset(&Items[0], 0, sizeof(*Items));
		Items[0]._iSeed = AdvanceRndSeed();
		SetRndSeed(Items[0]._iSeed);
		int itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(Items[0], itype, lvl);
		healitem[i] = Items[0];
		healitem[i]._iCreateInfo = lvl | CF_HEALER;
		healitem[i]._iIdentified = true;
		healitem[i]._iStatFlag = StoreStatOk(healitem[i]);
	}
	for (int i = nsi; i < 20; i++) {
		healitem[i]._itype = ItemType::None;
	}
	SortVendor(healitem + PinnedItemCount);
}

void SpawnStoreGold()
{
	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(Items[0], IDI_GOLD, 1);
	golditem = Items[0];
	golditem._iStatFlag = true;
}

int ItemNoFlippy()
{
	int r = ActiveItems[ActiveItemCount - 1];
	Items[r].AnimInfo.CurrentFrame = Items[r].AnimInfo.NumberOfFrames;
	Items[r]._iAnimFlag = false;
	Items[r]._iSelFlag = 1;

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

	int idx = RndTypeItems(ItemType::Misc, IMISC_BOOK, lvl);
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];

	while (true) {
		memset(&item, 0, sizeof(*Items));
		SetupAllItems(item, idx, AdvanceRndSeed(), 2 * lvl, 1, true, false, delta);
		if (item._iMiscId == IMISC_BOOK && item._iSpell == ispell)
			break;
	}
	GetSuperItemSpace(position, ii);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
	if (delta)
		DeltaAddItem(ii);
}

void CreateMagicArmor(Point position, ItemType itemType, int icurs, bool sendmsg, bool delta)
{
	int lvl = ItemsGetCurrlevel();
	CreateMagicItem(position, lvl, itemType, IMISC_NONE, icurs, sendmsg, delta);
}

void CreateAmulet(Point position, int lvl, bool sendmsg, bool delta)
{
	CreateMagicItem(position, lvl, ItemType::Amulet, IMISC_AMULET, ICURS_AMULET, sendmsg, delta);
}

void CreateMagicWeapon(Point position, ItemType itemType, int icurs, bool sendmsg, bool delta)
{
	int imid = IMISC_NONE;
	if (itemType == ItemType::Staff)
		imid = IMISC_STAFF;

	int curlv = ItemsGetCurrlevel();

	CreateMagicItem(position, curlv, itemType, imid, icurs, sendmsg, delta);
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

#ifdef _DEBUG
std::mt19937 BetterRng;
std::string DebugSpawnItem(std::string itemName)
{
	if (ActiveItemCount >= MAXITEMS)
		return "No space to generate the item!";

	const int max_time = 3000;
	const int max_iter = 1000000;

	std::transform(itemName.begin(), itemName.end(), itemName.begin(), [](unsigned char c) { return std::tolower(c); });

	int ii = AllocateItem();
	auto &item = Items[ii];
	Point pos = Players[MyPlayerId].position.tile;
	GetSuperItemSpace(pos, ii);

	uint32_t begin = SDL_GetTicks();
	Monster fake_m;
	fake_m.MData = &MonstersData[0];
	fake_m._uniqtype = 0;

	int i = 0;
	for (;; i++) {
		// using a better rng here to seed the item to prevent getting stuck repeating same values using old one
		std::uniform_int_distribution<int32_t> dist(0, INT_MAX);
		SetRndSeed(dist(BetterRng));
		if (SDL_GetTicks() - begin > max_time)
			return fmt::format("Item not found in {:d} seconds!", max_time / 1000);

		if (i > max_iter)
			return fmt::format("Item not found in {:d} tries!", max_iter);

		fake_m.mLevel = dist(BetterRng) % CF_LEVEL + 1;

		int idx = RndItem(fake_m);
		if (idx > 1) {
			idx--;
		} else
			continue;

		Point bkp = item.position;
		memset(&item, 0, sizeof(Item));
		item.position = bkp;
		SetupAllItems(item, idx, AdvanceRndSeed(), fake_m.mLevel, 1, false, false, false);

		std::string tmp(item._iIName);
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](unsigned char c) { return std::tolower(c); });
		if (tmp.find(itemName) != std::string::npos)
			break;
	}

	item._iIdentified = true;
	NetSendCmdDItem(false, ii);
	return fmt::format("Item generated successfully - iterations: {:d}", i);
}

std::string DebugSpawnUniqueItem(std::string itemName)
{
	if (ActiveItemCount >= MAXITEMS)
		return "No space to generate the item!";

	const int max_time = 3000;
	const int max_iter = 1000000;

	std::transform(itemName.begin(), itemName.end(), itemName.begin(), [](unsigned char c) { return std::tolower(c); });
	UniqueItem uniqueItem;
	bool foundUnique = false;
	int uniqueBaseIndex = 0;
	int uniqueIndex = 0;
	for (int j = 0; UniqueItems[j].UIItemId != UITYPE_INVALID; j++) {
		if (!IsUniqueAvailable(j))
			break;

		std::string tmp(UniqueItems[j].UIName);
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](unsigned char c) { return std::tolower(c); });
		if (tmp.find(itemName) != std::string::npos) {
			itemName = tmp;
			uniqueItem = UniqueItems[j];
			uniqueIndex = j;
			foundUnique = true;
			break;
		}
	}
	if (!foundUnique)
		return "No unique found!";

	for (int j = 0; AllItemsList[j].iLoc != ILOC_INVALID; j++) {
		if (!IsItemAvailable(j))
			continue;
		if (AllItemsList[j].iItemId == uniqueItem.UIItemId)
			uniqueBaseIndex = j;
	}

	int ii = AllocateItem();
	auto &item = Items[ii];
	Point pos = Players[MyPlayerId].position.tile;
	GetSuperItemSpace(pos, ii);

	int i = 0;
	for (uint32_t begin = SDL_GetTicks();; i++) {
		if (SDL_GetTicks() - begin > max_time)
			return fmt::format("Item not found in {:d} seconds!", max_time / 1000);

		if (i > max_iter)
			return fmt::format("Item not found in {:d} tries!", max_iter);

		Point bkp = item.position;
		memset(&item, 0, sizeof(Item));
		item.position = bkp;
		std::uniform_int_distribution<int32_t> dist(0, INT_MAX);
		SetRndSeed(dist(BetterRng));
		for (auto &flag : UniqueItemFlags)
			flag = true;
		UniqueItemFlags[uniqueIndex] = false;
		SetupAllItems(item, uniqueBaseIndex, AdvanceRndSeed(), uniqueItem.UIMinLvl, 1, false, false, false);
		for (auto &flag : UniqueItemFlags)
			flag = false;

		if (item._iMagical != ITEM_QUALITY_UNIQUE)
			continue;

		std::string tmp(item._iIName);
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](unsigned char c) { return std::tolower(c); });
		if (tmp.find(itemName) != std::string::npos)
			break;
		return "Impossible to generate!";
	}

	item._iIdentified = true;
	NetSendCmdDItem(false, ii);
	return fmt::format("Item generated successfully - iterations: {:d}", i);
}
#endif

void Item::SetNewAnimation(bool showAnimation)
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

void initItemGetRecords()
{
	memset(itemrecord, 0, sizeof(itemrecord));
	gnNumGetRecords = 0;
}

} // namespace devilution
