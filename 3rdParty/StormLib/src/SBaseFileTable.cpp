/*****************************************************************************/
/* SBaseFileTable.cpp                     Copyright (c) Ladislav Zezula 2010 */
/*---------------------------------------------------------------------------*/
/* Description: Common handler for classic and new hash&block tables         */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 06.09.10  1.00  Lad  The first version of SBaseFileTable.cpp              */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "StormCommon.h"

//-----------------------------------------------------------------------------
// Local defines

#define INVALID_FLAG_VALUE 0xCCCCCCCC
#define MAX_FLAG_INDEX     512

//-----------------------------------------------------------------------------
// Support for calculating bit sizes

static void InitFileFlagArray(LPDWORD FlagArray)
{
    memset(FlagArray, 0xCC, MAX_FLAG_INDEX * sizeof(DWORD));
}

static DWORD GetFileFlagIndex(LPDWORD FlagArray, DWORD dwFlags)
{
    // Find free or equal entry in the flag array
    for(DWORD dwFlagIndex = 0; dwFlagIndex < MAX_FLAG_INDEX; dwFlagIndex++)
    {
        if(FlagArray[dwFlagIndex] == INVALID_FLAG_VALUE || FlagArray[dwFlagIndex] == dwFlags)
        {
            FlagArray[dwFlagIndex] = dwFlags;
            return dwFlagIndex;
        }
    }

    // This should never happen
    assert(false);
    return 0xFFFFFFFF;
}

static DWORD GetNecessaryBitCount(ULONGLONG MaxValue)
{
    DWORD dwBitCount = 0;

    while(MaxValue > 0)
    {
        MaxValue >>= 1;
        dwBitCount++;
    }

    return dwBitCount;
}

//-----------------------------------------------------------------------------
// Implementation of the TMPQBits struct

struct TMPQBits
{
    static TMPQBits * Create(DWORD NumberOfBits, BYTE FillValue);

    void GetBits(unsigned int nBitPosition, unsigned int nBitLength, void * pvBuffer, int nResultSize);
    void SetBits(unsigned int nBitPosition, unsigned int nBitLength, void * pvBuffer, int nResultSize);

    static const USHORT SetBitsMask[];          // Bit mask for each number of bits (0-8)

    DWORD NumberOfBytes;                        // Total number of bytes in "Elements"
    DWORD NumberOfBits;                         // Total number of bits that are available
    BYTE Elements[1];                           // Array of elements (variable length)
};

const USHORT TMPQBits::SetBitsMask[] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

TMPQBits * TMPQBits::Create(
    DWORD NumberOfBits,
    BYTE FillValue)
{
    TMPQBits * pBitArray;
    size_t nSize = sizeof(TMPQBits) + (NumberOfBits + 7) / 8;

    // Allocate the bit array
    pBitArray = (TMPQBits *)STORM_ALLOC(BYTE, nSize);
    if(pBitArray != NULL)
    {
        memset(pBitArray, FillValue, nSize);
        pBitArray->NumberOfBytes = (NumberOfBits + 7) / 8;
        pBitArray->NumberOfBits = NumberOfBits;
    }

    return pBitArray;
}

void TMPQBits::GetBits(
    unsigned int nBitPosition,
    unsigned int nBitLength,
    void * pvBuffer,
    int nResultByteSize)
{
    unsigned char * pbBuffer = (unsigned char *)pvBuffer;
    unsigned int nBytePosition0 = (nBitPosition / 8);
    unsigned int nBytePosition1 = nBytePosition0 + 1;
    unsigned int nByteLength = (nBitLength / 8);
    unsigned int nBitOffset = (nBitPosition & 0x07);
    unsigned char BitBuffer;

    // Keep compiler happy for platforms where nResultByteSize is not used
    nResultByteSize = nResultByteSize;

#ifdef _DEBUG
    // Check if the target is properly zeroed
    for(int i = 0; i < nResultByteSize; i++)
        assert(pbBuffer[i] == 0);
#endif

#ifndef STORMLIB_LITTLE_ENDIAN
    // Adjust the buffer pointer for big endian platforms
    pbBuffer += (nResultByteSize - 1);
#endif

    // Copy whole bytes, if any
    while(nByteLength > 0)
    {
        // Is the current position in the Elements byte-aligned?
        if(nBitOffset != 0)
        {
            BitBuffer = (unsigned char)((Elements[nBytePosition0] >> nBitOffset) | (Elements[nBytePosition1] << (0x08 - nBitOffset)));
        }
        else
        {
            BitBuffer = Elements[nBytePosition0];
        }

#ifdef STORMLIB_LITTLE_ENDIAN
        *pbBuffer++ = BitBuffer;
#else
        *pbBuffer-- = BitBuffer;
#endif

        // Move byte positions and lengths
        nBytePosition1++;
        nBytePosition0++;
        nByteLength--;
    }

    // Get the rest of the bits
    nBitLength = (nBitLength & 0x07);
    if(nBitLength != 0)
    {
        *pbBuffer = (unsigned char)(Elements[nBytePosition0] >> nBitOffset);

        if(nBitLength > (8 - nBitOffset))
            *pbBuffer = (unsigned char)((Elements[nBytePosition1] << (8 - nBitOffset)) | (Elements[nBytePosition0] >> nBitOffset));

        *pbBuffer &= (0x01 << nBitLength) - 1;
    }
}

void TMPQBits::SetBits(
    unsigned int nBitPosition,
    unsigned int nBitLength,
    void * pvBuffer,
    int nResultByteSize)
{
    unsigned char * pbBuffer = (unsigned char *)pvBuffer;
    unsigned int nBytePosition = (nBitPosition / 8);
    unsigned int nBitOffset = (nBitPosition & 0x07);
    unsigned short BitBuffer = 0;
    unsigned short AndMask = 0;
    unsigned short OneByte = 0;

    // Keep compiler happy for platforms where nResultByteSize is not used
    nResultByteSize = nResultByteSize;

#ifndef STORMLIB_LITTLE_ENDIAN
    // Adjust the buffer pointer for big endian platforms
    pbBuffer += (nResultByteSize - 1);
#endif

    // Copy whole bytes, if any
    while(nBitLength > 8)
    {
        // Reload the bit buffer
#ifdef STORMLIB_LITTLE_ENDIAN
        OneByte = *pbBuffer++;
#else
        OneByte = *pbBuffer--;
#endif
        // Update the BitBuffer and AndMask for the bit array
        BitBuffer = (BitBuffer >> 0x08) | (OneByte << nBitOffset);
        AndMask = (AndMask >> 0x08) | (0x00FF << nBitOffset);

        // Update the byte in the array
        Elements[nBytePosition] = (BYTE)((Elements[nBytePosition] & ~AndMask) | BitBuffer);

        // Move byte positions and lengths
        nBytePosition++;
        nBitLength -= 0x08;
    }

    if(nBitLength != 0)
    {
        // Reload the bit buffer
        OneByte = *pbBuffer;

        // Update the AND mask for the last bit
        BitBuffer = (BitBuffer >> 0x08) | (OneByte << nBitOffset);
        AndMask = (AndMask >> 0x08) | (SetBitsMask[nBitLength] << nBitOffset);

        // Update the byte in the array
        Elements[nBytePosition] = (BYTE)((Elements[nBytePosition] & ~AndMask) | BitBuffer);

        // Update the next byte, if needed
        if(AndMask & 0xFF00)
        {
            nBytePosition++;
            BitBuffer >>= 0x08;
            AndMask >>= 0x08;

            Elements[nBytePosition] = (BYTE)((Elements[nBytePosition] & ~AndMask) | BitBuffer);
        }
    }
}

void GetMPQBits(TMPQBits * pBits, unsigned int nBitPosition, unsigned int nBitLength, void * pvBuffer, int nResultByteSize)
{
    pBits->GetBits(nBitPosition, nBitLength, pvBuffer, nResultByteSize);
}

//-----------------------------------------------------------------------------
// Support for MPQ header

static bool VerifyTablePosition64(
    ULONGLONG MpqOffset,                // Position of the MPQ header
    ULONGLONG TableOffset,              // Position of the MPQ table, relative to MPQ header
    ULONGLONG TableSize,                // Size of the MPQ table, in bytes
    ULONGLONG FileSize)                 // Size of the entire file, in bytes
{
    if(TableOffset != 0)
    {
        // Verify overflows
        if((MpqOffset + TableOffset) < MpqOffset)
            return false;
        if((MpqOffset + TableOffset + TableSize) < MpqOffset)
            return false;

        // Verify sizes
        if(TableOffset >= FileSize || TableSize >= FileSize)
            return false;
        if((MpqOffset + TableOffset) >= FileSize)
            return false;
        if((MpqOffset + TableOffset + TableSize) >= FileSize)
            return false;
    }
    return true;
}

static bool VerifyTableTandemPositions(
    ULONGLONG MpqOffset,                // Position of the MPQ header
    ULONGLONG TableOffset1,             // 1st table: Position, relative to MPQ header
    ULONGLONG TableSize1,               // 1st table: Size in bytes
    ULONGLONG TableOffset2,             // 2nd table: Position, relative to MPQ header
    ULONGLONG TableSize2,               // 2nd table: Size in bytes
    ULONGLONG FileSize)                 // Size of the entire file, in bytes
{
    return VerifyTablePosition64(MpqOffset, TableOffset1, TableSize1, FileSize) &&
           VerifyTablePosition64(MpqOffset, TableOffset2, TableSize2, FileSize);
}

static ULONGLONG DetermineArchiveSize_V1(
    TMPQArchive * ha,
    TMPQHeader * pHeader,
    ULONGLONG MpqOffset,
    ULONGLONG FileSize)
{
    ULONGLONG ByteOffset;
    ULONGLONG EndOfMpq = FileSize;
    DWORD SignatureHeader = 0;
    DWORD dwArchiveSize32;

    // This could only be called for MPQs version 1.0
    assert(pHeader->wFormatVersion == MPQ_FORMAT_VERSION_1);

    // Check if we can rely on the archive size in the header
    if(pHeader->dwBlockTablePos < pHeader->dwArchiveSize)
    {
        // The block table cannot be compressed, so the sizes must match
        if((pHeader->dwArchiveSize - pHeader->dwBlockTablePos) == (pHeader->dwBlockTableSize * sizeof(TMPQBlock)))
            return pHeader->dwArchiveSize;

        // If the archive size in the header is less than real file size
        dwArchiveSize32 = (DWORD)(FileSize - MpqOffset);
        if(pHeader->dwArchiveSize == dwArchiveSize32)
            return pHeader->dwArchiveSize;
    }

    // Check if there is a signature header
    if((EndOfMpq - MpqOffset) > (MPQ_STRONG_SIGNATURE_SIZE + 4))
    {
        ByteOffset = EndOfMpq - MPQ_STRONG_SIGNATURE_SIZE - 4;
        if(FileStream_Read(ha->pStream, &ByteOffset, &SignatureHeader, sizeof(DWORD)))
        {
            if(BSWAP_INT32_UNSIGNED(SignatureHeader) == MPQ_STRONG_SIGNATURE_ID)
                EndOfMpq = EndOfMpq - MPQ_STRONG_SIGNATURE_SIZE - 4;
        }
    }

    // Return the returned archive size
    return (EndOfMpq - MpqOffset);
}

static ULONGLONG DetermineArchiveSize_V2(
    TMPQHeader * pHeader,
    ULONGLONG MpqOffset,
    ULONGLONG FileSize)
{
    ULONGLONG EndOfMpq = FileSize;
    DWORD dwArchiveSize32;

    // This could only be called for MPQs version 2.0
    assert(pHeader->wFormatVersion == MPQ_FORMAT_VERSION_2);

    // Check if we can rely on the archive size in the header
    if((FileSize >> 0x20) == 0)
    {
        if(pHeader->dwBlockTablePos < pHeader->dwArchiveSize)
        {
            if((pHeader->dwArchiveSize - pHeader->dwBlockTablePos) <= (pHeader->dwBlockTableSize * sizeof(TMPQBlock)))
                return pHeader->dwArchiveSize;

            // If the archive size in the header is less than real file size
            dwArchiveSize32 = (DWORD)(FileSize - MpqOffset);
            if(pHeader->dwArchiveSize <= dwArchiveSize32)
                return pHeader->dwArchiveSize;
        }
    }

    // Return the calculated archive size
    return (EndOfMpq - MpqOffset);
}

static ULONGLONG DetermineArchiveSize_V4(
    TMPQHeader * pHeader,
    ULONGLONG /* MpqOffset */,
    ULONGLONG /* FileSize */)
{
    ULONGLONG ArchiveSize = 0;
    ULONGLONG EndOfTable;

    // This could only be called for MPQs version 4
    assert(pHeader->wFormatVersion == MPQ_FORMAT_VERSION_4);

    // Check position of BET table, if correct
    if((pHeader->BetTablePos64 >> 0x20) == 0 && (pHeader->BetTableSize64 >> 0x20) == 0)
    {
        EndOfTable = pHeader->BetTablePos64 + pHeader->BetTableSize64;
        if(EndOfTable > ArchiveSize)
            ArchiveSize = EndOfTable;
    }

    // Check position of HET table, if correct
    if((pHeader->HetTablePos64 >> 0x20) == 0 && (pHeader->HetTableSize64 >> 0x20) == 0)
    {
        EndOfTable = pHeader->HetTablePos64 + pHeader->HetTableSize64;
        if(EndOfTable > ArchiveSize)
            ArchiveSize = EndOfTable;
    }

    EndOfTable = pHeader->dwHashTablePos + pHeader->dwHashTableSize * sizeof(TMPQHash);
    if(EndOfTable > ArchiveSize)
        ArchiveSize = EndOfTable;

    EndOfTable = pHeader->dwBlockTablePos + pHeader->dwBlockTableSize * sizeof(TMPQBlock);
    if(EndOfTable > ArchiveSize)
        ArchiveSize = EndOfTable;

    // Return the calculated archive size
    return ArchiveSize;
}

ULONGLONG FileOffsetFromMpqOffset(TMPQArchive * ha, ULONGLONG MpqOffset)
{
    if(ha->pHeader->wFormatVersion == MPQ_FORMAT_VERSION_1)
    {
        // For MPQ archive v1, any file offset is only 32-bit
        return (ULONGLONG)((DWORD)ha->MpqPos + (DWORD)MpqOffset);
    }
    else
    {
        // For MPQ archive v2+, file offsets are full 64-bit
        return ha->MpqPos + MpqOffset;
    }
}

ULONGLONG CalculateRawSectorOffset(
    TMPQFile * hf,
    DWORD dwSectorOffset)
{
    ULONGLONG RawFilePos;

    // Must be used for files within a MPQ
    assert(hf->ha != NULL);
    assert(hf->ha->pHeader != NULL);

    //
    // Some MPQ protectors place the sector offset table after the actual file data.
    // Sector offsets in the sector offset table are negative. When added
    // to MPQ file offset from the block table entry, the result is a correct
    // position of the file data in the MPQ.
    //
    // For MPQs version 1.0, the offset is purely 32-bit
    //

    RawFilePos = hf->RawFilePos + dwSectorOffset;
    if(hf->ha->pHeader->wFormatVersion == MPQ_FORMAT_VERSION_1)
        RawFilePos = (DWORD)hf->ha->MpqPos + (DWORD)hf->pFileEntry->ByteOffset + dwSectorOffset;

    // We also have to add patch header size, if patch header is present
    if(hf->pPatchInfo != NULL)
        RawFilePos += hf->pPatchInfo->dwLength;

    // Return the result offset
    return RawFilePos;
}

