//HEADER_GOES_HERE

#include "../types.h"

LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter; // idb

struct exception_cpp_init
{
	exception_cpp_init()
	{
		exception_install_filter();
		j_exception_init_filter();
	}
} _exception_cpp_init;

void __cdecl exception_install_filter()
{
	exception_set_filter();
}

void __cdecl j_exception_init_filter()
{
	atexit(exception_init_filter);
}

void __cdecl exception_init_filter()
{
	exception_set_filter_ptr();
}

LONG __stdcall TopLevelExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	log_dump_computer_info();
	PEXCEPTION_RECORD xcpt = ExceptionInfo->ExceptionRecord;

	char szExceptionNameBuf[MAX_PATH]; // [esp+Ch] [ebp-210h]
	char *pszExceptionName = exception_get_error_type(ExceptionInfo->ExceptionRecord->ExceptionCode, szExceptionNameBuf, sizeof(szExceptionNameBuf));
	log_printf("Exception code: %08X %s\r\n", xcpt->ExceptionCode, pszExceptionName);

	char szModuleName[MAX_PATH];
	int sectionNumber, sectionOffset;
	exception_unknown_module(xcpt->ExceptionAddress, szModuleName, MAX_PATH, &sectionNumber, &sectionOffset);
	log_printf("Fault address:\t%08X %02X:%08X %s\r\n", xcpt->ExceptionAddress, sectionNumber, sectionOffset, szModuleName);
	
	PCONTEXT ctx = ExceptionInfo->ContextRecord;
	
	log_printf("\r\nRegisters:\r\n");
	log_printf(
		"EAX:%08X\r\nEBX:%08X\r\nECX:%08X\r\nEDX:%08X\r\nESI:%08X\r\nEDI:%08X\r\n",
		ctx->Eax,
		ctx->Ebx,
		ctx->Ecx,
		ctx->Edx,
		ctx->Esi,
		ctx->Edi);
	log_printf("CS:EIP:%04X:%08X\r\n", ctx->SegCs, ctx->Eip);
	log_printf("SS:ESP:%04X:%08X EBP:%08X\r\n", ctx->SegSs, ctx->Esp, ctx->Ebp);
	log_printf("DS:%04X ES:%04X FS:%04X GS:%04X\r\n", ctx->SegDs, ctx->SegEs, ctx->SegFs, ctx->SegGs);
	
	log_printf("Flags:%08X\r\n", ctx->EFlags);
	exception_call_stack((void *)ctx->Eip, (STACK_FRAME*)ctx->Ebp);
	
	log_printf("Stack bytes:\r\n");
	exception_hex_format((BYTE *)ctx->Esp, 0);
	
	log_printf("Code bytes:\r\n");
	exception_hex_format((BYTE *)ctx->Eip, 16);
	
	log_printf("\r\n");
	log_flush(1);

	if ( lpTopLevelExceptionFilter )
		return lpTopLevelExceptionFilter(ExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

void __fastcall exception_hex_format(BYTE *ptr, unsigned int numBytes)
{
	int i;

	while (numBytes > 0)
	{
		unsigned int bytesRead = 16;
		if (numBytes < 16 )
			bytesRead = numBytes;

		if ( IsBadReadPtr(ptr, bytesRead) )
			break;

		log_printf("0x%08x: ", ptr);

		for (i = 0; i < 16; ++i)
		{
			const char *fmt = "%02x ";
			if (i >= bytesRead)
				fmt = "   ";
			log_printf(fmt, ptr[i]);
			if (i % 4 == 3)
				log_printf(" ");
		}

		for (i = 0; i < bytesRead; ++i)
		{
			char c;
			if (isprint(ptr[i]))
				c = ptr[i];
			else
				c = '.';
			log_printf("%c", c);
		}

		log_printf("\r\n");
		ptr += bytesRead;
		numBytes -= bytesRead;
	}
	log_printf("\r\n");
}

void __fastcall exception_unknown_module(LPCVOID lpAddress, LPSTR lpModuleName, int iMaxLength, int *sectionNum, int *sectionOffset)
{
	lstrcpyn(lpModuleName, "*unknown*", iMaxLength);
	*sectionNum = 0;
	*sectionOffset = 0;

	MEMORY_BASIC_INFORMATION memInfo; // [esp+Ch] [ebp-24h]
	if (!VirtualQuery(lpAddress, &memInfo, sizeof(memInfo)))
		return;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)memInfo.AllocationBase;
	if ( !memInfo.AllocationBase )
		dosHeader = (PIMAGE_DOS_HEADER)GetModuleHandle(0);

	if (!GetModuleFileName((HMODULE)dosHeader, lpModuleName, iMaxLength))
	{
		lstrcpyn(lpModuleName, "*unknown*", iMaxLength);
		return;
	}

	if (dosHeader && dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
	{
		LONG ntOffset = dosHeader->e_lfanew;
		if (ntOffset)
		{
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)dosHeader + ntOffset);
			if (ntHeader->Signature == IMAGE_NT_SIGNATURE)
			{
				DWORD numSections = ntHeader->FileHeader.NumberOfSections;
				DWORD moduleOffset = (_BYTE *)lpAddress - (_BYTE *)dosHeader;
				PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeader);
				for (int i = 0; i < numSections; ++i, ++section)
				{
					DWORD sectionSize = section->SizeOfRawData;
					DWORD sectionAddress = section->VirtualAddress;
					if (section->SizeOfRawData <= section->Misc.VirtualSize)
						sectionSize = section->Misc.VirtualSize;

					if (moduleOffset >= sectionAddress && moduleOffset <= sectionAddress + sectionSize)
					{
						*sectionNum = i + 1;
						*sectionOffset = moduleOffset - sectionAddress;
						return;
					}
				}
			}
		}
	}
}

