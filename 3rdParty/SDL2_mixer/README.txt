
SDL_mixer 2.0

The latest version of this library is available from:
http://www.libsdl.org/projects/SDL_mixer/

Due to popular demand, here is a simple multi-channel audio mixer.
It supports 8 channels of 16 bit stereo audio, plus a single channel
of music.

See the header file SDL_mixer.h and the examples playwave.c and playmus.c
for documentation on this mixer library.

The mixer can currently load Microsoft WAVE files and Creative Labs VOC
files as audio samples, it can load FLAC files with libFLAC, it can load
Ogg Vorbis files with Ogg Vorbis or Tremor libraries, it can load MP3 files
using mpg123 or libmad, and it can load MIDI files with Timidity,
FluidSynth, and natively on Windows, Mac OSX, and Linux, and finally it can
load the following file formats via ModPlug or MikMod: .MOD .S3M .IT .XM.

Tremor decoding is disabled by default; you can enable it by passing
	--enable-music-ogg-tremor
to configure, or by defining MUSIC_OGG and OGG_USE_TREMOR.

libmad decoding is disabled by default; you can enable it by passing
	--enable-music-mp3-mad
to configure, or by defining MUSIC_MP3_MAD
vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
WARNING: The license for libmad is GPL, which means that in order to
         use it your application must also be GPL!
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The process of mixing MIDI files to wave output is very CPU intensive,
so if playing regular WAVE files sound great, but playing MIDI files
sound choppy, try using 8-bit audio, mono audio, or lower frequencies.

To play MIDI files using FluidSynth, you'll need to set the SDL_SOUNDFONTS
environment variable to a Sound Font 2 (.sf2) file containing the musical
instruments you want to use for MIDI playback.
(On some Linux distributions you can install the fluid-soundfont-gm package)

To play MIDI files using Timidity, you'll need to get a complete set of
GUS patches from:
http://www.libsdl.org/projects/mixer/timidity/timidity.tar.gz
and unpack them in /usr/local/lib under UNIX, and C:\ under Win32.

iOS:
In order to use this library on iOS, you should include the SDL.xcodeproj
and Xcode-iOS/SDL_mixer.xcodeproj in your application, add the SDL/include
and SDL_mixer directories to your "Header Search Paths" setting, then add the
libSDL2.a and libSDL2_mixer.a to your "Link Binary with Libraries" setting.

This library is under the zlib license, see the file "COPYING.txt" for details.

