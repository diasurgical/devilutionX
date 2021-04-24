/**
 * @file textdat.h
 *
 * Interface of all dialog texts.
 */
#pragma once

#include <string_view>

#include "effects.h"

namespace devilution {

enum _speech_id : int16_t {
	TEXT_KING1,
	TEXT_KING2,
	TEXT_KING3,
	TEXT_KING4,
	TEXT_KING5,
	TEXT_KING6,
	TEXT_KING7,
	TEXT_KING8,
	TEXT_KING9,
	TEXT_KING10,
	TEXT_KING11,
	TEXT_BANNER1,
	TEXT_BANNER2,
	TEXT_BANNER3,
	TEXT_BANNER4,
	TEXT_BANNER5,
	TEXT_BANNER6,
	TEXT_BANNER7,
	TEXT_BANNER8,
	TEXT_BANNER9,
	TEXT_BANNER10,
	TEXT_BANNER11,
	TEXT_BANNER12,
	TEXT_VILE1,
	TEXT_VILE2,
	TEXT_VILE3,
	TEXT_VILE4,
	TEXT_VILE5,
	TEXT_VILE6,
	TEXT_VILE7,
	TEXT_VILE8,
	TEXT_VILE9,
	TEXT_VILE10,
	TEXT_VILE11,
	TEXT_VILE12,
	TEXT_VILE13,
	TEXT_VILE14,
	TEXT_POISON1,
	TEXT_POISON2,
	TEXT_POISON3,
	TEXT_POISON4,
	TEXT_POISON5,
	TEXT_POISON6,
	TEXT_POISON7,
	TEXT_POISON8,
	TEXT_POISON9,
	TEXT_POISON10,
	TEXT_BONE1,
	TEXT_BONE2,
	TEXT_BONE3,
	TEXT_BONE4,
	TEXT_BONE5,
	TEXT_BONE6,
	TEXT_BONE7,
	TEXT_BONE8,
	TEXT_BUTCH1,
	TEXT_BUTCH2,
	TEXT_BUTCH3,
	TEXT_BUTCH4,
	TEXT_BUTCH5,
	TEXT_BUTCH6,
	TEXT_BUTCH7,
	TEXT_BUTCH8,
	TEXT_BUTCH9,
	TEXT_BUTCH10,
	TEXT_BLIND1,
	TEXT_BLIND2,
	TEXT_BLIND3,
	TEXT_BLIND4,
	TEXT_BLIND5,
	TEXT_BLIND6,
	TEXT_BLIND7,
	TEXT_BLIND8,
	TEXT_VEIL1,
	TEXT_VEIL2,
	TEXT_VEIL3,
	TEXT_VEIL4,
	TEXT_VEIL5,
	TEXT_VEIL6,
	TEXT_VEIL7,
	TEXT_VEIL8,
	TEXT_VEIL9,
	TEXT_VEIL10,
	TEXT_VEIL11,
	TEXT_ANVIL1,
	TEXT_ANVIL2,
	TEXT_ANVIL3,
	TEXT_ANVIL4,
	TEXT_ANVIL5,
	TEXT_ANVIL6,
	TEXT_ANVIL7,
	TEXT_ANVIL8,
	TEXT_ANVIL9,
	TEXT_ANVIL10,
	TEXT_BLOOD1,
	TEXT_BLOOD2,
	TEXT_BLOOD3,
	TEXT_BLOOD4,
	TEXT_BLOOD5,
	TEXT_BLOOD6,
	TEXT_BLOOD7,
	TEXT_BLOOD8,
	TEXT_WARLRD1,
	TEXT_WARLRD2,
	TEXT_WARLRD3,
	TEXT_WARLRD4,
	TEXT_WARLRD5,
	TEXT_WARLRD6,
	TEXT_WARLRD7,
	TEXT_WARLRD8,
	TEXT_WARLRD9,
	TEXT_INFRA1,
	TEXT_INFRA2,
	TEXT_INFRA3,
	TEXT_INFRA4,
	TEXT_INFRA5,
	TEXT_INFRA6,
	TEXT_INFRA7,
	TEXT_INFRA8,
	TEXT_INFRA9,
	TEXT_INFRA10,
	TEXT_MUSH1,
	TEXT_MUSH2,
	TEXT_MUSH3,
	TEXT_MUSH4,
	TEXT_MUSH5,
	TEXT_MUSH6,
	TEXT_MUSH7,
	TEXT_MUSH8,
	TEXT_MUSH9,
	TEXT_MUSH10,
	TEXT_MUSH11,
	TEXT_MUSH12,
	TEXT_MUSH13,
	TEXT_DOOM1,
	TEXT_DOOM2,
	TEXT_DOOM3,
	TEXT_DOOM4,
	TEXT_DOOM5,
	TEXT_DOOM6,
	TEXT_DOOM7,
	TEXT_DOOM8,
	TEXT_DOOM9,
	TEXT_DOOM10,
	TEXT_GARBUD1,
	TEXT_GARBUD2,
	TEXT_GARBUD3,
	TEXT_GARBUD4,
	TEXT_ZHAR1,
	TEXT_ZHAR2,
	TEXT_STORY1,
	TEXT_STORY2,
	TEXT_STORY3,
	TEXT_STORY4,
	TEXT_STORY5,
	TEXT_STORY6,
	TEXT_STORY7,
	TEXT_STORY9,
	TEXT_STORY10,
	TEXT_STORY11,
	TEXT_OGDEN1,
	TEXT_OGDEN2,
	TEXT_OGDEN3,
	TEXT_OGDEN4,
	TEXT_OGDEN5,
	TEXT_OGDEN6,
	TEXT_OGDEN8,
	TEXT_OGDEN9,
	TEXT_OGDEN10,
	TEXT_PEPIN1,
	TEXT_PEPIN2,
	TEXT_PEPIN3,
	TEXT_PEPIN4,
	TEXT_PEPIN5,
	TEXT_PEPIN6,
	TEXT_PEPIN7,
	TEXT_PEPIN9,
	TEXT_PEPIN10,
	TEXT_PEPIN11,
	TEXT_GILLIAN1,
	TEXT_GILLIAN2,
	TEXT_GILLIAN3,
	TEXT_GILLIAN4,
	TEXT_GILLIAN5,
	TEXT_GILLIAN6,
	TEXT_GILLIAN7,
	TEXT_GILLIAN9,
	TEXT_GILLIAN10,
	TEXT_GRISWOLD1,
	TEXT_GRISWOLD2,
	TEXT_GRISWOLD3,
	TEXT_GRISWOLD4,
	TEXT_GRISWOLD5,
	TEXT_GRISWOLD6,
	TEXT_GRISWOLD7,
	TEXT_GRISWOLD8,
	TEXT_GRISWOLD9,
	TEXT_GRISWOLD10,
	TEXT_GRISWOLD12,
	TEXT_GRISWOLD13,
	TEXT_FARNHAM1,
	TEXT_FARNHAM2,
	TEXT_FARNHAM3,
	TEXT_FARNHAM4,
	TEXT_FARNHAM5,
	TEXT_FARNHAM6,
	TEXT_FARNHAM8,
	TEXT_FARNHAM9,
	TEXT_FARNHAM10,
	TEXT_FARNHAM11,
	TEXT_FARNHAM12,
	TEXT_FARNHAM13,
	TEXT_ADRIA1,
	TEXT_ADRIA2,
	TEXT_ADRIA3,
	TEXT_ADRIA4,
	TEXT_ADRIA5,
	TEXT_ADRIA6,
	TEXT_ADRIA7,
	TEXT_ADRIA8,
	TEXT_ADRIA9,
	TEXT_ADRIA10,
	TEXT_ADRIA12,
	TEXT_ADRIA13,
	TEXT_WIRT1,
	TEXT_WIRT2,
	TEXT_WIRT3,
	TEXT_WIRT4,
	TEXT_WIRT5,
	TEXT_WIRT6,
	TEXT_WIRT7,
	TEXT_WIRT8,
	TEXT_WIRT9,
	TEXT_WIRT11,
	TEXT_WIRT12,
	TEXT_BONER,
	TEXT_BLOODY,
	TEXT_BLINDING,
	TEXT_BLOODWAR,
	TEXT_MBONER,
	TEXT_MBLOODY,
	TEXT_MBLINDING,
	TEXT_MBLOODWAR,
	TEXT_RBONER,
	TEXT_RBLOODY,
	TEXT_RBLINDING,
	TEXT_RBLOODWAR,
	TEXT_COW1,
	TEXT_COW2,
	TEXT_BOOK11,
	TEXT_BOOK12,
	TEXT_BOOK13,
	TEXT_BOOK21,
	TEXT_BOOK22,
	TEXT_BOOK23,
	TEXT_BOOK31,
	TEXT_BOOK32,
	TEXT_BOOK33,
	TEXT_INTRO,
	TEXT_HBONER,
	TEXT_HBLOODY,
	TEXT_HBLINDING,
	TEXT_HBLOODWAR,
	TEXT_BBONER,
	TEXT_BBLOODY,
	TEXT_BBLINDING,
	TEXT_BBLOODWAR,
	TEXT_GRAVE1,
	TEXT_GRAVE2,
	TEXT_GRAVE3,
	TEXT_GRAVE4,
	TEXT_GRAVE5,
	TEXT_GRAVE6,
	TEXT_GRAVE7,
	TEXT_GRAVE8,
	TEXT_GRAVE9,
	TEXT_GRAVE10,
	TEXT_FARMER1,
	TEXT_FARMER2,
	TEXT_FARMER3,
	TEXT_FARMER4,
	TEXT_FARMER5,
	TEXT_GIRL1,
	TEXT_GIRL2,
	TEXT_GIRL3,
	TEXT_GIRL4,
	TEXT_DEFILER1,
	TEXT_DEFILER2,
	TEXT_DEFILER3,
	TEXT_DEFILER4,
	TEXT_DEFILER5,
	TEXT_NAKRUL1,
	TEXT_NAKRUL2,
	TEXT_NAKRUL3,
	TEXT_NAKRUL4,
	TEXT_NAKRUL5,
	TEXT_CORNSTN,
	TEXT_JERSEY1,
	TEXT_JERSEY2,
	TEXT_JERSEY3,
	TEXT_JERSEY4,
	TEXT_JERSEY5,
	TEXT_JERSEY6,
	TEXT_JERSEY7,
	TEXT_JERSEY8,
	TEXT_JERSEY9,
	TEXT_TRADER,
	TEXT_FARMER6,
	TEXT_FARMER7,
	TEXT_FARMER8,
	TEXT_FARMER9,
	TEXT_FARMER10,
	TEXT_JERSEY10,
	TEXT_JERSEY11,
	TEXT_JERSEY12,
	TEXT_JERSEY13,
	TEXT_SKLJRN,
	TEXT_BOOK4,
	TEXT_BOOK5,
	TEXT_BOOK6,
	TEXT_BOOK7,
	TEXT_BOOK8,
	TEXT_BOOK9,
	TEXT_BOOKA,
	TEXT_BOOKB,
	TEXT_BOOKC,
	TEXT_OBOOKA,
	TEXT_OBOOKB,
	TEXT_OBOOKC,
	TEXT_MBOOKA,
	TEXT_MBOOKB,
	TEXT_MBOOKC,
	TEXT_RBOOKA,
	TEXT_RBOOKB,
	TEXT_RBOOKC,
	TEXT_BBOOKA,
	TEXT_BBOOKB,
	TEXT_BBOOKC,
	/*
	TEXT_DEADGUY,
	TEXT_FARNHAM14,
	TEXT_FARNHAM15,
	TEXT_FARNHAM16,
	TEXT_FARNHAM17,
	TEXT_FARNHAM18,
	TEXT_FARNHAM19,
	TEXT_FARNHAM20,
	TEXT_FARNHAM21,
	TEXT_FARNHAM22,
	TEXT_GILLIAN11,
	TEXT_GILLIAN12,
	TEXT_GILLIAN13,
	TEXT_GILLIAN14,
	TEXT_GILLIAN15,
	TEXT_GILLIAN16,
	TEXT_GILLIAN17,
	TEXT_GILLIAN18,
	TEXT_GILLIAN19,
	TEXT_GILLIAN20,
	TEXT_GILLIAN21,
	TEXT_GILLIAN22,
	TEXT_GILLIAN23,
	TEXT_GILLIAN24,
	TEXT_GILLIAN25,
	TEXT_GILLIAN26,
	TEXT_PEPIN12,
	TEXT_PEPIN13,
	TEXT_PEPIN14,
	TEXT_PEPIN15,
	TEXT_PEPIN16,
	TEXT_PEPIN17,
	TEXT_PEPIN18,
	TEXT_PEPIN19,
	TEXT_PEPIN20,
	TEXT_PEPIN21,
	TEXT_PEPIN22,
	TEXT_PEPIN23,
	TEXT_PEPIN24,
	TEXT_PEPIN25,
	TEXT_PEPIN26,
	TEXT_PEPIN27,
	TEXT_PEPIN28,
	TEXT_PEPIN29,
	TEXT_PEPIN30,
	*/
	TEXT_NONE = -1,
};

[[maybe_unused]] constexpr std::string_view toString(_speech_id value)
{
	switch(value) {
	case TEXT_KING1:
		return "King1";
	case TEXT_KING2:
		return "King2";
	case TEXT_KING3:
		return "King3";
	case TEXT_KING4:
		return "King4";
	case TEXT_KING5:
		return "King5";
	case TEXT_KING6:
		return "King6";
	case TEXT_KING7:
		return "King7";
	case TEXT_KING8:
		return "King8";
	case TEXT_KING9:
		return "King9";
	case TEXT_KING10:
		return "King10";
	case TEXT_KING11:
		return "King11";
	case TEXT_BANNER1:
		return "Banner1";
	case TEXT_BANNER2:
		return "Banner2";
	case TEXT_BANNER3:
		return "Banner3";
	case TEXT_BANNER4:
		return "Banner4";
	case TEXT_BANNER5:
		return "Banner5";
	case TEXT_BANNER6:
		return "Banner6";
	case TEXT_BANNER7:
		return "Banner7";
	case TEXT_BANNER8:
		return "Banner8";
	case TEXT_BANNER9:
		return "Banner9";
	case TEXT_BANNER10:
		return "Banner10";
	case TEXT_BANNER11:
		return "Banner11";
	case TEXT_BANNER12:
		return "Banner12";
	case TEXT_VILE1:
		return "Vile1";
	case TEXT_VILE2:
		return "Vile2";
	case TEXT_VILE3:
		return "Vile3";
	case TEXT_VILE4:
		return "Vile4";
	case TEXT_VILE5:
		return "Vile5";
	case TEXT_VILE6:
		return "Vile6";
	case TEXT_VILE7:
		return "Vile7";
	case TEXT_VILE8:
		return "Vile8";
	case TEXT_VILE9:
		return "Vile9";
	case TEXT_VILE10:
		return "Vile10";
	case TEXT_VILE11:
		return "Vile11";
	case TEXT_VILE12:
		return "Vile12";
	case TEXT_VILE13:
		return "Vile13";
	case TEXT_VILE14:
		return "Vile14";
	case TEXT_POISON1:
		return "Poison1";
	case TEXT_POISON2:
		return "Poison2";
	case TEXT_POISON3:
		return "Poison3";
	case TEXT_POISON4:
		return "Poison4";
	case TEXT_POISON5:
		return "Poison5";
	case TEXT_POISON6:
		return "Poison6";
	case TEXT_POISON7:
		return "Poison7";
	case TEXT_POISON8:
		return "Poison8";
	case TEXT_POISON9:
		return "Poison9";
	case TEXT_POISON10:
		return "Poison10";
	case TEXT_BONE1:
		return "Bone1";
	case TEXT_BONE2:
		return "Bone2";
	case TEXT_BONE3:
		return "Bone3";
	case TEXT_BONE4:
		return "Bone4";
	case TEXT_BONE5:
		return "Bone5";
	case TEXT_BONE6:
		return "Bone6";
	case TEXT_BONE7:
		return "Bone7";
	case TEXT_BONE8:
		return "Bone8";
	case TEXT_BUTCH1:
		return "Butch1";
	case TEXT_BUTCH2:
		return "Butch2";
	case TEXT_BUTCH3:
		return "Butch3";
	case TEXT_BUTCH4:
		return "Butch4";
	case TEXT_BUTCH5:
		return "Butch5";
	case TEXT_BUTCH6:
		return "Butch6";
	case TEXT_BUTCH7:
		return "Butch7";
	case TEXT_BUTCH8:
		return "Butch8";
	case TEXT_BUTCH9:
		return "Butch9";
	case TEXT_BUTCH10:
		return "Butch10";
	case TEXT_BLIND1:
		return "Blind1";
	case TEXT_BLIND2:
		return "Blind2";
	case TEXT_BLIND3:
		return "Blind3";
	case TEXT_BLIND4:
		return "Blind4";
	case TEXT_BLIND5:
		return "Blind5";
	case TEXT_BLIND6:
		return "Blind6";
	case TEXT_BLIND7:
		return "Blind7";
	case TEXT_BLIND8:
		return "Blind8";
	case TEXT_VEIL1:
		return "Veil1";
	case TEXT_VEIL2:
		return "Veil2";
	case TEXT_VEIL3:
		return "Veil3";
	case TEXT_VEIL4:
		return "Veil4";
	case TEXT_VEIL5:
		return "Veil5";
	case TEXT_VEIL6:
		return "Veil6";
	case TEXT_VEIL7:
		return "Veil7";
	case TEXT_VEIL8:
		return "Veil8";
	case TEXT_VEIL9:
		return "Veil9";
	case TEXT_VEIL10:
		return "Veil10";
	case TEXT_VEIL11:
		return "Veil11";
	case TEXT_ANVIL1:
		return "Anvil1";
	case TEXT_ANVIL2:
		return "Anvil2";
	case TEXT_ANVIL3:
		return "Anvil3";
	case TEXT_ANVIL4:
		return "Anvil4";
	case TEXT_ANVIL5:
		return "Anvil5";
	case TEXT_ANVIL6:
		return "Anvil6";
	case TEXT_ANVIL7:
		return "Anvil7";
	case TEXT_ANVIL8:
		return "Anvil8";
	case TEXT_ANVIL9:
		return "Anvil9";
	case TEXT_ANVIL10:
		return "Anvil10";
	case TEXT_BLOOD1:
		return "Blood1";
	case TEXT_BLOOD2:
		return "Blood2";
	case TEXT_BLOOD3:
		return "Blood3";
	case TEXT_BLOOD4:
		return "Blood4";
	case TEXT_BLOOD5:
		return "Blood5";
	case TEXT_BLOOD6:
		return "Blood6";
	case TEXT_BLOOD7:
		return "Blood7";
	case TEXT_BLOOD8:
		return "Blood8";
	case TEXT_WARLRD1:
		return "Warlrd1";
	case TEXT_WARLRD2:
		return "Warlrd2";
	case TEXT_WARLRD3:
		return "Warlrd3";
	case TEXT_WARLRD4:
		return "Warlrd4";
	case TEXT_WARLRD5:
		return "Warlrd5";
	case TEXT_WARLRD6:
		return "Warlrd6";
	case TEXT_WARLRD7:
		return "Warlrd7";
	case TEXT_WARLRD8:
		return "Warlrd8";
	case TEXT_WARLRD9:
		return "Warlrd9";
	case TEXT_INFRA1:
		return "Infra1";
	case TEXT_INFRA2:
		return "Infra2";
	case TEXT_INFRA3:
		return "Infra3";
	case TEXT_INFRA4:
		return "Infra4";
	case TEXT_INFRA5:
		return "Infra5";
	case TEXT_INFRA6:
		return "Infra6";
	case TEXT_INFRA7:
		return "Infra7";
	case TEXT_INFRA8:
		return "Infra8";
	case TEXT_INFRA9:
		return "Infra9";
	case TEXT_INFRA10:
		return "Infra10";
	case TEXT_MUSH1:
		return "Mush1";
	case TEXT_MUSH2:
		return "Mush2";
	case TEXT_MUSH3:
		return "Mush3";
	case TEXT_MUSH4:
		return "Mush4";
	case TEXT_MUSH5:
		return "Mush5";
	case TEXT_MUSH6:
		return "Mush6";
	case TEXT_MUSH7:
		return "Mush7";
	case TEXT_MUSH8:
		return "Mush8";
	case TEXT_MUSH9:
		return "Mush9";
	case TEXT_MUSH10:
		return "Mush10";
	case TEXT_MUSH11:
		return "Mush11";
	case TEXT_MUSH12:
		return "Mush12";
	case TEXT_MUSH13:
		return "Mush13";
	case TEXT_DOOM1:
		return "Doom1";
	case TEXT_DOOM2:
		return "Doom2";
	case TEXT_DOOM3:
		return "Doom3";
	case TEXT_DOOM4:
		return "Doom4";
	case TEXT_DOOM5:
		return "Doom5";
	case TEXT_DOOM6:
		return "Doom6";
	case TEXT_DOOM7:
		return "Doom7";
	case TEXT_DOOM8:
		return "Doom8";
	case TEXT_DOOM9:
		return "Doom9";
	case TEXT_DOOM10:
		return "Doom10";
	case TEXT_GARBUD1:
		return "Garbud1";
	case TEXT_GARBUD2:
		return "Garbud2";
	case TEXT_GARBUD3:
		return "Garbud3";
	case TEXT_GARBUD4:
		return "Garbud4";
	case TEXT_ZHAR1:
		return "Zhar1";
	case TEXT_ZHAR2:
		return "Zhar2";
	case TEXT_STORY1:
		return "Story1";
	case TEXT_STORY2:
		return "Story2";
	case TEXT_STORY3:
		return "Story3";
	case TEXT_STORY4:
		return "Story4";
	case TEXT_STORY5:
		return "Story5";
	case TEXT_STORY6:
		return "Story6";
	case TEXT_STORY7:
		return "Story7";
	case TEXT_STORY9:
		return "Story9";
	case TEXT_STORY10:
		return "Story10";
	case TEXT_STORY11:
		return "Story11";
	case TEXT_OGDEN1:
		return "Ogden1";
	case TEXT_OGDEN2:
		return "Ogden2";
	case TEXT_OGDEN3:
		return "Ogden3";
	case TEXT_OGDEN4:
		return "Ogden4";
	case TEXT_OGDEN5:
		return "Ogden5";
	case TEXT_OGDEN6:
		return "Ogden6";
	case TEXT_OGDEN8:
		return "Ogden8";
	case TEXT_OGDEN9:
		return "Ogden9";
	case TEXT_OGDEN10:
		return "Ogden10";
	case TEXT_PEPIN1:
		return "Pepin1";
	case TEXT_PEPIN2:
		return "Pepin2";
	case TEXT_PEPIN3:
		return "Pepin3";
	case TEXT_PEPIN4:
		return "Pepin4";
	case TEXT_PEPIN5:
		return "Pepin5";
	case TEXT_PEPIN6:
		return "Pepin6";
	case TEXT_PEPIN7:
		return "Pepin7";
	case TEXT_PEPIN9:
		return "Pepin9";
	case TEXT_PEPIN10:
		return "Pepin10";
	case TEXT_PEPIN11:
		return "Pepin11";
	case TEXT_GILLIAN1:
		return "Gillian1";
	case TEXT_GILLIAN2:
		return "Gillian2";
	case TEXT_GILLIAN3:
		return "Gillian3";
	case TEXT_GILLIAN4:
		return "Gillian4";
	case TEXT_GILLIAN5:
		return "Gillian5";
	case TEXT_GILLIAN6:
		return "Gillian6";
	case TEXT_GILLIAN7:
		return "Gillian7";
	case TEXT_GILLIAN9:
		return "Gillian9";
	case TEXT_GILLIAN10:
		return "Gillian10";
	case TEXT_GRISWOLD1:
		return "Griswold1";
	case TEXT_GRISWOLD2:
		return "Griswold2";
	case TEXT_GRISWOLD3:
		return "Griswold3";
	case TEXT_GRISWOLD4:
		return "Griswold4";
	case TEXT_GRISWOLD5:
		return "Griswold5";
	case TEXT_GRISWOLD6:
		return "Griswold6";
	case TEXT_GRISWOLD7:
		return "Griswold7";
	case TEXT_GRISWOLD8:
		return "Griswold8";
	case TEXT_GRISWOLD9:
		return "Griswold9";
	case TEXT_GRISWOLD10:
		return "Griswold10";
	case TEXT_GRISWOLD12:
		return "Griswold12";
	case TEXT_GRISWOLD13:
		return "Griswold13";
	case TEXT_FARNHAM1:
		return "Farnham1";
	case TEXT_FARNHAM2:
		return "Farnham2";
	case TEXT_FARNHAM3:
		return "Farnham3";
	case TEXT_FARNHAM4:
		return "Farnham4";
	case TEXT_FARNHAM5:
		return "Farnham5";
	case TEXT_FARNHAM6:
		return "Farnham6";
	case TEXT_FARNHAM8:
		return "Farnham8";
	case TEXT_FARNHAM9:
		return "Farnham9";
	case TEXT_FARNHAM10:
		return "Farnham10";
	case TEXT_FARNHAM11:
		return "Farnham11";
	case TEXT_FARNHAM12:
		return "Farnham12";
	case TEXT_FARNHAM13:
		return "Farnham13";
	case TEXT_ADRIA1:
		return "Adria1";
	case TEXT_ADRIA2:
		return "Adria2";
	case TEXT_ADRIA3:
		return "Adria3";
	case TEXT_ADRIA4:
		return "Adria4";
	case TEXT_ADRIA5:
		return "Adria5";
	case TEXT_ADRIA6:
		return "Adria6";
	case TEXT_ADRIA7:
		return "Adria7";
	case TEXT_ADRIA8:
		return "Adria8";
	case TEXT_ADRIA9:
		return "Adria9";
	case TEXT_ADRIA10:
		return "Adria10";
	case TEXT_ADRIA12:
		return "Adria12";
	case TEXT_ADRIA13:
		return "Adria13";
	case TEXT_WIRT1:
		return "Wirt1";
	case TEXT_WIRT2:
		return "Wirt2";
	case TEXT_WIRT3:
		return "Wirt3";
	case TEXT_WIRT4:
		return "Wirt4";
	case TEXT_WIRT5:
		return "Wirt5";
	case TEXT_WIRT6:
		return "Wirt6";
	case TEXT_WIRT7:
		return "Wirt7";
	case TEXT_WIRT8:
		return "Wirt8";
	case TEXT_WIRT9:
		return "Wirt9";
	case TEXT_WIRT11:
		return "Wirt11";
	case TEXT_WIRT12:
		return "Wirt12";
	case TEXT_BONER:
		return "Boner";
	case TEXT_BLOODY:
		return "Bloody";
	case TEXT_BLINDING:
		return "Blinding";
	case TEXT_BLOODWAR:
		return "Bloodwar";
	case TEXT_MBONER:
		return "Mboner";
	case TEXT_MBLOODY:
		return "Mbloody";
	case TEXT_MBLINDING:
		return "Mblinding";
	case TEXT_MBLOODWAR:
		return "Mbloodwar";
	case TEXT_RBONER:
		return "Rboner";
	case TEXT_RBLOODY:
		return "Rbloody";
	case TEXT_RBLINDING:
		return "Rblinding";
	case TEXT_RBLOODWAR:
		return "Rbloodwar";
	case TEXT_COW1:
		return "Cow1";
	case TEXT_COW2:
		return "Cow2";
	case TEXT_BOOK11:
		return "Book11";
	case TEXT_BOOK12:
		return "Book12";
	case TEXT_BOOK13:
		return "Book13";
	case TEXT_BOOK21:
		return "Book21";
	case TEXT_BOOK22:
		return "Book22";
	case TEXT_BOOK23:
		return "Book23";
	case TEXT_BOOK31:
		return "Book31";
	case TEXT_BOOK32:
		return "Book32";
	case TEXT_BOOK33:
		return "Book33";
	case TEXT_INTRO:
		return "Intro";
	case TEXT_HBONER:
		return "Hboner";
	case TEXT_HBLOODY:
		return "Hbloody";
	case TEXT_HBLINDING:
		return "Hblinding";
	case TEXT_HBLOODWAR:
		return "Hbloodwar";
	case TEXT_BBONER:
		return "Bboner";
	case TEXT_BBLOODY:
		return "Bbloody";
	case TEXT_BBLINDING:
		return "Bblinding";
	case TEXT_BBLOODWAR:
		return "Bbloodwar";
	case TEXT_GRAVE1:
		return "Grave1";
	case TEXT_GRAVE2:
		return "Grave2";
	case TEXT_GRAVE3:
		return "Grave3";
	case TEXT_GRAVE4:
		return "Grave4";
	case TEXT_GRAVE5:
		return "Grave5";
	case TEXT_GRAVE6:
		return "Grave6";
	case TEXT_GRAVE7:
		return "Grave7";
	case TEXT_GRAVE8:
		return "Grave8";
	case TEXT_GRAVE9:
		return "Grave9";
	case TEXT_GRAVE10:
		return "Grave10";
	case TEXT_FARMER1:
		return "Farmer1";
	case TEXT_FARMER2:
		return "Farmer2";
	case TEXT_FARMER3:
		return "Farmer3";
	case TEXT_FARMER4:
		return "Farmer4";
	case TEXT_FARMER5:
		return "Farmer5";
	case TEXT_GIRL1:
		return "Girl1";
	case TEXT_GIRL2:
		return "Girl2";
	case TEXT_GIRL3:
		return "Girl3";
	case TEXT_GIRL4:
		return "Girl4";
	case TEXT_DEFILER1:
		return "Defiler1";
	case TEXT_DEFILER2:
		return "Defiler2";
	case TEXT_DEFILER3:
		return "Defiler3";
	case TEXT_DEFILER4:
		return "Defiler4";
	case TEXT_DEFILER5:
		return "Defiler5";
	case TEXT_NAKRUL1:
		return "Nakrul1";
	case TEXT_NAKRUL2:
		return "Nakrul2";
	case TEXT_NAKRUL3:
		return "Nakrul3";
	case TEXT_NAKRUL4:
		return "Nakrul4";
	case TEXT_NAKRUL5:
		return "Nakrul5";
	case TEXT_CORNSTN:
		return "Cornstn";
	case TEXT_JERSEY1:
		return "Jersey1";
	case TEXT_JERSEY2:
		return "Jersey2";
	case TEXT_JERSEY3:
		return "Jersey3";
	case TEXT_JERSEY4:
		return "Jersey4";
	case TEXT_JERSEY5:
		return "Jersey5";
	case TEXT_JERSEY6:
		return "Jersey6";
	case TEXT_JERSEY7:
		return "Jersey7";
	case TEXT_JERSEY8:
		return "Jersey8";
	case TEXT_JERSEY9:
		return "Jersey9";
	case TEXT_TRADER:
		return "Trader";
	case TEXT_FARMER6:
		return "Farmer6";
	case TEXT_FARMER7:
		return "Farmer7";
	case TEXT_FARMER8:
		return "Farmer8";
	case TEXT_FARMER9:
		return "Farmer9";
	case TEXT_FARMER10:
		return "Farmer10";
	case TEXT_JERSEY10:
		return "Jersey10";
	case TEXT_JERSEY11:
		return "Jersey11";
	case TEXT_JERSEY12:
		return "Jersey12";
	case TEXT_JERSEY13:
		return "Jersey13";
	case TEXT_SKLJRN:
		return "Skljrn";
	case TEXT_BOOK4:
		return "Book4";
	case TEXT_BOOK5:
		return "Book5";
	case TEXT_BOOK6:
		return "Book6";
	case TEXT_BOOK7:
		return "Book7";
	case TEXT_BOOK8:
		return "Book8";
	case TEXT_BOOK9:
		return "Book9";
	case TEXT_BOOKA:
		return "Booka";
	case TEXT_BOOKB:
		return "Bookb";
	case TEXT_BOOKC:
		return "Bookc";
	case TEXT_OBOOKA:
		return "Obooka";
	case TEXT_OBOOKB:
		return "Obookb";
	case TEXT_OBOOKC:
		return "Obookc";
	case TEXT_MBOOKA:
		return "Mbooka";
	case TEXT_MBOOKB:
		return "Mbookb";
	case TEXT_MBOOKC:
		return "Mbookc";
	case TEXT_RBOOKA:
		return "Rbooka";
	case TEXT_RBOOKB:
		return "Rbookb";
	case TEXT_RBOOKC:
		return "Rbookc";
	case TEXT_BBOOKA:
		return "Bbooka";
	case TEXT_BBOOKB:
		return "Bbookb";
	case TEXT_BBOOKC:
		return "Bbookc";
	case TEXT_NONE:
		return "None";
	}
}

struct TextDataStruct {
	const char *txtstr;
	bool scrlltxt;
	_sfx_id sfxnr;
};

extern const TextDataStruct alltext[];

} // namespace devilution
