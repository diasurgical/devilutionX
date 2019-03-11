// ------------------------------------------------------------------------------------------------

#include <windows.h>

#include <string>
#include <vector>

#include <IIEP_FileIn.H>
#include <IIEP_Hash.H>
#include <IIEP_String.H>

// ------------------------------------------------------------------------------------------------

struct MEM_FILE_IDX
{
  MEM_FILE_IDX(void)
  {
    m_dwID     = 0;
    m_dwLength = 0;
    m_pucData  = 0;
  }

  DWORD m_dwID;     // hash ID of filename
  DWORD m_dwLength; // file length
  PBYTE m_pucData;  // memory pointer
};

static std::vector <MEM_FILE_IDX> s_vecMemFile;

// ------------------------------------------------------------------------------------------------
// name: SearchMemFile()
// desc:
// ------------------------------------------------------------------------------------------------

static std::vector <MEM_FILE_IDX> :: iterator SearchMemFile(PCWSTR pcwsFileName)
{
  DWORD dwID = IIEP::HashFNV_1a(pcwsFileName);

  std::vector <MEM_FILE_IDX>::iterator it;

  for (it = s_vecMemFile.begin(); it < s_vecMemFile.end(); it ++)
  {
      if (it -> m_dwID == dwID) return it;
  }

  return s_vecMemFile.end();
}

// ------------------------------------------------------------------------------------------------

static std::vector <MEM_FILE_IDX> :: iterator SearchMemFile(const char *pcszFileName)
{
  WORD wzNameBuffer[0x1000];

  wzNameBuffer[0] = 0;

  IIEP::StringFromANSI(pcszFileName, wzNameBuffer, 0x1000);

  return SearchMemFile(wzNameBuffer);
}

// ------------------------------------------------------------------------------------------------
// name: CreateMemoryFile()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CreateMemoryFile(PVOID pMemory, DWORD dwLength, PCWSTR pcwsFileName)
{
  if (0 == pcwsFileName || 0 == *pcwsFileName) return false;

  RemoveMemoryFile(pcwsFileName);

  DWORD dwID = IIEP::HashFNV_1a(pcwsFileName);

  MEM_FILE_IDX soIDX;

  soIDX.m_dwID     = dwID;
  soIDX.m_dwLength = dwLength;
  soIDX.m_pucData  = (PBYTE) pMemory;

  s_vecMemFile.push_back(soIDX);

  return true;
}

// ------------------------------------------------------------------------------------------------

bool IIEP::CreateMemoryFile(PVOID pMemory, DWORD dwLength, const char *pcszFileName)
{
  WORD wzNameBuffer[0x1000];

  wzNameBuffer[0] = 0;

  StringFromANSI(pcszFileName, wzNameBuffer, 0x1000);

  return CreateMemoryFile(pMemory, dwLength, wzNameBuffer);
}

// ------------------------------------------------------------------------------------------------
// name: RemoveMemoryFile()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::RemoveMemoryFile(PCWSTR pcwsFileName)
{
  if (0 == pcwsFileName || 0 == *pcwsFileName) return false;

  std::vector <MEM_FILE_IDX>::iterator it = SearchMemFile(pcwsFileName);

  if (it == s_vecMemFile.end()) return false;

  s_vecMemFile.erase(it);

  return true;
}

// ------------------------------------------------------------------------------------------------

bool IIEP::RemoveMemoryFile(const char *pcszFileName)
{
  WORD wzNameBuffer[0x1000];

  wzNameBuffer[0] = 0;

  StringFromANSI(pcszFileName, wzNameBuffer, 0x1000);

  return RemoveMemoryFile(wzNameBuffer);
}

// ------------------------------------------------------------------------------------------------

void IIEP::RemoveMemoryFile(void)
{
  s_vecMemFile.clear();
}

// ------------------------------------------------------------------------------------------------
// name: CFileIn()
// desc:
// ------------------------------------------------------------------------------------------------

IIEP::CFileIn::CFileIn(void)
{
  m_pFile      = 0;
  m_pucMemData = 0;
  m_dwLength   = 0;
  m_dwReadPos  = 0;
}

// ------------------------------------------------------------------------------------------------
// name: ~CFileIn()
// desc:
// ------------------------------------------------------------------------------------------------

IIEP::CFileIn::~CFileIn(void)
{
  Close();
}

// ------------------------------------------------------------------------------------------------
// name: Open()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CFileIn::Open(const char *pcszFileName)
{
  WORD wzNameBuffer[0x1000];

  wzNameBuffer[0] = 0;

  StringFromANSI(pcszFileName, wzNameBuffer, 0x1000);

  return Open(wzNameBuffer);
}

// ------------------------------------------------------------------------------------------------

bool IIEP::CFileIn::Open(const WORD *pcwsFileName)
{
  Close();

  if (0 == pcwsFileName || 0 == *pcwsFileName) return false;

  std::vector <MEM_FILE_IDX>::iterator it = SearchMemFile(pcwsFileName);

  if (it == s_vecMemFile.end()) // not found in memory
  {
    m_pFile = _wfopen(pcwsFileName, L"rb");

    if (m_pFile)
    {
      return true;  // found DOS file
    }

    return false;
  }

  if (s_vecMemFile.empty()) return false;

  m_pucMemData = it -> m_pucData;
  m_dwLength   = it -> m_dwLength;

  return true;
}

