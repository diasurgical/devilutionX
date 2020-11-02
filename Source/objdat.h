/**
 * @file objdat.h
 *
 * Interface of all object data.
 */
#ifndef __OBJDAT_H__
#define __OBJDAT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int ObjTypeConv[];
extern ObjDataStruct AllObjects[];
extern const char *const ObjMasterLoadList[];
#ifdef HELLFIRE
extern char *ObjCryptLoadList[];
extern char *ObjHiveLoadList[];
#endif

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __OBJDAT_H__ */
