/*****************************************************************************/
/* SBaseSubTypes.cpp                      Copyright (c) Ladislav Zezula 2013 */
/*---------------------------------------------------------------------------*/
/* Conversion routines for archive formats that are similar to MPQ format    */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 02.11.11  1.00  Lad  The first version of SBaseSubTypes.cpp               */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "StormCommon.h"

/*****************************************************************************/
/*                                                                           */
/*         Support for SQP file format (War of the Immortals)                */
/*                                                                           */
/*****************************************************************************/

typedef struct _TSQPHeader
{
    // The ID_MPQ ('MPQ\x1A') signature
    DWORD dwID;                         

    // Size of the archive header
    DWORD dwHeaderSize;                   

    // 32-bit size of MPQ archive
    DWORD dwArchiveSize;

    // Offset to the beginning of the hash table, relative to the beginning of the archive.
    DWORD dwHashTablePos;
    
    // Offset to the beginning of the block table, relative to the beginning of the archive.
    DWORD dwBlockTablePos;
    
    // Number of entries in the hash table. Must be a power of two, and must be less than 2^16 for
    // the original MoPaQ format, or less than 2^20 for the Burning Crusade format.
    DWORD dwHashTableSize;
    
    // Number of entries in the block table
    DWORD dwBlockTableSize;

    // Must be zero for SQP files
    USHORT wFormatVersion;

    // Power of two exponent specifying the number of 512-byte disk sectors in each file sector
    // in the archive. The size of each file sector in the archive is 512 * 2 ^ wSectorSize.
    USHORT wSectorSize;

} TSQPHeader;

typedef struct _TSQPHash
{
    // Most likely the lcLocale+wPlatform.
    DWORD dwAlwaysZero;

    // If the hash table entry is valid, this is the index into the block table of the file.
    // Otherwise, one of the following two values:
    //  - FFFFFFFFh: Hash table entry is empty, and has always been empty.
    //               Terminates searches for a given file.
    //  - FFFFFFFEh: Hash table entry is empty, but was valid at some point (a deleted file).
    //               Does not terminate searches for a given file.
    DWORD dwBlockIndex;

    // The hash of the file path, using method A.
    DWORD dwName1;
    
    // The hash of the file path, using method B.
    DWORD dwName2;

} TSQPHash;

typedef struct _TSQPBlock
{
    // Offset of the beginning of the file, relative to the beginning of the archive.
    DWORD dwFilePos;
    
    // Flags for the file. See MPQ_FILE_XXXX constants
    DWORD dwFlags;                      

    // Compressed file size
    DWORD dwCSize;
    
    // Uncompressed file size
    DWORD dwFSize;                      
    
} TSQPBlock;

//-----------------------------------------------------------------------------
// Functions - SQP file format

