/**
 * @file drlg_l4.h
 *
 * Interface of the hell level generation algorithms.
 */
#ifndef __DRLG_L4_H__
#define __DRLG_L4_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int diabquad1x;
extern int diabquad1y;
extern int diabquad2x;
extern int diabquad2y;
extern int diabquad3x;
extern int diabquad3y;
extern int diabquad4x;
extern int diabquad4y;
BOOL IsDURWall(char d);
BOOL IsDLLWall(char dd);
void CreateL4Dungeon(DWORD rseed, int entry);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __DRLG_L4_H__ */
