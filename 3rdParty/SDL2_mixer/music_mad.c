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

#ifdef MUSIC_MP3_MAD

#include "music_mad.h"

#include "mad.h"


/* NOTE: The dithering functions are GPL, which should be fine if your
         application is GPL (which would need to be true if you enabled
         libmad support in SDL_mixer). If you're using libmad under the
         commercial license, you need to disable this code.
*/
/************************ dithering functions ***************************/

#ifdef MUSIC_MP3_MAD_GPL_DITHERING

/* All dithering done here is taken from the GPL'ed xmms-mad plugin. */

/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.       */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */

/* Period parameters */
#define MP3_DITH_N 624
#define MP3_DITH_M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static unsigned long mt[MP3_DITH_N]; /* the array for the state vector  */
static int mti=MP3_DITH_N+1; /* mti==MP3_DITH_N+1 means mt[MP3_DITH_N] is not initialized */

/* initializing the array with a NONZERO seed */
static void sgenrand(unsigned long seed)
{
    /* setting initial seeds to mt[MP3_DITH_N] using         */
    /* the generator Line 25 of Table 1 in          */
    /* [KNUTH 1981, The Art of Computer Programming */
    /*    Vol. 2 (2nd Ed.), pp102]                  */
    mt[0]= seed & 0xffffffff;
    for (mti=1; mti<MP3_DITH_N; mti++)
        mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

static unsigned long genrand(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= MP3_DITH_N) { /* generate MP3_DITH_N words at one time */
        int kk;

        if (mti == MP3_DITH_N+1)   /* if sgenrand() has not been called, */
            sgenrand(4357); /* a default initial seed is used   */

        for (kk=0;kk<MP3_DITH_N-MP3_DITH_M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+MP3_DITH_M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<MP3_DITH_N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(MP3_DITH_M-MP3_DITH_N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[MP3_DITH_N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[MP3_DITH_N-1] = mt[MP3_DITH_M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }

    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    return y;
}

static long triangular_dither_noise(int nbits) {
    /* parameter nbits : the peak-to-peak amplitude desired (in bits)
     *  use with nbits set to    2 + nber of bits to be trimmed.
     * (because triangular is made from two uniformly distributed processes,
     * it starts at 2 bits peak-to-peak amplitude)
     * see The Theory of Dithered Quantization by Robert Alexander Wannamaker
     * for complete proof of why that's optimal
     */
    long v = (genrand()/2 - genrand()/2); /* in ]-2^31, 2^31[ */
    long P = 1 << (32 - nbits); /* the power of 2 */
    v /= P;
    /* now v in ]-2^(nbits-1), 2^(nbits-1) [ */

    return v;
}

#endif /* MUSIC_MP3_MAD_GPL_DITHERING */


#define MAD_INPUT_BUFFER_SIZE   (5*8192)

enum {
    MS_input_eof      = 0x0001,
    MS_input_error    = 0x0001,
    MS_decode_error   = 0x0002,
    MS_error_flags    = 0x000f,
};

typedef struct {
    int play_count;
    SDL_RWops *src;
    int freesrc;
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
    mad_timer_t next_frame_start;
    int volume;
    int status;
    SDL_AudioStream *audiostream;

    unsigned char input_buffer[MAD_INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
} MAD_Music;


static int MAD_Seek(void *context, double position);

static void *MAD_CreateFromRW(SDL_RWops *src, int freesrc)
{
    MAD_Music *music;

    music = (MAD_Music *)SDL_calloc(1, sizeof(MAD_Music));
    if (!music) {
        SDL_OutOfMemory();
        return NULL;
    }
    music->src = src;
    music->volume = MIX_MAX_VOLUME;

    mad_stream_init(&music->stream);
    mad_frame_init(&music->frame);
    mad_synth_init(&music->synth);
    mad_timer_reset(&music->next_frame_start);

    music->freesrc = freesrc;
    return music;
}

static void MAD_SetVolume(void *context, int volume)
{
    MAD_Music *music = (MAD_Music *)context;
    music->volume = volume;
}

/* Starts the playback. */
static int MAD_Play(void *context, int play_count)
{
    MAD_Music *music = (MAD_Music *)context;
    music->play_count = play_count;
    return MAD_Seek(music, 0.0);
}


/*************************** TAG HANDLING: ******************************/

static SDL_INLINE SDL_bool is_id3v1(const Uint8 *data, size_t length)
{
    /* http://id3.org/ID3v1 :  3 bytes "TAG" identifier and 125 bytes tag data */
    if (length < 3 || SDL_memcmp(data,"TAG",3) != 0) {
        return SDL_FALSE;
    }
    return SDL_TRUE;
}
static SDL_INLINE SDL_bool is_id3v2(const Uint8 *data, size_t length)
{
    /* ID3v2 header is 10 bytes:  http://id3.org/id3v2.4.0-structure */
    /* bytes 0-2: "ID3" identifier */
    if (length < 10 || SDL_memcmp(data,"ID3",3) != 0) {
        return SDL_FALSE;
    }
    /* bytes 3-4: version num (major,revision), each byte always less than 0xff. */
    if (data[3] == 0xff || data[4] == 0xff) {
        return SDL_FALSE;
    }
    /* bytes 6-9 are the ID3v2 tag size: a 32 bit 'synchsafe' integer, i.e. the
     * highest bit 7 in each byte zeroed.  i.e.: 7 bit information in each byte ->
     * effectively a 28 bit value.  */
    if (data[6] >= 0x80 || data[7] >= 0x80 || data[8] >= 0x80 || data[9] >= 0x80) {
        return SDL_FALSE;
    }
    return SDL_TRUE;
}
static SDL_INLINE SDL_bool is_apetag(const Uint8 *data, size_t length)
{
   /* http://wiki.hydrogenaud.io/index.php?title=APEv2_specification
    * APEv2 header is 32 bytes: bytes 0-7 ident, bytes 8-11 version,
    * bytes 12-17 size.  bytes 24-31 are reserved: must be all zeroes.
    * APEv1 has no header, so no luck.  */
    Uint32 v;

    if (length < 32 || SDL_memcmp(data,"APETAGEX",8) != 0) {
        return SDL_FALSE;
    }
    v = (data[11]<<24) | (data[10]<<16) | (data[9]<<8) | data[8]; /* version */
    if (v != 2000U) {
        return SDL_FALSE;
    }
    v = 0; /* reserved bits : */
    if (SDL_memcmp(&data[24],&v,4) != 0 || SDL_memcmp(&data[28],&v,4) != 0) {
        return SDL_FALSE;
    }
    return SDL_TRUE;
}

static size_t get_tagsize(const Uint8 *data, size_t length)
{
    size_t size;

    if (is_id3v1(data, length)) {
        return 128; /* fixed length */
    }
    if (is_id3v2(data, length)) {
        /* size is a 'synchsafe' integer (see above) */
        size = (data[6]<<21) + (data[7]<<14) + (data[8]<<7) + data[9];
        size += 10; /* header size */
        /* ID3v2 header[5] is flags (bits 4-7 only, 0-3 are zero).
         * bit 4 set: footer is present (a copy of the header but
         * with "3DI" as ident.)  */
        if (data[5] & 0x10) {
            size += 10; /* footer size */
        }
        /* optional padding (always zeroes) */
        while (size < length && data[size] == 0) {
            ++size;
        }
        return size;
    }
    if (is_apetag(data, length)) {
        size = (data[15]<<24) | (data[14]<<16) | (data[13]<<8) | data[12];
        size += 32; /* header size */
        return size;
    }
    return 0;
}

static int consume_tag(struct mad_stream *stream)
{
    /* FIXME: what if the buffer doesn't have the full tag ??? */
    size_t remaining = stream->bufend - stream->next_frame;
    size_t tagsize = get_tagsize(stream->this_frame, remaining);
    if (tagsize != 0) {
        mad_stream_skip(stream, tagsize);
        return 0;
    }
    return -1;
}

/* Reads the next frame from the file.
   Returns true on success or false on failure.
 */
static SDL_bool read_next_frame(MAD_Music *music)
{
    if (music->stream.buffer == NULL ||
        music->stream.error == MAD_ERROR_BUFLEN) {
        size_t read_size;
        size_t remaining;
        unsigned char *read_start;

        /* There might be some bytes in the buffer left over from last
           time.    If so, move them down and read more bytes following
           them. */
        if (music->stream.next_frame != NULL) {
            remaining = music->stream.bufend - music->stream.next_frame;
            memmove(music->input_buffer, music->stream.next_frame, remaining);
            read_start = music->input_buffer + remaining;
            read_size = MAD_INPUT_BUFFER_SIZE - remaining;

        } else {
            read_size = MAD_INPUT_BUFFER_SIZE;
            read_start = music->input_buffer;
            remaining = 0;
        }

        /* Now read additional bytes from the input file. */
        read_size = SDL_RWread(music->src, read_start, 1, read_size);

        if (read_size == 0) {
            if ((music->status & (MS_input_eof | MS_input_error)) == 0) {
                /* FIXME: how to detect error? */
                music->status |= MS_input_eof;

                /* At the end of the file, we must stuff MAD_BUFFER_GUARD
                   number of 0 bytes. */
                SDL_memset(read_start + read_size, 0, MAD_BUFFER_GUARD);
                read_size += MAD_BUFFER_GUARD;
            }
        }

        /* Now feed those bytes into the libmad stream. */
        mad_stream_buffer(&music->stream, music->input_buffer,
                                            read_size + remaining);
        music->stream.error = MAD_ERROR_NONE;
    }

    /* Now ask libmad to extract a frame from the data we just put in
       its buffer. */
    if (mad_frame_decode(&music->frame, &music->stream)) {
        if (MAD_RECOVERABLE(music->stream.error)) {
            consume_tag(&music->stream); /* consume any ID3 tags */
            mad_stream_sync(&music->stream); /* to frame seek mode */
            return SDL_FALSE;

        } else if (music->stream.error == MAD_ERROR_BUFLEN) {
            return SDL_FALSE;

        } else {
            Mix_SetError("mad_frame_decode() failed, corrupt stream?");
            music->status |= MS_decode_error;
            return SDL_FALSE;
        }
    }

    mad_timer_add(&music->next_frame_start, music->frame.header.duration);

    return SDL_TRUE;
}

/* Scale a MAD sample to 16 bits for output. */
static Sint16 scale(mad_fixed_t sample)
{
    const int n_bits_to_loose = MAD_F_FRACBITS + 1 - 16;

    /* round */
    sample += (1L << (n_bits_to_loose - 1));

#ifdef MUSIC_MP3_MAD_GPL_DITHERING
    sample += triangular_dither_noise(n_bits_to_loose + 1);
#endif

    /* clip */
    if (sample >= MAD_F_ONE)
        sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
        sample = -MAD_F_ONE;

    /* quantize */
    return (Sint16)(sample >> n_bits_to_loose);
}

/* Once the frame has been read, copies its samples into the output buffer. */
static SDL_bool decode_frame(MAD_Music *music)
{
    struct mad_pcm *pcm;
    unsigned int i, nchannels, nsamples;
    mad_fixed_t const *left_ch, *right_ch;
    Sint16 *buffer, *dst;
    int result;

    mad_synth_frame(&music->synth, &music->frame);
    pcm = &music->synth.pcm;

    if (!music->audiostream) {
        music->audiostream = SDL_NewAudioStream(AUDIO_S16, pcm->channels, pcm->samplerate, music_spec.format, music_spec.channels, music_spec.freq);
        if (!music->audiostream) {
            return SDL_FALSE;
        }
    }

    nchannels = pcm->channels;
    nsamples = pcm->length;
    left_ch = pcm->samples[0];
    right_ch = pcm->samples[1];
    buffer = SDL_stack_alloc(Sint16, nsamples*nchannels);
    if (!buffer) {
        SDL_OutOfMemory();
        return SDL_FALSE;
    }

    dst = buffer;
    if (nchannels == 1) {
        for (i = nsamples; i--;) {
            *dst++ = scale(*left_ch++);
        }
    } else {
        for (i = nsamples; i--;) {
            *dst++ = scale(*left_ch++);
            *dst++ = scale(*right_ch++);
        }
    }

    result = SDL_AudioStreamPut(music->audiostream, buffer, (nsamples * nchannels * sizeof(Sint16)));
    SDL_stack_free(buffer);

    if (result < 0) {
        return SDL_FALSE;
    }
    return SDL_TRUE;
}

static int MAD_GetSome(void *context, void *data, int bytes, SDL_bool *done)
{
    MAD_Music *music = (MAD_Music *)context;
    int filled;

    if (music->audiostream) {
        filled = SDL_AudioStreamGet(music->audiostream, data, bytes);
        if (filled != 0) {
            return filled;
        }
    }

    if (!music->play_count) {
        /* All done */
        *done = SDL_TRUE;
        return 0;
    }

    if (read_next_frame(music)) {
        if (!decode_frame(music)) {
            return -1;
        }
    } else if (music->status & MS_input_eof) {
        int play_count = -1;
        if (music->play_count > 0) {
            play_count = (music->play_count - 1);
        }
        if (MAD_Play(music, play_count) < 0) {
            return -1;
        }
    } else if (music->status & MS_decode_error) {
        return -1;
    }
    return 0;
}
static int MAD_GetAudio(void *context, void *data, int bytes)
{
    MAD_Music *music = (MAD_Music *)context;
    return music_pcm_getaudio(context, data, bytes, music->volume, MAD_GetSome);
}

static int MAD_Seek(void *context, double position)
{
    MAD_Music *music = (MAD_Music *)context;
    mad_timer_t target;
    int int_part;

    int_part = (int)position;
    mad_timer_set(&target, int_part, (int)((position - int_part) * 1000000), 1000000);

    if (mad_timer_compare(music->next_frame_start, target) > 0) {
        /* In order to seek backwards in a VBR file, we have to rewind and
           start again from the beginning.    This isn't necessary if the
           file happens to be CBR, of course; in that case we could seek
           directly to the frame we want.    But I leave that little
           optimization for the future developer who discovers she really
           needs it. */
        mad_timer_reset(&music->next_frame_start);
        music->status &= ~MS_error_flags;

        SDL_RWseek(music->src, 0, RW_SEEK_SET);
    }

    /* Now we have to skip frames until we come to the right one.
       Again, only truly necessary if the file is VBR. */
    while (mad_timer_compare(music->next_frame_start, target) < 0) {
        if (!read_next_frame(music)) {
            if ((music->status & MS_error_flags) != 0) {
                /* Couldn't read a frame; either an error condition or
                     end-of-file.    Stop. */
                return Mix_SetError("Seek position out of range");
            }
        }
    }

    /* Here we are, at the beginning of the frame that contains the
       target time.    Ehh, I say that's close enough.    If we wanted to,
       we could get more precise by decoding the frame now and counting
       the appropriate number of samples out of it. */
    return 0;
}

static void MAD_Delete(void *context)
{
    MAD_Music *music = (MAD_Music *)context;

    mad_stream_finish(&music->stream);
    mad_frame_finish(&music->frame);
    mad_synth_finish(&music->synth);

    if (music->audiostream) {
        SDL_FreeAudioStream(music->audiostream);
    }
    if (music->freesrc) {
        SDL_RWclose(music->src);
    }
    SDL_free(music);
}

Mix_MusicInterface Mix_MusicInterface_MAD =
{
    "MAD",
    MIX_MUSIC_MAD,
    MUS_MP3,
    SDL_FALSE,
    SDL_FALSE,

    NULL,   /* Load */
    NULL,   /* Open */
    MAD_CreateFromRW,
    NULL,   /* CreateFromFile */
    MAD_SetVolume,
    MAD_Play,
    NULL,   /* IsPlaying */
    MAD_GetAudio,
    MAD_Seek,
    NULL,   /* Pause */
    NULL,   /* Resume */
    NULL,   /* Stop */
    MAD_Delete,
    NULL,   /* Close */
    NULL,   /* Unload */
};

#endif /* MUSIC_MP3_MAD */

/* vi: set ts=4 sw=4 expandtab: */