// This function converts SQP file header into MPQ file header
int ConvertSqpHeaderToFormat4(
    TMPQArchive * ha,
    ULONGLONG FileSize,
    DWORD dwFlags)
{
    TSQPHeader * pSqpHeader = (TSQPHeader *)ha->HeaderData;
    TMPQHeader Header;

    // SQP files from War of the Immortal use MPQ file format with slightly
    // modified structure. These fields have different position:
    //
    //  Offset  TMPQHeader             TSQPHeader
    //  ------  ----------             -----------
    //   000C   wFormatVersion         dwHashTablePos (lo)
    //   000E   wSectorSize            dwHashTablePos (hi)
    //   001C   dwBlockTableSize (lo)  wBlockSize
    //   001E   dwHashTableSize (hi)   wFormatVersion

    // Can't open the archive with certain flags
    if(dwFlags & MPQ_OPEN_FORCE_MPQ_V1)
        return ERROR_FILE_CORRUPT;

    // The file must not be greater than 4 GB
    if((FileSize >> 0x20) != 0)
        return ERROR_FILE_CORRUPT;

    // Translate the SQP header into a MPQ header
    memset(&Header, 0, sizeof(TMPQHeader));
    Header.dwID = BSWAP_INT32_UNSIGNED(pSqpHeader->dwID);
    Header.dwHeaderSize = BSWAP_INT32_UNSIGNED(pSqpHeader->dwHeaderSize);
    Header.dwArchiveSize = BSWAP_INT32_UNSIGNED(pSqpHeader->dwArchiveSize);
    Header.dwHashTablePos = BSWAP_INT32_UNSIGNED(pSqpHeader->dwHashTablePos);
    Header.dwBlockTablePos = BSWAP_INT32_UNSIGNED(pSqpHeader->dwBlockTablePos);
    Header.dwHashTableSize = BSWAP_INT32_UNSIGNED(pSqpHeader->dwHashTableSize);
    Header.dwBlockTableSize = BSWAP_INT32_UNSIGNED(pSqpHeader->dwBlockTableSize);
    Header.wFormatVersion = BSWAP_INT16_UNSIGNED(pSqpHeader->wFormatVersion);
    Header.wSectorSize = BSWAP_INT16_UNSIGNED(pSqpHeader->wSectorSize);

    // Verify the SQP header
    if(Header.dwID == ID_MPQ && Header.dwHeaderSize == sizeof(TSQPHeader) && Header.dwArchiveSize == FileSize)
    {
        // Check for fixed values of version and sector size
        if(Header.wFormatVersion == MPQ_FORMAT_VERSION_1 && Header.wSectorSize == 3)
        {
            // Initialize the fields of 3.0 header
            Header.ArchiveSize64    = Header.dwArchiveSize;
            Header.HashTableSize64  = Header.dwHashTableSize * sizeof(TMPQHash);
            Header.BlockTableSize64 = Header.dwBlockTableSize * sizeof(TMPQBlock);

            // Copy the converted MPQ header back
            memcpy(ha->HeaderData, &Header, sizeof(TMPQHeader));
            
            // Mark this file as SQP file
            ha->pfnHashString = HashStringSlash;
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;
            ha->dwSubType = MPQ_SUBTYPE_SQP;
            return ERROR_SUCCESS;
        }
    }

    return ERROR_FILE_CORRUPT;
}

void * LoadSqpTable(TMPQArchive * ha, DWORD dwByteOffset, DWORD cbTableSize, DWORD dwKey)
{
    ULONGLONG ByteOffset;
    LPBYTE pbSqpTable;

    // Allocate buffer for the table
    pbSqpTable = STORM_ALLOC(BYTE, cbTableSize);
    if(pbSqpTable != NULL)
    {
        // Load the table
        ByteOffset = ha->MpqPos + dwByteOffset;
        if(FileStream_Read(ha->pStream, &ByteOffset, pbSqpTable, cbTableSize))
        {
            // Decrypt the SQP table
            DecryptMpqBlock(pbSqpTable, cbTableSize, dwKey);
            return pbSqpTable;
        }

        // Free the table
        STORM_FREE(pbSqpTable);
    }

    return NULL;
}

