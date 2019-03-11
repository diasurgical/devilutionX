/*
  SDL_mixer:    An audio mixer library based on the SDL library
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.    In no event will the authors be held liable for any damages
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

/* This file supports playing MP3 files with mpg123 */

#ifdef MUSIC_MP3_MPG123

#include <stdio.h>      // For SEEK_SET

#include "SDL_assert.h"
#include "SDL_loadso.h"

#include "music_mpg123.h"

#include <mpg123.h>


typedef struct {
    int loaded;
    void *handle;

    int (*mpg123_close)(mpg123_handle *mh);
    void (*mpg123_delete)(mpg123_handle *mh);
    void (*mpg123_exit)(void);
    int (*mpg123_format)( mpg123_handle *mh, long rate, int channels, int encodings );
    int (*mpg123_format_none)(mpg123_handle *mh);
    int (*mpg123_getformat)( mpg123_handle *mh, long *rate, int *channels, int *encoding );
    int (*mpg123_init)(void);
    mpg123_handle *(*mpg123_new)(const char* decoder, int *error);
    int (*mpg123_open_handle)(mpg123_handle *mh, void *iohandle);
    const char* (*mpg123_plain_strerror)(int errcode);
    void (*mpg123_rates)(const long **list, size_t *number);
    int (*mpg123_read)(mpg123_handle *mh, unsigned char *outmemory, size_t outmemsize, size_t *done );
    int (*mpg123_replace_reader_handle)( mpg123_handle *mh, ssize_t (*r_read) (void *, void *, size_t), off_t (*r_lseek)(void *, off_t, int), void (*cleanup)(void*) );
    off_t (*mpg123_seek)( mpg123_handle *mh, off_t sampleoff, int whence );
    const char* (*mpg123_strerror)(mpg123_handle *mh);
} mpg123_loader;

static mpg123_loader mpg123 = {
    0, NULL
};

