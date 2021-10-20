/*****************************************************************************/
/* adpcm.cpp                              Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* This module contains implementation of adpcm decompression method used by */
/* Storm.dll to decompress WAVE files. Thanks to Tom Amigo for releasing     */
/* his sources.                                                              */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 11.03.03  1.00  Lad  Splitted from Pkware.cpp                             */
/* 20.05.03  2.00  Lad  Added compression                                    */
/* 19.11.03  2.01  Dan  Big endian handling                                  */
/* 10.01.13  3.00  Lad  Refactored, beautified, documented :-)               */
/*****************************************************************************/

#include <stddef.h>

#include "adpcm.h"

//-----------------------------------------------------------------------------
// Tables necessary dor decompression

static int NextStepTable[] =
{
    -1, 0, -1, 4, -1, 2, -1, 6,
    -1, 1, -1, 5, -1, 3, -1, 7,
    -1, 1, -1, 5, -1, 3, -1, 7,
    -1, 2, -1, 4, -1, 6, -1, 8
};

static int StepSizeTable[] =
{
        7,     8,     9,    10,     11,    12,    13,    14,
       16,    17,    19,    21,     23,    25,    28,    31,
       34,    37,    41,    45,     50,    55,    60,    66,
       73,    80,    88,    97,    107,   118,   130,   143,
      157,   173,   190,   209,    230,   253,   279,   307,
      337,   371,   408,   449,    494,   544,   598,   658,
      724,   796,   876,   963,   1060,  1166,  1282,  1411,
     1552,  1707,  1878,  2066,   2272,  2499,  2749,  3024,
     3327,  3660,  4026,  4428,   4871,  5358,  5894,  6484,
     7132,  7845,  8630,  9493,  10442, 11487, 12635, 13899,
     15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
     32767
};

//-----------------------------------------------------------------------------
// Helper class for writing output ADPCM data

class TADPCMStream
{
    public:

    TADPCMStream(void * pvBuffer, size_t cbBuffer)
    {
        pbBufferEnd = (unsigned char *)pvBuffer + cbBuffer;
        pbBuffer = (unsigned char *)pvBuffer;
    }

    bool ReadByteSample(unsigned char & ByteSample)
    {
        // Check if there is enough space in the buffer
        if(pbBuffer >= pbBufferEnd)
            return false;

        ByteSample = *pbBuffer++;
        return true;
    }

    bool WriteByteSample(unsigned char ByteSample)
    {
        // Check if there is enough space in the buffer
        if(pbBuffer >= pbBufferEnd)
            return false;

        *pbBuffer++ = ByteSample;
        return true;
    }

    bool ReadWordSample(short & OneSample)
    {
        // Check if we have enough space in the output buffer
        if((size_t)(pbBufferEnd - pbBuffer) < sizeof(short))
            return false;

        // Write the sample
        OneSample = pbBuffer[0] + (((short)pbBuffer[1]) << 0x08);
        pbBuffer += sizeof(short);
        return true;
    }

    bool WriteWordSample(short OneSample)
    {
        // Check if we have enough space in the output buffer
        if((size_t)(pbBufferEnd - pbBuffer) < sizeof(short))
            return false;

        // Write the sample
        *pbBuffer++ = (unsigned char)(OneSample & 0xFF);
        *pbBuffer++ = (unsigned char)(OneSample >> 0x08);
        return true;
    }

    int LengthProcessed(void * pvBuffer)
    {
        return pbBuffer - (unsigned char *)pvBuffer;
    }

    unsigned char * pbBufferEnd;
    unsigned char * pbBuffer;
};                    

//----------------------------------------------------------------------------
// Local functions

static inline short GetNextStepIndex(int StepIndex, unsigned int EncodedSample)
{
    // Get the next step index
    StepIndex = StepIndex + NextStepTable[EncodedSample & 0x1F];

    // Don't make the step index overflow
    if(StepIndex < 0)
        StepIndex = 0;
    else if(StepIndex > 88)
        StepIndex = 88;

    return (short)StepIndex;
}

static inline int UpdatePredictedSample(int PredictedSample, int EncodedSample, int Difference)
{
    // Is the sign bit set?
    if(EncodedSample & 0x40)
    {
        PredictedSample -= Difference;
        if(PredictedSample <= -32768)
            PredictedSample = -32768;
    }
    else
    {
        PredictedSample += Difference;
        if(PredictedSample >= 32767)
            PredictedSample = 32767;
    }

    return PredictedSample;
}

static inline int DecodeSample(int PredictedSample, int EncodedSample, int StepSize, int Difference)
{
    if(EncodedSample & 0x01)
        Difference += (StepSize >> 0);

    if(EncodedSample & 0x02)
        Difference += (StepSize >> 1);

    if(EncodedSample & 0x04)
        Difference += (StepSize >> 2);

    if(EncodedSample & 0x08)
        Difference += (StepSize >> 3);

    if(EncodedSample & 0x10)
        Difference += (StepSize >> 4);

    if(EncodedSample & 0x20)
        Difference += (StepSize >> 5);

    return UpdatePredictedSample(PredictedSample, EncodedSample, Difference);
}

