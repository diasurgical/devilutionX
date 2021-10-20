/*****************************************************************************/
/* adpcm.h                                Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Header file for adpcm decompress functions                                */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 31.03.03  1.00  Lad  The first version of adpcm.h                         */
/*****************************************************************************/

#ifndef __ADPCM_H__
#define __ADPCM_H__

//-----------------------------------------------------------------------------
// Defines

#define MAX_ADPCM_CHANNEL_COUNT   2
#define INITIAL_ADPCM_STEP_INDEX  0x2C

//-----------------------------------------------------------------------------
// Public functions

int  CompressADPCM  (void * pvOutBuffer, int dwOutLength, void * pvInBuffer, int dwInLength, int nCmpType, int ChannelCount);
int  DecompressADPCM(void * pvOutBuffer, int dwOutLength, void * pvInBuffer, int dwInLength, int ChannelCount);

#endif // __ADPCM_H__
