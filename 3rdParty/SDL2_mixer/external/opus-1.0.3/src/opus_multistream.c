/* Copyright (c) 2011 Xiph.Org Foundation
   Written by Jean-Marc Valin */
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

#include "opus_multistream.h"
#include "opus.h"
#include "opus_private.h"
#include "stack_alloc.h"
#include <stdarg.h>
#include "float_cast.h"
#include "os_support.h"

typedef struct ChannelLayout {
   int nb_channels;
   int nb_streams;
   int nb_coupled_streams;
   unsigned char mapping[256];
} ChannelLayout;

typedef struct {
   int nb_streams;
   int nb_coupled_streams;
   unsigned char mapping[8];
} VorbisLayout;

#ifdef OPUS_ENABLE_ENCODER
/* Index is nb_channel-1*/
static const VorbisLayout vorbis_mappings[8] = {
      {1, 0, {0}},                      /* 1: mono */
      {1, 1, {0, 1}},                   /* 2: stereo */
      {2, 1, {0, 2, 1}},                /* 3: 1-d surround */
      {2, 2, {0, 1, 2, 3}},             /* 4: quadraphonic surround */
      {3, 2, {0, 4, 1, 2, 3}},          /* 5: 5-channel surround */
      {4, 2, {0, 4, 1, 2, 3, 5}},       /* 6: 5.1 surround */
      {4, 3, {0, 4, 1, 2, 3, 5, 6}},    /* 7: 6.1 surround */
      {5, 3, {0, 6, 1, 2, 3, 4, 5, 7}}, /* 8: 7.1 surround */
};

struct OpusMSEncoder {
   ChannelLayout layout;
   opus_int32 bitrate_bps;
   int surround;
   int lfe_stream;
   /* Encoder states go here */
};
#endif /* OPUS_ENABLE_ENCODER */

struct OpusMSDecoder {
   ChannelLayout layout;
   /* Decoder states go here */
};

#ifdef FIXED_POINT
#define opus_encode_native opus_encode
#else
#define opus_encode_native opus_encode_float
#endif

static int validate_layout(const ChannelLayout *layout)
{
   int i, max_channel;

   max_channel = layout->nb_streams+layout->nb_coupled_streams;
   if (max_channel>255)
      return 0;
   for (i=0;i<layout->nb_channels;i++)
   {
      if (layout->mapping[i] >= max_channel && layout->mapping[i] != 255)
         return 0;
   }
   return 1;
}


static int get_left_channel(const ChannelLayout *layout, int stream_id, int prev)
{
   int i;
   i = (prev<0) ? 0 : prev+1;
   for (;i<layout->nb_channels;i++)
   {
      if (layout->mapping[i]==stream_id*2)
         return i;
   }
   return -1;
}

static int get_right_channel(const ChannelLayout *layout, int stream_id, int prev)
{
   int i;
   i = (prev<0) ? 0 : prev+1;
   for (;i<layout->nb_channels;i++)
   {
      if (layout->mapping[i]==stream_id*2+1)
         return i;
   }
   return -1;
}

static int get_mono_channel(const ChannelLayout *layout, int stream_id, int prev)
{
   int i;
   i = (prev<0) ? 0 : prev+1;
   for (;i<layout->nb_channels;i++)
   {
      if (layout->mapping[i]==stream_id+layout->nb_coupled_streams)
         return i;
   }
   return -1;
}

#ifdef OPUS_ENABLE_ENCODER
static int validate_encoder_layout(const ChannelLayout *layout)
{
   int s;
   for (s=0;s<layout->nb_streams;s++)
   {
      if (s < layout->nb_coupled_streams)
      {
         if (get_left_channel(layout, s, -1)==-1)
            return 0;
         if (get_right_channel(layout, s, -1)==-1)
            return 0;
      } else {
         if (get_mono_channel(layout, s, -1)==-1)
            return 0;
      }
   }
   return 1;
}

opus_int32 opus_multistream_encoder_get_size(int nb_streams, int nb_coupled_streams)
{
   int coupled_size;
   int mono_size;

   if(nb_streams<1||nb_coupled_streams>nb_streams||nb_coupled_streams<0)return 0;
   coupled_size = opus_encoder_get_size(2);
   mono_size = opus_encoder_get_size(1);
   return align(sizeof(OpusMSEncoder))
        + nb_coupled_streams * align(coupled_size)
        + (nb_streams-nb_coupled_streams) * align(mono_size);
}

opus_int32 opus_multistream_surround_encoder_get_size(int channels, int mapping_family)
{
   int nb_streams;
   int nb_coupled_streams;
   opus_int32 size;

   if (mapping_family==0)
   {
      if (channels==1)
      {
         nb_streams=1;
         nb_coupled_streams=0;
      } else if (channels==2)
      {
         nb_streams=1;
         nb_coupled_streams=1;
      } else
         return 0;
   } else if (mapping_family==1 && channels<=8 && channels>=1)
   {
      nb_streams=vorbis_mappings[channels-1].nb_streams;
      nb_coupled_streams=vorbis_mappings[channels-1].nb_coupled_streams;
   } else if (mapping_family==255)
   {
      nb_streams=channels;
      nb_coupled_streams=0;
   } else
      return 0;
   size = opus_multistream_encoder_get_size(nb_streams, nb_coupled_streams);
   return size;
}


