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

#include "DiabloUI/ui_flags.hpp"
#include "controls/plrctrls.h"
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
#include "panels/info_box.hpp"
#include "panels/ui_panels.hpp"
#include "player.h"
#include "qol/stash.h"
#include "spells.h"
#include "stores.h"
#include "town.h"
#include "utils/language.h"
#include "utils/math.h"
#include "utils/stdcompat/algorithm.hpp"
#include "utils/utf8.hpp"

namespace devilution {

Item Items[MAXITEMS + 1];
uint8_t ActiveItems[MAXITEMS];
uint8_t ActiveItemCount;
int8_t dItem[MAXDUNX][MAXDUNY];
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

std::optional<OwnedCelSprite> itemanims[ITEMTYPES];

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

bool IsPrefixValidForItemType(int i, AffixItemType flgs)
{
	AffixItemType itemTypes = ItemPrefixes[i].PLIType;

	if (!gbIsHellfire) {
		if (i > 82)
			return false;

		if (i >= 12 && i <= 20)
			itemTypes &= ~AffixItemType::Staff;
	}

	return HasAnyOf(flgs, itemTypes);
}

bool IsSuffixValidForItemType(int i, AffixItemType flgs)
{
	AffixItemType itemTypes = ItemSuffixes[i].PLIType;

	if (!gbIsHellfire) {
		if (i > 94)
			return false;

		if ((i >= 0 && i <= 1)
		    || (i >= 14 && i <= 15)
		    || (i >= 21 && i <= 22)
		    || (i >= 34 && i <= 36)
		    || (i >= 41 && i <= 44)
		    || (i >= 60 && i <= 63))
			itemTypes &= ~AffixItemType::Staff;
	}

	return HasAnyOf(flgs, itemTypes);
}

int ItemsGetCurrlevel()
{
	if (leveltype == DTYPE_NEST)
		return currlevel - 8;

	if (leveltype == DTYPE_CRYPT)
		return currlevel - 7;

	return currlevel;
}

bool ItemPlace(Point position)
{
	if (dMonster[position.x][position.y] != 0)
		return false;
	if (dPlayer[position.x][position.y] != 0)
		return false;
	if (dItem[position.x][position.y] != 0)
		return false;
	if (IsObjectAtPosition(position))
		return false;
	if (TileContainsSetPiece(position))
		return false;
	if (IsTileSolid(position))
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
		item.AnimInfo.CurrentFrame = item.AnimInfo.NumberOfFrames - 1;
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

	// first iteration is used for collecting stat bonuses from items
	for (Item &equipment : EquippedPlayerItemsRange(player)) {
		equipment._iStatFlag = true;
		if (equipment._iIdentified) {
			sa += equipment._iPLStr;
			ma += equipment._iPLMag;
			da += equipment._iPLDex;
		}
	}

	bool changeflag;
	do {
		// cap stats to 0
		const int currstr = std::max(0, sa + player._pBaseStr);
		const int currmag = std::max(0, ma + player._pBaseMag);
		const int currdex = std::max(0, da + player._pBaseDex);

		changeflag = false;
		for (Item &equipment : EquippedPlayerItemsRange(player)) {
			if (!equipment._iStatFlag)
				continue;

			if (currstr >= equipment._iMinStr
			    && currmag >= equipment._iMinMag
			    && currdex >= equipment._iMinDex)
				continue;

			changeflag = true;
			equipment._iStatFlag = false;
			if (equipment._iIdentified) {
				sa -= equipment._iPLStr;
				ma -= equipment._iPLMag;
				da -= equipment._iPLDex;
			}
		}
	} while (changeflag);
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
	std::string spellName = pgettext("spell", spelldata[bs].sNameText);
	CopyUtf8(item._iName, std::string(item._iName + spellName), sizeof(item._iIName));
	CopyUtf8(item._iIName, std::string(item._iIName + spellName), sizeof(item._iIName));
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

int SaveItemPower(Item &item, ItemPower &power)
{
	if (!gbIsHellfire) {
		if (power.type == IPL_TARGAC) {
			power.param1 = 1 << power.param1;
			power.param2 = 3 << power.param2;
		}
	}

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
		item._iDamAcFlags |= ItemSpecialEffectHf::Doppelganger;
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
		item._iFlags |= ItemSpecialEffect::FireDamage;
		item._iFlags &= ~ItemSpecialEffect::LightningDamage;
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 0;
		item._iLMaxDam = 0;
		break;
	case IPL_LIGHTDAM:
		item._iFlags |= ItemSpecialEffect::LightningDamage;
		item._iFlags &= ~ItemSpecialEffect::FireDamage;
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
		item._iFlags |= ItemSpecialEffect::MultipleArrows;
		break;
	case IPL_FIRE_ARROWS:
		item._iFlags |= ItemSpecialEffect::FireArrows;
		item._iFlags &= ~ItemSpecialEffect::LightningArrows;
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 0;
		item._iLMaxDam = 0;
		break;
	case IPL_LIGHT_ARROWS:
		item._iFlags |= ItemSpecialEffect::LightningArrows;
		item._iFlags &= ~ItemSpecialEffect::FireArrows;
		item._iLMinDam = power.param1;
		item._iLMaxDam = power.param2;
		item._iFMinDam = 0;
		item._iFMaxDam = 0;
		break;
	case IPL_FIREBALL:
		item._iFlags |= (ItemSpecialEffect::LightningArrows | ItemSpecialEffect::FireArrows);
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 0;
		item._iLMaxDam = 0;
		break;
	case IPL_THORNS:
		item._iFlags |= ItemSpecialEffect::Thorns;
		break;
	case IPL_NOMANA:
		item._iFlags |= ItemSpecialEffect::NoMana;
		drawmanaflag = true;
		break;
	case IPL_NOHEALPLR:
		item._iFlags |= ItemSpecialEffect::NoHealOnPlayer;
		break;
	case IPL_ABSHALFTRAP:
		item._iFlags |= ItemSpecialEffect::HalfTrapDamage;
		break;
	case IPL_KNOCKBACK:
		item._iFlags |= ItemSpecialEffect::Knockback;
		break;
	case IPL_3XDAMVDEM:
		item._iFlags |= ItemSpecialEffect::TripleDemonDamage;
		break;
	case IPL_ALLRESZERO:
		item._iFlags |= ItemSpecialEffect::ZeroResistance;
		break;
	case IPL_NOHEALMON:
		item._iFlags |= ItemSpecialEffect::NoHealOnMonsters;
		break;
	case IPL_STEALMANA:
		if (power.param1 == 3)
			item._iFlags |= ItemSpecialEffect::StealMana3;
		if (power.param1 == 5)
			item._iFlags |= ItemSpecialEffect::StealMana5;
		drawmanaflag = true;
		break;
	case IPL_STEALLIFE:
		if (power.param1 == 3)
			item._iFlags |= ItemSpecialEffect::StealLife3;
		if (power.param1 == 5)
			item._iFlags |= ItemSpecialEffect::StealLife5;
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
			item._iFlags |= ItemSpecialEffect::QuickAttack;
		if (power.param1 == 2)
			item._iFlags |= ItemSpecialEffect::FastAttack;
		if (power.param1 == 3)
			item._iFlags |= ItemSpecialEffect::FasterAttack;
		if (power.param1 == 4)
			item._iFlags |= ItemSpecialEffect::FastestAttack;
		break;
	case IPL_FASTRECOVER:
		if (power.param1 == 1)
			item._iFlags |= ItemSpecialEffect::FastHitRecovery;
		if (power.param1 == 2)
			item._iFlags |= ItemSpecialEffect::FasterHitRecovery;
		if (power.param1 == 3)
			item._iFlags |= ItemSpecialEffect::FastestHitRecovery;
		break;
	case IPL_FASTBLOCK:
		item._iFlags |= ItemSpecialEffect::FastBlock;
		break;
	case IPL_DAMMOD:
		item._iPLDamMod += r;
		break;
	case IPL_RNDARROWVEL:
		item._iFlags |= ItemSpecialEffect::RandomArrowVelocity;
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
		item._iFlags |= ItemSpecialEffect::FasterAttack;
		break;
	case IPL_ONEHAND:
		item._iLoc = ILOC_ONEHAND;
		break;
	case IPL_DRAINLIFE:
		item._iFlags |= ItemSpecialEffect::DrainLife;
		break;
	case IPL_RNDSTEALLIFE:
		item._iFlags |= ItemSpecialEffect::RandomStealLife;
		break;
	case IPL_INFRAVISION:
		item._iFlags |= ItemSpecialEffect::Infravision;
		break;
	case IPL_NOMINSTR:
		item._iMinStr = 0;
		break;
	case IPL_INVCURS:
		item._iCurs = power.param1;
		break;
	case IPL_ADDACLIFE:
		item._iFlags |= (ItemSpecialEffect::LightningArrows | ItemSpecialEffect::FireArrows);
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 1;
		item._iLMaxDam = 0;
		break;
	case IPL_ADDMANAAC:
		item._iFlags |= (ItemSpecialEffect::LightningDamage | ItemSpecialEffect::FireDamage);
		item._iFMinDam = power.param1;
		item._iFMaxDam = power.param2;
		item._iLMinDam = 2;
		item._iLMaxDam = 0;
		break;
	case IPL_FIRERESCLVL:
		item._iPLFR = 30 - MyPlayer->_pLevel;
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
		item._iDamAcFlags |= ItemSpecialEffectHf::Devastation;
		break;
	case IPL_DECAY:
		item._iDamAcFlags |= ItemSpecialEffectHf::Decay;
		item._iPLDam += r;
		break;
	case IPL_PERIL:
		item._iDamAcFlags |= ItemSpecialEffectHf::Peril;
		break;
	case IPL_JESTERS:
		item._iDamAcFlags |= ItemSpecialEffectHf::Jesters;
		break;
	case IPL_ACDEMON:
		item._iDamAcFlags |= ItemSpecialEffectHf::ACAgainstDemons;
		break;
	case IPL_ACUNDEAD:
		item._iDamAcFlags |= ItemSpecialEffectHf::ACAgainstUndead;
		break;
	case IPL_MANATOLIFE: {
		int portion = ((MyPlayer->_pMaxManaBase >> 6) * 50 / 100) << 6;
		item._iPLMana -= portion;
		item._iPLHP += portion;
	} break;
	case IPL_LIFETOMANA: {
		int portion = ((MyPlayer->_pMaxHPBase >> 6) * 40 / 100) << 6;
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
			if (!IsPrefixValidForItemType(j, AffixItemType::Staff) || ItemPrefixes[j].PLMinLvl > lvl)
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
			item._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemAffix(item, ItemPrefixes[preidx]);
			item._iPrePower = ItemPrefixes[preidx].power.type;
		}
	}

	string_view baseName = _(AllItemsList[item.IDidx].iName);
	string_view shortName = _(AllItemsList[item.IDidx].iSName);
	string_view spellName = pgettext("spell", spelldata[bs].sNameText);
	string_view normalFmt = pgettext("spell", /* TRANSLATORS: Constructs item names. Format: {Item} of {Spell}. Example: War Staff of Firewall */ "{0} of {1}");

	CopyUtf8(item._iName, fmt::format(normalFmt, baseName, spellName), sizeof(item._iName));
	if (!StringInPanel(item._iName)) {
		CopyUtf8(item._iName, fmt::format(normalFmt, shortName, spellName), sizeof(item._iName));
	}

	if (preidx != -1) {
		string_view magicFmt = pgettext("spell", /* TRANSLATORS: Constructs item names. Format: {Prefix} {Item} of {Spell}. Example: King's War Staff of Firewall */ "{0} {1} of {2}");
		string_view prefixName = _(ItemPrefixes[preidx].PLName);
		CopyUtf8(item._iIName, fmt::format(magicFmt, prefixName, baseName, spellName), sizeof(item._iIName));
		if (!StringInPanel(item._iIName)) {
			CopyUtf8(item._iIName, fmt::format(magicFmt, prefixName, shortName, spellName), sizeof(item._iIName));
		}
	} else {
		CopyUtf8(item._iIName, item._iName, sizeof(item._iIName));
	}

	CalcItemValue(item);
}

namespace {

std::string GenerateMagicItemName(const string_view &baseNamel, int preidx, int sufidx)
{
	if (preidx != -1 && sufidx != -1) {
		string_view fmt = _(/* TRANSLATORS: Constructs item names. Format: {Prefix} {Item} of {Suffix}. Example: King's Long Sword of the Whale */ "{0} {1} of {2}");
		return fmt::format(fmt, _(ItemPrefixes[preidx].PLName), baseNamel, _(ItemSuffixes[sufidx].PLName));
	} else if (preidx != -1) {
		string_view fmt = _(/* TRANSLATORS: Constructs item names. Format: {Prefix} {Item}. Example: King's Long Sword */ "{0} {1}");
		return fmt::format(fmt, _(ItemPrefixes[preidx].PLName), baseNamel);
	} else if (sufidx != -1) {
		string_view fmt = _(/* TRANSLATORS: Constructs item names. Format: {Item} of {Suffix}. Example: Long Sword of the Whale */ "{0} of {1}");
		return fmt::format(fmt, baseNamel, _(ItemSuffixes[sufidx].PLName));
	}

	return std::string(baseNamel);
}

} // namespace

void GetItemPower(Item &item, int minlvl, int maxlvl, AffixItemType flgs, bool onlygood)
{
	int l[256];
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
			if (HasAnyOf(flgs, AffixItemType::Staff) && ItemPrefixes[j].power.type == IPL_CHARGES)
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
			item._iMagical = ITEM_QUALITY_MAGIC;
			SaveItemAffix(item, ItemSuffixes[sufidx]);
			item._iSufPower = ItemSuffixes[sufidx].power.type;
		}
	}

	CopyUtf8(item._iIName, GenerateMagicItemName(item._iName, preidx, sufidx), sizeof(item._iIName));
	if (!StringInPanel(item._iIName)) {
		CopyUtf8(item._iIName, GenerateMagicItemName(_(AllItemsList[item.IDidx].iSName), preidx, sufidx), sizeof(item._iIName));
	}
	if (preidx != -1 || sufidx != -1)
		CalcItemValue(item);
}

