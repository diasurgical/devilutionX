//HEADER_GOES_HERE
#ifndef __FAULT_H__
#define __FAULT_H__

typedef struct STACK_FRAME {
	struct STACK_FRAME *pNext;
	void *pCallRet;
} STACK_FRAME;

extern int fault_unused;
extern LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter;

void fault_init_filter();
void fault_cleanup_filter_atexit();
LPTOP_LEVEL_EXCEPTION_FILTER __cdecl fault_cleanup_filter();
LONG __stdcall TopLevelExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo);
void fault_hex_format(BYTE *ptr, DWORD numBytes);
void fault_unknown_module(LPCVOID lpAddress, LPSTR lpModuleName, int iMaxLength, int *sectionNum, int *sectionOffset);
void fault_call_stack(void *instr, STACK_FRAME *stackAddr);
char *fault_get_error_type(DWORD dwMessageId, LPSTR lpString1, DWORD nSize);
void *fault_set_filter(void *unused);
LPTOP_LEVEL_EXCEPTION_FILTER fault_reset_filter(void *unused);
LPTOP_LEVEL_EXCEPTION_FILTER fault_get_filter();

#endif /* __FAULT_H__ */