static int opus_multistream_encoder_init_impl(
      OpusMSEncoder *st,
      opus_int32 Fs,
      int channels,
      int streams,
      int coupled_streams,
      const unsigned char *mapping,
      int application,
      int surround
)
{
   int coupled_size;
   int mono_size;
   int i, ret;
   char *ptr;

   if ((channels>255) || (channels<1) || (coupled_streams>streams) ||
       (streams<1) || (coupled_streams<0) || (streams>255-coupled_streams))
      return OPUS_BAD_ARG;

   st->layout.nb_channels = channels;
   st->layout.nb_streams = streams;
   st->layout.nb_coupled_streams = coupled_streams;
   if (!surround)
      st->lfe_stream = -1;
   st->bitrate_bps = OPUS_AUTO;
   for (i=0;i<st->layout.nb_channels;i++)
      st->layout.mapping[i] = mapping[i];
   if (!validate_layout(&st->layout) || !validate_encoder_layout(&st->layout))
      return OPUS_BAD_ARG;
   ptr = (char*)st + align(sizeof(OpusMSEncoder));
   coupled_size = opus_encoder_get_size(2);
   mono_size = opus_encoder_get_size(1);

   for (i=0;i<st->layout.nb_coupled_streams;i++)
   {
      ret = opus_encoder_init((OpusEncoder*)ptr, Fs, 2, application);
      if(ret!=OPUS_OK)return ret;
      ptr += align(coupled_size);
   }
   for (;i<st->layout.nb_streams;i++)
   {
      ret = opus_encoder_init((OpusEncoder*)ptr, Fs, 1, application);
      if(ret!=OPUS_OK)return ret;
      ptr += align(mono_size);
   }
   st->surround = surround;
   return OPUS_OK;
}

int opus_multistream_encoder_init(
      OpusMSEncoder *st,
      opus_int32 Fs,
      int channels,
      int streams,
      int coupled_streams,
      const unsigned char *mapping,
      int application
)
{
   return opus_multistream_encoder_init_impl(st, Fs, channels, streams, coupled_streams, mapping, application, 0);
}

int opus_multistream_surround_encoder_init(
      OpusMSEncoder *st,
      opus_int32 Fs,
      int channels,
      int mapping_family,
      int *streams,
      int *coupled_streams,
      unsigned char *mapping,
      int application
)
{
   if ((channels>255) || (channels<1))
      return OPUS_BAD_ARG;
   st->lfe_stream = -1;
   if (mapping_family==0)
   {
      if (channels==1)
      {
         *streams=1;
         *coupled_streams=0;
         mapping[0]=0;
      } else if (channels==2)
      {
         *streams=1;
         *coupled_streams=1;
         mapping[0]=0;
         mapping[1]=1;
      } else
         return OPUS_UNIMPLEMENTED;
   } else if (mapping_family==1 && channels<=8 && channels>=1)
   {
      int i;
      *streams=vorbis_mappings[channels-1].nb_streams;
      *coupled_streams=vorbis_mappings[channels-1].nb_coupled_streams;
      for (i=0;i<channels;i++)
         mapping[i] = vorbis_mappings[channels-1].mapping[i];
      if (channels>=6)
         st->lfe_stream = *streams-1;
   } else if (mapping_family==255)
   {
      int i;
      *streams=channels;
      *coupled_streams=0;
      for(i=0;i<channels;i++)
         mapping[i] = i;
   } else
      return OPUS_UNIMPLEMENTED;
   return opus_multistream_encoder_init_impl(st, Fs, channels, *streams, *coupled_streams,
         mapping, application, channels>2&&mapping_family==1);
}

OpusMSEncoder *opus_multistream_encoder_create(
      opus_int32 Fs,
      int channels,
      int streams,
      int coupled_streams,
      const unsigned char *mapping,
      int application,
      int *error
)
{
   int ret;
   OpusMSEncoder *st;
   if ((channels>255) || (channels<1) || (coupled_streams>streams) ||
       (streams<1) || (coupled_streams<0) || (streams>255-coupled_streams))
   {
      if (error)
         *error = OPUS_BAD_ARG;
      return NULL;
   }
   st = (OpusMSEncoder *)opus_alloc(opus_multistream_encoder_get_size(streams, coupled_streams));
   if (st==NULL)
   {
      if (error)
         *error = OPUS_ALLOC_FAIL;
      return NULL;
   }
   ret = opus_multistream_encoder_init(st, Fs, channels, streams, coupled_streams, mapping, application);
   if (ret != OPUS_OK)
   {
      opus_free(st);
      st = NULL;
   }
   if (error)
      *error = ret;
   return st;
}

