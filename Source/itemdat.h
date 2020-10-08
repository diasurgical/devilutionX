/**
 * @file itemdat.h
 *
 * Interface of all item data.
 */
#ifndef __ITEMDAT_H__
#define __ITEMDAT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern ItemDataStruct AllItemsList[];
extern const PLStruct PL_Prefix[];
extern const PLStruct PL_Suffix[];
extern const UItemStruct UniqueItemList[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __ITEMDAT_H__ */
