/*****************************************************************************/
/* SFileFindFile.cpp                      Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* A module for file searching within MPQs                                   */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 25.03.03  1.00  Lad  The first version of SFileFindFile.cpp               */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "StormCommon.h"

//-----------------------------------------------------------------------------
// Private structure used for file search (search handle)

// Used by searching in MPQ archives
struct TMPQSearch
{
    TMPQArchive * ha;                   // Handle to MPQ, where the search runs
    TFileEntry ** pSearchTable;         // Table for files that have been already found
    DWORD  dwSearchTableItems;          // Number of items in the search table
    DWORD  dwNextIndex;                 // Next file index to be checked
    DWORD  dwFlagMask;                  // For checking flag mask
    char   szSearchMask[1];             // Search mask (variable length)
};

//-----------------------------------------------------------------------------
// Local functions

static TMPQSearch * IsValidSearchHandle(HANDLE hFind)
{
    TMPQSearch * hs = (TMPQSearch *)hFind;

    if(hs != NULL && IsValidMpqHandle(hs->ha))
        return hs;

    return NULL;
}

bool SFileCheckWildCard(const char * szString, const char * szWildCard)
{
    const char * szWildCardPtr;

    for(;;)
    {
        // If there is '?' in the wildcard, we skip one char
        while(szWildCard[0] == '?')
        {
            if(szString[0] == 0)
                return false;

            szWildCard++;
            szString++;
        }

        // Handle '*'
        szWildCardPtr = szWildCard;
        if(szWildCardPtr[0] != 0)
        {
            if(szWildCardPtr[0] == '*')
            {
                szWildCardPtr++;

                if(szWildCardPtr[0] == '*')
                    continue;

                if(szWildCardPtr[0] == 0)
                    return true;

                if(AsciiToUpperTable[szWildCardPtr[0]] == AsciiToUpperTable[szString[0]])
                {
                    if(SFileCheckWildCard(szString, szWildCardPtr))
                        return true;
                }
            }
            else
            {
                if(AsciiToUpperTable[szWildCardPtr[0]] != AsciiToUpperTable[szString[0]])
                    return false;

                szWildCard = szWildCardPtr + 1;
            }

            if(szString[0] == 0)
                return false;
            szString++;
        }
        else
        {
            return (szString[0] == 0) ? true : false;
        }
    }
}

static DWORD GetSearchTableItems(TMPQArchive * ha)
{
    DWORD dwMergeItems = 0;

    // Loop over all patches
    while(ha != NULL)
    {
        // Append the number of files
        dwMergeItems += (ha->pHetTable != NULL) ? ha->pHetTable->dwEntryCount
                                                : ha->pHeader->dwBlockTableSize;
        // Move to the patched archive
        ha = ha->haPatch;
    }

    // Return the double size of number of items
    return (dwMergeItems | 1);
}

static bool FileWasFoundBefore(
    TMPQArchive * ha,
    TMPQSearch * hs,
    TFileEntry * pFileEntry)
{
    TFileEntry * pEntry;
    char * szRealFileName = pFileEntry->szFileName;
    DWORD dwStartIndex;
    DWORD dwNameHash;
    DWORD dwIndex;

    if(hs->pSearchTable != NULL && szRealFileName != NULL)
    {
        // If we are in patch MPQ, we check if patch prefix matches
        // and then trim the patch prefix
        if(ha->pPatchPrefix != NULL)
        {
            // If the patch prefix doesn't fit, we pretend that the file
            // was there before and it will be skipped
            if(_strnicmp(szRealFileName, ha->pPatchPrefix->szPatchPrefix, ha->pPatchPrefix->nLength))
                return true;

            szRealFileName += ha->pPatchPrefix->nLength;
        }

        // Calculate the hash to the table
        dwNameHash = ha->pfnHashString(szRealFileName, MPQ_HASH_NAME_A);
        dwStartIndex = dwIndex = (dwNameHash % hs->dwSearchTableItems);

        // The file might have been found before
        // only if this is not the first MPQ being searched
        if(ha->haBase != NULL)
        {
            // Enumerate all entries in the search table
            for(;;)
            {
                // Get the file entry at that position
                pEntry = hs->pSearchTable[dwIndex];
                if(pEntry == NULL)
                    break;

                if(pEntry->szFileName != NULL)
                {
                    // Does the name match?
                    if(!_stricmp(pEntry->szFileName, szRealFileName))
                        return true;
                }

                // Move to the next entry
                dwIndex = (dwIndex + 1) % hs->dwSearchTableItems;
                if(dwIndex == dwStartIndex)
                    break;
            }
        }

        // Put the entry to the table for later use
        hs->pSearchTable[dwIndex] = pFileEntry;
    }
    return false;
}

