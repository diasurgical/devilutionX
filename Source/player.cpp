/**
 * @file player.cpp
 *
 * Implementation of player functionality, leveling, actions, creation, loading, etc.
 */
#include <algorithm>
#include <cstdint>

#include "control.h"
#include "cursor.h"
#include "dead.h"
#include "gamemenu.h"
#include "init.h"
#include "lighting.h"
#include "loadsave.h"
#include "minitext.h"
#include "missiles.h"
#include "options.h"
#include "qol/autopickup.h"
#include "spells.h"
#include "stores.h"
#include "storm/storm.h"
#include "towners.h"
#include "utils/language.h"
#include "utils/log.hpp"

namespace devilution {

int myplr;
PlayerStruct plr[MAX_PLRS];
bool deathflag;
int deathdelay;

/** Maps from armor animation to letter used in graphic files. */
const char ArmourChar[4] = {
	'L', // light
	'M', // medium
	'H', // heavy
	0
};
/** Maps from weapon animation to letter used in graphic files. */
const char WepChar[10] = {
	'N', // unarmed
	'U', // no weapon + shield
	'S', // sword + no shield
	'D', // sword + shield
	'B', // bow
	'A', // axe
	'M', // blunt + no shield
	'H', // blunt + shield
	'T', // staff
	0
};
/** Maps from player class to letter used in graphic files. */
const char CharChar[] = {
	'W', // warrior
	'R', // rogue
	'S', // sorcerer
	'M', // monk
	'B',
	'C',
	0
};

/* data */

/** Specifies the X-coordinate delta from the player start location in Tristram. */
int plrxoff[9] = { 0, 2, 0, 2, 1, 0, 1, 2, 1 };
/** Specifies the Y-coordinate delta from the player start location in Tristram. */
int plryoff[9] = { 0, 2, 2, 0, 1, 1, 0, 1, 2 };
/** Specifies the X-coordinate delta from a player, used for instanced when casting resurrect. */
int plrxoff2[9] = { 0, 1, 0, 1, 2, 0, 1, 2, 2 };
/** Specifies the Y-coordinate delta from a player, used for instanced when casting resurrect. */
int plryoff2[9] = { 0, 0, 1, 1, 0, 2, 2, 1, 2 };
/** Specifies the frame of each animation for which an action is triggered, for each player class. */
char PlrGFXAnimLens[enum_size<HeroClass>::value][11] = {
	{ 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 },
	{ 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 },
	{ 8, 16, 8, 6, 20, 12, 8, 20, 8, 12, 8 },
	{ 8, 16, 8, 3, 20, 18, 6, 20, 8, 12, 13 },
	{ 8, 18, 8, 4, 20, 16, 7, 20, 8, 10, 12 },
	{ 10, 16, 8, 2, 20, 20, 6, 20, 8, 9, 14 },
};
/** Maps from player class to player velocity. */
int PWVel[enum_size<HeroClass>::value][3] = {
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
	{ 2048, 1024, 512 },
};
/** Maps from player_class to starting stat in strength. */
int StrengthTbl[enum_size<HeroClass>::value] = {
	30,
	20,
	15,
	25,
	20,
	40,
};
/** Maps from player_class to starting stat in magic. */
int MagicTbl[enum_size<HeroClass>::value] = {
	// clang-format off
	10,
	15,
	35,
	15,
	20,
	 0,
	// clang-format on
};
/** Maps from player_class to starting stat in dexterity. */
int DexterityTbl[enum_size<HeroClass>::value] = {
	20,
	30,
	15,
	25,
	25,
	20,
};
/** Maps from player_class to starting stat in vitality. */
int VitalityTbl[enum_size<HeroClass>::value] = {
	25,
	20,
	20,
	20,
	20,
	25,
};
/** Specifies the chance to block bonus of each player class.*/
int ToBlkTbl[enum_size<HeroClass>::value] = {
	30,
	20,
	10,
	25,
	25,
	30,
};

/** Specifies the experience point limit of each level. */
int ExpLvlsTbl[MAXCHARLEVEL] = {
	0,
	2000,
	4620,
	8040,
	12489,
	18258,
	25712,
	35309,
	47622,
	63364,
	83419,
	108879,
	141086,
	181683,
	231075,
	313656,
	424067,
	571190,
	766569,
	1025154,
	1366227,
	1814568,
	2401895,
	3168651,
	4166200,
	5459523,
	7130496,
	9281874,
	12042092,
	15571031,
	20066900,
	25774405,
	32994399,
	42095202,
	53525811,
	67831218,
	85670061,
	107834823,
	135274799,
	169122009,
	210720231,
	261657253,
	323800420,
	399335440,
	490808349,
	601170414,
	733825617,
	892680222,
	1082908612,
	1310707109,
	1583495809
};
const char *const ClassPathTbl[] = {
	"Warrior",
	"Rogue",
	"Sorceror",
	"Monk",
	"Rogue",
	"Warrior",
};

void PlayerStruct::CalcScrolls()
{
	_pScrlSpells = 0;
	for (int i = 0; i < _pNumInv; i++) {
		if (!InvList[i].isEmpty() && (InvList[i]._iMiscId == IMISC_SCROLL || InvList[i]._iMiscId == IMISC_SCROLLT)) {
			if (InvList[i]._iStatFlag)
				_pScrlSpells |= GetSpellBitmask(InvList[i]._iSpell);
		}
	}

	for (int j = 0; j < MAXBELTITEMS; j++) {
		if (!SpdList[j].isEmpty() && (SpdList[j]._iMiscId == IMISC_SCROLL || SpdList[j]._iMiscId == IMISC_SCROLLT)) {
			if (SpdList[j]._iStatFlag)
				_pScrlSpells |= GetSpellBitmask(SpdList[j]._iSpell);
		}
	}
	EnsureValidReadiedSpell(*this);
}

bool PlayerStruct::HasItem(int item, int *idx) const
{
	for (int i = 0; i < _pNumInv; i++) {
		if (InvList[i].IDidx == item) {
			if (idx != nullptr)
				*idx = i;
			return true;
		}
	}

	return false;
}

void PlayerStruct::RemoveInvItem(int iv, bool calcScrolls)
{
	iv++;

	//Iterate through invGrid and remove every reference to item
	for (int i = 0; i < NUM_INV_GRID_ELEM; i++) {
		if (InvGrid[i] == iv || InvGrid[i] == -iv) {
			InvGrid[i] = 0;
		}
	}

	iv--;
	_pNumInv--;

	//If the item at the end of inventory array isn't the one we removed, we need to swap its position in the array with the removed item
	if (_pNumInv > 0 && _pNumInv != iv) {
		InvList[iv] = InvList[_pNumInv];

		for (int j = 0; j < NUM_INV_GRID_ELEM; j++) {
			if (InvGrid[j] == _pNumInv + 1) {
				InvGrid[j] = iv + 1;
			}
			if (InvGrid[j] == -(_pNumInv + 1)) {
				InvGrid[j] = -(iv + 1);
			}
		}
	}

	if (calcScrolls)
		CalcScrolls();
}

int PlayerStruct::GetBaseAttributeValue(CharacterAttribute attribute) const
{
	switch (attribute) {
	case CharacterAttribute::Dexterity:
		return this->_pBaseDex;
	case CharacterAttribute::Magic:
		return this->_pBaseMag;
	case CharacterAttribute::Strength:
		return this->_pBaseStr;
	case CharacterAttribute::Vitality:
		return this->_pBaseVit;
	default:
		app_fatal("Unsupported attribute");
	}
}

int PlayerStruct::GetMaximumAttributeValue(CharacterAttribute attribute) const
{
	static const int MaxStats[enum_size<HeroClass>::value][enum_size<CharacterAttribute>::value] = {
		// clang-format off
		{ 250,  50,  60, 100 },
		{  55,  70, 250,  80 },
		{  45, 250,  85,  80 },
		{ 150,  80, 150,  80 },
		{ 120, 120, 120, 100 },
		{ 255,   0,  55, 150 },
		// clang-format on
	};

	return MaxStats[static_cast<std::size_t>(_pClass)][static_cast<std::size_t>(attribute)];
}

Point PlayerStruct::GetTargetPosition() const
{
	// clang-format off
	constexpr int directionOffsetX[8] = {  0,-1, 1, 0,-1, 1, 1,-1 };
	constexpr int directionOffsetY[8] = { -1, 0, 0, 1,-1,-1, 1, 1 };
	// clang-format on
	Point target = position.future;
	for (auto step : walkpath) {
		if (step == WALK_NONE)
			break;
		if (step > 0) {
			target.x += directionOffsetX[step - 1];
			target.y += directionOffsetY[step - 1];
		}
	}
	return target;
}

_sfx_id herosounds[enum_size<HeroClass>::value][102] = {
	// clang-format off
	{ PS_WARR1,  PS_WARR2,  PS_WARR3,  PS_WARR4,  PS_WARR5,  PS_WARR6,  PS_WARR7,  PS_WARR8,  PS_WARR9,  PS_WARR10,  PS_WARR11,  PS_WARR12,  PS_WARR13,  PS_WARR14,  PS_WARR15,  PS_WARR16,  PS_WARR17,  PS_WARR18,  PS_WARR19,  PS_WARR20,  PS_WARR21,  PS_WARR22,  PS_WARR23,  PS_WARR24,  PS_WARR25,  PS_WARR26,  PS_WARR27,  PS_WARR28,  PS_WARR29,  PS_WARR30,  PS_WARR31,  PS_WARR32,  PS_WARR33,  PS_WARR34,  PS_WARR35,  PS_WARR36,  PS_WARR37,  PS_WARR38,  PS_WARR39,  PS_WARR40,  PS_WARR41,  PS_WARR42,  PS_WARR43,  PS_WARR44,  PS_WARR45,  PS_WARR46,  PS_WARR47,  PS_WARR48,  PS_WARR49,  PS_WARR50,  PS_WARR51,  PS_WARR52,  PS_WARR53,  PS_WARR54,  PS_WARR55,  PS_WARR56,  PS_WARR57,  PS_WARR58,  PS_WARR59,  PS_WARR60,  PS_WARR61,  PS_WARR62,  PS_WARR63,  PS_WARR64,  PS_WARR65,  PS_WARR66,  PS_WARR67,  PS_WARR68,  PS_WARR69,  PS_WARR70,  PS_WARR71,  PS_WARR72,  PS_WARR73,  PS_WARR74,  PS_WARR75,  PS_WARR76,  PS_WARR77,  PS_WARR78,  PS_WARR79,  PS_WARR80,  PS_WARR81,  PS_WARR82,  PS_WARR83,  PS_WARR84,  PS_WARR85,  PS_WARR86,  PS_WARR87,  PS_WARR88,  PS_WARR89,  PS_WARR90,  PS_WARR91,  PS_WARR92,  PS_WARR93,  PS_WARR94,  PS_WARR95,  PS_WARR96B,  PS_WARR97,  PS_WARR98,  PS_WARR99,  PS_WARR100,  PS_WARR101,  PS_WARR102  },
	{ PS_ROGUE1, PS_ROGUE2, PS_ROGUE3, PS_ROGUE4, PS_ROGUE5, PS_ROGUE6, PS_ROGUE7, PS_ROGUE8, PS_ROGUE9, PS_ROGUE10, PS_ROGUE11, PS_ROGUE12, PS_ROGUE13, PS_ROGUE14, PS_ROGUE15, PS_ROGUE16, PS_ROGUE17, PS_ROGUE18, PS_ROGUE19, PS_ROGUE20, PS_ROGUE21, PS_ROGUE22, PS_ROGUE23, PS_ROGUE24, PS_ROGUE25, PS_ROGUE26, PS_ROGUE27, PS_ROGUE28, PS_ROGUE29, PS_ROGUE30, PS_ROGUE31, PS_ROGUE32, PS_ROGUE33, PS_ROGUE34, PS_ROGUE35, PS_ROGUE36, PS_ROGUE37, PS_ROGUE38, PS_ROGUE39, PS_ROGUE40, PS_ROGUE41, PS_ROGUE42, PS_ROGUE43, PS_ROGUE44, PS_ROGUE45, PS_ROGUE46, PS_ROGUE47, PS_ROGUE48, PS_ROGUE49, PS_ROGUE50, PS_ROGUE51, PS_ROGUE52, PS_ROGUE53, PS_ROGUE54, PS_ROGUE55, PS_ROGUE56, PS_ROGUE57, PS_ROGUE58, PS_ROGUE59, PS_ROGUE60, PS_ROGUE61, PS_ROGUE62, PS_ROGUE63, PS_ROGUE64, PS_ROGUE65, PS_ROGUE66, PS_ROGUE67, PS_ROGUE68, PS_ROGUE69, PS_ROGUE70, PS_ROGUE71, PS_ROGUE72, PS_ROGUE73, PS_ROGUE74, PS_ROGUE75, PS_ROGUE76, PS_ROGUE77, PS_ROGUE78, PS_ROGUE79, PS_ROGUE80, PS_ROGUE81, PS_ROGUE82, PS_ROGUE83, PS_ROGUE84, PS_ROGUE85, PS_ROGUE86, PS_ROGUE87, PS_ROGUE88, PS_ROGUE89, PS_ROGUE90, PS_ROGUE91, PS_ROGUE92, PS_ROGUE93, PS_ROGUE94, PS_ROGUE95, PS_ROGUE96,  PS_ROGUE97, PS_ROGUE98, PS_ROGUE99, PS_ROGUE100, PS_ROGUE101, PS_ROGUE102 },
	{ PS_MAGE1,  PS_MAGE2,  PS_MAGE3,  PS_MAGE4,  PS_MAGE5,  PS_MAGE6,  PS_MAGE7,  PS_MAGE8,  PS_MAGE9,  PS_MAGE10,  PS_MAGE11,  PS_MAGE12,  PS_MAGE13,  PS_MAGE14,  PS_MAGE15,  PS_MAGE16,  PS_MAGE17,  PS_MAGE18,  PS_MAGE19,  PS_MAGE20,  PS_MAGE21,  PS_MAGE22,  PS_MAGE23,  PS_MAGE24,  PS_MAGE25,  PS_MAGE26,  PS_MAGE27,  PS_MAGE28,  PS_MAGE29,  PS_MAGE30,  PS_MAGE31,  PS_MAGE32,  PS_MAGE33,  PS_MAGE34,  PS_MAGE35,  PS_MAGE36,  PS_MAGE37,  PS_MAGE38,  PS_MAGE39,  PS_MAGE40,  PS_MAGE41,  PS_MAGE42,  PS_MAGE43,  PS_MAGE44,  PS_MAGE45,  PS_MAGE46,  PS_MAGE47,  PS_MAGE48,  PS_MAGE49,  PS_MAGE50,  PS_MAGE51,  PS_MAGE52,  PS_MAGE53,  PS_MAGE54,  PS_MAGE55,  PS_MAGE56,  PS_MAGE57,  PS_MAGE58,  PS_MAGE59,  PS_MAGE60,  PS_MAGE61,  PS_MAGE62,  PS_MAGE63,  PS_MAGE64,  PS_MAGE65,  PS_MAGE66,  PS_MAGE67,  PS_MAGE68,  PS_MAGE69,  PS_MAGE70,  PS_MAGE71,  PS_MAGE72,  PS_MAGE73,  PS_MAGE74,  PS_MAGE75,  PS_MAGE76,  PS_MAGE77,  PS_MAGE78,  PS_MAGE79,  PS_MAGE80,  PS_MAGE81,  PS_MAGE82,  PS_MAGE83,  PS_MAGE84,  PS_MAGE85,  PS_MAGE86,  PS_MAGE87,  PS_MAGE88,  PS_MAGE89,  PS_MAGE90,  PS_MAGE91,  PS_MAGE92,  PS_MAGE93,  PS_MAGE94,  PS_MAGE95,  PS_MAGE96,   PS_MAGE97,  PS_MAGE98,  PS_MAGE99,  PS_MAGE100,  PS_MAGE101,  PS_MAGE102  },
	{ PS_MONK1,  PS_MONK2,  PS_MONK3,  PS_MONK4,  PS_MONK5,  PS_MONK6,  PS_MONK7,  PS_MONK8,  PS_MONK9,  PS_MONK10,  PS_MONK11,  PS_MONK12,  PS_MONK13,  PS_MONK14,  PS_MONK15,  PS_MONK16,  PS_MONK17,  PS_MONK18,  PS_MONK19,  PS_MONK20,  PS_MONK21,  PS_MONK22,  PS_MONK23,  PS_MONK24,  PS_MONK25,  PS_MONK26,  PS_MONK27,  PS_MONK28,  PS_MONK29,  PS_MONK30,  PS_MONK31,  PS_MONK32,  PS_MONK33,  PS_MONK34,  PS_MONK35,  PS_MONK36,  PS_MONK37,  PS_MONK38,  PS_MONK39,  PS_MONK40,  PS_MONK41,  PS_MONK42,  PS_MONK43,  PS_MONK44,  PS_MONK45,  PS_MONK46,  PS_MONK47,  PS_MONK48,  PS_MONK49,  PS_MONK50,  PS_MONK51,  PS_MONK52,  PS_MONK53,  PS_MONK54,  PS_MONK55,  PS_MONK56,  PS_MONK57,  PS_MONK58,  PS_MONK59,  PS_MONK60,  PS_MONK61,  PS_MONK62,  PS_MONK63,  PS_MONK64,  PS_MONK65,  PS_MONK66,  PS_MONK67,  PS_MONK68,  PS_MONK69,  PS_MONK70,  PS_MONK71,  PS_MONK72,  PS_MONK73,  PS_MONK74,  PS_MONK75,  PS_MONK76,  PS_MONK77,  PS_MONK78,  PS_MONK79,  PS_MONK80,  PS_MONK81,  PS_MONK82,  PS_MONK83,  PS_MONK84,  PS_MONK85,  PS_MONK86,  PS_MONK87,  PS_MONK88,  PS_MONK89,  PS_MONK90,  PS_MONK91,  PS_MONK92,  PS_MONK93,  PS_MONK94,  PS_MONK95,  PS_MONK96,   PS_MONK97,  PS_MONK98,  PS_MONK99,  PS_MONK100,  PS_MONK101,  PS_MONK102  },
	{ PS_ROGUE1, PS_ROGUE2, PS_ROGUE3, PS_ROGUE4, PS_ROGUE5, PS_ROGUE6, PS_ROGUE7, PS_ROGUE8, PS_ROGUE9, PS_ROGUE10, PS_ROGUE11, PS_ROGUE12, PS_ROGUE13, PS_ROGUE14, PS_ROGUE15, PS_ROGUE16, PS_ROGUE17, PS_ROGUE18, PS_ROGUE19, PS_ROGUE20, PS_ROGUE21, PS_ROGUE22, PS_ROGUE23, PS_ROGUE24, PS_ROGUE25, PS_ROGUE26, PS_ROGUE27, PS_ROGUE28, PS_ROGUE29, PS_ROGUE30, PS_ROGUE31, PS_ROGUE32, PS_ROGUE33, PS_ROGUE34, PS_ROGUE35, PS_ROGUE36, PS_ROGUE37, PS_ROGUE38, PS_ROGUE39, PS_ROGUE40, PS_ROGUE41, PS_ROGUE42, PS_ROGUE43, PS_ROGUE44, PS_ROGUE45, PS_ROGUE46, PS_ROGUE47, PS_ROGUE48, PS_ROGUE49, PS_ROGUE50, PS_ROGUE51, PS_ROGUE52, PS_ROGUE53, PS_ROGUE54, PS_ROGUE55, PS_ROGUE56, PS_ROGUE57, PS_ROGUE58, PS_ROGUE59, PS_ROGUE60, PS_ROGUE61, PS_ROGUE62, PS_ROGUE63, PS_ROGUE64, PS_ROGUE65, PS_ROGUE66, PS_ROGUE67, PS_ROGUE68, PS_ROGUE69, PS_ROGUE70, PS_ROGUE71, PS_ROGUE72, PS_ROGUE73, PS_ROGUE74, PS_ROGUE75, PS_ROGUE76, PS_ROGUE77, PS_ROGUE78, PS_ROGUE79, PS_ROGUE80, PS_ROGUE81, PS_ROGUE82, PS_ROGUE83, PS_ROGUE84, PS_ROGUE85, PS_ROGUE86, PS_ROGUE87, PS_ROGUE88, PS_ROGUE89, PS_ROGUE90, PS_ROGUE91, PS_ROGUE92, PS_ROGUE93, PS_ROGUE94, PS_ROGUE95, PS_ROGUE96,  PS_ROGUE97, PS_ROGUE98, PS_ROGUE99, PS_ROGUE100, PS_ROGUE101, PS_ROGUE102 },
	{ PS_WARR1,  PS_WARR2,  PS_WARR3,  PS_WARR4,  PS_WARR5,  PS_WARR6,  PS_WARR7,  PS_WARR8,  PS_WARR9,  PS_WARR10,  PS_WARR11,  PS_WARR12,  PS_WARR13,  PS_WARR14,  PS_WARR15,  PS_WARR16,  PS_WARR17,  PS_WARR18,  PS_WARR19,  PS_WARR20,  PS_WARR21,  PS_WARR22,  PS_WARR23,  PS_WARR24,  PS_WARR25,  PS_WARR26,  PS_WARR27,  PS_WARR28,  PS_WARR29,  PS_WARR30,  PS_WARR31,  PS_WARR32,  PS_WARR33,  PS_WARR34,  PS_WARR35,  PS_WARR36,  PS_WARR37,  PS_WARR38,  PS_WARR39,  PS_WARR40,  PS_WARR41,  PS_WARR42,  PS_WARR43,  PS_WARR44,  PS_WARR45,  PS_WARR46,  PS_WARR47,  PS_WARR48,  PS_WARR49,  PS_WARR50,  PS_WARR51,  PS_WARR52,  PS_WARR53,  PS_WARR54,  PS_WARR55,  PS_WARR56,  PS_WARR57,  PS_WARR58,  PS_WARR59,  PS_WARR60,  PS_WARR61,  PS_WARR62,  PS_WARR63,  PS_WARR64,  PS_WARR65,  PS_WARR66,  PS_WARR67,  PS_WARR68,  PS_WARR69,  PS_WARR70,  PS_WARR71,  PS_WARR72,  PS_WARR73,  PS_WARR74,  PS_WARR75,  PS_WARR76,  PS_WARR77,  PS_WARR78,  PS_WARR79,  PS_WARR80,  PS_WARR81,  PS_WARR82,  PS_WARR83,  PS_WARR84,  PS_WARR85,  PS_WARR86,  PS_WARR87,  PS_WARR88,  PS_WARR89,  PS_WARR90,  PS_WARR91,  PS_WARR92,  PS_WARR93,  PS_WARR94,  PS_WARR95,  PS_WARR96B,  PS_WARR97,  PS_WARR98,  PS_WARR99,  PS_WARR100,  PS_WARR101,  PS_WARR102  },
	// clang-format on
};

void PlayerStruct::PlaySpeach(int speachId) const
{
	_sfx_id soundEffect = herosounds[static_cast<size_t>(_pClass)][speachId - 1];

	PlaySfxLoc(soundEffect, position.tile.x, position.tile.y);
}

void PlayerStruct::PlaySpecificSpeach(int speachId) const
{
	_sfx_id soundEffect = herosounds[static_cast<size_t>(_pClass)][speachId - 1];

	if (effect_is_playing(soundEffect))
		return;

	PlaySfxLoc(soundEffect, position.tile.x, position.tile.y, false);
}

void PlayerStruct::PlaySpeach(int speachId, int delay) const
{
	sfxdelay = delay;
	sfxdnum = herosounds[static_cast<size_t>(_pClass)][speachId - 1];
}

void PlayerStruct::Stop()
{
	ClrPlrPath(*this);
	destAction = ACTION_NONE;
}

bool PlayerStruct::IsWalking() const
{
	switch (_pmode)
	{
	case PM_WALK:
	case PM_WALK2:
	case PM_WALK3:
		return true;
	default:
		return false;
	}
}

void SetPlayerGPtrs(byte *pData, byte **pAnim)
{
	int i;

	for (i = 0; i < 8; i++) {
		pAnim[i] = CelGetFrameStart(pData, i);
	}
}

void LoadPlrGFX(int pnum, player_graphic gfxflag)
{
	char prefix[16];
	char pszName[256];
	const char *szCel;
	byte *pData;
	byte **pAnim;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("LoadPlrGFX: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	HeroClass c = player._pClass;
	if (c == HeroClass::Bard && hfbard_mpq == nullptr) {
		c = HeroClass::Rogue;
	} else if (c == HeroClass::Barbarian && hfbarb_mpq == nullptr) {
		c = HeroClass::Warrior;
	}

	sprintf(prefix, "%c%c%c", CharChar[static_cast<std::size_t>(c)], ArmourChar[player._pgfxnum >> 4], WepChar[player._pgfxnum & 0xF]);
	const char *cs = ClassPathTbl[static_cast<std::size_t>(c)];

	for (unsigned i = 1; i <= PFILE_NONDEATH; i <<= 1) {
		if ((i & gfxflag) == 0) {
			continue;
		}

		switch (i) {
		case PFILE_STAND:
			szCel = "AS";
			if (leveltype == DTYPE_TOWN) {
				szCel = "ST";
			}
			pData = player._pNData;
			pAnim = player._pNAnim;
			break;
		case PFILE_WALK:
			szCel = "AW";
			if (leveltype == DTYPE_TOWN) {
				szCel = "WL";
			}
			pData = player._pWData;
			pAnim = player._pWAnim;
			break;
		case PFILE_ATTACK:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "AT";
			pData = player._pAData;
			pAnim = player._pAAnim;
			break;
		case PFILE_HIT:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "HT";
			pData = player._pHData;
			pAnim = player._pHAnim;
			break;
		case PFILE_LIGHTNING:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "LM";
			pData = player._pLData;
			pAnim = player._pLAnim;
			break;
		case PFILE_FIRE:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "FM";
			pData = player._pFData;
			pAnim = player._pFAnim;
			break;
		case PFILE_MAGIC:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			szCel = "QM";
			pData = player._pTData;
			pAnim = player._pTAnim;
			break;
		case PFILE_DEATH:
			if ((player._pgfxnum & 0xF) != 0) {
				continue;
			}
			szCel = "DT";
			pData = player._pDData;
			pAnim = player._pDAnim;
			break;
		case PFILE_BLOCK:
			if (leveltype == DTYPE_TOWN) {
				continue;
			}
			if (!player._pBlockFlag) {
				continue;
			}

			szCel = "BL";
			pData = player._pBData;
			pAnim = player._pBAnim;
			break;
		default:
			app_fatal("PLR:2");
		}

		sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", cs, prefix, prefix, szCel);
		LoadFileInMem(pszName, pData);
		SetPlayerGPtrs(pData, pAnim);
		player._pGFXLoad |= i;
	}
}

void InitPlayerGFX(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlayerGFX: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player._pHitPoints >> 6 == 0) {
		player._pgfxnum = 0;
		LoadPlrGFX(pnum, PFILE_DEATH);
	} else {
		LoadPlrGFX(pnum, PFILE_NONDEATH);
	}
}

static HeroClass GetPlrGFXClass(HeroClass c)
{
	switch (c) {
	case HeroClass::Bard:
		return hfbard_mpq == nullptr ? HeroClass::Rogue : c;
	case HeroClass::Barbarian:
		return hfbarb_mpq == nullptr ? HeroClass::Warrior : c;
	default:
		return c;
	}
}

static DWORD GetPlrGFXSize(HeroClass c, const char *szCel)
{
	const char *a, *w;
	DWORD dwSize, dwMaxSize;
	HANDLE hsFile;
	char pszName[256];
	char Type[16];

	c = GetPlrGFXClass(c);
	dwMaxSize = 0;

	const auto hasBlockAnimation = [c](char w) {
		return w == 'D' || w == 'U' || w == 'H'
		    || (c == HeroClass::Monk && (w == 'S' || w == 'M' || w == 'N' || w == 'T'));
	};

	for (a = &ArmourChar[0]; *a; a++) {
		if (gbIsSpawn && a != &ArmourChar[0])
			break;
		for (w = &WepChar[0]; *w; w++) {
			if (szCel[0] == 'D' && szCel[1] == 'T' && *w != 'N') {
				continue; //Death has no weapon
			}
			if (szCel[0] == 'B' && szCel[1] == 'L' && !hasBlockAnimation(*w)) {
				continue; // No block animation
			}
			sprintf(Type, "%c%c%c", CharChar[static_cast<std::size_t>(c)], *a, *w);
			sprintf(pszName, "PlrGFX\\%s\\%s\\%s%s.CL2", ClassPathTbl[static_cast<std::size_t>(c)], Type, Type, szCel);
			if (SFileOpenFile(pszName, &hsFile)) {
				assert(hsFile);
				dwSize = SFileGetFileSize(hsFile);
				SFileCloseFileThreadSafe(hsFile);
				if (dwMaxSize <= dwSize) {
					dwMaxSize = dwSize;
				}
			}
		}
	}

	return dwMaxSize;
}

void InitPlrGFXMem(PlayerStruct &player)
{
	const HeroClass c = player._pClass;

	// STAND (ST: TOWN, AS: DUNGEON)
	player._pNData = new byte[std::max(GetPlrGFXSize(c, "ST"), GetPlrGFXSize(c, "AS"))];

	// WALK (WL: TOWN, AW: DUNGEON)
	player._pWData = new byte[std::max(GetPlrGFXSize(c, "WL"), GetPlrGFXSize(c, "AW"))];

	// ATTACK
	player._pAData = new byte[GetPlrGFXSize(c, "AT")];

	// HIT
	player._pHData = new byte[GetPlrGFXSize(c, "HT")];

	// LIGHTNING
	player._pLData = new byte[GetPlrGFXSize(c, "LM")];

	// FIRE
	player._pFData = new byte[GetPlrGFXSize(c, "FM")];

	// MAGIC
	player._pTData = new byte[GetPlrGFXSize(c, "QM")];

	// DEATH
	player._pDData = new byte[GetPlrGFXSize(c, "DT")];

	// BLOCK
	player._pBData = new byte[GetPlrGFXSize(c, "BL")];

	player._pGFXLoad = 0;
}

void FreePlayerGFX(PlayerStruct &player)
{
	delete[] player._pNData;
	player._pNData = nullptr;
	delete[] player._pWData;
	player._pWData = nullptr;
	delete[] player._pAData;
	player._pAData = nullptr;
	delete[] player._pHData;
	player._pHData = nullptr;
	delete[] player._pLData;
	player._pLData = nullptr;
	delete[] player._pFData;
	player._pFData = nullptr;
	delete[] player._pTData;
	player._pTData = nullptr;
	delete[] player._pDData;
	player._pDData = nullptr;
	delete[] player._pBData;
	player._pBData = nullptr;
	player._pGFXLoad = 0;
}

void NewPlrAnim(PlayerStruct &player, byte *pData, int numberOfFrames, int delayLen, int width, AnimationDistributionFlags flags /*= AnimationDistributionFlags::None*/, int numSkippedFrames /*= 0*/, int distributeFramesBeforeFrame /*= 0*/)
{
	player._pAnimWidth = width;
	player.AnimInfo.SetNewAnimation(pData, numberOfFrames, delayLen, flags, numSkippedFrames, distributeFramesBeforeFrame);
}

static void ClearPlrPVars(PlayerStruct &player)
{
	player.position.temp = { 0, 0 };
	player.tempDirection = DIR_S;
	player._pVar4 = 0;
	player._pVar5 = 0;
	player.position.offset2 = { 0, 0 };
	player.deathFrame = 0;
}

void SetPlrAnims(PlayerStruct &player)
{
	player._pNWidth = 96;
	player._pWWidth = 96;
	player._pAWidth = 128;
	player._pHWidth = 96;
	player._pSWidth = 96;
	player._pDWidth = 128;
	player._pBWidth = 96;

	HeroClass pc = player._pClass;

	if (leveltype == DTYPE_TOWN) {
		player._pNFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][7];
		player._pWFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][8];
		player._pDFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][4];
		player._pSFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][5];
	} else {
		player._pNFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][0];
		player._pWFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][2];
		player._pAFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][1];
		player._pHFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][6];
		player._pSFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][5];
		player._pDFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][4];
		player._pBFrames = PlrGFXAnimLens[static_cast<std::size_t>(pc)][3];
		player._pAFNum = PlrGFXAnimLens[static_cast<std::size_t>(pc)][9];
	}
	player._pSFNum = PlrGFXAnimLens[static_cast<std::size_t>(pc)][10];

	auto gn = static_cast<anim_weapon_id>(player._pgfxnum & 0xF);
	if (pc == HeroClass::Warrior) {
		if (gn == ANIM_ID_BOW) {
			if (leveltype != DTYPE_TOWN) {
				player._pNFrames = 8;
			}
			player._pAWidth = 96;
			player._pAFNum = 11;
		} else if (gn == ANIM_ID_AXE) {
			player._pAFrames = 20;
			player._pAFNum = 10;
		} else if (gn == ANIM_ID_STAFF) {
			player._pAFrames = 16;
			player._pAFNum = 11;
		}
	} else if (pc == HeroClass::Rogue) {
		if (gn == ANIM_ID_AXE) {
			player._pAFrames = 22;
			player._pAFNum = 13;
		} else if (gn == ANIM_ID_BOW) {
			player._pAFrames = 12;
			player._pAFNum = 7;
		} else if (gn == ANIM_ID_STAFF) {
			player._pAFrames = 16;
			player._pAFNum = 11;
		}
	} else if (pc == HeroClass::Sorcerer) {
		player._pSWidth = 128;
		if (gn == ANIM_ID_UNARMED) {
			player._pAFrames = 20;
		} else if (gn == ANIM_ID_UNARMED_SHIELD) {
			player._pAFNum = 9;
		} else if (gn == ANIM_ID_BOW) {
			player._pAFrames = 20;
			player._pAFNum = 16;
		} else if (gn == ANIM_ID_AXE) {
			player._pAFrames = 24;
			player._pAFNum = 16;
		}
	} else if (pc == HeroClass::Monk) {
		player._pNWidth = 112;
		player._pWWidth = 112;
		player._pAWidth = 130;
		player._pHWidth = 98;
		player._pSWidth = 114;
		player._pDWidth = 160;
		player._pBWidth = 98;

		switch (gn) {
		case ANIM_ID_UNARMED:
		case ANIM_ID_UNARMED_SHIELD:
			player._pAFrames = 12;
			player._pAFNum = 7;
			break;
		case ANIM_ID_BOW:
			player._pAFrames = 20;
			player._pAFNum = 14;
			break;
		case ANIM_ID_AXE:
			player._pAFrames = 23;
			player._pAFNum = 14;
			break;
		case ANIM_ID_STAFF:
			player._pAFrames = 13;
			player._pAFNum = 8;
			break;
		default:
			break;
		}
	} else if (pc == HeroClass::Bard) {
		if (gn == ANIM_ID_AXE) {
			player._pAFrames = 22;
			player._pAFNum = 13;
		} else if (gn == ANIM_ID_BOW) {
			player._pAFrames = 12;
			player._pAFNum = 11;
		} else if (gn == ANIM_ID_STAFF) {
			player._pAFrames = 16;
			player._pAFNum = 11;
		}
	} else if (pc == HeroClass::Barbarian) {
		if (gn == ANIM_ID_AXE) {
			player._pAFrames = 20;
			player._pAFNum = 8;
		} else if (gn == ANIM_ID_BOW) {
			if (leveltype != DTYPE_TOWN) {
				player._pNFrames = 8;
			}
			player._pAWidth = 96;
			player._pAFNum = 11;
		} else if (gn == ANIM_ID_STAFF) {
			player._pAFrames = 16;
			player._pAFNum = 11;
		} else if (gn == ANIM_ID_MACE || gn == ANIM_ID_MACE_SHIELD) {
			player._pAFNum = 8;
		}
	}
}