OpusMSEncoder *opus_multistream_surround_encoder_create(
      opus_int32 Fs,
      int channels,
      int mapping_family,
      int *streams,
      int *coupled_streams,
      unsigned char *mapping,
      int application,
      int *error
)
{
   int ret;
   opus_int32 size;
   OpusMSEncoder *st;
   if ((channels>255) || (channels<1))
   {
      if (error)
         *error = OPUS_BAD_ARG;
      return NULL;
   }
   size = opus_multistream_surround_encoder_get_size(channels, mapping_family);
   if (!size)
   {
      if (error)
         *error = OPUS_UNIMPLEMENTED;
      return NULL;
   }
   st = (OpusMSEncoder *)opus_alloc(size);
   if (st==NULL)
   {
      if (error)
         *error = OPUS_ALLOC_FAIL;
      return NULL;
   }
   ret = opus_multistream_surround_encoder_init(st, Fs, channels, mapping_family, streams, coupled_streams, mapping, application);
   if (ret != OPUS_OK)
   {
      opus_free(st);
      st = NULL;
   }
   if (error)
      *error = ret;
   return st;
}

typedef void (*opus_copy_channel_in_func)(
  opus_val16 *dst,
  int dst_stride,
  const void *src,
  int src_stride,
  int src_channel,
  int frame_size
);

static void surround_rate_allocation(
      OpusMSEncoder *st,
      opus_int32 *rate,
      int frame_size
      )
{
   int i;
   opus_int32 channel_rate;
   opus_int32 Fs;
   char *ptr;
   int stream_offset;
   int lfe_offset;
   int coupled_ratio; /* Q8 */
   int lfe_ratio;     /* Q8 */

   ptr = (char*)st + align(sizeof(OpusMSEncoder));
   opus_encoder_ctl((OpusEncoder*)ptr, OPUS_GET_SAMPLE_RATE(&Fs));

   /* We start by giving each stream (coupled or uncoupled) the same bitrate.
      This models the main saving of coupled channels over uncoupled. */
   stream_offset = 20000;
   /* The LFE stream is an exception to the above and gets fewer bits. */
   lfe_offset = 7000;
   /* Coupled streams get twice the mono rate after the first 20 kb/s. */
   coupled_ratio = 512;
   /* Should depend on the bitrate, for now we assume LFE gets 3/8 the bits of mono */
   lfe_ratio = 96;

   /* Compute bitrate allocation between streams */
   if (st->bitrate_bps==OPUS_AUTO)
   {
      channel_rate = Fs+60*Fs/frame_size;
   } else if (st->bitrate_bps==OPUS_BITRATE_MAX)
   {
      channel_rate = 300000;
   } else {
      int nb_lfe;
      int nb_uncoupled;
      int nb_coupled;
      int total;
      nb_lfe = (st->lfe_stream!=-1);
      nb_coupled = st->layout.nb_coupled_streams;
      nb_uncoupled = st->layout.nb_streams-nb_coupled-nb_lfe;
      total = (nb_uncoupled<<8)         /* mono */
            + coupled_ratio*nb_coupled /* stereo */
            + nb_lfe*lfe_ratio;
      channel_rate = 256*(st->bitrate_bps-lfe_offset*nb_lfe-stream_offset*(nb_coupled+nb_uncoupled))/total;
   }

   for (i=0;i<st->layout.nb_streams;i++)
   {
      if (i<st->layout.nb_coupled_streams)
         rate[i] = stream_offset+(channel_rate*coupled_ratio>>8);
      else if (i!=st->lfe_stream)
         rate[i] = stream_offset+channel_rate;
      else
         rate[i] = lfe_offset+(channel_rate*lfe_ratio>>8);
   }
}

