/**
 * @file objdat.h
 *
 * Interface of all object data.
 */
#pragma once

#include <stdint.h>
#include <string_view>

#include "gendung.h"

namespace devilution {

enum theme_id : int8_t {
	THEME_BARREL,
	THEME_SHRINE,
	THEME_MONSTPIT,
	THEME_SKELROOM,
	THEME_TREASURE,
	THEME_LIBRARY,
	THEME_TORTURE,
	THEME_BLOODFOUNTAIN,
	THEME_DECAPITATED,
	THEME_PURIFYINGFOUNTAIN,
	THEME_ARMORSTAND,
	THEME_GOATSHRINE,
	THEME_CAULDRON,
	THEME_MURKYFOUNTAIN,
	THEME_TEARFOUNTAIN,
	THEME_BRNCROSS,
	THEME_WEAPONRACK,
	THEME_NONE = -1,
};

constexpr std::string_view toString(theme_id value)
{
	switch(value) {
	case THEME_BARREL:
		return "Barrel";
	case THEME_SHRINE:
		return "Shrine";
	case THEME_MONSTPIT:
		return "Monstpit";
	case THEME_SKELROOM:
		return "Skelroom";
	case THEME_TREASURE:
		return "Treasure";
	case THEME_LIBRARY:
		return "Library";
	case THEME_TORTURE:
		return "Torture";
	case THEME_BLOODFOUNTAIN:
		return "Bloodfountain";
	case THEME_DECAPITATED:
		return "Decapitated";
	case THEME_PURIFYINGFOUNTAIN:
		return "Purifyingfountain";
	case THEME_ARMORSTAND:
		return "Armorstand";
	case THEME_GOATSHRINE:
		return "Goatshrine";
	case THEME_CAULDRON:
		return "Cauldron";
	case THEME_MURKYFOUNTAIN:
		return "Murkyfountain";
	case THEME_TEARFOUNTAIN:
		return "Tearfountain";
	case THEME_BRNCROSS:
		return "Brncross";
	case THEME_WEAPONRACK:
		return "Weaponrack";
	case THEME_NONE:
		return "None";
	}
}

enum object_graphic_id : int8_t {
	OFILE_L1BRAZ,
	OFILE_L1DOORS,
	OFILE_LEVER,
	OFILE_CHEST1,
	OFILE_CHEST2,
	OFILE_BANNER,
	OFILE_SKULPILE,
	OFILE_SKULFIRE,
	OFILE_SKULSTIK,
	OFILE_CRUXSK1,
	OFILE_CRUXSK2,
	OFILE_CRUXSK3,
	OFILE_BOOK1,
	OFILE_BOOK2,
	OFILE_ROCKSTAN,
	OFILE_ANGEL,
	OFILE_CHEST3,
	OFILE_BURNCROS,
	OFILE_CANDLE2,
	OFILE_NUDE2,
	OFILE_SWITCH4,
	OFILE_TNUDEM,
	OFILE_TNUDEW,
	OFILE_TSOUL,
	OFILE_L2DOORS,
	OFILE_WTORCH4,
	OFILE_WTORCH3,
	OFILE_SARC,
	OFILE_FLAME1,
	OFILE_PRSRPLT1,
	OFILE_TRAPHOLE,
	OFILE_MINIWATR,
	OFILE_WTORCH2,
	OFILE_WTORCH1,
	OFILE_BCASE,
	OFILE_BSHELF,
	OFILE_WEAPSTND,
	OFILE_BARREL,
	OFILE_BARRELEX,
	OFILE_LSHRINEG,
	OFILE_RSHRINEG,
	OFILE_BLOODFNT,
	OFILE_DECAP,
	OFILE_PEDISTL,
	OFILE_L3DOORS,
	OFILE_PFOUNTN,
	OFILE_ARMSTAND,
	OFILE_GOATSHRN,
	OFILE_CAULDREN,
	OFILE_MFOUNTN,
	OFILE_TFOUNTN,
	OFILE_ALTBOY,
	OFILE_MCIRL,
	OFILE_BKSLBRNT,
	OFILE_MUSHPTCH,
	OFILE_LZSTAND,
	OFILE_NULL = -1,
};

constexpr std::string_view toString(object_graphic_id value)
{
	switch(value) {
	case OFILE_L1BRAZ:
		return "L1braz";
	case OFILE_L1DOORS:
		return "L1doors";
	case OFILE_LEVER:
		return "Lever";
	case OFILE_CHEST1:
		return "Chest1";
	case OFILE_CHEST2:
		return "Chest2";
	case OFILE_BANNER:
		return "Banner";
	case OFILE_SKULPILE:
		return "Skulpile";
	case OFILE_SKULFIRE:
		return "Skulfire";
	case OFILE_SKULSTIK:
		return "Skulstik";
	case OFILE_CRUXSK1:
		return "Cruxsk1";
	case OFILE_CRUXSK2:
		return "Cruxsk2";
	case OFILE_CRUXSK3:
		return "Cruxsk3";
	case OFILE_BOOK1:
		return "Book1";
	case OFILE_BOOK2:
		return "Book2";
	case OFILE_ROCKSTAN:
		return "Rockstan";
	case OFILE_ANGEL:
		return "Angel";
	case OFILE_CHEST3:
		return "Chest3";
	case OFILE_BURNCROS:
		return "Burncros";
	case OFILE_CANDLE2:
		return "Candle2";
	case OFILE_NUDE2:
		return "Nude2";
	case OFILE_SWITCH4:
		return "Switch4";
	case OFILE_TNUDEM:
		return "Tnudem";
	case OFILE_TNUDEW:
		return "Tnudew";
	case OFILE_TSOUL:
		return "Tsoul";
	case OFILE_L2DOORS:
		return "L2doors";
	case OFILE_WTORCH4:
		return "Wtorch4";
	case OFILE_WTORCH3:
		return "Wtorch3";
	case OFILE_SARC:
		return "Sarc";
	case OFILE_FLAME1:
		return "Flame1";
	case OFILE_PRSRPLT1:
		return "Prsrplt1";
	case OFILE_TRAPHOLE:
		return "Traphole";
	case OFILE_MINIWATR:
		return "Miniwatr";
	case OFILE_WTORCH2:
		return "Wtorch2";
	case OFILE_WTORCH1:
		return "Wtorch1";
	case OFILE_BCASE:
		return "Bcase";
	case OFILE_BSHELF:
		return "Bshelf";
	case OFILE_WEAPSTND:
		return "Weapstnd";
	case OFILE_BARREL:
		return "Barrel";
	case OFILE_BARRELEX:
		return "Barrelex";
	case OFILE_LSHRINEG:
		return "Lshrineg";
	case OFILE_RSHRINEG:
		return "Rshrineg";
	case OFILE_BLOODFNT:
		return "Bloodfnt";
	case OFILE_DECAP:
		return "Decap";
	case OFILE_PEDISTL:
		return "Pedistl";
	case OFILE_L3DOORS:
		return "L3doors";
	case OFILE_PFOUNTN:
		return "Pfountn";
	case OFILE_ARMSTAND:
		return "Armstand";
	case OFILE_GOATSHRN:
		return "Goatshrn";
	case OFILE_CAULDREN:
		return "Cauldren";
	case OFILE_MFOUNTN:
		return "Mfountn";
	case OFILE_TFOUNTN:
		return "Tfountn";
	case OFILE_ALTBOY:
		return "Altboy";
	case OFILE_MCIRL:
		return "Mcirl";
	case OFILE_BKSLBRNT:
		return "Bkslbrnt";
	case OFILE_MUSHPTCH:
		return "Mushptch";
	case OFILE_LZSTAND:
		return "Lzstand";
	case OFILE_NULL:
		return "Null";
	}
}

enum _object_id : int8_t {
	OBJ_L1LIGHT,
	OBJ_L1LDOOR,
	OBJ_L1RDOOR,
	OBJ_SKFIRE,
	OBJ_LEVER,
	OBJ_CHEST1,
	OBJ_CHEST2,
	OBJ_CHEST3,
	OBJ_CANDLE1,
	OBJ_CANDLE2,
	OBJ_CANDLEO,
	OBJ_BANNERL,
	OBJ_BANNERM,
	OBJ_BANNERR,
	OBJ_SKPILE,
	OBJ_SKSTICK1,
	OBJ_SKSTICK2,
	OBJ_SKSTICK3,
	OBJ_SKSTICK4,
	OBJ_SKSTICK5,
	OBJ_CRUX1,
	OBJ_CRUX2,
	OBJ_CRUX3,
	OBJ_STAND,
	OBJ_ANGEL,
	OBJ_BOOK2L,
	OBJ_BCROSS,
	OBJ_NUDEW2R,
	OBJ_SWITCHSKL,
	OBJ_TNUDEM1,
	OBJ_TNUDEM2,
	OBJ_TNUDEM3,
	OBJ_TNUDEM4,
	OBJ_TNUDEW1,
	OBJ_TNUDEW2,
	OBJ_TNUDEW3,
	OBJ_TORTURE1,
	OBJ_TORTURE2,
	OBJ_TORTURE3,
	OBJ_TORTURE4,
	OBJ_TORTURE5,
	OBJ_BOOK2R,
	OBJ_L2LDOOR,
	OBJ_L2RDOOR,
	OBJ_TORCHL,
	OBJ_TORCHR,
	OBJ_TORCHL2,
	OBJ_TORCHR2,
	OBJ_SARC,
	OBJ_FLAMEHOLE,
	OBJ_FLAMELVR,
	OBJ_WATER,
	OBJ_BOOKLVR,
	OBJ_TRAPL,
	OBJ_TRAPR,
	OBJ_BOOKSHELF,
	OBJ_WEAPRACK,
	OBJ_BARREL,
	OBJ_BARRELEX,
	OBJ_SHRINEL,
	OBJ_SHRINER,
	OBJ_SKELBOOK,
	OBJ_BOOKCASEL,
	OBJ_BOOKCASER,
	OBJ_BOOKSTAND,
	OBJ_BOOKCANDLE,
	OBJ_BLOODFTN,
	OBJ_DECAP,
	OBJ_TCHEST1,
	OBJ_TCHEST2,
	OBJ_TCHEST3,
	OBJ_BLINDBOOK,
	OBJ_BLOODBOOK,
	OBJ_PEDISTAL,
	OBJ_L3LDOOR,
	OBJ_L3RDOOR,
	OBJ_PURIFYINGFTN,
	OBJ_ARMORSTAND,
	OBJ_ARMORSTANDN,
	OBJ_GOATSHRINE,
	OBJ_CAULDRON,
	OBJ_MURKYFTN,
	OBJ_TEARFTN,
	OBJ_ALTBOY,
	OBJ_MCIRCLE1,
	OBJ_MCIRCLE2,
	OBJ_STORYBOOK,
	OBJ_STORYCANDLE,
	OBJ_STEELTOME,
	OBJ_WARARMOR,
	OBJ_WARWEAP,
	OBJ_TBCROSS,
	OBJ_WEAPONRACK,
	OBJ_WEAPONRACKN,
	OBJ_MUSHPATCH,
	OBJ_LAZSTAND,
	OBJ_SLAINHERO,
	OBJ_SIGNCHEST,
	OBJ_BOOKSHELFR,
	OBJ_NULL = -1,
};

constexpr std::string_view toString(_object_id value)
{
	switch(value) {
	case OBJ_L1LIGHT:
		return "L1light";
	case OBJ_L1LDOOR:
		return "L1ldoor";
	case OBJ_L1RDOOR:
		return "L1rdoor";
	case OBJ_SKFIRE:
		return "Skfire";
	case OBJ_LEVER:
		return "Lever";
	case OBJ_CHEST1:
		return "Chest1";
	case OBJ_CHEST2:
		return "Chest2";
	case OBJ_CHEST3:
		return "Chest3";
	case OBJ_CANDLE1:
		return "Candle1";
	case OBJ_CANDLE2:
		return "Candle2";
	case OBJ_CANDLEO:
		return "Candleo";
	case OBJ_BANNERL:
		return "Bannerl";
	case OBJ_BANNERM:
		return "Bannerm";
	case OBJ_BANNERR:
		return "Bannerr";
	case OBJ_SKPILE:
		return "Skpile";
	case OBJ_SKSTICK1:
		return "Skstick1";
	case OBJ_SKSTICK2:
		return "Skstick2";
	case OBJ_SKSTICK3:
		return "Skstick3";
	case OBJ_SKSTICK4:
		return "Skstick4";
	case OBJ_SKSTICK5:
		return "Skstick5";
	case OBJ_CRUX1:
		return "Crux1";
	case OBJ_CRUX2:
		return "Crux2";
	case OBJ_CRUX3:
		return "Crux3";
	case OBJ_STAND:
		return "Stand";
	case OBJ_ANGEL:
		return "Angel";
	case OBJ_BOOK2L:
		return "Book2l";
	case OBJ_BCROSS:
		return "Bcross";
	case OBJ_NUDEW2R:
		return "Nudew2r";
	case OBJ_SWITCHSKL:
		return "Switchskl";
	case OBJ_TNUDEM1:
		return "Tnudem1";
	case OBJ_TNUDEM2:
		return "Tnudem2";
	case OBJ_TNUDEM3:
		return "Tnudem3";
	case OBJ_TNUDEM4:
		return "Tnudem4";
	case OBJ_TNUDEW1:
		return "Tnudew1";
	case OBJ_TNUDEW2:
		return "Tnudew2";
	case OBJ_TNUDEW3:
		return "Tnudew3";
	case OBJ_TORTURE1:
		return "Torture1";
	case OBJ_TORTURE2:
		return "Torture2";
	case OBJ_TORTURE3:
		return "Torture3";
	case OBJ_TORTURE4:
		return "Torture4";
	case OBJ_TORTURE5:
		return "Torture5";
	case OBJ_BOOK2R:
		return "Book2r";
	case OBJ_L2LDOOR:
		return "L2ldoor";
	case OBJ_L2RDOOR:
		return "L2rdoor";
	case OBJ_TORCHL:
		return "Torchl";
	case OBJ_TORCHR:
		return "Torchr";
	case OBJ_TORCHL2:
		return "Torchl2";
	case OBJ_TORCHR2:
		return "Torchr2";
	case OBJ_SARC:
		return "Sarc";
	case OBJ_FLAMEHOLE:
		return "Flamehole";
	case OBJ_FLAMELVR:
		return "Flamelvr";
	case OBJ_WATER:
		return "Water";
	case OBJ_BOOKLVR:
		return "Booklvr";
	case OBJ_TRAPL:
		return "Trapl";
	case OBJ_TRAPR:
		return "Trapr";
	case OBJ_BOOKSHELF:
		return "Bookshelf";
	case OBJ_WEAPRACK:
		return "Weaprack";
	case OBJ_BARREL:
		return "Barrel";
	case OBJ_BARRELEX:
		return "Barrelex";
	case OBJ_SHRINEL:
		return "Shrinel";
	case OBJ_SHRINER:
		return "Shriner";
	case OBJ_SKELBOOK:
		return "Skelbook";
	case OBJ_BOOKCASEL:
		return "Bookcasel";
	case OBJ_BOOKCASER:
		return "Bookcaser";
	case OBJ_BOOKSTAND:
		return "Bookstand";
	case OBJ_BOOKCANDLE:
		return "Bookcandle";
	case OBJ_BLOODFTN:
		return "Bloodftn";
	case OBJ_DECAP:
		return "Decap";
	case OBJ_TCHEST1:
		return "Tchest1";
	case OBJ_TCHEST2:
		return "Tchest2";
	case OBJ_TCHEST3:
		return "Tchest3";
	case OBJ_BLINDBOOK:
		return "Blindbook";
	case OBJ_BLOODBOOK:
		return "Bloodbook";
	case OBJ_PEDISTAL:
		return "Pedistal";
	case OBJ_L3LDOOR:
		return "L3ldoor";
	case OBJ_L3RDOOR:
		return "L3rdoor";
	case OBJ_PURIFYINGFTN:
		return "Purifyingftn";
	case OBJ_ARMORSTAND:
		return "Armorstand";
	case OBJ_ARMORSTANDN:
		return "Armorstandn";
	case OBJ_GOATSHRINE:
		return "Goatshrine";
	case OBJ_CAULDRON:
		return "Cauldron";
	case OBJ_MURKYFTN:
		return "Murkyftn";
	case OBJ_TEARFTN:
		return "Tearftn";
	case OBJ_ALTBOY:
		return "Altboy";
	case OBJ_MCIRCLE1:
		return "Mcircle1";
	case OBJ_MCIRCLE2:
		return "Mcircle2";
	case OBJ_STORYBOOK:
		return "Storybook";
	case OBJ_STORYCANDLE:
		return "Storycandle";
	case OBJ_STEELTOME:
		return "Steeltome";
	case OBJ_WARARMOR:
		return "Wararmor";
	case OBJ_WARWEAP:
		return "Warweap";
	case OBJ_TBCROSS:
		return "Tbcross";
	case OBJ_WEAPONRACK:
		return "Weaponrack";
	case OBJ_WEAPONRACKN:
		return "Weaponrackn";
	case OBJ_MUSHPATCH:
		return "Mushpatch";
	case OBJ_LAZSTAND:
		return "Lazstand";
	case OBJ_SLAINHERO:
		return "Slainhero";
	case OBJ_SIGNCHEST:
		return "Signchest";
	case OBJ_BOOKSHELFR:
		return "Bookshelfr";
	case OBJ_NULL:
		return "Null";
	}
}

enum quest_id : int8_t {
	Q_ROCK,
	Q_MUSHROOM,
	Q_GARBUD,
	Q_ZHAR,
	Q_VEIL,
	Q_DIABLO,
	Q_BUTCHER,
	Q_LTBANNER,
	Q_BLIND,
	Q_BLOOD,
	Q_ANVIL,
	Q_WARLORD,
	Q_SKELKING,
	Q_PWATER,
	Q_SCHAMB,
	Q_BETRAYER,
	Q_GRAVE,
	Q_FARMER,
	Q_GIRL,
	Q_TRADER,
	Q_DEFILER,
	Q_NAKRUL,
	Q_CORNSTN,
	Q_JERSEY,
	Q_INVALID = -1,
};

constexpr std::string_view toString(quest_id value)
{
	switch(value) {
	case Q_ROCK:
		return "Rock";
	case Q_MUSHROOM:
		return "Mushroom";
	case Q_GARBUD:
		return "Garbud";
	case Q_ZHAR:
		return "Zhar";
	case Q_VEIL:
		return "Veil";
	case Q_DIABLO:
		return "Diablo";
	case Q_BUTCHER:
		return "Butcher";
	case Q_LTBANNER:
		return "Ltbanner";
	case Q_BLIND:
		return "Blind";
	case Q_BLOOD:
		return "Blood";
	case Q_ANVIL:
		return "Anvil";
	case Q_WARLORD:
		return "Warlord";
	case Q_SKELKING:
		return "Skelking";
	case Q_PWATER:
		return "Pwater";
	case Q_SCHAMB:
		return "Schamb";
	case Q_BETRAYER:
		return "Betrayer";
	case Q_GRAVE:
		return "Grave";
	case Q_FARMER:
		return "Farmer";
	case Q_GIRL:
		return "Girl";
	case Q_TRADER:
		return "Trader";
	case Q_DEFILER:
		return "Defiler";
	case Q_NAKRUL:
		return "Nakrul";
	case Q_CORNSTN:
		return "Cornstn";
	case Q_JERSEY:
		return "Jersey";
	case Q_INVALID:
		return "Invalid";
	}
}

struct ObjDataStruct {
	Sint8 oload; // Todo create enum
	object_graphic_id ofindex;
	Sint8 ominlvl;
	Sint8 omaxlvl;
	dungeon_type olvltype;
	theme_id otheme;
	quest_id oquest;
	Sint32 oAnimFlag;  // TODO Create enum
	Sint32 oAnimDelay; // Tick length of each frame in the current animation
	Sint32 oAnimLen;   // Number of frames in current animation
	Sint32 oAnimWidth;
	bool oSolidFlag;
	bool oMissFlag;
	bool oLightFlag;
	Sint8 oBreak;   // TODO Create enum
	Sint8 oSelFlag; // TODO Create enum
	bool oTrapFlag;
};

extern const _object_id ObjTypeConv[];
extern const ObjDataStruct AllObjects[];
extern const char *const ObjMasterLoadList[];
extern const char *ObjCryptLoadList[];
extern const char *ObjHiveLoadList[];

} // namespace devilution
