//HEADER_GOES_HERE
#ifndef __DEAD_H__
#define __DEAD_H__

extern int spurtndx;
extern DeadStruct dead[MAXDEAD];
extern int stonendx;

void InitDead();
void AddDead(int dx, int dy, char dv, int ddir);
void SetDead();

#endif /* __DEAD_H__ */
