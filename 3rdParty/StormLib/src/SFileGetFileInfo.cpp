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
// Local defines

// Information types for SFileGetFileInfo
#define SFILE_INFO_TYPE_INVALID_HANDLE  0
#define SFILE_INFO_TYPE_NOT_FOUND       1
#define SFILE_INFO_TYPE_DIRECT_POINTER  2
#define SFILE_INFO_TYPE_ALLOCATED       3
#define SFILE_INFO_TYPE_READ_FROM_FILE  4
#define SFILE_INFO_TYPE_TABLE_POINTER   5
#define SFILE_INFO_TYPE_FILE_ENTRY      6

//-----------------------------------------------------------------------------
// Local functions

static void ConvertFileEntryToSelfRelative(TFileEntry * pFileEntry, TFileEntry * pSrcFileEntry)
{
    // Copy the file entry itself
    memcpy(pFileEntry, pSrcFileEntry, sizeof(TFileEntry));

    // If source is NULL, leave it NULL
    if(pSrcFileEntry->szFileName != NULL)
    {
        // Set the file name pointer after the file entry
        pFileEntry->szFileName = (char *)(pFileEntry + 1);
        strcpy(pFileEntry->szFileName, pSrcFileEntry->szFileName);
    }
}


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

static bool GetFilePatchChain(TMPQFile * hf, void * pvFileInfo, DWORD cbFileInfo, DWORD * pcbLengthNeeded)
{
    TMPQFile * hfTemp;
    TCHAR * szFileInfo = (TCHAR *)pvFileInfo;
    size_t cchCharsNeeded = 1;
    size_t cchFileInfo = (cbFileInfo / sizeof(TCHAR));
    size_t nLength;

    // Patch chain is only supported on MPQ files.
    if(hf->pStream != NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

    // Calculate the necessary length of the multi-string
    for(hfTemp = hf; hfTemp != NULL; hfTemp = hfTemp->hfPatch)
        cchCharsNeeded += _tcslen(FileStream_GetFileName(hfTemp->ha->pStream)) + 1;

    // Give the caller the needed length
    if(pcbLengthNeeded != NULL)
        pcbLengthNeeded[0] = (DWORD)(cchCharsNeeded * sizeof(TCHAR));

    // If the caller gave both buffer pointer and data length,
    // try to copy the patch chain
    if(szFileInfo != NULL && cchFileInfo != 0)
    {
        // If there is enough space in the buffer, copy the patch chain
        if(cchCharsNeeded > cchFileInfo)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return false;
        }

        // Copy each patch
        for(hfTemp = hf; hfTemp != NULL; hfTemp = hfTemp->hfPatch)
        {
            // Get the file name and its length
            const TCHAR * szFileName = FileStream_GetFileName(hfTemp->ha->pStream);
            nLength = _tcslen(szFileName) + 1;

            // Copy the file name
            memcpy(szFileInfo, szFileName, nLength * sizeof(TCHAR));
            szFileInfo += nLength;
        }

        // Make it multi-string
        szFileInfo[0] = 0;
    }

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
    TMPQArchive * ha = NULL;
    TFileEntry * pFileEntry = NULL;
    ULONGLONG Int64Value = 0;
    ULONGLONG ByteOffset = 0;
    TMPQFile * hf = NULL;
    void * pvSrcFileInfo = NULL;
    DWORD cbSrcFileInfo = 0;
    DWORD dwInt32Value = 0;
    int nInfoType = SFILE_INFO_TYPE_INVALID_HANDLE;
    int nError = ERROR_SUCCESS;

    switch(InfoClass)
    {
        case SFileMpqFileName:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = (void *)FileStream_GetFileName(ha->pStream);
                cbSrcFileInfo = (DWORD)(_tcslen((TCHAR *)pvSrcFileInfo) + 1) * sizeof(TCHAR);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqStreamBitmap:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
                return FileStream_GetBitmap(ha->pStream, pvFileInfo, cbFileInfo, pcbLengthNeeded);
            break;

        case SFileMpqUserDataOffset:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(ha->pUserData != NULL)
                {
                    pvSrcFileInfo = &ha->UserDataPos;
                    cbSrcFileInfo = sizeof(ULONGLONG);
                    nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
                }
            }
            break;

        case SFileMpqUserDataHeader:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(ha->pUserData != NULL)
                {
                    ByteOffset = ha->UserDataPos;
                    cbSrcFileInfo = sizeof(TMPQUserData);
                    nInfoType = SFILE_INFO_TYPE_READ_FROM_FILE;
                }
            }
            break;

        case SFileMpqUserData:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(ha->pUserData != NULL)
                {
                    ByteOffset = ha->UserDataPos + sizeof(TMPQUserData);
                    cbSrcFileInfo = ha->pUserData->dwHeaderOffs - sizeof(TMPQUserData);
                    nInfoType = SFILE_INFO_TYPE_READ_FROM_FILE;
                }
            }
            break;

        case SFileMpqHeaderOffset:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->MpqPos;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqHeaderSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->dwHeaderSize;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqHeader:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                ByteOffset = ha->MpqPos;
                cbSrcFileInfo = ha->pHeader->dwHeaderSize;
                nInfoType = SFILE_INFO_TYPE_READ_FROM_FILE;
            }
            break;

        case SFileMpqHetTableOffset:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->HetTablePos64;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

