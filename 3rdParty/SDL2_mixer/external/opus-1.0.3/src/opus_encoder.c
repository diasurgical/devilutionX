/* Copyright (c) 2010-2011 Xiph.Org Foundation, Skype Limited
   Written by Jean-Marc Valin and Koen Vos */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include "celt.h"
#include "entenc.h"
#include "modes.h"
#include "API.h"
#include "stack_alloc.h"
#include "float_cast.h"
#include "opus.h"
#include "arch.h"
#include "opus_private.h"
#include "os_support.h"

#include "tuning_parameters.h"
#ifdef FIXED_POINT
#include "fixed/structs_FIX.h"
#else
#include "float/structs_FLP.h"
#endif

#define MAX_ENCODER_BUFFER 480

struct OpusEncoder {
    int          celt_enc_offset;
    int          silk_enc_offset;
    silk_EncControlStruct silk_mode;
    int          application;
    int          channels;
    int          delay_compensation;
    int          force_channels;
    int          signal_type;
    int          user_bandwidth;
    int          max_bandwidth;
    int          user_forced_mode;
    int          voice_ratio;
    opus_int32   Fs;
    int          use_vbr;
    int          vbr_constraint;
    opus_int32   bitrate_bps;
    opus_int32   user_bitrate_bps;
    int          encoder_buffer;

#define OPUS_ENCODER_RESET_START stream_channels
    int          stream_channels;
    opus_int16   hybrid_stereo_width_Q14;
    opus_int32   variable_HP_smth2_Q15;
    opus_val32   hp_mem[4];
    int          mode;
    int          prev_mode;
    int          prev_channels;
    int          prev_framesize;
    int          bandwidth;
    int          silk_bw_switch;
    /* Sampling rate (at the API level) */
    int          first;
    opus_val16   delay_buffer[MAX_ENCODER_BUFFER*2];

    opus_uint32  rangeFinal;
};

/* Transition tables for the voice and music. First column is the
   middle (memoriless) threshold. The second column is the hysteresis
   (difference with the middle) */
static const opus_int32 mono_voice_bandwidth_thresholds[8] = {
        11000, 1000, /* NB<->MB */
        14000, 1000, /* MB<->WB */
        21000, 2000, /* WB<->SWB */
        29000, 2000, /* SWB<->FB */
};
static const opus_int32 mono_music_bandwidth_thresholds[8] = {
        14000, 1000, /* MB not allowed */
        18000, 2000, /* MB<->WB */
        24000, 2000, /* WB<->SWB */
        33000, 2000, /* SWB<->FB */
};
static const opus_int32 stereo_voice_bandwidth_thresholds[8] = {
        11000, 1000, /* NB<->MB */
        14000, 1000, /* MB<->WB */
        21000, 2000, /* WB<->SWB */
        32000, 2000, /* SWB<->FB */
};
static const opus_int32 stereo_music_bandwidth_thresholds[8] = {
        14000, 1000, /* MB not allowed */
        18000, 2000, /* MB<->WB */
        24000, 2000, /* WB<->SWB */
        48000, 2000, /* SWB<->FB */
};
/* Threshold bit-rates for switching between mono and stereo */
static const opus_int32 stereo_voice_threshold = 26000;
static const opus_int32 stereo_music_threshold = 36000;

/* Threshold bit-rate for switching between SILK/hybrid and CELT-only */
static const opus_int32 mode_thresholds[2][2] = {
      /* voice */ /* music */
      {  48000,      24000}, /* mono */
      {  48000,      24000}, /* stereo */
};

int opus_encoder_get_size(int channels)
{
    int silkEncSizeBytes, celtEncSizeBytes;
    int ret;
    if (channels<1 || channels > 2)
        return 0;
    ret = silk_Get_Encoder_Size( &silkEncSizeBytes );
    if (ret)
        return 0;
    silkEncSizeBytes = align(silkEncSizeBytes);
    celtEncSizeBytes = celt_encoder_get_size(channels);
    return align(sizeof(OpusEncoder))+silkEncSizeBytes+celtEncSizeBytes;
}

int opus_encoder_init(OpusEncoder* st, opus_int32 Fs, int channels, int application)
{
    void *silk_enc;
    CELTEncoder *celt_enc;
    int err;
    int ret, silkEncSizeBytes;

   if((Fs!=48000&&Fs!=24000&&Fs!=16000&&Fs!=12000&&Fs!=8000)||(channels!=1&&channels!=2)||
        (application != OPUS_APPLICATION_VOIP && application != OPUS_APPLICATION_AUDIO
        && application != OPUS_APPLICATION_RESTRICTED_LOWDELAY))
        return OPUS_BAD_ARG;

    OPUS_CLEAR((char*)st, opus_encoder_get_size(channels));
    /* Create SILK encoder */
    ret = silk_Get_Encoder_Size( &silkEncSizeBytes );
    if (ret)
        return OPUS_BAD_ARG;
    silkEncSizeBytes = align(silkEncSizeBytes);
    st->silk_enc_offset = align(sizeof(OpusEncoder));
    st->celt_enc_offset = st->silk_enc_offset+silkEncSizeBytes;
    silk_enc = (char*)st+st->silk_enc_offset;
    celt_enc = (CELTEncoder*)((char*)st+st->celt_enc_offset);

    st->stream_channels = st->channels = channels;

    st->Fs = Fs;

    ret = silk_InitEncoder( silk_enc, &st->silk_mode );
    if(ret)return OPUS_INTERNAL_ERROR;

    /* default SILK parameters */
    st->silk_mode.nChannelsAPI              = channels;
    st->silk_mode.nChannelsInternal         = channels;
    st->silk_mode.API_sampleRate            = st->Fs;
    st->silk_mode.maxInternalSampleRate     = 16000;
    st->silk_mode.minInternalSampleRate     = 8000;
    st->silk_mode.desiredInternalSampleRate = 16000;
    st->silk_mode.payloadSize_ms            = 20;
    st->silk_mode.bitRate                   = 25000;
    st->silk_mode.packetLossPercentage      = 0;
    st->silk_mode.complexity                = 10;
    st->silk_mode.useInBandFEC              = 0;
    st->silk_mode.useDTX                    = 0;
    st->silk_mode.useCBR                    = 0;

    /* Create CELT encoder */
    /* Initialize CELT encoder */
    err = celt_encoder_init(celt_enc, Fs, channels);
    if(err!=OPUS_OK)return OPUS_INTERNAL_ERROR;

    celt_encoder_ctl(celt_enc, CELT_SET_SIGNALLING(0));
    celt_encoder_ctl(celt_enc, OPUS_SET_COMPLEXITY(10));

    st->use_vbr = 1;
    /* Makes constrained VBR the default (safer for real-time use) */
    st->vbr_constraint = 1;
    st->user_bitrate_bps = OPUS_AUTO;
    st->bitrate_bps = 3000+Fs*channels;
    st->application = application;
    st->signal_type = OPUS_AUTO;
    st->user_bandwidth = OPUS_AUTO;
    st->max_bandwidth = OPUS_BANDWIDTH_FULLBAND;
    st->force_channels = OPUS_AUTO;
    st->user_forced_mode = OPUS_AUTO;
    st->voice_ratio = -1;
    st->encoder_buffer = st->Fs/100;

    /* Delay compensation of 4 ms (2.5 ms for SILK's extra look-ahead 
       + 1.5 ms for SILK resamplers and stereo prediction) */
    st->delay_compensation = st->Fs/250;

    st->hybrid_stereo_width_Q14 = 1 << 14;
    st->variable_HP_smth2_Q15 = silk_LSHIFT( silk_lin2log( VARIABLE_HP_MIN_CUTOFF_HZ ), 8 );
    st->first = 1;
    st->mode = MODE_HYBRID;
    st->bandwidth = OPUS_BANDWIDTH_FULLBAND;

    return OPUS_OK;
}