/**
 * @param c The hero class.
 */
void CreatePlayer(int pnum, HeroClass c)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("CreatePlayer: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	memset(&player, 0, sizeof(PlayerStruct));
	SetRndSeed(SDL_GetTicks());

	player._pClass = c;

	player._pBaseStr = StrengthTbl[static_cast<std::size_t>(c)];
	player._pStrength = player._pBaseStr;

	player._pBaseMag = MagicTbl[static_cast<std::size_t>(c)];
	player._pMagic = player._pBaseMag;

	player._pBaseDex = DexterityTbl[static_cast<std::size_t>(c)];
	player._pDexterity = player._pBaseDex;

	player._pBaseVit = VitalityTbl[static_cast<std::size_t>(c)];
	player._pVitality = player._pBaseVit;

	player._pStatPts = 0;
	player.pTownWarps = 0;
	player.pDungMsgs = 0;
	player.pDungMsgs2 = 0;
	player.pLvlLoad = 0;
	player.pDiabloKillLevel = 0;
	player.pDifficulty = DIFF_NORMAL;

	player._pLevel = 1;

	if (player._pClass == HeroClass::Monk) {
		player._pDamageMod = (player._pStrength + player._pDexterity) * player._pLevel / 150;
	} else if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Bard) {
		player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 200;
	} else {
		player._pDamageMod = player._pStrength * player._pLevel / 100;
	}

	player._pBaseToBlk = ToBlkTbl[static_cast<std::size_t>(c)];

	player._pHitPoints = (player._pVitality + 10) << 6;
	if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Barbarian) {
		player._pHitPoints *= 2;
	} else if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk || player._pClass == HeroClass::Bard) {
		player._pHitPoints += player._pHitPoints / 2;
	}

	player._pMaxHP = player._pHitPoints;
	player._pHPBase = player._pHitPoints;
	player._pMaxHPBase = player._pHitPoints;

	player._pMana = player._pMagic << 6;
	if (player._pClass == HeroClass::Sorcerer) {
		player._pMana *= 2;
	} else if (player._pClass == HeroClass::Bard) {
		player._pMana += player._pMana * 3 / 4;
	} else if (player._pClass == HeroClass::Rogue || player._pClass == HeroClass::Monk) {
		player._pMana += player._pMana / 2;
	}

	player._pMaxMana = player._pMana;
	player._pManaBase = player._pMana;
	player._pMaxManaBase = player._pMana;

	player._pMaxLvl = player._pLevel;
	player._pExperience = 0;
	player._pMaxExp = player._pExperience;
	player._pNextExper = ExpLvlsTbl[1];
	player._pArmorClass = 0;
	if (player._pClass == HeroClass::Barbarian) {
		player._pMagResist = 1;
		player._pFireResist = 1;
		player._pLghtResist = 1;
	} else {
		player._pMagResist = 0;
		player._pFireResist = 0;
		player._pLghtResist = 0;
	}
	player._pLightRad = 10;
	player._pInfraFlag = false;

	player._pRSplType = RSPLTYPE_SKILL;
	if (c == HeroClass::Warrior) {
		player._pAblSpells = GetSpellBitmask(SPL_REPAIR);
		player._pRSpell = SPL_REPAIR;
	} else if (c == HeroClass::Rogue) {
		player._pAblSpells = GetSpellBitmask(SPL_DISARM);
		player._pRSpell = SPL_DISARM;
	} else if (c == HeroClass::Sorcerer) {
		player._pAblSpells = GetSpellBitmask(SPL_RECHARGE);
		player._pRSpell = SPL_RECHARGE;
	} else if (c == HeroClass::Monk) {
		player._pAblSpells = GetSpellBitmask(SPL_SEARCH);
		player._pRSpell = SPL_SEARCH;
	} else if (c == HeroClass::Bard) {
		player._pAblSpells = GetSpellBitmask(SPL_IDENTIFY);
		player._pRSpell = SPL_IDENTIFY;
	} else if (c == HeroClass::Barbarian) {
		player._pAblSpells = GetSpellBitmask(SPL_BLODBOIL);
		player._pRSpell = SPL_BLODBOIL;
	}

	if (c == HeroClass::Sorcerer) {
		player._pMemSpells = GetSpellBitmask(SPL_FIREBOLT);
		player._pRSplType = RSPLTYPE_SPELL;
		player._pRSpell = SPL_FIREBOLT;
	} else {
		player._pMemSpells = 0;
	}

	for (int8_t &spellLevel : player._pSplLvl) {
		spellLevel = 0;
	}

	player._pSpellFlags = 0;

	if (player._pClass == HeroClass::Sorcerer) {
		player._pSplLvl[SPL_FIREBOLT] = 2;
	}

	// interestingly, only the first three hotkeys are reset
	// TODO: BUGFIX: clear all 4 hotkeys instead of 3 (demo leftover)
	for (int i = 0; i < 3; i++) {
		player._pSplHotKey[i] = SPL_INVALID;
	}

	if (c == HeroClass::Warrior) {
		player._pgfxnum = ANIM_ID_SWORD_SHIELD;
	} else if (c == HeroClass::Rogue) {
		player._pgfxnum = ANIM_ID_BOW;
	} else if (c == HeroClass::Sorcerer) {
		player._pgfxnum = ANIM_ID_STAFF;
	} else if (c == HeroClass::Monk) {
		player._pgfxnum = ANIM_ID_STAFF;
	} else if (c == HeroClass::Bard) {
		player._pgfxnum = ANIM_ID_SWORD_SHIELD;
	} else if (c == HeroClass::Barbarian) {
		player._pgfxnum = ANIM_ID_SWORD_SHIELD;
	}

	for (bool &levelVisited : player._pLvlVisited) {
		levelVisited = false;
	}

	for (int i = 0; i < 10; i++) {
		player._pSLvlVisited[i] = false;
	}

	player._pLvlChanging = false;
	player.pTownWarps = 0;
	player.pLvlLoad = 0;
	player.pBattleNet = false;
	player.pManaShield = false;
	player.pDamAcFlags = 0;
	player.wReflections = 0;

	InitDungMsgs(player);
	CreatePlrItems(pnum);
	SetRndSeed(0);
}