void GetStaffSpell(Item &item, int lvl, bool onlygood)
{
	if (!gbIsHellfire && GenerateRnd(4) == 0) {
		GetItemPower(item, lvl / 2, lvl, AffixItemType::Staff, onlygood);
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

	CopyUtf8(item._iName, _(OilNames[t]), sizeof(item._iName));
	CopyUtf8(item._iIName, _(OilNames[t]), sizeof(item._iIName));
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
		GetItemPower(item, minlvl, maxlvl, AffixItemType::Weapon, onlygood);
		break;
	case ItemType::Bow:
		GetItemPower(item, minlvl, maxlvl, AffixItemType::Bow, onlygood);
		break;
	case ItemType::Shield:
		GetItemPower(item, minlvl, maxlvl, AffixItemType::Shield, onlygood);
		break;
	case ItemType::LightArmor:
	case ItemType::Helm:
	case ItemType::MediumArmor:
	case ItemType::HeavyArmor:
		GetItemPower(item, minlvl, maxlvl, AffixItemType::Armor, onlygood);
		break;
	case ItemType::Staff:
		if (allowspells)
			GetStaffSpell(item, maxlvl, onlygood);
		else
			GetItemPower(item, minlvl, maxlvl, AffixItemType::Staff, onlygood);
		break;
	case ItemType::Ring:
	case ItemType::Amulet:
		GetItemPower(item, minlvl, maxlvl, AffixItemType::Misc, onlygood);
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

	for (auto power : UniqueItems[uid].powers) {
		if (power.type == IPL_INVALID)
			break;
		SaveItemPower(item, power);
	}

	CopyUtf8(item._iIName, _(UniqueItems[uid].UIName), sizeof(item._iIName));
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
		NetSendCmdPItem(false, CMD_DROPITEM, item.position, item);
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
	item.AnimInfo.CurrentFrame = 10;
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

void PrintItemOil(char iDidx)
{
	switch (iDidx) {
	case IMISC_OILACC:
		AddPanelString(_("increases a weapon's"));
		AddPanelString(_("chance to hit"));
		break;
	case IMISC_OILMAST:
		AddPanelString(_("greatly increases a"));
		AddPanelString(_("weapon's chance to hit"));
		break;
	case IMISC_OILSHARP:
		AddPanelString(_("increases a weapon's"));
		AddPanelString(_("damage potential"));
		break;
	case IMISC_OILDEATH:
		AddPanelString(_("greatly increases a weapon's"));
		AddPanelString(_("damage potential - not bows"));
		break;
	case IMISC_OILSKILL:
		AddPanelString(_("reduces attributes needed"));
		AddPanelString(_("to use armor or weapons"));
		break;
	case IMISC_OILBSMTH:
		AddPanelString(/*xgettext:no-c-format*/ _("restores 20% of an"));
		AddPanelString(_("item's durability"));
		break;
	case IMISC_OILFORT:
		AddPanelString(_("increases an item's"));
		AddPanelString(_("current and max durability"));
		break;
	case IMISC_OILPERM:
		AddPanelString(_("makes an item indestructible"));
		break;
	case IMISC_OILHARD:
		AddPanelString(_("increases the armor class"));
		AddPanelString(_("of armor and shields"));
		break;
	case IMISC_OILIMP:
		AddPanelString(_("greatly increases the armor"));
		AddPanelString(_("class of armor and shields"));
		break;
	case IMISC_RUNEF:
		AddPanelString(_("sets fire trap"));
		break;
	case IMISC_RUNEL:
	case IMISC_GR_RUNEL:
		AddPanelString(_("sets lightning trap"));
		break;
	case IMISC_GR_RUNEF:
		AddPanelString(_("sets fire trap"));
		break;
	case IMISC_RUNES:
		AddPanelString(_("sets petrification trap"));
		break;
	case IMISC_FULLHEAL:
		AddPanelString(_("restore all life"));
		break;
	case IMISC_HEAL:
		AddPanelString(_("restore some life"));
		break;
	case IMISC_OLDHEAL:
		AddPanelString(_("recover life"));
		break;
	case IMISC_DEADHEAL:
		AddPanelString(_("deadly heal"));
		break;
	case IMISC_MANA:
		AddPanelString(_("restore some mana"));
		break;
	case IMISC_FULLMANA:
		AddPanelString(_("restore all mana"));
		break;
	case IMISC_ELIXSTR:
		AddPanelString(_("increase strength"));
		break;
	case IMISC_ELIXMAG:
		AddPanelString(_("increase magic"));
		break;
	case IMISC_ELIXDEX:
		AddPanelString(_("increase dexterity"));
		break;
	case IMISC_ELIXVIT:
		AddPanelString(_("increase vitality"));
		break;
	case IMISC_ELIXWEAK:
	case IMISC_ELIXDIS:
		AddPanelString(_("decrease strength"));
		break;
	case IMISC_ELIXCLUM:
		AddPanelString(_("decrease dexterity"));
		break;
	case IMISC_ELIXSICK:
		AddPanelString(_("decrease vitality"));
		break;
	case IMISC_REJUV:
		AddPanelString(_("restore some life and mana"));
		break;
	case IMISC_FULLREJUV:
		AddPanelString(_("restore all life and mana"));
		break;
	}
}

void DrawUniqueInfoWindow(const Surface &out)
{
	CelDrawTo(out, GetPanelPosition(UiPanels::Inventory, { 24 - SPANEL_WIDTH, 327 }), *pSTextBoxCels, 0);
	DrawHalfTransparentRectTo(out, GetRightPanel().position.x - SPANEL_WIDTH + 27, GetRightPanel().position.y + 28, 265, 297);
}

void PrintItemMisc(const Item &item)
{
	if (item._iMiscId == IMISC_SCROLL) {
		if (ControlMode == ControlTypes::KeyboardAndMouse) {
			AddPanelString(_("Right-click to read"));
		} else {
			if (!invflag) {
				AddPanelString(_("Open inventory to use"));
			} else {
				AddPanelString(_("Activate to read"));
			}
		}
	}
	if (item._iMiscId == IMISC_SCROLLT) {
		if (ControlMode == ControlTypes::KeyboardAndMouse) {
			AddPanelString(_("Right-click to read, then"));
			AddPanelString(_("left-click to target"));
		} else {
			if (TargetsMonster(item._iSpell)) {
				AddPanelString(_("Select from spell book, then"));
				AddPanelString(_("cast spell to read"));
			} else if (!invflag) {
				AddPanelString(_("Open inventory to use"));
			} else {
				AddPanelString(_("Activate to read"));
			}
		}
	}
	if ((item._iMiscId >= IMISC_USEFIRST && item._iMiscId <= IMISC_USELAST)
	    || (item._iMiscId > IMISC_OILFIRST && item._iMiscId < IMISC_OILLAST)
	    || (item._iMiscId > IMISC_RUNEFIRST && item._iMiscId < IMISC_RUNELAST)) {
		PrintItemOil(item._iMiscId);
		if (ControlMode == ControlTypes::KeyboardAndMouse) {
			AddPanelString(_("Right-click to use"));
		} else {
			if (!invflag) {
				AddPanelString(_("Open inventory to use"));
			} else {
				AddPanelString(_("Activate to use"));
			}
		}
	}
	if (IsAnyOf(item._iMiscId, IMISC_BOOK, IMISC_NOTE)) {
		if (ControlMode == ControlTypes::KeyboardAndMouse) {
			AddPanelString(_("Right-click to read"));
		} else {
			AddPanelString(_("Activate to read"));
		}
	}
	if (item._iMiscId == IMISC_MAPOFDOOM) {
		if (ControlMode == ControlTypes::KeyboardAndMouse) {
			AddPanelString(_("Right-click to view"));
		} else {
			AddPanelString(_("Activate to view"));
		}
	}
	if (item._iMiscId == IMISC_EAR) {
		AddPanelString(fmt::format(pgettext("player", "Level: {:d}"), item._ivalue));
	}
	if (item._iMiscId == IMISC_AURIC) {
		AddPanelString(_("Doubles gold capacity"));
	}
}

void PrintItemInfo(const Item &item)
{
	PrintItemMisc(item);
	uint8_t str = item._iMinStr;
	uint8_t dex = item._iMinDex;
	uint8_t mag = item._iMinMag;
	if (str != 0 || mag != 0 || dex != 0) {
		std::string text = _("Required:");
		if (str != 0)
			text.append(fmt::format(_(" {:d} Str"), str));
		if (mag != 0)
			text.append(fmt::format(_(" {:d} Mag"), mag));
		if (dex != 0)
			text.append(fmt::format(_(" {:d} Dex"), dex));
		AddPanelString(text);
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

void SpawnOnePremium(Item &premiumItem, int plvl, int playerId)
{
	int itemValue = 0;
	bool keepGoing = false;

	Player &player = Players[playerId];

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
		premiumItem = {};
		premiumItem._iSeed = AdvanceRndSeed();
		SetRndSeed(premiumItem._iSeed);
		int itemType = RndPremiumItem(plvl / 4, plvl) - 1;
		GetItemAttrs(premiumItem, itemType, plvl);
		GetItemBonus(premiumItem, plvl / 2, plvl, true, !gbIsHellfire);

		if (!gbIsHellfire) {
			if (premiumItem._iIvalue > 140000) {
				keepGoing = true; // prevent breaking the do/while loop too early by failing hellfire's condition in while
				continue;
			}
			break;
		}

		switch (premiumItem._itype) {
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
			    [filterType = premiumItem._itype](const Item &item) { return item._itype == filterType; });

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
	            premiumItem._iIvalue > 200000
	            || premiumItem._iMinStr > strength
	            || premiumItem._iMinMag > magic
	            || premiumItem._iMinDex > dexterity
	            || premiumItem._iIvalue < itemValue)
	        && count < 150));
	premiumItem._iCreateInfo = plvl | CF_SMITHPREMIUM;
	premiumItem._iIdentified = true;
	premiumItem._iStatFlag = player.CanUseItem(premiumItem);
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
		Player &myPlayer = *MyPlayer;

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

void CreateMagicItem(Point position, int lvl, ItemType itemType, int imid, int icurs, bool sendmsg, bool delta)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];
	int idx = RndTypeItems(itemType, imid, lvl);

	while (true) {
		item = {};
		SetupAllItems(item, idx, AdvanceRndSeed(), 2 * lvl, 1, true, false, delta);
		if (item._iCurs == icurs)
			break;

		idx = RndTypeItems(itemType, imid, lvl);
	}
	GetSuperItemSpace(position, ii);

	if (sendmsg)
		NetSendCmdPItem(false, CMD_DROPITEM, item.position, item);
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
	        *sgOptions.Gameplay.testBard && IsAnyOf(i, IDI_BARDSWORD, IDI_BARDDAGGER));
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
	ActiveItemCount = 0;
	memset(dItem, 0, sizeof(dItem));

	for (auto &item : Items) {
		item.clear();
		item.position = { 0, 0 };
		item._iAnimFlag = false;
		item._iSelFlag = 0;
		item._iIdentified = false;
		item._iPostDraw = false;
	}

	for (uint8_t i = 0; i < MAXITEMS; i++) {
		ActiveItems[i] = i;
	}

	if (!setlevel) {
		AdvanceRndSeed(); /* unused */
		if (Quests[Q_ROCK].IsAvailable())
			SpawnRock();
		if (Quests[Q_ANVIL].IsAvailable())
			SpawnQuestItem(IDI_ANVIL, SetPiece.position.megaToWorld() + Displacement { 11, 11 }, 0, 1);
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

	ItemSpecialEffect iflgs = ItemSpecialEffect::None; // item_special_effect flags

	ItemSpecialEffectHf pDamAcFlags = ItemSpecialEffectHf::None;

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

	if (HasAnyOf(player._pSpellFlags, SpellFlag::RageActive)) {
		sadd += 2 * player._pLevel;
		dadd += player._pLevel + player._pLevel / 2;
		vadd += 2 * player._pLevel;
	}
	if (HasAnyOf(player._pSpellFlags, SpellFlag::RageCooldown)) {
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

	if (player._pLightRad != lrad) {
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

	if (HasAnyOf(player._pSpellFlags, SpellFlag::RageCooldown)) {
		mr -= player._pLevel;
		fr -= player._pLevel;
		lr -= player._pLevel;
	}

	if (HasAnyOf(iflgs, ItemSpecialEffect::ZeroResistance)) {
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

	if (&player == MyPlayer && (player._pHitPoints >> 6) <= 0) {
		SetPlayerHitPoints(player, 0);
	}

	player._pMaxMana = imana + player._pMaxManaBase;
	player._pMana = std::min(imana + player._pManaBase, player._pMaxMana);

	player._pIFMinDam = fmin;
	player._pIFMaxDam = fmax;
	player._pILMinDam = lmin;
	player._pILMaxDam = lmax;

	player._pInfraFlag = HasAnyOf(iflgs, ItemSpecialEffect::Infravision);

	player._pBlockFlag = false;
	if (player._pClass == HeroClass::Monk) {
		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ItemType::Staff && player.InvBody[INVLOC_HAND_LEFT]._iStatFlag) {
			player._pBlockFlag = true;
			player._pIFlags |= ItemSpecialEffect::FastBlock;
		}
		if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ItemType::Staff && player.InvBody[INVLOC_HAND_RIGHT]._iStatFlag) {
			player._pBlockFlag = true;
			player._pIFlags |= ItemSpecialEffect::FastBlock;
		}
		if (player.InvBody[INVLOC_HAND_LEFT].isEmpty() && player.InvBody[INVLOC_HAND_RIGHT].isEmpty())
			player._pBlockFlag = true;
		if (player.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON && player.GetItemLocation(player.InvBody[INVLOC_HAND_LEFT]) != ILOC_TWOHAND && player.InvBody[INVLOC_HAND_RIGHT].isEmpty())
			player._pBlockFlag = true;
		if (player.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON && player.GetItemLocation(player.InvBody[INVLOC_HAND_RIGHT]) != ILOC_TWOHAND && player.InvBody[INVLOC_HAND_LEFT].isEmpty())
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
		player.previewCelSprite = std::nullopt;
		if (player._pmode == PM_STAND) {
			LoadPlrGFX(player, player_graphic::Stand);
			player.AnimInfo.ChangeAnimationData(player.AnimationData[static_cast<size_t>(player_graphic::Stand)].GetCelSpritesForDirection(player._pdir), player._pNFrames, 4);
		} else {
			LoadPlrGFX(player, player_graphic::Walk);
			player.AnimInfo.ChangeAnimationData(player.AnimationData[static_cast<size_t>(player_graphic::Walk)].GetCelSpritesForDirection(player._pdir), player._pWFrames, 1);
		}
	} else {
		player._pgfxnum = gfxNum;
	}

	if (&player == MyPlayer) {
		if (player.InvBody[INVLOC_AMULET].isEmpty() || player.InvBody[INVLOC_AMULET].IDidx != IDI_AURIC) {
			int half = MaxGold;
			MaxGold = GOLD_MAX_LIMIT;

			if (half != MaxGold)
				StripTopGold(player);
		} else {
			MaxGold = GOLD_MAX_LIMIT * 2;
		}
	}

	drawmanaflag = true;
	drawhpflag = true;
}

void CalcPlrInv(Player &player, bool loadgfx)
{
	// Determine the players current stats, this updates the statFlag on all equipped items that became unusable after
	//  a change in equipment.
	CalcSelfItems(player);

	// Determine the current item bonuses gained from usable equipped items
	CalcPlrItemVals(player, loadgfx);

	if (&player == MyPlayer) {
		// Now that stat gains from equipped items have been calculated, mark unusable scrolls etc
		for (Item &item : InventoryAndBeltPlayerItemsRange { player }) {
			item.updateRequiredStatsCacheForPlayer(player);
		}
		player.CalcScrolls();
		CalcPlrStaff(player);
		if (IsStashOpen) {
			// If stash is open, ensure the items are displayed correctly
			Stash.RefreshItemStatFlags();
		}
	}
}

void InitializeItem(Item &item, int itemData)
{
	auto &pAllItem = AllItemsList[itemData];

	// zero-initialize struct
	item = {};

	item._itype = pAllItem.itype;
	item._iCurs = pAllItem.iCurs;
	CopyUtf8(item._iName, _(pAllItem.iName), sizeof(item._iName));
	CopyUtf8(item._iIName, _(pAllItem.iName), sizeof(item._iIName));
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

void GenerateNewSeed(Item &item)
{
	item._iSeed = AdvanceRndSeed();
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
	Player &player = Players[playerId];

	for (auto &item : player.InvBody) {
		item.clear();
	}

	// converting this to a for loop creates a `rep stosd` instruction,
	// so this probably actually was a memset
	memset(&player.InvGrid, 0, sizeof(player.InvGrid));

	for (auto &item : player.InvList) {
		item.clear();
	}

	player._pNumInv = 0;

	for (auto &item : player.SpdList) {
		item.clear();
	}

	switch (player._pClass) {
	case HeroClass::Warrior:
		InitializeItem(player.InvBody[INVLOC_HAND_LEFT], IDI_WARRIOR);
		GenerateNewSeed(player.InvBody[INVLOC_HAND_LEFT]);

		InitializeItem(player.InvBody[INVLOC_HAND_RIGHT], IDI_WARRSHLD);
		GenerateNewSeed(player.InvBody[INVLOC_HAND_RIGHT]);

		{
			Item club;
			InitializeItem(club, IDI_WARRCLUB);
			GenerateNewSeed(club);
			AutoPlaceItemInInventorySlot(player, 0, club, true);
		}

		InitializeItem(player.SpdList[0], IDI_HEAL);
		GenerateNewSeed(player.SpdList[0]);

		InitializeItem(player.SpdList[1], IDI_HEAL);
		GenerateNewSeed(player.SpdList[1]);
		break;
	case HeroClass::Rogue:
		InitializeItem(player.InvBody[INVLOC_HAND_LEFT], IDI_ROGUE);
		GenerateNewSeed(player.InvBody[INVLOC_HAND_LEFT]);

		InitializeItem(player.SpdList[0], IDI_HEAL);
		GenerateNewSeed(player.SpdList[0]);

		InitializeItem(player.SpdList[1], IDI_HEAL);
		GenerateNewSeed(player.SpdList[1]);
		break;
	case HeroClass::Sorcerer:
		InitializeItem(player.InvBody[INVLOC_HAND_LEFT], gbIsHellfire ? IDI_SORCERER : 166);
		GenerateNewSeed(player.InvBody[INVLOC_HAND_LEFT]);

		InitializeItem(player.SpdList[0], gbIsHellfire ? IDI_HEAL : IDI_MANA);
		GenerateNewSeed(player.SpdList[0]);

		InitializeItem(player.SpdList[1], gbIsHellfire ? IDI_HEAL : IDI_MANA);
		GenerateNewSeed(player.SpdList[1]);
		break;

	case HeroClass::Monk:
		InitializeItem(player.InvBody[INVLOC_HAND_LEFT], IDI_SHORTSTAFF);
		GenerateNewSeed(player.InvBody[INVLOC_HAND_LEFT]);
		InitializeItem(player.SpdList[0], IDI_HEAL);
		GenerateNewSeed(player.SpdList[0]);

		InitializeItem(player.SpdList[1], IDI_HEAL);
		GenerateNewSeed(player.SpdList[1]);
		break;
	case HeroClass::Bard:
		InitializeItem(player.InvBody[INVLOC_HAND_LEFT], IDI_BARDSWORD);
		GenerateNewSeed(player.InvBody[INVLOC_HAND_LEFT]);

		InitializeItem(player.InvBody[INVLOC_HAND_RIGHT], IDI_BARDDAGGER);
		GenerateNewSeed(player.InvBody[INVLOC_HAND_RIGHT]);
		InitializeItem(player.SpdList[0], IDI_HEAL);
		GenerateNewSeed(player.SpdList[0]);

		InitializeItem(player.SpdList[1], IDI_HEAL);
		GenerateNewSeed(player.SpdList[1]);
		break;
	case HeroClass::Barbarian:
		InitializeItem(player.InvBody[INVLOC_HAND_LEFT], 139); // TODO: add more enums to items
		GenerateNewSeed(player.InvBody[INVLOC_HAND_LEFT]);

		InitializeItem(player.InvBody[INVLOC_HAND_RIGHT], IDI_WARRSHLD);
		GenerateNewSeed(player.InvBody[INVLOC_HAND_RIGHT]);
		InitializeItem(player.SpdList[0], IDI_HEAL);
		GenerateNewSeed(player.SpdList[0]);

		InitializeItem(player.SpdList[1], IDI_HEAL);
		GenerateNewSeed(player.SpdList[1]);
		break;
	}

	Item &goldItem = player.InvList[player._pNumInv];
	MakeGoldStack(goldItem, 100);

	player._pNumInv++;
	player.InvGrid[30] = player._pNumInv;

	player._pGold = goldItem._ivalue;

	CalcPlrItemVals(player, false);
}

bool ItemSpaceOk(Point position)
{
	if (!InDungeonBounds(position)) {
		return false;
	}

	if (IsTileSolid(position)) {
		return false;
	}

	if (dItem[position.x][position.y] != 0) {
		return false;
	}

	if (dMonster[position.x][position.y] != 0) {
		return false;
	}

	if (dPlayer[position.x][position.y] != 0) {
		return false;
	}

	if (IsItemBlockingObjectAtPosition(position)) {
		return false;
	}

	return true;
}

int AllocateItem()
{
	assert(ActiveItemCount < MAXITEMS);

	int inum = ActiveItems[ActiveItemCount];
	ActiveItemCount++;

	Items[inum] = {};

	return inum;
}

Point GetSuperItemLoc(Point position)
{
	std::optional<Point> itemPosition = FindClosestValidPosition(ItemSpaceOk, position, 1, 50);

	return itemPosition.value_or(Point { 0, 0 }); // TODO handle no space for dropping items
}

void GetItemAttrs(Item &item, int itemData, int lvl)
{
	item._itype = AllItemsList[itemData].itype;
	item._iCurs = AllItemsList[itemData].iCurs;
	CopyUtf8(item._iName, _(AllItemsList[itemData].iName), sizeof(item._iName));
	CopyUtf8(item._iIName, _(AllItemsList[itemData].iName), sizeof(item._iIName));
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
	item.setNewAnimation(MyPlayer->pLvlLoad == 0);
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

	int8_t mLevel = monster.MData->mLevel;
	if (!gbIsHellfire && monster.MType->mtype == MT_DIABLO)
		mLevel -= 15;

	SetupAllItems(item, idx, AdvanceRndSeed(), mLevel, uper, onlygood, false, false);

	if (sendmsg)
		NetSendCmdPItem(false, CMD_DROPITEM, item.position, item);
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
		NetSendCmdPItem(false, CMD_DROPITEM, item.position, item);
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
		InitializeItem(item, IDI_GOLD);
		item._iSeed = iseed;
		item._iCreateInfo = icreateinfo;
		item._ivalue = ivalue;
		SetPlrHandGoldCurs(item);
		gbIsHellfire = tmpIsHellfire;
		return;
	}

	if (icreateinfo == 0) {
		InitializeItem(item, idx);
		item._iSeed = iseed;
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
	InitializeItem(item, IDI_EAR);

	char heroName[17];
	heroName[0] = static_cast<char>((ic >> 8) & 0x7F);
	heroName[1] = static_cast<char>(ic & 0x7F);
	heroName[2] = static_cast<char>((iseed >> 24) & 0x7F);
	heroName[3] = static_cast<char>((iseed >> 16) & 0x7F);
	heroName[4] = static_cast<char>((iseed >> 8) & 0x7F);
	heroName[5] = static_cast<char>(iseed & 0x7F);
	heroName[6] = static_cast<char>(id & 0x7F);
	heroName[7] = static_cast<char>(dur & 0x7F);
	heroName[8] = static_cast<char>(mdur & 0x7F);
	heroName[9] = static_cast<char>(ch & 0x7F);
	heroName[10] = static_cast<char>(mch & 0x7F);
	heroName[11] = static_cast<char>((ivalue >> 8) & 0x7F);
	heroName[12] = static_cast<char>((ibuff >> 24) & 0x7F);
	heroName[13] = static_cast<char>((ibuff >> 16) & 0x7F);
	heroName[14] = static_cast<char>((ibuff >> 8) & 0x7F);
	heroName[15] = static_cast<char>(ibuff & 0x7F);
	heroName[16] = '\0';

	std::string itemName = fmt::format(_(/* TRANSLATORS: {:s} will be a Character Name */ "Ear of {:s}"), heroName);

	CopyUtf8(item._iName, itemName, sizeof(item._iName));
	CopyUtf8(item._iIName, heroName, sizeof(item._iIName));

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
		PackItem(id, CornerStone.item, (CornerStone.item.dwBuff & CF_HELLFIRE) != 0);
		const auto *buffer = reinterpret_cast<uint8_t *>(&id);
		for (size_t i = 0; i < sizeof(ItemPack); i++) {
			snprintf(&sgOptions.Hellfire.szItem[i * 2], 3, "%02hhX", buffer[i]);
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

	CornerStone.item.clear();
	CornerStone.activated = true;
	if (dItem[position.x][position.y] != 0) {
		int ii = dItem[position.x][position.y] - 1;
		for (int i = 0; i < ActiveItemCount; i++) {
			if (ActiveItems[i] == ii) {
				DeleteItem(i);
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

	UnPackItem(pkSItem, item, (pkSItem.dwBuff & CF_HELLFIRE) != 0);
	item.position = position;
	RespawnItem(item, false);
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
		item.AnimInfo.CurrentFrame = item.AnimInfo.NumberOfFrames - 1;
		item._iAnimFlag = false;
	}
}

void SpawnRewardItem(int itemid, Point position, bool sendmsg)
{
	if (ActiveItemCount >= MAXITEMS)
		return;

	int ii = AllocateItem();
	auto &item = Items[ii];

	item.position = position;
	dItem[position.x][position.y] = ii + 1;
	int curlv = ItemsGetCurrlevel();
	GetItemAttrs(item, itemid, curlv);
	item.setNewAnimation(true);
	item._iSelFlag = 2;
	item._iPostDraw = true;
	item._iIdentified = true;
	GenerateNewSeed(item);

	if (sendmsg) {
		NetSendCmdPItem(true, CMD_SPAWNITEM, item.position, item);
	}
}

void SpawnMapOfDoom(Point position, bool sendmsg)
{
	SpawnRewardItem(IDI_MAPOFDOOM, position, sendmsg);
}

void SpawnRuneBomb(Point position, bool sendmsg)
{
	SpawnRewardItem(IDI_RUNEBOMB, position, sendmsg);
}

void SpawnTheodore(Point position, bool sendmsg)
{
	SpawnRewardItem(IDI_THEODORE, position, sendmsg);
}

void RespawnItem(Item &item, bool flipFlag)
{
	int it = ItemCAnimTbl[item._iCurs];
	item.setNewAnimation(flipFlag);
	item._iRequest = false;

	if (IsAnyOf(item._iCurs, ICURS_MAGIC_ROCK, ICURS_TAVERN_SIGN, ICURS_ANVIL_OF_FURY))
		item._iSelFlag = 1;
	else if (IsAnyOf(item._iCurs, ICURS_MAP_OF_THE_STARS, ICURS_RUNE_BOMB, ICURS_THEODORE, ICURS_AURIC_AMULET))
		item._iSelFlag = 2;

	if (item._iCurs == ICURS_MAGIC_ROCK) {
		PlaySfxLoc(ItemDropSnds[it], item.position);
	}
}

void DeleteItem(int i)
{
	if (ActiveItemCount > 0)
		ActiveItemCount--;

	assert(i >= 0 && i < MAXITEMS && ActiveItemCount < MAXITEMS);

	if (pcursitem == ActiveItems[i]) // Unselect item if player has it highlighted
		pcursitem = -1;

	if (i < ActiveItemCount) {
		// If the deleted item was not already at the end of the active list, swap the indexes around to make the next item allocation simpler.
		std::swap(ActiveItems[i], ActiveItems[ActiveItemCount]);
	}
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
			if (item._iSelFlag == 1 && item.AnimInfo.CurrentFrame == 10)
				item.AnimInfo.CurrentFrame = 0;
			if (item._iSelFlag == 2 && item.AnimInfo.CurrentFrame == 20)
				item.AnimInfo.CurrentFrame = 10;
		} else {
			if (item.AnimInfo.CurrentFrame == (item.AnimInfo.NumberOfFrames - 1) / 2)
				PlaySfxLoc(ItemDropSnds[ItemCAnimTbl[item._iCurs]], item.position);

			if (item.AnimInfo.CurrentFrame >= item.AnimInfo.NumberOfFrames - 1) {
				item.AnimInfo.CurrentFrame = item.AnimInfo.NumberOfFrames - 1;
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
	item.AnimInfo.celSprite.emplace(*itemanims[ItemCAnimTbl[item._iCurs]]);
}

void GetItemStr(Item &item)
{
	if (item._itype != ItemType::Gold) {
		if (item._iIdentified)
			InfoString = item._iIName;
		else
			InfoString = item._iName;

		InfoColor = item.getTextColor();
	} else {
		int nGold = item._ivalue;
		InfoString = fmt::format(ngettext("{:d} gold piece", "{:d} gold pieces", nGold), nGold);
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
}

void DoRecharge(Player &player, int cii)
{
	Item *pi;

	if (cii >= NUM_INVLOC) {
		pi = &player.InvList[cii - NUM_INVLOC];
	} else {
		pi = &player.InvBody[cii];
	}

	RechargeItem(*pi, player);
	CalcPlrInv(player, true);
}

bool DoOil(Player &player, int cii)
{
	Item *pi;
	if (cii >= NUM_INVLOC) {
		pi = &player.InvList[cii - NUM_INVLOC];
	} else {
		pi = &player.InvBody[cii];
	}
	if (!ApplyOilToItem(*pi, player))
		return false;
	CalcPlrInv(player, true);
	return true;
}

[[nodiscard]] std::string PrintItemPower(char plidx, const Item &item)
{
	switch (plidx) {
	case IPL_TOHIT:
	case IPL_TOHIT_CURSE:
		return fmt::format(_("chance to hit: {:+d}%"), item._iPLToHit);
	case IPL_DAMP:
	case IPL_DAMP_CURSE:
		return fmt::format(_(/*xgettext:no-c-format*/ "{:+d}% damage"), item._iPLDam);
	case IPL_TOHIT_DAMP:
	case IPL_TOHIT_DAMP_CURSE:
		return fmt::format(_("to hit: {:+d}%, {:+d}% damage"), item._iPLToHit, item._iPLDam);
	case IPL_ACP:
	case IPL_ACP_CURSE:
		return fmt::format(_(/*xgettext:no-c-format*/ "{:+d}% armor"), item._iPLAC);
	case IPL_SETAC:
	case IPL_AC_CURSE:
		return fmt::format(_("armor class: {:d}"), item._iAC);
	case IPL_FIRERES:
	case IPL_FIRERES_CURSE:
		if (item._iPLFR < MAXRESIST)
			return fmt::format(_("Resist Fire: {:+d}%"), item._iPLFR);
		else
			return fmt::format(_("Resist Fire: {:+d}% MAX"), MAXRESIST);
	case IPL_LIGHTRES:
	case IPL_LIGHTRES_CURSE:
		if (item._iPLLR < MAXRESIST)
			return fmt::format(_("Resist Lightning: {:+d}%"), item._iPLLR);
		else
			return fmt::format(_("Resist Lightning: {:+d}% MAX"), MAXRESIST);
	case IPL_MAGICRES:
	case IPL_MAGICRES_CURSE:
		if (item._iPLMR < MAXRESIST)
			return fmt::format(_("Resist Magic: {:+d}%"), item._iPLMR);
		else
			return fmt::format(_("Resist Magic: {:+d}% MAX"), MAXRESIST);
	case IPL_ALLRES:
	case IPL_ALLRES_CURSE:
		if (item._iPLFR < MAXRESIST)
			return fmt::format(_("Resist All: {:+d}%"), item._iPLFR);
		else
			return fmt::format(_("Resist All: {:+d}% MAX"), MAXRESIST);
	case IPL_SPLLVLADD:
		if (item._iSplLvlAdd > 0)
			return fmt::format(ngettext("spells are increased {:d} level", "spells are increased {:d} levels", item._iSplLvlAdd), item._iSplLvlAdd);
		else if (item._iSplLvlAdd < 0)
			return fmt::format(ngettext("spells are decreased {:d} level", "spells are decreased {:d} levels", -item._iSplLvlAdd), -item._iSplLvlAdd);
		else
			return _("spell levels unchanged (?)");
	case IPL_CHARGES:
		return _("Extra charges");
	case IPL_SPELL:
		return fmt::format(ngettext("{:d} {:s} charge", "{:d} {:s} charges", item._iMaxCharges), item._iMaxCharges, pgettext("spell", spelldata[item._iSpell].sNameText));
	case IPL_FIREDAM:
		if (item._iFMinDam == item._iFMaxDam)
			return fmt::format(_("Fire hit damage: {:d}"), item._iFMinDam);
		else
			return fmt::format(_("Fire hit damage: {:d}-{:d}"), item._iFMinDam, item._iFMaxDam);
	case IPL_LIGHTDAM:
		if (item._iLMinDam == item._iLMaxDam)
			return fmt::format(_("Lightning hit damage: {:d}"), item._iLMinDam);
		else
			return fmt::format(_("Lightning hit damage: {:d}-{:d}"), item._iLMinDam, item._iLMaxDam);
	case IPL_STR:
	case IPL_STR_CURSE:
		return fmt::format(_("{:+d} to strength"), item._iPLStr);
	case IPL_MAG:
	case IPL_MAG_CURSE:
		return fmt::format(_("{:+d} to magic"), item._iPLMag);
	case IPL_DEX:
	case IPL_DEX_CURSE:
		return fmt::format(_("{:+d} to dexterity"), item._iPLDex);
	case IPL_VIT:
	case IPL_VIT_CURSE:
		return fmt::format(_("{:+d} to vitality"), item._iPLVit);
	case IPL_ATTRIBS:
	case IPL_ATTRIBS_CURSE:
		return fmt::format(_("{:+d} to all attributes"), item._iPLStr);
	case IPL_GETHIT_CURSE:
	case IPL_GETHIT:
		return fmt::format(_("{:+d} damage from enemies"), item._iPLGetHit);
	case IPL_LIFE:
	case IPL_LIFE_CURSE:
		return fmt::format(_("Hit Points: {:+d}"), item._iPLHP >> 6);
	case IPL_MANA:
	case IPL_MANA_CURSE:
		return fmt::format(_("Mana: {:+d}"), item._iPLMana >> 6);
	case IPL_DUR:
		return _("high durability");
	case IPL_DUR_CURSE:
		return _("decreased durability");
	case IPL_INDESTRUCTIBLE:
		return _("indestructible");
	case IPL_LIGHT:
		return fmt::format(_(/*xgettext:no-c-format*/ "+{:d}% light radius"), 10 * item._iPLLight);
	case IPL_LIGHT_CURSE:
		return fmt::format(_(/*xgettext:no-c-format*/ "-{:d}% light radius"), -10 * item._iPLLight);
	case IPL_MULT_ARROWS:
		return _("multiple arrows per shot");
	case IPL_FIRE_ARROWS:
		if (item._iFMinDam == item._iFMaxDam)
			return fmt::format(_("fire arrows damage: {:d}"), item._iFMinDam);
		else
			return fmt::format(_("fire arrows damage: {:d}-{:d}"), item._iFMinDam, item._iFMaxDam);
	case IPL_LIGHT_ARROWS:
		if (item._iLMinDam == item._iLMaxDam)
			return fmt::format(_("lightning arrows damage {:d}"), item._iLMinDam);
		else
			return fmt::format(_("lightning arrows damage {:d}-{:d}"), item._iLMinDam, item._iLMaxDam);
	case IPL_FIREBALL:
		if (item._iFMinDam == item._iFMaxDam)
			return fmt::format(_("fireball damage: {:d}"), item._iFMinDam);
		else
			return fmt::format(_("fireball damage: {:d}-{:d}"), item._iFMinDam, item._iFMaxDam);
	case IPL_THORNS:
		return _("attacker takes 1-3 damage");
	case IPL_NOMANA:
		return _("user loses all mana");
	case IPL_NOHEALPLR:
		return _("you can't heal");
	case IPL_ABSHALFTRAP:
		return _("absorbs half of trap damage");
	case IPL_KNOCKBACK:
		return _("knocks target back");
	case IPL_3XDAMVDEM:
		return _(/*xgettext:no-c-format*/ "+200% damage vs. demons");
	case IPL_ALLRESZERO:
		return _("All Resistance equals 0");
	case IPL_NOHEALMON:
		return _("hit monster doesn't heal");
	case IPL_STEALMANA:
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::StealMana3))
			return _(/*xgettext:no-c-format*/ "hit steals 3% mana");
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::StealMana5))
			return _(/*xgettext:no-c-format*/ "hit steals 5% mana");
		return "";
	case IPL_STEALLIFE:
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::StealLife3))
			return _(/*xgettext:no-c-format*/ "hit steals 3% life");
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::StealLife5))
			return _(/*xgettext:no-c-format*/ "hit steals 5% life");
		return "";
	case IPL_TARGAC:
		return _("penetrates target's armor");
	case IPL_FASTATTACK:
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::QuickAttack))
			return _("quick attack");
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::FastAttack))
			return _("fast attack");
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::FasterAttack))
			return _("faster attack");
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::FastestAttack))
			return _("fastest attack");
		return _("Another ability (NW)");
	case IPL_FASTRECOVER:
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::FastHitRecovery))
			return _("fast hit recovery");
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::FasterHitRecovery))
			return _("faster hit recovery");
		if (HasAnyOf(item._iFlags, ItemSpecialEffect::FastestHitRecovery))
			return _("fastest hit recovery");
		return _("Another ability (NW)");
	case IPL_FASTBLOCK:
		return _("fast block");
	case IPL_DAMMOD:
		return fmt::format(ngettext("adds {:d} point to damage", "adds {:d} points to damage", item._iPLDamMod), item._iPLDamMod);
	case IPL_RNDARROWVEL:
		return _("fires random speed arrows");
	case IPL_SETDAM:
		return _("unusual item damage");
	case IPL_SETDUR:
		return _("altered durability");
	case IPL_FASTSWING:
		return _("Faster attack swing");
	case IPL_ONEHAND:
		return _("one handed sword");
	case IPL_DRAINLIFE:
		return _("constantly lose hit points");
	case IPL_RNDSTEALLIFE:
		return _("life stealing");
	case IPL_NOMINSTR:
		return _("no strength requirement");
	case IPL_INFRAVISION:
		return _("see with infravision");
	case IPL_INVCURS:
		return " ";
	case IPL_ADDACLIFE:
		if (item._iFMinDam == item._iFMaxDam)
			return fmt::format(_("lightning damage: {:d}"), item._iFMinDam);
		else
			return fmt::format(_("lightning damage: {:d}-{:d}"), item._iFMinDam, item._iFMaxDam);
	case IPL_ADDMANAAC:
		return _("charged bolts on hits");
	case IPL_FIRERESCLVL:
		if (item._iPLFR > 0)
			return fmt::format(_("Resist Fire: {:+d}%"), item._iPLFR);
		else
			return " ";
	case IPL_DEVASTATION:
		return _("occasional triple damage");
	case IPL_DECAY:
		return fmt::format(_(/*xgettext:no-c-format*/ "decaying {:+d}% damage"), item._iPLDam);
	case IPL_PERIL:
		return _("2x dmg to monst, 1x to you");
	case IPL_JESTERS:
		return _(/*xgettext:no-c-format*/ "Random 0 - 500% damage");
	case IPL_CRYSTALLINE:
		return fmt::format(_(/*xgettext:no-c-format*/ "low dur, {:+d}% damage"), item._iPLDam);
	case IPL_DOPPELGANGER:
		return fmt::format(_("to hit: {:+d}%, {:+d}% damage"), item._iPLToHit, item._iPLDam);
	case IPL_ACDEMON:
		return _("extra AC vs demons");
	case IPL_ACUNDEAD:
		return _("extra AC vs undead");
	case IPL_MANATOLIFE:
		return _("50% Mana moved to Health");
	case IPL_LIFETOMANA:
		return _("40% Health moved to Mana");
	default:
		return _("Another ability (NW)");
	}
}