TMPQHash * LoadSqpHashTable(TMPQArchive * ha)
{
    TMPQHeader * pHeader = ha->pHeader;
    TSQPHash * pSqpHashTable;
    TSQPHash * pSqpHashEnd;
    TSQPHash * pSqpHash;
    TMPQHash * pMpqHash;
    int nError = ERROR_SUCCESS;

    // Load the hash table
    pSqpHashTable = (TSQPHash *)LoadSqpTable(ha, pHeader->dwHashTablePos, pHeader->dwHashTableSize * sizeof(TSQPHash), MPQ_KEY_HASH_TABLE);
    if(pSqpHashTable != NULL)
    {
        // Parse the entire hash table and convert it to MPQ hash table
        pSqpHashEnd = pSqpHashTable + pHeader->dwHashTableSize;
        pMpqHash = (TMPQHash *)pSqpHashTable;
        for(pSqpHash = pSqpHashTable; pSqpHash < pSqpHashEnd; pSqpHash++, pMpqHash++)
        {
            // Ignore free entries
            if(pSqpHash->dwBlockIndex != HASH_ENTRY_FREE)
            {
                // Check block index against the size of the block table
                if(pHeader->dwBlockTableSize <= MPQ_BLOCK_INDEX(pSqpHash) && pSqpHash->dwBlockIndex < HASH_ENTRY_DELETED)
                    nError = ERROR_FILE_CORRUPT;

                // We do not support nonzero locale and platform ID
                if(pSqpHash->dwAlwaysZero != 0 && pSqpHash->dwAlwaysZero != HASH_ENTRY_FREE)
                    nError = ERROR_FILE_CORRUPT;

                // Store the file name hash
                pMpqHash->dwName1 = pSqpHash->dwName1;
                pMpqHash->dwName2 = pSqpHash->dwName2;

                // Store the rest. Note that this must be done last,
                // because block index corresponds to pMpqHash->dwName2
                pMpqHash->dwBlockIndex = MPQ_BLOCK_INDEX(pSqpHash);
                pMpqHash->Platform = 0;
                pMpqHash->lcLocale = 0;
            }
        }

        // If an error occured, we need to free the hash table
        if(nError != ERROR_SUCCESS)
        {
            STORM_FREE(pSqpHashTable);
            pSqpHashTable = NULL;
        }
    }

    // Return the converted hash table (or NULL on failure)
    return (TMPQHash *)pSqpHashTable;
}

// Loads the SQP Block table and converts it to a MPQ block table
TMPQBlock * LoadSqpBlockTable(TMPQArchive * ha)
{
    TMPQHeader * pHeader = ha->pHeader;
    TSQPBlock * pSqpBlockTable;
    TSQPBlock * pSqpBlockEnd;
    TSQPBlock * pSqpBlock;
    TMPQBlock * pMpqBlock;
    DWORD dwFlags;
    int nError = ERROR_SUCCESS;

    // Load the hash table
    pSqpBlockTable = (TSQPBlock *)LoadSqpTable(ha, pHeader->dwBlockTablePos, pHeader->dwBlockTableSize * sizeof(TSQPBlock), MPQ_KEY_BLOCK_TABLE);
    if(pSqpBlockTable != NULL)
    {
        // Parse the entire hash table and convert it to MPQ hash table
        pSqpBlockEnd = pSqpBlockTable + pHeader->dwBlockTableSize;
        pMpqBlock = (TMPQBlock *)pSqpBlockTable;
        for(pSqpBlock = pSqpBlockTable; pSqpBlock < pSqpBlockEnd; pSqpBlock++, pMpqBlock++)
        {
            // Check for valid flags
            if(pSqpBlock->dwFlags & ~MPQ_FILE_VALID_FLAGS)
                nError = ERROR_FILE_CORRUPT;

            // Convert SQP block table entry to MPQ block table entry
            dwFlags = pSqpBlock->dwFlags;
            pMpqBlock->dwCSize = pSqpBlock->dwCSize;
            pMpqBlock->dwFSize = pSqpBlock->dwFSize;
            pMpqBlock->dwFlags = dwFlags;
        }

        // If an error occured, we need to free the hash table
        if(nError != ERROR_SUCCESS)
        {
            STORM_FREE(pSqpBlockTable);
            pSqpBlockTable = NULL;
        }
    }

    // Return the converted hash table (or NULL on failure)
    return (TMPQBlock *)pSqpBlockTable;
}

/*****************************************************************************/
/*                                                                           */
/*         Support for MPK file format (Longwu Online)                       */
/*                                                                           */
/*****************************************************************************/

#define MPK_FILE_UNKNOWN_0001   0x00000001      // Seems to be always present
#define MPK_FILE_UNKNOWN_0010   0x00000010      // Seems to be always present
#define MPK_FILE_COMPRESSED     0x00000100      // Indicates a compressed file
#define MPK_FILE_UNKNOWN_2000   0x00002000      // Seems to be always present
#define MPK_FILE_EXISTS         0x01000000      // Seems to be always present

