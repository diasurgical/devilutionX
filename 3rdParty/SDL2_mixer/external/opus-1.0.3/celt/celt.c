/* Copyright (c) 2007-2008 CSIRO
   Copyright (c) 2007-2010 Xiph.Org Foundation
   Copyright (c) 2008 Gregory Maxwell
   Written by Jean-Marc Valin and Gregory Maxwell */
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

#define CELT_C

#include "os_support.h"
#include "mdct.h"
#include <math.h>
#include "celt.h"
#include "pitch.h"
#include "bands.h"
#include "modes.h"
#include "entcode.h"
#include "quant_bands.h"
#include "rate.h"
#include "stack_alloc.h"
#include "mathops.h"
#include "float_cast.h"
#include <stdarg.h>
#include "celt_lpc.h"
#include "vq.h"

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

#ifdef CUSTOM_MODES
#define OPUS_CUSTOM_NOSTATIC
#else
#define OPUS_CUSTOM_NOSTATIC static inline
#endif

static const unsigned char trim_icdf[11] = {126, 124, 119, 109, 87, 41, 19, 9, 4, 2, 0};
/* Probs: NONE: 21.875%, LIGHT: 6.25%, NORMAL: 65.625%, AGGRESSIVE: 6.25% */
static const unsigned char spread_icdf[4] = {25, 23, 2, 0};

static const unsigned char tapset_icdf[3]={2,1,0};

#ifdef CUSTOM_MODES
static const unsigned char toOpusTable[20] = {
      0xE0, 0xE8, 0xF0, 0xF8,
      0xC0, 0xC8, 0xD0, 0xD8,
      0xA0, 0xA8, 0xB0, 0xB8,
      0x00, 0x00, 0x00, 0x00,
      0x80, 0x88, 0x90, 0x98,
};

static const unsigned char fromOpusTable[16] = {
      0x80, 0x88, 0x90, 0x98,
      0x40, 0x48, 0x50, 0x58,
      0x20, 0x28, 0x30, 0x38,
      0x00, 0x08, 0x10, 0x18
};

static inline int toOpus(unsigned char c)
{
   int ret=0;
   if (c<0xA0)
      ret = toOpusTable[c>>3];
   if (ret == 0)
      return -1;
   else
      return ret|(c&0x7);
}

static inline int fromOpus(unsigned char c)
{
   if (c<0x80)
      return -1;
   else
      return fromOpusTable[(c>>3)-16] | (c&0x7);
}
#endif /* CUSTOM_MODES */

#define COMBFILTER_MAXPERIOD 1024
#define COMBFILTER_MINPERIOD 15

static int resampling_factor(opus_int32 rate)
{
   int ret;
   switch (rate)
   {
   case 48000:
      ret = 1;
      break;
   case 24000:
      ret = 2;
      break;
   case 16000:
      ret = 3;
      break;
   case 12000:
      ret = 4;
      break;
   case 8000:
      ret = 6;
      break;
   default:
#ifndef CUSTOM_MODES
      celt_assert(0);
#endif
      ret = 0;
      break;
   }
   return ret;
}

#ifdef OPUS_ENABLE_ENCODER
/** Encoder state
 @brief Encoder state
 */
struct OpusCustomEncoder {
   const OpusCustomMode *mode;     /**< Mode used by the encoder */
   int overlap;
   int channels;
   int stream_channels;

   int force_intra;
   int clip;
   int disable_pf;
   int complexity;
   int upsample;
   int start, end;

   opus_int32 bitrate;
   int vbr;
   int signalling;
   int constrained_vbr;      /* If zero, VBR can do whatever it likes with the rate */
   int loss_rate;
   int lsb_depth;

   /* Everything beyond this point gets cleared on a reset */
#define ENCODER_RESET_START rng

   opus_uint32 rng;
   int spread_decision;
   opus_val32 delayedIntra;
   int tonal_average;
   int lastCodedBands;
   int hf_average;
   int tapset_decision;

   int prefilter_period;
   opus_val16 prefilter_gain;
   int prefilter_tapset;
#ifdef RESYNTH
   int prefilter_period_old;
   opus_val16 prefilter_gain_old;
   int prefilter_tapset_old;
#endif
   int consec_transient;

   opus_val32 preemph_memE[2];
   opus_val32 preemph_memD[2];

   /* VBR-related parameters */
   opus_int32 vbr_reservoir;
   opus_int32 vbr_drift;
   opus_int32 vbr_offset;
   opus_int32 vbr_count;

#ifdef RESYNTH
   celt_sig syn_mem[2][2*MAX_PERIOD];
#endif

   celt_sig in_mem[1]; /* Size = channels*mode->overlap */
   /* celt_sig prefilter_mem[],  Size = channels*COMBFILTER_MAXPERIOD */
   /* opus_val16 oldBandE[],     Size = channels*mode->nbEBands */
   /* opus_val16 oldLogE[],      Size = channels*mode->nbEBands */
   /* opus_val16 oldLogE2[],     Size = channels*mode->nbEBands */
#ifdef RESYNTH
   /* opus_val16 overlap_mem[],  Size = channels*overlap */
#endif
};

int celt_encoder_get_size(int channels)
{
   CELTMode *mode = opus_custom_mode_create(48000, 960, NULL);
   return opus_custom_encoder_get_size(mode, channels);
}

OPUS_CUSTOM_NOSTATIC int opus_custom_encoder_get_size(const CELTMode *mode, int channels)
{
   int size = sizeof(struct CELTEncoder)
         + (channels*mode->overlap-1)*sizeof(celt_sig)    /* celt_sig in_mem[channels*mode->overlap]; */
         + channels*COMBFILTER_MAXPERIOD*sizeof(celt_sig) /* celt_sig prefilter_mem[channels*COMBFILTER_MAXPERIOD]; */
         + 3*channels*mode->nbEBands*sizeof(opus_val16);  /* opus_val16 oldBandE[channels*mode->nbEBands]; */
                                                          /* opus_val16 oldLogE[channels*mode->nbEBands]; */
                                                          /* opus_val16 oldLogE2[channels*mode->nbEBands]; */
#ifdef RESYNTH
   size += channels*mode->overlap*sizeof(celt_sig);       /* celt_sig overlap_mem[channels*mode->nbEBands]; */
#endif
   return size;
}

#ifdef CUSTOM_MODES
CELTEncoder *opus_custom_encoder_create(const CELTMode *mode, int channels, int *error)
{
   int ret;
   CELTEncoder *st = (CELTEncoder *)opus_alloc(opus_custom_encoder_get_size(mode, channels));
   /* init will handle the NULL case */
   ret = opus_custom_encoder_init(st, mode, channels);
   if (ret != OPUS_OK)
   {
      opus_custom_encoder_destroy(st);
      st = NULL;
   }
   if (error)
      *error = ret;
   return st;
}
#endif /* CUSTOM_MODES */

int celt_encoder_init(CELTEncoder *st, opus_int32 sampling_rate, int channels)
{
   int ret;
   ret = opus_custom_encoder_init(st, opus_custom_mode_create(48000, 960, NULL), channels);
   if (ret != OPUS_OK)
      return ret;
   st->upsample = resampling_factor(sampling_rate);
   return OPUS_OK;
}

OPUS_CUSTOM_NOSTATIC int opus_custom_encoder_init(CELTEncoder *st, const CELTMode *mode, int channels)
{
   if (channels < 0 || channels > 2)
      return OPUS_BAD_ARG;

   if (st==NULL || mode==NULL)
      return OPUS_ALLOC_FAIL;

   OPUS_CLEAR((char*)st, opus_custom_encoder_get_size(mode, channels));

   st->mode = mode;
   st->overlap = mode->overlap;
   st->stream_channels = st->channels = channels;

   st->upsample = 1;
   st->start = 0;
   st->end = st->mode->effEBands;
   st->signalling = 1;

   st->constrained_vbr = 1;
   st->clip = 1;

   st->bitrate = OPUS_BITRATE_MAX;
   st->vbr = 0;
   st->force_intra  = 0;
   st->complexity = 5;
   st->lsb_depth=24;

   opus_custom_encoder_ctl(st, OPUS_RESET_STATE);

   return OPUS_OK;
}
#endif /* OPUS_ENABLE_ENCODER */

#ifdef CUSTOM_MODES
void opus_custom_encoder_destroy(CELTEncoder *st)
{
   opus_free(st);
}
#endif /* CUSTOM_MODES */

static inline opus_val16 SIG2WORD16(celt_sig x)
{
#ifdef FIXED_POINT
   x = PSHR32(x, SIG_SHIFT);
   x = MAX32(x, -32768);
   x = MIN32(x, 32767);
   return EXTRACT16(x);
#else
   return (opus_val16)x;
#endif
}

#ifdef OPUS_ENABLE_ENCODER
static int transient_analysis(const opus_val32 * OPUS_RESTRICT in, int len, int C,
                              int overlap)
{
   int i;
   VARDECL(opus_val16, tmp);
   opus_val32 mem0=0,mem1=0;
   int is_transient = 0;
   int block;
   int N;
   VARDECL(opus_val16, bins);
   SAVE_STACK;
   ALLOC(tmp, len, opus_val16);

   block = overlap/2;
   N=len/block;
   ALLOC(bins, N, opus_val16);
   if (C==1)
   {
      for (i=0;i<len;i++)
         tmp[i] = SHR32(in[i],SIG_SHIFT);
   } else {
      for (i=0;i<len;i++)
         tmp[i] = SHR32(ADD32(in[i],in[i+len]), SIG_SHIFT+1);
   }

   /* High-pass filter: (1 - 2*z^-1 + z^-2) / (1 - z^-1 + .5*z^-2) */
   for (i=0;i<len;i++)
   {
      opus_val32 x,y;
      x = tmp[i];
      y = ADD32(mem0, x);
#ifdef FIXED_POINT
      mem0 = mem1 + y - SHL32(x,1);
      mem1 = x - SHR32(y,1);
#else
      mem0 = mem1 + y - 2*x;
      mem1 = x - .5f*y;
#endif
      tmp[i] = EXTRACT16(SHR32(y,2));
   }
   /* First few samples are bad because we don't propagate the memory */
   for (i=0;i<12;i++)
      tmp[i] = 0;

   for (i=0;i<N;i++)
   {
      int j;
      opus_val16 max_abs=0;
      for (j=0;j<block;j++)
         max_abs = MAX16(max_abs, ABS16(tmp[i*block+j]));
      bins[i] = max_abs;
   }
   for (i=0;i<N;i++)
   {
      int j;
      int conseq=0;
      opus_val16 t1, t2, t3;

      t1 = MULT16_16_Q15(QCONST16(.15f, 15), bins[i]);
      t2 = MULT16_16_Q15(QCONST16(.4f, 15), bins[i]);
      t3 = MULT16_16_Q15(QCONST16(.15f, 15), bins[i]);
      for (j=0;j<i;j++)
      {
         if (bins[j] < t1)
            conseq++;
         if (bins[j] < t2)
            conseq++;
         else
            conseq = 0;
      }
      if (conseq>=3)
         is_transient=1;
      conseq = 0;
      for (j=i+1;j<N;j++)
      {
         if (bins[j] < t3)
            conseq++;
         else
            conseq = 0;
      }
      if (conseq>=7)
         is_transient=1;
   }
   RESTORE_STACK;
#ifdef FUZZING
   is_transient = rand()&0x1;
#endif
   return is_transient;
}

/** Apply window and compute the MDCT for all sub-frames and
    all channels in a frame */
static void compute_mdcts(const CELTMode *mode, int shortBlocks, celt_sig * OPUS_RESTRICT in, celt_sig * OPUS_RESTRICT out, int C, int LM)
{
   if (C==1 && !shortBlocks)
   {
      const int overlap = OVERLAP(mode);
      clt_mdct_forward(&mode->mdct, in, out, mode->window, overlap, mode->maxLM-LM, 1);
   } else {
      const int overlap = OVERLAP(mode);
      int N = mode->shortMdctSize<<LM;
      int B = 1;
      int b, c;
      if (shortBlocks)
      {
         N = mode->shortMdctSize;
         B = shortBlocks;
      }
      c=0; do {
         for (b=0;b<B;b++)
         {
            /* Interleaving the sub-frames while doing the MDCTs */
            clt_mdct_forward(&mode->mdct, in+c*(B*N+overlap)+b*N, &out[b+c*N*B], mode->window, overlap, shortBlocks ? mode->maxLM : mode->maxLM-LM, B);
         }
      } while (++c<C);
   }
}
#endif /* OPUS_ENABLE_ENCODER */

/** Compute the IMDCT and apply window for all sub-frames and
    all channels in a frame */