static int pad_frame(unsigned char *data, opus_int32 len, opus_int32 new_len)
{
   if (len == new_len)
      return 0;
   if (len > new_len)
      return 1;

   if ((data[0]&0x3)==0)
   {
      int i;
      int padding, nb_255s;

      padding = new_len - len;
      if (padding >= 2)
      {
         nb_255s = (padding-2)/255;

         for (i=len-1;i>=1;i--)
            data[i+nb_255s+2] = data[i];
         data[0] |= 0x3;
         data[1] = 0x41;
         for (i=0;i<nb_255s;i++)
            data[i+2] = 255;
         data[nb_255s+2] = padding-255*nb_255s-2;
         for (i=len+3+nb_255s;i<new_len;i++)
            data[i] = 0;
      } else {
         for (i=len-1;i>=1;i--)
            data[i+1] = data[i];
         data[0] |= 0x3;
         data[1] = 1;
      }
      return 0;
   } else {
      return 1;
   }
}

static unsigned char gen_toc(int mode, int framerate, int bandwidth, int channels)
{
   int period;
   unsigned char toc;
   period = 0;
   while (framerate < 400)
   {
       framerate <<= 1;
       period++;
   }
   if (mode == MODE_SILK_ONLY)
   {
       toc = (bandwidth-OPUS_BANDWIDTH_NARROWBAND)<<5;
       toc |= (period-2)<<3;
   } else if (mode == MODE_CELT_ONLY)
   {
       int tmp = bandwidth-OPUS_BANDWIDTH_MEDIUMBAND;
       if (tmp < 0)
           tmp = 0;
       toc = 0x80;
       toc |= tmp << 5;
       toc |= period<<3;
   } else /* Hybrid */
   {
       toc = 0x60;
       toc |= (bandwidth-OPUS_BANDWIDTH_SUPERWIDEBAND)<<4;
       toc |= (period-2)<<3;
   }
   toc |= (channels==2)<<2;
   return toc;
}

#ifndef FIXED_POINT
static void silk_biquad_float(
    const opus_val16      *in,            /* I:    Input signal                   */
    const opus_int32      *B_Q28,         /* I:    MA coefficients [3]            */
    const opus_int32      *A_Q28,         /* I:    AR coefficients [2]            */
    opus_val32            *S,             /* I/O:  State vector [2]               */
    opus_val16            *out,           /* O:    Output signal                  */
    const opus_int32      len,            /* I:    Signal length (must be even)   */
    int stride
)
{
    /* DIRECT FORM II TRANSPOSED (uses 2 element state vector) */
    opus_int   k;
    opus_val32 vout;
    opus_val32 inval;
    opus_val32 A[2], B[3];

    A[0] = (opus_val32)(A_Q28[0] * (1.f/((opus_int32)1<<28)));
    A[1] = (opus_val32)(A_Q28[1] * (1.f/((opus_int32)1<<28)));
    B[0] = (opus_val32)(B_Q28[0] * (1.f/((opus_int32)1<<28)));
    B[1] = (opus_val32)(B_Q28[1] * (1.f/((opus_int32)1<<28)));
    B[2] = (opus_val32)(B_Q28[2] * (1.f/((opus_int32)1<<28)));

    /* Negate A_Q28 values and split in two parts */

    for( k = 0; k < len; k++ ) {
        /* S[ 0 ], S[ 1 ]: Q12 */
        inval = in[ k*stride ];
        vout = S[ 0 ] + B[0]*inval;

        S[ 0 ] = S[1] - vout*A[0] + B[1]*inval;

        S[ 1 ] = - vout*A[1] + B[2]*inval;

        /* Scale back to Q0 and saturate */
        out[ k*stride ] = vout;
    }
}
#endif

static void hp_cutoff(const opus_val16 *in, opus_int32 cutoff_Hz, opus_val16 *out, opus_val32 *hp_mem, int len, int channels, opus_int32 Fs)
{
   opus_int32 B_Q28[ 3 ], A_Q28[ 2 ];
   opus_int32 Fc_Q19, r_Q28, r_Q22;

   silk_assert( cutoff_Hz <= silk_int32_MAX / SILK_FIX_CONST( 1.5 * 3.14159 / 1000, 19 ) );
   Fc_Q19 = silk_DIV32_16( silk_SMULBB( SILK_FIX_CONST( 1.5 * 3.14159 / 1000, 19 ), cutoff_Hz ), Fs/1000 );
   silk_assert( Fc_Q19 > 0 && Fc_Q19 < 32768 );

   r_Q28 = SILK_FIX_CONST( 1.0, 28 ) - silk_MUL( SILK_FIX_CONST( 0.92, 9 ), Fc_Q19 );

   /* b = r * [ 1; -2; 1 ]; */
   /* a = [ 1; -2 * r * ( 1 - 0.5 * Fc^2 ); r^2 ]; */
   B_Q28[ 0 ] = r_Q28;
   B_Q28[ 1 ] = silk_LSHIFT( -r_Q28, 1 );
   B_Q28[ 2 ] = r_Q28;

   /* -r * ( 2 - Fc * Fc ); */
   r_Q22  = silk_RSHIFT( r_Q28, 6 );
   A_Q28[ 0 ] = silk_SMULWW( r_Q22, silk_SMULWW( Fc_Q19, Fc_Q19 ) - SILK_FIX_CONST( 2.0,  22 ) );
   A_Q28[ 1 ] = silk_SMULWW( r_Q22, r_Q22 );

#ifdef FIXED_POINT
   silk_biquad_alt( in, B_Q28, A_Q28, hp_mem, out, len, channels );
   if( channels == 2 ) {
       silk_biquad_alt( in+1, B_Q28, A_Q28, hp_mem+2, out+1, len, channels );
   }
#else
   silk_biquad_float( in, B_Q28, A_Q28, hp_mem, out, len, channels );
   if( channels == 2 ) {
       silk_biquad_float( in+1, B_Q28, A_Q28, hp_mem+2, out+1, len, channels );
   }
#endif
}

static void stereo_fade(const opus_val16 *in, opus_val16 *out, opus_val16 g1, opus_val16 g2,
        int overlap48, int frame_size, int channels, const opus_val16 *window, opus_int32 Fs)
{
    int i;
    int overlap;
    int inc;
    inc = 48000/Fs;
    overlap=overlap48/inc;
    g1 = Q15ONE-g1;
    g2 = Q15ONE-g2;
    for (i=0;i<overlap;i++)
    {
       opus_val32 diff;
       opus_val16 g, w;
       w = MULT16_16_Q15(window[i*inc], window[i*inc]);
       g = SHR32(MAC16_16(MULT16_16(w,g2),
             Q15ONE-w, g1), 15);
       diff = EXTRACT16(HALF32((opus_val32)in[i*channels] - (opus_val32)in[i*channels+1]));
       diff = MULT16_16_Q15(g, diff);
       out[i*channels] = out[i*channels] - diff;
       out[i*channels+1] = out[i*channels+1] + diff;
    }
    for (;i<frame_size;i++)
    {
       opus_val32 diff;
       diff = EXTRACT16(HALF32((opus_val32)in[i*channels] - (opus_val32)in[i*channels+1]));
       diff = MULT16_16_Q15(g2, diff);
       out[i*channels] = out[i*channels] - diff;
       out[i*channels+1] = out[i*channels+1] + diff;
    }
}

OpusEncoder *opus_encoder_create(opus_int32 Fs, int channels, int application, int *error)
{
   int ret;
   OpusEncoder *st;
   if((Fs!=48000&&Fs!=24000&&Fs!=16000&&Fs!=12000&&Fs!=8000)||(channels!=1&&channels!=2)||
       (application != OPUS_APPLICATION_VOIP && application != OPUS_APPLICATION_AUDIO
       && application != OPUS_APPLICATION_RESTRICTED_LOWDELAY))
   {
      if (error)
         *error = OPUS_BAD_ARG;
      return NULL;
   }
   st = (OpusEncoder *)opus_alloc(opus_encoder_get_size(channels));
   if (st == NULL)
   {
      if (error)
         *error = OPUS_ALLOC_FAIL;
      return NULL;
   }
   ret = opus_encoder_init(st, Fs, channels, application);
   if (error)
      *error = ret;
   if (ret != OPUS_OK)
   {
      opus_free(st);
      st = NULL;
   }
   return st;
}