typedef struct _TMPKHeader
{
    // The ID_MPK ('MPK\x1A') signature
    DWORD dwID;
    
    // Contains '2000'
    DWORD dwVersion;
    
    // 32-bit size of the archive
    DWORD dwArchiveSize;
    
    // Size of the archive header
    DWORD dwHeaderSize;

    DWORD dwHashTablePos;
    DWORD dwHashTableSize;
    DWORD dwBlockTablePos;
    DWORD dwBlockTableSize;
    DWORD dwUnknownPos;
    DWORD dwUnknownSize;
} TMPKHeader;


typedef struct _TMPKHash
{
    // The hash of the file path, using method A.
    DWORD dwName1;
    
    // The hash of the file path, using method B.
    DWORD dwName2;

    // The hash of the file path, using method C.
    DWORD dwName3;

    // If the hash table entry is valid, this is the index into the block table of the file.
    // Otherwise, one of the following two values:
    //  - FFFFFFFFh: Hash table entry is empty, and has always been empty.
    //               Terminates searches for a given file.
    //  - FFFFFFFEh: Hash table entry is empty, but was valid at some point (a deleted file).
    //               Does not terminate searches for a given file.
    DWORD dwBlockIndex;

} TMPKHash;

typedef struct _TMPKBlock
{
    DWORD  dwFlags;         // 0x1121 - Compressed , 0x1120 - Not compressed 
    DWORD  dwFilePos;       // Offset of the beginning of the file, relative to the beginning of the archive.
    DWORD  dwFSize;         // Uncompressed file size
    DWORD  dwCSize;         // Compressed file size
    DWORD  dwUnknown;       // 0x86364E6D
} TMPKBlock;

//-----------------------------------------------------------------------------
// Local variables - MPK file format