static void compute_inv_mdcts(const CELTMode *mode, int shortBlocks, celt_sig *X,
      celt_sig * OPUS_RESTRICT out_mem[],
      celt_sig * OPUS_RESTRICT overlap_mem[], int C, int LM)
{
   int c;
   const int N = mode->shortMdctSize<<LM;
   const int overlap = OVERLAP(mode);
   VARDECL(opus_val32, x);
   SAVE_STACK;

   ALLOC(x, N+overlap, opus_val32);
   c=0; do {
      int j;
      int b;
      int N2 = N;
      int B = 1;

      if (shortBlocks)
      {
         N2 = mode->shortMdctSize;
         B = shortBlocks;
      }
      /* Prevents problems from the imdct doing the overlap-add */
      OPUS_CLEAR(x, overlap);

      for (b=0;b<B;b++)
      {
         /* IMDCT on the interleaved the sub-frames */
         clt_mdct_backward(&mode->mdct, &X[b+c*N2*B], x+N2*b, mode->window, overlap, shortBlocks ? mode->maxLM : mode->maxLM-LM, B);
      }

      for (j=0;j<overlap;j++)
         out_mem[c][j] = x[j] + overlap_mem[c][j];
      for (;j<N;j++)
         out_mem[c][j] = x[j];
      for (j=0;j<overlap;j++)
         overlap_mem[c][j] = x[N+j];
   } while (++c<C);
   RESTORE_STACK;
}

static void deemphasis(celt_sig *in[], opus_val16 *pcm, int N, int C, int downsample, const opus_val16 *coef, celt_sig *mem)
{
   int c;
   int count=0;
   c=0; do {
      int j;
      celt_sig * OPUS_RESTRICT x;
      opus_val16  * OPUS_RESTRICT y;
      celt_sig m = mem[c];
      x =in[c];
      y = pcm+c;
      for (j=0;j<N;j++)
      {
         celt_sig tmp = *x + m;
         m = MULT16_32_Q15(coef[0], tmp)
           - MULT16_32_Q15(coef[1], *x);
         tmp = SHL32(MULT16_32_Q15(coef[3], tmp), 2);
         x++;
         /* Technically the store could be moved outside of the if because
            the stores we don't want will just be overwritten */
         if (count==0)
            *y = SCALEOUT(SIG2WORD16(tmp));
         if (++count==downsample)
         {
            y+=C;
            count=0;
         }
      }
      mem[c] = m;
   } while (++c<C);
}

static void comb_filter(opus_val32 *y, opus_val32 *x, int T0, int T1, int N,
      opus_val16 g0, opus_val16 g1, int tapset0, int tapset1,
      const opus_val16 *window, int overlap)
{
   int i;
   /* printf ("%d %d %f %f\n", T0, T1, g0, g1); */
   opus_val16 g00, g01, g02, g10, g11, g12;
   static const opus_val16 gains[3][3] = {
         {QCONST16(0.3066406250f, 15), QCONST16(0.2170410156f, 15), QCONST16(0.1296386719f, 15)},
         {QCONST16(0.4638671875f, 15), QCONST16(0.2680664062f, 15), QCONST16(0.f, 15)},
         {QCONST16(0.7998046875f, 15), QCONST16(0.1000976562f, 15), QCONST16(0.f, 15)}};
   g00 = MULT16_16_Q15(g0, gains[tapset0][0]);
   g01 = MULT16_16_Q15(g0, gains[tapset0][1]);
   g02 = MULT16_16_Q15(g0, gains[tapset0][2]);
   g10 = MULT16_16_Q15(g1, gains[tapset1][0]);
   g11 = MULT16_16_Q15(g1, gains[tapset1][1]);
   g12 = MULT16_16_Q15(g1, gains[tapset1][2]);
   for (i=0;i<overlap;i++)
   {
      opus_val16 f;
      f = MULT16_16_Q15(window[i],window[i]);
      y[i] = x[i]
               + MULT16_32_Q15(MULT16_16_Q15((Q15ONE-f),g00),x[i-T0])
               + MULT16_32_Q15(MULT16_16_Q15((Q15ONE-f),g01),x[i-T0-1])
               + MULT16_32_Q15(MULT16_16_Q15((Q15ONE-f),g01),x[i-T0+1])
               + MULT16_32_Q15(MULT16_16_Q15((Q15ONE-f),g02),x[i-T0-2])
               + MULT16_32_Q15(MULT16_16_Q15((Q15ONE-f),g02),x[i-T0+2])
               + MULT16_32_Q15(MULT16_16_Q15(f,g10),x[i-T1])
               + MULT16_32_Q15(MULT16_16_Q15(f,g11),x[i-T1-1])
               + MULT16_32_Q15(MULT16_16_Q15(f,g11),x[i-T1+1])
               + MULT16_32_Q15(MULT16_16_Q15(f,g12),x[i-T1-2])
               + MULT16_32_Q15(MULT16_16_Q15(f,g12),x[i-T1+2]);

   }
   for (i=overlap;i<N;i++)
      y[i] = x[i]
               + MULT16_32_Q15(g10,x[i-T1])
               + MULT16_32_Q15(g11,x[i-T1-1])
               + MULT16_32_Q15(g11,x[i-T1+1])
               + MULT16_32_Q15(g12,x[i-T1-2])
               + MULT16_32_Q15(g12,x[i-T1+2]);
}

static const signed char tf_select_table[4][8] = {
      {0, -1, 0, -1,    0,-1, 0,-1},
      {0, -1, 0, -2,    1, 0, 1,-1},
      {0, -2, 0, -3,    2, 0, 1,-1},
      {0, -2, 0, -3,    3, 0, 1,-1},
};

#ifdef OPUS_ENABLE_ENCODER
static opus_val32 l1_metric(const celt_norm *tmp, int N, int LM, int width)
{
   int i, j;
   static const opus_val16 sqrtM_1[4] = {Q15ONE, QCONST16(.70710678f,15), QCONST16(0.5f,15), QCONST16(0.35355339f,15)};
   opus_val32 L1;
   opus_val16 bias;
   L1=0;
   for (i=0;i<1<<LM;i++)
   {
      opus_val32 L2 = 0;
      for (j=0;j<N>>LM;j++)
         L2 = MAC16_16(L2, tmp[(j<<LM)+i], tmp[(j<<LM)+i]);
      L1 += celt_sqrt(L2);
   }
   L1 = MULT16_32_Q15(sqrtM_1[LM], L1);
   if (width==1)
      bias = QCONST16(.12f,15)*LM;
   else if (width==2)
      bias = QCONST16(.05f,15)*LM;
   else
      bias = QCONST16(.02f,15)*LM;
   L1 = MAC16_32_Q15(L1, bias, L1);
   return L1;
}

static int tf_analysis(const CELTMode *m, int len, int C, int isTransient,
      int *tf_res, int nbCompressedBytes, celt_norm *X, int N0, int LM,
      int start, int *tf_sum)
{
   int i;
   VARDECL(int, metric);
   int cost0;
   int cost1;
   VARDECL(int, path0);
   VARDECL(int, path1);
   VARDECL(celt_norm, tmp);
   int lambda;
   int tf_select=0;
   SAVE_STACK;

   if (nbCompressedBytes<15*C || start!=0)
   {
      *tf_sum = 0;
      for (i=0;i<len;i++)
         tf_res[i] = isTransient;
      return 0;
   }
   if (nbCompressedBytes<40)
      lambda = 12;
   else if (nbCompressedBytes<60)
      lambda = 6;
   else if (nbCompressedBytes<100)
      lambda = 4;
   else
      lambda = 3;

   ALLOC(metric, len, int);
   ALLOC(tmp, (m->eBands[len]-m->eBands[len-1])<<LM, celt_norm);
   ALLOC(path0, len, int);
   ALLOC(path1, len, int);

   *tf_sum = 0;
   for (i=0;i<len;i++)
   {
      int j, k, N;
      opus_val32 L1, best_L1;
      int best_level=0;
      N = (m->eBands[i+1]-m->eBands[i])<<LM;
      for (j=0;j<N;j++)
         tmp[j] = X[j+(m->eBands[i]<<LM)];
      /* Just add the right channel if we're in stereo */
      if (C==2)
         for (j=0;j<N;j++)
            tmp[j] = ADD16(SHR16(tmp[j], 1),SHR16(X[N0+j+(m->eBands[i]<<LM)], 1));
      L1 = l1_metric(tmp, N, isTransient ? LM : 0, N>>LM);
      best_L1 = L1;
      /*printf ("%f ", L1);*/
      for (k=0;k<LM;k++)
      {
         int B;

         if (isTransient)
            B = (LM-k-1);
         else
            B = k+1;

         if (isTransient)
            haar1(tmp, N>>(LM-k), 1<<(LM-k));
         else
            haar1(tmp, N>>k, 1<<k);

         L1 = l1_metric(tmp, N, B, N>>LM);

         if (L1 < best_L1)
         {
            best_L1 = L1;
            best_level = k+1;
         }
      }
      /*printf ("%d ", isTransient ? LM-best_level : best_level);*/
      if (isTransient)
         metric[i] = best_level;
      else
         metric[i] = -best_level;
      *tf_sum += metric[i];
   }
   /*printf("\n");*/
   /* NOTE: Future optimized implementations could detect extreme transients and set
      tf_select = 1 but so far we have not found a reliable way of making this useful */
   tf_select = 0;

   cost0 = 0;
   cost1 = isTransient ? 0 : lambda;
   /* Viterbi forward pass */
   for (i=1;i<len;i++)
   {
      int curr0, curr1;
      int from0, from1;

      from0 = cost0;
      from1 = cost1 + lambda;
      if (from0 < from1)
      {
         curr0 = from0;
         path0[i]= 0;
      } else {
         curr0 = from1;
         path0[i]= 1;
      }

      from0 = cost0 + lambda;
      from1 = cost1;
      if (from0 < from1)
      {
         curr1 = from0;
         path1[i]= 0;
      } else {
         curr1 = from1;
         path1[i]= 1;
      }
      cost0 = curr0 + abs(metric[i]-tf_select_table[LM][4*isTransient+2*tf_select+0]);
      cost1 = curr1 + abs(metric[i]-tf_select_table[LM][4*isTransient+2*tf_select+1]);
   }
   tf_res[len-1] = cost0 < cost1 ? 0 : 1;
   /* Viterbi backward pass to check the decisions */
   for (i=len-2;i>=0;i--)
   {
      if (tf_res[i+1] == 1)
         tf_res[i] = path1[i+1];
      else
         tf_res[i] = path0[i+1];
   }
   RESTORE_STACK;
#ifdef FUZZING
   tf_select = rand()&0x1;
   tf_res[0] = rand()&0x1;
   for (i=1;i<len;i++)
      tf_res[i] = tf_res[i-1] ^ ((rand()&0xF) == 0);
#endif
   return tf_select;
}

static void tf_encode(int start, int end, int isTransient, int *tf_res, int LM, int tf_select, ec_enc *enc)
{
   int curr, i;
   int tf_select_rsv;
   int tf_changed;
   int logp;
   opus_uint32 budget;
   opus_uint32 tell;
   budget = enc->storage*8;
   tell = ec_tell(enc);
   logp = isTransient ? 2 : 4;
   /* Reserve space to code the tf_select decision. */
   tf_select_rsv = LM>0 && tell+logp+1 <= budget;
   budget -= tf_select_rsv;
   curr = tf_changed = 0;
   for (i=start;i<end;i++)
   {
      if (tell+logp<=budget)
      {
         ec_enc_bit_logp(enc, tf_res[i] ^ curr, logp);
         tell = ec_tell(enc);
         curr = tf_res[i];
         tf_changed |= curr;
      }
      else
         tf_res[i] = curr;
      logp = isTransient ? 4 : 5;
   }
   /* Only code tf_select if it would actually make a difference. */
   if (tf_select_rsv &&
         tf_select_table[LM][4*isTransient+0+tf_changed]!=
         tf_select_table[LM][4*isTransient+2+tf_changed])
      ec_enc_bit_logp(enc, tf_select, 1);
   else
      tf_select = 0;
   for (i=start;i<end;i++)
      tf_res[i] = tf_select_table[LM][4*isTransient+2*tf_select+tf_res[i]];
   /*printf("%d %d ", isTransient, tf_select); for(i=0;i<end;i++)printf("%d ", tf_res[i]);printf("\n");*/
}
#endif /* OPUS_ENABLE_ENCODER */

static void tf_decode(int start, int end, int isTransient, int *tf_res, int LM, ec_dec *dec)
{
   int i, curr, tf_select;
   int tf_select_rsv;
   int tf_changed;
   int logp;
   opus_uint32 budget;
   opus_uint32 tell;

   budget = dec->storage*8;
   tell = ec_tell(dec);
   logp = isTransient ? 2 : 4;
   tf_select_rsv = LM>0 && tell+logp+1<=budget;
   budget -= tf_select_rsv;
   tf_changed = curr = 0;
   for (i=start;i<end;i++)
   {
      if (tell+logp<=budget)
      {
         curr ^= ec_dec_bit_logp(dec, logp);
         tell = ec_tell(dec);
         tf_changed |= curr;
      }
      tf_res[i] = curr;
      logp = isTransient ? 4 : 5;
   }
   tf_select = 0;
   if (tf_select_rsv &&
     tf_select_table[LM][4*isTransient+0+tf_changed] !=
     tf_select_table[LM][4*isTransient+2+tf_changed])
   {
      tf_select = ec_dec_bit_logp(dec, 1);
   }
   for (i=start;i<end;i++)
   {
      tf_res[i] = tf_select_table[LM][4*isTransient+2*tf_select+tf_res[i]];
   }
}