void DrawUniqueInfo(const Surface &out)
{
	const Point position = GetRightPanel().position - Displacement { SPANEL_WIDTH, 0 };
	if (IsLeftPanelOpen() && GetLeftPanel().Contains(position)) {
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
		DrawString(out, PrintItemPower(power.type, curruitem), rect, UiFlags::ColorWhite | UiFlags::AlignCenter);
	}
}

void PrintItemDetails(const Item &item)
{
	if (item._iClass == ICLASS_WEAPON) {
		if (item._iMinDam == item._iMaxDam) {
			if (item._iMaxDur == DUR_INDESTRUCTIBLE)
				AddPanelString(fmt::format(_("damage: {:d}  Indestructible"), item._iMinDam));
			else
				AddPanelString(fmt::format(_(/* TRANSLATORS: Dur: is durability */ "damage: {:d}  Dur: {:d}/{:d}"), item._iMinDam, item._iDurability, item._iMaxDur));
		} else {
			if (item._iMaxDur == DUR_INDESTRUCTIBLE)
				AddPanelString(fmt::format(_("damage: {:d}-{:d}  Indestructible"), item._iMinDam, item._iMaxDam));
			else
				AddPanelString(fmt::format(_(/* TRANSLATORS: Dur: is durability */ "damage: {:d}-{:d}  Dur: {:d}/{:d}"), item._iMinDam, item._iMaxDam, item._iDurability, item._iMaxDur));
		}
	}
	if (item._iClass == ICLASS_ARMOR) {
		if (item._iMaxDur == DUR_INDESTRUCTIBLE)
			AddPanelString(fmt::format(_("armor: {:d}  Indestructible"), item._iAC));
		else
			AddPanelString(fmt::format(_(/* TRANSLATORS: Dur: is durability */ "armor: {:d}  Dur: {:d}/{:d}"), item._iAC, item._iDurability, item._iMaxDur));
	}
	if (item._iMiscId == IMISC_STAFF && item._iMaxCharges != 0) {
		AddPanelString(fmt::format(_("Charges: {:d}/{:d}"), item._iCharges, item._iMaxCharges));
	}
	if (item._iPrePower != -1) {
		AddPanelString(PrintItemPower(item._iPrePower, item));
	}
	if (item._iSufPower != -1) {
		AddPanelString(PrintItemPower(item._iSufPower, item));
	}
	if (item._iMagical == ITEM_QUALITY_UNIQUE) {
		AddPanelString(_("unique item"));
		ShowUniqueItemInfoBox = true;
		curruitem = item;
	}
	PrintItemInfo(item);
}

