//HEADER_GOES_HERE
#ifndef __DEAD_H__
#define __DEAD_H__

extern int spurtndx; // weak
extern DeadStruct dead[MAXDEAD];
extern int stonendx;

void __cdecl InitDead();
void __fastcall AddDead(int dx, int dy, char dv, int ddir);
void __cdecl SetDead();

#endif /* __DEAD_H__ */
