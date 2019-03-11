AUTOMAKE_OPTIONS = foreign

INCLUDES = -I./ @OGG_CFLAGS@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vorbisidec.pc

lib_LTLIBRARIES = libvorbisidec.la

libvorbisidec_la_SOURCES = mdct.c block.c window.c \
                        synthesis.c info.c \
                        floor1.c floor0.c vorbisfile.c \
                        res012.c mapping0.c registry.c codebook.c \
			sharedbook.c \
                        codebook.h misc.h mdct_lookup.h\
                        os.h mdct.h block.h ivorbisfile.h lsp_lookup.h\
                        registry.h window.h window_lookup.h\
                        codec_internal.h backends.h \
			asm_arm.h ivorbiscodec.h
libvorbisidec_la_LDFLAGS = -version-info @V_LIB_CURRENT@:@V_LIB_REVISION@:@V_LIB_AGE@
libvorbisidec_la_LIBADD = @OGG_LIBS@

EXTRA_PROGRAMS = ivorbisfile_example iseeking_example
CLEANFILES = $(EXTRA_PROGRAMS) $(lib_LTLIBRARIES)

ivorbisfile_example_SOURCES = ivorbisfile_example.c
ivorbisfile_example_LDFLAGS = -static 
ivorbisfile_example_LDADD = libvorbisidec.la @OGG_LIBS@

iseeking_example_SOURCES = iseeking_example.c
iseeking_example_LDFLAGS = -static 
iseeking_example_LDADD = libvorbisidec.la @OGG_LIBS@

includedir = $(prefix)/include/tremor

include_HEADERS = ivorbiscodec.h ivorbisfile.h config_types.h

EXTRA_DIST = vorbisidec.pc.in \
        $(srcdir)/doc/*.html $(srcdir)/win32/VS*/libtremor/*.vcproj

example:
	-ln -fs . vorbis
	$(MAKE) ivorbisfile_example
	$(MAKE) iseeking_example

debug:
	$(MAKE) all CFLAGS="@DEBUG@" 

profile:
	$(MAKE) all CFLAGS="@PROFILE@"
