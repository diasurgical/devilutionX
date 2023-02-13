#include "./console.h"

#if (defined(_WIN64) || defined(_WIN32)) && !defined(NXDK)
#include <cstddef>
#include <cstdio>
#include <string>

// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace devilution {

namespace {

HANDLE GetStderrHandle()
{
	static HANDLE handle = NULL;
	if (handle == NULL) {
		if (AttachConsole(ATTACH_PARENT_PROCESS)) {
			handle = GetStdHandle(STD_ERROR_HANDLE);
		}
	}
	return handle;
}

void WriteToStderr(string_view str)
{
	HANDLE handle = GetStderrHandle();
	if (handle == NULL)
		return;
	WriteConsole(handle, str.data(), str.size(), NULL, NULL);
}

} // namespace

void printInConsole(string_view str)
{
	OutputDebugString(std::string(str).c_str());
	WriteToStderr(str);
}

void printNewlineInConsole()
{
	OutputDebugString("\r\n");
	WriteToStderr("\r\n");
}

void printfInConsole(const char *fmt, ...)
{
	char message[4096];
	va_list ap;
	va_start(ap, fmt);
	std::vsnprintf(message, sizeof(message), fmt, ap);
	va_end(ap);
	OutputDebugString(message);
	WriteToStderr(message);
}

void vprintfInConsole(const char *fmt, va_list ap)
{
	char message[4096];
	std::vsnprintf(message, sizeof(message), fmt, ap);
	OutputDebugString(message);
	WriteToStderr(message);
}

} // namespace devilution
#else
#include <cstdio>

namespace devilution {

void printInConsole(string_view str)
{
	std::fwrite(str.data(), sizeof(char), str.size(), stderr);
}

void printNewlineInConsole()
{
	std::fputs("\n", stderr);
}

void printfInConsole(const char *fmt, ...)
{
	std::va_list ap;
	va_start(ap, fmt);
	std::vfprintf(stderr, fmt, ap);
	va_end(ap);
}

void vprintfInConsole(const char *fmt, std::va_list ap)
{
	std::vfprintf(stderr, fmt, ap);
}

} // namespace devilution
#endif
