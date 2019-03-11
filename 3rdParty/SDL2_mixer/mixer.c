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

/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "SDL_mixer.h"
#include "mixer.h"
#include "music.h"
#include "load_aiff.h"
#include "load_voc.h"

#define __MIX_INTERNAL_EFFECT__
#include "effects_internal.h"

/* Magic numbers for various audio file formats */
#define RIFF        0x46464952      /* "RIFF" */
#define WAVE        0x45564157      /* "WAVE" */
#define FORM        0x4d524f46      /* "FORM" */
#define CREA        0x61657243      /* "Crea" */

static int audio_opened = 0;
static SDL_AudioSpec mixer;
static SDL_AudioDeviceID audio_device;

typedef struct _Mix_effectinfo
{
    Mix_EffectFunc_t callback;
    Mix_EffectDone_t done_callback;
    void *udata;
    struct _Mix_effectinfo *next;
} effect_info;

static struct _Mix_Channel {
    Mix_Chunk *chunk;
    int playing;
    int paused;
    Uint8 *samples;
    int volume;
    int looping;
    int tag;
    Uint32 expire;
    Uint32 start_time;
    Mix_Fading fading;
    int fade_volume;
    int fade_volume_reset;
    Uint32 fade_length;
    Uint32 ticks_fade;
    effect_info *effects;
} *mix_channel = NULL;

static effect_info *posteffects = NULL;

static int num_channels;
static int reserved_channels = 0;


/* Support for hooking into the mixer callback system */
static void (SDLCALL *mix_postmix)(void *udata, Uint8 *stream, int len) = NULL;
static void *mix_postmix_data = NULL;

/* rcg07062001 callback to alert when channels are done playing. */
static void (SDLCALL *channel_done_callback)(int channel) = NULL;

/* Support for user defined music functions */
static void (SDLCALL *mix_music)(void *udata, Uint8 *stream, int len) = music_mixer;
static void *music_data = NULL;

/* rcg06042009 report available decoders at runtime. */
static const char **chunk_decoders = NULL;
static int num_decoders = 0;


int Mix_GetNumChunkDecoders(void)
{
    return(num_decoders);
}

const char *Mix_GetChunkDecoder(int index)
{
    if ((index < 0) || (index >= num_decoders)) {
        return NULL;
    }
    return(chunk_decoders[index]);
}

