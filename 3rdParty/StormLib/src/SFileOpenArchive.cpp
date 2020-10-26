/*****************************************************************************/
/* SFileOpenArchive.cpp                       Copyright Ladislav Zezula 1999 */
/*                                                                           */
/* Author : Ladislav Zezula                                                  */
/* E-mail : ladik@zezula.net                                                 */
/* WWW    : www.zezula.net                                                   */
/*---------------------------------------------------------------------------*/
/*                       Archive functions of Storm.dll                      */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* xx.xx.xx  1.00  Lad  The first version of SFileOpenArchive.cpp            */
/* 19.11.03  1.01  Dan  Big endian handling                                  */
/*****************************************************************************/

#ifndef FULL
#include <algorithm>
#include <string>
#endif

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "StormCommon.h"

#define HEADER_SEARCH_BUFFER_SIZE   0x1000

/*****************************************************************************/
/* Local functions                                                           */
/*****************************************************************************/

static bool IsAviFile(DWORD * HeaderData)
{
    DWORD DwordValue0 = BSWAP_INT32_UNSIGNED(HeaderData[0]);
    DWORD DwordValue2 = BSWAP_INT32_UNSIGNED(HeaderData[2]);
    DWORD DwordValue3 = BSWAP_INT32_UNSIGNED(HeaderData[3]);

    // Test for 'RIFF', 'AVI ' or 'LIST'
    return (DwordValue0 == 0x46464952 && DwordValue2 == 0x20495641 && DwordValue3 == 0x5453494C);
}

static bool IsWarcraft3Map(DWORD * HeaderData)
{
    DWORD DwordValue0 = BSWAP_INT32_UNSIGNED(HeaderData[0]);
    DWORD DwordValue1 = BSWAP_INT32_UNSIGNED(HeaderData[1]);

    return (DwordValue0 == 0x57334D48 && DwordValue1 == 0x00000000);
}

static TMPQUserData * IsValidMpqUserData(ULONGLONG ByteOffset, ULONGLONG FileSize, void * pvUserData)
{
    TMPQUserData * pUserData;

    // BSWAP the source data and copy them to our buffer
    BSWAP_ARRAY32_UNSIGNED(&pvUserData, sizeof(TMPQUserData));
    pUserData = (TMPQUserData *)pvUserData;

    // Check the sizes
    if(pUserData->cbUserDataHeader <= pUserData->cbUserDataSize && pUserData->cbUserDataSize <= pUserData->dwHeaderOffs)
    {
        // Move to the position given by the userdata
        ByteOffset += pUserData->dwHeaderOffs;

        // The MPQ header should be within range of the file size
        if((ByteOffset + MPQ_HEADER_SIZE_V1) < FileSize)
        {
            // Note: We should verify if there is the MPQ header.
            // However, the header could be at any position below that
            // that is multiplier of 0x200
            return (TMPQUserData *)pvUserData;
        }
    }

    return NULL;
}

// This function gets the right positions of the hash table and the block table.
static int VerifyMpqTablePositions(TMPQArchive * ha, ULONGLONG FileSize)
{
    TMPQHeader * pHeader = ha->pHeader;
    ULONGLONG ByteOffset;

    // Check the begin of HET table
    if(pHeader->HetTablePos64)
    {
        ByteOffset = ha->MpqPos + pHeader->HetTablePos64;
        if(ByteOffset > FileSize)
            return ERROR_BAD_FORMAT;
    }

    // Check the begin of BET table
    if(pHeader->BetTablePos64)
    {
        ByteOffset = ha->MpqPos + pHeader->BetTablePos64;
        if(ByteOffset > FileSize)
            return ERROR_BAD_FORMAT;
    }

    // Check the begin of hash table
    if(pHeader->wHashTablePosHi || pHeader->dwHashTablePos)
    {
        ByteOffset = FileOffsetFromMpqOffset(ha, MAKE_OFFSET64(pHeader->wHashTablePosHi, pHeader->dwHashTablePos));
        if(ByteOffset > FileSize)
            return ERROR_BAD_FORMAT;
    }

    // Check the begin of block table
    if(pHeader->wBlockTablePosHi || pHeader->dwBlockTablePos)
    {
        ByteOffset = FileOffsetFromMpqOffset(ha, MAKE_OFFSET64(pHeader->wBlockTablePosHi, pHeader->dwBlockTablePos));
        if(ByteOffset > FileSize)
            return ERROR_BAD_FORMAT;
    }

    // Check the begin of hi-block table
    if(pHeader->HiBlockTablePos64 != 0)
    {
        ByteOffset = ha->MpqPos + pHeader->HiBlockTablePos64;
        if(ByteOffset > FileSize)
            return ERROR_BAD_FORMAT;
    }

    // All OK.
    return ERROR_SUCCESS;
}


