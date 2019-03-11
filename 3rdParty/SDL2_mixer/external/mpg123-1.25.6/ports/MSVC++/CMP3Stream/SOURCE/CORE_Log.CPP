// ------------------------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>

#include <windows.h>

#include <IIEP_Def.H>

// ------------------------------------------------------------------------------------------------

static bool s_bLogFileOpened = false;

// ------------------------------------------------------------------------------------------------
// name: DebugLog()
// desc:
// ------------------------------------------------------------------------------------------------

void IIEP::DebugLog(const char *pcszFormat, ...)
{
#ifdef _DEBUG
  char cBuffer[0x8000];

  va_list pcArg;

  va_start(pcArg, pcszFormat);
  _vsnprintf(cBuffer, 0x8000, pcszFormat, pcArg);
  va_end(pcArg);

  ::OutputDebugStringA(cBuffer);
#endif
}

// ------------------------------------------------------------------------------------------------

void IIEP::DebugLog(const WORD *pcwsFormat, ...)
{
#ifdef _DEBUG
  WCHAR wcBuffer[0x8000];

  va_list pcArg;

  va_start(pcArg, pcwsFormat);
  _vsnwprintf(wcBuffer, 0x8000, pcwsFormat, pcArg);
  va_end(pcArg);

  ::OutputDebugStringW(wcBuffer);
#endif
}

// ------------------------------------------------------------------------------------------------
// name: WriteLog()
// desc:
// ------------------------------------------------------------------------------------------------

void IIEP::WriteLog(const char *pcszFormat, ...)
{
  char cBuffer[0x8000];

  va_list pcArg;

  va_start(pcArg, pcszFormat);
  _vsnprintf(cBuffer, 0x8000, pcszFormat, pcArg);
  va_end(pcArg);

  FILE *phFile;

  #define LOG_FILE "C:\\IIEP.LOG"

  if (s_bLogFileOpened)
  {
    phFile = fopen(LOG_FILE, "a+");
  }
  else
  {
    phFile = fopen(LOG_FILE, "w");

    s_bLogFileOpened = true;
  }

  if (phFile)
  {
    fprintf(phFile, cBuffer);
    fclose(phFile);
  }
}

// ------------------------------------------------------------------------------------------------

void IIEP::WriteLog(const WORD *pcwsFormat, ...)
{
  WCHAR wcBuffer[0x8000];

  va_list pcArg;

  va_start(pcArg, pcwsFormat);
  _vsnwprintf(wcBuffer, 0x8000, pcwsFormat, pcArg);
  va_end(pcArg);

  FILE *phFile;

  #define LOG_FILE "C:\\IIEP.LOG"

  if (s_bLogFileOpened)
  {
    phFile = fopen(LOG_FILE, "a");
  }
  else
  {
    phFile = fopen(LOG_FILE, "w");

    s_bLogFileOpened = true;
  }

  if (phFile)
  {
    fwprintf(phFile, wcBuffer);
    fclose(phFile);
  }
}

// ------------------------------------------------------------------------------------------------