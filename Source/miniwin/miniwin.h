#pragma once

namespace devilution {

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef uint32_t DWORD;
typedef unsigned char BYTE;

//
// Everything else
//

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

} // namespace devilution