static void init_caps(const CELTMode *m,int *cap,int LM,int C)
{
   int i;
   for (i=0;i<m->nbEBands;i++)
   {
      int N;
      N=(m->eBands[i+1]-m->eBands[i])<<LM;
      cap[i] = (m->cache.caps[m->nbEBands*(2*LM+C-1)+i]+64)*C*N>>2;
   }
}

#ifdef OPUS_ENABLE_ENCODER
static int alloc_trim_analysis(const CELTMode *m, const celt_norm *X,
      const opus_val16 *bandLogE, int end, int LM, int C, int N0)
{
   int i;
   opus_val32 diff=0;
   int c;
   int trim_index = 5;
   if (C==2)
   {
      opus_val16 sum = 0; /* Q10 */
      /* Compute inter-channel correlation for low frequencies */
      for (i=0;i<8;i++)
      {
         int j;
         opus_val32 partial = 0;
         for (j=m->eBands[i]<<LM;j<m->eBands[i+1]<<LM;j++)
            partial = MAC16_16(partial, X[j], X[N0+j]);
         sum = ADD16(sum, EXTRACT16(SHR32(partial, 18)));
      }
      sum = MULT16_16_Q15(QCONST16(1.f/8, 15), sum);
      /*printf ("%f\n", sum);*/
      if (sum > QCONST16(.995f,10))
         trim_index-=4;
      else if (sum > QCONST16(.92f,10))
         trim_index-=3;
      else if (sum > QCONST16(.85f,10))
         trim_index-=2;
      else if (sum > QCONST16(.8f,10))
         trim_index-=1;
   }

   /* Estimate spectral tilt */
   c=0; do {
      for (i=0;i<end-1;i++)
      {
         diff += bandLogE[i+c*m->nbEBands]*(opus_int32)(2+2*i-m->nbEBands);
      }
   } while (++c<C);
   /* We divide by two here to avoid making the tilt larger for stereo as a
      result of a bug in the loop above */
   diff /= 2*C*(end-1);
   /*printf("%f\n", diff);*/
   if (diff > QCONST16(2.f, DB_SHIFT))
      trim_index--;
   if (diff > QCONST16(8.f, DB_SHIFT))
      trim_index--;
   if (diff < -QCONST16(4.f, DB_SHIFT))
      trim_index++;
   if (diff < -QCONST16(10.f, DB_SHIFT))
      trim_index++;

   if (trim_index<0)
      trim_index = 0;
   if (trim_index>10)
      trim_index = 10;
#ifdef FUZZING
   trim_index = rand()%11;
#endif
   return trim_index;
}

static int stereo_analysis(const CELTMode *m, const celt_norm *X,
      int LM, int N0)
{
   int i;
   int thetas;
   opus_val32 sumLR = EPSILON, sumMS = EPSILON;

   /* Use the L1 norm to model the entropy of the L/R signal vs the M/S signal */
   for (i=0;i<13;i++)
   {
      int j;
      for (j=m->eBands[i]<<LM;j<m->eBands[i+1]<<LM;j++)
      {
         opus_val32 L, R, M, S;
         /* We cast to 32-bit first because of the -32768 case */
         L = EXTEND32(X[j]);
         R = EXTEND32(X[N0+j]);
         M = ADD32(L, R);
         S = SUB32(L, R);
         sumLR = ADD32(sumLR, ADD32(ABS32(L), ABS32(R)));
         sumMS = ADD32(sumMS, ADD32(ABS32(M), ABS32(S)));
      }
   }
   sumMS = MULT16_32_Q15(QCONST16(0.707107f, 15), sumMS);
   thetas = 13;
   /* We don't need thetas for lower bands with LM<=1 */
   if (LM<=1)
      thetas -= 8;
   return MULT16_32_Q15((m->eBands[13]<<(LM+1))+thetas, sumMS)
         > MULT16_32_Q15(m->eBands[13]<<(LM+1), sumLR);
}