#ifdef MPG123_DYNAMIC
#define FUNCTION_LOADER(FUNC, SIG) \
    mpg123.FUNC = (SIG) SDL_LoadFunction(mpg123.handle, #FUNC); \
    if (mpg123.FUNC == NULL) { SDL_UnloadObject(mpg123.handle); return -1; }
#else
#define FUNCTION_LOADER(FUNC, SIG) \
    mpg123.FUNC = FUNC;
#endif

static int MPG123_Load(void)
{
    if (mpg123.loaded == 0) {
#ifdef MPG123_DYNAMIC
        mpg123.handle = SDL_LoadObject(MPG123_DYNAMIC);
        if (mpg123.handle == NULL) {
            return -1;
        }
#elif defined(__MACOSX__)
        extern int mpg123_init(void) __attribute__((weak_import));
        if (mpg123_init == NULL)
        {
            /* Missing weakly linked framework */
            Mix_SetError("Missing mpg123.framework");
            return -1;
        }
#endif
        FUNCTION_LOADER(mpg123_close, int (*)(mpg123_handle *mh))
        FUNCTION_LOADER(mpg123_delete, void (*)(mpg123_handle *mh))
        FUNCTION_LOADER(mpg123_exit, void (*)(void))
        FUNCTION_LOADER(mpg123_format, int (*)( mpg123_handle *mh, long rate, int channels, int encodings ))
        FUNCTION_LOADER(mpg123_format_none, int (*)(mpg123_handle *mh))
        FUNCTION_LOADER(mpg123_getformat, int (*)( mpg123_handle *mh, long *rate, int *channels, int *encoding ))
        FUNCTION_LOADER(mpg123_init, int (*)(void))
        FUNCTION_LOADER(mpg123_new, mpg123_handle *(*)(const char* decoder, int *error))
        FUNCTION_LOADER(mpg123_open_handle, int (*)(mpg123_handle *mh, void *iohandle))
        FUNCTION_LOADER(mpg123_plain_strerror, const char* (*)(int errcode))
        FUNCTION_LOADER(mpg123_rates, void (*)(const long **list, size_t *number));
        FUNCTION_LOADER(mpg123_read, int (*)(mpg123_handle *mh, unsigned char *outmemory, size_t outmemsize, size_t *done ))
        FUNCTION_LOADER(mpg123_replace_reader_handle, int (*)( mpg123_handle *mh, ssize_t (*r_read) (void *, void *, size_t), off_t (*r_lseek)(void *, off_t, int), void (*cleanup)(void*) ))
        FUNCTION_LOADER(mpg123_seek, off_t (*)( mpg123_handle *mh, off_t sampleoff, int whence ))
        FUNCTION_LOADER(mpg123_strerror, const char* (*)(mpg123_handle *mh))
    }
    ++mpg123.loaded;

    return 0;
}

static void MPG123_Unload(void)
{
    if (mpg123.loaded == 0) {
        return;
    }
    if (mpg123.loaded == 1) {
#ifdef MPG123_DYNAMIC
        SDL_UnloadObject(mpg123.handle);
#endif
    }
    --mpg123.loaded;
}


typedef struct
{
    int play_count;
    SDL_RWops* src;
    int freesrc;
    int volume;

    mpg123_handle* handle;
    SDL_AudioStream *stream;
    unsigned char *buffer;
    size_t buffer_size;
} MPG123_Music;


static int MPG123_Seek(void *context, double secs);
static void MPG123_Delete(void *context);

static int mpg123_format_to_sdl(int fmt)
{
    switch (fmt)
    {
        case MPG123_ENC_SIGNED_8:       return AUDIO_S8;
        case MPG123_ENC_UNSIGNED_8:     return AUDIO_U8;
        case MPG123_ENC_SIGNED_16:      return AUDIO_S16SYS;
        case MPG123_ENC_UNSIGNED_16:    return AUDIO_U16SYS;
        case MPG123_ENC_SIGNED_32:      return AUDIO_S32SYS;
        case MPG123_ENC_FLOAT_32:       return AUDIO_F32SYS;
        default:                        return -1;
    }
}

/*
static const char *mpg123_format_str(int fmt)
{
    switch (fmt)
    {
#define f(x) case x: return #x;
        f(MPG123_ENC_UNSIGNED_8)
        f(MPG123_ENC_UNSIGNED_16)
        f(MPG123_ENC_SIGNED_8)
        f(MPG123_ENC_SIGNED_16)
        f(MPG123_ENC_SIGNED_32)
        f(MPG123_ENC_FLOAT_32)
#undef f
    }
    return "unknown";
}
*/

static char const* mpg_err(mpg123_handle* mpg, int result)
{
    char const* err = "unknown error";

    if (mpg && result == MPG123_ERR) {
        err = mpg123.mpg123_strerror(mpg);
    } else {
        err = mpg123.mpg123_plain_strerror(result);
    }
    return err;
}

/* we're gonna override mpg123's I/O with these wrappers for RWops */
static ssize_t rwops_read(void* p, void* dst, size_t n)
{
    return (ssize_t)SDL_RWread((SDL_RWops*)p, dst, 1, n);
}

static off_t rwops_seek(void* p, off_t offset, int whence)
{
    return (off_t)SDL_RWseek((SDL_RWops*)p, (Sint64)offset, whence);
}

static void rwops_cleanup(void* p)
{
    (void)p;
    /* do nothing, we will free the file later */
}


static int MPG123_Open(const SDL_AudioSpec *spec)
{
    if (mpg123.mpg123_init() != MPG123_OK) {
        Mix_SetError("mpg123_init() failed");
        return -1;
    }
    return 0;
}

static void *MPG123_CreateFromRW(SDL_RWops *src, int freesrc)
{
    MPG123_Music *music;
    int result;
    const long *rates;
    size_t i, num_rates;

    music = (MPG123_Music*)SDL_calloc(1, sizeof(*music));
    if (!music) {
        return NULL;
    }
    music->src = src;
    music->volume = MIX_MAX_VOLUME;

    /* Just assume 16-bit 2 channel audio for now */
    music->buffer_size = music_spec.samples * sizeof(Sint16) * 2;
    music->buffer = (unsigned char *)SDL_malloc(music->buffer_size);
    if (!music->buffer) {
        MPG123_Delete(music);
        SDL_OutOfMemory();
        return NULL;
    }

    music->handle = mpg123.mpg123_new(0, &result);
    if (result != MPG123_OK) {
        MPG123_Delete(music);
        Mix_SetError("mpg123_new failed");
        return NULL;
    }

    result = mpg123.mpg123_replace_reader_handle(
        music->handle,
        rwops_read, rwops_seek, rwops_cleanup
    );
    if (result != MPG123_OK) {
        MPG123_Delete(music);
        Mix_SetError("mpg123_replace_reader_handle: %s", mpg_err(music->handle, result));
        return NULL;
    }

    result = mpg123.mpg123_format_none(music->handle);
    if (result != MPG123_OK) {
        MPG123_Delete(music);
        Mix_SetError("mpg123_format_none: %s", mpg_err(music->handle, result));
        return NULL;
    }

    mpg123.mpg123_rates(&rates, &num_rates);
    for (i = 0; i < num_rates; ++i) {
        const int channels = (MPG123_MONO|MPG123_STEREO);
        const int formats = (MPG123_ENC_SIGNED_8 |
                             MPG123_ENC_UNSIGNED_8 |
                             MPG123_ENC_SIGNED_16 |
                             MPG123_ENC_UNSIGNED_16 |
                             MPG123_ENC_SIGNED_32 |
                             MPG123_ENC_FLOAT_32);

        mpg123.mpg123_format(music->handle, rates[i], channels, formats);
    }

    result = mpg123.mpg123_open_handle(music->handle, music->src);
    if (result != MPG123_OK) {
        MPG123_Delete(music);
        Mix_SetError("mpg123_open_handle: %s", mpg_err(music->handle, result));
        return NULL;
    }

    music->freesrc = freesrc;
    return music;
}

static void MPG123_SetVolume(void *context, int volume)
{
    MPG123_Music *music = (MPG123_Music *)context;
    music->volume = volume;
}

static int MPG123_Play(void *context, int play_count)
{
    MPG123_Music *music = (MPG123_Music *)context;
    music->play_count = play_count;
    return MPG123_Seek(music, 0.0);
}

/* read some mp3 stream data and convert it for output */
static int MPG123_GetSome(void *context, void *data, int bytes, SDL_bool *done)
{
    MPG123_Music *music = (MPG123_Music *)context;
    int filled, result;
    size_t amount;
    long rate;
    int channels, encoding, format;

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

    result = mpg123.mpg123_read(music->handle, music->buffer, music->buffer_size, &amount);
    switch (result) {
    case MPG123_OK:
        if (SDL_AudioStreamPut(music->stream, music->buffer, (int)amount) < 0) {
            return -1;
        }
        break;

    case MPG123_NEW_FORMAT:
        result = mpg123.mpg123_getformat(music->handle, &rate, &channels, &encoding);
        if (result != MPG123_OK) {
            Mix_SetError("mpg123_getformat: %s", mpg_err(music->handle, result));
            return -1;
        }
/*printf("MPG123 format: %s, channels = %d, rate = %ld\n", mpg123_format_str(encoding), channels, rate);*/

        format = mpg123_format_to_sdl(encoding);
        SDL_assert(format != -1);

        music->stream = SDL_NewAudioStream(format, channels, (int)rate,
                                           music_spec.format, music_spec.channels, music_spec.freq);
        if (!music->stream) {
            return -1;
        }
        break;

    case MPG123_DONE:
        if (music->play_count == 1) {
            music->play_count = 0;
            SDL_AudioStreamFlush(music->stream);
        } else {
            int play_count = -1;
            if (music->play_count > 0) {
                play_count = (music->play_count - 1);
            }
            if (MPG123_Play(music, play_count) < 0) {
                return -1;
            }
        }
        break;
    default:
        Mix_SetError("mpg123_read: %s", mpg_err(music->handle, result));
        return -1;
    }
    return 0;
}
static int MPG123_GetAudio(void *context, void *data, int bytes)
{
    MPG123_Music *music = (MPG123_Music *)context;
    return music_pcm_getaudio(context, data, bytes, music->volume, MPG123_GetSome);
}

static int MPG123_Seek(void *context, double secs)
{
    MPG123_Music *music = (MPG123_Music *)context;
    off_t offset = (off_t)(music_spec.freq * secs);

    if ((offset = mpg123.mpg123_seek(music->handle, offset, SEEK_SET)) < 0) {
        return Mix_SetError("mpg123_seek: %s", mpg_err(music->handle, (int)-offset));
    }
    return 0;
}

static void MPG123_Delete(void *context)
{
    MPG123_Music *music = (MPG123_Music *)context;

    if (music->handle) {
        mpg123.mpg123_close(music->handle);
        mpg123.mpg123_delete(music->handle);
    }
    if (music->stream) {
        SDL_FreeAudioStream(music->stream);
    }
    if (music->buffer) {
        SDL_free(music->buffer);
    }
    if (music->freesrc) {
        SDL_RWclose(music->src);
    }
    SDL_free(music);
}

static void MPG123_Close(void)
{
    mpg123.mpg123_exit();
}

Mix_MusicInterface Mix_MusicInterface_MPG123 =
{
    "MPG123",
    MIX_MUSIC_MPG123,
    MUS_MP3,
    SDL_FALSE,
    SDL_FALSE,

    MPG123_Load,
    MPG123_Open,
    MPG123_CreateFromRW,
    NULL,   /* CreateFromFile */
    MPG123_SetVolume,
    MPG123_Play,
    NULL,   /* IsPlaying */
    MPG123_GetAudio,
    MPG123_Seek,
    NULL,   /* Pause */
    NULL,   /* Resume */
    NULL,   /* Stop */
    MPG123_Delete,
    MPG123_Close,
    MPG123_Unload
};

#endif /* MUSIC_MP3_MPG123 */

/* vi: set ts=4 sw=4 expandtab: */
