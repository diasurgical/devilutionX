// ------------------------------------------------------------------------------------------------

#include <IIEP_Def.H>

#include <CORE/SourceFilter_MP3.H>

// ------------------------------------------------------------------------------------------------

extern BYTE g_ucWaveHeader[WAVE_HEADER_LENGTH];

// ------------------------------------------------------------------------------------------------
// name: CMP3Stream()
// desc:
// ------------------------------------------------------------------------------------------------

static size_t read_sub(int fd, void *pBuf, size_t nSize)
{
  if (fd)
  {
    IIEP::CMP3Stream *pcoStream = (IIEP::CMP3Stream *) fd;

    return pcoStream -> ReadFile(pBuf, nSize);
  }

  return 0;
}

// ------------------------------------------------------------------------------------------------
// name: CMP3Stream()
// desc:
// ------------------------------------------------------------------------------------------------

static off_t seek_sub(int fd, off_t nOffset, int iOrigin)
{
  if (fd)
  {
    IIEP::CMP3Stream *pcoStream = (IIEP::CMP3Stream *) fd;

    return pcoStream -> SeekFile(nOffset, iOrigin);
  }

  return 0;
}

// ------------------------------------------------------------------------------------------------
// name: CMP3Stream()
// desc:
// ------------------------------------------------------------------------------------------------

IIEP::CMP3Stream::CMP3Stream(void)
{
  m_dwKBPerSec    = INFINITE;
  m_dwTimeStart   = 0;

  m_pcoMH         = 0;

  m_dwTotalBlocks = 0;
  m_dwAlignment   = 4;

  m_dwLength      = 0;
  m_dwPosition    = 0;

  memcpy(m_ucHeader, g_ucWaveHeader, WAVE_HEADER_LENGTH);

  m_dwDataBufPos  = 0;
  m_dwDataBufLen  = 0;

  m_bSeekFlag     = false;
}

// ------------------------------------------------------------------------------------------------
// name: ~CMP3Stream()
// desc:
// ------------------------------------------------------------------------------------------------

IIEP::CMP3Stream::~CMP3Stream(void)
{
  Close();
}

// ------------------------------------------------------------------------------------------------
// name: SetSeekFlag()
// desc: called by IIEP::CPlayer::SetMediaPosition(QWORD qwPosition)
// ------------------------------------------------------------------------------------------------

void IIEP::CMP3Stream::SetSeekFlag(void)
{
  m_bSeekFlag = true;
}

