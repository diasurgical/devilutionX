/**
 * @file misdat.h
 *
 * Interface of data related to missiles.
 */
#ifndef __MISDAT_H__
#define __MISDAT_H__

#include "missiles.h"

namespace dvl {

#ifdef __cplusplus
extern "C" {
#endif

extern MissileData missiledata[];
extern MisFileData misfiledata[];

#ifdef __cplusplus
}
#endif

}

#endif /* __MISDAT_H__ */