int CalcStatDiff(PlayerStruct &player)
{
	return player.GetMaximumAttributeValue(CharacterAttribute::Strength)
	    - player._pBaseStr
	    + player.GetMaximumAttributeValue(CharacterAttribute::Magic)
	    - player._pBaseMag
	    + player.GetMaximumAttributeValue(CharacterAttribute::Dexterity)
	    - player._pBaseDex
	    + player.GetMaximumAttributeValue(CharacterAttribute::Vitality)
	    - player._pBaseVit;
}

void NextPlrLevel(int pnum)
{
	int hp, mana;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("NextPlrLevel: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	player._pLevel++;
	player._pMaxLvl++;

	CalcPlrInv(pnum, true);

	if (CalcStatDiff(player) < 5) {
		player._pStatPts = CalcStatDiff(player);
	} else {
		player._pStatPts += 5;
	}

	player._pNextExper = ExpLvlsTbl[player._pLevel];

	hp = player._pClass == HeroClass::Sorcerer ? 64 : 128;
	if (!gbIsMultiplayer) {
		hp++;
	}
	player._pMaxHP += hp;
	player._pHitPoints = player._pMaxHP;
	player._pMaxHPBase += hp;
	player._pHPBase = player._pMaxHPBase;

	if (pnum == myplr) {
		drawhpflag = true;
	}

	if (player._pClass == HeroClass::Warrior)
		mana = 64;
	else if (player._pClass == HeroClass::Barbarian)
		mana = 0;
	else
		mana = 128;

	if (!gbIsMultiplayer) {
		mana++;
	}
	player._pMaxMana += mana;
	player._pMaxManaBase += mana;

	if ((player._pIFlags & ISPL_NOMANA) == 0) {
		player._pMana = player._pMaxMana;
		player._pManaBase = player._pMaxManaBase;
	}

	if (pnum == myplr) {
		drawmanaflag = true;
	}

	if (sgbControllerActive)
		FocusOnCharInfo();

	CalcPlrInv(pnum, true);
}

#define MAXEXP 2000000000

void AddPlrExperience(int pnum, int lvl, int exp)
{
	int powerLvlCap, expCap, newLvl, i;

	if (pnum != myplr) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("AddPlrExperience: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player._pHitPoints <= 0) {
		return;
	}

	// Adjust xp based on difference in level between player and monster
	exp *= 1 + ((double)lvl - player._pLevel) / 10;
	if (exp < 0) {
		exp = 0;
	}

	// Prevent power leveling
	if (gbIsMultiplayer) {
		powerLvlCap = player._pLevel < 0 ? 0 : player._pLevel;
		if (powerLvlCap >= 50) {
			powerLvlCap = 50;
		}
		// cap to 1/20 of current levels xp
		if (exp >= ExpLvlsTbl[powerLvlCap] / 20) {
			exp = ExpLvlsTbl[powerLvlCap] / 20;
		}
		// cap to 200 * current level
		expCap = 200 * powerLvlCap;
		if (exp >= expCap) {
			exp = expCap;
		}
	}

	player._pExperience += exp;
	if ((DWORD)player._pExperience > MAXEXP) {
		player._pExperience = MAXEXP;
	}

	if (sgOptions.Gameplay.bExperienceBar) {
		force_redraw = 255;
	}

	if (player._pExperience >= ExpLvlsTbl[49]) {
		player._pLevel = 50;
		return;
	}

	// Increase player level if applicable
	newLvl = 0;
	while (player._pExperience >= ExpLvlsTbl[newLvl]) {
		newLvl++;
	}
	if (newLvl != player._pLevel) {
		for (i = newLvl - player._pLevel; i > 0; i--) {
			NextPlrLevel(pnum);
		}
	}

	NetSendCmdParam1(false, CMD_PLRLEVEL, player._pLevel);
}

void AddPlrMonstExper(int lvl, int exp, char pmask)
{
	int totplrs, i, e;

	totplrs = 0;
	for (i = 0; i < MAX_PLRS; i++) {
		if (((1 << i) & pmask) != 0) {
			totplrs++;
		}
	}

	if (totplrs) {
		e = exp / totplrs;
		if ((pmask & (1 << myplr)) != 0)
			AddPlrExperience(myplr, lvl, e);
	}
}

void InitPlayer(int pnum, bool FirstTime)
{
	DWORD i;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %i", pnum);
	}
	auto &player = plr[pnum];
	auto &myPlayer = plr[myplr];

	if (FirstTime) {
		player._pRSplType = RSPLTYPE_INVALID;
		player._pRSpell = SPL_INVALID;
		if (pnum == myplr)
			LoadHotkeys();
		player._pSBkSpell = SPL_INVALID;
		player._pSpell = player._pRSpell;
		player._pSplType = player._pRSplType;
		if ((player._pgfxnum & 0xF) == ANIM_ID_BOW) {
			player._pwtype = WT_RANGED;
		} else {
			player._pwtype = WT_MELEE;
		}
		player.pManaShield = false;
	}

	if (player.plrlevel == currlevel || leveldebug) {

		SetPlrAnims(player);

		player.position.offset = { 0, 0 };
		player.position.velocity = { 0, 0 };

		ClearPlrPVars(player);

		if (player._pHitPoints >> 6 > 0) {
			player._pmode = PM_STAND;
			NewPlrAnim(player, player._pNAnim[DIR_S], player._pNFrames, 3, player._pNWidth);
			player.AnimInfo.CurrentFrame = GenerateRnd(player._pNFrames - 1) + 1;
			player.AnimInfo.DelayCounter = GenerateRnd(3);
		} else {
			player._pmode = PM_DEATH;
			NewPlrAnim(player, player._pDAnim[DIR_S], player._pDFrames, 1, player._pDWidth);
			player.AnimInfo.CurrentFrame = player.AnimInfo.NumberOfFrames - 1;
		}

		player._pdir = DIR_S;

		if (pnum == myplr) {
			if (!FirstTime || currlevel != 0) {
				player.position.tile = { ViewX, ViewY };
			}
		} else {
			for (i = 0; i < 8 && !PosOkPlayer(pnum, plrxoff2[i] + player.position.tile.x, plryoff2[i] + player.position.tile.y); i++)
				;
			player.position.tile.x += plrxoff2[i];
			player.position.tile.y += plryoff2[i];
		}

		player.position.future = player.position.tile;
		player.walkpath[0] = WALK_NONE;
		player.destAction = ACTION_NONE;

		if (pnum == myplr) {
			player._plid = AddLight(player.position.tile.x, player.position.tile.y, player._pLightRad);
			ChangeLightXY(myPlayer._plid, myPlayer.position.tile.x, myPlayer.position.tile.y); // fix for a bug where old light is still visible at the entrance after reentering level
		} else {
			player._plid = NO_LIGHT;
		}
		player._pvid = AddVision(player.position.tile.x, player.position.tile.y, player._pLightRad, pnum == myplr);
	}

	if (player._pClass == HeroClass::Warrior) {
		player._pAblSpells = GetSpellBitmask(SPL_REPAIR);
	} else if (player._pClass == HeroClass::Rogue) {
		player._pAblSpells = GetSpellBitmask(SPL_DISARM);
	} else if (player._pClass == HeroClass::Sorcerer) {
		player._pAblSpells = GetSpellBitmask(SPL_RECHARGE);
	} else if (player._pClass == HeroClass::Monk) {
		player._pAblSpells = GetSpellBitmask(SPL_SEARCH);
	} else if (player._pClass == HeroClass::Bard) {
		player._pAblSpells = GetSpellBitmask(SPL_IDENTIFY);
	} else if (player._pClass == HeroClass::Barbarian) {
		player._pAblSpells = GetSpellBitmask(SPL_BLODBOIL);
	}

#ifdef _DEBUG
	if (debug_mode_dollar_sign && FirstTime) {
		player._pMemSpells |= 1 << (SPL_TELEPORT - 1);
		if (!myPlayer._pSplLvl[SPL_TELEPORT]) {
			myPlayer._pSplLvl[SPL_TELEPORT] = 1;
		}
	}
	if (debug_mode_key_inverted_v && FirstTime) {
		player._pMemSpells = SPL_INVALID;
	}
#endif

	player._pNextExper = ExpLvlsTbl[player._pLevel];
	player._pInvincible = false;

	if (pnum == myplr) {
		deathdelay = 0;
		deathflag = false;
		ScrollInfo.offset = { 0, 0 };
		ScrollInfo._sdir = SDIR_NONE;
	}
}

void InitMultiView()
{
	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("InitPlayer: illegal player %i", myplr);
	}
	auto &myPlayer = plr[myplr];

	ViewX = myPlayer.position.tile.x;
	ViewY = myPlayer.position.tile.y;
}

bool SolidLoc(int x, int y)
{
	if (x < 0 || y < 0 || x >= MAXDUNX || y >= MAXDUNY) {
		return false;
	}

	return nSolidTable[dPiece[x][y]];
}

bool PlrDirOK(int pnum, int dir)
{
	int px, py;
	bool isOk;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrDirOK: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	px = player.position.tile.x + offset_x[dir];
	py = player.position.tile.y + offset_y[dir];

	if (px < 0 || !dPiece[px][py] || !PosOkPlayer(pnum, px, py)) {
		return false;
	}

	isOk = true;
	if (dir == DIR_E) {
		isOk = !SolidLoc(px, py + 1) && !(dFlags[px][py + 1] & BFLAG_PLAYERLR);
	}

	if (isOk && dir == DIR_W) {
		isOk = !SolidLoc(px + 1, py) && !(dFlags[px + 1][py] & BFLAG_PLAYERLR);
	}

	return isOk;
}

void PlrClrTrans(int x, int y)
{
	int i, j;

	for (i = y - 1; i <= y + 1; i++) {
		for (j = x - 1; j <= x + 1; j++) {
			TransList[dTransVal[j][i]] = false;
		}
	}
}

void PlrDoTrans(int x, int y)
{
	int i, j;

	if (leveltype != DTYPE_CATHEDRAL && leveltype != DTYPE_CATACOMBS) {
		TransList[1] = true;
	} else {
		for (i = y - 1; i <= y + 1; i++) {
			for (j = x - 1; j <= x + 1; j++) {
				if (!nSolidTable[dPiece[j][i]] && dTransVal[j][i]) {
					TransList[dTransVal[j][i]] = true;
				}
			}
		}
	}
}

void SetPlayerOld(PlayerStruct &player)
{
	player.position.old = player.position.tile;
}