//----------------------------------------------------------------------------
// Compression routine

int CompressADPCM(void * pvOutBuffer, int cbOutBuffer, void * pvInBuffer, int cbInBuffer, int ChannelCount, int CompressionLevel)
{
    TADPCMStream os(pvOutBuffer, cbOutBuffer);      // The output stream
    TADPCMStream is(pvInBuffer, cbInBuffer);        // The input stream
    unsigned char BitShift = (unsigned char)(CompressionLevel - 1);
    short PredictedSamples[MAX_ADPCM_CHANNEL_COUNT];// Predicted samples for each channel
    short StepIndexes[MAX_ADPCM_CHANNEL_COUNT];     // Step indexes for each channel
    short InputSample;                              // Input sample for the current channel
    int TotalStepSize;
    int ChannelIndex;
    int AbsDifference;
    int Difference;
    int MaxBitMask;
    int StepSize;

//  _tprintf(_T("== CMPR Started ==============\n"));

    // First byte in the output stream contains zero. The second one contains the compression level
    os.WriteByteSample(0);
    if(!os.WriteByteSample(BitShift))
        return 2;

    // Set the initial step index for each channel
    PredictedSamples[0] = PredictedSamples[1] = 0;
    StepIndexes[0] = StepIndexes[1] = INITIAL_ADPCM_STEP_INDEX;

    // Next, InitialSample value for each channel follows
    for(int i = 0; i < ChannelCount; i++)
    {
        // Get the initial sample from the input stream
        if(!is.ReadWordSample(InputSample))
            return os.LengthProcessed(pvOutBuffer);

        // Store the initial sample to our sample array
        PredictedSamples[i] = InputSample;

        // Also store the loaded sample to the output stream
        if(!os.WriteWordSample(InputSample))
            return os.LengthProcessed(pvOutBuffer);
    }

    // Get the initial index
    ChannelIndex = ChannelCount - 1;
    
    // Now keep reading the input data as long as there is something in the input buffer
    while(is.ReadWordSample(InputSample))
    {
        int EncodedSample = 0;

        // If we have two channels, we need to flip the channel index
        ChannelIndex = (ChannelIndex + 1) % ChannelCount;

        // Get the difference from the previous sample.
        // If the difference is negative, set the sign bit to the encoded sample
        AbsDifference = InputSample - PredictedSamples[ChannelIndex];
        if(AbsDifference < 0)
        {
            AbsDifference = -AbsDifference;
            EncodedSample |= 0x40;
        }

        // If the difference is too low (higher that difference treshold),
        // write a step index modifier marker
        StepSize = StepSizeTable[StepIndexes[ChannelIndex]];
        if(AbsDifference < (StepSize >> CompressionLevel))
        {
            if(StepIndexes[ChannelIndex] != 0)
                StepIndexes[ChannelIndex]--;
            
            os.WriteByteSample(0x80);
        }
        else
        {
            // If the difference is too high, write marker that
            // indicates increase in step size
            while(AbsDifference > (StepSize << 1))
            {
                if(StepIndexes[ChannelIndex] >= 0x58)
                    break;

                // Modify the step index
                StepIndexes[ChannelIndex] += 8;
                if(StepIndexes[ChannelIndex] > 0x58)
                    StepIndexes[ChannelIndex] = 0x58;

                // Write the "modify step index" marker
                StepSize = StepSizeTable[StepIndexes[ChannelIndex]];
                os.WriteByteSample(0x81);
            }

            // Get the limit bit value
            MaxBitMask = (1 << (BitShift - 1));
            MaxBitMask = (MaxBitMask > 0x20) ? 0x20 : MaxBitMask;
            Difference = StepSize >> BitShift;
            TotalStepSize = 0;

            for(int BitVal = 0x01; BitVal <= MaxBitMask; BitVal <<= 1)
            {
                if((TotalStepSize + StepSize) <= AbsDifference)
                {
                    TotalStepSize += StepSize;
                    EncodedSample |= BitVal;
                }
                StepSize >>= 1;
            }

            PredictedSamples[ChannelIndex] = (short)UpdatePredictedSample(PredictedSamples[ChannelIndex],
                                                                          EncodedSample,
                                                                          Difference + TotalStepSize);
            // Write the encoded sample to the output stream
            if(!os.WriteByteSample((unsigned char)EncodedSample))
                break;
            
            // Calculates the step index to use for the next encode
            StepIndexes[ChannelIndex] = GetNextStepIndex(StepIndexes[ChannelIndex], EncodedSample);
        }
    }

//  _tprintf(_T("== CMPR Ended ================\n"));
    return os.LengthProcessed(pvOutBuffer);
}

