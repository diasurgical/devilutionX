/**
 * @file monstdat.h
 *
 * Interface of all monster data.
 */
#ifndef __MONSTDAT_H__
#define __MONSTDAT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern const MonsterData monsterdata[];
extern const int MonstConvTbl[];
extern const char MonstAvailTbl[];
extern const UniqMonstStruct UniqMonst[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MONSTDAT_H__ */
