/*****************************************************************************/
/* SFileGetFileInfo.cpp                   Copyright (c) Ladislav Zezula 2013 */
/*---------------------------------------------------------------------------*/
/* Description:                                                              */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 30.11.13  1.00  Lad  The first version of SFileGetFileInfo.cpp            */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "StormCommon.h"

//-----------------------------------------------------------------------------
// Local functions

static DWORD GetMpqFileCount(TMPQArchive * ha)
{
    TFileEntry * pFileTableEnd;
    TFileEntry * pFileEntry;
    DWORD dwFileCount = 0;

    // Go through all open MPQs, including patches
    while(ha != NULL)
    {
        // Only count files that are not patch files
        pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
        for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++)
        {
            // If the file is patch file and this is not primary archive, skip it
            // BUGBUG: This errorneously counts non-patch files that are in both
            // base MPQ and in patches, and increases the number of files by cca 50%
            if((pFileEntry->dwFlags & (MPQ_FILE_EXISTS | MPQ_FILE_PATCH_FILE)) == MPQ_FILE_EXISTS)
                dwFileCount++;
        }

        // Move to the next patch archive
        ha = ha->haPatch;
    }

    return dwFileCount;
}

static bool GetInfo_ReturnError(DWORD dwErrCode)
{
    SetLastError(dwErrCode);
    return false;
}

static bool GetInfo_BufferCheck(void * pvFileInfo, DWORD cbFileInfo, DWORD cbData, LPDWORD pcbLengthNeeded)
{
    // Give the length needed to store the info
    if(pcbLengthNeeded != NULL)
        pcbLengthNeeded[0] = cbData;

    // Check for sufficient buffer
    if(cbData > cbFileInfo)
        return GetInfo_ReturnError(ERROR_INSUFFICIENT_BUFFER);

    // If the buffer size is sufficient, check for valid user buffer
    if(pvFileInfo == NULL)
        return GetInfo_ReturnError(ERROR_INVALID_PARAMETER);

    // Buffers and sizes are OK, we are ready to proceed file copying
    return true;
}

static bool GetInfo(void * pvFileInfo, DWORD cbFileInfo, const void * pvData, DWORD cbData, LPDWORD pcbLengthNeeded)
{
    // Verify buffer pointer and buffer size
    if(!GetInfo_BufferCheck(pvFileInfo, cbFileInfo, cbData, pcbLengthNeeded))
        return false;

    // Copy the data to the caller-supplied buffer
    memcpy(pvFileInfo, pvData, cbData);
    return true;
}

static bool GetInfo_Allocated(void * pvFileInfo, DWORD cbFileInfo, void * pvData, DWORD cbData, LPDWORD pcbLengthNeeded)
{
    bool bResult;

    // Verify buffer pointer and buffer size
    if((bResult = GetInfo_BufferCheck(pvFileInfo, cbFileInfo, cbData, pcbLengthNeeded)) != false)
        memcpy(pvFileInfo, pvData, cbData);

    // Copy the data to the user buffer
    STORM_FREE(pvData);
    return bResult;
}

static bool GetInfo_TablePointer(void * pvFileInfo, DWORD cbFileInfo, void * pvTablePointer, SFileInfoClass InfoClass, LPDWORD pcbLengthNeeded)
{
    // Verify buffer pointer and buffer size
    if(!GetInfo_BufferCheck(pvFileInfo, cbFileInfo, sizeof(void *), pcbLengthNeeded))
    {
#ifdef FULL
        SFileFreeFileInfo(pvTablePointer, InfoClass);
#endif // FULL
        return false;
    }

    // The user buffer receives pointer to the table.
    // When done, the caller needs to call SFileFreeFileInfo on it
    *(void **)pvFileInfo = pvTablePointer;
    return true;
}

static bool GetInfo_ReadFromFile(void * pvFileInfo, DWORD cbFileInfo, TFileStream * pStream, ULONGLONG ByteOffset, DWORD cbData, LPDWORD pcbLengthNeeded)
{
    // Verify buffer pointer and buffer size
    if(!GetInfo_BufferCheck(pvFileInfo, cbFileInfo, cbData, pcbLengthNeeded))
        return false;

    return FileStream_Read(pStream, &ByteOffset, pvFileInfo, cbData);
}