#ifdef FULL
        case SFileMpqHetTableSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->HetTableSize64;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqHetHeader:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                pvSrcFileInfo = LoadExtTable(ha, ha->pHeader->HetTablePos64, (size_t)ha->pHeader->HetTableSize64, HET_TABLE_SIGNATURE, MPQ_KEY_HASH_TABLE);
                if(pvSrcFileInfo != NULL)
                {
                    cbSrcFileInfo = sizeof(TMPQHetHeader);
                    nInfoType = SFILE_INFO_TYPE_ALLOCATED;
                }
            }
            break;

        case SFileMpqHetTable:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                pvSrcFileInfo = LoadHetTable(ha);
                if(pvSrcFileInfo != NULL)
                {
                    cbSrcFileInfo = sizeof(void *);
                    nInfoType = SFILE_INFO_TYPE_TABLE_POINTER;
                }
            }
            break;

        case SFileMpqBetTableOffset:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->BetTablePos64;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqBetTableSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->BetTableSize64;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqBetHeader:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                pvSrcFileInfo = LoadExtTable(ha, ha->pHeader->BetTablePos64, (size_t)ha->pHeader->BetTableSize64, BET_TABLE_SIGNATURE, MPQ_KEY_BLOCK_TABLE);
                if(pvSrcFileInfo != NULL)
                {
                    // It is allowed for the caller to only require BET header.
                    cbSrcFileInfo = sizeof(TMPQBetHeader) + ((TMPQBetHeader *)pvSrcFileInfo)->dwFlagCount * sizeof(DWORD);
                    if(cbFileInfo == sizeof(TMPQBetHeader))
                        cbSrcFileInfo = sizeof(TMPQBetHeader);
                    nInfoType = SFILE_INFO_TYPE_ALLOCATED;
                }
            }
            break;

        case SFileMpqBetTable:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                pvSrcFileInfo = LoadBetTable(ha);
                if(pvSrcFileInfo != NULL)
                {
                    cbSrcFileInfo = sizeof(void *);
                    nInfoType = SFILE_INFO_TYPE_TABLE_POINTER;
                }
            }
            break;
#endif
        case SFileMpqHashTableOffset:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                Int64Value = MAKE_OFFSET64(ha->pHeader->wHashTablePosHi, ha->pHeader->dwHashTablePos);
                pvSrcFileInfo = &Int64Value;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqHashTableSize64:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->HashTableSize64;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqHashTableSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->dwHashTableSize;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqHashTable:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL && ha->pHashTable != NULL)
            {
                pvSrcFileInfo = ha->pHashTable;
                cbSrcFileInfo = ha->pHeader->dwHashTableSize * sizeof(TMPQHash);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqBlockTableOffset:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                Int64Value = MAKE_OFFSET64(ha->pHeader->wBlockTablePosHi, ha->pHeader->dwBlockTablePos);
                pvSrcFileInfo = &Int64Value;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqBlockTableSize64:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->BlockTableSize64;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqBlockTableSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->dwBlockTableSize;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqBlockTable:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(MAKE_OFFSET64(ha->pHeader->wBlockTablePosHi, ha->pHeader->dwBlockTablePos) < ha->FileSize)
                {
                    cbSrcFileInfo = ha->pHeader->dwBlockTableSize * sizeof(TMPQBlock);
                    if(cbFileInfo >= cbSrcFileInfo)
                        pvSrcFileInfo = LoadBlockTable(ha, true);
                    nInfoType = SFILE_INFO_TYPE_ALLOCATED;
                }
            }
            break;

        case SFileMpqHiBlockTableOffset:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->HiBlockTablePos64;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqHiBlockTableSize64:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->HiBlockTableSize64;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqHiBlockTable:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(ha->pHeader->HiBlockTablePos64 && ha->pHeader->HiBlockTableSize64)
                {
                    assert(false);
                }
            }
            break;

