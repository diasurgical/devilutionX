#pragma once

namespace dvl {

typedef void *CRITICAL_SECTION, **LPCRITICAL_SECTION;
void WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);

} // namespace dvl
