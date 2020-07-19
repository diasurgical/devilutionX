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
