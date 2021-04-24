/**
 * @file error.h
 *
 * Interface of in-game message functions.
 */
#pragma once

#include <stdint.h>
#include <string_view>

#include "engine.h"

namespace devilution {

enum diablo_message : uint8_t {
	EMSG_NONE,
	EMSG_NO_AUTOMAP_IN_TOWN,
	EMSG_NO_MULTIPLAYER_IN_DEMO,
	EMSG_DIRECT_SOUND_FAILED,
	EMSG_NOT_IN_SHAREWARE,
	EMSG_NO_SPACE_TO_SAVE,
	EMSG_NO_PAUSE_IN_TOWN,
	EMSG_COPY_TO_HDD,
	EMSG_DESYNC,
	EMSG_NO_PAUSE_IN_MP,
	EMSG_LOADING,
	EMSG_SAVING,
	EMSG_SHRINE_MYSTERIOUS,
	EMSG_SHRINE_HIDDEN,
	EMSG_SHRINE_GLOOMY,
	EMSG_SHRINE_WEIRD,
	EMSG_SHRINE_MAGICAL,
	EMSG_SHRINE_STONE,
	EMSG_SHRINE_RELIGIOUS,
	EMSG_SHRINE_ENCHANTED,
	EMSG_SHRINE_THAUMATURGIC,
	EMSG_SHRINE_FASCINATING,
	EMSG_SHRINE_CRYPTIC,
	EMSG_SHRINE_UNUSED,
	EMSG_SHRINE_ELDRITCH,
	EMSG_SHRINE_EERIE,
	EMSG_SHRINE_DIVINE,
	EMSG_SHRINE_HOLY,
	EMSG_SHRINE_SACRED,
	EMSG_SHRINE_SPIRITUAL,
	EMSG_SHRINE_SPOOKY1,
	EMSG_SHRINE_SPOOKY2,
	EMSG_SHRINE_ABANDONED,
	EMSG_SHRINE_CREEPY,
	EMSG_SHRINE_QUIET,
	EMSG_SHRINE_SECLUDED,
	EMSG_SHRINE_ORNATE,
	EMSG_SHRINE_GLIMMERING,
	EMSG_SHRINE_TAINTED1,
	EMSG_SHRINE_TAINTED2,
	EMSG_REQUIRES_LVL_8,
	EMSG_REQUIRES_LVL_13,
	EMSG_REQUIRES_LVL_17,
	EMSG_BONECHAMB,
	EMSG_SHRINE_OILY,
	EMSG_SHRINE_GLOWING,
	EMSG_SHRINE_MENDICANT,
	EMSG_SHRINE_SPARKLING,
	EMSG_SHRINE_TOWN,
	EMSG_SHRINE_SHIMMERING,
	EMSG_SHRINE_SOLAR1,
	EMSG_SHRINE_SOLAR2,
	EMSG_SHRINE_SOLAR3,
	EMSG_SHRINE_SOLAR4,
	EMSG_SHRINE_MURPHYS,
};

[[maybe_unused]] constexpr std::string_view toString(diablo_message value)
{
	switch(value) {
	case EMSG_NONE:
		return "None";
	case EMSG_NO_AUTOMAP_IN_TOWN:
		return "No Automap in town";
	case EMSG_NO_MULTIPLAYER_IN_DEMO:
		return "No Multiplayer in Demo";
	case EMSG_DIRECT_SOUND_FAILED:
		return "Direct Sound Failed";
	case EMSG_NOT_IN_SHAREWARE:
		return "Not in Shareware";
	case EMSG_NO_SPACE_TO_SAVE:
		return "No Space to Save";
	case EMSG_NO_PAUSE_IN_TOWN:
		return "No Pause in town";
	case EMSG_COPY_TO_HDD:
		return "Copy to Hdd";
	case EMSG_DESYNC:
		return "Desync";
	case EMSG_NO_PAUSE_IN_MP:
		return "No Pause in Mp";
	case EMSG_LOADING:
		return "Loading";
	case EMSG_SAVING:
		return "Saving";
	case EMSG_SHRINE_MYSTERIOUS:
		return "Shrine Mysterious";
	case EMSG_SHRINE_HIDDEN:
		return "Shrine Hidden";
	case EMSG_SHRINE_GLOOMY:
		return "Shrine Gloomy";
	case EMSG_SHRINE_WEIRD:
		return "Shrine Weird";
	case EMSG_SHRINE_MAGICAL:
		return "Shrine Magical";
	case EMSG_SHRINE_STONE:
		return "Shrine Stone";
	case EMSG_SHRINE_RELIGIOUS:
		return "Shrine Religious";
	case EMSG_SHRINE_ENCHANTED:
		return "Shrine Enchanted";
	case EMSG_SHRINE_THAUMATURGIC:
		return "Shrine Thaumaturgic";
	case EMSG_SHRINE_FASCINATING:
		return "Shrine Fascinating";
	case EMSG_SHRINE_CRYPTIC:
		return "Shrine Cryptic";
	case EMSG_SHRINE_UNUSED:
		return "Shrine Unused";
	case EMSG_SHRINE_ELDRITCH:
		return "Shrine Eldritch";
	case EMSG_SHRINE_EERIE:
		return "Shrine Eerie";
	case EMSG_SHRINE_DIVINE:
		return "Shrine Divine";
	case EMSG_SHRINE_HOLY:
		return "Shrine Holy";
	case EMSG_SHRINE_SACRED:
		return "Shrine Sacred";
	case EMSG_SHRINE_SPIRITUAL:
		return "Shrine Spiritual";
	case EMSG_SHRINE_SPOOKY1:
		return "Shrine Spooky1";
	case EMSG_SHRINE_SPOOKY2:
		return "Shrine Spooky2";
	case EMSG_SHRINE_ABANDONED:
		return "Shrine Abandoned";
	case EMSG_SHRINE_CREEPY:
		return "Shrine Creepy";
	case EMSG_SHRINE_QUIET:
		return "Shrine Quiet";
	case EMSG_SHRINE_SECLUDED:
		return "Shrine Secluded";
	case EMSG_SHRINE_ORNATE:
		return "Shrine Ornate";
	case EMSG_SHRINE_GLIMMERING:
		return "Shrine Glimmering";
	case EMSG_SHRINE_TAINTED1:
		return "Shrine Tainted1";
	case EMSG_SHRINE_TAINTED2:
		return "Shrine Tainted2";
	case EMSG_REQUIRES_LVL_8:
		return "Requires Lvl 8";
	case EMSG_REQUIRES_LVL_13:
		return "Requires Lvl 13";
	case EMSG_REQUIRES_LVL_17:
		return "Requires Lvl 17";
	case EMSG_BONECHAMB:
		return "Bonechamb";
	case EMSG_SHRINE_OILY:
		return "Shrine Oily";
	case EMSG_SHRINE_GLOWING:
		return "Shrine Glowing";
	case EMSG_SHRINE_MENDICANT:
		return "Shrine Mendicant";
	case EMSG_SHRINE_SPARKLING:
		return "Shrine Sparkling";
	case EMSG_SHRINE_TOWN:
		return "Shrine town";
	case EMSG_SHRINE_SHIMMERING:
		return "Shrine Shimmering";
	case EMSG_SHRINE_SOLAR1:
		return "Shrine Solar1";
	case EMSG_SHRINE_SOLAR2:
		return "Shrine Solar2";
	case EMSG_SHRINE_SOLAR3:
		return "Shrine Solar3";
	case EMSG_SHRINE_SOLAR4:
		return "Shrine Solar4";
	case EMSG_SHRINE_MURPHYS:
		return "Shrine Murphys";
	}
}

extern DWORD msgdelay;
extern diablo_message msgflag;

void InitDiabloMsg(diablo_message e);
void ClrDiabloMsg();
void DrawDiabloMsg(const CelOutputBuffer &out);

} // namespace devilution