void PrintItemDur(const Item &item)
{
	if (item._iClass == ICLASS_WEAPON) {
		if (item._iMinDam == item._iMaxDam) {
			if (item._iMaxDur == DUR_INDESTRUCTIBLE)
				AddPanelString(fmt::format(_("damage: {:d}  Indestructible"), item._iMinDam));
			else
				AddPanelString(fmt::format(_("damage: {:d}  Dur: {:d}/{:d}"), item._iMinDam, item._iDurability, item._iMaxDur));
		} else {
			if (item._iMaxDur == DUR_INDESTRUCTIBLE)
				AddPanelString(fmt::format(_("damage: {:d}-{:d}  Indestructible"), item._iMinDam, item._iMaxDam));
			else
				AddPanelString(fmt::format(_("damage: {:d}-{:d}  Dur: {:d}/{:d}"), item._iMinDam, item._iMaxDam, item._iDurability, item._iMaxDur));
		}
		if (item._iMiscId == IMISC_STAFF && item._iMaxCharges > 0) {
			AddPanelString(fmt::format(_("Charges: {:d}/{:d}"), item._iCharges, item._iMaxCharges));
		}
		if (item._iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString(_("Not Identified"));
	}
	if (item._iClass == ICLASS_ARMOR) {
		if (item._iMaxDur == DUR_INDESTRUCTIBLE)
			AddPanelString(fmt::format(_("armor: {:d}  Indestructible"), item._iAC));
		else
			AddPanelString(fmt::format(_("armor: {:d}  Dur: {:d}/{:d}"), item._iAC, item._iDurability, item._iMaxDur));
		if (item._iMagical != ITEM_QUALITY_NORMAL)
			AddPanelString(_("Not Identified"));
		if (item._iMiscId == IMISC_STAFF && item._iMaxCharges > 0) {
			AddPanelString(fmt::format(_("Charges: {:d}/{:d}"), item._iCharges, item._iMaxCharges));
		}
	}
	if (IsAnyOf(item._itype, ItemType::Ring, ItemType::Amulet))
		AddPanelString(_("Not Identified"));
	PrintItemInfo(item);
}

void UseItem(int pnum, item_misc_id mid, spell_id spl)
{
	Player &player = Players[pnum];

	switch (mid) {
	case IMISC_HEAL:
	case IMISC_FOOD:
		player.RestorePartialLife();
		if (&player == MyPlayer) {
			drawhpflag = true;
		}
		break;
	case IMISC_FULLHEAL:
		player.RestoreFullLife();
		if (&player == MyPlayer) {
			drawhpflag = true;
		}
		break;
	case IMISC_MANA:
		player.RestorePartialMana();
		if (&player == MyPlayer) {
			drawmanaflag = true;
		}
		break;
	case IMISC_FULLMANA:
		player.RestoreFullMana();
		if (&player == MyPlayer) {
			drawmanaflag = true;
		}
		break;
	case IMISC_ELIXSTR:
		ModifyPlrStr(pnum, 1);
		break;
	case IMISC_ELIXMAG:
		ModifyPlrMag(pnum, 1);
		if (gbIsHellfire) {
			player.RestoreFullMana();
			if (&player == MyPlayer) {
				drawmanaflag = true;
			}
		}
		break;
	case IMISC_ELIXDEX:
		ModifyPlrDex(pnum, 1);
		break;
	case IMISC_ELIXVIT:
		ModifyPlrVit(pnum, 1);
		if (gbIsHellfire) {
			player.RestoreFullLife();
			if (&player == MyPlayer) {
				drawhpflag = true;
			}
		}
		break;
	case IMISC_REJUV: {
		player.RestorePartialLife();
		player.RestorePartialMana();
		if (&player == MyPlayer) {
			drawhpflag = true;
			drawmanaflag = true;
		}
	} break;
	case IMISC_FULLREJUV:
		player.RestoreFullLife();
		player.RestoreFullMana();
		if (&player == MyPlayer) {
			drawhpflag = true;
			drawmanaflag = true;
		}
		break;
	case IMISC_SCROLL:
		if (ControlMode == ControlTypes::KeyboardAndMouse && spelldata[spl].sTargeted) {
			player._pTSpell = spl;
			if (pnum == MyPlayerId)
				NewCursor(CURSOR_TELEPORT);
		} else {
			ClrPlrPath(player);
			player._pSpell = spl;
			player._pSplType = RSPLTYPE_INVALID;
			player._pSplFrom = 3;
			player.destAction = ACTION_SPELL;
			player.destParam1 = cursPosition.x;
			player.destParam2 = cursPosition.y;
			if (pnum == MyPlayerId && spl == SPL_NOVA)
				NetSendCmdLoc(pnum, true, CMD_NOVA, cursPosition);
		}
		break;
	case IMISC_SCROLLT:
		if (ControlMode == ControlTypes::KeyboardAndMouse && spelldata[spl].sTargeted) {
			player._pTSpell = spl;
			if (pnum == MyPlayerId)
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
		if (HasNoneOf(player._pIFlags, ItemSpecialEffect::NoMana)) {
			player._pMana += spelldata[spl].sManaCost << 6;
			player._pMana = std::min(player._pMana, player._pMaxMana);
			player._pManaBase += spelldata[spl].sManaCost << 6;
			player._pManaBase = std::min(player._pManaBase, player._pMaxManaBase);
		}
		if (&player == MyPlayer) {
			for (Item &item : InventoryPlayerItemsRange { player }) {
				item.updateRequiredStatsCacheForPlayer(player);
			}
			if (IsStashOpen) {
				Stash.RefreshItemStatFlags();
			}
		}
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
		if (pnum != MyPlayerId) {
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
		ModifyPlrStr(pnum, 3);
		ModifyPlrMag(pnum, 3);
		ModifyPlrDex(pnum, 3);
		ModifyPlrVit(pnum, 3);
		break;
	case IMISC_RUNEF:
		player._pTSpell = SPL_RUNEFIRE;
		if (pnum == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_RUNEL:
		player._pTSpell = SPL_RUNELIGHT;
		if (pnum == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_GR_RUNEL:
		player._pTSpell = SPL_RUNENOVA;
		if (pnum == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_GR_RUNEF:
		player._pTSpell = SPL_RUNEIMMOLAT;
		if (pnum == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	case IMISC_RUNES:
		player._pTSpell = SPL_RUNESTONE;
		if (pnum == MyPlayerId)
			NewCursor(CURSOR_TELEPORT);
		break;
	default:
		break;
	}
}

bool UseItemOpensHive(const Item &item, Point position)
{
	if (item.IDidx != IDI_RUNEBOMB)
		return false;
	for (auto dir : PathDirs) {
		Point adjacentPosition = position + dir;
		if (OpensHive(adjacentPosition))
			return true;
	}
	return false;
}

bool UseItemOpensCrypt(const Item &item, Point position)
{
	if (item.IDidx != IDI_MAPOFDOOM)
		return false;
	for (auto dir : PathDirs) {
		Point adjacentPosition = position + dir;
		if (OpensGrave(adjacentPosition))
			return true;
	}
	return false;
}

void SpawnSmith(int lvl)
{
	constexpr int PinnedItemCount = 0;

	int maxValue = 140000;
	int maxItems = 20;
	if (gbIsHellfire) {
		maxValue = 200000;
		maxItems = 25;
	}

	int iCnt = GenerateRnd(maxItems - 10) + 10;
	for (int i = 0; i < iCnt; i++) {
		Item &newItem = smithitem[i];

		do {
			newItem = {};
			newItem._iSeed = AdvanceRndSeed();
			SetRndSeed(newItem._iSeed);
			int itemData = RndSmithItem(lvl) - 1;
			GetItemAttrs(newItem, itemData, lvl);
		} while (newItem._iIvalue > maxValue);

		newItem._iCreateInfo = lvl | CF_SMITH;
		newItem._iIdentified = true;
	}
	for (int i = iCnt; i < SMITH_ITEMS; i++)
		smithitem[i].clear();

	SortVendor(smithitem + PinnedItemCount);
}

void SpawnPremium(int pnum)
{
	int8_t lvl = Players[pnum]._pLevel;
	int maxItems = gbIsHellfire ? SMITH_PREMIUM_ITEMS : 6;
	if (numpremium < maxItems) {
		for (int i = 0; i < maxItems; i++) {
			if (premiumitems[i].isEmpty()) {
				int plvl = premiumlevel + (gbIsHellfire ? premiumLvlAddHellfire[i] : premiumlvladd[i]);
				SpawnOnePremium(premiumitems[i], plvl, pnum);
			}
		}
		numpremium = maxItems;
	}
	while (premiumlevel < lvl) {
		premiumlevel++;
		if (gbIsHellfire) {
			// Discard first 3 items and shift next 10
			std::move(&premiumitems[3], &premiumitems[12] + 1, &premiumitems[0]);
			SpawnOnePremium(premiumitems[10], premiumlevel + premiumLvlAddHellfire[10], pnum);
			premiumitems[11] = premiumitems[13];
			SpawnOnePremium(premiumitems[12], premiumlevel + premiumLvlAddHellfire[12], pnum);
			premiumitems[13] = premiumitems[14];
			SpawnOnePremium(premiumitems[14], premiumlevel + premiumLvlAddHellfire[14], pnum);
		} else {
			// Discard first 2 items and shift next 3
			std::move(&premiumitems[2], &premiumitems[4] + 1, &premiumitems[0]);
			SpawnOnePremium(premiumitems[3], premiumlevel + premiumlvladd[3], pnum);
			premiumitems[4] = premiumitems[5];
			SpawnOnePremium(premiumitems[5], premiumlevel + premiumlvladd[5], pnum);
		}
	}
}

void SpawnWitch(int lvl)
{
	constexpr int PinnedItemCount = 3;
	constexpr std::array<int, PinnedItemCount> PinnedItemTypes = { IDI_MANA, IDI_FULLMANA, IDI_PORTAL };
	constexpr int MaxPinnedBookCount = 4;
	constexpr std::array<int, MaxPinnedBookCount> PinnedBookTypes = { 114, 115, 116, 117 };

	int bookCount = 0;
	const int pinnedBookCount = gbIsHellfire ? GenerateRnd(MaxPinnedBookCount) : 0;
	const int reservedItems = gbIsHellfire ? 10 : 17;
	const int itemCount = GenerateRnd(WITCH_ITEMS - reservedItems) + 10;
	const int maxValue = gbIsHellfire ? 200000 : 140000;

	for (int i = 0; i < WITCH_ITEMS; i++) {
		Item &item = witchitem[i];
		item = {};

		if (i < PinnedItemCount) {
			item._iSeed = AdvanceRndSeed();
			GetItemAttrs(item, PinnedItemTypes[i], 1);
			item._iCreateInfo = lvl;
			item._iStatFlag = true;
			continue;
		}

		if (gbIsHellfire) {
			if (i < PinnedItemCount + MaxPinnedBookCount && bookCount < pinnedBookCount) {
				int bookType = PinnedBookTypes[i - PinnedItemCount];
				if (lvl >= AllItemsList[bookType].iMinMLvl) {
					item._iSeed = AdvanceRndSeed();
					SetRndSeed(item._iSeed);
					GetItemAttrs(item, bookType, lvl);
					item._iCreateInfo = lvl | CF_WITCH;
					item._iIdentified = true;
					bookCount++;
					continue;
				}
			}
		}

		if (i >= itemCount) {
			item.clear();
			continue;
		}

		do {
			item = {};
			item._iSeed = AdvanceRndSeed();
			SetRndSeed(item._iSeed);
			int itemData = RndWitchItem(lvl) - 1;
			GetItemAttrs(item, itemData, lvl);
			int maxlvl = -1;
			if (GenerateRnd(100) <= 5)
				maxlvl = 2 * lvl;
			if (maxlvl == -1 && item._iMiscId == IMISC_STAFF)
				maxlvl = 2 * lvl;
			if (maxlvl != -1)
				GetItemBonus(item, maxlvl / 2, maxlvl, true, true);
		} while (item._iIvalue > maxValue);

		item._iCreateInfo = lvl | CF_WITCH;
		item._iIdentified = true;
	}

	SortVendor(witchitem + PinnedItemCount);
}

void SpawnBoy(int lvl)
{
	int ivalue = 0;
	bool keepgoing = false;
	int count = 0;

	Player &myPlayer = *MyPlayer;

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
		boyitem = {};
		boyitem._iSeed = AdvanceRndSeed();
		SetRndSeed(boyitem._iSeed);
		int itype = RndBoyItem(lvl) - 1;
		GetItemAttrs(boyitem, itype, lvl);
		GetItemBonus(boyitem, lvl, 2 * lvl, true, true);

		if (!gbIsHellfire) {
			if (boyitem._iIvalue > 90000) {
				keepgoing = true; // prevent breaking the do/while loop too early by failing hellfire's condition in while
				continue;
			}
			break;
		}

		ivalue = 0;

		ItemType itemType = boyitem._itype;

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
	            boyitem._iIvalue > 200000
	            || boyitem._iMinStr > strength
	            || boyitem._iMinMag > magic
	            || boyitem._iMinDex > dexterity
	            || boyitem._iIvalue < ivalue)
	        && count < 250));
	boyitem._iCreateInfo = lvl | CF_BOY;
	boyitem._iIdentified = true;
	boylevel = lvl / 2;
}

void SpawnHealer(int lvl)
{
	constexpr int PinnedItemCount = 2;
	constexpr std::array<int, PinnedItemCount + 1> PinnedItemTypes = { IDI_HEAL, IDI_FULLHEAL, IDI_RESURRECT };
	const int itemCount = GenerateRnd(gbIsHellfire ? 10 : 8) + 10;

	for (int i = 0; i < 20; i++) {
		Item &item = healitem[i];
		item = {};

		if (i < PinnedItemCount || (gbIsMultiplayer && i == PinnedItemCount)) {
			item._iSeed = AdvanceRndSeed();
			GetItemAttrs(item, PinnedItemTypes[i], 1);
			item._iCreateInfo = lvl;
			item._iStatFlag = true;
			continue;
		}

		if (i >= itemCount) {
			item.clear();
			continue;
		}

		item._iSeed = AdvanceRndSeed();
		SetRndSeed(item._iSeed);
		int itype = RndHealerItem(lvl) - 1;
		GetItemAttrs(item, itype, lvl);
		item._iCreateInfo = lvl | CF_HEALER;
		item._iIdentified = true;
	}

	SortVendor(healitem + PinnedItemCount);
}

void MakeGoldStack(Item &goldItem, int value)
{
	InitializeItem(goldItem, IDI_GOLD);
	GenerateNewSeed(goldItem);
	goldItem._iStatFlag = true;
	goldItem._ivalue = value;
	SetPlrHandGoldCurs(goldItem);
}

int ItemNoFlippy()
{
	int r = ActiveItems[ActiveItemCount - 1];
	Items[r].AnimInfo.CurrentFrame = Items[r].AnimInfo.NumberOfFrames - 1;
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
		item = {};
		SetupAllItems(item, idx, AdvanceRndSeed(), 2 * lvl, 1, true, false, delta);
		if (item._iMiscId == IMISC_BOOK && item._iSpell == ispell)
			break;
	}
	GetSuperItemSpace(position, ii);

	if (sendmsg)
		NetSendCmdPItem(false, CMD_DROPITEM, item.position, item);
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
	Point pos = MyPlayer->position.tile;
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
		item = {};
		item.position = bkp;
		SetupAllItems(item, idx, AdvanceRndSeed(), fake_m.mLevel, 1, false, false, false);

		std::string tmp(item._iIName);
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](unsigned char c) { return std::tolower(c); });
		if (tmp.find(itemName) != std::string::npos)
			break;
	}

	item._iIdentified = true;
	NetSendCmdPItem(false, CMD_DROPITEM, item.position, item);
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
	Point pos = MyPlayer->position.tile;
	GetSuperItemSpace(pos, ii);

	int i = 0;
	for (uint32_t begin = SDL_GetTicks();; i++) {
		if (SDL_GetTicks() - begin > max_time)
			return fmt::format("Item not found in {:d} seconds!", max_time / 1000);

		if (i > max_iter)
			return fmt::format("Item not found in {:d} tries!", max_iter);

		Point bkp = item.position;
		item = {};
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
	NetSendCmdPItem(false, CMD_DROPITEM, item.position, item);
	return fmt::format("Item generated successfully - iterations: {:d}", i);
}
#endif

void Item::setNewAnimation(bool showAnimation)
{
	int it = ItemCAnimTbl[_iCurs];
	int numberOfFrames = ItemAnimLs[it];
	auto celSprite = itemanims[it] ? std::optional<CelSprite> { *itemanims[it] } : std::nullopt;
	if (_iCurs != ICURS_MAGIC_ROCK)
		AnimInfo.SetNewAnimation(celSprite, numberOfFrames, 1, AnimationDistributionFlags::ProcessAnimationPending, 0, numberOfFrames);
	else
		AnimInfo.SetNewAnimation(celSprite, numberOfFrames, 1);
	_iPostDraw = false;
	_iRequest = false;
	if (showAnimation) {
		_iAnimFlag = true;
		_iSelFlag = 0;
	} else {
		AnimInfo.CurrentFrame = AnimInfo.NumberOfFrames - 1;
		_iAnimFlag = false;
		_iSelFlag = 1;
	}
}

void Item::updateRequiredStatsCacheForPlayer(const Player &player)
{
	if (_itype == ItemType::Misc && _iMiscId == IMISC_BOOK) {
		_iMinMag = spelldata[_iSpell].sMinInt;
		int8_t spellLevel = player._pSplLvl[_iSpell];
		while (spellLevel != 0) {
			_iMinMag += 20 * _iMinMag / 100;
			spellLevel--;
			if (_iMinMag + 20 * _iMinMag / 100 > 255) {
				_iMinMag = 255;
				spellLevel = 0;
			}
		}
	}
	_iStatFlag = player.CanUseItem(*this);
}

void initItemGetRecords()
{
	memset(itemrecord, 0, sizeof(itemrecord));
	gnNumGetRecords = 0;
}

void RepairItem(Item &item, int lvl)
{
	if (item._iDurability == item._iMaxDur) {
		return;
	}

	if (item._iMaxDur <= 0) {
		item.clear();
		return;
	}

	int rep = 0;
	do {
		rep += lvl + GenerateRnd(lvl);
		item._iMaxDur -= std::max(item._iMaxDur / (lvl + 9), 1);
		if (item._iMaxDur == 0) {
			item.clear();
			return;
		}
	} while (rep + item._iDurability < item._iMaxDur);

	item._iDurability = std::min<int>(item._iDurability + rep, item._iMaxDur);
}

void RechargeItem(Item &item, Player &player)
{
	if (item._itype != ItemType::Staff || item._iSpell == SPL_NULL)
		return;

	if (item._iCharges == item._iMaxCharges)
		return;

	int r = GetSpellBookLevel(item._iSpell);
	r = GenerateRnd(player._pLevel / r) + 1;

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
		if (item._iMaxDur == DUR_INDESTRUCTIBLE)
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
		item._iDurability = DUR_INDESTRUCTIBLE;
		item._iMaxDur = DUR_INDESTRUCTIBLE;
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

} // namespace devilution