static TFileEntry * FindPatchEntry(TMPQArchive * ha, TFileEntry * pFileEntry)
{
    TFileEntry * pPatchEntry = pFileEntry;
    TFileEntry * pTempEntry;
    char szFileName[MAX_PATH+1];

    // Can't find patch entry for a file that doesn't have name
    if(pFileEntry->szFileName != NULL && pFileEntry->szFileName[0] != 0)
    {
        // Go while there are patches
        while(ha->haPatch != NULL)
        {
            // Move to the patch archive
            ha = ha->haPatch;
            szFileName[0] = 0;

            // Prepare the prefix for the file name
            if(ha->pPatchPrefix && ha->pPatchPrefix->nLength)
                StringCopy(szFileName, _countof(szFileName), ha->pPatchPrefix->szPatchPrefix);
            StringCat(szFileName, _countof(szFileName), pFileEntry->szFileName);

            // Try to find the file there
            pTempEntry = GetFileEntryExact(ha, szFileName, 0, NULL);
            if(pTempEntry != NULL)
                pPatchEntry = pTempEntry;
        }
    }

    // Return the found patch entry
    return pPatchEntry;
}

static bool DoMPQSearch_FileEntry(
    TMPQSearch * hs,
    SFILE_FIND_DATA * lpFindFileData,
    TMPQArchive * ha,
    TMPQHash * pHashEntry,
    TFileEntry * pFileEntry)
{
    TFileEntry * pPatchEntry;
    HANDLE hFile = NULL;
    const char * szFileName;
    size_t nPrefixLength = (ha->pPatchPrefix != NULL) ? ha->pPatchPrefix->nLength : 0;
    DWORD dwBlockIndex;
    char szNameBuff[MAX_PATH];

    // Is it a file but not a patch file?
    if((pFileEntry->dwFlags & hs->dwFlagMask) == MPQ_FILE_EXISTS)
    {
        // Ignore fake files which are not compressed but have size higher than the archive
        if ((pFileEntry->dwFlags & MPQ_FILE_COMPRESS_MASK) == 0 && (pFileEntry->dwFileSize > ha->FileSize))
            return false;

        // Now we have to check if this file was not enumerated before
        if(!FileWasFoundBefore(ha, hs, pFileEntry))
        {
//          if(pFileEntry != NULL && !_stricmp(pFileEntry->szFileName, "TriggerLibs\\NativeLib.galaxy"))
//              DebugBreak();

            // Find a patch to this file
            // Note: This either succeeds or returns pFileEntry
            pPatchEntry = FindPatchEntry(ha, pFileEntry);

            // Prepare the block index
            dwBlockIndex = (DWORD)(pFileEntry - ha->pFileTable);
            if(dwBlockIndex == 569)
                szNameBuff[0] = 'F';

            // Get the file name. If it's not known, we will create pseudo-name
            szFileName = pFileEntry->szFileName;
            if(szFileName == NULL)
            {
                // Open the file by its pseudo-name.
                StringCreatePseudoFileName(szNameBuff, _countof(szNameBuff), dwBlockIndex, "xxx");
                if(SFileOpenFileEx((HANDLE)hs->ha, szNameBuff, SFILE_OPEN_BASE_FILE, &hFile))
                {
                    SFileGetFileName(hFile, szNameBuff);
                    szFileName = szNameBuff;
                    SFileCloseFile(hFile);
                }
            }

            // If the file name is still NULL, we cannot include the file to search results
            if(szFileName != NULL)
            {
                // Check the file name against the wildcard
                if(SFileCheckWildCard(szFileName + nPrefixLength, hs->szSearchMask))
                {
                    // Fill the found entry. hash entry and block index are taken from the base MPQ
                    lpFindFileData->dwHashIndex  = HASH_ENTRY_FREE;
                    lpFindFileData->dwBlockIndex = dwBlockIndex;
                    lpFindFileData->dwFileSize   = pPatchEntry->dwFileSize;
                    lpFindFileData->dwFileFlags  = pPatchEntry->dwFlags;
                    lpFindFileData->dwCompSize   = pPatchEntry->dwCmpSize;
                    lpFindFileData->lcLocale     = 0;   // pPatchEntry->lcLocale;

                    // Fill the filetime
                    lpFindFileData->dwFileTimeHi = (DWORD)(pPatchEntry->FileTime >> 32);
                    lpFindFileData->dwFileTimeLo = (DWORD)(pPatchEntry->FileTime);

                    // Fill-in the entries from hash table entry, if given
                    if(pHashEntry != NULL)
                    {
                        lpFindFileData->dwHashIndex = (DWORD)(pHashEntry - ha->pHashTable);
                        lpFindFileData->lcLocale = pHashEntry->lcLocale;
                    }

                    // Fill the file name and plain file name
                    StringCopy(lpFindFileData->cFileName, _countof(lpFindFileData->cFileName), szFileName + nPrefixLength);
                    lpFindFileData->szPlainName = (char *)GetPlainFileName(lpFindFileData->cFileName);
                    return true;
                }
            }
        }
    }

    // Either not a valid item or was found before
    return false;
}

