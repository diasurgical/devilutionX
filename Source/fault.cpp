//HEADER_GOES_HERE

#include "../types.h"

LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter; // idb

#ifndef _MSC_VER
__attribute__((constructor))
#endif
static void
exception_c_init(void)
{
	exception_install_filter();
	j_exception_init_filter();
}

SEG_ALLOCATE(SEGMENT_C_INIT)
_PVFV exception_c_init_funcs[] = { &exception_c_init };

void __cdecl exception_install_filter()
{
	exception_set_filter();
}

void __cdecl j_exception_init_filter()
{
	atexit(exception_init_filter);
}

void __cdecl exception_init_filter(void)
{
	exception_set_filter_ptr();
}

LONG __stdcall TopLevelExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo)
{
	PEXCEPTION_RECORD xcpt;
	char szExceptionNameBuf[MAX_PATH];
	char szModuleName[MAX_PATH];
	char *pszExceptionName;
	int sectionNumber, sectionOffset;
	PCONTEXT ctx;

	log_dump_computer_info();
	xcpt = ExceptionInfo->ExceptionRecord;
	pszExceptionName = exception_get_error_type(ExceptionInfo->ExceptionRecord->ExceptionCode, szExceptionNameBuf, sizeof(szExceptionNameBuf));
	log_printf("Exception code: %08X %s\r\n", xcpt->ExceptionCode, pszExceptionName);

	exception_unknown_module(xcpt->ExceptionAddress, szModuleName, MAX_PATH, &sectionNumber, &sectionOffset);
	log_printf("Fault address:\t%08X %02X:%08X %s\r\n", xcpt->ExceptionAddress, sectionNumber, sectionOffset, szModuleName);

	ctx = ExceptionInfo->ContextRecord;

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
	exception_call_stack((void *)ctx->Eip, (STACK_FRAME *)ctx->Ebp);

	log_printf("Stack bytes:\r\n");
	exception_hex_format((BYTE *)ctx->Esp, 768);

	log_printf("Code bytes:\r\n");
	exception_hex_format((BYTE *)ctx->Eip, 16);

	log_printf("\r\n");
	log_flush(1);

	if (lpTopLevelExceptionFilter)
		return lpTopLevelExceptionFilter(ExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

void __fastcall exception_hex_format(BYTE *ptr, unsigned int numBytes)
{
	DWORD i, bytesRead;
	const char *fmt;
	BYTE c;

	while (numBytes > 0) {
		if (numBytes < 16)
			bytesRead = numBytes;
		else
			bytesRead = 16;

		if (IsBadReadPtr(ptr, bytesRead))
			break;

		log_printf("0x%08x: ", ptr);

		for (i = 0; i < 16; ++i) {
			fmt = "%02x ";
			if (i >= bytesRead)
				fmt = "   ";
			log_printf(fmt, ptr[i]);
			if (i % 4 == 3)
				log_printf(" ");
		}

		for (i = 0; i < bytesRead; i++) {
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
	MEMORY_BASIC_INFORMATION memInfo;
	PIMAGE_DOS_HEADER dosHeader;
	LONG ntOffset;
	PIMAGE_NT_HEADERS ntHeader;
	PIMAGE_SECTION_HEADER section;
	DWORD numSections, moduleOffset, sectionSize, sectionAddress;
	int i;

	lstrcpyn(lpModuleName, "*unknown*", iMaxLength);
	*sectionNum = 0;
	*sectionOffset = 0;

	if (!VirtualQuery(lpAddress, &memInfo, sizeof(memInfo)))
		return;

	dosHeader = (PIMAGE_DOS_HEADER)memInfo.AllocationBase;
	if (!memInfo.AllocationBase)
		dosHeader = (PIMAGE_DOS_HEADER)GetModuleHandle(0);

	if (!GetModuleFileName((HMODULE)dosHeader, lpModuleName, iMaxLength)) {
		lstrcpyn(lpModuleName, "*unknown*", iMaxLength);
		return;
	}

	if (dosHeader && dosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
		ntOffset = dosHeader->e_lfanew;
		if (ntOffset) {
			ntHeader = (PIMAGE_NT_HEADERS)((DWORD)dosHeader + ntOffset);
			if (ntHeader->Signature == IMAGE_NT_SIGNATURE) {
				section = IMAGE_FIRST_SECTION(ntHeader);
				numSections = ntHeader->FileHeader.NumberOfSections;
				moduleOffset = (BYTE *)lpAddress - (BYTE *)dosHeader;
				for (i = 0; i < numSections; i++, section++) {
					sectionSize = section->SizeOfRawData;
					sectionAddress = section->VirtualAddress;
					if (section->SizeOfRawData <= section->Misc.VirtualSize)
						sectionSize = section->Misc.VirtualSize;

					if (moduleOffset >= sectionAddress && moduleOffset <= sectionAddress + sectionSize) {
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
	char szModuleName[MAX_PATH];
	int sectionNumber, sectionOffset;

	log_printf("Call stack:\r\nAddress  Frame    Logical addr  Module\r\n");
	do {
		exception_unknown_module(instr, szModuleName, MAX_PATH, &sectionNumber, &sectionOffset);
		log_printf("%08X %08X %04X:%08X %s\r\n", instr, stackFrame, sectionNumber, sectionOffset, szModuleName);

		if (IsBadWritePtr(stackFrame, 8))
			break;

		instr = stackFrame->pCallRet;
		oldStackFrame = stackFrame;
		stackFrame = stackFrame->pNext;

		if ((DWORD)stackFrame % 4 != 0)
			break;
	} while (stackFrame > oldStackFrame && !IsBadWritePtr(stackFrame, 8));

	log_printf("\r\n");
}

#define CASE_EXCEPTION(v, errName) \
	case EXCEPTION_##errName:      \
		v = #errName;              \
		break;
char *__fastcall exception_get_error_type(DWORD dwMessageId, LPSTR lpString1, DWORD nSize)
{
	const char *v4; // eax
	HMODULE ntdll;

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
		ntdll = GetModuleHandle("NTDLL.DLL");
		if (!FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, (LPCVOID)ntdll, dwMessageId, 0, lpString1, nSize, NULL)) {
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