static opus_int32 user_bitrate_to_bitrate(OpusEncoder *st, int frame_size, int max_data_bytes)
{
  if(!frame_size)frame_size=st->Fs/400;
  if (st->user_bitrate_bps==OPUS_AUTO)
    return 60*st->Fs/frame_size + st->Fs*st->channels;
  else if (st->user_bitrate_bps==OPUS_BITRATE_MAX)
    return max_data_bytes*8*st->Fs/frame_size;
  else
    return st->user_bitrate_bps;
}

#ifdef FIXED_POINT
#define opus_encode_native opus_encode
opus_int32 opus_encode(OpusEncoder *st, const opus_val16 *pcm, int frame_size,
                unsigned char *data, opus_int32 out_data_bytes)
#else
#define opus_encode_native opus_encode_float
opus_int32 opus_encode_float(OpusEncoder *st, const opus_val16 *pcm, int frame_size,
                      unsigned char *data, opus_int32 out_data_bytes)
#endif
{
    void *silk_enc;
    CELTEncoder *celt_enc;
    int i;
    int ret=0;
    opus_int32 nBytes;
    ec_enc enc;
    int bytes_target;
    int prefill=0;
    int start_band = 0;
    int redundancy = 0;
    int redundancy_bytes = 0; /* Number of bytes to use for redundancy frame */
    int celt_to_silk = 0;
    VARDECL(opus_val16, pcm_buf);
    int nb_compr_bytes;
    int to_celt = 0;
    opus_uint32 redundant_rng = 0;
    int cutoff_Hz, hp_freq_smth1;
    int voice_est; /* Probability of voice in Q7 */
    opus_int32 equiv_rate;
    int delay_compensation;
    int frame_rate;
    opus_int32 max_rate; /* Max bitrate we're allowed to use */
    int curr_bandwidth;
    opus_int32 max_data_bytes; /* Max number of bytes we're allowed to use */
    VARDECL(opus_val16, tmp_prefill);

    ALLOC_STACK;

    max_data_bytes = IMIN(1276, out_data_bytes);

    st->rangeFinal = 0;
    if (400*frame_size != st->Fs && 200*frame_size != st->Fs && 100*frame_size != st->Fs &&
         50*frame_size != st->Fs &&  25*frame_size != st->Fs &&  50*frame_size != 3*st->Fs)
    {
       RESTORE_STACK;
       return OPUS_BAD_ARG;
    }
    if (max_data_bytes<=0)
    {
       RESTORE_STACK;
       return OPUS_BAD_ARG;
    }
    silk_enc = (char*)st+st->silk_enc_offset;
    celt_enc = (CELTEncoder*)((char*)st+st->celt_enc_offset);

    if (st->application == OPUS_APPLICATION_RESTRICTED_LOWDELAY)
       delay_compensation = 0;
    else
       delay_compensation = st->delay_compensation;

    st->bitrate_bps = user_bitrate_to_bitrate(st, frame_size, max_data_bytes);

    frame_rate = st->Fs/frame_size;
    if (max_data_bytes<3 || st->bitrate_bps < 3*frame_rate*8
       || (frame_rate<50 && (max_data_bytes*frame_rate<300 || st->bitrate_bps < 2400)))
    {
       /*If the space is too low to do something useful, emit 'PLC' frames.*/
       int tocmode = st->mode;
       int bw = st->bandwidth == 0 ? OPUS_BANDWIDTH_NARROWBAND : st->bandwidth;
       if (tocmode==0)
          tocmode = MODE_SILK_ONLY;
       if (frame_rate>100)
          tocmode = MODE_CELT_ONLY;
       if (frame_rate < 50)
          tocmode = MODE_SILK_ONLY;
       if(tocmode==MODE_SILK_ONLY&&bw>OPUS_BANDWIDTH_WIDEBAND)
          bw=OPUS_BANDWIDTH_WIDEBAND;
       else if (tocmode==MODE_CELT_ONLY&&bw==OPUS_BANDWIDTH_MEDIUMBAND)
          bw=OPUS_BANDWIDTH_NARROWBAND;
       else if (bw<=OPUS_BANDWIDTH_SUPERWIDEBAND)
          bw=OPUS_BANDWIDTH_SUPERWIDEBAND;
       data[0] = gen_toc(tocmode, frame_rate, bw, st->stream_channels);
       RESTORE_STACK;
       return 1;
    }
    if (!st->use_vbr)
    {
       int cbrBytes;
       cbrBytes = IMIN( (st->bitrate_bps + 4*frame_rate)/(8*frame_rate) , max_data_bytes);
       st->bitrate_bps = cbrBytes * (8*frame_rate);
       max_data_bytes = cbrBytes;
    }
    max_rate = frame_rate*max_data_bytes*8;

    /* Equivalent 20-ms rate for mode/channel/bandwidth decisions */
    equiv_rate = st->bitrate_bps - 60*(st->Fs/frame_size - 50);

    if (st->signal_type == OPUS_SIGNAL_VOICE)
       voice_est = 127;
    else if (st->signal_type == OPUS_SIGNAL_MUSIC)
       voice_est = 0;
    else if (st->voice_ratio >= 0)
       voice_est = st->voice_ratio*327>>8;
    else if (st->application == OPUS_APPLICATION_VOIP)
       voice_est = 115;
    else
       voice_est = 48;

    if (st->force_channels!=OPUS_AUTO && st->channels == 2)
    {
        st->stream_channels = st->force_channels;
    } else {
#ifdef FUZZING
       /* Random mono/stereo decision */
       if (st->channels == 2 && (rand()&0x1F)==0)
          st->stream_channels = 3-st->stream_channels;
#else
       /* Rate-dependent mono-stereo decision */
       if (st->channels == 2)
       {
          opus_int32 stereo_threshold;
          stereo_threshold = stereo_music_threshold + ((voice_est*voice_est*(stereo_voice_threshold-stereo_music_threshold))>>14);
          if (st->stream_channels == 2)
             stereo_threshold -= 4000;
          else
             stereo_threshold += 4000;
          st->stream_channels = (equiv_rate > stereo_threshold) ? 2 : 1;
       } else {
          st->stream_channels = st->channels;
       }
#endif
    }

    /* Mode selection depending on application and signal type */
    if (st->application == OPUS_APPLICATION_RESTRICTED_LOWDELAY)
    {
       st->mode = MODE_CELT_ONLY;
    } else if (st->user_forced_mode == OPUS_AUTO)
    {
#ifdef FUZZING
       /* Random mode switching */
       if ((rand()&0xF)==0)
       {
          if ((rand()&0x1)==0)
             st->mode = MODE_CELT_ONLY;
          else
             st->mode = MODE_SILK_ONLY;
       } else {
          if (st->prev_mode==MODE_CELT_ONLY)
             st->mode = MODE_CELT_ONLY;
          else
             st->mode = MODE_SILK_ONLY;
       }
#else
       int chan;
       opus_int32 mode_voice, mode_music;
       opus_int32 threshold;

       chan = (st->channels==2) && st->force_channels!=1;
       mode_voice = mode_thresholds[chan][0];
       mode_music = mode_thresholds[chan][1];
       threshold = mode_music + ((voice_est*voice_est*(mode_voice-mode_music))>>14);

       /* Hysteresis */
       if (st->prev_mode == MODE_CELT_ONLY)
           threshold -= 4000;
       else if (st->prev_mode>0)
           threshold += 4000;

       st->mode = (equiv_rate >= threshold) ? MODE_CELT_ONLY: MODE_SILK_ONLY;

       /* When FEC is enabled and there's enough packet loss, use SILK */
       if (st->silk_mode.useInBandFEC && st->silk_mode.packetLossPercentage > (128-voice_est)>>4)
          st->mode = MODE_SILK_ONLY;
       /* When encoding voice and DTX is enabled, set the encoder to SILK mode (at least for now) */
       if (st->silk_mode.useDTX && voice_est > 100)
          st->mode = MODE_SILK_ONLY;
#endif
    } else {
       st->mode = st->user_forced_mode;
    }

    /* Override the chosen mode to make sure we meet the requested frame size */
    if (st->mode != MODE_CELT_ONLY && frame_size < st->Fs/100)
       st->mode = MODE_CELT_ONLY;

    if (st->stream_channels == 1 && st->prev_channels ==2 && st->silk_mode.toMono==0
          && st->mode != MODE_CELT_ONLY && st->prev_mode != MODE_CELT_ONLY)
    {
       /* Delay stereo->mono transition by two frames so that SILK can do a smooth downmix */
       st->silk_mode.toMono = 1;
       st->stream_channels = 2;
    } else {
       st->silk_mode.toMono = 0;
    }

    if (st->prev_mode > 0 &&
        ((st->mode != MODE_CELT_ONLY && st->prev_mode == MODE_CELT_ONLY) ||
    (st->mode == MODE_CELT_ONLY && st->prev_mode != MODE_CELT_ONLY)))
    {
        redundancy = 1;
        celt_to_silk = (st->mode != MODE_CELT_ONLY);
        if (!celt_to_silk)
        {
            /* Switch to SILK/hybrid if frame size is 10 ms or more*/
            if (frame_size >= st->Fs/100)
            {
                st->mode = st->prev_mode;
                to_celt = 1;
            } else {
                redundancy=0;
            }
        }
    }
    /* For the first frame at a new SILK bandwidth */
    if (st->silk_bw_switch)
    {
       redundancy = 1;
       celt_to_silk = 1;
       st->silk_bw_switch = 0;
       prefill=1;
    }

    if (redundancy)
    {
       /* Fair share of the max size allowed */
       redundancy_bytes = IMIN(257, max_data_bytes*(opus_int32)(st->Fs/200)/(frame_size+st->Fs/200));
       /* For VBR, target the actual bitrate (subject to the limit above) */
       if (st->use_vbr)
          redundancy_bytes = IMIN(redundancy_bytes, st->bitrate_bps/1600);
    }

    if (st->mode != MODE_CELT_ONLY && st->prev_mode == MODE_CELT_ONLY)
    {
        silk_EncControlStruct dummy;
        silk_InitEncoder( silk_enc, &dummy);
        prefill=1;
    }

    /* Automatic (rate-dependent) bandwidth selection */
    if (st->mode == MODE_CELT_ONLY || st->first || st->silk_mode.allowBandwidthSwitch)
    {
        const opus_int32 *voice_bandwidth_thresholds, *music_bandwidth_thresholds;
        opus_int32 bandwidth_thresholds[8];
        int bandwidth = OPUS_BANDWIDTH_FULLBAND;
        opus_int32 equiv_rate2;

        equiv_rate2 = equiv_rate;
        if (st->mode != MODE_CELT_ONLY)
        {
           /* Adjust the threshold +/- 10% depending on complexity */
           equiv_rate2 = equiv_rate2 * (45+st->silk_mode.complexity)/50;
           /* CBR is less efficient by ~1 kb/s */
           if (!st->use_vbr)
              equiv_rate2 -= 1000;
        }
        if (st->channels==2 && st->force_channels!=1)
        {
           voice_bandwidth_thresholds = stereo_voice_bandwidth_thresholds;
           music_bandwidth_thresholds = stereo_music_bandwidth_thresholds;
        } else {
           voice_bandwidth_thresholds = mono_voice_bandwidth_thresholds;
           music_bandwidth_thresholds = mono_music_bandwidth_thresholds;
        }
        /* Interpolate bandwidth thresholds depending on voice estimation */
        for (i=0;i<8;i++)
        {
           bandwidth_thresholds[i] = music_bandwidth_thresholds[i]
                    + ((voice_est*voice_est*(voice_bandwidth_thresholds[i]-music_bandwidth_thresholds[i]))>>14);
        }
        do {
            int threshold, hysteresis;
            threshold = bandwidth_thresholds[2*(bandwidth-OPUS_BANDWIDTH_MEDIUMBAND)];
            hysteresis = bandwidth_thresholds[2*(bandwidth-OPUS_BANDWIDTH_MEDIUMBAND)+1];
            if (!st->first)
            {
                if (st->bandwidth >= bandwidth)
                    threshold -= hysteresis;
                else
                    threshold += hysteresis;
            }
            if (equiv_rate2 >= threshold)
                break;
        } while (--bandwidth>OPUS_BANDWIDTH_NARROWBAND);
        st->bandwidth = bandwidth;
        /* Prevents any transition to SWB/FB until the SILK layer has fully
           switched to WB mode and turned the variable LP filter off */
        if (!st->first && st->mode != MODE_CELT_ONLY && !st->silk_mode.inWBmodeWithoutVariableLP && st->bandwidth > OPUS_BANDWIDTH_WIDEBAND)
            st->bandwidth = OPUS_BANDWIDTH_WIDEBAND;
    }

    if (st->bandwidth>st->max_bandwidth)
       st->bandwidth = st->max_bandwidth;

    if (st->user_bandwidth != OPUS_AUTO)
        st->bandwidth = st->user_bandwidth;

    /* This prevents us from using hybrid at unsafe CBR/max rates */
    if (st->mode != MODE_CELT_ONLY && max_rate < 15000)
    {
       st->bandwidth = IMIN(st->bandwidth, OPUS_BANDWIDTH_WIDEBAND);
    }

    /* Prevents Opus from wasting bits on frequencies that are above
       the Nyquist rate of the input signal */
    if (st->Fs <= 24000 && st->bandwidth > OPUS_BANDWIDTH_SUPERWIDEBAND)
        st->bandwidth = OPUS_BANDWIDTH_SUPERWIDEBAND;
    if (st->Fs <= 16000 && st->bandwidth > OPUS_BANDWIDTH_WIDEBAND)
        st->bandwidth = OPUS_BANDWIDTH_WIDEBAND;
    if (st->Fs <= 12000 && st->bandwidth > OPUS_BANDWIDTH_MEDIUMBAND)
        st->bandwidth = OPUS_BANDWIDTH_MEDIUMBAND;
    if (st->Fs <= 8000 && st->bandwidth > OPUS_BANDWIDTH_NARROWBAND)
        st->bandwidth = OPUS_BANDWIDTH_NARROWBAND;

    /* If max_data_bytes represents less than 8 kb/s, switch to CELT-only mode */
    if (max_data_bytes < (frame_rate > 50 ? 12000 : 8000)*frame_size / (st->Fs * 8))
       st->mode = MODE_CELT_ONLY;

    /* CELT mode doesn't support mediumband, use wideband instead */
    if (st->mode == MODE_CELT_ONLY && st->bandwidth == OPUS_BANDWIDTH_MEDIUMBAND)
        st->bandwidth = OPUS_BANDWIDTH_WIDEBAND;

    /* Can't support higher than wideband for >20 ms frames */
    if (frame_size > st->Fs/50 && (st->mode == MODE_CELT_ONLY || st->bandwidth > OPUS_BANDWIDTH_WIDEBAND))
    {
       VARDECL(unsigned char, tmp_data);
       int nb_frames;
       int bak_mode, bak_bandwidth, bak_channels, bak_to_mono;
       OpusRepacketizer rp;
       opus_int32 bytes_per_frame;


       nb_frames = frame_size > st->Fs/25 ? 3 : 2;
       bytes_per_frame = IMIN(1276,(out_data_bytes-3)/nb_frames);

       ALLOC(tmp_data, nb_frames*bytes_per_frame, unsigned char);

       opus_repacketizer_init(&rp);

       bak_mode = st->user_forced_mode;
       bak_bandwidth = st->user_bandwidth;
       bak_channels = st->force_channels;

       st->user_forced_mode = st->mode;
       st->user_bandwidth = st->bandwidth;
       st->force_channels = st->stream_channels;
       bak_to_mono = st->silk_mode.toMono;

       if (bak_to_mono)
          st->force_channels = 1;
       else
          st->prev_channels = st->stream_channels;
       for (i=0;i<nb_frames;i++)
       {
          int tmp_len;
          st->silk_mode.toMono = 0;
          /* When switching from SILK/Hybrid to CELT, only ask for a switch at the last frame */
          if (to_celt && i==nb_frames-1)
             st->user_forced_mode = MODE_CELT_ONLY;
          tmp_len = opus_encode_native(st, pcm+i*(st->channels*st->Fs/50), st->Fs/50, tmp_data+i*bytes_per_frame, bytes_per_frame);
          if (tmp_len<0)
          {
             RESTORE_STACK;
             return OPUS_INTERNAL_ERROR;
          }
          ret = opus_repacketizer_cat(&rp, tmp_data+i*bytes_per_frame, tmp_len);
          if (ret<0)
          {
             RESTORE_STACK;
             return OPUS_INTERNAL_ERROR;
          }
       }
       ret = opus_repacketizer_out(&rp, data, out_data_bytes);
       if (ret<0)
       {
          RESTORE_STACK;
          return OPUS_INTERNAL_ERROR;
       }
       st->user_forced_mode = bak_mode;
       st->user_bandwidth = bak_bandwidth;
       st->force_channels = bak_channels;
       st->silk_mode.toMono = bak_to_mono;
       RESTORE_STACK;
       return ret;
    }

    curr_bandwidth = st->bandwidth;

    /* Chooses the appropriate mode for speech
       *NEVER* switch to/from CELT-only mode here as this will invalidate some assumptions */
    if (st->mode == MODE_SILK_ONLY && curr_bandwidth > OPUS_BANDWIDTH_WIDEBAND)
        st->mode = MODE_HYBRID;
    if (st->mode == MODE_HYBRID && curr_bandwidth <= OPUS_BANDWIDTH_WIDEBAND)
        st->mode = MODE_SILK_ONLY;

    /* printf("%d %d %d %d\n", st->bitrate_bps, st->stream_channels, st->mode, curr_bandwidth); */
    bytes_target = IMIN(max_data_bytes-redundancy_bytes, st->bitrate_bps * frame_size / (st->Fs * 8)) - 1;

    data += 1;

    ec_enc_init(&enc, data, max_data_bytes-1);

    ALLOC(pcm_buf, (delay_compensation+frame_size)*st->channels, opus_val16);
    for (i=0;i<delay_compensation*st->channels;i++)
       pcm_buf[i] = st->delay_buffer[(st->encoder_buffer-delay_compensation)*st->channels+i];

    if (st->mode == MODE_CELT_ONLY)
       hp_freq_smth1 = silk_LSHIFT( silk_lin2log( VARIABLE_HP_MIN_CUTOFF_HZ ), 8 );
    else
       hp_freq_smth1 = ((silk_encoder*)silk_enc)->state_Fxx[0].sCmn.variable_HP_smth1_Q15;

    st->variable_HP_smth2_Q15 = silk_SMLAWB( st->variable_HP_smth2_Q15,
          hp_freq_smth1 - st->variable_HP_smth2_Q15, SILK_FIX_CONST( VARIABLE_HP_SMTH_COEF2, 16 ) );

    /* convert from log scale to Hertz */
    cutoff_Hz = silk_log2lin( silk_RSHIFT( st->variable_HP_smth2_Q15, 8 ) );

    if (st->application == OPUS_APPLICATION_VOIP)
    {
       hp_cutoff(pcm, cutoff_Hz, &pcm_buf[delay_compensation*st->channels], st->hp_mem, frame_size, st->channels, st->Fs);
    } else {
       for (i=0;i<frame_size*st->channels;i++)
          pcm_buf[delay_compensation*st->channels + i] = pcm[i];
    }

    /* SILK processing */
    if (st->mode != MODE_CELT_ONLY)
    {
#ifdef FIXED_POINT
       const opus_int16 *pcm_silk;
#else
       VARDECL(opus_int16, pcm_silk);
       ALLOC(pcm_silk, st->channels*frame_size, opus_int16);
#endif
        st->silk_mode.bitRate = 8*bytes_target*frame_rate;
        if( st->mode == MODE_HYBRID ) {
            st->silk_mode.bitRate /= st->stream_channels;
            if( curr_bandwidth == OPUS_BANDWIDTH_SUPERWIDEBAND ) {
                if( st->Fs == 100 * frame_size ) {
                    /* 24 kHz, 10 ms */
                    st->silk_mode.bitRate = ( ( st->silk_mode.bitRate + 2000 + st->use_vbr * 1000 ) * 2 ) / 3;
                } else {
                    /* 24 kHz, 20 ms */
                    st->silk_mode.bitRate = ( ( st->silk_mode.bitRate + 1000 + st->use_vbr * 1000 ) * 2 ) / 3;
                }
            } else {
                if( st->Fs == 100 * frame_size ) {
                    /* 48 kHz, 10 ms */
                    st->silk_mode.bitRate = ( st->silk_mode.bitRate + 8000 + st->use_vbr * 3000 ) / 2;
                } else {
                    /* 48 kHz, 20 ms */
                    st->silk_mode.bitRate = ( st->silk_mode.bitRate + 9000 + st->use_vbr * 1000 ) / 2;
                }
            }
            st->silk_mode.bitRate *= st->stream_channels;
            /* don't let SILK use more than 80% */
            if( st->silk_mode.bitRate > ( st->bitrate_bps - 8*st->Fs/frame_size ) * 4/5 ) {
                st->silk_mode.bitRate = ( st->bitrate_bps - 8*st->Fs/frame_size ) * 4/5;
            }
        }

        st->silk_mode.payloadSize_ms = 1000 * frame_size / st->Fs;
        st->silk_mode.nChannelsAPI = st->channels;
        st->silk_mode.nChannelsInternal = st->stream_channels;
        if (curr_bandwidth == OPUS_BANDWIDTH_NARROWBAND) {
            st->silk_mode.desiredInternalSampleRate = 8000;
        } else if (curr_bandwidth == OPUS_BANDWIDTH_MEDIUMBAND) {
            st->silk_mode.desiredInternalSampleRate = 12000;
        } else {
            silk_assert( st->mode == MODE_HYBRID || curr_bandwidth == OPUS_BANDWIDTH_WIDEBAND );
            st->silk_mode.desiredInternalSampleRate = 16000;
        }
        if( st->mode == MODE_HYBRID ) {
            /* Don't allow bandwidth reduction at lowest bitrates in hybrid mode */
            st->silk_mode.minInternalSampleRate = 16000;
        } else {
            st->silk_mode.minInternalSampleRate = 8000;
        }

        if (st->mode == MODE_SILK_ONLY)
        {
           opus_int32 effective_max_rate = max_rate;
           st->silk_mode.maxInternalSampleRate = 16000;
           if (frame_rate > 50)
              effective_max_rate = effective_max_rate*2/3;
           if (effective_max_rate < 13000)
           {
              st->silk_mode.maxInternalSampleRate = 12000;
              st->silk_mode.desiredInternalSampleRate = IMIN(12000, st->silk_mode.desiredInternalSampleRate);
           }
           if (effective_max_rate < 9600)
           {
              st->silk_mode.maxInternalSampleRate = 8000;
              st->silk_mode.desiredInternalSampleRate = IMIN(8000, st->silk_mode.desiredInternalSampleRate);
           }
        } else {
           st->silk_mode.maxInternalSampleRate = 16000;
        }

        st->silk_mode.useCBR = !st->use_vbr;

        /* Call SILK encoder for the low band */
        nBytes = IMIN(1275, max_data_bytes-1-redundancy_bytes);

        st->silk_mode.maxBits = nBytes*8;
        /* Only allow up to 90% of the bits for hybrid mode*/
        if (st->mode == MODE_HYBRID)
           st->silk_mode.maxBits = (opus_int32)st->silk_mode.maxBits*9/10;
        if (st->silk_mode.useCBR)
        {
           st->silk_mode.maxBits = (st->silk_mode.bitRate * frame_size / (st->Fs * 8))*8;
           /* Reduce the initial target to make it easier to reach the CBR rate */
           st->silk_mode.bitRate = IMAX(1, st->silk_mode.bitRate-2000);
        }

        if (prefill)
        {
            opus_int32 zero=0;
#ifdef FIXED_POINT
            pcm_silk = st->delay_buffer;
#else
            for (i=0;i<st->encoder_buffer*st->channels;i++)
                pcm_silk[i] = FLOAT2INT16(st->delay_buffer[i]);
#endif
            silk_Encode( silk_enc, &st->silk_mode, pcm_silk, st->encoder_buffer, NULL, &zero, 1 );
        }

#ifdef FIXED_POINT
        pcm_silk = pcm_buf+delay_compensation*st->channels;
#else
        for (i=0;i<frame_size*st->channels;i++)
            pcm_silk[i] = FLOAT2INT16(pcm_buf[delay_compensation*st->channels + i]);
#endif
        ret = silk_Encode( silk_enc, &st->silk_mode, pcm_silk, frame_size, &enc, &nBytes, 0 );
        if( ret ) {
            /*fprintf (stderr, "SILK encode error: %d\n", ret);*/
            /* Handle error */
           RESTORE_STACK;
           return OPUS_INTERNAL_ERROR;
        }
        if (nBytes==0)
        {
           st->rangeFinal = 0;
           data[-1] = gen_toc(st->mode, st->Fs/frame_size, curr_bandwidth, st->stream_channels);
           RESTORE_STACK;
           return 1;
        }
        /* Extract SILK internal bandwidth for signaling in first byte */
        if( st->mode == MODE_SILK_ONLY ) {
            if( st->silk_mode.internalSampleRate == 8000 ) {
               curr_bandwidth = OPUS_BANDWIDTH_NARROWBAND;
            } else if( st->silk_mode.internalSampleRate == 12000 ) {
               curr_bandwidth = OPUS_BANDWIDTH_MEDIUMBAND;
            } else if( st->silk_mode.internalSampleRate == 16000 ) {
               curr_bandwidth = OPUS_BANDWIDTH_WIDEBAND;
            }
        } else {
            silk_assert( st->silk_mode.internalSampleRate == 16000 );
        }

        st->silk_mode.opusCanSwitch = st->silk_mode.switchReady;
        /* FIXME: How do we allocate the redundancy for CBR? */
        if (st->silk_mode.opusCanSwitch)
        {
           redundancy = 1;
           celt_to_silk = 0;
           st->silk_bw_switch = 1;
        }
    }

    /* CELT processing */
    {
        int endband=21;

        switch(curr_bandwidth)
        {
            case OPUS_BANDWIDTH_NARROWBAND:
                endband = 13;
                break;
            case OPUS_BANDWIDTH_MEDIUMBAND:
            case OPUS_BANDWIDTH_WIDEBAND:
                endband = 17;
                break;
            case OPUS_BANDWIDTH_SUPERWIDEBAND:
                endband = 19;
                break;
            case OPUS_BANDWIDTH_FULLBAND:
                endband = 21;
                break;
        }
        celt_encoder_ctl(celt_enc, CELT_SET_END_BAND(endband));
        celt_encoder_ctl(celt_enc, CELT_SET_CHANNELS(st->stream_channels));
    }
    celt_encoder_ctl(celt_enc, OPUS_SET_BITRATE(OPUS_BITRATE_MAX));
    if (st->mode != MODE_SILK_ONLY)
    {
        celt_encoder_ctl(celt_enc, OPUS_SET_VBR(0));
        /* Allow prediction unless we decide to disable it later */
        celt_encoder_ctl(celt_enc, CELT_SET_PREDICTION(2));

        if (st->mode == MODE_HYBRID)
        {
            int len;

            len = (ec_tell(&enc)+7)>>3;
            if (redundancy)
               len += st->mode == MODE_HYBRID ? 3 : 1;
            if( st->use_vbr ) {
                nb_compr_bytes = len + bytes_target - (st->silk_mode.bitRate * frame_size) / (8 * st->Fs);
            } else {
                /* check if SILK used up too much */
                nb_compr_bytes = len > bytes_target ? len : bytes_target;
            }
        } else {
            if (st->use_vbr)
            {
                celt_encoder_ctl(celt_enc, OPUS_SET_VBR(1));
                celt_encoder_ctl(celt_enc, OPUS_SET_VBR_CONSTRAINT(st->vbr_constraint));
                celt_encoder_ctl(celt_enc, OPUS_SET_BITRATE(st->bitrate_bps));
                nb_compr_bytes = max_data_bytes-1-redundancy_bytes;
            } else {
                nb_compr_bytes = bytes_target;
            }
        }

    } else {
        nb_compr_bytes = 0;
    }

    ALLOC(tmp_prefill, st->channels*st->Fs/400, opus_val16);
    if (st->mode != MODE_SILK_ONLY && st->mode != st->prev_mode && st->prev_mode > 0)
    {
       for (i=0;i<st->channels*st->Fs/400;i++)
          tmp_prefill[i] = st->delay_buffer[(st->encoder_buffer-st->delay_compensation-st->Fs/400)*st->channels + i];
    }

    for (i=0;i<st->channels*(st->encoder_buffer-(frame_size+delay_compensation));i++)
        st->delay_buffer[i] = st->delay_buffer[i+st->channels*frame_size];
    for (;i<st->encoder_buffer*st->channels;i++)
        st->delay_buffer[i] = pcm_buf[(frame_size+delay_compensation-st->encoder_buffer)*st->channels+i];


    if (st->mode != MODE_HYBRID || st->stream_channels==1)
       st->silk_mode.stereoWidth_Q14 = 1<<14;
    if( st->channels == 2 ) {
        /* Apply stereo width reduction (at low bitrates) */
        if( st->hybrid_stereo_width_Q14 < (1 << 14) || st->silk_mode.stereoWidth_Q14 < (1 << 14) ) {
            opus_val16 g1, g2;
            const CELTMode *celt_mode;

            celt_encoder_ctl(celt_enc, CELT_GET_MODE(&celt_mode));
            g1 = st->hybrid_stereo_width_Q14;
            g2 = (opus_val16)(st->silk_mode.stereoWidth_Q14);
#ifdef FIXED_POINT
            g1 = g1==16384 ? Q15ONE : SHL16(g1,1);
            g2 = g2==16384 ? Q15ONE : SHL16(g2,1);
#else
            g1 *= (1.f/16384);
            g2 *= (1.f/16384);
#endif
            stereo_fade(pcm_buf, pcm_buf, g1, g2, celt_mode->overlap,
                  frame_size, st->channels, celt_mode->window, st->Fs);
            st->hybrid_stereo_width_Q14 = st->silk_mode.stereoWidth_Q14;
        }
    }

    if ( st->mode != MODE_CELT_ONLY && ec_tell(&enc)+17+20*(st->mode == MODE_HYBRID) <= 8*(max_data_bytes-1))
    {
        /* For SILK mode, the redundancy is inferred from the length */
        if (st->mode == MODE_HYBRID && (redundancy || ec_tell(&enc)+37 <= 8*nb_compr_bytes))
           ec_enc_bit_logp(&enc, redundancy, 12);
        if (redundancy)
        {
            int max_redundancy;
            ec_enc_bit_logp(&enc, celt_to_silk, 1);
            if (st->mode == MODE_HYBRID)
               max_redundancy = (max_data_bytes-1)-nb_compr_bytes-1;
            else
               max_redundancy = (max_data_bytes-1)-((ec_tell(&enc)+7)>>3);
            /* Target the same bit-rate for redundancy as for the rest,
               up to a max of 257 bytes */
            redundancy_bytes = IMIN(max_redundancy, st->bitrate_bps/1600);
            redundancy_bytes = IMIN(257, IMAX(2, redundancy_bytes));
            if (st->mode == MODE_HYBRID)
                ec_enc_uint(&enc, redundancy_bytes-2, 256);
        }
    } else {
        redundancy = 0;
    }

    if (!redundancy)
    {
       st->silk_bw_switch = 0;
       redundancy_bytes = 0;
    }
    if (st->mode != MODE_CELT_ONLY)start_band=17;

    if (st->mode == MODE_SILK_ONLY)
    {
        ret = (ec_tell(&enc)+7)>>3;
        ec_enc_done(&enc);
        nb_compr_bytes = ret;
    } else {
       nb_compr_bytes = IMIN((max_data_bytes-1)-redundancy_bytes, nb_compr_bytes);
       ec_enc_shrink(&enc, nb_compr_bytes);
    }


    /* 5 ms redundant frame for CELT->SILK */
    if (redundancy && celt_to_silk)
    {
        int err;
        celt_encoder_ctl(celt_enc, CELT_SET_START_BAND(0));
        celt_encoder_ctl(celt_enc, OPUS_SET_VBR(0));
        err = celt_encode_with_ec(celt_enc, pcm_buf, st->Fs/200, data+nb_compr_bytes, redundancy_bytes, NULL);
        if (err < 0)
        {
           RESTORE_STACK;
           return OPUS_INTERNAL_ERROR;
        }
        celt_encoder_ctl(celt_enc, OPUS_GET_FINAL_RANGE(&redundant_rng));
        celt_encoder_ctl(celt_enc, OPUS_RESET_STATE);
    }

    celt_encoder_ctl(celt_enc, CELT_SET_START_BAND(start_band));

    if (st->mode != MODE_SILK_ONLY)
    {
        if (st->mode != st->prev_mode && st->prev_mode > 0)
        {
           unsigned char dummy[2];
           celt_encoder_ctl(celt_enc, OPUS_RESET_STATE);

           /* Prefilling */
           celt_encode_with_ec(celt_enc, tmp_prefill, st->Fs/400, dummy, 2, NULL);
           celt_encoder_ctl(celt_enc, CELT_SET_PREDICTION(0));
        }
        /* If false, we already busted the budget and we'll end up with a "PLC packet" */
        if (ec_tell(&enc) <= 8*nb_compr_bytes)
        {
           ret = celt_encode_with_ec(celt_enc, pcm_buf, frame_size, NULL, nb_compr_bytes, &enc);
           if (ret < 0)
           {
              RESTORE_STACK;
              return OPUS_INTERNAL_ERROR;
           }
        }
    }

    /* 5 ms redundant frame for SILK->CELT */
    if (redundancy && !celt_to_silk)
    {
        int err;
        unsigned char dummy[2];
        int N2, N4;
        N2 = st->Fs/200;
        N4 = st->Fs/400;

        celt_encoder_ctl(celt_enc, OPUS_RESET_STATE);
        celt_encoder_ctl(celt_enc, CELT_SET_START_BAND(0));
        celt_encoder_ctl(celt_enc, CELT_SET_PREDICTION(0));

        /* NOTE: We could speed this up slightly (at the expense of code size) by just adding a function that prefills the buffer */
        celt_encode_with_ec(celt_enc, pcm_buf+st->channels*(frame_size-N2-N4), N4, dummy, 2, NULL);

        err = celt_encode_with_ec(celt_enc, pcm_buf+st->channels*(frame_size-N2), N2, data+nb_compr_bytes, redundancy_bytes, NULL);
        if (err < 0)
        {
           RESTORE_STACK;
           return OPUS_INTERNAL_ERROR;
        }
        celt_encoder_ctl(celt_enc, OPUS_GET_FINAL_RANGE(&redundant_rng));
    }



    /* Signalling the mode in the first byte */
    data--;
    data[0] = gen_toc(st->mode, st->Fs/frame_size, curr_bandwidth, st->stream_channels);

    st->rangeFinal = enc.rng ^ redundant_rng;

    if (to_celt)
        st->prev_mode = MODE_CELT_ONLY;
    else
        st->prev_mode = st->mode;
    st->prev_channels = st->stream_channels;
    st->prev_framesize = frame_size;

    st->first = 0;

    /* In the unlikely case that the SILK encoder busted its target, tell
       the decoder to call the PLC */
    if (ec_tell(&enc) > (max_data_bytes-1)*8)
    {
       if (max_data_bytes < 2)
       {
          RESTORE_STACK;
          return OPUS_BUFFER_TOO_SMALL;
       }
       data[1] = 0;
       ret = 1;
       st->rangeFinal = 0;
    } else if (st->mode==MODE_SILK_ONLY&&!redundancy)
    {
       /*When in LPC only mode it's perfectly
         reasonable to strip off trailing zero bytes as
         the required range decoder behavior is to
         fill these in. This can't be done when the MDCT
         modes are used because the decoder needs to know
         the actual length for allocation purposes.*/
       while(ret>2&&data[ret]==0)ret--;
    }
    /* Count ToC and redundancy */
    ret += 1+redundancy_bytes;
    if (!st->use_vbr && ret >= 3)
    {
       if (pad_frame(data, ret, max_data_bytes))
       {
          RESTORE_STACK;
          return OPUS_INTERNAL_ERROR;
       }
       ret = max_data_bytes;
    }
    RESTORE_STACK;
    return ret;
}

