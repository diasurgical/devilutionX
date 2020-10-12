/**
 * @file types.h
 *
 * Include OS headers and set compiler state.
 */
#ifndef _TYPES_H
#define _TYPES_H

#define DEVILUTION_BEGIN_NAMESPACE namespace dvl {
#define DEVILUTION_END_NAMESPACE }

#include "miniwin.h"
#include "soundsample.h"
#include "thread.h"
#include "ui_fwd.h"

#include <limits.h>
#include "defs.h"
#include "enums.h"
#include "structs.h"

#endif

#ifdef ANDROID
extern bool FullGame;
#endif
