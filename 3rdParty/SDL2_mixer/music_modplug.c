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

#ifdef MUSIC_MOD_MODPLUG

#include "SDL_loadso.h"

#include "music_modplug.h"

#ifdef MODPLUG_HEADER
#include MODPLUG_HEADER
#else
#include <libmodplug/modplug.h>
#endif

typedef struct {
    int loaded;
    void *handle;

    ModPlugFile* (*ModPlug_Load)(const void* data, int size);
    void (*ModPlug_Unload)(ModPlugFile* file);
    int  (*ModPlug_Read)(ModPlugFile* file, void* buffer, int size);
    void (*ModPlug_Seek)(ModPlugFile* file, int millisecond);
    void (*ModPlug_GetSettings)(ModPlug_Settings* settings);
    void (*ModPlug_SetSettings)(const ModPlug_Settings* settings);
    void (*ModPlug_SetMasterVolume)(ModPlugFile* file,unsigned int cvol) ;
} modplug_loader;

static modplug_loader modplug = {
    0, NULL
};


static ModPlug_Settings settings;

#ifdef MODPLUG_DYNAMIC
#define FUNCTION_LOADER(FUNC, SIG) \
    modplug.FUNC = (SIG) SDL_LoadFunction(modplug.handle, #FUNC); \
    if (modplug.FUNC == NULL) { SDL_UnloadObject(modplug.handle); return -1; }
#else
#define FUNCTION_LOADER(FUNC, SIG) \
    modplug.FUNC = FUNC;
#endif

static int MODPLUG_Load(void)
{
    if (modplug.loaded == 0) {
#ifdef MODPLUG_DYNAMIC
        modplug.handle = SDL_LoadObject(MODPLUG_DYNAMIC);
        if (modplug.handle == NULL) {
            return -1;
        }
#elif defined(__MACOSX__)
        extern ModPlugFile* ModPlug_Load(const void* data, int size) __attribute__((weak_import));
        if (ModPlug_Load == NULL)
        {
            /* Missing weakly linked framework */
            Mix_SetError("Missing modplug.framework");
            return -1;
        }
#endif
        FUNCTION_LOADER(ModPlug_Load, ModPlugFile* (*)(const void* data, int size))
        FUNCTION_LOADER(ModPlug_Unload, void (*)(ModPlugFile* file))
        FUNCTION_LOADER(ModPlug_Read, int  (*)(ModPlugFile* file, void* buffer, int size))
        FUNCTION_LOADER(ModPlug_Seek, void (*)(ModPlugFile* file, int millisecond))
        FUNCTION_LOADER(ModPlug_GetSettings, void (*)(ModPlug_Settings* settings))
        FUNCTION_LOADER(ModPlug_SetSettings, void (*)(const ModPlug_Settings* settings))
        FUNCTION_LOADER(ModPlug_SetMasterVolume, void (*)(ModPlugFile* file,unsigned int cvol))
    }
    ++modplug.loaded;

    return 0;
}

static void MODPLUG_Unload(void)
{
    if (modplug.loaded == 0) {
        return;
    }
    if (modplug.loaded == 1) {
#ifdef MODPLUG_DYNAMIC
        SDL_UnloadObject(modplug.handle);
#endif
    }
    --modplug.loaded;
}


typedef struct
{
    int play_count;
    ModPlugFile *file;
    SDL_AudioStream *stream;
    void *buffer;
    int buffer_size;
} MODPLUG_Music;


static int MODPLUG_Seek(void *context, double position);
static void MODPLUG_Delete(void *context);

static int MODPLUG_Open(const SDL_AudioSpec *spec)
{
    /* ModPlug supports U8 or S16 audio output */
    modplug.ModPlug_GetSettings(&settings);
    settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING;
    if (spec->channels == 1) {
        settings.mChannels = 1;
    } else {
        settings.mChannels = 2;
    }
    if (SDL_AUDIO_BITSIZE(spec->format) == 8) {
        settings.mBits = 8;
    } else {
        settings.mBits = 16;
    }
    if (spec->freq >= 44100) {
        settings.mFrequency = 44100;
    } else if (spec->freq >= 22050) {
        settings.mFrequency = 22050;
    } else {
        settings.mFrequency = 11025;
    }
    settings.mResamplingMode = MODPLUG_RESAMPLE_FIR;
    settings.mReverbDepth = 0;
    settings.mReverbDelay = 100;
    settings.mBassAmount = 0;
    settings.mBassRange = 50;
    settings.mSurroundDepth = 0;
    settings.mSurroundDelay = 10;
    settings.mLoopCount = 0;
    modplug.ModPlug_SetSettings(&settings);
    return 0;
}

/* Load a modplug stream from an SDL_RWops object */
void *MODPLUG_CreateFromRW(SDL_RWops *src, int freesrc)
{
    MODPLUG_Music *music;
    void *buffer;
    size_t size;

    music = (MODPLUG_Music *)SDL_calloc(1, sizeof(*music));
    if (!music) {
        SDL_OutOfMemory();
        return NULL;
    }

    music->stream = SDL_NewAudioStream((settings.mBits == 8) ? AUDIO_U8 : AUDIO_S16SYS, settings.mChannels, settings.mFrequency,
                                       music_spec.format, music_spec.channels, music_spec.freq);
    if (!music->stream) {
        MODPLUG_Delete(music);
        return NULL;
    }

    music->buffer_size = music_spec.samples * (settings.mBits / 8) * settings.mChannels;
    music->buffer = SDL_malloc(music->buffer_size);
    if (!music->buffer) {
        MODPLUG_Delete(music);
        return NULL;
    }

    buffer = SDL_LoadFile_RW(src, &size, SDL_FALSE);
    if (buffer) {
        music->file = modplug.ModPlug_Load(buffer, (int)size);
        if (!music->file) {
            Mix_SetError("ModPlug_Load failed");
        }
        SDL_free(buffer);
    }

    if (!music->file) {
        MODPLUG_Delete(music);
        return NULL;
    }

    if (freesrc) {
        SDL_RWclose(src);
    }
    return music;
}

/* Set the volume for a modplug stream */
static void MODPLUG_SetVolume(void *context, int volume)
{
    MODPLUG_Music *music = (MODPLUG_Music *)context;
    modplug.ModPlug_SetMasterVolume(music->file, volume*4);
}

/* Start playback of a given modplug stream */
static int MODPLUG_Play(void *context, int play_count)
{
    MODPLUG_Music *music = (MODPLUG_Music *)context;
    music->play_count = play_count;
    return MODPLUG_Seek(music, 0.0);
}

/* Play some of a stream previously started with modplug_play() */
static int MODPLUG_GetSome(void *context, void *data, int bytes, SDL_bool *done)
{
    MODPLUG_Music *music = (MODPLUG_Music *)context;
    int filled, amount;

    filled = SDL_AudioStreamGet(music->stream, data, bytes);
    if (filled != 0) {
        return filled;
    }

    if (!music->play_count) {
        /* All done */
        *done = SDL_TRUE;
        return 0;
    }

    amount = modplug.ModPlug_Read(music->file, music->buffer, music->buffer_size);
    if (amount > 0) {
        if (SDL_AudioStreamPut(music->stream, music->buffer, amount) < 0) {
            return -1;
        }
    } else {
        if (music->play_count == 1) {
            music->play_count = 0;
            SDL_AudioStreamFlush(music->stream);
        } else {
            int play_count = -1;
            if (music->play_count > 0) {
                play_count = (music->play_count - 1);
            }
            if (MODPLUG_Play(music, play_count) < 0) {
                return -1;
            }
        }
    }
    return 0;
}
static int MODPLUG_GetAudio(void *context, void *data, int bytes)
{
    return music_pcm_getaudio(context, data, bytes, MIX_MAX_VOLUME, MODPLUG_GetSome);
}

/* Jump (seek) to a given position */
static int MODPLUG_Seek(void *context, double position)
{
    MODPLUG_Music *music = (MODPLUG_Music *)context;
    modplug.ModPlug_Seek(music->file, (int)(position*1000));
    return 0;
}

/* Close the given modplug stream */
static void MODPLUG_Delete(void *context)
{
    MODPLUG_Music *music = (MODPLUG_Music *)context;
    if (music->file) {
        modplug.ModPlug_Unload(music->file);
    }
    if (music->stream) {
        SDL_FreeAudioStream(music->stream);
    }
    if (music->buffer) {
        SDL_free(music->buffer);
    }
    SDL_free(music);
}

Mix_MusicInterface Mix_MusicInterface_MODPLUG =
{
    "MODPLUG",
    MIX_MUSIC_MODPLUG,
    MUS_MOD,
    SDL_FALSE,
    SDL_FALSE,

    MODPLUG_Load,
    MODPLUG_Open,
    MODPLUG_CreateFromRW,
    NULL,   /* CreateFromFile */
    MODPLUG_SetVolume,
    MODPLUG_Play,
    NULL,   /* IsPlaying */
    MODPLUG_GetAudio,
    MODPLUG_Seek,
    NULL,   /* Pause */
    NULL,   /* Resume */
    NULL,   /* Stop */
    MODPLUG_Delete,
    NULL,   /* Close */
    MODPLUG_Unload,
};

#endif /* MUSIC_MOD_MODPLUG */

/* vi: set ts=4 sw=4 expandtab: */
