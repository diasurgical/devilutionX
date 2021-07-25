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

/** Contains the items on ground in the current game. */
ItemStruct Items[MAXITEMS + 1];
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
ItemStruct curruitem;
ItemGetRecordStruct itemrecord[MAXITEMS];
bool itemhold[3][3];
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

		Point position = GetRandomAvailableItemPosition();
		Items[ii].position = position;

		dItem[position.x][position.y] = ii + 1;

		Items[ii]._iSeed = AdvanceRndSeed();

		if (GenerateRnd(2) != 0)
			GetItemAttrs(ii, IDI_HEAL, curlv);
		else
			GetItemAttrs(ii, IDI_MANA, curlv);

		Items[ii]._iCreateInfo = curlv | CF_PREGEN;
		SetupItem(ii);
		Items[ii].AnimInfo.CurrentFrame = Items[ii].AnimInfo.NumberOfFrames;
		Items[ii]._iAnimFlag = false;
		Items[ii]._iSelFlag = 1;
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

void CalcSelfItems(PlayerStruct &player)
{
	int sa = 0;
	int ma = 0;
	int da = 0;
	ItemStruct *pi = player.InvBody;
	for (int i = 0; i < NUM_INVLOC; i++, pi++) {
		if (!pi->isEmpty()) {
			pi->_iStatFlag = true;
			if (pi->_iIdentified) {
				sa += pi->_iPLStr;
				ma += pi->_iPLMag;
				da += pi->_iPLDex;
			}
		}
	}

	bool changeflag;
	do {
		changeflag = false;
		pi = player.InvBody;
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

bool ItemMinStats(const PlayerStruct &player, ItemStruct *x)
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

bool StoreStatOk(ItemStruct *h)
{
	const auto &myPlayer = Players[MyPlayerId];

	if (myPlayer._pStrength < h->_iMinStr)
		return false;
	if (myPlayer._pMagic < h->_iMinMag)
		return false;
	if (myPlayer._pDexterity < h->_iMinDex)
		return false;

	return true;
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
			int8_t spellLevel = player._pSplLvl[player.InvList[i]._iSpell];

			while (spellLevel != 0) {
				player.InvList[i]._iMinMag += 20 * player.InvList[i]._iMinMag / 100;
				spellLevel--;
				if (player.InvList[i]._iMinMag + 20 * player.InvList[i]._iMinMag / 100 > 255) {
					player.InvList[i]._iMinMag = 255;
					spellLevel = 0;
				}
			}
			player.InvList[i]._iStatFlag = ItemMinStats(player, &player.InvList[i]);
		}
	}
}

