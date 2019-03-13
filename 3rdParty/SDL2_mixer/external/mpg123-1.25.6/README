* * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   mpg123 - MPEG 1.0/2.0/2.5 audio player            *
*   README for version 1.x.y, dated at 14.06.2009     *
*                                                     *
* ...still the fastest MPEG audio player for UNIX ;)  *
* * * * * * * * * * * * * * * * * * * * * * * * * * * *
(This file has very long lines - die-hard terminal nostalgists can be satisfied by `fmt -s -w 75 < README | less`. I think it's better to let the reader's preference rule than to preformat the stuff to some arbitrary width.)


0. Stuff

For building/installation info see INSTALL.

The mpg123 project was started by Michel Hipp and is now being maintained by Thomas Orgis and Nicholas J. Humfrey, who initiated the Sourceforge project.
The source code contains contributions from quite a few people - see AUTHORS for more info.
It is Open Source software licensed mostly under the LGPL with some parts restricted to GPL. See COPYING for details.
As for every mp3 player, some of mpg123's functionality may be covered by patents in a country where these are valid. See PATENTS for details.

Project's official website URL is

	http://mpg123.org
(or http://mpg123.orgis.org as fallback address if there is a problem with the DNS forwarding)

for the traditional home page and

	http://sourceforge.net/projects/mpg123

for sourceforge.net based services like download mirrors, mailing lists and bug/feature trackers.
Please use the sourceforge download mirrors when possible to minimize load on the mpg123.org server.


1. Introduction

This is a console based decoder/player for mono/stereo mpeg audio files, probably more familiar as MP3 or MP2 files.
It's focus is speed. We still need some low-end benchmarks for the current version, but playback should be possible even on i486 CPUs. There is hand-optimized assembly code for i586, MMX, 3DNow, SEE and 3DNowExt instructions, while generic code runs on a variety of different platforms and CPUs.
It can play MPEG1.0/2.0/2.5 layer I, II, II (1, 2, 3;-) files (VBR files are fine, too) and produce output on a number of different ways: raw data to stdout and different sound systems depending on your platform (see INSTALL).
Most tested are Linux on x86 and Alpha/AXP and MacOSX on ppc as the environments the current developers work in.
We are always thankful for user reports on success (and failure) on any platform!


2. Contact

short:

	mpg123-devel@lists.sourceforge.net
	mpg123-users@lists.sourceforge.net
or 
	maintainer@mpg123.org

long: see doc/CONTACT


3. Interface/Usage

Please consult the manpage mpg123(1). Some starter info follows.

3.1 Simple Console Usage

Mpg123 is a console program - normally it just plays a list of files you specify on command line and that's it. See the included manpage or

	mpg123 --help

or, for the full story,

	mpg123 --longhelp

on command line syntax/options. I encourage you to check out the --gapless and --rva-album/--rva-mix options:-)

In the simple "mpg123 file1.mp3 file2.mp3" mode, the only thing you can do to interact is to press Ctrl+C to skip to next track or end the whole playback if pressing it twice.

Note that this Ctrl+C behaviour is special to this mode; when any of the following is activated, Ctrl+C will just kill the program like you would expect normally (this changed from earlier versions).

3.2 Advanced Console Usage

You can specify the option -C to enable a terminal control interface enabling to influence playback on current title/playlist by pressing some key:

 -= terminal control keys =-
[s] or [ ]	interrupt/restart playback (i.e. 'pause')
[f]	next track
[d]	previous track
[b]	back to beginning of track
[p]	pause while looping current sound chunk
[.]	forward
[,]	rewind
[:]	fast forward
[;]	fast rewind
[>]	fine forward
[<]	fine rewind
[+]	volume up
[-]	volume down
[r]	RVA switch
[v]	verbose switch
[l]	list current playlist, indicating current track there
[t]	display tag info (again)
[m]	print MPEG header info (again)
[h]	this help
[q]	quit

You can change these bindings to key to your liking by modifying term.h .

Note: This interface needs not to be available on _every_ platform/build.

Another note: The volume up and down is performed by changing the scale factor (like the -f parameter) ... so the audio is scaled digitally in the given range of the output format (usually 16bits). That means the lowering the volume will decrease the dynamic range and possibly lessen the quality while increasing volume can in fact increase the dynamic range and thus make it better, if you deal with a silent source and no clipping is necessary.
It is a good idea to use RVA values stored in the file for adjusting low volume files, though - mpg123 handles that in addition to your volume setting.

3.3 Control Interface for Frontends

There used to be several interfaces for frontends left over from that past, but only one of them remains for the present and future:

	The Generic Control Interface

It contains of communication of textual messages via standard input to mpg123 and responses to standard output unless the -s switch for output of audio data on stdout is used - then the responses come via stderr.

See doc/README.remote for usage.


4. Speed

mpg123 is fast. Any faster software player is probably based on some hacked mpg123;-)
MPlayer included mpg123 source code in it's mp3lib and we have to be thankful for the MPlayer folks adding SSE, 3DNowExt and AltiVec optimizations over the years, which we were able to backport.

mpg123 includes the AltiVec optimization since version 0.61 and the SSE and 3DNowExt optimizations since 0.66 .
Also, version 0.66 adds the merged x86 optimization build, which includes every applicable optimization for x86 cpus except the one for i486, wich is a bit special.