int celt_encode_with_ec(CELTEncoder * OPUS_RESTRICT st, const opus_val16 * pcm, int frame_size, unsigned char *compressed, int nbCompressedBytes, ec_enc *enc)
{
   int i, c, N;
   opus_int32 bits;
   ec_enc _enc;
   VARDECL(celt_sig, in);
   VARDECL(celt_sig, freq);
   VARDECL(celt_norm, X);
   VARDECL(celt_ener, bandE);
   VARDECL(opus_val16, bandLogE);
   VARDECL(int, fine_quant);
   VARDECL(opus_val16, error);
   VARDECL(int, pulses);
   VARDECL(int, cap);
   VARDECL(int, offsets);
   VARDECL(int, fine_priority);
   VARDECL(int, tf_res);
   VARDECL(unsigned char, collapse_masks);
   celt_sig *prefilter_mem;
   opus_val16 *oldBandE, *oldLogE, *oldLogE2;
   int shortBlocks=0;
   int isTransient=0;
   const int CC = st->channels;
   const int C = st->stream_channels;
   int LM, M;
   int tf_select;
   int nbFilledBytes, nbAvailableBytes;
   int effEnd;
   int codedBands;
   int tf_sum;
   int alloc_trim;
   int pitch_index=COMBFILTER_MINPERIOD;
   opus_val16 gain1 = 0;
   int intensity=0;
   int dual_stereo=0;
   int effectiveBytes;
   opus_val16 pf_threshold;
   int dynalloc_logp;
   opus_int32 vbr_rate;
   opus_int32 total_bits;
   opus_int32 total_boost;
   opus_int32 balance;
   opus_int32 tell;
   int prefilter_tapset=0;
   int pf_on;
   int anti_collapse_rsv;
   int anti_collapse_on=0;
   int silence=0;
   ALLOC_STACK;

   if (nbCompressedBytes<2 || pcm==NULL)
     return OPUS_BAD_ARG;

   frame_size *= st->upsample;
   for (LM=0;LM<=st->mode->maxLM;LM++)
      if (st->mode->shortMdctSize<<LM==frame_size)
         break;
   if (LM>st->mode->maxLM)
      return OPUS_BAD_ARG;
   M=1<<LM;
   N = M*st->mode->shortMdctSize;

   prefilter_mem = st->in_mem+CC*(st->overlap);
   oldBandE = (opus_val16*)(st->in_mem+CC*(st->overlap+COMBFILTER_MAXPERIOD));
   oldLogE = oldBandE + CC*st->mode->nbEBands;
   oldLogE2 = oldLogE + CC*st->mode->nbEBands;

   if (enc==NULL)
   {
      tell=1;
      nbFilledBytes=0;
   } else {
      tell=ec_tell(enc);
      nbFilledBytes=(tell+4)>>3;
   }

#ifdef CUSTOM_MODES
   if (st->signalling && enc==NULL)
   {
      int tmp = (st->mode->effEBands-st->end)>>1;
      st->end = IMAX(1, st->mode->effEBands-tmp);
      compressed[0] = tmp<<5;
      compressed[0] |= LM<<3;
      compressed[0] |= (C==2)<<2;
      /* Convert "standard mode" to Opus header */
      if (st->mode->Fs==48000 && st->mode->shortMdctSize==120)
      {
         int c0 = toOpus(compressed[0]);
         if (c0<0)
            return OPUS_BAD_ARG;
         compressed[0] = c0;
      }
      compressed++;
      nbCompressedBytes--;
   }
#else
   celt_assert(st->signalling==0);
#endif

   /* Can't produce more than 1275 output bytes */
   nbCompressedBytes = IMIN(nbCompressedBytes,1275);
   nbAvailableBytes = nbCompressedBytes - nbFilledBytes;

   if (st->vbr && st->bitrate!=OPUS_BITRATE_MAX)
   {
      opus_int32 den=st->mode->Fs>>BITRES;
      vbr_rate=(st->bitrate*frame_size+(den>>1))/den;
#ifdef CUSTOM_MODES
      if (st->signalling)
         vbr_rate -= 8<<BITRES;
#endif
      effectiveBytes = vbr_rate>>(3+BITRES);
   } else {
      opus_int32 tmp;
      vbr_rate = 0;
      tmp = st->bitrate*frame_size;
      if (tell>1)
         tmp += tell;
      if (st->bitrate!=OPUS_BITRATE_MAX)
         nbCompressedBytes = IMAX(2, IMIN(nbCompressedBytes,
               (tmp+4*st->mode->Fs)/(8*st->mode->Fs)-!!st->signalling));
      effectiveBytes = nbCompressedBytes;
   }

   if (enc==NULL)
   {
      ec_enc_init(&_enc, compressed, nbCompressedBytes);
      enc = &_enc;
   }

   if (vbr_rate>0)
   {
      /* Computes the max bit-rate allowed in VBR mode to avoid violating the
          target rate and buffering.
         We must do this up front so that bust-prevention logic triggers
          correctly if we don't have enough bits. */
      if (st->constrained_vbr)
      {
         opus_int32 vbr_bound;
         opus_int32 max_allowed;
         /* We could use any multiple of vbr_rate as bound (depending on the
             delay).
            This is clamped to ensure we use at least two bytes if the encoder
             was entirely empty, but to allow 0 in hybrid mode. */
         vbr_bound = vbr_rate;
         max_allowed = IMIN(IMAX(tell==1?2:0,
               (vbr_rate+vbr_bound-st->vbr_reservoir)>>(BITRES+3)),
               nbAvailableBytes);
         if(max_allowed < nbAvailableBytes)
         {
            nbCompressedBytes = nbFilledBytes+max_allowed;
            nbAvailableBytes = max_allowed;
            ec_enc_shrink(enc, nbCompressedBytes);
         }
      }
   }
   total_bits = nbCompressedBytes*8;

   effEnd = st->end;
   if (effEnd > st->mode->effEBands)
      effEnd = st->mode->effEBands;

   ALLOC(in, CC*(N+st->overlap), celt_sig);

   /* Find pitch period and gain */
   {
      VARDECL(celt_sig, _pre);
      celt_sig *pre[2];
      SAVE_STACK;
      ALLOC(_pre, CC*(N+COMBFILTER_MAXPERIOD), celt_sig);

      pre[0] = _pre;
      pre[1] = _pre + (N+COMBFILTER_MAXPERIOD);

      silence = 1;
      c=0; do {
         int count = 0;
         const opus_val16 * OPUS_RESTRICT pcmp = pcm+c;
         celt_sig * OPUS_RESTRICT inp = in+c*(N+st->overlap)+st->overlap;

         for (i=0;i<N;i++)
         {
            celt_sig x, tmp;

            x = SCALEIN(*pcmp);
#ifndef FIXED_POINT
            if (!(x==x))
               x = 0;
            if (st->clip)
               x = MAX32(-65536.f, MIN32(65536.f,x));
#endif
            if (++count==st->upsample)
            {
               count=0;
               pcmp+=CC;
            } else {
               x = 0;
            }
            /* Apply pre-emphasis */
            tmp = MULT16_16(st->mode->preemph[2], x);
            *inp = tmp + st->preemph_memE[c];
            st->preemph_memE[c] = MULT16_32_Q15(st->mode->preemph[1], *inp)
                                   - MULT16_32_Q15(st->mode->preemph[0], tmp);
            silence = silence && *inp == 0;
            inp++;
         }
         OPUS_COPY(pre[c], prefilter_mem+c*COMBFILTER_MAXPERIOD, COMBFILTER_MAXPERIOD);
         OPUS_COPY(pre[c]+COMBFILTER_MAXPERIOD, in+c*(N+st->overlap)+st->overlap, N);
      } while (++c<CC);

#ifdef FUZZING
      if ((rand()&0x3F)==0)
         silence = 1;
#endif
      if (tell==1)
         ec_enc_bit_logp(enc, silence, 15);
      else
         silence=0;
      if (silence)
      {
         /*In VBR mode there is no need to send more than the minimum. */
         if (vbr_rate>0)
         {
            effectiveBytes=nbCompressedBytes=IMIN(nbCompressedBytes, nbFilledBytes+2);
            total_bits=nbCompressedBytes*8;
            nbAvailableBytes=2;
            ec_enc_shrink(enc, nbCompressedBytes);
         }
         /* Pretend we've filled all the remaining bits with zeros
            (that's what the initialiser did anyway) */
         tell = nbCompressedBytes*8;
         enc->nbits_total+=tell-ec_tell(enc);
      }
      if (nbAvailableBytes>12*C && st->start==0 && !silence && !st->disable_pf && st->complexity >= 5)
      {
         VARDECL(opus_val16, pitch_buf);
         ALLOC(pitch_buf, (COMBFILTER_MAXPERIOD+N)>>1, opus_val16);

         pitch_downsample(pre, pitch_buf, COMBFILTER_MAXPERIOD+N, CC);
         pitch_search(pitch_buf+(COMBFILTER_MAXPERIOD>>1), pitch_buf, N,
               COMBFILTER_MAXPERIOD-COMBFILTER_MINPERIOD, &pitch_index);
         pitch_index = COMBFILTER_MAXPERIOD-pitch_index;

         gain1 = remove_doubling(pitch_buf, COMBFILTER_MAXPERIOD, COMBFILTER_MINPERIOD,
               N, &pitch_index, st->prefilter_period, st->prefilter_gain);
         if (pitch_index > COMBFILTER_MAXPERIOD-2)
            pitch_index = COMBFILTER_MAXPERIOD-2;
         gain1 = MULT16_16_Q15(QCONST16(.7f,15),gain1);
         if (st->loss_rate>2)
            gain1 = HALF32(gain1);
         if (st->loss_rate>4)
            gain1 = HALF32(gain1);
         if (st->loss_rate>8)
            gain1 = 0;
         prefilter_tapset = st->tapset_decision;
      } else {
         gain1 = 0;
      }

      /* Gain threshold for enabling the prefilter/postfilter */
      pf_threshold = QCONST16(.2f,15);

      /* Adjusting the threshold based on rate and continuity */
      if (abs(pitch_index-st->prefilter_period)*10>pitch_index)
         pf_threshold += QCONST16(.2f,15);
      if (nbAvailableBytes<25)
         pf_threshold += QCONST16(.1f,15);
      if (nbAvailableBytes<35)
         pf_threshold += QCONST16(.1f,15);
      if (st->prefilter_gain > QCONST16(.4f,15))
         pf_threshold -= QCONST16(.1f,15);
      if (st->prefilter_gain > QCONST16(.55f,15))
         pf_threshold -= QCONST16(.1f,15);

      /* Hard threshold at 0.2 */
      pf_threshold = MAX16(pf_threshold, QCONST16(.2f,15));
      if (gain1<pf_threshold)
      {
         if(st->start==0 && tell+16<=total_bits)
            ec_enc_bit_logp(enc, 0, 1);
         gain1 = 0;
         pf_on = 0;
      } else {
         /*This block is not gated by a total bits check only because
           of the nbAvailableBytes check above.*/
         int qg;
         int octave;

         if (ABS16(gain1-st->prefilter_gain)<QCONST16(.1f,15))
            gain1=st->prefilter_gain;

#ifdef FIXED_POINT
         qg = ((gain1+1536)>>10)/3-1;
#else
         qg = (int)floor(.5f+gain1*32/3)-1;
#endif
         qg = IMAX(0, IMIN(7, qg));
         ec_enc_bit_logp(enc, 1, 1);
         pitch_index += 1;
         octave = EC_ILOG(pitch_index)-5;
         ec_enc_uint(enc, octave, 6);
         ec_enc_bits(enc, pitch_index-(16<<octave), 4+octave);
         pitch_index -= 1;
         ec_enc_bits(enc, qg, 3);
         if (ec_tell(enc)+2<=total_bits)
            ec_enc_icdf(enc, prefilter_tapset, tapset_icdf, 2);
         else
           prefilter_tapset = 0;
         gain1 = QCONST16(0.09375f,15)*(qg+1);
         pf_on = 1;
      }
      /*printf("%d %f\n", pitch_index, gain1);*/

      c=0; do {
         int offset = st->mode->shortMdctSize-st->mode->overlap;
         st->prefilter_period=IMAX(st->prefilter_period, COMBFILTER_MINPERIOD);
         OPUS_COPY(in+c*(N+st->overlap), st->in_mem+c*(st->overlap), st->overlap);
         if (offset)
            comb_filter(in+c*(N+st->overlap)+st->overlap, pre[c]+COMBFILTER_MAXPERIOD,
                  st->prefilter_period, st->prefilter_period, offset, -st->prefilter_gain, -st->prefilter_gain,
                  st->prefilter_tapset, st->prefilter_tapset, NULL, 0);

         comb_filter(in+c*(N+st->overlap)+st->overlap+offset, pre[c]+COMBFILTER_MAXPERIOD+offset,
               st->prefilter_period, pitch_index, N-offset, -st->prefilter_gain, -gain1,
               st->prefilter_tapset, prefilter_tapset, st->mode->window, st->mode->overlap);
         OPUS_COPY(st->in_mem+c*(st->overlap), in+c*(N+st->overlap)+N, st->overlap);

         if (N>COMBFILTER_MAXPERIOD)
         {
            OPUS_MOVE(prefilter_mem+c*COMBFILTER_MAXPERIOD, pre[c]+N, COMBFILTER_MAXPERIOD);
         } else {
            OPUS_MOVE(prefilter_mem+c*COMBFILTER_MAXPERIOD, prefilter_mem+c*COMBFILTER_MAXPERIOD+N, COMBFILTER_MAXPERIOD-N);
            OPUS_MOVE(prefilter_mem+c*COMBFILTER_MAXPERIOD+COMBFILTER_MAXPERIOD-N, pre[c]+COMBFILTER_MAXPERIOD, N);
         }
      } while (++c<CC);

      RESTORE_STACK;
   }

   isTransient = 0;
   shortBlocks = 0;
   if (LM>0 && ec_tell(enc)+3<=total_bits)
   {
      if (st->complexity > 1)
      {
         isTransient = transient_analysis(in, N+st->overlap, CC,
                  st->overlap);
         if (isTransient)
            shortBlocks = M;
      }
      ec_enc_bit_logp(enc, isTransient, 3);
   }

   ALLOC(freq, CC*N, celt_sig); /**< Interleaved signal MDCTs */
   ALLOC(bandE,st->mode->nbEBands*CC, celt_ener);
   ALLOC(bandLogE,st->mode->nbEBands*CC, opus_val16);
   /* Compute MDCTs */
   compute_mdcts(st->mode, shortBlocks, in, freq, CC, LM);

   if (CC==2&&C==1)
   {
      for (i=0;i<N;i++)
         freq[i] = ADD32(HALF32(freq[i]), HALF32(freq[N+i]));
   }
   if (st->upsample != 1)
   {
      c=0; do
      {
         int bound = N/st->upsample;
         for (i=0;i<bound;i++)
            freq[c*N+i] *= st->upsample;
         for (;i<N;i++)
            freq[c*N+i] = 0;
      } while (++c<C);
   }
   ALLOC(X, C*N, celt_norm);         /**< Interleaved normalised MDCTs */

   compute_band_energies(st->mode, freq, bandE, effEnd, C, M);

   amp2Log2(st->mode, effEnd, st->end, bandE, bandLogE, C);

   /* Band normalisation */
   normalise_bands(st->mode, freq, X, bandE, effEnd, C, M);

   ALLOC(tf_res, st->mode->nbEBands, int);
   tf_select = tf_analysis(st->mode, effEnd, C, isTransient, tf_res, effectiveBytes, X, N, LM, st->start, &tf_sum);
   for (i=effEnd;i<st->end;i++)
      tf_res[i] = tf_res[effEnd-1];

   ALLOC(error, C*st->mode->nbEBands, opus_val16);
   quant_coarse_energy(st->mode, st->start, st->end, effEnd, bandLogE,
         oldBandE, total_bits, error, enc,
         C, LM, nbAvailableBytes, st->force_intra,
         &st->delayedIntra, st->complexity >= 4, st->loss_rate);

   tf_encode(st->start, st->end, isTransient, tf_res, LM, tf_select, enc);

   if (ec_tell(enc)+4<=total_bits)
   {
      if (shortBlocks || st->complexity < 3 
          || nbAvailableBytes < 10*C || st->start!=0)
      {
         if (st->complexity == 0)
            st->spread_decision = SPREAD_NONE;
         else
            st->spread_decision = SPREAD_NORMAL;
      } else {
         st->spread_decision = spreading_decision(st->mode, X,
               &st->tonal_average, st->spread_decision, &st->hf_average,
               &st->tapset_decision, pf_on&&!shortBlocks, effEnd, C, M);
      }
      ec_enc_icdf(enc, st->spread_decision, spread_icdf, 5);
   }

   ALLOC(cap, st->mode->nbEBands, int);
   ALLOC(offsets, st->mode->nbEBands, int);

   init_caps(st->mode,cap,LM,C);
   for (i=0;i<st->mode->nbEBands;i++)
      offsets[i] = 0;
   /* Dynamic allocation code */
   /* Make sure that dynamic allocation can't make us bust the budget */
   if (effectiveBytes > 50 && LM>=1)
   {
      int t1, t2;
      if (LM <= 1)
      {
         t1 = 3;
         t2 = 5;
      } else {
         t1 = 2;
         t2 = 4;
      }
      for (i=st->start+1;i<st->end-1;i++)
      {
         opus_val32 d2;
         d2 = 2*bandLogE[i]-bandLogE[i-1]-bandLogE[i+1];
         if (C==2)
            d2 = HALF32(d2 + 2*bandLogE[i+st->mode->nbEBands]-
                  bandLogE[i-1+st->mode->nbEBands]-bandLogE[i+1+st->mode->nbEBands]);
#ifdef FUZZING
         if((rand()&0xF)==0)
         {
            offsets[i] += 1;
            if((rand()&0x3)==0)
               offsets[i] += 1+(rand()&0x3);
         }
#else
         if (d2 > SHL16(t1,DB_SHIFT))
            offsets[i] += 1;
         if (d2 > SHL16(t2,DB_SHIFT))
            offsets[i] += 1;
#endif
      }
   }
   dynalloc_logp = 6;
   total_bits<<=BITRES;
   total_boost = 0;
   tell = ec_tell_frac(enc);
   for (i=st->start;i<st->end;i++)
   {
      int width, quanta;
      int dynalloc_loop_logp;
      int boost;
      int j;
      width = C*(st->mode->eBands[i+1]-st->mode->eBands[i])<<LM;
      /* quanta is 6 bits, but no more than 1 bit/sample
         and no less than 1/8 bit/sample */
      quanta = IMIN(width<<BITRES, IMAX(6<<BITRES, width));
      dynalloc_loop_logp = dynalloc_logp;
      boost = 0;
      for (j = 0; tell+(dynalloc_loop_logp<<BITRES) < total_bits-total_boost
            && boost < cap[i]; j++)
      {
         int flag;
         flag = j<offsets[i];
         ec_enc_bit_logp(enc, flag, dynalloc_loop_logp);
         tell = ec_tell_frac(enc);
         if (!flag)
            break;
         boost += quanta;
         total_boost += quanta;
         dynalloc_loop_logp = 1;
      }
      /* Making dynalloc more likely */
      if (j)
         dynalloc_logp = IMAX(2, dynalloc_logp-1);
      offsets[i] = boost;
   }
   alloc_trim = 5;
   if (tell+(6<<BITRES) <= total_bits - total_boost)
   {
      alloc_trim = alloc_trim_analysis(st->mode, X, bandLogE,
            st->end, LM, C, N);
      ec_enc_icdf(enc, alloc_trim, trim_icdf, 7);
      tell = ec_tell_frac(enc);
   }

   /* Variable bitrate */
   if (vbr_rate>0)
   {
     opus_val16 alpha;
     opus_int32 delta;
     /* The target rate in 8th bits per frame */
     opus_int32 target;
     opus_int32 min_allowed;
     int lm_diff = st->mode->maxLM - LM;

     /* Don't attempt to use more than 510 kb/s, even for frames smaller than 20 ms.
        The CELT allocator will just not be able to use more than that anyway. */
     nbCompressedBytes = IMIN(nbCompressedBytes,1275>>(3-LM));
     target = vbr_rate + (st->vbr_offset>>lm_diff) - ((40*C+20)<<BITRES);

     /* Shortblocks get a large boost in bitrate, but since they
        are uncommon long blocks are not greatly affected */
     if (shortBlocks || tf_sum < -2*(st->end-st->start))
        target = 7*target/4;
     else if (tf_sum < -(st->end-st->start))
        target = 3*target/2;
     else if (M > 1)
        target-=(target+14)/28;

     /* The current offset is removed from the target and the space used
        so far is added*/
     target=target+tell;

     /* In VBR mode the frame size must not be reduced so much that it would
         result in the encoder running out of bits.
        The margin of 2 bytes ensures that none of the bust-prevention logic
         in the decoder will have triggered so far. */
     min_allowed = ((tell+total_boost+(1<<(BITRES+3))-1)>>(BITRES+3)) + 2 - nbFilledBytes;

     nbAvailableBytes = (target+(1<<(BITRES+2)))>>(BITRES+3);
     nbAvailableBytes = IMAX(min_allowed,nbAvailableBytes);
     nbAvailableBytes = IMIN(nbCompressedBytes,nbAvailableBytes+nbFilledBytes) - nbFilledBytes;

     /* By how much did we "miss" the target on that frame */
     delta = target - vbr_rate;

     target=nbAvailableBytes<<(BITRES+3);

     /*If the frame is silent we don't adjust our drift, otherwise
       the encoder will shoot to very high rates after hitting a
       span of silence, but we do allow the bitres to refill.
       This means that we'll undershoot our target in CVBR/VBR modes
       on files with lots of silence. */
     if(silence)
     {
       nbAvailableBytes = 2;
       target = 2*8<<BITRES;
       delta = 0;
     }

     if (st->vbr_count < 970)
     {
        st->vbr_count++;
        alpha = celt_rcp(SHL32(EXTEND32(st->vbr_count+20),16));
     } else
        alpha = QCONST16(.001f,15);
     /* How many bits have we used in excess of what we're allowed */
     if (st->constrained_vbr)
        st->vbr_reservoir += target - vbr_rate;
     /*printf ("%d\n", st->vbr_reservoir);*/

     /* Compute the offset we need to apply in order to reach the target */
     st->vbr_drift += (opus_int32)MULT16_32_Q15(alpha,(delta*(1<<lm_diff))-st->vbr_offset-st->vbr_drift);
     st->vbr_offset = -st->vbr_drift;
     /*printf ("%d\n", st->vbr_drift);*/

     if (st->constrained_vbr && st->vbr_reservoir < 0)
     {
        /* We're under the min value -- increase rate */
        int adjust = (-st->vbr_reservoir)/(8<<BITRES);
        /* Unless we're just coding silence */
        nbAvailableBytes += silence?0:adjust;
        st->vbr_reservoir = 0;
        /*printf ("+%d\n", adjust);*/
     }
     nbCompressedBytes = IMIN(nbCompressedBytes,nbAvailableBytes+nbFilledBytes);
     /* This moves the raw bits to take into account the new compressed size */
     ec_enc_shrink(enc, nbCompressedBytes);
   }
   if (C==2)
   {
      int effectiveRate;

      /* Always use MS for 2.5 ms frames until we can do a better analysis */
      if (LM!=0)
         dual_stereo = stereo_analysis(st->mode, X, LM, N);

      /* Account for coarse energy */
      effectiveRate = (8*effectiveBytes - 80)>>LM;

      /* effectiveRate in kb/s */
      effectiveRate = 2*effectiveRate/5;
      if (effectiveRate<35)
         intensity = 8;
      else if (effectiveRate<50)
         intensity = 12;
      else if (effectiveRate<68)
         intensity = 16;
      else if (effectiveRate<84)
         intensity = 18;
      else if (effectiveRate<102)
         intensity = 19;
      else if (effectiveRate<130)
         intensity = 20;
      else
         intensity = 100;
      intensity = IMIN(st->end,IMAX(st->start, intensity));
   }

   /* Bit allocation */
   ALLOC(fine_quant, st->mode->nbEBands, int);
   ALLOC(pulses, st->mode->nbEBands, int);
   ALLOC(fine_priority, st->mode->nbEBands, int);

   /* bits =           packet size                    - where we are - safety*/
   bits = (((opus_int32)nbCompressedBytes*8)<<BITRES) - ec_tell_frac(enc) - 1;
   anti_collapse_rsv = isTransient&&LM>=2&&bits>=((LM+2)<<BITRES) ? (1<<BITRES) : 0;
   bits -= anti_collapse_rsv;
   codedBands = compute_allocation(st->mode, st->start, st->end, offsets, cap,
         alloc_trim, &intensity, &dual_stereo, bits, &balance, pulses,
         fine_quant, fine_priority, C, LM, enc, 1, st->lastCodedBands);
   st->lastCodedBands = codedBands;

   quant_fine_energy(st->mode, st->start, st->end, oldBandE, error, fine_quant, enc, C);

#ifdef MEASURE_NORM_MSE
   float X0[3000];
   float bandE0[60];
   c=0; do
      for (i=0;i<N;i++)
         X0[i+c*N] = X[i+c*N];
   while (++c<C);
   for (i=0;i<C*st->mode->nbEBands;i++)
      bandE0[i] = bandE[i];
#endif

   /* Residual quantisation */
   ALLOC(collapse_masks, C*st->mode->nbEBands, unsigned char);
   quant_all_bands(1, st->mode, st->start, st->end, X, C==2 ? X+N : NULL, collapse_masks,
         bandE, pulses, shortBlocks, st->spread_decision, dual_stereo, intensity, tf_res,
         nbCompressedBytes*(8<<BITRES)-anti_collapse_rsv, balance, enc, LM, codedBands, &st->rng);

   if (anti_collapse_rsv > 0)
   {
      anti_collapse_on = st->consec_transient<2;
#ifdef FUZZING
      anti_collapse_on = rand()&0x1;
#endif
      ec_enc_bits(enc, anti_collapse_on, 1);
   }
   quant_energy_finalise(st->mode, st->start, st->end, oldBandE, error, fine_quant, fine_priority, nbCompressedBytes*8-ec_tell(enc), enc, C);

   if (silence)
   {
      for (i=0;i<C*st->mode->nbEBands;i++)
         oldBandE[i] = -QCONST16(28.f,DB_SHIFT);
   }

#ifdef RESYNTH
   /* Re-synthesis of the coded audio if required */
   {
      celt_sig *out_mem[2];
      celt_sig *overlap_mem[2];

      log2Amp(st->mode, st->start, st->end, bandE, oldBandE, C);
      if (silence)
      {
         for (i=0;i<C*st->mode->nbEBands;i++)
            bandE[i] = 0;
      }

#ifdef MEASURE_NORM_MSE
      measure_norm_mse(st->mode, X, X0, bandE, bandE0, M, N, C);
#endif
      if (anti_collapse_on)
      {
         anti_collapse(st->mode, X, collapse_masks, LM, C, N,
               st->start, st->end, oldBandE, oldLogE, oldLogE2, pulses, st->rng);
      }

      /* Synthesis */
      denormalise_bands(st->mode, X, freq, bandE, effEnd, C, M);

      OPUS_MOVE(st->syn_mem[0], st->syn_mem[0]+N, MAX_PERIOD);
      if (CC==2)
         OPUS_MOVE(st->syn_mem[1], st->syn_mem[1]+N, MAX_PERIOD);

      c=0; do
         for (i=0;i<M*st->mode->eBands[st->start];i++)
            freq[c*N+i] = 0;
      while (++c<C);
      c=0; do
         for (i=M*st->mode->eBands[st->end];i<N;i++)
            freq[c*N+i] = 0;
      while (++c<C);

      if (CC==2&&C==1)
      {
         for (i=0;i<N;i++)
            freq[N+i] = freq[i];
      }

      out_mem[0] = st->syn_mem[0]+MAX_PERIOD;
      if (CC==2)
         out_mem[1] = st->syn_mem[1]+MAX_PERIOD;

      overlap_mem[0] = (celt_sig*)(oldLogE2 + CC*st->mode->nbEBands);
      if (CC==2)
         overlap_mem[1] = overlap_mem[0] + st->overlap;

      compute_inv_mdcts(st->mode, shortBlocks, freq, out_mem, overlap_mem, CC, LM);

      c=0; do {
         st->prefilter_period=IMAX(st->prefilter_period, COMBFILTER_MINPERIOD);
         st->prefilter_period_old=IMAX(st->prefilter_period_old, COMBFILTER_MINPERIOD);
         comb_filter(out_mem[c], out_mem[c], st->prefilter_period_old, st->prefilter_period, st->mode->shortMdctSize,
               st->prefilter_gain_old, st->prefilter_gain, st->prefilter_tapset_old, st->prefilter_tapset,
               st->mode->window, st->overlap);
         if (LM!=0)
            comb_filter(out_mem[c]+st->mode->shortMdctSize, out_mem[c]+st->mode->shortMdctSize, st->prefilter_period, pitch_index, N-st->mode->shortMdctSize,
                  st->prefilter_gain, gain1, st->prefilter_tapset, prefilter_tapset,
                  st->mode->window, st->mode->overlap);
      } while (++c<CC);

      deemphasis(out_mem, (opus_val16*)pcm, N, CC, st->upsample, st->mode->preemph, st->preemph_memD);
      st->prefilter_period_old = st->prefilter_period;
      st->prefilter_gain_old = st->prefilter_gain;
      st->prefilter_tapset_old = st->prefilter_tapset;
   }
#endif

   st->prefilter_period = pitch_index;
   st->prefilter_gain = gain1;
   st->prefilter_tapset = prefilter_tapset;
#ifdef RESYNTH
   if (LM!=0)
   {
      st->prefilter_period_old = st->prefilter_period;
      st->prefilter_gain_old = st->prefilter_gain;
      st->prefilter_tapset_old = st->prefilter_tapset;
   }
#endif

   if (CC==2&&C==1) {
      for (i=0;i<st->mode->nbEBands;i++)
         oldBandE[st->mode->nbEBands+i]=oldBandE[i];
   }

   if (!isTransient)
   {
      for (i=0;i<CC*st->mode->nbEBands;i++)
         oldLogE2[i] = oldLogE[i];
      for (i=0;i<CC*st->mode->nbEBands;i++)
         oldLogE[i] = oldBandE[i];
   } else {
      for (i=0;i<CC*st->mode->nbEBands;i++)
         oldLogE[i] = MIN16(oldLogE[i], oldBandE[i]);
   }
   /* In case start or end were to change */
   c=0; do
   {
      for (i=0;i<st->start;i++)
      {
         oldBandE[c*st->mode->nbEBands+i]=0;
         oldLogE[c*st->mode->nbEBands+i]=oldLogE2[c*st->mode->nbEBands+i]=-QCONST16(28.f,DB_SHIFT);
      }
      for (i=st->end;i<st->mode->nbEBands;i++)
      {
         oldBandE[c*st->mode->nbEBands+i]=0;
         oldLogE[c*st->mode->nbEBands+i]=oldLogE2[c*st->mode->nbEBands+i]=-QCONST16(28.f,DB_SHIFT);
      }
   } while (++c<CC);

   if (isTransient)
      st->consec_transient++;
   else
      st->consec_transient=0;
   st->rng = enc->rng;

   /* If there's any room left (can only happen for very high rates),
      it's already filled with zeros */
   ec_enc_done(enc);

#ifdef CUSTOM_MODES
   if (st->signalling)
      nbCompressedBytes++;
#endif

   RESTORE_STACK;
   if (ec_get_error(enc))
      return OPUS_INTERNAL_ERROR;
   else
      return nbCompressedBytes;
}