/* Max size in case the encoder decides to return three frames */
#define MS_FRAME_TMP (3*1275+7)
static int opus_multistream_encode_native
(
    OpusMSEncoder *st,
    opus_copy_channel_in_func copy_channel_in,
    const void *pcm,
    int frame_size,
    unsigned char *data,
    opus_int32 max_data_bytes
)
{
   opus_int32 Fs;
   int coupled_size;
   int mono_size;
   int s;
   char *ptr;
   int tot_size;
   VARDECL(opus_val16, buf);
   unsigned char tmp_data[MS_FRAME_TMP];
   OpusRepacketizer rp;
   opus_int32 bitrates[256];
   ALLOC_STACK;

   ptr = (char*)st + align(sizeof(OpusMSEncoder));
   opus_encoder_ctl((OpusEncoder*)ptr, OPUS_GET_SAMPLE_RATE(&Fs));

   if (400*frame_size < Fs)
   {
      RESTORE_STACK;
      return OPUS_BAD_ARG;
   }
   /* Validate frame_size before using it to allocate stack space.
      This mirrors the checks in opus_encode[_float](). */
   if (400*frame_size != Fs && 200*frame_size != Fs &&
       100*frame_size != Fs &&  50*frame_size != Fs &&
        25*frame_size != Fs &&  50*frame_size != 3*Fs)
   {
      RESTORE_STACK;
      return OPUS_BAD_ARG;
   }
   ALLOC(buf, 2*frame_size, opus_val16);
   coupled_size = opus_encoder_get_size(2);
   mono_size = opus_encoder_get_size(1);

   if (max_data_bytes < 4*st->layout.nb_streams-1)
   {
      RESTORE_STACK;
      return OPUS_BUFFER_TOO_SMALL;
   }
   /* Compute bitrate allocation between streams (this could be a lot better) */
   surround_rate_allocation(st, bitrates, frame_size);

   ptr = (char*)st + align(sizeof(OpusMSEncoder));
   for (s=0;s<st->layout.nb_streams;s++)
   {
      OpusEncoder *enc;
      enc = (OpusEncoder*)ptr;
      if (s < st->layout.nb_coupled_streams)
         ptr += align(coupled_size);
      else
         ptr += align(mono_size);
      opus_encoder_ctl(enc, OPUS_SET_BITRATE(bitrates[s]));
      if (st->surround)
      {
         opus_encoder_ctl(enc, OPUS_SET_FORCE_MODE(MODE_CELT_ONLY));
         opus_encoder_ctl(enc, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_FULLBAND));
         if (s < st->layout.nb_coupled_streams)
            opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(2));
      }
   }

   ptr = (char*)st + align(sizeof(OpusMSEncoder));
   /* Counting ToC */
   tot_size = 0;
   for (s=0;s<st->layout.nb_streams;s++)
   {
      OpusEncoder *enc;
      int len;
      int curr_max;

      opus_repacketizer_init(&rp);
      enc = (OpusEncoder*)ptr;
      if (s < st->layout.nb_coupled_streams)
      {
         int left, right;
         left = get_left_channel(&st->layout, s, -1);
         right = get_right_channel(&st->layout, s, -1);
         (*copy_channel_in)(buf, 2,
            pcm, st->layout.nb_channels, left, frame_size);
         (*copy_channel_in)(buf+1, 2,
            pcm, st->layout.nb_channels, right, frame_size);
         ptr += align(coupled_size);
      } else {
         int chan = get_mono_channel(&st->layout, s, -1);
         (*copy_channel_in)(buf, 1,
            pcm, st->layout.nb_channels, chan, frame_size);
         ptr += align(mono_size);
      }
      /* number of bytes left (+Toc) */
      curr_max = max_data_bytes - tot_size;
      /* Reserve three bytes for the last stream and four for the others */
      curr_max -= IMAX(0,4*(st->layout.nb_streams-s-1)-1);
      curr_max = IMIN(curr_max,MS_FRAME_TMP);
      len = opus_encode_native(enc, buf, frame_size, tmp_data, curr_max);
      if (len<0)
      {
         RESTORE_STACK;
         return len;
      }
      /* We need to use the repacketizer to add the self-delimiting lengths
         while taking into account the fact that the encoder can now return
         more than one frame at a time (e.g. 60 ms CELT-only) */
      opus_repacketizer_cat(&rp, tmp_data, len);
      len = opus_repacketizer_out_range_impl(&rp, 0, opus_repacketizer_get_nb_frames(&rp), data, max_data_bytes-tot_size, s != st->layout.nb_streams-1);
      data += len;
      tot_size += len;
   }
   RESTORE_STACK;
   return tot_size;

}

#if !defined(DISABLE_FLOAT_API)
static void opus_copy_channel_in_float(
  opus_val16 *dst,
  int dst_stride,
  const void *src,
  int src_stride,
  int src_channel,
  int frame_size
)
{
   const float *float_src;
   int i;
   float_src = (const float *)src;
   for (i=0;i<frame_size;i++)
#if defined(FIXED_POINT)
      dst[i*dst_stride] = FLOAT2INT16(float_src[i*src_stride+src_channel]);
#else
      dst[i*dst_stride] = float_src[i*src_stride+src_channel];
#endif
}
#endif

static void opus_copy_channel_in_short(
  opus_val16 *dst,
  int dst_stride,
  const void *src,
  int src_stride,
  int src_channel,
  int frame_size
)
{
   const opus_int16 *short_src;
   int i;
   short_src = (const opus_int16 *)src;
   for (i=0;i<frame_size;i++)
#if defined(FIXED_POINT)
      dst[i*dst_stride] = short_src[i*src_stride+src_channel];
#else
      dst[i*dst_stride] = (1/32768.f)*short_src[i*src_stride+src_channel];
#endif
}

#ifdef FIXED_POINT
int opus_multistream_encode(
    OpusMSEncoder *st,
    const opus_val16 *pcm,
    int frame_size,
    unsigned char *data,
    opus_int32 max_data_bytes
)
{
   return opus_multistream_encode_native(st, opus_copy_channel_in_short,
      pcm, frame_size, data, max_data_bytes);
}

