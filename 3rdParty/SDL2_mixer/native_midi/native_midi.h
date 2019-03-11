/*
  native_midi:  Hardware Midi support for the SDL_mixer library
  Copyright (C) 2000  Florian 'Proff' Schulze <florian.proff.schulze@gmx.net>

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
*/

#ifndef _NATIVE_MIDI_H_
#define _NATIVE_MIDI_H_

#include "SDL_rwops.h"

typedef struct _NativeMidiSong NativeMidiSong;

int native_midi_detect(void);
NativeMidiSong *native_midi_loadsong_RW(SDL_RWops *src, int freesrc);
void native_midi_freesong(NativeMidiSong *song);
void native_midi_start(NativeMidiSong *song, int loops);
void native_midi_pause(void);
void native_midi_resume(void);
void native_midi_stop(void);
int native_midi_active(void);
void native_midi_setvolume(int volume);
const char *native_midi_error(void);

#endif /* _NATIVE_MIDI_H_ */
