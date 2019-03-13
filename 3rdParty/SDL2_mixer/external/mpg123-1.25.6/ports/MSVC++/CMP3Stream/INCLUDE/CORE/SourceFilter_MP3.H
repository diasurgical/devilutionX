// ------------------------------------------------------------------------------------------------

#pragma once

// ------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#pragma comment(lib, "libMPG123_d.lib")
#else
#pragma comment(lib, "libMPG123.lib")
#endif

// ------------------------------------------------------------------------------------------------

#include "../DShowBase/streams.h"

#include <CORE/SourceFilter_AsyncIO.H>
#include <CORE/SourceFilter_AsyncReader.H>

#include "../../libMPG123/MPG123.H"

#include <IIEP_FileIn.H>

// ------------------------------------------------------------------------------------------------

#define WAVE_HEADER_LENGTH  44

// ------------------------------------------------------------------------------------------------

#define MP3_DECODE_LEN      0x4000

#define MP3_DECODE_BUF_LEN  0x8000

// ------------------------------------------------------------------------------------------------

namespace IIEP
{
  class CMP3Stream : public CAsyncStream
  {
    public:

    CMP3Stream(void);
    ~CMP3Stream(void);

    void SetSeekFlag(void);

    bool Open(const WORD *pcwsFileName, CMediaType &coMT, DWORD dwKBPerSec = INFINITE);
    void Close(void);

    HRESULT SetPointer(LONGLONG llPos);
    HRESULT Read(PBYTE   pucBuffer,
                 DWORD   dwBytesToRead,
                 BOOL    bAlign,
                 LPDWORD pdwBytesRead);

    LONGLONG Size(LONGLONG *pSizeAvailable);

    DWORD Alignment(void);

    void Lock  (void);
    void Unlock(void);

    bool DecodeFrame(void);

    //

    bool  OpenFile(const WORD *pcwsFileName);

    DWORD ReadFile(PVOID pBuf   , DWORD dwLen  );
    long  SeekFile(long  nOffset, int   iOrigin);

    //

    private:
  
    CCritSec       m_csLock;

    DWORD          m_dwKBPerSec;
    DWORD          m_dwTimeStart;

    mpg123_handle *m_pcoMH;

    DWORD          m_dwTotalBlocks;
    DWORD          m_dwAlignment;

    DWORD          m_dwLength;
    DWORD          m_dwPosition;

    BYTE m_ucHeader[WAVE_HEADER_LENGTH];

    //

    DWORD          m_dwDataBufPos;
    DWORD          m_dwDataBufLen;
    BYTE           m_ucDataBuffer[MP3_DECODE_BUF_LEN];

    //

    bool           m_bSeekFlag;

    //

    IIEP::CFileIn m_coFileIn;
    long          m_nDataBegin;
    long          m_nDataTotal;
  };

  // ----------------------------------------------------------------------------------------------

  class CMP3Reader : public CAsyncReader
  {
    public:

    //  We're not going to be CoCreate'd so we don't need registration stuff etc
  
    STDMETHODIMP Register(void)
    {
      return S_OK;
    }

    STDMETHODIMP Unregister(void)
    {
      return S_OK;
    }

    CMP3Reader(CMP3Stream *pcoStream,
               CMediaType *pcoMT,
               HRESULT    *phr) :
      CAsyncReader(NAME("MP3 Reader\0"),
                   NULL,
                   pcoStream,
                   phr)
    {
      m_coMT = *pcoMT;
    }
  };
}

// ------------------------------------------------------------------------------------------------