#ifdef CUSTOM_MODES

#ifdef FIXED_POINT
int opus_custom_encode(CELTEncoder * OPUS_RESTRICT st, const opus_int16 * pcm, int frame_size, unsigned char *compressed, int nbCompressedBytes)
{
   return celt_encode_with_ec(st, pcm, frame_size, compressed, nbCompressedBytes, NULL);
}

#ifndef DISABLE_FLOAT_API
int opus_custom_encode_float(CELTEncoder * OPUS_RESTRICT st, const float * pcm, int frame_size, unsigned char *compressed, int nbCompressedBytes)
{
   int j, ret, C, N;
   VARDECL(opus_int16, in);
   ALLOC_STACK;

   if (pcm==NULL)
      return OPUS_BAD_ARG;

   C = st->channels;
   N = frame_size;
   ALLOC(in, C*N, opus_int16);

   for (j=0;j<C*N;j++)
     in[j] = FLOAT2INT16(pcm[j]);

   ret=celt_encode_with_ec(st,in,frame_size,compressed,nbCompressedBytes, NULL);
#ifdef RESYNTH
   for (j=0;j<C*N;j++)
      ((float*)pcm)[j]=in[j]*(1.f/32768.f);
#endif
   RESTORE_STACK;
   return ret;
}
#endif /* DISABLE_FLOAT_API */
#else

int opus_custom_encode(CELTEncoder * OPUS_RESTRICT st, const opus_int16 * pcm, int frame_size, unsigned char *compressed, int nbCompressedBytes)
{
   int j, ret, C, N;
   VARDECL(celt_sig, in);
   ALLOC_STACK;

   if (pcm==NULL)
      return OPUS_BAD_ARG;

   C=st->channels;
   N=frame_size;
   ALLOC(in, C*N, celt_sig);
   for (j=0;j<C*N;j++) {
     in[j] = SCALEOUT(pcm[j]);
   }

   ret = celt_encode_with_ec(st,in,frame_size,compressed,nbCompressedBytes, NULL);
#ifdef RESYNTH
   for (j=0;j<C*N;j++)
      ((opus_int16*)pcm)[j] = FLOAT2INT16(in[j]);
#endif
   RESTORE_STACK;
   return ret;
}

int opus_custom_encode_float(CELTEncoder * OPUS_RESTRICT st, const float * pcm, int frame_size, unsigned char *compressed, int nbCompressedBytes)
{
   return celt_encode_with_ec(st, pcm, frame_size, compressed, nbCompressedBytes, NULL);
}

#endif

#endif /* CUSTOM_MODES */

