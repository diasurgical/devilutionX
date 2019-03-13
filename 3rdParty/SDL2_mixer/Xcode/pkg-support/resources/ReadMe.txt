SDL_mixer is an example portable sound library for use with SDL.

The source code is available from: http://www.libsdl.org/projects/SDL_mixer

This library is distributed under the terms of the zlib license: http://www.zlib.net/zlib_license.html

This packages contains the SDL2_mixer.framework for OS X. Conforming with Apple guidelines, this framework contains both the SDL runtime component and development header files.

Requirements:
You must have the SDL2.framework installed.

To Install:
Copy the SDL2_mixer.framework to /Library/Frameworks

You may alternatively install it in <your home directory>/Library/Frameworks if your access privileges are not high enough. (Be aware that the Xcode templates we provide in the SDL Developer Extras package may require some adjustment for your system if you do this.)


(Partial) History of PB/Xcode projects:
2009-09-21 - Updated for 64-bit (Snow Leopard) Universal Binaries.
	Switched to 10.4 minimum requirement.
	Reebuilt Ogg Vorbis components for 64-bit Universal.
	Ogg 1.1.4
	Vorbis 1.2.3
	Mac native midi had to be disabled because the code depends on legacy Quicktime and won't compile in 64-bit.
	
2006-01-31 - First entry in history. Updated for Universal Binaries. Static libraries of libogg and libvorbis have been brought up-to-date and built as Universal. Infrastructure has been added to support building against smpeg statically, but there may be bugs in smpeg itself (unrelated to static linking) which prevent MP3 playback.