// This function converts the MPQ header so it always looks like version 4
int ConvertMpqHeaderToFormat4(
    TMPQArchive * ha,
    ULONGLONG ByteOffset,
    ULONGLONG FileSize,
    DWORD dwFlags,
    MTYPE MapType)
{
    TMPQHeader * pHeader = (TMPQHeader *)ha->HeaderData;
    ULONGLONG BlockTablePos64 = 0;
    ULONGLONG HashTablePos64 = 0;
    ULONGLONG BlockTableMask = (ULONGLONG)-1;
    ULONGLONG MaxOffset;
    USHORT wFormatVersion = BSWAP_INT16_UNSIGNED(pHeader->wFormatVersion);
    bool bHashBlockOffsetOK = false;
    bool bHetBetOffsetOK = false;
    int nError = ERROR_SUCCESS;

    // If version 1.0 is forced, then the format version is forced to be 1.0
    // Reason: Storm.dll in Warcraft III ignores format version value
    if((MapType == MapTypeWarcraft3) || (dwFlags & MPQ_OPEN_FORCE_MPQ_V1))
        wFormatVersion = MPQ_FORMAT_VERSION_1;

    // Don't accept format 3 for Starcraft II maps
    if((MapType == MapTypeStarcraft2) && (pHeader->wFormatVersion > MPQ_FORMAT_VERSION_2))
        wFormatVersion = MPQ_FORMAT_VERSION_4;

    // Format-specific fixes
    switch(wFormatVersion)
    {
        case MPQ_FORMAT_VERSION_1:

            // Check for malformed MPQ header version 1.0
            BSWAP_TMPQHEADER(pHeader, MPQ_FORMAT_VERSION_1);
            if(pHeader->wFormatVersion != MPQ_FORMAT_VERSION_1 || pHeader->dwHeaderSize != MPQ_HEADER_SIZE_V1)
            {
                pHeader->wFormatVersion = MPQ_FORMAT_VERSION_1;
                pHeader->dwHeaderSize = MPQ_HEADER_SIZE_V1;
                ha->dwFlags |= MPQ_FLAG_MALFORMED;
            }

            //
            // Note: The value of "dwArchiveSize" member in the MPQ header
            // is ignored by Storm.dll and can contain garbage value
            // ("w3xmaster" protector).
            //

            Label_ArchiveVersion1:
            if(pHeader->dwBlockTableSize > 1)  // Prevent empty MPQs being marked as malformed
            {
                if(pHeader->dwHashTablePos <= pHeader->dwHeaderSize || (pHeader->dwHashTablePos & 0x80000000))
                    ha->dwFlags |= MPQ_FLAG_MALFORMED;
                if(pHeader->dwBlockTablePos <= pHeader->dwHeaderSize || (pHeader->dwBlockTablePos & 0x80000000))
                    ha->dwFlags |= MPQ_FLAG_MALFORMED;
            }

            // Only low byte of sector size is really used
            if(pHeader->wSectorSize & 0xFF00)
                ha->dwFlags |= MPQ_FLAG_MALFORMED;
            pHeader->wSectorSize = pHeader->wSectorSize & 0xFF;

            // Fill the rest of the header
            memset((LPBYTE)pHeader + MPQ_HEADER_SIZE_V1, 0, sizeof(TMPQHeader) - MPQ_HEADER_SIZE_V1);
            pHeader->BlockTableSize64 = pHeader->dwBlockTableSize * sizeof(TMPQBlock);
            pHeader->HashTableSize64 = pHeader->dwHashTableSize * sizeof(TMPQHash);
            pHeader->ArchiveSize64 = pHeader->dwArchiveSize;

            // Block table position must be calculated as 32-bit value
            // Note: BOBA protector puts block table before the MPQ header, so it is negative
            BlockTablePos64 = (ULONGLONG)((DWORD)ByteOffset + pHeader->dwBlockTablePos);
            BlockTableMask = 0xFFFFFFF0;

            // Determine the archive size on malformed MPQs
            if(ha->dwFlags & MPQ_FLAG_MALFORMED)
            {
                // Calculate the archive size
                pHeader->ArchiveSize64 = DetermineArchiveSize_V1(ha, pHeader, ByteOffset, FileSize);
                pHeader->dwArchiveSize = (DWORD)pHeader->ArchiveSize64;
            }

            // EWIX_v8_7.w3x: TMPQHeader::dwBlockTableSize = 0x00319601
            // Size of TFileTable goes to ~200MB, so we artificially cut it
            if(BlockTablePos64 + (pHeader->dwBlockTableSize * sizeof(TMPQBlock)) > FileSize)
            {
                pHeader->dwBlockTableSize = (DWORD)((FileSize - BlockTablePos64) / sizeof(TMPQBlock));
                pHeader->BlockTableSize64 = pHeader->dwBlockTableSize * sizeof(TMPQBlock);
            }
            break;

        case MPQ_FORMAT_VERSION_2:

            // Check for malformed MPQ header version 1.0
            BSWAP_TMPQHEADER(pHeader, MPQ_FORMAT_VERSION_2);
            if(pHeader->wFormatVersion != MPQ_FORMAT_VERSION_2 || pHeader->dwHeaderSize != MPQ_HEADER_SIZE_V2)
            {
                pHeader->wFormatVersion = MPQ_FORMAT_VERSION_1;
                pHeader->dwHeaderSize = MPQ_HEADER_SIZE_V1;
                ha->dwFlags |= MPQ_FLAG_MALFORMED;
                goto Label_ArchiveVersion1;
            }

            // Fill the rest of the header with zeros
            memset((LPBYTE)pHeader + MPQ_HEADER_SIZE_V2, 0, sizeof(TMPQHeader) - MPQ_HEADER_SIZE_V2);

            // Calculate the expected hash table size
            pHeader->HashTableSize64 = (pHeader->dwHashTableSize * sizeof(TMPQHash));
            HashTablePos64 = MAKE_OFFSET64(pHeader->wHashTablePosHi, pHeader->dwHashTablePos);

            // Calculate the expected block table size
            pHeader->BlockTableSize64 = (pHeader->dwBlockTableSize * sizeof(TMPQBlock));
            BlockTablePos64 = MAKE_OFFSET64(pHeader->wBlockTablePosHi, pHeader->dwBlockTablePos);

            // We require the block table to follow hash table
            if(BlockTablePos64 >= HashTablePos64)
            {
                // HashTableSize64 may be less than TblSize * sizeof(TMPQHash).
                // That means that the hash table is compressed.
                pHeader->HashTableSize64 = BlockTablePos64 - HashTablePos64;

                // Calculate the compressed block table size
                if(pHeader->HiBlockTablePos64 != 0)
                {
                    // BlockTableSize64 may be less than TblSize * sizeof(TMPQBlock).
                    // That means that the block table is compressed.
                    pHeader->BlockTableSize64 = pHeader->HiBlockTablePos64 - BlockTablePos64;
                    assert(pHeader->BlockTableSize64 <= (pHeader->dwBlockTableSize * sizeof(TMPQBlock)));

                    // Determine real archive size
                    pHeader->ArchiveSize64 = DetermineArchiveSize_V2(pHeader, ByteOffset, FileSize);

                    // Calculate the size of the hi-block table
                    pHeader->HiBlockTableSize64 = pHeader->ArchiveSize64 - pHeader->HiBlockTablePos64;
                    assert(pHeader->HiBlockTableSize64 == (pHeader->dwBlockTableSize * sizeof(USHORT)));
                }
                else
                {
                    // Determine real archive size
                    pHeader->ArchiveSize64 = DetermineArchiveSize_V2(pHeader, ByteOffset, FileSize);

                    // Calculate size of the block table
                    pHeader->BlockTableSize64 = pHeader->ArchiveSize64 - BlockTablePos64;
                    assert(pHeader->BlockTableSize64 <= (pHeader->dwBlockTableSize * sizeof(TMPQBlock)));
                }
            }
            else
            {
                pHeader->ArchiveSize64 = pHeader->dwArchiveSize;
                ha->dwFlags |= MPQ_FLAG_MALFORMED;
            }

            // Add the MPQ Offset
            BlockTablePos64 += ByteOffset;
            break;

        case MPQ_FORMAT_VERSION_3:

            // In MPQ format 3.0, the entire header is optional
            // and the size of the header can actually be identical
            // to size of header 2.0
            BSWAP_TMPQHEADER(pHeader, MPQ_FORMAT_VERSION_3);
            if(pHeader->dwHeaderSize < MPQ_HEADER_SIZE_V3)
            {
                pHeader->ArchiveSize64 = pHeader->dwArchiveSize;
                pHeader->HetTablePos64 = 0;
                pHeader->BetTablePos64 = 0;
            }

            //
            // We need to calculate the compressed size of each table. We assume the following order:
            // 1) HET table
            // 2) BET table
            // 3) Classic hash table
            // 4) Classic block table
            // 5) Hi-block table
            //

            // Fill the rest of the header with zeros
            memset((LPBYTE)pHeader + MPQ_HEADER_SIZE_V3, 0, sizeof(TMPQHeader) - MPQ_HEADER_SIZE_V3);
            BlockTablePos64 = MAKE_OFFSET64(pHeader->wBlockTablePosHi, pHeader->dwBlockTablePos);
            HashTablePos64 = MAKE_OFFSET64(pHeader->wHashTablePosHi, pHeader->dwHashTablePos);
            MaxOffset = pHeader->ArchiveSize64;

            // Size of the hi-block table
            if(pHeader->HiBlockTablePos64)
            {
                pHeader->HiBlockTableSize64 = MaxOffset - pHeader->HiBlockTablePos64;
                MaxOffset = pHeader->HiBlockTablePos64;
            }

            // Size of the block table
            if(BlockTablePos64)
            {
                pHeader->BlockTableSize64 = MaxOffset - BlockTablePos64;
                MaxOffset = BlockTablePos64;
            }

            // Size of the hash table
            if(HashTablePos64)
            {
                pHeader->HashTableSize64 = MaxOffset - HashTablePos64;
                MaxOffset = HashTablePos64;
            }

            // Size of the BET table
            if(pHeader->BetTablePos64)
            {
                pHeader->BetTableSize64 = MaxOffset - pHeader->BetTablePos64;
                MaxOffset = pHeader->BetTablePos64;
            }

            // Size of the HET table
            if(pHeader->HetTablePos64)
            {
                pHeader->HetTableSize64 = MaxOffset - pHeader->HetTablePos64;
//              MaxOffset = pHeader->HetTablePos64;
            }

            // Add the MPQ Offset
            BlockTablePos64 += ByteOffset;
            break;

        case MPQ_FORMAT_VERSION_4:

            // Verify header MD5. Header MD5 is calculated from the MPQ header since the 'MPQ\x1A'
            // signature until the position of header MD5 at offset 0xC0
            BSWAP_TMPQHEADER(pHeader, MPQ_FORMAT_VERSION_4);

            // Apparently, Starcraft II only accepts MPQ headers where the MPQ header hash matches
            // If MD5 doesn't match, we ignore this offset. We also ignore it if there's no MD5 at all
            if(!IsValidMD5(pHeader->MD5_MpqHeader))
                return ERROR_FAKE_MPQ_HEADER;
#ifdef FULL
            if(!VerifyDataBlockHash(pHeader, MPQ_HEADER_SIZE_V4 - MD5_DIGEST_SIZE, pHeader->MD5_MpqHeader))
                return ERROR_FAKE_MPQ_HEADER;
#endif // FULL

            // HiBlockTable must be 0 for archives under 4GB
            if((pHeader->ArchiveSize64 >> 0x20) == 0 && pHeader->HiBlockTablePos64 != 0)
                return ERROR_FAKE_MPQ_HEADER;

            // Is the "HET&BET" table tandem OK?
            bHetBetOffsetOK = VerifyTableTandemPositions(ByteOffset,
                                                         pHeader->HetTablePos64, pHeader->HetTableSize64,
                                                         pHeader->BetTablePos64, pHeader->BetTableSize64,
                                                         FileSize);

            // Is the "Hash&Block" table tandem OK?
            bHashBlockOffsetOK = VerifyTableTandemPositions(ByteOffset,
                                                            pHeader->dwHashTablePos, pHeader->HashTableSize64,
                                                            pHeader->dwBlockTablePos, pHeader->BlockTableSize64,
                                                            FileSize);

            // At least one pair must be OK
            if(bHetBetOffsetOK == false && bHashBlockOffsetOK == false)
                return ERROR_FAKE_MPQ_HEADER;

            // Check for malformed MPQs
            if(pHeader->wFormatVersion != MPQ_FORMAT_VERSION_4 || (ByteOffset + pHeader->ArchiveSize64) != FileSize || (ByteOffset + pHeader->HiBlockTablePos64) >= FileSize)
            {
                pHeader->wFormatVersion = MPQ_FORMAT_VERSION_4;
                pHeader->dwHeaderSize = MPQ_HEADER_SIZE_V4;
                ha->dwFlags |= MPQ_FLAG_MALFORMED;
            }

            // Recalculate archive size
            if(ha->dwFlags & MPQ_FLAG_MALFORMED)
            {
                // Calculate the archive size
                pHeader->ArchiveSize64 = DetermineArchiveSize_V4(pHeader, ByteOffset, FileSize);
                pHeader->dwArchiveSize = (DWORD)pHeader->ArchiveSize64;
            }

            // Calculate the block table position
            BlockTablePos64 = ByteOffset + MAKE_OFFSET64(pHeader->wBlockTablePosHi, pHeader->dwBlockTablePos);
            break;

        default:

            // Check if it's a War of the Immortal data file (SQP)
            // If not, we treat it as malformed MPQ version 1.0
            if(ConvertSqpHeaderToFormat4(ha, FileSize, dwFlags) != ERROR_SUCCESS)
            {
                pHeader->wFormatVersion = MPQ_FORMAT_VERSION_1;
                pHeader->dwHeaderSize = MPQ_HEADER_SIZE_V1;
                ha->dwFlags |= MPQ_FLAG_MALFORMED;
                goto Label_ArchiveVersion1;
            }

            // Calculate the block table position
            BlockTablePos64 = ByteOffset + MAKE_OFFSET64(pHeader->wBlockTablePosHi, pHeader->dwBlockTablePos);
            break;
    }

    // Handle case when block table is placed before the MPQ header
    // Used by BOBA protector
    if(BlockTablePos64 < ByteOffset)
        ha->dwFlags |= MPQ_FLAG_MALFORMED;
    return nError;
}

//-----------------------------------------------------------------------------
// Support for hash table

// Hash entry verification when the file table does not exist yet
bool IsValidHashEntry(TMPQArchive * ha, TMPQHash * pHash)
{
    TFileEntry * pFileEntry = ha->pFileTable + MPQ_BLOCK_INDEX(pHash);

    return ((MPQ_BLOCK_INDEX(pHash) < ha->dwFileTableSize) && (pFileEntry->dwFlags & MPQ_FILE_EXISTS)) ? true : false;
}

