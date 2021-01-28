/**
 * @file tmsg.h
 *
 * Interface of functionality transmitting chat messages.
 */
#ifndef __TMSG_H__
#define __TMSG_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

int tmsg_get(Uint8 *pbMsg, Uint32 dwMaxLen);
void tmsg_add(Uint8 *pbMsg, Uint8 bLen);
void tmsg_start();
void tmsg_cleanup();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __TMSG_H__ */
