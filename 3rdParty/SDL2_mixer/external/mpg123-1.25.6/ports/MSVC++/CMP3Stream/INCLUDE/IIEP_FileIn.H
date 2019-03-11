//
//                         __ __ __ __   __ __ __ __   __ __ __ __   __ __ __ __
//                        /_//_//_//_/\ /_//_//_//_/\ /_//_//_//_/\ /_//_//_//_/\
//                        \_/_//_/\\_\/ \_/_//_/\\_\//_/\\_\\_\\_\//_/\\_\\_/_/\/
//                         /_//_/\/      /_//_/\/   /_/\/ __      /_/\/ __ /_/\/
//                        /_//_/\/      /_//_/\/   /_//_//_/\    /_//_//_//_/\/
//                       /_//_/\/      /_//_/\/   /_/\\_\\_\/   /_/\\_\\_\\_\/
//                   __ /_//_/\/   __ /_//_/\/   /_/\/ __ __   /_/\/
//                  /_//_//_//_/\ /_//_//_//_/\ /_//_//_//_/\ /_/\/
//                  \_\\_\\_\\_\/ \_\\_\\_\\_\/ \_\\_\\_\\_\/ \_\/
//                                                    
//
//                     ..::  Interactive Internet Entertainment Platform  ::..
//
//                         (C) 2000-2005, FAN YI PENG. ALL RIGHTS RESERVED
//
// ________________________________________________________________________________________________
//
/// @file  IIEP_FileIn.H
/// @brief 文件输入接口类
// ________________________________________________________________________________________________
//
//
//                               _____________________________________
//                              |                                     |
//                              |                DATA                 |
// _____________________________|_____________________________________|____________________________
//
//                                            __________
// __________________________________________|  CONFIG  |__________________________________________

#pragma once

// ____________________________________________ CONFIG ____________________________________________
//
//                                            __________
// __________________________________________|  HEADER  |__________________________________________

#include <stdio.h>
#include <string>

#include <IIEP_Def.H>

// ____________________________________________ HEADER ____________________________________________
//
//                                            __________
// __________________________________________|  DEFINE  |__________________________________________
//
//      ___________
// ____/  CFileIn  \_______________________________________________________________________________

namespace IIEP
{
  /// @brief 文件输入接口类

  class CFileIn
  {
    public:

    CFileIn(void);
    ~CFileIn(void);

    bool Open(const char *pcszFileName);
    bool Open(const WORD *pcwsFileName);

    void Close(void);

    DWORD GetFileSize(void) const;
    DWORD GetReadPosition(void) const;

    bool Seek(long nOffset, long nOrigin = 0);  // SEEK_SET = 0     SEEK_CUR = 1     SEEK_END = 2

    bool Skip(long nOffset) { return Seek(nOffset, 1); }

    DWORD Read(PVOID pDataBuffer, DWORD dwReadSize);

    bool ReadB(BYTE &ucData)
    {
      return (1 == Read(&ucData, 1));
    }

    bool ReadW(WORD &wData)
    {
      return (2 == Read(&wData, 2));
    }

    bool ReadDW(DWORD &dwData)
    {
      return (4 == Read(&dwData, 4));
    }

    bool ReadInverseW(WORD &wData)
    {
      if (2 == Read(&wData, 2))
      {
        _asm
        {
          MOV       ECX, wData
          MOV       AX, [ECX]
          XCHG      AH, AL
          MOV       [ECX], AX
        }

        return true;
      }

      return false;
    }

    bool ReadInverseDW(DWORD &dwData)
    {
      if (4 == Read(&dwData, 4))
      {
        _asm
        {
          MOV       ECX, dwData
          MOV       EAX, [ECX]
          BSWAP     EAX
          MOV       [ECX], EAX
        }

        return true;
      }

      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool ReadAnsiLine(std::string &strLine);
    bool ReadUnicodeLine(std::wstring &wstrLine, bool bBigEndian);

    bool ReadAnsiString   (char *pszBuffer, DWORD dwBufferLength);
    bool ReadUnicodeString(WORD *pwzBuffer, DWORD dwBufferLength);

    // --------------------------------------------------------------------------------------------

    private:

    CFileIn(const CFileIn &);				        // forbidden
    CFileIn & operator = (const CFileIn &);	// forbidden

    #include <CORE/CORE_FileIn.H>
  };
}

// ______ CFileIn _________________________________________________________________________________
//
//      _______________
// ____/  Memory File  \___________________________________________________________________________

namespace IIEP
{
  bool CreateMemoryFile(PVOID pMemory, DWORD dwLength, const char *pcszFileName);
  bool CreateMemoryFile(PVOID pMemory, DWORD dwLength, const WORD *pcwsFileName);
  bool RemoveMemoryFile(const char *pcszFileName);
  bool RemoveMemoryFile(const WORD *pcwsFileName);
  void RemoveMemoryFile(void);
}

// ______ Memory File _____________________________________________________________________________
//
// ____________________________________________ DEFINE ____________________________________________
//
// ____________________________________________________________________________________ DATA ______
//
// ___________________________________________ FILE END ___________________________________________