static bool GetInfo_FileEntry(void * pvFileInfo, DWORD cbFileInfo, TFileEntry * pFileEntry, LPDWORD pcbLengthNeeded)
{
    LPBYTE pbFileInfo = (LPBYTE)pvFileInfo;
    DWORD cbSrcFileInfo = sizeof(TFileEntry);
    DWORD cbFileName = 1;

    // The file name belongs to the file entry
    if(pFileEntry->szFileName)
        cbFileName = (DWORD)strlen(pFileEntry->szFileName) + 1;
    cbSrcFileInfo += cbFileName;

    // Verify buffer pointer and buffer size
    if(!GetInfo_BufferCheck(pvFileInfo, cbFileInfo, cbSrcFileInfo, pcbLengthNeeded))
        return false;

    // Copy the file entry
    memcpy(pbFileInfo, pFileEntry, sizeof(TFileEntry));
    pbFileInfo += sizeof(TFileEntry);
    pbFileInfo[0] = 0;

    // Copy the file name
    if(pFileEntry->szFileName)
        memcpy(pbFileInfo, pFileEntry->szFileName, cbFileName);
    return true;
}

static bool GetInfo_PatchChain(TMPQFile * hf, void * pvFileInfo, DWORD cbFileInfo, LPDWORD pcbLengthNeeded)
{
    TMPQFile * hfTemp;
    LPCTSTR szPatchName;
    LPTSTR szFileInfo = (LPTSTR)pvFileInfo;
    size_t cchCharsNeeded = 1;
    size_t nLength;

    // Patch chain is only supported on MPQ files. Local files are not supported.
    if(hf->pStream != NULL)
        return GetInfo_ReturnError(ERROR_INVALID_PARAMETER);

    // Calculate the necessary length of the multi-string
    for(hfTemp = hf; hfTemp != NULL; hfTemp = hfTemp->hfPatch)
        cchCharsNeeded += _tcslen(FileStream_GetFileName(hfTemp->ha->pStream)) + 1;

    // Verify whether the caller gave us valid buffer with enough size
    if(!GetInfo_BufferCheck(pvFileInfo, cbFileInfo, (DWORD)(cchCharsNeeded * sizeof(TCHAR)), pcbLengthNeeded))
        return false;

    // Copy each patch name
    for(hfTemp = hf; hfTemp != NULL; hfTemp = hfTemp->hfPatch)
    {
        // Get the file name and its length
        szPatchName = FileStream_GetFileName(hfTemp->ha->pStream);
        nLength = _tcslen(szPatchName) + 1;

        // Copy the file name
        memcpy(szFileInfo, szPatchName, nLength * sizeof(TCHAR));
        szFileInfo += nLength;
    }

    // Make it multi-string
    szFileInfo[0] = 0;
    return true;
}

//-----------------------------------------------------------------------------
// Retrieves an information about an archive or about a file within the archive
//
//  hMpqOrFile - Handle to an MPQ archive or to a file
//  InfoClass  - Information to obtain
//  pvFileInfo - Pointer to buffer to store the information
//  cbFileInfo - Size of the buffer pointed by pvFileInfo
//  pcbLengthNeeded - Receives number of bytes necessary to store the information