void FixPlayerLocation(int pnum, direction bDir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("FixPlayerLocation: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	player.position.future = player.position.tile;
	player.position.offset = { 0, 0 };
	player._pdir = bDir;
	if (pnum == myplr) {
		ScrollInfo.offset = { 0, 0 };
		ScrollInfo._sdir = SDIR_NONE;
		ViewX = player.position.tile.x;
		ViewY = player.position.tile.y;
	}
	ChangeLightXY(player._plid, player.position.tile.x, player.position.tile.y);
	ChangeVisionXY(player._pvid, player.position.tile.x, player.position.tile.y);
}

void StartStand(int pnum, direction dir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartStand: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (!player._pInvincible || player._pHitPoints != 0 || pnum != myplr) {
		if ((player._pGFXLoad & PFILE_STAND) == 0) {
			LoadPlrGFX(pnum, PFILE_STAND);
		}

		NewPlrAnim(player, player._pNAnim[dir], player._pNFrames, 3, player._pNWidth);
		player._pmode = PM_STAND;
		FixPlayerLocation(pnum, dir);
		FixPlrWalkTags(pnum);
		dPlayer[player.position.tile.x][player.position.tile.y] = pnum + 1;
		SetPlayerOld(player);
	} else {
		SyncPlrKill(pnum, -1);
	}
}

void StartWalkStand(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartWalkStand: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	player._pmode = PM_STAND;
	player.position.future = player.position.tile;
	player.position.offset = { 0, 0 };

	if (pnum == myplr) {
		ScrollInfo.offset = { 0, 0 };
		ScrollInfo._sdir = SDIR_NONE;
		ViewX = player.position.tile.x;
		ViewY = player.position.tile.y;
	}
}

static void PM_ChangeLightOff(PlayerStruct &player)
{
	if (player._plid == NO_LIGHT)
		return;

	const LightListStruct *l = &LightList[player._plid];
	int x = 2 * player.position.offset.y + player.position.offset.x;
	int y = 2 * player.position.offset.y - player.position.offset.x;

	x = (x / 8) * (x < 0 ? 1 : -1);
	y = (y / 8) * (y < 0 ? 1 : -1);
	int lx = x + (l->position.tile.x * 8);
	int ly = y + (l->position.tile.y * 8);
	int offx = l->position.offset.x + (l->position.tile.x * 8);
	int offy = l->position.offset.y + (l->position.tile.y * 8);

	if (abs(lx - offx) < 3 && abs(ly - offy) < 3)
		return;

	ChangeLightOff(player._plid, x, y);
}

void PM_ChangeOffset(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_ChangeOffset: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	int px = player.position.offset2.x / 256;
	int py = player.position.offset2.y / 256;

	player.position.offset2 += player.position.velocity;

	if (currlevel == 0 && sgGameInitInfo.bRunInTown) {
		player.position.offset2 += player.position.velocity;
	}

	player.position.offset = { player.position.offset2.x >> 8, player.position.offset2.y >> 8 };

	px -= player.position.offset2.x >> 8;
	py -= player.position.offset2.y >> 8;

	if (pnum == myplr && ScrollInfo._sdir) {
		ScrollInfo.offset.x += px;
		ScrollInfo.offset.y += py;
	}

	PM_ChangeLightOff(player);
}

/**
 * @brief Start moving a player to a new tile
 */
void StartWalk(int pnum, int xvel, int yvel, int xoff, int yoff, int xadd, int yadd, int mapx, int mapy, direction EndDir, _scroll_direction sdir, int variant, bool pmWillBeCalled)
{
	auto &player = plr[pnum];

	if (player._pInvincible && player._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	SetPlayerOld(player);

	if (!PlrDirOK(pnum, EndDir)) {
		return;
	}

	//The player's tile position after finishing this movement action
	int px = xadd + player.position.tile.x;
	int py = yadd + player.position.tile.y;
	player.position.future = { px, py };

	//If this is the local player then update the camera offset position
	if (pnum == myplr) {
		ScrollInfo.tile.x = player.position.tile.x - ViewX;
		ScrollInfo.tile.y = player.position.tile.y - ViewY;
	}

	switch (variant) {
	case PM_WALK:
		dPlayer[px][py] = -(pnum + 1);
		player._pmode = PM_WALK;
		player.position.velocity = { xvel, yvel };
		player.position.offset = { 0, 0 };
		player.position.temp = { xadd, yadd };
		player.tempDirection = EndDir;

		player.position.offset2 = { 0, 0 };
		break;
	case PM_WALK2:
		dPlayer[player.position.tile.x][player.position.tile.y] = -(pnum + 1);
		player.position.temp = player.position.tile;
		player.position.tile = { px, py }; // Move player to the next tile to maintain correct render order
		dPlayer[player.position.tile.x][player.position.tile.y] = pnum + 1;
		player.position.offset = { xoff, yoff }; // Offset player sprite to align with their previous tile position

		ChangeLightXY(player._plid, player.position.tile.x, player.position.tile.y);
		PM_ChangeLightOff(player);

		player._pmode = PM_WALK2;
		player.position.velocity = { xvel, yvel };
		player.position.offset2 = { xoff * 256, yoff * 256 };
		player.tempDirection = EndDir;
		break;
	case PM_WALK3:
		int x = mapx + player.position.tile.x;
		int y = mapy + player.position.tile.y;

		dPlayer[player.position.tile.x][player.position.tile.y] = -(pnum + 1);
		dPlayer[px][py] = -(pnum + 1);
		player._pVar4 = x;
		player._pVar5 = y;
		dFlags[x][y] |= BFLAG_PLAYERLR;
		player.position.offset = { xoff, yoff }; // Offset player sprite to align with their previous tile position

		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(player._plid, x, y);
			PM_ChangeLightOff(player);
		}

		player._pmode = PM_WALK3;
		player.position.velocity = { xvel, yvel };
		player.position.temp = { px, py };
		player.position.offset2 = { xoff * 256, yoff * 256 };
		player.tempDirection = EndDir;
		break;
	}

	//Load walk animation in case it's not loaded yet
	if ((player._pGFXLoad & PFILE_WALK) == 0) {
		LoadPlrGFX(pnum, PFILE_WALK);
	}

	//Start walk animation
	int skippedFrames = -2;
	if (currlevel == 0 && sgGameInitInfo.bRunInTown)
		skippedFrames = 2;
	if (pmWillBeCalled)
		skippedFrames += 1;
	NewPlrAnim(player, player._pWAnim[EndDir], player._pWFrames, 0, player._pWWidth, AnimationDistributionFlags::ProcessAnimationPending, skippedFrames);

	player._pdir = EndDir;

	if (pnum != myplr) {
		return;
	}

	if (zoomflag) {
		if (abs(ScrollInfo.tile.x) >= 3 || abs(ScrollInfo.tile.y) >= 3) {
			ScrollInfo._sdir = SDIR_NONE;
		} else {
			ScrollInfo._sdir = sdir;
		}
	} else if (abs(ScrollInfo.tile.x) >= 2 || abs(ScrollInfo.tile.y) >= 2) {
		ScrollInfo._sdir = SDIR_NONE;
	} else {
		ScrollInfo._sdir = sdir;
	}
}

void StartAttack(int pnum, direction d)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartAttack: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player._pInvincible && player._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if ((player._pGFXLoad & PFILE_ATTACK) == 0) {
		LoadPlrGFX(pnum, PFILE_ATTACK);
	}

	int skippedAnimationFrames = 0;
	if ((player._pIFlags & ISPL_FASTERATTACK) != 0) {
		// The combination of Faster and Fast Attack doesn't result in more skipped skipped frames, cause the secound frame skip of Faster Attack is not triggered.
		skippedAnimationFrames = 2;
	} else if ((player._pIFlags & ISPL_FASTATTACK) != 0) {
		skippedAnimationFrames = 1;
	} else if ((player._pIFlags & ISPL_FASTESTATTACK) != 0) {
		// Fastest Attack is skipped if Fast or Faster Attack is also specified, cause both skip the frame that triggers fastest attack skipping
		skippedAnimationFrames = 2;
	}

	auto animationFlags = AnimationDistributionFlags::ProcessAnimationPending;
	if (player._pmode == PM_ATTACK)
		animationFlags = static_cast<AnimationDistributionFlags>(animationFlags | AnimationDistributionFlags::RepeatedAction);
	NewPlrAnim(player, player._pAAnim[d], player._pAFrames, 0, player._pAWidth, animationFlags, skippedAnimationFrames, player._pAFNum);
	player._pmode = PM_ATTACK;
	FixPlayerLocation(pnum, d);
	SetPlayerOld(player);
}

void StartRangeAttack(int pnum, direction d, int cx, int cy)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartRangeAttack: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player._pInvincible && player._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if ((player._pGFXLoad & PFILE_ATTACK) == 0) {
		LoadPlrGFX(pnum, PFILE_ATTACK);
	}

	int skippedAnimationFrames = 0;
	if (!gbIsHellfire) {
		if ((player._pIFlags & ISPL_FASTATTACK) != 0) {
			skippedAnimationFrames += 1;
		}
	}

	auto animationFlags = AnimationDistributionFlags::ProcessAnimationPending;
	if (player._pmode == PM_RATTACK)
		animationFlags = static_cast<AnimationDistributionFlags>(animationFlags | AnimationDistributionFlags::RepeatedAction);
	NewPlrAnim(player, player._pAAnim[d], player._pAFrames, 0, player._pAWidth, animationFlags, skippedAnimationFrames, player._pAFNum);

	player._pmode = PM_RATTACK;
	FixPlayerLocation(pnum, d);
	SetPlayerOld(player);
	player.position.temp = { cx, cy };
}

void StartPlrBlock(int pnum, direction dir)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlrBlock: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player._pInvincible && player._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	PlaySfxLoc(IS_ISWORD, player.position.tile.x, player.position.tile.y);

	if ((player._pGFXLoad & PFILE_BLOCK) == 0) {
		LoadPlrGFX(pnum, PFILE_BLOCK);
	}

	int skippedAnimationFrames = 0;
	if ((player._pIFlags & ISPL_FASTBLOCK) != 0) {
		skippedAnimationFrames = (player._pBFrames - 2); // ISPL_FASTBLOCK means we cancel the animation if frame 2 was shown
	}

	NewPlrAnim(player, player._pBAnim[dir], player._pBFrames, 2, player._pBWidth, AnimationDistributionFlags::SkipsDelayOfLastFrame, skippedAnimationFrames);

	player._pmode = PM_BLOCK;
	FixPlayerLocation(pnum, dir);
	SetPlayerOld(player);
}

void StartSpell(int pnum, direction d, int cx, int cy)
{
	if ((DWORD)pnum >= MAX_PLRS)
		app_fatal("StartSpell: illegal player %i", pnum);
	auto &player = plr[pnum];

	if (player._pInvincible && player._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	if (leveltype != DTYPE_TOWN) {
		auto animationFlags = AnimationDistributionFlags::ProcessAnimationPending;
		if (player._pmode == PM_SPELL)
			animationFlags = static_cast<AnimationDistributionFlags>(animationFlags | AnimationDistributionFlags::RepeatedAction);

		switch (spelldata[player._pSpell].sType) {
		case STYPE_FIRE:
			if ((player._pGFXLoad & PFILE_FIRE) == 0) {
				LoadPlrGFX(pnum, PFILE_FIRE);
			}
			NewPlrAnim(player, player._pFAnim[d], player._pSFrames, 0, player._pSWidth, animationFlags, 0, player._pSFNum);
			break;
		case STYPE_LIGHTNING:
			if ((player._pGFXLoad & PFILE_LIGHTNING) == 0) {
				LoadPlrGFX(pnum, PFILE_LIGHTNING);
			}
			NewPlrAnim(player, player._pLAnim[d], player._pSFrames, 0, player._pSWidth, animationFlags, 0, player._pSFNum);
			break;
		case STYPE_MAGIC:
			if ((player._pGFXLoad & PFILE_MAGIC) == 0) {
				LoadPlrGFX(pnum, PFILE_MAGIC);
			}
			NewPlrAnim(player, player._pTAnim[d], player._pSFrames, 0, player._pSWidth, animationFlags, 0, player._pSFNum);
			break;
		}
	}

	PlaySfxLoc(spelldata[player._pSpell].sSFX, player.position.tile.x, player.position.tile.y);

	player._pmode = PM_SPELL;

	FixPlayerLocation(pnum, d);
	SetPlayerOld(player);

	player.position.temp = { cx, cy };
	player._pVar4 = GetSpellLevel(pnum, player._pSpell);
}

void FixPlrWalkTags(int pnum)
{
	int pp, pn;
	int dx, dy, y, x;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("FixPlrWalkTags: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	pp = pnum + 1;
	pn = -(pnum + 1);
	dx = player.position.old.x;
	dy = player.position.old.y;
	for (y = dy - 1; y <= dy + 1; y++) {
		for (x = dx - 1; x <= dx + 1; x++) {
			if (x >= 0 && x < MAXDUNX && y >= 0 && y < MAXDUNY && (dPlayer[x][y] == pp || dPlayer[x][y] == pn)) {
				dPlayer[x][y] = 0;
			}
		}
	}

	if (dx >= 0 && dx < MAXDUNX - 1 && dy >= 0 && dy < MAXDUNY - 1) {
		dFlags[dx + 1][dy] &= ~BFLAG_PLAYERLR;
		dFlags[dx][dy + 1] &= ~BFLAG_PLAYERLR;
	}
}

void RemovePlrFromMap(int pnum)
{
	int x, y;
	int pp, pn;

	pp = pnum + 1;
	pn = -(pnum + 1);

	for (y = 1; y < MAXDUNY; y++) {
		for (x = 1; x < MAXDUNX; x++) {
			if (dPlayer[x][y - 1] == pn || dPlayer[x - 1][y] == pn) {
				if ((dFlags[x][y] & BFLAG_PLAYERLR) != 0) {
					dFlags[x][y] &= ~BFLAG_PLAYERLR;
				}
			}
		}
	}

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++)
			if (dPlayer[x][y] == pp || dPlayer[x][y] == pn)
				dPlayer[x][y] = 0;
	}
}

void StartPlrHit(int pnum, int dam, bool forcehit)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlrHit: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player._pInvincible && player._pHitPoints == 0 && pnum == myplr) {
		SyncPlrKill(pnum, -1);
		return;
	}

	player.PlaySpeach(69);

	drawhpflag = true;
	if (player._pClass == HeroClass::Barbarian) {
		if (dam >> 6 < player._pLevel + player._pLevel / 4 && !forcehit) {
			return;
		}
	} else if (dam >> 6 < player._pLevel && !forcehit) {
		return;
	}

	direction pd = player._pdir;

	if ((player._pGFXLoad & PFILE_HIT) == 0) {
		LoadPlrGFX(pnum, PFILE_HIT);
	}

	int skippedAnimationFrames = 0;
	const int ZenFlags = ISPL_FASTRECOVER | ISPL_FASTERRECOVER | ISPL_FASTESTRECOVER;
	if ((player._pIFlags & ZenFlags) == ZenFlags) { // if multiple hitrecovery modes are present the skipping of frames can go so far, that they skip frames that would skip. so the additional skipping thats skipped. that means we can't add the different modes together.
		skippedAnimationFrames = 4;
	} else if ((player._pIFlags & ISPL_FASTESTRECOVER) != 0) {
		skippedAnimationFrames = 3;
	} else if ((player._pIFlags & ISPL_FASTERRECOVER) != 0) {
		skippedAnimationFrames = 2;
	} else if ((player._pIFlags & ISPL_FASTRECOVER) != 0) {
		skippedAnimationFrames = 1;
	} else {
		skippedAnimationFrames = 0;
	}

	NewPlrAnim(player, player._pHAnim[pd], player._pHFrames, 0, player._pHWidth, AnimationDistributionFlags::None, skippedAnimationFrames);

	player._pmode = PM_GOTHIT;
	FixPlayerLocation(pnum, pd);
	FixPlrWalkTags(pnum);
	dPlayer[player.position.tile.x][player.position.tile.y] = pnum + 1;
	SetPlayerOld(player);
}

void RespawnDeadItem(ItemStruct *itm, int x, int y)
{
	if (numitems >= MAXITEMS)
		return;

	int ii = AllocateItem();

	dItem[x][y] = ii + 1;

	items[ii] = *itm;
	items[ii].position = { x, y };
	RespawnItem(&items[ii], true);

	itm->_itype = ITYPE_NONE;
}

