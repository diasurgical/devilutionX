/**
 * @file interfac.h
 *
 * Interface of load screens.
 */
#pragma once

#include <cstdint>

#include "utils/ui_fwd.h"

namespace devilution {

inline Sint16 GetUIOffsetY()
{
	return ((Sint16)((GetScreenHeight() - 480) / 2));
}
#define UI_OFFSET_Y (GetUIOffsetY())

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
	WM_DIABLOADGAME = 0x40B,
	// clang-format on

	WM_FIRST = WM_DIABNEXTLVL,
	WM_LAST = WM_DIABLOADGAME,
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

void interface_msg_pump();
bool IncProgress();
void ShowProgress(interface_mode uMsg);

} // namespace devilution
