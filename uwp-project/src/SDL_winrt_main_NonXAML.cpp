/*
    SDL_winrt_main_NonXAML.cpp, placed in the public domain by David Ludwig  3/13/14
*/

#include <SDL.h>
#include <diablo.h>
#include <wrl.h>
#include <iostream>
#include <string>

/* At least one file in any SDL/WinRT app appears to require compilation
   with C++/CX, otherwise a Windows Metadata file won't get created, and
   an APPX0702 build error can appear shortly after linking.

   The following set of preprocessor code forces this file to be compiled
   as C++/CX, which appears to cause Visual C++ 2012's build tools to
   create this .winmd file, and will help allow builds of SDL/WinRT apps
   to proceed without error.

   If other files in an app's project enable C++/CX compilation, then it might
   be possible for SDL_winrt_main_NonXAML.cpp to be compiled without /ZW,
   for Visual C++'s build tools to create a winmd file, and for the app to
   build without APPX0702 errors.  In this case, if
   SDL_WINRT_METADATA_FILE_AVAILABLE is defined as a C/C++ macro, then
   the #error (to force C++/CX compilation) will be disabled.

   Please note that /ZW can be specified on a file-by-file basis.  To do this,
   right click on the file in Visual C++, click Properties, then change the
   setting through the dialog that comes up.
*/
#ifndef SDL_WINRT_METADATA_FILE_AVAILABLE
#ifndef __cplusplus_winrt
#error SDL_winrt_main_NonXAML.cpp must be compiled with /ZW, otherwise build errors due to missing .winmd files can occur.
#endif
#endif

/* Prevent MSVC++ from warning about threading models when defining our
   custom WinMain.  The threading model will instead be set via a direct
   call to Windows::Foundation::Initialize (rather than via an attributed
   function).

   To note, this warning (C4447) does not seem to come up unless this file
   is compiled with C++/CX enabled (via the /ZW compiler flag).
*/
#ifdef _MSC_VER
#pragma warning(disable:4447)
#endif

/* Make sure the function to initialize the Windows Runtime gets linked in. */
#ifdef _MSC_VER
#pragma comment(lib, "runtimeobject.lib")
#endif

// This handler is needed otherwise B button will exit the game as it is the behavior default for UWP apps
void OnBackRequested(Platform::Object^, Windows::UI::Core::BackRequestedEventArgs^ args)
{
	args->Handled = true;
}

void onInitialized()
{
	Windows::UI::Core::SystemNavigationManager::GetForCurrentView()->BackRequested += ref new Windows::Foundation::EventHandler<Windows::UI::Core::BackRequestedEventArgs^>(OnBackRequested);
	
	// workaround untill new config is released
	std::string controllerMapping = ",*,a:b1,b:b0,x:b3,y:b2,back:b6,start:b7,leftstick:b8,rightstick:b9,leftshoulder:b4,rightshoulder:b5,dpup:b10,dpdown:b12,dpleft:b13,dpright:b11,leftx:a1,lefty:a0~,rightx:a3,righty:a2~,lefttrigger:a4,righttrigger:a5,platform:WinRT";

	for(int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		SDL_JoystickType type = SDL_JoystickGetDeviceType(i);

		if(type == SDL_JOYSTICK_POWER_UNKNOWN)
			continue;

		SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(i);

		if(!guid.data)
			continue;

		char guidString[33];
		SDL_JoystickGetGUIDString(guid, guidString, 33);
		SDL_GameControllerAddMapping((guidString + controllerMapping).c_str());
	}
}

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	devilution::setOnInitialized(&onInitialized);
    return SDL_WinRTRunApp(devilution::DiabloMain, NULL);
}