static const unsigned char MpkDecryptionKey[512] =
{
    0x60, 0x20, 0x29, 0xE1, 0x01, 0xCE, 0xAA, 0xFE, 0xA3, 0xAB, 0x8E, 0x30, 0xAF, 0x02, 0xD1, 0x7D,
    0x41, 0x24, 0x06, 0xBD, 0xAE, 0xBE, 0x43, 0xC3, 0xBA, 0xB7, 0x08, 0x13, 0x51, 0xCF, 0xF8, 0xF7,
    0x25, 0x42, 0xA5, 0x4A, 0xDA, 0x0F, 0x52, 0x1C, 0x90, 0x3B, 0x63, 0x49, 0x36, 0xF6, 0xDD, 0x1B,
    0xEA, 0x58, 0xD4, 0x40, 0x70, 0x61, 0x55, 0x09, 0xCD, 0x0B, 0xA2, 0x4B, 0x68, 0x2C, 0x8A, 0xF1,
    0x3C, 0x3A, 0x65, 0xBB, 0xA1, 0xA8, 0x23, 0x97, 0xFD, 0x15, 0x00, 0x94, 0x88, 0x33, 0x59, 0xE9,
    0xFB, 0x69, 0x21, 0xEF, 0x85, 0x5B, 0x57, 0x6C, 0xFA, 0xB5, 0xEE, 0xB8, 0x71, 0xDC, 0xB1, 0x38,
    0x0C, 0x0A, 0x5C, 0x56, 0xC9, 0xB4, 0x84, 0x17, 0x1E, 0xE5, 0xD3, 0x5A, 0xCC, 0xFC, 0x11, 0x86,
    0x7F, 0x45, 0x4F, 0x54, 0xC8, 0x8D, 0x73, 0x89, 0x79, 0x5D, 0xB3, 0xBF, 0xB9, 0xE3, 0x93, 0xE4,
    0x6F, 0x35, 0x2D, 0x46, 0xF2, 0x76, 0xC5, 0x7E, 0xE2, 0xA4, 0xE6, 0xD9, 0x6E, 0x48, 0x34, 0x2B,
    0xC6, 0x5F, 0xBC, 0xA0, 0x6D, 0x0D, 0x47, 0x6B, 0x95, 0x96, 0x92, 0x91, 0xB2, 0x27, 0xEB, 0x9E,
    0xEC, 0x8F, 0xDF, 0x9C, 0x74, 0x99, 0x64, 0xF5, 0xFF, 0x28, 0xB6, 0x37, 0xF3, 0x7C, 0x81, 0x03,
    0x44, 0x62, 0x1F, 0xDB, 0x04, 0x7B, 0xB0, 0x9B, 0x31, 0xA7, 0xDE, 0x78, 0x9F, 0xAD, 0x0E, 0x3F,
    0x3E, 0x4D, 0xC7, 0xD7, 0x39, 0x19, 0x5E, 0xC2, 0xD0, 0xAC, 0xE8, 0x1A, 0x87, 0x8B, 0x07, 0x05,
    0x22, 0xED, 0x72, 0x2E, 0x1D, 0xC1, 0xA9, 0xD6, 0xE0, 0x83, 0xD5, 0xD8, 0xCB, 0x80, 0xF0, 0x66,
    0x7A, 0x9D, 0x50, 0xF9, 0x10, 0x4E, 0x16, 0x14, 0x77, 0x75, 0x6A, 0x67, 0xD2, 0xC0, 0xA6, 0xC4,
    0x53, 0x8C, 0x32, 0xCA, 0x82, 0x2A, 0x18, 0x9A, 0xF4, 0x4C, 0x3D, 0x26, 0x12, 0xE7, 0x98, 0x2F,
    0x4A, 0x04, 0x0D, 0xAF, 0xB4, 0xCF, 0x12, 0xCE, 0x1A, 0x37, 0x61, 0x39, 0x60, 0x95, 0xBE, 0x25,
    0xE4, 0x6E, 0xFC, 0x1B, 0xE7, 0x49, 0xE6, 0x67, 0xF6, 0xC5, 0xCB, 0x2F, 0x27, 0xD4, 0x68, 0xB2,
    0x01, 0x52, 0xD0, 0x46, 0x11, 0x20, 0xFB, 0x9D, 0xA9, 0x02, 0xF5, 0x8F, 0x3D, 0x82, 0xD3, 0xFF,
    0x0B, 0xB8, 0xF2, 0x4D, 0x8E, 0x81, 0x2C, 0xAB, 0x5F, 0xC4, 0x41, 0x29, 0x40, 0xFA, 0xC0, 0xBF,
    0x33, 0x10, 0x21, 0x16, 0xB0, 0x71, 0x83, 0x96, 0x8D, 0x2B, 0x23, 0x3B, 0xF9, 0xC1, 0xE5, 0x72,
    0xE2, 0x1C, 0x26, 0xF0, 0x73, 0x36, 0x63, 0x56, 0x31, 0x4E, 0x6B, 0x55, 0x62, 0x79, 0xC6, 0x91,
    0x00, 0x35, 0xB1, 0x2A, 0xA6, 0x42, 0xDF, 0xEB, 0x3C, 0x51, 0xEA, 0x97, 0x57, 0x94, 0x8C, 0x80,
    0x34, 0x5C, 0xD2, 0x76, 0xA4, 0xE9, 0x85, 0xE8, 0xBB, 0x78, 0xE0, 0xB5, 0xAD, 0x0F, 0x87, 0x70,
    0xDD, 0xAE, 0xF4, 0xD9, 0x66, 0x54, 0x6F, 0xCC, 0x4C, 0x77, 0x3E, 0xCD, 0xF1, 0x75, 0x0A, 0xA1,
    0x28, 0x9B, 0x9A, 0x7E, 0x4B, 0x98, 0x99, 0x47, 0xFE, 0xA5, 0xF7, 0xB7, 0xA3, 0xE1, 0x9F, 0xBC,
    0x93, 0x44, 0x3A, 0x08, 0x89, 0x22, 0xEE, 0xB9, 0x45, 0xD6, 0x06, 0x09, 0xC9, 0xBD, 0x14, 0x0C,
    0xB6, 0x5E, 0x9C, 0x7A, 0x65, 0x59, 0xAA, 0x19, 0x5B, 0x7C, 0x18, 0x43, 0x92, 0x13, 0x15, 0x7B,
    0xED, 0xD5, 0xC7, 0x17, 0xEF, 0x86, 0x90, 0xC2, 0x74, 0x64, 0xF3, 0xDC, 0x6C, 0x38, 0x05, 0x1D,
    0xC8, 0x0E, 0xEC, 0x6A, 0x32, 0xDA, 0xD7, 0xC3, 0xDB, 0x8B, 0x24, 0xB3, 0x5D, 0x2E, 0xBA, 0xA2,
    0xD8, 0x03, 0x88, 0x7D, 0x7F, 0x69, 0x8A, 0xFD, 0xCA, 0x4F, 0x30, 0x9E, 0xA0, 0xD1, 0x5A, 0x53,
    0xDE, 0x3F, 0x84, 0xAC, 0xF8, 0xA7, 0x2D, 0x1F, 0x1E, 0xE3, 0x58, 0x50, 0x6D, 0x48, 0x07, 0xA8
};

