/**
 * @file error.h
 *
 * Interface of in-game message functions.
 */
#pragma once

#include "engine.h"

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

extern DWORD msgdelay;
extern char msgflag;

void InitDiabloMsg(char e);
void ClrDiabloMsg();
void DrawDiabloMsg(CelOutputBuffer out);

#ifdef __cplusplus
}
#endif

}
