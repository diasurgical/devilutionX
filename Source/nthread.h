//HEADER_GOES_HERE
#ifndef __NTHREAD_H__
#define __NTHREAD_H__

extern BYTE sgbNetUpdateRate;
extern DWORD gdwMsgLenTbl[MAX_PLRS];
extern DWORD gdwDeltaBytesSec;
extern BOOLEAN nthread_should_run;
extern DWORD gdwTurnsInTransit;
extern uintptr_t glpMsgTbl[MAX_PLRS];
extern unsigned int glpNThreadId;
extern int turn_upper_bit;
extern BOOLEAN sgbThreadIsRunning;
extern DWORD gdwLargestMsgSize;
extern DWORD gdwNormalMsgSize;
extern int last_tick;

void nthread_terminate_game(const char *pszFcn);
DWORD nthread_send_and_recv_turn(DWORD cur_turn, int turn_delta);
BOOL nthread_recv_turns(BOOL *pfSendAsync);
void nthread_set_turn_upper_bit();
void nthread_start(BOOL set_turn_upper_bit);
unsigned int nthread_handler(void *);
void nthread_cleanup();
void nthread_ignore_mutex(BOOL bStart);
BOOL nthread_has_500ms_passed(BOOL unused);

/* rdata */

#endif /* __NTHREAD_H__ */
