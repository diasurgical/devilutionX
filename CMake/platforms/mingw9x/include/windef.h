#ifndef _WINDEF_OVERRIDE_
#define _WINDEF_OVERRIDE_

#include_next <windef.h>

// MinGW does not define these when _WIN32_WINNT < 0x0400
// but it declares functions that use it unconditionally.
typedef enum _FINDEX_INFO_LEVELS {
	FindExInfoStandard,
	FindExInfoBasic,
	FindExInfoMaxInfoLevel
} FINDEX_INFO_LEVELS;
typedef enum _FINDEX_SEARCH_OPS {
	FindExSearchNameMatch,
	FindExSearchLimitToDirectories,
	FindExSearchLimitToDevices,
	FindExSearchMaxSearchOp
} FINDEX_SEARCH_OPS;

typedef void* SOLE_AUTHENTICATION_SERVICE;

#endif /* _WINDEF_ */
