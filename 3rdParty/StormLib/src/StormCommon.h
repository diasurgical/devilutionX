/*****************************************************************************/
/* SCommon.h                              Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Common functions for encryption/decryption from Storm.dll. Included by    */
/* SFile*** functions, do not include and do not use this file directly      */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 24.03.03  1.00  Lad  The first version of SFileCommon.h                   */
/* 12.06.04  1.00  Lad  Renamed to SCommon.h                                 */
/* 06.09.10  1.00  Lad  Renamed to StormCommon.h                             */
/*****************************************************************************/

#ifndef __STORMCOMMON_H__
#define __STORMCOMMON_H__

//-----------------------------------------------------------------------------
// Compression support

// Include functions from Pkware Data Compression Library
#include "../../PKWare/pkware.h"

// Include functions from Huffmann compression
#include "huffman/huff.h"

// Include functions from IMA ADPCM compression
#include "adpcm/adpcm.h"

#ifdef FULL
// Include functions from SPARSE compression
#include "sparse/sparse.h"

// Include functions from LZMA compression
#include "lzma/C/LzmaEnc.h"
#include "lzma/C/LzmaDec.h"

// Include functions from zlib
#ifndef __SYS_ZLIB
  #include "zlib/zlib.h"
#else
  #include <zlib.h>
#endif

// Include functions from bzlib
#ifndef __SYS_BZLIB
  #include "bzip2/bzlib.h"
#else
  #include <bzlib.h>
#endif

//-----------------------------------------------------------------------------
// Cryptography support

// Headers from LibTomCrypt
#include "libtomcrypt/src/headers/tomcrypt.h"

// For HashStringJenkins
#include "jenkins/lookup.h"
#endif // FULL

//-----------------------------------------------------------------------------
// StormLib private defines

#define ID_MPQ_FILE            0x46494c45     // Used internally for checking TMPQFile ('FILE')

// Prevent problems with CRT "min" and "max" functions,
// as they are not defined on all platforms
#define STORMLIB_MIN(a, b) ((a < b) ? a : b)
#define STORMLIB_MAX(a, b) ((a > b) ? a : b)
#define STORMLIB_UNUSED(p) ((void)(p))

// Macro for building 64-bit file offset from two 32-bit
#define MAKE_OFFSET64(hi, lo)      (((ULONGLONG)hi << 32) | (ULONGLONG)lo)

//-----------------------------------------------------------------------------
// MTYPE definition - specifies what kind of MPQ is the map type

typedef enum _MTYPE
{
    MapTypeNotChecked,                  // The map type was not checked yet
    MapTypeNotRecognized,               // The file does not seems to be a map
    MapTypeAviFile,                     // The file is actually an AVI file (Warcraft III cinematics)
    MapTypeWarcraft3,                   // The file is a Warcraft III map
    MapTypeStarcraft2                   // The file is a Starcraft II map
} MTYPE, *PMTYPE;

//-----------------------------------------------------------------------------
// MPQ signature information

// Size of each signature type
#define MPQ_WEAK_SIGNATURE_SIZE                 64
#define MPQ_STRONG_SIGNATURE_SIZE              256
#define MPQ_STRONG_SIGNATURE_ID         0x5349474E      // ID of the strong signature ("NGIS")
#define MPQ_SIGNATURE_FILE_SIZE (MPQ_WEAK_SIGNATURE_SIZE + 8)

// MPQ signature info
typedef struct _MPQ_SIGNATURE_INFO
{
    ULONGLONG BeginMpqData;                     // File offset where the hashing starts
    ULONGLONG BeginExclude;                     // Begin of the excluded area (used for (signature) file)
    ULONGLONG EndExclude;                       // End of the excluded area (used for (signature) file)
    ULONGLONG EndMpqData;                       // File offset where the hashing ends
    ULONGLONG EndOfFile;                        // Size of the entire file
    BYTE  Signature[MPQ_STRONG_SIGNATURE_SIZE + 0x10];
    DWORD cbSignatureSize;                      // Length of the signature
    DWORD SignatureTypes;                       // See SIGNATURE_TYPE_XXX

} MPQ_SIGNATURE_INFO, *PMPQ_SIGNATURE_INFO;

