// temporary file

#ifndef _TYPES_H
#define _TYPES_H



#include "resource.h"

#ifdef MINIWIN
#include "miniwin.h"

#else
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <ddraw.h>
#include <dsound.h>
#include <io.h>
#include <math.h>
#include <time.h>
#include <process.h>
#include <shlobj.h>
#include <shellapi.h>
#endif

#ifdef __GNUC__
#include <ctype.h>
#endif

// tell Visual C++ to shut the hell up
#ifdef _MSC_VER
#pragma warning (disable : 4309) // truncation of constant value
#pragma warning (disable : 4305) // truncation of int
#pragma warning (disable : 4018) // signed/unsigned mismatch
#pragma warning (disable : 4700) // used without having been initialized
#pragma warning (disable : 4804) // unsafe use of type 'bool' in operation
#pragma warning (disable : 4805) // unsafe bool mix
#pragma warning (disable : 4244) // conversion loss
#pragma warning (disable : 4800) // bool perf
#pragma warning (disable : 4146) // negative unsigned
#endif

#include "defs.h"
#include "enums.h"
#include "structs.h"

#include "DiabloUI/diabloui.h"
#include "3rdParty/Storm/Source/storm.h"
#include "3rdParty/PKWare/pkware.h"

// If defined, use copy protection [Default -> Defined]
//#define COPYPROT
// If defined, don't reload for debuggers [Default -> Undefined]
// Note that with patch 1.03 the command line was hosed, this is required to pass arguments to the game
#ifdef _DEBUG
#define DEBUGGER
#endif
// If defined, don't fry the CPU [Default -> Undefined]
#define SLEEP
// If defined, use standard memcpy() in place of qmemcpy() [Default -> Undefined]
// Will be replaced with [rep movsd] if optimization is used
#define FAST_MEMCPY


/* temp macro for asm XLAT */
#define ASM_XLAT(eax,ebx) eax = (eax & 0xFFFFFF00) + LOBYTE(ebx[LOBYTE(eax)])

// header files
#include "Source/appfat.h"
#include "Source/automap.h"
#include "Source/capture.h"
#include "Source/codec.h"
#include "Source/control.h"
#include "Source/cursor.h"
#include "Source/dead.h"
#include "Source/debug.h"
#include "Source/diablo.h"
#include "Source/doom.h"
#include "Source/drlg_l1.h"
#include "Source/drlg_l2.h"
#include "Source/drlg_l3.h"
#include "Source/drlg_l4.h"
#include "Source/dthread.h"
#include "Source/dx.h"
#include "Source/effects.h"
#include "Source/encrypt.h"
#include "Source/engine.h"
#include "Source/error.h"
#include "Source/fault.h"
#include "Source/gamemenu.h"
#include "Source/gendung.h"
#include "Source/gmenu.h"
#include "Source/help.h"
#include "Source/init.h"
#include "Source/interfac.h"
#include "Source/inv.h"
#include "Source/items.h"
#include "Source/lighting.h"
#include "Source/loadsave.h"
#include "Source/logging.h"
#include "Source/mainmenu.h"
#include "Source/minitext.h"
#include "Source/missiles.h"
#include "Source/monster.h"
#include "Source/movie.h"
#include "Source/mpqapi.h"
#include "Source/msg.h"
#include "Source/msgcmd.h"
#include "Source/multi.h"
#include "Source/nthread.h"
#include "Source/objects.h"
#include "Source/pack.h"
#include "Source/palette.h"
#include "Source/path.h"
#include "Source/pfile.h"
#include "Source/player.h"
#include "Source/plrmsg.h"
#include "Source/portal.h"
#include "Source/quests.h"
#include "Source/restrict.h"
#include "Source/scrollrt.h"
#include "Source/setmaps.h"
#include "Source/sha.h"
#include "Source/sound.h"
#include "Source/spells.h"
#include "Source/stores.h"
#include "Source/sync.h"
#include "Source/textdat.h" // check file name
#include "Source/themes.h"
#include "Source/tmsg.h"
#include "Source/town.h"
#include "Source/towners.h"
#include "Source/track.h"
#include "Source/trigs.h"
#include "Source/wave.h"
#include "Source/render.h" // linked last, likely .s/.asm

#endif
