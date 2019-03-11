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
*/

#ifdef MUSIC_MID_NATIVE

/* This file supports playing MIDI files with OS APIs */

#include "music_nativemidi.h"
#include "native_midi/native_midi.h"


static void *NATIVEMIDI_CreateFromRW(SDL_RWops *src, int freesrc)
{
    NativeMidiSong *music = native_midi_loadsong_RW(src, freesrc);
    if (!music) {
        Mix_SetError("%s", native_midi_error());
    }
    return music;
}

static int NATIVEMIDI_Play(void *context, int play_count)
{
    NativeMidiSong *music = (NativeMidiSong *)context;
    int loops = play_count;
    if (loops > 0) {
        --loops;
    }
    native_midi_start(music, loops);
    return 0;
}

static void NATIVEMIDI_SetVolume(void *context, int volume)
{
    native_midi_setvolume(volume);
}

static SDL_bool NATIVEMIDI_IsPlaying(void *context)
{
    return native_midi_active() ? SDL_TRUE : SDL_FALSE;
}

static void NATIVEMIDI_Pause(void *context)
{
    native_midi_pause();
}

static void NATIVEMIDI_Resume(void *context)
{
    native_midi_resume();
}

static void NATIVEMIDI_Stop(void *context)
{
    native_midi_stop();
}

static void NATIVEMIDI_Delete(void *context)
{
    NativeMidiSong *music = (NativeMidiSong *)context;
    native_midi_freesong(music);
}

Mix_MusicInterface Mix_MusicInterface_NATIVEMIDI =
{
    "NATIVEMIDI",
    MIX_MUSIC_NATIVEMIDI,
    MUS_MID,
    SDL_FALSE,
    SDL_FALSE,

    NULL,   /* Load */
    NULL,   /* Open */
    NATIVEMIDI_CreateFromRW,
    NULL,   /* CreateFromFile */
    NATIVEMIDI_SetVolume,
    NATIVEMIDI_Play,
    NATIVEMIDI_IsPlaying,
    NULL,   /* GetAudio */
    NULL,   /* Seek */
    NATIVEMIDI_Pause,
    NATIVEMIDI_Resume,
    NATIVEMIDI_Stop,
    NATIVEMIDI_Delete,
    NULL,   /* Close */
    NULL,   /* Unload */
};

#endif /* MUSIC_MID_NATIVE */

/* vi: set ts=4 sw=4 expandtab: */