static void PlrDeadItem(PlayerStruct &player, ItemStruct *itm, int xx, int yy)
{
	int i, j, k;

	if (itm->isEmpty())
		return;

	int x = xx + player.position.tile.x;
	int y = yy + player.position.tile.y;
	if ((xx || yy) && ItemSpaceOk(x, y)) {
		RespawnDeadItem(itm, x, y);
		player.HoldItem = *itm;
		NetSendCmdPItem(false, CMD_RESPAWNITEM, { x, y });
		return;
	}

	for (k = 1; k < 50; k++) {
		for (j = -k; j <= k; j++) {
			y = j + player.position.tile.y;
			for (i = -k; i <= k; i++) {
				x = i + player.position.tile.x;
				if (ItemSpaceOk(x, y)) {
					RespawnDeadItem(itm, x, y);
					player.HoldItem = *itm;
					NetSendCmdPItem(false, CMD_RESPAWNITEM, { x, y });
					return;
				}
			}
		}
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void
StartPlayerKill(int pnum, int earflag)
{
	bool diablolevel;
	int i, pdd;
	ItemStruct ear;
	ItemStruct *pi;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartPlayerKill: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player._pHitPoints <= 0 && player._pmode == PM_DEATH) {
		return;
	}

	if (myplr == pnum) {
		NetSendCmdParam1(true, CMD_PLRDEAD, earflag);
	}

	diablolevel = gbIsMultiplayer && player.plrlevel == 16;

	player.PlaySpeach(71);

	if (player._pgfxnum) {
		player._pgfxnum = 0;
		player._pGFXLoad = 0;
		SetPlrAnims(player);
	}

	if ((player._pGFXLoad & PFILE_DEATH) == 0) {
		LoadPlrGFX(pnum, PFILE_DEATH);
	}

	NewPlrAnim(player, player._pDAnim[player._pdir], player._pDFrames, 1, player._pDWidth);

	player._pBlockFlag = false;
	player._pmode = PM_DEATH;
	player._pInvincible = true;
	SetPlayerHitPoints(pnum, 0);
	player.deathFrame = 1;

	if (pnum != myplr && !earflag && !diablolevel) {
		for (i = 0; i < NUM_INVLOC; i++) {
			player.InvBody[i]._itype = ITYPE_NONE;
		}
		CalcPlrInv(pnum, false);
	}

	if (player.plrlevel == currlevel) {
		FixPlayerLocation(pnum, player._pdir);
		RemovePlrFromMap(pnum);
		dFlags[player.position.tile.x][player.position.tile.y] |= BFLAG_DEAD_PLAYER;
		SetPlayerOld(player);

		if (pnum == myplr) {
			drawhpflag = true;
			deathdelay = 30;

			if (pcurs >= CURSOR_FIRSTITEM) {
				PlrDeadItem(player, &player.HoldItem, 0, 0);
				NewCursor(CURSOR_HAND);
			}

			if (!diablolevel) {
				DropHalfPlayersGold(pnum);
				if (earflag != -1) {
					if (earflag != 0) {
						SetPlrHandItem(&ear, IDI_EAR);
						sprintf(ear._iName, _("Ear of %s"), player._pName);
						if (player._pClass == HeroClass::Sorcerer) {
							ear._iCurs = ICURS_EAR_SORCERER;
						} else if (player._pClass == HeroClass::Warrior) {
							ear._iCurs = ICURS_EAR_WARRIOR;
						} else if (player._pClass == HeroClass::Rogue) {
							ear._iCurs = ICURS_EAR_ROGUE;
						} else if (player._pClass == HeroClass::Monk || player._pClass == HeroClass::Bard || player._pClass == HeroClass::Barbarian) {
							ear._iCurs = ICURS_EAR_ROGUE;
						}

						ear._iCreateInfo = player._pName[0] << 8 | player._pName[1];
						ear._iSeed = player._pName[2] << 24 | player._pName[3] << 16 | player._pName[4] << 8 | player._pName[5];
						ear._ivalue = player._pLevel;

						if (FindGetItem(IDI_EAR, ear._iCreateInfo, ear._iSeed) == -1) {
							PlrDeadItem(player, &ear, 0, 0);
						}
					} else {
						pi = &player.InvBody[0];
						i = NUM_INVLOC;
						while (i--) {
							pdd = (i + player._pdir) & 7;
							PlrDeadItem(player, pi, offset_x[pdd], offset_y[pdd]);
							pi++;
						}

						CalcPlrInv(pnum, false);
					}
				}
			}
		}
	}
	SetPlayerHitPoints(pnum, 0);
}

void DropHalfPlayersGold(int pnum)
{
	int i, hGold;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("DropHalfPlayersGold: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	hGold = player._pGold / 2;
	for (i = 0; i < MAXBELTITEMS && hGold > 0; i++) {
		if (player.SpdList[i]._itype == ITYPE_GOLD && player.SpdList[i]._ivalue != MaxGold) {
			if (hGold < player.SpdList[i]._ivalue) {
				player.SpdList[i]._ivalue -= hGold;
				SetSpdbarGoldCurs(pnum, i);
				SetPlrHandItem(&player.HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &player.HoldItem);
				SetPlrHandGoldCurs(&player.HoldItem);
				player.HoldItem._ivalue = hGold;
				PlrDeadItem(player, &player.HoldItem, 0, 0);
				hGold = 0;
			} else {
				hGold -= player.SpdList[i]._ivalue;
				RemoveSpdBarItem(pnum, i);
				SetPlrHandItem(&player.HoldItem, IDI_GOLD);
				GetGoldSeed(pnum, &player.HoldItem);
				SetPlrHandGoldCurs(&player.HoldItem);
				player.HoldItem._ivalue = player.SpdList[i]._ivalue;
				PlrDeadItem(player, &player.HoldItem, 0, 0);
				i = -1;
			}
		}
	}
	if (hGold > 0) {
		for (i = 0; i < MAXBELTITEMS && hGold > 0; i++) {
			if (player.SpdList[i]._itype == ITYPE_GOLD) {
				if (hGold < player.SpdList[i]._ivalue) {
					player.SpdList[i]._ivalue -= hGold;
					SetSpdbarGoldCurs(pnum, i);
					SetPlrHandItem(&player.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &player.HoldItem);
					SetPlrHandGoldCurs(&player.HoldItem);
					player.HoldItem._ivalue = hGold;
					PlrDeadItem(player, &player.HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= player.SpdList[i]._ivalue;
					RemoveSpdBarItem(pnum, i);
					SetPlrHandItem(&player.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &player.HoldItem);
					SetPlrHandGoldCurs(&player.HoldItem);
					player.HoldItem._ivalue = player.SpdList[i]._ivalue;
					PlrDeadItem(player, &player.HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	force_redraw = 255;
	if (hGold > 0) {
		for (i = 0; i < player._pNumInv && hGold > 0; i++) {
			if (player.InvList[i]._itype == ITYPE_GOLD && player.InvList[i]._ivalue != MaxGold) {
				if (hGold < player.InvList[i]._ivalue) {
					player.InvList[i]._ivalue -= hGold;
					SetGoldCurs(pnum, i);
					SetPlrHandItem(&player.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &player.HoldItem);
					SetPlrHandGoldCurs(&player.HoldItem);
					player.HoldItem._ivalue = hGold;
					PlrDeadItem(player, &player.HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= player.InvList[i]._ivalue;
					player.RemoveInvItem(i);
					SetPlrHandItem(&player.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &player.HoldItem);
					SetPlrHandGoldCurs(&player.HoldItem);
					player.HoldItem._ivalue = player.InvList[i]._ivalue;
					PlrDeadItem(player, &player.HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	if (hGold > 0) {
		for (i = 0; i < player._pNumInv && hGold > 0; i++) {
			if (player.InvList[i]._itype == ITYPE_GOLD) {
				if (hGold < player.InvList[i]._ivalue) {
					player.InvList[i]._ivalue -= hGold;
					SetGoldCurs(pnum, i);
					SetPlrHandItem(&player.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &player.HoldItem);
					SetPlrHandGoldCurs(&player.HoldItem);
					player.HoldItem._ivalue = hGold;
					PlrDeadItem(player, &player.HoldItem, 0, 0);
					hGold = 0;
				} else {
					hGold -= player.InvList[i]._ivalue;
					player.RemoveInvItem(i);
					SetPlrHandItem(&player.HoldItem, IDI_GOLD);
					GetGoldSeed(pnum, &player.HoldItem);
					SetPlrHandGoldCurs(&player.HoldItem);
					player.HoldItem._ivalue = player.InvList[i]._ivalue;
					PlrDeadItem(player, &player.HoldItem, 0, 0);
					i = -1;
				}
			}
		}
	}
	player._pGold = CalculateGold(pnum);
}

void StripTopGold(int pnum)
{
	ItemStruct tmpItem;
	int i, val;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StripTopGold: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	tmpItem = player.HoldItem;

	for (i = 0; i < player._pNumInv; i++) {
		if (player.InvList[i]._itype == ITYPE_GOLD) {
			if (player.InvList[i]._ivalue > MaxGold) {
				val = player.InvList[i]._ivalue - MaxGold;
				player.InvList[i]._ivalue = MaxGold;
				SetGoldCurs(pnum, i);
				SetPlrHandItem(&player.HoldItem, 0);
				GetGoldSeed(pnum, &player.HoldItem);
				player.HoldItem._ivalue = val;
				SetPlrHandGoldCurs(&player.HoldItem);
				if (!GoldAutoPlace(pnum))
					PlrDeadItem(player, &player.HoldItem, 0, 0);
			}
		}
	}
	player._pGold = CalculateGold(pnum);
	player.HoldItem = tmpItem;
}

void ApplyPlrDamage(int pnum, int dam, int minHP /*= 0*/, int frac /*= 0*/, int earflag /*= 0*/)
{
	auto &player = plr[pnum];

	int totalDamage = (dam << 6) + frac;
	if (totalDamage > 0) {
		for (int i = 0; i < nummissiles; i++) {
			int ma = missileactive[i];
			if (missile[ma]._mitype == MIS_MANASHIELD && missile[ma]._misource == pnum && !missile[ma]._miDelFlag) {
				if (missile[ma]._mispllvl > 0) {
					totalDamage += totalDamage / -3;
				}

				drawmanaflag = true;
				if (player._pMana >= totalDamage) {
					player._pMana -= totalDamage;
					player._pManaBase -= totalDamage;
					totalDamage = 0;
				} else {
					totalDamage -= player._pMana;
					if (missile[ma]._mispllvl > 0) {
						totalDamage += totalDamage / 2;
					}
					player._pMana = 0;
					player._pManaBase = player._pMaxManaBase - player._pMaxMana;
				}

				break;
			}
		}
	}

	if (totalDamage == 0)
		return;

	drawhpflag = true;
	player._pHitPoints -= totalDamage;
	player._pHPBase -= totalDamage;
	if (player._pHitPoints > player._pMaxHP) {
		player._pHitPoints = player._pMaxHP;
		player._pHPBase = player._pMaxHPBase;
	}
	int minHitPoints = minHP << 6;
	if (player._pHitPoints < minHitPoints) {
		SetPlayerHitPoints(pnum, minHitPoints);
	}
	if (player._pHitPoints >> 6 <= 0) {
		SyncPlrKill(pnum, earflag);
	}
}

void SyncPlrKill(int pnum, int earflag)
{
	auto &player = plr[pnum];

	if (player._pHitPoints <= 0 && currlevel == 0) {
		SetPlayerHitPoints(pnum, 64);
		return;
	}

	SetPlayerHitPoints(pnum, 0);
	StartPlayerKill(pnum, earflag);
}

void RemovePlrMissiles(int pnum)
{
	int i, am;
	int mx, my;

	if (currlevel != 0 && pnum == myplr && (monster[myplr].position.tile.x != 1 || monster[myplr].position.tile.y != 0)) {
		M_StartKill(myplr, myplr);
		AddDead(monster[myplr].position.tile, (monster[myplr].MType)->mdeadval, monster[myplr]._mdir);
		mx = monster[myplr].position.tile.x;
		my = monster[myplr].position.tile.y;
		dMonster[mx][my] = 0;
		monster[myplr]._mDelFlag = true;
		DeleteMonsterList();
	}

	for (i = 0; i < nummissiles; i++) {
		am = missileactive[i];
		if (missile[am]._mitype == MIS_STONE && missile[am]._misource == pnum) {
			monster[missile[am]._miVar2]._mmode = (MON_MODE)missile[am]._miVar1;
		}
		if (missile[am]._mitype == MIS_MANASHIELD && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
		if (missile[am]._mitype == MIS_ETHEREALIZE && missile[am]._misource == pnum) {
			ClearMissileSpot(am);
			DeleteMissile(am, i);
		}
	}
}

void InitLevelChange(int pnum)
{
	auto &player = plr[pnum];

	RemovePlrMissiles(pnum);
	if (pnum == myplr && qtextflag) {
		qtextflag = false;
		stream_stop();
	}

	RemovePlrFromMap(pnum);
	SetPlayerOld(player);
	if (pnum == myplr) {
		auto &myPlayer = plr[myplr];
		dPlayer[myPlayer.position.tile.x][myPlayer.position.tile.y] = myplr + 1;
	} else {
		player._pLvlVisited[player.plrlevel] = true;
	}

	ClrPlrPath(player);
	player.destAction = ACTION_NONE;
	player._pLvlChanging = true;

	if (pnum == myplr) {
		player.pLvlLoad = 10;
	}
}

#if defined(__clang__) || defined(__GNUC__)
__attribute__((no_sanitize("shift-base")))
#endif
void
StartNewLvl(int pnum, interface_mode fom, int lvl)
{
	InitLevelChange(pnum);

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("StartNewLvl: illegal player %i", pnum);
	}
	auto &player = plr[pnum];
	auto &myPlayer = plr[myplr];

	switch (fom) {
	case WM_DIABNEXTLVL:
	case WM_DIABPREVLVL:
		player.plrlevel = lvl;
		break;
	case WM_DIABRTNLVL:
	case WM_DIABTOWNWARP:
		player.plrlevel = lvl;
		break;
	case WM_DIABSETLVL:
		setlvlnum = (_setlevels)lvl;
		break;
	case WM_DIABTWARPUP:
		myPlayer.pTownWarps |= 1 << (leveltype - 2);
		player.plrlevel = lvl;
		break;
	case WM_DIABRETOWN:
		break;
	default:
		app_fatal("StartNewLvl");
	}

	if (pnum == myplr) {
		player._pmode = PM_NEWLVL;
		player._pInvincible = true;
		PostMessage(fom, 0, 0);
		if (gbIsMultiplayer) {
			NetSendCmdParam2(true, CMD_NEWLVL, fom, lvl);
		}
	}
}

void RestartTownLvl(int pnum)
{
	InitLevelChange(pnum);
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("RestartTownLvl: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	player.plrlevel = 0;
	player._pInvincible = false;

	SetPlayerHitPoints(pnum, 64);

	player._pMana = 0;
	player._pManaBase = player._pMana - (player._pMaxMana - player._pMaxManaBase);

	CalcPlrInv(pnum, false);

	if (pnum == myplr) {
		player._pmode = PM_NEWLVL;
		player._pInvincible = true;
		PostMessage(WM_DIABRETOWN, 0, 0);
	}
}

void StartWarpLvl(int pnum, int pidx)
{
	auto &player = plr[pnum];

	InitLevelChange(pnum);

	if (gbIsMultiplayer) {
		if (player.plrlevel != 0) {
			player.plrlevel = 0;
		} else {
			player.plrlevel = portal[pidx].level;
		}
	}

	if (pnum == myplr) {
		SetCurrentPortal(pidx);
		player._pmode = PM_NEWLVL;
		player._pInvincible = true;
		PostMessage(WM_DIABWARPLVL, 0, 0);
	}
}

/**
 * @brief Continue movement towards new tile
 */
bool PM_DoWalk(int pnum, int variant)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoWalk: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	//Play walking sound effect on certain animation frames
	if (sgOptions.Audio.bWalkingSound && (currlevel != 0 || !sgGameInitInfo.bRunInTown)) {
		if (player.AnimInfo.CurrentFrame == 1
		    || player.AnimInfo.CurrentFrame == 5) {
			PlaySfxLoc(PS_WALK1, player.position.tile.x, player.position.tile.y);
		}
	}

	//Check if we reached new tile
	if (player.AnimInfo.CurrentFrame >= player._pWFrames) {

		//Update the player's tile position
		switch (variant) {
		case PM_WALK:
			dPlayer[player.position.tile.x][player.position.tile.y] = 0;
			player.position.tile += player.position.temp;
			dPlayer[player.position.tile.x][player.position.tile.y] = pnum + 1;
			break;
		case PM_WALK2:
			dPlayer[player.position.temp.x][player.position.temp.y] = 0;
			break;
		case PM_WALK3:
			dPlayer[player.position.tile.x][player.position.tile.y] = 0;
			dFlags[player._pVar4][player._pVar5] &= ~BFLAG_PLAYERLR;
			player.position.tile = player.position.temp;
			dPlayer[player.position.tile.x][player.position.tile.y] = pnum + 1;
			break;
		}

		//Update the coordinates for lighting and vision entries for the player
		if (leveltype != DTYPE_TOWN) {
			ChangeLightXY(player._plid, player.position.tile.x, player.position.tile.y);
			ChangeVisionXY(player._pvid, player.position.tile.x, player.position.tile.y);
		}

		//Update the "camera" tile position
		if (pnum == myplr && ScrollInfo._sdir) {
			ViewX = player.position.tile.x - ScrollInfo.tile.x;
			ViewY = player.position.tile.y - ScrollInfo.tile.y;
		}

		if (player.walkpath[0] != WALK_NONE) {
			StartWalkStand(pnum);
		} else {
			StartStand(pnum, player.tempDirection);
		}

		ClearPlrPVars(player);

		//Reset the "sub-tile" position of the player's light entry to 0
		if (leveltype != DTYPE_TOWN) {
			ChangeLightOff(player._plid, 0, 0);
		}

		AutoGoldPickup(pnum);
		return true;
	} //We didn't reach new tile so update player's "sub-tile" position
	PM_ChangeOffset(pnum);
	return false;
}

static bool WeaponDurDecay(int pnum, int ii)
{
	auto &player = plr[pnum];

	if (!player.InvBody[ii].isEmpty() && player.InvBody[ii]._iClass == ICLASS_WEAPON && player.InvBody[ii]._iDamAcFlags & 2) {
		player.InvBody[ii]._iPLDam -= 5;
		if (player.InvBody[ii]._iPLDam <= -100) {
			NetSendCmdDelItem(true, ii);
			player.InvBody[ii]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, true);
			return true;
		}
		CalcPlrInv(pnum, true);
	}
	return false;
}

bool WeaponDur(int pnum, int durrnd)
{
	if (pnum != myplr) {
		return false;
	}

	if (WeaponDurDecay(pnum, INVLOC_HAND_LEFT))
		return true;
	if (WeaponDurDecay(pnum, INVLOC_HAND_RIGHT))
		return true;

	if (GenerateRnd(durrnd) != 0) {
		return false;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("WeaponDur: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (!player.InvBody[INVLOC_HAND_LEFT].isEmpty() && player.InvBody[INVLOC_HAND_LEFT]._iClass == ICLASS_WEAPON) {
		if (player.InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return false;
		}

		player.InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (player.InvBody[INVLOC_HAND_LEFT]._iDurability <= 0) {
			NetSendCmdDelItem(true, INVLOC_HAND_LEFT);
			player.InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, true);
			return true;
		}
	}

	if (!player.InvBody[INVLOC_HAND_RIGHT].isEmpty() && player.InvBody[INVLOC_HAND_RIGHT]._iClass == ICLASS_WEAPON) {
		if (player.InvBody[INVLOC_HAND_RIGHT]._iDurability == DUR_INDESTRUCTIBLE) {
			return false;
		}

		player.InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if (player.InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
			NetSendCmdDelItem(true, INVLOC_HAND_RIGHT);
			player.InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, true);
			return true;
		}
	}

	if (player.InvBody[INVLOC_HAND_LEFT].isEmpty() && player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
		if (player.InvBody[INVLOC_HAND_RIGHT]._iDurability == DUR_INDESTRUCTIBLE) {
			return false;
		}

		player.InvBody[INVLOC_HAND_RIGHT]._iDurability--;
		if (player.InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
			NetSendCmdDelItem(true, INVLOC_HAND_RIGHT);
			player.InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, true);
			return true;
		}
	}

	if (player.InvBody[INVLOC_HAND_RIGHT].isEmpty() && player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
		if (player.InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return false;
		}

		player.InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (player.InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(true, INVLOC_HAND_LEFT);
			player.InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, true);
			return true;
		}
	}

	return false;
}

bool PlrHitMonst(int pnum, int m)
{
	bool rv, ret;
	int hit, hper, mind, maxd, ddp, dam, skdam, phanditype, tmac;
	hper = 0;
	ret = false;
	bool adjacentDamage = false;

	if ((DWORD)m >= MAXMONSTERS) {
		app_fatal("PlrHitMonst: illegal monster %i", m);
	}
	auto &player = plr[pnum];

	if ((monster[m]._mhitpoints >> 6) <= 0) {
		return false;
	}

	if (monster[m].MType->mtype == MT_ILLWEAV && monster[m]._mgoal == MGOAL_RETREAT) {
		return false;
	}

	if (monster[m]._mmode == MM_CHARGE) {
		return false;
	}

	if (pnum < 0) {
		adjacentDamage = true;
		pnum = -pnum;
		if (player._pLevel > 20)
			hper -= 30;
		else
			hper -= (35 - player._pLevel) * 2;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrHitMonst: illegal player %i", pnum);
	}

	rv = false;

	hit = GenerateRnd(100);
	if (monster[m]._mmode == MM_STONE) {
		hit = 0;
	}

	tmac = monster[m].mArmorClass;
	if (gbIsHellfire && player._pIEnAc > 0) {
		int _pIEnAc = player._pIEnAc - 1;
		if (_pIEnAc > 0)
			tmac >>= _pIEnAc;
		else
			tmac -= tmac / 4;

		if (player._pClass == HeroClass::Barbarian) {
			tmac -= monster[m].mArmorClass / 8;
		}

		if (tmac < 0)
			tmac = 0;
	} else {
		tmac -= player._pIEnAc;
	}

	hper += (player._pDexterity / 2) + player._pLevel + 50 - tmac;
	if (player._pClass == HeroClass::Warrior) {
		hper += 20;
	}
	hper += player._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if (CheckMonsterHit(m, &ret)) {
		return ret;
	}
#ifdef _DEBUG
	if (hit < hper || debug_mode_key_inverted_v || debug_mode_dollar_sign) {
#else
	if (hit < hper) {
#endif
		if (player._pIFlags & ISPL_FIREDAM && player._pIFlags & ISPL_LIGHTDAM) {
			int midam = player._pIFMinDam + GenerateRnd(player._pIFMaxDam - player._pIFMinDam);
			AddMissile(player.position.tile.x, player.position.tile.y, player.position.temp.x, player.position.temp.y, player._pdir, MIS_SPECARROW, TARGET_MONSTERS, pnum, midam, 0);
		}
		mind = player._pIMinDam;
		maxd = player._pIMaxDam;
		dam = GenerateRnd(maxd - mind + 1) + mind;
		dam += dam * player._pIBonusDam / 100;
		dam += player._pIBonusDamMod;
		int dam2 = dam << 6;
		dam += player._pDamageMod;
		if (player._pClass == HeroClass::Warrior || player._pClass == HeroClass::Barbarian) {
			ddp = player._pLevel;
			if (GenerateRnd(100) < ddp) {
				dam *= 2;
			}
		}

		phanditype = ITYPE_NONE;
		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD) {
			phanditype = ITYPE_SWORD;
		}
		if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_MACE) {
			phanditype = ITYPE_MACE;
		}

		switch (monster[m].MData->mMonstClass) {
		case MC_UNDEAD:
			if (phanditype == ITYPE_SWORD) {
				dam -= dam / 2;
			} else if (phanditype == ITYPE_MACE) {
				dam += dam / 2;
			}
			break;
		case MC_ANIMAL:
			if (phanditype == ITYPE_MACE) {
				dam -= dam / 2;
			} else if (phanditype == ITYPE_SWORD) {
				dam += dam / 2;
			}
			break;
		case MC_DEMON:
			if ((player._pIFlags & ISPL_3XDAMVDEM) != 0) {
				dam *= 3;
			}
			break;
		}

		if (player.pDamAcFlags & 0x01 && GenerateRnd(100) < 5) {
			dam *= 3;
		}

		if (player.pDamAcFlags & 0x10 && monster[m].MType->mtype != MT_DIABLO && monster[m]._uniqtype == 0 && GenerateRnd(100) < 10) {
			monster_43C785(m);
		}

		dam <<= 6;
		if ((player.pDamAcFlags & 0x08) != 0) {
			int r = GenerateRnd(201);
			if (r >= 100)
				r = 100 + (r - 100) * 5;
			dam = dam * r / 100;
		}

		if (adjacentDamage)
			dam >>= 2;

		if (pnum == myplr) {
			if ((player.pDamAcFlags & 0x04) != 0) {
				dam2 += player._pIGetHit << 6;
				if (dam2 >= 0) {
					ApplyPlrDamage(pnum, 0, 1, dam2);
				}
				dam *= 2;
			}
			monster[m]._mhitpoints -= dam;
		}

		if ((player._pIFlags & ISPL_RNDSTEALLIFE) != 0) {
			skdam = GenerateRnd(dam / 8);
			player._pHitPoints += skdam;
			if (player._pHitPoints > player._pMaxHP) {
				player._pHitPoints = player._pMaxHP;
			}
			player._pHPBase += skdam;
			if (player._pHPBase > player._pMaxHPBase) {
				player._pHPBase = player._pMaxHPBase;
			}
			drawhpflag = true;
		}
		if (player._pIFlags & (ISPL_STEALMANA_3 | ISPL_STEALMANA_5) && !(player._pIFlags & ISPL_NOMANA)) {
			if (player._pIFlags & ISPL_STEALMANA_3) {
				skdam = 3 * dam / 100;
			}
			if ((player._pIFlags & ISPL_STEALMANA_5) != 0) {
				skdam = 5 * dam / 100;
			}
			player._pMana += skdam;
			if (player._pMana > player._pMaxMana) {
				player._pMana = player._pMaxMana;
			}
			player._pManaBase += skdam;
			if (player._pManaBase > player._pMaxManaBase) {
				player._pManaBase = player._pMaxManaBase;
			}
			drawmanaflag = true;
		}
		if (player._pIFlags & (ISPL_STEALLIFE_3 | ISPL_STEALLIFE_5)) {
			if (player._pIFlags & ISPL_STEALLIFE_3) {
				skdam = 3 * dam / 100;
			}
			if ((player._pIFlags & ISPL_STEALLIFE_5) != 0) {
				skdam = 5 * dam / 100;
			}
			player._pHitPoints += skdam;
			if (player._pHitPoints > player._pMaxHP) {
				player._pHitPoints = player._pMaxHP;
			}
			player._pHPBase += skdam;
			if (player._pHPBase > player._pMaxHPBase) {
				player._pHPBase = player._pMaxHPBase;
			}
			drawhpflag = true;
		}
		if ((player._pIFlags & ISPL_NOHEALPLR) != 0) {
			monster[m]._mFlags |= MFLAG_NOHEAL;
		}
#ifdef _DEBUG
		if (debug_mode_dollar_sign || debug_mode_key_inverted_v) {
			monster[m]._mhitpoints = 0; /* double check */
		}
#endif
		if ((monster[m]._mhitpoints >> 6) <= 0) {
			if (monster[m]._mmode == MM_STONE) {
				M_StartKill(m, pnum);
				monster[m]._mmode = MM_STONE;
			} else {
				M_StartKill(m, pnum);
			}
		} else {
			if (monster[m]._mmode == MM_STONE) {
				M_StartHit(m, pnum, dam);
				monster[m]._mmode = MM_STONE;
			} else {
				if ((player._pIFlags & ISPL_KNOCKBACK) != 0) {
					M_GetKnockback(m);
				}
				M_StartHit(m, pnum, dam);
			}
		}
		rv = true;
	}

	return rv;
}

bool PlrHitPlr(int pnum, int8_t p)
{
	bool rv;
	int hit, hper, blk, blkper, mind, maxd, dam, lvl, skdam, tac;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal target player %i", p);
	}
	auto &target = plr[p];

	rv = false;

	if (target._pInvincible) {
		return rv;
	}

	if ((target._pSpellFlags & 1) != 0) {
		return rv;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PlrHitPlr: illegal attacking player %i", pnum);
	}
	auto &attacker = plr[pnum];

	hit = GenerateRnd(100);

	hper = (attacker._pDexterity / 2) + attacker._pLevel + 50 - (target._pIBonusAC + target._pIAC + target._pDexterity / 5);

	if (attacker._pClass == HeroClass::Warrior) {
		hper += 20;
	}
	hper += attacker._pIBonusToHit;
	if (hper < 5) {
		hper = 5;
	}
	if (hper > 95) {
		hper = 95;
	}

	if ((target._pmode == PM_STAND || target._pmode == PM_ATTACK) && target._pBlockFlag) {
		blk = GenerateRnd(100);
	} else {
		blk = 100;
	}

	blkper = target._pDexterity + target._pBaseToBlk + (target._pLevel * 2) - (attacker._pLevel * 2);
	if (blkper < 0) {
		blkper = 0;
	}
	if (blkper > 100) {
		blkper = 100;
	}

	if (hit < hper) {
		if (blk < blkper) {
			direction dir = GetDirection(target.position.tile, attacker.position.tile);
			StartPlrBlock(p, dir);
		} else {
			mind = attacker._pIMinDam;
			maxd = attacker._pIMaxDam;
			dam = GenerateRnd(maxd - mind + 1) + mind;
			dam += (dam * attacker._pIBonusDam) / 100;
			dam += attacker._pIBonusDamMod + attacker._pDamageMod;

			if (attacker._pClass == HeroClass::Warrior || attacker._pClass == HeroClass::Barbarian) {
				lvl = attacker._pLevel;
				if (GenerateRnd(100) < lvl) {
					dam *= 2;
				}
			}
			skdam = dam << 6;
			if ((attacker._pIFlags & ISPL_RNDSTEALLIFE) != 0) {
				tac = GenerateRnd(skdam / 8);
				attacker._pHitPoints += tac;
				if (attacker._pHitPoints > attacker._pMaxHP) {
					attacker._pHitPoints = attacker._pMaxHP;
				}
				attacker._pHPBase += tac;
				if (attacker._pHPBase > attacker._pMaxHPBase) {
					attacker._pHPBase = attacker._pMaxHPBase;
				}
				drawhpflag = true;
			}
			if (pnum == myplr) {
				NetSendCmdDamage(true, p, skdam);
			}
			StartPlrHit(p, skdam, false);
		}

		rv = true;
	}

	return rv;
}

bool PlrHitObj(int pnum, int mx, int my)
{
	int oi;

	if (dObject[mx][my] > 0) {
		oi = dObject[mx][my] - 1;
	} else {
		oi = -dObject[mx][my] - 1;
	}

	if (object[oi]._oBreak == 1) {
		BreakObject(pnum, oi);
		return true;
	}

	return false;
}

bool PM_DoAttack(int pnum)
{
	int dx, dy, m;
	bool didhit = false;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoAttack: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.AnimInfo.CurrentFrame == player._pAFNum - 1) {
		PlaySfxLoc(PS_SWING, player.position.tile.x, player.position.tile.y);
	}

	if (player.AnimInfo.CurrentFrame == player._pAFNum) {
		dx = player.position.tile.x + offset_x[player._pdir];
		dy = player.position.tile.y + offset_y[player._pdir];

		if (dMonster[dx][dy] != 0) {
			if (dMonster[dx][dy] > 0) {
				m = dMonster[dx][dy] - 1;
			} else {
				m = -(dMonster[dx][dy] + 1);
			}
			if (CanTalkToMonst(m)) {
				player.position.temp.x = 0; /** @todo Looks to be irrelevant, probably just remove it */
				return false;
			}
		}

		if (!(player._pIFlags & ISPL_FIREDAM) || !(player._pIFlags & ISPL_LIGHTDAM)) {
			if (player._pIFlags & ISPL_FIREDAM) {
				AddMissile(dx, dy, 1, 0, 0, MIS_WEAPEXP, TARGET_MONSTERS, pnum, 0, 0);
			} else if ((player._pIFlags & ISPL_LIGHTDAM) != 0) {
				AddMissile(dx, dy, 2, 0, 0, MIS_WEAPEXP, TARGET_MONSTERS, pnum, 0, 0);
			}
		}

		if (dMonster[dx][dy]) {
			m = dMonster[dx][dy];
			if (dMonster[dx][dy] > 0) {
				m = dMonster[dx][dy] - 1;
			} else {
				m = -(dMonster[dx][dy] + 1);
			}
			didhit = PlrHitMonst(pnum, m);
		} else if (dPlayer[dx][dy] != 0 && (!gbFriendlyMode || sgGameInitInfo.bFriendlyFire)) {
			BYTE p = dPlayer[dx][dy];
			if (dPlayer[dx][dy] > 0) {
				p = dPlayer[dx][dy] - 1;
			} else {
				p = -(dPlayer[dx][dy] + 1);
			}
			didhit = PlrHitPlr(pnum, p);
		} else if (dObject[dx][dy] > 0) {
			didhit = PlrHitObj(pnum, dx, dy);
		}
		if ((player._pClass == HeroClass::Monk
		        && (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_STAFF || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_STAFF))
		    || (player._pClass == HeroClass::Bard
		        && player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD && player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD)
		    || (player._pClass == HeroClass::Barbarian
		        && (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_AXE || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_AXE
		            || (((player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_MACE && player.InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND)
		                    || (player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_MACE && player.InvBody[INVLOC_HAND_RIGHT]._iLoc == ILOC_TWOHAND)
		                    || (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SWORD && player.InvBody[INVLOC_HAND_LEFT]._iLoc == ILOC_TWOHAND)
		                    || (player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SWORD && player.InvBody[INVLOC_HAND_RIGHT]._iLoc == ILOC_TWOHAND))
		                && !(player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD || player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD))))) {
			dx = player.position.tile.x + offset_x[(player._pdir + 1) % 8];
			dy = player.position.tile.y + offset_y[(player._pdir + 1) % 8];
			m = ((dMonster[dx][dy] > 0) ? dMonster[dx][dy] : -dMonster[dx][dy]) - 1;
			if (dMonster[dx][dy] != 0 && !CanTalkToMonst(m) && monster[m].position.old.x == dx && monster[m].position.old.y == dy) {
				if (PlrHitMonst(-pnum, m))
					didhit = true;
			}
			dx = player.position.tile.x + offset_x[(player._pdir + 7) % 8];
			dy = player.position.tile.y + offset_y[(player._pdir + 7) % 8];
			m = ((dMonster[dx][dy] > 0) ? dMonster[dx][dy] : -dMonster[dx][dy]) - 1;
			if (dMonster[dx][dy] != 0 && !CanTalkToMonst(m) && monster[m].position.old.x == dx && monster[m].position.old.y == dy) {
				if (PlrHitMonst(-pnum, m))
					didhit = true;
			}
		}

		if (didhit && WeaponDur(pnum, 30)) {
			StartStand(pnum, player._pdir);
			ClearPlrPVars(player);
			return true;
		}
	}

	if (player.AnimInfo.CurrentFrame == player._pAFrames) {
		StartStand(pnum, player._pdir);
		ClearPlrPVars(player);
		return true;
	}
	return false;
}

bool PM_DoRangeAttack(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoRangeAttack: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	int arrows = 0;
	if (player.AnimInfo.CurrentFrame == player._pAFNum) {
		arrows = 1;
	}
	if ((player._pIFlags & ISPL_MULT_ARROWS) != 0 && player.AnimInfo.CurrentFrame == player._pAFNum + 2) {
		arrows = 2;
	}

	for (int arrow = 0; arrow < arrows; arrow++) {
		int xoff = 0;
		int yoff = 0;
		if (arrows != 1) {
			int angle = arrow == 0 ? -1 : 1;
			int x = player.position.temp.x - player.position.tile.x;
			if (x != 0)
				yoff = x < 0 ? angle : -angle;
			int y = player.position.temp.y - player.position.tile.y;
			if (y != 0)
				xoff = y < 0 ? -angle : angle;
		}

		int dmg = 4;
		missile_id mistype = MIS_ARROW;
		if ((player._pIFlags & ISPL_FIRE_ARROWS) != 0) {
			mistype = MIS_FARROW;
		}
		if ((player._pIFlags & ISPL_LIGHT_ARROWS) != 0) {
			mistype = MIS_LARROW;
		}
		if ((player._pIFlags & ISPL_FIRE_ARROWS) != 0 && (player._pIFlags & ISPL_LIGHT_ARROWS) != 0) {
			dmg = player._pIFMinDam + GenerateRnd(player._pIFMaxDam - player._pIFMinDam);
			mistype = MIS_SPECARROW;
		}

		AddMissile(
		    player.position.tile.x,
		    player.position.tile.y,
		    player.position.temp.x + xoff,
		    player.position.temp.y + yoff,
		    player._pdir,
		    mistype,
		    TARGET_MONSTERS,
		    pnum,
		    dmg,
		    0);

		if (arrow == 0 && mistype != MIS_SPECARROW) {
			PlaySfxLoc(arrows != 1 ? IS_STING1 : PS_BFIRE, player.position.tile.x, player.position.tile.y);
		}

		if (WeaponDur(pnum, 40)) {
			StartStand(pnum, player._pdir);
			ClearPlrPVars(player);
			return true;
		}
	}

	if (player.AnimInfo.CurrentFrame >= player._pAFrames) {
		StartStand(pnum, player._pdir);
		ClearPlrPVars(player);
		return true;
	}
	return false;
}

void ShieldDur(int pnum)
{
	if (pnum != myplr) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ShieldDur: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.InvBody[INVLOC_HAND_LEFT]._itype == ITYPE_SHIELD) {
		if (player.InvBody[INVLOC_HAND_LEFT]._iDurability == DUR_INDESTRUCTIBLE) {
			return;
		}

		player.InvBody[INVLOC_HAND_LEFT]._iDurability--;
		if (player.InvBody[INVLOC_HAND_LEFT]._iDurability == 0) {
			NetSendCmdDelItem(true, INVLOC_HAND_LEFT);
			player.InvBody[INVLOC_HAND_LEFT]._itype = ITYPE_NONE;
			CalcPlrInv(pnum, true);
		}
	}

	if (player.InvBody[INVLOC_HAND_RIGHT]._itype == ITYPE_SHIELD) {
		if (player.InvBody[INVLOC_HAND_RIGHT]._iDurability != DUR_INDESTRUCTIBLE) {
			player.InvBody[INVLOC_HAND_RIGHT]._iDurability--;
			if (player.InvBody[INVLOC_HAND_RIGHT]._iDurability == 0) {
				NetSendCmdDelItem(true, INVLOC_HAND_RIGHT);
				player.InvBody[INVLOC_HAND_RIGHT]._itype = ITYPE_NONE;
				CalcPlrInv(pnum, true);
			}
		}
	}
}

bool PM_DoBlock(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoBlock: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.AnimInfo.CurrentFrame >= player._pBFrames) {
		StartStand(pnum, player._pdir);
		ClearPlrPVars(player);

		if (GenerateRnd(10) == 0) {
			ShieldDur(pnum);
		}
		return true;
	}

	return false;
}

static void ArmorDur(int pnum)
{
	int a;
	ItemStruct *pi;

	if (pnum != myplr) {
		return;
	}

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("ArmorDur: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.InvBody[INVLOC_CHEST].isEmpty() && player.InvBody[INVLOC_HEAD].isEmpty()) {
		return;
	}

	a = GenerateRnd(3);
	if (!player.InvBody[INVLOC_CHEST].isEmpty() && player.InvBody[INVLOC_HEAD].isEmpty()) {
		a = 1;
	}
	if (player.InvBody[INVLOC_CHEST].isEmpty() && !player.InvBody[INVLOC_HEAD].isEmpty()) {
		a = 0;
	}

	if (a != 0) {
		pi = &player.InvBody[INVLOC_CHEST];
	} else {
		pi = &player.InvBody[INVLOC_HEAD];
	}
	if (pi->_iDurability == DUR_INDESTRUCTIBLE) {
		return;
	}

	pi->_iDurability--;
	if (pi->_iDurability != 0) {
		return;
	}

	if (a != 0) {
		NetSendCmdDelItem(true, INVLOC_CHEST);
	} else {
		NetSendCmdDelItem(true, INVLOC_HEAD);
	}
	pi->_itype = ITYPE_NONE;
	CalcPlrInv(pnum, true);
}

bool PM_DoSpell(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoSpell: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.AnimInfo.CurrentFrame == (player._pSFNum + 1)) {
		CastSpell(
		    pnum,
		    player._pSpell,
		    player.position.tile.x,
		    player.position.tile.y,
		    player.position.temp.x,
		    player.position.temp.y,
		    player._pVar4);

		if (player._pSplFrom == 0) {
			EnsureValidReadiedSpell(player);
		}
	}

	if (leveltype == DTYPE_TOWN) {
		if (player.AnimInfo.CurrentFrame > player._pSFrames) {
			StartWalkStand(pnum);
			ClearPlrPVars(player);
			return true;
		}
	} else if (player.AnimInfo.CurrentFrame == player._pSFrames) {
		StartStand(pnum, player._pdir);
		ClearPlrPVars(player);
		return true;
	}

	return false;
}

bool PM_DoGotHit(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoGotHit: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.AnimInfo.CurrentFrame >= player._pHFrames) {
		StartStand(pnum, player._pdir);
		ClearPlrPVars(player);
		if (GenerateRnd(4) != 0) {
			ArmorDur(pnum);
		}

		return true;
	}

	return false;
}

bool PM_DoDeath(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("PM_DoDeath: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.deathFrame >= 2 * player._pDFrames) {
		if (deathdelay > 1 && pnum == myplr) {
			deathdelay--;
			if (deathdelay == 1) {
				deathflag = true;
				if (!gbIsMultiplayer) {
					gamemenu_on();
				}
			}
		}

		player.AnimInfo.DelayLen = 10000;
		player.AnimInfo.CurrentFrame = player.AnimInfo.NumberOfFrames;
		dFlags[player.position.tile.x][player.position.tile.y] |= BFLAG_DEAD_PLAYER;
	}

	if (player.deathFrame < 100) {
		player.deathFrame++;
	}

	return false;
}

void CheckNewPath(int pnum, bool pmWillBeCalled)
{
	int i, x, y;
	int xvel3, xvel, yvel;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("CheckNewPath: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.destAction == ACTION_ATTACKMON) {
		i = player.destParam1;
		MakePlrPath(pnum, monster[i].position.future.x, monster[i].position.future.y, false);
	}

	if (player.destAction == ACTION_ATTACKPLR) {
		auto &target = plr[player.destParam1];
		MakePlrPath(pnum, target.position.future.x, target.position.future.y, false);
	}

	direction d;
	if (player.walkpath[0] != WALK_NONE) {
		if (player._pmode == PM_STAND) {
			if (pnum == myplr) {
				if (player.destAction == ACTION_ATTACKMON || player.destAction == ACTION_ATTACKPLR) {

					if (player.destAction == ACTION_ATTACKMON) {
						i = player.destParam1;
						x = abs(player.position.future.x - monster[i].position.future.x);
						y = abs(player.position.future.y - monster[i].position.future.y);
						d = GetDirection(player.position.future, monster[i].position.future);
					} else {
						auto &target = plr[player.destParam1];
						x = abs(player.position.future.x - target.position.future.x);
						y = abs(player.position.future.y - target.position.future.y);
						d = GetDirection(player.position.future, target.position.future);
					}

					if (x < 2 && y < 2) {
						ClrPlrPath(player);
						if (monster[i].mtalkmsg != TEXT_NONE && monster[i].mtalkmsg != TEXT_VILE14) {
							TalktoMonster(i);
						} else {
							StartAttack(pnum, d);
						}
						player.destAction = ACTION_NONE;
					}
				}
			}

			if (currlevel != 0) {
				xvel3 = PWVel[static_cast<std::size_t>(player._pClass)][0];
				xvel = PWVel[static_cast<std::size_t>(player._pClass)][1];
				yvel = PWVel[static_cast<std::size_t>(player._pClass)][2];
			} else {
				xvel3 = 2048;
				xvel = 1024;
				yvel = 512;
			}

			switch (player.walkpath[0]) {
			case WALK_N:
				StartWalk(pnum, 0, -xvel, 0, 0, -1, -1, 0, 0, DIR_N, SDIR_N, PM_WALK, pmWillBeCalled);
				break;
			case WALK_NE:
				StartWalk(pnum, xvel, -yvel, 0, 0, 0, -1, 0, 0, DIR_NE, SDIR_NE, PM_WALK, pmWillBeCalled);
				break;
			case WALK_E:
				StartWalk(pnum, xvel3, 0, -32, -16, 1, -1, 1, 0, DIR_E, SDIR_E, PM_WALK3, pmWillBeCalled);
				break;
			case WALK_SE:
				StartWalk(pnum, xvel, yvel, -32, -16, 1, 0, 0, 0, DIR_SE, SDIR_SE, PM_WALK2, pmWillBeCalled);
				break;
			case WALK_S:
				StartWalk(pnum, 0, xvel, 0, -32, 1, 1, 0, 0, DIR_S, SDIR_S, PM_WALK2, pmWillBeCalled);
				break;
			case WALK_SW:
				StartWalk(pnum, -xvel, yvel, 32, -16, 0, 1, 0, 0, DIR_SW, SDIR_SW, PM_WALK2, pmWillBeCalled);
				break;
			case WALK_W:
				StartWalk(pnum, -xvel3, 0, 32, -16, -1, 1, 0, 1, DIR_W, SDIR_W, PM_WALK3, pmWillBeCalled);
				break;
			case WALK_NW:
				StartWalk(pnum, -xvel, -yvel, 0, 0, -1, 0, 0, 0, DIR_NW, SDIR_NW, PM_WALK, pmWillBeCalled);
				break;
			}

			for (i = 1; i < MAX_PATH_LENGTH; i++) {
				player.walkpath[i - 1] = player.walkpath[i];
			}

			player.walkpath[MAX_PATH_LENGTH - 1] = WALK_NONE;

			if (player._pmode == PM_STAND) {
				StartStand(pnum, player._pdir);
				player.destAction = ACTION_NONE;
			}
		}

		return;
	}
	if (player.destAction == ACTION_NONE) {
		return;
	}

	if (player._pmode == PM_STAND) {
		switch (player.destAction) {
		case ACTION_ATTACK:
			d = GetDirection(player.position.tile, { player.destParam1, player.destParam2 });
			StartAttack(pnum, d);
			break;
		case ACTION_ATTACKMON:
			i = player.destParam1;
			x = abs(player.position.tile.x - monster[i].position.future.x);
			y = abs(player.position.tile.y - monster[i].position.future.y);
			if (x <= 1 && y <= 1) {
				d = GetDirection(player.position.future, monster[i].position.future);
				if (monster[i].mtalkmsg != TEXT_NONE && monster[i].mtalkmsg != TEXT_VILE14) {
					TalktoMonster(i);
				} else {
					StartAttack(pnum, d);
				}
			}
			break;
		case ACTION_ATTACKPLR: {
			auto &target = plr[player.destParam1];
			x = abs(player.position.tile.x - target.position.future.x);
			y = abs(player.position.tile.y - target.position.future.y);
			if (x <= 1 && y <= 1) {
				d = GetDirection(player.position.future, target.position.future);
				StartAttack(pnum, d);
			}
		} break;
		case ACTION_RATTACK:
			d = GetDirection(player.position.tile, { player.destParam1, player.destParam2 });
			StartRangeAttack(pnum, d, player.destParam1, player.destParam2);
			break;
		case ACTION_RATTACKMON:
			i = player.destParam1;
			d = GetDirection(player.position.future, monster[i].position.future);
			if (monster[i].mtalkmsg != TEXT_NONE && monster[i].mtalkmsg != TEXT_VILE14) {
				TalktoMonster(i);
			} else {
				StartRangeAttack(pnum, d, monster[i].position.future.x, monster[i].position.future.y);
			}
			break;
		case ACTION_RATTACKPLR: {
			auto &target = plr[player.destParam1];
			d = GetDirection(player.position.future, target.position.future);
			StartRangeAttack(pnum, d, target.position.future.x, target.position.future.y);

		} break;
		case ACTION_SPELL:
			d = GetDirection(player.position.tile, { player.destParam1, player.destParam2 });
			StartSpell(pnum, d, player.destParam1, player.destParam2);
			player._pVar4 = player.destParam3;
			break;
		case ACTION_SPELLWALL:
			StartSpell(pnum, player.destParam3, player.destParam1, player.destParam2);
			player.tempDirection = player.destParam3;
			player._pVar4 = player.destParam4;
			break;
		case ACTION_SPELLMON:
			i = player.destParam1;
			d = GetDirection(player.position.tile, monster[i].position.future);
			StartSpell(pnum, d, monster[i].position.future.x, monster[i].position.future.y);
			player._pVar4 = player.destParam2;
			break;
		case ACTION_SPELLPLR: {
			auto &target = plr[player.destParam1];
			d = GetDirection(player.position.tile, target.position.future);
			StartSpell(pnum, d, target.position.future.x, target.position.future.y);
			player._pVar4 = player.destParam2;

		} break;
		case ACTION_OPERATE:
			i = player.destParam1;
			x = abs(player.position.tile.x - object[i].position.x);
			y = abs(player.position.tile.y - object[i].position.y);
			if (y > 1 && dObject[object[i].position.x][object[i].position.y - 1] == -(i + 1)) {
				y = abs(player.position.tile.y - object[i].position.y + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(player.position.tile, object[i].position);
					StartAttack(pnum, d);
				} else {
					OperateObject(pnum, i, false);
				}
			}
			break;
		case ACTION_DISARM:
			i = player.destParam1;
			x = abs(player.position.tile.x - object[i].position.x);
			y = abs(player.position.tile.y - object[i].position.y);
			if (y > 1 && dObject[object[i].position.x][object[i].position.y - 1] == -(i + 1)) {
				y = abs(player.position.tile.y - object[i].position.y + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(player.position.tile, object[i].position);
					StartAttack(pnum, d);
				} else {
					TryDisarm(pnum, i);
					OperateObject(pnum, i, false);
				}
			}
			break;
		case ACTION_OPERATETK:
			i = player.destParam1;
			if (object[i]._oBreak != 1) {
				OperateObject(pnum, i, true);
			}
			break;
		case ACTION_PICKUPITEM:
			if (pnum == myplr) {
				i = player.destParam1;
				x = abs(player.position.tile.x - items[i].position.x);
				y = abs(player.position.tile.y - items[i].position.y);
				if (x <= 1 && y <= 1 && pcurs == CURSOR_HAND && !items[i]._iRequest) {
					NetSendCmdGItem(true, CMD_REQUESTGITEM, myplr, myplr, i);
					items[i]._iRequest = true;
				}
			}
			break;
		case ACTION_PICKUPAITEM:
			if (pnum == myplr) {
				i = player.destParam1;
				x = abs(player.position.tile.x - items[i].position.x);
				y = abs(player.position.tile.y - items[i].position.y);
				if (x <= 1 && y <= 1 && pcurs == CURSOR_HAND) {
					NetSendCmdGItem(true, CMD_REQUESTAGITEM, myplr, myplr, i);
				}
			}
			break;
		case ACTION_TALK:
			if (pnum == myplr) {
				TalkToTowner(player, player.destParam1);
			}
			break;
		default:
			break;
		}

		FixPlayerLocation(pnum, player._pdir);
		player.destAction = ACTION_NONE;

		return;
	}

	auto &myPlayer = plr[myplr];

	if (player._pmode == PM_ATTACK && player.AnimInfo.CurrentFrame > myPlayer._pAFNum) {
		if (player.destAction == ACTION_ATTACK) {
			d = GetDirection(player.position.future, { player.destParam1, player.destParam2 });
			StartAttack(pnum, d);
			player.destAction = ACTION_NONE;
		} else if (player.destAction == ACTION_ATTACKMON) {
			i = player.destParam1;
			x = abs(player.position.tile.x - monster[i].position.future.x);
			y = abs(player.position.tile.y - monster[i].position.future.y);
			if (x <= 1 && y <= 1) {
				d = GetDirection(player.position.future, monster[i].position.future);
				StartAttack(pnum, d);
			}
			player.destAction = ACTION_NONE;
		} else if (player.destAction == ACTION_ATTACKPLR) {
			auto &target = plr[player.destParam1];
			x = abs(player.position.tile.x - target.position.future.x);
			y = abs(player.position.tile.y - target.position.future.y);
			if (x <= 1 && y <= 1) {
				d = GetDirection(player.position.future, target.position.future);
				StartAttack(pnum, d);
			}
			player.destAction = ACTION_NONE;
		} else if (player.destAction == ACTION_OPERATE) {
			i = player.destParam1;
			x = abs(player.position.tile.x - object[i].position.x);
			y = abs(player.position.tile.y - object[i].position.y);
			if (y > 1 && dObject[object[i].position.x][object[i].position.y - 1] == -(i + 1)) {
				y = abs(player.position.tile.y - object[i].position.y + 1);
			}
			if (x <= 1 && y <= 1) {
				if (object[i]._oBreak == 1) {
					d = GetDirection(player.position.tile, object[i].position);
					StartAttack(pnum, d);
				}
			}
		}
	}

	if (player._pmode == PM_RATTACK && player.AnimInfo.CurrentFrame > myPlayer._pAFNum) {
		if (player.destAction == ACTION_RATTACK) {
			d = GetDirection(player.position.tile, { player.destParam1, player.destParam2 });
			StartRangeAttack(pnum, d, player.destParam1, player.destParam2);
			player.destAction = ACTION_NONE;
		} else if (player.destAction == ACTION_RATTACKMON) {
			i = player.destParam1;
			d = GetDirection(player.position.tile, monster[i].position.future);
			StartRangeAttack(pnum, d, monster[i].position.future.x, monster[i].position.future.y);
			player.destAction = ACTION_NONE;
		} else if (player.destAction == ACTION_RATTACKPLR) {
			auto &target = plr[player.destParam1];
			d = GetDirection(player.position.tile, target.position.future);
			StartRangeAttack(pnum, d, target.position.future.x, target.position.future.y);
			player.destAction = ACTION_NONE;
		}
	}

	if (player._pmode == PM_SPELL && player.AnimInfo.CurrentFrame > player._pSFNum) {
		if (player.destAction == ACTION_SPELL) {
			d = GetDirection(player.position.tile, { player.destParam1, player.destParam2 });
			StartSpell(pnum, d, player.destParam1, player.destParam2);
			player.destAction = ACTION_NONE;
		} else if (player.destAction == ACTION_SPELLMON) {
			i = player.destParam1;
			d = GetDirection(player.position.tile, monster[i].position.future);
			StartSpell(pnum, d, monster[i].position.future.x, monster[i].position.future.y);
			player.destAction = ACTION_NONE;
		} else if (player.destAction == ACTION_SPELLPLR) {
			auto &target = plr[player.destParam1];
			d = GetDirection(player.position.tile, target.position.future);
			StartSpell(pnum, d, target.position.future.x, target.position.future.y);
			player.destAction = ACTION_NONE;
		}
	}
}

bool PlrDeathModeOK(int p)
{
	if (p != myplr) {
		return true;
	}

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("PlrDeathModeOK: illegal player %i", p);
	}
	auto &player = plr[p];

	if (player._pmode == PM_DEATH) {
		return true;
	}
	if (player._pmode == PM_QUIT) {
		return true;
	} else if (player._pmode == PM_NEWLVL) {
		return true;
	}

	return false;
}

void ValidatePlayer()
{
	int gt, i, b;

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("ValidatePlayer: illegal player %i", myplr);
	}
	auto &myPlayer = plr[myplr];

	if (myPlayer._pLevel > MAXCHARLEVEL - 1)
		myPlayer._pLevel = MAXCHARLEVEL - 1;
	if (myPlayer._pExperience > myPlayer._pNextExper) {
		myPlayer._pExperience = myPlayer._pNextExper;
		if (sgOptions.Gameplay.bExperienceBar) {
			force_redraw = 255;
		}
	}

	gt = 0;
	for (i = 0; i < myPlayer._pNumInv; i++) {
		if (myPlayer.InvList[i]._itype == ITYPE_GOLD) {
			int maxGold = GOLD_MAX_LIMIT;
			if (gbIsHellfire) {
				maxGold *= 2;
			}
			if (myPlayer.InvList[i]._ivalue > maxGold) {
				myPlayer.InvList[i]._ivalue = maxGold;
			}
			gt += myPlayer.InvList[i]._ivalue;
		}
	}
	if (gt != myPlayer._pGold)
		myPlayer._pGold = gt;

	if (myPlayer._pBaseStr > myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength)) {
		myPlayer._pBaseStr = myPlayer.GetMaximumAttributeValue(CharacterAttribute::Strength);
	}
	if (myPlayer._pBaseMag > myPlayer.GetMaximumAttributeValue(CharacterAttribute::Magic)) {
		myPlayer._pBaseMag = myPlayer.GetMaximumAttributeValue(CharacterAttribute::Magic);
	}
	if (myPlayer._pBaseDex > myPlayer.GetMaximumAttributeValue(CharacterAttribute::Dexterity)) {
		myPlayer._pBaseDex = myPlayer.GetMaximumAttributeValue(CharacterAttribute::Dexterity);
	}
	if (myPlayer._pBaseVit > myPlayer.GetMaximumAttributeValue(CharacterAttribute::Vitality)) {
		myPlayer._pBaseVit = myPlayer.GetMaximumAttributeValue(CharacterAttribute::Vitality);
	}

	uint64_t msk = 0;
	for (b = SPL_FIREBOLT; b < MAX_SPELLS; b++) {
		if (GetSpellBookLevel((spell_id)b) != -1) {
			msk |= GetSpellBitmask(b);
			if (myPlayer._pSplLvl[b] > MAX_SPELL_LEVEL)
				myPlayer._pSplLvl[b] = MAX_SPELL_LEVEL;
		}
	}

	myPlayer._pMemSpells &= msk;
}

