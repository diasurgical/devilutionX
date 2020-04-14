/**
 * @file minitext.h
 *
 * Interface of scrolling dialog text.
 */
#ifndef __MINITEXT_H__
#define __MINITEXT_H__

extern int qtexty;
extern char *qtextptr;
extern BOOLEAN qtextflag;
extern BYTE *pMedTextCels;
extern BYTE *pTextBoxCels;

void FreeQuestText();
void InitQuestText();
void InitQTextMsg(int m);
void DrawQTextBack();
void PrintQTextChr(int sx, int sy, BYTE *pCelBuff, int nCel);
void DrawQText();

/* rdata */

extern const BYTE mfontframe[127];
extern const BYTE mfontkern[56];

/* data */

extern int qscroll_spd_tbl[9];

#endif /* __MINITEXT_H__ */