//-----------------------------------------------------------------------------
// Functions - MPK file format

// This function converts MPK file header into MPQ file header
int ConvertMpkHeaderToFormat4(
    TMPQArchive * ha,
    ULONGLONG FileSize,
    DWORD dwFlags)
{
    TMPKHeader * pMpkHeader = (TMPKHeader *)ha->HeaderData;
    TMPQHeader Header;

    // Can't open the archive with certain flags
    if(dwFlags & MPQ_OPEN_FORCE_MPQ_V1)
        return ERROR_FILE_CORRUPT;

    // Translate the MPK header into a MPQ header
    // Note: Hash table size and block table size are in bytes, not in entries
    memset(&Header, 0, sizeof(TMPQHeader));
    Header.dwID = BSWAP_INT32_UNSIGNED(pMpkHeader->dwID);
    Header.dwArchiveSize = BSWAP_INT32_UNSIGNED(pMpkHeader->dwArchiveSize);
    Header.dwHeaderSize = BSWAP_INT32_UNSIGNED(pMpkHeader->dwHeaderSize);
    Header.dwHashTablePos = BSWAP_INT32_UNSIGNED(pMpkHeader->dwHashTablePos);
    Header.dwHashTableSize = BSWAP_INT32_UNSIGNED(pMpkHeader->dwHashTableSize) / sizeof(TMPKHash);
    Header.dwBlockTablePos = BSWAP_INT32_UNSIGNED(pMpkHeader->dwBlockTablePos);
    Header.dwBlockTableSize = BSWAP_INT32_UNSIGNED(pMpkHeader->dwBlockTableSize) / sizeof(TMPKBlock);
//  Header.dwUnknownPos = BSWAP_INT32_UNSIGNED(pMpkHeader->dwUnknownPos);
//  Header.dwUnknownSize = BSWAP_INT32_UNSIGNED(pMpkHeader->dwUnknownSize);
    assert(Header.dwHeaderSize == sizeof(TMPKHeader));

    // Verify the MPK header
    if(Header.dwID == ID_MPK && Header.dwHeaderSize == sizeof(TMPKHeader) && Header.dwArchiveSize == (DWORD)FileSize)
    {
        // The header ID must be ID_MPQ
        Header.dwID = ID_MPQ;
        Header.wFormatVersion = MPQ_FORMAT_VERSION_1;
        Header.wSectorSize = 3;

        // Initialize the fields of 3.0 header
        Header.ArchiveSize64    = Header.dwArchiveSize;
        Header.HashTableSize64  = Header.dwHashTableSize * sizeof(TMPQHash);
        Header.BlockTableSize64 = Header.dwBlockTableSize * sizeof(TMPQBlock);

        // Copy the converted MPQ header back
        memcpy(ha->HeaderData, &Header, sizeof(TMPQHeader));

        // Mark this file as MPK file
        ha->pfnHashString = HashStringLower;
        ha->dwFlags |= MPQ_FLAG_READ_ONLY;
        ha->dwSubType = MPQ_SUBTYPE_MPK;
        return ERROR_SUCCESS;
    }
    return ERROR_FILE_CORRUPT;
}

