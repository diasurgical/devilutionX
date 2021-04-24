/**
 * @file interfac.h
 *
 * Interface of load screens.
 */
#pragma once

#include <stdint.h>
#include <string_view>

#include "utils/ui_fwd.h"

namespace devilution {

#define UI_OFFSET_Y ((Sint16)((gnScreenHeight - 480) / 2))

enum interface_mode : uint16_t {
	// clang-format off
	WM_DIABNEXTLVL  = 0x402, // WM_USER+2
	WM_DIABPREVLVL  = 0x403,
	WM_DIABRTNLVL   = 0x404,
	WM_DIABSETLVL   = 0x405,
	WM_DIABWARPLVL  = 0x406,
	WM_DIABTOWNWARP = 0x407,
	WM_DIABTWARPUP  = 0x408,
	WM_DIABRETOWN   = 0x409,
	WM_DIABNEWGAME  = 0x40A,
	WM_DIABLOADGAME = 0x40B
	// clang-format on
};

enum Cutscenes : uint8_t {
	CutStart,
	CutTown,
	CutLevel1,
	CutLevel2,
	CutLevel3,
	CutLevel4,
	CutLevel5,
	CutLevel6,
	CutPortal,
	CutPortalRed,
	CutGate,
};

constexpr std::string_view toString(Cutscenes value)
{
	switch(value) {
	case CutStart:
		return "Start";
	case CutTown:
		return "Town";
	case CutLevel1:
		return "Level1";
	case CutLevel2:
		return "Level2";
	case CutLevel3:
		return "Level3";
	case CutLevel4:
		return "Level4";
	case CutLevel5:
		return "Level5";
	case CutLevel6:
		return "Level6";
	case CutPortal:
		return "Portal";
	case CutPortalRed:
		return "Red Portal";
	case CutGate:
		return "Gate";
	}
}

void interface_msg_pump();
bool IncProgress();
void ShowProgress(interface_mode uMsg);

} // namespace devilution