//-----------------------------------------------------------------------------
// Memory management
//
// We use our own macros for allocating/freeing memory. If you want
// to redefine them, please keep the following rules:
//
//  - The memory allocation must return NULL if not enough memory
//    (i.e not to throw exception)
//  - The allocating function does not need to fill the allocated buffer with zeros
//  - Memory freeing function doesn't have to test the pointer to NULL
//

//#if defined(_MSC_VER) && defined(_DEBUG)
//
//#define STORM_ALLOC(type, nitems)        (type *)HeapAlloc(GetProcessHeap(), 0, ((nitems) * sizeof(type)))
//#define STORM_REALLOC(type, ptr, nitems) (type *)HeapReAlloc(GetProcessHeap(), 0, ptr, ((nitems) * sizeof(type)))
//#define STORM_FREE(ptr)                  HeapFree(GetProcessHeap(), 0, ptr)
//
//#else

#define STORM_ALLOC(type, nitems)        (type *)malloc((nitems) * sizeof(type))
#define STORM_REALLOC(type, ptr, nitems) (type *)realloc(ptr, ((nitems) * sizeof(type)))
#define STORM_FREE(ptr)                  free(ptr)

//#endif

//-----------------------------------------------------------------------------
// StormLib internal global variables

extern DWORD g_dwMpqSignature;                  // Marker for MPQ header
extern DWORD g_dwHashTableKey;                  // Key for hash table
extern DWORD g_dwBlockTableKey;                 // Key for block table
extern LCID  g_lcFileLocale;                    // Preferred file locale

//-----------------------------------------------------------------------------
// Conversion to uppercase/lowercase (and "/" to "\")

extern unsigned char AsciiToLowerTable[256];
extern unsigned char AsciiToUpperTable[256];

//-----------------------------------------------------------------------------
// Safe string functions

template <typename XCHAR, typename XINT>
XCHAR * IntToString(XCHAR * szBuffer, size_t cchMaxChars, XINT nValue, size_t nDigitCount = 0)
{
    XCHAR * szBufferEnd = szBuffer + cchMaxChars - 1;
    XCHAR szNumberRev[0x20];
    size_t nLength = 0;

    // Always put the first digit
    szNumberRev[nLength++] = (XCHAR)(nValue % 10) + '0';
    nValue /= 10;

    // Continue as long as we have non-zero
    while(nValue != 0)
    {
        szNumberRev[nLength++] = (XCHAR)(nValue % 10) + '0';
        nValue /= 10;
    }

    // Fill zeros, if needed
    while(szBuffer < szBufferEnd && nLength < nDigitCount)
    {
        *szBuffer++ = '0';
        nDigitCount--;
    }

    // Fill the buffer
    while(szBuffer < szBufferEnd && nLength > 0)
    {
        nLength--;
        *szBuffer++ = szNumberRev[nLength];
    }

    // Terminate the number with zeros
    szBuffer[0] = 0;
    return szBuffer;
}

char * StringCopy(char * szTarget, size_t cchTarget, const char * szSource);
void StringCat(char * szTarget, size_t cchTargetMax, const char * szSource);
void StringCreatePseudoFileName(char * szBuffer, size_t cchMaxChars, unsigned int nIndex, const char * szExtension);

#ifdef _UNICODE
void StringCopy(TCHAR * szTarget, size_t cchTarget, const char * szSource);
void StringCopy(char * szTarget, size_t cchTarget, const TCHAR * szSource);
void StringCopy(TCHAR * szTarget, size_t cchTarget, const TCHAR * szSource);
void StringCat(TCHAR * szTarget, size_t cchTargetMax, const TCHAR * szSource);
#endif

//-----------------------------------------------------------------------------
// Encryption and decryption functions

