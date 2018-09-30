//HEADER_GOES_HERE
#ifndef __CURSOR_H__
#define __CURSOR_H__

extern int cursH; // weak
extern int icursH28; // idb
extern int cursW; // idb
extern int pcursmonst; // idb
extern int icursW28; // idb
extern void *pCursCels;
extern int icursH; // weak
extern char pcursinvitem; // weak
extern int icursW; // weak
extern char pcursitem; // weak
extern char pcursobj; // weak
extern char pcursplr; // weak
extern int cursmx;
extern int cursmy;
extern int dword_4B8CCC; // weak
extern int pcurs; // idb

void __cdecl InitCursor();
void __cdecl FreeCursor();
void __fastcall SetICursor(int i);
void __fastcall SetCursor(int i);
void __fastcall NewCursor(int i);
void __cdecl InitLevelCursor();
void __cdecl CheckTown();
void __cdecl CheckRportal();
void __cdecl CheckCursMove();

/* rdata */
extern const int InvItemWidth[180];
extern const int InvItemHeight[180];

#endif /* __CURSOR_H__ */