static void CheckCheatStats(PlayerStruct &player)
{
	if (player._pStrength > 750) {
		player._pStrength = 750;
	}

	if (player._pDexterity > 750) {
		player._pDexterity = 750;
	}

	if (player._pMagic > 750) {
		player._pMagic = 750;
	}

	if (player._pVitality > 750) {
		player._pVitality = 750;
	}

	if (player._pHitPoints > 128000) {
		player._pHitPoints = 128000;
	}

	if (player._pMana > 128000) {
		player._pMana = 128000;
	}
}

void ProcessPlayers()
{
	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("ProcessPlayers: illegal player %i", myplr);
	}
	auto &myPlayer = plr[myplr];

	if (myPlayer.pLvlLoad > 0) {
		myPlayer.pLvlLoad--;
	}

	if (sfxdelay > 0) {
		sfxdelay--;
		if (sfxdelay == 0) {
			switch (sfxdnum) {
			case USFX_DEFILER1:
				InitQTextMsg(TEXT_DEFILER1);
				break;
			case USFX_DEFILER2:
				InitQTextMsg(TEXT_DEFILER2);
				break;
			case USFX_DEFILER3:
				InitQTextMsg(TEXT_DEFILER3);
				break;
			case USFX_DEFILER4:
				InitQTextMsg(TEXT_DEFILER4);
				break;
			default:
				PlaySFX(sfxdnum);
			}
		}
	}

	ValidatePlayer();

	for (int pnum = 0; pnum < MAX_PLRS; pnum++) {
		auto &player = plr[pnum];
		if (player.plractive && currlevel == player.plrlevel && (pnum == myplr || !player._pLvlChanging)) {
			CheckCheatStats(player);

			if (!PlrDeathModeOK(pnum) && (player._pHitPoints >> 6) <= 0) {
				SyncPlrKill(pnum, -1);
			}

			if (pnum == myplr) {
				if ((player._pIFlags & ISPL_DRAINLIFE) && currlevel != 0) {
					ApplyPlrDamage(pnum, 0, 0, 4);
				}
				if (player._pIFlags & ISPL_NOMANA && player._pManaBase > 0) {
					player._pManaBase -= player._pMana;
					player._pMana = 0;
					drawmanaflag = true;
				}
			}

			bool tplayer = false;
			do {
				switch (player._pmode) {
				case PM_STAND:
				case PM_NEWLVL:
				case PM_QUIT:
					tplayer = false;
					break;
				case PM_WALK:
				case PM_WALK2:
				case PM_WALK3:
					tplayer = PM_DoWalk(pnum, player._pmode);
					break;
				case PM_ATTACK:
					tplayer = PM_DoAttack(pnum);
					break;
				case PM_RATTACK:
					tplayer = PM_DoRangeAttack(pnum);
					break;
				case PM_BLOCK:
					tplayer = PM_DoBlock(pnum);
					break;
				case PM_SPELL:
					tplayer = PM_DoSpell(pnum);
					break;
				case PM_GOTHIT:
					tplayer = PM_DoGotHit(pnum);
					break;
				case PM_DEATH:
					tplayer = PM_DoDeath(pnum);
					break;
				}
				CheckNewPath(pnum, tplayer);
			} while (tplayer);

			player.AnimInfo.ProcessAnimation();
		}
	}
}