#define MPQ_HASH_TABLE_INDEX    0x000
#define MPQ_HASH_NAME_A         0x100
#define MPQ_HASH_NAME_B         0x200
#define MPQ_HASH_FILE_KEY       0x300
#define MPQ_HASH_KEY2_MIX       0x400

DWORD HashString(const char * szFileName, DWORD dwHashType);
DWORD HashStringSlash(const char * szFileName, DWORD dwHashType);
DWORD HashStringLower(const char * szFileName, DWORD dwHashType);

void  InitializeMpqCryptography();

DWORD GetNearestPowerOfTwo(DWORD dwFileCount);

bool IsPseudoFileName(const char * szFileName, LPDWORD pdwFileIndex);
#ifdef FULL
ULONGLONG HashStringJenkins(const char * szFileName);
#endif // FULL

DWORD GetDefaultSpecialFileFlags(DWORD dwFileSize, USHORT wFormatVersion);

void  EncryptMpqBlock(void * pvDataBlock, DWORD dwLength, DWORD dwKey);
void  DecryptMpqBlock(void * pvDataBlock, DWORD dwLength, DWORD dwKey);

DWORD DetectFileKeyBySectorSize(LPDWORD EncryptedData, DWORD dwSectorSize, DWORD dwSectorOffsLen);
DWORD DetectFileKeyByContent(void * pvEncryptedData, DWORD dwSectorSize, DWORD dwFileSize);
DWORD DecryptFileKey(const char * szFileName, ULONGLONG MpqPos, DWORD dwFileSize, DWORD dwFlags);

bool IsValidMD5(LPBYTE pbMd5);
bool IsValidSignature(LPBYTE pbSignature);
#ifdef FULL
bool VerifyDataBlockHash(void * pvDataBlock, DWORD cbDataBlock, LPBYTE expected_md5);
void CalculateDataBlockHash(void * pvDataBlock, DWORD cbDataBlock, LPBYTE md5_hash);
#endif // FULL

//-----------------------------------------------------------------------------
// Handle validation functions

TMPQArchive * IsValidMpqHandle(HANDLE hMpq);
TMPQFile * IsValidFileHandle(HANDLE hFile);

//-----------------------------------------------------------------------------
// Support for MPQ file tables

ULONGLONG FileOffsetFromMpqOffset(TMPQArchive * ha, ULONGLONG MpqOffset);
ULONGLONG CalculateRawSectorOffset(TMPQFile * hf, DWORD dwSectorOffset);

int ConvertMpqHeaderToFormat4(TMPQArchive * ha, ULONGLONG MpqOffset, ULONGLONG FileSize, DWORD dwFlags, MTYPE MapType);

bool IsValidHashEntry(TMPQArchive * ha, TMPQHash * pHash);

TMPQHash * FindFreeHashEntry(TMPQArchive * ha, DWORD dwStartIndex, DWORD dwName1, DWORD dwName2, LCID lcLocale);
TMPQHash * GetFirstHashEntry(TMPQArchive * ha, const char * szFileName);
TMPQHash * GetNextHashEntry(TMPQArchive * ha, TMPQHash * pFirstHash, TMPQHash * pPrevHash);
TMPQHash * AllocateHashEntry(TMPQArchive * ha, TFileEntry * pFileEntry, LCID lcLocale);

TMPQExtHeader * LoadExtTable(TMPQArchive * ha, ULONGLONG ByteOffset, size_t Size, DWORD dwSignature, DWORD dwKey);
TMPQHetTable * LoadHetTable(TMPQArchive * ha);
TMPQBetTable * LoadBetTable(TMPQArchive * ha);

TMPQBlock * LoadBlockTable(TMPQArchive * ha, bool bDontFixEntries = false);
TMPQBlock * TranslateBlockTable(TMPQArchive * ha, ULONGLONG * pcbTableSize, bool * pbNeedHiBlockTable);

ULONGLONG FindFreeMpqSpace(TMPQArchive * ha);

