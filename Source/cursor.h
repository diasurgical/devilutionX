/**
 * @file cursor.h
 *
 * Interface of cursor tracking functionality.
 */
#ifndef __CURSOR_H__
#define __CURSOR_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern int cursW;
extern int cursH;
extern int pcursmonst;
extern int icursW28;
extern int icursH28;
extern BYTE *pCursCels;
extern BYTE *pCursCels2;
extern int icursH;
extern char pcursinvitem;
extern int icursW;
extern char pcursitem;
extern char pcursobj;
extern char pcursplr;
extern int cursmx;
extern int cursmy;
extern int pcurs;

void InitCursor();
void FreeCursor();
void SetICursor(int i);
void SetCursor_(int i);
void NewCursor(int i);
void InitLevelCursor();
void CheckRportal();
void CheckTown();
void CheckCursMove();

/* rdata */
extern const int InvItemWidth[];
extern const int InvItemHeight[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __CURSOR_H__ */