void ClrPlrPath(PlayerStruct &player)
{
	memset(player.walkpath, WALK_NONE, sizeof(player.walkpath));
}

bool PosOkPlayer(int pnum, int x, int y)
{
	int8_t p, bv;

	if (x < 0 || x >= MAXDUNX || y < 0 || y >= MAXDUNY)
		return false;
	if (dPiece[x][y] == 0)
		return false;
	if (SolidLoc(x, y))
		return false;
	if (dPlayer[x][y] != 0) {
		if (dPlayer[x][y] > 0) {
			p = dPlayer[x][y] - 1;
		} else {
			p = -(dPlayer[x][y] + 1);
		}
		if (p != pnum
		    && p >= 0
		    && p < MAX_PLRS
		    && plr[p]._pHitPoints != 0) {
			return false;
		}
	}

	if (dMonster[x][y] != 0) {
		if (currlevel == 0) {
			return false;
		}
		if (dMonster[x][y] <= 0) {
			return false;
		}
		if ((monster[dMonster[x][y] - 1]._mhitpoints >> 6) > 0) {
			return false;
		}
	}

	if (dObject[x][y] != 0) {
		if (dObject[x][y] > 0) {
			bv = dObject[x][y] - 1;
		} else {
			bv = -(dObject[x][y] + 1);
		}
		if (object[bv]._oSolidFlag) {
			return false;
		}
	}

	return true;
}

