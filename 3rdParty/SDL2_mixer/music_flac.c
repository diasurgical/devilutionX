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

  This file is used to support SDL_LoadMUS playback of FLAC files.
    ~ Austen Dicken (admin@cvpcs.org)
*/

#ifdef MUSIC_FLAC

#include "SDL_assert.h"
#include "SDL_loadso.h"

#include "music_flac.h"

#include <FLAC/stream_decoder.h>


typedef struct {
    int loaded;
    void *handle;
    FLAC__StreamDecoder *(*FLAC__stream_decoder_new)(void);
    void (*FLAC__stream_decoder_delete)(FLAC__StreamDecoder *decoder);
    FLAC__StreamDecoderInitStatus (*FLAC__stream_decoder_init_stream)(
                        FLAC__StreamDecoder *decoder,
                        FLAC__StreamDecoderReadCallback read_callback,
                        FLAC__StreamDecoderSeekCallback seek_callback,
                        FLAC__StreamDecoderTellCallback tell_callback,
                        FLAC__StreamDecoderLengthCallback length_callback,
                        FLAC__StreamDecoderEofCallback eof_callback,
                        FLAC__StreamDecoderWriteCallback write_callback,
                        FLAC__StreamDecoderMetadataCallback metadata_callback,
                        FLAC__StreamDecoderErrorCallback error_callback,
                        void *client_data);
    FLAC__bool (*FLAC__stream_decoder_finish)(FLAC__StreamDecoder *decoder);
    FLAC__bool (*FLAC__stream_decoder_flush)(FLAC__StreamDecoder *decoder);
    FLAC__bool (*FLAC__stream_decoder_process_single)(
                        FLAC__StreamDecoder *decoder);
    FLAC__bool (*FLAC__stream_decoder_process_until_end_of_metadata)(
                        FLAC__StreamDecoder *decoder);
    FLAC__bool (*FLAC__stream_decoder_process_until_end_of_stream)(
                        FLAC__StreamDecoder *decoder);
    FLAC__bool (*FLAC__stream_decoder_seek_absolute)(
                        FLAC__StreamDecoder *decoder,
                        FLAC__uint64 sample);
    FLAC__StreamDecoderState (*FLAC__stream_decoder_get_state)(
                        const FLAC__StreamDecoder *decoder);
} flac_loader;

static flac_loader flac = {
    0, NULL
};

