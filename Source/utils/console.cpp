#include "./console.h"

#include <cstddef>
#include <cstdio>

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

} // namespace

void printInConsole(const char *fmt, ...)
{
	HANDLE handle = GetStderrHandle();
	if (handle == NULL)
		return;

	char message[4096];
	va_list ap;
	va_start(ap, fmt);
	std::vsnprintf(message, sizeof(message), fmt, ap);
	va_end(ap);

	WriteConsole(handle, message, strlen(message), NULL, NULL);
}

void printInConsoleV(const char *fmt, va_list ap)
{
	HANDLE handle = GetStderrHandle();
	if (handle == NULL)
		return;

	char message[4096];
	std::vsnprintf(message, sizeof(message), fmt, ap);

	if (handle == NULL)
		return;
	WriteConsole(handle, message, strlen(message), NULL, NULL);
}

} // namespace devilution
