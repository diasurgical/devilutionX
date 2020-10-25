/**
 * @file error.h
 *
 * Interface of in-game message functions.
 */
#ifndef __ERROR_H__
#define __ERROR_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern char msgtable[MAX_SEND_STR_LEN];
extern DWORD msgdelay;
extern char msgflag;
extern char msgcnt;

void InitDiabloMsg(char e);
void ClrDiabloMsg();
void DrawDiabloMsg();

/* data */
extern const char *const MsgStrings[];

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __ERROR_H__ */
