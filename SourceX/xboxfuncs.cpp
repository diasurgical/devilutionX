#include "xboxfuncs.h"
#include <xtl.h>

void CXBFunctions::QuitToDash()
{
	LD_LAUNCH_DASHBOARD LaunchData = { XLD_LAUNCH_DASHBOARD_MAIN_MENU };
	XLaunchNewImage( NULL, (LAUNCH_DATA*)&LaunchData );	
}

void CXBFunctions::Check128MBPatch()
{
	MEMORYSTATUS lpBuffer;
	lpBuffer.dwLength = sizeof( MEMORYSTATUS );
	GlobalMemoryStatus( &lpBuffer );

	// Set MTRRDefType memory type to write-back as done in other XBox apps - seems a bit of a hack as really the def type
	// should be uncachable and the mtrr/mask for ram instead set up for 128MB with writeback as is done in cromwell
	if( lpBuffer.dwTotalPhys > 67108864 ) //Check if > 64MB
	{
		__asm
		{
			mov ecx, 0x2ff
			rdmsr
			mov al, 0x06
			wrmsr
		}
	}
}

#define MB	(1024*1024)
#define AddStr(a,b) (pstrOut += wsprintf( pstrOut, a, b ))

void CXBFunctions::GetMemoryUsage()
{
	static DWORD dwTicks = 0;

	if(dwTicks < GetTickCount())
	{
		MEMORYSTATUS stat;
		CHAR strOut[1024], *pstrOut;

		// Get the memory status.
		GlobalMemoryStatus( &stat );

		// Setup the output string.
		pstrOut = strOut;
		AddStr( "%4d total MB of virtual memory.\n", stat.dwTotalVirtual / MB );
		AddStr( "%4d  free MB of virtual memory.\n", stat.dwAvailVirtual / MB );
		AddStr( "%4d total MB of physical memory.\n", stat.dwTotalPhys / MB );
		AddStr( "%4d  free MB of physical memory.\n", stat.dwAvailPhys / MB );
		AddStr( "%4d total MB of paging file.\n", stat.dwTotalPageFile / MB );
		AddStr( "%4d  free MB of paging file.\n", stat.dwAvailPageFile / MB );
		AddStr( "%4d  percent of memory is in use.\n", stat.dwMemoryLoad );

		// Output the string.
		OutputDebugString( strOut );

		dwTicks = GetTickCount() + 2000;
	}
}