#ifdef FLAC_DYNAMIC
#define FUNCTION_LOADER(FUNC, SIG) \
    flac.FUNC = (SIG) SDL_LoadFunction(flac.handle, #FUNC); \
    if (flac.FUNC == NULL) { SDL_UnloadObject(flac.handle); return -1; }
#else
#define FUNCTION_LOADER(FUNC, SIG) \
    flac.FUNC = FUNC;
#endif

static int FLAC_Load(void)
{
    if (flac.loaded == 0) {
#ifdef FLAC_DYNAMIC
        flac.handle = SDL_LoadObject(FLAC_DYNAMIC);
        if (flac.handle == NULL) {
            return -1;
        }
#elif defined(__MACOSX__)
        extern FLAC__StreamDecoder *FLAC__stream_decoder_new(void) __attribute__((weak_import));
        if (FLAC__stream_decoder_new == NULL)
        {
            /* Missing weakly linked framework */
            Mix_SetError("Missing FLAC.framework");
            return -1;
        }
#endif

        FUNCTION_LOADER(FLAC__stream_decoder_new, FLAC__StreamDecoder *(*)(void))
        FUNCTION_LOADER(FLAC__stream_decoder_delete, void (*)(FLAC__StreamDecoder *))
        FUNCTION_LOADER(FLAC__stream_decoder_init_stream, FLAC__StreamDecoderInitStatus (*)(
                        FLAC__StreamDecoder *,
                        FLAC__StreamDecoderReadCallback,
                        FLAC__StreamDecoderSeekCallback,
                        FLAC__StreamDecoderTellCallback,
                        FLAC__StreamDecoderLengthCallback,
                        FLAC__StreamDecoderEofCallback,
                        FLAC__StreamDecoderWriteCallback,
                        FLAC__StreamDecoderMetadataCallback,
                        FLAC__StreamDecoderErrorCallback,
                        void *))
        FUNCTION_LOADER(FLAC__stream_decoder_finish, FLAC__bool (*)(FLAC__StreamDecoder *))
        FUNCTION_LOADER(FLAC__stream_decoder_flush, FLAC__bool (*)(FLAC__StreamDecoder *))
        FUNCTION_LOADER(FLAC__stream_decoder_process_single, FLAC__bool (*)(FLAC__StreamDecoder *))
        FUNCTION_LOADER(FLAC__stream_decoder_process_until_end_of_metadata, FLAC__bool (*)(FLAC__StreamDecoder *))
        FUNCTION_LOADER(FLAC__stream_decoder_process_until_end_of_stream, FLAC__bool (*)(FLAC__StreamDecoder *))
        FUNCTION_LOADER(FLAC__stream_decoder_seek_absolute, FLAC__bool (*)(FLAC__StreamDecoder *, FLAC__uint64))
        FUNCTION_LOADER(FLAC__stream_decoder_get_state, FLAC__StreamDecoderState (*)(const FLAC__StreamDecoder *decoder))
    }
    ++flac.loaded;

    return 0;
}

static void FLAC_Unload(void)
{
    if (flac.loaded == 0) {
        return;
    }
    if (flac.loaded == 1) {
#ifdef FLAC_DYNAMIC
        SDL_UnloadObject(flac.handle);
#endif
    }
    --flac.loaded;
}


typedef struct {
    int volume;
    int play_count;
    FLAC__StreamDecoder *flac_decoder;
    unsigned sample_rate;
    unsigned channels;
    unsigned bits_per_sample;
    SDL_RWops *src;
    int freesrc;
    SDL_AudioStream *stream;
} FLAC_Music;


static int FLAC_Seek(void *context, double position);

static FLAC__StreamDecoderReadStatus flac_read_music_cb(
                                    const FLAC__StreamDecoder *decoder,
                                    FLAC__byte buffer[],
                                    size_t *bytes,
                                    void *client_data)
{
    FLAC_Music *data = (FLAC_Music*)client_data;

    /* make sure there is something to be reading */
    if (*bytes > 0) {
        *bytes = SDL_RWread (data->src, buffer, sizeof (FLAC__byte), *bytes);

        if (*bytes == 0) { /* error or no data was read (EOF) */
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
        } else { /* data was read, continue */
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        }
    } else {
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }
}

static FLAC__StreamDecoderSeekStatus flac_seek_music_cb(
                                    const FLAC__StreamDecoder *decoder,
                                    FLAC__uint64 absolute_byte_offset,
                                    void *client_data)
{
    FLAC_Music *data = (FLAC_Music*)client_data;

    if (SDL_RWseek(data->src, absolute_byte_offset, RW_SEEK_SET) < 0) {
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    } else {
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
    }
}

static FLAC__StreamDecoderTellStatus flac_tell_music_cb(
                                    const FLAC__StreamDecoder *decoder,
                                    FLAC__uint64 *absolute_byte_offset,
                                    void *client_data)
{
    FLAC_Music *data = (FLAC_Music*)client_data;

    Sint64 pos = SDL_RWtell(data->src);

    if (pos < 0) {
        return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
    } else {
        *absolute_byte_offset = (FLAC__uint64)pos;
        return FLAC__STREAM_DECODER_TELL_STATUS_OK;
    }
}

static FLAC__StreamDecoderLengthStatus flac_length_music_cb(
                                    const FLAC__StreamDecoder *decoder,
                                    FLAC__uint64 *stream_length,
                                    void *client_data)
{
    FLAC_Music *data = (FLAC_Music*)client_data;

    Sint64 pos = SDL_RWtell(data->src);
    Sint64 length = SDL_RWseek(data->src, 0, RW_SEEK_END);

    if (SDL_RWseek(data->src, pos, RW_SEEK_SET) != pos || length < 0) {
        /* there was an error attempting to return the stream to the original
         * position, or the length was invalid. */
        return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
    } else {
        *stream_length = (FLAC__uint64)length;
        return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
    }
}

static FLAC__bool flac_eof_music_cb(
                                const FLAC__StreamDecoder *decoder,
                                void *client_data)
{
    FLAC_Music *data = (FLAC_Music*)client_data;

    Sint64 pos = SDL_RWtell(data->src);
    Sint64 end = SDL_RWseek(data->src, 0, RW_SEEK_END);

    /* was the original position equal to the end (a.k.a. the seek didn't move)? */
    if (pos == end) {
        /* must be EOF */
        return true;
    } else {
        /* not EOF, return to the original position */
        SDL_RWseek(data->src, pos, RW_SEEK_SET);
        return false;
    }
}

static FLAC__StreamDecoderWriteStatus flac_write_music_cb(
                                    const FLAC__StreamDecoder *decoder,
                                    const FLAC__Frame *frame,
                                    const FLAC__int32 *const buffer[],
                                    void *client_data)
{
    FLAC_Music *music = (FLAC_Music *)client_data;
    Sint16 *data;
    unsigned int i, j, channels;
    int shift_amount = 0;

    if (!music->stream) {
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    switch (music->bits_per_sample) {
    case 16:
        shift_amount = 0;
        break;
    case 20:
        shift_amount = 4;
        break;
    case 24:
        shift_amount = 8;
        break;
    default:
        SDL_SetError("FLAC decoder doesn't support %d bits_per_sample", music->bits_per_sample);
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    if (music->channels == 3) {
        /* We'll just drop the center channel for now */
        channels = 2;
    } else {
        channels = music->channels;
    }

    data = SDL_stack_alloc(Sint16, (frame->header.blocksize * channels));
    if (!data) {
        SDL_SetError("Couldn't allocate %d bytes stack memory", (int)(frame->header.blocksize * channels * sizeof(*data)));
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }
    if (music->channels == 3) {
        Sint16 *dst = data;
        for (i = 0; i < frame->header.blocksize; ++i) {
            Sint16 FL = (buffer[0][i] >> shift_amount);
            Sint16 FR = (buffer[1][i] >> shift_amount);
            Sint16 FCmix = (Sint16)((buffer[2][i] >> shift_amount) * 0.5f);
            int sample;

            sample = (FL + FCmix);
            if (sample > SDL_MAX_SINT16) {
                *dst = SDL_MAX_SINT16;
            } else if (sample < SDL_MIN_SINT16) {
                *dst = SDL_MIN_SINT16;
            } else {
                *dst = sample;
            }
            ++dst;

            sample = (FR + FCmix);
            if (sample > SDL_MAX_SINT16) {
                *dst = SDL_MAX_SINT16;
            } else if (sample < SDL_MIN_SINT16) {
                *dst = SDL_MIN_SINT16;
            } else {
                *dst = sample;
            }
            ++dst;
        }
    } else {
        for (i = 0; i < channels; ++i) {
            Sint16 *dst = data + i;
            for (j = 0; j < frame->header.blocksize; ++j) {
                *dst = (buffer[i][j] >> shift_amount);
                dst += channels;
            }
        }
    }
    SDL_AudioStreamPut(music->stream, data, (frame->header.blocksize * channels * sizeof(*data)));
    SDL_stack_free(data);

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void flac_metadata_music_cb(
                    const FLAC__StreamDecoder *decoder,
                    const FLAC__StreamMetadata *metadata,
                    void *client_data)
{
    FLAC_Music *music = (FLAC_Music *)client_data;
    int channels;

    if (metadata->type != FLAC__METADATA_TYPE_STREAMINFO) {
        return;
    }

    music->sample_rate = metadata->data.stream_info.sample_rate;
    music->channels = metadata->data.stream_info.channels;
    music->bits_per_sample = metadata->data.stream_info.bits_per_sample;
/*printf("FLAC: Sample rate = %d, channels = %d, bits_per_sample = %d\n", music->sample_rate, music->channels, music->bits_per_sample);*/

    /* SDL's channel mapping and FLAC channel mapping are the same,
       except for 3 channels: SDL is FL FR LFE and FLAC is FL FR FC
     */
    if (music->channels == 3) {
        channels = 2;
    } else {
        channels = music->channels;
    }
    /* We check for NULL stream later when we get data */
    SDL_assert(!music->stream);
    music->stream = SDL_NewAudioStream(AUDIO_S16SYS, channels, music->sample_rate,
                                      music_spec.format, music_spec.channels, music_spec.freq);
}

static void flac_error_music_cb(
                const FLAC__StreamDecoder *decoder,
                FLAC__StreamDecoderErrorStatus status,
                void *client_data)
{
    /* print an SDL error based on the error status */
    switch (status) {
    case FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC:
        SDL_SetError("Error processing the FLAC file [LOST_SYNC].");
        break;
    case FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER:
        SDL_SetError("Error processing the FLAC file [BAD_HEADER].");
        break;
    case FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH:
        SDL_SetError("Error processing the FLAC file [CRC_MISMATCH].");
        break;
    case FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM:
        SDL_SetError("Error processing the FLAC file [UNPARSEABLE].");
        break;
    default:
        SDL_SetError("Error processing the FLAC file [UNKNOWN].");
        break;
    }
}

/* Load an FLAC stream from an SDL_RWops object */
static void *FLAC_CreateFromRW(SDL_RWops *src, int freesrc)
{
    FLAC_Music *music;
    int init_stage = 0;
    int was_error = 1;

    music = (FLAC_Music *)SDL_calloc(1, sizeof(*music));
    if (!music) {
        SDL_OutOfMemory();
        return NULL;
    }
    music->src = src;
    music->volume = MIX_MAX_VOLUME;

    music->flac_decoder = flac.FLAC__stream_decoder_new();
    if (music->flac_decoder) {
        init_stage++; /* stage 1! */

        if (flac.FLAC__stream_decoder_init_stream(
                    music->flac_decoder,
                    flac_read_music_cb, flac_seek_music_cb,
                    flac_tell_music_cb, flac_length_music_cb,
                    flac_eof_music_cb, flac_write_music_cb,
                    flac_metadata_music_cb, flac_error_music_cb,
                    music) == FLAC__STREAM_DECODER_INIT_STATUS_OK) {
            init_stage++; /* stage 2! */

            if (flac.FLAC__stream_decoder_process_until_end_of_metadata(music->flac_decoder)) {
                was_error = 0;
            } else {
                SDL_SetError("FLAC__stream_decoder_process_until_end_of_metadata() failed");
            }
        } else {
            SDL_SetError("FLAC__stream_decoder_init_stream() failed");
        }
    } else {
        SDL_SetError("FLAC__stream_decoder_new() failed");
    }

    if (was_error) {
        switch (init_stage) {
            case 2:
                flac.FLAC__stream_decoder_finish(music->flac_decoder);
            case 1:
                flac.FLAC__stream_decoder_delete(music->flac_decoder);
            case 0:
                SDL_free(music);
                break;
        }
        return NULL;
    }

    music->freesrc = freesrc;
    return music;
}

/* Set the volume for an FLAC stream */
static void FLAC_SetVolume(void *context, int volume)
{
    FLAC_Music *music = (FLAC_Music *)context;
    music->volume = volume;
}

/* Start playback of a given FLAC stream */
static int FLAC_Play(void *context, int play_count)
{
    FLAC_Music *music = (FLAC_Music *)context;
    music->play_count = play_count;
    return FLAC_Seek(music, 0.0);
}

/* Read some FLAC stream data and convert it for output */
static int FLAC_GetSome(void *context, void *data, int bytes, SDL_bool *done)
{
    FLAC_Music *music = (FLAC_Music *)context;
    int filled;

    filled = SDL_AudioStreamGet(music->stream, data, bytes);
    if (filled != 0) {
        return filled;
    }

    if (!music->play_count) {
        /* All done */
        *done = SDL_TRUE;
        return 0;
    }

    if (!flac.FLAC__stream_decoder_process_single(music->flac_decoder)) {
        SDL_SetError("FLAC__stream_decoder_process_single() failed");
        return -1;
    }

    if (flac.FLAC__stream_decoder_get_state(music->flac_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM) {
        if (music->play_count == 1) {
            music->play_count = 0;
            SDL_AudioStreamFlush(music->stream);
        } else {
            int play_count = -1;
            if (music->play_count > 0) {
                play_count = (music->play_count - 1);
            }
            if (FLAC_Play(music, play_count) < 0) {
                return -1;
            }
        }
    }
    return 0;
}

/* Play some of a stream previously started with FLAC_play() */
static int FLAC_GetAudio(void *context, void *data, int bytes)
{
    FLAC_Music *music = (FLAC_Music *)context;
    return music_pcm_getaudio(context, data, bytes, music->volume, FLAC_GetSome);
}

/* Jump (seek) to a given position (position is in seconds) */
static int FLAC_Seek(void *context, double position)
{
    FLAC_Music *music = (FLAC_Music *)context;
    double seek_sample = music->sample_rate * position;

    if (!flac.FLAC__stream_decoder_seek_absolute(music->flac_decoder, (FLAC__uint64)seek_sample)) {
        if (flac.FLAC__stream_decoder_get_state(music->flac_decoder) == FLAC__STREAM_DECODER_SEEK_ERROR) {
            flac.FLAC__stream_decoder_flush(music->flac_decoder);
        }

        SDL_SetError("Seeking of FLAC stream failed: libFLAC seek failed.");
        return -1;
    }
    return 0;
}

/* Close the given FLAC_Music object */
static void FLAC_Delete(void *context)
{
    FLAC_Music *music = (FLAC_Music *)context;
    if (music) {
        if (music->flac_decoder) {
            flac.FLAC__stream_decoder_finish(music->flac_decoder);
            flac.FLAC__stream_decoder_delete(music->flac_decoder);
        }
        if (music->stream) {
            SDL_FreeAudioStream(music->stream);
        }
        if (music->freesrc) {
            SDL_RWclose(music->src);
        }
        SDL_free(music);
    }
}

Mix_MusicInterface Mix_MusicInterface_FLAC =
{
    "FLAC",
    MIX_MUSIC_FLAC,
    MUS_FLAC,
    SDL_FALSE,
    SDL_FALSE,

    FLAC_Load,
    NULL,   /* Open */
    FLAC_CreateFromRW,
    NULL,   /* CreateFromFile */
    FLAC_SetVolume,
    FLAC_Play,
    NULL,   /* IsPlaying */
    FLAC_GetAudio,
    FLAC_Seek,
    NULL,   /* Pause */
    NULL,   /* Resume */
    NULL,   /* Stop */
    FLAC_Delete,
    NULL,   /* Close */
    FLAC_Unload,
};

#endif /* MUSIC_FLAC */

/* vi: set ts=4 sw=4 expandtab: */
