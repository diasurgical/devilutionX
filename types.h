// temporary file

#ifndef _TYPES_H
#define _TYPES_H

#include "resource.h"

#define DEVILUTION_BEGIN_NAMESPACE namespace dvl {
#define DEVILUTION_END_NAMESPACE }

#include "miniwin/pushdecl.inc"
#include "miniwin.h"

DEVILUTION_BEGIN_NAMESPACE

#include <limits.h>
#include "defs.h"
#include "enums.h"
#include "structs.h"

// If defined, use copy protection [Default -> Defined]
#if !defined(_DEBUG) && !defined(SPAWN)
//#define COPYPROT
#endif

// If defined, don't reload for debuggers [Default -> Undefined]
// Note that with patch 1.03 the command line was hosed, this is required to pass arguments to the game
#ifdef _DEBUG
#define DEBUGGER
#endif

// If defined, don't fry the CPU [Default -> Undefined]
//#ifdef _DEBUG
#define SLEEPFIX
//#endif

// If defined, fix palette glitch in Windows Vista+ [Default -> Undefined]
//#define COLORFIX

#endif
