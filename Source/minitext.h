/**
 * @file minitext.h
 *
 * Interface of scrolling dialog text.
 */
#ifndef __MINITEXT_H__
#define __MINITEXT_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern bool qtextflag;

void FreeQuestText();
void InitQuestText();
void InitQTextMsg(int m);
void DrawQTextBack();
void DrawQText();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __MINITEXT_H__ */