/*****************************************************************************/
/* Public functions                                                          */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// SFileGetLocale and SFileSetLocale
// Set the locale for all newly opened files

LCID STORMAPI SFileGetLocale()
{
    return lcFileLocale;
}

LCID STORMAPI SFileSetLocale(LCID lcNewLocale)
{
    lcFileLocale = lcNewLocale;
    return lcFileLocale;
}

//-----------------------------------------------------------------------------
// SFileOpenArchive
//
//   szFileName - MPQ archive file name to open
//   dwPriority - When SFileOpenFileEx called, this contains the search priority for searched archives
//   dwFlags    - See MPQ_OPEN_XXX in StormLib.h
//   phMpq      - Pointer to store open archive handle

bool STORMAPI SFileOpenArchive(
    const TCHAR * szMpqName,
    DWORD dwPriority,
    DWORD dwFlags,
    HANDLE * phMpq)
{
    TMPQUserData * pUserData;
    TFileStream * pStream = NULL;       // Open file stream
    TMPQArchive * ha = NULL;            // Archive handle
    TFileEntry * pFileEntry;
    ULONGLONG FileSize = 0;             // Size of the file
    LPBYTE pbHeaderBuffer = NULL;       // Buffer for searching MPQ header
    DWORD dwStreamFlags = (dwFlags & STREAM_FLAGS_MASK);
    bool bIsWarcraft3Map = false;
    int nError = ERROR_SUCCESS;

    // Verify the parameters
    if(szMpqName == NULL || *szMpqName == 0 || phMpq == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

    // One time initialization of MPQ cryptography
    InitializeMpqCryptography();
    dwPriority = dwPriority;

    // If not forcing MPQ v 1.0, also use file bitmap
    dwStreamFlags |= (dwFlags & MPQ_OPEN_FORCE_MPQ_V1) ? 0 : STREAM_FLAG_USE_BITMAP;

#ifndef FULL
    std::string name = szMpqName;
    std::replace(name.begin(), name.end(), '\\', '/');
    szMpqName = name.c_str();
#endif

    // Open the MPQ archive file
    pStream = FileStream_OpenFile(szMpqName, dwStreamFlags);
    if(pStream == NULL)
        return false;

    // Check the file size. There must be at least 0x20 bytes
    if(nError == ERROR_SUCCESS)
    {
        FileStream_GetSize(pStream, &FileSize);
        if(FileSize < MPQ_HEADER_SIZE_V1)
            nError = ERROR_BAD_FORMAT;
    }

    // Allocate the MPQhandle
    if(nError == ERROR_SUCCESS)
    {
        if((ha = STORM_ALLOC(TMPQArchive, 1)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Allocate buffer for searching MPQ header
    if(nError == ERROR_SUCCESS)
    {
        pbHeaderBuffer = STORM_ALLOC(BYTE, HEADER_SEARCH_BUFFER_SIZE);
        if(pbHeaderBuffer == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Find the position of MPQ header
    if(nError == ERROR_SUCCESS)
    {
        ULONGLONG SearchOffset = 0;
        ULONGLONG EndOfSearch = FileSize;
        DWORD dwStrmFlags = 0;
        DWORD dwHeaderSize;
        DWORD dwHeaderID;
        bool bSearchComplete = false;

        memset(ha, 0, sizeof(TMPQArchive));
        ha->pfnHashString = HashStringSlash;
        ha->pStream = pStream;
        pStream = NULL;

        // Set the archive read only if the stream is read-only
        FileStream_GetFlags(ha->pStream, &dwStrmFlags);
        ha->dwFlags |= (dwStrmFlags & STREAM_FLAG_READ_ONLY) ? MPQ_FLAG_READ_ONLY : 0;

        // Also remember if we shall check sector CRCs when reading file
        ha->dwFlags |= (dwFlags & MPQ_OPEN_CHECK_SECTOR_CRC) ? MPQ_FLAG_CHECK_SECTOR_CRC : 0;

        // Also remember if this MPQ is a patch
        ha->dwFlags |= (dwFlags & MPQ_OPEN_PATCH) ? MPQ_FLAG_PATCH : 0;

        // Limit the header searching to about 130 MB of data
        if(EndOfSearch > 0x08000000)
            EndOfSearch = 0x08000000;

        // Find the offset of MPQ header within the file
        while(bSearchComplete == false && SearchOffset < EndOfSearch)
        {
            // Always read at least 0x1000 bytes for performance.
            // This is what Storm.dll (2002) does.
            DWORD dwBytesAvailable = HEADER_SEARCH_BUFFER_SIZE;
            DWORD dwInBufferOffset = 0;

            // Cut the bytes available, if needed
            if((FileSize - SearchOffset) < HEADER_SEARCH_BUFFER_SIZE)
                dwBytesAvailable = (DWORD)(FileSize - SearchOffset);

            // Read the eventual MPQ header
            if(!FileStream_Read(ha->pStream, &SearchOffset, pbHeaderBuffer, dwBytesAvailable))
            {
                nError = GetLastError();
                break;
            }

            // There are AVI files from Warcraft III with 'MPQ' extension.
            if(SearchOffset == 0)
            {
                if(IsAviFile((DWORD *)pbHeaderBuffer))
                {
                    nError = ERROR_AVI_FILE;
                    break;
                }

                bIsWarcraft3Map = IsWarcraft3Map((DWORD *)pbHeaderBuffer);
            }

            // Search the header buffer
            while(dwInBufferOffset < dwBytesAvailable)
            {
                // Copy the data from the potential header buffer to the MPQ header
                memcpy(ha->HeaderData, pbHeaderBuffer + dwInBufferOffset, sizeof(ha->HeaderData));

                // If there is the MPQ user data, process it
                // Note that Warcraft III does not check for user data, which is abused by many map protectors
                dwHeaderID = BSWAP_INT32_UNSIGNED(ha->HeaderData[0]);
                if(bIsWarcraft3Map == false && (dwFlags & MPQ_OPEN_FORCE_MPQ_V1) == 0)
                {
                    if(ha->pUserData == NULL && dwHeaderID == ID_MPQ_USERDATA)
                    {
                        // Verify if this looks like a valid user data
                        pUserData = IsValidMpqUserData(SearchOffset, FileSize, ha->HeaderData);
                        if(pUserData != NULL)
                        {
                            // Fill the user data header
                            ha->UserDataPos = SearchOffset;
                            ha->pUserData = &ha->UserData;
                            memcpy(ha->pUserData, pUserData, sizeof(TMPQUserData));

                            // Continue searching from that position
                            SearchOffset += ha->pUserData->dwHeaderOffs;
                            break;
                        }
                    }
                }

                // There must be MPQ header signature. Note that STORM.dll from Warcraft III actually
                // tests the MPQ header size. It must be at least 0x20 bytes in order to load it
                // Abused by Spazzler Map protector. Note that the size check is not present
                // in Storm.dll v 1.00, so Diablo I code would load the MPQ anyway.
                dwHeaderSize = BSWAP_INT32_UNSIGNED(ha->HeaderData[1]);
                if(dwHeaderID == ID_MPQ && dwHeaderSize >= MPQ_HEADER_SIZE_V1)
                {
                    // Now convert the header to version 4
                    nError = ConvertMpqHeaderToFormat4(ha, SearchOffset, FileSize, dwFlags, bIsWarcraft3Map);
                    bSearchComplete = true;
                    break;
                }

                // Check for MPK archives (Longwu Online - MPQ fork)
                if(bIsWarcraft3Map == false && dwHeaderID == ID_MPK)
                {
                    // Now convert the MPK header to MPQ Header version 4
                    nError = ConvertMpkHeaderToFormat4(ha, FileSize, dwFlags);
                    bSearchComplete = true;
                    break;
                }

                // If searching for the MPQ header is disabled, return an error
                if(dwFlags & MPQ_OPEN_NO_HEADER_SEARCH)
                {
                    nError = ERROR_NOT_SUPPORTED;
                    bSearchComplete = true;
                    break;
                }

                // Move the pointers
                SearchOffset += 0x200;
                dwInBufferOffset += 0x200;
            }
        }

        // Did we identify one of the supported headers?
        if(nError == ERROR_SUCCESS)
        {
            // Set the user data position to the MPQ header, if none
            if(ha->pUserData == NULL)
                ha->UserDataPos = SearchOffset;

            // Set the position of the MPQ header
            ha->pHeader  = (TMPQHeader *)ha->HeaderData;
            ha->MpqPos   = SearchOffset;
            ha->FileSize = FileSize;

            // Sector size must be nonzero.
            if(SearchOffset >= FileSize || ha->pHeader->wSectorSize == 0)
                nError = ERROR_BAD_FORMAT;
        }
    }

    // Fix table positions according to format
    if(nError == ERROR_SUCCESS)
    {
        // Dump the header
//      DumpMpqHeader(ha->pHeader);

        // W3x Map Protectors use the fact that War3's Storm.dll ignores the MPQ user data,
        // and ignores the MPQ format version as well. The trick is to
        // fake MPQ format 2, with an improper hi-word position of hash table and block table
        // We can overcome such protectors by forcing opening the archive as MPQ v 1.0
        if(dwFlags & MPQ_OPEN_FORCE_MPQ_V1)
        {
            ha->pHeader->wFormatVersion = MPQ_FORMAT_VERSION_1;
            ha->pHeader->dwHeaderSize = MPQ_HEADER_SIZE_V1;
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;
            ha->pUserData = NULL;
        }

        // Anti-overflow. If the hash table size in the header is
        // higher than 0x10000000, it would overflow in 32-bit version
        // Observed in the malformed Warcraft III maps
        // Example map: MPQ_2016_v1_ProtectedMap_TableSizeOverflow.w3x
        ha->pHeader->dwBlockTableSize = (ha->pHeader->dwBlockTableSize & BLOCK_INDEX_MASK);
        ha->pHeader->dwHashTableSize = (ha->pHeader->dwHashTableSize & BLOCK_INDEX_MASK);

        // Both MPQ_OPEN_NO_LISTFILE or MPQ_OPEN_NO_ATTRIBUTES trigger read only mode
        if(dwFlags & (MPQ_OPEN_NO_LISTFILE | MPQ_OPEN_NO_ATTRIBUTES))
            ha->dwFlags |= MPQ_FLAG_READ_ONLY;

        // Remember whether whis is a map for Warcraft III
        if(bIsWarcraft3Map)
            ha->dwFlags |= MPQ_FLAG_WAR3_MAP;

        // Set the size of file sector
        ha->dwSectorSize = (0x200 << ha->pHeader->wSectorSize);

        // Verify if any of the tables doesn't start beyond the end of the file
        nError = VerifyMpqTablePositions(ha, FileSize);
    }

    // Read the hash table. Ignore the result, as hash table is no longer required
    // Read HET table. Ignore the result, as HET table is no longer required
    if(nError == ERROR_SUCCESS)
    {
        nError = LoadAnyHashTable(ha);
    }

    // Now, build the file table. It will be built by combining
    // the block table, BET table, hi-block table, (attributes) and (listfile).
    if(nError == ERROR_SUCCESS)
    {
        nError = BuildFileTable(ha);
    }

#ifdef FULL
    // Load the internal listfile and include it to the file table
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_OPEN_NO_LISTFILE) == 0)
    {
        // Quick check for (listfile)
        pFileEntry = GetFileEntryLocale(ha, LISTFILE_NAME, LANG_NEUTRAL);
        if(pFileEntry != NULL)
        {
            // Ignore result of the operation. (listfile) is optional.
            SFileAddListFile((HANDLE)ha, NULL);
            ha->dwFileFlags1 = pFileEntry->dwFlags;
        }
    }

    // Load the "(attributes)" file and merge it to the file table
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_OPEN_NO_ATTRIBUTES) == 0 && (ha->dwFlags & MPQ_FLAG_BLOCK_TABLE_CUT) == 0)
    {
        // Quick check for (attributes)
        pFileEntry = GetFileEntryLocale(ha, ATTRIBUTES_NAME, LANG_NEUTRAL);
        if(pFileEntry != NULL)
        {
            // Ignore result of the operation. (attributes) is optional.
            SAttrLoadAttributes(ha);
            ha->dwFileFlags2 = pFileEntry->dwFlags;
        }
    }
#endif

    // Remember whether the archive has weak signature. Only for MPQs format 1.0.
    if(nError == ERROR_SUCCESS)
    {
        // Quick check for (signature)
        pFileEntry = GetFileEntryLocale(ha, SIGNATURE_NAME, LANG_NEUTRAL);
        if(pFileEntry != NULL)
        {
            // Just remember that the archive is weak-signed
            assert((pFileEntry->dwFlags & MPQ_FILE_EXISTS) != 0);
            ha->dwFileFlags3 = pFileEntry->dwFlags;
        }

        // Finally, set the MPQ_FLAG_READ_ONLY if the MPQ was found malformed
        ha->dwFlags |= (ha->dwFlags & MPQ_FLAG_MALFORMED) ? MPQ_FLAG_READ_ONLY : 0;
    }

    // Cleanup and exit
    if(nError != ERROR_SUCCESS)
    {
        FileStream_Close(pStream);
        FreeArchiveHandle(ha);
        SetLastError(nError);
        ha = NULL;
    }

    // Free the header buffer
    if(pbHeaderBuffer != NULL)
        STORM_FREE(pbHeaderBuffer);
    if(phMpq != NULL)
        *phMpq = ha;
    return (nError == ERROR_SUCCESS);
}


#ifdef FULL
//-----------------------------------------------------------------------------
// bool STORMAPI SFileSetDownloadCallback(HANDLE, SFILE_DOWNLOAD_CALLBACK, void *);
//
// Sets a callback that is called when content is downloaded from the master MPQ
//

bool STORMAPI SFileSetDownloadCallback(HANDLE hMpq, SFILE_DOWNLOAD_CALLBACK DownloadCB, void * pvUserData)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;

    // Do nothing if 'hMpq' is bad parameter
    if(!IsValidMpqHandle(hMpq))
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return false;
    }

    return FileStream_SetCallback(ha->pStream, DownloadCB, pvUserData);
}