// ------------------------------------------------------------------------------------------------
// name: Open()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CMP3Stream::Open(const WORD *pcwsFileName, CMediaType &coMT, DWORD dwKBPerSec)
{
  Close();

  //

  if (MPG123_OK != mpg123_init())
  {
    goto OPEN_FAIL;
  }

  m_pcoMH = mpg123_new(NULL, NULL);

  if (0 == m_pcoMH)
  {
    goto OPEN_FAIL;
  }

  if (false == OpenFile(pcwsFileName))
  {
    goto OPEN_FAIL;
  }

  if (MPG123_OK != mpg123_replace_reader(m_pcoMH, read_sub, seek_sub))
  {
    goto OPEN_FAIL;
  }

  if (MPG123_OK != mpg123_open_fd(m_pcoMH, (int) this))
  {
    goto OPEN_FAIL;
  }

  long nRate;
  int  iChannels;
  int  iEncoding;

  if (MPG123_OK != mpg123_getformat(m_pcoMH, &nRate, &iChannels, &iEncoding))
  {
    goto OPEN_FAIL;
  }

  //  Ensure that this output format will not change (it could, when we allow it)

  mpg123_format_none(m_pcoMH);
	mpg123_format(m_pcoMH, nRate, iChannels, iEncoding);

  // scan & get total samples

  mpg123_scan(m_pcoMH);

  off_t nOff = mpg123_length(m_pcoMH); 

  if (nOff < 0)
  {
    goto OPEN_FAIL;
  }

  m_dwTotalBlocks = (DWORD) nOff;

  // set other format

  m_dwAlignment = iChannels * 2;
  
  if (0 == m_dwAlignment)
  {
    m_dwAlignment = 4;
  }

  m_dwLength = m_dwTotalBlocks * m_dwAlignment + WAVE_HEADER_LENGTH;

  WORD  wChannels        = (WORD ) iChannels;
  DWORD dwSamplesPerSec  = (DWORD) nRate;
  DWORD dwAvgBytesPerSec =         m_dwAlignment * dwSamplesPerSec;
  WORD  wBlockAlign      = (WORD ) m_dwAlignment;
  WORD  wBitsPerSample   =         16;

  PWORD  pwData;
  PDWORD pdwData;
  
  pdwData = (PDWORD) &m_ucHeader[0x04];
 *pdwData = m_dwTotalBlocks * m_dwAlignment + WAVE_HEADER_LENGTH - 8;

  pwData = (PWORD) &m_ucHeader[0x16];
 *pwData = wChannels;

  pdwData = (PDWORD) &m_ucHeader[0x18];
 *pdwData = dwSamplesPerSec;

  pdwData = (PDWORD) &m_ucHeader[0x1C];
 *pdwData = dwAvgBytesPerSec;

  pwData = (PWORD) &m_ucHeader[0x20];
 *pwData = wBlockAlign;

  pwData = (PWORD) &m_ucHeader[0x22];
 *pwData = wBitsPerSample;

  pdwData = (PDWORD) &m_ucHeader[0x28];
 *pdwData = m_dwTotalBlocks * m_dwAlignment;

  coMT.majortype = MEDIATYPE_Stream;
  coMT.subtype   = MEDIASUBTYPE_WAVE;

  SetPointer(0);

  m_dwTimeStart = ::timeGetTime();

  return true;

OPEN_FAIL:

  Close();

  return false;
}

// ------------------------------------------------------------------------------------------------
// name: Close()
// desc:
// ------------------------------------------------------------------------------------------------

void IIEP::CMP3Stream::Close(void)
{
  m_coFileIn.Close();

  if (m_pcoMH)
  {
	  mpg123_close (m_pcoMH);
	  mpg123_delete(m_pcoMH);

    m_pcoMH = 0;
  }

  mpg123_exit(); 

  m_dwTimeStart   = 0;
  m_dwTotalBlocks = 0;

  m_dwLength   = 0;
  m_dwPosition = 0;

  m_bSeekFlag = false;

  m_nDataBegin = 0;
  m_nDataTotal = 0;
}

// ------------------------------------------------------------------------------------------------
// name: SetPointer()
// desc:
// ------------------------------------------------------------------------------------------------

HRESULT IIEP::CMP3Stream::SetPointer(LONGLONG llPos)
{
  if (llPos < 0 || llPos > (LONGLONG) m_dwLength)
  {
    return S_FALSE;
  }

  m_dwPosition = (DWORD) llPos;

  if (llPos < WAVE_HEADER_LENGTH)
  {
    mpg123_seek(m_pcoMH, 0, SEEK_SET);

    m_dwDataBufLen = 0;
  }
  else
  {
    if (m_bSeekFlag && m_dwAlignment > 0)
    {
      m_bSeekFlag = false;

      mpg123_seek(m_pcoMH, (long) (m_dwPosition - WAVE_HEADER_LENGTH) / m_dwAlignment, SEEK_SET);

      m_dwDataBufLen = 0;
    }
  }

  return S_OK;
}

// ------------------------------------------------------------------------------------------------
// name: Read()
// desc:
// ------------------------------------------------------------------------------------------------