//----------------------------------------------------------------------------
// Decompression routine

int DecompressADPCM(void * pvOutBuffer, int cbOutBuffer, void * pvInBuffer, int cbInBuffer, int ChannelCount)
{
    TADPCMStream os(pvOutBuffer, cbOutBuffer);          // Output stream
    TADPCMStream is(pvInBuffer, cbInBuffer);            // Input stream
    unsigned char EncodedSample;
    unsigned char BitShift;
    short PredictedSamples[MAX_ADPCM_CHANNEL_COUNT];    // Predicted sample for each channel
    short StepIndexes[MAX_ADPCM_CHANNEL_COUNT];         // Predicted step index for each channel
    int ChannelIndex;                                   // Current channel index

    // Initialize the StepIndex for each channel
    PredictedSamples[0] = PredictedSamples[1] = 0;
    StepIndexes[0] = StepIndexes[1] = INITIAL_ADPCM_STEP_INDEX;

//  _tprintf(_T("== DCMP Started ==============\n"));

    // The first byte is always zero, the second one contains bit shift (compression level - 1)
    is.ReadByteSample(BitShift);
    is.ReadByteSample(BitShift);
//  _tprintf(_T("DCMP: BitShift = %u\n"), (unsigned int)(unsigned char)BitShift);

    // Next, InitialSample value for each channel follows
    for(int i = 0; i < ChannelCount; i++)
    {
        // Get the initial sample from the input stream
        short InitialSample;

        // Attempt to read the initial sample
        if(!is.ReadWordSample(InitialSample))
            return os.LengthProcessed(pvOutBuffer);

//      _tprintf(_T("DCMP: Loaded InitialSample[%u]: %04X\n"), i, (unsigned int)(unsigned short)InitialSample);

        // Store the initial sample to our sample array
        PredictedSamples[i] = InitialSample;

        // Also store the loaded sample to the output stream
        if(!os.WriteWordSample(InitialSample))
            return os.LengthProcessed(pvOutBuffer);
    }

    // Get the initial index
    ChannelIndex = ChannelCount - 1;

    // Keep reading as long as there is something in the input buffer
    while(is.ReadByteSample(EncodedSample))
    {
//      _tprintf(_T("DCMP: Loaded Encoded Sample: %02X\n"), (unsigned int)(unsigned char)EncodedSample);

        // If we have two channels, we need to flip the channel index
        ChannelIndex = (ChannelIndex + 1) % ChannelCount;

        if(EncodedSample == 0x80)
        {
            if(StepIndexes[ChannelIndex] != 0)
                StepIndexes[ChannelIndex]--;

//          _tprintf(_T("DCMP: Writing Decoded Sample: %04lX\n"), (unsigned int)(unsigned short)PredictedSamples[ChannelIndex]);
            if(!os.WriteWordSample(PredictedSamples[ChannelIndex]))
                return os.LengthProcessed(pvOutBuffer);
        }
        else if(EncodedSample == 0x81)
        {
            // Modify the step index
            StepIndexes[ChannelIndex] += 8;
            if(StepIndexes[ChannelIndex] > 0x58)
                StepIndexes[ChannelIndex] = 0x58;

//          _tprintf(_T("DCMP: New value of StepIndex: %04lX\n"), (unsigned int)(unsigned short)StepIndexes[ChannelIndex]);

            // Next pass, keep going on the same channel
            ChannelIndex = (ChannelIndex + 1) % ChannelCount;
        }
        else
        {
            int StepIndex = StepIndexes[ChannelIndex];
            int StepSize = StepSizeTable[StepIndex];

            // Encode one sample
            PredictedSamples[ChannelIndex] = (short)DecodeSample(PredictedSamples[ChannelIndex],
                                                                 EncodedSample, 
                                                                 StepSize,
                                                                 StepSize >> BitShift);

//          _tprintf(_T("DCMP: Writing decoded sample: %04X\n"), (unsigned int)(unsigned short)PredictedSamples[ChannelIndex]);

            // Write the decoded sample to the output stream
            if(!os.WriteWordSample(PredictedSamples[ChannelIndex]))
                break;

            // Calculates the step index to use for the next encode
            StepIndexes[ChannelIndex] = GetNextStepIndex(StepIndex, EncodedSample);
//          _tprintf(_T("DCMP: New step index: %04X\n"), (unsigned int)(unsigned short)StepIndexes[ChannelIndex]);
        }
    }

//  _tprintf(_T("DCMP: Total length written: %u\n"), (unsigned int)os.LengthProcessed(pvOutBuffer));
//  _tprintf(_T("== DCMP Ended ================\n"));

    // Return total bytes written since beginning of the output buffer
    return os.LengthProcessed(pvOutBuffer);
}