SDL_bool Mix_HasChunkDecoder(const char *name)
{
    int index;
    for (index = 0; index < num_decoders; ++index) {
        if (SDL_strcasecmp(name, chunk_decoders[index]) == 0) {
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}

void add_chunk_decoder(const char *decoder)
{
    int i;
    void *ptr;

    /* Check to see if we already have this decoder */
    for (i = 0; i < num_decoders; ++i) {
        if (SDL_strcmp(chunk_decoders[i], decoder) == 0) {
            return;
        }
    }

    ptr = SDL_realloc((void *)chunk_decoders, (num_decoders + 1) * sizeof (const char *));
    if (ptr == NULL) {
        return;  /* oh well, go on without it. */
    }
    chunk_decoders = (const char **) ptr;
    chunk_decoders[num_decoders++] = decoder;
}

/* rcg06192001 get linked library's version. */
const SDL_version *Mix_Linked_Version(void)
{
    static SDL_version linked_version;
    SDL_MIXER_VERSION(&linked_version);
    return(&linked_version);
}

int Mix_Init(int flags)
{
    int result = 0;

    if (flags & MIX_INIT_FLAC) {
        if (load_music_type(MUS_FLAC)) {
            open_music_type(MUS_FLAC);
            result |= MIX_INIT_FLAC;
        } else {
            Mix_SetError("FLAC support not available");
        }
    }
    if (flags & MIX_INIT_MOD) {
        if (load_music_type(MUS_MOD)) {
            open_music_type(MUS_MOD);
            result |= MIX_INIT_MOD;
        } else {
            Mix_SetError("MOD support not available");
        }
    }
    if (flags & MIX_INIT_MP3) {
        if (load_music_type(MUS_MP3)) {
            open_music_type(MUS_MP3);
            result |= MIX_INIT_MP3;
        } else {
            Mix_SetError("MP3 support not available");
        }
    }
    if (flags & MIX_INIT_OGG) {
        if (load_music_type(MUS_OGG)) {
            open_music_type(MUS_OGG);
            result |= MIX_INIT_OGG;
        } else {
            Mix_SetError("OGG support not available");
        }
    }
    if (flags & MIX_INIT_OPUS) {
        if (load_music_type(MUS_OPUS)) {
            open_music_type(MUS_OPUS);
            result |= MIX_INIT_OPUS;
        } else {
            Mix_SetError("OPUS support not available");
        }
    }
    if (flags & MIX_INIT_MID) {
        if (load_music_type(MUS_MID)) {
            open_music_type(MUS_MID);
            result |= MIX_INIT_MID;
        } else {
            Mix_SetError("MIDI support not available");
        }
    }
    return result;
}

void Mix_Quit()
{
    unload_music();
}

static int _Mix_remove_all_effects(int channel, effect_info **e);

/*
 * rcg06122001 Cleanup effect callbacks.
 *  MAKE SURE Mix_LockAudio() is called before this (or you're in the
 *   audio callback).
 */
static void _Mix_channel_done_playing(int channel)
{
    if (channel_done_callback) {
        channel_done_callback(channel);
    }

    /*
     * Call internal function directly, to avoid locking audio from
     *   inside audio callback.
     */
    _Mix_remove_all_effects(channel, &mix_channel[channel].effects);
}


static void *Mix_DoEffects(int chan, void *snd, int len)
{
    int posteffect = (chan == MIX_CHANNEL_POST);
    effect_info *e = ((posteffect) ? posteffects : mix_channel[chan].effects);
    void *buf = snd;

    if (e != NULL) {    /* are there any registered effects? */
        /* if this is the postmix, we can just overwrite the original. */
        if (!posteffect) {
            buf = SDL_malloc(len);
            if (buf == NULL) {
                return(snd);
            }
            SDL_memcpy(buf, snd, len);
        }

        for (; e != NULL; e = e->next) {
            if (e->callback != NULL) {
                e->callback(chan, buf, len, e->udata);
            }
        }
    }

    /* be sure to SDL_free() the return value if != snd ... */
    return(buf);
}


/* Mixing function */
static void SDLCALL
mix_channels(void *udata, Uint8 *stream, int len)
{
    Uint8 *mix_input;
    int i, mixable, volume = MIX_MAX_VOLUME;
    Uint32 sdl_ticks;

#if SDL_VERSION_ATLEAST(1, 3, 0)
    /* Need to initialize the stream in SDL 1.3+ */
    SDL_memset(stream, mixer.silence, len);
#endif

    /* Mix the music (must be done before the channels are added) */
    mix_music(music_data, stream, len);

    /* Mix any playing channels... */
    sdl_ticks = SDL_GetTicks();
    for (i=0; i<num_channels; ++i) {
        if (!mix_channel[i].paused) {
            if (mix_channel[i].expire > 0 && mix_channel[i].expire < sdl_ticks) {
                /* Expiration delay for that channel is reached */
                mix_channel[i].playing = 0;
                mix_channel[i].looping = 0;
                mix_channel[i].fading = MIX_NO_FADING;
                mix_channel[i].expire = 0;
                _Mix_channel_done_playing(i);
            } else if (mix_channel[i].fading != MIX_NO_FADING) {
                Uint32 ticks = sdl_ticks - mix_channel[i].ticks_fade;
                if (ticks >= mix_channel[i].fade_length) {
                    Mix_Volume(i, mix_channel[i].fade_volume_reset); /* Restore the volume */
                    if(mix_channel[i].fading == MIX_FADING_OUT) {
                        mix_channel[i].playing = 0;
                        mix_channel[i].looping = 0;
                        mix_channel[i].expire = 0;
                        _Mix_channel_done_playing(i);
                    }
                    mix_channel[i].fading = MIX_NO_FADING;
                } else {
                    if (mix_channel[i].fading == MIX_FADING_OUT) {
                        Mix_Volume(i, (mix_channel[i].fade_volume * (mix_channel[i].fade_length-ticks))
                                   / mix_channel[i].fade_length);
                    } else {
                        Mix_Volume(i, (mix_channel[i].fade_volume * ticks) / mix_channel[i].fade_length);
                    }
                }
            }
            if (mix_channel[i].playing > 0) {
                int index = 0;
                int remaining = len;
                while (mix_channel[i].playing > 0 && index < len) {
                    remaining = len - index;
                    volume = (mix_channel[i].volume*mix_channel[i].chunk->volume) / MIX_MAX_VOLUME;
                    mixable = mix_channel[i].playing;
                    if (mixable > remaining) {
                        mixable = remaining;
                    }

                    mix_input = Mix_DoEffects(i, mix_channel[i].samples, mixable);
                    SDL_MixAudioFormat(stream+index,mix_input,mixer.format,mixable,volume);
                    if (mix_input != mix_channel[i].samples)
                        SDL_free(mix_input);

                    mix_channel[i].samples += mixable;
                    mix_channel[i].playing -= mixable;
                    index += mixable;

                    /* rcg06072001 Alert app if channel is done playing. */
                    if (!mix_channel[i].playing && !mix_channel[i].looping) {
                        _Mix_channel_done_playing(i);
                    }
                }

                /* If looping the sample and we are at its end, make sure
                   we will still return a full buffer */
                while (mix_channel[i].looping && index < len) {
                    int alen = mix_channel[i].chunk->alen;
                    remaining = len - index;
                    if (remaining > alen) {
                        remaining = alen;
                    }

                    mix_input = Mix_DoEffects(i, mix_channel[i].chunk->abuf, remaining);
                    SDL_MixAudioFormat(stream+index, mix_input, mixer.format, remaining, volume);
                    if (mix_input != mix_channel[i].chunk->abuf)
                        SDL_free(mix_input);

                    if (mix_channel[i].looping > 0) {
                        --mix_channel[i].looping;
                    }
                    mix_channel[i].samples = mix_channel[i].chunk->abuf + remaining;
                    mix_channel[i].playing = mix_channel[i].chunk->alen - remaining;
                    index += remaining;
                }
                if (! mix_channel[i].playing && mix_channel[i].looping) {
                    if (mix_channel[i].looping > 0) {
                        --mix_channel[i].looping;
                    }
                    mix_channel[i].samples = mix_channel[i].chunk->abuf;
                    mix_channel[i].playing = mix_channel[i].chunk->alen;
                }
            }
        }
    }

    /* rcg06122001 run posteffects... */
    Mix_DoEffects(MIX_CHANNEL_POST, stream, len);

    if (mix_postmix) {
        mix_postmix(mix_postmix_data, stream, len);
    }
}

#if 0
static void PrintFormat(char *title, SDL_AudioSpec *fmt)
{
    printf("%s: %d bit %s audio (%s) at %u Hz\n", title, (fmt->format&0xFF),
            (fmt->format&0x8000) ? "signed" : "unsigned",
            (fmt->channels > 2) ? "surround" :
            (fmt->channels > 1) ? "stereo" : "mono", fmt->freq);
}
#endif

/* Open the mixer with a certain desired audio format */
int Mix_OpenAudioDevice(int frequency, Uint16 format, int nchannels, int chunksize,
                        const char* device, int allowed_changes)
{
    int i;
    SDL_AudioSpec desired;

    /* This used to call SDL_OpenAudio(), which initializes the audio
       subsystem if necessary. Since SDL_OpenAudioDevice() doesn't,
       we have to handle this case here. */
    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            return -1;
        }
    }

    /* If the mixer is already opened, increment open count */
    if (audio_opened) {
        if (format == mixer.format && nchannels == mixer.channels) {
            ++audio_opened;
            return(0);
        }
        while (audio_opened) {
            Mix_CloseAudio();
        }
    }

    /* Set the desired format and frequency */
    desired.freq = frequency;
    desired.format = format;
    desired.channels = nchannels;
    desired.samples = chunksize;
    desired.callback = mix_channels;
    desired.userdata = NULL;

    /* Accept nearly any audio format */
    if ((audio_device = SDL_OpenAudioDevice(device, 0, &desired, &mixer, allowed_changes)) == 0) {
        return(-1);
    }
#if 0
    PrintFormat("Audio device", &mixer);
#endif

    num_channels = MIX_CHANNELS;
    mix_channel = (struct _Mix_Channel *) SDL_malloc(num_channels * sizeof(struct _Mix_Channel));

    /* Clear out the audio channels */
    for (i=0; i<num_channels; ++i) {
        mix_channel[i].chunk = NULL;
        mix_channel[i].playing = 0;
        mix_channel[i].looping = 0;
        mix_channel[i].volume = SDL_MIX_MAXVOLUME;
        mix_channel[i].fade_volume = SDL_MIX_MAXVOLUME;
        mix_channel[i].fade_volume_reset = SDL_MIX_MAXVOLUME;
        mix_channel[i].fading = MIX_NO_FADING;
        mix_channel[i].tag = -1;
        mix_channel[i].expire = 0;
        mix_channel[i].effects = NULL;
        mix_channel[i].paused = 0;
    }
    Mix_VolumeMusic(SDL_MIX_MAXVOLUME);

    _Mix_InitEffects();

    add_chunk_decoder("WAVE");
    add_chunk_decoder("AIFF");
    add_chunk_decoder("VOC");

    /* Initialize the music players */
    open_music(&mixer);

    audio_opened = 1;
    SDL_PauseAudioDevice(audio_device, 0);
    return(0);
}

