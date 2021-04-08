/**
 * @file interfac.h
 *
 * Interface of load screens.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Cutscenes {
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
} Cutscenes;

void interface_msg_pump();
bool IncProgress();
void ShowProgress(interface_mode uMsg);

#ifdef __cplusplus
}
#endif

}