#ifndef DISABLE_FLOAT_API
int opus_multistream_encode_float(
    OpusMSEncoder *st,
    const float *pcm,
    int frame_size,
    unsigned char *data,
    opus_int32 max_data_bytes
)
{
   return opus_multistream_encode_native(st, opus_copy_channel_in_float,
      pcm, frame_size, data, max_data_bytes);
}
#endif

#else

int opus_multistream_encode_float
(
    OpusMSEncoder *st,
    const opus_val16 *pcm,
    int frame_size,
    unsigned char *data,
    opus_int32 max_data_bytes
)
{
   return opus_multistream_encode_native(st, opus_copy_channel_in_float,
      pcm, frame_size, data, max_data_bytes);
}

int opus_multistream_encode(
    OpusMSEncoder *st,
    const opus_int16 *pcm,
    int frame_size,
    unsigned char *data,
    opus_int32 max_data_bytes
)
{
   return opus_multistream_encode_native(st, opus_copy_channel_in_short,
      pcm, frame_size, data, max_data_bytes);
}
#endif

int opus_multistream_encoder_ctl(OpusMSEncoder *st, int request, ...)
{
   va_list ap;
   int coupled_size, mono_size;
   char *ptr;
   int ret = OPUS_OK;

   va_start(ap, request);

   coupled_size = opus_encoder_get_size(2);
   mono_size = opus_encoder_get_size(1);
   ptr = (char*)st + align(sizeof(OpusMSEncoder));
   switch (request)
   {
   case OPUS_SET_BITRATE_REQUEST:
   {
      opus_int32 value = va_arg(ap, opus_int32);
      if (value<0 && value!=OPUS_AUTO && value!=OPUS_BITRATE_MAX)
         goto bad_arg;
      st->bitrate_bps = value;
   }
   break;
   case OPUS_GET_BITRATE_REQUEST:
   {
      int s;
      opus_int32 *value = va_arg(ap, opus_int32*);
      *value = 0;
      for (s=0;s<st->layout.nb_streams;s++)
      {
         opus_int32 rate;
         OpusEncoder *enc;
         enc = (OpusEncoder*)ptr;
         if (s < st->layout.nb_coupled_streams)
            ptr += align(coupled_size);
         else
            ptr += align(mono_size);
         opus_encoder_ctl(enc, request, &rate);
         *value += rate;
      }
   }
   break;
   case OPUS_GET_LSB_DEPTH_REQUEST:
   case OPUS_GET_VBR_REQUEST:
   case OPUS_GET_APPLICATION_REQUEST:
   case OPUS_GET_BANDWIDTH_REQUEST:
   case OPUS_GET_COMPLEXITY_REQUEST:
   case OPUS_GET_PACKET_LOSS_PERC_REQUEST:
   case OPUS_GET_DTX_REQUEST:
   case OPUS_GET_VOICE_RATIO_REQUEST:
   case OPUS_GET_VBR_CONSTRAINT_REQUEST:
   case OPUS_GET_SIGNAL_REQUEST:
   case OPUS_GET_LOOKAHEAD_REQUEST:
   case OPUS_GET_SAMPLE_RATE_REQUEST:
   case OPUS_GET_INBAND_FEC_REQUEST:
   case OPUS_GET_FORCE_CHANNELS_REQUEST:
   {
      OpusEncoder *enc;
      /* For int32* GET params, just query the first stream */
      opus_int32 *value = va_arg(ap, opus_int32*);
      enc = (OpusEncoder*)ptr;
      ret = opus_encoder_ctl(enc, request, value);
   }
   break;
   case OPUS_GET_FINAL_RANGE_REQUEST:
   {
      int s;
      opus_uint32 *value = va_arg(ap, opus_uint32*);
      opus_uint32 tmp;
      *value=0;
      for (s=0;s<st->layout.nb_streams;s++)
      {
         OpusEncoder *enc;
         enc = (OpusEncoder*)ptr;
         if (s < st->layout.nb_coupled_streams)
            ptr += align(coupled_size);
         else
            ptr += align(mono_size);
         ret = opus_encoder_ctl(enc, request, &tmp);
         if (ret != OPUS_OK) break;
         *value ^= tmp;
      }
   }
   break;
   case OPUS_SET_LSB_DEPTH_REQUEST:
   case OPUS_SET_COMPLEXITY_REQUEST:
   case OPUS_SET_VBR_REQUEST:
   case OPUS_SET_VBR_CONSTRAINT_REQUEST:
   case OPUS_SET_BANDWIDTH_REQUEST:
   case OPUS_SET_SIGNAL_REQUEST:
   case OPUS_SET_APPLICATION_REQUEST:
   case OPUS_SET_INBAND_FEC_REQUEST:
   case OPUS_SET_PACKET_LOSS_PERC_REQUEST:
   case OPUS_SET_DTX_REQUEST:
   case OPUS_SET_FORCE_MODE_REQUEST:
   case OPUS_SET_FORCE_CHANNELS_REQUEST:
   {
      int s;
      /* This works for int32 params */
      opus_int32 value = va_arg(ap, opus_int32);
      for (s=0;s<st->layout.nb_streams;s++)
      {
         OpusEncoder *enc;

         enc = (OpusEncoder*)ptr;
         if (s < st->layout.nb_coupled_streams)
            ptr += align(coupled_size);
         else
            ptr += align(mono_size);
         ret = opus_encoder_ctl(enc, request, value);
         if (ret != OPUS_OK)
            break;
      }
   }
   break;
   case OPUS_MULTISTREAM_GET_ENCODER_STATE_REQUEST:
   {
      int s;
      opus_int32 stream_id;
      OpusEncoder **value;
      stream_id = va_arg(ap, opus_int32);
      if (stream_id<0 || stream_id >= st->layout.nb_streams)
         ret = OPUS_BAD_ARG;
      value = va_arg(ap, OpusEncoder**);
      for (s=0;s<stream_id;s++)
      {
         if (s < st->layout.nb_coupled_streams)
            ptr += align(coupled_size);
         else
            ptr += align(mono_size);
      }
      *value = (OpusEncoder*)ptr;
   }
      break;
   default:
      ret = OPUS_UNIMPLEMENTED;
      break;
   }

   va_end(ap);
   return ret;
bad_arg:
   va_end(ap);
   return OPUS_BAD_ARG;
}

