/**
 * @file misdat.h
 *
 * Interface of data related to missiles.
 */
#ifndef __MISDAT_H__
#define __MISDAT_H__

#include "missiles.h"

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern MissileData missiledata[];
extern MisFileData misfiledata[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MISDAT_H__ */
