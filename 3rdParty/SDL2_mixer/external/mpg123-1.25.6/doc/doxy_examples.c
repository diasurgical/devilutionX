/** \defgroup mpg123_examples example programs using libmpg123 and libout123
	@{ */

/** \file mpg123_to_out123.c A simple MPEG audio to WAV converter using libmpg123 (read) and libout123 (write).
    ...an excersize on two simple APIs. */

/** \file mpglib.c Example program mimicking the old mpglib test program.
	It takes an MPEG bitstream from standard input and writes raw audio to standard output.
	This is an use case of the mpg123_decode() in and out function in the feeder mode, quite close to classic mpglib usage and thus a template to convert from that to libmpg123.
*/

/** \file scan.c Example program that examines the exact length of an MPEG file.
    It opens a list of files and does mpg123_scan() on each and reporting the mpg123_length() before and after that. */

/** \file id3dump.c Parse ID3 info and print to standard output. */

/** \file extract_frames.c Parse stream and extract only the valid MPEG frames to standard output. */

/** \file feedseek.c Fuzzy feeder seeking. */

/* @} */
