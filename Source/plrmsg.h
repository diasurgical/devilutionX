/**
 * @file plrmsg.h
 *
 * Interface of functionality for printing the ingame chat messages.
 */
#ifndef __PLRMSG_H__
#define __PLRMSG_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _plrmsg {
	Uint32 time;
	Uint8 player;
	char str[144];
} _plrmsg;

void plrmsg_delay(BOOL delay);
char *ErrorPlrMsg(const char *pszMsg);
size_t EventPlrMsg(const char *pszFmt, ...);
void SendPlrMsg(int pnum, const char *pszStr);
void ClearPlrMsg();
void InitPlrMsg();
void DrawPlrMsg();
void PrintPlrMsg(DWORD x, DWORD y, DWORD width, const char *str, BYTE col);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __PLRMSG_H__ */
