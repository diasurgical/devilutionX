/*
  native_midi:  Hardware Midi support for the SDL_mixer library
  Copyright (C) 2000,2001  Florian 'Proff' Schulze <florian.proff.schulze@gmx.net>

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

#ifndef _NATIVE_MIDI_COMMON_H_
#define _NATIVE_MIDI_COMMON_H_

#include "SDL.h"

/* Midi Status Bytes */
#define MIDI_STATUS_NOTE_OFF    0x8
#define MIDI_STATUS_NOTE_ON     0x9
#define MIDI_STATUS_AFTERTOUCH  0xA
#define MIDI_STATUS_CONTROLLER  0xB
#define MIDI_STATUS_PROG_CHANGE 0xC
#define MIDI_STATUS_PRESSURE    0xD
#define MIDI_STATUS_PITCH_WHEEL 0xE
#define MIDI_STATUS_SYSEX       0xF

/* We store the midi events in a linked list; this way it is
   easy to shuffle the tracks together later on; and we are
   flexible in the size of each elemnt.
 */
typedef struct MIDIEvent
{
    Uint32  time;       /* Time at which this midi events occurs */
    Uint8   status;     /* Status byte */
    Uint8   data[2];    /* 1 or 2 bytes additional data for most events */

    Uint32  extraLen;   /* For some SysEx events, we need additional storage */
    Uint8   *extraData;

    struct MIDIEvent *next;
} MIDIEvent;


/* Load a midifile to memory, converting it to a list of MIDIEvents.
   This function returns a linked lists of MIDIEvents, 0 if an error occured.
 */
MIDIEvent *CreateMIDIEventList(SDL_RWops *rw, Uint16 *division);

/* Release a MIDIEvent list after usage. */
void FreeMIDIEventList(MIDIEvent *head);


#endif /* _NATIVE_MIDI_COMMON_H_ */