void MakePlrPath(int pnum, int xx, int yy, bool endspace)
{
	int path;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("MakePlrPath: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	if (player.position.future.x == xx && player.position.future.y == yy) {
		return;
	}

	path = FindPath(PosOkPlayer, pnum, player.position.future.x, player.position.future.y, xx, yy, player.walkpath);
	if (!path) {
		return;
	}

	if (!endspace) {
		path--;

		switch (player.walkpath[path]) {
		case WALK_NE:
			yy++;
			break;
		case WALK_NW:
			xx++;
			break;
		case WALK_SE:
			xx--;
			break;
		case WALK_SW:
			yy--;
			break;
		case WALK_N:
			xx++;
			yy++;
			break;
		case WALK_E:
			xx--;
			yy++;
			break;
		case WALK_S:
			xx--;
			yy--;
			break;
		case WALK_W:
			xx++;
			yy--;
			break;
		}
	}

	player.walkpath[path] = WALK_NONE;
}

void CheckPlrSpell()
{
	bool addflag = false;
	int sl;

	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("CheckPlrSpell: illegal player %i", myplr);
	}
	auto &myPlayer = plr[myplr];

	spell_id rspell = myPlayer._pRSpell;
	if (rspell == SPL_INVALID) {
		myPlayer.PlaySpeach(34);
		return;
	}

	if (leveltype == DTYPE_TOWN && !spelldata[rspell].sTownSpell) {
		myPlayer.PlaySpeach(27);
		return;
	}

	if (!sgbControllerActive) {
		if (pcurs != CURSOR_HAND)
			return;

		if (MouseY >= PANEL_TOP && MouseX >= PANEL_LEFT && MouseX <= RIGHT_PANEL) // inside main panel
			return;

		if (
		    ((chrflag || questlog) && MouseX < SPANEL_WIDTH && MouseY < SPANEL_HEIGHT)    // inside left panel
		    || ((invflag || sbookflag) && MouseX > RIGHT_PANEL && MouseY < SPANEL_HEIGHT) // inside right panel
		) {
			if (rspell != SPL_HEAL
			    && rspell != SPL_IDENTIFY
			    && rspell != SPL_REPAIR
			    && rspell != SPL_INFRA
			    && rspell != SPL_RECHARGE)
				return;
		}
	}

	switch (myPlayer._pRSplType) {
	case RSPLTYPE_SKILL:
	case RSPLTYPE_SPELL:
		addflag = CheckSpell(myplr, rspell, myPlayer._pRSplType, false);
		break;
	case RSPLTYPE_SCROLL:
		addflag = UseScroll();
		break;
	case RSPLTYPE_CHARGES:
		addflag = UseStaff();
		break;
	case RSPLTYPE_INVALID:
		return;
	}

	if (addflag) {
		if (myPlayer._pRSpell == SPL_FIREWALL || myPlayer._pRSpell == SPL_LIGHTWALL) {
			direction sd = GetDirection(myPlayer.position.tile, { cursmx, cursmy });
			sl = GetSpellLevel(myplr, myPlayer._pRSpell);
			NetSendCmdLocParam3(true, CMD_SPELLXYD, { cursmx, cursmy }, myPlayer._pRSpell, sd, sl);
		} else if (pcursmonst != -1) {
			sl = GetSpellLevel(myplr, myPlayer._pRSpell);
			NetSendCmdParam3(true, CMD_SPELLID, pcursmonst, myPlayer._pRSpell, sl);
		} else if (pcursplr != -1) {
			sl = GetSpellLevel(myplr, myPlayer._pRSpell);
			NetSendCmdParam3(true, CMD_SPELLPID, pcursplr, myPlayer._pRSpell, sl);
		} else { //145
			sl = GetSpellLevel(myplr, myPlayer._pRSpell);
			NetSendCmdLocParam2(true, CMD_SPELLXY, { cursmx, cursmy }, myPlayer._pRSpell, sl);
		}
		return;
	}

	if (myPlayer._pRSplType == RSPLTYPE_SPELL) {
		myPlayer.PlaySpeach(35);
	}
}

void SyncPlrAnim(int pnum)
{
	int dir, sType;

	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SyncPlrAnim: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	dir = player._pdir;
	switch (player._pmode) {
	case PM_STAND:
		player.AnimInfo.pData = player._pNAnim[dir];
		break;
	case PM_WALK:
	case PM_WALK2:
	case PM_WALK3:
		player.AnimInfo.pData = player._pWAnim[dir];
		break;
	case PM_ATTACK:
		player.AnimInfo.pData = player._pAAnim[dir];
		break;
	case PM_RATTACK:
		player.AnimInfo.pData = player._pAAnim[dir];
		break;
	case PM_BLOCK:
		player.AnimInfo.pData = player._pBAnim[dir];
		break;
	case PM_SPELL:
		if (pnum == myplr)
			sType = spelldata[player._pSpell].sType;
		else
			sType = STYPE_FIRE;
		if (sType == STYPE_FIRE)
			player.AnimInfo.pData = player._pFAnim[dir];
		if (sType == STYPE_LIGHTNING)
			player.AnimInfo.pData = player._pLAnim[dir];
		if (sType == STYPE_MAGIC)
			player.AnimInfo.pData = player._pTAnim[dir];
		break;
	case PM_GOTHIT:
		player.AnimInfo.pData = player._pHAnim[dir];
		break;
	case PM_NEWLVL:
		player.AnimInfo.pData = player._pNAnim[dir];
		break;
	case PM_DEATH:
		player.AnimInfo.pData = player._pDAnim[dir];
		break;
	case PM_QUIT:
		player.AnimInfo.pData = player._pNAnim[dir];
		break;
	default:
		app_fatal("SyncPlrAnim");
	}
}

void SyncInitPlrPos(int pnum)
{
	int x, y, xx, yy, range;
	DWORD i;
	bool posOk;

	auto &player = plr[pnum];

	if (!gbIsMultiplayer || player.plrlevel != currlevel) {
		return;
	}

	for (i = 0; i < 8; i++) {
		x = player.position.tile.x + plrxoff2[i];
		y = player.position.tile.y + plryoff2[i];
		if (PosOkPlayer(pnum, x, y)) {
			break;
		}
	}

	if (!PosOkPlayer(pnum, x, y)) {
		posOk = false;
		for (range = 1; range < 50 && !posOk; range++) {
			for (yy = -range; yy <= range && !posOk; yy++) {
				y = yy + player.position.tile.y;
				for (xx = -range; xx <= range && !posOk; xx++) {
					x = xx + player.position.tile.x;
					if (PosOkPlayer(pnum, x, y) && !PosOkPortal(currlevel, x, y)) {
						posOk = true;
					}
				}
			}
		}
	}

	player.position.tile = { x, y };
	dPlayer[x][y] = pnum + 1;

	if (pnum == myplr) {
		player.position.future = { x, y };
		ViewX = x;
		ViewY = y;
	}
}

void SyncInitPlr(int pnum)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SyncInitPlr: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	SetPlrAnims(player);
	SyncInitPlrPos(pnum);
}

void CheckStats(PlayerStruct &player)
{
	for (auto attribute : enum_values<CharacterAttribute>()) {
		int maxStatPoint = player.GetMaximumAttributeValue(attribute);
		switch (attribute) {
		case CharacterAttribute::Strength:
			if (player._pBaseStr > maxStatPoint) {
				player._pBaseStr = maxStatPoint;
			} else if (player._pBaseStr < 0) {
				player._pBaseStr = 0;
			}
			break;
		case CharacterAttribute::Magic:
			if (player._pBaseMag > maxStatPoint) {
				player._pBaseMag = maxStatPoint;
			} else if (player._pBaseMag < 0) {
				player._pBaseMag = 0;
			}
			break;
		case CharacterAttribute::Dexterity:
			if (player._pBaseDex > maxStatPoint) {
				player._pBaseDex = maxStatPoint;
			} else if (player._pBaseDex < 0) {
				player._pBaseDex = 0;
			}
			break;
		case CharacterAttribute::Vitality:
			if (player._pBaseVit > maxStatPoint) {
				player._pBaseVit = maxStatPoint;
			} else if (player._pBaseVit < 0) {
				player._pBaseVit = 0;
			}
			break;
		}
	}
}

void ModifyPlrStr(int p, int l)
{
	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrStr: illegal player %i", p);
	}
	auto &player = plr[p];

	int max = player.GetMaximumAttributeValue(CharacterAttribute::Strength);
	if (player._pBaseStr + l > max) {
		l = max - player._pBaseStr;
	}

	player._pStrength += l;
	player._pBaseStr += l;

	if (player._pClass == HeroClass::Rogue) {
		player._pDamageMod = player._pLevel * (player._pStrength + player._pDexterity) / 200;
	} else {
		player._pDamageMod = player._pLevel * player._pStrength / 100;
	}

	CalcPlrInv(p, true);

	if (p == myplr) {
		NetSendCmdParam1(false, CMD_SETSTR, player._pBaseStr);
	}
}

void ModifyPlrMag(int p, int l)
{
	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrMag: illegal player %i", p);
	}
	auto &player = plr[p];

	int max = player.GetMaximumAttributeValue(CharacterAttribute::Magic);
	if (player._pBaseMag + l > max) {
		l = max - player._pBaseMag;
	}

	player._pMagic += l;
	player._pBaseMag += l;

	int ms = l << 6;
	if (player._pClass == HeroClass::Sorcerer) {
		ms *= 2;
	} else if (player._pClass == HeroClass::Bard) {
		ms += ms / 2;
	}

	player._pMaxManaBase += ms;
	player._pMaxMana += ms;
	if ((player._pIFlags & ISPL_NOMANA) == 0) {
		player._pManaBase += ms;
		player._pMana += ms;
	}

	CalcPlrInv(p, true);

	if (p == myplr) {
		NetSendCmdParam1(false, CMD_SETMAG, player._pBaseMag);
	}
}

void ModifyPlrDex(int p, int l)
{
	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrDex: illegal player %i", p);
	}
	auto &player = plr[p];

	int max = player.GetMaximumAttributeValue(CharacterAttribute::Dexterity);
	if (player._pBaseDex + l > max) {
		l = max - player._pBaseDex;
	}

	player._pDexterity += l;
	player._pBaseDex += l;
	CalcPlrInv(p, true);

	if (player._pClass == HeroClass::Rogue) {
		player._pDamageMod = player._pLevel * (player._pDexterity + player._pStrength) / 200;
	}

	if (p == myplr) {
		NetSendCmdParam1(false, CMD_SETDEX, player._pBaseDex);
	}
}

void ModifyPlrVit(int p, int l)
{
	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("ModifyPlrVit: illegal player %i", p);
	}
	auto &player = plr[p];

	int max = player.GetMaximumAttributeValue(CharacterAttribute::Vitality);
	if (player._pBaseVit + l > max) {
		l = max - player._pBaseVit;
	}

	player._pVitality += l;
	player._pBaseVit += l;

	int ms = l << 6;
	if (player._pClass == HeroClass::Warrior) {
		ms *= 2;
	} else if (player._pClass == HeroClass::Barbarian) {
		ms *= 2;
	}

	player._pHPBase += ms;
	player._pMaxHPBase += ms;
	player._pHitPoints += ms;
	player._pMaxHP += ms;

	CalcPlrInv(p, true);

	if (p == myplr) {
		NetSendCmdParam1(false, CMD_SETVIT, player._pBaseVit);
	}
}

void SetPlayerHitPoints(int pnum, int val)
{
	if ((DWORD)pnum >= MAX_PLRS) {
		app_fatal("SetPlayerHitPoints: illegal player %i", pnum);
	}
	auto &player = plr[pnum];

	player._pHitPoints = val;
	player._pHPBase = val + player._pMaxHPBase - player._pMaxHP;

	if (pnum == myplr) {
		drawhpflag = true;
	}
}

void SetPlrStr(int p, int v)
{
	int dm;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrStr: illegal player %i", p);
	}
	auto &player = plr[p];

	player._pBaseStr = v;
	CalcPlrInv(p, true);

	if (player._pClass == HeroClass::Rogue) {
		dm = player._pLevel * (player._pStrength + player._pDexterity) / 200;
	} else {
		dm = player._pLevel * player._pStrength / 100;
	}

	player._pDamageMod = dm;
}

void SetPlrMag(int p, int v)
{
	int m;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrMag: illegal player %i", p);
	}
	auto &player = plr[p];

	player._pBaseMag = v;

	m = v << 6;
	if (player._pClass == HeroClass::Sorcerer) {
		m *= 2;
	} else if (player._pClass == HeroClass::Bard) {
		m += m / 2;
	}

	player._pMaxManaBase = m;
	player._pMaxMana = m;
	CalcPlrInv(p, true);
}

void SetPlrDex(int p, int v)
{
	int dm;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrDex: illegal player %i", p);
	}
	auto &player = plr[p];

	player._pBaseDex = v;
	CalcPlrInv(p, true);

	if (player._pClass == HeroClass::Rogue) {
		dm = player._pLevel * (player._pStrength + player._pDexterity) / 200;
	} else {
		dm = player._pStrength * player._pLevel / 100;
	}

	player._pDamageMod = dm;
}

void SetPlrVit(int p, int v)
{
	int hp;

	if ((DWORD)p >= MAX_PLRS) {
		app_fatal("SetPlrVit: illegal player %i", p);
	}
	auto &player = plr[p];

	player._pBaseVit = v;

	hp = v << 6;
	if (player._pClass == HeroClass::Warrior) {
		hp *= 2;
	} else if (player._pClass == HeroClass::Barbarian) {
		hp *= 2;
	}

	player._pHPBase = hp;
	player._pMaxHPBase = hp;
	CalcPlrInv(p, true);
}

void InitDungMsgs(PlayerStruct &player)
{
	player.pDungMsgs = 0;
	player.pDungMsgs2 = 0;
}

enum {
	// clang-format off
	DMSG_CATHEDRAL = 1 << 0,
	DMSG_CATACOMBS = 1 << 1,
	DMSG_CAVES     = 1 << 2,
	DMSG_HELL      = 1 << 3,
	DMSG_DIABLO    = 1 << 4,
	// clang-format on
};

void PlayDungMsgs()
{
	if ((DWORD)myplr >= MAX_PLRS) {
		app_fatal("PlayDungMsgs: illegal player %i", myplr);
	}
	auto &myPlayer = plr[myplr];

	if (currlevel == 1 && !myPlayer._pLvlVisited[1] && !gbIsMultiplayer && !(myPlayer.pDungMsgs & DMSG_CATHEDRAL)) {
		myPlayer.PlaySpeach(97, 40);
		myPlayer.pDungMsgs = myPlayer.pDungMsgs | DMSG_CATHEDRAL;
	} else if (currlevel == 5 && !myPlayer._pLvlVisited[5] && !gbIsMultiplayer && !(myPlayer.pDungMsgs & DMSG_CATACOMBS)) {
		myPlayer.PlaySpeach(96, 40);
		myPlayer.pDungMsgs |= DMSG_CATACOMBS;
	} else if (currlevel == 9 && !myPlayer._pLvlVisited[9] && !gbIsMultiplayer && !(myPlayer.pDungMsgs & DMSG_CAVES)) {
		myPlayer.PlaySpeach(98, 40);
		myPlayer.pDungMsgs |= DMSG_CAVES;
	} else if (currlevel == 13 && !myPlayer._pLvlVisited[13] && !gbIsMultiplayer && !(myPlayer.pDungMsgs & DMSG_HELL)) {
		myPlayer.PlaySpeach(99, 40);
		myPlayer.pDungMsgs |= DMSG_HELL;
	} else if (currlevel == 16 && !myPlayer._pLvlVisited[15] && !gbIsMultiplayer && !(myPlayer.pDungMsgs & DMSG_DIABLO)) { // BUGFIX: _pLvlVisited should check 16 or this message will never play
		sfxdelay = 40;
		sfxdnum = PS_DIABLVLINT;
		myPlayer.pDungMsgs |= DMSG_DIABLO;
	} else if (currlevel == 17 && !myPlayer._pLvlVisited[17] && !gbIsMultiplayer && !(myPlayer.pDungMsgs2 & 1)) {
		sfxdelay = 10;
		sfxdnum = USFX_DEFILER1;
		quests[Q_DEFILER]._qactive = QUEST_ACTIVE;
		quests[Q_DEFILER]._qlog = true;
		quests[Q_DEFILER]._qmsg = TEXT_DEFILER1;
		myPlayer.pDungMsgs2 |= 1;
	} else if (currlevel == 19 && !myPlayer._pLvlVisited[19] && !gbIsMultiplayer && !(myPlayer.pDungMsgs2 & 4)) {
		sfxdelay = 10;
		sfxdnum = USFX_DEFILER3;
		myPlayer.pDungMsgs2 |= 4;
	} else if (currlevel == 21 && !myPlayer._pLvlVisited[21] && !gbIsMultiplayer && !(myPlayer.pDungMsgs & 32)) {
		myPlayer.PlaySpeach(92, 30);
		myPlayer.pDungMsgs |= 32;
	} else {
		sfxdelay = 0;
	}
}

} // namespace devilution