//-----------------------------------------------------------------------------
// bool SFileFlushArchive(HANDLE hMpq)
//
// Saves all dirty data into MPQ archive.
// Has similar effect like SFileCloseArchive, but the archive is not closed.
// Use on clients who keep MPQ archive open even for write operations,
// and terminating without calling SFileCloseArchive might corrupt the archive.
//

bool STORMAPI SFileFlushArchive(HANDLE hMpq)
{
    TMPQArchive * ha;
    int nResultError = ERROR_SUCCESS;
    int nError;

    // Do nothing if 'hMpq' is bad parameter
    if((ha = IsValidMpqHandle(hMpq)) == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return false;
    }

    // Only if the MPQ was changed
    if(ha->dwFlags & MPQ_FLAG_CHANGED)
    {
        // Indicate that we are saving MPQ internal structures
        ha->dwFlags |= MPQ_FLAG_SAVING_TABLES;

        // Defragment the file table. This will allow us to put the internal files to the end
        DefragmentFileTable(ha);

        //
        // Create each internal file
        // Note that the (signature) file is usually before (listfile) in the file table
        //

        if(ha->dwFlags & MPQ_FLAG_SIGNATURE_NEW)
        {
#ifdef FULL
            nError = SSignFileCreate(ha);
            if(nError != ERROR_SUCCESS)
                nResultError = nError;
#else
			assert(0);
#endif
        }

        if(ha->dwFlags & MPQ_FLAG_LISTFILE_NEW)
        {
            nError = SListFileSaveToMpq(ha);
            if(nError != ERROR_SUCCESS)
                nResultError = nError;
        }

        if(ha->dwFlags & MPQ_FLAG_ATTRIBUTES_NEW)
        {
            nError = SAttrFileSaveToMpq(ha);
            if(nError != ERROR_SUCCESS)
                nResultError = nError;
        }

        // Save HET table, BET table, hash table, block table, hi-block table
        if(ha->dwFlags & MPQ_FLAG_CHANGED)
        {
            // Rebuild the HET table
            if(ha->pHetTable != NULL)
                RebuildHetTable(ha);

            // Save all MPQ tables first
            nError = SaveMPQTables(ha);
            if(nError != ERROR_SUCCESS)
                nResultError = nError;

            // If the archive has weak signature, we need to finish it
            if(ha->dwFileFlags3 != 0)
            {
#ifdef FULL
                nError = SSignFileFinish(ha);
                if(nError != ERROR_SUCCESS)
                    nResultError = nError;
#else
				assert(0);
#endif
            }
        }

        // We are no longer saving internal MPQ structures
        ha->dwFlags &= ~MPQ_FLAG_SAVING_TABLES;
    }

    // Return the error
    if(nResultError != ERROR_SUCCESS)
        SetLastError(nResultError);
    return (nResultError == ERROR_SUCCESS);
}
#endif

//-----------------------------------------------------------------------------
// bool SFileCloseArchive(HANDLE hMpq);
//

bool STORMAPI SFileCloseArchive(HANDLE hMpq)
{
    TMPQArchive * ha = IsValidMpqHandle(hMpq);
    bool bResult = true;

    // Only if the handle is valid
    if(ha == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return false;
    }

    // Invalidate the add file callback so it won't be called
    // when saving (listfile) and (attributes)
    ha->pfnAddFileCB = NULL;
    ha->pvAddFileUserData = NULL;

#ifdef FULL
    // Flush all unsaved data to the storage
    bResult = SFileFlushArchive(hMpq);
#endif

    // Free all memory used by MPQ archive
    FreeArchiveHandle(ha);
    return bResult;
}