// Functions that load the HET and BET tables
int  CreateHashTable(TMPQArchive * ha, DWORD dwHashTableSize);
int  LoadAnyHashTable(TMPQArchive * ha);
int  BuildFileTable(TMPQArchive * ha);
#ifdef FULL
int  DefragmentFileTable(TMPQArchive * ha);

int  CreateFileTable(TMPQArchive * ha, DWORD dwFileTableSize);
int  RebuildHetTable(TMPQArchive * ha);
int  RebuildFileTable(TMPQArchive * ha, DWORD dwNewHashTableSize);
int  SaveMPQTables(TMPQArchive * ha);

TMPQHetTable * CreateHetTable(DWORD dwEntryCount, DWORD dwTotalCount, DWORD dwHashBitSize, LPBYTE pbSrcData);
void FreeHetTable(TMPQHetTable * pHetTable);

TMPQBetTable * CreateBetTable(DWORD dwMaxFileCount);
void FreeBetTable(TMPQBetTable * pBetTable);
#endif // FULL

// Functions for finding files in the file table
TFileEntry * GetFileEntryLocale2(TMPQArchive * ha, const char * szFileName, LCID lcLocale, LPDWORD PtrHashIndex);
TFileEntry * GetFileEntryLocale(TMPQArchive * ha, const char * szFileName, LCID lcLocale);
TFileEntry * GetFileEntryExact(TMPQArchive * ha, const char * szFileName, LCID lcLocale, LPDWORD PtrHashIndex);

// Allocates file name in the file entry
void AllocateFileName(TMPQArchive * ha, TFileEntry * pFileEntry, const char * szFileName);

// Allocates new file entry in the MPQ tables. Reuses existing, if possible
TFileEntry * AllocateFileEntry(TMPQArchive * ha, const char * szFileName, LCID lcLocale, LPDWORD PtrHashIndex);
int  RenameFileEntry(TMPQArchive * ha, TMPQFile * hf, const char * szNewFileName);
int  DeleteFileEntry(TMPQArchive * ha, TMPQFile * hf);

// Invalidates entries for (listfile) and (attributes)
void InvalidateInternalFiles(TMPQArchive * ha);

// Retrieves information about the strong signature
bool QueryMpqSignatureInfo(TMPQArchive * ha, PMPQ_SIGNATURE_INFO pSignatureInfo);

//-----------------------------------------------------------------------------
// Support for alternate file formats (SBaseSubTypes.cpp)

int ConvertSqpHeaderToFormat4(TMPQArchive * ha, ULONGLONG FileSize, DWORD dwFlags);
TMPQHash * LoadSqpHashTable(TMPQArchive * ha);
TMPQBlock * LoadSqpBlockTable(TMPQArchive * ha);

int ConvertMpkHeaderToFormat4(TMPQArchive * ha, ULONGLONG FileSize, DWORD dwFlags);
void DecryptMpkTable(void * pvMpkTable, size_t cbSize);
TMPQHash * LoadMpkHashTable(TMPQArchive * ha);
TMPQBlock * LoadMpkBlockTable(TMPQArchive * ha);
#ifdef FULL
int SCompDecompressMpk(void * pvOutBuffer, int * pcbOutBuffer, void * pvInBuffer, int cbInBuffer);
#endif // FULL

//-----------------------------------------------------------------------------
// Common functions - MPQ File

TMPQFile * CreateFileHandle(TMPQArchive * ha, TFileEntry * pFileEntry);
TMPQFile * CreateWritableHandle(TMPQArchive * ha, DWORD dwFileSize);
void * LoadMpqTable(TMPQArchive * ha, ULONGLONG ByteOffset, LPBYTE pbTableHash, DWORD dwCompressedSize, DWORD dwRealSize, DWORD dwKey, bool * pbTableIsCut);
int  AllocateSectorBuffer(TMPQFile * hf);
int  AllocatePatchInfo(TMPQFile * hf, bool bLoadFromFile);
int  AllocateSectorOffsets(TMPQFile * hf, bool bLoadFromFile);
int  AllocateSectorChecksums(TMPQFile * hf, bool bLoadFromFile);
int  WritePatchInfo(TMPQFile * hf);
int  WriteSectorOffsets(TMPQFile * hf);
int  WriteSectorChecksums(TMPQFile * hf);
#ifdef FULL
int  WriteMemDataMD5(TFileStream * pStream, ULONGLONG RawDataOffs, void * pvRawData, DWORD dwRawDataSize, DWORD dwChunkSize, LPDWORD pcbTotalSize);
int  WriteMpqDataMD5(TFileStream * pStream, ULONGLONG RawDataOffs, DWORD dwRawDataSize, DWORD dwChunkSize);
#endif // FULL
void FreeFileHandle(TMPQFile *& hf);
void FreeArchiveHandle(TMPQArchive *& ha);

