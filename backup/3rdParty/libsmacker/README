libsmacker
A C library for decoding .smk Smacker Video files

version 1.1
2017-01-27

(c) Greg Kennedy 2013-2017
http://libsmacker.sourceforge.net
----

---
Introduction
---
libsmacker is a cross-platform C library which can be used for decoding Smacker Video files produced by RAD Game Tools. Smacker Video was the king of video middleware in the 1990s, and its 256-color compressed video format was used in over 2600 software titles.

libsmacker implements the minimum feature set required from smackw32.dll to get an smk off a disk and the frames / audio into a buffer in the correct order.

---
License
---
libsmacker is released under a Creative Commons Attribution Non-Commercial License V2.0. This means, first, that projects using libsmacker must indicate somewhere that they use libsmacker in the code. Also: the Non-Commercial tag means exactly what it suggests: LIBSMACKER MAY NOT BE USED IN ANY COMMERCIAL PROJECT. PERIOD. Non-commercial DOES NOT mean simply "non-profit": commercial includes for-profit, but may also encompass other non-profit activites as well.

Static- or dynamic-linking against unmodified libsmacker does not automatically re-license your code.  If you alter the library code (.c, .h files), your changes become a Derivative Work and are subject to the conditions outlined in the license.  Unmodified redistributions of binary libsmacker in your own projects must meet the conditions of this license.

A human-readable summary of the license is available online:
http://creativecommons.org/licenses/by-nc/2.0/

The complete license text is here:
http://creativecommons.org/licenses/by-nc/2.0/legalcode

---
Usage
---
See the webpage for sample code and function documentation.  The source package additionally includes a pair of driver programs:
* driver.c - dumps all frames of a file to a bmp/ subdirectory, and all audio as raw streams to out_*.raw files in CWD
* smk2avi.c - converts smk file(s) to AVI files - uncompressed 24-bit color and PCM audio stream.

Though the libraries are "bulletproofed" the sample apps are not: be cautious if you plan to implement in some critical environment.

---
Changelog
---
1.1
* Switch to autotools-based build
* Incorporates patches from Dalerank Slim, Gennady Trafimenkov, and Bianca van Schaik
* Performance improvements and code cleanup / safety.
1.0
* Initial revision

---
Contact
---
Questions/comments:
* by email: kennedy.greg@gmail.com
* by website: http://libsmacker.sourceforge.net

Enjoy!
