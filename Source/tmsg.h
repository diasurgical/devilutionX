//HEADER_GOES_HERE
#ifndef __TMSG_H__
#define __TMSG_H__

int tmsg_get(BYTE *pbMsg, DWORD dwMaxLen);
void tmsg_add(BYTE *pbMsg, BYTE bLen);
void tmsg_start();
void tmsg_cleanup();

#endif /* __TMSG_H__ */