void opus_multistream_encoder_destroy(OpusMSEncoder *st)
{
    opus_free(st);
}
#endif /* OPUS_ENABLE_ENCODER */


/* DECODER */

opus_int32 opus_multistream_decoder_get_size(int nb_streams, int nb_coupled_streams)
{
   int coupled_size;
   int mono_size;

   if(nb_streams<1||nb_coupled_streams>nb_streams||nb_coupled_streams<0)return 0;
   coupled_size = opus_decoder_get_size(2);
   mono_size = opus_decoder_get_size(1);
   return align(sizeof(OpusMSDecoder))
         + nb_coupled_streams * align(coupled_size)
         + (nb_streams-nb_coupled_streams) * align(mono_size);
}

int opus_multistream_decoder_init(
      OpusMSDecoder *st,
      opus_int32 Fs,
      int channels,
      int streams,
      int coupled_streams,
      const unsigned char *mapping
)
{
   int coupled_size;
   int mono_size;
   int i, ret;
   char *ptr;

   if ((channels>255) || (channels<1) || (coupled_streams>streams) ||
       (streams<1) || (coupled_streams<0) || (streams>255-coupled_streams))
      return OPUS_BAD_ARG;

   st->layout.nb_channels = channels;
   st->layout.nb_streams = streams;
   st->layout.nb_coupled_streams = coupled_streams;

   for (i=0;i<st->layout.nb_channels;i++)
      st->layout.mapping[i] = mapping[i];
   if (!validate_layout(&st->layout))
      return OPUS_BAD_ARG;

   ptr = (char*)st + align(sizeof(OpusMSDecoder));
   coupled_size = opus_decoder_get_size(2);
   mono_size = opus_decoder_get_size(1);

   for (i=0;i<st->layout.nb_coupled_streams;i++)
   {
      ret=opus_decoder_init((OpusDecoder*)ptr, Fs, 2);
      if(ret!=OPUS_OK)return ret;
      ptr += align(coupled_size);
   }
   for (;i<st->layout.nb_streams;i++)
   {
      ret=opus_decoder_init((OpusDecoder*)ptr, Fs, 1);
      if(ret!=OPUS_OK)return ret;
      ptr += align(mono_size);
   }
   return OPUS_OK;
}


OpusMSDecoder *opus_multistream_decoder_create(
      opus_int32 Fs,
      int channels,
      int streams,
      int coupled_streams,
      const unsigned char *mapping,
      int *error
)
{
   int ret;
   OpusMSDecoder *st;
   if ((channels>255) || (channels<1) || (coupled_streams>streams) ||
       (streams<1) || (coupled_streams<0) || (streams>255-coupled_streams))
   {
      if (error)
         *error = OPUS_BAD_ARG;
      return NULL;
   }
   st = (OpusMSDecoder *)opus_alloc(opus_multistream_decoder_get_size(streams, coupled_streams));
   if (st==NULL)
   {
      if (error)
         *error = OPUS_ALLOC_FAIL;
      return NULL;
   }
   ret = opus_multistream_decoder_init(st, Fs, channels, streams, coupled_streams, mapping);
   if (error)
      *error = ret;
   if (ret != OPUS_OK)
   {
      opus_free(st);
      st = NULL;
   }
   return st;
}

typedef void (*opus_copy_channel_out_func)(
  void *dst,
  int dst_stride,
  int dst_channel,
  const opus_val16 *src,
  int src_stride,
  int frame_size
);