void SetPlrHandSeed(ItemStruct *h, int iseed)
{
	h->_iSeed = iseed;
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

void CalcItemValue(int i)
{
	int v = Items[i]._iVMult1 + Items[i]._iVMult2;
	if (v > 0) {
		v *= Items[i]._ivalue;
	}
	if (v < 0) {
		v = Items[i]._ivalue / v;
	}
	v = Items[i]._iVAdd1 + Items[i]._iVAdd2 + v;
	Items[i]._iIvalue = std::max(v, 1);
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
	strcat(Items[i]._iName, _(spelldata[bs].sNameText));
	strcat(Items[i]._iIName, _(spelldata[bs].sNameText));
	Items[i]._iSpell = bs;
	Items[i]._iMinMag = spelldata[bs].sMinInt;
	Items[i]._ivalue += spelldata[bs].sBookCost;
	Items[i]._iIvalue += spelldata[bs].sBookCost;
	if (spelldata[bs].sType == STYPE_FIRE)
		Items[i]._iCurs = ICURS_BOOK_RED;
	else if (spelldata[bs].sType == STYPE_LIGHTNING)
		Items[i]._iCurs = ICURS_BOOK_BLUE;
	else if (spelldata[bs].sType == STYPE_MAGIC)
		Items[i]._iCurs = ICURS_BOOK_GREY;
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

int SaveItemPower(ItemStruct &item, const ItemPower &power)
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
	return GetLineWidth(str, GameFontSmall, 0) < 125;
}

int PLVal(int pv, int p1, int p2, int minv, int maxv)
{
	if (p1 == p2)
		return minv;
	if (minv == maxv)
		return minv;
	return minv + (maxv - minv) * (100 * (pv - p1) / (p2 - p1)) / 100;
}

void SaveItemAffix(ItemStruct &item, const PLStruct &affix)
{
	auto power = affix.power;

	if (!gbIsHellfire) {
		if (power.type == IPL_TARGAC) {
			power.param1 = 2 << power.param1;
			power.param2 = 6 << power.param2;
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

void GetStaffPower(int i, int lvl, int bs, bool onlygood)
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
			sprintf(istr, "%s %s", _(ItemPrefixes[preidx].PLName), Items[i]._iIName);
			strcpy(Items[i]._iIName, istr);
			Items[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemAffix(Items[i], ItemPrefixes[preidx]);
			Items[i]._iPrePower = ItemPrefixes[preidx].power.type;
		}
	}
	if (!StringInPanel(Items[i]._iIName)) {
		strcpy(Items[i]._iIName, _(AllItemsList[Items[i].IDidx].iSName));
		char istr[128];
		if (preidx != -1) {
			sprintf(istr, "%s %s", _(ItemPrefixes[preidx].PLName), Items[i]._iIName);
			strcpy(Items[i]._iIName, istr);
		}
		strcpy(istr, fmt::format(_(/* TRANSLATORS: Constructs item names. Format: <Prefix> <Item> of <Suffix>. Example: King's Long Sword of the Whale */ "{:s} of {:s}"), Items[i]._iIName, _(spelldata[bs].sNameText)).c_str());
		strcpy(Items[i]._iIName, istr);
		if (Items[i]._iMagical == ITEM_QUALITY_NORMAL)
			strcpy(Items[i]._iName, Items[i]._iIName);
	}
	CalcItemValue(i);
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
			sprintf(istr, "%s %s", _(ItemPrefixes[preidx].PLName), Items[i]._iIName);
			strcpy(Items[i]._iIName, istr);
			Items[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemAffix(Items[i], ItemPrefixes[preidx]);
			Items[i]._iPrePower = ItemPrefixes[preidx].power.type;
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
			strcpy(istr, fmt::format(_("{:s} of {:s}"), Items[i]._iIName, _(ItemSuffixes[sufidx].PLName)).c_str());
			strcpy(Items[i]._iIName, istr);
			Items[i]._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemAffix(Items[i], ItemSuffixes[sufidx]);
			Items[i]._iSufPower = ItemSuffixes[sufidx].power.type;
		}
	}
	if (!StringInPanel(Items[i]._iIName)) {
		int aii = Items[i].IDidx;
		if (AllItemsList[aii].iSName != nullptr)
			strcpy(Items[i]._iIName, _(AllItemsList[aii].iSName));
		else
			Items[i]._iName[0] = 0;

		if (preidx != -1) {
			sprintf(istr, "%s %s", _(ItemPrefixes[preidx].PLName), Items[i]._iIName);
			strcpy(Items[i]._iIName, istr);
		}
		if (sufidx != -1) {
			strcpy(istr, fmt::format(_("{:s} of {:s}"), Items[i]._iIName, _(ItemSuffixes[sufidx].PLName)).c_str());
			strcpy(Items[i]._iIName, istr);
		}
	}
	if (preidx != -1 || sufidx != -1)
		CalcItemValue(i);
}

void GetStaffSpell(int i, int lvl, bool onlygood)
{
	if (!gbIsHellfire && GenerateRnd(4) == 0) {
		GetItemPower(i, lvl / 2, lvl, PLT_STAFF, onlygood);
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
		strcpy(istr, fmt::format(_("{:s} of {:s}"), Items[i]._iName, _(spelldata[bs].sNameText)).c_str());
	strcpy(istr, fmt::format(_("Staff of {:s}"), _(spelldata[bs].sNameText)).c_str());
	strcpy(Items[i]._iName, istr);
	strcpy(Items[i]._iIName, istr);

	int minc = spelldata[bs].sStaffMin;
	int maxc = spelldata[bs].sStaffMax - minc + 1;
	Items[i]._iSpell = bs;
	Items[i]._iCharges = minc + GenerateRnd(maxc);
	Items[i]._iMaxCharges = Items[i]._iCharges;

	Items[i]._iMinMag = spelldata[bs].sMinInt;
	int v = Items[i]._iCharges * spelldata[bs].sStaffCost / 5;
	Items[i]._ivalue += v;
	Items[i]._iIvalue += v;
	GetStaffPower(i, lvl, bs, onlygood);
}

void GetOilType(int i, int maxLvl)
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

	strcpy(Items[i]._iName, _(OilNames[t]));
	strcpy(Items[i]._iIName, _(OilNames[t]));
	Items[i]._iMiscId = OilMagic[t];
	Items[i]._ivalue = OilValues[t];
	Items[i]._iIvalue = OilValues[t];
}

void GetItemBonus(int i, int minlvl, int maxlvl, bool onlygood, bool allowspells)
{
	if (minlvl > 25)
		minlvl = 25;

	switch (Items[i]._itype) {
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

int RndUItem(MonsterStruct *monster)
{
	if (monster != nullptr && (monster->MData->mTreasure & 0x8000) != 0 && !gbIsMultiplayer)
		return -((monster->MData->mTreasure & 0xFFF) + 1);

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
		if (UniqueItemList[j].UIItemId == AllItemsList[Items[i].IDidx].iItemId
		    && lvl >= UniqueItemList[j].UIMinLvl
		    && (recreate || !UniqueItemFlags[j] || gbIsMultiplayer)) {
			uok[j] = true;
			numu++;
		}
	}

	if (numu == 0)
		return UITEM_INVALID;

	AdvanceRndSeed();
	uint8_t idata = 0;
	while (numu > 0) {
		if (uok[idata])
			numu--;
		if (numu > 0)
			idata = (idata + 1) % 128;
	}

	return (_unique_items)idata;
}

void GetUniqueItem(ItemStruct &item, _unique_items uid)
{
	UniqueItemFlags[uid] = true;

	for (const auto &power : UniqueItemList[uid].powers) {
		if (power.type == IPL_INVALID)
			break;
		SaveItemPower(item, power);
	}

	strcpy(item._iIName, _(UniqueItemList[uid].UIName));
	item._iIvalue = UniqueItemList[uid].UIValue;

	if (item._iMiscId == IMISC_UNIQUE)
		item._iSeed = uid;

	item._iUid = uid;
	item._iMagical = ITEM_QUALITY_UNIQUE;
	item._iCreateInfo |= CF_UNIQUE;
}

void ItemRndDur(int ii)
{
	if (Items[ii]._iDurability > 0 && Items[ii]._iDurability != DUR_INDESTRUCTIBLE)
		Items[ii]._iDurability = GenerateRnd(Items[ii]._iMaxDur / 2) + (Items[ii]._iMaxDur / 4) + 1;
}

void SetupAllItems(int ii, int idx, int iseed, int lvl, int uper, bool onlygood, bool recreate, bool pregen)
{
	int iblvl;

	Items[ii]._iSeed = iseed;
	SetRndSeed(iseed);
	GetItemAttrs(ii, idx, lvl / 2);
	Items[ii]._iCreateInfo = lvl;

	if (pregen)
		Items[ii]._iCreateInfo |= CF_PREGEN;
	if (onlygood)
		Items[ii]._iCreateInfo |= CF_ONLYGOOD;

	if (uper == 15)
		Items[ii]._iCreateInfo |= CF_UPER15;
	else if (uper == 1)
		Items[ii]._iCreateInfo |= CF_UPER1;

	if (Items[ii]._iMiscId != IMISC_UNIQUE) {
		iblvl = -1;
		if (GenerateRnd(100) <= 10 || GenerateRnd(100) <= lvl) {
			iblvl = lvl;
		}
		if (iblvl == -1 && Items[ii]._iMiscId == IMISC_STAFF) {
			iblvl = lvl;
		}
		if (iblvl == -1 && Items[ii]._iMiscId == IMISC_RING) {
			iblvl = lvl;
		}
		if (iblvl == -1 && Items[ii]._iMiscId == IMISC_AMULET) {
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
				GetUniqueItem(Items[ii], uid);
			}
		}
		if (Items[ii]._iMagical != ITEM_QUALITY_UNIQUE)
			ItemRndDur(ii);
	} else {
		if (Items[ii]._iLoc != ILOC_UNEQUIPABLE) {
			GetUniqueItem(Items[ii], (_unique_items)iseed); // uid is stored in iseed for uniques
		}
	}
	SetupItem(ii);
}

void SetupBaseItem(Point position, int idx, bool onlygood, bool sendmsg, bool delta)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	GetSuperItemSpace(position, ii);
	int curlv = ItemsGetCurrlevel();

	SetupAllItems(ii, idx, AdvanceRndSeed(), 2 * curlv, 1, onlygood, false, delta);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
	if (delta)
		DeltaAddItem(ii);
}