#ifdef FULL
        case SFileMpqSignatures:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL && QueryMpqSignatureInfo(ha, &SignatureInfo))
            {
                pvSrcFileInfo = &SignatureInfo.SignatureTypes;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqStrongSignatureOffset:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(QueryMpqSignatureInfo(ha, &SignatureInfo) && (SignatureInfo.SignatureTypes & SIGNATURE_TYPE_STRONG))
                {
                    pvSrcFileInfo = &SignatureInfo.EndMpqData;
                    cbSrcFileInfo = sizeof(ULONGLONG);
                    nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
                }
            }
            break;

        case SFileMpqStrongSignatureSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(QueryMpqSignatureInfo(ha, &SignatureInfo) && (SignatureInfo.SignatureTypes & SIGNATURE_TYPE_STRONG))
                {
                    dwInt32Value = MPQ_STRONG_SIGNATURE_SIZE + 4;
                    pvSrcFileInfo = &dwInt32Value;
                    cbSrcFileInfo = sizeof(DWORD);
                    nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
                }
            }
            break;

        case SFileMpqStrongSignature:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(QueryMpqSignatureInfo(ha, &SignatureInfo) && (SignatureInfo.SignatureTypes & SIGNATURE_TYPE_STRONG))
                {
                    pvSrcFileInfo = SignatureInfo.Signature;
                    cbSrcFileInfo = MPQ_STRONG_SIGNATURE_SIZE + 4;
                    nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
                }
            }
            break;