// Hash entry verification when the file table does not exist yet
static bool IsValidHashEntry1(TMPQArchive * ha, TMPQHash * pHash, TMPQBlock * pBlockTable)
{
    ULONGLONG ByteOffset;
    TMPQBlock * pBlock;

    // The block index is considered valid if it's less than block table size
    if(MPQ_BLOCK_INDEX(pHash) < ha->pHeader->dwBlockTableSize)
    {
        // Calculate the block table position
        pBlock = pBlockTable + MPQ_BLOCK_INDEX(pHash);

        // Check whether this is an existing file
        // Also we do not allow to be file size greater than 2GB
        if((pBlock->dwFlags & MPQ_FILE_EXISTS) && (pBlock->dwFSize & 0x80000000) == 0)
        {
            // The begin of the file must be within the archive
            ByteOffset = FileOffsetFromMpqOffset(ha, pBlock->dwFilePos);
            return (ByteOffset < ha->FileSize);
        }
    }

    return false;
}

// Returns a hash table entry in the following order:
// 1) A hash table entry with the preferred locale and platform
// 2) A hash table entry with the neutral|matching locale and neutral|matching platform
// 3) NULL
// Storm_2016.dll: 15020940
static TMPQHash * GetHashEntryLocale(TMPQArchive * ha, const char * szFileName, LCID lcLocale, BYTE Platform)
{
    TMPQHash * pFirstHash = GetFirstHashEntry(ha, szFileName);
    TMPQHash * pBestEntry = NULL;
    TMPQHash * pHash = pFirstHash;

    // Parse the found hashes
    while(pHash != NULL)
    {
        // Storm_2016.dll: 150209CB
        // If the hash entry matches both locale and platform, return it immediately
        // Note: We only succeed this check if the locale is non-neutral, because
        // some Warcraft III maps have several items with neutral locale&platform, which leads
        // to wrong item being returned
        if((lcLocale || Platform) && pHash->lcLocale == lcLocale && pHash->Platform == Platform)
            return pHash;

        // Storm_2016.dll: 150209D9
        // If (locale matches or is neutral) OR (platform matches or is neutral)
        // remember this as the best entry
        if(pHash->lcLocale == 0 || pHash->lcLocale == lcLocale)
        {
            if(pHash->Platform == 0 || pHash->Platform == Platform)
                pBestEntry = pHash;
        }

        // Get the next hash entry for that file
        pHash = GetNextHashEntry(ha, pFirstHash, pHash);
    }

    // At the end, return neutral hash (if found), otherwise NULL
    return pBestEntry;
}

// Returns a hash table entry in the following order:
// 1) A hash table entry with the preferred locale
// 2) NULL
static TMPQHash * GetHashEntryExact(TMPQArchive * ha, const char * szFileName, LCID lcLocale)
{
    TMPQHash * pFirstHash = GetFirstHashEntry(ha, szFileName);
    TMPQHash * pHash = pFirstHash;

    // Parse the found hashes
    while(pHash != NULL)
    {
        // If the locales match, return it
        if(pHash->lcLocale == lcLocale)
            return pHash;

        // Get the next hash entry for that file
        pHash = GetNextHashEntry(ha, pFirstHash, pHash);
    }

    // Not found
    return NULL;
}

// Defragment the file table so it does not contain any gaps
// Note: As long as all values of all TMPQHash::dwBlockIndex
// are not HASH_ENTRY_FREE, the startup search index does not matter.
// Hash table is circular, so as long as there is no terminator,
// all entries will be found.
static TMPQHash * DefragmentHashTable(
    TMPQArchive * ha,
    TMPQHash  * pHashTable,
    TMPQBlock * pBlockTable)
{
    TMPQHeader * pHeader = ha->pHeader;
    TMPQHash * pHashTableEnd = pHashTable + pHeader->dwHashTableSize;
    TMPQHash * pSource = pHashTable;
    TMPQHash * pTarget = pHashTable;
    DWORD dwFirstFreeEntry;
    DWORD dwNewTableSize;

    // Sanity checks
    assert(pHeader->wFormatVersion == MPQ_FORMAT_VERSION_1);
    assert(pHeader->HiBlockTablePos64 == 0);

    // Parse the hash table and move the entries to the begin of it
    for(pSource = pHashTable; pSource < pHashTableEnd; pSource++)
    {
        // Check whether this is a valid hash table entry
        if(IsValidHashEntry1(ha, pSource, pBlockTable))
        {
            // Copy the hash table entry back
            if(pSource > pTarget)
                pTarget[0] = pSource[0];

            // Move the target
            pTarget++;
        }
    }

    // Calculate how many entries in the hash table we really need
    dwFirstFreeEntry = (DWORD)(pTarget - pHashTable);
    dwNewTableSize = GetNearestPowerOfTwo(dwFirstFreeEntry);

    // Fill the rest with entries that look like deleted
    pHashTableEnd = pHashTable + dwNewTableSize;
    pSource = pHashTable + dwFirstFreeEntry;
    memset(pSource, 0xFF, (dwNewTableSize - dwFirstFreeEntry) * sizeof(TMPQHash));

    // Mark the block indexes as deleted
    for(; pSource < pHashTableEnd; pSource++)
        pSource->dwBlockIndex = HASH_ENTRY_DELETED;

    // Free some of the space occupied by the hash table
    if(dwNewTableSize < pHeader->dwHashTableSize)
    {
        pHashTable = STORM_REALLOC(TMPQHash, pHashTable, dwNewTableSize);
        ha->pHeader->BlockTableSize64 = dwNewTableSize * sizeof(TMPQHash);
        ha->pHeader->dwHashTableSize = dwNewTableSize;
    }

    return pHashTable;
}

static int BuildFileTableFromBlockTable(
    TMPQArchive * ha,
    TMPQBlock * pBlockTable)
{
    TFileEntry * pFileEntry;
    TMPQHeader * pHeader = ha->pHeader;
    TMPQBlock * pBlock;
    TMPQHash * pHashTableEnd;
    TMPQHash * pHash;
    LPDWORD DefragmentTable = NULL;
    DWORD dwItemCount = 0;
    DWORD dwFlagMask;

    // Sanity checks
    assert(ha->pFileTable != NULL);
    assert(ha->dwFileTableSize >= ha->dwMaxFileCount);

    // MPQs for Warcraft III doesn't know some flags, namely MPQ_FILE_SINGLE_UNIT and MPQ_FILE_PATCH_FILE
    dwFlagMask = (ha->dwFlags & MPQ_FLAG_WAR3_MAP) ? MPQ_FILE_VALID_FLAGS_W3X : MPQ_FILE_VALID_FLAGS;

    // Defragment the hash table, if needed
    if(ha->dwFlags & MPQ_FLAG_HASH_TABLE_CUT)
    {
        ha->pHashTable = DefragmentHashTable(ha, ha->pHashTable, pBlockTable);
        ha->dwMaxFileCount = pHeader->dwHashTableSize;
    }

    // If the hash table or block table is cut,
    // we will defragment the block table
    if(ha->dwFlags & (MPQ_FLAG_HASH_TABLE_CUT | MPQ_FLAG_BLOCK_TABLE_CUT))
    {
        // Sanity checks
        assert(pHeader->wFormatVersion == MPQ_FORMAT_VERSION_1);
        assert(pHeader->HiBlockTablePos64 == 0);

        // Allocate the translation table
        DefragmentTable = STORM_ALLOC(DWORD, pHeader->dwBlockTableSize);
        if(DefragmentTable == NULL)
            return ERROR_NOT_ENOUGH_MEMORY;

        // Fill the translation table
        memset(DefragmentTable, 0xFF, pHeader->dwBlockTableSize * sizeof(DWORD));
    }

    // Parse the entire hash table
    pHashTableEnd = ha->pHashTable + pHeader->dwHashTableSize;
    for(pHash = ha->pHashTable; pHash < pHashTableEnd; pHash++)
    {
        //
        // We need to properly handle these cases:
        // - Multiple hash entries (same file name) point to the same block entry
        // - Multiple hash entries (different file name) point to the same block entry
        //
        // Ignore all hash table entries where:
        // - Block Index >= BlockTableSize
        // - Flags of the appropriate block table entry
        //

        if(IsValidHashEntry1(ha, pHash, pBlockTable))
        {
            DWORD dwOldIndex = MPQ_BLOCK_INDEX(pHash);
            DWORD dwNewIndex = MPQ_BLOCK_INDEX(pHash);

            // Determine the new block index
            if(DefragmentTable != NULL)
            {
                // Need to handle case when multiple hash
                // entries point to the same block entry
                if(DefragmentTable[dwOldIndex] == HASH_ENTRY_FREE)
                {
                    DefragmentTable[dwOldIndex] = dwItemCount;
                    dwNewIndex = dwItemCount++;
                }
                else
                {
                    dwNewIndex = DefragmentTable[dwOldIndex];
                }

                // Fix the pointer in the hash entry
                pHash->dwBlockIndex = dwNewIndex;

                // Dump the relocation entry
//              printf("Relocating hash entry %08X-%08X: %08X -> %08X\n", pHash->dwName1, pHash->dwName2, dwBlockIndex, dwNewIndex);
            }

            // Get the pointer to the file entry and the block entry
            pFileEntry = ha->pFileTable + dwNewIndex;
            pBlock = pBlockTable + dwOldIndex;

            // ByteOffset is only valid if file size is not zero
            pFileEntry->ByteOffset = pBlock->dwFilePos;
            if(pFileEntry->ByteOffset == 0 && pBlock->dwFSize == 0)
                pFileEntry->ByteOffset = ha->pHeader->dwHeaderSize;

            // Fill the rest of the file entry
            pFileEntry->dwFileSize  = pBlock->dwFSize;
            pFileEntry->dwCmpSize   = pBlock->dwCSize;
            pFileEntry->dwFlags     = pBlock->dwFlags & dwFlagMask;
        }
    }

    // Free the translation table
    if(DefragmentTable != NULL)
    {
        // If we defragmented the block table in the process,
        // free some memory by shrinking the file table
        if(ha->dwFileTableSize > ha->dwMaxFileCount)
        {
            ha->pFileTable = STORM_REALLOC(TFileEntry, ha->pFileTable, ha->dwMaxFileCount);
            ha->pHeader->BlockTableSize64 = ha->dwMaxFileCount * sizeof(TMPQBlock);
            ha->pHeader->dwBlockTableSize = ha->dwMaxFileCount;
            ha->dwFileTableSize = ha->dwMaxFileCount;
        }

//      DumpFileTable(ha->pFileTable, ha->dwFileTableSize);

        // Free the translation table
        STORM_FREE(DefragmentTable);
    }

    return ERROR_SUCCESS;
}

static TMPQHash * TranslateHashTable(
    TMPQArchive * ha,
    ULONGLONG * pcbTableSize)
{
    TMPQHash * pHashTable;
    size_t HashTableSize;

    // Allocate copy of the hash table
    pHashTable = STORM_ALLOC(TMPQHash, ha->pHeader->dwHashTableSize);
    if(pHashTable != NULL)
    {
        // Copy the hash table
        HashTableSize = sizeof(TMPQHash) * ha->pHeader->dwHashTableSize;
        memcpy(pHashTable, ha->pHashTable, HashTableSize);

        // Give the size to the caller
        if(pcbTableSize != NULL)
        {
            *pcbTableSize = (ULONGLONG)HashTableSize;
        }
    }

    return pHashTable;
}

// Also used in SFileGetFileInfo
TMPQBlock * TranslateBlockTable(
    TMPQArchive * ha,
    ULONGLONG * pcbTableSize,
    bool * pbNeedHiBlockTable)
{
    TFileEntry * pFileEntry = ha->pFileTable;
    TMPQBlock * pBlockTable;
    TMPQBlock * pBlock;
    DWORD NeedHiBlockTable = 0;
    DWORD dwBlockTableSize = ha->pHeader->dwBlockTableSize;

    // Allocate copy of the hash table
    pBlockTable = pBlock = STORM_ALLOC(TMPQBlock, dwBlockTableSize);
    if(pBlockTable != NULL)
    {
        // Convert the block table
        for(DWORD i = 0; i < dwBlockTableSize; i++)
        {
            NeedHiBlockTable |= (DWORD)(pFileEntry->ByteOffset >> 32);
            pBlock->dwFilePos = (DWORD)pFileEntry->ByteOffset;
            pBlock->dwFSize   = pFileEntry->dwFileSize;
            pBlock->dwCSize   = pFileEntry->dwCmpSize;
            pBlock->dwFlags   = pFileEntry->dwFlags;

            pFileEntry++;
            pBlock++;
        }

        // Give the size to the caller
        if(pcbTableSize != NULL)
            *pcbTableSize = (ULONGLONG)dwBlockTableSize * sizeof(TMPQBlock);

        if(pbNeedHiBlockTable != NULL)
            *pbNeedHiBlockTable = NeedHiBlockTable ? true : false;
    }

    return pBlockTable;
}

static USHORT * TranslateHiBlockTable(
    TMPQArchive * ha,
    ULONGLONG * pcbTableSize)
{
    TFileEntry * pFileEntry = ha->pFileTable;
    USHORT * pHiBlockTable;
    USHORT * pHiBlock;
    DWORD dwBlockTableSize = ha->pHeader->dwBlockTableSize;

    // Allocate copy of the hash table
    pHiBlockTable = pHiBlock = STORM_ALLOC(USHORT, dwBlockTableSize);
    if(pHiBlockTable != NULL)
    {
        // Copy the block table
        for(DWORD i = 0; i < dwBlockTableSize; i++)
            pHiBlock[i] = (USHORT)(pFileEntry[i].ByteOffset >> 0x20);

        // Give the size to the caller
        if(pcbTableSize != NULL)
            *pcbTableSize = (ULONGLONG)dwBlockTableSize * sizeof(USHORT);
    }

    return pHiBlockTable;
}

//-----------------------------------------------------------------------------
// General EXT table functions

TMPQExtHeader * LoadExtTable(
    TMPQArchive * ha,
    ULONGLONG ByteOffset,
    size_t Size,
    DWORD dwSignature,
    DWORD dwKey)
{
    TMPQExtHeader * pCompressed = NULL;      // Compressed table
    TMPQExtHeader * pExtTable = NULL;        // Uncompressed table

    // Do nothing if the size is zero
    if(ByteOffset != 0 && Size != 0)
    {
        // Allocate size for the compressed table
        pExtTable = (TMPQExtHeader *)STORM_ALLOC(BYTE, Size);
        if(pExtTable != NULL)
        {
            // Load the table from the MPQ
            ByteOffset += ha->MpqPos;
            if(!FileStream_Read(ha->pStream, &ByteOffset, pExtTable, (DWORD)Size))
            {
                STORM_FREE(pExtTable);
                return NULL;
            }

            // Swap the ext table header
            BSWAP_ARRAY32_UNSIGNED(pExtTable, sizeof(TMPQExtHeader));
            if(pExtTable->dwSignature != dwSignature)
            {
                STORM_FREE(pExtTable);
                return NULL;
            }

            // Decrypt the block
            BSWAP_ARRAY32_UNSIGNED(pExtTable + 1, pExtTable->dwDataSize);
            DecryptMpqBlock(pExtTable + 1, (DWORD)(Size - sizeof(TMPQExtHeader)), dwKey);
            BSWAP_ARRAY32_UNSIGNED(pExtTable + 1, pExtTable->dwDataSize);

            // If the table is compressed, decompress it
            if((pExtTable->dwDataSize + sizeof(TMPQExtHeader)) > Size)
            {
                pCompressed = pExtTable;
                pExtTable = (TMPQExtHeader *)STORM_ALLOC(BYTE, sizeof(TMPQExtHeader) + pCompressed->dwDataSize);
                if(pExtTable != NULL)
                {
                    int cbOutBuffer = (int)pCompressed->dwDataSize;
                    int cbInBuffer = (int)Size;

                    // Decompress the extended table
                    pExtTable->dwSignature = pCompressed->dwSignature;
                    pExtTable->dwVersion   = pCompressed->dwVersion;
                    pExtTable->dwDataSize  = pCompressed->dwDataSize;
                    if(!SCompDecompress2(pExtTable + 1, &cbOutBuffer, pCompressed + 1, cbInBuffer))
                    {
                        STORM_FREE(pExtTable);
                        pExtTable = NULL;
                    }
                }

                // Free the compressed block
                STORM_FREE(pCompressed);
            }
        }
    }

    // Return the decompressed table to the caller
    return pExtTable;
}