static int opus_multistream_decode_native(
      OpusMSDecoder *st,
      const unsigned char *data,
      opus_int32 len,
      void *pcm,
      opus_copy_channel_out_func copy_channel_out,
      int frame_size,
      int decode_fec
)
{
   opus_int32 Fs;
   int coupled_size;
   int mono_size;
   int s, c;
   char *ptr;
   int do_plc=0;
   VARDECL(opus_val16, buf);
   ALLOC_STACK;

   /* Limit frame_size to avoid excessive stack allocations. */
   opus_multistream_decoder_ctl(st, OPUS_GET_SAMPLE_RATE(&Fs));
   frame_size = IMIN(frame_size, Fs/25*3);
   ALLOC(buf, 2*frame_size, opus_val16);
   ptr = (char*)st + align(sizeof(OpusMSDecoder));
   coupled_size = opus_decoder_get_size(2);
   mono_size = opus_decoder_get_size(1);

   if (len==0)
      do_plc = 1;
   if (len < 0)
      return OPUS_BAD_ARG;
   if (!do_plc && len < 2*st->layout.nb_streams-1)
      return OPUS_INVALID_PACKET;
   for (s=0;s<st->layout.nb_streams;s++)
   {
      OpusDecoder *dec;
      int packet_offset, ret;

      dec = (OpusDecoder*)ptr;
      ptr += (s < st->layout.nb_coupled_streams) ? align(coupled_size) : align(mono_size);

      if (!do_plc && len<=0)
      {
         RESTORE_STACK;
         return OPUS_INVALID_PACKET;
      }
      packet_offset = 0;
      ret = opus_decode_native(dec, data, len, buf, frame_size, decode_fec, s!=st->layout.nb_streams-1, &packet_offset);
      data += packet_offset;
      len -= packet_offset;
      if (ret > frame_size)
      {
         RESTORE_STACK;
         return OPUS_BUFFER_TOO_SMALL;
      }
      if (s>0 && ret != frame_size)
      {
         RESTORE_STACK;
         return OPUS_INVALID_PACKET;
      }
      if (ret <= 0)
      {
         RESTORE_STACK;
         return ret;
      }
      frame_size = ret;
      if (s < st->layout.nb_coupled_streams)
      {
         int chan, prev;
         prev = -1;
         /* Copy "left" audio to the channel(s) where it belongs */
         while ( (chan = get_left_channel(&st->layout, s, prev)) != -1)
         {
            (*copy_channel_out)(pcm, st->layout.nb_channels, chan,
               buf, 2, frame_size);
            prev = chan;
         }
         prev = -1;
         /* Copy "right" audio to the channel(s) where it belongs */
         while ( (chan = get_right_channel(&st->layout, s, prev)) != -1)
         {
            (*copy_channel_out)(pcm, st->layout.nb_channels, chan,
               buf+1, 2, frame_size);
            prev = chan;
         }
      } else {
         int chan, prev;
         prev = -1;
         /* Copy audio to the channel(s) where it belongs */
         while ( (chan = get_mono_channel(&st->layout, s, prev)) != -1)
         {
            (*copy_channel_out)(pcm, st->layout.nb_channels, chan,
               buf, 1, frame_size);
            prev = chan;
         }
      }
   }
   /* Handle muted channels */
   for (c=0;c<st->layout.nb_channels;c++)
   {
      if (st->layout.mapping[c] == 255)
      {
         (*copy_channel_out)(pcm, st->layout.nb_channels, c,
            NULL, 0, frame_size);
      }
   }
   RESTORE_STACK;
   return frame_size;
}

#if !defined(DISABLE_FLOAT_API)
static void opus_copy_channel_out_float(
  void *dst,
  int dst_stride,
  int dst_channel,
  const opus_val16 *src,
  int src_stride,
  int frame_size
)
{
   float *float_dst;
   int i;
   float_dst = (float*)dst;
   if (src != NULL)
   {
      for (i=0;i<frame_size;i++)
#if defined(FIXED_POINT)
         float_dst[i*dst_stride+dst_channel] = (1/32768.f)*src[i*src_stride];
#else
         float_dst[i*dst_stride+dst_channel] = src[i*src_stride];
#endif
   }
   else
   {
      for (i=0;i<frame_size;i++)
         float_dst[i*dst_stride+dst_channel] = 0;
   }
}
#endif

static void opus_copy_channel_out_short(
  void *dst,
  int dst_stride,
  int dst_channel,
  const opus_val16 *src,
  int src_stride,
  int frame_size
)
{
   opus_int16 *short_dst;
   int i;
   short_dst = (opus_int16*)dst;
   if (src != NULL)
   {
      for (i=0;i<frame_size;i++)
#if defined(FIXED_POINT)
         short_dst[i*dst_stride+dst_channel] = src[i*src_stride];
#else
         short_dst[i*dst_stride+dst_channel] = FLOAT2INT16(src[i*src_stride]);
#endif
   }
   else
   {
      for (i=0;i<frame_size;i++)
         short_dst[i*dst_stride+dst_channel] = 0;
   }
}



