/**
 * @file interfac.h
 *
 * Interface of load screens.
 */
#ifndef __INTERFAC_H__
#define __INTERFAC_H__

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

#endif /* __INTERFAC_H__ */
