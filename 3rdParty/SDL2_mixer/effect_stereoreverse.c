/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  This file by Ryan C. Gordon (icculus@icculus.org)

  These are some internally supported special effects that use SDL_mixer's
  effect callback API. They are meant for speed over quality.  :)
*/

/* $Id$ */

#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_mixer.h"

#define __MIX_INTERNAL_EFFECT__
#include "effects_internal.h"

/* profile code:
    #include <sys/time.h>
    #include <unistd.h>
    struct timeval tv1;
    struct timeval tv2;

    gettimeofday(&tv1, NULL);

        ... do your thing here ...

    gettimeofday(&tv2, NULL);
    printf("%ld\n", tv2.tv_usec - tv1.tv_usec);
*/



/*
 * Stereo reversal effect...this one's pretty straightforward...
 */

static void SDLCALL _Eff_reversestereo32(int chan, void *stream, int len, void *udata)
{
    /* 16 bits * 2 channels. */
    Uint32 *ptr = (Uint32 *) stream;
    Uint32 tmp;
    int i;

    for (i = 0; i < len; i += 2 * sizeof (Uint32), ptr += 2) {
        tmp = ptr[0];
        ptr[0] = ptr[1];
        ptr[1] = tmp;
    }
}


static void SDLCALL _Eff_reversestereo16(int chan, void *stream, int len, void *udata)
{
    /* 16 bits * 2 channels. */
    Uint32 *ptr = (Uint32 *) stream;
    int i;

    for (i = 0; i < len; i += sizeof (Uint32), ptr++) {
        *ptr = (((*ptr) & 0xFFFF0000) >> 16) | (((*ptr) & 0x0000FFFF) << 16);
    }
}


static void SDLCALL _Eff_reversestereo8(int chan, void *stream, int len, void *udata)
{
    /* 8 bits * 2 channels. */
    Uint32 *ptr = (Uint32 *) stream;
    int i;

    /* get the last two bytes if len is not divisible by four... */
    if (len % sizeof (Uint32) != 0) {
        Uint16 *p = (Uint16 *) (((Uint8 *) stream) + (len - 2));
        *p = (Uint16)((((*p) & 0xFF00) >> 8) | (((*ptr) & 0x00FF) << 8));
        len -= 2;
    }

    for (i = 0; i < len; i += sizeof (Uint32), ptr++) {
        *ptr = (((*ptr) & 0x0000FF00) >> 8) | (((*ptr) & 0x000000FF) << 8) |
               (((*ptr) & 0xFF000000) >> 8) | (((*ptr) & 0x00FF0000) << 8);
    }
}


int Mix_SetReverseStereo(int channel, int flip)
{
    Mix_EffectFunc_t f = NULL;
    int channels;
    Uint16 format;

    Mix_QuerySpec(NULL, &format, &channels);

    if (channels == 2) {
        int bits = (format & 0xFF);
        switch (bits) {
        case 8:
            f = _Eff_reversestereo8;
            break;
        case 16:
            f = _Eff_reversestereo16;
            break;
        case 32:
            f = _Eff_reversestereo32;
            break;
        default:
            Mix_SetError("Unsupported audio format");
            return(0);
        }

        if (!flip) {
            return(Mix_UnregisterEffect(channel, f));
        } else {
            return(Mix_RegisterEffect(channel, f, NULL, NULL));
        }
    } else {
        Mix_SetError("Trying to reverse stereo on a non-stereo stream");
        return(0);
    }

    return(1);
}


/* end of effect_stereoreverse.c ... */

/* vi: set ts=4 sw=4 expandtab: */