Now mpg123 catched up with MPlayer's mp3lib concerning decoding speed on my Pentium M (which supports SSE):
Decoding a certain album (Queensryche's Rage for Order) to /dev/null took 22.4s user time with mpg123-0.66 compared to 24.7s with MPlayer-1.0rc1 .

Also, beginning with mpg123 1.8.0, there are fresh x86-64 SSE optimizations (provided by Taihei Monma) which make mpg123 the fastest MPEG audio decoder in my knowledge also on current 64bit x86 systems.

5. Accuracy

The mpg123 engine is able to decode in full compliance to ISO/IEC 11172-3, for all three layers, using floating point or integer math (the latter since 1.8.1).
Accuracy of 16bit output depends on specific optimization in use and compile-time choice about the rounding mode (which is performance relevant).

The ISO test suite is incorporated in the mpg123 subversion repository under svn://orgis.org/mpg123/test, nightly tests of a build (with high-quality 16bit rounding) are published on the mpg123 website.

Dithered 16bit output is available as an option (the --cpu choices ending with _dither). See

	http://dither123.dyndns.org

on the whereabouts.

6. History

A looooong time ago (mid-90s), Michael Hipp wrote some initial mpg123 and made it _the_ Unix console mp3 player in the following years.
The exact date of birth is fuzzy in human memory, but according to the master himself (Michael) mpg123 started in 1994 as an MP2 player which a year later, 1995, gained MP3 ability.
The core decoder files have mostly 1995 as their birth year listed, so one can say that mpg123 as the layer1,2,3 player was born in 1995.
In any case, that is a looooong time ago for a media player - especially for one that is still alive!

This is the historic description:

	This isn't a new player. It's a fully rewritten version originally based 
	on the mpegaudio (FHG-version) package. The DCT algorithm in the
	synthesis filter is a rewritten version of the DCT first seen in the maplay
	package, which was written by Tobias Bading (bading@cs.tu-berlin.de). The 
	rewrite was necessary, because the GPL may not allow this copyright mix.
	The mpegaudio package was written by various people from the MPEG/audio
	software simulation group. The maplay is under GPL .. You can find the
	original source code and the mpegaudio package on: ftp.tnt.uni-hannover.de.

	Especially layer3.c common.c and mpg123.h is based on the dist10 package.
	The code is fully rewritten but I'm using sometimes the
	same variable names or similar function names as in the
	original package.

	In the current layer3.c I'm using a DCT36 first seen in Jeff Tsay's 
	(ctsay@pasteur.eecs.berkeley.edu) maplay 1.2+ package. His code is
	under GPL .. I also tried the enhancement from Mikko Tommila. His
	code is also in layer3.c (but it's disabled at the moment, because
	it was slightly slower than the unrolled 9 point DCT (at least on 
	_my_ system)). Theoretically it should be faster. You may try it on
	your system.

Well, that's how it started...
Official development ceased due to the typical lack-of-time syndrome around 2002 and the free-floating patches began to seize the day.

But before that, Michael wrote or rewrote the essential code; others contributed their bits.
The main message is:

Code is copyrighted by Michael Hipp, who made it free software under the terms of the LGPL 2.1.

Please see doc/ROAD_TO_LGPL, COPYING and AUTHORS for details on that. Note that the only notable legacy non-LGPL file was the old alsa output that didn't work with alsa 0.9/1.0 anymore.
Also, there has been a libao output in the betas 0.60 for a short period. Libao being generally problematic for us because of its GPL license, this output is not distributed anymore in the release packages. There is now a new, LGPLed alsa output that made both the old alsa and libao obsolete for our purposes.
So, the distributed mpg123 releases actually only contain LGPL code, but you get the other files from our subversion repository if you checkout the trunk / version tags.

There has been quite some confusion about the licensing and "freeness" of mpg123 in the past.
The initial "free for private use, ask me when you want to do something commercial" license caused some people to avoid mpg123 and even to write a replacement mimicking the interface but using a different decoding engine - what was not actively developed for too long but entered the "free" software sections.

The Debian (non-free section) and Gentoo distributions cared about the last stable and the last development release of mpg123 over the years with mainly applying security fixes. Thanks go to the distribution maintainers for not letting it alone to bitrot over the years.

Thomas Orgis started to hack on mpg123 in 2004 while working on his personal audio experience with mixplayd and later DerMixD, utilizing the generic control interface. In Feb 2005, he crammed control interface improvements together with Debian's r19 fixes and released the personal fork/patch named mpg123-thor.
Little later that year, Nicholas J. Humphrey independently created the sourceforge project and released an autotooled 0.59r under official GPL flag with Debian and MacOSX fixes.
In the beginning of 2006, Thomas finally decided that he could work "officially" on mpg123 and contacted Michael Hipp for taking over maintainership.
Michael was all-positive about letting mpg123 really live again (and perhaps see version 1.0 some time;-) and also pointed at the sourceforge project that didn't see much activity since the initial release. 
A lot of emails and some weeks later there was the two-developer team of Nicholas and Thomas working on merging their mpg123 variants as well as adding some features and fixes to let it shine again.

And there we are now...

7. End

Have fun!
____________
Thomas Orgis
