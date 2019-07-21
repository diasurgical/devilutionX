//HEADER_GOES_HERE
#ifndef __CURSOR_H__
#define __CURSOR_H__

extern int cursH;
extern int icursH28;
extern int cursW;
extern int pcursmonst;
extern int icursW28;
extern BYTE *pCursCels;
extern int icursH;
extern char pcursinvitem;
extern int icursW;
extern char pcursitem;
extern char pcursobj;
extern char pcursplr;
extern int cursmx;
extern int cursmy;
extern int pcurstemp;
extern int pcurs;

void InitCursor();
void FreeCursor();
void SetICursor(int i);
void SetCursor_(int i);
void NewCursor(int i);
void InitLevelCursor();
void CheckTown();
void CheckRportal();
void CheckCursMove();

/* rdata */
extern const int InvItemWidth[180];
extern const int InvItemHeight[180];

#endif /* __CURSOR_H__ */