#ifdef FULL
static int SaveMpqTable(
    TMPQArchive * ha,
    void * pMpqTable,
    ULONGLONG ByteOffset,
    size_t Size,
    unsigned char * md5,
    DWORD dwKey,
    bool bCompress)
{
    ULONGLONG FileOffset;
    void * pCompressed = NULL;
    int nError = ERROR_SUCCESS;

    // Do we have to compress the table?
    if(bCompress)
    {
        int cbOutBuffer = (int)Size;
        int cbInBuffer = (int)Size;

        // Allocate extra space for compressed table
        pCompressed = STORM_ALLOC(BYTE, Size);
        if(pCompressed == NULL)
            return ERROR_NOT_ENOUGH_MEMORY;

        // Compress the table
        SCompCompress(pCompressed, &cbOutBuffer, pMpqTable, cbInBuffer, MPQ_COMPRESSION_ZLIB, 0, 0);

        // If the compression failed, revert it. Otherwise, swap the tables
        if(cbOutBuffer >= cbInBuffer)
        {
            STORM_FREE(pCompressed);
            pCompressed = NULL;
        }
        else
        {
            pMpqTable = pCompressed;
        }
    }

    // Encrypt the table
    if(dwKey != 0)
    {
        BSWAP_ARRAY32_UNSIGNED(pMpqTable, Size);
        EncryptMpqBlock(pMpqTable, (DWORD)Size, dwKey);
        BSWAP_ARRAY32_UNSIGNED(pMpqTable, Size);
    }

    // Calculate the MD5
    if(md5 != NULL)
    {
        CalculateDataBlockHash(pMpqTable, (DWORD)Size, md5);
    }

    // Save the table to the MPQ
    BSWAP_ARRAY32_UNSIGNED(pMpqTable, Size);
    FileOffset = ha->MpqPos + ByteOffset;
    if(!FileStream_Write(ha->pStream, &FileOffset, pMpqTable, (DWORD)Size))
        nError = GetLastError();

    // Free the compressed table, if any
    if(pCompressed != NULL)
        STORM_FREE(pCompressed);
    return nError;
}

static int SaveExtTable(
    TMPQArchive * ha,
    TMPQExtHeader * pExtTable,
    ULONGLONG ByteOffset,
    DWORD dwTableSize,
    unsigned char * md5,
    DWORD dwKey,
    bool bCompress,
    LPDWORD pcbTotalSize)
{
    ULONGLONG FileOffset;
    TMPQExtHeader * pCompressed = NULL;
    DWORD cbTotalSize = 0;
    int nError = ERROR_SUCCESS;

    // Do we have to compress the table?
    if(bCompress)
    {
        int cbOutBuffer = (int)dwTableSize;
        int cbInBuffer = (int)dwTableSize;

        // Allocate extra space for compressed table
        pCompressed = (TMPQExtHeader *)STORM_ALLOC(BYTE, dwTableSize);
        if(pCompressed == NULL)
            return ERROR_NOT_ENOUGH_MEMORY;

        // Compress the table
        pCompressed->dwSignature = pExtTable->dwSignature;
        pCompressed->dwVersion   = pExtTable->dwVersion;
        pCompressed->dwDataSize  = pExtTable->dwDataSize;
        SCompCompress((pCompressed + 1), &cbOutBuffer, (pExtTable + 1), cbInBuffer, MPQ_COMPRESSION_ZLIB, 0, 0);

        // If the compression failed, revert it. Otherwise, swap the tables
        if(cbOutBuffer >= cbInBuffer)
        {
            STORM_FREE(pCompressed);
            pCompressed = NULL;
        }
        else
        {
            pExtTable = pCompressed;
        }
    }

    // Encrypt the table
    if(dwKey != 0)
    {
        BSWAP_ARRAY32_UNSIGNED(pExtTable + 1, pExtTable->dwDataSize);
        EncryptMpqBlock(pExtTable + 1, (DWORD)(dwTableSize - sizeof(TMPQExtHeader)), dwKey);
        BSWAP_ARRAY32_UNSIGNED(pExtTable + 1, pExtTable->dwDataSize);
    }

    // Calculate the MD5 of the table after
    if(md5 != NULL)
    {
        CalculateDataBlockHash(pExtTable, dwTableSize, md5);
    }

    // Save the table to the MPQ
    FileOffset = ha->MpqPos + ByteOffset;
    if(FileStream_Write(ha->pStream, &FileOffset, pExtTable, dwTableSize))
        cbTotalSize += dwTableSize;
    else
        nError = GetLastError();

    // We have to write raw data MD5
    if(nError == ERROR_SUCCESS && ha->pHeader->dwRawChunkSize != 0)
    {
        nError = WriteMemDataMD5(ha->pStream,
                                 FileOffset,
                                 pExtTable,
                                 dwTableSize,
                                 ha->pHeader->dwRawChunkSize,
                                &cbTotalSize);
    }

    // Give the total written size, if needed
    if(pcbTotalSize != NULL)
        *pcbTotalSize = cbTotalSize;

    // Free the compressed table, if any
    if(pCompressed != NULL)
        STORM_FREE(pCompressed);
    return nError;
}

//-----------------------------------------------------------------------------
// Support for HET table

static void CreateHetHeader(
    TMPQHetTable * pHetTable,
    TMPQHetHeader * pHetHeader)
{
    // Fill the common header
    pHetHeader->ExtHdr.dwSignature  = HET_TABLE_SIGNATURE;
    pHetHeader->ExtHdr.dwVersion    = 1;
    pHetHeader->ExtHdr.dwDataSize   = 0;

    // Fill the HET header
    pHetHeader->dwEntryCount        = pHetTable->dwEntryCount;
    pHetHeader->dwTotalCount        = pHetTable->dwTotalCount;
    pHetHeader->dwNameHashBitSize   = pHetTable->dwNameHashBitSize;
    pHetHeader->dwIndexSizeTotal    = pHetTable->dwIndexSizeTotal;
    pHetHeader->dwIndexSizeExtra    = pHetTable->dwIndexSizeExtra;
    pHetHeader->dwIndexSize         = pHetTable->dwIndexSize;
    pHetHeader->dwIndexTableSize    = ((pHetHeader->dwIndexSizeTotal * pHetTable->dwTotalCount) + 7) / 8;

    // Calculate the total size needed for holding HET table
    pHetHeader->ExtHdr.dwDataSize =
    pHetHeader->dwTableSize = sizeof(TMPQHetHeader) - sizeof(TMPQExtHeader) +
                              pHetHeader->dwTotalCount +
                              pHetHeader->dwIndexTableSize;
}

TMPQHetTable * CreateHetTable(DWORD dwEntryCount, DWORD dwTotalCount, DWORD dwNameHashBitSize, LPBYTE pbSrcData)
{
    TMPQHetTable * pHetTable;

    pHetTable = STORM_ALLOC(TMPQHetTable, 1);
    if(pHetTable != NULL)
    {
        // Zero the HET table
        memset(pHetTable, 0, sizeof(TMPQHetTable));

        // Hash sizes less than 0x40 bits are not tested
        assert(dwNameHashBitSize == 0x40);

        // Calculate masks
        pHetTable->AndMask64 = ((dwNameHashBitSize != 0x40) ? ((ULONGLONG)1 << dwNameHashBitSize) : 0) - 1;
        pHetTable->OrMask64 = (ULONGLONG)1 << (dwNameHashBitSize - 1);

        // If the total count is not entered, use default
        if(dwTotalCount == 0)
            dwTotalCount = (dwEntryCount * 4) / 3;

        // Store the HET table parameters
        pHetTable->dwEntryCount        = dwEntryCount;
        pHetTable->dwTotalCount        = dwTotalCount;
        pHetTable->dwNameHashBitSize   = dwNameHashBitSize;
        pHetTable->dwIndexSizeTotal    = GetNecessaryBitCount(dwEntryCount);
        pHetTable->dwIndexSizeExtra    = 0;
        pHetTable->dwIndexSize         = pHetTable->dwIndexSizeTotal;

        // Allocate array of hashes
        pHetTable->pNameHashes = STORM_ALLOC(BYTE, dwTotalCount);
        if(pHetTable->pNameHashes != NULL)
        {
            // Make sure the data are initialized
            memset(pHetTable->pNameHashes, 0, dwTotalCount);

            // Allocate the bit array for file indexes
            pHetTable->pBetIndexes = TMPQBits::Create(dwTotalCount * pHetTable->dwIndexSizeTotal, 0xFF);
            if(pHetTable->pBetIndexes != NULL)
            {
                // Initialize the HET table from the source data (if given)
                if(pbSrcData != NULL)
                {
                    // Copy the name hashes
                    memcpy(pHetTable->pNameHashes, pbSrcData, dwTotalCount);

                    // Copy the file indexes
                    memcpy(pHetTable->pBetIndexes->Elements, pbSrcData + dwTotalCount, pHetTable->pBetIndexes->NumberOfBytes);
                }

                // Return the result HET table
                return pHetTable;
            }

            // Free the name hashes
            STORM_FREE(pHetTable->pNameHashes);
        }

        STORM_FREE(pHetTable);
    }

    // Failed
    return NULL;
}

static int InsertHetEntry(TMPQHetTable * pHetTable, ULONGLONG FileNameHash, DWORD dwFileIndex)
{
    DWORD StartIndex;
    DWORD Index;
    BYTE NameHash1;

    // Get the start index and the high 8 bits of the name hash
    StartIndex = Index = (DWORD)(FileNameHash % pHetTable->dwTotalCount);
    NameHash1 = (BYTE)(FileNameHash >> (pHetTable->dwNameHashBitSize - 8));

    // Find a place where to put it
    for(;;)
    {
        // Did we find a free HET entry?
        if(pHetTable->pNameHashes[Index] == HET_ENTRY_FREE)
        {
            // Set the entry in the name hash table
            pHetTable->pNameHashes[Index] = NameHash1;

            // Set the entry in the file index table
            pHetTable->pBetIndexes->SetBits(pHetTable->dwIndexSizeTotal * Index,
                                            pHetTable->dwIndexSize,
                                           &dwFileIndex,
                                            4);
            return ERROR_SUCCESS;
        }

        // Move to the next entry in the HET table
        // If we came to the start index again, we are done
        Index = (Index + 1) % pHetTable->dwTotalCount;
        if(Index == StartIndex)
            break;
    }

    // No space in the HET table. Should never happen,
    // because the HET table is created according to the number of files
    assert(false);
    return ERROR_DISK_FULL;
}

static TMPQHetTable * TranslateHetTable(TMPQHetHeader * pHetHeader)
{
    TMPQHetTable * pHetTable = NULL;
    LPBYTE pbSrcData = (LPBYTE)(pHetHeader + 1);

    // Sanity check
    assert(pHetHeader->ExtHdr.dwSignature == HET_TABLE_SIGNATURE);
    assert(pHetHeader->ExtHdr.dwVersion == 1);

    // Verify size of the HET table
    if(pHetHeader->ExtHdr.dwDataSize >= (sizeof(TMPQHetHeader) - sizeof(TMPQExtHeader)))
    {
        // Verify the size of the table in the header
        if(pHetHeader->ExtHdr.dwDataSize >= pHetHeader->dwTableSize)
        {
            // The size of the HET table must be sum of header, hash and index table size
            assert((sizeof(TMPQHetHeader) - sizeof(TMPQExtHeader) + pHetHeader->dwTotalCount + pHetHeader->dwIndexTableSize) == pHetHeader->dwTableSize);

            // So far, all MPQs with HET Table have had total number of entries equal to 4/3 of file count
            // Exception: "2010 - Starcraft II\!maps\Tya's Zerg Defense (unprotected).SC2Map"
//          assert(((pHetHeader->dwEntryCount * 4) / 3) == pHetHeader->dwTotalCount);

            // The size of one index is predictable as well
            assert(GetNecessaryBitCount(pHetHeader->dwEntryCount) == pHetHeader->dwIndexSizeTotal);

            // The size of index table (in entries) is expected
            // to be the same like the hash table size (in bytes)
            assert(((pHetHeader->dwTotalCount * pHetHeader->dwIndexSizeTotal) + 7) / 8 == pHetHeader->dwIndexTableSize);

            // Create translated table
            pHetTable = CreateHetTable(pHetHeader->dwEntryCount, pHetHeader->dwTotalCount, pHetHeader->dwNameHashBitSize, pbSrcData);
            if(pHetTable != NULL)
            {
                // Now the sizes in the hash table should be already set
                assert(pHetTable->dwEntryCount     == pHetHeader->dwEntryCount);
                assert(pHetTable->dwTotalCount     == pHetHeader->dwTotalCount);
                assert(pHetTable->dwIndexSizeTotal == pHetHeader->dwIndexSizeTotal);

                // Copy the missing variables
                pHetTable->dwIndexSizeExtra = pHetHeader->dwIndexSizeExtra;
                pHetTable->dwIndexSize      = pHetHeader->dwIndexSize;
            }
        }
    }

    return pHetTable;
}

static TMPQExtHeader * TranslateHetTable(TMPQHetTable * pHetTable, ULONGLONG * pcbHetTable)
{
    TMPQHetHeader * pHetHeader = NULL;
    TMPQHetHeader HetHeader;
    LPBYTE pbLinearTable = NULL;
    LPBYTE pbTrgData;

    // Prepare header of the HET table
    CreateHetHeader(pHetTable, &HetHeader);

    // Allocate space for the linear table
    pbLinearTable = STORM_ALLOC(BYTE, sizeof(TMPQExtHeader) + HetHeader.dwTableSize);
    if(pbLinearTable != NULL)
    {
        // Copy the table header
        pHetHeader = (TMPQHetHeader *)pbLinearTable;
        memcpy(pHetHeader, &HetHeader, sizeof(TMPQHetHeader));
        pbTrgData = (LPBYTE)(pHetHeader + 1);

        // Copy the array of name hashes
        memcpy(pbTrgData, pHetTable->pNameHashes, pHetTable->dwTotalCount);
        pbTrgData += pHetTable->dwTotalCount;

        // Copy the bit array of BET indexes
        memcpy(pbTrgData, pHetTable->pBetIndexes->Elements, HetHeader.dwIndexTableSize);

        // Calculate the total size of the table, including the TMPQExtHeader
        if(pcbHetTable != NULL)
        {
            *pcbHetTable = (ULONGLONG)(sizeof(TMPQExtHeader) + HetHeader.dwTableSize);
        }
    }

    // Keep Coverity happy
    assert((TMPQExtHeader *)&pHetHeader->ExtHdr == (TMPQExtHeader *)pbLinearTable);
    return (TMPQExtHeader *)pbLinearTable;
}