int opus_custom_encoder_ctl(CELTEncoder * OPUS_RESTRICT st, int request, ...)
{
   va_list ap;

   va_start(ap, request);
   switch (request)
   {
      case OPUS_SET_COMPLEXITY_REQUEST:
      {
         int value = va_arg(ap, opus_int32);
         if (value<0 || value>10)
            goto bad_arg;
         st->complexity = value;
      }
      break;
      case CELT_SET_START_BAND_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         if (value<0 || value>=st->mode->nbEBands)
            goto bad_arg;
         st->start = value;
      }
      break;
      case CELT_SET_END_BAND_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         if (value<1 || value>st->mode->nbEBands)
            goto bad_arg;
         st->end = value;
      }
      break;
      case CELT_SET_PREDICTION_REQUEST:
      {
         int value = va_arg(ap, opus_int32);
         if (value<0 || value>2)
            goto bad_arg;
         st->disable_pf = value<=1;
         st->force_intra = value==0;
      }
      break;
      case OPUS_SET_PACKET_LOSS_PERC_REQUEST:
      {
         int value = va_arg(ap, opus_int32);
         if (value<0 || value>100)
            goto bad_arg;
         st->loss_rate = value;
      }
      break;
      case OPUS_SET_VBR_CONSTRAINT_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         st->constrained_vbr = value;
      }
      break;
      case OPUS_SET_VBR_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         st->vbr = value;
      }
      break;
      case OPUS_SET_BITRATE_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         if (value<=500 && value!=OPUS_BITRATE_MAX)
            goto bad_arg;
         value = IMIN(value, 260000*st->channels);
         st->bitrate = value;
      }
      break;
      case CELT_SET_CHANNELS_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         if (value<1 || value>2)
            goto bad_arg;
         st->stream_channels = value;
      }
      break;
      case OPUS_SET_LSB_DEPTH_REQUEST:
      {
          opus_int32 value = va_arg(ap, opus_int32);
          if (value<8 || value>24)
             goto bad_arg;
          st->lsb_depth=value;
      }
      break;
      case OPUS_GET_LSB_DEPTH_REQUEST:
      {
          opus_int32 *value = va_arg(ap, opus_int32*);
          *value=st->lsb_depth;
      }
      break;
      case OPUS_RESET_STATE:
      {
         int i;
         opus_val16 *oldBandE, *oldLogE, *oldLogE2;
         oldBandE = (opus_val16*)(st->in_mem+st->channels*(st->overlap+COMBFILTER_MAXPERIOD));
         oldLogE = oldBandE + st->channels*st->mode->nbEBands;
         oldLogE2 = oldLogE + st->channels*st->mode->nbEBands;
         OPUS_CLEAR((char*)&st->ENCODER_RESET_START,
               opus_custom_encoder_get_size(st->mode, st->channels)-
               ((char*)&st->ENCODER_RESET_START - (char*)st));
         for (i=0;i<st->channels*st->mode->nbEBands;i++)
            oldLogE[i]=oldLogE2[i]=-QCONST16(28.f,DB_SHIFT);
         st->vbr_offset = 0;
         st->delayedIntra = 1;
         st->spread_decision = SPREAD_NORMAL;
         st->tonal_average = 256;
         st->hf_average = 0;
         st->tapset_decision = 0;
      }
      break;
#ifdef CUSTOM_MODES
      case CELT_SET_INPUT_CLIPPING_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         st->clip = value;
      }
      break;
#endif
      case CELT_SET_SIGNALLING_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         st->signalling = value;
      }
      break;
      case CELT_GET_MODE_REQUEST:
      {
         const CELTMode ** value = va_arg(ap, const CELTMode**);
         if (value==0)
            goto bad_arg;
         *value=st->mode;
      }
      break;
      case OPUS_GET_FINAL_RANGE_REQUEST:
      {
         opus_uint32 * value = va_arg(ap, opus_uint32 *);
         if (value==0)
            goto bad_arg;
         *value=st->rng;
      }
      break;
      default:
         goto bad_request;
   }
   va_end(ap);
   return OPUS_OK;
bad_arg:
   va_end(ap);
   return OPUS_BAD_ARG;
bad_request:
   va_end(ap);
   return OPUS_UNIMPLEMENTED;
}
#endif /* OPUS_ENABLE_ENCODER */

/**********************************************************************/
/*                                                                    */
/*                             DECODER                                */
/*                                                                    */
/**********************************************************************/
#define DECODE_BUFFER_SIZE 2048

/** Decoder state
 @brief Decoder state
 */
struct OpusCustomDecoder {
   const OpusCustomMode *mode;
   int overlap;
   int channels;
   int stream_channels;

   int downsample;
   int start, end;
   int signalling;

   /* Everything beyond this point gets cleared on a reset */
#define DECODER_RESET_START rng

   opus_uint32 rng;
   int error;
   int last_pitch_index;
   int loss_count;
   int postfilter_period;
   int postfilter_period_old;
   opus_val16 postfilter_gain;
   opus_val16 postfilter_gain_old;
   int postfilter_tapset;
   int postfilter_tapset_old;

   celt_sig preemph_memD[2];

   celt_sig _decode_mem[1]; /* Size = channels*(DECODE_BUFFER_SIZE+mode->overlap) */
   /* opus_val16 lpc[],  Size = channels*LPC_ORDER */
   /* opus_val16 oldEBands[], Size = 2*mode->nbEBands */
   /* opus_val16 oldLogE[], Size = 2*mode->nbEBands */
   /* opus_val16 oldLogE2[], Size = 2*mode->nbEBands */
   /* opus_val16 backgroundLogE[], Size = 2*mode->nbEBands */
};

int celt_decoder_get_size(int channels)
{
   const CELTMode *mode = opus_custom_mode_create(48000, 960, NULL);
   return opus_custom_decoder_get_size(mode, channels);
}

OPUS_CUSTOM_NOSTATIC int opus_custom_decoder_get_size(const CELTMode *mode, int channels)
{
   int size = sizeof(struct CELTDecoder)
            + (channels*(DECODE_BUFFER_SIZE+mode->overlap)-1)*sizeof(celt_sig)
            + channels*LPC_ORDER*sizeof(opus_val16)
            + 4*2*mode->nbEBands*sizeof(opus_val16);
   return size;
}

#ifdef CUSTOM_MODES
CELTDecoder *opus_custom_decoder_create(const CELTMode *mode, int channels, int *error)
{
   int ret;
   CELTDecoder *st = (CELTDecoder *)opus_alloc(opus_custom_decoder_get_size(mode, channels));
   ret = opus_custom_decoder_init(st, mode, channels);
   if (ret != OPUS_OK)
   {
      opus_custom_decoder_destroy(st);
      st = NULL;
   }
   if (error)
      *error = ret;
   return st;
}
#endif /* CUSTOM_MODES */

int celt_decoder_init(CELTDecoder *st, opus_int32 sampling_rate, int channels)
{
   int ret;
   ret = opus_custom_decoder_init(st, opus_custom_mode_create(48000, 960, NULL), channels);
   if (ret != OPUS_OK)
      return ret;
   st->downsample = resampling_factor(sampling_rate);
   if (st->downsample==0)
      return OPUS_BAD_ARG;
   else
      return OPUS_OK;
}

OPUS_CUSTOM_NOSTATIC int opus_custom_decoder_init(CELTDecoder *st, const CELTMode *mode, int channels)
{
   if (channels < 0 || channels > 2)
      return OPUS_BAD_ARG;

   if (st==NULL)
      return OPUS_ALLOC_FAIL;

   OPUS_CLEAR((char*)st, opus_custom_decoder_get_size(mode, channels));

   st->mode = mode;
   st->overlap = mode->overlap;
   st->stream_channels = st->channels = channels;

   st->downsample = 1;
   st->start = 0;
   st->end = st->mode->effEBands;
   st->signalling = 1;

   st->loss_count = 0;

   opus_custom_decoder_ctl(st, OPUS_RESET_STATE);

   return OPUS_OK;
}

#ifdef CUSTOM_MODES
void opus_custom_decoder_destroy(CELTDecoder *st)
{
   opus_free(st);
}
#endif /* CUSTOM_MODES */

static void celt_decode_lost(CELTDecoder * OPUS_RESTRICT st, opus_val16 * OPUS_RESTRICT pcm, int N, int LM)
{
   int c;
   int pitch_index;
   opus_val16 fade = Q15ONE;
   int i, len;
   const int C = st->channels;
   int offset;
   celt_sig *out_mem[2];
   celt_sig *decode_mem[2];
   celt_sig *overlap_mem[2];
   opus_val16 *lpc;
   opus_val32 *out_syn[2];
   opus_val16 *oldBandE, *oldLogE, *oldLogE2, *backgroundLogE;
   const OpusCustomMode *mode;
   int nbEBands;
   int overlap;
   const opus_int16 *eBands;
   SAVE_STACK;

   mode = st->mode;
   nbEBands = mode->nbEBands;
   overlap = mode->overlap;
   eBands = mode->eBands;

   c=0; do {
      decode_mem[c] = st->_decode_mem + c*(DECODE_BUFFER_SIZE+st->overlap);
      out_mem[c] = decode_mem[c]+DECODE_BUFFER_SIZE-MAX_PERIOD;
      overlap_mem[c] = decode_mem[c]+DECODE_BUFFER_SIZE;
   } while (++c<C);
   lpc = (opus_val16*)(st->_decode_mem+(DECODE_BUFFER_SIZE+st->overlap)*C);
   oldBandE = lpc+C*LPC_ORDER;
   oldLogE = oldBandE + 2*nbEBands;
   oldLogE2 = oldLogE + 2*nbEBands;
   backgroundLogE = oldLogE2  + 2*nbEBands;

   c=0; do {
      out_syn[c] = out_mem[c]+MAX_PERIOD-N;
   } while (++c<C);

   len = N+overlap;

   if (st->loss_count >= 5 || st->start!=0)
   {
      /* Noise-based PLC/CNG */
      VARDECL(celt_sig, freq);
      VARDECL(celt_norm, X);
      VARDECL(celt_ener, bandE);
      opus_uint32 seed;
      int effEnd;

      effEnd = st->end;
      if (effEnd > mode->effEBands)
         effEnd = mode->effEBands;

      ALLOC(freq, C*N, celt_sig); /**< Interleaved signal MDCTs */
      ALLOC(X, C*N, celt_norm);   /**< Interleaved normalised MDCTs */
      ALLOC(bandE, nbEBands*C, celt_ener);

      if (st->loss_count >= 5)
         log2Amp(mode, st->start, st->end, bandE, backgroundLogE, C);
      else {
         /* Energy decay */
         opus_val16 decay = st->loss_count==0 ? QCONST16(1.5f, DB_SHIFT) : QCONST16(.5f, DB_SHIFT);
         c=0; do
         {
            for (i=st->start;i<st->end;i++)
               oldBandE[c*nbEBands+i] -= decay;
         } while (++c<C);
         log2Amp(mode, st->start, st->end, bandE, oldBandE, C);
      }
      seed = st->rng;
      for (c=0;c<C;c++)
      {
         for (i=0;i<(st->mode->eBands[st->start]<<LM);i++)
            X[c*N+i] = 0;
         for (i=st->start;i<mode->effEBands;i++)
         {
            int j;
            int boffs;
            int blen;
            boffs = N*c+(eBands[i]<<LM);
            blen = (eBands[i+1]-eBands[i])<<LM;
            for (j=0;j<blen;j++)
            {
               seed = celt_lcg_rand(seed);
               X[boffs+j] = (celt_norm)((opus_int32)seed>>20);
            }
            renormalise_vector(X+boffs, blen, Q15ONE);
         }
         for (i=(st->mode->eBands[st->end]<<LM);i<N;i++)
            X[c*N+i] = 0;
      }
      st->rng = seed;

      denormalise_bands(mode, X, freq, bandE, mode->effEBands, C, 1<<LM);

      c=0; do
         for (i=0;i<st->mode->eBands[st->start]<<LM;i++)
            freq[c*N+i] = 0;
      while (++c<C);
      c=0; do {
         int bound = eBands[effEnd]<<LM;
         if (st->downsample!=1)
            bound = IMIN(bound, N/st->downsample);
         for (i=bound;i<N;i++)
            freq[c*N+i] = 0;
      } while (++c<C);
      c=0; do {
         OPUS_MOVE(decode_mem[c], decode_mem[c]+N, DECODE_BUFFER_SIZE-N+overlap);
      } while (++c<C);
      compute_inv_mdcts(mode, 0, freq, out_syn, overlap_mem, C, LM);
   } else {
      /* Pitch-based PLC */
      VARDECL(opus_val32, etmp);

      if (st->loss_count == 0)
      {
         opus_val16 pitch_buf[DECODE_BUFFER_SIZE>>1];
         /* Corresponds to a min pitch of 67 Hz. It's possible to save CPU in this
         search by using only part of the decode buffer */
         int poffset = 720;
         pitch_downsample(decode_mem, pitch_buf, DECODE_BUFFER_SIZE, C);
         /* Max pitch is 100 samples (480 Hz) */
         pitch_search(pitch_buf+((poffset)>>1), pitch_buf, DECODE_BUFFER_SIZE-poffset,
               poffset-100, &pitch_index);
         pitch_index = poffset-pitch_index;
         st->last_pitch_index = pitch_index;
      } else {
         pitch_index = st->last_pitch_index;
         fade = QCONST16(.8f,15);
      }

      ALLOC(etmp, overlap, opus_val32);
      c=0; do {
         opus_val16 exc[MAX_PERIOD];
         opus_val32 ac[LPC_ORDER+1];
         opus_val16 decay;
         opus_val16 attenuation;
         opus_val32 S1=0;
         opus_val16 mem[LPC_ORDER]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
         opus_val32 *e = out_syn[c];


         offset = MAX_PERIOD-pitch_index;
         for (i=0;i<MAX_PERIOD;i++)
            exc[i] = ROUND16(out_mem[c][i], SIG_SHIFT);

         /* Compute LPC coefficients for the last MAX_PERIOD samples before the loss so we can
            work in the excitation-filter domain */
         if (st->loss_count == 0)
         {
            _celt_autocorr(exc, ac, mode->window, overlap,
                  LPC_ORDER, MAX_PERIOD);

            /* Noise floor -40 dB */
#ifdef FIXED_POINT
            ac[0] += SHR32(ac[0],13);
#else
            ac[0] *= 1.0001f;
#endif
            /* Lag windowing */
            for (i=1;i<=LPC_ORDER;i++)
            {
               /*ac[i] *= exp(-.5*(2*M_PI*.002*i)*(2*M_PI*.002*i));*/
#ifdef FIXED_POINT
               ac[i] -= MULT16_32_Q15(2*i*i, ac[i]);
#else
               ac[i] -= ac[i]*(.008f*i)*(.008f*i);
#endif
            }

            _celt_lpc(lpc+c*LPC_ORDER, ac, LPC_ORDER);
         }
         /* Samples just before the beginning of exc  */
         for (i=0;i<LPC_ORDER;i++)
            mem[i] = ROUND16(out_mem[c][-1-i], SIG_SHIFT);
         /* Compute the excitation for MAX_PERIOD samples before the loss */
         celt_fir(exc, lpc+c*LPC_ORDER, exc, MAX_PERIOD, LPC_ORDER, mem);

         /* Check if the waveform is decaying (and if so how fast)
            We do this to avoid adding energy when concealing in a segment
            with decaying energy */
         {
            opus_val32 E1=1, E2=1;
            int period;
#ifdef FIXED_POINT
            int shift;
#endif

            if (pitch_index <= MAX_PERIOD/2)
               period = pitch_index;
            else
               period = MAX_PERIOD/2;
#ifdef FIXED_POINT
            shift = IMAX(0,2*celt_zlog2(celt_maxabs16(&exc[MAX_PERIOD-2*period], 2*period))-20);
#endif
            for (i=0;i<period;i++)
            {
               E1 += SHR32(MULT16_16(exc[MAX_PERIOD-period+i],exc[MAX_PERIOD-period+i]),shift);
               E2 += SHR32(MULT16_16(exc[MAX_PERIOD-2*period+i],exc[MAX_PERIOD-2*period+i]),shift);
            }
            if (E1 > E2)
               E1 = E2;
            decay = celt_sqrt(frac_div32(SHR32(E1,1),E2));
            attenuation = decay;
         }

         /* Move memory one frame to the left */
         OPUS_MOVE(decode_mem[c], decode_mem[c]+N, DECODE_BUFFER_SIZE-N+overlap);

         /* Extrapolate excitation with the right period, taking decay into account */
         for (i=0;i<len;i++)
         {
            opus_val16 tmp;
            if (offset+i >= MAX_PERIOD)
            {
               offset -= pitch_index;
               attenuation = MULT16_16_Q15(attenuation, decay);
            }
            e[i] = SHL32(EXTEND32(MULT16_16_Q15(attenuation, exc[offset+i])), SIG_SHIFT);
            /* Compute the energy of the previously decoded signal whose
               excitation we're copying */
            tmp = ROUND16(out_mem[c][-N+offset+i],SIG_SHIFT);
            S1 += SHR32(MULT16_16(tmp,tmp),8);
         }

         /* Copy the last decoded samples (prior to the overlap region) to
            synthesis filter memory so we can have a continuous signal. */
         for (i=0;i<LPC_ORDER;i++)
            mem[i] = ROUND16(out_mem[c][MAX_PERIOD-N-1-i], SIG_SHIFT);
         /* Apply the fading if not the first loss */
         for (i=0;i<len;i++)
            e[i] = MULT16_32_Q15(fade, e[i]);
         /* Synthesis filter -- back in the signal domain */
         celt_iir(e, lpc+c*LPC_ORDER, e, len, LPC_ORDER, mem);

         /* Check if the synthesis energy is higher than expected, which can
            happen with the signal changes during our window. If so, attenuate. */
         {
            opus_val32 S2=0;
            for (i=0;i<len;i++)
            {
               opus_val16 tmp = ROUND16(e[i],SIG_SHIFT);
               S2 += SHR32(MULT16_16(tmp,tmp),8);
            }
            /* This checks for an "explosion" in the synthesis */
#ifdef FIXED_POINT
            if (!(S1 > SHR32(S2,2)))
#else
            /* Float test is written this way to catch NaNs at the same time */
            if (!(S1 > 0.2f*S2))
#endif
            {
               for (i=0;i<len;i++)
                  e[i] = 0;
            } else if (S1 < S2)
            {
               opus_val16 ratio = celt_sqrt(frac_div32(SHR32(S1,1)+1,S2+1));
               for (i=0;i<overlap;i++)
               {
                  opus_val16 tmp_g = Q15ONE - MULT16_16_Q15(mode->window[i], Q15ONE-ratio);
                  e[i] = MULT16_32_Q15(tmp_g, e[i]);
               }
               for (i=overlap;i<len;i++)
                  e[i] = MULT16_32_Q15(ratio, e[i]);
            }
         }

         /* Apply pre-filter to the MDCT overlap for the next frame because the
            post-filter will be re-applied in the decoder after the MDCT overlap */
         comb_filter(etmp, out_mem[c]+MAX_PERIOD, st->postfilter_period, st->postfilter_period, st->overlap,
               -st->postfilter_gain, -st->postfilter_gain, st->postfilter_tapset, st->postfilter_tapset,
               NULL, 0);

         /* Simulate TDAC on the concealed audio so that it blends with the
            MDCT of next frames. */
         for (i=0;i<overlap/2;i++)
         {
            opus_val32 tmp;
            tmp = MULT16_32_Q15(mode->window[i],           etmp[overlap-1-i]) +
                  MULT16_32_Q15(mode->window[overlap-i-1], etmp[i          ]);
            out_mem[c][MAX_PERIOD+i] = MULT16_32_Q15(mode->window[overlap-i-1], tmp);
            out_mem[c][MAX_PERIOD+overlap-i-1] = MULT16_32_Q15(mode->window[i], tmp);
         }
      } while (++c<C);
   }

   deemphasis(out_syn, pcm, N, C, st->downsample, mode->preemph, st->preemph_memD);

   st->loss_count++;

   RESTORE_STACK;
}

