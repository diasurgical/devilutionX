/***********************************************************************
Copyright (c) 2006-2011, Skype Limited. All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
- Neither the name of Internet Society, IETF or IETF Trust, nor the 
names of specific contributors, may be used to endorse or promote
products derived from this software without specific prior written
permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SigProc_FIX.h"

/* Compute autocorrelation */
void silk_autocorr(
    opus_int32                  *results,           /* O    Result (length correlationCount)                            */
    opus_int                    *scale,             /* O    Scaling of the correlation vector                           */
    const opus_int16            *inputData,         /* I    Input data to correlate                                     */
    const opus_int              inputDataSize,      /* I    Length of input                                             */
    const opus_int              correlationCount    /* I    Number of correlation taps to compute                       */
)
{
    opus_int   i, lz, nRightShifts, corrCount;
    opus_int64 corr64;

    corrCount = silk_min_int( inputDataSize, correlationCount );

    /* compute energy (zero-lag correlation) */
    corr64 = silk_inner_prod16_aligned_64( inputData, inputData, inputDataSize );

    /* deal with all-zero input data */
    corr64 += 1;

    /* number of leading zeros */
    lz = silk_CLZ64( corr64 );

    /* scaling: number of right shifts applied to correlations */
    nRightShifts = 35 - lz;
    *scale = nRightShifts;

    if( nRightShifts <= 0 ) {
        results[ 0 ] = silk_LSHIFT( (opus_int32)silk_CHECK_FIT32( corr64 ), -nRightShifts );

        /* compute remaining correlations based on int32 inner product */
          for( i = 1; i < corrCount; i++ ) {
            results[ i ] = silk_LSHIFT( silk_inner_prod_aligned( inputData, inputData + i, inputDataSize - i ), -nRightShifts );
        }
    } else {
        results[ 0 ] = (opus_int32)silk_CHECK_FIT32( silk_RSHIFT64( corr64, nRightShifts ) );

        /* compute remaining correlations based on int64 inner product */
          for( i = 1; i < corrCount; i++ ) {
            results[ i ] =  (opus_int32)silk_CHECK_FIT32( silk_RSHIFT64( silk_inner_prod16_aligned_64( inputData, inputData + i, inputDataSize - i ), nRightShifts ) );
        }
    }
}