#endif

        case SFileMpqArchiveSize64:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->ArchiveSize64;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqArchiveSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->pHeader->dwArchiveSize;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqMaxFileCount:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->dwMaxFileCount;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqFileTableSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->dwFileTableSize;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqSectorSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &ha->dwSectorSize;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqNumberOfFiles:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                dwInt32Value = GetMpqFileCount(ha);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqRawChunkSize:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                nInfoType = SFILE_INFO_TYPE_NOT_FOUND;
                if(ha->pHeader->dwRawChunkSize != 0)
                {
                    pvSrcFileInfo = &ha->pHeader->dwRawChunkSize;
                    cbSrcFileInfo = sizeof(DWORD);
                    nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
                }
            }
            break;

        case SFileMpqStreamFlags:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                FileStream_GetFlags(ha->pStream, &dwInt32Value);
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileMpqFlags:
            ha = IsValidMpqHandle(hMpqOrFile);
            if(ha != NULL)
            {
                dwInt32Value  = ha->dwFlags;
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoPatchChain:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL)
                return GetFilePatchChain(hf, pvFileInfo, cbFileInfo, pcbLengthNeeded);
            break;

        case SFileInfoFileEntry:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                pvSrcFileInfo = pFileEntry = hf->pFileEntry;
                cbSrcFileInfo = sizeof(TFileEntry);
                if(pFileEntry->szFileName != NULL)
                    cbSrcFileInfo += (DWORD)strlen(pFileEntry->szFileName) + 1;
                nInfoType = SFILE_INFO_TYPE_FILE_ENTRY;
            }
            break;

        case SFileInfoHashEntry:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pHashEntry != NULL)
            {
                pvSrcFileInfo = hf->pHashEntry;
                cbSrcFileInfo = sizeof(TMPQHash);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoHashIndex:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pHashEntry != NULL)
            {
                pvSrcFileInfo = &hf->dwHashIndex;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoNameHash1:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pHashEntry != NULL)
            {
                dwInt32Value = hf->pHashEntry->dwName1;
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoNameHash2:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pHashEntry != NULL)
            {
                dwInt32Value = hf->pHashEntry->dwName2;
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoNameHash3:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                pvSrcFileInfo = &hf->pFileEntry->FileNameHash;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoLocale:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pHashEntry != NULL)
            {
                dwInt32Value = hf->pHashEntry->lcLocale;
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoFileIndex:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->ha != NULL && hf->pFileEntry != NULL)
            {
                dwInt32Value = (DWORD)(hf->pFileEntry - hf->ha->pFileTable);
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoByteOffset:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                pvSrcFileInfo = &hf->pFileEntry->ByteOffset;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoFileTime:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                pvSrcFileInfo = &hf->pFileEntry->FileTime;
                cbSrcFileInfo = sizeof(ULONGLONG);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoFileSize:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                pvSrcFileInfo = &hf->pFileEntry->dwFileSize;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoCompressedSize:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                pvSrcFileInfo = &hf->pFileEntry->dwCmpSize;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoFlags:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                pvSrcFileInfo = &hf->pFileEntry->dwFlags;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoEncryptionKey:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL)
            {
                pvSrcFileInfo = &hf->dwFileKey;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoEncryptionKeyRaw:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                dwInt32Value = hf->dwFileKey;
                if(hf->pFileEntry->dwFlags & MPQ_FILE_FIX_KEY)
                    dwInt32Value = (dwInt32Value ^ hf->pFileEntry->dwFileSize) - (DWORD)hf->MpqFilePos;
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        case SFileInfoCRC32:
            hf = IsValidFileHandle(hMpqOrFile);
            if(hf != NULL && hf->pFileEntry != NULL)
            {
                dwInt32Value = hf->pFileEntry->dwCrc32;
                pvSrcFileInfo = &dwInt32Value;
                cbSrcFileInfo = sizeof(DWORD);
                nInfoType = SFILE_INFO_TYPE_DIRECT_POINTER;
            }
            break;

        default:    // Invalid info class
            SetLastError(ERROR_INVALID_PARAMETER);
            return false;
    }

    // If we validated the handle and info class, give as much info as possible
    if(nInfoType >= SFILE_INFO_TYPE_DIRECT_POINTER)
    {
        // Give the length needed, if wanted
        if(pcbLengthNeeded != NULL)
            pcbLengthNeeded[0] = cbSrcFileInfo;

        // If the caller entered an output buffer, the output size must also be entered
        if(pvFileInfo != NULL && cbFileInfo != 0)
        {
            // Check if there is enough space in the output buffer
            if(cbSrcFileInfo <= cbFileInfo)
            {
                switch(nInfoType)
                {
                    case SFILE_INFO_TYPE_DIRECT_POINTER:
                    case SFILE_INFO_TYPE_ALLOCATED:
                        assert(pvSrcFileInfo != NULL);
                        memcpy(pvFileInfo, pvSrcFileInfo, cbSrcFileInfo);
                        break;

                    case SFILE_INFO_TYPE_READ_FROM_FILE:
                        if(!FileStream_Read(ha->pStream, &ByteOffset, pvFileInfo, cbSrcFileInfo))
                            nError = GetLastError();
                        break;

                    case SFILE_INFO_TYPE_TABLE_POINTER:
                        assert(pvSrcFileInfo != NULL);
                        *(void **)pvFileInfo = pvSrcFileInfo;
                        pvSrcFileInfo = NULL;
                        break;

                    case SFILE_INFO_TYPE_FILE_ENTRY:
                        assert(pFileEntry != NULL);
                        ConvertFileEntryToSelfRelative((TFileEntry *)pvFileInfo, pFileEntry);
                        break;
                }
            }
            else
            {
                nError = ERROR_INSUFFICIENT_BUFFER;
            }
        }

        // Free the file info if needed
        if(nInfoType == SFILE_INFO_TYPE_ALLOCATED && pvSrcFileInfo != NULL)
            STORM_FREE(pvSrcFileInfo);
#ifdef FULL
        if(nInfoType == SFILE_INFO_TYPE_TABLE_POINTER && pvSrcFileInfo != NULL)
            SFileFreeFileInfo(pvSrcFileInfo, InfoClass);
#endif
    }
    else
    {
        // Handle error cases
        if(nInfoType == SFILE_INFO_TYPE_INVALID_HANDLE)
            nError = ERROR_INVALID_HANDLE;
        if(nInfoType == SFILE_INFO_TYPE_NOT_FOUND)
            nError = ERROR_FILE_NOT_FOUND;
    }

    // Set the last error value, if needed
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
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
#endif

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
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, "xxx"},    // Default extension
    {0, 0, 0, 0, NULL}                                          // Terminator
};

static int CreatePseudoFileName(HANDLE hFile, TFileEntry * pFileEntry, char * szFileName)
{
    TMPQFile * hf = (TMPQFile *)hFile;  // MPQ File handle
    DWORD FirstBytes[2] = {0, 0};       // The first 4 bytes of the file
    DWORD dwBytesRead = 0;
    DWORD dwFilePos;                    // Saved file position

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
                char szPseudoName[20] = "";

                // Format the pseudo-name
                sprintf(szPseudoName, "File%08u.%s", (unsigned int)(pFileEntry - hf->ha->pFileTable), data2ext[i].szExt);

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