// Attempts to search a free hash entry in the hash table being converted.
// The created hash table must always be of nonzero size,
// should have no duplicated items and no deleted entries
TMPQHash * FindFreeHashEntry(TMPQHash * pHashTable, DWORD dwHashTableSize, DWORD dwStartIndex)
{
    TMPQHash * pHash;
    DWORD dwIndex;

    // Set the initial index
    dwStartIndex = dwIndex = (dwStartIndex & (dwHashTableSize - 1));
    assert(dwHashTableSize != 0);

    // Search the hash table and return the found entries in the following priority:
    for(;;)
    {
        // We are not expecting to find matching entry in the hash table being built
        // We are not expecting to find deleted entry either
        pHash = pHashTable + dwIndex;

        // If we found a free entry, we need to stop searching
        if(pHash->dwBlockIndex == HASH_ENTRY_FREE)
            return pHash;

        // Move to the next hash entry.
        // If we reached the starting entry, it's failure.
        dwIndex = (dwIndex + 1) & (dwHashTableSize - 1);
        if(dwIndex == dwStartIndex)
            break;
    }

    // We haven't found anything
    assert(false);
    return NULL;
}

void DecryptMpkTable(void * pvMpkTable, size_t cbSize)
{
    LPBYTE pbMpkTable = (LPBYTE)pvMpkTable;

    for(size_t i = 0; i < cbSize; i++)
        pbMpkTable[i] = MpkDecryptionKey[pbMpkTable[i]];
}

void * LoadMpkTable(TMPQArchive * ha, DWORD dwByteOffset, DWORD cbTableSize)
{
    ULONGLONG ByteOffset;
    LPBYTE pbMpkTable = NULL;

    // Allocate space for the table
    pbMpkTable = STORM_ALLOC(BYTE, cbTableSize);
    if(pbMpkTable != NULL)
    {
        // Load and the MPK hash table
        ByteOffset = ha->MpqPos + dwByteOffset;
        if(FileStream_Read(ha->pStream, &ByteOffset, pbMpkTable, cbTableSize))
        {
            // Decrypt the table
            DecryptMpkTable(pbMpkTable, cbTableSize);
            return pbMpkTable;
        }

        // Free the MPK table
        STORM_FREE(pbMpkTable);
        pbMpkTable = NULL;
    }

    // Return the table
    return pbMpkTable;
}