/* Open the mixer with a certain desired audio format */
int Mix_OpenAudio(int frequency, Uint16 format, int nchannels, int chunksize)
{
    return Mix_OpenAudioDevice(frequency, format, nchannels, chunksize, NULL,
                                SDL_AUDIO_ALLOW_FREQUENCY_CHANGE |
                                SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
}

/* Dynamically change the number of channels managed by the mixer.
   If decreasing the number of channels, the upper channels are
   stopped.
 */
int Mix_AllocateChannels(int numchans)
{
    if (numchans<0 || numchans==num_channels)
        return(num_channels);

    if (numchans < num_channels) {
        /* Stop the affected channels */
        int i;
        for(i=numchans; i < num_channels; i++) {
            Mix_UnregisterAllEffects(i);
            Mix_HaltChannel(i);
        }
    }
    Mix_LockAudio();
    mix_channel = (struct _Mix_Channel *) SDL_realloc(mix_channel, numchans * sizeof(struct _Mix_Channel));
    if (numchans > num_channels) {
        /* Initialize the new channels */
        int i;
        for(i=num_channels; i < numchans; i++) {
            mix_channel[i].chunk = NULL;
            mix_channel[i].playing = 0;
            mix_channel[i].looping = 0;
            mix_channel[i].volume = MIX_MAX_VOLUME;
            mix_channel[i].fade_volume = MIX_MAX_VOLUME;
            mix_channel[i].fade_volume_reset = MIX_MAX_VOLUME;
            mix_channel[i].fading = MIX_NO_FADING;
            mix_channel[i].tag = -1;
            mix_channel[i].expire = 0;
            mix_channel[i].effects = NULL;
            mix_channel[i].paused = 0;
        }
    }
    num_channels = numchans;
    Mix_UnlockAudio();
    return(num_channels);
}

/* Return the actual mixer parameters */
int Mix_QuerySpec(int *frequency, Uint16 *format, int *channels)
{
    if (audio_opened) {
        if (frequency) {
            *frequency = mixer.freq;
        }
        if (format) {
            *format = mixer.format;
        }
        if (channels) {
            *channels = mixer.channels;
        }
    }
    return(audio_opened);
}

typedef struct _MusicFragment
{
    Uint8 *data;
    int size;
    struct _MusicFragment *next;
} MusicFragment;

static SDL_AudioSpec *Mix_LoadMusic_RW(Mix_MusicType music_type, SDL_RWops *src, int freesrc, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
{
    int i;
    Mix_MusicInterface *interface = NULL;
    void *music = NULL;
    Sint64 start;
    SDL_bool playing;
    MusicFragment *first = NULL, *last = NULL, *fragment = NULL;
    int count = 0;
    int fragment_size;

    if (!load_music_type(music_type) || !open_music_type(music_type)) {
        return NULL;
    }

    *spec = mixer;

    /* Use fragments sized on full audio frame boundaries - this'll do */
    fragment_size = spec->size;

    start = SDL_RWtell(src);
    for (i = 0; i < get_num_music_interfaces(); ++i) {
        interface = get_music_interface(i);
        if (!interface->opened) {
            continue;
        }
        if (interface->type != music_type) {
            continue;
        }
        if (!interface->CreateFromRW || !interface->GetAudio) {
            continue;
        }

        /* These music interfaces are not safe to use while music is playing */
        if (interface->api == MIX_MUSIC_CMD ||
             interface->api == MIX_MUSIC_MIKMOD ||
             interface->api == MIX_MUSIC_NATIVEMIDI) {
            continue;
        }

        music = interface->CreateFromRW(src, freesrc);
        if (music) {
            /* The interface owns the data source now */
            freesrc = SDL_FALSE;
            break;
        }

        /* Reset the stream for the next decoder */
        SDL_RWseek(src, start, RW_SEEK_SET);
    }

    if (!music) {
        if (freesrc) {
            SDL_RWclose(src);
        }
        Mix_SetError("Unrecognized audio format");
        return NULL;
    }

    Mix_LockAudio();

    if (interface->Play) {
        interface->Play(music, 1);
    }
    playing = SDL_TRUE;

    while (playing) {
        int left;

        fragment = (MusicFragment *)SDL_malloc(sizeof(*fragment));
        if (!fragment) {
            /* Uh oh, out of memory, let's return what we have */
            break;
        }
        fragment->data = (Uint8 *)SDL_malloc(fragment_size);
        if (!fragment->data) {
            /* Uh oh, out of memory, let's return what we have */
            SDL_free(fragment);
            break;
        }
        fragment->next = NULL;

        left = interface->GetAudio(music, fragment->data, fragment_size);
        if (left > 0) {
            playing = SDL_FALSE;
        } else if (interface->IsPlaying) {
            playing = interface->IsPlaying(music);
        }
        fragment->size = (fragment_size - left);

        if (!first) {
            first = fragment;
        }
        if (last) {
            last->next = fragment;
        }
        last = fragment;
        ++count;
    }

    if (interface->Stop) {
        interface->Stop(music);
    }

    if (music) {
        interface->Delete(music);
    }

    Mix_UnlockAudio();

    if (count > 0) {
        *audio_len = (count - 1) * fragment_size + fragment->size;
        *audio_buf = (Uint8 *)SDL_malloc(*audio_len);
        if (*audio_buf) {
            Uint8 *dst = *audio_buf;
            for (fragment = first; fragment; fragment = fragment->next) {
                SDL_memcpy(dst, fragment->data, fragment->size);
                dst += fragment->size;
            }
        } else {
            SDL_OutOfMemory();
            spec = NULL;
        }
    } else {
        Mix_SetError("No audio data");
        spec = NULL;
    }

    while (first) {
        fragment = first;
        first = first->next;
        SDL_free(fragment->data);
        SDL_free(fragment);
    }

    if (freesrc) {
        SDL_RWclose(src);
    }
    return spec;
}

/* Load a wave file */
Mix_Chunk *Mix_LoadWAV_RW(SDL_RWops *src, int freesrc)
{
    Uint8 magic[4];
    Mix_Chunk *chunk;
    SDL_AudioSpec wavespec, *loaded;
    SDL_AudioCVT wavecvt;
    int samplesize;

    /* rcg06012001 Make sure src is valid */
    if (!src) {
        SDL_SetError("Mix_LoadWAV_RW with NULL src");
        return(NULL);
    }

    /* Make sure audio has been opened */
    if (!audio_opened) {
        SDL_SetError("Audio device hasn't been opened");
        if (freesrc) {
            SDL_RWclose(src);
        }
        return(NULL);
    }

    /* Allocate the chunk memory */
    chunk = (Mix_Chunk *)SDL_malloc(sizeof(Mix_Chunk));
    if (chunk == NULL) {
        SDL_SetError("Out of memory");
        if (freesrc) {
            SDL_RWclose(src);
        }
        return(NULL);
    }

    /* Find out what kind of audio file this is */
    if (SDL_RWread(src, magic, 1, 4) != 4) {
        if (freesrc) {
            SDL_RWclose(src);
        }
        Mix_SetError("Couldn't read first 4 bytes of audio data");
        return NULL;
    }
    /* Seek backwards for compatibility with older loaders */
    SDL_RWseek(src, -4, RW_SEEK_CUR);

    if (SDL_memcmp(magic, "WAVE", 4) == 0 || SDL_memcmp(magic, "RIFF", 4) == 0) {
        loaded = SDL_LoadWAV_RW(src, freesrc, &wavespec, (Uint8 **)&chunk->abuf, &chunk->alen);
    } else if (SDL_memcmp(magic, "FORM", 4) == 0) {
        loaded = Mix_LoadAIFF_RW(src, freesrc, &wavespec, (Uint8 **)&chunk->abuf, &chunk->alen);
    } else if (SDL_memcmp(magic, "Crea", 4) == 0) {
        loaded = Mix_LoadVOC_RW(src, freesrc, &wavespec, (Uint8 **)&chunk->abuf, &chunk->alen);
    } else {
        Mix_MusicType music_type = detect_music_type_from_magic(magic);
        loaded = Mix_LoadMusic_RW(music_type, src, freesrc, &wavespec, (Uint8 **)&chunk->abuf, &chunk->alen);
    }
    if (!loaded) {
        /* The individual loaders have closed src if needed */
        SDL_free(chunk);
        return(NULL);
    }

#if 0
    PrintFormat("Audio device", &mixer);
    PrintFormat("-- Wave file", &wavespec);
#endif

    /* Build the audio converter and create conversion buffers */
    if (wavespec.format != mixer.format ||
         wavespec.channels != mixer.channels ||
         wavespec.freq != mixer.freq) {
        if (SDL_BuildAudioCVT(&wavecvt,
                wavespec.format, wavespec.channels, wavespec.freq,
                mixer.format, mixer.channels, mixer.freq) < 0) {
            SDL_free(chunk->abuf);
            SDL_free(chunk);
            return(NULL);
        }
        samplesize = ((wavespec.format & 0xFF)/8)*wavespec.channels;
        wavecvt.len = chunk->alen & ~(samplesize-1);
        wavecvt.buf = (Uint8 *)SDL_calloc(1, wavecvt.len*wavecvt.len_mult);
        if (wavecvt.buf == NULL) {
            SDL_SetError("Out of memory");
            SDL_free(chunk->abuf);
            SDL_free(chunk);
            return(NULL);
        }
        SDL_memcpy(wavecvt.buf, chunk->abuf, wavecvt.len);
        SDL_free(chunk->abuf);

        /* Run the audio converter */
        if (SDL_ConvertAudio(&wavecvt) < 0) {
            SDL_free(wavecvt.buf);
            SDL_free(chunk);
            return(NULL);
        }

        chunk->abuf = wavecvt.buf;
        chunk->alen = wavecvt.len_cvt;
    }

    chunk->allocated = 1;
    chunk->volume = MIX_MAX_VOLUME;

    return(chunk);
}

/* Load a wave file of the mixer format from a memory buffer */
Mix_Chunk *Mix_QuickLoad_WAV(Uint8 *mem)
{
    Mix_Chunk *chunk;
    Uint8 magic[4];

    /* Make sure audio has been opened */
    if (! audio_opened) {
        SDL_SetError("Audio device hasn't been opened");
        return(NULL);
    }

    /* Allocate the chunk memory */
    chunk = (Mix_Chunk *)SDL_calloc(1,sizeof(Mix_Chunk));
    if (chunk == NULL) {
        SDL_SetError("Out of memory");
        return(NULL);
    }

    /* Essentially just skip to the audio data (no error checking - fast) */
    chunk->allocated = 0;
    mem += 12; /* WAV header */
    do {
        SDL_memcpy(magic, mem, 4);
        mem += 4;
        chunk->alen = ((mem[3]<<24)|(mem[2]<<16)|(mem[1]<<8)|(mem[0]));
        mem += 4;
        chunk->abuf = mem;
        mem += chunk->alen;
    } while (memcmp(magic, "data", 4) != 0);
    chunk->volume = MIX_MAX_VOLUME;

    return(chunk);
}

/* Load raw audio data of the mixer format from a memory buffer */
Mix_Chunk *Mix_QuickLoad_RAW(Uint8 *mem, Uint32 len)
{
    Mix_Chunk *chunk;

    /* Make sure audio has been opened */
    if (! audio_opened) {
        SDL_SetError("Audio device hasn't been opened");
        return(NULL);
    }

    /* Allocate the chunk memory */
    chunk = (Mix_Chunk *)SDL_malloc(sizeof(Mix_Chunk));
    if (chunk == NULL) {
        SDL_SetError("Out of memory");
        return(NULL);
    }

    /* Essentially just point at the audio data (no error checking - fast) */
    chunk->allocated = 0;
    chunk->alen = len;
    chunk->abuf = mem;
    chunk->volume = MIX_MAX_VOLUME;

    return(chunk);
}

/* Free an audio chunk previously loaded */
void Mix_FreeChunk(Mix_Chunk *chunk)
{
    int i;

    /* Caution -- if the chunk is playing, the mixer will crash */
    if (chunk) {
        /* Guarantee that this chunk isn't playing */
        Mix_LockAudio();
        if (mix_channel) {
            for (i=0; i<num_channels; ++i) {
                if (chunk == mix_channel[i].chunk) {
                    mix_channel[i].playing = 0;
                    mix_channel[i].looping = 0;
                }
            }
        }
        Mix_UnlockAudio();
        /* Actually free the chunk */
        if (chunk->allocated) {
            SDL_free(chunk->abuf);
        }
        SDL_free(chunk);
    }
}

/* Set a function that is called after all mixing is performed.
   This can be used to provide real-time visual display of the audio stream
   or add a custom mixer filter for the stream data.
*/
void Mix_SetPostMix(void (SDLCALL *mix_func)
                    (void *udata, Uint8 *stream, int len), void *arg)
{
    Mix_LockAudio();
    mix_postmix_data = arg;
    mix_postmix = mix_func;
    Mix_UnlockAudio();
}

/* Add your own music player or mixer function.
   If 'mix_func' is NULL, the default music player is re-enabled.
 */
void Mix_HookMusic(void (SDLCALL *mix_func)(void *udata, Uint8 *stream, int len),
                                                                void *arg)
{
    Mix_LockAudio();
    if (mix_func != NULL) {
        music_data = arg;
        mix_music = mix_func;
    } else {
        music_data = NULL;
        mix_music = music_mixer;
    }
    Mix_UnlockAudio();
}

void *Mix_GetMusicHookData(void)
{
    return(music_data);
}

void Mix_ChannelFinished(void (SDLCALL *channel_finished)(int channel))
{
    Mix_LockAudio();
    channel_done_callback = channel_finished;
    Mix_UnlockAudio();
}


/* Reserve the first channels (0 -> n-1) for the application, i.e. don't allocate
   them dynamically to the next sample if requested with a -1 value below.
   Returns the number of reserved channels.
 */
int Mix_ReserveChannels(int num)
{
    if (num > num_channels)
        num = num_channels;
    reserved_channels = num;
    return num;
}

static int checkchunkintegral(Mix_Chunk *chunk)
{
    int frame_width = 1;

    if ((mixer.format & 0xFF) == 16) frame_width = 2;
    frame_width *= mixer.channels;
    while (chunk->alen % frame_width) chunk->alen--;
    return chunk->alen;
}

/* Play an audio chunk on a specific channel.
   If the specified channel is -1, play on the first free channel.
   'ticks' is the number of milliseconds at most to play the sample, or -1
   if there is no limit.
   Returns which channel was used to play the sound.
*/
int Mix_PlayChannelTimed(int which, Mix_Chunk *chunk, int loops, int ticks)
{
    int i;

    /* Don't play null pointers :-) */
    if (chunk == NULL) {
        Mix_SetError("Tried to play a NULL chunk");
        return(-1);
    }
    if (!checkchunkintegral(chunk)) {
        Mix_SetError("Tried to play a chunk with a bad frame");
        return(-1);
    }

    /* Lock the mixer while modifying the playing channels */
    Mix_LockAudio();
    {
        /* If which is -1, play on the first free channel */
        if (which == -1) {
            for (i=reserved_channels; i<num_channels; ++i) {
                if (mix_channel[i].playing <= 0)
                    break;
            }
            if (i == num_channels) {
                Mix_SetError("No free channels available");
                which = -1;
            } else {
                which = i;
            }
        }

        /* Queue up the audio data for this channel */
        if (which >= 0 && which < num_channels) {
            Uint32 sdl_ticks = SDL_GetTicks();
            if (Mix_Playing(which))
                _Mix_channel_done_playing(which);
            mix_channel[which].samples = chunk->abuf;
            mix_channel[which].playing = chunk->alen;
            mix_channel[which].looping = loops;
            mix_channel[which].chunk = chunk;
            mix_channel[which].paused = 0;
            mix_channel[which].fading = MIX_NO_FADING;
            mix_channel[which].start_time = sdl_ticks;
            mix_channel[which].expire = (ticks>0) ? (sdl_ticks + ticks) : 0;
        }
    }
    Mix_UnlockAudio();

    /* Return the channel on which the sound is being played */
    return(which);
}

/* Change the expiration delay for a channel */
int Mix_ExpireChannel(int which, int ticks)
{
    int status = 0;

    if (which == -1) {
        int i;
        for (i=0; i < num_channels; ++ i) {
            status += Mix_ExpireChannel(i, ticks);
        }
    } else if (which < num_channels) {
        Mix_LockAudio();
        mix_channel[which].expire = (ticks>0) ? (SDL_GetTicks() + ticks) : 0;
        Mix_UnlockAudio();
        ++ status;
    }
    return(status);
}

/* Fade in a sound on a channel, over ms milliseconds */
int Mix_FadeInChannelTimed(int which, Mix_Chunk *chunk, int loops, int ms, int ticks)
{
    int i;

    /* Don't play null pointers :-) */
    if (chunk == NULL) {
        return(-1);
    }
    if (!checkchunkintegral(chunk)) {
        Mix_SetError("Tried to play a chunk with a bad frame");
        return(-1);
    }

    /* Lock the mixer while modifying the playing channels */
    Mix_LockAudio();
    {
        /* If which is -1, play on the first free channel */
        if (which == -1) {
            for (i=reserved_channels; i<num_channels; ++i) {
                if (mix_channel[i].playing <= 0)
                    break;
            }
            if (i == num_channels) {
                which = -1;
            } else {
                which = i;
            }
        }

        /* Queue up the audio data for this channel */
        if (which >= 0 && which < num_channels) {
            Uint32 sdl_ticks = SDL_GetTicks();
            if (Mix_Playing(which))
                _Mix_channel_done_playing(which);
            mix_channel[which].samples = chunk->abuf;
            mix_channel[which].playing = chunk->alen;
            mix_channel[which].looping = loops;
            mix_channel[which].chunk = chunk;
            mix_channel[which].paused = 0;
            mix_channel[which].fading = MIX_FADING_IN;
            mix_channel[which].fade_volume = mix_channel[which].volume;
            mix_channel[which].fade_volume_reset = mix_channel[which].volume;
            mix_channel[which].volume = 0;
            mix_channel[which].fade_length = (Uint32)ms;
            mix_channel[which].start_time = mix_channel[which].ticks_fade = sdl_ticks;
            mix_channel[which].expire = (ticks > 0) ? (sdl_ticks+ticks) : 0;
        }
    }
    Mix_UnlockAudio();

    /* Return the channel on which the sound is being played */
    return(which);
}

/* Set volume of a particular channel */
int Mix_Volume(int which, int volume)
{
    int i;
    int prev_volume = 0;

    if (which == -1) {
        for (i=0; i<num_channels; ++i) {
            prev_volume += Mix_Volume(i, volume);
        }
        prev_volume /= num_channels;
    } else if (which < num_channels) {
        prev_volume = mix_channel[which].volume;
        if (volume >= 0) {
            if (volume > MIX_MAX_VOLUME) {
                volume = MIX_MAX_VOLUME;
            }
            mix_channel[which].volume = volume;
        }
    }
    return(prev_volume);
}
/* Set volume of a particular chunk */
int Mix_VolumeChunk(Mix_Chunk *chunk, int volume)
{
    int prev_volume;

    prev_volume = chunk->volume;
    if (volume >= 0) {
        if (volume > MIX_MAX_VOLUME) {
            volume = MIX_MAX_VOLUME;
        }
        chunk->volume = volume;
    }
    return(prev_volume);
}

/* Halt playing of a particular channel */
int Mix_HaltChannel(int which)
{
    int i;

    if (which == -1) {
        for (i=0; i<num_channels; ++i) {
            Mix_HaltChannel(i);
        }
    } else if (which < num_channels) {
        Mix_LockAudio();
        if (mix_channel[which].playing) {
            _Mix_channel_done_playing(which);
            mix_channel[which].playing = 0;
            mix_channel[which].looping = 0;
        }
        mix_channel[which].expire = 0;
        if(mix_channel[which].fading != MIX_NO_FADING) /* Restore volume */
            mix_channel[which].volume = mix_channel[which].fade_volume_reset;
        mix_channel[which].fading = MIX_NO_FADING;
        Mix_UnlockAudio();
    }
    return(0);
}

/* Halt playing of a particular group of channels */
int Mix_HaltGroup(int tag)
{
    int i;

    for (i=0; i<num_channels; ++i) {
        if(mix_channel[i].tag == tag) {
            Mix_HaltChannel(i);
        }
    }
    return(0);
}

/* Fade out a channel and then stop it automatically */
int Mix_FadeOutChannel(int which, int ms)
{
    int status;

    status = 0;
    if (audio_opened) {
        if (which == -1) {
            int i;

            for (i=0; i<num_channels; ++i) {
                status += Mix_FadeOutChannel(i, ms);
            }
        } else if (which < num_channels) {
            Mix_LockAudio();
            if (mix_channel[which].playing &&
                (mix_channel[which].volume > 0) &&
                (mix_channel[which].fading != MIX_FADING_OUT)) {
                mix_channel[which].fade_volume = mix_channel[which].volume;
                mix_channel[which].fading = MIX_FADING_OUT;
                mix_channel[which].fade_length = (Uint32)ms;
                mix_channel[which].ticks_fade = SDL_GetTicks();

                /* only change fade_volume_reset if we're not fading. */
                if (mix_channel[which].fading == MIX_NO_FADING) {
                    mix_channel[which].fade_volume_reset = mix_channel[which].volume;
                }
                ++status;
            }
            Mix_UnlockAudio();
        }
    }
    return(status);
}

/* Halt playing of a particular group of channels */
int Mix_FadeOutGroup(int tag, int ms)
{
    int i;
    int status = 0;
    for (i=0; i<num_channels; ++i) {
        if(mix_channel[i].tag == tag) {
            status += Mix_FadeOutChannel(i,ms);
        }
    }
    return(status);
}

Mix_Fading Mix_FadingChannel(int which)
{
    if (which < 0 || which >= num_channels) {
        return MIX_NO_FADING;
    }
    return mix_channel[which].fading;
}

/* Check the status of a specific channel.
   If the specified mix_channel is -1, check all mix channels.
*/
int Mix_Playing(int which)
{
    int status;

    status = 0;
    if (which == -1) {
        int i;

        for (i=0; i<num_channels; ++i) {
            if ((mix_channel[i].playing > 0) ||
                mix_channel[i].looping)
            {
                ++status;
            }
        }
    } else if (which < num_channels) {
        if ((mix_channel[which].playing > 0) ||
             mix_channel[which].looping)
        {
            ++status;
        }
    }
    return(status);
}

/* rcg06072001 Get the chunk associated with a channel. */
Mix_Chunk *Mix_GetChunk(int channel)
{
    Mix_Chunk *retval = NULL;

    if ((channel >= 0) && (channel < num_channels)) {
        retval = mix_channel[channel].chunk;
    }

    return(retval);
}

/* Close the mixer, halting all playing audio */
void Mix_CloseAudio(void)
{
    int i;

    if (audio_opened) {
        if (audio_opened == 1) {
            for (i = 0; i < num_channels; i++) {
                Mix_UnregisterAllEffects(i);
            }
            Mix_UnregisterAllEffects(MIX_CHANNEL_POST);
            close_music();
            Mix_SetMusicCMD(NULL);
            Mix_HaltChannel(-1);
            _Mix_DeinitEffects();
            SDL_CloseAudioDevice(audio_device);
            audio_device = 0;
            SDL_free(mix_channel);
            mix_channel = NULL;

            /* rcg06042009 report available decoders at runtime. */
            SDL_free((void *)chunk_decoders);
            chunk_decoders = NULL;
            num_decoders = 0;
        }
        --audio_opened;
    }
}

/* Pause a particular channel (or all) */
void Mix_Pause(int which)
{
    Uint32 sdl_ticks = SDL_GetTicks();
    if (which == -1) {
        int i;

        for (i=0; i<num_channels; ++i) {
            if (mix_channel[i].playing > 0) {
                mix_channel[i].paused = sdl_ticks;
            }
        }
    } else if (which < num_channels) {
        if (mix_channel[which].playing > 0) {
            mix_channel[which].paused = sdl_ticks;
        }
    }
}

/* Resume a paused channel */
void Mix_Resume(int which)
{
    Uint32 sdl_ticks = SDL_GetTicks();

    Mix_LockAudio();
    if (which == -1) {
        int i;

        for (i=0; i<num_channels; ++i) {
            if (mix_channel[i].playing > 0) {
                if(mix_channel[i].expire > 0)
                    mix_channel[i].expire += sdl_ticks - mix_channel[i].paused;
                mix_channel[i].paused = 0;
            }
        }
    } else if (which < num_channels) {
        if (mix_channel[which].playing > 0) {
            if(mix_channel[which].expire > 0)
                mix_channel[which].expire += sdl_ticks - mix_channel[which].paused;
            mix_channel[which].paused = 0;
        }
    }
    Mix_UnlockAudio();
}

int Mix_Paused(int which)
{
    if (which < 0) {
        int status = 0;
        int i;
        for(i=0; i < num_channels; ++i) {
            if (mix_channel[i].paused) {
                ++ status;
            }
        }
        return(status);
    } else if (which < num_channels) {
        return(mix_channel[which].paused != 0);
    } else {
        return(0);
    }
}

/* Change the group of a channel */
int Mix_GroupChannel(int which, int tag)
{
    if (which < 0 || which > num_channels)
        return(0);

    Mix_LockAudio();
    mix_channel[which].tag = tag;
    Mix_UnlockAudio();
    return(1);
}

/* Assign several consecutive channels to a group */
int Mix_GroupChannels(int from, int to, int tag)
{
    int status = 0;
    for(; from <= to; ++ from) {
        status += Mix_GroupChannel(from, tag);
    }
    return(status);
}

/* Finds the first available channel in a group of channels */
int Mix_GroupAvailable(int tag)
{
    int i;
    for(i=0; i < num_channels; i ++) {
        if (((tag == -1) || (tag == mix_channel[i].tag)) &&
                            (mix_channel[i].playing <= 0))
            return i;
    }
    return(-1);
}

int Mix_GroupCount(int tag)
{
    int count = 0;
    int i;
    for(i=0; i < num_channels; i ++) {
        if (mix_channel[i].tag==tag || tag==-1)
            ++ count;
    }
    return(count);
}

/* Finds the "oldest" sample playing in a group of channels */
int Mix_GroupOldest(int tag)
{
    int chan = -1;
    Uint32 mintime = SDL_GetTicks();
    int i;
    for(i=0; i < num_channels; i ++) {
        if ((mix_channel[i].tag==tag || tag==-1) && mix_channel[i].playing > 0
             && mix_channel[i].start_time <= mintime) {
            mintime = mix_channel[i].start_time;
            chan = i;
        }
    }
    return(chan);
}

/* Finds the "most recent" (i.e. last) sample playing in a group of channels */
int Mix_GroupNewer(int tag)
{
    int chan = -1;
    Uint32 maxtime = 0;
    int i;
    for(i=0; i < num_channels; i ++) {
        if ((mix_channel[i].tag==tag || tag==-1) && mix_channel[i].playing > 0
             && mix_channel[i].start_time >= maxtime) {
            maxtime = mix_channel[i].start_time;
            chan = i;
        }
    }
    return(chan);
}



/*
 * rcg06122001 The special effects exportable API.
 *  Please see effect_*.c for internally-implemented effects, such
 *  as Mix_SetPanning().
 */

/* MAKE SURE you hold the audio lock (Mix_LockAudio()) before calling this! */
static int _Mix_register_effect(effect_info **e, Mix_EffectFunc_t f,
                Mix_EffectDone_t d, void *arg)
{
    effect_info *new_e;

    if (!e) {
        Mix_SetError("Internal error");
        return(0);
    }

    if (f == NULL) {
        Mix_SetError("NULL effect callback");
        return(0);
    }

    new_e = SDL_malloc(sizeof (effect_info));
    if (new_e == NULL) {
        Mix_SetError("Out of memory");
        return(0);
    }

    new_e->callback = f;
    new_e->done_callback = d;
    new_e->udata = arg;
    new_e->next = NULL;

    /* add new effect to end of linked list... */
    if (*e == NULL) {
        *e = new_e;
    } else {
        effect_info *cur = *e;
        while (1) {
            if (cur->next == NULL) {
                cur->next = new_e;
                break;
            }
            cur = cur->next;
        }
    }

    return(1);
}


/* MAKE SURE you hold the audio lock (Mix_LockAudio()) before calling this! */
static int _Mix_remove_effect(int channel, effect_info **e, Mix_EffectFunc_t f)
{
    effect_info *cur;
    effect_info *prev = NULL;
    effect_info *next = NULL;

    if (!e) {
        Mix_SetError("Internal error");
        return(0);
    }

    for (cur = *e; cur != NULL; cur = cur->next) {
        if (cur->callback == f) {
            next = cur->next;
            if (cur->done_callback != NULL) {
                cur->done_callback(channel, cur->udata);
            }
            SDL_free(cur);

            if (prev == NULL) {   /* removing first item of list? */
                *e = next;
            } else {
                prev->next = next;
            }
            return(1);
        }
        prev = cur;
    }

    Mix_SetError("No such effect registered");
    return(0);
}


/* MAKE SURE you hold the audio lock (Mix_LockAudio()) before calling this! */
static int _Mix_remove_all_effects(int channel, effect_info **e)
{
    effect_info *cur;
    effect_info *next;

    if (!e) {
        Mix_SetError("Internal error");
        return(0);
    }

    for (cur = *e; cur != NULL; cur = next) {
        next = cur->next;
        if (cur->done_callback != NULL) {
            cur->done_callback(channel, cur->udata);
        }
        SDL_free(cur);
    }
    *e = NULL;

    return(1);
}


/* MAKE SURE you hold the audio lock (Mix_LockAudio()) before calling this! */
int _Mix_RegisterEffect_locked(int channel, Mix_EffectFunc_t f,
            Mix_EffectDone_t d, void *arg)
{
    effect_info **e = NULL;

    if (channel == MIX_CHANNEL_POST) {
        e = &posteffects;
    } else {
        if ((channel < 0) || (channel >= num_channels)) {
            Mix_SetError("Invalid channel number");
            return(0);
        }
        e = &mix_channel[channel].effects;
    }

    return _Mix_register_effect(e, f, d, arg);
}

int Mix_RegisterEffect(int channel, Mix_EffectFunc_t f,
            Mix_EffectDone_t d, void *arg)
{
    int retval;
    Mix_LockAudio();
    retval = _Mix_RegisterEffect_locked(channel, f, d, arg);
    Mix_UnlockAudio();
    return retval;
}


/* MAKE SURE you hold the audio lock (Mix_LockAudio()) before calling this! */
int _Mix_UnregisterEffect_locked(int channel, Mix_EffectFunc_t f)
{
    effect_info **e = NULL;

    if (channel == MIX_CHANNEL_POST) {
        e = &posteffects;
    } else {
        if ((channel < 0) || (channel >= num_channels)) {
            Mix_SetError("Invalid channel number");
            return(0);
        }
        e = &mix_channel[channel].effects;
    }

    return _Mix_remove_effect(channel, e, f);
}

int Mix_UnregisterEffect(int channel, Mix_EffectFunc_t f)
{
    int retval;
    Mix_LockAudio();
    retval = _Mix_UnregisterEffect_locked(channel, f);
    Mix_UnlockAudio();
    return(retval);
}

/* MAKE SURE you hold the audio lock (Mix_LockAudio()) before calling this! */
int _Mix_UnregisterAllEffects_locked(int channel)
{
    effect_info **e = NULL;

    if (channel == MIX_CHANNEL_POST) {
        e = &posteffects;
    } else {
        if ((channel < 0) || (channel >= num_channels)) {
            Mix_SetError("Invalid channel number");
            return(0);
        }
        e = &mix_channel[channel].effects;
    }

    return _Mix_remove_all_effects(channel, e);
}

int Mix_UnregisterAllEffects(int channel)
{
    int retval;
    Mix_LockAudio();
    retval = _Mix_UnregisterAllEffects_locked(channel);
    Mix_UnlockAudio();
    return(retval);
}

void Mix_LockAudio(void)
{
    SDL_LockAudioDevice(audio_device);
}

void Mix_UnlockAudio(void)
{
    SDL_UnlockAudioDevice(audio_device);
}

/* end of mixer.c ... */

/* vi: set ts=4 sw=4 expandtab: */