//-----------------------------------------------------------------------------
// Patch functions

// Structure used for the patching process
typedef struct _TMPQPatcher
{
    BYTE this_md5[MD5_DIGEST_SIZE];             // MD5 of the current file state
    LPBYTE pbFileData1;                         // Primary working buffer
    LPBYTE pbFileData2;                         // Secondary working buffer
    DWORD cbMaxFileData;                        // Maximum allowed size of the patch data
    DWORD cbFileData;                           // Current size of the result data
    DWORD nCounter;                             // Counter of the patch process

} TMPQPatcher;

bool IsIncrementalPatchFile(const void * pvData, DWORD cbData, LPDWORD pdwPatchedFileSize);
int Patch_InitPatcher(TMPQPatcher * pPatcher, TMPQFile * hf);
int Patch_Process(TMPQPatcher * pPatcher, TMPQFile * hf);
void Patch_Finalize(TMPQPatcher * pPatcher);

//-----------------------------------------------------------------------------
// Utility functions

bool IsInternalMpqFileName(const char * szFileName);

template <typename XCHAR>
const XCHAR * GetPlainFileName(const XCHAR * szFileName)
{
    const XCHAR * szPlainName = szFileName;

    while(*szFileName != 0)
    {
        if(*szFileName == '\\' || *szFileName == '/')
            szPlainName = szFileName + 1;
        szFileName++;
    }

    return szPlainName;
}

//-----------------------------------------------------------------------------
// Internal support for MPQ modifications

int SFileAddFile_Init(
    TMPQArchive * ha,
    const char * szArchivedName,
    ULONGLONG ft,
    DWORD dwFileSize,
    LCID lcLocale,
    DWORD dwFlags,
    TMPQFile ** phf
    );

int SFileAddFile_Init(
    TMPQArchive * ha,
    TMPQFile * hfSrc,
    TMPQFile ** phf
    );

int SFileAddFile_Write(
    TMPQFile * hf,
    const void * pvData,
    DWORD dwSize,
    DWORD dwCompression
    );

int SFileAddFile_Finish(
    TMPQFile * hf
    );

//-----------------------------------------------------------------------------
// Attributes support

int  SAttrLoadAttributes(TMPQArchive * ha);
int  SAttrFileSaveToMpq(TMPQArchive * ha);

//-----------------------------------------------------------------------------
// Listfile functions

int  SListFileSaveToMpq(TMPQArchive * ha);

//-----------------------------------------------------------------------------
// Weak signature support

int SSignFileCreate(TMPQArchive * ha);
int SSignFileFinish(TMPQArchive * ha);

//-----------------------------------------------------------------------------
// Dump data support

#ifdef __STORMLIB_DUMP_DATA__

void DumpMpqHeader(TMPQHeader * pHeader);
void DumpHashTable(TMPQHash * pHashTable, DWORD dwHashTableSize);
void DumpHetAndBetTable(TMPQHetTable * pHetTable, TMPQBetTable * pBetTable);
void DumpFileTable(TFileEntry * pFileTable, DWORD dwFileTableSize);

#else

#define DumpMpqHeader(h)            /* */
#define DumpHashTable(t, s)         /* */
#define DumpHetAndBetTable(t, s)    /* */
#define DumpFileTable(t, s)         /* */

#endif

#endif // __STORMCOMMON_H__