HRESULT IIEP::CMP3Stream::Read(PBYTE   pucBuffer,
                               DWORD   dwBytesToRead,
                               BOOL    bAlign,
                               LPDWORD pdwBytesRead)
{
  if (0 == m_pcoMH) return S_FALSE;

  CAutoLock lck(&m_csLock);

  DWORD dwReadLength;

  // wait until the bytes are here

  DWORD dwTime = ::timeGetTime();

  if (m_dwPosition + dwBytesToRead > m_dwLength)
  {
    dwReadLength = m_dwLength - m_dwPosition;
  }
  else
  {
    dwReadLength = dwBytesToRead;
  }

  DWORD dwTimeToArrive = (m_dwPosition + dwReadLength) / m_dwKBPerSec;

  if (dwTime - m_dwTimeStart < dwTimeToArrive)
  {
    ::Sleep(dwTimeToArrive - dwTime + m_dwTimeStart);
  }

  // read data
	
  DWORD dwRead = 0;

  if (m_dwPosition < WAVE_HEADER_LENGTH)
  {
    DWORD dwPatchLen = WAVE_HEADER_LENGTH - m_dwPosition;

    if (dwReadLength < dwPatchLen)
    {
      dwPatchLen = dwReadLength;
    }

    memcpy(pucBuffer, m_ucHeader + m_dwPosition, dwPatchLen);

    m_dwPosition += dwPatchLen;
    dwReadLength -= dwPatchLen;

    pucBuffer += dwPatchLen;
    dwRead    += dwPatchLen;
  }

  if (dwReadLength > 0)
  {
    while (m_dwDataBufLen < dwReadLength)
    {
      if (m_dwDataBufLen > 0)
      {
        memcpy(pucBuffer, m_ucDataBuffer + m_dwDataBufPos, m_dwDataBufLen);

        dwRead         += m_dwDataBufLen;

        dwReadLength   -= m_dwDataBufLen;

        pucBuffer      += m_dwDataBufLen;
        m_dwDataBufPos += m_dwDataBufLen;
        m_dwDataBufLen =  0;
      }

      if (DecodeFrame() == false) break;
    }

    if (dwReadLength > 0)
    {
      if (m_dwDataBufLen >= dwReadLength)
      {
        memcpy(pucBuffer, m_ucDataBuffer + m_dwDataBufPos, dwReadLength);

        dwRead         += dwReadLength;

        m_dwDataBufPos += dwReadLength;
        m_dwDataBufLen -= dwReadLength;
      }
    }
  }

  m_dwPosition += dwRead;
  *pdwBytesRead = dwRead;

  return S_OK;
}

// ------------------------------------------------------------------------------------------------
// name: Size()
// desc:
// ------------------------------------------------------------------------------------------------

LONGLONG IIEP::CMP3Stream::Size(LONGLONG *pSizeAvailable)
{
//  LONGLONG llCurrentAvailable = Int32x32To64((::timeGetTime() - m_dwTimeStart), m_dwKBPerSec);
  LONGLONG llLength = (LONGLONG) m_dwTotalBlocks * m_dwAlignment + WAVE_HEADER_LENGTH;

  if (pSizeAvailable)
  {
//  *pSizeAvailable = min(llLength, llCurrentAvailable);
    *pSizeAvailable = llLength;
  }

  return llLength;
}

// ------------------------------------------------------------------------------------------------
// name: Alignment()
// desc:
// ------------------------------------------------------------------------------------------------

DWORD IIEP::CMP3Stream::Alignment(void)
{
  return m_dwAlignment;
}

// ------------------------------------------------------------------------------------------------
// name: Lock()
// desc:
// ------------------------------------------------------------------------------------------------

void IIEP::CMP3Stream::Lock(void)
{
  m_csLock.Lock();
}

// ------------------------------------------------------------------------------------------------
// name: Unlock()
// desc:
// ------------------------------------------------------------------------------------------------

void IIEP::CMP3Stream::Unlock(void)
{
  m_csLock.Unlock();
}

// ------------------------------------------------------------------------------------------------
// name: DecodeFrame()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CMP3Stream::DecodeFrame(void)
{
  if (0 == m_dwDataBufLen)
  {
    m_dwDataBufPos = 0;
  }

  if (m_dwDataBufPos + m_dwDataBufLen + MP3_DECODE_LEN > MP3_DECODE_BUF_LEN)
  {
    return true;
  }

  size_t nBytesRead = 0;

  mpg123_read(m_pcoMH, m_ucDataBuffer + m_dwDataBufPos + m_dwDataBufLen, MP3_DECODE_LEN, &nBytesRead);

  m_dwDataBufLen += (DWORD) nBytesRead;

  return (m_dwDataBufLen > 0);
}

