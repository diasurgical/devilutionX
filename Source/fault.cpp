#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter;

int fault_unused;

#ifndef _MSC_VER
__attribute__((constructor))
#endif
static void
fault_c_init(void)
{
	fault_init_filter();
	fault_cleanup_filter_atexit();
}

SEG_ALLOCATE(SEGMENT_C_INIT)
_PVFV exception_c_init_funcs[] = { &fault_c_init };

void fault_init_filter()
{
	fault_set_filter(&fault_unused);
}

void fault_cleanup_filter_atexit()
{
	atexit((void(__cdecl *)(void))fault_cleanup_filter);
}

LPTOP_LEVEL_EXCEPTION_FILTER __cdecl fault_cleanup_filter()
{
	return fault_reset_filter(&fault_unused);
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
	pszExceptionName = fault_get_error_type(ExceptionInfo->ExceptionRecord->ExceptionCode, szExceptionNameBuf, sizeof(szExceptionNameBuf));
	log_printf("Exception code: %08X %s\r\n", xcpt->ExceptionCode, pszExceptionName);

	fault_unknown_module(xcpt->ExceptionAddress, szModuleName, MAX_PATH, &sectionNumber, &sectionOffset);
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
	fault_call_stack((void *)ctx->Eip, (STACK_FRAME *)ctx->Ebp);

	log_printf("Stack bytes:\r\n");
	fault_hex_format((BYTE *)ctx->Esp, 768);

	log_printf("Code bytes:\r\n");
	fault_hex_format((BYTE *)ctx->Eip, 16);

	log_printf("\r\n");
	log_flush(1);

	if (lpTopLevelExceptionFilter)
		return lpTopLevelExceptionFilter(ExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

void fault_hex_format(BYTE *ptr, DWORD numBytes)
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

void fault_unknown_module(LPCVOID lpAddress, LPSTR lpModuleName, int iMaxLength, int *sectionNum, int *sectionOffset)
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

void fault_call_stack(void *instr, STACK_FRAME *stackFrame)
{
	STACK_FRAME *oldStackFrame;
	char szModuleName[MAX_PATH];
	int sectionNumber, sectionOffset;

	log_printf("Call stack:\r\nAddress  Frame    Logical addr  Module\r\n");
	do {
		fault_unknown_module(instr, szModuleName, MAX_PATH, &sectionNumber, &sectionOffset);
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

char *fault_get_error_type(DWORD dwMessageId, LPSTR lpString1, DWORD nSize)
{
	const char *s;

	switch (dwMessageId) {
	case EXCEPTION_STACK_OVERFLOW:
		s = "STACK_OVERFLOW";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		s = "FLT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		s = "FLT_INEXACT_RESULT";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		s = "FLT_INVALID_OPERATION";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		s = "FLT_OVERFLOW";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		s = "FLT_STACK_CHECK";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		s = "FLT_UNDERFLOW";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		s = "INT_DIVIDE_BY_ZERO";
		break;
	case EXCEPTION_INT_OVERFLOW:
		s = "INT_OVERFLOW";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		s = "PRIV_INSTRUCTION";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		s = "FLT_DENORMAL_OPERAND";
		break;
	case EXCEPTION_INVALID_HANDLE:
		s = "INVALID_HANDLE";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		s = "ILLEGAL_INSTRUCTION";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		s = "NONCONTINUABLE_EXCEPTION";
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		s = "INVALID_DISPOSITION";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		s = "ARRAY_BOUNDS_EXCEEDED";
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		s = "IN_PAGE_ERROR";
		break;
	case EXCEPTION_GUARD_PAGE:
		s = "GUARD_PAGE";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		s = "DATATYPE_MISALIGNMENT";
		break;
	case EXCEPTION_BREAKPOINT:
		s = "BREAKPOINT";
		break;
	case EXCEPTION_SINGLE_STEP:
		s = "SINGLE_STEP";
		break;
	case EXCEPTION_ACCESS_VIOLATION:
		s = "ACCESS_VIOLATION";
		break;
	default:
		if (FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, (LPCVOID)GetModuleHandle("NTDLL.DLL"), dwMessageId, 0, lpString1, nSize, NULL)) {
			return lpString1;
		}
		s = "*unknown*";
		break;
	}
	lstrcpyn(lpString1, s, nSize);
	return lpString1;
}

void *fault_set_filter(void *unused)
{
	lpTopLevelExceptionFilter = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)TopLevelExceptionFilter);
	return unused;
}

LPTOP_LEVEL_EXCEPTION_FILTER fault_reset_filter(void *unused)
{
	//return SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
	return lpTopLevelExceptionFilter;
}

LPTOP_LEVEL_EXCEPTION_FILTER fault_get_filter()
{
	return lpTopLevelExceptionFilter;
}

DEVILUTION_END_NAMESPACE