void __fastcall exception_call_stack(void *instr, STACK_FRAME *stackFrame)
{
	STACK_FRAME *oldStackFrame;

	log_printf("Call stack:\r\nAddress  Frame    Logical addr  Module\r\n");
	do
	{
		char szModuleName[MAX_PATH];
		int sectionNumber, sectionOffset;
		exception_unknown_module(instr, szModuleName, MAX_PATH, &sectionNumber, &sectionOffset);
		log_printf("%08X %08X %04X:%08X %s\r\n", instr, stackFrame, sectionNumber, sectionOffset, szModuleName);

		if ( IsBadWritePtr(stackFrame, 8u) )
			break;

		instr = stackFrame->pCallRet;
		oldStackFrame = stackFrame;
		stackFrame = stackFrame->pNext;

		if ((int)stackFrame % 4 != 0)
			break;
	}
	while (stackFrame > oldStackFrame && !IsBadWritePtr(stackFrame, 8u) );

	log_printf("\r\n");
}

#define CASE_EXCEPTION(v, errName) case EXCEPTION_ ## errName: v = #errName; break;
char *__fastcall exception_get_error_type(DWORD dwMessageId, LPSTR lpString1, DWORD nSize)
{
	const char *v4; // eax

	switch (dwMessageId) {
	CASE_EXCEPTION(v4, STACK_OVERFLOW);
	CASE_EXCEPTION(v4, FLT_DIVIDE_BY_ZERO);
	CASE_EXCEPTION(v4, FLT_INEXACT_RESULT);
	CASE_EXCEPTION(v4, FLT_INVALID_OPERATION);
	CASE_EXCEPTION(v4, FLT_OVERFLOW);
	CASE_EXCEPTION(v4, FLT_STACK_CHECK);
	CASE_EXCEPTION(v4, FLT_UNDERFLOW);
	CASE_EXCEPTION(v4, INT_DIVIDE_BY_ZERO);
	CASE_EXCEPTION(v4, INT_OVERFLOW);
	CASE_EXCEPTION(v4, PRIV_INSTRUCTION);
	CASE_EXCEPTION(v4, FLT_DENORMAL_OPERAND);
	CASE_EXCEPTION(v4, INVALID_HANDLE);
	CASE_EXCEPTION(v4, ILLEGAL_INSTRUCTION);
	CASE_EXCEPTION(v4, NONCONTINUABLE_EXCEPTION);
	CASE_EXCEPTION(v4, INVALID_DISPOSITION);
	CASE_EXCEPTION(v4, ARRAY_BOUNDS_EXCEEDED);
	CASE_EXCEPTION(v4, IN_PAGE_ERROR);
	CASE_EXCEPTION(v4, GUARD_PAGE);
	CASE_EXCEPTION(v4, DATATYPE_MISALIGNMENT);
	CASE_EXCEPTION(v4, BREAKPOINT);
	CASE_EXCEPTION(v4, SINGLE_STEP);
	CASE_EXCEPTION(v4, ACCESS_VIOLATION);
	default:
		HMODULE ntdll = GetModuleHandle("NTDLL.DLL");
		if (!FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, ntdll, dwMessageId, 0, lpString1, nSize, NULL))
		{
			v4 = "*unknown*";
		}
	}
	lstrcpyn(lpString1, v4, nSize);
	return lpString1;
}

void __fastcall exception_set_filter()
{
	lpTopLevelExceptionFilter = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)TopLevelExceptionFilter);
}

LPTOP_LEVEL_EXCEPTION_FILTER __cdecl exception_set_filter_ptr()
{
	return SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
}

LPTOP_LEVEL_EXCEPTION_FILTER __cdecl exception_get_filter()
{
	return lpTopLevelExceptionFilter;
}
