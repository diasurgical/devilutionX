//HEADER_GOES_HERE
#ifndef __FAULT_H__
#define __FAULT_H__

struct STACK_FRAME {
	STACK_FRAME *pNext;
	void *pCallRet;
};

//int dword_52B9F4;
extern LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter; // idb

void __cdecl exception_cpp_init();
void __cdecl exception_install_filter();
void __cdecl j_exception_init_filter();
void __cdecl exception_init_filter();
LONG __stdcall TopLevelExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo);
void __fastcall exception_hex_format(BYTE *ptr, unsigned int numBytes);
void __fastcall exception_unknown_module(LPCVOID lpAddress, LPSTR lpModuleName, int iMaxLength, int *sectionNum, int *sectionOffset);
void __fastcall exception_call_stack(void *instr, STACK_FRAME *stackAddr);
char *__fastcall exception_get_error_type(DWORD dwMessageId, LPSTR lpString1, DWORD nSize);
void __fastcall exception_set_filter();
LPTOP_LEVEL_EXCEPTION_FILTER __cdecl exception_set_filter_ptr();
LPTOP_LEVEL_EXCEPTION_FILTER __cdecl exception_get_filter();

#endif /* __FAULT_H__ */
