//HEADER_GOES_HERE
#ifndef __DTHREAD_H__
#define __DTHREAD_H__

extern int dthread_cpp_init_value; // weak
extern unsigned int glpDThreadId; // idb
extern TMegaPkt *sgpInfoHead; /* may not be right struct */
extern char byte_52A508; // weak
extern HANDLE sghWorkToDoEvent; // idb

void __cdecl dthread_cpp_init_1();
void __cdecl dthread_cpp_init_2();
void __cdecl dthread_init_mutex();
void __cdecl dthread_cleanup_mutex_atexit();
void __cdecl dthread_cleanup_mutex();
void __fastcall dthread_remove_player(int pnum);
void __fastcall dthread_send_delta(int pnum, char cmd, void *pbSrc, int dwLen);
void __cdecl dthread_start();
unsigned int __stdcall dthread_handler(void *a1);
void __cdecl dthread_cleanup();

/* data */
extern int dthread_inf; // weak

#endif /* __DTHREAD_H__ */
