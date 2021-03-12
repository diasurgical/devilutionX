#pragma once

#if defined(_WIN64) || defined(_WIN32)
// Suppress definitions of `min` and `max` macros by <windows.h>:
#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace dvl {

void printInConsole(const char *fmt, ...)
{
	static HANDLE stderrHandle = NULL;
	if (stderrHandle == NULL) {
		if (AttachConsole(ATTACH_PARENT_PROCESS)) {
			stderrHandle = GetStdHandle(STD_ERROR_HANDLE);
		}
	}

	if (stderrHandle == NULL)
		return;

	char message[256];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(message, fmt, ap);
	va_end(ap);

	WriteConsole(stderrHandle, message, strlen(message), NULL, NULL);
}

} // namespace dvl
#else
#define printInConsole printf
#endif