static int DoMPQSearch_HashTable(TMPQSearch * hs, SFILE_FIND_DATA * lpFindFileData, TMPQArchive * ha)
{
    TMPQHash * pHashTableEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPQHash * pHash;

    // Parse the file table
    for(pHash = ha->pHashTable + hs->dwNextIndex; pHash < pHashTableEnd; pHash++)
    {
        // Increment the next index for subsequent search
        hs->dwNextIndex++;

        // Does this hash table entry point to a proper block table entry?
        if(IsValidHashEntry(ha, pHash))
        {
            // Check if this file entry should be included in the search result
            if(DoMPQSearch_FileEntry(hs, lpFindFileData, ha, pHash, ha->pFileTable + MPQ_BLOCK_INDEX(pHash)))
                return ERROR_SUCCESS;
        }
    }

    // No more files
    return ERROR_NO_MORE_FILES;
}

static int DoMPQSearch_FileTable(TMPQSearch * hs, SFILE_FIND_DATA * lpFindFileData, TMPQArchive * ha)
{
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pFileEntry;

    // Parse the file table
    for(pFileEntry = ha->pFileTable + hs->dwNextIndex; pFileEntry < pFileTableEnd; pFileEntry++)
    {
        // Increment the next index for subsequent search
        hs->dwNextIndex++;

        // Check if this file entry should be included in the search result
        if(DoMPQSearch_FileEntry(hs, lpFindFileData, ha, NULL, pFileEntry))
            return ERROR_SUCCESS;
    }

    // No more files
    return ERROR_NO_MORE_FILES;
}

// Performs one MPQ search
static int DoMPQSearch(TMPQSearch * hs, SFILE_FIND_DATA * lpFindFileData)
{
    TMPQArchive * ha = hs->ha;
    int nError;

    // Start searching with base MPQ
    while(ha != NULL)
    {
        // If the archive has hash table, we need to use hash table
        // in order to catch hash table index and file locale.
        // Note: If multiple hash table entries, point to the same block entry,
        // we need, to report them all
        nError = (ha->pHashTable != NULL) ? DoMPQSearch_HashTable(hs, lpFindFileData, ha)
                                          : DoMPQSearch_FileTable(hs, lpFindFileData, ha);
        if(nError == ERROR_SUCCESS)
            return nError;

        // If there is no more patches in the chain, stop it.
        // This also keeps hs->ha non-NULL, which is required
        // for freeing the handle later
        if(ha->haPatch == NULL)
            break;

        // Move to the next patch in the patch chain
        hs->ha = ha = ha->haPatch;
        hs->dwNextIndex = 0;
    }

    // No more files found, return error
    return ERROR_NO_MORE_FILES;
}