static DWORD GetFileIndex_Het(TMPQArchive * ha, const char * szFileName)
{
    TMPQHetTable * pHetTable = ha->pHetTable;
    ULONGLONG FileNameHash;
    DWORD StartIndex;
    DWORD Index;
    BYTE NameHash1;                 // Upper 8 bits of the masked file name hash

    // If there are no entries in the HET table, do nothing
    if(pHetTable->dwEntryCount == 0)
        return HASH_ENTRY_FREE;

    // Do nothing if the MPQ has no HET table
    assert(ha->pHetTable != NULL);

    // Calculate 64-bit hash of the file name
    FileNameHash = (HashStringJenkins(szFileName) & pHetTable->AndMask64) | pHetTable->OrMask64;

    // Split the file name hash into two parts:
    // NameHash1: The highest 8 bits of the name hash
    // NameHash2: File name hash limited to hash size
    // Note: Our file table contains full name hash, no need to cut the high 8 bits before comparison
    NameHash1 = (BYTE)(FileNameHash >> (pHetTable->dwNameHashBitSize - 8));

    // Calculate the starting index to the hash table
    StartIndex = Index = (DWORD)(FileNameHash % pHetTable->dwTotalCount);

    // Go through HET table until we find a terminator
    while(pHetTable->pNameHashes[Index] != HET_ENTRY_FREE)
    {
        // Did we find a match ?
        if(pHetTable->pNameHashes[Index] == NameHash1)
        {
            DWORD dwFileIndex = 0;

            // Get the file index
            pHetTable->pBetIndexes->GetBits(pHetTable->dwIndexSizeTotal * Index,
                                            pHetTable->dwIndexSize,
                                           &dwFileIndex,
                                            sizeof(DWORD));

            // Verify the FileNameHash against the entry in the table of name hashes
            if(dwFileIndex <= ha->dwFileTableSize && ha->pFileTable[dwFileIndex].FileNameHash == FileNameHash)
            {
                return dwFileIndex;
            }
        }

        // Move to the next entry in the HET table
        // If we came to the start index again, we are done
        Index = (Index + 1) % pHetTable->dwTotalCount;
        if(Index == StartIndex)
            break;
    }

    // File not found
    return HASH_ENTRY_FREE;
}

void FreeHetTable(TMPQHetTable * pHetTable)
{
    if(pHetTable != NULL)
    {
        if(pHetTable->pNameHashes != NULL)
            STORM_FREE(pHetTable->pNameHashes);
        if(pHetTable->pBetIndexes != NULL)
            STORM_FREE(pHetTable->pBetIndexes);

        STORM_FREE(pHetTable);
    }
}

//-----------------------------------------------------------------------------
// Support for BET table

static void CreateBetHeader(
    TMPQArchive * ha,
    TMPQBetHeader * pBetHeader)
{
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pFileEntry;
    ULONGLONG MaxByteOffset = 0;
    DWORD FlagArray[MAX_FLAG_INDEX];
    DWORD dwMaxFlagIndex = 0;
    DWORD dwMaxFileSize = 0;
    DWORD dwMaxCmpSize = 0;
    DWORD dwFlagIndex;

    // Initialize array of flag combinations
    InitFileFlagArray(FlagArray);

    // Fill the common header
    pBetHeader->ExtHdr.dwSignature  = BET_TABLE_SIGNATURE;
    pBetHeader->ExtHdr.dwVersion    = 1;
    pBetHeader->ExtHdr.dwDataSize   = 0;

    // Get the maximum values for the BET table
    pFileTableEnd = ha->pFileTable + ha->pHeader->dwBlockTableSize;
    for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++)
    {
        //
        // Note: Deleted files must be counted as well
        //

        // Highest file position in the MPQ
        if(pFileEntry->ByteOffset > MaxByteOffset)
            MaxByteOffset = pFileEntry->ByteOffset;

        // Biggest file size
        if(pFileEntry->dwFileSize > dwMaxFileSize)
            dwMaxFileSize = pFileEntry->dwFileSize;

        // Biggest compressed size
        if(pFileEntry->dwCmpSize > dwMaxCmpSize)
            dwMaxCmpSize = pFileEntry->dwCmpSize;

        // Check if this flag was there before
        dwFlagIndex = GetFileFlagIndex(FlagArray, pFileEntry->dwFlags);
        if(dwFlagIndex > dwMaxFlagIndex)
            dwMaxFlagIndex = dwFlagIndex;
    }

    // Now save bit count for every piece of file information
    pBetHeader->dwBitIndex_FilePos   = 0;
    pBetHeader->dwBitCount_FilePos   = GetNecessaryBitCount(MaxByteOffset);

    pBetHeader->dwBitIndex_FileSize  = pBetHeader->dwBitIndex_FilePos + pBetHeader->dwBitCount_FilePos;
    pBetHeader->dwBitCount_FileSize  = GetNecessaryBitCount(dwMaxFileSize);

    pBetHeader->dwBitIndex_CmpSize   = pBetHeader->dwBitIndex_FileSize + pBetHeader->dwBitCount_FileSize;
    pBetHeader->dwBitCount_CmpSize   = GetNecessaryBitCount(dwMaxCmpSize);

    pBetHeader->dwBitIndex_FlagIndex = pBetHeader->dwBitIndex_CmpSize + pBetHeader->dwBitCount_CmpSize;
    pBetHeader->dwBitCount_FlagIndex = GetNecessaryBitCount(dwMaxFlagIndex + 1);

    pBetHeader->dwBitIndex_Unknown   = pBetHeader->dwBitIndex_FlagIndex + pBetHeader->dwBitCount_FlagIndex;
    pBetHeader->dwBitCount_Unknown   = 0;

    // Calculate the total size of one entry
    pBetHeader->dwTableEntrySize     = pBetHeader->dwBitCount_FilePos +
                                       pBetHeader->dwBitCount_FileSize +
                                       pBetHeader->dwBitCount_CmpSize +
                                       pBetHeader->dwBitCount_FlagIndex +
                                       pBetHeader->dwBitCount_Unknown;

    // Save the file count and flag count
    pBetHeader->dwEntryCount         = ha->pHeader->dwBlockTableSize;
    pBetHeader->dwFlagCount          = dwMaxFlagIndex + 1;
    pBetHeader->dwUnknown08          = 0x10;

    // Save the total size of the BET hash
    pBetHeader->dwBitTotal_NameHash2 = ha->pHetTable->dwNameHashBitSize - 0x08;
    pBetHeader->dwBitExtra_NameHash2 = 0;
    pBetHeader->dwBitCount_NameHash2 = pBetHeader->dwBitTotal_NameHash2;
    pBetHeader->dwNameHashArraySize  = ((pBetHeader->dwBitTotal_NameHash2 * pBetHeader->dwEntryCount) + 7) / 8;

    // Save the total table size
    pBetHeader->ExtHdr.dwDataSize    =
    pBetHeader->dwTableSize          = sizeof(TMPQBetHeader) - sizeof(TMPQExtHeader) +
                                       pBetHeader->dwFlagCount * sizeof(DWORD) +
                                     ((pBetHeader->dwTableEntrySize * pBetHeader->dwEntryCount) + 7) / 8 +
                                      pBetHeader->dwNameHashArraySize;
}

TMPQBetTable * CreateBetTable(DWORD dwEntryCount)
{
    TMPQBetTable * pBetTable;

    // Allocate BET table
    pBetTable = STORM_ALLOC(TMPQBetTable, 1);
    if(pBetTable != NULL)
    {
        memset(pBetTable, 0, sizeof(TMPQBetTable));
        pBetTable->dwEntryCount = dwEntryCount;
    }

    return pBetTable;
}

static TMPQBetTable * TranslateBetTable(
    TMPQArchive * ha,
    TMPQBetHeader * pBetHeader)
{
    TMPQBetTable * pBetTable = NULL;
    LPBYTE pbSrcData = (LPBYTE)(pBetHeader + 1);
    DWORD LengthInBytes = 0;

    // Sanity check
    assert(pBetHeader->ExtHdr.dwSignature == BET_TABLE_SIGNATURE);
    assert(pBetHeader->ExtHdr.dwVersion == 1);
    assert(ha->pHetTable != NULL);
    ha = ha;

    // Verify size of the HET table
    if(pBetHeader->ExtHdr.dwDataSize >= (sizeof(TMPQBetHeader) - sizeof(TMPQExtHeader)))
    {
        // Verify the size of the table in the header
        if(pBetHeader->ExtHdr.dwDataSize >= pBetHeader->dwTableSize)
        {
            // The number of entries in the BET table must be the same like number of entries in the block table
            // Note: Ignored if there is no block table
            //assert(pBetHeader->dwEntryCount == ha->pHeader->dwBlockTableSize);
            assert(pBetHeader->dwEntryCount <= ha->dwMaxFileCount);

            // The number of entries in the BET table must be the same like number of entries in the HET table
            // Note that if it's not, it is not a problem
            //assert(pBetHeader->dwEntryCount == ha->pHetTable->dwEntryCount);

            // Create translated table
            pBetTable = CreateBetTable(pBetHeader->dwEntryCount);
            if(pBetTable != NULL)
            {
                // Copy the variables from the header to the BetTable
                pBetTable->dwTableEntrySize     = pBetHeader->dwTableEntrySize;
                pBetTable->dwBitIndex_FilePos   = pBetHeader->dwBitIndex_FilePos;
                pBetTable->dwBitIndex_FileSize  = pBetHeader->dwBitIndex_FileSize;
                pBetTable->dwBitIndex_CmpSize   = pBetHeader->dwBitIndex_CmpSize;
                pBetTable->dwBitIndex_FlagIndex = pBetHeader->dwBitIndex_FlagIndex;
                pBetTable->dwBitIndex_Unknown   = pBetHeader->dwBitIndex_Unknown;
                pBetTable->dwBitCount_FilePos   = pBetHeader->dwBitCount_FilePos;
                pBetTable->dwBitCount_FileSize  = pBetHeader->dwBitCount_FileSize;
                pBetTable->dwBitCount_CmpSize   = pBetHeader->dwBitCount_CmpSize;
                pBetTable->dwBitCount_FlagIndex = pBetHeader->dwBitCount_FlagIndex;
                pBetTable->dwBitCount_Unknown   = pBetHeader->dwBitCount_Unknown;

                // Since we don't know what the "unknown" is, we'll assert when it's zero
                assert(pBetTable->dwBitCount_Unknown == 0);

                // Allocate array for flags
                if(pBetHeader->dwFlagCount != 0)
                {
                    // Allocate array for file flags and load it
                    pBetTable->pFileFlags = STORM_ALLOC(DWORD, pBetHeader->dwFlagCount);
                    if(pBetTable->pFileFlags != NULL)
                    {
                        LengthInBytes = pBetHeader->dwFlagCount * sizeof(DWORD);
                        memcpy(pBetTable->pFileFlags, pbSrcData, LengthInBytes);
                        BSWAP_ARRAY32_UNSIGNED(pBetTable->pFileFlags, LengthInBytes);
                        pbSrcData += LengthInBytes;
                    }

                    // Save the number of flags
                    pBetTable->dwFlagCount = pBetHeader->dwFlagCount;
                }

                // Load the bit-based file table
                pBetTable->pFileTable = TMPQBits::Create(pBetTable->dwTableEntrySize * pBetHeader->dwEntryCount, 0);
                if(pBetTable->pFileTable != NULL)
                {
                    LengthInBytes = (pBetTable->pFileTable->NumberOfBits + 7) / 8;
                    memcpy(pBetTable->pFileTable->Elements, pbSrcData, LengthInBytes);
                    pbSrcData += LengthInBytes;
                }

                // Fill the sizes of BET hash
                pBetTable->dwBitTotal_NameHash2 = pBetHeader->dwBitTotal_NameHash2;
                pBetTable->dwBitExtra_NameHash2 = pBetHeader->dwBitExtra_NameHash2;
                pBetTable->dwBitCount_NameHash2 = pBetHeader->dwBitCount_NameHash2;

                // Create and load the array of BET hashes
                pBetTable->pNameHashes = TMPQBits::Create(pBetTable->dwBitTotal_NameHash2 * pBetHeader->dwEntryCount, 0);
                if(pBetTable->pNameHashes != NULL)
                {
                    LengthInBytes = (pBetTable->pNameHashes->NumberOfBits + 7) / 8;
                    memcpy(pBetTable->pNameHashes->Elements, pbSrcData, LengthInBytes);
//                  pbSrcData += LengthInBytes;
                }

                // Dump both tables
//              DumpHetAndBetTable(ha->pHetTable, pBetTable);
            }
        }
    }

    return pBetTable;
}

TMPQExtHeader * TranslateBetTable(
    TMPQArchive * ha,
    ULONGLONG * pcbBetTable)
{
    TMPQBetHeader * pBetHeader = NULL;
    TMPQBetHeader BetHeader;
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pFileEntry;
    TMPQBits * pBitArray = NULL;
    LPBYTE pbLinearTable = NULL;
    LPBYTE pbTrgData;
    DWORD LengthInBytes;
    DWORD FlagArray[MAX_FLAG_INDEX];

    // Calculate the bit sizes of various entries
    InitFileFlagArray(FlagArray);
    CreateBetHeader(ha, &BetHeader);

    // Allocate space
    pbLinearTable = STORM_ALLOC(BYTE, sizeof(TMPQExtHeader) + BetHeader.dwTableSize);
    if(pbLinearTable != NULL)
    {
        // Copy the BET header to the linear buffer
        pBetHeader = (TMPQBetHeader *)pbLinearTable;
        memcpy(pBetHeader, &BetHeader, sizeof(TMPQBetHeader));
        pbTrgData = (LPBYTE)(pBetHeader + 1);

        // Save the bit-based block table
        pBitArray = TMPQBits::Create(BetHeader.dwEntryCount * BetHeader.dwTableEntrySize, 0);
        if(pBitArray != NULL)
        {
            DWORD dwFlagIndex = 0;
            DWORD nBitOffset = 0;

            // Construct the bit-based file table
            pFileTableEnd = ha->pFileTable + BetHeader.dwEntryCount;
            for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++)
            {
                //
                // Note: Missing files must be included as well
                //

                // Save the byte offset
                pBitArray->SetBits(nBitOffset + BetHeader.dwBitIndex_FilePos,
                                   BetHeader.dwBitCount_FilePos,
                                  &pFileEntry->ByteOffset,
                                   8);
                pBitArray->SetBits(nBitOffset + BetHeader.dwBitIndex_FileSize,
                                   BetHeader.dwBitCount_FileSize,
                                  &pFileEntry->dwFileSize,
                                   4);
                pBitArray->SetBits(nBitOffset + BetHeader.dwBitIndex_CmpSize,
                                   BetHeader.dwBitCount_CmpSize,
                                  &pFileEntry->dwCmpSize,
                                   4);

                // Save the flag index
                dwFlagIndex = GetFileFlagIndex(FlagArray, pFileEntry->dwFlags);
                pBitArray->SetBits(nBitOffset + BetHeader.dwBitIndex_FlagIndex,
                                   BetHeader.dwBitCount_FlagIndex,
                                  &dwFlagIndex,
                                   4);

                // Move the bit offset
                nBitOffset += BetHeader.dwTableEntrySize;
            }

            // Write the array of flags
            LengthInBytes = BetHeader.dwFlagCount * sizeof(DWORD);
            memcpy(pbTrgData, FlagArray, LengthInBytes);
            BSWAP_ARRAY32_UNSIGNED(pbTrgData, LengthInBytes);
            pbTrgData += LengthInBytes;

            // Write the bit-based block table
            LengthInBytes = (pBitArray->NumberOfBits + 7) / 8;
            memcpy(pbTrgData, pBitArray->Elements, LengthInBytes);
            pbTrgData += LengthInBytes;

            // Free the bit array
            STORM_FREE(pBitArray);
        }

        // Create bit array for name hashes
        pBitArray = TMPQBits::Create(BetHeader.dwBitTotal_NameHash2 * BetHeader.dwEntryCount, 0);
        if(pBitArray != NULL)
        {
            DWORD dwFileIndex = 0;

            for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++)
            {
                // Insert the name hash to the bit array
                pBitArray->SetBits(BetHeader.dwBitTotal_NameHash2 * dwFileIndex,
                                   BetHeader.dwBitCount_NameHash2,
                                  &pFileEntry->FileNameHash,
                                   8);

                assert(dwFileIndex < BetHeader.dwEntryCount);
                dwFileIndex++;
            }

            // Write the array of BET hashes
            LengthInBytes = (pBitArray->NumberOfBits + 7) / 8;
            memcpy(pbTrgData, pBitArray->Elements, LengthInBytes);
//          pbTrgData += LengthInBytes;

            // Free the bit array
            STORM_FREE(pBitArray);
        }

        // Write the size of the BET table in the MPQ
        if(pcbBetTable != NULL)
        {
            *pcbBetTable = (ULONGLONG)(sizeof(TMPQExtHeader) + BetHeader.dwTableSize);
        }
    }

    // Keep Coverity happy
    assert((TMPQExtHeader *)&pBetHeader->ExtHdr == (TMPQExtHeader *)pbLinearTable);
    return (TMPQExtHeader *)pbLinearTable;
}