void SetupAllUseful(int ii, int iseed, int lvl)
{
	int idx;

	Items[ii]._iSeed = iseed;
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
	Items[ii]._iCreateInfo = lvl | CF_USEFUL;
	SetupItem(ii);
}

int Char2int(char input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	return 0;
}

void Hex2bin(const char *src, int bytes, char *target)
{
	for (int i = 0; i < bytes; i++, src += 2) {
		target[i] = (Char2int(*src) << 4) | Char2int(src[1]);
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

	Items[ii].position = Objects[oi].position;
	dItem[Objects[oi].position.x][Objects[oi].position.y] = ii + 1;
	int curlv = ItemsGetCurrlevel();
	GetItemAttrs(ii, IDI_ROCK, curlv);
	SetupItem(ii);
	Items[ii]._iSelFlag = 2;
	Items[ii]._iPostDraw = true;
	Items[ii].AnimInfo.CurrentFrame = 11;
}

void ItemDoppel()
{
	if (!gbIsMultiplayer)
		return;

	static int idoppely = 16;

	for (int idoppelx = 16; idoppelx < 96; idoppelx++) {
		if (dItem[idoppelx][idoppely] != 0) {
			ItemStruct *i = &Items[dItem[idoppelx][idoppely] - 1];
			if (i->position.x != idoppelx || i->position.y != idoppely)
				dItem[idoppelx][idoppely] = 0;
		}
	}

	idoppely++;
	if (idoppely == 96)
		idoppely = 16;
}

void RepairItem(ItemStruct *i, int lvl)
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

void RechargeItem(ItemStruct *i, int r)
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

bool ApplyOilToItem(ItemStruct *x, PlayerStruct &player)
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
		if (x->_iMaxDur != DUR_INDESTRUCTIBLE && x->_iMaxDur < 200) {
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
	CelDrawTo(out, { RIGHT_PANEL_X - SPANEL_WIDTH + 24, 327 }, *pSTextBoxCels, 1);
	DrawHalfTransparentRectTo(out, RIGHT_PANEL_X - SPANEL_WIDTH + 27, 28, 265, 297);
}

void DrawUniqueInfoDevider(const Surface &out, int y)
{
	BYTE *src = out.at(26 + RIGHT_PANEL - SPANEL_WIDTH, 25);
	BYTE *dst = out.at(26 + RIGHT_PANEL_X - SPANEL_WIDTH, y * 12 + 38);

	for (int i = 0; i < 3; i++, src += out.pitch(), dst += out.pitch())
		memcpy(dst, src, 267); // BUGFIX: should be 267 (fixed)
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

void PrintItemInfo(ItemStruct *x)
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

void SortVendor(ItemStruct *itemList)
{
	int count = 1;
	while (!itemList[count].isEmpty())
		count++;

	auto cmp = [](const ItemStruct &a, const ItemStruct &b) {
		return a.IDidx < b.IDidx;
	};

	std::sort(itemList, itemList + count, cmp);
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

void SpawnOnePremium(int i, int plvl, int playerId)
{
	int itemValue = 0;
	bool keepGoing = false;
	ItemStruct tempItem = Items[0];

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
		int itemType = RndPremiumItem(plvl / 4, plvl) - 1;
		GetItemAttrs(0, itemType, plvl);
		GetItemBonus(0, plvl / 2, plvl, true, !gbIsHellfire);

		if (!gbIsHellfire) {
			if (Items[0]._iIvalue > 140000) {
				keepGoing = true; // prevent breaking the do/while loop too early by failing hellfire's condition in while
				continue;
			}
			break;
		}

		switch (Items[0]._itype) {
		case ITYPE_LARMOR:
		case ITYPE_MARMOR:
		case ITYPE_HARMOR: {
			const auto *const mostValuablePlayerArmor = player.GetMostValuableItem(
			    [](const ItemStruct &item) {
				    return item._itype == ITYPE_LARMOR
				        || item._itype == ITYPE_MARMOR
				        || item._itype == ITYPE_HARMOR;
			    });

			itemValue = mostValuablePlayerArmor == nullptr ? 0 : mostValuablePlayerArmor->_iIvalue;
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
			const auto *const mostValuablePlayerItem = player.GetMostValuableItem(
			    [](const ItemStruct &item) { return item._itype == Items[0]._itype; });

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
	premiumitems[i]._iStatFlag = StoreStatOk(&premiumitems[i]);
	Items[0] = tempItem;
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

int RndBoyItem(int lvl)
{
	return RndVendorItem<PremiumItemOk>(0, lvl);
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

void RecreateSmithItem(int ii, int lvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndSmithItem(lvl) - 1;
	GetItemAttrs(ii, itype, lvl);

	Items[ii]._iSeed = iseed;
	Items[ii]._iCreateInfo = lvl | CF_SMITH;
	Items[ii]._iIdentified = true;
}

void RecreatePremiumItem(int ii, int plvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndPremiumItem(plvl / 4, plvl) - 1;
	GetItemAttrs(ii, itype, plvl);
	GetItemBonus(ii, plvl / 2, plvl, true, !gbIsHellfire);

	Items[ii]._iSeed = iseed;
	Items[ii]._iCreateInfo = plvl | CF_SMITHPREMIUM;
	Items[ii]._iIdentified = true;
}

void RecreateBoyItem(int ii, int lvl, int iseed)
{
	SetRndSeed(iseed);
	int itype = RndBoyItem(lvl) - 1;
	GetItemAttrs(ii, itype, lvl);
	GetItemBonus(ii, lvl, 2 * lvl, true, true);

	Items[ii]._iSeed = iseed;
	Items[ii]._iCreateInfo = lvl | CF_BOY;
	Items[ii]._iIdentified = true;
}

void RecreateWitchItem(int ii, int idx, int lvl, int iseed)
{
	if (idx == IDI_MANA || idx == IDI_FULLMANA || idx == IDI_PORTAL) {
		GetItemAttrs(ii, idx, lvl);
	} else if (gbIsHellfire && idx >= 114 && idx <= 117) {
		SetRndSeed(iseed);
		AdvanceRndSeed();
		GetItemAttrs(ii, idx, lvl);
	} else {
		SetRndSeed(iseed);
		int itype = RndWitchItem(lvl) - 1;
		GetItemAttrs(ii, itype, lvl);
		int iblvl = -1;
		if (GenerateRnd(100) <= 5)
			iblvl = 2 * lvl;
		if (iblvl == -1 && Items[ii]._iMiscId == IMISC_STAFF)
			iblvl = 2 * lvl;
		if (iblvl != -1)
			GetItemBonus(ii, iblvl / 2, iblvl, true, true);
	}

	Items[ii]._iSeed = iseed;
	Items[ii]._iCreateInfo = lvl | CF_WITCH;
	Items[ii]._iIdentified = true;
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

	Items[ii]._iSeed = iseed;
	Items[ii]._iCreateInfo = lvl | CF_HEALER;
	Items[ii]._iIdentified = true;
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

void CreateMagicItem(Point position, int lvl, int imisc, int imid, int icurs, bool sendmsg, bool delta)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	int idx = RndTypeItems(imisc, imid, lvl);

	while (true) {
		memset(&Items[ii], 0, sizeof(*Items));
		SetupAllItems(ii, idx, AdvanceRndSeed(), 2 * lvl, 1, true, false, delta);
		if (Items[ii]._iCurs == icurs)
			break;

		idx = RndTypeItems(imisc, imid, lvl);
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
	GetItemAttrs(0, IDI_GOLD, 1);
	golditem = Items[0];
	golditem._iStatFlag = true;
	ActiveItemCount = 0;

	for (int i = 0; i < MAXITEMS; i++) {
		Items[i]._itype = ITYPE_NONE;
		Items[i].position = { 0, 0 };
		Items[i]._iAnimFlag = false;
		Items[i]._iSelFlag = 0;
		Items[i]._iIdentified = false;
		Items[i]._iPostDraw = false;
	}

	for (int i = 0; i < MAXITEMS; i++) {
		AvailableItems[i] = i;
		ActiveItems[i] = 0;
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
			SpawnNote();
	}

	ShowUniqueItemInfoBox = false;
}

void CalcPlrItemVals(int playerId, bool loadgfx)
{
	auto &player = Players[playerId];

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

	if (player._pLightRad != lrad && playerId == MyPlayerId) {
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

	if (playerId == MyPlayerId && (player._pHitPoints >> 6) <= 0) {
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

	item_type weaponItemType = item_type::ITYPE_NONE;
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

	if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
		player._pBlockFlag = true;
		holdsShield = true;
	}
	if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
		player._pBlockFlag = true;
		holdsShield = true;
	}

	PlayerWeaponGraphic animWeaponId = holdsShield ? PlayerWeaponGraphic::UnarmedShield : PlayerWeaponGraphic::Unarmed;
	switch (weaponItemType) {
	case ITYPE_SWORD:
		animWeaponId = holdsShield ? PlayerWeaponGraphic::SwordShield : PlayerWeaponGraphic::Sword;
		break;
	case ITYPE_AXE:
		animWeaponId = PlayerWeaponGraphic::Axe;
		break;
	case ITYPE_BOW:
		animWeaponId = PlayerWeaponGraphic::Bow;
		break;
	case ITYPE_MACE:
		animWeaponId = holdsShield ? PlayerWeaponGraphic::MaceShield : PlayerWeaponGraphic::Mace;
		break;
	case ITYPE_STAFF:
		animWeaponId = PlayerWeaponGraphic::Staff;
		break;
    default:
		break;
	}

	PlayerArmorGraphic animArmorId = PlayerArmorGraphic::Light;
	if (player.InvBody[INVLOC_CHEST]._itype == ITYPE_HARMOR && player.InvBody[INVLOC_CHEST]._iStatFlag) {
		if (player._pClass == HeroClass::Monk && player.InvBody[INVLOC_CHEST]._iMagical == ITEM_QUALITY_UNIQUE)
			player._pIAC += player._pLevel / 2;
		animArmorId = PlayerArmorGraphic::Heavy;
	} else if (player.InvBody[INVLOC_CHEST]._itype == ITYPE_MARMOR && player.InvBody[INVLOC_CHEST]._iStatFlag) {
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
			player.AnimInfo.ChangeAnimationData(&*player.AnimationData[static_cast<size_t>(player_graphic::Stand)].CelSpritesForDirections[player._pdir], player._pNFrames, 4);
		} else {
			LoadPlrGFX(player, player_graphic::Walk);
			player.AnimInfo.ChangeAnimationData(&*player.AnimationData[static_cast<size_t>(player_graphic::Walk)].CelSpritesForDirections[player._pdir], player._pWFrames, 1);
		}
	} else {
		player._pgfxnum = gfxNum;
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

void CalcPlrInv(int playerId, bool loadgfx)
{
	auto &player = Players[playerId];

	CalcPlrItemMin(player);
	CalcSelfItems(player);
	CalcPlrItemVals(playerId, loadgfx);
	CalcPlrItemMin(player);
	if (playerId == MyPlayerId) {
		CalcPlrBookVals(player);
		player.CalcScrolls();
		CalcPlrStaff(player);
		if (playerId == MyPlayerId && currlevel == 0)
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
	int s = 0;

	bool doneflag;
	do {
		doneflag = true;
		s = AdvanceRndSeed();
		for (int i = 0; i < ActiveItemCount; i++) {
			int ii = ActiveItems[i];
			if (Items[ii]._iSeed == s)
				doneflag = false;
		}
		if (pnum == MyPlayerId) {
			for (int i = 0; i < Players[pnum]._pNumInv; i++) {
				if (Players[pnum].InvList[i]._iSeed == s)
					doneflag = false;
			}
		}
	} while (!doneflag);

	h->_iSeed = s;
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
	auto &player = Players[playerId];

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

void GetItemAttrs(int i, int idata, int lvl)
{
	Items[i]._itype = AllItemsList[idata].itype;
	Items[i]._iCurs = AllItemsList[idata].iCurs;
	strcpy(Items[i]._iName, _(AllItemsList[idata].iName));
	strcpy(Items[i]._iIName, _(AllItemsList[idata].iName));
	Items[i]._iLoc = AllItemsList[idata].iLoc;
	Items[i]._iClass = AllItemsList[idata].iClass;
	Items[i]._iMinDam = AllItemsList[idata].iMinDam;
	Items[i]._iMaxDam = AllItemsList[idata].iMaxDam;
	Items[i]._iAC = AllItemsList[idata].iMinAC + GenerateRnd(AllItemsList[idata].iMaxAC - AllItemsList[idata].iMinAC + 1);
	Items[i]._iFlags = AllItemsList[idata].iFlags;
	Items[i]._iMiscId = AllItemsList[idata].iMiscId;
	Items[i]._iSpell = AllItemsList[idata].iSpell;
	Items[i]._iMagical = ITEM_QUALITY_NORMAL;
	Items[i]._ivalue = AllItemsList[idata].iValue;
	Items[i]._iIvalue = AllItemsList[idata].iValue;
	Items[i]._iDurability = AllItemsList[idata].iDurability;
	Items[i]._iMaxDur = AllItemsList[idata].iDurability;
	Items[i]._iMinStr = AllItemsList[idata].iMinStr;
	Items[i]._iMinMag = AllItemsList[idata].iMinMag;
	Items[i]._iMinDex = AllItemsList[idata].iMinDex;
	Items[i].IDidx = static_cast<_item_indexes>(idata);
	if (gbIsHellfire)
		Items[i].dwBuff |= CF_HELLFIRE;
	Items[i]._iPrePower = IPL_INVALID;
	Items[i]._iSufPower = IPL_INVALID;

	if (Items[i]._iMiscId == IMISC_BOOK)
		GetBookSpell(i, lvl);

	if (gbIsHellfire && Items[i]._iMiscId == IMISC_OILOF)
		GetOilType(i, lvl);

	if (Items[i]._itype != ITYPE_GOLD)
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

	Items[i]._ivalue = std::min(rndv, GOLD_MAX_LIMIT);
	SetPlrHandGoldCurs(&Items[i]);
}

void SetupItem(int i)
{
	Items[i].SetNewAnimation(Players[MyPlayerId].pLvlLoad == 0);
	Items[i]._iIdentified = false;
}

int RndItem(const MonsterStruct &monster)
{
	if ((monster.MData->mTreasure & 0x8000) != 0)
		return -((monster.MData->mTreasure & 0xFFF) + 1);

	if ((monster.MData->mTreasure & 0x4000) != 0)
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
	GetSuperItemSpace(position, ii);
	int curlv = ItemsGetCurrlevel();

	int idx = 0;
	while (AllItemsList[idx].iItemId != UniqueItemList[uid].UIItemId)
		idx++;

	GetItemAttrs(ii, idx, curlv);
	GetUniqueItem(Items[ii], uid);
	SetupItem(ii);
}

void SpawnItem(MonsterStruct &monster, Point position, bool sendmsg)
{
	int idx;
	bool onlygood = true;

	if (monster._uniqtype != 0 || ((monster.MData->mTreasure & 0x8000) != 0 && gbIsMultiplayer)) {
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
	GetSuperItemSpace(position, ii);
	int uper = monster._uniqtype != 0 ? 15 : 1;

	int8_t mLevel = monster.MData->mLevel;
	if (!gbIsHellfire && monster.MType->mtype == MT_DIABLO)
		mLevel -= 15;

	SetupAllItems(ii, idx, AdvanceRndSeed(), mLevel, uper, onlygood, false, false);

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
	GetSuperItemSpace(position, ii);
	int curlv = ItemsGetCurrlevel();

	SetupAllUseful(ii, AdvanceRndSeed(), curlv);
	if (sendmsg)
		NetSendCmdDItem(false, ii);
}

void CreateTypeItem(Point position, bool onlygood, int itype, int imisc, bool sendmsg, bool delta)
{
	int idx;

	int curlv = ItemsGetCurrlevel();
	if (itype != ITYPE_GOLD)
		idx = RndTypeItems(itype, imisc, curlv);
	else
		idx = IDI_GOLD;

	SetupBaseItem(position, idx, onlygood, sendmsg, delta);
}

void RecreateItem(int ii, int idx, uint16_t icreateinfo, int iseed, int ivalue, bool isHellfire)
{
	bool tmpIsHellfire = gbIsHellfire;
	gbIsHellfire = isHellfire;

	if (idx == IDI_GOLD) {
		SetPlrHandItem(&Items[ii], IDI_GOLD);
		Items[ii]._iSeed = iseed;
		Items[ii]._iCreateInfo = icreateinfo;
		Items[ii]._ivalue = ivalue;
		SetPlrHandGoldCurs(&Items[ii]);
		gbIsHellfire = tmpIsHellfire;
		return;
	}

	if (icreateinfo == 0) {
		SetPlrHandItem(&Items[ii], idx);
		SetPlrHandSeed(&Items[ii], iseed);
		gbIsHellfire = tmpIsHellfire;
		return;
	}

	if ((icreateinfo & CF_UNIQUE) == 0) {
		if ((icreateinfo & CF_TOWN) != 0) {
			RecreateTownItem(ii, idx, icreateinfo, iseed);
			gbIsHellfire = tmpIsHellfire;
			return;
		}

		if ((icreateinfo & CF_USEFUL) == CF_USEFUL) {
			SetupAllUseful(ii, iseed, icreateinfo & CF_LEVEL);
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

	SetupAllItems(ii, idx, iseed, level, uper, onlygood, recreate, pregen);
	gbIsHellfire = tmpIsHellfire;
}

void RecreateEar(int ii, uint16_t ic, int iseed, int id, int dur, int mdur, int ch, int mch, int ivalue, int ibuff)
{
	SetPlrHandItem(&Items[ii], IDI_EAR);
	tempstr[0] = (ic >> 8) & 0x7F;
	tempstr[1] = ic & 0x7F;
	tempstr[2] = (iseed >> 24) & 0x7F;
	tempstr[3] = (iseed >> 16) & 0x7F;
	tempstr[4] = (iseed >> 8) & 0x7F;
	tempstr[5] = iseed & 0x7F;
	tempstr[6] = id & 0x7F;
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
	strcpy(Items[ii]._iName, fmt::format(_(/* TRANSLATORS: {:s} will be a Character Name */ "Ear of {:s}"), tempstr).c_str());
	Items[ii]._iCurs = ((ivalue >> 6) & 3) + ICURS_EAR_SORCERER;
	Items[ii]._ivalue = ivalue & 0x3F;
	Items[ii]._iCreateInfo = ic;
	Items[ii]._iSeed = iseed;
}

void CornerstoneSave()
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

void CornerstoneLoad(Point position)
{
	PkItemStruct pkSItem;

	if (CornerStone.activated || position.x == 0 || position.y == 0) {
		return;
	}

	CornerStone.item._itype = ITYPE_NONE;
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

	if (strlen(sgOptions.Hellfire.szItem) < sizeof(PkItemStruct) * 2)
		return;

	Hex2bin(sgOptions.Hellfire.szItem, sizeof(PkItemStruct), (char *)&pkSItem);

	int ii = AllocateItem();

	dItem[position.x][position.y] = ii + 1;

	UnPackItem(&pkSItem, &Items[ii], (pkSItem.dwBuff & CF_HELLFIRE) != 0);
	Items[ii].position = position;
	RespawnItem(&Items[ii], false);
	CornerStone.item = Items[ii];
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

	Items[ii].position = position;

	dItem[position.x][position.y] = ii + 1;

	int curlv = ItemsGetCurrlevel();
	GetItemAttrs(ii, itemid, curlv);

	SetupItem(ii);
	Items[ii]._iSeed = AdvanceRndSeed();
	Items[ii]._iPostDraw = true;
	if (selflag != 0) {
		Items[ii]._iSelFlag = selflag;
		Items[ii].AnimInfo.CurrentFrame = Items[ii].AnimInfo.NumberOfFrames;
		Items[ii]._iAnimFlag = false;
	}
}

void SpawnRewardItem(int itemid, Point position)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();

	Items[ii].position = position;
	dItem[position.x][position.y] = ii + 1;
	int curlv = ItemsGetCurrlevel();
	GetItemAttrs(ii, itemid, curlv);
	Items[ii].SetNewAnimation(true);
	Items[ii]._iSelFlag = 2;
	Items[ii]._iPostDraw = true;
	Items[ii]._iIdentified = true;
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

void RespawnItem(ItemStruct *item, bool flipFlag)
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
		if (!Items[ii]._iAnimFlag)
			continue;
		Items[ii].AnimInfo.ProcessAnimation();
		if (Items[ii]._iCurs == ICURS_MAGIC_ROCK) {
			if (Items[ii]._iSelFlag == 1 && Items[ii].AnimInfo.CurrentFrame == 11)
				Items[ii].AnimInfo.CurrentFrame = 1;
			if (Items[ii]._iSelFlag == 2 && Items[ii].AnimInfo.CurrentFrame == 21)
				Items[ii].AnimInfo.CurrentFrame = 11;
		} else {
			if (Items[ii].AnimInfo.CurrentFrame == Items[ii].AnimInfo.NumberOfFrames / 2)
				PlaySfxLoc(ItemDropSnds[ItemCAnimTbl[Items[ii]._iCurs]], Items[ii].position);

			if (Items[ii].AnimInfo.CurrentFrame >= Items[ii].AnimInfo.NumberOfFrames) {
				Items[ii].AnimInfo.CurrentFrame = Items[ii].AnimInfo.NumberOfFrames;
				Items[ii]._iAnimFlag = false;
				Items[ii]._iSelFlag = 1;
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
	Items[i].AnimInfo.pCelSprite = &*itemanims[ItemCAnimTbl[Items[i]._iCurs]];
}

void GetItemStr(int i)
{
	if (Items[i]._itype != ITYPE_GOLD) {
		if (Items[i]._iIdentified)
			strcpy(infostr, Items[i]._iIName);
		else
			strcpy(infostr, Items[i]._iName);

		infoclr = Items[i].getTextColor();
	} else {
		int nGold = Items[i]._ivalue;
		strcpy(infostr, fmt::format(ngettext("{:d} gold piece", "{:d} gold pieces", nGold), nGold).c_str());
	}
}

void CheckIdentify(int pnum, int cii)
{
	ItemStruct *pi;

	if (cii >= NUM_INVLOC)
		pi = &Players[pnum].InvList[cii - NUM_INVLOC];
	else
		pi = &Players[pnum].InvBody[cii];

	pi->_iIdentified = true;
	CalcPlrInv(pnum, true);

	if (pnum == MyPlayerId)
		NewCursor(CURSOR_HAND);
}

void DoRepair(int pnum, int cii)
{
	ItemStruct *pi;

	auto &player = Players[pnum];

	PlaySfxLoc(IS_REPAIR, player.position.tile);

	if (cii >= NUM_INVLOC) {
		pi = &player.InvList[cii - NUM_INVLOC];
	} else {
		pi = &player.InvBody[cii];
	}

	RepairItem(pi, player._pLevel);
	CalcPlrInv(pnum, true);

	if (pnum == MyPlayerId)
		NewCursor(CURSOR_HAND);
}

void DoRecharge(int pnum, int cii)
{
	ItemStruct *pi;

	auto &player = Players[pnum];
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

	if (pnum == MyPlayerId)
		NewCursor(CURSOR_HAND);
}

void DoOil(int pnum, int cii)
{
	if (cii < NUM_INVLOC && cii != INVLOC_HEAD && (cii <= INVLOC_AMULET || cii > INVLOC_CHEST))
		return;
	auto &player = Players[pnum];
	if (!ApplyOilToItem(&player.InvBody[cii], player))
		return;
	CalcPlrInv(pnum, true);
	if (pnum == MyPlayerId) {
		NewCursor(CURSOR_HAND);
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

void DrawUniqueInfo(const Surface &out)
{
	if ((chrflag || QuestLogIsOpen) && gnScreenWidth < SPANEL_WIDTH * 3) {
		return;
	}

	DrawUniqueInfoWindow(GlobalBackBuffer());

	Rectangle rect { { 32 + RIGHT_PANEL - SPANEL_WIDTH, 44 + 2 * 12 }, { 257, 0 } };
	const UItemStruct &uitem = UniqueItemList[curruitem._iUid];
	DrawString(out, _(uitem.UIName), rect, UIS_CENTER);

	DrawUniqueInfoDevider(out, 5);

	rect.position.y += (10 - uitem.UINumPL) * 12;
	assert(uitem.UINumPL <= sizeof(uitem.powers) / sizeof(*uitem.powers));
	for (const auto &power : uitem.powers) {
		if (power.type == IPL_INVALID)
			break;
		rect.position.y += 2 * 12;
		PrintItemPower(power.type, &curruitem);
		DrawString(out, tempstr, rect, UIS_SILVER | UIS_CENTER);
	}
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
		ShowUniqueItemInfoBox = true;
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

void UseItem(int p, item_misc_id mid, spell_id spl)
{
	auto &player = Players[p];

	switch (mid) {
	case IMISC_HEAL:
	case IMISC_FOOD: {
		int j = player._pMaxHP >> 8;
		int l = ((j / 2) + GenerateRnd(j)) << 6;
		if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Barbarian)
			l *= 2;
		if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk || player._pClass == HeroClass::Bard)
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
		if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk || player._pClass == HeroClass::Bard)
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
			player.destParam1 = cursmx;
			player.destParam2 = cursmy;
			if (p == MyPlayerId && spl == SPL_NOVA)
				NetSendCmdLoc(MyPlayerId, true, CMD_NOVA, { cursmx, cursmy });
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
			player.destParam1 = cursmx;
			player.destParam2 = cursmy;
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

	ItemStruct holditem;
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
			int idata = RndSmithItem(lvl) - 1;
			GetItemAttrs(0, idata, lvl);
		} while (Items[0]._iIvalue > maxValue);
		smithitem[i] = Items[0];
		smithitem[i]._iCreateInfo = lvl | CF_SMITH;
		smithitem[i]._iIdentified = true;
		smithitem[i]._iStatFlag = StoreStatOk(&smithitem[i]);
	}
	for (int i = iCnt; i < SMITH_ITEMS; i++)
		smithitem[i]._itype = ITYPE_NONE;

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
	GetItemAttrs(0, IDI_MANA, 1);
	witchitem[0] = Items[0];
	witchitem[0]._iCreateInfo = lvl;
	witchitem[0]._iStatFlag = true;
	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(0, IDI_FULLMANA, 1);
	witchitem[1] = Items[0];
	witchitem[1]._iCreateInfo = lvl;
	witchitem[1]._iStatFlag = true;
	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(0, IDI_PORTAL, 1);
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
			AdvanceRndSeed();

			GetItemAttrs(0, i, lvl);
			witchitem[j] = Items[0];
			witchitem[j]._iCreateInfo = lvl | CF_WITCH;
			witchitem[j]._iIdentified = true;
			WitchBookLevel(j);
			witchitem[j]._iStatFlag = StoreStatOk(&witchitem[j]);
			j++;
			bCnt++;
		}
	}
	int iCnt = GenerateRnd(WITCH_ITEMS - reservedItems) + 10;

	for (int i = j; i < iCnt; i++) {
		do {
			memset(&Items[0], 0, sizeof(*Items));
			Items[0]._iSeed = AdvanceRndSeed();
			int idata = RndWitchItem(lvl) - 1;
			GetItemAttrs(0, idata, lvl);
			int maxlvl = -1;
			if (GenerateRnd(100) <= 5)
				maxlvl = 2 * lvl;
			if (maxlvl == -1 && Items[0]._iMiscId == IMISC_STAFF)
				maxlvl = 2 * lvl;
			if (maxlvl != -1)
				GetItemBonus(0, maxlvl / 2, maxlvl, true, true);
		} while (Items[0]._iIvalue > maxValue);
		witchitem[i] = Items[0];
		witchitem[i]._iCreateInfo = lvl | CF_WITCH;
		witchitem[i]._iIdentified = true;
		WitchBookLevel(i);
		witchitem[i]._iStatFlag = StoreStatOk(&witchitem[i]);
	}

	for (int i = iCnt; i < WITCH_ITEMS; i++)
		witchitem[i]._itype = ITYPE_NONE;

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
		int itype = RndBoyItem(lvl) - 1;
		GetItemAttrs(0, itype, lvl);
		GetItemBonus(0, lvl, 2 * lvl, true, true);

		if (!gbIsHellfire) {
			if (Items[0]._iIvalue > 90000) {
				keepgoing = true; // prevent breaking the do/while loop too early by failing hellfire's condition in while
				continue;
			}
			break;
		}

		ivalue = 0;

		int itemType = Items[0]._itype;

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
		ivalue = ivalue * 4 / 5; // avoids forced int > float > int conversion

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
	            Items[0]._iIvalue > 200000
	            || Items[0]._iMinStr > strength
	            || Items[0]._iMinMag > magic
	            || Items[0]._iMinDex > dexterity
	            || Items[0]._iIvalue < ivalue)
	        && count < 250));
	boyitem = Items[0];
	boyitem._iCreateInfo = lvl | CF_BOY;
	boyitem._iIdentified = true;
	boyitem._iStatFlag = StoreStatOk(&boyitem);
	boylevel = lvl / 2;
}

void SpawnHealer(int lvl)
{
	constexpr int PinnedItemCount = 2;

	int srnd;

	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(0, IDI_HEAL, 1);
	healitem[0] = Items[0];
	healitem[0]._iCreateInfo = lvl;
	healitem[0]._iStatFlag = true;

	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(0, IDI_FULLHEAL, 1);
	healitem[1] = Items[0];
	healitem[1]._iCreateInfo = lvl;
	healitem[1]._iStatFlag = true;

	if (gbIsMultiplayer) {
		memset(&Items[0], 0, sizeof(*Items));
		GetItemAttrs(0, IDI_RESURRECT, 1);
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
		int itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(0, itype, lvl);
		healitem[i] = Items[0];
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
	memset(&Items[0], 0, sizeof(*Items));
	GetItemAttrs(0, IDI_GOLD, 1);
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

	int idx = RndTypeItems(ITYPE_MISC, IMISC_BOOK, lvl);
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();

	while (true) {
		memset(&Items[ii], 0, sizeof(*Items));
		SetupAllItems(ii, idx, AdvanceRndSeed(), 2 * lvl, 1, true, false, delta);
		if (Items[ii]._iMiscId == IMISC_BOOK && Items[ii]._iSpell == ispell)
			break;
	}
	GetSuperItemSpace(position, ii);

	if (sendmsg)
		NetSendCmdDItem(false, ii);
	if (delta)
		DeltaAddItem(ii);
}

void CreateMagicArmor(Point position, int imisc, int icurs, bool sendmsg, bool delta)
{
	int lvl = ItemsGetCurrlevel();
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

	int curlv = ItemsGetCurrlevel();

	CreateMagicItem(position, curlv, imisc, imid, icurs, sendmsg, delta);
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