bool WINAPI SFileGetFileInfo(
    HANDLE hMpqOrFile,
    SFileInfoClass InfoClass,
    void * pvFileInfo,
    DWORD cbFileInfo,
    LPDWORD pcbLengthNeeded)
{
    MPQ_SIGNATURE_INFO SignatureInfo;
    const TCHAR * szSrcFileInfo;
    TMPQArchive * ha = NULL;
    TFileEntry * pFileEntry = NULL;
    TMPQHeader * pHeader = NULL;
    ULONGLONG Int64Value = 0;
    TMPQFile * hf = NULL;
    void * pvSrcFileInfo = NULL;
    DWORD cbSrcFileInfo = 0;
    DWORD dwInt32Value = 0;

    // Validate archive/file handle
    if((int)InfoClass <= (int)SFileMpqFlags)
    {
        if((ha = IsValidMpqHandle(hMpqOrFile)) == NULL)
            return GetInfo_ReturnError(ERROR_INVALID_HANDLE);
        pHeader = ha->pHeader;
    }
    else
    {
        if((hf = IsValidFileHandle(hMpqOrFile)) == NULL)
            return GetInfo_ReturnError(ERROR_INVALID_HANDLE);
        pFileEntry = hf->pFileEntry;
    }

    // Return info-class-specific data
    switch(InfoClass)
    {
        case SFileMpqFileName:
            szSrcFileInfo = FileStream_GetFileName(ha->pStream);
            cbSrcFileInfo = (DWORD)((_tcslen(szSrcFileInfo) + 1) * sizeof(TCHAR));
            return GetInfo(pvFileInfo, cbFileInfo, szSrcFileInfo, cbSrcFileInfo, pcbLengthNeeded);

        case SFileMpqStreamBitmap:
            return FileStream_GetBitmap(ha->pStream, pvFileInfo, cbFileInfo, pcbLengthNeeded);

        case SFileMpqUserDataOffset:
            return GetInfo(pvFileInfo, cbFileInfo, &ha->UserDataPos, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqUserDataHeader:
            return GetInfo_ReadFromFile(pvFileInfo, cbFileInfo, ha->pStream, ha->UserDataPos, sizeof(TMPQUserData), pcbLengthNeeded);

        case SFileMpqUserData:
            return GetInfo_ReadFromFile(pvFileInfo, cbFileInfo, ha->pStream, ha->UserDataPos + sizeof(TMPQUserData), ha->pUserData->dwHeaderOffs - sizeof(TMPQUserData), pcbLengthNeeded);

        case SFileMpqHeaderOffset:
            return GetInfo(pvFileInfo, cbFileInfo, &ha->MpqPos, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqHeaderSize:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->dwHeaderSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqHeader:
            return GetInfo_ReadFromFile(pvFileInfo, cbFileInfo, ha->pStream, ha->MpqPos, pHeader->dwHeaderSize, pcbLengthNeeded);

#ifdef FULL
        case SFileMpqHetTableOffset:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->HetTablePos64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqHetTableSize:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->HetTableSize64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqHetHeader:
            pvSrcFileInfo = LoadExtTable(ha, pHeader->HetTablePos64, (size_t)pHeader->HetTableSize64, HET_TABLE_SIGNATURE, MPQ_KEY_HASH_TABLE);
            if(pvSrcFileInfo == NULL)
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            return GetInfo_Allocated(pvFileInfo, cbFileInfo, pvSrcFileInfo, sizeof(TMPQHetHeader), pcbLengthNeeded);

        case SFileMpqHetTable:
            if((pvSrcFileInfo = LoadHetTable(ha)) == NULL)
                return GetInfo_ReturnError(ERROR_NOT_ENOUGH_MEMORY);
            return GetInfo_TablePointer(pvFileInfo, cbFileInfo, pvSrcFileInfo, InfoClass, pcbLengthNeeded);

        case SFileMpqBetTableOffset:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->BetTablePos64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqBetTableSize:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->BetTableSize64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqBetHeader:

            // Retrieve the table and its size
            pvSrcFileInfo = LoadExtTable(ha, pHeader->BetTablePos64, (size_t)pHeader->BetTableSize64, BET_TABLE_SIGNATURE, MPQ_KEY_BLOCK_TABLE);
            if(pvSrcFileInfo == NULL)
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            cbSrcFileInfo = sizeof(TMPQBetHeader) + ((TMPQBetHeader *)pvSrcFileInfo)->dwFlagCount * sizeof(DWORD);

            // It is allowed for the caller to only require BET header
            if(cbFileInfo == sizeof(TMPQBetHeader))
                cbSrcFileInfo = sizeof(TMPQBetHeader);
            return GetInfo_Allocated(pvFileInfo, cbFileInfo, pvSrcFileInfo, cbSrcFileInfo, pcbLengthNeeded);

        case SFileMpqBetTable:
            if((pvSrcFileInfo = LoadBetTable(ha)) == NULL)
                return GetInfo_ReturnError(ERROR_NOT_ENOUGH_MEMORY);
            return GetInfo_TablePointer(pvFileInfo, cbFileInfo, pvSrcFileInfo, InfoClass, pcbLengthNeeded);
#endif // FULL

        case SFileMpqHashTableOffset:
            Int64Value = MAKE_OFFSET64(pHeader->wHashTablePosHi, pHeader->dwHashTablePos);
            return GetInfo(pvFileInfo, cbFileInfo, &Int64Value, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqHashTableSize64:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->HashTableSize64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqHashTableSize:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->dwHashTableSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqHashTable:
            cbSrcFileInfo = pHeader->dwHashTableSize * sizeof(TMPQHash);
            return GetInfo(pvFileInfo, cbFileInfo, ha->pHashTable, cbSrcFileInfo, pcbLengthNeeded);

        case SFileMpqBlockTableOffset:
            Int64Value = MAKE_OFFSET64(pHeader->wBlockTablePosHi, pHeader->dwBlockTablePos);
            return GetInfo(pvFileInfo, cbFileInfo, &Int64Value, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqBlockTableSize64:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->BlockTableSize64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqBlockTableSize:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->dwBlockTableSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqBlockTable:
            if(MAKE_OFFSET64(pHeader->wBlockTablePosHi, pHeader->dwBlockTablePos) >= ha->FileSize)
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            cbSrcFileInfo = pHeader->dwBlockTableSize * sizeof(TMPQBlock);
            pvSrcFileInfo = LoadBlockTable(ha, true);
            return GetInfo_Allocated(pvFileInfo, cbFileInfo, pvSrcFileInfo, cbSrcFileInfo, pcbLengthNeeded);

        case SFileMpqHiBlockTableOffset:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->HiBlockTablePos64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqHiBlockTableSize64:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->HiBlockTableSize64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqHiBlockTable:
            return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);

#ifdef FULL
        case SFileMpqSignatures:
            if(!QueryMpqSignatureInfo(ha, &SignatureInfo))
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            return GetInfo(pvFileInfo, cbFileInfo, &SignatureInfo.SignatureTypes, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqStrongSignatureOffset:
            if(QueryMpqSignatureInfo(ha, &SignatureInfo) == false || (SignatureInfo.SignatureTypes & SIGNATURE_TYPE_STRONG) == 0)
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            return GetInfo(pvFileInfo, cbFileInfo, &SignatureInfo.EndMpqData, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqStrongSignatureSize:
            if(QueryMpqSignatureInfo(ha, &SignatureInfo) == false || (SignatureInfo.SignatureTypes & SIGNATURE_TYPE_STRONG) == 0)
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            dwInt32Value = MPQ_STRONG_SIGNATURE_SIZE + 4;
            return GetInfo(pvFileInfo, cbFileInfo, &dwInt32Value, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqStrongSignature:
            if(QueryMpqSignatureInfo(ha, &SignatureInfo) == false || (SignatureInfo.SignatureTypes & SIGNATURE_TYPE_STRONG) == 0)
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            return GetInfo(pvFileInfo, cbFileInfo, SignatureInfo.Signature, MPQ_STRONG_SIGNATURE_SIZE + 4, pcbLengthNeeded);
#endif // FULL

        case SFileMpqArchiveSize64:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->ArchiveSize64, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileMpqArchiveSize:
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->dwArchiveSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqMaxFileCount:
            return GetInfo(pvFileInfo, cbFileInfo, &ha->dwMaxFileCount, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqFileTableSize:
            return GetInfo(pvFileInfo, cbFileInfo, &ha->dwFileTableSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqSectorSize:
            return GetInfo(pvFileInfo, cbFileInfo, &ha->dwSectorSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqNumberOfFiles:
            dwInt32Value = GetMpqFileCount(ha);
            return GetInfo(pvFileInfo, cbFileInfo, &dwInt32Value, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqRawChunkSize:
            if(pHeader->dwRawChunkSize == 0)
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            return GetInfo(pvFileInfo, cbFileInfo, &pHeader->dwRawChunkSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqStreamFlags:
            FileStream_GetFlags(ha->pStream, &dwInt32Value);
            return GetInfo(pvFileInfo, cbFileInfo, &dwInt32Value, sizeof(DWORD), pcbLengthNeeded);

        case SFileMpqFlags:
            return GetInfo(pvFileInfo, cbFileInfo, &ha->dwFlags, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoPatchChain:
            return GetInfo_PatchChain(hf, pvFileInfo, cbFileInfo, pcbLengthNeeded);

        case SFileInfoFileEntry:
            if(pFileEntry == NULL)
                return GetInfo_ReturnError(ERROR_FILE_NOT_FOUND);
            return GetInfo_FileEntry(pvFileInfo, cbFileInfo, pFileEntry, pcbLengthNeeded);

        case SFileInfoHashEntry:
            return GetInfo(pvFileInfo, cbFileInfo, hf->pHashEntry, sizeof(TMPQHash), pcbLengthNeeded);

        case SFileInfoHashIndex:
            return GetInfo(pvFileInfo, cbFileInfo, &hf->dwHashIndex, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoNameHash1:
            return GetInfo(pvFileInfo, cbFileInfo, &hf->pHashEntry->dwName1, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoNameHash2:
            return GetInfo(pvFileInfo, cbFileInfo, &hf->pHashEntry->dwName2, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoNameHash3:
            return GetInfo(pvFileInfo, cbFileInfo, &pFileEntry->FileNameHash, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileInfoLocale:
            dwInt32Value = hf->pHashEntry->lcLocale;
            return GetInfo(pvFileInfo, cbFileInfo, &dwInt32Value, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoFileIndex:
            dwInt32Value = (DWORD)(pFileEntry - hf->ha->pFileTable);
            return GetInfo(pvFileInfo, cbFileInfo, &dwInt32Value, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoByteOffset:
            return GetInfo(pvFileInfo, cbFileInfo, &pFileEntry->ByteOffset, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileInfoFileTime:
            return GetInfo(pvFileInfo, cbFileInfo, &pFileEntry->FileTime, sizeof(ULONGLONG), pcbLengthNeeded);

        case SFileInfoFileSize:
            return GetInfo(pvFileInfo, cbFileInfo, &pFileEntry->dwFileSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoCompressedSize:
            return GetInfo(pvFileInfo, cbFileInfo, &pFileEntry->dwCmpSize, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoFlags:
            return GetInfo(pvFileInfo, cbFileInfo, &pFileEntry->dwFlags, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoEncryptionKey:
            return GetInfo(pvFileInfo, cbFileInfo, &hf->dwFileKey, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoEncryptionKeyRaw:
            dwInt32Value = hf->dwFileKey;
            if(pFileEntry->dwFlags & MPQ_FILE_FIX_KEY)
                dwInt32Value = (dwInt32Value ^ pFileEntry->dwFileSize) - (DWORD)hf->MpqFilePos;
            return GetInfo(pvFileInfo, cbFileInfo, &dwInt32Value, sizeof(DWORD), pcbLengthNeeded);

        case SFileInfoCRC32:
            return GetInfo(pvFileInfo, cbFileInfo, &hf->pFileEntry->dwCrc32, sizeof(DWORD), pcbLengthNeeded);
        default:
            // Invalid info class
            return GetInfo_ReturnError(ERROR_INVALID_PARAMETER);
    }
}

#ifdef FULL
bool WINAPI SFileFreeFileInfo(void * pvFileInfo, SFileInfoClass InfoClass)
{
    switch(InfoClass)
    {
        case SFileMpqHetTable:
            FreeHetTable((TMPQHetTable *)pvFileInfo);
            return true;

        case SFileMpqBetTable:
            FreeBetTable((TMPQBetTable *)pvFileInfo);
            return true;

        default:
            break;
    }

    SetLastError(ERROR_INVALID_PARAMETER);
    return false;
}
#endif // FULL

//-----------------------------------------------------------------------------
// Tries to retrieve the file name

struct TFileHeader2Ext
{
    DWORD dwOffset00Data;               // Required data at offset 00 (32-bits)
    DWORD dwOffset00Mask;               // Mask for data at offset 00 (32 bits). 0 = data are ignored
    DWORD dwOffset04Data;               // Required data at offset 04 (32-bits)
    DWORD dwOffset04Mask;               // Mask for data at offset 04 (32 bits). 0 = data are ignored
    const char * szExt;                 // Supplied extension, if the condition is true
};

static TFileHeader2Ext data2ext[] =
{
    {0x00005A4D, 0x0000FFFF, 0x00000000, 0x00000000, "exe"},    // EXE files
    {0x00000006, 0xFFFFFFFF, 0x00000001, 0xFFFFFFFF, "dc6"},    // EXE files
    {0x1A51504D, 0xFFFFFFFF, 0x00000000, 0x00000000, "mpq"},    // MPQ archive header ID ('MPQ\x1A')
    {0x46464952, 0xFFFFFFFF, 0x00000000, 0x00000000, "wav"},    // WAVE header 'RIFF'
    {0x324B4D53, 0xFFFFFFFF, 0x00000000, 0x00000000, "smk"},    // Old "Smacker Video" files 'SMK2'
    {0x694B4942, 0xFFFFFFFF, 0x00000000, 0x00000000, "bik"},    // Bink video files (new)
    {0x0801050A, 0xFFFFFFFF, 0x00000000, 0x00000000, "pcx"},    // PCX images used in Diablo I
    {0x544E4F46, 0xFFFFFFFF, 0x00000000, 0x00000000, "fnt"},    // Font files used in Diablo II
    {0x6D74683C, 0xFFFFFFFF, 0x00000000, 0x00000000, "html"},   // HTML '<htm'
    {0x4D54483C, 0xFFFFFFFF, 0x00000000, 0x00000000, "html"},   // HTML '<HTM
    {0x216F6F57, 0xFFFFFFFF, 0x00000000, 0x00000000, "tbl"},    // Table files
    {0x31504C42, 0xFFFFFFFF, 0x00000000, 0x00000000, "blp"},    // BLP textures
    {0x32504C42, 0xFFFFFFFF, 0x00000000, 0x00000000, "blp"},    // BLP textures (v2)
    {0x584C444D, 0xFFFFFFFF, 0x00000000, 0x00000000, "mdx"},    // MDX files
    {0x45505954, 0xFFFFFFFF, 0x00000000, 0x00000000, "pud"},    // Warcraft II maps
    {0x38464947, 0xFFFFFFFF, 0x00000000, 0x00000000, "gif"},    // GIF images 'GIF8'
    {0x3032444D, 0xFFFFFFFF, 0x00000000, 0x00000000, "m2"},     // WoW ??? .m2
    {0x43424457, 0xFFFFFFFF, 0x00000000, 0x00000000, "dbc"},    // ??? .dbc
    {0x47585053, 0xFFFFFFFF, 0x00000000, 0x00000000, "bls"},    // WoW pixel shaders
    {0xE0FFD8FF, 0xFFFFFFFF, 0x00000000, 0x00000000, "jpg"},    // JPEG image
    {0x503B4449, 0xFFFFFFFF, 0x3B4C5857, 0xFFFFFFFF, "slk"},    // SLK file (usually starts with "ID;PWXL;N;E")
    {0x61754C1B, 0xFFFFFFFF, 0x00000000, 0x00000000, "lua"},    // Compiled LUA files
    {0x20534444, 0xFFFFFFFF, 0x00000000, 0x00000000, "dds"},    // DDS textures
    {0x43614C66, 0xFFFFFFFF, 0x00000000, 0x00000000, "flac"},   // FLAC sound files
    {0x0000FBFF, 0x0000FFFF, 0x00000000, 0x00000000, "mp3"},    // MP3 sound files
    {0x0000F3FF, 0x0000FFFF, 0x00000000, 0x00000000, "mp3"},    // MP3 sound files
    {0x0000F2FF, 0x0000FFFF, 0x00000000, 0x00000000, "mp3"},    // MP3 sound files
    {0x00334449, 0x00FFFFFF, 0x00000000, 0x00000000, "mp3"},    // MP3 sound files
    {0x57334D48, 0xFFFFFFFF, 0x00000000, 0x00000000, "w3x"},    // Warcraft III map files, can also be w3m
    {0x6F643357, 0xFFFFFFFF, 0x00000000, 0x00000000, "doo"},    // Warcraft III doodad files
    {0x21453357, 0xFFFFFFFF, 0x00000000, 0x00000000, "w3e"},    // Warcraft III environment files
    {0x5733504D, 0xFFFFFFFF, 0x00000000, 0x00000000, "wpm"},    // Warcraft III pathing map files
    {0x21475457, 0xFFFFFFFF, 0x00000000, 0x00000000, "wtg"},    // Warcraft III trigger files
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, "xxx"},    // Default extension
    {0, 0, 0, 0, NULL}                                          // Terminator
};

static int CreatePseudoFileName(HANDLE hFile, TFileEntry * pFileEntry, char * szFileName)
{
    TMPQFile * hf = (TMPQFile *)hFile;  // MPQ File handle
    DWORD FirstBytes[2] = {0, 0};       // The first 4 bytes of the file
    DWORD dwBytesRead = 0;
    DWORD dwFilePos;                    // Saved file position
    char szPseudoName[20];

    // Read the first 2 DWORDs bytes from the file
    dwFilePos = SFileSetFilePointer(hFile, 0, NULL, FILE_CURRENT);
    SFileReadFile(hFile, FirstBytes, sizeof(FirstBytes), &dwBytesRead, NULL);
    SFileSetFilePointer(hFile, dwFilePos, NULL, FILE_BEGIN);

    // If we read at least 8 bytes
    if(dwBytesRead == sizeof(FirstBytes))
    {
        // Make sure that the array is properly BSWAP-ed
        BSWAP_ARRAY32_UNSIGNED(FirstBytes, sizeof(FirstBytes));

        // Try to guess file extension from those 2 DWORDs
        for(size_t i = 0; data2ext[i].szExt != NULL; i++)
        {
            if((FirstBytes[0] & data2ext[i].dwOffset00Mask) == data2ext[i].dwOffset00Data &&
               (FirstBytes[1] & data2ext[i].dwOffset04Mask) == data2ext[i].dwOffset04Data)
            {
                // Format the pseudo-name
                StringCreatePseudoFileName(szPseudoName, _countof(szPseudoName), (unsigned int)(pFileEntry - hf->ha->pFileTable), data2ext[i].szExt);

                // Save the pseudo-name in the file entry as well
                AllocateFileName(hf->ha, pFileEntry, szPseudoName);

                // If the caller wants to copy the file name, do it
                if(szFileName != NULL)
                    strcpy(szFileName, szPseudoName);
                return ERROR_SUCCESS;
            }
        }
    }

    return ERROR_CAN_NOT_COMPLETE;
}

bool WINAPI SFileGetFileName(HANDLE hFile, char * szFileName)
{
    TMPQFile * hf = (TMPQFile *)hFile;  // MPQ File handle
    int nError = ERROR_INVALID_HANDLE;

    // Check valid parameters
    if(IsValidFileHandle(hFile))
    {
        TFileEntry * pFileEntry = hf->pFileEntry;

        // For MPQ files, retrieve the file name from the file entry
        if(hf->pStream == NULL)
        {
            if(pFileEntry != NULL)
            {
                // If the file name is not there yet, create a pseudo name
                if(pFileEntry->szFileName == NULL)
                    nError = CreatePseudoFileName(hFile, pFileEntry, szFileName);

                // Copy the file name to the output buffer, if any
                if(pFileEntry->szFileName && szFileName)
                {
                    strcpy(szFileName, pFileEntry->szFileName);
                    nError = ERROR_SUCCESS;
                }
            }
        }

        // For local files, copy the file name from the stream
        else
        {
            if(szFileName != NULL)
            {
                const TCHAR * szStreamName = FileStream_GetFileName(hf->pStream);
                StringCopy(szFileName, MAX_PATH, szStreamName);
            }
            nError = ERROR_SUCCESS;
        }
    }

    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

