/**
 * @file error.h
 *
 * Interface of in-game message functions.
 */
#ifndef __ERROR_H__
#define __ERROR_H__

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

#endif /* __ERROR_H__ */
