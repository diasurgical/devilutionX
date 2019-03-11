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

/* This file supports playing MIDI files with timidity */

#ifdef MUSIC_MID_TIMIDITY

#include "music_timidity.h"

#include "timidity/timidity.h"


typedef struct
{
    int play_count;
    MidiSong *song;
    SDL_AudioStream *stream;
    void *buffer;
    Sint32 buffer_size;
} TIMIDITY_Music;


static int TIMIDITY_Seek(void *context, double position);
static void TIMIDITY_Delete(void *context);

static int TIMIDITY_Open(const SDL_AudioSpec *spec)
{
    return Timidity_Init();
}

static void TIMIDITY_Close(void)
{
    Timidity_Exit();
}

void *TIMIDITY_CreateFromRW(SDL_RWops *src, int freesrc)
{
    TIMIDITY_Music *music;
    SDL_AudioSpec spec;
    SDL_bool need_stream = SDL_FALSE;

    music = (TIMIDITY_Music *)SDL_calloc(1, sizeof(*music));
    if (!music) {
        SDL_OutOfMemory();
        return NULL;
    }

    SDL_memcpy(&spec, &music_spec, sizeof(spec));
    if (spec.channels > 2) {
        need_stream = SDL_TRUE;
        spec.channels = 2;
    }
    music->song = Timidity_LoadSong(src, &spec);
    if (!music->song) {
        TIMIDITY_Delete(music);
        return NULL;
    }

    if (need_stream) {
        music->stream = SDL_NewAudioStream(spec.format, spec.channels, spec.freq,
                                           music_spec.format, music_spec.channels, music_spec.freq);
        if (!music->stream) {
            TIMIDITY_Delete(music);
            return NULL;
        }

        music->buffer_size = spec.samples * (SDL_AUDIO_BITSIZE(spec.format) / 8) * spec.channels;
        music->buffer = SDL_malloc(music->buffer_size);
        if (!music->buffer) {
            SDL_OutOfMemory();
            TIMIDITY_Delete(music);
            return NULL;
        }
    }

    if (freesrc) {
        SDL_RWclose(src);
    }
    return music;
}

static void TIMIDITY_SetVolume(void *context, int volume)
{
    TIMIDITY_Music *music = (TIMIDITY_Music *)context;
    Timidity_SetVolume(music->song, volume);
}

static int TIMIDITY_Play(void *context, int play_count)
{
    TIMIDITY_Music *music = (TIMIDITY_Music *)context;
    music->play_count = play_count;
    Timidity_Start(music->song);
    return TIMIDITY_Seek(music, 0.0);
}

static int TIMIDITY_GetSome(void *context, void *data, int bytes, SDL_bool *done)
{
    TIMIDITY_Music *music = (TIMIDITY_Music *)context;
    int filled, amount, expected;

    if (music->stream) {
        filled = SDL_AudioStreamGet(music->stream, data, bytes);
        if (filled != 0) {
            return filled;
        }
    }

    if (!music->play_count) {
        /* All done */
        *done = SDL_TRUE;
        return 0;
    }

    if (music->stream) {
        expected = music->buffer_size;
        amount = Timidity_PlaySome(music->song, music->buffer, music->buffer_size);
        if (SDL_AudioStreamPut(music->stream, music->buffer, amount) < 0) {
            return -1;
        }
    } else {
        expected = bytes;
        amount = Timidity_PlaySome(music->song, data, bytes);
    }
        
    if (amount < expected) {
        if (music->play_count == 1) {
            /* We didn't consume anything and we're done */
            music->play_count = 0;
        } else {
            int play_count = -1;
            if (music->play_count > 0) {
                play_count = (music->play_count - 1);
            }
            if (TIMIDITY_Play(music, play_count) < 0) {
                return -1;
            }
        }
    }
    if (music->stream) {
        /* We'll pick it up from the stream next time around */
        return 0;
    } else {
        /* We wrote output data */
        return amount;
    }
}
static int TIMIDITY_GetAudio(void *context, void *data, int bytes)
{
    return music_pcm_getaudio(context, data, bytes, MIX_MAX_VOLUME, TIMIDITY_GetSome);
}

static int TIMIDITY_Seek(void *context, double position)
{
    TIMIDITY_Music *music = (TIMIDITY_Music *)context;
    Timidity_Seek(music->song, (Uint32)(position * 1000));
    return 0;
}

static void TIMIDITY_Delete(void *context)
{
    TIMIDITY_Music *music = (TIMIDITY_Music *)context;

    if (music->song) {
        Timidity_FreeSong(music->song);
    }
    if (music->stream) {
        SDL_FreeAudioStream(music->stream);
    }
    if (music->buffer) {
        SDL_free(music->buffer);
    }
    SDL_free(music);
}

Mix_MusicInterface Mix_MusicInterface_TIMIDITY =
{
    "TIMIDITY",
    MIX_MUSIC_TIMIDITY,
    MUS_MID,
    SDL_FALSE,
    SDL_FALSE,

    NULL,   /* Load */
    TIMIDITY_Open,
    TIMIDITY_CreateFromRW,
    NULL,   /* CreateFromFile */
    TIMIDITY_SetVolume,
    TIMIDITY_Play,
    NULL,   /* IsPlaying */
    TIMIDITY_GetAudio,
    TIMIDITY_Seek,
    NULL,   /* Pause */
    NULL,   /* Resume */
    NULL,   /* Stop */
    TIMIDITY_Delete,
    TIMIDITY_Close,
    NULL,   /* Unload */
};

#endif /* MUSIC_MID_TIMIDITY */

/* vi: set ts=4 sw=4 expandtab: */
