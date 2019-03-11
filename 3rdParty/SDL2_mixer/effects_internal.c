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

  These are some helper functions for the internal mixer special effects.
*/

/* $Id$ */


     /* ------ These are used internally only. Don't touch. ------ */



#include <stdio.h>
#include <stdlib.h>
#include "SDL_mixer.h"

#define __MIX_INTERNAL_EFFECT__
#include "effects_internal.h"

/* Should we favor speed over memory usage and/or quality of output? */
int _Mix_effects_max_speed = 0;


void _Mix_InitEffects(void)
{
    _Mix_effects_max_speed = (SDL_getenv(MIX_EFFECTSMAXSPEED) != NULL);
}

void _Mix_DeinitEffects(void)
{
    _Eff_PositionDeinit();
}


void *_Eff_volume_table = NULL;


/* Build the volume table for Uint8-format samples.
 *
 * Each column of the table is a possible sample, while each row of the
 *  table is a volume. Volume is a Uint8, where 0 is silence and 255 is full
 *  volume. So _Eff_volume_table[128][mysample] would be the value of
 *  mysample, at half volume.
 */
void *_Eff_build_volume_table_u8(void)
{
    int volume;
    int sample;
    Uint8 *rc;

    if (!_Mix_effects_max_speed) {
        return(NULL);
    }

    if (!_Eff_volume_table) {
        rc = SDL_malloc(256 * 256);
        if (rc) {
            _Eff_volume_table = (void *) rc;
            for (volume = 0; volume < 256; volume++) {
                for (sample = -128; sample < 128; sample ++) {
                    *rc = (Uint8)(((float) sample) * ((float) volume / 255.0))
                        + 128;
                    rc++;
                }
            }
        }
    }

    return(_Eff_volume_table);
}


/* Build the volume table for Sint8-format samples.
 *
 * Each column of the table is a possible sample, while each row of the
 *  table is a volume. Volume is a Uint8, where 0 is silence and 255 is full
 *  volume. So _Eff_volume_table[128][mysample+128] would be the value of
 *  mysample, at half volume.
 */
void *_Eff_build_volume_table_s8(void)
{
    int volume;
    int sample;
    Sint8 *rc;

    if (!_Eff_volume_table) {
        rc = SDL_malloc(256 * 256);
        if (rc) {
            _Eff_volume_table = (void *) rc;
            for (volume = 0; volume < 256; volume++) {
                for (sample = -128; sample < 128; sample ++) {
                    *rc = (Sint8)(((float) sample) * ((float) volume / 255.0));
                    rc++;
                }
            }
        }
    }

    return(_Eff_volume_table);
}


/* end of effects.c ... */

/* vi: set ts=4 sw=4 expandtab: */