// ------------------------------------------------------------------------------------------------
// name: Close()
// desc:
// ------------------------------------------------------------------------------------------------

void IIEP::CFileIn::Close(void)
{
  if (m_pFile)
  {
    fclose(m_pFile);

    m_pFile = 0;
  }

  m_pucMemData = 0;
  m_dwLength   = 0;
  m_dwReadPos  = 0;
}

// ------------------------------------------------------------------------------------------------
// name: GetFileSize()
// desc:
// ------------------------------------------------------------------------------------------------

DWORD IIEP::CFileIn::GetFileSize(void) const
{
  if (m_pFile)
  {
    DWORD dwPostion = ftell(m_pFile);

    fseek(m_pFile, 0, SEEK_END);

    DWORD dwFileSize = ftell(m_pFile);

    fseek(m_pFile, dwPostion, SEEK_SET);

    return dwFileSize;
  }

  return m_dwLength;
}

// ------------------------------------------------------------------------------------------------
// name: GetReadPosition()
// desc:
// ------------------------------------------------------------------------------------------------

DWORD IIEP::CFileIn::GetReadPosition(void) const
{
  if (m_pFile)
  {
    return ftell(m_pFile);
  }

  return m_dwReadPos;
}

// ------------------------------------------------------------------------------------------------
// name: Seek()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CFileIn::Seek(long nOffset, long nOrigin)
{
  if (m_pFile)
  {
    return (0 == fseek(m_pFile, nOffset, nOrigin));
  }

  if (m_pucMemData)
  {
    switch (nOrigin)
    {
    case SEEK_SET:
      if (nOffset < 0 || nOffset >= (long) m_dwLength) return false;
      m_dwReadPos = nOffset;
      return true;

    case SEEK_CUR:
      nOffset += m_dwReadPos;
      if (nOffset < 0 || nOffset >= (long) m_dwLength) return false;
      m_dwReadPos = nOffset;
      return true;

    case SEEK_END:
      ASSERT(nOffset <= 0);   // 这个必须是负数
      nOffset = m_dwLength - 1 + nOffset;
      if (nOffset < 0 || nOffset >= (long) m_dwLength) return false;
      m_dwReadPos = nOffset;
      return true;
    }
  }

  return false;
}

// ------------------------------------------------------------------------------------------------
// name: Read()
// desc:
// ------------------------------------------------------------------------------------------------

DWORD IIEP::CFileIn::Read(PVOID pDataBuffer, DWORD dwReadSize)
{
  if (m_pFile)
  {
    return (DWORD) fread(pDataBuffer, 1, dwReadSize, m_pFile);
  }

  if (m_pucMemData)
  {
    if (m_dwReadPos + dwReadSize > m_dwLength) dwReadSize = m_dwLength - m_dwReadPos;

    memcpy(pDataBuffer, m_pucMemData + m_dwReadPos, dwReadSize);

    m_dwReadPos += dwReadSize;

    return dwReadSize;
  }

  return 0;
}

// ------------------------------------------------------------------------------------------------
// name: ReadAnsiLine()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CFileIn::ReadAnsiLine(std::string &strLine)
{
  BYTE ucChar = 0;

  strLine.clear();

  while (ReadB(ucChar))
  {
    if (0x0D == ucChar)
    {
    }
    else
    if (0x0A == ucChar)
    {
      return true;
    }
    else
    {
      strLine.push_back(ucChar);
    }
  }

  if (strLine.empty()) return false;

  return true;
}

// ------------------------------------------------------------------------------------------------
// name: ReadUnicodeLine()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CFileIn::ReadUnicodeLine(std::wstring &wstrLine, bool bBigEndian)
{
  WORD wChar  = 0;

  wstrLine.clear();

  while (bBigEndian ? ReadInverseW(wChar) : ReadW(wChar))
  {
    if (0x000D == wChar)
    {
    }
    else
    if (0x000A == wChar)
    {
      return true;
    }
    else
    {
      wstrLine.push_back(wChar);
    }
  }

  if (wstrLine.empty()) return false;

  return true;
}

// ------------------------------------------------------------------------------------------------
// name: ReadAnsiString()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CFileIn::ReadAnsiString(char *pszBuffer, DWORD dwBufferLength)
{
  DWORD dwIndex = 0;
  BYTE  ucChar;

  while (ReadB(ucChar))
  {
    if (dwIndex >= dwBufferLength) return true;

    pszBuffer[dwIndex ++] = ucChar;

    if (0 == ucChar)
    {
      return true;
    }
  }

  return false;
}

// ------------------------------------------------------------------------------------------------
// name: ReadUnicodeString()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CFileIn::ReadUnicodeString(WORD *pwzBuffer, DWORD dwBufferLength)
{
  DWORD dwIndex = 0;
  WORD  wChar;

  ReadInverseW(wChar);

  if (wChar == 0xFFFE)  // Unicode
  {
    while (ReadW(wChar))
    {
      if (dwIndex >= dwBufferLength) return true;

      pwzBuffer[dwIndex ++] = wChar;

      if (0 == wChar)
      {
        return true;
      }
    }
  }
  else
  if (wChar == 0xFEFF)  // Unicode big endian
  {
    while (ReadInverseW(wChar))
    {
      if (dwIndex >= dwBufferLength) return true;

      pwzBuffer[dwIndex ++] = wChar;

      if (0 == wChar)
      {
        return true;
      }
    }
  }

  return false;
}

// ------------------------------------------------------------------------------------------------