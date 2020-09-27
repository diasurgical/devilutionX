/**
 * @file restrict.h
 *
 * Interface of functionality for checking if the game will be able run on the system.
 */
#ifndef __RESTRICT_H__
#define __RESTRICT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

void ReadOnlyTest();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __RESTRICT_H__ */