#ifdef FIXED_POINT
int opus_multistream_decode(
      OpusMSDecoder *st,
      const unsigned char *data,
      opus_int32 len,
      opus_int16 *pcm,
      int frame_size,
      int decode_fec
)
{
   return opus_multistream_decode_native(st, data, len,
       pcm, opus_copy_channel_out_short, frame_size, decode_fec);
}

#ifndef DISABLE_FLOAT_API
int opus_multistream_decode_float(OpusMSDecoder *st, const unsigned char *data,
      opus_int32 len, float *pcm, int frame_size, int decode_fec)
{
   return opus_multistream_decode_native(st, data, len,
       pcm, opus_copy_channel_out_float, frame_size, decode_fec);
}
#endif

#else

int opus_multistream_decode(OpusMSDecoder *st, const unsigned char *data,
      opus_int32 len, opus_int16 *pcm, int frame_size, int decode_fec)
{
   return opus_multistream_decode_native(st, data, len,
       pcm, opus_copy_channel_out_short, frame_size, decode_fec);
}

int opus_multistream_decode_float(
      OpusMSDecoder *st,
      const unsigned char *data,
      opus_int32 len,
      float *pcm,
      int frame_size,
      int decode_fec
)
{
   return opus_multistream_decode_native(st, data, len,
       pcm, opus_copy_channel_out_float, frame_size, decode_fec);
}
#endif

int opus_multistream_decoder_ctl(OpusMSDecoder *st, int request, ...)
{
   va_list ap;
   int coupled_size, mono_size;
   char *ptr;
   int ret = OPUS_OK;

   va_start(ap, request);

   coupled_size = opus_decoder_get_size(2);
   mono_size = opus_decoder_get_size(1);
   ptr = (char*)st + align(sizeof(OpusMSDecoder));
   switch (request)
   {
       case OPUS_GET_BANDWIDTH_REQUEST:
       case OPUS_GET_SAMPLE_RATE_REQUEST:
       {
          OpusDecoder *dec;
          /* For int32* GET params, just query the first stream */
          opus_int32 *value = va_arg(ap, opus_int32*);
          dec = (OpusDecoder*)ptr;
          ret = opus_decoder_ctl(dec, request, value);
       }
       break;
       case OPUS_GET_FINAL_RANGE_REQUEST:
       {
          int s;
          opus_uint32 *value = va_arg(ap, opus_uint32*);
          opus_uint32 tmp;
          *value = 0;
          for (s=0;s<st->layout.nb_streams;s++)
          {
             OpusDecoder *dec;
             dec = (OpusDecoder*)ptr;
             if (s < st->layout.nb_coupled_streams)
                ptr += align(coupled_size);
             else
                ptr += align(mono_size);
             ret = opus_decoder_ctl(dec, request, &tmp);
             if (ret != OPUS_OK) break;
             *value ^= tmp;
          }
       }
       break;
       case OPUS_RESET_STATE:
       {
          int s;
          for (s=0;s<st->layout.nb_streams;s++)
          {
             OpusDecoder *dec;

             dec = (OpusDecoder*)ptr;
             if (s < st->layout.nb_coupled_streams)
                ptr += align(coupled_size);
             else
                ptr += align(mono_size);
             ret = opus_decoder_ctl(dec, OPUS_RESET_STATE);
             if (ret != OPUS_OK)
                break;
          }
       }
       break;
       case OPUS_MULTISTREAM_GET_DECODER_STATE_REQUEST:
       {
          int s;
          opus_int32 stream_id;
          OpusDecoder **value;
          stream_id = va_arg(ap, opus_int32);
          if (stream_id<0 || stream_id >= st->layout.nb_streams)
             ret = OPUS_BAD_ARG;
          value = va_arg(ap, OpusDecoder**);
          for (s=0;s<stream_id;s++)
          {
             if (s < st->layout.nb_coupled_streams)
                ptr += align(coupled_size);
             else
                ptr += align(mono_size);
          }
          *value = (OpusDecoder*)ptr;
       }
       break;
       case OPUS_SET_GAIN_REQUEST:
       {
          int s;
          /* This works for int32 params */
          opus_int32 value = va_arg(ap, opus_int32);
          for (s=0;s<st->layout.nb_streams;s++)
          {
             OpusDecoder *dec;

             dec = (OpusDecoder*)ptr;
             if (s < st->layout.nb_coupled_streams)
                ptr += align(coupled_size);
             else
                ptr += align(mono_size);
             ret = opus_decoder_ctl(dec, request, value);
             if (ret != OPUS_OK)
                break;
          }
       }
       break;
       default:
          ret = OPUS_UNIMPLEMENTED;
       break;
   }

   va_end(ap);
   return ret;
}


void opus_multistream_decoder_destroy(OpusMSDecoder *st)
{
    opus_free(st);
}