static void FreeMPQSearch(TMPQSearch *& hs)
{
    if(hs != NULL)
    {
        if(hs->pSearchTable != NULL)
            STORM_FREE(hs->pSearchTable);
        STORM_FREE(hs);
        hs = NULL;
    }
}

//-----------------------------------------------------------------------------
// Public functions

HANDLE WINAPI SFileFindFirstFile(HANDLE hMpq, const char * szMask, SFILE_FIND_DATA * lpFindFileData, const TCHAR * szListFile)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;
    TMPQSearch * hs = NULL;
    size_t nSize  = 0;
    int nError = ERROR_SUCCESS;

    // Check for the valid parameters
    if(!IsValidMpqHandle(hMpq))
        nError = ERROR_INVALID_HANDLE;
    if(szMask == NULL || lpFindFileData == NULL)
        nError = ERROR_INVALID_PARAMETER;

    // Include the listfile into the MPQ's internal listfile
    // Note that if the listfile name is NULL, do nothing because the
    // internal listfile is always included.
    if(nError == ERROR_SUCCESS && szListFile != NULL && *szListFile != 0)
        nError = SFileAddListFile((HANDLE)ha, szListFile);

    // Allocate the structure for MPQ search
    if(nError == ERROR_SUCCESS)
    {
        nSize = sizeof(TMPQSearch) + strlen(szMask) + 1;
        if((hs = (TMPQSearch *)STORM_ALLOC(char, nSize)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Perform the first search
    if(nError == ERROR_SUCCESS)
    {
        memset(hs, 0, sizeof(TMPQSearch));
        strcpy(hs->szSearchMask, szMask);
        hs->dwFlagMask = MPQ_FILE_EXISTS;
        hs->ha = ha;

        // If the archive is patched archive, we have to create a merge table
        // to prevent files being repeated
        if(ha->haPatch != NULL)
        {
            hs->dwSearchTableItems = GetSearchTableItems(ha);
            hs->pSearchTable = STORM_ALLOC(TFileEntry *, hs->dwSearchTableItems);
            hs->dwFlagMask = MPQ_FILE_EXISTS | MPQ_FILE_PATCH_FILE;
            if(hs->pSearchTable != NULL)
                memset(hs->pSearchTable, 0, hs->dwSearchTableItems * sizeof(TFileEntry *));
            else
                nError = ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    // Perform first item searching
    if(nError == ERROR_SUCCESS)
    {
        nError = DoMPQSearch(hs, lpFindFileData);
    }

    // Cleanup
    if(nError != ERROR_SUCCESS)
    {
        FreeMPQSearch(hs);
        SetLastError(nError);
    }

    // Return the result value
    return (HANDLE)hs;
}

bool WINAPI SFileFindNextFile(HANDLE hFind, SFILE_FIND_DATA * lpFindFileData)
{
    TMPQSearch * hs = IsValidSearchHandle(hFind);
    int nError = ERROR_SUCCESS;

    // Check the parameters
    if(hs == NULL)
        nError = ERROR_INVALID_HANDLE;
    if(lpFindFileData == NULL)
        nError = ERROR_INVALID_PARAMETER;

    if(nError == ERROR_SUCCESS)
        nError = DoMPQSearch(hs, lpFindFileData);

    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

bool WINAPI SFileFindClose(HANDLE hFind)
{
    TMPQSearch * hs = IsValidSearchHandle(hFind);

    // Check the parameters
    if(hs == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return false;
    }

    FreeMPQSearch(hs);
    return true;
}