TMPQHash * LoadMpkHashTable(TMPQArchive * ha)
{
    TMPQHeader * pHeader = ha->pHeader;
    TMPQHash * pHashTable = NULL;
    TMPKHash * pMpkHash;
    TMPQHash * pHash = NULL;
    DWORD dwHashTableSize = pHeader->dwHashTableSize;

    // MPKs use different hash table searching.
    // Instead of using MPQ_HASH_TABLE_INDEX hash as index,
    // they store the value directly in the hash table.
    // Also for faster searching, the hash table is sorted ascending by the value

    // Load and decrypt the MPK hash table.
    pMpkHash = (TMPKHash *)LoadMpkTable(ha, pHeader->dwHashTablePos, pHeader->dwHashTableSize * sizeof(TMPKHash));
    if(pMpkHash != NULL)
    {
        // Calculate the hash table size as if it was real MPQ hash table
        pHeader->dwHashTableSize = GetNearestPowerOfTwo(pHeader->dwHashTableSize);
        pHeader->HashTableSize64 = pHeader->dwHashTableSize * sizeof(TMPQHash);

        // Now allocate table that will serve like a true MPQ hash table,
        // so we translate the MPK hash table to MPQ hash table
        pHashTable = STORM_ALLOC(TMPQHash, pHeader->dwHashTableSize);
        if(pHashTable != NULL)
        {
            // Set the entire hash table to free
            memset(pHashTable, 0xFF, (size_t)pHeader->HashTableSize64);

            // Copy the MPK hash table into MPQ hash table
            for(DWORD i = 0; i < dwHashTableSize; i++)
            {
                // Finds the free hash entry in the hash table
                // We don't expect any errors here, because we are putting files to empty hash table
                pHash = FindFreeHashEntry(pHashTable, pHeader->dwHashTableSize, pMpkHash[i].dwName1);
                assert(pHash->dwBlockIndex == HASH_ENTRY_FREE);

                // Copy the MPK hash entry to the hash table
                pHash->dwBlockIndex = pMpkHash[i].dwBlockIndex;
                pHash->Platform = 0;
                pHash->lcLocale = 0;
                pHash->dwName1 = pMpkHash[i].dwName2;
                pHash->dwName2 = pMpkHash[i].dwName3;
            }
        }

        // Free the temporary hash table
        STORM_FREE(pMpkHash);
    }

    return pHashTable;
}

static DWORD ConvertMpkFlagsToMpqFlags(DWORD dwMpkFlags)
{
    DWORD dwMpqFlags = MPQ_FILE_EXISTS;

    // Check for flags that are always present
    assert((dwMpkFlags & MPK_FILE_UNKNOWN_0001) != 0);
    assert((dwMpkFlags & MPK_FILE_UNKNOWN_0010) != 0);
    assert((dwMpkFlags & MPK_FILE_UNKNOWN_2000) != 0);
    assert((dwMpkFlags & MPK_FILE_EXISTS) != 0);

    // Append the compressed flag
    dwMpqFlags |= (dwMpkFlags & MPK_FILE_COMPRESSED) ? MPQ_FILE_COMPRESS : 0;

    // All files in the MPQ seem to be single unit files
    dwMpqFlags |= MPQ_FILE_ENCRYPTED | MPQ_FILE_SINGLE_UNIT;

    return dwMpqFlags;
}

TMPQBlock * LoadMpkBlockTable(TMPQArchive * ha)
{
    TMPQHeader * pHeader = ha->pHeader;
    TMPKBlock * pMpkBlockTable;
    TMPKBlock * pMpkBlockEnd;
    TMPQBlock * pBlockTable = NULL;
    TMPKBlock * pMpkBlock;
    TMPQBlock * pMpqBlock;

    // Load and decrypt the MPK block table
    pMpkBlockTable = pMpkBlock = (TMPKBlock *)LoadMpkTable(ha, pHeader->dwBlockTablePos, pHeader->dwBlockTableSize * sizeof(TMPKBlock));
    if(pMpkBlockTable != NULL)
    {
        // Allocate buffer for MPQ-like block table
        pBlockTable = pMpqBlock = STORM_ALLOC(TMPQBlock, pHeader->dwBlockTableSize);
        if(pBlockTable != NULL)
        {
            // Convert the MPK block table to MPQ block table
            pMpkBlockEnd = pMpkBlockTable + pHeader->dwBlockTableSize;
            while(pMpkBlock < pMpkBlockEnd)
            {
                // Translate the MPK block table entry to MPQ block table entry
                pMpqBlock->dwFilePos = pMpkBlock->dwFilePos;
                pMpqBlock->dwCSize = pMpkBlock->dwCSize;
                pMpqBlock->dwFSize = pMpkBlock->dwFSize;
                pMpqBlock->dwFlags = ConvertMpkFlagsToMpqFlags(pMpkBlock->dwFlags);

                // Move both
                pMpkBlock++;
                pMpqBlock++;
            }
        }

        // Free the MPK block table
        STORM_FREE(pMpkBlockTable);
    }

    return pBlockTable;
}
