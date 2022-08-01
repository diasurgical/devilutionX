/**
 * @file interfac.h
 *
 * Interface of load screens.
 */
#pragma once

#include <cstdint>

#include "utils/ui_fwd.h"

namespace devilution {

/**
 * @brief Custom events.
 */
enum interface_mode : uint16_t {
	WM_DIABNEXTLVL = 0,
	WM_DIABPREVLVL,
	WM_DIABRTNLVL,
	WM_DIABSETLVL,
	WM_DIABWARPLVL,
	WM_DIABTOWNWARP,
	WM_DIABTWARPUP,
	WM_DIABRETOWN,
	WM_DIABNEWGAME,
	WM_DIABLOADGAME,

	WM_FIRST = WM_DIABNEXTLVL,
	WM_LAST = WM_DIABLOADGAME,
};

void RegisterCustomEvents();

bool IsCustomEvent(uint32_t eventType);

interface_mode GetCustomEvent(uint32_t eventType);

uint32_t CustomEventToSdlEvent(interface_mode eventType);

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
void IncProgress();
void CompleteProgress();
void ShowProgress(interface_mode uMsg);

} // namespace devilution