#ifdef FIXED_POINT

#ifndef DISABLE_FLOAT_API
opus_int32 opus_encode_float(OpusEncoder *st, const float *pcm, int frame_size,
      unsigned char *data, opus_int32 max_data_bytes)
{
   int i, ret;
   VARDECL(opus_int16, in);
   ALLOC_STACK;

   if(frame_size<0)
   {
      RESTORE_STACK;
      return OPUS_BAD_ARG;
   }

   ALLOC(in, frame_size*st->channels, opus_int16);

   for (i=0;i<frame_size*st->channels;i++)
      in[i] = FLOAT2INT16(pcm[i]);
   ret = opus_encode(st, in, frame_size, data, max_data_bytes);
   RESTORE_STACK;
   return ret;
}
#endif

#else
opus_int32 opus_encode(OpusEncoder *st, const opus_int16 *pcm, int frame_size,
      unsigned char *data, opus_int32 max_data_bytes)
{
   int i, ret;
   VARDECL(float, in);
   ALLOC_STACK;

   ALLOC(in, frame_size*st->channels, float);

   for (i=0;i<frame_size*st->channels;i++)
      in[i] = (1.0f/32768)*pcm[i];
   ret = opus_encode_float(st, in, frame_size, data, max_data_bytes);
   RESTORE_STACK;
   return ret;
}
#endif