int celt_decode_with_ec(CELTDecoder * OPUS_RESTRICT st, const unsigned char *data, int len, opus_val16 * OPUS_RESTRICT pcm, int frame_size, ec_dec *dec)
{
   int c, i, N;
   int spread_decision;
   opus_int32 bits;
   ec_dec _dec;
   VARDECL(celt_sig, freq);
   VARDECL(celt_norm, X);
   VARDECL(celt_ener, bandE);
   VARDECL(int, fine_quant);
   VARDECL(int, pulses);
   VARDECL(int, cap);
   VARDECL(int, offsets);
   VARDECL(int, fine_priority);
   VARDECL(int, tf_res);
   VARDECL(unsigned char, collapse_masks);
   celt_sig *out_mem[2];
   celt_sig *decode_mem[2];
   celt_sig *overlap_mem[2];
   celt_sig *out_syn[2];
   opus_val16 *lpc;
   opus_val16 *oldBandE, *oldLogE, *oldLogE2, *backgroundLogE;

   int shortBlocks;
   int isTransient;
   int intra_ener;
   const int CC = st->channels;
   int LM, M;
   int effEnd;
   int codedBands;
   int alloc_trim;
   int postfilter_pitch;
   opus_val16 postfilter_gain;
   int intensity=0;
   int dual_stereo=0;
   opus_int32 total_bits;
   opus_int32 balance;
   opus_int32 tell;
   int dynalloc_logp;
   int postfilter_tapset;
   int anti_collapse_rsv;
   int anti_collapse_on=0;
   int silence;
   int C = st->stream_channels;
   ALLOC_STACK;

   frame_size *= st->downsample;

   c=0; do {
      decode_mem[c] = st->_decode_mem + c*(DECODE_BUFFER_SIZE+st->overlap);
      out_mem[c] = decode_mem[c]+DECODE_BUFFER_SIZE-MAX_PERIOD;
      overlap_mem[c] = decode_mem[c]+DECODE_BUFFER_SIZE;
   } while (++c<CC);
   lpc = (opus_val16*)(st->_decode_mem+(DECODE_BUFFER_SIZE+st->overlap)*CC);
   oldBandE = lpc+CC*LPC_ORDER;
   oldLogE = oldBandE + 2*st->mode->nbEBands;
   oldLogE2 = oldLogE + 2*st->mode->nbEBands;
   backgroundLogE = oldLogE2  + 2*st->mode->nbEBands;

#ifdef CUSTOM_MODES
   if (st->signalling && data!=NULL)
   {
      int data0=data[0];
      /* Convert "standard mode" to Opus header */
      if (st->mode->Fs==48000 && st->mode->shortMdctSize==120)
      {
         data0 = fromOpus(data0);
         if (data0<0)
            return OPUS_INVALID_PACKET;
      }
      st->end = IMAX(1, st->mode->effEBands-2*(data0>>5));
      LM = (data0>>3)&0x3;
      C = 1 + ((data0>>2)&0x1);
      data++;
      len--;
      if (LM>st->mode->maxLM)
         return OPUS_INVALID_PACKET;
      if (frame_size < st->mode->shortMdctSize<<LM)
         return OPUS_BUFFER_TOO_SMALL;
      else
         frame_size = st->mode->shortMdctSize<<LM;
   } else {
#else
   {
#endif
      for (LM=0;LM<=st->mode->maxLM;LM++)
         if (st->mode->shortMdctSize<<LM==frame_size)
            break;
      if (LM>st->mode->maxLM)
         return OPUS_BAD_ARG;
   }
   M=1<<LM;

   if (len<0 || len>1275 || pcm==NULL)
      return OPUS_BAD_ARG;

   N = M*st->mode->shortMdctSize;

   effEnd = st->end;
   if (effEnd > st->mode->effEBands)
      effEnd = st->mode->effEBands;

   if (data == NULL || len<=1)
   {
      celt_decode_lost(st, pcm, N, LM);
      RESTORE_STACK;
      return frame_size/st->downsample;
   }

   ALLOC(freq, IMAX(CC,C)*N, celt_sig); /**< Interleaved signal MDCTs */
   ALLOC(X, C*N, celt_norm);   /**< Interleaved normalised MDCTs */
   ALLOC(bandE, st->mode->nbEBands*C, celt_ener);
   c=0; do
      for (i=0;i<M*st->mode->eBands[st->start];i++)
         X[c*N+i] = 0;
   while (++c<C);
   c=0; do
      for (i=M*st->mode->eBands[effEnd];i<N;i++)
         X[c*N+i] = 0;
   while (++c<C);

   if (dec == NULL)
   {
      ec_dec_init(&_dec,(unsigned char*)data,len);
      dec = &_dec;
   }

   if (C==1)
   {
      for (i=0;i<st->mode->nbEBands;i++)
         oldBandE[i]=MAX16(oldBandE[i],oldBandE[st->mode->nbEBands+i]);
   }

   total_bits = len*8;
   tell = ec_tell(dec);

   if (tell >= total_bits)
      silence = 1;
   else if (tell==1)
      silence = ec_dec_bit_logp(dec, 15);
   else
      silence = 0;
   if (silence)
   {
      /* Pretend we've read all the remaining bits */
      tell = len*8;
      dec->nbits_total+=tell-ec_tell(dec);
   }

   postfilter_gain = 0;
   postfilter_pitch = 0;
   postfilter_tapset = 0;
   if (st->start==0 && tell+16 <= total_bits)
   {
      if(ec_dec_bit_logp(dec, 1))
      {
         int qg, octave;
         octave = ec_dec_uint(dec, 6);
         postfilter_pitch = (16<<octave)+ec_dec_bits(dec, 4+octave)-1;
         qg = ec_dec_bits(dec, 3);
         if (ec_tell(dec)+2<=total_bits)
            postfilter_tapset = ec_dec_icdf(dec, tapset_icdf, 2);
         postfilter_gain = QCONST16(.09375f,15)*(qg+1);
      }
      tell = ec_tell(dec);
   }

   if (LM > 0 && tell+3 <= total_bits)
   {
      isTransient = ec_dec_bit_logp(dec, 3);
      tell = ec_tell(dec);
   }
   else
      isTransient = 0;

   if (isTransient)
      shortBlocks = M;
   else
      shortBlocks = 0;

   /* Decode the global flags (first symbols in the stream) */
   intra_ener = tell+3<=total_bits ? ec_dec_bit_logp(dec, 3) : 0;
   /* Get band energies */
   unquant_coarse_energy(st->mode, st->start, st->end, oldBandE,
         intra_ener, dec, C, LM);

   ALLOC(tf_res, st->mode->nbEBands, int);
   tf_decode(st->start, st->end, isTransient, tf_res, LM, dec);

   tell = ec_tell(dec);
   spread_decision = SPREAD_NORMAL;
   if (tell+4 <= total_bits)
      spread_decision = ec_dec_icdf(dec, spread_icdf, 5);

   ALLOC(pulses, st->mode->nbEBands, int);
   ALLOC(cap, st->mode->nbEBands, int);
   ALLOC(offsets, st->mode->nbEBands, int);
   ALLOC(fine_priority, st->mode->nbEBands, int);

   init_caps(st->mode,cap,LM,C);

   dynalloc_logp = 6;
   total_bits<<=BITRES;
   tell = ec_tell_frac(dec);
   for (i=st->start;i<st->end;i++)
   {
      int width, quanta;
      int dynalloc_loop_logp;
      int boost;
      width = C*(st->mode->eBands[i+1]-st->mode->eBands[i])<<LM;
      /* quanta is 6 bits, but no more than 1 bit/sample
         and no less than 1/8 bit/sample */
      quanta = IMIN(width<<BITRES, IMAX(6<<BITRES, width));
      dynalloc_loop_logp = dynalloc_logp;
      boost = 0;
      while (tell+(dynalloc_loop_logp<<BITRES) < total_bits && boost < cap[i])
      {
         int flag;
         flag = ec_dec_bit_logp(dec, dynalloc_loop_logp);
         tell = ec_tell_frac(dec);
         if (!flag)
            break;
         boost += quanta;
         total_bits -= quanta;
         dynalloc_loop_logp = 1;
      }
      offsets[i] = boost;
      /* Making dynalloc more likely */
      if (boost>0)
         dynalloc_logp = IMAX(2, dynalloc_logp-1);
   }

   ALLOC(fine_quant, st->mode->nbEBands, int);
   alloc_trim = tell+(6<<BITRES) <= total_bits ?
         ec_dec_icdf(dec, trim_icdf, 7) : 5;

   bits = (((opus_int32)len*8)<<BITRES) - ec_tell_frac(dec) - 1;
   anti_collapse_rsv = isTransient&&LM>=2&&bits>=((LM+2)<<BITRES) ? (1<<BITRES) : 0;
   bits -= anti_collapse_rsv;
   codedBands = compute_allocation(st->mode, st->start, st->end, offsets, cap,
         alloc_trim, &intensity, &dual_stereo, bits, &balance, pulses,
         fine_quant, fine_priority, C, LM, dec, 0, 0);

   unquant_fine_energy(st->mode, st->start, st->end, oldBandE, fine_quant, dec, C);

   /* Decode fixed codebook */
   ALLOC(collapse_masks, C*st->mode->nbEBands, unsigned char);
   quant_all_bands(0, st->mode, st->start, st->end, X, C==2 ? X+N : NULL, collapse_masks,
         NULL, pulses, shortBlocks, spread_decision, dual_stereo, intensity, tf_res,
         len*(8<<BITRES)-anti_collapse_rsv, balance, dec, LM, codedBands, &st->rng);

   if (anti_collapse_rsv > 0)
   {
      anti_collapse_on = ec_dec_bits(dec, 1);
   }

   unquant_energy_finalise(st->mode, st->start, st->end, oldBandE,
         fine_quant, fine_priority, len*8-ec_tell(dec), dec, C);

   if (anti_collapse_on)
      anti_collapse(st->mode, X, collapse_masks, LM, C, N,
            st->start, st->end, oldBandE, oldLogE, oldLogE2, pulses, st->rng);

   log2Amp(st->mode, st->start, st->end, bandE, oldBandE, C);

   if (silence)
   {
      for (i=0;i<C*st->mode->nbEBands;i++)
      {
         bandE[i] = 0;
         oldBandE[i] = -QCONST16(28.f,DB_SHIFT);
      }
   }
   /* Synthesis */
   denormalise_bands(st->mode, X, freq, bandE, effEnd, C, M);

   OPUS_MOVE(decode_mem[0], decode_mem[0]+N, DECODE_BUFFER_SIZE-N);
   if (CC==2)
      OPUS_MOVE(decode_mem[1], decode_mem[1]+N, DECODE_BUFFER_SIZE-N);

   c=0; do
      for (i=0;i<M*st->mode->eBands[st->start];i++)
         freq[c*N+i] = 0;
   while (++c<C);
   c=0; do {
      int bound = M*st->mode->eBands[effEnd];
      if (st->downsample!=1)
         bound = IMIN(bound, N/st->downsample);
      for (i=bound;i<N;i++)
         freq[c*N+i] = 0;
   } while (++c<C);

   out_syn[0] = out_mem[0]+MAX_PERIOD-N;
   if (CC==2)
      out_syn[1] = out_mem[1]+MAX_PERIOD-N;

   if (CC==2&&C==1)
   {
      for (i=0;i<N;i++)
         freq[N+i] = freq[i];
   }
   if (CC==1&&C==2)
   {
      for (i=0;i<N;i++)
         freq[i] = HALF32(ADD32(freq[i],freq[N+i]));
   }

   /* Compute inverse MDCTs */
   compute_inv_mdcts(st->mode, shortBlocks, freq, out_syn, overlap_mem, CC, LM);

   c=0; do {
      st->postfilter_period=IMAX(st->postfilter_period, COMBFILTER_MINPERIOD);
      st->postfilter_period_old=IMAX(st->postfilter_period_old, COMBFILTER_MINPERIOD);
      comb_filter(out_syn[c], out_syn[c], st->postfilter_period_old, st->postfilter_period, st->mode->shortMdctSize,
            st->postfilter_gain_old, st->postfilter_gain, st->postfilter_tapset_old, st->postfilter_tapset,
            st->mode->window, st->overlap);
      if (LM!=0)
         comb_filter(out_syn[c]+st->mode->shortMdctSize, out_syn[c]+st->mode->shortMdctSize, st->postfilter_period, postfilter_pitch, N-st->mode->shortMdctSize,
               st->postfilter_gain, postfilter_gain, st->postfilter_tapset, postfilter_tapset,
               st->mode->window, st->mode->overlap);

   } while (++c<CC);
   st->postfilter_period_old = st->postfilter_period;
   st->postfilter_gain_old = st->postfilter_gain;
   st->postfilter_tapset_old = st->postfilter_tapset;
   st->postfilter_period = postfilter_pitch;
   st->postfilter_gain = postfilter_gain;
   st->postfilter_tapset = postfilter_tapset;
   if (LM!=0)
   {
      st->postfilter_period_old = st->postfilter_period;
      st->postfilter_gain_old = st->postfilter_gain;
      st->postfilter_tapset_old = st->postfilter_tapset;
   }

   if (C==1) {
      for (i=0;i<st->mode->nbEBands;i++)
         oldBandE[st->mode->nbEBands+i]=oldBandE[i];
   }

   /* In case start or end were to change */
   if (!isTransient)
   {
      for (i=0;i<2*st->mode->nbEBands;i++)
         oldLogE2[i] = oldLogE[i];
      for (i=0;i<2*st->mode->nbEBands;i++)
         oldLogE[i] = oldBandE[i];
      for (i=0;i<2*st->mode->nbEBands;i++)
         backgroundLogE[i] = MIN16(backgroundLogE[i] + M*QCONST16(0.001f,DB_SHIFT), oldBandE[i]);
   } else {
      for (i=0;i<2*st->mode->nbEBands;i++)
         oldLogE[i] = MIN16(oldLogE[i], oldBandE[i]);
   }
   c=0; do
   {
      for (i=0;i<st->start;i++)
      {
         oldBandE[c*st->mode->nbEBands+i]=0;
         oldLogE[c*st->mode->nbEBands+i]=oldLogE2[c*st->mode->nbEBands+i]=-QCONST16(28.f,DB_SHIFT);
      }
      for (i=st->end;i<st->mode->nbEBands;i++)
      {
         oldBandE[c*st->mode->nbEBands+i]=0;
         oldLogE[c*st->mode->nbEBands+i]=oldLogE2[c*st->mode->nbEBands+i]=-QCONST16(28.f,DB_SHIFT);
      }
   } while (++c<2);
   st->rng = dec->rng;

   deemphasis(out_syn, pcm, N, CC, st->downsample, st->mode->preemph, st->preemph_memD);
   st->loss_count = 0;
   RESTORE_STACK;
   if (ec_tell(dec) > 8*len)
      return OPUS_INTERNAL_ERROR;
   if(ec_get_error(dec))
      st->error = 1;
   return frame_size/st->downsample;
}


#ifdef CUSTOM_MODES

#ifdef FIXED_POINT
int opus_custom_decode(CELTDecoder * OPUS_RESTRICT st, const unsigned char *data, int len, opus_int16 * OPUS_RESTRICT pcm, int frame_size)
{
   return celt_decode_with_ec(st, data, len, pcm, frame_size, NULL);
}

#ifndef DISABLE_FLOAT_API
int opus_custom_decode_float(CELTDecoder * OPUS_RESTRICT st, const unsigned char *data, int len, float * OPUS_RESTRICT pcm, int frame_size)
{
   int j, ret, C, N;
   VARDECL(opus_int16, out);
   ALLOC_STACK;

   if (pcm==NULL)
      return OPUS_BAD_ARG;

   C = st->channels;
   N = frame_size;

   ALLOC(out, C*N, opus_int16);
   ret=celt_decode_with_ec(st, data, len, out, frame_size, NULL);
   if (ret>0)
      for (j=0;j<C*ret;j++)
         pcm[j]=out[j]*(1.f/32768.f);

   RESTORE_STACK;
   return ret;
}
#endif /* DISABLE_FLOAT_API */

#else

int opus_custom_decode_float(CELTDecoder * OPUS_RESTRICT st, const unsigned char *data, int len, float * OPUS_RESTRICT pcm, int frame_size)
{
   return celt_decode_with_ec(st, data, len, pcm, frame_size, NULL);
}

int opus_custom_decode(CELTDecoder * OPUS_RESTRICT st, const unsigned char *data, int len, opus_int16 * OPUS_RESTRICT pcm, int frame_size)
{
   int j, ret, C, N;
   VARDECL(celt_sig, out);
   ALLOC_STACK;

   if (pcm==NULL)
      return OPUS_BAD_ARG;

   C = st->channels;
   N = frame_size;
   ALLOC(out, C*N, celt_sig);

   ret=celt_decode_with_ec(st, data, len, out, frame_size, NULL);

   if (ret>0)
      for (j=0;j<C*ret;j++)
         pcm[j] = FLOAT2INT16 (out[j]);

   RESTORE_STACK;
   return ret;
}

#endif
#endif /* CUSTOM_MODES */

int opus_custom_decoder_ctl(CELTDecoder * OPUS_RESTRICT st, int request, ...)
{
   va_list ap;

   va_start(ap, request);
   switch (request)
   {
      case CELT_SET_START_BAND_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         if (value<0 || value>=st->mode->nbEBands)
            goto bad_arg;
         st->start = value;
      }
      break;
      case CELT_SET_END_BAND_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         if (value<1 || value>st->mode->nbEBands)
            goto bad_arg;
         st->end = value;
      }
      break;
      case CELT_SET_CHANNELS_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         if (value<1 || value>2)
            goto bad_arg;
         st->stream_channels = value;
      }
      break;
      case CELT_GET_AND_CLEAR_ERROR_REQUEST:
      {
         opus_int32 *value = va_arg(ap, opus_int32*);
         if (value==NULL)
            goto bad_arg;
         *value=st->error;
         st->error = 0;
      }
      break;
      case OPUS_GET_LOOKAHEAD_REQUEST:
      {
         opus_int32 *value = va_arg(ap, opus_int32*);
         if (value==NULL)
            goto bad_arg;
         *value = st->overlap/st->downsample;
      }
      break;
      case OPUS_RESET_STATE:
      {
         int i;
         opus_val16 *lpc, *oldBandE, *oldLogE, *oldLogE2;
         lpc = (opus_val16*)(st->_decode_mem+(DECODE_BUFFER_SIZE+st->overlap)*st->channels);
         oldBandE = lpc+st->channels*LPC_ORDER;
         oldLogE = oldBandE + 2*st->mode->nbEBands;
         oldLogE2 = oldLogE + 2*st->mode->nbEBands;
         OPUS_CLEAR((char*)&st->DECODER_RESET_START,
               opus_custom_decoder_get_size(st->mode, st->channels)-
               ((char*)&st->DECODER_RESET_START - (char*)st));
         for (i=0;i<2*st->mode->nbEBands;i++)
            oldLogE[i]=oldLogE2[i]=-QCONST16(28.f,DB_SHIFT);
      }
      break;
      case OPUS_GET_PITCH_REQUEST:
      {
         opus_int32 *value = va_arg(ap, opus_int32*);
         if (value==NULL)
            goto bad_arg;
         *value = st->postfilter_period;
      }
      break;
      case CELT_GET_MODE_REQUEST:
      {
         const CELTMode ** value = va_arg(ap, const CELTMode**);
         if (value==0)
            goto bad_arg;
         *value=st->mode;
      }
      break;
      case CELT_SET_SIGNALLING_REQUEST:
      {
         opus_int32 value = va_arg(ap, opus_int32);
         st->signalling = value;
      }
      break;
      case OPUS_GET_FINAL_RANGE_REQUEST:
      {
         opus_uint32 * value = va_arg(ap, opus_uint32 *);
         if (value==0)
            goto bad_arg;
         *value=st->rng;
      }
      break;
      default:
         goto bad_request;
   }
   va_end(ap);
   return OPUS_OK;
bad_arg:
   va_end(ap);
   return OPUS_BAD_ARG;
bad_request:
      va_end(ap);
  return OPUS_UNIMPLEMENTED;
}



const char *opus_strerror(int error)
{
   static const char * const error_strings[8] = {
      "success",
      "invalid argument",
      "buffer too small",
      "internal error",
      "corrupted stream",
      "request not implemented",
      "invalid state",
      "memory allocation failed"
   };
   if (error > 0 || error < -7)
      return "unknown error";
   else
      return error_strings[-error];
}

const char *opus_get_version_string(void)
{
    return "libopus " PACKAGE_VERSION
#ifdef FIXED_POINT
          "-fixed"
#endif
#ifdef FUZZING
          "-fuzzing"
#endif
          ;
}