void FreeBetTable(TMPQBetTable * pBetTable)
{
    if(pBetTable != NULL)
    {
        if(pBetTable->pFileTable != NULL)
            STORM_FREE(pBetTable->pFileTable);
        if(pBetTable->pFileFlags != NULL)
            STORM_FREE(pBetTable->pFileFlags);
        if(pBetTable->pNameHashes != NULL)
            STORM_FREE(pBetTable->pNameHashes);

        STORM_FREE(pBetTable);
    }
}
#endif // FULL

//-----------------------------------------------------------------------------
// Support for file table

TFileEntry * GetFileEntryLocale2(TMPQArchive * ha, const char * szFileName, LCID lcLocale, LPDWORD PtrHashIndex)
{
    TMPQHash * pHash;
    DWORD dwFileIndex;

    // First, we have to search the classic hash table
    // This is because on renaming, deleting, or changing locale,
    // we will need the pointer to hash table entry
    if(ha->pHashTable != NULL)
    {
        pHash = GetHashEntryLocale(ha, szFileName, lcLocale, 0);
        if(pHash != NULL && MPQ_BLOCK_INDEX(pHash) < ha->dwFileTableSize)
        {
            if(PtrHashIndex != NULL)
                PtrHashIndex[0] = (DWORD)(pHash - ha->pHashTable);
            return ha->pFileTable + MPQ_BLOCK_INDEX(pHash);
        }
    }

#ifdef FULL
    // If we have HET table in the MPQ, try to find the file in HET table
    if(ha->pHetTable != NULL)
    {
        dwFileIndex = GetFileIndex_Het(ha, szFileName);
        if(dwFileIndex != HASH_ENTRY_FREE)
            return ha->pFileTable + dwFileIndex;
    }
#endif // FULL

    // Not found
    return NULL;
}

TFileEntry * GetFileEntryLocale(TMPQArchive * ha, const char * szFileName, LCID lcLocale)
{
    return GetFileEntryLocale2(ha, szFileName, lcLocale, NULL);
}

TFileEntry * GetFileEntryExact(TMPQArchive * ha, const char * szFileName, LCID lcLocale, LPDWORD PtrHashIndex)
{
    TMPQHash * pHash;
    DWORD dwFileIndex;

    // If the hash table is present, find the entry from hash table
    if(ha->pHashTable != NULL)
    {
        pHash = GetHashEntryExact(ha, szFileName, lcLocale);
        if(pHash != NULL && MPQ_BLOCK_INDEX(pHash) < ha->dwFileTableSize)
        {
            if(PtrHashIndex != NULL)
                PtrHashIndex[0] = (DWORD)(pHash - ha->pHashTable);
            return ha->pFileTable + MPQ_BLOCK_INDEX(pHash);
        }
    }

#ifdef FULL
    // If we have HET table in the MPQ, try to find the file in HET table
    if(ha->pHetTable != NULL)
    {
        dwFileIndex = GetFileIndex_Het(ha, szFileName);
        if(dwFileIndex != HASH_ENTRY_FREE)
        {
            if(PtrHashIndex != NULL)
                PtrHashIndex[0] = HASH_ENTRY_FREE;
            return ha->pFileTable + dwFileIndex;
        }
    }
#endif // FULL

    // Not found
    return NULL;
}

void AllocateFileName(TMPQArchive * ha, TFileEntry * pFileEntry, const char * szFileName)
{
    // Sanity check
    assert(pFileEntry != NULL);

    // If the file name is pseudo file name, free it at this point
    if(IsPseudoFileName(pFileEntry->szFileName, NULL))
    {
        if(pFileEntry->szFileName != NULL)
            STORM_FREE(pFileEntry->szFileName);
        pFileEntry->szFileName = NULL;
    }

    // Only allocate new file name if it's not there yet
    if(pFileEntry->szFileName == NULL)
    {
        pFileEntry->szFileName = STORM_ALLOC(char, strlen(szFileName) + 1);
        if(pFileEntry->szFileName != NULL)
            strcpy(pFileEntry->szFileName, szFileName);
    }

#ifdef FULL
    // We also need to create the file name hash
    if(ha->pHetTable != NULL)
    {
        ULONGLONG AndMask64 = ha->pHetTable->AndMask64;
        ULONGLONG OrMask64 = ha->pHetTable->OrMask64;

        pFileEntry->FileNameHash = (HashStringJenkins(szFileName) & AndMask64) | OrMask64;
    }
#endif // FULL
}

TFileEntry * AllocateFileEntry(TMPQArchive * ha, const char * szFileName, LCID lcLocale, LPDWORD PtrHashIndex)
{
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pFreeEntry = NULL;
    TFileEntry * pFileEntry;
    TMPQHash * pHash = NULL;
    DWORD dwReservedFiles = ha->dwReservedFiles;
    DWORD dwFreeCount = 0;

    // Sanity check: File table size must be greater or equal to max file count
    assert(ha->dwFileTableSize >= ha->dwMaxFileCount);

    // If we are saving MPQ tables, we don't tale number of reserved files into account
    dwReservedFiles = (ha->dwFlags & MPQ_FLAG_SAVING_TABLES) ? 0 : ha->dwReservedFiles;

    // Now find a free entry in the file table.
    // Note that in the case when free entries are in the middle,
    // we need to use these
    for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++)
    {
        if((pFileEntry->dwFlags & MPQ_FILE_EXISTS) == 0)
        {
            // Remember the first free entry
            if(pFreeEntry == NULL)
                pFreeEntry = pFileEntry;
            dwFreeCount++;

            // If the number of free items is greater than number
            // of reserved items, We can add the file
            if(dwFreeCount > dwReservedFiles)
                break;
        }
    }

    // If the total number of free entries is less than number of reserved files,
    // we cannot add the file to the archive
    if(pFreeEntry == NULL || dwFreeCount <= dwReservedFiles)
        return NULL;

    // Initialize the file entry and set its file name
    memset(pFreeEntry, 0, sizeof(TFileEntry));
    AllocateFileName(ha, pFreeEntry, szFileName);

    // If the archive has a hash table, we need to first free entry there
    if(ha->pHashTable != NULL)
    {
        // Make sure that the entry is not there yet
        assert(GetHashEntryExact(ha, szFileName, lcLocale) == NULL);

        // Find a free hash table entry for the name
        pHash = AllocateHashEntry(ha, pFreeEntry, lcLocale);
        if(pHash == NULL)
            return NULL;

        // Set the file index to the hash table
        pHash->dwBlockIndex = (DWORD)(pFreeEntry - ha->pFileTable);
        PtrHashIndex[0] = (DWORD)(pHash - ha->pHashTable);
    }

#ifdef FULL
    // If the archive has a HET table, just do some checks
    // Note: Don't bother modifying the HET table. It will be rebuilt from scratch after, anyway
    if(ha->pHetTable != NULL)
    {
        assert(GetFileIndex_Het(ha, szFileName) == HASH_ENTRY_FREE);
    }
#endif // FULL

    // Return the free table entry
    return pFreeEntry;
}

int RenameFileEntry(
    TMPQArchive * ha,
    TMPQFile * hf,
    const char * szNewFileName)
{
    TFileEntry * pFileEntry = hf->pFileEntry;
    TMPQHash * pHashEntry = hf->pHashEntry;
    LCID lcLocale = 0;

    // If the archive hash hash table, we need to free the hash table entry
    if(ha->pHashTable != NULL)
    {
        // The file must have hash table entry assigned
        // Will exit if there are multiple HASH entries pointing to the same file entry
        if(pHashEntry == NULL)
            return ERROR_NOT_SUPPORTED;

        // Save the locale
        lcLocale = pHashEntry->lcLocale;

        // Mark the hash table entry as deleted
        pHashEntry->dwName1      = 0xFFFFFFFF;
        pHashEntry->dwName2      = 0xFFFFFFFF;
        pHashEntry->lcLocale     = 0xFFFF;
        pHashEntry->Platform     = 0xFF;
        pHashEntry->Reserved     = 0xFF;
        pHashEntry->dwBlockIndex = HASH_ENTRY_DELETED;
    }

    // Free the old file name
    if(pFileEntry->szFileName != NULL)
        STORM_FREE(pFileEntry->szFileName);
    pFileEntry->szFileName = NULL;

    // Allocate new file name
    AllocateFileName(ha, pFileEntry, szNewFileName);

    // Allocate new hash entry
    if(ha->pHashTable != NULL)
    {
        // Since we freed one hash entry before, this must succeed
        hf->pHashEntry = AllocateHashEntry(ha, pFileEntry, lcLocale);
        assert(hf->pHashEntry != NULL);
    }

    return ERROR_SUCCESS;
}

int DeleteFileEntry(TMPQArchive * ha, TMPQFile * hf)
{
    TFileEntry * pFileEntry = hf->pFileEntry;
    TMPQHash * pHashEntry = hf->pHashEntry;

    // If the archive hash hash table, we need to free the hash table entry
    if(ha->pHashTable != NULL)
    {
        // The file must have hash table entry assigned
        // Will exit if there are multiple HASH entries pointing to the same file entry
        if(pHashEntry == NULL)
            return ERROR_NOT_SUPPORTED;

        // Mark the hash table entry as deleted
        pHashEntry->dwName1      = 0xFFFFFFFF;
        pHashEntry->dwName2      = 0xFFFFFFFF;
        pHashEntry->lcLocale     = 0xFFFF;
        pHashEntry->Platform     = 0xFF;
        pHashEntry->Reserved     = 0xFF;
        pHashEntry->dwBlockIndex = HASH_ENTRY_DELETED;
    }

    // Free the file name, and set the file entry as deleted
    if(pFileEntry->szFileName != NULL)
        STORM_FREE(pFileEntry->szFileName);
    pFileEntry->szFileName = NULL;

    //
    // Don't modify the HET table, because it gets recreated by the caller
    // Don't decrement the number of entries in the file table
    // Keep Byte Offset, file size, compressed size, CRC32 and MD5
    // Clear the file name hash and the MPQ_FILE_EXISTS bit
    //

    pFileEntry->dwFlags &= ~MPQ_FILE_EXISTS;
    pFileEntry->FileNameHash = 0;
    return ERROR_SUCCESS;
}

DWORD InvalidateInternalFile(TMPQArchive * ha, const char * szFileName, DWORD dwFlagNone, DWORD dwFlagNew, DWORD dwForceAddTheFile = 0)
{
    TMPQFile * hf = NULL;
    DWORD dwFileFlags = MPQ_FILE_DEFAULT_INTERNAL;
    int nError = ERROR_FILE_NOT_FOUND;

    // Open the file from the MPQ
    if(SFileOpenFileEx((HANDLE)ha, szFileName, SFILE_OPEN_BASE_FILE, (HANDLE *)&hf))
    {
        // Remember the file flags
        dwFileFlags = hf->pFileEntry->dwFlags;

        // Delete the file entry
        nError = DeleteFileEntry(ha, hf);
        if(nError == ERROR_SUCCESS)
            dwForceAddTheFile = 1;

        // Close the file
        FreeFileHandle(hf);
    }

    // Are we going to add the file?
    if(dwForceAddTheFile)
    {
        ha->dwFlags |= dwFlagNew;
        ha->dwReservedFiles++;
    }
    else
    {
        ha->dwFlags |= dwFlagNone;
        dwFileFlags = 0;
    }

    // Return the intended file flags
    return dwFileFlags;
}

void InvalidateInternalFiles(TMPQArchive * ha)
{
    // Do nothing if we are in the middle of saving internal files
    if(!(ha->dwFlags & MPQ_FLAG_SAVING_TABLES))
    {
        //
        // We clear the file entries for (listfile), (attributes) and (signature)
        // For each internal file cleared, we increment the number
        // of reserved entries in the file table.
        //

        // Invalidate the (listfile), if not done yet
        if((ha->dwFlags & (MPQ_FLAG_LISTFILE_NONE | MPQ_FLAG_LISTFILE_NEW)) == 0)
        {
            ha->dwFileFlags1 = InvalidateInternalFile(ha, LISTFILE_NAME, MPQ_FLAG_LISTFILE_NONE, MPQ_FLAG_LISTFILE_NEW, (ha->dwFlags & MPQ_FLAG_LISTFILE_FORCE));
        }

        // Invalidate the (attributes), if not done yet
        if((ha->dwFlags & (MPQ_FLAG_ATTRIBUTES_NONE | MPQ_FLAG_ATTRIBUTES_NEW)) == 0)
        {
            ha->dwFileFlags2 = InvalidateInternalFile(ha, ATTRIBUTES_NAME, MPQ_FLAG_ATTRIBUTES_NONE, MPQ_FLAG_ATTRIBUTES_NEW);
        }

        // Invalidate the (signature), if not done yet
        if((ha->dwFlags & (MPQ_FLAG_SIGNATURE_NONE | MPQ_FLAG_SIGNATURE_NEW)) == 0)
        {
            ha->dwFileFlags3 = InvalidateInternalFile(ha, SIGNATURE_NAME, MPQ_FLAG_SIGNATURE_NONE, MPQ_FLAG_SIGNATURE_NEW);
        }

        // Remember that the MPQ has been changed
        ha->dwFlags |= MPQ_FLAG_CHANGED;
    }
}

//-----------------------------------------------------------------------------
// Support for file tables - hash table, block table, hi-block table

