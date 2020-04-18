/**
 * @file dead.h
 *
 * Interface of functions for placing dead monsters.
 */
#ifndef __DEAD_H__
#define __DEAD_H__

DEVILUTION_BEGIN_NAMESPACE

extern int spurtndx;
extern DeadStruct dead[MAXDEAD];
extern int stonendx;

void InitDead();
void AddDead(int dx, int dy, char dv, int ddir);
void SetDead();

DEVILUTION_END_NAMESPACE

#endif /* __DEAD_H__ */