// ------------------------------------------------------------------------------------------------
// name: OpenFile()
// desc:
// ------------------------------------------------------------------------------------------------

bool IIEP::CMP3Stream::OpenFile(const WORD *pcwsFileName)
{
  if (m_coFileIn.Open(pcwsFileName))
  {
    m_nDataBegin = 0;
    m_nDataTotal = m_coFileIn.GetFileSize();

    // check ID3 Tag V2.x

    BYTE ucBuffer[8];
    bool bFound;

TRY_ID3_AGAIN:

    bFound = false;

    if (6 == m_coFileIn.Read(ucBuffer, 6))
    {
      if (ucBuffer[0] == 'I' &&
          ucBuffer[1] == 'D' &&
          ucBuffer[2] == '3')
      {
        bFound = true;

        DWORD dwHeaderSize;

        if (m_coFileIn.ReadInverseDW(dwHeaderSize))
        {
          dwHeaderSize = ((dwHeaderSize & 0x7F000000) >> 3) +
                         ((dwHeaderSize & 0x007F0000) >> 2) +
                         ((dwHeaderSize & 0x00007F00) >> 1) +
                         ((dwHeaderSize & 0x0000007F)     );

          dwHeaderSize += 10;

          m_nDataBegin += dwHeaderSize;
          m_nDataTotal -= dwHeaderSize;

          if (ucBuffer[3] >= 0x03)  // ID3 Tag version >= 2.3
          {
            if (ucBuffer[5] & 0x40) // has extended header
            {
              if (m_coFileIn.ReadInverseDW(dwHeaderSize))
              {
                dwHeaderSize = ((dwHeaderSize & 0x7F000000) >> 3) +
                               ((dwHeaderSize & 0x007F0000) >> 2) +
                               ((dwHeaderSize & 0x00007F00) >> 1) +
                               ((dwHeaderSize & 0x0000007F)     );

                dwHeaderSize += 4;

                m_nDataBegin += dwHeaderSize;
                m_nDataTotal -= dwHeaderSize;
              }
            }
          }
        }
      }
    }

    SeekFile(0, SEEK_SET);

    if (bFound)
    {
      goto TRY_ID3_AGAIN;
    }
/*
    m_coFileIn.Read(ucBuffer, 2);

    if (ucBuffer[0] != 0xFF || ucBuffer[1] < 0xF0)
    {
      m_nDataBegin ++;
      m_nDataTotal --;

      SeekFile(0, SEEK_SET);

      goto TRY_ID3_AGAIN;
    }

    SeekFile(0, SEEK_SET);
*/
    return true;
  }

  return false;
}

// ------------------------------------------------------------------------------------------------
// name: ReadFile()
// desc:
// ------------------------------------------------------------------------------------------------

DWORD IIEP::CMP3Stream::ReadFile(PVOID pBuf, DWORD dwLen)
{
  return m_coFileIn.Read(pBuf, dwLen);
}

// ------------------------------------------------------------------------------------------------
// name: SeekFile()
// desc:
// ------------------------------------------------------------------------------------------------

long IIEP::CMP3Stream::SeekFile(long nOffset, int iOrigin)
{
  switch (iOrigin)
  {
  case SEEK_SET:
    if (nOffset < 0 || nOffset >= m_nDataTotal) return -1;
    break;

  case SEEK_CUR:
    nOffset += (m_coFileIn.GetReadPosition() - m_nDataBegin);
    if (nOffset < 0 || nOffset >= m_nDataTotal) return -1;
    break;

  case SEEK_END:
    nOffset = m_nDataTotal - 1 + nOffset;
    if (nOffset < 0 || nOffset >= m_nDataTotal) return -1;
    break;
  }

  if (m_coFileIn.Seek(nOffset + m_nDataBegin, SEEK_SET))
  {
    return nOffset;
  }

  return -1;
}

// ------------------------------------------------------------------------------------------------