int CreateHashTable(TMPQArchive * ha, DWORD dwHashTableSize)
{
    TMPQHash * pHashTable;

    // Sanity checks
    assert((dwHashTableSize & (dwHashTableSize - 1)) == 0);
    assert(ha->pHashTable == NULL);

    // If the required hash table size is zero, don't create anything
    if(dwHashTableSize == 0)
        dwHashTableSize = HASH_TABLE_SIZE_DEFAULT;

    // Create the hash table
    pHashTable = STORM_ALLOC(TMPQHash, dwHashTableSize);
    if(pHashTable == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    // Fill it
    memset(pHashTable, 0xFF, dwHashTableSize * sizeof(TMPQHash));
    ha->pHeader->dwHashTableSize = dwHashTableSize;
    ha->dwMaxFileCount = dwHashTableSize;
    ha->pHashTable = pHashTable;
    return ERROR_SUCCESS;
}

static TMPQHash * LoadHashTable(TMPQArchive * ha)
{
    TMPQHeader * pHeader = ha->pHeader;
    ULONGLONG ByteOffset;
    TMPQHash * pHashTable = NULL;
    DWORD dwTableSize;
    DWORD dwCmpSize;
    bool bHashTableIsCut = false;

    // Note: It is allowed to load hash table if it is at offset 0.
    // Example: MPQ_2016_v1_ProtectedMap_HashOffsIsZero.w3x
//  if(pHeader->dwHashTablePos == 0 && pHeader->wHashTablePosHi == 0)
//      return NULL;

    // If the hash table size is zero, do nothing
    if(pHeader->dwHashTableSize == 0)
        return NULL;

    // Load the hash table for MPQ variations
    switch(ha->dwSubType)
    {
        case MPQ_SUBTYPE_MPQ:

            // Calculate the position and size of the hash table
            ByteOffset = FileOffsetFromMpqOffset(ha, MAKE_OFFSET64(pHeader->wHashTablePosHi, pHeader->dwHashTablePos));
            dwTableSize = pHeader->dwHashTableSize * sizeof(TMPQHash);
            dwCmpSize = (DWORD)pHeader->HashTableSize64;

            // Read, decrypt and uncompress the hash table
            pHashTable = (TMPQHash *)LoadMpqTable(ha, ByteOffset, pHeader->MD5_HashTable, dwCmpSize, dwTableSize, g_dwHashTableKey, &bHashTableIsCut);
//          DumpHashTable(pHashTable, pHeader->dwHashTableSize);

            // If the hash table was cut, we can/have to defragment it
            if(pHashTable != NULL && bHashTableIsCut)
                ha->dwFlags |= (MPQ_FLAG_MALFORMED | MPQ_FLAG_HASH_TABLE_CUT);
            break;

        case MPQ_SUBTYPE_SQP:
            pHashTable = LoadSqpHashTable(ha);
            break;

        case MPQ_SUBTYPE_MPK:
            pHashTable = LoadMpkHashTable(ha);
            break;
    }

    // Return the loaded hash table
    return pHashTable;
}

int CreateFileTable(TMPQArchive * ha, DWORD dwFileTableSize)
{
    ha->pFileTable = STORM_ALLOC(TFileEntry, dwFileTableSize);
    if(ha->pFileTable == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    memset(ha->pFileTable, 0x00, sizeof(TFileEntry) * dwFileTableSize);
    ha->dwFileTableSize = dwFileTableSize;
    return ERROR_SUCCESS;
}

TMPQBlock * LoadBlockTable(TMPQArchive * ha, bool /* bDontFixEntries */)
{
    TMPQHeader * pHeader = ha->pHeader;
    TMPQBlock * pBlockTable = NULL;
    ULONGLONG ByteOffset;
    DWORD dwTableSize;
    DWORD dwCmpSize;
    bool bBlockTableIsCut = false;

    // Note: It is possible that the block table starts at offset 0
    // Example: MPQ_2016_v1_ProtectedMap_HashOffsIsZero.w3x
//  if(pHeader->dwBlockTablePos == 0 && pHeader->wBlockTablePosHi == 0)
//      return NULL;

    // Do nothing if the block table size is zero
    if(pHeader->dwBlockTableSize == 0)
        return NULL;

    // Load the block table for MPQ variations
    switch(ha->dwSubType)
    {
        case MPQ_SUBTYPE_MPQ:

            // Calculate byte position of the block table
            ByteOffset = FileOffsetFromMpqOffset(ha, MAKE_OFFSET64(pHeader->wBlockTablePosHi, pHeader->dwBlockTablePos));
            dwTableSize = pHeader->dwBlockTableSize * sizeof(TMPQBlock);
            dwCmpSize = (DWORD)pHeader->BlockTableSize64;

            // Read, decrypt and uncompress the block table
            pBlockTable = (TMPQBlock * )LoadMpqTable(ha, ByteOffset, NULL, dwCmpSize, dwTableSize, g_dwBlockTableKey, &bBlockTableIsCut);

            // If the block table was cut, we need to remember it
            if(pBlockTable != NULL && bBlockTableIsCut)
                ha->dwFlags |= (MPQ_FLAG_MALFORMED | MPQ_FLAG_BLOCK_TABLE_CUT);
            break;

        case MPQ_SUBTYPE_SQP:

            pBlockTable = LoadSqpBlockTable(ha);
            break;

        case MPQ_SUBTYPE_MPK:

            pBlockTable = LoadMpkBlockTable(ha);
            break;
    }

    return pBlockTable;
}

#ifdef FULL
TMPQHetTable * LoadHetTable(TMPQArchive * ha)
{
    TMPQExtHeader * pExtTable;
    TMPQHetTable * pHetTable = NULL;
    TMPQHeader * pHeader = ha->pHeader;

    // If the HET table position is not 0, we expect the table to be present
    if(pHeader->HetTablePos64 && pHeader->HetTableSize64)
    {
        // Attempt to load the HET table (Hash Extended Table)
        pExtTable = LoadExtTable(ha, pHeader->HetTablePos64, (size_t)pHeader->HetTableSize64, HET_TABLE_SIGNATURE, MPQ_KEY_HASH_TABLE);
        if(pExtTable != NULL)
        {
            // If loading HET table fails, we ignore the result.
            pHetTable = TranslateHetTable((TMPQHetHeader *)pExtTable);
            STORM_FREE(pExtTable);
        }
    }

    return pHetTable;
}

TMPQBetTable * LoadBetTable(TMPQArchive * ha)
{
    TMPQExtHeader * pExtTable;
    TMPQBetTable * pBetTable = NULL;
    TMPQHeader * pHeader = ha->pHeader;

    // If the BET table position is not 0, we expect the table to be present
    if(pHeader->BetTablePos64 && pHeader->BetTableSize64)
    {
        // Attempt to load the HET table (Hash Extended Table)
        pExtTable = LoadExtTable(ha, pHeader->BetTablePos64, (size_t)pHeader->BetTableSize64, BET_TABLE_SIGNATURE, MPQ_KEY_BLOCK_TABLE);
        if(pExtTable != NULL)
        {
            // If succeeded, we translate the BET table
            // to more readable form
            pBetTable = TranslateBetTable(ha, (TMPQBetHeader *)pExtTable);
            STORM_FREE(pExtTable);
        }
    }

    return pBetTable;
}
#endif // FULL

int LoadAnyHashTable(TMPQArchive * ha)
{
    TMPQHeader * pHeader = ha->pHeader;

    // If the MPQ archive is empty, don't bother trying to load anything
    if(pHeader->dwHashTableSize == 0 && pHeader->HetTableSize64 == 0)
        return CreateHashTable(ha, HASH_TABLE_SIZE_DEFAULT);

#ifdef FULL
    // Try to load HET table
    if(pHeader->HetTablePos64 != 0)
        ha->pHetTable = LoadHetTable(ha);
#endif // FULL

    // Try to load classic hash table
    // Note that we load the classic hash table even when HET table exists,
    // because if the MPQ gets modified and saved, hash table must be there
    if(pHeader->dwHashTableSize)
        ha->pHashTable = LoadHashTable(ha);

    // At least one of the tables must be present
    if(ha->pHetTable == NULL && ha->pHashTable == NULL)
        return ERROR_FILE_CORRUPT;

    // Set the maximum file count to the size of the hash table.
    // Note: We don't care about HET table limits, because HET table is rebuilt
    // after each file add/rename/delete.
    ha->dwMaxFileCount = (ha->pHashTable != NULL) ? pHeader->dwHashTableSize : HASH_TABLE_SIZE_MAX;
    return ERROR_SUCCESS;
}

static int BuildFileTable_Classic(TMPQArchive * ha)
{
    TMPQHeader * pHeader = ha->pHeader;
    TMPQBlock * pBlockTable;
    int nError = ERROR_SUCCESS;

    // Sanity checks
    assert(ha->pHashTable != NULL);
    assert(ha->pFileTable != NULL);

    // If the MPQ has no block table, do nothing
    if(pHeader->dwBlockTableSize == 0)
        return ERROR_SUCCESS;
    assert(ha->dwFileTableSize >= pHeader->dwBlockTableSize);

    // Load the block table
    // WARNING! ha->pFileTable can change in the process!!
    pBlockTable = (TMPQBlock *)LoadBlockTable(ha);
    if(pBlockTable != NULL)
    {
        nError = BuildFileTableFromBlockTable(ha, pBlockTable);
        STORM_FREE(pBlockTable);
    }
    else
    {
        nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Load the hi-block table
    if(nError == ERROR_SUCCESS && pHeader->HiBlockTablePos64 != 0)
    {
        ULONGLONG ByteOffset;
        USHORT * pHiBlockTable = NULL;
        DWORD dwTableSize = pHeader->dwBlockTableSize * sizeof(USHORT);

        // Allocate space for the hi-block table
        // Note: pHeader->dwBlockTableSize can be zero !!!
        pHiBlockTable = STORM_ALLOC(USHORT, pHeader->dwBlockTableSize + 1);
        if(pHiBlockTable != NULL)
        {
            // Load the hi-block table. It is not encrypted, nor compressed
            ByteOffset = ha->MpqPos + pHeader->HiBlockTablePos64;
            if(!FileStream_Read(ha->pStream, &ByteOffset, pHiBlockTable, dwTableSize))
                nError = GetLastError();

            // Now merge the hi-block table to the file table
            if(nError == ERROR_SUCCESS)
            {
                TFileEntry * pFileEntry = ha->pFileTable;

                // Swap the hi-block table
                BSWAP_ARRAY16_UNSIGNED(pHiBlockTable, dwTableSize);

                // Add the high file offset to the base file offset.
                for(DWORD i = 0; i < pHeader->dwBlockTableSize; i++, pFileEntry++)
                    pFileEntry->ByteOffset = MAKE_OFFSET64(pHiBlockTable[i], pFileEntry->ByteOffset);
            }

            // Free the hi-block table
            STORM_FREE(pHiBlockTable);
        }
        else
        {
            nError = ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    return nError;
}

#ifdef FULL
static int BuildFileTable_HetBet(TMPQArchive * ha)
{
    TMPQHetTable * pHetTable = ha->pHetTable;
    TMPQBetTable * pBetTable;
    TFileEntry * pFileEntry = ha->pFileTable;
    TMPQBits * pBitArray;
    DWORD dwBitPosition = 0;
    DWORD i;
    int nError = ERROR_FILE_CORRUPT;

    // Load the BET table from the MPQ
    pBetTable = LoadBetTable(ha);
    if(pBetTable != NULL)
    {
        // Verify the size of NameHash2 in the BET table.
        // It has to be 8 bits less than the information in HET table
        if((pBetTable->dwBitCount_NameHash2 + 8) != pHetTable->dwNameHashBitSize)
        {
            FreeBetTable(pBetTable);
            return ERROR_FILE_CORRUPT;
        }

        // Step one: Fill the name indexes
        for(i = 0; i < pHetTable->dwTotalCount; i++)
        {
            DWORD dwFileIndex = 0;

            // Is the entry in the HET table occupied?
            if(pHetTable->pNameHashes[i] != HET_ENTRY_FREE)
            {
                // Load the index to the BET table
                pHetTable->pBetIndexes->GetBits(pHetTable->dwIndexSizeTotal * i,
                                                pHetTable->dwIndexSize,
                                               &dwFileIndex,
                                                4);
                // Overflow test
                if(dwFileIndex < pBetTable->dwEntryCount)
                {
                    ULONGLONG NameHash1 = pHetTable->pNameHashes[i];
                    ULONGLONG NameHash2 = 0;

                    // Load the BET hash
                    pBetTable->pNameHashes->GetBits(pBetTable->dwBitTotal_NameHash2 * dwFileIndex,
                                                    pBetTable->dwBitCount_NameHash2,
                                                   &NameHash2,
                                                    8);

                    // Combine both part of the name hash and put it to the file table
                    pFileEntry = ha->pFileTable + dwFileIndex;
                    pFileEntry->FileNameHash = (NameHash1 << pBetTable->dwBitCount_NameHash2) | NameHash2;
                }
            }
        }

        // Go through the entire BET table and convert it to the file table.
        pFileEntry = ha->pFileTable;
        pBitArray = pBetTable->pFileTable;
        for(i = 0; i < pBetTable->dwEntryCount; i++)
        {
            DWORD dwFlagIndex = 0;

            // Read the file position
            pBitArray->GetBits(dwBitPosition + pBetTable->dwBitIndex_FilePos,
                               pBetTable->dwBitCount_FilePos,
                              &pFileEntry->ByteOffset,
                               8);

            // Read the file size
            pBitArray->GetBits(dwBitPosition + pBetTable->dwBitIndex_FileSize,
                               pBetTable->dwBitCount_FileSize,
                              &pFileEntry->dwFileSize,
                               4);

            // Read the compressed size
            pBitArray->GetBits(dwBitPosition + pBetTable->dwBitIndex_CmpSize,
                               pBetTable->dwBitCount_CmpSize,
                              &pFileEntry->dwCmpSize,
                               4);


            // Read the flag index
            if(pBetTable->dwFlagCount != 0)
            {
                pBitArray->GetBits(dwBitPosition + pBetTable->dwBitIndex_FlagIndex,
                                   pBetTable->dwBitCount_FlagIndex,
                                  &dwFlagIndex,
                                   4);
                pFileEntry->dwFlags = pBetTable->pFileFlags[dwFlagIndex];
            }

            //
            // TODO: Locale (?)
            //

            // Move the current bit position
            dwBitPosition += pBetTable->dwTableEntrySize;
            pFileEntry++;
        }

        // Set the current size of the file table
        FreeBetTable(pBetTable);
        nError = ERROR_SUCCESS;
    }
    else
    {
        nError = ERROR_FILE_CORRUPT;
    }

    return nError;
}
#endif // FULL

int BuildFileTable(TMPQArchive * ha)
{
    DWORD dwFileTableSize;
    bool bFileTableCreated = false;

    // Sanity checks
    assert(ha->pFileTable == NULL);
    assert(ha->dwFileTableSize == 0);
    assert(ha->dwMaxFileCount != 0);

    // Determine the allocation size for the file table
    dwFileTableSize = STORMLIB_MAX(ha->pHeader->dwBlockTableSize, ha->dwMaxFileCount);

    // Allocate the file table with size determined before
    ha->pFileTable = STORM_ALLOC(TFileEntry, dwFileTableSize);
    if(ha->pFileTable == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    // Fill the table with zeros
    memset(ha->pFileTable, 0, dwFileTableSize * sizeof(TFileEntry));
    ha->dwFileTableSize = dwFileTableSize;

#ifdef FULL
    // If we have HET table, we load file table from the BET table
    // Note: If BET table is corrupt or missing, we set the archive as read only
    if(ha->pHetTable != NULL)
    {
        if(BuildFileTable_HetBet(ha) != ERROR_SUCCESS)
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;
        else
            bFileTableCreated = true;
    }
#endif // FULL

    // If we have hash table, we load the file table from the block table
    // Note: If block table is corrupt or missing, we set the archive as read only
    if(ha->pHashTable != NULL)
    {
        if(BuildFileTable_Classic(ha) != ERROR_SUCCESS)
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;
        else
            bFileTableCreated = true;
    }

    // Return result
    return bFileTableCreated ? ERROR_SUCCESS : ERROR_FILE_CORRUPT;
}

/*
void UpdateBlockTableSize(TMPQArchive * ha)
{
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pFileEntry;
    DWORD dwBlockTableSize = 0;

    // Calculate the number of files
    for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++)
    {
        // If the source table entry is valid,
        if(pFileEntry->dwFlags & MPQ_FILE_EXISTS)
            dwBlockTableSize = (DWORD)(pFileEntry - ha->pFileTable) + 1;
    }

    // Save the block table size to the MPQ header
    ha->pHeader->dwBlockTableSize = ha->dwReservedFiles + dwBlockTableSize;
}
*/

#ifdef FULL
// Defragment the file table so it does not contain any gaps
int DefragmentFileTable(TMPQArchive * ha)
{
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pSource = ha->pFileTable;
    TFileEntry * pTarget = ha->pFileTable;
    LPDWORD DefragmentTable;
    DWORD dwBlockTableSize = 0;
    DWORD dwSrcIndex;
    DWORD dwTrgIndex;

    // Allocate brand new file table
    DefragmentTable = STORM_ALLOC(DWORD, ha->dwFileTableSize);
    if(DefragmentTable != NULL)
    {
        // Clear the file table
        memset(DefragmentTable, 0xFF, sizeof(DWORD) * ha->dwFileTableSize);

        // Parse the entire file table and defragment it
        for(; pSource < pFileTableEnd; pSource++)
        {
            // If the source table entry is valid,
            if(pSource->dwFlags & MPQ_FILE_EXISTS)
            {
                // Remember the index conversion
                dwSrcIndex = (DWORD)(pSource - ha->pFileTable);
                dwTrgIndex = (DWORD)(pTarget - ha->pFileTable);
                DefragmentTable[dwSrcIndex] = dwTrgIndex;

                // Move the entry, if needed
                if(pTarget != pSource)
                    pTarget[0] = pSource[0];
                pTarget++;

                // Update the block table size
                dwBlockTableSize = (DWORD)(pTarget - ha->pFileTable);
            }
			else
			{
				// If there is file name left, free it
				if(pSource->szFileName != NULL)
					STORM_FREE(pSource->szFileName);
				pSource->szFileName = NULL;
			}
        }

        // Did we defragment something?
        if(pTarget < pFileTableEnd)
        {
            // Clear the remaining file entries
            memset(pTarget, 0, (pFileTableEnd - pTarget) * sizeof(TFileEntry));

            // Go through the hash table and relocate the block indexes
            if(ha->pHashTable != NULL)
            {
                TMPQHash * pHashTableEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
                TMPQHash * pHash;
				DWORD dwNewBlockIndex;

                for(pHash = ha->pHashTable; pHash < pHashTableEnd; pHash++)
                {
                    if(MPQ_BLOCK_INDEX(pHash) < ha->dwFileTableSize)
					{
						// If that block entry is there, set it to the hash entry
						// If not, set it as DELETED
						dwNewBlockIndex = DefragmentTable[MPQ_BLOCK_INDEX(pHash)];
						pHash->dwBlockIndex = (dwNewBlockIndex != HASH_ENTRY_FREE) ? dwNewBlockIndex : HASH_ENTRY_DELETED;
					}
                }
            }
        }

        // Save the block table size
        ha->pHeader->dwBlockTableSize = ha->dwReservedFiles + dwBlockTableSize;

        // Free the defragment table
        STORM_FREE(DefragmentTable);
    }

    return ERROR_SUCCESS;
}

// Rebuilds the HET table from scratch based on the file table
// Used after a modifying operation (add, rename, delete)
int RebuildHetTable(TMPQArchive * ha)
{
    TMPQHetTable * pOldHetTable = ha->pHetTable;
    TFileEntry * pFileTableEnd;
    TFileEntry * pFileEntry;
    DWORD dwBlockTableSize = ha->dwFileTableSize;
    int nError = ERROR_SUCCESS;

    // If we are in the state of saving MPQ tables, the real size of block table
    // must already have been calculated. Use that value instead
    if(ha->dwFlags & MPQ_FLAG_SAVING_TABLES)
    {
        assert(ha->pHeader->dwBlockTableSize != 0);
        dwBlockTableSize = ha->pHeader->dwBlockTableSize;
    }

    // Create new HET table based on the total number of entries in the file table
    // Note that if we fail to create it, we just stop using HET table
    ha->pHetTable = CreateHetTable(dwBlockTableSize, 0, 0x40, NULL);
    if(ha->pHetTable != NULL)
    {
        // Go through the file table again and insert all existing files
        pFileTableEnd = ha->pFileTable + dwBlockTableSize;
        for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++)
        {
            if(pFileEntry->dwFlags & MPQ_FILE_EXISTS)
            {
                // Get the high
                nError = InsertHetEntry(ha->pHetTable, pFileEntry->FileNameHash, (DWORD)(pFileEntry - ha->pFileTable));
                if(nError != ERROR_SUCCESS)
                    break;
            }
        }
    }

    // Free the old HET table
    FreeHetTable(pOldHetTable);
    return nError;
}

// Rebuilds the file table, removing all deleted file entries.
// Used when compacting the archive
int RebuildFileTable(TMPQArchive * ha, DWORD dwNewHashTableSize)
{
    TFileEntry * pFileEntry;
    TMPQHash * pHashTableEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPQHash * pOldHashTable = ha->pHashTable;
    TMPQHash * pHashTable = NULL;
    TMPQHash * pHash;
    int nError = ERROR_SUCCESS;

    // The new hash table size must be greater or equal to the current hash table size
    assert(dwNewHashTableSize >= ha->pHeader->dwHashTableSize);
    assert(dwNewHashTableSize >= ha->dwMaxFileCount);
    assert((dwNewHashTableSize & (dwNewHashTableSize - 1)) == 0);
    assert(ha->pHashTable != NULL);

    // Reallocate the new file table, if needed
    if(dwNewHashTableSize > ha->dwFileTableSize)
    {
        ha->pFileTable = STORM_REALLOC(TFileEntry, ha->pFileTable, dwNewHashTableSize);
        if(ha->pFileTable == NULL)
            return ERROR_NOT_ENOUGH_MEMORY;

        memset(ha->pFileTable + ha->dwFileTableSize, 0, (dwNewHashTableSize - ha->dwFileTableSize) * sizeof(TFileEntry));
    }

    // Allocate new hash table
    if(nError == ERROR_SUCCESS)
    {
        pHashTable = STORM_ALLOC(TMPQHash, dwNewHashTableSize);
        if(pHashTable == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // If both succeeded, we need to rebuild the file table
    if(nError == ERROR_SUCCESS)
    {
        // Make sure that the hash table is properly filled
        memset(pHashTable, 0xFF, sizeof(TMPQHash) * dwNewHashTableSize);
        ha->pHashTable = pHashTable;

        // Set the new limits to the MPQ archive
        ha->pHeader->dwHashTableSize = dwNewHashTableSize;

        // Parse the old hash table and copy all entries to the new table
        for(pHash = pOldHashTable; pHash < pHashTableEnd; pHash++)
        {
            if(IsValidHashEntry(ha, pHash))
            {
                pFileEntry = ha->pFileTable + MPQ_BLOCK_INDEX(pHash);
                AllocateHashEntry(ha, pFileEntry, pHash->lcLocale);
            }
        }

        // Increment the max file count for the file
        ha->dwFileTableSize = dwNewHashTableSize;
        ha->dwMaxFileCount = dwNewHashTableSize;
        ha->dwFlags |= MPQ_FLAG_CHANGED;
    }

    // Now free the remaining entries
    if(pOldHashTable != NULL)
        STORM_FREE(pOldHashTable);
    return nError;
}

// Saves MPQ header, hash table, block table and hi-block table.
int SaveMPQTables(TMPQArchive * ha)
{
    TMPQExtHeader * pHetTable = NULL;
    TMPQExtHeader * pBetTable = NULL;
    TMPQHeader * pHeader = ha->pHeader;
    TMPQBlock * pBlockTable = NULL;
    TMPQHash * pHashTable = NULL;
    ULONGLONG HetTableSize64 = 0;
    ULONGLONG BetTableSize64 = 0;
    ULONGLONG HashTableSize64 = 0;
    ULONGLONG BlockTableSize64 = 0;
    ULONGLONG HiBlockTableSize64 = 0;
    ULONGLONG TablePos = 0;             // A table position, relative to the begin of the MPQ
    USHORT * pHiBlockTable = NULL;
    DWORD cbTotalSize;
    bool bNeedHiBlockTable = false;
    int nError = ERROR_SUCCESS;

    // We expect this function to be called only when tables have been changed
    assert(ha->dwFlags & MPQ_FLAG_CHANGED);

    // Find the space where the MPQ tables will be saved
    TablePos = FindFreeMpqSpace(ha);

    // If the MPQ has HET table, we prepare a ready-to-save version
    if(nError == ERROR_SUCCESS && ha->pHetTable != NULL)
    {
        pHetTable = TranslateHetTable(ha->pHetTable, &HetTableSize64);
        if(pHetTable == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // If the MPQ has HET table, we also must create BET table to be saved
    if(nError == ERROR_SUCCESS && ha->pHetTable != NULL)
    {
        pBetTable = TranslateBetTable(ha, &BetTableSize64);
        if(pBetTable == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Now create hash table
    if(nError == ERROR_SUCCESS && ha->pHashTable != NULL)
    {
        pHashTable = TranslateHashTable(ha, &HashTableSize64);
        if(pHashTable == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Create block table
    if(nError == ERROR_SUCCESS && ha->pFileTable != NULL)
    {
        pBlockTable = TranslateBlockTable(ha, &BlockTableSize64, &bNeedHiBlockTable);
        if(pBlockTable == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Create hi-block table, if needed
    if(nError == ERROR_SUCCESS && bNeedHiBlockTable)
    {
        pHiBlockTable = TranslateHiBlockTable(ha, &HiBlockTableSize64);
        if(pHiBlockTable == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Write the HET table, if any
    if(nError == ERROR_SUCCESS && pHetTable != NULL)
    {
        pHeader->HetTableSize64 = HetTableSize64;
        pHeader->HetTablePos64  = TablePos;
        nError = SaveExtTable(ha, pHetTable, TablePos, (DWORD)HetTableSize64, pHeader->MD5_HetTable, MPQ_KEY_HASH_TABLE, false, &cbTotalSize);
        TablePos += cbTotalSize;
    }

    // Write the BET table, if any
    if(nError == ERROR_SUCCESS && pBetTable != NULL)
    {
        pHeader->BetTableSize64 = BetTableSize64;
        pHeader->BetTablePos64  = TablePos;
        nError = SaveExtTable(ha, pBetTable, TablePos, (DWORD)BetTableSize64, pHeader->MD5_BetTable, MPQ_KEY_BLOCK_TABLE, false, &cbTotalSize);
        TablePos += cbTotalSize;
    }

    // Write the hash table, if we have any
    if(nError == ERROR_SUCCESS && pHashTable != NULL)
    {
        pHeader->HashTableSize64 = HashTableSize64;
        pHeader->wHashTablePosHi = (USHORT)(TablePos >> 32);
        pHeader->dwHashTableSize = (DWORD)(HashTableSize64 / sizeof(TMPQHash));
        pHeader->dwHashTablePos = (DWORD)TablePos;
        nError = SaveMpqTable(ha, pHashTable, TablePos, (size_t)HashTableSize64, pHeader->MD5_HashTable, MPQ_KEY_HASH_TABLE, false);
        TablePos += HashTableSize64;
    }

    // Write the block table, if we have any
    if(nError == ERROR_SUCCESS && pBlockTable != NULL)
    {
        pHeader->BlockTableSize64 = BlockTableSize64;
        pHeader->wBlockTablePosHi = (USHORT)(TablePos >> 32);
        pHeader->dwBlockTableSize = (DWORD)(BlockTableSize64 / sizeof(TMPQBlock));
        pHeader->dwBlockTablePos = (DWORD)TablePos;
        nError = SaveMpqTable(ha, pBlockTable, TablePos, (size_t)BlockTableSize64, pHeader->MD5_BlockTable, MPQ_KEY_BLOCK_TABLE, false);
        TablePos += BlockTableSize64;
    }

    // Write the hi-block table, if we have any
    if(nError == ERROR_SUCCESS && pHiBlockTable != NULL)
    {
        ULONGLONG ByteOffset = ha->MpqPos + TablePos;

        pHeader->HiBlockTableSize64 = HiBlockTableSize64;
        pHeader->HiBlockTablePos64 = TablePos;
        BSWAP_ARRAY16_UNSIGNED(pHiBlockTable, HiBlockTableSize64);

        if(!FileStream_Write(ha->pStream, &ByteOffset, pHiBlockTable, (DWORD)HiBlockTableSize64))
            nError = GetLastError();
        TablePos += HiBlockTableSize64;
    }

    // Cut the MPQ
    if(nError == ERROR_SUCCESS)
    {
        ULONGLONG FileSize = ha->MpqPos + TablePos;

        if(!FileStream_SetSize(ha->pStream, FileSize))
            nError = GetLastError();
    }

    // Write the MPQ header
    if(nError == ERROR_SUCCESS)
    {
        TMPQHeader SaveMpqHeader;

        // Update the size of the archive
        pHeader->ArchiveSize64 = TablePos;
        pHeader->dwArchiveSize = (DWORD)TablePos;

        // Update the MD5 of the archive header
        CalculateDataBlockHash(pHeader, MPQ_HEADER_SIZE_V4 - MD5_DIGEST_SIZE, pHeader->MD5_MpqHeader);

        // Write the MPQ header to the file
        memcpy(&SaveMpqHeader, pHeader, pHeader->dwHeaderSize);
        BSWAP_TMPQHEADER(&SaveMpqHeader, MPQ_FORMAT_VERSION_1);
        BSWAP_TMPQHEADER(&SaveMpqHeader, MPQ_FORMAT_VERSION_2);
        BSWAP_TMPQHEADER(&SaveMpqHeader, MPQ_FORMAT_VERSION_3);
        BSWAP_TMPQHEADER(&SaveMpqHeader, MPQ_FORMAT_VERSION_4);
        if(!FileStream_Write(ha->pStream, &ha->MpqPos, &SaveMpqHeader, pHeader->dwHeaderSize))
            nError = GetLastError();
    }

    // Clear the changed flag
    if(nError == ERROR_SUCCESS)
        ha->dwFlags &= ~MPQ_FLAG_CHANGED;

    // Cleanup and exit
    if(pHetTable != NULL)
        STORM_FREE(pHetTable);
    if(pBetTable != NULL)
        STORM_FREE(pBetTable);
    if(pHashTable != NULL)
        STORM_FREE(pHashTable);
    if(pBlockTable != NULL)
        STORM_FREE(pBlockTable);
    if(pHiBlockTable != NULL)
        STORM_FREE(pHiBlockTable);
    return nError;
}
#endif // FULL