int opus_encoder_ctl(OpusEncoder *st, int request, ...)
{
    int ret;
    CELTEncoder *celt_enc;
    va_list ap;

    ret = OPUS_OK;
    va_start(ap, request);

    celt_enc = (CELTEncoder*)((char*)st+st->celt_enc_offset);

    switch (request)
    {
        case OPUS_SET_APPLICATION_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if (   (value != OPUS_APPLICATION_VOIP && value != OPUS_APPLICATION_AUDIO
                 && value != OPUS_APPLICATION_RESTRICTED_LOWDELAY)
               || (!st->first && st->application != value))
            {
               ret = OPUS_BAD_ARG;
               break;
            }
            st->application = value;
        }
        break;
        case OPUS_GET_APPLICATION_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->application;
        }
        break;
        case OPUS_SET_BITRATE_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if (value != OPUS_AUTO && value != OPUS_BITRATE_MAX)
            {
                if (value <= 0)
                    goto bad_arg;
                else if (value <= 500)
                    value = 500;
                else if (value > (opus_int32)300000*st->channels)
                    value = (opus_int32)300000*st->channels;
            }
            st->user_bitrate_bps = value;
        }
        break;
        case OPUS_GET_BITRATE_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = user_bitrate_to_bitrate(st, st->prev_framesize, 1276);
        }
        break;
        case OPUS_SET_FORCE_CHANNELS_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if((value<1 || value>st->channels) && value != OPUS_AUTO)
                return OPUS_BAD_ARG;
            st->force_channels = value;
        }
        break;
        case OPUS_GET_FORCE_CHANNELS_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->force_channels;
        }
        break;
        case OPUS_SET_MAX_BANDWIDTH_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if (value < OPUS_BANDWIDTH_NARROWBAND || value > OPUS_BANDWIDTH_FULLBAND)
                return OPUS_BAD_ARG;
            st->max_bandwidth = value;
            if (st->max_bandwidth == OPUS_BANDWIDTH_NARROWBAND) {
                st->silk_mode.maxInternalSampleRate = 8000;
            } else if (st->max_bandwidth == OPUS_BANDWIDTH_MEDIUMBAND) {
                st->silk_mode.maxInternalSampleRate = 12000;
            } else {
                st->silk_mode.maxInternalSampleRate = 16000;
            }
        }
        break;
        case OPUS_GET_MAX_BANDWIDTH_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->max_bandwidth;
        }
        break;
        case OPUS_SET_BANDWIDTH_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if ((value < OPUS_BANDWIDTH_NARROWBAND || value > OPUS_BANDWIDTH_FULLBAND) && value != OPUS_AUTO)
                return OPUS_BAD_ARG;
            st->user_bandwidth = value;
            if (st->user_bandwidth == OPUS_BANDWIDTH_NARROWBAND) {
                st->silk_mode.maxInternalSampleRate = 8000;
            } else if (st->user_bandwidth == OPUS_BANDWIDTH_MEDIUMBAND) {
                st->silk_mode.maxInternalSampleRate = 12000;
            } else {
                st->silk_mode.maxInternalSampleRate = 16000;
            }
        }
        break;
        case OPUS_GET_BANDWIDTH_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->bandwidth;
        }
        break;
        case OPUS_SET_DTX_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if(value<0 || value>1)
                return OPUS_BAD_ARG;
            st->silk_mode.useDTX = value;
        }
        break;
        case OPUS_GET_DTX_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->silk_mode.useDTX;
        }
        break;
        case OPUS_SET_COMPLEXITY_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if(value<0 || value>10)
                return OPUS_BAD_ARG;
            st->silk_mode.complexity = value;
            celt_encoder_ctl(celt_enc, OPUS_SET_COMPLEXITY(value));
        }
        break;
        case OPUS_GET_COMPLEXITY_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->silk_mode.complexity;
        }
        break;
        case OPUS_SET_INBAND_FEC_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if(value<0 || value>1)
                return OPUS_BAD_ARG;
            st->silk_mode.useInBandFEC = value;
        }
        break;
        case OPUS_GET_INBAND_FEC_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->silk_mode.useInBandFEC;
        }
        break;
        case OPUS_SET_PACKET_LOSS_PERC_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if (value < 0 || value > 100)
                return OPUS_BAD_ARG;
            st->silk_mode.packetLossPercentage = value;
            celt_encoder_ctl(celt_enc, OPUS_SET_PACKET_LOSS_PERC(value));
        }
        break;
        case OPUS_GET_PACKET_LOSS_PERC_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->silk_mode.packetLossPercentage;
        }
        break;
        case OPUS_SET_VBR_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if(value<0 || value>1)
                return OPUS_BAD_ARG;
            st->use_vbr = value;
            st->silk_mode.useCBR = 1-value;
        }
        break;
        case OPUS_GET_VBR_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->use_vbr;
        }
        break;
        case OPUS_SET_VOICE_RATIO_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if (value>100 || value<-1)
                goto bad_arg;
            st->voice_ratio = value;
        }
        break;
        case OPUS_GET_VOICE_RATIO_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->voice_ratio;
        }
        break;
        case OPUS_SET_VBR_CONSTRAINT_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if(value<0 || value>1)
                return OPUS_BAD_ARG;
            st->vbr_constraint = value;
        }
        break;
        case OPUS_GET_VBR_CONSTRAINT_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->vbr_constraint;
        }
        break;
        case OPUS_SET_SIGNAL_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if(value!=OPUS_AUTO && value!=OPUS_SIGNAL_VOICE && value!=OPUS_SIGNAL_MUSIC)
                return OPUS_BAD_ARG;
            st->signal_type = value;
        }
        break;
        case OPUS_GET_SIGNAL_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->signal_type;
        }
        break;
        case OPUS_GET_LOOKAHEAD_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            *value = st->Fs/400;
            if (st->application != OPUS_APPLICATION_RESTRICTED_LOWDELAY)
                *value += st->delay_compensation;
        }
        break;
        case OPUS_GET_SAMPLE_RATE_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            if (value==NULL)
            {
                ret = OPUS_BAD_ARG;
                break;
            }
            *value = st->Fs;
        }
        break;
        case OPUS_GET_FINAL_RANGE_REQUEST:
        {
            opus_uint32 *value = va_arg(ap, opus_uint32*);
            *value = st->rangeFinal;
        }
        break;
        case OPUS_SET_LSB_DEPTH_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            ret = celt_encoder_ctl(celt_enc, OPUS_SET_LSB_DEPTH(value));
        }
        break;
        case OPUS_GET_LSB_DEPTH_REQUEST:
        {
            opus_int32 *value = va_arg(ap, opus_int32*);
            celt_encoder_ctl(celt_enc, OPUS_GET_LSB_DEPTH(value));
        }
        break;
        case OPUS_RESET_STATE:
        {
           void *silk_enc;
           silk_EncControlStruct dummy;
           silk_enc = (char*)st+st->silk_enc_offset;

           OPUS_CLEAR((char*)&st->OPUS_ENCODER_RESET_START,
                 sizeof(OpusEncoder)-
                 ((char*)&st->OPUS_ENCODER_RESET_START - (char*)st));

           celt_encoder_ctl(celt_enc, OPUS_RESET_STATE);
           silk_InitEncoder( silk_enc, &dummy );
           st->stream_channels = st->channels;
           st->hybrid_stereo_width_Q14 = 1 << 14;
           st->first = 1;
           st->mode = MODE_HYBRID;
           st->bandwidth = OPUS_BANDWIDTH_FULLBAND;
           st->variable_HP_smth2_Q15 = silk_LSHIFT( silk_lin2log( VARIABLE_HP_MIN_CUTOFF_HZ ), 8 );
        }
        break;
        case OPUS_SET_FORCE_MODE_REQUEST:
        {
            opus_int32 value = va_arg(ap, opus_int32);
            if ((value < MODE_SILK_ONLY || value > MODE_CELT_ONLY) && value != OPUS_AUTO)
               goto bad_arg;
            st->user_forced_mode = value;
        }
        break;
        default:
            /* fprintf(stderr, "unknown opus_encoder_ctl() request: %d", request);*/
            ret = OPUS_UNIMPLEMENTED;
            break;
    }
    va_end(ap);
    return ret;
bad_arg:
    va_end(ap);
    return OPUS_BAD_ARG;
}

void opus_encoder_destroy(OpusEncoder *st)
{
    opus_free(st);
}
